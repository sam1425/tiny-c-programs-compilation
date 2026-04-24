#include <stdio.h>

#define MAX 10

int main()
{
  int key[MAX];
  for(int i = 0; i < (sizeof(key)/sizeof(key[0])); i++)
  {
    key[i] = i;
    int buffer = key[i] % MAX;
    printf("index: %i \n number: %i\n buffer: %i\n",i, key[i],buffer);
  }
}
