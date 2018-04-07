# Using AshBF with JavaScript

**Warning:** This is experimental. Except fairly limited functionality and performance compared to commandline ashbf.

## Building

Refer to [the README build steps](README.md).

## Usage

Set up the WebAssembly functions:

```js
bf = Module.cwrap("brainfuck", "number", ["string", "string"]);
bf_loglevel = Module.cwrap("brainfuck_loglevel", null, ["number"]);
```

You will need to set up the `bf_out(program_out)` output callback in order to execute programs.  
Use `success = bf(source, input)` to compile, optimize and execute a program.

You can use `bf_loglevel(level)` to set the compiler log levels to ignore. Disable all output with `bf_loglevel(3)` or enable all output with `bf_loglevel(-1)`. 
Loglevel `0` is verbose, `1` is info, `2` is warning, `3` is error.

Example program:

```js
bf = Module.cwrap("brainfuck", "number", ["string", "string"]);
bf_loglevel = Module.cwrap("brainfuck_loglevel", null, ["number"]);

bf_out = console.log;
bf_loglevel(3);
bf(",.,.,.", "abc");
```