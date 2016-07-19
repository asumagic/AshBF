# AshBF

## Introduction

AshBF is an optimizing C++14 brainfuck interpreter designed for speed. It relies upon a virtual machine designed to use with Brainfuck.

## Compiling

AshBF ships with a CMake configuration file. Typically, you should create a build folder and generate the makefile and executable inside of it.

```bash
mkdir build
cd build
cmake ..
make -j # The -j flags allows for multicore building
```

*Note :* If you experience any issue when compiling with clang or an older gcc, try removing the `-flto` flag in the CMakeLists.txt file and rerun `cmake ..` to attempt compiling again.

## Usage

As for now, the program doesn't take any other argument than the brainfuck source. Flags will be added in the future.
``./AshBF <filename>``

## Planned features

- More optimizations to the brainfuck code.
- Possible support for Brainfuck Extended types.
- Attaching a brainfuck debugger
- Brainfuck IDE or a brainfuck Discord bot?

## Disclaimer

This is *unstable* software. The interpreter or the compiler will crash if an invalid program is input. This behaviour will be improved in the future.
