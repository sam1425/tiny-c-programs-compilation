#include <iostream>
#include <SDL.h>

int main( int argc, char *argv[])
{
	if( SDL_Init( SDL_INIT_EVERYTHING ) <0 )
	{
		std::cout << "SDL could not initialise error:" << SDL_GetError( ) << std::endl;
	}

	system ( "pause" );

	return EXIT_SUCCESS;
}