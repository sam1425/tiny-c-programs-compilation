#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

void center_internal(char *first, ...) {
    struct winsize w;
    int cols = 80;

    const char *env = getenv("COLUMNS");
    if (env) {
        cols = atoi(env);
    } else {
        int fd = open("/dev/tty", O_RDONLY);
        if (fd >= 0) {
            if (ioctl(fd, TIOCGWINSZ, &w) == 0 && w.ws_col > 0)
                cols = w.ws_col;
            close(fd);
        }
    }

    char buf[1024] = {0};
    va_list args;
    if (first) strncpy(buf, first, sizeof(buf) - 1);
    va_start(args, first);
    char *next;
    while ((next = va_arg(args, char *)) != NULL)
        strncat(buf, next, sizeof(buf) - strlen(buf) - 1);
    va_end(args);

    int position = (cols - (int)strlen(buf)) / 2;
    if (position > 0) printf("%*s", position, "");
    printf("%s\n", buf);
}

#define center(...) center_internal(__VA_ARGS__, NULL)
