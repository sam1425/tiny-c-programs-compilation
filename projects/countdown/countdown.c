
// A c program that counts down until a condition is met
// usage: ./countdown <input_number> [-h]

#include <stdio.h>
#include <stdint.h>
#include <time.h>

#define REQUIRED_ARGS \
    REQUIRED_INT_ARG(n, "N", "The number to process")

#define BOOLEAN_ARGS \
    BOOLEAN_ARG(help, "-h", "Show help")

#include "easyargs.h"

int main(int argc, char* argv[]){

    args_t args = make_default_args();
    if (!parse_args(argc, argv, &args) || args.help) {
        print_help(argv[0]);
        return 1;
    }

    time_t start, now;
    start = time(NULL);
    uint32_t countdown = args.n;
    while (1){
        now = time(NULL);

        if (now - start >= 1){
            start = now;
            countdown -= 1;
            printf("%i\n", countdown );
            if (countdown <= 0)
            {
                printf("countdown ended\n");
                return 0;
            }
        }
    }
}
