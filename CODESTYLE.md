# AshBF codestyle

**This is WIP**: very incomplete, may be inaccurate and unorganized.

This stores my few opinions on codestyle I am still thinking about.

Some points are just general philosophy that the codebase is being reworked to be following.

## Size restrictions

A line must not exceed 132 columns.

Files over 300 lines of code may benefit from being split.  

## Indent, line returns

Very WIP.

Avoid too much levels of indent: From over 4 levels of indent it often gets unreadable.

Indent is done with tabs, which are visually 4 spaces large in a monospace font. Extra cosmetic alignment is done with spaces, when required.

Scope delimiters are always alone on their own line, e.g.:
```cpp
if (cond())
{
    f();
}
```

An exception except is done for do..while, where `while` and the conditional expression is on the same line as the scope end delimiter `}`:

```cpp
do
{
	f();
} while (cond());
```

## Struct vs class

TODO: wording on class philosophy

Whenever possible, `struct`s are used to pack meaningfully related data that don't need to be hidden and keep member functions at a minimum. `struct`s generally don't have constructors under that convention, so it is okay and prefered to use aggregate initialization when possible. If C++20 is used, named initializers are recommended.  
Otherwise, `class` is used, and public members are generally avoided, as `class`es under that convention tend to act as "black boxes". 

Basically, `struct`s are pure data, whereas `class` acts more like a state machine.

## Function declaration and calls, function parameters

Always break function parameter lists, for both declaration and calling, when the parameter count exceeds two or when a function parameter is visually long and inconvenient to read, e.g.:

```cpp
void foo(int a, int b);
```

```cpp
// Declaration
void h(
	FooBarLongVariableType relatively_descriptive_name,
	int c
);

// Definition
void h(
	FooBarLongVariableType relatively_descriptive_name,
	int c
)
{
	blah();
}
```

However, when dealing with a significant amount of arguments, consider packing them within a `struct` if relevant.

## Naming

Class names use `CamelCase`.

Function names, variables, enum class entries, namespace names use lowercase `snake_case`.

Macro names, non-`enum class` `enum`s use uppercase `SNAKE_CASE`.

Private members are prefixed with `_`, whereas public members have no prefix.  
When a constructor initializes at least one public member variable, all of its parameters shall be prefixed by `p_`.

## Streams and string literals

When dealing with long string literals to print, you can save a few characters moving it to the next line, indented once, e.g.:

```cpp
void f()
{
	verbout(relatively_long_thing) <<
		"Lorem ipsum dolor sir amet, foo bar, blah blah.\n";
}
```

If absolutely necessary, you can split the string using the style below.

The same rationale is used for multiline strings. Do not separate string literals in streams using `<<`, use regular string concatenation instead.

```cpp
	stream <<
		"Hello, world!\n"
		"How are you doing?\n"
		"Lorem ipsum dolor sir amet.\n";
```

## Operator overloading, implicit cast

Operator overloading is acceptable, but should not hide unexpected behavior: Keep your overload behavior to something related to the operator itself, unless there is a strong rationale behind it.

Implicit cast to different types is acceptable for `struct`s when relevant.  
Implicit cast to `bool` is recommended for `class`es and `struct`s in order to check whether it is in a "good" (usable) state.

## Standard library

Prefer modern language features.

Do not use manual memory management (i.e. raw `new`/`delete`). As for internal use, smart pointers are the only pointers to carry ownership information. Raw pointers (`*`) carry no ownership information: assume that something else is managing its pointed object lifetime.

Use standard containers when possible.

Prefer `std::string_view` over `std::string`, `std::array` over `std::vector` when possible.

## Exceptions

For now, exceptions are disabled. The idea was to reduce runtime overhead as AshBF is performance-sensitive, but truth is, the cost is unsignificant.

## Documentation and commenting

Fairly undecided yet, but documenting function/variable/class behavior is done using `//!` comments for now, placed just before the function declaration.

Avoid commenting code in general if you can make it as understandable by splitting up intermediate results, more clear naming and splitting functions. Comments can be useful to explain more tough behavior or to justify a rationale, warn about pitfalls, etc.