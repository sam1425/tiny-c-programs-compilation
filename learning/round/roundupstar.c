#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>


double roundup(double x)
{
  return ceil(x*2.0) / 2.0;

}

double smart_round(double x) {
    double int_part = floor(x);
    double frac = x - int_part;

    if (frac < 0.25)
        return int_part;
    else if (frac < 0.75)
        return int_part + 0.5;
    else
        return int_part + 1.0;
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
        double rounded = smart_round(val);
        printf("Len: %i  Rounded: %.1f \n", i , rounded);
    }
    double nums[] = {3.1, 3.4, 3.6, 2.2, 4.9};
    return 0;

}
