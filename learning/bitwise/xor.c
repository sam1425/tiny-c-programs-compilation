/*
* A simple program to swap a variable from one another using xor
*/

#include <stdio.h>

int main(void){
  int a = 59;
  int b = 10;
  printf("%d %d\n",a, b);
  a ^= b;
  b ^= a;
  a ^= b;
  printf("%d %d\n",a, b);
  return 0;
}
