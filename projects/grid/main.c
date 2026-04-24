#include <stdio.h>
#include <stdlib.h>

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
            printf("%c ", grid[y][x]);
        }
        printf("\n");
    }
}

