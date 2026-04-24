#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

double smart_round(double x) {
    double int_part = floor(x);
    double frac = x - int_part;

    if (frac < 0.25)
        return int_part;
    else if (frac < 0.75)
        return int_part + 0.5;
    else
        return int_part + 1.0;
}

int main() {
    FILE *in = fopen("ratings.csv", "r");
    FILE *out = fopen("rounded.csv", "w");

    if (!in || !out) {
        perror("Could not open file");
        return 1;
    }

    char line[256];
    int is_header = 1;

    while (fgets(line, sizeof(line), in)) {
        if (is_header) {
            fprintf(out, "%s", line);  // Copy header as-is
            is_header = 0;
            continue;
        }

        int id;
        char name[50];
        double rating;

        // Parse line (simple version, assumes no commas in name)
        if (sscanf(line, "%d,%49[^,],%lf", &id, name, &rating) == 3) {
            double rounded = smart_round(rating);
            fprintf(out, "%d,%s,%.1f\n", id, name, rounded);
        } else {
            fprintf(stderr, "Invalid line: %s", line);
        }
    }

    fclose(in);
    fclose(out);

    printf("Rounded ratings saved to 'rounded.csv'.\n");
    return 0;
}

