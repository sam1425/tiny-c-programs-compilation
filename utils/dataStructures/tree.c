#include <stdio.h>


// A tree is just an array with some rules
// A parent can have multiple children
// But a child can only have one parent

enum child{
    LEFT,
    RIGHT,
};

typedef struct parent {
    int* childs[];
}Something;

int main(){

    printf("hello, world!\n");
    return 0;
    }
