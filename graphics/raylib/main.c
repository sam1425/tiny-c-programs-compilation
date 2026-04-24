//#include <stdio.h>
//a window
#include <raylib.h>
#include <stdio.h>
#include <math.h>
//linux standard lib
//#include <unistd.h>
//ctypes
//#include <stdint.h>
//typedef uint16_t u16;
#define WINDOW_WIDTH   1280
#define WINDOW_HEIGHT  720
#define FPS            60
const char * title = "simple raylib example";

const int speed = 200;

Vector2 WhichKeyIsDown();
void Normalize(Vector2);
int main(void)
{
  //int dt = FPS/FPS;
  InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, title);
  Vector2 Player={(float)WINDOW_WIDTH/2 , (float)WINDOW_HEIGHT/2};
  Vector2 direction;

  float velx = 0;
  float vely = 0;

  SetTargetFPS(FPS);
  while(!WindowShouldClose())
  {
    direction = WhichKeyIsDown();
    Normalize(direction);
    velx = speed * direction.x ;
    vely = speed * direction.y ;

    Player.x += velx * GetFrameTime();
    Player.y += vely * GetFrameTime();

    BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("first window",190,200,20,LIGHTGRAY);
        DrawRectangle(Player.x,Player.y, 20 , 20 , MAROON);
    EndDrawing();
  }
  CloseWindow();
  return 0;
}

Vector2 WhichKeyIsDown()
{
    Vector2 moveto = {0, 0};
    if(IsKeyDown(KEY_D)) moveto.x += 1;
    if(IsKeyDown(KEY_A)) moveto.x -= 1;
    if(IsKeyDown(KEY_S)) moveto.y += 1;
    if(IsKeyDown(KEY_W)) moveto.y -= 1;
  return moveto;
}


void Normalize(Vector2 direction)
{
    float length = sqrt(direction.x * direction.x + direction.y * direction.y);
    if(length > 0)
    {
      direction.x /= length;
      direction.y /= length;
    }
}
