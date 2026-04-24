#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

int main(void)
{
	bool loose = 0;
	while (loose == 0)
	{
			printf("shot the gun? ");
			int user = getchar();
			int r = rand() % 6;
			int r_u = rand() % 6;
			if (r == r_u)
			{
				printf("it hit you, you have lost\n");
				loose = true;
				return 0;
			}
	
			printf("the shot didn't hit you\n");
	}
	return 0;
}