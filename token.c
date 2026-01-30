#include <stdio.h>
#include <string.h>


int main(const int argc, char* argv[])
{
  if (argc < 2) {
    printf("usage: command <minutes:seconds>");
    return 1;
  }

  char str[100];
  strcpy(str, argv[1]);

  char* minutes = strtok(str, ":");
  char* seconds = strtok(NULL, ":");

  if(minutes && seconds) {
    printf("Minutes: %s\n", minutes);
    printf("Seconds: %s\n", minutes);
  }
  else {
    printf("invalid formatting: command minutes:seconds\n separated by :");
  }
  return 0;
}
