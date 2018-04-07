# Using AshBF with JavaScript

**Warning:** This is experimental. Except fairly limited functionality and performance compared to commandline ashbf.

## Building

Refer to [the README build steps](README.md).

## Basic usage

The `Module.Ashbf` class handles most of the interaction with the AshBF native code.  
Instanciate it with:

```js
var bf = new Module.Ashbf();
```

Compile a Brainfuck program using the **`compile`** method, e.g. `bf.compile(",.,.,.")`.  
You can pass an input to the program directly using `bf.setInputString("Hello world!")`.  
Finally, run the program using `bf.run()`.

**Warning**: You have to delete the instance once you are done or you will leak memory.

```js
bf.delete();
```

Sample program:

```js
var bf = new Module.Ashbf();
bf.compile(",.,.,.");
bf.setInputString("abc");
console.log(bf.run());
```

## **`compile(source)`** - Compiling a Brainfuck program downto AshBF bytecode

Perform parsing, optimizing and linking of a brainfuck source so it can be interpreted by the AshBF VM.  
Characters that are not part of the Brainfuck command set are ignored.  
Returns `true` on success, `false` on failure.

It is in fact a shorthand for 3 successive calls:

```js
return parse(source) && optimize() && link();
```

Example:

```js
if (!bf.compile("+[+.]")) {
	console.log("Failed to compile program");
} else {
	output = bf.run();
}
```

Do note that you can reuse the `Ashbf` object with a different program by simply compiling over again.

See [COMPILER](COMPILER.md) for details on how AshBF compiles programs.

## **`setInputString`**

Switches the input pipe for the Brainfuck program to a string and sets it.

**Warning**: When the string is entirely read by the Brainfuck program, the last character of the input string will be repeated over again. Append `\0` to the string if you want the input to be NUL-terminated.  
**Warning**: When using strings as input, you should set it again after every `run()` call if you plan to call it multiple times. This is because the input string will get consumed internally.

Example: *See Basic usage above*

## **`setInputStdio`**

Switches the input pipe for the Brainfuck program to the standard input (`std::cin`), which is the default.

Example:

```js
// Feed 'Hello, world!' as an input to the Brainfuck program
bf.setInputString("Hello, world!");
bf.run();

// Request the input from the standard input, i.e. from an input box with emscripten
bf.setInputStdio();
bf.run();
```

## **`setLogLevel`** - Disabling logging

You can disable loglevels directly through the `Module.Ashbf` class globally using `setLogLevel`:

```js
Module.Ashbf.setLogLevel(3);
```

When setting the log level to `-1` or lower, no log level will be filtered.  
For `0` and higher, verbose logging is filtered.  
For `1` and higher, infos are filtered.  
For `2` and higher, warnings are filtered.  
For `3` and higher, errors are filtered.

## Tips

If you want to focus on performance, reuse `Ashbf` objects. It is possible to call `compile` during its lifetime.  
Furthermore, if you are compiling a lot of small programs, you may want to skip the optimization pass by calling `bf.parse(program)` then `bf.link()` instead of using `bf.compile()` (which performs `bf.optimize()`). It may add overhead that in fact lengthens the execution time as a whole.

## Limitations

It is not yet possible to redirect logs to a custom output.  
It is not yet possible to stream program output nor it is possible to have a custom callback for Brainfuck program inputs.