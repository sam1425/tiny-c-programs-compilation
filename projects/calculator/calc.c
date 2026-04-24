#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]){

  if(argc < 4)
  {
    printf("Usage: calc <operation> <number1> <number2>");
    return 1;
  }

  char op = argv[1][0];
  int Num1 = atoi(argv[2]);
  int Num2 = atoi(argv[3]);

  switch(op)
  {
    case '+': printf("%i\n", Num1 + Num2); break;
    case '-': printf("%i\n", Num1 - Num2); break;
    case '*': printf("%i\n", Num1 * Num2); break;
    case '/':
      if (Num2 != 0)
      {
      printf("%i\n", Num1 / Num2); break;
      }
  }
}
