#include <stdio.h>
#include <time.h>

int get_current_day_and_month(){
  time_t now = time(NULL);
  struct tm *lt = localtime(&now);
  int current_day= lt->tm_mday;
  int current_month = lt->tm_mon + 1;
  printf("%i %i\n", current_day, current_month);
  return 0;
}


int get_current_day_and_month(){
   time_t now = time(NULL);
   struct tm *lt = localtime(&now);
   int current_day= lt->tm_mday;
   int current_month = lt->tm_mon + 1;
   printf("%i %i\n", current_day, current_month);
   return 0;
}

