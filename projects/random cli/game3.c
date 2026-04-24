#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdio.h>

// Dimensiones de la ventana
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int CIRCLE_RADIUS = 20;

int SDL_main(int argc, char *argv[]) {
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    SDL_Event event;
    bool quit = false;
    int circle_x = 0;
    int circle_y = SCREEN_HEIGHT / 2;
    int circle_velocity = 5;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    window = SDL_CreateWindow("Simple SDL Game",
                              SDL_WINDOWPOS_UNDEFINED,
                              SDL_WINDOWPOS_UNDEFINED,
                              SCREEN_WIDTH,
                              SCREEN_HEIGHT,
                              SDL_WINDOW_SHOWN);

    if (window == NULL) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    while (!quit) {
        while (SDL_PollEvent(&event) != 0) {
            if (event.type == SDL_QUIT) {
                quit = true;
            }
        }

        SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
        SDL_RenderClear(renderer);

        circle_x += circle_velocity;

        if (circle_x - CIRCLE_RADIUS < 0 || circle_x + CIRCLE_RADIUS > SCREEN_WIDTH) {
            circle_velocity = -circle_velocity;
        }

        SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0xFF);
        SDL_RenderDrawLine(renderer, circle_x - CIRCLE_RADIUS, circle_y, circle_x + CIRCLE_RADIUS, circle_y);
        SDL_RenderDrawLine(renderer, circle_x, circle_y - CIRCLE_RADIUS, circle_x, circle_y + CIRCLE_RADIUS);

        SDL_RenderPresent(renderer);

        SDL_Delay(10);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
