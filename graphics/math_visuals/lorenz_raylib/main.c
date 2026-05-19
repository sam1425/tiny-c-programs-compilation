#include "raylib.h"
#include <stdio.h>

#define MAX_POINTS 10000

typedef struct {
    float x, y, z;
} Point;

int main(void) {
    const int screenWidth = 800;
    const int screenHeight = 600;

    InitWindow(screenWidth, screenHeight, "Lorenz Attractor - Raylib");

    Camera3D camera = { 0 };
    camera.position = (Vector3){ 30.0f, 30.0f, 30.0f };
    camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    float x = 0.01f, y = 0.0f, z = 0.0f;
    float a = 10.0f, b = 28.0f, c = 8.0f/3.0f;
    float dt = 0.01f;

    Point points[MAX_POINTS];
    int count = 0;

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        // Math update
        if (count < MAX_POINTS) {
            float dx = (a * (y - x)) * dt;
            float dy = (x * (b - z) - y) * dt;
            float dz = (x * y - c * z) * dt;
            x += dx; y += dy; z += dz;
            points[count].x = x;
            points[count].y = y;
            points[count].z = z;
            count++;
        }

        UpdateCamera(&camera, CAMERA_ORBITAL);

        BeginDrawing();
            ClearBackground(BLACK);
            BeginMode3D(camera);
                for (int i = 1; i < count; i++) {
                    Vector3 p1 = { points[i-1].x, points[i-1].y, points[i-1].z };
                    Vector3 p2 = { points[i].x, points[i].y, points[i].z };
                    
                    // Color based on Z position or index
                    Color col = ColorFromHSV((float)i/MAX_POINTS * 360, 1.0f, 1.0f);
                    DrawLine3D(p1, p2, col);
                }
            EndMode3D();
            
            DrawFPS(10, 10);
            DrawText("Drag to rotate camera", 10, 30, 20, RAYWHITE);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
