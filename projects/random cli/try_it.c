#include <stdio.h>

int getPass(char *b);
int main(int argc, char **argv)
{
    char buffer[128];
    printf("Welcome to your first crackme problem!\n");
    printf("what is the password");
    scanf("%120s", buffer);

    if(getPass(buffer))
    {
        printf("Thats correct!\n");
        return 0;

    }
}
