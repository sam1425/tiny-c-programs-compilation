#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

int main(){
    int fd = open("./MyFileDescriptorFile", O_WRONLY | O_CREAT, 0644);
    if (fd == -1){
        return 1;
    }
    printf("fd = %d", fd);
    return 0;
}
