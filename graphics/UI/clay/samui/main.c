
#define CLAY_IMPLEMENTATION
#include "clay.h"
#include "clay_renderer_raylilb.c"

#define WINDOW_WIDTH  1024
#define WINDOW_HEIGHT 768

const int FONT_ID_BODY_16 = 0;
const Clay_Color COLOR_BLACK = { 0, 0, 0, 0};
const Clay_Color COLOR_WHITE = { 255, 255, 255, 255};
const Clay_Color COLOR_LIGHT = (Clay_Color) {224, 215, 210, 255};
const Clay_Color COLOR_RED = (Clay_Color) {168, 66, 28, 255};
const Clay_Color COLOR_ORANGE = (Clay_Color) {225, 138, 50, 255};


void HandleClayErrors(Clay_ErrorData errorData) {
    printf("%s", errorData.errorText.chars);
}

Clay_ElementDeclaration sidebarItemConfig = (Clay_ElementDeclaration) {
    .layout = {
        .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIXED(50) }
    },
    .backgroundColor = COLOR_ORANGE
};


int main(int argc, char *argv[])
{
    int screenwidth  = GetScreenWidth();
    int screenheight = GetScreenHeight();

    Clay_Raylib_Initialize( screenwidth , screenheight, "IDK", FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_HIGHDPI | FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT);

    Font fonts[1];
    fonts[FONT_ID_BODY_16] = LoadFontEx("/home/c0mplex/.local/share/fonts/ComicMono.ttf", 48, 0, 400);
    SetTextureFilter(fonts[FONT_ID_BODY_16].texture, TEXTURE_FILTER_BILINEAR);

    uint64_t clayRequiredMemory = Clay_MinMemorySize();
    Clay_Arena arena = Clay_CreateArenaWithCapacityAndMemory(clayRequiredMemory, malloc(clayRequiredMemory));
    Clay_Initialize(arena, (Clay_Dimensions) { WINDOW_WIDTH, WINDOW_HEIGHT}, (Clay_ErrorHandler) { HandleClayErrors });

    Clay_SetMeasureTextFunction(Raylib_MeasureText, fonts);

    while (!WindowShouldClose()){
        Clay_SetLayoutDimensions((Clay_Dimensions){
                .width = GetScreenWidth(),
                .height = GetScreenHeight()
            });

        Clay_BeginLayout();

        CLAY(
            CLAY_ID("OuterContainer"),{
                .layout = {
                    .sizing = {
                     CLAY_SIZING_GROW(0), CLAY_SIZING_GROW(0)},
                    .padding = CLAY_PADDING_ALL(16), .childGap = 16 },
                    .backgroundColor = {250,250,255,255} }) {
            CLAY(CLAY_ID("SideBar"), {
                    .layout = {
                        .layoutDirection = CLAY_TOP_TO_BOTTOM, .sizing = {
                            .width = CLAY_SIZING_FIXED(300), .height = CLAY_SIZING_GROW(0) },
                            .padding = CLAY_PADDING_ALL(16), .childGap = 16 },
                            .backgroundColor = COLOR_LIGHT
                }) {
                //CLAY(CLAY_ID("ProfilePictureOuter"), { .layout = { .sizing = { .width = CLAY_SIZING_GROW(0) }, .padding = CLAY_PADDING_ALL(16), .childGap = 16, .childAlignment = { .y = CLAY_ALIGN_Y_CENTER } }, .backgroundColor = COLOR_RED }) {
                //CLAY(CLAY_ID("ProfilePicture"), {.layout = { .sizing = { .width = CLAY_SIZING_FIXED(60), .height = CLAY_SIZING_FIXED(60) }}, .image = { .imageData = &profilePicture } }) {}
                //CLAY_TEXT(CLAY_STRING("Clay - UI Library"), CLAY_TEXT_CONFIG({ .fontSize = 24, .textColor = {255, 255, 255, 255} }));
                //}


            CLAY(CLAY_ID("MainContent"), { .layout = { .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0) } }, .backgroundColor = COLOR_LIGHT }) {}
            }
        }

        Clay_RenderCommandArray renderCommands = Clay_EndLayout();


        BeginDrawing();
        ClearBackground(BLACK);
        //Clay_Raylib_Render(renderCommands);
        Clay_Raylib_Render(renderCommands, fonts);
        EndDrawing();
    }

    return 0;
}
