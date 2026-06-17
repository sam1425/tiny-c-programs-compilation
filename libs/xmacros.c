#include <stdio.h>

// Defining a macro 
// with the values of colors.
#define COLORS \
    X(RED)     \
    X(BLACK)   \
    X(WHITE)   \
    X(BLUE)

// Creating an enum of colors
// by macro expansion.
enum colors {
    #define X(value) value,
        COLORS
    #undef X
};

// A utility that takes the enum value
// and returns corresponding string value
char* toString(enum colors value)
{
    switch (value) {
        #define X(color) \
            case color:  \
                return #color;
                COLORS
        #undef X
    }
}

// driver program.
int main(void)
{
    enum colors color = WHITE;
    printf("%s", toString(color));
    return 0;
}
