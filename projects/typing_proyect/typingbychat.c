#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <signal.h>

#define GREEN "\033[32m"
#define RED   "\033[31m"
#define RESET "\033[0m"
#define CLEAR_SCREEN "\033[2J\033[H"

struct termios orig_term;

void restore_terminal(void) {
    tcsetattr(STDIN_FILENO, TCSANOW, &orig_term);
}

void handle_sigint(int sig) {
    (void)sig;
    restore_terminal();
    exit(1);
}

void get_term_size(int *rows, int *cols) {
    struct winsize w;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0) {
        *cols = w.ws_col ? w.ws_col : 80;
        *rows = w.ws_row ? w.ws_row : 24;
    } else {
        *cols = 80;
        *rows = 24;
    }
}

/* move cursor and print string */
void print_at(int row, int col, const char *s) {
    if (row < 1) row = 1;
    if (col < 1) col = 1;
    printf("\033[%d;%dH%s", row, col, s);
    fflush(stdout);
}

int main(void) {
    const char *words[] = {
        "cat", "dog", "apple", "banana", "zebra",
        "sam", "papaya", "yellow", "successfully", 
        "incledible", "impossible", "impressive" 
    };
    const int word_count = sizeof(words) / sizeof(words[0]);
    srand(time(NULL));

    int N = 10;
    char sentence[1024] = "";
    for (int i = 0; i < N; ++i) {
        if (i) strcat(sentence, " ");
        strcat(sentence, words[rand() % word_count]);
    }

    int rows, cols;
    get_term_size(&rows, &cols);

    const char *header = "Welcome to my typing test c0mplex!";
    int header_len = (int)strlen(header);
    int sentence_len = (int)strlen(sentence);

    /* if sentence wider than terminal, clip N down (simple fallback) */
    if (sentence_len > cols - 2) {
        /* try fewer words until it fits */
        while (sentence_len > cols - 2 && N > 1) {
            N--;
            sentence[0] = '\0';
            for (int i = 0; i < N; ++i) {
                if (i) strcat(sentence, " ");
                strcat(sentence, words[rand() % word_count]);
            }
            sentence_len = (int)strlen(sentence);
        }
    }

    /* vertical centering for a small block: header, sentence, typed line */
    int block_height = 3;
    int start_row = (rows - block_height) / 2;
    if (start_row < 1) start_row = 1;
    int header_row = start_row;
    int sentence_row = start_row + 1;
    int typed_row = start_row + 2;

    /* compute horizontal pad (1-based column positions) */
    int header_col = (cols - header_len) / 2 + 1;
    if (header_col < 1) header_col = 1;
    int sentence_col = (cols - sentence_len) / 2 + 1;
    if (sentence_col < 1) sentence_col = 1;

    /* clear screen and print centered header + sentence */
    printf(CLEAR_SCREEN);
    print_at(header_row, header_col, header);
    print_at(sentence_row, sentence_col, sentence);

    /* prepare terminal raw mode */
    tcgetattr(STDIN_FILENO, &orig_term);
    atexit(restore_terminal);
    signal(SIGINT, handle_sigint);
    struct termios raw = orig_term;
    raw.c_lflag &= ~(ICANON | ECHO); /* raw-ish */
    raw.c_cc[VMIN] = 1;
    raw.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSANOW, &raw);

    /* state array:
       0 = not typed yet
       1 = correct
      -1 = incorrect
    */
    int *state = calloc(sentence_len, sizeof(int));
    if (!state) {
        restore_terminal();
        fprintf(stderr, "allocation failed\n");
        return 2;
    }

    /* position cursor where typing should start */
    print_at(typed_row, sentence_col, ""); /* move cursor to typing start */
    fflush(stdout);

    int pos = 0;
    char c;
    while (pos < sentence_len) {
        ssize_t r = read(STDIN_FILENO, &c, 1);
        if (r <= 0) continue;

        /* handle escape / arrow sequences: consume extra bytes and ignore */
        if ((unsigned char)c == 0x1B) { /* ESC */
            /* try to read up to 2 more bytes (like "[A") and ignore */
            char seq;
            if (read(STDIN_FILENO, &seq, 1) > 0) {
                if (seq == '[') {
                    read(STDIN_FILENO, &seq, 1); /* consume */
                }
            }
            continue;
        }

        /* backspace (127 DEL or 8) */
        if ((unsigned char)c == 127 || (unsigned char)c == 8) {
            if (pos > 0) {
                pos--;
                state[pos] = 0;
                /* move cursor to that column, erase, and reposition */
                print_at(typed_row, sentence_col + pos, " ");
                print_at(typed_row, sentence_col + pos, ""); /* reposition */
                fflush(stdout);
            }
            continue;
        }

        /* ignore other control chars */
        if ((unsigned char)c < 32) continue;

        /* compare with expected character */
        char expected = sentence[pos];
        if (c == expected) {
            state[pos] = 1;
            /* paint green at the exact column */
            printf("\033[%d;%dH%s%c%s", typed_row, sentence_col + pos, GREEN, c, RESET);
        } else {
            state[pos] = -1;
            printf("\033[%d;%dH%s%c%s", typed_row, sentence_col + pos, RED, c, RESET);
        }
        fflush(stdout);
        pos++;
    }

    /* finished typing the whole sentence */
    int mistakes = 0;
    for (int i = 0; i < sentence_len; ++i) if (state[i] == -1) ++mistakes;

    /* print results below (left column) so they don't interfere with centered block */
    int result_row = typed_row + 2;
    if (result_row > rows) result_row = rows;
    print_at(result_row, 1, "Done!");
    char buf[64];
    snprintf(buf, sizeof(buf), "Mistakes: %d\n", mistakes);
    print_at(result_row + 1, 1, buf);

    /* cleanup */
    restore_terminal();
    free(state);
    return 0;
}

