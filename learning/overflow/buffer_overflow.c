/* This is an example of how to not do things,
 * this is a buffer overflow since the buffer is 26 chars of data
 * And you are trying to access more bytes than the specified on the buffer 
 * so the buffer overflow leaving to unexpected behaviour,
 * it may not crash now or later but it is still memory unsafe code
 */


#include <stdio.h>
#include <string.h>

int main(void)
{
  char sam[26];
  char *input = "abcdefghijklmn;opqrstuvwxyz,.:";
  printf("Buffer length: %lu\n", sizeof(sam));
  printf("input length: %lu\n", strlen(input));

  for(unsigned long i; i < strlen(input);i++)
  {
    printf("%lu: ",i);
    sam[i] = input[i];
    printf("%c\n", sam[i]);
  }

  //printf("%s",sam);
  return 0;
}
