# Scripting Functions
_[back to Index](../index.md)_

![Storm Engine Logo](../media/SE_logo.png)

**Related articles**: [Scripting Language Overview](0002-scripting-overview.md)

Here you will find the list of the built-in and some other useful functions for the game scripting.

## Built-In Functions

These are the functions that are part of the compiler API. 

``` C++
int Rand(int range);
```
**Compiler Token**: `FUNC_RAND`

``` C++
float frnd(); // float from [0,..,1.0];
```
**Compiler Token**: `FUNC_FRAND`

``` C++
object CreateClass(string className); // bind entity to object (object attributes will be cleared)
```
**Compiler Token**: `FUNC_CREATE_CLASS`

``` C++
int CreateEntity(object& objectReference, string className); // bind entity to object, return 0 if failed
```
**Compiler Token**: `FUNC_CREATE_Entity`

``` C++
void DeleteClass(object obj);
```
**Compiler Token**: `FUNC_DELETE_Entity`

``` C++
void SetEventHandler(string eventName, string functionName, int post);
```
**Compiler Token**: `FUNC_SET_EVENT_HANDLER`

``` C++
void ExitProgram(); // quit to desktop
```
**Compiler Token**: `FUNC_EXIT_PROGRAM`

``` C++
0, GetEventData(); UNKNOWN,

// Code send entity id "i" to script and it comes as aref variable into script (GetEventData())
```
**Compiler Token**: `FUNC_GET_EVENTDATA`

``` C++
void Stop(); // stop executing (thread like)
```
**Compiler Token**: `FUNC_STOP`

``` C++
void SendMessage(object to, string formatStrinf,...); // Send a message to an object
```
**Compiler Token**: `FUNC_SEND_MESSAGE`

``` C++
int LoadSegment(string fileName); // load program into current space, return zero if failed
```
**Compiler Token**: `FUNC_LOAD_SEGMENT`

``` C++
void UnloadSegment(string fileName); // unload segment (delayed)
```
**Compiler Token**: `FUNC_UNLOAD_SEGMENT`

``` C++
void Trace(float message);
void Trace(float message);
void Trace(int message);
 // send a trace to system.log
```
**Compiler Token**: `FUNC_TRACE`

``` C++
int MakeInt(string value);
int MakeInt(float value);
// convert to int
```
**Compiler Token**: `FUNC_MAKE_INT`

``` C++
float MakeFloat(string value);
float MakeFloat(int value);
```
**Compiler Token**: `FUNC_MAKE_FLOAT`

``` C++
void LayerDeleteContent(string layerName);
// delete classes linked to this layer
```
**Compiler Token**: `FUNC_LAYER_DELETE_CONTENT`

``` C++
void LayerSetRealize(string layerName);
```
**Compiler Token**: `FUNC_LAYER_SET_REALIZE`

``` C++
void LayerSetExecute(string layerName);
```
**Compiler Token**: `FUNC_LAYER_SET_EXECUTE`

``` C++
void LayerSetMessages(string layerName, bool isEnabled); 
```
**Compiler Token**: `FUNC_LAYER_SET_MESSAGES`

``` C++
void LayerAddObject(string name, object obj, int level);
```
**Compiler Token**: `FUNC_LAYER_ADDOBJECT`

``` C++
void LayerDelObject(string name, object obj);
```
**Compiler Token**: `FUNC_LAYER_DELOBJECT`

``` C++
void LayerFreeze(string name, bool isEnabled); 
```
**Compiler Token**: `FUNC_LAYER_FREEZE`

``` C++
int abs(int value);
float abs(float value);
```
**Compiler Token**: `FUNC_ABS`

``` C++
float sqrt(int value);
float sqrt(float value);
```
**Compiler Token**: `FUNC_SQRT`

``` C++
float sqr(int value);
float sqr(float value);
```
**Compiler Token**: `FUNC_SQR`

``` C++
float sin(int value);
float sin(float value);
```
**Compiler Token**: `FUNC_SIN`

``` C++
float cos(int value);
float cos(float value);
```
**Compiler Token**: `FUNC_COS`

``` C++
float tan(int value);
float tan(float value);
```
**Compiler Token**: `FUNC_TAN`

``` C++
float atan(int value);
float atan(float value);
```
**Compiler Token**: `FUNC_ATAN`

``` C++
float atan2(int value);
float atan2(float value);
```
**Compiler Token**: `FUNC_TAN2`

``` C++
float asin(int value);
float asin(float value);
```
**Compiler Token**: `FUNC_ASIN`

``` C++
float acos(int value);
float acos(float value);
```
**Compiler Token**: `FUNC_ACOS`

``` C++
void DeleteAttribute(object obj, string attributeAccessString); 
```
**Compiler Token**: `FUNC_DELETE_ATTRIBUTE`

``` C++
int SegmentIsLoaded(1); // string segmentName?
```
**Compiler Token**: `FUNC_SEGMENT_IS_LOADED`

``` C++
int GetAttributesNum(object obj);
int GetAttributesNum(aref attribute);
```
**Compiler Token**: `FUNC_GET_ATTRIBUTES_NUM`

``` C++
aref GetAttributeN(object obj, int index);
aref GetAttributeN(aref attribute, int index);
```
**Compiler Token**: `FUNC_GET_ATTRIBUTE_BYN`

``` C++
string GetAttributeName(object obj);
string GetAttributeName(aref attribute);
```
**Compiler Token**: `FUNC_GET_ATTRIBUTE_NAME`

``` C++
void DelEventHandler(string eventName, string functionName);
```
**Compiler Token**: `FUNC_DEL_EVENT_HANDLER`

``` C++
void EntityUpdate(bool isEnabled);
```
**Compiler Token**: `FUNC_Entity_UPDATE`

``` C++
int IsEntity(object obj);
```
**Compiler Token**: `FUNC_IS_Entity_LOADED`

``` C++
void DumpAttributes(ref objRef);
```
**Compiler Token**: `FUNC_DUMP_ATTRIBUTES`

``` C++
int sti(string value);
```
**Compiler Token**: `FUNC_STI`

``` C++
float stf(string value);
```
**Compiler Token**: `FUNC_STF`

``` C++
bool CheckAttribute(object &obj, string attribute);
```
**Compiler Token**: `FUNC_CHECK_ATTRIBUTE`

``` C++
int argb(int a, int r, int g, int b);
```
**Compiler Token**: `FUNC_ARGB`

``` C++
void DeleteEntities();
```
**Compiler Token**: `FUNC_DELETE_ENTITIES`

``` C++
void ClearEvents(); 
**Compiler Token**: `FUNC_CLEAR_EVENTS`
```

``` C++
void SaveEngineState(1);
**Compiler Token**: `FUNC_SAVEENGINESTATE`
```

``` C++
1, LoadEngineState(); TVOID, //??
**Compiler Token**: `FUNC_LOADENGINESTATE`
```

``` C++
void Event(); 
**Compiler Token**: `FUNC_EVENT`
```

``` C++
void PostEvent(); 
**Compiler Token**: `FUNC_POSTEVENT`
```

``` C++
string fts(float, int); 
```
**Compiler Token**: `FUNC_FTS`

``` C++
void ClearPostEvents();
```
**Compiler Token**: `FUNC_CLEAR_POST_EVENTS`

``` C++
void SetArraySize(2); 
```
**Compiler Token**: `FUNC_SET_ARRAY_SIZE`

``` C++
string GetAttributeValue(aref attribute);
```
**Compiler Token**: `FUNC_GET_ATTRIBUTE_VALUE`

``` C++
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
void Breakpoint();
**Compiler Token**: `FUNC_BREAKPOINT`
```

``` C++
float Pow(float exponent, float base);
// TODO(yakvi): base/exponent inverted??? to verify
**Compiler Token**: `FUNC_POW`
```

``` C++
2, CopyAttributes(); TVOID,
**Compiler Token**: 
```

``` C++
3, strcut(); VAR_STRING, 3, "findSubStr", VAR_STRING, 
**Compiler Token**: 
```

``` C++
1, ClearRef(); TVOID, 
**Compiler Token**: 
```

``` C++
1, strlen(); VAR_INTEGER, 
**Compiler Token**: 
```

``` C++
0, GetDeltaTime(); VAR_INTEGER, 
**Compiler Token**: 
```

``` C++
0, EventsBreak(); TVOID, 
**Compiler Token**: 
```

``` C++
2, shl(); VAR_INTEGER, 
**Compiler Token**: 
```

``` C++
2, shr(); VAR_INTEGER, 
**Compiler Token**: 
```

``` C++
2, and(); VAR_INTEGER, 
**Compiler Token**: 
```

``` C++
2, or(); VAR_INTEGER, 
**Compiler Token**: 
```

``` C++
1, DeleteEntitiesByType(); TVOID, 
**Compiler Token**: 
```

``` C++
1, CreateControl(); VAR_INTEGER, 
**Compiler Token**: 
```

``` C++
1, DeleteControl(); TVOID, 
**Compiler Token**: 
```

``` C++
2, MapControl(); TVOID, 
**Compiler Token**: 
```

``` C++
2, SetControlFlags(); TVOID, 
**Compiler Token**: 
```

``` C++
1, ClearEntityAP(); TVOID, 
**Compiler Token**: 
```

``` C++
1, GetArraySize(); VAR_INTEGER, 
**Compiler Token**: 
```

``` C++
0, GetTargetPlatform(); VAR_STRING, 
**Compiler Token**: 
```

``` C++
2, GetEntity(); VAR_INTEGER, 
**Compiler Token**: 
```

``` C++
2, FindEntity(); VAR_INTEGER, 
**Compiler Token**: 
```

``` C++
1, FindEntityNext(); VAR_INTEGER, 
**Compiler Token**: 
```

``` C++
2, GetSymbol(); VAR_STRING, 
**Compiler Token**: 
```

``` C++
2, IsDigit(); VAR_INTEGER, 
**Compiler Token**: 
```

``` C++
2, SaveVariable(); VAR_INTEGER, 
**Compiler Token**: 
```

``` C++
2, LoadVariable(); VAR_INTEGER, 
**Compiler Token**: 
```

``` C++
2, SetControlTreshold(); TVOID, 
**Compiler Token**: 
```

``` C++
2, LockControl(); TVOID, 
**Compiler Token**: 
```

``` C++
1, TestRef(); VAR_INTEGER, 
**Compiler Token**: 
```

``` C++
1, SetTimeScale(); TVOID, 
**Compiler Token**: 
```

``` C++
1, CheckFunction(); VAR_INTEGER, 
**Compiler Token**: 
```

``` C++
0, GetEngineVersion(); //???
**Compiler Token**: 

```



---

| [Home](../index.md) | [Site Map](../site-map.md) | 