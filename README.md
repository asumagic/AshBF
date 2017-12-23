# AshBF

## Introduction

[Brainfuck](https://en.wikipedia.org/wiki/Brainfuck) is a very minimalist turing-complete esoteric language with only 8 instructions: `+-><[].,`.

AshBF is an optimizing C++17 brainfuck interpreter tailored for speed.  
It recompiles brainfuck into an optimized IL, later interpreted by a VM.

## Compiling

You will need `cmake` and `make`, unless you use `ninja` or something else:

```bash
cmake . && make
```

## Usage

`./ashbf <filename> (flags)`

Specify flags with `-flag=value`. When a flag is enabled (`-flag`) without a given value, it will default to `1`.

### `-optimizepasses`

Prevents the optimizer from iterating over N times.  
Typically, one optimization pass will be sufficient. However, due to the way the optimizer is built, one optimization may enable another one next pass.  
`5` is the default.

### `-optimize`

Enables IL optimizations.  
When disabled, the IL will be very similar to the brainfuck source. Pattern optimization will not be performed and stackable instructions (e.g. +, -, >, <) will not be merged.  
`1` is the default.

### `-msize`

Defines the brainfuck tape allocated memory.  
Do note that without the `-sanitize` flag passed, out of bounds memory accesses will cause problems.  
`30000` is the default.

### `-sanitize`

Sanitize brainfuck memory accesses to prevent from out of memory reads or writes.  
When an invalid read or write is detected, the interpreter will exit and print an error.  
`0` is the default.

### `-printil`

Enable IL assembly listings.  
Example for program `+[+.]`:

```
Compiler: Info: Compiled program size is 6 instructions (96 bytes)
0 add 1
1 jz 5
2 add 1
3 cout
4 jnz 2
5 end```

### `-execute`

Enables brainfuck program execution.  
Disabling this may be useful when you are only interested by the IL assembly listings or when you want to profile IL generation.  
`1` is the default.

## Planned features

- More static optimizations. VM improvements?
- Machine code / JIT?
- Brainfuck debugger?
- IRC / Discord bot?
