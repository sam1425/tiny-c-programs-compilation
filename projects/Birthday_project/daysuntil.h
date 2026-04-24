#ifndef DAYSUNTIL_H
#define DAYSUNTIL_H

#include <time.h>

void add_months(struct tm *date, int months);

void diff_months_days(struct tm start, struct tm end, int *out_months, int *out_days);

void countdown(int year, int month, int day);

#endif 
