#include "grid3.h"
#include <math.h>
#include <stdbool.h>

#define MAX_ROT_X 1.5f  // around 90 degrees
#define MIN_ROT_X -1.5f // around -90 degrees
#define TWO_PI 6.28318530718f

void normalize_angle(float *angle) {
    while (*angle < 0) *angle += TWO_PI;
    while (*angle >= TWO_PI) *angle -= TWO_PI;
}

void clamp_rotation(Camera *cam) {
    if (cam->rotX > MAX_ROT_X) cam->rotX = MAX_ROT_X;
    if (cam->rotX < MIN_ROT_X) cam->rotX = MIN_ROT_X;
}

// Perspective projection function
void project(Vec3D point, int *x, int *y) {
    if (point.z + FOV != 0) {
        *x = (int)((point.x * FOV) / (point.z + FOV)) + SCREEN_WIDTH / 2;
        *y = (int)((point.y * FOV) / (point.z + FOV)) + SCREEN_HEIGHT / 2;
    }
}

// Rotate a point around the Y-axis
Vec3D rotate_y(Vec3D point, float angle) {
    float s = sin(angle), c = cos(angle);
    return (Vec3D){
        point.x * c - point.z * s,
        point.y,
        point.x * s + point.z * c
    };
}

// Rotate a point around the X-axis
Vec3D rotate_x(Vec3D point, float angle) {
    float s = sin(angle), c = cos(angle);
    return (Vec3D){
        point.x,
        point.y * c - point.z * s,
        point.y * s + point.z * c
    };
}

void move_camera(Camera *cam, float dx, float dz) {
    float s = sin(cam->rotY), c = cos(cam->rotY);

    // Forward/Backward movement based on camera's orientation
    cam->position.x += dx * c - dz * s;  // Moving along X axis (camera direction)
    cam->position.z += dz * c + dx * s;  // Moving along Z axis (strafe direction)
}


// Draw the 3D grid
void draw_grid(SDL_Renderer *renderer, Camera cam) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White grid

    for (int x = -GRID_SIZE; x <= GRID_SIZE; x++) {
        for (int y = -GRID_SIZE; y <= GRID_SIZE; y++) {
            for (int z = -GRID_SIZE; z <= GRID_SIZE; z++) {
                Vec3D point = { x * CUBE_SIZE / GRID_SIZE, y * CUBE_SIZE / GRID_SIZE, z * CUBE_SIZE / GRID_SIZE };

                // Apply camera translation
                point.x -= cam.position.x;
                point.y -= cam.position.y;
                point.z -= cam.position.z;

                // Apply camera rotation
                point = rotate_y(point, -cam.rotY);
                point = rotate_x(point, -cam.rotX);

                // Project to 2D and draw
                int px, py;
                project(point, &px, &py);
                SDL_RenderDrawPoint(renderer, px, py);
            }
        }
    }
}

int main() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        SDL_Log("SDL Initialization Failed: %s", SDL_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("3D Grid Simulation",
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

    // Initialize camera
    Camera cam = { { 0, 0, -500 }, 0, 0 }; // Camera starts at (0,0,-500)

    bool running = true;
    SDL_Event event;
    bool keys[SDL_NUM_SCANCODES] = {false}; // Key states

    while (running) {
        // Event handling
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            //if (event.key.keysym.sym) {
            //   SDLK_ESCAPE:
            //   running = false;
            } else if (event.type == SDL_KEYDOWN) {
                keys[event.key.keysym.scancode] = true;
            } else if (event.type == SDL_KEYUP) {
                keys[event.key.keysym.scancode] = false;
            }
        }

        // Handle movement input
        if (keys[SDL_SCANCODE_W]) move_camera(&cam, 0, SPEED);  // Forward
        if (keys[SDL_SCANCODE_S]) move_camera(&cam, 0, -SPEED); // Backward
        if (keys[SDL_SCANCODE_A]) move_camera(&cam, -SPEED, 0); // Left
        if (keys[SDL_SCANCODE_D]) move_camera(&cam, SPEED, 0);  // Right

        // Handle rotation input
        if (keys[SDL_SCANCODE_LEFT]) cam.rotY -= ROT_SPEED;  // Look Left
        if (keys[SDL_SCANCODE_RIGHT]) cam.rotY += ROT_SPEED; // Look Right
        if (keys[SDL_SCANCODE_UP]) cam.rotX -= ROT_SPEED;    // Look Up
        if (keys[SDL_SCANCODE_DOWN]) cam.rotX += ROT_SPEED;  // Look Down
        clamp_rotation(&cam);
		normalize_angle(&cam.rotX);
		normalize_angle(&cam.rotY);
        // Clear screen
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black background
        SDL_RenderClear(renderer);

        // Draw the 3D grid
        draw_grid(renderer, cam);

        // Present frame
        SDL_RenderPresent(renderer);
        SDL_Delay(16); // ~60 FPS
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

