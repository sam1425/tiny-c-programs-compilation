# EasyArgs

A simple, single-header argument parser for C that eliminates boilerplate and code duplication.

![Cover photo. Easy Args: A simple, single-header argument parser for C](./coverphoto.jpg)

## Features

- **Single header file:** Just include `easyargs.h` and you're good to go
- **Zero duplication:** Define your arguments once to get struct fields, parsing, and help generation automatically
- **Type-safe:** Supports all common C types with built-in parsing
- **Flexible:** Handle required arguments, optional arguments with defaults, and boolean flags
- **Automatic help generation:** Professional-looking usage and help text generated from your definitions

## Quick Start

Check out ./examples to see some use cases!

### 1. Define your arguments

```c
// Define required positional arguments
#define REQUIRED_ARGS \
    REQUIRED_STRING_ARG(input_file, "input", "Input file path") \
    REQUIRED_STRING_ARG(output_file, "output", "Output file path")

// Define optional arguments with defaults
#define OPTIONAL_ARGS \
    OPTIONAL_UINT_ARG(threads, 1, "-t", "threads", "Number of threads to use")

// Define boolean flags
#define BOOLEAN_ARGS \
    BOOLEAN_ARG(help, "-h", "Show help")
```

### 2. Include EasyArgs

```c
#include "easyargs.h"
```

### 3. Use it in your main function

```c
int main(int argc, char* argv[]) {
    // Initialize with the default values specified above
    args_t args = make_default_args();

    // Parse arguments
    if (!parse_args(argc, argv, &args) || args.help) {
        print_help(argv[0]);
        return 1;
    }

    // Use your arguments
    printf("Processing %s -> %s\n", args.input_file, args.output_file);
    printf("Using %u threads\n", args.threads);

    return 0;
}
```

### 4. Compile and Run

```bash
gcc file_processor.c -o file_processor
./file_processor input.txt output.txt -t 4
```

## Argument Types

### Required Arguments

Required arguments are positional and must be provided in order:

```c
#define REQUIRED_ARGS \
    REQUIRED_STRING_ARG(name, "label", "description") \
    REQUIRED_SIZE_ARG(count, "count", "Number of items") \
    REQUIRED_DOUBLE_ARG(threshold, "threshold", "Processing threshold")
```

**Supported types:**
- `REQUIRED_STRING_ARG` - `char*`
- `REQUIRED_CHAR_ARG` - `char`
- `REQUIRED_INT_ARG` - `int`
- `REQUIRED_UINT_ARG` - `unsigned int`
- `REQUIRED_LONG_ARG` - `long`
- `REQUIRED_ULONG_ARG` - `unsigned long`
- `REQUIRED_LONG_LONG_ARG` - `long long`
- `REQUIRED_ULONG_LONG_ARG` - `unsigned long long`
- `REQUIRED_SIZE_ARG` - `size_t`
- `REQUIRED_FLOAT_ARG` - `float`
- `REQUIRED_DOUBLE_ARG` - `double`

### Optional Arguments

Optional arguments have flags and default values:

```c
#define OPTIONAL_ARGS \
    OPTIONAL_INT_ARG(verbose, 0, "-v", "level", "Verbosity level") \
    OPTIONAL_STRING_ARG(config, "config.ini", "--config", "file", "Configuration file") \
    OPTIONAL_DOUBLE_ARG(factor, 1.5, "-f", "factor", "Scaling factor", 2)
```

**Note:** `OPTIONAL_FLOAT_ARG` and `OPTIONAL_DOUBLE_ARG` take an additional precision parameter for displaying defaults in help text.

**Supported types:** Same as required arguments, but with `OPTIONAL_` prefix.

### Boolean Arguments

Boolean flags toggle between true and false if present or missing, respectively:

```c
#define BOOLEAN_ARGS \
    BOOLEAN_ARG(verbose, "-v", "Enable verbose output") \
    BOOLEAN_ARG(quiet, "-q", "Suppress all output") \
    BOOLEAN_ARG(force, "--force", "Force overwrite existing files")
```

## Advanced Usage

### Custom Parsing

For basic types, parsing is handled automatically. For required arguments that need custom parsing, you can specify a parser function:

```c
#define REQUIRED_ARG(type, name, label, description, parser)
```

### Error Handling

`parse_args()` returns 0 if parsing fails (e.g., not enough required arguments). Always check the return value:

```c
if (!parse_args(argc, argv, &args)) {
    fprintf(stderr, "Error parsing arguments\n");
    print_help(argv[0]);
    return 1;
}
```

## Installation

1. Download `easyargs.h`
2. Place it in your project directory or include path
3. Include it in your source files: `#include "easyargs.h"`

No compilation or linking required &mdash; it's header-only!
