#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/ioctl.h>

void clear_screen()
{
	system("clear");
}

int main(int argc, char *argv[])

{
	clear_screen();
	printf("wanna play? y: ");
	char user_in;
	scanf("%c", &user_in);
	user_in = tolower(user_in);
	if (user_in == 'y')

	{

		printf("the arguments in this run is: %d\n", argc);

		for (int i = 0; i < argc; i++)
		{
			printf("%d: %s\n",i ,argv[i]);
		}
	}
	else if(user_in =='n')
	{
		printf("exiting..")
		return 0;
	}

	return 0;
}
