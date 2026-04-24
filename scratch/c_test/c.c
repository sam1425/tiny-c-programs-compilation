// decrypt: a hello program written on 202404173.013830 by dwrr.
// making an interesting hello world program that prints matrix-like
// text and a message to the screen using terminal escape sequences.
#include <unistd.h> // Agregar la inclusión de la biblioteca para usleep
#include <stdio.h>
#include <iso646.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <sys/ioctl.h>

int main(int argc, const char** argv) {
	static const int display_speed = 4;
	static const int initial_delay = 5;

    srand((unsigned) time(0));
    struct winsize window = {0};
    ioctl(0, TIOCGWINSZ, &window); // Corregir la función ioctl

    const char* string = argc == 2 ? argv[1] : "Hello there from space.";
    const int string_length = (int) strlen(string);
    const size_t max_length = 100 * window.ws_col * window.ws_row;
    const int at_row = window.ws_row >> 1;
    const int at_column = (window.ws_col >> 1) - string_length;

    size_t length = 0;
    int timestep = 0;
    char* screen = calloc(max_length, 1);

    while (true) {
        memcpy(screen, "\033[H\033[2J", 7);
        length = 7;

        memcpy(screen + length, "\033[38;5;", 7);
        length += 7;
        const int color1 = 232 + rand() % 3;
        screen[length++] = '0' + (color1 / 100) % 10;
        screen[length++] = '0' + (color1 / 10) % 10;
        screen[length++] = '0' + color1 % 10;
        screen[length++] = 'm';

		for (int r = 0; r < window.ws_row; r++) {
			for (int c = 0; c < window.ws_col; c++) {

				const int random = (rand() % 95) * (rand() % 2) * (rand() % 2) * (rand() % 2);
				
				if (not (rand() % 100)) {	
					memcpy(screen + length, "\033[38;5;", 7);
					length += 7;
					const int color = 232 + rand() % 4;
					screen[length++] = '0' + (color / 100) % 10;
					screen[length++] = '0' + (color / 10) % 10;
					screen[length++] = '0' + color % 10;
					screen[length++] = 'm';
				}
				if (not (rand() % 80)) {
					length += (size_t) snprintf(screen + length, max_length - length, 
						"\033[38;5;%um%c", 
						250 - (rand() % 9), ' ' + (char) random
					);
					memcpy(screen + length, "\033[38;5;", 7);
					length += 7;
					const int color = 232 + rand() % 4;
					screen[length++] = '0' + (color / 100) % 10;
					screen[length++] = '0' + (color / 10) % 10;
					screen[length++] = '0' + color % 10;
					screen[length++] = 'm';
				} else screen[length++] = ' ' + (char) random;
			}
			if (r < window.ws_row - 1) screen[length++] = 10;
		}

		for (int string_index = 0; string_index < string_length; string_index++) {
			if (string[string_index] == 32) continue;

			if (timestep >= display_speed * (string_index + initial_delay + (rand() % 2))) {

				length += (size_t) snprintf(screen + length, max_length - length, 
						"\033[38;5;%um\033[%u;%uH%c\033[0m", 
						rand() % 80 == 0 ? 33 :
						rand() % 80 == 1 ? 63 :
						rand() % 80 == 2 ? 160 : 255,
						at_row, at_column + (string_index << 1), 
						rand() % 80 == 0 ? (' ' + rand() % 95) : 
						string[string_index]
				);
			} else if (string_index and timestep >= 
				display_speed * (string_index - (1 + (rand() % 2)) + initial_delay + (rand() % 2))
			) {
				length += (size_t) snprintf(screen + length, max_length - length, 
						"\033[38;5;%um\033[%u;%uH%c\033[0m", 
						rand() % 80 == 0 ? 33 :
						rand() % 80 == 1 ? 63 :
						rand() % 80 == 2 ? 160 : 255,
						at_row, at_column + (string_index << 1), 
						' ' + rand() % 95
				);
			}
		}
		write(1, screen, length);
		static const unsigned int frame_delay = 60000;
		timestep++;
	}
}

