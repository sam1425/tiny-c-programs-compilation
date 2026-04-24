#include <stdio.h>
#include <stdbool.h>

int main(void){

  bool unstopable = true;
  while (unstopable){
    unsigned int a = 65;
    printf("%c \n", a);
    a += 1;
  }
}
