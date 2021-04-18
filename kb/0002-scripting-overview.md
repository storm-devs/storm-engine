# Scripting System Overview
_[back to Index](../index.md)_

![Storm Engine Logo](../media/SE_logo.png)

**Related articles**: [Scripting Functions](0003-scripting-functions.md), [Entities](0004-entities.md)

Upon startup, the game (`ENGINE.exe`) will attempt to load and compile the game logic located in `program` directory. These "scripts" are written in C using the game's API, and compiled at runtime using the built-in compiler. 

## `Program/` File Structure

Each folder inside `program` is roughly responsible for its own area of interest as indicated below. In practice, they also serve as databases for related areas of interest: thus, `characters` will contain all the information and logic related to characters, `items` will have all the stats on items, etc. 

Please note that these do not include any other media than text: all the animations, models, textures, sounds, UI layouts, sounds and videos are located in the `resource` directory. 

The following list is sorted alphabetically and is based on the latest build of _Sea Dogs: To Each His Own_. Because the contents largely define each game, they differ from one title to another.

* `program\` - shared and startup code.
* `program\battle_interface\` - UI logic on land and sea, quick travel.
* `program\BIManager\` - Battle Interface manager.
* `program\cannons\` - cannon generation and game logic.
* `program\characters\` - NPC generation and game logic: faces, names, spawn locations, etc.
* `program\colonies\` - definition of each colony default values.
* `program\controls\` - keybindings and related functionality.
* `program\dialogs\` - quests and conversation logic.
* `program\encounters\` - events on the land and sea.
* `program\interface\` - generic UI logic.
* `program\islands\` - island and city generation: trade/black market data, fort, various locations, etc.
* `program\items\` - everything item-related: types, descriptions, usage...
* `program\loc_ai\` - NPC AI behavior in any situation.
* `program\locations\` - location maps, loading NPC, etc.
* `program\nations\` - nations generation and definitions (relationships et al.)
* `program\net\` - multiplayer module (currently inactive)
* `program\questBook\` - all the journal entries.
* `program\quests\` - quest event handlers, various related modules.
* `program\scripts\` misc. game logic (rumours, sieges, duels, bounty hunters, etc.)
* `program\sea_ai\` - Player and AI behaviour on sea.
* `program\ships\` - ships generation and definitions.
* `program\store\` - merchants logic and database (items, prices, etc.)
* `program\weather\` - weather system definitions and usage.
* `program\worldMap\` - everything world map related.

## Runtime

The game goes through the following phases: 

* Upon startup, each file is analyzed and compiled into a single translation unit (`COMPILER::Compile`).
* During the game runtime, compiler will analyze each token and act upon it by introducing new tokens, updating existing, or removing old ones. This way the program will ebb and flow until it reaches its exit code (`COMPILER::BC_Execute`).

## Typing

In order to be compiled, the scripting files should contain good C-like code. This also means that the variables should be strongly typed. Commonly used are base C types: `int`, `float`, but also `bool` and a simple to use `string`. Any custom structs are supported in the form of `object`s. 

All of these types can also be agglomerated inside arrays. The arrays may be declared both in global and local scope, albeit with a few caveats.

### Base Types

There're three base types in scripting language: 

* `int`: a signed 32-bit integer

``` C++
syntax: 
    int var_name;
examples:
    int GameTime = -2053; // OK
    int FreeSlots = 75;   // OK
```

* `float`: a single-precision floating-point number. Because there're no `double`s, specifying `f` at the end is not required but allowed. 

``` C++
syntax: 
    float var_name;
examples:
    float WindSpeed = 2.0;         // OK
    float WindDirection = -0.0057; // OK
    float ShipDirection = 3.14f;   // OK
```

* `bool`: a logical `true`/`false` operator where `true` is any non-zero value and `false` is zero.

``` C++
syntax: 
    bool var_name;
examples:
    bool IsSpeaking = true;                  // OK
    bool IsDrunk = (AlcoholInBlood > 10.0f); // OK
    bool KilledAtLeastOneEnemy = KillCount;  // OK where KillCount is an int
```

### Strings

In the scripting code, strings are sequences of characters of arbitrary length. You can concatenate multiple strings together using `+` operator. 

``` C++
syntax: 
    string var_name;
example:
    string Name = "John";                    // OK
    string LastName = GetRandomLastName();   // OK, e.g. "Jameson"
    string FullName = Name + " " + LastName; // OK, "John Jameson"
```

### Arrays

The scripting code allows fixed-size arrays of any type. This means that, after being initialized, the length of the array cannot be modified. However, unlike in C, the array _may_ be initialized from a variable. In that case, the array size would be that variable's value at the array creation time. 

``` C++
syntax: 
    var_type var_name[int number_of_elements];
example:
    int Characteristics[10];                // OK
    string CharacteristicsNames[10];        // OK

    int array_size;
    array_size = 10;
    string CharacteristicsNames[array_size]; // Also OK
```

Due to the limitations of the script compiler, a very strict array initialization syntax must be respected.

* **Arrays declared in global scope (outside of a specific function) can only be initialized with constants (as opposed to `#define` variables).**

``` C++
examples:
    #define FOOD     0
    #define WEAPON   5
    #define MAHOGANY 10

    int ExampleOne[3] = { 0, 5, 10 };               // OK
    int ExampleTwo[3] = { FOOD, WEAPON, MAHOGANY }; // Init OK, but all values are 0!
    
    int ExampleThree[3];
    ExampleThree[0] = FOOD;                         // Illegal
    ExampleThree[1] = WEAPON;                       // Illegal
    ExampleThree[2] = MAHOGANY;                     // Illegal

    int ExampleFour[3];
    ExampleThree[0] = 0;                            // OK
    ExampleThree[1] = 5;                            // OK
    ExampleThree[2] = 10;                           // OK

    void SomeFunction()
    {
        ...
    }
```

* **Arrays declared in local scope cannot be initialized with constants. They can, however, be initialized with the `#define`d values.** 

That said, inside the local scope arrays can be assigned values from the `#define` variables, contrary to the global scope.

``` C++
examples:
    #define LEATHER 0
    #define TOBACCO 5
    #define PAPRIKA 10

    void SomeFunction()
    {
        int ExampleOne[3] = { 0, 5, 10 };                  // Illegal
        int ExampleTwo[3] = { LEATHER, TOBACCO, PAPRIKA }; // ??
        
        int ExampleThree[3];
        ExampleThree[0] = LEATHER;                         // OK
        ExampleThree[1] = TOBACCO;                         // OK
        ExampleThree[2] = PAPRIKA;                         // OK
        
        int ExampleFour[3];
        ExampleThree[0] = 0;                               // OK
        ExampleThree[1] = 5;                               // OK
        ExampleThree[2] = 10;                              // OK
    }
```

### Objects, References and Attributes

An `object` is a treelike text structure of an arbitrary shape. This means that it can store any amount of fields in _text_ form, even though specifying a number without quotes is allowed.

Each object can be promoted to 'entity object' by binding itself to an entity type (using `CreateClass` or `CreateEntity` functions). Entity types are described in detail [here](0004-entities.md). 

Uninitialized objects can still be created and used by the scripts, except they won't be considered engine API.

The strings may be converted back to `int` or `float` using `sti` and `stf` functions. 

``` C++
syntax:
    object object_name;
example:
    object Sky;
    if (!isEntity(&Sky))
    {
        CreateEntity(&Sky, "Sky");
    }
    Sky.State = "sunset";
    Sky.Size = 10;
    Sky.Angle = "25.0f";

    int skyAngle = stf(Sky.Angle); // 25.0f
    float skySize = sti(Sky.Size); // 10
```

Objects can also be accessed by reference, defined as `ref`. Usually it's useful if you need to access a specific object inside the array. 

In order to create a reference, `makeref` call must be made. After `ref` has been created, usage syntax is identical to object usage.

``` C++
syntax:
    object obj_name;
    ref ref_name;
    makeref(ref, obj);
example:
    object SkyStates[SKY_STATES_N];
    ref Sky;
    int n = 0;

    // iterate over all sky states    
    makeref(Sky, SkyStates[n]);
    Sky.State = "sunset";
    Sky.Size = "10";
    Sky.Angle = "25.0f";
    n++;
    // rinse and repeat
```

Finally, you can access a single attribute of an object using an _attribute_, or `aref`, variable. Similarly to the `ref`, `makearef` call must be made to store the attribute. Since each object can store multiple objects, `aref`s can be quite helpful. 

``` C++
syntax:
    obj obj_name;
    aref aref_name;
    makearef(aref, obj);
example:
    object Item;
    aref  ItemType;

    Item.type = "sword";

    makearef(ItemType, Item.type);
```

If you are accessing an attribute via a variable, you should put that variable inside brackets:

``` C++
example:
    aref aSky;
    string attribute = "d20";
    aSky.Dir.d20 = "NNE";         // OK
    aSky.Dir.(attribute) = "NNE"; // OK
    aSky.Dir.attribute = "NNE";   // Error
```

Predefined attributes of the entity objects will be interpreted by the engine at runtime.

``` C++
example:
    object sky;
    if (!isEntity(&sky))
    {
        CreateEntity(&sky, "Sky");
    }
    sky.State = "sunset";   // Affects game engine directly
    sky.Size = 10;          // Affects game engine directly
    sky.Angle = "25.0f";    // Affects game engine directly
    sky.Coverage = "clear"; // Does not affect the game engine directly, 
                            // probably used elsewhere in the scripts
```

## Branching

Script compiler supports the common branching options: `if` and `switch` statements, `while` and `for` loops, `goto` jumps. However, due to some scripting engine properties, there're some caveats in the usage.

### Logical Comparisons

The usual operators can be used to determine a boolean value: 

* `!` - Negates a value (if it was `true` it becomes `false`, and viceversa).
* `>` - Greater than
* `<` - Less than
* `>=` - Greater or equal
* `<=` - Less or equal
* `!=` - Not equal
* `==` - equal

// TODO(yakvi): Info below is dated 2005. Is this bug still relevant?

Multiple comparisons may be chained using logical AND (`&&`) and OR (`||`) operators. Inline cal of the functions returning a `bool` is also supported. However, operator mixing of these is not allowed. 

``` C++
example:
    bool a = true;
    bool b = false;
    bool c = false;

    if (a && b) // OK
    {
        // ...
    }

    if (a && b && doWork()) // OK
    {
        // ... 
    }

    if (a && (b || c)) // Illegal
    {
        // ...
    }
```

That said, chaining results of multiple operators is allowed, as well as nesting.

``` C++
example:
    bool a = true;
    bool b = false;
    bool c = false;

    if (a && (b || c)) // Illegal
    {
        // ...
    }

    bool bc = b || c;
    if (a && bc) // OK
    {
        // ...
    }

    if (a) // OK
    {
        if (b || c) // OK
        {
            // ... 
        }
    }
```

Last but not least, the scripting engine supports expression optimization. If an expression's result can be determined earlier (e.g. the first value in an OR chain is true), following values will not be evaluated.

``` C++
example:
    bool a = false;
    bool b = true;
    bool c = false;
    
    if (a ||       // false
        b ||       // true
        c ||       // ignored
        doWork())  // skipped
        {
            // doWork will only be executed if a, b, c are false.
            // ...
        }

    if (a &&      // false
        doWork()) // skipped
        {
            // ...
        }
```

### `if` statement

The code inside the `if` statement block will be executed if the condition is true. Optional `else` block is also supported.

``` C++
syntax:
    if(condition)
    {
        // code if true
    }
    else
    {
        // code if false
    }
```

### `switch` statement

The code inside the `switch` block is evaluated, and code is executed based on specific constants. `default` case is not supported.

``` C++
syntax:
    switch (variable)
    {
        case CONSTANT_1:
            // code
            break;
        case CONSTANT_2:
            // code
            break;
        case CONSTANT_3:
            // code
            break;
        // etc.
    }
```

### `while` loop

The code inside the while loop will run forever until the condition becomes `false`.

``` C++
syntax:
    while(condition)
    {
        // code while condition is true
        // remember to add an exit condition!
    }
```

Additionally, `continue` and `break` keywords are implemented and work as expected.

``` C++
example:
    int i = 0;
    bool isRunning = true;
    while(isRunning)
    {
        // do so work
        if (someEvent)
        {
            i++; 
            continue; // go immediately to the next loop iteration
        }
        
        if (i > 20)
        {
            break; // go out, code below won't be executed
        }

        // more work
    }
```

### `for` loop

A `for` loop behaves as expected. It's similar to a while loop in running until a condition is `false` but it also has two other blocks for initializing an iterator and increasing its value.

``` C++
syntax:
    for(int_var = init_value; condition; increase/decrease int_var)
    {
        // code while condition is true
    }
example:
    for (int i = 0; i < 20; i++)
    {
        // do some work 20 times
    }
```

### `goto` statement

The `goto` statement allows jumping to a label specified above or below the `goto`. 

``` C++
syntax: 
    goto label_name;
    // ... 

    :label_name; 

example:
    :label_above;

    // ...
    if (someEvent)
    {
        goto label_above; // OK
    }
    else
    {
        goto label_below; // OK
    }

    :label_below;
    // ...
```

## Other Considerations

### Functions

Functions are implemented similarly to C. Each function can have several parameters as input and a return type (which can be `void` if there's nothing to return) as output.

``` C++
syntax: 
    return_type FunctionName(parameter_type Param, ...) 
    {
        // function body
    }    
example:
    void DoWork() 
    {
        // ...
    }
    
    bool IsItRaining(float timeOfDay)
    {
        // ... 
    }

```
You can consult the list of built-in functions [here](0003-scripting-functions.md).

### Comments

Both single line `//` and multi-line comments `/**/` are supported.

``` C++
syntax: 
    // single-line comment
    /* 
        multi-line comment
    */
```


--- 

## Original article authors

ALexusB, 22.04.05

Warship, 02.08.09


---

| [Home](../index.md) | [Site Map](../site-map.md) | 