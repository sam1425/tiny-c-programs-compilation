#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "daysuntil.h"
#include "center.c"

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


int main()
{
   //#ifdef ME
   //printf("A simple program for cheking birthdays made by %s.\n", ME);
   //#endif
   parse_csv();

   return 0;
}

int day_of_year(int month, int day) 
{
    int days_in_months[] = {
        0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
    };

    for (int i = 1; i < month; i++) {
        day += days_in_months[i];
    }
    return day;
}

void find_closest_birthday(birthday *persons, int count) 
    {
    time_t now = time(NULL);
    struct tm *lt = localtime(&now);

    int today = day_of_year(lt->tm_mon + 1, lt->tm_mday);

    int closest_index = 0;
    int closest_distance = 9999;
    bool found = false;

    for (int i = 0; i < count; i++) {
        int month, day, year;

        if (sscanf(persons[i].birthdate, "%d/%d/%d", &day, &month, &year) != 3)
            continue;

        int dob = day_of_year(month, day);

        int diff = dob - today;
        if (diff < 0) diff += 365;  // wrap around (birthday already passed)

        if (!found || diff < closest_distance) {
            closest_distance = diff;
            closest_index = i;
            found = true;
        }
    }

    if (found) {
        // Parse the birthday date
        int month, day, year;
        sscanf(persons[closest_index].birthdate, "%d/%d/%d", &day, &month, &year);
        
        // Create target date
        struct tm target_tm = {0};
        target_tm.tm_year = lt->tm_year;
        target_tm.tm_mon = month - 1;
        target_tm.tm_mday = day;
        mktime(&target_tm);
        
        // If birthday already passed this year, use next year
        if (day_of_year(month, day) < today) {
            target_tm.tm_year++;
            mktime(&target_tm);
        }
        
        // Calculate months and days
        int months, days;
        diff_months_days(*lt, target_tm, &months, &days);
        
        center("Closest birthday");
        center((persons[closest_index].name));
        center((persons[closest_index].birthdate));
        char stringbuffer[32];
        snprintf(stringbuffer, sizeof(stringbuffer), "%d days left", closest_distance);
        center(stringbuffer);
        snprintf(stringbuffer, sizeof(stringbuffer), "%d months %d days\n", months, days);
        center(stringbuffer);
    }
}

int count_lines(FILE *fp) {
    int lines = 0;
    char buffer[256];
    while (fgets(buffer, sizeof(buffer), fp)) {
        lines++;
    }
    return lines;
}


int parse_csv(){
   FILE *dictionary = fopen("dictionary.csv", "r");
   if (!dictionary) {
   perror("Could not open dictionary.csv");
   return -1;
   }

   char buffer[256];
   int TOTAL_LINES = count_lines(dictionary);
   if (TOTAL_LINES < 2) {
      fclose(dictionary);
      return 0;
   }

   int MAX_PEOPLE = TOTAL_LINES - 1;

   rewind(dictionary);
   fgets(buffer, sizeof(buffer), dictionary);

   birthday *person = calloc(MAX_PEOPLE, sizeof(birthday));
   if (!person) {
      fclose(dictionary);
      return -1;
   }

   int i = 0;
   while (fgets(buffer, sizeof(buffer), dictionary) && i < MAX_PEOPLE) {
      char* token = strtok(buffer,",");
      if (token) strncpy(person[i].id,token,sizeof(person[i].id));
      token = strtok(NULL, ",");
      if (token) strncpy(person[i].name,token,sizeof(person[i].name));
      token = strtok(NULL, "\n");
      if (token) strncpy(person[i].birthdate,token,sizeof(person[i].birthdate));

      //printf("%s, Name: %s, Birth: %s\n", person[i].id, person[i].name, person[i].birthdate);

      //if (sscanf(person[i].birthdate, "%d/%d/%d", &day, &month, &year) == 3) {
      //printf("Month: %d\nDay: %d\nYear: %d\n", month, day, year);
      //} else {
         //printf("Invalid date format.\n");
      //}


      i++;
   }

   find_closest_birthday(person, i);
   fclose(dictionary);
   free(person);
   return 0;
}
