// A sleep implementation that is not like github's
//
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
  if (argc < 2)
  {
    printf("too few arguments");
    printf("Usage: sleep <seconds>\n");
    return 1;
  }
  if (argc > 2)
  {
    printf("too much arguments");
    printf("Usage: sleep <seconds>\n");
    return 1;
  }
  char *seconds_raw = argv[1];
  long seconds = atol(seconds_raw);

  printf("sleeping for %d seconds \n");
  sleep(seconds);

  return 0;
}
