// Desired usage: ./config [-c <config>] [-v] [--disable-cache] [-h]

// No required arguments for this example
#define OPTIONAL_ARGS \
    OPTIONAL_STRING_ARG(config_file, "config.ini", "-c", "config", "Configuration file path")

#define BOOLEAN_ARGS \
    BOOLEAN_ARG(verbose, "-v", "Verbose output") \
    BOOLEAN_ARG(no_cache, "--disable-cache", "Disable the use of cache") \
    BOOLEAN_ARG(help, "-h", "Show help")

#include "../includes/easyargs.h"

int main(int argc, char* argv[]) {
    args_t args = make_default_args();

    parse_args(argc, argv, &args);

    if (args.help) {
        print_help(argv[0]);
        return 1;
    }

    printf("Configuration file: %s\n", args.config_file);
    printf("Verbose: %s\n", args.verbose ? "On" : "Off");
    printf("Using cache: %s\n", args.no_cache ? "No" : "Yes");

    return 0;
}
