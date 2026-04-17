#include <git2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* ── gruvbox dark colours ────────────────────────────────────────── */
#define C_GREEN  "\033[1;38;2;184;187;38m"   /* bright green  #b8bb26 */
#define C_YELLOW "\033[1;38;2;250;189;47m"   /* bright yellow #fabd2f */
#define C_AQUA   "\033[1;38;2;142;192;124m"  /* bright aqua   #8ec07c */
#define C_ORANGE "\033[1;38;2;254;128;25m"   /* Gruvbox Orange #fe8019 */
#define C_RESET  "\033[0m"

static int status_cb(const char *path, unsigned int status_flags, void *payload) {
    (void)path; (void)status_flags;
    *(int *)payload = 1;
    return 1; /* stop iteration immediately */
}

int main(int argc, char *argv[]) {
    const char *path = (argc >= 2) ? argv[1] : getenv("PWD");
    if (!path) return 0;

    git_libgit2_init();

    git_repository *repo = NULL;
    /* Open repo searching upwards. Fast. */
    if (git_repository_open_ext(&repo, path, GIT_REPOSITORY_OPEN_FROM_ENV, NULL) != 0) {
        _exit(0);
    }

    git_status_options opts;
    memset(&opts, 0, sizeof(opts));
    opts.version = GIT_STATUS_OPTIONS_VERSION;
    opts.flags = GIT_STATUS_OPT_NO_REFRESH | 
                 GIT_STATUS_OPT_EXCLUDE_SUBMODULES | 
                 GIT_STATUS_OPT_DISABLE_PATHSPEC_MATCH;
    
    int found = 0;

    /* 1. Check Index (Staged) - Priority 1 */
    opts.show = GIT_STATUS_SHOW_INDEX_ONLY;
    git_status_foreach_ext(repo, &opts, status_cb, &found);
    if (found) {
        write(1, C_YELLOW "🔘 " C_RESET, sizeof(C_YELLOW "🔘 " C_RESET) - 1);
        _exit(0);
    }

    /* 2. Check Workdir (Dirty/Untracked) - Priority 2 */
    opts.show = GIT_STATUS_SHOW_WORKDIR_ONLY;
    opts.flags |= GIT_STATUS_OPT_INCLUDE_UNTRACKED | GIT_STATUS_OPT_RECURSE_UNTRACKED_DIRS;
    git_status_foreach_ext(repo, &opts, status_cb, &found);
    if (found) {
        write(1, C_ORANGE "꩜ " C_RESET, sizeof(C_ORANGE "꩜ " C_RESET) - 1);
        _exit(0);
    }

    /* 3. Ahead/Behind - Priority 3 */
    {
        git_reference *head = NULL;
        if (git_repository_head(&head, repo) == 0) {
            git_reference *upstream = NULL;
            if (git_branch_upstream(&upstream, head) == 0) {
                size_t ahead = 0, behind = 0;
                const git_oid *head_oid = git_reference_target(head);
                const git_oid *upstream_oid = git_reference_target(upstream);
                
                if (head_oid && upstream_oid) {
                    git_graph_ahead_behind(&ahead, &behind, repo, head_oid, upstream_oid);
                }
                
                if (ahead > 0) {
                    write(1, C_AQUA "⬤↑" C_RESET, sizeof(C_AQUA "⬤↑" C_RESET) - 1);
                    _exit(0);
                }
            }
        }
    }

    /* 4. Clean */
    write(1, C_GREEN "⬤ " C_RESET, sizeof(C_GREEN "⬤ " C_RESET) - 1);

    _exit(0);
}
