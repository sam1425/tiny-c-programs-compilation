#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define CACHE_FILE    "/tmp/task_today"

#define COLOR_GREEN   "\x08" // Array index 7
#define COLOR_RED     "\x09" // Array index 8
#define COLOR_RESTORE "\x03" // Array index 2

int main(void) {
    int completed = 0;
    char *button = getenv("BLOCK_BUTTON");
    char *cmd;

    if (button) {
        char cmd[256];
        // Formats the command string securely into the buffer
        snprintf(cmd, sizeof(cmd), "notify-send \"something\" \"%s\"", button);
        system(cmd);
    }

    if (access(CACHE_FILE, F_OK) == 0) {
        FILE *f = fopen(CACHE_FILE, "r");
        if (f) {
            completed = (fgetc(f) == '1');
            fclose(f);
        }
    }

    if (button && strcmp(button, "1") == 0) {
        completed = !completed;
        FILE *f = fopen(CACHE_FILE, "w");
        if (f) {
            fputc(completed ? '1' : '0', f);
            fclose(f);
        }
    }

    if (completed) {
        printf(COLOR_GREEN "X" COLOR_RESTORE);
    } else {
        printf(COLOR_RED "C" COLOR_RESTORE);
    }

    return 0;
}
