#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "daysuntil.h"
#include "centerbyenv.c"

#define SYSTEM "GNU/LINUX"
#define YEARS_OLD 19
#define ME "Sam"

int parse_csv();

typedef struct {
    char id[16];
    char name[32];
    char birthdate[64];
    int age;
} birthday;



int get_day_of_year(int m, int d) {
    static const int days[] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};
    return days[m - 1] + d;
}

void display_closest(birthday *persons, int count) {
    time_t now = time(NULL);
    struct tm lt = *localtime(&now);
    int today_rel = get_day_of_year(lt.tm_mon + 1, lt.tm_mday);

    int best_idx = -1;
    int min_diff = 366;

    for (int i = 0; i < count; i++) {
        int d, m, y;
        if (sscanf(persons[i].birthdate, "%d/%d/%d", &d, &m, &y) != 3) continue;

        int dob_rel = get_day_of_year(m, d);
        int diff = dob_rel - today_rel;
        if (diff < 0) diff += 365; 

        if (diff < min_diff) {
            min_diff = diff;
            best_idx = i;
        }
    }

    if (best_idx != -1) {
        int d, m, y;
        sscanf(persons[best_idx].birthdate, "%d/%d/%d", &d, &m, &y);

        struct tm target = lt;
        target.tm_mon = m - 1;
        target.tm_mday = d;
        if (get_day_of_year(m, d) < today_rel) target.tm_year++;
        mktime(&target);

        int out_m, out_d;
        diff_months_days(lt, target, &out_m, &out_d);

        center("Upcoming Birthday");
        center(persons[best_idx].name,"'s");

        char buf[64];
        if (out_m == 1) {
            snprintf(buf, sizeof(buf), "%d month and %d days remaining", out_m, out_d);
        } else if (out_m > 1) {
            snprintf(buf, sizeof(buf), "%d months and %d days remaining", out_m, out_d);
        } else if (out_d == 1){
            snprintf(buf, sizeof(buf), "%d day remaining", out_d);
        } else if (out_d > 1){
            snprintf(buf, sizeof(buf), "%d days remaining", out_d);
        } else {
            snprintf(buf, sizeof(buf), "Is  Today");
        }
        center(buf);
    }
}

int main() {
    //parse csv
    FILE *fp = fopen("/home/c0mplex/.scripts/dictionary.csv", "r");
    if (!fp) return 1;

    birthday people[100];
    char line[256];
    int count = 0;

    fgets(line, sizeof(line), fp);
    while (fgets(line, sizeof(line), fp) && count < 100) {
        char *id = strtok(line, ",");
        char *name = strtok(NULL, ",");
        char *date = strtok(NULL, "\n");

        if (name && date) {
            strncpy(people[count].name, name, 31);
            strncpy(people[count].birthdate, date, 63);
            count++;
        }
    }
    fclose(fp);

    display_closest(people, count);
    return 0;
}
