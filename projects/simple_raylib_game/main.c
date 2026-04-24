#include <stdio.h>
#include <raylib.h>

const int WINDOW_WIDTH= 1280;
const int WINDOW_HEIGHT= 720;

const char *title = "simple raylib game";

int main(void)
{
  InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, title);

  SetTargetFPS(60);
  while (!WindowShouldClose())
  {
    BeginDrawing();
    ClearBackground(RAYWHITE);
    EndDrawing();
  }
  CloseWindow();
  return 0;
}
