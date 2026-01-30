#include <stdio.h>

int main() 
{
    int user = 0; 
    int is_prime = 1;

    printf("What is your number? : ");
    scanf("%d", &user);
    if (user <= 1) {
        is_prime = 0;
    } 
    else {
        for (int i = 2; i <= user / 2; i++) {
            if (user % i == 0) {
                is_prime = 0;
                break;
            }
        }
    }

    if (is_prime)
        printf("%d is a prime number.\n", user);
    else
        printf("%d is not a prime number.\n", user);
    return 0;
}
