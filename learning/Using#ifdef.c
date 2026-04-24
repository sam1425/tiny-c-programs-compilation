#include <stdio.h>
#include <time.h>

#define SYSTEM "GNU/LINUX"
#define YEARS_OLD 19
#define ME "Samuel Dario Sierra Pacheco"



int main()
{
   struct tm tm_sec;
   struct tm* Local_Time;
   time_t t;
   t = time(NULL);

   Local_Time = localtime(&t);
   printf("Who am I?\n");
   #ifdef ME
   printf("I am %s, a guy that like computers a lot.\n", ME);
   #endif
   printf("The time is %s", asctime(Local_Time));
   return 0;
}
