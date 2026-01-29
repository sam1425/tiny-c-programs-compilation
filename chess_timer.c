#include <time.h>
#include <stdio.h>
#include <termios.h>//for user input
#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>
#include <stdbool.h>
#include <stdlib.h>

void set_nonblocking(bool enable) {
    struct termios ttystate;
    tcgetattr(STDIN_FILENO, &ttystate);

    if (enable) {
        ttystate.c_lflag &= ~ICANON; // turn off buffered i/o
        ttystate.c_lflag &= ~ECHO;   // turn off echo
        ttystate.c_cc[VMIN] = 0;     // return immediately
        ttystate.c_cc[VTIME] = 0;
    } else {
        ttystate.c_lflag |= ICANON;
        ttystate.c_lflag |= ECHO;
    }

    tcsetattr(STDIN_FILENO, TCSANOW, &ttystate);
}

int kbhit() {
    struct timeval tv = {0L, 0L};
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(0, &fds);
    return select(1, &fds, NULL, NULL, &tv);
}

int main(int argc, char* argv[])
{
  if(argc < 2)
  {
    printf("Usage: ./command <player timers> (in minutes)");
    return 1;
  }

  printf("press s to start and space to change between players\n\n");
  printf("you can use Crl + c or q to quit\n");
  set_nonblocking(true);
  time_t time_before, time_after;

  time_before = time(NULL);

  long int player_1_timer = atoi(argv[1]) * 60;
  long int player_2_timer = atoi(argv[1]) * 60;

  bool Active = false;
  bool playerswitch = true;

  for(;;)
  {
    if (kbhit()) {
      char c = getchar();

      if (c == 'q') 
      {
        Active = false;
        break;
      }
      if (c == 's') 
      {
        Active = true;
        printf("game started\n\n");
      }
      if (c == ' ')
      {
        playerswitch = !playerswitch;
      }
    }
    if (Active)
    {

      time_after = time(NULL);
      if (time_after - time_before >= 1){
        time_before = time_after;

        if(playerswitch) player_1_timer -= 1;
        else player_2_timer -= 1;

        printf("P1: %ld | P2: %ld\n", player_1_timer, player_2_timer);
        if (player_1_timer <= 0|| player_2_timer <= 0 ) 
        {
            printf("countdown ended\n");
            return 0;
        }

      }
    }
    usleep(50000); 
  }
  set_nonblocking(false); 
  return 0;
}
