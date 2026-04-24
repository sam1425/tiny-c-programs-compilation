#ifndef GRID3D_H
#define GRID3D_H

#include <SDL2/SDL.h>

#define SCREEN_WIDTH  800
#define SCREEN_HEIGHT 600
#define GRID_SIZE     4      // Number of grid divisions per axis
#define CUBE_SIZE     200    // Size of 3D cube
#define FOV           500    // Perspective Field of View
#define SPEED         5.0f   // Movement speed
#define ROT_SPEED     0.05f  // Rotation speed

typedef struct {
    float x, y, z;
} Vec3D;

typedef struct {
    Vec3D position;
    float rotX, rotY;
} Camera;

void project(Vec3D point, int *x, int *y);
Vec3D rotate_y(Vec3D point, float angle);
Vec3D rotate_x(Vec3D point, float angle);
void move_camera(Camera *cam, float dx, float dz);
void draw_grid(SDL_Renderer *renderer, Camera cam);

#endif

