#include <stdio.h>
#include <unistd.h>

int main() {
	const char *cursors[] = {
		"\033[2 q", // block
		"\033[4 q", // underline
		"\033[6 q"  // vertical bar
	};

	int i = 0;
	while (1) {
		// Clear the screen
		printf("\033[2J\033[H");

		printf("Testing cursor: %d\n", i+1);
		printf("Test loop. Ctrl+C to exit.\n");

		printf("%s", cursors[i]);

		fflush(stdout);

		usleep(500000);

		i = (i + 1) % 3;
	}

	return 0;
}

