#include <stdio.h>

int main(void){
  long num = 1 ;
  while(1)
  {
    printf("You have $%li Do you want to double it to the next person?:", num);
    getchar();
    num = num * 2;
  }
  return 0;
}
