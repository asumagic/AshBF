# AshBF

## Introduction

AshBF is an optimizing C++14 brainfuck interpreter designed for speed. It relies upon a virtual machine designed to use with Brainfuck.

It fully supports standard Brainfuck and Brainfuck Extended type I.

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

While AshBF's commandline support is as for now limited (you may not have spaces in the filename as an example), some flags are present and most notably allow managing the program's available memory, the optimization level and the brainfuck extended support.

``./AshBF <filename> (flags)``

Flags always begin with a ``-``, the flag name, optionally a ``=`` for clarity and its value. When no value is specified, the flag is set to ``1``.

Available flags:

- ``x`` : Select the extended brainfuck type. (``-x1`` for example, ``-x0`` is the default)
- ``Opasses`` : Select the amount of passes used by the optimizer. For now, nearly no optimization may benefit from more than a pass, and the optimizer will abort when no optimization was done for the current pass. (``-Opasses=5`` is the default)
- ``O`` : Enable optimizations (``-O`` is the default, disable with ``-O0``)
- ``msize`` : Define the amount of cells/bytes available by the program. From Extended Type II, it will define the program's available memory, not the entire memory.

## Planned features

- More optimizations to the brainfuck code.
- Support for extended type II and III
- Attaching a brainfuck debugger
- Brainfuck IDE or a brainfuck Discord bot?

## Disclaimer

This is *unstable* software. The interpreter or the compiler will crash if an invalid program is input. This behaviour will be improved in the future.
