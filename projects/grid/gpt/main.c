#include <SDL2/SDL.h>
#include <stdbool.h>

#define SCREEN_WIDTH  800
#define SCREEN_HEIGHT 600
#define GRID_SIZE     40  // Size of each grid cell

void draw_grid(SDL_Renderer *renderer, int cell_size) {
    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255); // Grid color (light gray)

    // Draw vertical lines
    for (int x = 0; x < SCREEN_WIDTH; x += cell_size) {
        SDL_RenderDrawLine(renderer, x, 0, x, SCREEN_HEIGHT);
    }

    // Draw horizontal lines
    for (int y = 0; y < SCREEN_HEIGHT; y += cell_size) {
        SDL_RenderDrawLine(renderer, 0, y, SCREEN_WIDTH, y);
    }
}

int main() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("Grid-Based Design",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

    if (!window) {
        SDL_Log("Failed to create window: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        SDL_Log("Failed to create renderer: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    bool running = true;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }

        SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255); // Background color (dark gray)
        SDL_RenderClear(renderer);

        draw_grid(renderer, GRID_SIZE);

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

