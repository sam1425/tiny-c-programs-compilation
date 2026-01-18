// A very unsafe program that skips main and uses the acutual entry point _start
// Since I am using standard libraries without the c runtime environment
// it is undefined behavior for all of them


//TODO: Exclude std libraries and write directly to memory instead of using external libs


#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

void dotAnimation(int n)
{
    usleep(20000);
    for(int i = 0; i < n; i++)
    {
      printf(".");
      fflush(stdout);
      usleep(300000);
    }
    printf("\n");
}

void newCharacterAnimation(int n)
{
    usleep(20000);
    for(int i = 0; i < n; i++)
    {
      puts(" ");
      usleep(500000);
    }
}

int customMain() {
  printf("Printinf without Main!\n");
  printf("Do you think this is useful? : ");
  char input = getchar();
  if(input == 'Y' || input == 'y')
  {
      printf("Yeahh.. in most scenary is not useful BUT");
      newCharacterAnimation(4);
      puts("hear me out");
      usleep(500000);
      const char *msg = "This message was printed on asembly :D \n";
      asm volatile (
      "mov $1, %%rax\n"
      "mov $1, %%rdi\n"
      "mov %0, %%rsi\n"
      "mov $3, %%rdx\n"
      "syscall"
  :
      : "r"(msg)
      : "rax", "rdi", "rsi", "rdx", "memory"
      );

      asm volatile (
      "mov $60, %%rax\n"
      "xor %%rdi, %%rdi\n"
      "syscall"
  :
  :
      : "rax", "rdi"
      );
      puts("not suprised yet?")
      usleep(500000)
      puts("I give up :C ")
  }
  else if(input == 'N'|| input == 'n')
  {
      printf("Oh");
      fflush(stdout);
      dotAnimation(3);
      puts("closing program then :c");
      usleep(300000);
      return 0;
  }
  return 0;
}

void _start() {
  int Return = customMain();
  exit(Return);
}
