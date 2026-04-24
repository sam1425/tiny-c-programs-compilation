#include <stdio.h>
#include <string.h>

int parse_csv(){
FILE *f = fopen("dictionary.csv", "r");
char buffer[256];

while (fgets(buffer, sizeof(buffer), f)) {
    char *id = strtok(buffer, ",");
    char *name = strtok(NULL, ",");
    char *birthdate = strtok(NULL, "\n");

    printf("%s, Name: %s, Msg: %s\n", id, name, message);
}

fclose(f);

}
