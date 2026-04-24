#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>

int main() {
    const char *filepath = "/home/c0mplex/Documents/bible/RandomJs/kjv.txt";
    int fd = open(filepath, O_RDONLY);
    if (fd < 0) {
        perror("Error opening file");
        return 1;
    }

    struct stat st;
    if (fstat(fd, &st) < 0 || st.st_size == 0) {
        close(fd);
        return 1;
    }

    char *map = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (map == MAP_FAILED) {
        close(fd);
        return 1;
    }

    srand(time(NULL));
    off_t offset = rand() % st.st_size;

    while (offset > 0 && map[offset - 1] != '\n') {
        offset--;
    }

    while (offset < st.st_size && map[offset] != '\n') {
        putchar(map[offset]);
        offset++;
    }
    putchar('\n');

    munmap(map, st.st_size);
    close(fd);
    return 0;
}
