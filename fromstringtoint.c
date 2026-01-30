#include <stdio.h>
#include <stdlib.h>


const int ARGUMENT_INDEX = 2;

char* stoi(char *string)
{
  return string;
}

int main(int argc, char* argv[])
{
  if (argc != ARGUMENT_INDEX) {
    printf("usage: command <string>\n");
    return EXIT_FAILURE;
  }
  char* input = argv[1];
  printf("%s\n",input);

  for (int i ; input[i] !='\0'; i++) {
    printf("%d\n", (int)input[i]);
  }

  printf("Status: %i\n", EXIT_SUCCESS);
  return EXIT_SUCCESS;
}
