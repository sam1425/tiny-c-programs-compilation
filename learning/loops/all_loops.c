/*
* Using C23
* Tiny Program to learn properly the usage of loops in c
* by samuel aka c0mplex
*/
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>


void for2()
{
  for(size_t i = 0; i < 10; i++)
  {
    printf("loop iteration: %i\n", i);
  }
}

int while2(void){
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

int do2()
{
  size_t i = 0;
  do
  {
    printf("%i\n", i);
    i++;
  } while(i<10);
}


int main(int argc, char **argv)
{
  if (argc < 2) {
    printf("Usage: %s <command>\n", argv[0]);
    printf("Commands: for, while, do, infinite\n");
    return 1;
  }
  else if (strcmp(argv[1], "for" ) == 0)
  {
    for2();
  }
  else if (strcmp(argv[1], "while" ) == 0)
  {
    while2();
  }
  else if (strcmp(argv[1], "do" ) == 0)
  {
    do2();
  }
  else
  {
    printf("command not found\n");
    return 1;
  }
  return 0;
}
