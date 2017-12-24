# AshBF optimizer

## Instruction stacking

Some instructions are "stackable", meaning that they can be merged into one in some way.

### Instruction stacking

`+` and `-` are compiled down to the same instruction in the IL, but have different arguments. This greatly simplifies basic optimization routines like the peephole optimizer.  
Consider the following program: `+++-++`  
This compiles into the following unoptimized IL:

```
0 add 1
1 add 1
2 add 1
3 add -1
4 add 1
5 add 1
```

We can merge the add instructions in order to generate the following IL:

```
0 add 4
```

`>` and `<` behave the same way, with the `shift` instruction instead.

### Null argument stackable instruction elimination

Stackable instructions with a null argument will be optimized away. Consider this IL (which could appear at some moment in the optimizer loop when compiling `+-`):

```
0 add 0
```

`add 0` will have no side effect and will be removed.

## Peephole optimizations

Peephole optimizations are simple pattern matching optimizations, used to simplify a few common expressions.

### Set to zero optimization

`[+]` and `[-]` will be recompiled down to `set 0`.  
This is because those loops will exit only when the cell is cleared.  
Consider this IL:

```
0 jz 3
1 add 256
2 jnz 1
```

This will be still optimized down to a `set 0` instruction, though the program's visible effect will be different.  
AshBF does not handle this case, and has lax rules on cell overflow.

### Add-set optimization

Consider this IL:

```
0 add 4
1 set 2
```

The first `add` instruction will have no effect and will be optimized away.

### Set-add optimization

Consider this IL (for `[-]++++`):

```
0 set 0
1 add 4
```

We may merge the `set 0` and `add 4` instructions down to `set 4`.

### Set-set optimization

Consider this IL (which could e.g. be generated after a few optimization passes):

```
0 set 4
1 set 0
```

This will be optimized down to `set 0`, because the `set 4` instruction is ineffective.

### Shift until zero optimization

Consider this IL:

```
0 jz 3
1 shift 2
2 jnz 1
```

This can be optimized down to `suz 2`, which is implemented into native code.

## Balanced loop optimizations

A balanced loop is a loop that exits with the same tape pointer as when the loop began.  
In other words, when there are as many `>` as there are `<` in a loop within a brainfuck program, we can make a few assumptions because we precisely know, relative to the tape pointer as it is when the loop begins, which memory cells are being modified.

### Set-balanced loop optimization **(UNIMPLEMENTED)**

Consider the following program: `[-]++++[>+>+<<-]`

This program will compile, unoptimized, into:

```
0  jz 3
1  add -1
2  jnz 1
3  add 1
4  add 1
5  add 1
6  add 1
7  jz 16
8  shift 1
9  add 1
10 shift 1
11 add 1
12 shift -1
13 shift -1
14 add -1
15 jnz 8
16 end
```

The peephole optimizer will recompile `[-]` as a `set 0` statement and stack down the shifts and adds.  
Within the next pass, it will detect a `set`-`add` sequence (`[-]`-`++++`). This can be optimized down into `set 4`.

The program already gets much simpler:

```
0 set 4
1 jz 9
2 shift 1
3 add 1
4 shift 1
5 add 1
6 shift -2
7 add -1
8 jnz 2
9 end
```

There is still more we can do. `[>+>+<<-]` is an innermost loop. The sum of its shifts is 0 - the loop is balanced.  

This allows us to make the following assumptions about the loop:
- `*sp -= 1` each iteration, so this loop will iterate `*sp` times
- `*(sp + 1) += *sp`
- `*(sp + 2) += *sp`

However, we know `*sp` because of the `set 4` instruction! Plus, when the loop exits `*sp` will be `0`. We thus can unroll the loop and eliminate some code:

```
0 shift 1
1 add 4
2 shift 1
3 add 4
4 shift -2
```

### 1-decremented balanced loop optimization (UNIMPLEMENTED)

`[>++>+<<-]` alone could be optimized (a lot).

An instruction `relmul` would multiply `*sp` by `*(sp + relmul)` and *could* help there. However, in case of `++` we want to add `2 * N`. Because we do not know the value at `*(sp + 1)` we cannot quite do it this way, because there is only one argument.

A (complicated) solution I've thought of: implement a 'data' segment for the program, beyond `end` perhaps, which would use the arguments to store data. This would require implementation effort, is not quite optimal and would look hacky.
