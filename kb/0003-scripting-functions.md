# Scripting Functions
_[back to Index](../index.md)_

![Storm Engine Logo](../media/SE_logo.png)

**Related articles**: [Scripting Language Overview](0002-scripting-overview.md)

Here you will find the list of the built-in and some game-level useful functions for the game scripting.


## Built-In Functions

These are the functions that are part of the compiler API. 

``` C++
syntax:
    int Rand(int range);
```
**Compiler Token**: `FUNC_RAND`

``` C++
syntax:
    float frnd(); // float from [0,..,1.0];
```
**Compiler Token**: `FUNC_FRAND`

``` C++
syntax:
    object CreateClass(string className); // bind entity to object (object attributes will be cleared)
```
**Compiler Token**: `FUNC_CREATE_CLASS`

``` C++
syntax:
    int CreateEntity(object& objectReference, string className); // bind entity to object, return 0 if failed
```
**Compiler Token**: `FUNC_CREATE_Entity`

``` C++
syntax:
    void DeleteClass(object obj);
```
**Compiler Token**: `FUNC_DELETE_Entity`

``` C++
syntax:
    void SetEventHandler(string eventName, string functionName, int post);
```
**Compiler Token**: `FUNC_SET_EVENT_HANDLER`

``` C++
syntax:
    void ExitProgram(); // quit to desktop
```
**Compiler Token**: `FUNC_EXIT_PROGRAM`

``` C++
syntax:
    0, GetEventData(); UNKNOWN,

// Code send entity id "i" to script and it comes as aref variable into script (GetEventData())
```
**Compiler Token**: `FUNC_GET_EVENTDATA`

``` C++
syntax:
    void Stop(); // stop executing (thread like)
```
**Compiler Token**: `FUNC_STOP`

``` C++
syntax:
    void SendMessage(object to, string formatStrinf,...); // Send a message to an object
```
**Compiler Token**: `FUNC_SEND_MESSAGE`

``` C++
syntax:
    int LoadSegment(string fileName); // load program into current space, return zero if failed
```
**Compiler Token**: `FUNC_LOAD_SEGMENT`

``` C++
syntax:
    void UnloadSegment(string fileName); // unload segment (delayed)
```
**Compiler Token**: `FUNC_UNLOAD_SEGMENT`

``` C++
syntax:
    void Trace(float message);
void Trace(float message);
void Trace(int message);
 // send a trace to system.log
```
**Compiler Token**: `FUNC_TRACE`

``` C++
syntax:
    int MakeInt(string value);
int MakeInt(float value);
// convert to int
```
**Compiler Token**: `FUNC_MAKE_INT`

``` C++
syntax:
    float MakeFloat(string value);
float MakeFloat(int value);
```
**Compiler Token**: `FUNC_MAKE_FLOAT`

``` C++
syntax:
    void LayerDeleteContent(string layerName);
// delete classes linked to this layer
```
**Compiler Token**: `FUNC_LAYER_DELETE_CONTENT`

``` C++
syntax:
    void LayerSetRealize(string layerName);
```
**Compiler Token**: `FUNC_LAYER_SET_REALIZE`

``` C++
syntax:
    void LayerSetExecute(string layerName);
```
**Compiler Token**: `FUNC_LAYER_SET_EXECUTE`

``` C++
syntax:
    void LayerSetMessages(string layerName, bool isEnabled); 
```
**Compiler Token**: `FUNC_LAYER_SET_MESSAGES`

``` C++
syntax:
    void LayerAddObject(string name, object obj, int level);
```
**Compiler Token**: `FUNC_LAYER_ADDOBJECT`

``` C++
syntax:
    void LayerDelObject(string name, object obj);
```
**Compiler Token**: `FUNC_LAYER_DELOBJECT`

``` C++
syntax:
    void LayerFreeze(string name, bool isEnabled); 
```
**Compiler Token**: `FUNC_LAYER_FREEZE`

``` C++
syntax:
    int abs(int value);
float abs(float value);
```
**Compiler Token**: `FUNC_ABS`

``` C++
syntax:
    float sqrt(int value);
float sqrt(float value);
```
**Compiler Token**: `FUNC_SQRT`

``` C++
syntax:
    float sqr(int value);
float sqr(float value);
```
**Compiler Token**: `FUNC_SQR`

``` C++
syntax:
    float sin(int value);
float sin(float value);
```
**Compiler Token**: `FUNC_SIN`

``` C++
syntax:
    float cos(int value);
float cos(float value);
```
**Compiler Token**: `FUNC_COS`

``` C++
syntax:
    float tan(int value);
float tan(float value);
```
**Compiler Token**: `FUNC_TAN`

``` C++
syntax:
    float atan(int value);
float atan(float value);
```
**Compiler Token**: `FUNC_ATAN`

``` C++
syntax:
    float atan2(int value);
float atan2(float value);
```
**Compiler Token**: `FUNC_TAN2`

``` C++
syntax:
    float asin(int value);
float asin(float value);
```
**Compiler Token**: `FUNC_ASIN`

``` C++
syntax:
    float acos(int value);
float acos(float value);
```
**Compiler Token**: `FUNC_ACOS`

``` C++
syntax:
    void DeleteAttribute(object obj, string attributeAccessString); 
```
**Compiler Token**: `FUNC_DELETE_ATTRIBUTE`

``` C++
syntax:
    int SegmentIsLoaded(1); // string segmentName?
```
**Compiler Token**: `FUNC_SEGMENT_IS_LOADED`

``` C++
syntax:
    int GetAttributesNum(object obj);
int GetAttributesNum(aref attribute);
```
**Compiler Token**: `FUNC_GET_ATTRIBUTES_NUM`

``` C++
syntax:
    aref GetAttributeN(object obj, int index);
aref GetAttributeN(aref attribute, int index);
```
**Compiler Token**: `FUNC_GET_ATTRIBUTE_BYN`

``` C++
syntax:
    string GetAttributeName(object obj);
string GetAttributeName(aref attribute);
```
**Compiler Token**: `FUNC_GET_ATTRIBUTE_NAME`

``` C++
syntax:
    void DelEventHandler(string eventName, string functionName);
```
**Compiler Token**: `FUNC_DEL_EVENT_HANDLER`

``` C++
syntax:
    void EntityUpdate(bool isEnabled);
```
**Compiler Token**: `FUNC_Entity_UPDATE`

``` C++
syntax:
    int IsEntity(object obj);
```
**Compiler Token**: `FUNC_IS_Entity_LOADED`

``` C++
syntax:
    void DumpAttributes(ref objRef);
```
**Compiler Token**: `FUNC_DUMP_ATTRIBUTES`

``` C++
syntax:
    int sti(string value);
```
**Compiler Token**: `FUNC_STI`

``` C++
syntax:
    float stf(string value);
```
**Compiler Token**: `FUNC_STF`

``` C++
syntax:
    bool CheckAttribute(object &obj, string attribute);
```
**Compiler Token**: `FUNC_CHECK_ATTRIBUTE`

``` C++
syntax:
    int argb(int a, int r, int g, int b);
```
**Compiler Token**: `FUNC_ARGB`

``` C++
syntax:
    void DeleteEntities();
```
**Compiler Token**: `FUNC_DELETE_ENTITIES`

``` C++
syntax:
    void ClearEvents(); 
**Compiler Token**: `FUNC_CLEAR_EVENTS`
```

``` C++
syntax:
    void SaveEngineState(1);
**Compiler Token**: `FUNC_SAVEENGINESTATE`
```

``` C++
syntax:
    void LoadEngineState(); /1/?
**Compiler Token**: `FUNC_LOADENGINESTATE`
```

``` C++
syntax:
    void Event(); 
**Compiler Token**: `FUNC_EVENT`
```

``` C++
syntax:
    void PostEvent(); 
**Compiler Token**: `FUNC_POSTEVENT`
```

``` C++
syntax:
    string fts(float, int); 
```
**Compiler Token**: `FUNC_FTS`

``` C++
syntax:
    void ClearPostEvents();
```
**Compiler Token**: `FUNC_CLEAR_POST_EVENTS`

``` C++
syntax:
    void SetArraySize(2); 
```
**Compiler Token**: `FUNC_SET_ARRAY_SIZE`

``` C++
syntax:
    string GetAttributeValue(aref attribute);
```
**Compiler Token**: `FUNC_GET_ATTRIBUTE_VALUE`

``` C++
syntax:
    string Vartype(aref attribute);

/* TODO(yakvi): actually confirm it's an aref input, from the source looks like it 
   Unfortunately, I'm not yet well versed in reading this code. Isn't 
   `pV->IsReference()` can only refer to an aref or any value at all?

    ================
    pV = SStack.Pop();
    if (!pV) { ... }
    pV = pV->GetReference();
    if (!pV)
    {
        SetError(INVALID_FA); // "Invalid function argument"
        break;
    }
    if (pV->IsReference())
        strcpy_s(sVarName, "ref:");
    else
        sVarName[0] = 0;
    pV = pV->GetVarPointer();
    switch (pV->GetType())
    {
        // print out return type
    }
    pV = SStack.Push();
    pV->Set(sVarName);
    pVResult = pV;
    return pV; // return to user
    ================

    Will need to go in and compare with others that I know are arefs.
*/
```
**Compiler Token**: `FUNC_VARTYPE`

``` C++
syntax:
    void Breakpoint();
**Compiler Token**: `FUNC_BREAKPOINT`
```

``` C++
syntax:
    float Pow(float exponent, float base);
// TODO(yakvi): base/exponent inverted??? to verify
**Compiler Token**: `FUNC_POW`
```

``` C++
syntax:
    void CopyAttributes(2);
```
**Compiler Token**: `FUNC_COPYATTRIBUTES`

``` C++
syntax:
    string strcut(3);
```
**Compiler Token**: `FUNC_STRCUT`

``` C++
syntax:
    string findSubStr(3)
```
**Compiler Token**: `FUNC_FINDSUBSTR`

``` C++
syntax:
    void ClearRef(1);
```
**Compiler Token**: `FUNC_CLEARREF`

``` C++
syntax:
    int strlen(1); 
```
**Compiler Token**: `FUNC_STRLEN`

``` C++
syntax:
    int GetDeltaTime(0); 
```
**Compiler Token**: `FUNC_GETDELTATIME`

``` C++
syntax:
    void EventsBreak(0);
```
**Compiler Token**: `FUNC_EVENTSBREAK`

``` C++
syntax:
    int shl(2); 
```
**Compiler Token**: `FUNC_SHL`

``` C++
syntax:
    int shr(2); 
```
**Compiler Token**: `FUNC_SHR`

``` C++
syntax:
    int and(2); 
```
**Compiler Token**: `FUNC_AND`

``` C++
syntax:
    int or(2); 
```
**Compiler Token**: `FUNC_OR`

``` C++
syntax:
    void DeleteEntitiesByType(1);
```
**Compiler Token**: `FUNC_DELETEENTITIESBYTYPE`

``` C++
syntax:
    int CreateControl(1); 
```
**Compiler Token**: `FUNC_CREATE_CONTROL`

``` C++
syntax:
    void DeleteControl(1);
```
**Compiler Token**: `FUNC_DELETE_CONTROL`

``` C++
syntax:
    void MapControl(2);
```
**Compiler Token**: `FUNC_MAP_CONTROL`

``` C++
syntax:
    void SetControlFlags(2);
```
**Compiler Token**: `FUNC_SET_CONTROL_FLAGS`

``` C++
syntax:
    void ClearEntityAP(1);
```
**Compiler Token**: `FUNC_CLEAR_Entity_AP`

``` C++
syntax:
    int GetArraySize(1); 
```
**Compiler Token**: `FUNC_GET_ARRAY_SIZE`

``` C++
syntax:
    0, GetTargetPlatform(); VAR_STRING, 
```
**Compiler Token**: `FUNC_GETTARGETPLATFORM`

``` C++
syntax:
    int GetEntity(2); 
```
**Compiler Token**: `FUNC_GETENTITY`

``` C++
syntax:
    int FindEntity(2); 
```
**Compiler Token**: `FUNC_FINDENTITY`

``` C++
syntax:
    int FindEntityNext(1); 
```
**Compiler Token**: `FUNC_FINDENTITYNEXT`

``` C++
syntax:
    2, GetSymbol(); VAR_STRING, 
```
**Compiler Token**: `FUNC_GETSYMBOL`

``` C++
syntax:
    int IsDigit(2); 
```
**Compiler Token**: `FUNC_ISDIGIT`

``` C++
syntax:
    int SaveVariable(2); 
```
**Compiler Token**: `FUNC_SAVEVARIABLE`

``` C++
syntax:
    int LoadVariable(2); 
```
**Compiler Token**: `FUNC_LOADVARIABLE`

``` C++
syntax:
    void SetControlTreshold(2);
```
**Compiler Token**: `FUNC_SET_CONTROL_TRESHOLD`

``` C++
syntax:
    void LockControl(2);
```
**Compiler Token**: `FUNC_LOCK_CONTROL`

``` C++
syntax:
    int TestRef(1); 
```
**Compiler Token**: `FUNC_TEST_REF`

``` C++
syntax:
    void SetTimeScale(1);
```
**Compiler Token**: `FUNC_SETTIMESCALE`
### General

**GetEngineVersion**: returns `ENGINE_SCRIPT_VERSION` constant (defined in `core.h`). 
`pVResult` is **set** to the same value, so retrieving and using the engine version in the scripting language is not necessary.
``` C++
syntax:
    int GetEngineVersion();
```

**Compiler Token**: `FUNC_GETENGINEVERSION`

``` C++
syntax:
    int CheckFunction(string value);
```
**Compiler Token**: `FUNC_CHECKFUNCTION`



---

| [Home](../index.md) | [Site Map](../site-map.md) | 