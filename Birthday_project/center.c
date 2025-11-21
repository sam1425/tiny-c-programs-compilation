//A simple program to center text on the screen
//Made by sam @2025


#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>

void center(char* string){
  struct winsize width;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &width);
  int textlength = strlen(string);
  int position = (width.ws_col / 2) - (textlength / 2);
  for (int i = 0; i < position; i++)
  {
    putchar(' ');
  }
  printf("%s\n", string);
}

