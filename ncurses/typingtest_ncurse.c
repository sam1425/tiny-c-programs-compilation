#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int main(void) {
    const char *words[] = {
        "cat", "dog", "apple", "banana", "zebra",
        "sam", "papaya", "yellow", "successfully"
    };
    const int word_count = sizeof(words) / sizeof(words[0]);
    srand(time(NULL));

    int N = 10;
    char sentence[1024] = "";
    for (int i = 0; i < N; i++) {
        if (i) strcat(sentence, " ");
        strcat(sentence, words[rand() % word_count]);
    }
    int sentence_len = (int)strlen(sentence);

    initscr();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);
    start_color();
    use_default_colors();
    init_pair(1, COLOR_GREEN, -1); 
    init_pair(2, COLOR_RED, -1);

    int rows, cols;
    getmaxyx(stdscr, rows, cols);

    const char *header = "Welcome to my typing test by c0mplex!";

    int header_row = rows / 2 - 2;
    int sentence_row = rows / 2;
    int typed_row = rows / 2 + 1;

    mvprintw(header_row, (cols - (int)strlen(header)) / 2, "%s", header);
    mvprintw(sentence_row, (cols - sentence_len) / 2, "%s", sentence);
    refresh();

    int start_col = (cols - sentence_len) / 2;
    int pos = 0;
    int *state = calloc(sentence_len, sizeof(int));
    int cursor;

    while (pos < sentence_len) {
cursor = getch();

if (cursor == KEY_BACKSPACE || cursor == 127 || cursor == 8) {
    if (pos > 0) {
        pos--;
        state[pos] = 0;
        mvaddch(typed_row, start_col + pos, ' ');
        move(typed_row, start_col + pos);
    }
    continue;
}

if (cursor < 32 || cursor > 126) continue;

char expected = sentence[pos];
if (cursor == expected) {
    state[pos] = 1;
    attron(COLOR_PAIR(1));
    mvaddch(typed_row, start_col + pos, cursor);
    attroff(COLOR_PAIR(1));
} else {
    state[pos] = -1;
    attron(COLOR_PAIR(2));
    mvaddch(typed_row, start_col + pos, cursor);
    attroff(COLOR_PAIR(2));
}
        pos++;
        move(typed_row, start_col + pos);
        refresh();
    }

    int mistakes = 0;
    for (int i = 0; i < sentence_len; i++)
        if (state[i] == -1) mistakes++;

    mvprintw(typed_row + 2, 2, "Done!\n Mistakes: %d", mistakes);
    refresh();

    getch();
    free(state);
    endwin();

    return 0;
}
