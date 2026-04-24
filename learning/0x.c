#include <stdio.h>

typedef int a;

int main(void) {
    a a = 40;
    printf("0x%lx\n", (unsigned long)&a);
    return 0;
}
