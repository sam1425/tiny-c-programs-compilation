#include <stdio.h>
int main() {
    size_t large a = 7306060462991417354;
    size_t b = 7499628;
    size_t large z = 678026374908020;
    for (size_t = 0; i < 14; i++, z>>=4)
        putchar((((((z&15)<8)*a)|(((z&15)>=8)*b))>>(8*((z&15)%8)))&255);
}