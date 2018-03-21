# AshBF Virtual Machine

AshBF compiles Brainfuck into an intermediate representation. This allows for optimizations and a faster, simpler interpreter.

The VM is aggressively optimized for performance to minimize the execution time of complex programs.

The VM makes use of two registers:
- `pc`: The program counter, which refers to the current instruction being interpreted.
- `sp`: The brainfuck tape pointer. It can be mainly set through the `shift` instruction.

## VM Instructions

### `add [increment]`

Adds `[increment]` to the memory cell refered to by `sp`.

### `shift [offset]`

Move the `sp` memory pointer by `offset`.

### `mac [immA] [offB]`

Multiply-accumulate `*sp` with an immediate `[immA]` multipled by `*(sp + [offB])`.  
i.e. `*sp = *sp + [immA] * *(sp + [offB])`

### `cout`

Prints the character representation of the memory cell refered to by `sp`.

### `cin`

Reads a character to the memory cell refered to by `sp`.

### `jz [addr]`

Changes `pc` to `[addr]` when the memory cell refered to by `sp` has value `0`.  
This is generally what the the `[` brainfuck command is compiled to.

### `jnz [addr]`

Changes `pc` to `[addr]` when the memory cell refered to by `sp` has a value not equal to `0`.  
This is generally what the `]` brainfuck command is compiled to.

### `set [value]`

Changes the value refered to by `sp` to `[value]`.

### `suz [increment]`

Adds `[increment]` to `sp` until the value refered to by `sp` is `0`.
