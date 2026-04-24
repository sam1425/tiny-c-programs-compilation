// Usage: ./minimal source.txt destination.txt

#define REQUIRED_ARGS \
    REQUIRED_STRING_ARG(source, "source", "Source file") \
    REQUIRED_STRING_ARG(destination, "dest", "Destination file")

#include "../includes/easyargs.h"

int main(int argc, char* argv[]) {
    args_t args = make_default_args();

    if (!parse_args(argc, argv, &args)) {
        print_help(argv[0]);
        return 1;
    }

    printf("Copying %s to %s\n", args.source, args.destination);

    return 0;
}
