#pragma once

#include "core_private.h"

#include "compiler.h"
#include "entity_manager.h"
#include "services_list.h"
#include "timer.h"
#include "vma.hpp"

#define ENGINE_SCRIPT_VERSION 54128

class CoreImpl final : public CorePrivate
{
  public:
    void Init();

    void InitBase();
    void ReleaseBase();

    void CleanUp();

    void SetWindow(std::shared_ptr<storm::OSWindow> window) override;
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
    storm::OSWindow *GetWindow() override;
#ifdef _WIN32 // HINSTANCE
    HINSTANCE GetAppInstance();
#endif
    // set time scale; affect on std entity functions DeltaTime parameter
    void SetTimeScale(float _scale) override;
    // write message to system log file
    void Trace(const char *Format, ...) override;

    // return service object pointer;
    void *GetService(const char *service_name) override;

    ATTRIBUTES *Entity_GetAttributeClass(entid_t id_PTR, const char *name) override;
    const char *Entity_GetAttribute(entid_t id_PTR, const char *name) override;
    uint32_t Entity_GetAttributeAsDword(entid_t id_PTR, const char *name, uint32_t def = 0) override;
    float Entity_GetAttributeAsFloat(entid_t id_PTR, const char *name, float def = 0) override;
    bool Entity_SetAttribute(entid_t id_PTR, const char *name, const char *attribute) override;
    bool Entity_SetAttributeUseDword(entid_t id_PTR, const char *name, uint32_t val) override;
    bool Entity_SetAttributeUseFloat(entid_t id_PTR, const char *name, float val) override;
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
    VDATA *Event(const std::string_view &event_name) override;
    VDATA *Event(const std::string_view &event_name, MESSAGE& message) override;
    uint32_t PostEvent(const char *Event_name, uint32_t post_time, const char *Format, ...) override;

    void *GetSaveData(const char *file_name, int32_t &data_size) override;

    bool SetSaveData(const char *file_name, void *data_ptr, int32_t data_size) override;

    uint32_t SetScriptFunction(IFUNCINFO *pFuncInfo) override;

    const char *EngineIniFileName() override;

    void *GetScriptVariable(const char *pVariableName, uint32_t *pdwVarIndex = nullptr) override;

    [[nodiscard]] storm::ENGINE_VERSION GetTargetEngineVersion() const noexcept override;

    [[nodiscard]] ScreenSize GetScreenSize() const noexcept override;

    void stopFrameProcessing() override;

    // Entity management
    void AddToLayer(layer_index_t index, entid_t id, priority_t priority) override;
    void EraseEntity(entid_t entity) override;
    entid_t CreateEntity(const char *name, ATTRIBUTES *attr) override;
    entptr_t GetEntityPointer(entid_t id) const override;
    entptr_t GetEntityPointerSafe(entid_t id) const override;
    entid_t GetEntityId(const char *name) const override;
    bool IsEntityValid(entid_t id) const override;
    entity_container_cref GetEntityIds(layer_type_t type) const override;
    entity_container_cref GetEntityIds(layer_index_t index) const override;
    entity_container_cref GetEntityIds(const char *name) const override;
    void SetLayerType(layer_index_t index, layer_type_t type) override;
    void SetLayerFrozen(layer_index_t index, bool freeze) override;
    void RemoveFromLayer(layer_index_t index, entid_t id) override;
    hash_t GetClassCode(entid_t id) const override;
    bool IsLayerFrozen(layer_index_t index) const override;
    void ForEachEntity(const std::function<void(entptr_t)>& f) override;

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

    EntityManager entity_manager_;

    storm::ENGINE_VERSION targetVersion_ = storm::ENGINE_VERSION::LATEST;

    bool stopFrameProcessing_ = false;

    bool bAppActive{};
    bool Memory_Leak_flag; // true if core detected memory leak
    bool Root_flag;
    bool Initialized; // initialized flag (false at startup or after Reset())
    bool bEngineIniProcessed;
    std::shared_ptr<storm::OSWindow> window_; // application handle
    char gstring[MAX_PATH]{}; // general purpose string
    bool State_loading;
    bool bEnableTimeScale{};

    SERVICES_LIST Services_List; // list for subsequent calls RunStart/RunEnd service functions

#ifdef _WIN32 // HINSTANCE
    HINSTANCE hInstance{};
#endif

    char *State_file_name;

    float fTimeScale;
};

inline CoreImpl core_internal;
