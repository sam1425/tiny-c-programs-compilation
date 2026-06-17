#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <strings.h>
#include "daysuntil.h"
#include "centerbyenv.c"

#define SYSTEM "GNU/LINUX"
#define YEARS_OLD 19
#define ME "Sam"


#define MAX_PEOPLE 100

typedef struct birthday birthday;

struct birthday{
    char id[16];
    char name[32];
    char birthdate[64];
    int age;
    birthday *left;
    birthday *right;
};

int get_day_of_year(int m, int d) {
    static const int days[] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};
    return days[m - 1] + d;
}

birthday* insert_node(birthday *root, birthday *new_person) {
    if (root == NULL) {
        birthday *node = malloc(sizeof(birthday));
        if (!node) return NULL;

        // Copy data from stack structure
        *node = *new_person;

        // CRITICAL: Explicitly clear pointers to avoid copying stack garbage
        node->left = NULL;
        node->right = NULL;
        return node;
    }

    int cmp = strcasecmp(new_person->name, root->name);
    if (cmp < 0) {
        root->left = insert_node(root->left, new_person);
    } else {
        root->right = insert_node(root->right, new_person);
    }
    return root;
}

birthday* search_tree(birthday *root, const char *name) {
    if (root == NULL) return NULL;

    // Case-insensitive comparison for lookup
    int cmp = strcasecmp(name, root->name);
    if (cmp == 0) return root;
    if (cmp < 0) return search_tree(root->left, name);
    return search_tree(root->right, name);

}

void find_closest_node(birthday *root, int today_rel, birthday **best_node, int *min_diff) {
    if (root == NULL) return;

    int d, m, y;
    if (sscanf(root->birthdate, "%d/%d/%d", &d, &m, &y) == 3) {
        int dob_rel = get_day_of_year(m, d);
        int diff = dob_rel - today_rel;
        if (diff < 0) diff += 365;

        if (diff < *min_diff) {
            *min_diff = diff;
            *best_node = root;
        }
    }

    find_closest_node(root->left, today_rel, best_node, min_diff);
    find_closest_node(root->right, today_rel, best_node, min_diff);
}

void free_tree(birthday *root) {
    if (root == NULL) return;
    free_tree(root->left);
    free_tree(root->right);
    free(root);
}


void render_birthday_output(birthday *person, int today_rel, struct tm lt) {
    int d, m, y;
    sscanf(person->birthdate, "%d/%d/%d", &d, &m, &y);

    struct tm target = lt;
    target.tm_mon = m - 1;
    target.tm_mday = d;
    if (get_day_of_year(m, d) < today_rel) target.tm_year++;
    mktime(&target);

    int out_m, out_d;
    diff_months_days(lt, target, &out_m, &out_d);

    center("Upcoming Birthday");
    center(person->name, "'s");

    char buf[64];
    if (out_m == 1) {
        snprintf(buf, sizeof(buf), "%d month and %d days remaining", out_m, out_d);
    } else if (out_m > 1) {
        snprintf(buf, sizeof(buf), "%d months and %d days remaining", out_m, out_d);
    } else if (out_d == 1) {
        snprintf(buf, sizeof(buf), "%d day remaining", out_d);
    } else if (out_d > 1) {
        snprintf(buf, sizeof(buf), "%d days remaining", out_d);
    } else {
        snprintf(buf, sizeof(buf), "Is  Today");
    }
    center(buf);
}

int main(int argc, char *argv[]) {
    FILE *fp = fopen("/home/c0mplex/.scripts/dictionary.csv", "r");
    if (!fp) return 1;

    birthday *root = NULL;
    char line[256];

    if (!fgets(line, sizeof(line), fp)) {
        fclose(fp);
        return 1;
    }

    while (fgets(line, sizeof(line), fp)) {
        line[strcspn(line, "\r\n")] = 0;

        char *id = strtok(line, ",");
        char *name = strtok(NULL, ",");
        char *date = strtok(NULL, ",");

        if (name && date) {
            birthday temp;
            memset(&temp, 0, sizeof(birthday));
            if (id) strncpy(temp.id, id, 15);
            strncpy(temp.name, name, 31);
            strncpy(temp.birthdate, date, 63);

            root = insert_node(root, &temp);
        }
    }
    fclose(fp);

    time_t now = time(NULL);
    struct tm lt = *localtime(&now);
    int today_rel = get_day_of_year(lt.tm_mon + 1, lt.tm_mday);

    if (argc >= 2) {
        birthday *found = search_tree(root, argv[1]);
        if (found) {
            render_birthday_output(found, today_rel, lt);
        } else {
            printf("Person '%s' not found.\n", argv[1]);
        }
    } else {
        birthday *best_node = NULL;
        int min_diff = 366;
        find_closest_node(root, today_rel, &best_node, &min_diff);

        if (best_node) {
            render_birthday_output(best_node, today_rel, lt);
        }
    }

    free_tree(root);
    return 0;
}
