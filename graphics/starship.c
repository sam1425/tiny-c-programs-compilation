#include <stdio.h>
#include <git2.h>

#define C_GREEN  "\033[38;2;152;151;26m"   /* #98971a */
#define C_YELLOW "\033[38;2;215;153;33m"   /* #d79921 */
#define C_PURPLE "\033[38;2;177;98;134m"   /* #b16286 */
#define C_AQUA   "\033[38;2;104;157;106m"  /* #689d6a */
#define C_RESET  "\033[0m"

/*# Priority 1 — Staged ◍
# Priority 2 — Modified/Untracked ○
# Priority 3 — Ahead ●⇡N
# Priority 4 — Clean ●*/
static const char SYM_CLEAN[]    = C_GREEN  "●"   C_RESET;
static const char SYM_STAGED[]   = C_YELLOW "◉"   C_RESET;
static const char SYM_MODIFIED[] = C_PURPLE "◎"   C_RESET;
static const char SYM_AHEAD[]    = C_AQUA   "●⇡"  C_RESET;

int main(int argc, char *argv[])
{
    git_libgit2_init();
    git_libgit2_shutdown();
    return 0;
}
