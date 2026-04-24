#include <stdio.h>

int main(void)
{
    int n = 50;

    //variable who's purpose in life is to store the pointer of n
    int *p = &n;

    printf("%p\n", p);
    
    printf("0x%lx\n", (unsigned long)&p);
    return 1;
}
