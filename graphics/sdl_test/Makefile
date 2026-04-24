OBJS = sam.cpp

PAR = sdlmain

all : $(OBJS)
	g++ $(OBJS) -IC:\Development\SDL_mingw32\include\SDL2 -LC:\Development\SDL_mingw32\lib -w -Wl,-subsystem,windows -lmingw32 -lSDL2main -lSDL2 -o $(PAR)