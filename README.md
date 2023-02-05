# AshBF

## Introduction

[Brainfuck](https://en.wikipedia.org/wiki/Brainfuck) is a minimalist turing-complete esoteric language using only 8 instructions: `+-><[].,`.

AshBF is C++20 optimizing brainfuck interpreter tailored for speed, written entirely from scratch.  
It recompiles brainfuck into an optimized IR to be interpreted by a VM or compiled down to native code.

## Features

- Fast execution through an optimized bytecode VM
- Speedy compilation and optimization
- AOT compilation to x86-64 assembly, C
- IR optimization
- Internal debugging tools for optimizations

## Rationale

lol

## Compiling

`clang` is strongly recommended. `gcc` works, but may poorly optimize the main interpreter loop.

```bash
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ -GNinja ..
ninja
```

## Usage

`./ashbf <filename> (flags)`

Specify flags with `-flag=value`, `-flag` (defaults to 1), `-flagvalue` (when `value` is a numeric value).  
Short names are available for a few flags, e.g. `-x` instead of `-execute`.

### `-optimize-passes`

Prevents the optimizer from running each optimizer more than N times for each stage.  
Typically, one optimization pass will be sufficient. However, due to the way the optimizer is built, one optimization may enable another one next pass.  
Do note that not all optimizations are pass-based.
`5` is the default.

### `-optimize` (`-O`)

Enables IL optimizations.  
When disabled, the IL will be very similar to the brainfuck source. Pattern optimization will not be performed and stackable instructions (e.g. +, -, >, <) will not be merged.  
`1` is the default.

### `-optimize-debug`

Detect optimization regression.
The optimizer will test every optimization to detect optimization bugs.    
It will then compare the ILs and print the difference between them. Note that this is extremely slow and should only be used to detect bugs in the compiler.  
Programs using `,` are not yet supported. Regressions involving VM crashes or sanitization errors are not yet supported.  
`-msize` is ignored during debug.  
Program output will not show to stdout.

### `-optimize-verbose` (`-v`)

Verbose optimization feedback.  
When enabled, the optimizer will give various information on optimization tasks and passes and even more in `-optimize-debug` mode.

### `-legalize-overflow`

By default, cell overflow is assumed illegal, as this is okay with most programs.  
This enables better optimizations in general, but certain programs may encounter issues, in which case you may want to set this flag.

### `-memory-size` (`-m`)

Defines the brainfuck tape allocated memory.  
Do note that without the `-sanitize` flag passed, out of bounds memory accesses will cause problems.  
`30000` is the default.

### `-sanitize` *(unimplemented)*

Sanitize brainfuck memory accesses to prevent from out of memory reads or writes.  
When an invalid read or write is detected, the interpreter will exit and print an error.  
`0` is the default.

### `-print-il`

Enable IL assembly listings.  
Example for program `+[+.]`:

```
Compiler: Info: Compiled program size is 6 instructions (96 bytes)
0 add 1
1 jz 5
2 add 1
3 cout
4 jnz 2
5 end
```

### `-print-il-line-numbers`

Determines whether the IL assembly listings should display line numbers.  
`0` is the default.

### `-execute` (`-x`)

Enables brainfuck program execution.  
Disabling this may be useful when you are only interested by the IL assembly listings or when you want to profile IL generation.  
`1` is the default.
