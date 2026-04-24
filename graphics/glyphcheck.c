#include <stdio.h>
#include <stdlib.h>

int main(void) {
    /*printf("%-8s | %-12s | %-12s\n", "Char", "Normal", "Bolded");
    printf("--------------------------------------------\n");

    for (int i = 33; i <= 126; i++) {
        printf("%-8d | \033[38;2;184;187;38m %c \033[0m    | \033[1;38;2;184;187;38m %c \033[0m\n", 
               i, (char)i, (char)i);
    }*/

    printf("--------------------------------------------\n");
    const char *extra[] = {"◉", "◎", "⬤","⬤", "✦", "✖", "▲", "✔", "꩜","∅","🔴","⭕","○","◯","⊙","⊗","◍","❍","◇","◈","☢","❂","⌬","⚇","🔘","⚫ "};

    /*
    *    〇  ◯ ◎ ⚬   ●● 〇🌕
    *     ⬤ ☯ 𖣠 ⚆  ⚈ ⚉ 
    *     ⚫꩜ 🌀 ⚪🟡🔵
    *   ✦	✖	▲	✔
    *   ◆		◇⇡	
    *   ●	⬢	○⇡ ■ ❖ ◈ 
    */
    printf("\nExtended Symbols:\n");
    size_t count = sizeof(extra) / sizeof(extra[0]);
    for (int i = 0; i < count; i++) {
        printf("%-8s | \033[38;2;250;189;47m %s \033[0m    | \033[1;38;2;250;189;47m %s \033[0m\n", 
               extra[i], extra[i], extra[i]);
    }

    return 0;
}
