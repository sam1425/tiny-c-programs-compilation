#include <SDL2/SDL.h>
#include <math.h>

#define WIDTH 640
#define HEIGHT 480
#define BALL_COUNT 10

typedef struct {
    float x, y, vx, vy, radius;
} Metaball;

void update_ball(Metaball *b) {
    b->x += b->vx;
    b->y += b->vy;
    if (b->x < 0 || b->x > WIDTH)  b->vx *= -1;
    if (b->y < 0 || b->y > HEIGHT) b->vy *= -1;
}

int main() {
    //sdl init
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *win = SDL_CreateWindow("C Metaballs", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, 0);
    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
    SDL_Texture *tex = SDL_CreateTexture(ren, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, WIDTH, HEIGHT);
    //end sdl

    Metaball balls[BALL_COUNT];
    for(int i=0; i<BALL_COUNT; i++) {
        balls[i] = (Metaball){rand()%WIDTH, rand()%HEIGHT, (rand()%40-20)/10.0f, (rand()%40-20)/10.0f, 60.0f};
    }

    uint32_t pixels[WIDTH * HEIGHT];
    int running = 1;
    SDL_Event e;

    while (running) {
        while (SDL_PollEvent(&e)) if (e.type == SDL_QUIT) running = 0;

        for (int i=0; i<BALL_COUNT; i++) update_ball(&balls[i]);

        for (int y = 0; y < HEIGHT; y++) {
            for (int x = 0; x < WIDTH; x++) {
                float sum = 0;
                for (int i = 0; i < BALL_COUNT; i++) {
                    float dx = x - balls[i].x;
                    float dy = y - balls[i].y;
                    sum += (balls[i].radius * balls[i].radius) / (dx*dx + dy*dy + 0.0001f);
                }
                pixels[y * WIDTH + x] = (sum > 1.0f) ? 0xFF00FFFF : 0xFF000000;
            }
        }

        SDL_UpdateTexture(tex, NULL, pixels, WIDTH * sizeof(uint32_t));
        SDL_RenderCopy(ren, tex, NULL, NULL);
        SDL_RenderPresent(ren);
    }

    SDL_DestroyTexture(tex);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}
