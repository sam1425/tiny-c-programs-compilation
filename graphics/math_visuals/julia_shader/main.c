#include "raylib.h"
#include <stdio.h>

int main(void)
{
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "Math Visuals - Julia Set Fractal");

    // Load shader
    Shader shader = LoadShader(0, "julia.fs");

    // Shader locations
    int screenCenterLoc = GetShaderLocation(shader, "screenCenter");
    int mousePosLoc = GetShaderLocation(shader, "mousePos");
    int zoomLoc = GetShaderLocation(shader, "zoom");

    float screenCenter[2] = { (float)screenWidth/2, (float)screenHeight/2 };
    float zoom = 1.0f;

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        // Zoom control
        zoom += GetMouseWheelMove() * 0.05f * zoom;
        if (zoom < 0.1f) zoom = 0.1f;

        Vector2 mouse = GetMousePosition();
        float mousePos[2] = { mouse.x, mouse.y };

        // Update shader uniforms
        SetShaderValue(shader, screenCenterLoc, screenCenter, SHADER_UNIFORM_VEC2);
        SetShaderValue(shader, mousePosLoc, mousePos, SHADER_UNIFORM_VEC2);
        SetShaderValue(shader, zoomLoc, &zoom, SHADER_UNIFORM_FLOAT);

        BeginDrawing();
            ClearBackground(BLACK);

            BeginShaderMode(shader);
                // Draw a rectangle covering the whole screen to run the shader on every pixel
                DrawRectangle(0, 0, screenWidth, screenHeight, WHITE);
            EndShaderMode();

            DrawFPS(10, 10);
            DrawText("Move mouse to change Fractal constants", 10, 30, 20, RAYWHITE);
            DrawText("Mouse wheel to zoom", 10, 55, 20, RAYWHITE);
        EndDrawing();
    }

    UnloadShader(shader);
    CloseWindow();

    return 0;
}
