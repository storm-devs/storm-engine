# Style Guideline
_[back to Index](../index.md)_

![Storm Engine Logo](../media/SE_logo.png)

**Related articles**: TBD

*Note: This guideline is based on https://github.com/lefticus/cppbestpractices*

Consistency is the most important aspect of style. The second most important aspect is following a style that the average C++ programmer is used to reading.

C++ allows for arbitrary-length identifier names, so there's no reason to be terse when naming things. Use descriptive names, and be consistent in the style.

This project uses `.clang-format` file that specifies the formatting style. While this cannot help with naming, it is particularly important to maintain a consistent style.

## Naming Conventions

 * This project uses CamelCase
 * Types (classes, structs, enums, etc) start with upper case: `MyClass`.
 * Functions and variables start with lower case: `myMethod`.
 * Constants are all upper case: `const double PI=3.14159265358979323;`.
 * Enum values start with upper case (`enum class MyEnum {VariantOne, VariantTwo}`) 
 * Private data must be with a `m_` prefix to distinguish it from public data. `m_` stands for "member" data.

## Don't Name Anything Starting With `_`

If you do, you risk colliding with names reserved for compiler and standard library implementation use:

http://stackoverflow.com/questions/228783/what-are-the-rules-about-using-an-underscore-in-a-c-identifier


## Well-Formed Example

```cpp
class MyClass
{
  public:
    MyClass(int data) : m_data(data)
    {
    }

    int getData() const
    {
        return m_data;
    }

  private:
    int m_data;
};
```



## Enable Out-of-Source-Directory Builds

Make sure generated files go into an output folder that is separate from the source folder.


## Use `nullptr`

C++11 introduces `nullptr` which is a special value denoting a null pointer. This should be used instead of `0` or `NULL` to indicate a null pointer.

## Comments

Comment blocks should use `//`, not `/* */`. Using `//` makes it much easier to comment out a block of code while debugging.

```cpp
// this function does something
int myFunc()
{
}
```

To comment out this function block during debugging we might do:

```cpp
/*
// this function does something
int myFunc()
{
}
*/
```

which would be impossible if the function comment header used `/* */`.

## Never Use `using namespace` in a Header File

This causes the namespace you are `using` to be pulled into the namespace of all files that include the header file.
It pollutes the namespace and it may lead to name collisions in the future.
Writing `using namespace` in an implementation file is fine though.


## Include Guards

Use the `#pragma once` directive instead include guards which is quasi-standard across many compilers.
It's short and makes the intent clear.

## Use "" for Including Local Files
... `<>` is [reserved for system includes](http://blog2.emptycrate.com/content/when-use-include-verses-include).

```cpp
// Bad Idea. Requires extra -I directives to the compiler
// and goes against standards.
#include <string>
#include <includes/MyHeader.hpp>

// Worse Idea
// Requires potentially even more specific -I directives and
// makes code more difficult to package and distribute.
#include <string>
#include <MyHeader.hpp>


// Good Idea
// Requires no extra params and notifies the user that the file
// is a local file.
#include <string>
#include "MyHeader.hpp"
```

## Initialize Member Variables
...with the member initializer list. 

For POD types, the performance of an initializer list is the same as manual initialization, but for other types there is a clear performance gain, see below.

```cpp
// Bad Idea
class MyClass
{
  public:
    MyClass(int value)
    {
        m_value = value;
    }

  private:
    int m_value;
};

// Bad Idea
// This leads to an additional constructor call for m_myOtherClass
// before the assignment.
class MyClass
{
  public:
    MyClass(MyOtherClass myOtherClass)
    {
        m_myOtherClass = myOtherClass;
    }

  private:
    MyOtherClass m_myOtherClass;
};

// Good Idea
// There is no performance gain here but the code is cleaner.
class MyClass
{
  public:
    MyClass(int value) : m_value(value)
    {
    }

  private:
    int m_value;
};

// Good Idea
// The default constructor for m_myOtherClass is never called here, so 
// there is a performance gain if MyOtherClass is not is_trivially_default_constructible. 
class MyClass
{
  public:
    MyClass(MyOtherClass myOtherClass) : m_myOtherClass(myOtherClass)
    {
    }

  private:
    MyOtherClass m_myOtherClass;
};
```

In C++11 you can assign default values to each member (using `=` or using `{}`).

### Assigning default values with =

```cpp
// ... //
  private:
    int m_value = 0; // allowed
    unsigned m_value_2 = -1; // narrowing from signed to unsigned allowed
// ... //
```
This ensures that no constructor ever "forgets" to initialize a member object.

### Assigning default values with brace initialization

Using brace initialization does not allow narrowing at compile-time.

```cpp
// Best Idea

// ... //
  private:
    int m_value{ 0 }; // allowed
    unsigned m_value_2 { -1 }; // narrowing from signed to unsigned not allowed, leads to a compile time error
// ... //
```

Forgetting to initialize a member is a source of undefined behavior bugs which are often extremely hard to find.

If the member variable is not expected to change after the initialization, then mark it `const`.

```cpp
class MyClass
{
  public:
    MyClass(int value) : m_value{value}
    {
    }

  private:
    const int m_value{0};
};
```

Since a const member variable cannot be assigned a new value, such a class may not have a meaningful copy assignment operator.

## Always Use Namespaces

There is almost never a reason to declare an identifier in the global namespace. Instead, functions and classes should exist in an appropriately named namespace or in a class inside of a namespace. Identifiers which are placed in the global namespace risk conflicting with identifiers from other libraries (mostly C, which doesn't have namespaces).

This project uses `storm` namespace

## Use the Correct Integer Type for Standard Library Features

The standard library generally uses `std::size_t` for anything related to size. The size of `size_t` is implementation defined.

In general, using `auto` will avoid most of these issues, but not all.

Make sure you stick with the correct integer types and remain consistent with the C++ standard library. It might not warn on the platform you are currently using, but it probably will when you change platforms.

*Note that you can cause integer underflow when performing some operations on unsigned values. For example:*

```cpp
std::vector<int> v1{2,3,4,5,6,7,8,9};
std::vector<int> v2{9,8,7,6,5,4,3,2,1};
const auto s1 = v1.size();
const auto s2 = v2.size();
const auto diff = s1 - s2; // diff underflows to a very large number
```

## Use .hpp and .cpp for Your File Extensions

Ultimately this is a matter of preference, but .hpp and .cpp are widely recognized by various editors and tools. So the choice is pragmatic. Specifically, Visual Studio only automatically recognizes .cpp and .cxx for C++ files, and Vim doesn't necessarily recognize .cc as a C++ file.

One particularly large project ([OpenStudio](https://github.com/NREL/OpenStudio)) uses .hpp and .cpp for user-generated files and .hxx and .cxx for tool-generated files. Both are well recognized and having the distinction is helpful.

## Never Mix Tabs and Spaces

Some editors like to indent with a mixture of tabs and spaces by default. This makes the code unreadable to anyone not using the exact same tab indentation settings. Configure your editor so this does not happen.

## Never Put Code with Side Effects Inside an assert()

```cpp
assert(registerSomeThing()); // make sure that registerSomeThing() returns true
```

The above code succeeds when making a debug build, but gets removed by the compiler when making a release build, giving you different behavior between debug and release builds.
This is because `assert()` is a macro which expands to nothing in release mode.

## Don't Be Afraid of Templates

They can help you stick to [DRY principles](http://en.wikipedia.org/wiki/Don%27t_repeat_yourself).
They should be preferred to macros, because macros do not honor namespaces, etc.

## Use Operator Overloads Judiciously

Operator overloading was invented to enable expressive syntax. Expressive in the sense that adding two big integers looks like `a + b` and not `a.add(b)`. Another common example is `std::string`, where it is very common to concatenate two strings with `string1 + string2`.

However, you can easily create unreadable expressions using too much or wrong operator overloading. When overloading operators, there are three basic rules to follow as described [on stackoverflow](http://stackoverflow.com/questions/4421706/operator-overloading/4421708#4421708).

Specifically, you should keep these things in mind:

* Overloading `operator=()` when handling resources is a must. See [Consider the Rule of Zero](#consider-the-rule-of-zero) below.
* For all other operators, only overload them when they are used in a context that is commonly connected to these operators. Typical scenarios are concatenating things with +, negating expressions that can be considered "true" or "false", etc.
* Always be aware of the [operator precedence](http://en.cppreference.com/w/cpp/language/operator_precedence) and try to circumvent unintuitive constructs.
* Do not overload exotic operators such as ~ or % unless implementing a numeric type or following a well recognized syntax in specific domain.
* [Never](http://stackoverflow.com/questions/5602112/when-to-overload-the-comma-operator?answertab=votes#tab-top) overload `operator,()` (the comma operator).
* Use non-member functions `operator>>()` and `operator<<()` when dealing with streams. For example, you can overload `operator<<(std::ostream &, MyClass const &)` to enable "writing" your class into a stream, such as `std::cout` or an `std::fstream` or `std::stringstream`. The latter is often used to create a string representation of a value.
* There are more common operators to overload [described here](http://stackoverflow.com/questions/4421706/operator-overloading?answertab=votes#tab-top).

More tips regarding the implementation details of your custom operators can be found [here](http://courses.cms.caltech.edu/cs11/material/cpp/donnie/cpp-ops.html).

## Avoid Implicit Conversions

### Single Parameter Constructors

Single parameter constructors can be applied at compile time to automatically convert between types. This is handy for things like `std::string(const char *)` but should be avoided in general because they can add to accidental runtime overhead.

Instead mark single parameter constructors as `explicit`, which requires them to be explicitly called.

### Conversion Operators

Similarly to single parameter constructors, conversion operators can be called by the compiler and introduce unexpected overhead. They should also be marked as `explicit`.

```cpp
//bad idea
struct S
{
    operator int()
    {
        return 2;
    }
};
```

```cpp
//good idea
struct S
{
    explicit operator int()
    {
        return 2;
    }
};
```

## Consider the Rule of Zero

The Rule of Zero states that you do not provide any of the functions that the compiler can provide (copy constructor, copy assignment operator, move constructor, move assignment operator, destructor) unless the class you are constructing does some novel form of ownership.

The goal is to let the compiler provide optimal versions that are automatically maintained when more member variables are added.

[This article](http://www.nirfriedman.com/2015/06/27/cpp-rule-of-zero/) provides a background and explains techniques for implementing nearly 100% of the time.

# Considering Safety


## Const as Much as Possible
`const` tells the compiler that a variable or method is immutable. This helps the compiler optimize the code and helps the developer know if a function has a side effect. Also, using `const &` prevents the compiler from copying data unnecessarily. The  [comments on `const` from John Carmack](http://kotaku.com/454293019) are also a good read.

```cpp
// Bad Idea
class MyClass
{
  public:
    void doSomething(int i);
    void doSomething(std::string str);
};

// Good Idea
class MyClass
{
  public:
    void doSomething(const int i);
    void doSomething(const std::string &str);
};

```

### Carefully Consider Your Return Types

 * Getters
   * Returning by `&` or `const &` can have significant performance savings when the normal use of the returned value is for observation
   * Returning by value is better for thread safety and if the normal use of the returned value is to make a copy anyhow, there's no performance lost
   * If your API uses covariant return types, you must return by `&` or `*`
 * Temporaries and local values
   * Always return by value.


references: https://github.com/lefticus/cppbestpractices/issues/21 https://twitter.com/lefticus/status/635943577328095232 

### Do not pass and return simple types by const ref 

```cpp
// Very Bad Idea
class MyClass
{
  public:
    explicit MyClass(const int &intValue) : m_intValue(intValue)
    {
    }

    const int &getIntValue() const
    {
        return m_intValue;
    }

  private:
    int m_intValue;
}
```

Instead, pass and return simple types by value. If you plan not to change passed value, declare them as `const`, but not `const` refs:

```cpp
// Good Idea
class MyClass
{
  public:
    explicit MyClass(const int intValue) : m_intValue(intValue)
    {
    }

    int getIntValue() const
    {
        return m_intValue;
    }

  private:
    int m_intValue;
}
```

Why? Because passing and returning by reference leads to pointer operations instead by much more faster passing values in processor registers.

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

Exceptions cannot be ignored. Return values, such as using `boost::optional`, can be ignored and if not checked can cause crashes or memory errors. An exception, on the other hand, can be caught and handled. Potentially all the way up the highest level of the application with a log and automatic restart of the application.

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

## Do not define a variadic function
Variadic functions can accept a variable number of parameters. The probably best known example is printf(). You have the possibility to define this kind of functions by yourself but this is a possible security risk. The usage of variadic functions is not type safe and the wrong input parameters can cause a program termination with an undefined behavior. This undefined behavior can be exploited to a security problem.
If you have the possibility to use a compiler that supports C++11, you can use variadic templates instead.

[It is technically possible to make typesafe C-style variadic functions with some compilers](https://github.com/lefticus/cppbestpractices/issues/53)

## Additional Resources

[How to Prevent The Next Heartbleed](http://www.dwheeler.com/essays/heartbleed.html) by David Wheeler is a good analysis of the current state of code safety and how to ensure safe code.

# Considering Maintainability


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

## Consider Avoiding Boolean Parameters

They do not provide any additional meaning while reading the code. You can either create a separate function that has a more meaningful name, or pass an enumeration that makes the meaning more clear.

See http://mortoray.com/2015/06/15/get-rid-of-those-boolean-function-parameters/ for more information.

## Avoid Raw Loops

Know and understand the existing C++ standard algorithms and put them to use.

 * See [cppreference](https://en.cppreference.com/w/cpp/algorithm)
 * Watch [C++ Seasoning](https://www.youtube.com/watch?v=qH6sSOr-yk8)
 
Consider a call to `[]` as a potential code smell, indicating that an algorithm was not used where it could have been.


## Never Use `assert` With Side Effects

```cpp
// Bad Idea
assert(setValue(something));

// Better Idea
[[maybe_unused]] const auto success = setValue(something);
assert(success);
```

The `assert()` will be removed in release builds which will prevent the `setValue` call from ever happening.

So while the second version is uglier, the first version is simply not correct.


## Properly Utilize 'override' and 'final'

These keywords make it clear to other developers how virtual functions are being utilized, can catch potential errors if the signature of a virtual function changes, and can possibly [hint to the compiler](http://stackoverflow.com/questions/7538820/how-does-the-compiler-benefit-from-cs-new-final-keyword) of optimizations that can be performed.


# Considering Portability

## Know Your Types

Most portability issues that generate warnings are because we are not careful about our types. Standard library and arrays are indexed with `size_t`. Standard container sizes are reported in `size_t`. If you get the handling of `size_t` wrong, you can create subtle lurking 64-bit issues that arise only after you start to overflow the indexing of 32-bit integers. char vs unsigned char.

http://www.viva64.com/en/a/0010/

## Use The Standard Library

### `std::filesystem`

C++17 added a new `filesystem` library which provides portable filesystem access across all supporting compilers

### `std::thread`

C++11's threading capabilities should be utilized over `pthread` or `WinThreads`.

## Other Concerns

Most of the other concerns in this document ultimately come back to portability issues. [Avoid statics](#statics) is particularly of note.

# Considering Threadability

## Avoid Global Data

Global data leads to unintended side effects between functions and can make code difficult or impossible to parallelize. Even if the code is not intended today for parallelization, there is no reason to make it impossible for the future.

### Statics

Besides being global data, statics are not always constructed and deconstructed as you would expect. This is particularly true in cross-platform environments. See for example, [this g++ bug](https://gcc.gnu.org/bugzilla/show_bug.cgi?id=66830) regarding the order of destruction of shared static data loaded from dynamic modules.

### Shared Pointers

`std::shared_ptr` is "as good as a global" (http://stackoverflow.com/a/18803611/29975) because it allows multiple pieces of code to interact with the same data.

### Singletons

A singleton is often implemented with a static and/or `shared_ptr`.

## Avoid Heap Operations

Much slower in threaded environments. In many or maybe even most cases, copying data is faster. Plus with move operations and such and things.

## Mutex and mutable go together (M&M rule, C++11)
For member variables it is good practice to use mutex and mutable together. This applies in both ways:
* A mutable member variable is presumed to be a shared variable so it should be synchronized with a mutex (or made atomic)
* If a member variable is itself a mutex, it should be mutable. This is required to use it inside a const member function.

For more information see the following article from Herb Sutter: http://herbsutter.com/2013/05/24/gotw-6a-const-correctness-part-1-3/

See also [related safety discussion](#consider-return-by-value-for-mutable-data-const--for-immutable) about `const &` return values

# Considering Performance

## Build Time



### Forward Declare When Possible

This:

```cpp
// some header file
class MyClass;

void doSomething(const MyClass &);
```

instead of:

```cpp
// some header file
#include "MyClass.hpp"

void doSomething(const MyClass &);
```


This applies to templates as well:

```cpp
template<typename T> class MyTemplatedType;
```

This is a proactive approach to reduce compilation time and rebuilding dependencies.

*Note: forward declaration does prevent more inlining and optimizations. It's recommended to use Link Time Optimization or Link Time Code Generation for release builds.*

### Avoid Unnecessary Template Instantiations

Templates are not free to instantiate. Instantiating many templates, or templates with more code than necessary increases compiled code size and build time.

For more examples see [this article](http://blog2.emptycrate.com/content/template-code-bloat-revisited-smaller-makeshared).

### Avoid Recursive Template Instantiations

Recursive template instantiations can result in a significant load on the compiler and more difficult to understand code. 

[Consider using variadic expansions and folds when possible instead.](http://articles.emptycrate.com/2016/05/14/folds_in_cpp11_ish.html)

### Analyze the Build

The tool [Templight](https://github.com/mikael-s-persson/templight) can be used to analyze the build time of your project. It takes some effort to get built, but once you do, it's a drop in replacement for clang++.

After you build using Templight, you will need to analyze the results. The [templight-tools](https://github.com/mikael-s-persson/templight-tools) project provides various methods. (Author's Note: I suggest using the callgrind converter and visualizing the results with kcachegrind).



### Firewall Frequently Changing Header Files



#### Don't Unnecessarily Include Headers

The compiler has to do something with each include directive it sees. Even if it stops as soon as it sees the `#ifndef` include guard, it still had to open the file and begin processing it.

[include-what-you-use](https://github.com/include-what-you-use/include-what-you-use) is a tool that can help you identify which headers you need.

#### Reduce the load on the preprocessor

This is a general form of "Firewall Frequently Changing Header Files" and "Don't Unnecessarily Include Headers." Tools like BOOST_PP can be very helpful, but they also put a huge burden on the preprocessor.

### Consider using precompiled headers

The usage of precompiled headers can considerably reduce the compile time in large projects. Selected headers are compiled to an intermediate form (PCH files) that can be faster processed by the compiler. It is recommended to define only frequently used header that changes rarely as precompiled header (e.g. system and library headers) to achieve the compile time reduction.
But you have to keep in mind, that using precompiled headers has several disadvantages:
* The usage of precompiled header is not portable.
* The generated PCH files are machine dependent.
* The generated PCH files can be quite large.
* It can break your header dependencies. Because of the precompiled headers, every file has the possibility to include every header that is marked as a precompiled header. In result it can happen, that the build fails if you disable the precompiled headers. This can be an issue if you ship something like a library. Because of this it is highly recommend to build once with precompiled header enabled and a second time without them.

Precompiled headers is supported by the most common compiler, like [GCC](https://gcc.gnu.org/onlinedocs/gcc/Precompiled-Headers.html), [Clang](http://clang.llvm.org/docs/PCHInternals.html) and [Visual Studio](https://msdn.microsoft.com/en-us/library/szfdksca.aspx).
Tools like [cotire](https://github.com/sakra/cotire/) (a plugin for cmake) can help you to add precompiled headers to your build system.

### Consider Using Tools

These are not meant to supersede good design

 * [ccache](https://ccache.samba.org/), compile results caching for unix-like operating systems
 * [clcache](https://github.com/frerich/clcache), compile results caching for cl.exe (MSVC)
 * [warp](https://github.com/facebook/warp), Facebook's preprocessor

### Put tmp on Ramdisk

See [this](https://www.youtube.com/watch?v=t4M3yG1dWho) YouTube video for more details.

### Use the gold linker

If on Linux, consider using the gold linker for GCC.

## Runtime

### Analyze the Code!

There's no real way to know where your bottlenecks are without analyzing the code.

 * http://developer.amd.com/tools-and-sdks/opencl-zone/codexl/
 * http://www.codersnotes.com/sleepy

### Simplify the Code

The cleaner, simpler, and easier to read the code is, the better chance the compiler has at implementing it well.

### Use Initializer Lists

```cpp
// This
std::vector<ModelObject> mos{mo1, mo2};

// -or-
auto mos = std::vector<ModelObject>{mo1, mo2};
```

```cpp
// Don't do this
std::vector<ModelObject> mos;
mos.push_back(mo1);
mos.push_back(mo2);
```

Initializer lists are significantly more efficient; reducing object copies and resizing of containers.

### Reduce Temporary Objects

```cpp
// Instead of
auto mo1 = getSomeModelObject();
auto mo2 = getAnotherModelObject();

doSomething(mo1, mo2);
```

```cpp
// consider:

doSomething(getSomeModelObject(), getAnotherModelObject());
```

This sort of code prevents the compiler from performing a move operation...

### Enable move operations

Move operations are one of the most touted features of C++11. They allow the compiler to avoid extra copies by moving temporary objects instead of copying them in certain cases.

Certain coding choices we make (such as declaring our own destructor or assignment operator or copy constructor) prevents the compiler from generating a move constructor.

For most code, a simple

```cpp
ModelObject(ModelObject &&) = default;
```

would suffice. However, MSVC2013 doesn't seem to like this code yet.

### Kill `shared_ptr` Copies

`shared_ptr` objects are much more expensive to copy than you'd think they would be. This is because the reference count must be atomic and thread-safe. So this comment just re-enforces the note above: avoid temporaries and too many copies of objects. Just because we are using a pImpl it does not mean our copies are free.

### Reduce Copies and Reassignments as Much as Possible

For more simple cases, the ternary operator can be used:

```cpp
// Bad Idea
std::string somevalue;

if (caseA)
{
    somevalue = "Value A";
}
else
{
    somevalue = "Value B";
}
```

```cpp
// Better Idea
const std::string somevalue = caseA ? "Value A" : "Value B";
```

More complex cases can be facilitated with an [immediately-invoked lambda](http://blog2.emptycrate.com/content/complex-object-initialization-optimization-iife-c11).

```cpp
// Bad Idea
std::string somevalue;

if (caseA)
{
    somevalue = "Value A";
}
else if (caseB)
{
    somevalue = "Value B";
}
else
{
    somevalue = "Value C";
}
```

```cpp
// Better Idea
const std::string somevalue = [&]() {
    if (caseA)
    {
        return "Value A";
    }
    else if (caseB)
    {
        return "Value B";
    }
    else
    {
        return "Value C";
    }
}();
```


### Avoid Excess Exceptions

Exceptions which are thrown and captured internally during normal processing slow down the application execution. They also destroy the user experience from within a debugger, as debuggers monitor and report on each exception event. It is best to just avoid internal exception processing when possible.

### Get rid of “new”

We already know that we should not be using raw memory access, so we are using `unique_ptr` and `shared_ptr` instead, right?
Heap allocations are much more expensive than stack allocations, but sometimes we have to use them. To make matters worse, creating a `shared_ptr` actually requires 2 heap allocations.

However, the `make_shared` function reduces this down to just one.

```cpp
std::shared_ptr<ModelObjectImpl>(new ModelObjectImpl());

// should become
std::make_shared<ModelObjectImpl>(); // (it's also more readable and concise)
```

### Prefer `unique_ptr` to `shared_ptr`

If possible use `unique_ptr` instead of `shared_ptr`. The `unique_ptr` does not need to keep track of its copies because it is not copyable. Because of this it is more efficient than the `shared_ptr`. Equivalent to `shared_ptr` and `make_shared` you should use `make_unique` (C++14 or greater) to create the `unique_ptr`:

```cpp
std::make_unique<ModelObjectImpl>();
```

Current best practices suggest returning a `unique_ptr` from factory functions as well, then converting the  `unique_ptr` to a `shared_ptr` if necessary.

```cpp
std::unique_ptr<ModelObjectImpl> factory();

auto shared = std::shared_ptr<ModelObjectImpl>(factory());
```

### Get rid of std::endl

`std::endl` implies a flush operation. It's equivalent to `"\n" << std::flush`.


### Limit Variable Scope

Variables should be declared as late as possible, and ideally only when it's possible to initialize the object. Reduced variable scope results in less memory being used, more efficient code in general, and helps the compiler optimize the code further.

```cpp
// Good Idea
for (int i = 0; i < 15; ++i)
{
    MyObject obj(i);
    // do something with obj
}

// Bad Idea
MyObject obj; // meaningless object initialization
for (int i = 0; i < 15; ++i)
{
    obj = MyObject(i); // unnecessary assignment operation
                       // do something with obj
}
// obj is still taking up memory for no reason
```

For C++17 and onwards, consider using init-statement in the `if` and `switch` statements:

```cpp
if (MyObject obj(index); obj.good())
{
    // do something if obj is good
}
else
{
    // do something if obj is not good
}
```

[This topic has an associated discussion thread](https://github.com/lefticus/cppbestpractices/issues/52).

### Prefer `double` to `float`, But Test First

Depending on the situation and the compiler's ability to optimize, one may be faster over the other. Choosing `float` will result in lower precision and may be slower due to conversions. On vectorizable operations `float` may be faster if you are able to sacrifice precision. 

`double` is the recommended default choice as it is the default type for floating point values in C++.

See this [stackoverflow](http://stackoverflow.com/questions/4584637/double-or-float-which-is-faster) discussion for some more information. 

### Prefer `++i` to `i++`
... when it is semantically correct. Pre-increment is [faster](http://blog2.emptycrate.com/content/why-i-faster-i-c) than post-increment because it does not require a copy of the object to be made.

```cpp
// Bad Idea
for (int i = 0; i < 15; i++)
{
    std::cout << i << '\n';
}

// Good Idea
for (int i = 0; i < 15; ++i)
{
    std::cout << i << '\n';
}
```

Even if many modern compilers will optimize these two loops to the same assembly code, it is still good practice to prefer `++i`. There is absolutely no reason not to and you can never be certain that your code will not pass a compiler that does not optimize this.
You should be also aware that the compiler will not be able optimize this only for integer types and not necessarily for all iterator or other user defined types.  
The bottom line is that it is always easier and recommended to use the pre-increment operator if it is semantically identical to the post-increment operator.

### Char is a char, string is a string

```cpp
// Bad Idea
std::cout << someThing() << "\n";

// Good Idea
std::cout << someThing() << '\n';
```

This is very minor, but a `"\n"` has to be parsed by the compiler as a `const char *` which has to do a range check for `\0` when writing it to the stream (or appending to a string). A '\n' is known to be a single character and avoids many CPU instructions.

If used inefficiently very many times it might have an impact on your performance, but more importantly thinking about these two usage cases gets you thinking more about what the compiler and runtime has to do to execute your code.


### Never Use `std::bind`

`std::bind` is almost always way more overhead (both compile time and runtime) than you need. Instead simply use a lambda.

```cpp
// Bad Idea
auto f = std::bind(&my_function, "hello", std::placeholders::_1);
f("world");

// Good Idea
auto f = [](const std::string &s) { return my_function("hello", s); };
f("world");
```


### Know The Standard Library

Properly use the already highly optimized components of the vendor provided standard library.

#### `in_place_t` And Related

Be aware of how to use `in_place_t` and related tags for efficient creation of objects such as `std::tuple`, `std::any` and `std::variant`.

# Considering Correctness

## Avoid Typeless Interfaces


Bad Idea:

```cpp
std::string find_file(const std::string &base, const std::string &pattern);
```

Better Idea:

```cpp
std::filesystem::path find_file(const std::filesystem::path &base, const std::regex &pattern);
```

The above is better but still suffers from having implicit conversions from `std::string` to `std::filesystem::path` and back.

Consider using a typesafe library like

 * https://foonathan.net/type_safe/
 * https://github.com/rollbear/strong_type
 * https://github.com/joboccara/NamedType

Note that stronger typing can also allow for more compiler optimizations.

* [Sorting in C vs C++](Sorting in C vs C++.pdf)

# Final Thoughts

Expand your horizons and use other programming languages. Other languages have different constructs and expressions. Learning what else is out there will encourage you to be more creative with your C++ and write cleaner, more expressive code.


---

| [Home](../index.md) | [Site Map](../site-map.md) | 
