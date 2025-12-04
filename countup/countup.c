#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <string.h>


void disable_raw_mode();
void enable_raw_mode();
void get_terminal_size(int *rows, int *cols);
void clear_screen();
void draw_centered_number(int count);

int main() {
    enable_raw_mode();

    int count = 0;
    draw_centered_number(count);

    char c;
    while (read(STDIN_FILENO, &c, 1) == 1) {
        if (c == ' ') {
            count++;
            draw_centered_number(count);
        } else if (c == 'q') {
            break;
        }
    }

    return 0;
}

struct termios orig_termios;

void disable_raw_mode() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

void enable_raw_mode() {
    tcgetattr(STDIN_FILENO, &orig_termios);
    atexit(disable_raw_mode);

    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ECHO | ICANON); // Turn off echo and canonical mode
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void get_terminal_size(int *rows, int *cols) {
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    *rows = w.ws_row;
    *cols = w.ws_col;
}
void clear_screen() {
    printf("\033[2J");      // Clear screen
    printf("\033[H");       // Move cursor to top-left
}

void draw_centered_number(int count) {
    int rows, cols;
    get_terminal_size(&rows, &cols);

    char buffer[32];
    snprintf(buffer, sizeof(buffer), "%d", count);

    int x = (cols - (int)strlen(buffer)) / 2;
    int y = rows / 2;

    clear_screen();
    printf("\033[%d;%dH%s", y, x, buffer); // Move cursor and print
    fflush(stdout);
}
