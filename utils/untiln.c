#include <stdio.h>

int main() {
    char *string = "something --help";
    int count = 0;
    printf("%s\n",string);
    for(int i = 0; string[i] != '\0'; i++) {
        //printf("%c", string[i]);
        if (string[i] == '-'){
            break;
        }else{
            count++;
            printf("^");
        }
    }
    printf("\n");
    printf("%d\n", count);
    return 0;
}
