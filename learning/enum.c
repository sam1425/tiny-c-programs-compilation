/* Raylib testing a state machine using an enum*/

//#include <stdio.h>
#include <time.h>
//#include <unistd.h>
#include <raylib.h>

enum GameState
{
  Menu,
  Game,
  Over
};

struct movable{
  int x;
  int y;
};

int main(void)
{
  GameState =  Menu;
  struct movable player;
  player.x = 100;
  player.y = 100;
  InitWindow(1280,720,"sam's");
  SetTargetFPS(60);
  while(!WindowShouldClose())
  {
    switch GameState
      case Menu:
        if(IsKeyDown(KEY_SPACE)) GameState = Game;
      case Game: 
        if(IsKeyDown(KEY_D)) player.x += 2.0f;
        if(IsKeyDown(KEY_A)) player.x -= 2.0f;
        if(IsKeyDown(KEY_S)) player.y += 2.0f;
        if(IsKeyDown(KEY_W)) player.y -= 2.0f;
        BeginDrawing();
          ClearBackground(BLACK);
          DrawText("hello",player.x,player.y,20,RAYWHITE);
        EndDrawing();
      case Over:
        CloseWindow();
  }
  return 0;
}
