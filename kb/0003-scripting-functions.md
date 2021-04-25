# Scripting Functions
_[back to Index](../index.md)_

![Storm Engine Logo](../media/SE_logo.png)

**Related articles**: [Scripting Language Overview](0002-scripting-overview.md), [Article 0004. Entities](0004-entities.md)

Here you will find the list of the built-in and some game-level useful functions for the game scripting.


## Built-In Functions

Below are the functions which make part of the compiler API. Each function has its own unique identifier token that's also listed below.

### Validation

* **GetEngineVersion**: Return `ENGINE_SCRIPT_VERSION` constant (defined in `core.h`). 
    * **Compiler Token**: `FUNC_GETENGINEVERSION`
    * The engine version is pushed back to the compiler, and an error will be produced in case of version mismatch. Therefore, retrieving and using it in the scripting language after the initial call isn't necessary.
    ``` C++
    syntax:
        int GetEngineVersion();
    usage: 
        GetEngineVersion();
    ```

* **CheckFunction**: Verify if a function with specified name exists. 
    * **Compiler Token**: `FUNC_CHECKFUNCTION`
    * `value`: function name.
    ``` C++
    syntax:
        bool CheckFunction(string value);
    usage: 
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
    usage: 
        SetTimeScale(0.0);
    ```

### Math

* **Rand**: Generate a random positive number from 0 to `range`.
    * **Compiler Token**: `FUNC_RAND`
    * `range`: Any positive number.
    ``` C++
    syntax:
        int Rand(int range);
    usage: 
        ref ch = GetCharacter(NPC_GenerateCharacter(...));
        ch.Nation = rand(4); // random nation
    ```

* **frnd**: Generate a random float from 0.0 to 1.0 (included).
    * **Compiler Token**: `FUNC_FRAND`
    ``` C++
    syntax:
        float frnd(); 
    usage: 
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

* **ExitProgram**
    * **Compiler Token**: `FUNC_EXIT_PROGRAM`
    ``` C++
    syntax:
        void ExitProgram(); // quit to desktop
    ```

* **Stop**
    * **Compiler Token**: `FUNC_STOP`
    ``` C++
    syntax:
        void Stop(); // stop executing (thread like)
    ```

* **LoadSegment**
    * **Compiler Token**: `FUNC_LOAD_SEGMENT`
    ``` C++
    syntax:
        int LoadSegment(string fileName); // load program into current space, return zero if failed
    ```

* **UnloadSegment**
    * **Compiler Token**: `FUNC_UNLOAD_SEGMENT`
    ``` C++
    syntax:
        void UnloadSegment(string fileName); // unload segment (delayed)
    ```

* **Trace**
    * **Compiler Token**: `FUNC_TRACE`
    ``` C++
    syntax:
        void Trace(int message);
        void Trace(float message);
        void Trace(string message);
        // send a trace to system.log
    ```

* **MakeInt**
    * **Compiler Token**: `FUNC_MAKE_INT`
    ``` C++
    syntax:
        int MakeInt(string value);
        int MakeInt(float value);
        // convert to int
    ```

* **MakeFloat**
    * **Compiler Token**: `FUNC_MAKE_FLOAT`
    ``` C++
    syntax:
        float MakeFloat(string value);
        float MakeFloat(int value);
    ```

* **DeleteAttribute**
    * **Compiler Token**: `FUNC_DELETE_ATTRIBUTE`
    ``` C++
    syntax:
        void DeleteAttribute(object obj, string attributeAccessString); 
    ```

* **SegmentIsLoaded**
    * **Compiler Token**: `FUNC_SEGMENT_IS_LOADED`
    ``` C++
    syntax:
        int SegmentIsLoaded(1); // string segmentName?
    ```

* **GetAttributesNum**
    * **Compiler Token**: `FUNC_GET_ATTRIBUTES_NUM`
    ``` C++
    syntax:
        int GetAttributesNum(object obj);
        int GetAttributesNum(aref attribute);
    ```

* **GetAttributeN**
    * **Compiler Token**: `FUNC_GET_ATTRIBUTE_BYN`
    ``` C++
    syntax:
        aref GetAttributeN(object obj, int index);
        aref GetAttributeN(aref attribute, int index);
    ```

* **GetAttributeName**
    * **Compiler Token**: `FUNC_GET_ATTRIBUTE_NAME`
    ``` C++
    syntax:
        string GetAttributeName(object obj);
        string GetAttributeName(aref attribute);
    ```


* **EntityUpdate**
    * **Compiler Token**: `FUNC_Entity_UPDATE`
    ``` C++
    syntax:
        void EntityUpdate(bool isEnabled);
    ```

* **IsEntity**
    * **Compiler Token**: `FUNC_IS_Entity_LOADED`
    ``` C++
    syntax:
        int IsEntity(object obj);
    ```

* **DumpAttributes**
    * **Compiler Token**: `FUNC_DUMP_ATTRIBUTES`
    ``` C++
    syntax:
        void DumpAttributes(ref objRef);
    ```

* **sti**
    * **Compiler Token**: `FUNC_STI`
    ``` C++
    syntax:
        int sti(string value);
    ```

* **stf**
    * **Compiler Token**: `FUNC_STF`
    ``` C++
    syntax:
        float stf(string value);
    ```

* **CheckAttribute**
    * **Compiler Token**: `FUNC_CHECK_ATTRIBUTE`
    ``` C++
    syntax:
        bool CheckAttribute(object &obj, string attribute);
    ```

* **argb**
    * **Compiler Token**: `FUNC_ARGB`
    ``` C++
    syntax:
        int argb(int a, int r, int g, int b);
    ```

* **DeleteEntities**
    * **Compiler Token**: `FUNC_DELETE_ENTITIES`
    ``` C++
    syntax:
        void DeleteEntities();
    ```

* **SaveEngineState**
    * **Compiler Token**: `FUNC_SAVEENGINESTATE`
    ``` C++
    syntax:
        void SaveEngineState(1);
    ```

* **LoadEngineState**
    * **Compiler Token**: `FUNC_LOADENGINESTATE`
    ``` C++
    syntax:
        void LoadEngineState(); /1/?
    ```

* **fts**
    * **Compiler Token**: `FUNC_FTS`
    ``` C++
    syntax:
        string fts(float, int); 
    ```

* **SetArraySize**
    * **Compiler Token**: `FUNC_SET_ARRAY_SIZE`
    ``` C++
    syntax:
        void SetArraySize(2); 
    ```

* **GetAttributeValue**
    * **Compiler Token**: `FUNC_GET_ATTRIBUTE_VALUE`
    ``` C++
    syntax:
        string GetAttributeValue(aref attribute);
    ```

* **Vartype**
    * **Compiler Token**: `FUNC_VARTYPE`
    ``` C++
    syntax:
        string Vartype(ref attribute);

* **Breakpoint**
    * **Compiler Token**: `FUNC_BREAKPOINT`
    ``` C++
    syntax:
        void Breakpoint();
    ```

* **CopyAttributes**
    * **Compiler Token**: `FUNC_COPYATTRIBUTES`
    ``` C++
    syntax:
        void CopyAttributes(2);
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

* **GetDeltaTime**
    * **Compiler Token**: `FUNC_GETDELTATIME`
    ``` C++
    syntax:
        int GetDeltaTime(0); 
    ```

* **shl**
    * **Compiler Token**: `FUNC_SHL`
    ``` C++
    syntax:
        int shl(2); 
    ```

* **shr**
    * **Compiler Token**: `FUNC_SHR`
    ``` C++
    syntax:
        int shr(2); 
    ```

* **and**
    * **Compiler Token**: `FUNC_AND`
    ``` C++
    syntax:
        int and(2); 
    ```

* **or**
    * **Compiler Token**: `FUNC_OR`
    ``` C++
    syntax:
        int or(2); 
    ```

* **DeleteEntitiesByType**
    * **Compiler Token**: `FUNC_DELETEENTITIESBYTYPE`
    ``` C++
    syntax:
        void DeleteEntitiesByType(1);
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

* **GetEntity**
    * **Compiler Token**: `FUNC_GETENTITY`
    ``` C++
    syntax:
        int GetEntity(2); 
    ```

* **FindEntity**
    * **Compiler Token**: `FUNC_FINDENTITY`
    ``` C++
    syntax:
        int FindEntity(2); 
    ```

* **FindEntityNext**
    * **Compiler Token**: `FUNC_FINDENTITYNEXT`
    ``` C++
    syntax:
        int FindEntityNext(1); 
    ```

* **GetSymbol**
    * **Compiler Token**: `FUNC_GETSYMBOL`
    ``` C++
    syntax:
        string GetSymbol(2); 
    ```

* **IsDigit**
    **Compiler Token**: `FUNC_ISDIGIT`
    ``` C++
    syntax:
        int IsDigit(2); 
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