#pragma once

// common includes
#include "message.h"
#include "entity_manager.h"
#include "controls.h"
#include "engine_version.hpp"
#include "v_data.h"
#include "v_file_service.h"

struct IFUNCINFO;

struct ScreenSize
{
    size_t width{};
    size_t height{};
};

class Core
{
  public:
    virtual ~Core() = default;

    // return application handle
    virtual void* GetAppHWND() = 0;
    // set time scale; affect on std entity functions DeltaTime parameter
    virtual void SetTimeScale(float _scale) = 0;
    // write message to system log file
    virtual void Trace(const char *Format, ...) = 0;
    // return service object pointer;
    virtual void *GetService(const char *service_name) = 0;

    virtual ATTRIBUTES *Entity_GetAttributeClass(entid_t id_PTR, const char *name) = 0;
    virtual char *Entity_GetAttribute(entid_t id_PTR, const char *name) = 0;
    virtual uint32_t Entity_GetAttributeAsDword(entid_t id_PTR, const char *name, uint32_t def = 0) = 0;
    virtual FLOAT Entity_GetAttributeAsFloat(entid_t id_PTR, const char *name, FLOAT def = 0) = 0;
    virtual bool Entity_SetAttribute(entid_t id_PTR, const char *name, const char *attribute) = 0;
    virtual bool Entity_SetAttributeUseDword(entid_t id_PTR, const char *name, uint32_t val) = 0;
    virtual bool Entity_SetAttributeUseFloat(entid_t id_PTR, const char *name, FLOAT val) = 0;
    virtual void Entity_SetAttributePointer(entid_t id_PTR, ATTRIBUTES *pA) = 0;
    virtual uint32_t Entity_AttributeChanged(entid_t id_PTR, ATTRIBUTES *) = 0;
    virtual ATTRIBUTES *Entity_GetAttributePointer(entid_t id_PTR) = 0;

    // send message to an object
    virtual uint64_t Send_Message(entid_t Destination, const char *Format, ...) = 0;

    // save core state
    virtual bool SaveState(const char *file_name) = 0;
    // force core to load state file at the start of next game loop, return false if no state file
    virtual bool InitiateStateLoading(const char *file_name) = 0;

    // return current fps
    virtual uint32_t EngineFps() = 0;
    // set fixed delta time mode, (-1) - off
    virtual void SetDeltaTime(int32_t delta_time) = 0;
    virtual uint32_t GetDeltaTime() = 0;
    virtual uint32_t GetRDeltaTime() = 0;
    //
    virtual VDATA *Event(const char *Event_name, const char *Format = nullptr, ...) = 0;
    virtual uint32_t PostEvent(const char *Event_name, uint32_t post_time, const char *Format, ...) = 0;

    virtual void *GetSaveData(const char *file_name, int32_t &data_size) = 0;

    virtual bool SetSaveData(const char *file_name, void *data_ptr, int32_t data_size) = 0;

    virtual uint32_t SetScriptFunction(IFUNCINFO *pFuncInfo) = 0;

    virtual const char *EngineIniFileName() = 0;

    virtual void *GetScriptVariable(const char *pVariableName, uint32_t *pdwVarIndex = nullptr) = 0;

    [[nodiscard]] virtual storm::ENGINE_VERSION GetTargetEngineVersion() const noexcept = 0;

    [[nodiscard]] virtual ScreenSize GetScreenSize() const noexcept = 0;

    virtual void stopFrameProcessing() = 0;

    CONTROLS* Controls{};
};

extern Core &core;