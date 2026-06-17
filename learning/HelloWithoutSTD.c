#include <unistd.h>

//you can make use of syscalls directly without using stdio
//1 is stdout
//2 is stderr
//3 and so on are file descriptors
int main(void){
    write(1, "Hello, from the stdout World\n", 29);
    write(2, "Hello, from the error World\n", 29);
}
