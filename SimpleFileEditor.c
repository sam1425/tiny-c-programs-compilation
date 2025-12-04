#include "cs50.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int msvsc_check(int argc,char *argv[]);
int check_error();

int main(int argc, char *argv[])
{
	
	if (argc < 2)
	{
	printf("usage for file: \n"
				 "w = Write into file\n"
				 "c = Clear file \n");
		return 1;
	}
	int status = msvsc_check(argc, argv);
  if (status != 0)
     return status;
	return check_error();
}


int check_error()
{
	printf("file:\n");
 	int return_value = system("cat something.txt");
 	if (return_value == -1)
	{
		perror("Error executing command");
 		return -1;  
 	}
	return 0;
}


int msvsc_check(int argc,char *argv[]){
if (strcmp(argv[1] , "w")== 0)
	{

		FILE *file = fopen("something.txt", "wd");
		if (file == NULL)	
		{
			return 1;
		}
		char *text = get_string("type what u wanna print: ");
		fprintf(file, "%s\n", text);
		fclose(file);

	}
	else if (strcmp(argv[1], "c")== 0)
	{
		FILE *file = fopen("something.txt", "w");
		if (file == NULL)	
		{
			return 1;
		}
		fprintf(file, "\n");
		fclose(file);
		printf("file has been cleared\n");
		return 0;
	}
}
