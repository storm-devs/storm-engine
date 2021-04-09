#pragma once

#include <windows.h>
// common includes
#include "EntityManager.h"
#include "controls.h"
#include "message.h"
#include "vdata.h"
#include "vfile_service.h"

#include "../apps/engine/services_list.h"
#include "../apps/engine/timer.h"
//#include "achievements.h"
#include "safequeue.h"

#include <queue>

#define ENGINE_SCRIPT_VERSION 54128

class COMPILER;
struct IFUNCINFO;
class VDATA;
class CSteamStatsAchievements;
class CSteamDLC;

class CORE
{
  public:
    CONTROLS *Controls;

    std::shared_ptr<spdlog::logger> tracelog;

    CORE() = default;
    ~CORE() = default;

    void Init();

    void InitBase();
    void ReleaseBase();

    void CleanUp();
    void Set_Hwnd(HWND _hwnd)
    {
        App_Hwnd = _hwnd;
    };
    bool Initialize();
    void ResetCore();
    bool Run();
    bool LoadClassesTable();

    void ProcessExecute();
    void ProcessRealize();
    void ProcessStateLoading();
    void ProcessRunStart(uint32_t section_code);
    void ProcessRunEnd(uint32_t section_code);

    void CheckAutoExceptions(uint32_t xflag) const;
    void ReleaseServices();
    void ProcessEngineIniFile();

    bool bAppActive{};
    bool Memory_Leak_flag; // true if core detected memory leak
    bool Root_flag;
    bool Exit_flag;   // true if the program closing
    bool Initialized; // initialized flag (false at startup or after Reset())
    bool bEngineIniProcessed;
    HWND App_Hwnd;             // application handle
    char gstring[_MAX_PATH]{}; // general purpose string
    bool State_loading;
    bool bEnableTimeScale{};

    SERVICES_LIST Services_List; // list for subsequent calls RunStart/RunEnd service functions

    HINSTANCE hInstance{};

    char *State_file_name;

    CRITICAL_SECTION lock;
    void Start_CriticalSection();
    void Leave_CriticalSection();

    TIMER Timer;

    COMPILER *Compiler;

    void ProcessControls();

    float fTimeScale;

    void DumpEntitiesInfo();
    void EraseEntities();
    void ClearEvents();
    void *MakeClass(const char *class_name);
    void AppState(bool state);
    uint32_t MakeHashValue(const char *string);
    VMA *FindVMA(const char *class_name);
    VMA *FindVMA(long hash);
    //------------------------------------------------------------------------------------------------
    // API functions : (virtual API)

    // common programm control

    // shutdown core, delete all objects and close programm
    void Exit();
    // return application handle
    HWND GetAppHWND();
    HINSTANCE GetAppInstance();
    // set time scale; affect on std entity functions DeltaTime parameter
    void SetTimeScale(float _scale);
    // write message to system log file
    void Trace(const char *Format, ...);

    // return service object pointer;
    void *CreateService(const char *service_name);

    ATTRIBUTES *Entity_GetAttributeClass(entid_t id_PTR, const char *name);
    char *Entity_GetAttribute(entid_t id_PTR, const char *name);
    uint32_t Entity_GetAttributeAsDword(entid_t id_PTR, const char *name, uint32_t def = 0);
    FLOAT Entity_GetAttributeAsFloat(entid_t id_PTR, const char *name, FLOAT def = 0);
    bool Entity_SetAttribute(entid_t id_PTR, const char *name, const char *attribute);
    bool Entity_SetAttributeUseDword(entid_t id_PTR, const char *name, uint32_t val);
    bool Entity_SetAttributeUseFloat(entid_t id_PTR, const char *name, FLOAT val);
    void Entity_SetAttributePointer(entid_t id_PTR, ATTRIBUTES *pA);
    uint32_t Entity_AttributeChanged(entid_t id_PTR, ATTRIBUTES *);
    ATTRIBUTES *Entity_GetAttributePointer(entid_t id_PTR);

    // messeges system

    // send message to an object
    uint64_t Send_Message(entid_t Destination, const char *Format, ...);

    // save core state
    bool SaveState(const char *file_name);
    // force core to load state file at the start of next game loop, return false if no state file
    bool InitiateStateLoading(const char *file_name);

    // return current fps
    uint32_t EngineFps();
    // set fixed delta time mode, (-1) - off
    void SetDeltaTime(long delta_time);
    uint32_t GetDeltaTime();
    uint32_t GetRDeltaTime();
    //
    VDATA *Event(const char *Event_name, const char *Format = nullptr, ...);
    uint32_t PostEvent(const char *Event_name, uint32_t post_time, const char *Format, ...);

    void *GetSaveData(const char *file_name, long &data_size);

    bool SetSaveData(const char *file_name, void *data_ptr, long data_size);

    uint32_t SetScriptFunction(IFUNCINFO *pFuncInfo);

    const char *EngineIniFileName();

    void *GetScriptVariable(const char *pVariableName, uint32_t *pdwVarIndex = nullptr);
};
// core instance
extern CORE core;