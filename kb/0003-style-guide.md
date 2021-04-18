# Style Guideline
_[back to Index](../index.md)_

![Storm Engine Logo](../media/SE_logo.png)

**Related articles**: TBD

*Note: This guideline is based on [Collaborative Collection of C++ Best Practices](https://github.com/lefticus/cppbestpractices)*

Consistency is the most important aspect of style. The second most important aspect is following a style that the average C++ programmer is used to reading.

C++ allows for arbitrary-length identifier names, so there's no reason to be terse when naming things. Use descriptive names, and be consistent in the style.

This project uses `.clang-format` file that specifies the formatting style. While this cannot help with naming, it is particularly important to maintain a consistent style.

## Naming Conventions

 * This project uses CamelCase
 * Types (classes, structs, enums, etc) start with upper case: `MyClass`.
 * Functions and variables start with upper case to respect legacy code: `MyMethod()`.
 * Constants have `k` prefix: `const double kPi=3.14159265358979323;`.
 * Enum values start with upper case (`enum class MyEnum {VariantOne, VariantTwo}`) 
 * Private data must be with a `m_` prefix to distinguish it from public data. `m_` stands for "member" data.

## Don't Name Anything Starting With `_`

If you do, you risk colliding with names reserved for compiler and standard library implementation use:

<http://stackoverflow.com/questions/228783/what-are-the-rules-about-using-an-underscore-in-a-c-identifier>


## Well-Formed Example

```cpp
class MyClass
{
  public:
    MyClass(int data) : m_data(data)
    {
    }

    int GetData() const
    {
        return m_data;
    }

  private:
    int m_data;
};
```


## Include Guards

Use the `#pragma once` directive instead include guards which is quasi-standard across many compilers.
It's short and makes the intent clear.

## Use "" for including Local Files and <> for Libraries

```cpp
// Bad Idea. Requires extra -I directives to the compiler
// and goes against standards.
#include <string>
#include <storm/OtherLibHeader.hpp>
#include <MyLib/MyLibHeader.hpp>

// Worse Idea
// Requires potentially even more specific -I directives and
// makes code more difficult to package and distribute.
#include <string>
#include <storm/OtherLibHeader.hpp>
#include <MyLibHeader.hpp>


// Good Idea
// Requires no extra params and notifies the user that the file
// is a local file.
#include <string>
#include <storm/OtherLibHeader.hpp>
#include "MyLibHeader.hpp"
```

## Always Use Namespaces

There is almost never a reason to declare an identifier in the global namespace. Instead, functions and classes should exist in an appropriately named namespace or in a class inside of a namespace. Identifiers which are placed in the global namespace risk conflicting with identifiers from other libraries (mostly C, which doesn't have namespaces).

This project uses `storm` namespace.

## Use .hpp and .cpp for Your File Extensions

Ultimately this is a matter of preference, but .hpp and .cpp are widely recognized by various editors and tools. So the choice is pragmatic. Specifically, Visual Studio only automatically recognizes .cpp and .cxx for C++ files, and Vim doesn't necessarily recognize .cc as a C++ file.

One particularly large project ([OpenStudio](https://github.com/NREL/OpenStudio)) uses .hpp and .cpp for user-generated files and .hxx and .cxx for tool-generated files. Both are well recognized and having the distinction is helpful.

## Always Use Spaces

Some editors like to indent with tabs by default. This makes the code unreadable to anyone not using the exact same tab indentation settings. Configure your editor so this does not happen.

## Avoid Raw Memory Access

Raw memory access, allocation and deallocation, are difficult to get correct in C++ without [risking memory errors and leaks](http://blog2.emptycrate.com/content/nobody-understands-c-part-6-are-you-still-using-pointers). C++11 provides tools to avoid these problems.

```cpp
// Bad Idea
MyClass *myobj = new MyClass;

// ...
delete myobj;


// Good Idea
auto myobj = std::make_unique<MyClass>(constructorParam1, constructorParam2); // C++14
auto myobj = std::unique_ptr<MyClass>(new MyClass(constructorParam1, constructoParam2)); // C++11
auto mybuffer = std::make_unique<char[]>(length); // C++14
auto mybuffer = std::unique_ptr<char[]>(new char[length]); // C++11

// or for reference counted objects
auto myobj = std::make_shared<MyClass>(); 

// ...
// myobj is automatically freed for you whenever it is no longer used.
```

## Use `std::array` or `std::vector` Instead of C-style Arrays

Both of these guarantee contiguous memory layout of objects and can (and should) completely replace your usage of C-style arrays for many of the reasons listed for not using bare pointers.

Also, [avoid](http://stackoverflow.com/questions/3266443/can-you-use-a-shared-ptr-for-raii-of-c-style-arrays) using `std::shared_ptr` to hold an array. 

## Use Exceptions

Exceptions cannot be ignored. Return values, such as using `std::optional`, can be ignored and if not checked can cause crashes or memory errors. An exception, on the other hand, can be caught and handled. Potentially all the way up the highest level of the application with a log and automatic restart of the application.

Stroustrup, the original designer of C++, [makes this point](http://www.stroustrup.com/bs_faq2.html#exceptions-why) much better than I ever could.

## Use C++-style cast instead of C-style cast
Use the C++-style cast (static\_cast<>, dynamic\_cast<> ...) instead of the C-style cast. The C++-style cast allows more compiler checks and is considerably safer.

```cpp
// Bad Idea
double x = getX();
int i = (int) x;

// Not a Bad Idea
int i = static_cast<int>(x);
```
Additionally the C++ cast style is more visible and has the possibility to search for.

But consider refactoring of program logic (for example, additional checking on overflow and underflow) if you need to cast `double` to `int`. Measure three times and cut 0.9999999999981 times.

## Avoid Compiler Macros

Compiler definitions and macros are replaced by the preprocessor before the compiler is ever run. This can make debugging very difficult because the debugger doesn't know where the source came from.

```cpp
// Bad Idea
#define PI 3.14159;

// Good Idea
namespace storm
{
class Constants
{
  public:
    // if the above macro would be expanded, then the following line would be:
    //   static const double 3.14159 = 3.14159;
    // which leads to a compile-time error. Sometimes such errors are hard to understand.
    static constexpr double PI = 3.14159;
};
}
```

## Always Use Properly 'override' and 'final'

These keywords make it clear to other developers how virtual functions are being utilized, can catch potential errors if the signature of a virtual function changes, and can possibly [hint to the compiler](http://stackoverflow.com/questions/7538820/how-does-the-compiler-benefit-from-cs-new-final-keyword) of optimizations that can be performed.

## Use The Standard Library

### `std::filesystem`

C++17 added a new `filesystem` library which provides portable filesystem access across all supporting compilers

### `std::thread`

C++11's threading capabilities should be utilized over `pthread` or `WinThreads`.

## Avoid Global Data

Global data leads to unintended side effects between functions and can make code difficult or impossible to parallelize. Even if the code is not intended today for parallelization, there is no reason to make it impossible for the future.

### Statics

Besides being global data, statics are not always constructed and deconstructed as you would expect. This is particularly true in cross-platform environments. See for example, [this g++ bug](https://gcc.gnu.org/bugzilla/show_bug.cgi?id=66830) regarding the order of destruction of shared static data loaded from dynamic modules.

### Shared Pointers

`std::shared_ptr` is "as good as a global" (http://stackoverflow.com/a/18803611/29975) because it allows multiple pieces of code to interact with the same data.

### Singletons

A singleton is often implemented with a static and/or `shared_ptr`.

## Avoid Excess Exceptions

Exceptions which are thrown and captured internally during normal processing slow down the application execution. They also destroy the user experience from within a debugger, as debuggers monitor and report on each exception event. It is best to just avoid internal exception processing when possible.


---

| [Home](../index.md) | [Site Map](../site-map.md) | 
