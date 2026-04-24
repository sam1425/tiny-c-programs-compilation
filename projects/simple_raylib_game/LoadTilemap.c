#include <stdio.h>
#include <raylib.h>
#include <stdint.h>
#include <stdlib.h>

//Setup
const char *title = "simple raylib game";
#define WINDOW_WIDTH    1280
#define WINDOW_HEIGHT   720
#define MAX_ENTITIES    200
#define TILE_SIZE       16

#define HAS_POSITION (1 << 0)
#define HAS_SPRITE   (1 << 1)
#define HAS_TILEMAP  (1 << 2)

typedef struct {
  uint16_t atlas_index;
  bool Colidable;
} Tile;
typedef struct {
  int width;
  int height;
  Tile* tiles;
  Texture2D atlas;
} Tilemap;

typedef struct {Vector2 value;}     Position;
typedef struct {Texture2D texture;} Sprite;

typedef uint32_t Entity;
static Entity entity_count = 0;

Entity CreateEntity(void)
{
    if (entity_count >= MAX_ENTITIES)
    {
        TraceLog(LOG_ERROR, "Max entities reached");
        return 0;
    }
    return entity_count++;
}

static Position positions[MAX_ENTITIES];
static bool has_position[MAX_ENTITIES];
static Sprite sprites[MAX_ENTITIES];
static bool has_sprite[MAX_ENTITIES];
static Tilemap tilemaps[MAX_ENTITIES];
static bool has_tilemap[MAX_ENTITIES];

void AddPosition(Entity e, float x, float y) {
    positions[e].value = (Vector2){x, y};
    has_position[e] = true;
}

void AddTilemap(Entity e, int width, int height, Texture2D atlas) {
    tilemaps[e].width = width;
    tilemaps[e].height = height;
    tilemaps[e].atlas = atlas;
    tilemaps[e].tiles = (Tile*)malloc(width * height * sizeof(Tile));

    for(int i = 0; i < width * height; i++) {
        tilemaps[e].tiles[i] = (Tile){ .atlas_index = 0, .Colidable = false };
    }
    has_tilemap[e] = true;
}

void DrawTilesSystem(void) {
    for (Entity e = 0; e < entity_count; e++) {
        if (has_tilemap[e] && has_position[e]) {
            Tilemap *map = &tilemaps[e];
            Vector2 mapPos = positions[e].value;

            // Calculate how many tiles fit in one row of your texture
            int tilesPerRow = map->atlas.width / TILE_SIZE;

            for (int y = 0; y < map->height; y++) {
                for (int x = 0; x < map->width; x++) {
                    Tile tile = map->tiles[y * map->width + x];

                    // Correct 2D math for an atlas
                    float srcX = (float)(tile.atlas_index % tilesPerRow) * TILE_SIZE;
                    float srcY = (float)(tile.atlas_index / tilesPerRow) * TILE_SIZE;

                    Rectangle src = { srcX, srcY, TILE_SIZE, TILE_SIZE };
                    Rectangle dst = {
                        mapPos.x + (x * TILE_SIZE),
                        mapPos.y + (y * TILE_SIZE),
                        TILE_SIZE, TILE_SIZE
                    };

                    DrawTexturePro(map->atlas, src, dst, (Vector2){0,0}, 0, WHITE);
                }
            }
        }
    }
}
int main(void)
{
  InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, title);

  SetTargetFPS(60);
    Texture2D atlas = LoadTexture("tileset.png");
    Entity worldMap = CreateEntity();
    AddPosition(worldMap, 0, 0);
    AddTilemap(worldMap, TILE_SIZE, TILE_SIZE, atlas);
    tilemaps[worldMap].tiles[5 + 5 * TILE_SIZE].atlas_index = 0;

  while (!WindowShouldClose())
  {
    BeginDrawing();
    ClearBackground(RAYWHITE);
        DrawTilesSystem();
    EndDrawing();
  }
  for(uint16_t i = 0; i < entity_count; i++) {
      if(has_tilemap[i]) free(tilemaps[i].tiles);
  }
  UnloadTexture(atlas);
  CloseWindow();
  return 0;
}
