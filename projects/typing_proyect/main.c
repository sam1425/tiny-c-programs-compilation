#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>

// Colors
#define GREEN "\033[32m"
#define RED   "\033[31m"
#define RESET "\033[0m"

// Clear screen
void clear_screen() {
    printf("\033[2J\033[H"); // Clear + move cursor to home
}

const char* words[] = {
    "cat", "dog", "apple", "banana", "zebra",
    "sam", "papaya", "yellow", "successfully"
};

void center_print(const char* text, int width) {
    int len = strlen(text);
    int pad = (width - len) / 2;
    if (pad < 0) pad = 0;
    for (int i = 0; i < pad; i++) printf(" ");
    printf("%s\n", text);
}

int main(void) {
    srand(time(NULL));
    int word_count = sizeof(words) / sizeof(words[0]);

    // Build sentence
    int N = 12;
    char sentence[512] = "";
    for (int i = 0; i < N; i++) {
        strcat(sentence, words[rand() % word_count]);
        if (i < N - 1) strcat(sentence, " ");
    }

    // Terminal width (default 80)
    int width = 80;
    char* env = getenv("COLUMNS");
    if (env) width = atoi(env);

    // Sentence padding
    int sentence_len = strlen(sentence);
    int pad = (width - sentence_len) / 2;
    if (pad < 0) pad = 0;

    // Clear and print header + sentence
    clear_screen();
    center_print("Welcome to my typing test c0mplex!", width);
    center_print(sentence, width);

    // Setup raw input
    struct termios original, modified;
    char c;
    tcgetattr(STDIN_FILENO, &original);
    modified = original;
    modified.c_lflag &= ~(ICANON | ECHO);
    modified.c_cc[VMIN] = 1;
    modified.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSANOW, &modified);

    // Move cursor to typing line, same padding
    printf("\n");
    for (int i = 0; i < pad; i++) printf(" ");
    fflush(stdout);

    // Typing loop
    int i = 0, mistakes = 0;
    while (1) {
        read(STDIN_FILENO, &c, 1);
        if (c == '\n') break;

        if (c == sentence[i]) {
            printf(GREEN "%c" RESET, c);
            fflush(stdout);
            i++;
            if (sentence[i] == '\0') {
                printf("\n\nDone!\nMistakes: %d\n", mistakes);
                break;
            }
        } else {
            printf(RED "%c" RESET, c);
            fflush(stdout);
            mistakes++;
        }
    }

    // Restore terminal
    tcsetattr(STDIN_FILENO, TCSANOW, &original);
    return 0;
}

