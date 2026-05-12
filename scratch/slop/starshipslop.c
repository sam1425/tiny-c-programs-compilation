#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <git2.h>

/* ── gruvbox dark colours ────────────────────────────────────────── */
#define C_GREEN  "\033[1;38;2;184;187;38m"   /* bright green  #b8bb26 */
#define C_YELLOW "\033[1;38;2;250;189;47m"   /* bright yellow #fabd2f */
#define C_AQUA   "\033[1;38;2;142;192;124m"  /* bright aqua   #8ec07c */
#define C_ORANGE "\033[1;38;2;254;128;25m"   /* Gruvbox Orange #fe8019 */
#define C_RESET  "\033[0m"

static int status_cb(const char *path, unsigned int status_flags, void *payload) {
    (void)path;
    *(unsigned int *)payload = status_flags;
    return 1; /* Abort on first change found */
}

int main(int argc, char *argv[]) {
    const char *path = (argc >= 2) ? argv[1] : getenv("PWD");
    if (!path) return 0;

    /* 1. Fast Upward Discovery for .git */
    char cur[4096];
    struct stat st;
    snprintf(cur, sizeof(cur), "%s", path);
    int found_git = 0;
    while (1) {
        char gitdir[4112];
        snprintf(gitdir, sizeof(gitdir), "%s/.git", cur);
        if (stat(gitdir, &st) == 0 && S_ISDIR(st.st_mode)) {
            found_git = 1;
            break;
        }
        char *last = strrchr(cur, '/');
        if (!last || last == cur) _exit(0);
        *last = '\0';
    }

    if (!found_git) _exit(0);

    /* 2. LIBGIT2 INIT & CONFIG BYPASS */
    git_libgit2_init();
    git_libgit2_opts(GIT_OPT_SET_SEARCH_PATH, GIT_CONFIG_LEVEL_SYSTEM, NULL);
    git_libgit2_opts(GIT_OPT_SET_SEARCH_PATH, GIT_CONFIG_LEVEL_GLOBAL, NULL);
    git_libgit2_opts(GIT_OPT_SET_SEARCH_PATH, GIT_CONFIG_LEVEL_XDG, NULL);
    git_libgit2_opts(GIT_OPT_SET_SEARCH_PATH, GIT_CONFIG_LEVEL_PROGRAMDATA, NULL);

    git_repository *repo = NULL;
    /* Use the discovered path to avoid re-searching */
    if (git_repository_open_ext(&repo, cur, GIT_REPOSITORY_OPEN_NO_SEARCH, NULL) != 0) _exit(0);

    /* 3. Status Scan (Early Exit) */
    unsigned int status = 0;
    git_status_options s_opts = GIT_STATUS_OPTIONS_INIT;
    s_opts.show = GIT_STATUS_SHOW_INDEX_AND_WORKDIR;
    s_opts.flags = GIT_STATUS_OPT_INCLUDE_UNTRACKED | 
                   GIT_STATUS_OPT_RENAMES_HEAD_TO_INDEX |
                   GIT_STATUS_OPT_EXCLUDE_SUBMODULES |
                   GIT_STATUS_OPT_NO_REFRESH;
    
    git_status_foreach_ext(repo, &s_opts, status_cb, &status);

    if (status & (GIT_STATUS_INDEX_NEW | GIT_STATUS_INDEX_MODIFIED | GIT_STATUS_INDEX_DELETED | GIT_STATUS_INDEX_RENAMED | GIT_STATUS_INDEX_TYPECHANGE)) {
        write(1, C_YELLOW "🔘 " C_RESET, sizeof(C_YELLOW "🔘 " C_RESET)-1);
        _exit(0);
    }
    
    if (status & (GIT_STATUS_WT_NEW | GIT_STATUS_WT_MODIFIED | GIT_STATUS_WT_DELETED | GIT_STATUS_WT_TYPECHANGE | GIT_STATUS_WT_RENAMED)) {
        write(1, C_ORANGE "꩜ " C_RESET, sizeof(C_ORANGE "꩜ " C_RESET)-1);
        _exit(0);
    }

    /* 4. Ahead/Behind (Only if clean) */
    {
        git_reference *head = NULL;
        if (git_repository_head(&head, repo) == 0) {
            git_reference *upstream = NULL;
            if (git_branch_upstream(&upstream, head) == 0) {
                const git_oid *h = git_reference_target(head), *u = git_reference_target(upstream);
                if (h && u && git_oid_cmp(h, u) != 0) {
                    size_t a = 0, b = 0;
                    if (git_graph_ahead_behind(&a, &b, repo, h, u) == 0 && a > 0) {
                        write(1, C_AQUA "⬤↑" C_RESET, sizeof(C_AQUA "⬤↑" C_RESET)-1);
                        _exit(0);
                    }
                }
                git_reference_free(upstream);
            }
            git_reference_free(head);
        }
    }

    write(1, C_GREEN "⬤ " C_RESET, sizeof(C_GREEN "⬤ " C_RESET)-1);
    _exit(0);
}
