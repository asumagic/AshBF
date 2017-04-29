# AshBF

## Introduction

AshBF is an optimizing C++14 (moving to C++17) brainfuck interpreter designed for speed. It relies upon a virtual machine designed to use with Brainfuck.

## Compiling

AshBF ships with a CMake configuration file. Typically, you should create a build folder and generate the makefile and executable inside of it.

```bash
mkdir build
cd build
cmake ..
make -j4 # adapt the number to your amount of cores
```

## Usage

While AshBF's commandline support is as for now limited (you may not have spaces in the filename as an example), some flags are present and most notably allow managing the program's available memory or the optimization level.

``./ashbf <filename> (flags)``

Specify flags with ``-flag=value``. When a flag is enabled (``-flag``) without a given value, it will default to ``1``.

Available flags:

- ``optimizepasses`` : Select the amount of passes used by the optimizer. For now, nearly no optimization may benefit from more than a pass. (``5`` is the default)
- ``optimize`` : Enable optimizations (``1`` is the default)
- ``msize`` : Define the amount of cells within for the brainfuck tape. (``30000`` is the default)
- ``sanitize`` : Sanitizes the memory accesses on by the VM (to be implemented).

## Planned features

- More static optimizations. VM improvements?
- Machine code / JIT?
- Brainfuck debugger?
- IRC/Discord bot?
