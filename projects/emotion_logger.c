#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

char *phrases[] = {
    "Emotions pass, actions remain.",
    "Even the smallest step forward is still progress.",
    "Chicks dig scars, pain heals, glory last forewa",
    "You can't fix anything until you fix yourserlf"
    };

void trim_newline(char *s) {
    for (int i = 0; s[i]; i++) {
        if (s[i] == '\n') {
            s[i] = '\0';
            return;
        }
    }
}


bool CheckIfEmpty(char emotion[64], char note[256])
{
    if (emotion[0] == '\0' || note[0] == '\0') {
        printf("No input. Nothing saved.\n");
        return true;
    }
    return false;
}

int main() {
    size_t upper_bound = (sizeof(phrases) / sizeof(phrases[0]));
    // random number(0 to upper_bound)
    int value = rand() % upper_bound; 

    char emotion[64];
    char note[256];

    printf("Emotion Loggin tool\n");

    printf("\nWhat are you feeling right now?\n > ");

    fgets(emotion, sizeof(emotion), stdin);
    trim_newline(emotion);
    if (CheckIfEmpty(emotion, note))return 1;

    printf("\nWrite about why you feel this way:\n > ");

    fgets(note, sizeof(note), stdin);
    trim_newline(note);
    if (CheckIfEmpty(emotion, note))return 1;

    FILE *file = fopen("emotions.log", "a");
    if (!file) {
        perror("Could not open file");
        return 1;
    }

    time_t now = time(NULL);
    fprintf(file, "\n[%s] \nEmotion: %s\nNote: %s",
            ctime(&now), emotion, note);

    fclose(file);

    printf("\nLogged.\n");
    printf("Cliche phrase here < %s >\n", phrases[value]);
    return 0;
}
