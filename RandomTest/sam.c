#include <time.h>
#include <stdio.h>
#include <stdlib.h>


long int random(void);

void srandom(unsigned int seed);

char *initstate(unsigned int seed, char *state, size_t n);
char *setstate(char *state);

int main(void)
{
  srand(time(NULL));
  int r = (rand() % 10) + 1;
  printf("%d \n",r);
}
