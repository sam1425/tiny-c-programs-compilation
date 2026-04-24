// Desired usage: ./file_processor <input> <output> [-t <threads>] [-h]

// 1. Set up your arguments
#define REQUIRED_ARGS \
    REQUIRED_STRING_ARG(input_file, "input", "Input file path") \
    REQUIRED_STRING_ARG(output_file, "output", "Output file path")

#define OPTIONAL_ARGS \
    OPTIONAL_UINT_ARG(threads, 1, "-t", "threads", "Number of threads to use")

#define BOOLEAN_ARGS \
    BOOLEAN_ARG(help, "-h", "Show help")

// 2. Include easyargs
#include "../includes/easyargs.h"

int main(int argc, char* argv[]) {
    // 3. Start with default arguments
    args_t args = make_default_args();

    // 4. If parsing fails OR help argument is passed, print help
    if (!parse_args(argc, argv, &args) || args.help) {
        print_help(argv[0]);
        return 1;
    }

    // 5. Use arguments
    printf("Processing %s -> %s\n", args.input_file, args.output_file);
    printf("Threads: %u\n", args.threads);

    return 0;
}
