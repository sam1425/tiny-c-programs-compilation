#include <stdio.h>
#include <string.h>
#include <math.h>  // Include this for sin() and cos()

int main() {
    float A = 0, B = 0, i, j;
    int k;
    float z[1760];
    char b[1760];

    printf("\x1b[2J");  // ANSI escape code to clear screen
    for (;;) {
        memset(b, 32, 1760);  // Fill with spaces
        memset(z, 0, 7040);   // Fill with zeroes

        for (j = 0; 6.28 > j; j += 0.07) {
            for (i = 0; 6.28 > i; i += 0.02) {
                float c = sin(i), d = cos(j), e = sin(A), f = sin(j), g = cos(A), h = d + 2;
                float D = 1 / (c * h * e + f * g + 5);
                float l = cos(i), m = cos(B), n = sin(B), t = c * h * g - f * e;
                int x = 40 + 30 * D * (l * h * m - t * n);
                int y = 12 + 15 * D * (l * h * n + t * m);
                int o = x + 80 * y;
                int N = 8 * ((f * e - c * d * g) * m - c * d * e - f * g - l * d * n);

                if (22 > y && y > 0 && x > 0 && 80 > x && D > z[o]) {
                    z[o] = D;
                    b[o] = ".,-~:;=!*#$@"[N > 0 ? N : 0];
                }
            }
        }

        printf("\x1b[H");  // ANSI escape code to reset cursor to home
        for (k = 0; 1761 > k; k++) {
            putchar(k % 80 ? b[k] : 10);  // Print character or newline
        }

        A += 0.04;
        B += 0.02;
    }
}

