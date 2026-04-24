#include <ncurses.h>
#include <unistd.h> // For usleep()

#define GRID_WIDTH 20
#define GRID_HEIGHT 10

// Function to initialize the grid with a default character
void initializeGrid(char grid[GRID_HEIGHT][GRID_WIDTH], char defaultChar) {
    for (int y = 0; y < GRID_HEIGHT; y++) {
        for (int x = 0; x < GRID_WIDTH; x++) {
            grid[y][x] = defaultChar;
        }
    }
}

// Function to render the grid to the console
void renderGrid(char grid[GRID_HEIGHT][GRID_WIDTH]) {
    for (int y = 0; y < GRID_HEIGHT; y++) {
        for (int x = 0; x < GRID_WIDTH; x++) {
            printf("%c ", grid[y][x]); // Use printw for ncurses output
        }
        printf("\n");
    }
}

int main() {
    char grid[GRID_HEIGHT][GRID_WIDTH];
    int playerX = GRID_WIDTH / 2; // Player starting X position
    int playerY = GRID_HEIGHT / 2; // Player starting Y position
    int input;

    // Initialize ncurses
    initscr();              // Start ncurses mode
    cbreak();               // Disable line buffering
    noecho();               // Don't echo input
    keypad(stdscr, TRUE);   // Enable special keys (e.g., arrow keys)
    nodelay(stdscr, TRUE);  // Make getch() non-blocking
    curs_set(0);            // Hide the cursor

    // Initialize the grid with empty spaces
    initializeGrid(grid, '.');

    // Main game loop
    while (1) {
        clear(); // Clear the screen

        // Update the grid with the player's position
        initializeGrid(grid, '.'); // Reset the grid
        grid[playerY][playerX] = '@'; // Place the player

        // Render the grid
        renderGrid(grid);

        // Refresh the screen
        refresh();

        // Get user input
        input = getch();

        // Move the player based on input
        switch (input) {
            case 'w': // Move up
                if (playerY > 0) playerY--;
                break;
            case 's': // Move down
                if (playerY < GRID_HEIGHT - 1) playerY++;
                break;
            case 'a': // Move left
                if (playerX > 0) playerX--;
                break;
            case 'd': // Move right
                if (playerX < GRID_WIDTH - 1) playerX++;
                break;
            case 'q': // Quit the game
                endwin(); // End ncurses mode
                printf("Exiting the game...\n");
                return 0;
            default:
                break;
        }

        // Add a small delay to control the speed of the game
        usleep(100000); // 100ms delay
    }

    // End ncurses mode (not reached in this loop)
    endwin();
    return 0;
}
