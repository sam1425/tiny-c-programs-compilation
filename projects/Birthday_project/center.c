//A simple program to center text on the screen
//Made by sam @2025
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

void center_internal(char *first, ...) {
    struct winsize w;
    ioc                    tl(STDOUT_FILENO, TIOCGWINSZ, &w);

    char buf[1024] = {0};
    va_list args;
    
    if (first) {
        strncpy(buf, first, sizeof(buf) - 1);
    }

    va_start(args, first);
    char *next;
    while ((next = va_arg(args, char *)) != NULL) {
        strncat(buf, next, sizeof(buf) - strlen(buf) - 1);
    }
    va_end(args);

    int textlength = strlen(buf);
    int position = (w.ws_col - textlength) / 2;

    if (position > 0) printf("%*s", position, "");
    printf("%s\n", buf);
}

#define center(...) center_internal(__VA_ARGS__, NULL)
