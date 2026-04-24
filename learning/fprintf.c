#include <stdio.h>
#include <stdlib.h>


void printfile(FILE *stream){
    fprintf(stream,"Hello from stdout;");
}

int main(){
    FILE *stream = stdout;
    printfile(stream);
    return 0;
}
