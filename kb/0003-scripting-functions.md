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

* **CreateEntity**: Similar to `CreateClass`, except it allows select attributes to be used directly by the engine. 
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

### Layer Manipulation

* **LayerAddObject**: add an entity to the specified layer 
    * **Compiler Token**: `FUNC_LAYER_ADDOBJECT`
    * `layerName`: name of the desired layer.
    * `obj`: address of the added object.
    * `priority`: Lower priority entities will be processed first, can be negative.
    ``` C++
    syntax:
        void LayerAddObject(string layerName, object &obj, int priority);
    example: 
        object tornado;
        CreateEntity(&tornado, "Tornado");
        LayerAddObject(SEA_EXECUTE, &tornado, 65535); 
        LayerAddObject(SEA_REALIZE, &tornado, 65535);
    ```

* **LayerSetExecute**: Mark a layer as an execute layer
    * **Compiler Token**: `FUNC_LAYER_SET_EXECUTE`
    * `layerName`: name of the desired layer.
    ``` C++
    syntax:
        void LayerSetExecute(string layerName);
    example:
        LayerSetExecute("sea_execute");
    ```

* **LayerSetRealize**: Mark a layer as a realize layer
    * **Compiler Token**: `FUNC_LAYER_SET_REALIZE`
    * `layerName`: name of the desired layer.
    ``` C++
    syntax:
        void LayerSetRealize(string layerName);
    example:
        LayerSetRealize("sea_realize")        
    ```

* **LayerFreeze**: Pause/unpause layer execution
    * **Compiler Token**: `FUNC_LAYER_FREEZE`
    * `layerName`: name of the desired layer.
    * `isEnabled`: whether or not the layer should be enabled.
    ``` C++
    syntax:
        void LayerFreeze(string name, bool isEnabled); 
    ```

* **LayerDelObject**
    * **Compiler Token**: `FUNC_LAYER_DELOBJECT`
    * `layerName`: name of the desired layer.
    * `obj`: address of the removed object.
    ``` C++
    syntax:
        void LayerDelObject(string name, object &obj);
    example:
        LayerDelObject("execute", &Sky);
        LayerDelObject("realize", &Sky);
    ```

Currently disabled/not implemented functions: 

* **LayerCreate**: create a layer with a specified name
    * **Compiler Token**: `FUNC_LAYER_CREATE`
    ``` C++
    syntax:
        void LayerCreate(string layerName, bool ordered);
    ```

* **LayerDelete**: delete a layer with the specified name
    * **Compiler Token**: `FUNC_LAYER_DELETE`
    ``` C++
    syntax:
        void LayerDeleteLayer(string layerName);
    ```

* **LayerDeleteContent**: delete all the objects linked to specified layer.
    * **Compiler Token**: `FUNC_LAYER_DELETE_CONTENT`
    ``` C++
    syntax:
        void LayerDeleteContent(string layerName);
    ```

* **LayerSetMessages**: set specific flags on a layer
    * **Compiler Token**: `FUNC_LAYER_SET_MESSAGES`
    ``` C++
    syntax:
        void LayerSetMessages(string layerName, bool isEnabled); 
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

* **SetEventHandler**
    * **Compiler Token**: `FUNC_SET_EVENT_HANDLER`
    ``` C++
    syntax:
        void SetEventHandler(string eventName, string functionName, int post);
    ```
    
* **ExitProgram**
    * **Compiler Token**: `FUNC_EXIT_PROGRAM`
    ``` C++
    syntax:
        void ExitProgram(); // quit to desktop
    ```

* **GetEventData**
    * **Compiler Token**: `FUNC_GET_EVENTDATA`
    ``` C++
    syntax:
        0, GetEventData(); UNKNOWN,

    // Code send entity id "i" to script and it comes as aref variable into script (GetEventData())
    ```

* **Stop**
    * **Compiler Token**: `FUNC_STOP`
    ``` C++
    syntax:
        void Stop(); // stop executing (thread like)
    ```

* **SendMessage**
    * **Compiler Token**: `FUNC_SEND_MESSAGE`
    ``` C++
    syntax:
        void SendMessage(object to, string formatStrinf,...); // Send a message to an object
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

* **abs**
    * **Compiler Token**: `FUNC_ABS`
    ``` C++
    syntax:
        int abs(int value);
        float abs(float value);
    ```

* **sqrt**
    * **Compiler Token**: `FUNC_SQRT`
    ``` C++
    syntax:
        float sqrt(int value);
        float sqrt(float value);
    ```

* **sqr**
    * **Compiler Token**: `FUNC_SQR`
    ``` C++
    syntax:
        float sqr(int value);
        float sqr(float value);
    ```

* **sin**
    * **Compiler Token**: `FUNC_SIN`
    ``` C++
    syntax:
        float sin(int value);
        float sin(float value);
    ```

* **cos**
    * **Compiler Token**: `FUNC_COS`
    ``` C++
    syntax:
        float cos(int value);
        float cos(float value);
    ```

* **tan**
    * **Compiler Token**: `FUNC_TAN`
    ``` C++
    syntax:
        float tan(int value);
        float tan(float value);
    ```

* **atan**
    * **Compiler Token**: `FUNC_ATAN`
    ``` C++
    syntax:
        float atan(int value);
        float atan(float value);
    ```

* **atan2**
    * **Compiler Token**: `FUNC_TAN2`
    ``` C++
    syntax:
        float atan2(int value);
        float atan2(float value);
    ```

* **asin**
    * **Compiler Token**: `FUNC_ASIN`
    ``` C++
    syntax:
        float asin(int value);
        float asin(float value);
    ```

* **acos**
    * **Compiler Token**: `FUNC_ACOS`
    ``` C++
    syntax:
        float acos(int value);
        float acos(float value);
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

* **DelEventHandler**
    * **Compiler Token**: `FUNC_DEL_EVENT_HANDLER`
    ``` C++
    syntax:
        void DelEventHandler(string eventName, string functionName);
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

* **ClearEvents**
    * **Compiler Token**: `FUNC_CLEAR_EVENTS`
    ``` C++
    syntax:
        void ClearEvents(); 
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

* **Event**
    * **Compiler Token**: `FUNC_EVENT`
    ``` C++
    syntax:
        void Event(); 
    ```

* **PostEvent**
    * **Compiler Token**: `FUNC_POSTEVENT`
    ``` C++
    syntax:
        void PostEvent(); 
    ```

* **fts**
    * **Compiler Token**: `FUNC_FTS`
    ``` C++
    syntax:
        string fts(float, int); 
    ```

* **ClearPostEvents**
    * **Compiler Token**: `FUNC_CLEAR_POST_EVENTS`
    ``` C++
    syntax:
        void ClearPostEvents();
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

* **Pow**
    **Compiler Token**: `FUNC_POW`
    ``` C++
    syntax:
        float Pow(float base, float exponent);
    example:
        Log_Info("" + pow(10.0, 3.0)); // 10^3, 1000
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

* **EventsBreak**
    * **Compiler Token**: `FUNC_EVENTSBREAK`
    ``` C++
    syntax:
        void EventsBreak(0);
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