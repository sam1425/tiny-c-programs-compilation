#include <stdio.h>
#include <stdlib.h>
#include <math.h>


double roundup(double x)
{
  return ceil(x*2.0) / 2.0;

}

int main(int argc, char *argv[])
{
    if(argc < 2)
    {
        printf("You need to put at least one argument\n");
        return 1;
    }
    for (int i; i < argc; i++)
    {
        double val = atof(argv[i]);
        double rounded = roundup(val);
        printf("Len: %i  Rounded: %.1f \n", i , rounded);
    }
    double nums[] = {3.1, 3.4, 3.6, 2.2, 4.9};
    return 0;

}
