#include <stdio.h>
#include <time.h>
#include <unistd.h>

//adds a year if month is > 12
void add_months(struct tm *date, int months) {
    date->tm_mon += months;
    mktime(date); 
}

// calculates months and days between two dates
void diff_months_days(struct tm start, struct tm end, int *out_months, int *out_days) {
    int months = 0;

    struct tm temp = start;

    while (1) {
        struct tm next = temp;
        add_months(&next, 1);

        if (mktime(&next) > mktime(&end))
            break;

        temp = next;
        months++;
    }

    time_t t_temp = mktime(&temp);
    time_t t_end  = mktime(&end);

    int days = (t_end - t_temp) / 86400;

    *out_months = months;
    *out_days = days;
}

void countdown(int year, int month, int day) {
    struct tm end_tm = {0};
    end_tm.tm_year = year - 1900;
    end_tm.tm_mon  = month - 1;
    end_tm.tm_mday = day;

    time_t end_t = mktime(&end_tm);

        time_t now_t = time(NULL);
        struct tm now_tm = *localtime(&now_t);

        if (difftime(end_t, now_t) <= 0) {
            printf("EXPIRED!\n");
        }

        int months, days;
        diff_months_days(now_tm, end_tm, &months, &days);

        printf("\r%02d months %02d days remaining  \n", months, days);
        fflush(stdout);

}

