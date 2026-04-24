# C Programming Collection

A collection of various C projects, experiments, and learning resources organized into a single repository.

## Project Structure

This repository is organized into several categories:

- **`projects/`**: More complete applications and tools.
  - `c-squares`: A graphical square-based project.
  - `text-editor`: Multiple text editor implementations.
  - `typing_proyect`: A typing speed test application.
  - `chess_timer`, `emotion_logger`, `calculator`, and more.
- **`graphics/`**: Graphical experiments and inspirations.
  - Projects using Raylib, SDL2, and custom implementations (metaballs, starship).
  - Includes various inspirations like `wang-tiles`.
- **`learning/`**: Exercises and code snippets for learning C concepts.
  - Topics: Bitwise operations, loops, memory overflow, and CS50 exercises.
- **`libs/`**: Single-header libraries and shared utilities.
  - `base.h`, `easy-args`, `mate.h`, and `nob.h`.
- **`utils/`**: Small utility programs and data structure implementations.
- **`scratch/`**: Temporary tests, experiments, and random code snippets.

## Getting Started

### Prerequisites

To compile and run these programs, you will need a C compiler:
- `gcc`
- `clang`
- `msvc`

### Compilation

To execute a program, navigate to its directory and compile it with your preferred compiler. For example:

```bash
gcc -o my_program main.c
./my_program
```

*Note: Some projects in `projects/` or `graphics/` might include a `Makefile` or use `nob.h` for building.*

## Important Note

**Read the code before executing.** While these programs are intended to be harmless experiments, you should always exercise caution when running code from the internet on your machine.

## Resources & Libraries

This collection makes use of or is inspired by several excellent libraries and projects:

- **Build Systems**: [nob.h](https://github.com/tsoding/nob.h)
- **CLI Arguments**: [easy-args](https://github.com/gouwsxander/easy-args)
- **Graphics**: [Raylib](https://www.raylib.com/), [SDL2](https://www.libsdl.org/)
- **Utilities**: [stb libraries](https://github.com/nothings/stb)
- **External Tools**: [Cjit](https://github.com/dyne/cjit), [C Scripting](https://github.com/ryanmjacobs/c)
