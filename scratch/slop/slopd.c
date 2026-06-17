#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <git2.h>
#include <fcntl.h>
#include <stdint.h>
#include <signal.h>
#include <limits.h>

#define SOCKET_PATH "/tmp/slop.sock"

/* ── gruvbox dark colours ────────────────────────────────────────── */
#define C_GREEN  "\033[1;38;2;184;187;38m"   /* bright green  #b8bb26 */
#define C_YELLOW "\033[1;38;2;250;189;47m"   /* bright yellow #fabd2f */
#define C_AQUA   "\033[1;38;2;142;192;124m"  /* bright aqua   #8ec07c */
#define C_ORANGE "\033[1;38;2;254;128;25m"   /* Gruvbox Orange #fe8019 */
#define C_RESET  "\033[0m"

static int status_cb(const char *path, unsigned int status_flags, void *payload) {
    (void)path;
    *(unsigned int *)payload |= status_flags;
    return 1;
}

static void handle_request(int client_fd) {
    char path[4096];
    ssize_t n = read(client_fd, path, sizeof(path) - 1);
    if (n <= 0) return;
    path[n] = '\0';

    /* discovery requires absolute path */
    char cur[4096];
    strncpy(cur, path, sizeof(cur)-1);
    
    /* Remove trailing slashes */
    size_t plen = strlen(cur);
    while (plen > 1 && cur[plen - 1] == '/') {
        cur[--plen] = '\0';
    }

    struct stat st;
    int found_git = 0;
    while (1) {
        char gitdir[4112];
        snprintf(gitdir, sizeof(gitdir), "%s/.git", cur);
        if (stat(gitdir, &st) == 0) {
            found_git = 1;
            break;
        }
        char *last = strrchr(cur, '/');
        if (!last || last == cur) break;
        *last = '\0';
    }

    if (!found_git) return;

    git_repository *repo = NULL;
    if (git_repository_open_ext(&repo, cur, GIT_REPOSITORY_OPEN_NO_SEARCH, NULL) != 0) return;

    unsigned int status = 0;
    git_status_options s_opts = {0};
    git_status_init_options(&s_opts, GIT_STATUS_OPTIONS_VERSION);
    s_opts.show = GIT_STATUS_SHOW_INDEX_AND_WORKDIR;
    s_opts.flags = GIT_STATUS_OPT_INCLUDE_UNTRACKED |
                   GIT_STATUS_OPT_EXCLUDE_SUBMODULES |
                   GIT_STATUS_OPT_NO_REFRESH |
                   GIT_STATUS_OPT_DISABLE_PATHSPEC_MATCH;
    
    git_status_foreach_ext(repo, &s_opts, status_cb, &status);

    char *msg = NULL;
    if (status & (GIT_STATUS_INDEX_NEW | GIT_STATUS_INDEX_MODIFIED | GIT_STATUS_INDEX_DELETED | GIT_STATUS_INDEX_RENAMED | GIT_STATUS_INDEX_TYPECHANGE)) {
        msg = C_YELLOW "🔘 " C_RESET;
    } else if (status & (GIT_STATUS_WT_NEW | GIT_STATUS_WT_MODIFIED | GIT_STATUS_WT_DELETED | GIT_STATUS_WT_TYPECHANGE | GIT_STATUS_WT_RENAMED)) {
        msg = C_ORANGE "꩜ " C_RESET;
    } else {
        git_reference *head = NULL;
        if (git_repository_head(&head, repo) == 0) {
            git_reference *upstream = NULL;
            if (git_branch_upstream(&upstream, head) == 0) {
                const git_oid *h = git_reference_target(head), *u = git_reference_target(upstream);
                if (h && u && git_oid_cmp(h, u) != 0) {
                    size_t a = 0, b = 0;
                    if (git_graph_ahead_behind(&a, &b, repo, h, u) == 0 && a > 0) {
                        msg = C_AQUA "⬤↑" C_RESET;
                    }
                }
                git_reference_free(upstream);
            }
            git_reference_free(head);
        }
    }

    if (!msg) msg = C_GREEN "⬤ " C_RESET;

    write(client_fd, msg, strlen(msg));
    git_repository_free(repo);
}

int main() {
    signal(SIGPIPE, SIG_IGN);
    if (fork() > 0) exit(0);
    setsid();

    unlink(SOCKET_PATH);
    int server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_fd < 0) return 1;

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) return 1;
    if (listen(server_fd, 64) < 0) return 1;

    git_libgit2_init();
    git_libgit2_opts(GIT_OPT_SET_SEARCH_PATH, GIT_CONFIG_LEVEL_SYSTEM, NULL);
    git_libgit2_opts(GIT_OPT_SET_SEARCH_PATH, GIT_CONFIG_LEVEL_GLOBAL, NULL);
    git_libgit2_opts(GIT_OPT_SET_SEARCH_PATH, GIT_CONFIG_LEVEL_XDG, NULL);
    git_libgit2_opts(GIT_OPT_SET_SEARCH_PATH, GIT_CONFIG_LEVEL_PROGRAMDATA, NULL);

    while (1) {
        int client_fd = accept(server_fd, NULL, NULL);
        if (client_fd < 0) continue;
        handle_request(client_fd);
        close(client_fd);
    }
    return 0;
}
