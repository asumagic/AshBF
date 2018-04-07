# AshBF compiler

AshBF does not interpret brainfuck code directly. It requires at least two compiler stages to generate AshBF bytecode (see [VM](VM.md)) from Brainfuck sources (typically `.b` or `.bf`).

## Parser

During the parser stage, AshBF compiles Brainfuck commands early into a bytecode form, but is not executable by the VM as-is.  
Unrecognized characters are treated as comments and thus are ignored.

## Optimizer

AshBF performs optimization over the bytecode in order to improve execution performance (see [OPTIMIZATIONS](OPTIMIZATIONS.md)).

## Linker

The bytecode is finally "linked", which means that it performs some final actions so the bytecode can be interpreted by the VM.  
Currently, it recompiles `bfLoopBegin` and `bfLoopEnd` opcodes into `bfJmpZero` and `bfJmpNotZero` opcodes, which is done for performance reasons.