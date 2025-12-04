#include <stdio.h>
#include <stdint.h>
#include <time.h>

int main(){
  time_t start, now;
  start = time(NULL);
  uint32_t countdown;
  uint32_t scanned = scanf("%d", &countdown);
  if(scanned != 1)
  {
    perror("countdown is null, closing..");
    return 1;
  }
  while (1){
    now = time(NULL);

    if (now - start >= 1){
      start = now;
      printf("%i\n", countdown);
      countdown -= 1;
      if (countdown <= 0)
      {
        printf("countdown ended\n");
        return 0;
      }
    }
  }
}
