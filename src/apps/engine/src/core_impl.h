#pragma once

#include "core.h"

#include "compiler.h"
#include "services_list.h"
#include "timer.h"

#include <windows.h>

#define ENGINE_SCRIPT_VERSION 54128

class CoreImpl : public Core
{
  public:
    CoreImpl() = default;
    ~CoreImpl() = default;

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

    void ProcessControls();


    void DumpEntitiesInfo();
    void EraseEntities();
    void ClearEvents();
    void *MakeClass(const char *class_name);
    void AppState(bool state);
    uint32_t MakeHashValue(const char *string);
    VMA *FindVMA(const char *class_name);
    VMA *FindVMA(int32_t hash);
    //------------------------------------------------------------------------------------------------
    // API functions : (virtual API)

    // common programm control

    // shutdown core, delete all objects and close programm
    void Exit();
    // return application handle
    void* GetAppHWND() override;
    HINSTANCE GetAppInstance();
    // set time scale; affect on std entity functions DeltaTime parameter
    void SetTimeScale(float _scale) override;
    // write message to system log file
    void Trace(const char *Format, ...) override;

    // return service object pointer;
    void *GetService(const char *service_name) override;

    ATTRIBUTES *Entity_GetAttributeClass(entid_t id_PTR, const char *name) override;
    char *Entity_GetAttribute(entid_t id_PTR, const char *name) override;
    uint32_t Entity_GetAttributeAsDword(entid_t id_PTR, const char *name, uint32_t def = 0) override;
    FLOAT Entity_GetAttributeAsFloat(entid_t id_PTR, const char *name, FLOAT def = 0) override;
    bool Entity_SetAttribute(entid_t id_PTR, const char *name, const char *attribute) override;
    bool Entity_SetAttributeUseDword(entid_t id_PTR, const char *name, uint32_t val) override;
    bool Entity_SetAttributeUseFloat(entid_t id_PTR, const char *name, FLOAT val) override;
    void Entity_SetAttributePointer(entid_t id_PTR, ATTRIBUTES *pA) override;
    uint32_t Entity_AttributeChanged(entid_t id_PTR, ATTRIBUTES *) override;
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
    void SetDeltaTime(int32_t delta_time) override;
    uint32_t GetDeltaTime() override;
    uint32_t GetRDeltaTime() override;
    //
    VDATA *Event(const char *Event_name, const char *Format = nullptr, ...) override;
    uint32_t PostEvent(const char *Event_name, uint32_t post_time, const char *Format, ...) override;

    void *GetSaveData(const char *file_name, int32_t &data_size) override;

    bool SetSaveData(const char *file_name, void *data_ptr, int32_t data_size) override;

    uint32_t SetScriptFunction(IFUNCINFO *pFuncInfo) override;

    const char *EngineIniFileName() override;

    void *GetScriptVariable(const char *pVariableName, uint32_t *pdwVarIndex = nullptr) override;

    [[nodiscard]] storm::ENGINE_VERSION GetTargetEngineVersion() const noexcept override;

    [[nodiscard]] ScreenSize GetScreenSize() const noexcept override;

    void stopFrameProcessing() override;

    void collectCrashInfo() const;

    [[nodiscard]] bool initialized() const
    {
        return Initialized;
    }

    TIMER Timer;

    COMPILER *Compiler;

    bool Exit_flag; // true if the program closing

  private:
    void loadCompatibilitySettings(INIFILE &inifile);

    storm::ENGINE_VERSION targetVersion_ = storm::ENGINE_VERSION::LATEST;

    bool stopFrameProcessing_ = false;

    bool bAppActive{};
    bool Memory_Leak_flag; // true if core detected memory leak
    bool Root_flag;
    bool Initialized; // initialized flag (false at startup or after Reset())
    bool bEngineIniProcessed;
    HWND App_Hwnd;             // application handle
    char gstring[MAX_PATH]{}; // general purpose string
    bool State_loading;
    bool bEnableTimeScale{};

    SERVICES_LIST Services_List; // list for subsequent calls RunStart/RunEnd service functions

    HINSTANCE hInstance{};

    char *State_file_name;

    float fTimeScale;
};

inline CoreImpl core_internal;