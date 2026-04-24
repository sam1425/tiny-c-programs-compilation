#include <stdio.h>
#include<unistd.h>

int main(void){
  int count = 0; 
  while( count < 20)
  {
    printf("U");
    printf("n");
    printf("U\n");
    sleep(1);
  }
  return 0;
}

