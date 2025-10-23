#include <stdio.h>

int main() 
{
    int user = 0; 
    printf("What is your number? : ");
    scanf("%d", &user);
    for (int prime_check = 1; prime_check <= user; prime_check++)
    {
        int division = user % prime_check;
        printf("prime_check: %i\n", prime_check);
        printf("division: %i\n", division);
    }
    return 0;
}

