#include <stdio.h>
#include <unistd.h>

int main(){
    FILE *fd = fopen("/home/c0mplex/.scripts/shellaliases", "r");
    if (fd == NULL) {
        perror("Error opening file");
        return 1; 
    }
    int ch;

    while ((ch = fgetc(fd)) != EOF) {
        putchar(ch);
    }

    fclose(fd);
    return 0;
}
