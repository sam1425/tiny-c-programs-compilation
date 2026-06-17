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
#define MAX_REPOS 16

/* No colors in daemon - Starship handles styling */
#define C_GREEN  ""
#define C_YELLOW ""
#define C_AQUA   ""
#define C_ORANGE ""
#define C_RESET  ""

typedef struct {
    char root[4096];
    git_repository *repo;
    struct timespec last_idx_mtime;
    char cached_msg[128];
    size_t cached_len;
} repo_cache_t;

static repo_cache_t cache[MAX_REPOS];
static int cache_count = 0;

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

    /* 1. Normalize and resolve absolute path */
    char abs_path[4096];
    if (path[0] == '/') {
        strncpy(abs_path, path, sizeof(abs_path)-1);
    } else {
        if (getcwd(abs_path, sizeof(abs_path)) == NULL) return;
        strncat(abs_path, "/", sizeof(abs_path)-strlen(abs_path)-1);
        strncat(abs_path, path, sizeof(abs_path)-strlen(abs_path)-1);
    }
    
    /* Remove trailing slashes */
    size_t plen = strlen(abs_path);
    while (plen > 1 && abs_path[plen - 1] == '/') {
        abs_path[--plen] = '\0';
    }

    /* 2. Upward Discovery */
    char cur[4096];
    strncpy(cur, abs_path, sizeof(cur)-1);
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

    /* 3. Cache Check */
    repo_cache_t *rc = NULL;
    for (int i = 0; i < cache_count; i++) {
        if (strcmp(cache[i].root, cur) == 0) {
            rc = &cache[i];
            break;
        }
    }

    char index_path[4120];
    snprintf(index_path, sizeof(index_path), "%s/.git/index", cur);
    struct stat idx_st;
    int has_index = (stat(index_path, &idx_st) == 0);

    if (rc && has_index) {
        if (rc->last_idx_mtime.tv_sec == idx_st.st_mtim.tv_sec &&
            rc->last_idx_mtime.tv_nsec == idx_st.st_mtim.tv_nsec) {
            write(client_fd, rc->cached_msg, rc->cached_len);
            return;
        }
    }

    if (!rc) {
        if (cache_count < MAX_REPOS) {
            rc = &cache[cache_count++];
        } else {
            git_repository_free(cache[0].repo);
            memmove(&cache[0], &cache[1], sizeof(repo_cache_t) * (MAX_REPOS - 1));
            rc = &cache[MAX_REPOS - 1];
        }
        strncpy(rc->root, cur, sizeof(rc->root)-1);
        if (git_repository_open_ext(&rc->repo, cur, GIT_REPOSITORY_OPEN_NO_SEARCH, NULL) != 0) {
            cache_count--;
            return;
        }
    }

    /* 4. Status (Index Only) */
    unsigned int status = 0;
    git_status_options s_opts = {0};
    git_status_init_options(&s_opts, GIT_STATUS_OPTIONS_VERSION);
    s_opts.show = GIT_STATUS_SHOW_INDEX_ONLY;
    s_opts.flags = GIT_STATUS_OPT_EXCLUDE_SUBMODULES |
                   GIT_STATUS_OPT_NO_REFRESH |
                   GIT_STATUS_OPT_DISABLE_PATHSPEC_MATCH;
    
    git_status_foreach_ext(rc->repo, &s_opts, status_cb, &status);

    char *msg = "⬤ ";
    if (status & (GIT_STATUS_INDEX_NEW | GIT_STATUS_INDEX_MODIFIED | GIT_STATUS_INDEX_DELETED | GIT_STATUS_INDEX_RENAMED | GIT_STATUS_INDEX_TYPECHANGE)) {
        msg = "🔘 ";
    } else {
        git_reference *head = NULL;
        if (git_repository_head(&head, rc->repo) == 0) {
            git_reference *upstream = NULL;
            if (git_branch_upstream(&upstream, head) == 0) {
                const git_oid *h = git_reference_target(head), *u = git_reference_target(upstream);
                if (h && u && git_oid_cmp(h, u) != 0) {
                    size_t a = 0, b = 0;
                    if (git_graph_ahead_behind(&a, &b, rc->repo, h, u) == 0 && a > 0) {
                        msg = "⬤↑";
                    }
                }
                git_reference_free(upstream);
            }
            git_reference_free(head);
        }
    }

    if (has_index) rc->last_idx_mtime = idx_st.st_mtim;
    rc->cached_len = strlen(msg);
    memcpy(rc->cached_msg, msg, rc->cached_len);
    write(client_fd, rc->cached_msg, rc->cached_len);
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
