#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
    char *button = getenv("BLOCK_BUTTON");

    if (button != NULL) {
        // Handle click event execution path
        if (strcmp(button, "1") == 0) {
            // Put your left-click specific action here
        }
    } else {
        // Handle normal timed update background path
        printf("No click event detected - background update\n");
    }

    return 0;
}
