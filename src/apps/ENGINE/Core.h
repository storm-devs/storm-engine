#pragma once

#include <process.h>
#include <windows.h>

#include "EntityManager.h"
#include "vapi.h"

#include "compiler.h"
#include "services_list.h"
#include "timer.h"
#include <vector>
//#include "program.h"
#include "achievements.h"
#include "safequeue.h"

#define ENGINE_SCRIPT_VERSION 54128

template <typename T> struct tThrd
{
    typedef uint32_t (__thiscall T::*PMethod)();
    static uint32_t WINAPI Function(PVOID pParam)
    {
        return (((tThrd *)pParam)->pThis->*((tThrd *)pParam)->pMethod)();
    };
    T *pThis;
    PMethod pMethod;
    HANDLE Handle;
};

typedef struct
{
    UINT iMsg;
    WPARAM wParam;
    LPARAM lParam;
} SYSTEM_MESSAGE;

#define SYSTEM_MESSAGE_STACK_SIZE 32

#define INVALID_LAYER_CODE 0xffffffff

typedef struct
{
    uint32_t engine_version;
    uint32_t Atoms_max_orbit;
    uint32_t Atoms_min_free_orbit;
    uint32_t Atoms_number;
    uint32_t Atoms_space;
    // ENTITY_CREATION_TIME Creation_Time;

} CORE_STATE;

class CORE : public VAPI
{
  public:
    CORE();
    ~CORE() = default;

    void InitBase();
    void ReleaseBase();

    void CleanUp();
    void Set_Hwnd(HWND _hwnd)
    {
        App_Hwnd = _hwnd;
    };
    bool Initialize();
    bool LoadCoreState(CORE_STATE cs);
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

    CORE_STATE CoreState{};
    char *State_file_name;

    CRITICAL_SECTION lock;
    void Start_CriticalSection();
    void Leave_CriticalSection();

    TIMER Timer;
    SYSTEM_MESSAGE MessageStack[SYSTEM_MESSAGE_STACK_SIZE]{};
    uint32_t SystemMessagesNum;

    // INPUT * Input;

    COMPILER Compiler;
    // PROGRAM Program;

    void ProcessControls();

    float fTimeScale;
    long nSplitScreenshotGrid;

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
    void Exit() override;
    // return application handle
    HWND GetAppHWND() override;
    HINSTANCE GetAppInstance() override;
    // set time scale; affect on std entity functions DeltaTime parameter
    void SetTimeScale(float _scale) override;
    // write message to system log file
    void Trace(const char *Format, ...) override;

    // return service object pointer;
    void *CreateService(const char *service_name) override;

    ATTRIBUTES *Entity_GetAttributeClass(entid_t id_PTR, const char *name) override;
    char *Entity_GetAttribute(entid_t id_PTR, const char *name) override;
    uint32_t Entity_GetAttributeAsDword(entid_t id_PTR, const char *name, uint32_t def = 0) override;
    FLOAT Entity_GetAttributeAsFloat(entid_t id_PTR, const char *name, FLOAT def = 0) override;
    BOOL Entity_SetAttribute(entid_t id_PTR, const char *name, const char *attribute) override;
    BOOL Entity_SetAttributeUseDword(entid_t id_PTR, const char *name, uint32_t val) override;
    BOOL Entity_SetAttributeUseFloat(entid_t id_PTR, const char *name, FLOAT val) override;
    void Entity_SetAttributePointer(entid_t id_PTR, ATTRIBUTES *pA) override;
    uint32_t Entity_AttributeChanged(entid_t id_PTR, ATTRIBUTES *);
    ATTRIBUTES *Entity_GetAttributePointer(entid_t id_PTR) override;

    // messeges system

    // send message to an object
    uint64_t Send_Message(entid_t Destination, const char *Format, ...) override;

    // save core state
    bool SaveState(const char *file_name) override;
    // force core to load state file at the start of next game loop, return false if no state file
    bool InitiateStateLoading(const char *file_name) override;

    // return current fps
    uint32_t EngineFps() override;
    // set fixed delta time mode, (-1) - off
    void SetDeltaTime(long delta_time) override;
    uint32_t GetDeltaTime() override;
    uint32_t GetRDeltaTime() override;
    //
    VDATA *Event(const char *Event_name, const char *Format, ...) override;
    uint32_t PostEvent(const char *Event_name, uint32_t post_time, const char *Format, ...) override;

    void *GetSaveData(const char *file_name, long &data_size) override;

    bool SetSaveData(const char *file_name, void *data_ptr, long data_size) override;

    uint32_t SetScriptFunction(IFUNCINFO *pFuncInfo) override;

    char *EngineIniFileName() override;

    void *GetScriptVariable(const char *pVariableName, uint32_t *pdwVarIndex) override;

    uint32_t Process();
    void StartEvent(uint32_t function_code);
    void StartThread();
    void ReleaseThread();

    // steam section
    CSteamStatsAchievements *g_SteamAchievements;

    bool isSteamEnabled();
    void InitAchievements();
    void DeleteAchievements();
    uint32_t SetAchievementState(const char *ID);
    uint32_t GetAchievementState(const char *ID);
    uint32_t SetStatValue(const char *ID, uint32_t Value);
    uint32_t GetStatValue(const char *ID);
    uint32_t StoreStats();
    bool isSteamConnected();
    bool ResetStats(bool bAchievementsToo);
    bool ClearAchievement(const char *ID);

    CSteamDLC *g_SteamDLC;

    void InitSteamDLC();
    void DeleteSteamDLC();
    bool isDLCActive(uint32_t nDLC);
    uint32_t getDLCCount();
    uint32_t getDLCDataByIndex(uint32_t iDLC);
    bool activateGameOverlayDLC(uint32_t nAppId);

  private:
    SafeQueue<uint32_t> thrQueue;
    tThrd<CORE> MyThread;
    HANDLE hEvent;
};