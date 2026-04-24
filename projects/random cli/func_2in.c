#include "cs50_stuff/cs50.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

void n_random(int n, int n_max);


int main()
{
	int nt = get_int("how much times? ");
	int max = get_int("max number: ");
	n_random(nt, max);
}


void n_random(int n, int n_max)
{
	for (int i = 0; i <= n; i++)
	{
		int r = rand() % n_max;
		printf("%i\n", r);
	}
}