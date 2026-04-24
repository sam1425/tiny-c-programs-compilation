#include <stdio.h>
#include <cs50.c>
#include <stdlib.h>

int main(void)
{
    string user_input = get_string("Enter your name: ");
    printf("this is your name: %s\n", user_input);
    return 0;
}