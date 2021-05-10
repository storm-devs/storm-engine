# Scripting Functions
_[back to Index](../index.md)_

![Storm Engine Logo](../media/SE_logo.png)

**Related articles**: [Scripting Language Overview](0002-scripting-overview.md), [Article 0004. Entities](0004-entities.md)

Here you will find the list of the built-in and some game-level useful functions for the game scripting.

## Built-In Functions

Below are the functions which make part of the compiler API. Each function has its own unique identifier token that's also listed below.

### Core Engine Interaction

* **SaveEngineState**: Save game state with a specific name.
    * **Compiler Token**: `FUNC_SAVEENGINESTATE`
    * `saveName`: name of the save. 
    ``` C++
    syntax:
        void SaveEngineState(string saveName);
    ```

* **LoadEngineState**: Load game state from a specific filename.
    * **Compiler Token**: `FUNC_LOADENGINESTATE`
    * `saveName`: name of the save. 
    ``` C++
    syntax:
        void LoadEngineState(string saveName);
    ```

* **LoadSegment**: Load a script file.
    * **Compiler Token**: `FUNC_LOAD_SEGMENT`
    * `filename`: relative path to the file to load
    ``` C++
    syntax:
        bool LoadSegment(string filename);
    example: 
        if (LoadSegment("interface\alchemy.c"))
        {
            // ...
            // do necessary work
            // ...
            UnloadSegment("interface\alchemy.c");
        }
    ```

* **UnloadSegment**: Unload a script file if loaded.
    * **Compiler Token**: `FUNC_UNLOAD_SEGMENT`
    * `filename`: relative path to the file to unload
    ``` C++
    syntax:
        void UnloadSegment(string fileName); // unload segment (delayed)
    example: 
        if (LoadSegment("interface\alchemy.c"))
        {
            // ...
            // do necessary work
            // ...
            UnloadSegment("interface\alchemy.c");
        }
    ```

* **GetDeltaTime**: Get the expected length of the frame (in seconds?)
    * **Compiler Token**: `FUNC_GETDELTATIME`
    ``` C++
    syntax:
        int GetDeltaTime(0); 
    ```

* **Stop**: Stop executing current thread.
    * **Compiler Token**: `FUNC_STOP`
    ``` C++
    syntax:
        void Stop(); 
    ```

* **ExitProgram**: Run `ExitMain` function if it's defined in the loaded segments and exit the game.
    * **Compiler Token**: `FUNC_EXIT_PROGRAM`
    ``` C++
    syntax:
        void ExitProgram();
    ```

### Validation

* **SegmentIsLoaded**: Verify whether a specific script file is loaded
    * **Compiler Token**: `FUNC_SEGMENT_IS_LOADED`
    * `filename`: relative path of the file to test
    ``` C++
    syntax:
        bool SegmentIsLoaded(string filename);
    ```

* **GetEngineVersion**: Return `ENGINE_SCRIPT_VERSION` constant (defined in `core.h`). 
    * **Compiler Token**: `FUNC_GETENGINEVERSION`
    * The engine version is pushed back to the compiler, and an error will be produced in case of version mismatch. Therefore, retrieving and using it in the scripting language after the initial call isn't necessary.
    ``` C++
    syntax:
        int GetEngineVersion();
    example: 
        GetEngineVersion();
    ```

* **CheckFunction**: Verify if a function with specified name exists. 
    * **Compiler Token**: `FUNC_CHECKFUNCTION`
    * `value`: function name.
    ``` C++
    syntax:
        bool CheckFunction(string value);
    example: 
        if (CheckFunction("ControlsTreeInit"))
        {
            ControlsTreeInit();
        }
    ```

* **TestRef**: Verify the referenced object or attribute is still alive.
    * **Compiler Token**: `FUNC_TEST_REF`
    * `target`: a reference or an attribute to test.
    ``` C++
    syntax:
        bool TestRef(ref target); 
        bool TestRef(aref target); 
    example:
        bool LAi_CheckCharacter(aref chr, string out)
        {
            if (!TestRef(chr))
            {
                Trace("LAi_CheckCharacter -> invalid aref, call from " + out);
                return false;
            }

            return true;
        }
    ```

* **IsEntity**: Check if given object was initialized as an entity.
    * **Compiler Token**: `FUNC_IS_Entity_LOADED`
    * `obj`: object to test
    ``` C++
    syntax:
        bool IsEntity(object obj);
        bool IsEntity(ref obj);
    example:
        object torn;
        if (!isEntity(&torn))
        {
            CreateEntity(&torn, "Tornado");
        }
    ```

* **CheckAttribute**: Verify if an attribute exists.
    * **Compiler Token**: `FUNC_CHECK_ATTRIBUTE`
    * `obj`: address of the target object
    * `attribute`: attribute to verify
    ``` C++
    syntax:
        bool CheckAttribute(object &obj, string attribute);
    example: 
        if (!CheckAttribute(&Weather, "Stars.Enable"))
        {
            Weather.Stars.Enable = false;
        };
    ```

* **IsDigit**: Verify whether a character in a string is a digit
    **Compiler Token**: `FUNC_ISDIGIT`
    * `source`: The string containing the character to verify
    * `position`: Position of the tested character in the string
    ``` C++
    syntax:
        bool IsDigit(string source, int position); 
    example: 
        string testString = "1ten20";
        bool test1 = IsDigit(testString, 0); // true
        bool test2 = IsDigit(testString, 3); // false
        bool test1 = IsDigit(testString, 5); // true
    ```

### Entity Manipulation

* **CreateClass**: Bind an object with an entity.
    * **Compiler Token**: `FUNC_CREATE_CLASS`
    * `entityType`: Class name, must be [predefined by the engine](0004-entity.md).
    * Objects bound by `CreateClass` don't expose their attributes to the engine.
    ``` C++
    syntax:
        object CreateClass(string entityType); 
    example:
        object obj = CreateClass("CustomType"); // "invalid entity name"
        object obj = CreateClass("Sky");        // OK
    ```

* **CreateEntity**: Similar to `CreateClass`, except it allows specific attributes to be used directly by the engine. 
    * `objectReference`: Address of the object to which the new entity will be bound.
    * `entityType`: Class name, must be [predefined by the engine](0004-entity.md).
    ``` C++
    syntax:
        bool CreateEntity(object& objectReference, string entityType); 
    example: 
        object torn;
        if (!isEntity(&torn))
        {
            CreateEntity(&torn, "Tornado");
        }
    ```

* **DeleteClass**: Unbinds the entity and clears all the data of an object. All the `ref`s and `aref`s will now fail.
    * **Compiler Token**: `FUNC_DELETE_Entity` (capitalization preserved)
    ``` C++
    syntax:
        void DeleteClass(object obj);
    ```

* **DeleteEntitiesByType**: delete all entities of a specific type.
    * **Compiler Token**: `FUNC_DELETEENTITIESBYTYPE`
    * `type`: type of the entities to delete
    ``` C++
    syntax:
        void DeleteEntitiesByType(string type);
    ```

* **DeleteEntities**: Delete ALL entities.
    * **Compiler Token**: `FUNC_DELETE_ENTITIES`
    ``` C++
    syntax:
        void DeleteEntities();
    example: 
        DeleteEntities();
    ```

* **GetEntity**: Get an entity of a specific type
    * **Compiler Token**: `FUNC_FINDENTITY`
    * `entityPointer`: Address of the ref to contain the entity reference, if found.
    * `name`: Name of the entity type
    * Result is `false` if no entities are found
    * Only the first entity found is returned, the rest are discarded. If you're interested in iterating on the entities of a specific name, please use `FindEntity`.
    ``` C++
    syntax:
        bool GetEntity(ref &entityPointer, string name); 
    example: 
        ref location;
        if (GetEntity(&location, "Location"))
        {
            // Do something with the entity
        }
    ```

* **FindEntity**: Find an entity of a specific type
    * **Compiler Token**: `FUNC_FINDENTITY`
    * `entityPointer`: Address of the ref to contain the entity reference, if found.
    * `name`: Name of the entity type
    * Result is `false` if no entities are found
    * If one or more entities of this type are found, further entities may be accessed by calling `FindEntityNext`.
    ``` C++
    syntax:
        bool FindEntity(ref &entityPointer, string name); 
    example: 
        ref location;
        if (FindEntity(&location, "Location"))
        {
            // Do something with the found entity
        }
    ```

* **FindEntityNext**: Find the next entity if any
    * **Compiler Token**: `FUNC_FINDENTITYNEXT`
    * `entityPointer`: Address of the ref to contain the entity reference, if found.
    * Result is `false` if no other entities are found
    * `FindEntity` must be called to initialize the search first.
    ``` C++
    syntax:
        bool FindEntityNext(ref &entityPointer); 
    example: 
        ref location;
        if (FindEntity(&location, "Location"))
        {
            // Do something with the found entity
            while (FindEntityNext(&location))
            {
                // Iterate of the rest
            }
        }
    ```

* **EntityUpdate**: Toggle entity update on attribute change.
    * **Compiler Token**: `FUNC_Entity_UPDATE`
    * `isEnabled`: if set to `false`, entity will not receive attribute updates.
    ``` C++
    syntax:
        void EntityUpdate(bool isEnabled);
    example:
        EntityUpdate(false);
        // ...
        // Update the object
        // ...
        EntityUpdate(true);
    ```

* **DeleteAttribute**: Remove an attribute (field and data) from given object.
    * **Compiler Token**: `FUNC_DELETE_ATTRIBUTE`
    * `obj`: Object to remove attribute from
    * `attribute`: attribute to remove.
    * If you pass `""` as `attribute`, all the attributes of the object are cleared.
    ``` C++
    syntax:
        void DeleteAttribute(object obj, string attribute); 
    example:
        DeleteAttribute(&Sky, "");        // clear the object
        DeleteAttribute(pchar, "Items"); // remove all collected items 
        string sQuest2 = "quest.Deposits." + city + "_Type2";
        DeleteAttribute(pchar, sQuest2);
    ```

* **GetAttributesNum**: Count how many child attributes an object has (non-recursive).
    * **Compiler Token**: `FUNC_GET_ATTRIBUTES_NUM`
    * `obj/attribute`: object to count child attributes of.
    * This function is often used in conjunction with `GetAttributeN` for iteration on attributes.
    ``` C++
    syntax:
        int GetAttributesNum(object obj);
        int GetAttributesNum(ref obj);
        int GetAttributesNum(aref attribute);
    example: 
        int count = GetAttributesNum(arRoot);
        for (i = 0; i < count; i++)
        {
            aref attribute = GetAttributeN(arRoot, i);
            // ... 
            // Do some work on attribute
            // ... 
        }
    ```

* **GetAttributeN**: Access child attribute by index.
    * **Compiler Token**: `FUNC_GET_ATTRIBUTE_BYN`
    * `obj/attribute`: object to count child attributes of.
    * `index`: Sequential number of the attribute inside the object.
    * This function is often used in conjunction with `GetAttributeNum` for iteration on attributes.
    ``` C++
    syntax:
        aref GetAttributeN(object obj, int index);
        aref GetAttributeN(ref obj, int index);
        aref GetAttributeN(aref attribute, int index);
    example: 
        int count = GetAttributesNum(arRoot);
        for (i = 0; i < count; i++)
        {
            aref attribute = GetAttributeN(arRoot, i);
            // ... 
            // Do some work on attribute
            // ... 
        }
    ```

* **GetAttributeName**: Retrieve the name of a given attribute or object. 
    * **Compiler Token**: `FUNC_GET_ATTRIBUTE_NAME`
    * `obj/attribute`: target object or attribute.
    ``` C++
    syntax:
        string GetAttributeName(object obj);
        string GetAttributeName(ref obj);
        string GetAttributeName(aref attribute);
    example:
        object Test;
        Test.value = "hello";
        string attrName = GetAttributeName(Test.value); // "value"
    ```

* **GetAttributeValue**: Retrieve the stored value of a given attribute or object.
    * **Compiler Token**: `FUNC_GET_ATTRIBUTE_VALUE`
    * `obj/attribute`: target object or attribute.
    * Every value is stored as a string! Stored `object`s, `float`s and `int`s need to be converted accordingly before use.
    ``` C++
    syntax:
        string GetAttributeValue(object obj);
        string GetAttributeValue(ref obj);
        string GetAttributeValue(aref attribute);
    example:
        object Test;
        Test.value = "hello";
        string attrValue = GetAttributeValue(Test.value); // "hello"
    ```
    
* **CopyAttributes**: Copy all the object's attributes to a new object.
    * **Compiler Token**: `FUNC_COPYATTRIBUTES`
    * `destination`: address of the object to receive attributes
    * `source`: object to copy
    ``` C++
    syntax:
        void CopyAttributes(object &destination, object source);
    example: 
        object Tmp;
        CopyAttributes(&Tmp, CargoOne);
    ```

* **DumpAttributes**: Trace (log) all the attribute data of a given object.
    * **Compiler Token**: `FUNC_DUMP_ATTRIBUTES`
    * `obj/attribute`: target object or attribute.
    ``` C++
    syntax:
        void DumpAttributes(object obj);
        void DumpAttributes(ref obj);
        void DumpAttributes(aref attribute);
    example: 
        trace("nConditionsNum : " + nConditionsNum);
        DumpAttributes(conditions);
    ```

### Messages

* **SendMessage**: Send a message to an entity
    * **Compiler Token**: `FUNC_SEND_MESSAGE`
    * `obj`: address of the recipient (or its reference)
    * `stringFormat`: the types of the passed arguments arranged in a string: 
        * `l`: int/bool
        * `f`: float
        * `s`: string
        * `a`, `i`: object or aref
        * `e`: ref
    * `msg`: the arguments passed to the handlers.
    * An object should be registered as an entity for the message to have an effect.
    * Each entity has its own way dealing with the messages it receives. 
    ``` C++
    syntax:
        void SendMessage(ref obj, string stringFormat, msg...); 
    example:
       SendMessage(&Dialog, "lii", 0, Character, &persRef); // pass an int and two objects to "Dialog" object
       SendMessage(&AIBalls, "l", MSG_MODEL_RELEASE); // pass a single int to "AIBalls" object
    ```

### Event Management

* **Event**: Trigger and and call all its event handlers.
    * **Compiler Token**: `FUNC_EVENT`
    * Message is optional, see Messages subsection for syntax. 
    ``` C++
    syntax:
        void Event(string eventName, [MSG]); 
    example: 
        Event("NextDay");                               // no message
        Event("ControlActivation", "s", "ChrAction");   // pass trigger type
    ```
* **PostEvent**: Trigger event's post phase with given delay.
    * **Compiler Token**: `FUNC_POSTEVENT`
    * `eventName`: Name of the event to trigger.
    * `delay`: Delay, in milliseconds.
    * Message is optional, see Messages subsection for syntax. 
    ``` C++
    syntax:
        void PostEvent(string eventName, int delay, [MSG]);
    example:
        PostEvent("My_eventMoveImg", 100);               // no message
        int charIndex = sti(CharacterRef.index);
        PostEvent("eventDialogExit", 1, "l", charIndex); // pass character ID
    ```

* **SetEventHandler**: Add an event handler to the given event.
    * **Compiler Token**: `FUNC_SET_EVENT_HANDLER`
    * `eventName`: name of the event.
    * `functionName`: name of the function to call when the event triggers.
    * `post`: `true` for events to be processed with a delay.
    * If an event with the given name doesn't exist, it will be created.
    ``` C++
    syntax:
        void SetEventHandler(string eventName, string functionName, bool post);
        #event_handler(string eventName, string functionName, bool post);
    example: 
        SetEventHandler("frame", "RefreshTableByFrameEvent", false); // execute when "Event()" is called
        SetEventHandler("frame", "ProcessFrame", true); // execute when "PostEvent()" is called
    ```

* **DelEventHandler**: Remove given event handler from the event.
    * **Compiler Token**: `FUNC_DEL_EVENT_HANDLER`
    * `eventName`: name of the event.
    * `functionName`: name of the function to remove.
    ``` C++
    syntax:
        void DelEventHandler(string eventName, string functionName);
    example:
        DelEventHandler("frame", "RefreshTableByFrameEvent");
    ```

* **GetEventData**: Retrieve an argument from the event message.
    * **Compiler Token**: `FUNC_GET_EVENTDATA`
    * This function is usually called from the function which was registered as an event handler.
    * Arguments are provided in order as they were passed to the message.
    ``` C++
    syntax:
        [any type] GetEventData(); 
    example:
        SetEventHandler("Control Activation", "InfoShow_Control", 0);
        Event("Control Activation", "s", "ChrAction");
        void InfoShow_Control()
        {
            string controlName = GetEventData(); // "ChrAction"
            // ...
        }
    ```

* **ClearEvents**: Remove all the normal event handlers from all the events.
    * **Compiler Token**: `FUNC_CLEAR_EVENTS`
    * Doesn't affect "post" event handlers.
    ``` C++
    syntax:
        void ClearEvents(); 
    ```

* **ClearPostEvents**: Remove all the post event handlers from all the events.
    * **Compiler Token**: `FUNC_CLEAR_POST_EVENTS`
    * Doesn't affect normal event handlers.
    ``` C++
    syntax:
        void ClearPostEvents();
    ```

* **EventsBreak**: Stop executing all the events for the remainder of the frame.
    * NOTE: Currently this function has no effect whatsoever as `bEventsBreak` is always false coming into the event loop.
    * **Compiler Token**: `FUNC_EVENTSBREAK`
    ``` C++
    syntax:
        void EventsBreak();
    ```

### Layer Manipulation

Please note that, unlike the previous versions, the current version of the Storm Engine requires the layers to be entered as IDs (as opposed to strings in the past). You can find the reference table [here](0005-entities.md).

* **LayerAddObject**: add an entity to the specified layer 
    * **Compiler Token**: `FUNC_LAYER_ADDOBJECT`
    * `layerID`: id of the desired layer.
    * `obj`: address of the added object.
    * `priority`: Lower priority entities will be processed first, can be negative.
    ``` C++
    syntax:
        void LayerAddObject(int layerID, object &obj, int priority);
    example: 
        #define SEA_EXECUTE 2
        #define SEA_REALIZE 3
        object tornado;
        CreateEntity(&tornado, "Tornado");
        LayerAddObject(SEA_EXECUTE, &tornado, 65535); 
        LayerAddObject(SEA_REALIZE, &tornado, 65535);
    ```

* **LayerSetExecute**: Mark a layer as an execute layer
    * **Compiler Token**: `FUNC_LAYER_SET_EXECUTE`
    * `layerID`: id of the desired layer.
    ``` C++
    syntax:
        void LayerSetExecute(int layerID);
    example:
        LayerSetExecute(SEA_EXECUTE);
    ```

* **LayerSetRealize**: Mark a layer as a realize layer
    * **Compiler Token**: `FUNC_LAYER_SET_REALIZE`
    * `layerID`: id of the desired layer.
    ``` C++
    syntax:
        void LayerSetRealize(int layerID);
    example:
        LayerSetRealize(SEA_REALIZE);
    ```

* **LayerFreeze**: Pause/unpause layer execution
    * **Compiler Token**: `FUNC_LAYER_FREEZE`
    * `layerID`: id of the desired layer.
    * `isEnabled`: whether or not the layer should be enabled.
    ``` C++
    syntax:
        void LayerFreeze(int layerID, bool isEnabled); 
    ```

* **LayerDelObject**
    * **Compiler Token**: `FUNC_LAYER_DELOBJECT`
    * `layerID`: id of the desired layer.
    * `obj`: address of the removed object.
    ``` C++
    syntax:
        void LayerDelObject(int layerID, object &obj);
    example:
        LayerDelObject(EXECUTE, &Sky);
        LayerDelObject(REALIZE, &Sky);
    ```

Currently disabled/not implemented functions: 

* **LayerCreate**: create a layer with a specified name
    * **Compiler Token**: `FUNC_LAYER_CREATE`
    ``` C++
    syntax:
        void LayerCreate(int layerID, bool ordered);
    ```

* **LayerDelete**: delete a layer with the specified name
    * **Compiler Token**: `FUNC_LAYER_DELETE`
    ``` C++
    syntax:
        void LayerDeleteLayer(int layerID);
    ```

* **LayerDeleteContent**: delete all the objects linked to specified layer.
    * **Compiler Token**: `FUNC_LAYER_DELETE_CONTENT`
    ``` C++
    syntax:
        void LayerDeleteContent(int layerID);
    ```

* **LayerSetMessages**: set specific flags on a layer
    * **Compiler Token**: `FUNC_LAYER_SET_MESSAGES`
    ``` C++
    syntax:
        void LayerSetMessages(int layerID, bool isEnabled); 
    ```

### Utility

* **SetTimeScale**: Set the speed of the game simulation for the whole engine. For your epic slo-mo moments.
    * **Compiler Token**: `FUNC_SETTIMESCALE`
    * `value`: time scale, from 0 to 1.
    * Internally, time scale is stored as a `float`, so passing both `float` and `int` values is allowed. 
    * Because the value affects the whole game, make sure you don't forget to return the time scale back to normal! 
    ``` C++
    syntax:
        void SetTimeScale(int value);
        void SetTimeScale(float value);
    example: 
        SetTimeScale(0.0);
    ```

* **Trace**: Send a trace (small log message) to `system.log` file.
    * **Compiler Token**: `FUNC_TRACE`
    ``` C++
    syntax:
        void Trace(int message);
        void Trace(float message);
        void Trace(string message);
    example:
        int n = LocationInitAntigua(n);
        Trace("Antigua locations " + n);
    ```

### Math

* **Rand**: Generate a random positive number from 0 to `range`.
    * **Compiler Token**: `FUNC_RAND`
    * `range`: Any positive number.
    ``` C++
    syntax:
        int Rand(int range);
    example: 
        ref ch = GetCharacter(NPC_GenerateCharacter(...));
        ch.Nation = rand(4); // random nation
    ```

* **frnd**: Generate a random float from 0.0 to 1.0 (included).
    * **Compiler Token**: `FUNC_FRAND`
    ``` C++
    syntax:
        float frnd(); 
    example: 
        float fChecker = frand();
        if (fChecker < 0.8) {...}
    ```

* **abs**: Return the absolute (always positive) of the given value.
    * **Compiler Token**: `FUNC_ABS`
    * `value`: can be positive or negative, integer or float
    ``` C++
    syntax:
        int abs(int value);
        float abs(float value);

    example:
        int x = 5;
        int y = -5;

        int absX = abs(x); // 5.0
        int absY = abs(y); // 5.0
    ```

* **sqr**: Elevate a number to its square (power of 2).
    * **Compiler Token**: `FUNC_SQR`
    * `value`: can be an integer or a float
    ``` C++
    syntax:
        float sqr(int value);
        float sqr(float value);
    example: 
        int x = 5 * 5;  // 25.0
        int y = sqr(5); // 25.0
    ```

* **Pow**: Elevate a number to a custom power. 
    **Compiler Token**: `FUNC_POW`
    * `base`: number to elevate
    * `exponent`: desired power
    ``` C++
    syntax:
        float Pow(float base, float exponent);
    example:
        Log_Info("" + pow(10.0, 3.0)); // 10^3, 1000
        Log_Info("" + pow(10.0, -3.0)); // 10^3, 0.001
    ```

* **sqrt**: Extract the square root of the given value.
    * **Compiler Token**: `FUNC_SQRT`
    * `value`: can be an integer or a float
    ``` C++
    syntax:
        float sqrt(int value);
        float sqrt(float value);
    example: 
        int x = sqrt(25); // 5.0
    ```

* **sin**: Calculate the sine of the given angle.
    * **Compiler Token**: `FUNC_SIN`
    * `value`: angle in radians.
    ``` C++
    syntax:
        float sin(int value);
        float sin(float value);
    example: 
        Particles.winddirection.x = sin(Whr_GetWindAngle());
    ```

* **cos**: Calculate the cosine of the given angle.
    * **Compiler Token**: `FUNC_COS`
    * `value`: angle in radians.
    ``` C++
    syntax:
        float cos(int value);
        float cos(float value);
    example:
        Particles.winddirection.z = cos(Whr_GetWindAngle());
    ```

* **tan**: Calculate the tangent of the given angle.
    * **Compiler Token**: `FUNC_TAN`
    * `value`: angle in radians.
    ``` C++
    syntax:
        float tan(int value);
        float tan(float value);
    ```

* **atan**: Calculate the arctangent (inverse tangent) of a given number.
    * **Compiler Token**: `FUNC_ATAN`
    * `value`: tangent of the desired angle.
    * returns angle in radians, in the range [-π/2, π/2].
    ``` C++
    syntax:
        float atan(int value);
        float atan(float value);
    ```

* **atan2**: Calculate the arctangent of a given 2D coordinate point.
    * **Compiler Token**: `FUNC_TAN2`
    * `x`: x-coordinate of the point
    * `y`: y-coordinate of the point
    * Return value depends on the input
    ``` C++
    syntax:
        float atan2(int/float x, int/float y); // in any combination
    ```

* **asin**: Calculate the arcsine (inverse sine) of a number.
    * **Compiler Token**: `FUNC_ASIN`
    * `value`: sine of the angle to be inverted, -1 to 1.
    ``` C++
    syntax:
        float asin(int value);
        float asin(float value);
    ```

* **acos**: Calculate the arccosine (inverse cosine) of a number.
    * **Compiler Token**: `FUNC_ACOS`
    * `value`: cosine of the angle to be inverted, -1 to 1.
    ``` C++
    syntax:
        float acos(int value);
        float acos(float value);
    ```

### Conversions

* **MakeInt**: Convert value to an `int`.
    * **Compiler Token**: `FUNC_MAKE_INT`
    * `value` may be a string or an int
    * Returns the converted integer value
    ``` C++
    syntax:
        int MakeInt(string value);
        int MakeInt(float value);
    ```

* **MakeFloat**: Convert value to an `float`.
    * **Compiler Token**: `FUNC_MAKE_FLOAT`
    * `value` may be a string or an int
    * Returns the converted real value
    ``` C++
    syntax:
        float MakeFloat(string value);
        float MakeFloat(int value);
    ```

* **sti**: Convert a `string` to `int`
    * **Compiler Token**: `FUNC_STI`
    * `value` can only be a `string`
    ``` C++
    syntax:
        int sti(string value);
    ```

* **stf**: Convert a `string` to a `float`
    * **Compiler Token**: `FUNC_STF`
    * `value` can only be a `string`
    ``` C++
    syntax:
        float stf(string value);
    ```

* **fts**: Parse a float to string
    * **Compiler Token**: `FUNC_FTS`
    `value`: value to be converted.
    `digits`: Number of significant digits stored.
    ``` C++
    syntax:
        string fts(float value, int digits); 
    example: 
        float pi = 3.141526535
        string pi2 = fts(pi, 3); // 3.14
        string pi4 = fts(pi, 5); // 3.1415
    ```

### Bitwise Operations

* **shl**: Shift the bits of the integer `value` to `n` bits to the left
    * **Compiler Token**: `FUNC_SHL`
    * `value`: The value containing the bits
    * `n`: By how many bits to shift
    * This function is effectively the same as `value << n;`
    ``` C++
    syntax:
        int shl(int value, int n); 
    ```

* **shr**: Shift the bits of the integer `value` to `n` bits to the right
    * **Compiler Token**: `FUNC_SHR`
    * `value`: The value containing the bits
    * `n`: By how many bits to shift
    * This function is effectively the same as `value >> n;`
    ``` C++
    syntax:
        int shr(int value, int n); 
    ```

* **and**: Bitwise AND
    * **Compiler Token**: `FUNC_AND`
    * `value`: The value containing the bits
    * `mask`: Mask containing the bits to compare
    * The resulting value contains all the bits if they're set in the value AND the mask
    * This function is effectively the same as `value & mask;`
    ``` C++
    syntax:
        int and(int value, int mask); 
    ```

* **or**: Bitwise OR
    * **Compiler Token**: `FUNC_OR`
    * `value`: The value containing the bits
    * `mask`: Mask containing the bits to compare
    * The resulting value contains all the bits if they're set in the value OR the mask
    * This function is effectively the same as `value | mask;`
    ``` C++
    syntax:
        int or(int value, int mask); 
    ```

* **argb**
    * **Compiler Token**: `FUNC_ARGB`
    ``` C++
    syntax:
        int argb(int a, int r, int g, int b);
    ```

* **SetArraySize**
    * **Compiler Token**: `FUNC_SET_ARRAY_SIZE`
    ``` C++
    syntax:
        void SetArraySize(2); 
    ```

* **Vartype**
    * **Compiler Token**: `FUNC_VARTYPE`
    ``` C++
    syntax:
        string Vartype(ref attribute);
    ```

* **Breakpoint**
    * **Compiler Token**: `FUNC_BREAKPOINT`
    ``` C++
    syntax:
        void Breakpoint();
    ```

* **strcut**
    * **Compiler Token**: `FUNC_STRCUT`
    ``` C++
    syntax:
        string strcut(3);
    ```

* **findSubStr**
    * **Compiler Token**: `FUNC_FINDSUBSTR`
    ``` C++
    syntax:
        string findSubStr(3)
    ```

* **ClearRef**
    * **Compiler Token**: `FUNC_CLEARREF`
    ``` C++
    syntax:
        void ClearRef(1);
    ```

* **strlen**
    * **Compiler Token**: `FUNC_STRLEN`
    ``` C++
    syntax:
        int strlen(1); 
    ```

* **CreateControl**
    * **Compiler Token**: `FUNC_CREATE_CONTROL`
    ``` C++
    syntax:
        int CreateControl(1); 
    ```

* **DeleteControl**
    * **Compiler Token**: `FUNC_DELETE_CONTROL`
    ``` C++
    syntax:
        void DeleteControl(1);
    ```

* **MapControl**
    * **Compiler Token**: `FUNC_MAP_CONTROL`
    ``` C++
    syntax:
        void MapControl(2);
    ```

* **SetControlFlags**
    * **Compiler Token**: `FUNC_SET_CONTROL_FLAGS`
    ``` C++
    syntax:
        void SetControlFlags(2);
    ```

* **ClearEntityAP**
    * **Compiler Token**: `FUNC_CLEAR_Entity_AP`
    ``` C++
    syntax:
        void ClearEntityAP(1);
    ```

* **GetArraySize**
    * **Compiler Token**: `FUNC_GET_ARRAY_SIZE`
    ``` C++
    syntax:
        int GetArraySize(1); 
    ```

* **GetTargetPlatform**
    * **Compiler Token**: `FUNC_GETTARGETPLATFORM`
    ``` C++
    syntax:
        string GetTargetPlatform(); 
    ```

* **GetSymbol**
    * **Compiler Token**: `FUNC_GETSYMBOL`
    ``` C++
    syntax:
        string GetSymbol(2); 
    ```

* **SaveVariable**
    * **Compiler Token**: `FUNC_SAVEVARIABLE`
    ``` C++
    syntax:
        int SaveVariable(2); 
    ```

* **LoadVariable**
    * **Compiler Token**: `FUNC_LOADVARIABLE`
    ``` C++
    syntax:
        int LoadVariable(2); 
    ```

* **SetControlTreshold**
    * **Compiler Token**: `FUNC_SET_CONTROL_TRESHOLD`
    ``` C++
    syntax:
        void SetControlTreshold(2);
    ```

* **LockControl**
    * **Compiler Token**: `FUNC_LOCK_CONTROL`
    ``` C++
    syntax:
        void LockControl(2);
    ```

---

| [Home](../index.md) | [Site Map](../site-map.md) | 