# Using AshBF with JavaScript

**Warning:** This is experimental. Except fairly limited functionality and performance compared to commandline ashbf.

## Building

Refer to [the README build steps](README.md).

## Usage

Use `success = bf(source, input)` to compile, optimize and execute a program.

You can change the `bf_out(program_out)` output callback, which defaults to `console.log`.

You can use `bf_loglevel(level)` to set the compiler log levels to ignore. Disable all output with `bf_loglevel(3)` or enable all output with `bf_loglevel(-1)`. 
Loglevel `0` is verbose, `1` is info, `2` is warning, `3` is error.

Example program:

```js
bf_loglevel(3);
bf(",.,.,.", "abc");
```