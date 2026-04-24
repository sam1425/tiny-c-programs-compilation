#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void hexToRgba(const char *hex, int *r, int *g, int *b, int *a) {
    if (hex[0] == '#') hex++;
    int length = strlen(hex);

    if (length == 6) {
        sscanf(hex, "%2x%2x%2x", r, g, b);
        *a = 255;
    } else if (length == 8) {
        sscanf(hex, "%2x%2x%2x%2x", r, g, b, a);
    } else {
        printf("Invalid hex format!\n");
        *r = *g = *b = *a = 0;
    }
}

int main() {
    char hexColor[10];
    int r, g, b, a;

    printf("Hex : ");
    scanf("%9s", hexColor);

    hexToRgba(hexColor, &r, &g, &b, &a);

    printf("rgba(%d, %d, %d, %d)\n", r, g, b, a);

    return 0;
}

//https://raw.githubusercontent.com/iamdevnitesh/gruvbox-discord/main/gruvbox-discord.css
https://raw.githubusercontent.com/NgNority/DiscordRecolor-Gruvbox/main/Gruvbox-DiscordRecolor.css
//https://raw.githubusercontent.com/nudejoebiden/discord-gruvbox-material-theme/main/comfygruvboxmaterial.theme.css
//https://raw.githubusercontent.com/acicco/Gruvbox-DiscordTheme/main/gruvbox.theme.css
//https://raw.githubusercontent.com/patrykf03/Discord-Gruvbox/main/duvbox.theme.css