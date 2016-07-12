# AshBF

## Introduction

AshBF is a C++ brainfuck interpreter designed for speed. It at least requires compiler support for the C++11 standard, and is best compiled with anything else than MVSC. It is developed with gcc, but should run fine with clang, and most likely icc.

## Compiling

AshBF ships with a CMake configuration file. Typically, you should create a build folder and generate the makefile and executable inside of it.

```bash
mkdir build
cd build
cmake ..
make -j # The -j flags allows for multicore building
```

*Note :* If you experience any issue when compiling with clang, try removing the `-flto` flag in the CMakeLists.txt file and rerun `cmake ..` to attempt compiling again.

## Usage

As for now, the program doesn't take any other argument than the brainfuck source. Flags will be added in the future

``./AshBF <filename>``

## Planned features

Brainfuck code optimizations are planned in order to use the bytecode VM built in the most efficient way as possible. Support for extended brainfuck may be added in the future.

## Disclaimer

This is *unstable* software. The interpreter or the compiler will crash if an invalid program is input. This, of course, will be improved in the future.