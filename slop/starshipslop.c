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

static int diff_cb(const git_diff_delta *delta, float progress, void *payload) {
    (void)delta; (void)progress;
    *(int *)payload = 1;
    return 1; /* abort immediately on first change */
}

int main(int argc, char *argv[]) {
    const char *path = (argc >= 2) ? argv[1] : getenv("PWD");
    if (!path) return 0;

    /* 1. Fast Upward Discovery for .git */
    char cur[4096];
    struct stat st;
    snprintf(cur, sizeof(cur), "%s", path);
    
    while (1) {
        char gitdir[4112]; /* path + /.git */
        snprintf(gitdir, sizeof(gitdir), "%s/.git", cur);
        if (stat(gitdir, &st) == 0) break;
        char *last = strrchr(cur, '/');
        if (!last || last == cur) _exit(0);
        *last = '\0';
    }

    git_libgit2_init();
    git_repository *repo = NULL;
    if (git_repository_open_ext(&repo, path, GIT_REPOSITORY_OPEN_FROM_ENV, NULL) != 0) _exit(0);

    int found = 0;
    /* Staged */
    {
        git_tree *head_tree = NULL;
        git_reference *head_ref = NULL;
        if (git_repository_head(&head_ref, repo) == 0) {
            git_object *head_obj = NULL;
            if (git_reference_peel(&head_obj, head_ref, GIT_OBJECT_TREE) == 0) head_tree = (git_tree *)head_obj;
            git_reference_free(head_ref);
        }
        
        git_diff_options opts;
        git_diff_options_init(&opts, GIT_DIFF_OPTIONS_VERSION);
        opts.flags = GIT_DIFF_DISABLE_PATHSPEC_MATCH | GIT_DIFF_SKIP_BINARY_CHECK | GIT_DIFF_IGNORE_SUBMODULES;
        
        git_diff *diff = NULL;
        if (git_diff_tree_to_index(&diff, repo, head_tree, NULL, &opts) == 0) {
            git_diff_foreach(diff, diff_cb, NULL, NULL, NULL, &found);
            git_diff_free(diff);
        }
        if (head_tree) git_tree_free(head_tree);
        if (found) { write(1, C_YELLOW "🔘 " C_RESET, sizeof(C_YELLOW "🔘 " C_RESET)-1); _exit(0); }
    }

    /* Workdir & Untracked (No Recursion) */
    {
        git_diff_options opts;
        git_diff_options_init(&opts, GIT_DIFF_OPTIONS_VERSION);
        opts.flags = GIT_DIFF_INCLUDE_UNTRACKED | GIT_DIFF_DISABLE_PATHSPEC_MATCH | GIT_DIFF_SKIP_BINARY_CHECK | GIT_DIFF_IGNORE_SUBMODULES;
        
        git_diff *diff = NULL;
        if (git_diff_index_to_workdir(&diff, repo, NULL, &opts) == 0) {
            git_diff_foreach(diff, diff_cb, NULL, NULL, NULL, &found);
            git_diff_free(diff);
        }
        if (found) { write(1, C_ORANGE "꩜ " C_RESET, sizeof(C_ORANGE "꩜ " C_RESET)-1); _exit(0); }
    }

    /* Ahead/Behind (Fast Path) */
    {
        git_reference *head = NULL;
        if (git_repository_head(&head, repo) == 0) {
            git_reference *upstream = NULL;
            if (git_branch_upstream(&upstream, head) == 0) {
                const git_oid *h = git_reference_target(head), *u = git_reference_target(upstream);
                if (h && u && git_oid_cmp(h, u) != 0) {
                    size_t a = 0, b = 0;
                    git_graph_ahead_behind(&a, &b, repo, h, u);
                    if (a > 0) { write(1, C_AQUA "⬤↑" C_RESET, sizeof(C_AQUA "⬤↑" C_RESET)-1); _exit(0); }
                }
                git_reference_free(upstream);
            }
            git_reference_free(head);
        }
    }

    write(1, C_GREEN "⬤ " C_RESET, sizeof(C_GREEN "⬤ " C_RESET)-1);
    _exit(0);
}
