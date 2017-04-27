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

Flags always begin with a ``-``, the flag name, optionally a ``=`` for clarity and its value. When no value is specified, the flag is set to ``1`` by default.

Available flags:

- ``Opasses`` : Select the amount of passes used by the optimizer. For now, nearly no optimization may benefit from more than a pass. (``-Opasses=5`` is the default)
- ``O`` : Enable optimizations (``-O`` is the default, disable with ``-O0``)
- ``msize`` : Define the amount of cells/bytes available by the program. From Extended Type II, it will define the program's available memory, not the entire memory. (``msize=30000`` is the default)
- ``sanitize`` : Sanitizes the memory accesses on by the VM.

## Planned features

- More static optimizations. VM improvements?
- Machine code / JIT?
- Brainfuck debugger?
- IRC/Discord bot?
