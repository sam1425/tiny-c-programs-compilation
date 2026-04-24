#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* fizzbuzz(int number);

int main (void)
{
  for(int numbers = 1; numbers < 100; numbers++)
  {
    char* result = fizzbuzz(numbers);
    printf("number: %i   |   result : %s\n",numbers,result );
    free(result);
  }
  return 0;
}

char* fizzbuzz(int number)
{
  char* buffer = malloc(16);
  buffer[0] = '\0';

  if(number % 3 == 0) buffer = strcat(buffer, "fizz");
  if(number % 5 == 0) buffer = strcat(buffer, "buzz");
  if(buffer[0] == '\0')
  {
    //concatenate
    asprintf(&buffer, "%d", number);
  }
  return buffer;
}
