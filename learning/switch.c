#include <stdio.h>

int main(int argc, char **argv)
{
    if(argc < 2) {
      printf("not enough arguments\n");
      return 1; 
    }

    printf("sizeof argv: %i\n", argc);
    for(int i = 1; i < argc; i++) {
        switch(**argv)
        {
          case 1:
            printf("1\n");
          default:
            printf("none\n");
        }
        size_t intt = sizeof(argv) / sizeof(argv[0]);
        printf("%i\n", i);
        printf("sizeof argv: %lu\n", intt);
    }
    return 0;
}
