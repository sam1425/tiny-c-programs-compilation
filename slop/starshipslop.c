/* gitstatus.c – single-shot git status symbol for shell prompts
 *
 * Runs once, prints a symbol, exits. No daemon, no socket, no state.
 * Fast because it calls libgit2 directly — zero shell subprocesses.
 *
 * Usage
 * ─────────────────────────────────────────────────────────────────
 *   gitstatus [path]     path defaults to $PWD
 *
 * Output (one line, ANSI coloured, empty if not in a git repo)
 * ─────────────────────────────────────────────────────────────────
 *   ◉  yellow  staged changes ready to commit          (priority 1)
 *   ◎  purple  unstaged modifications or untracked     (priority 2)
 *   ●⇡ aqua    clean tree but ahead of upstream        (priority 3)
 *   ●  green   fully clean                             (priority 4)
 * ─────────────────────────────────────────────────────────────────
 *   alternative
 *   ∅ 〇 ◯ ◯ ◎ ○⚬ ⊙ ◍ ☢ ❂
 *   ◯ ❍ ⬤ ☯ 𖣠 ⚆ ⚇ ⚈ ⚉ ⭕
 *   ⬤ ⊗ ⚫꩜ 🌀 ⚪🔴🟡🔵
 *   ✦	✖	▲	✔
 *   ◆	◈	◇⇡	◇
 *   ●	⬢	○⇡ ■ ❖ ◈ ⌬
 * Build
 * ─────────────────────────────────────────────────────────────────
 *   make
 *   make install    # → ~/.local/bin/gitstatus
 *
 * Starship custom module
 * ─────────────────────────────────────────────────────────────────
 *   [custom.git_dot]
 *   command = 'gitstatus'
 *   when    = 'git rev-parse --is-inside-work-tree 2>/dev/null'
 *   format  = '[ $output ]($style)'
 *   style   = 'bg:color_blue'
 *   shell   = ['bash', '--noprofile', '--norc']
 */

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <git2.h>

/* ── gruvbox dark colours ────────────────────────────────────────── */

#define C_GREEN  "\033[1;38;2;184;187;38m"   /* bright green  #b8bb26 */
#define C_YELLOW "\033[1;38;2;250;189;47m"   /* bright yellow #fabd2f */
#define C_YELLOW_BOLD "\033[1;38;2;250;189;47m"
#define C_PURPLE "\033[1;38;2;211;134;155m"  /* bright pink   #d3869b */
#define C_AQUA   "\033[1;38;2;142;192;124m"  /* bright aqua   #8ec07c */
#define C_ORANGE "\033[1;38;2;254;128;25m"  /* Gruvbox Orange #fe8019 */
#define C_RESET  "\033[0m"

/* ── entry point ─────────────────────────────────────────────────── */

int main(int argc, char *argv[])
{
    const char *path = (argc >= 2) ? argv[1] : getenv("PWD");
    if (!path) return 0;

    git_libgit2_init();

    /* ── 1. discover the repo root ───────────────────────────────── */
    git_buf disc = GIT_BUF_INIT;
    if (git_repository_discover(&disc, path, 0, NULL) != 0) {
        git_libgit2_shutdown();
        return 0;   /* not inside a git repo — print nothing */
    }

    git_repository *repo = NULL;
    if (git_repository_open(&repo, disc.ptr) != 0) {
        git_buf_dispose(&disc);
        git_libgit2_shutdown();
        return 0;
    }
    git_buf_dispose(&disc);

    /* ── 2. check working tree and index ─────────────────────────── */
    git_status_options opts;
    /*opts.version = 1;  GIT_STATUS_OPTIONS_VERSION
    opts.show    = GIT_STATUS_SHOW_INDEX_AND_WORKDIR;

    opts.flags   = GIT_STATUS_OPT_INCLUDE_UNTRACKED
                 | GIT_STATUS_OPT_EXCLUDE_SUBMODULES;*/


    opts.flags = GIT_STATUS_OPT_INCLUDE_UNTRACKED        |
                 GIT_STATUS_OPT_RENAMES_HEAD_TO_INDEX    |
                 GIT_STATUS_OPT_DEFAULTS                 |
                 GIT_STATUS_OPT_NO_REFRESH               |
                 GIT_STATUS_OPT_EXCLUDE_SUBMODULES       |
                 GIT_STATUS_OPT_DISABLE_PATHSPEC_MATCH;
    opts.version = GIT_STATUS_OPTIONS_VERSION;
    opts.show    = GIT_STATUS_SHOW_INDEX_AND_WORKDIR;

    memset(&opts, 0, sizeof(opts));

    int staged = 0, dirty = 0;

    git_status_list *slist = NULL;
    if (git_status_list_new(&slist, repo, &opts) == 0) {
        size_t n = git_status_list_entrycount(slist);

        for (size_t i = 0; i < n; i++) {
            const git_status_entry *e = git_status_byindex(slist, i);
            git_status_t s = e->status;

            if (s & (GIT_STATUS_INDEX_NEW       |
                     GIT_STATUS_INDEX_MODIFIED  |
                     GIT_STATUS_INDEX_DELETED   |
                     GIT_STATUS_INDEX_RENAMED   |
                     GIT_STATUS_INDEX_TYPECHANGE))
                staged = 1;

            if (!staged &&
                s & (GIT_STATUS_WT_MODIFIED  |
                     GIT_STATUS_WT_DELETED   |
                     GIT_STATUS_WT_NEW       |
                     GIT_STATUS_WT_TYPECHANGE))
                dirty = 1;
        }
        git_status_list_free(slist);
    }

    /* ── 3. priority decision ────────────────────────────────────── */
    if (staged) {
        printf(C_YELLOW "꩜ " C_RESET);
        goto done;
    }
    if (dirty) {
        printf(C_ORANGE "〇" C_RESET);
        goto done;
    }

    /* ── 4. clean — check if ahead of upstream ───────────────────── */
    {
        size_t ahead = 0, behind = 0;
        git_reference *head = NULL, *upstream = NULL;

        if (git_repository_head(&head, repo) == 0 &&
            git_branch_upstream(&upstream, head) == 0 &&
            git_reference_target(head)     != NULL    &&
            git_reference_target(upstream) != NULL)
        {
            git_graph_ahead_behind(&ahead, &behind, repo,
                                   git_reference_target(head),
                                   git_reference_target(upstream));
        }

        if (upstream) git_reference_free(upstream);
        if (head)     git_reference_free(head);

        if (ahead > 0)
            printf(C_AQUA  "⬤↑ " C_RESET);
        else
            printf(C_GREEN "⬤ " C_RESET);
    }

done:
    git_repository_free(repo);
    git_libgit2_shutdown();
    return 0;
}
