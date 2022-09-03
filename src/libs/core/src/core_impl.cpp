#include "core_impl.h"

#include "compiler.h"
#include "controls.h"
#include "fs.h"
#include "steam_api.hpp"

#include <fstream>

#include "string_compare.hpp"
#include <SDL2/SDL.h>

Core& core = core_internal;

uint64_t get_performance_counter()
{
    return SDL_GetPerformanceCounter();
}

namespace storm
{
namespace
{
ENGINE_VERSION getTargetEngineVersion(const std::string_view &version)
{
    using namespace std::string_view_literals;

    if (iEquals(version, "sd"sv))
    {
        return ENGINE_VERSION::SEA_DOGS;
    }
    else if (iEquals(version, "potc"sv))
    {
        return ENGINE_VERSION::PIRATES_OF_THE_CARIBBEAN;
    }
    else if (iEquals(version, "ct"sv))
    {
        return ENGINE_VERSION::CARIBBEAN_TALES;
    }
    else if (iEquals(version, "coas"sv))
    {
        return ENGINE_VERSION::CITY_OF_ABANDONED_SHIPS;
    }
    else if (iEquals(version, "teho"sv))
    {
        return ENGINE_VERSION::TO_EACH_HIS_OWN;
    }
    else if (iEquals(version, "latest"sv))
    {
        return ENGINE_VERSION::LATEST;
    }

    return ENGINE_VERSION::UNKNOWN;
}
} // namespace
} // namespace storm

uint32_t dwNumberScriptCommandsExecuted = 0;

typedef struct
{
    uint32_t code;
    void *pointer;
} CODE_AND_POINTER;

void CoreImpl::ResetCore()
{
    Initialized = false;
    bEngineIniProcessed = false;

    ReleaseServices();

    Services_List.Release();

    STORM_DELETE(State_file_name);
}

void CoreImpl::CleanUp()
{
    Initialized = false;
    bEngineIniProcessed = false;
    ReleaseServices();
    Compiler->Release();
    Services_List.Release();
    Services_List.Release();
    delete[] State_file_name;
}

void CoreImpl::SetWindow(std::shared_ptr<storm::OSWindow> window)
{
    window_ = std::move(window);
}

void CoreImpl::Init()
{
    Initialized = false;
    bEngineIniProcessed = false;
    State_file_name = nullptr;
    Exit_flag = false;
    State_loading = false;
    Memory_Leak_flag = false;
    Controls = nullptr;
    fTimeScale = 1.0f;
    Compiler = new COMPILER;

    /* TODO: place this outside CoreImpl */
    core.SetLayerType(EXECUTE, layer_type_t::execute);
    core.SetLayerType(REALIZE, layer_type_t::realize);
    core.SetLayerType(SEA_EXECUTE, layer_type_t::execute);
    core.SetLayerType(SEA_REALIZE, layer_type_t::realize);
    core.SetLayerType(INTERFACE_EXECUTE, layer_type_t::execute);
    core.SetLayerType(INTERFACE_REALIZE, layer_type_t::realize);
    core.SetLayerType(FADER_EXECUTE, layer_type_t::execute);
    core.SetLayerType(FADER_REALIZE, layer_type_t::realize);
    core.SetLayerType(LIGHTER_EXECUTE, layer_type_t::execute);
    core.SetLayerType(LIGHTER_REALIZE, layer_type_t::realize);
    core.SetLayerType(VIDEO_EXECUTE, layer_type_t::execute);
    core.SetLayerType(VIDEO_REALIZE, layer_type_t::realize);
    core.SetLayerType(EDITOR_REALIZE, layer_type_t::realize);
    core.SetLayerType(INFO_REALIZE, layer_type_t::realize);
    core.SetLayerType(SOUND_DEBUG_REALIZE, layer_type_t::realize);
}

void CoreImpl::InitBase()
{
    LoadClassesTable();
}

void CoreImpl::ReleaseBase()
{
    Compiler->Token.Release();
}

bool CoreImpl::Run()
{
    stopFrameProcessing_ = false;

    const auto bDebugWindow = true;
    if (bDebugWindow && core_internal.Controls && core_internal.Controls->GetDebugAsyncKeyState(VK_F7) < 0)
        DumpEntitiesInfo();
    dwNumberScriptCommandsExecuted = 0;

    if (Exit_flag)
        return false; // exit

    Timer.Run(); // calc delta time

    auto *pVCTime = static_cast<VDATA *>(core_internal.GetScriptVariable("iRealDeltaTime"));
    if (pVCTime)
        pVCTime->Set(static_cast<int32_t>(GetRDeltaTime()));

    auto tt = std::time(nullptr);
    auto local_tm = *std::localtime(&tt);

    auto *pVYear = static_cast<VDATA *>(core_internal.GetScriptVariable("iRealYear"));
    auto *pVMonth = static_cast<VDATA *>(core_internal.GetScriptVariable("iRealMonth"));
    auto *pVDay = static_cast<VDATA *>(core_internal.GetScriptVariable("iRealDay"));

    if (pVYear)
        pVYear->Set(local_tm.tm_year + 1900);
    if (pVMonth)
        pVMonth->Set(local_tm.tm_mon + 1); // tm_mon belongs [0, 11]
    if (pVDay)
        pVDay->Set(local_tm.tm_mday);

    if (Controls && Controls->GetDebugAsyncKeyState('R') < 0)
        Timer.Delta_Time *= 10;
    if (Controls && Controls->GetDebugAsyncKeyState('Y') < 0)
        Timer.Delta_Time = static_cast<uint32_t>(Timer.Delta_Time * 0.2f);

    Timer.Delta_Time = static_cast<uint32_t>(Timer.Delta_Time * fTimeScale);
    Timer.fDeltaTime *= fTimeScale;

    auto *pVData = static_cast<VDATA *>(GetScriptVariable("fHighPrecisionDeltaTime", nullptr));
    if (pVData)
        pVData->Set(Timer.fDeltaTime * 0.001f);

    if (!Initialized)
    {
        Initialize(); // initialization at start or after reset
    }
    if (!bEngineIniProcessed)
        ProcessEngineIniFile();

    Compiler->ProcessFrame(Timer.GetDeltaTime());
    Compiler->ProcessEvent("frame");

    ProcessStateLoading();

    ProcessRunStart(SECTION_ALL);
    if (stopFrameProcessing_)
    {
        // service asked to skip current frame processing
        return true;
    }

    ProcessExecute(); // transfer control to objects via Execute() function
    ProcessRealize(); // transfer control to objects via Realize() function

    steamapi::SteamApi::getInstance().RunCallbacks();

    if (Controls)
        Controls->Update(Timer.rDelta_Time);

    if (Controls)
        ProcessControls();

    entity_manager_.NewLifecycle();

    ProcessRunEnd(SECTION_ALL);

    return true;
}

void CoreImpl::ProcessControls()
{
    CONTROL_STATE cs;
    USER_CONTROL uc;

    if (!Controls)
        return;

    for (int32_t n = 0; n < Controls->GetControlsNum(); n++)
    {
        Controls->GetControlState(n, cs);
        if (cs.state == CST_ACTIVATED)
        {
            Controls->GetControlDesc(n, uc);
            Core::Event("Control Activation", "s", uc.name);
        }
        else if (cs.state == CST_INACTIVATED)
        {
            Controls->GetControlDesc(n, uc);
            Core::Event("Control Deactivation", "s", uc.name);
        }
    }
}

//-------------------------------------------------------------------------------------------------
// internal functions
//-------------------------------------------------------------------------------------------------
bool CoreImpl::Initialize()
{
    ResetCore();

    Initialized = true;

    return true;
}

void CoreImpl::ProcessEngineIniFile()
{
    char String[MAX_PATH];

    bEngineIniProcessed = true;

    auto engine_ini = fio->OpenIniFile(fs::ENGINE_INI_FILE_NAME);
    if (!engine_ini)
        throw std::runtime_error("no 'engine.ini' file");

    auto res = engine_ini->ReadString(nullptr, "program_directory", String, sizeof(String), "");
    if (res)
    {
        Compiler->SetProgramDirectory(String);
    }

    res = engine_ini->ReadString(nullptr, "controls", String, sizeof(String), "");
    if (res)
    {
        core_internal.Controls = static_cast<CONTROLS *>(MakeClass(String));
        if (core_internal.Controls == nullptr)
            core_internal.Controls = static_cast<CONTROLS *>(MakeClass("controls"));
    }
    else
    {
        delete Controls;
        Controls = nullptr;

        core_internal.Controls = new CONTROLS;
    }

    loadCompatibilitySettings(*engine_ini);

    res = engine_ini->ReadString(nullptr, "run", String, sizeof(String), "");
    if (res)
    {
        if (!Compiler->CreateProgram(String))
            throw std::runtime_error("fail to create program");
        if (!Compiler->Run())
            throw std::runtime_error("fail to run program");

        // Script version test
        if (targetVersion_ >= storm::ENGINE_VERSION::LATEST)
        {
            int32_t iScriptVersion = 0xFFFFFFFF;
            auto *pVScriptVersion = static_cast<VDATA *>(core_internal.GetScriptVariable("iScriptVersion"));
            if (pVScriptVersion)
                pVScriptVersion->Get(iScriptVersion);

            if (iScriptVersion != ENGINE_SCRIPT_VERSION)
            {
#ifdef _WIN32 // FIX_LINUX Cursor
                ShowCursor(true);
#endif
                SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Wrong script version", nullptr);
                Compiler->ExitProgram();
            }
        }
    }
}

bool CoreImpl::LoadClassesTable()
{
    for (auto *c : __STORM_CLASSES_REGISTRY)
    {
        const auto hash = MakeHashValue(c->GetName());
        c->SetHash(hash);
    }

    return true;
}

void CoreImpl::CheckAutoExceptions(uint32_t = 0) const
{
    spdlog::warn("exception thrown");
}

void CoreImpl::Exit()
{
    Exit_flag = true;
}

storm::OSWindow * CoreImpl::GetWindow()
{
    return window_.get();
}

#ifdef _WIN32 // HINSTANCE
HINSTANCE CoreImpl::GetAppInstance()
{
    return hInstance;
}
#endif

void CoreImpl::SetTimeScale(float _scale)
{
    fTimeScale = _scale;
}

//------------------------------------------------------------------------------------------------
// transfer message arguments and program control to entity, specified by Destination id
//
uint64_t CoreImpl::Send_Message(entid_t Destination, const char *Format, ...)
{
    MESSAGE message;
    auto *const ptr = core.GetEntityPointer(Destination); // check for valid destination
    if (!ptr)
        return 0;

    va_list args;
    va_start(args, Format);
    message.ResetVA(Format, args);
    const auto rc = static_cast<Entity *>(ptr)->ProcessMessage(message); // transfer control
    va_end(args);
    return rc;
}

uint32_t CoreImpl::PostEvent(const char *Event_name, uint32_t post_time, const char *Format, ...)
{
    MESSAGE *pMS;
    MESSAGE message;

    entid_t id;

    if (Format != nullptr)
    {
        pMS = new MESSAGE();
        va_list args;
        va_start(args, Format);
        message.ResetVA(Format, args);
        pMS->Reset(Format);

        auto bAction = true;
        while (bAction)
        {
            switch (message.GetCurrentFormatType())
            {
                //-------------------------------------
            case 'l':
                int32_t v;
                v = message.Long();
                pMS->Set(v);
                break;
            case 'f':
                float f;
                f = message.Float();
                pMS->Set(f);
                break;
            case 'i':
                id = message.EntityID();
                pMS->SetEntity(id);
                break;
            case 'e':
                VDATA *e;
                e = message.ScriptVariablePointer();
                pMS->Set(e);
                break;
            case 's': {
                const std::string &s = message.StringPointer();
                pMS->Set(s);
                break;
            }
            case 'a':
                ATTRIBUTES *a;
                a = message.AttributePointer();
                pMS->Set(a);
                break;

                //-------------------------------------
            default:
                bAction = false;
                break;
            }
        }
        va_end(args);
    }
    else
        pMS = nullptr;

    auto *pEM = new S_EVENTMSG(Event_name, pMS, post_time);
    pEM->bProcess = true;
    Compiler->AddPostEvent(pEM);
    return 0;
}

VDATA *CoreImpl::Event(const std::string_view &event_name)
{
    MESSAGE message;
    return Compiler->ProcessEvent(event_name.data(), message);
}

VDATA *CoreImpl::Event(const std::string_view &event_name, MESSAGE& message)
{
    return Compiler->ProcessEvent(event_name.data(), message);
}

void *CoreImpl::MakeClass(const char *class_name)
{
    const int32_t hash = MakeHashValue(class_name);
    for (auto *const c : __STORM_CLASSES_REGISTRY)
        if (c->GetHash() == hash && storm::iEquals(class_name, c->GetName()))
            return c->CreateClass();

    return nullptr;
}

void CoreImpl::ReleaseServices()
{
    for (auto *const c : __STORM_CLASSES_REGISTRY)
        if (c->Service())
            c->Clear();

    Controls = nullptr;
}

VMA *CoreImpl::FindVMA(const char *class_name)
{
    const int32_t hash = MakeHashValue(class_name);
    for (auto *const c : __STORM_CLASSES_REGISTRY)
        if (c->GetHash() == hash && storm::iEquals(class_name, c->GetName()))
            return c;

    return nullptr;
}

VMA *CoreImpl::FindVMA(int32_t hash)
{
    for (auto *const c : __STORM_CLASSES_REGISTRY)
        if (c->GetHash() == hash)
            return c;

    return nullptr;
}

void *CoreImpl::GetService(const char *service_name)
{
    auto *pClass = FindVMA(service_name);
    if (pClass == nullptr)
    {
        CheckAutoExceptions(0);
        return nullptr;
    }

    if (pClass->GetHash() == 0)
    {
        CheckAutoExceptions(0);
        return nullptr;
    }

    if (pClass->GetReference() > 0)
        return pClass->CreateClass();

    auto *service_PTR = static_cast<SERVICE *>(pClass->CreateClass());

    const auto class_code = MakeHashValue(service_name);
    pClass->SetHash(class_code);

    if (!service_PTR->Init())
        CheckAutoExceptions(0);

    Services_List.Add(class_code, class_code, service_PTR);

    return service_PTR;
}

void CoreImpl::Trace(const char *format, ...)
{
    static char buffer_4k[4096];

    va_list args;
    va_start(args, format);
    vsnprintf(buffer_4k, sizeof(buffer_4k) - 4, format, args);
    va_end(args);
    spdlog::info(buffer_4k);
}

//------------------------------------------------------------------------------------------------
// Transfer programm control to objects via Execute() functions
//
void CoreImpl::ProcessExecute()
{
    uint64_t ticks;

    ProcessRunStart(SECTION_EXECUTE);

    const auto deltatime = Timer.GetDeltaTime();
    const auto &entIds = core.GetEntityIds(layer_type_t::execute);
    for (auto id : entIds)
    {
        if (auto *ptr = core.GetEntityPointer(id))
        {
            ptr->ProcessStage(Entity::Stage::execute, deltatime);
        }
    }

    ProcessRunEnd(SECTION_EXECUTE);
}

void CoreImpl::ProcessRealize()
{
    uint64_t ticks;
    ProcessRunStart(SECTION_REALIZE);

    const auto deltatime = Timer.GetDeltaTime();
    const auto &entIds = core.GetEntityIds(layer_type_t::realize);
    for (auto id : entIds)
    {
        if (auto *ptr = core.GetEntityPointer(id))
        {
            ptr->ProcessStage(Entity::Stage::realize, deltatime);
        }
    }

    ProcessRunEnd(SECTION_REALIZE);
}

// save core state
bool CoreImpl::SaveState(const char *file_name)
{
    if (!file_name)
    {
        throw std::logic_error("Bad file name of save");
    }

    auto fileS = fio->_CreateFile(file_name, std::ios::binary | std::ios::out);

    if (!fileS.is_open())
    {
        return false;
    }

    Compiler->SaveState(fileS);
    fio->_CloseFile(fileS);

    return true;
}

// force core to load state file at the start of next game loop, return false if no state file
bool CoreImpl::InitiateStateLoading(const char *file_name)
{
    auto fileS = fio->_CreateFile(file_name, std::ios::binary | std::ios::in);
    if (!fileS.is_open())
    {
        return false;
    }
    fio->_CloseFile(fileS);
    delete[] State_file_name;

    const auto len = strlen(file_name) + 1;
    State_file_name = static_cast<char *>(new char[len]);
    strcpy_s(State_file_name, len, file_name);
    return true;
}

void CoreImpl::ProcessStateLoading()
{
    if (!State_file_name)
    {
        return;
    }

    State_loading = true;
    EraseEntities();

    auto fileS = fio->_CreateFile(State_file_name, std::ios::binary | std::ios::in);
    if (!fileS.is_open())
    {
        return;
    }
    Compiler->LoadState(fileS);
    fio->_CloseFile(fileS);

    delete[] State_file_name;
    State_file_name = nullptr;
    State_loading = false;
}

void CoreImpl::ProcessRunStart(uint32_t section_code)
{
    uint32_t class_code;
    SERVICE *service_PTR = Services_List.GetService(class_code);
    while (service_PTR)
    {
        const uint32_t section = service_PTR->RunSection();
        if (section == section_code)
        {
            service_PTR->RunStart();
        }
        service_PTR = Services_List.GetServiceNext(class_code);
    }
}

void CoreImpl::ProcessRunEnd(uint32_t section_code)
{
    uint32_t class_code;
    SERVICE *service_PTR = Services_List.GetService(class_code);
    while (service_PTR)
    {
        const uint32_t section = service_PTR->RunSection();
        if (section == section_code)
        {
            service_PTR->RunEnd();
        }
        service_PTR = Services_List.GetServiceNext(class_code);
    }
}

uint32_t CoreImpl::EngineFps()
{
    return Timer.fps;
}

void CoreImpl::SetDeltaTime(int32_t delta_time)
{
    Timer.SetDelta(delta_time);
}

uint32_t CoreImpl::GetDeltaTime()
{
    return Timer.GetDeltaTime();
}

uint32_t CoreImpl::GetRDeltaTime()
{
    return Timer.rDelta_Time;
}

ATTRIBUTES *CoreImpl::Entity_GetAttributeClass(entid_t id_PTR, const char *name)
{
    Entity *pE = core.GetEntityPointer(id_PTR);
    if (pE == nullptr)
        return nullptr;
    if (pE->AttributesPointer == nullptr)
        return nullptr;
    return pE->AttributesPointer->FindAClass(pE->AttributesPointer, name);
}

const char *CoreImpl::Entity_GetAttribute(entid_t id_PTR, const char *name)
{
    Entity *pE = core.GetEntityPointer(id_PTR);
    if (pE == nullptr)
        return nullptr;
    if (pE->AttributesPointer == nullptr)
        return nullptr;
    return pE->AttributesPointer->GetAttribute(name);
}

uint32_t CoreImpl::Entity_GetAttributeAsDword(entid_t id_PTR, const char *name, uint32_t def)
{
    Entity *pE = core.GetEntityPointer(id_PTR);
    if (pE == nullptr)
        return def;
    if (pE->AttributesPointer == nullptr)
        return def;
    return pE->AttributesPointer->GetAttributeAsDword(name, def);
}

FLOAT CoreImpl::Entity_GetAttributeAsFloat(entid_t id_PTR, const char *name, FLOAT def)
{
    Entity *pE = core.GetEntityPointer(id_PTR);
    if (pE == nullptr)
        return def;
    if (pE->AttributesPointer == nullptr)
        return def;
    return pE->AttributesPointer->GetAttributeAsFloat(name, def);
}

bool CoreImpl::Entity_SetAttribute(entid_t id_PTR, const char *name, const char *attribute)
{
    Entity *pE = core.GetEntityPointer(id_PTR);
    if (pE == nullptr)
        return false;
    if (pE->AttributesPointer == nullptr)
        return false;
    return pE->AttributesPointer->SetAttribute(name, attribute);
}

bool CoreImpl::Entity_SetAttributeUseDword(entid_t id_PTR, const char *name, uint32_t val)
{
    Entity *pE = core.GetEntityPointer(id_PTR);
    if (pE == nullptr)
        return false;
    if (pE->AttributesPointer == nullptr)
        return false;
    return pE->AttributesPointer->SetAttributeUseDword(name, val);
}

bool CoreImpl::Entity_SetAttributeUseFloat(entid_t id_PTR, const char *name, FLOAT val)
{
    Entity *pE = core.GetEntityPointer(id_PTR);
    if (pE == nullptr)
        return false;
    if (pE->AttributesPointer == nullptr)
        return false;
    return pE->AttributesPointer->SetAttributeUseFloat(name, val);
}

void CoreImpl::Entity_SetAttributePointer(entid_t id_PTR, ATTRIBUTES *pA)
{
    Entity *pE = core.GetEntityPointer(id_PTR);
    if (pE == nullptr)
        return;
    pE->AttributesPointer = pA;
}

uint32_t CoreImpl::Entity_AttributeChanged(entid_t id_PTR, ATTRIBUTES *pA)
{
    Entity *pE = core.GetEntityPointer(id_PTR);
    if (pE == nullptr)
        return 0;
    return pE->AttributeChanged(pA);
}

ATTRIBUTES *CoreImpl::Entity_GetAttributePointer(entid_t id_PTR)
{
    Entity *pE = core.GetEntityPointer(id_PTR);
    if (pE == nullptr)
        return nullptr;
    return pE->AttributesPointer;
}

void CoreImpl::EraseEntities()
{
    entity_manager_.EraseAll();
}

void CoreImpl::ClearEvents()
{
    Compiler->ClearEvents();
}

void CoreImpl::AppState(bool state)
{
    if (Controls)
        Controls->AppState(state);
}

uint32_t CoreImpl::MakeHashValue(const char *string)
{
    uint32_t hval = 0;

    while (*string != 0)
    {
        char v = *string++;
        if ('A' <= v && v <= 'Z')
            v += 'a' - 'A';

        hval = (hval << 4) + static_cast<uint32_t>(v);
        const uint32_t g = hval & (static_cast<uint32_t>(0xf) << (32 - 4));
        if (g != 0)
        {
            hval ^= g >> (32 - 8);
            hval ^= g;
        }
    }
    return hval;
}

//==========================================================================================================================
// end
//==========================================================================================================================

void CoreImpl::DumpEntitiesInfo()
{
    /*LARGE_INTEGER li;
    if (!QueryPerformanceFrequency(&li))
      throw std::runtime_error();

    const auto freq = double(li.QuadPart)/1000.0;

    uint32_t n;
    char * ptr;
    VMA * pClass;

    Trace("Script commands executed: %" PRIu32, dwNumberScriptCommandsExecuted);
    Trace("Entity Dump -----------------------------------");

    for(n=0;n<=CoreState.Atoms_max_orbit;n++)
    {
      if(Atoms_PTR[n] == nullptr) continue;

      pClass = FindVMA(Atoms_PTR[n]->atom_id.class_code);
      if(pClass)
      {
        ptr = pClass->GetName();
        if(ptr)
        {
          Trace("Class: %s", ptr);
          Trace("     : Realize:  Cur( %fms ) Max( %fms )", Atoms_PTR[n]->as.Realize_ticks_av/freq,
    Atoms_PTR[n]->as.Realize_ticks_max/freq); Trace("     : Execute:  Cur( %fms ) Max( %fms )",
    Atoms_PTR[n]->as.Execute_ticks_av/freq, Atoms_PTR[n]->as.Execute_ticks_max/freq);
        }
      }
    }

    Trace(" ------- Running objects ------- ");
    for(n=0;n<=CommonLayers.lss.Layer_max_index;n++)
    {
      LAYER* l_PTR = CommonLayers.Layer_Table[n];
      if(l_PTR == nullptr) continue;

      uint32_t flags = l_PTR->ls.Flags & LRFLAG_FROZEN;
      if(flags != 0)
      {
        Trace("LAYER: %s frozen -------------------------------------",l_PTR->Name);
        continue;
      }
      Trace("LAYER: %s --------------------------------------",l_PTR->Name);
      entid_t  eid_PTR = l_PTR->GetId();
      while(eid_PTR)
      {
        Trace("Entity:");
        if(ValidateEntity(eid_PTR))
        {
          pClass = FindVMA(Atoms_PTR[eid_PTR->atom_position]->atom_id.class_code);
          if(pClass) ptr = pClass->GetName();
          Trace("        %s",ptr);
          if(Atoms_PTR[eid_PTR->atom_position]->as.Deleted) Trace("        deleted");

        }
        eid_PTR = l_PTR->GetNextID();
      }
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(200));*/
}

void *CoreImpl::GetSaveData(const char *file_name, int32_t &data_size)
{
    return Compiler->GetSaveData(file_name, data_size);
}

bool CoreImpl::SetSaveData(const char *file_name, void *data_ptr, int32_t data_size)
{
    return Compiler->SetSaveData(file_name, data_ptr, data_size);
}

uint32_t CoreImpl::SetScriptFunction(IFUNCINFO *pFuncInfo)
{
    return Compiler->SetScriptFunction(pFuncInfo);
}

const char *CoreImpl::EngineIniFileName()
{
    return fs::ENGINE_INI_FILE_NAME;
}

void *CoreImpl::GetScriptVariable(const char *pVariableName, uint32_t *pdwVarIndex)
{
    const VarInfo *real_var;

    const auto dwVarIndex = Compiler->VarTab.FindVar(pVariableName);
    if (dwVarIndex == INVALID_VAR_CODE)
    {
        return nullptr;
    }

    real_var = Compiler->VarTab.GetVar(dwVarIndex);
    if (real_var == nullptr)
    {
        return nullptr;
    }

    if (pdwVarIndex)
        *pdwVarIndex = dwVarIndex;

    return real_var->value.get();
}

storm::ENGINE_VERSION CoreImpl::GetTargetEngineVersion() const noexcept
{
    return targetVersion_;
}

ScreenSize CoreImpl::GetScreenSize() const noexcept
{
    switch (targetVersion_)
    {
    case storm::ENGINE_VERSION::PIRATES_OF_THE_CARIBBEAN: {
        return {640, 480};
    }
    default: {
        return {800, 600};
    }
    }
}

void CoreImpl::stopFrameProcessing()
{
    stopFrameProcessing_ = true;
}

void CoreImpl::AddToLayer(layer_index_t index, entid_t id, priority_t priority)
{
    entity_manager_.AddToLayer(index, id, priority);
}

void CoreImpl::EraseEntity(entid_t entity)
{
    entity_manager_.EraseEntity(entity);
}

entid_t CoreImpl::CreateEntity(const char *name, ATTRIBUTES *attr)
{
    return entity_manager_.CreateEntity(name, attr);
}

entptr_t CoreImpl::GetEntityPointer(entid_t id) const
{
    return entity_manager_.GetEntityPointer(id);
}

entid_t CoreImpl::GetEntityId(const char *name) const
{
    return entity_manager_.GetEntityId(name);
}

entity_container_cref CoreImpl::GetEntityIds(layer_type_t type) const
{
    return entity_manager_.GetEntityIds(type);
}

entity_container_cref CoreImpl::GetEntityIds(layer_index_t index) const
{
    return entity_manager_.GetEntityIds(index);
}

entity_container_cref CoreImpl::GetEntityIds(const char *name) const
{
    return entity_manager_.GetEntityIds(name);
}

void CoreImpl::SetLayerType(layer_index_t index, layer_type_t type)
{
    entity_manager_.SetLayerType(index, type);
}

void CoreImpl::SetLayerFrozen(layer_index_t index, bool freeze)
{
    entity_manager_.SetLayerFrozen(index, freeze);
}

void CoreImpl::RemoveFromLayer(layer_index_t index, entid_t id)
{
    entity_manager_.RemoveFromLayer(index, id);
}

hash_t CoreImpl::GetClassCode(entid_t id) const
{
    return entity_manager_.GetClassCode(id);
}

bool CoreImpl::IsLayerFrozen(layer_index_t index) const
{
    return entity_manager_.IsLayerFrozen(index);
}

void CoreImpl::ForEachEntity(const std::function<void(entptr_t)> &f)
{
    entity_manager_.ForEachEntity(f);
}

void CoreImpl:: collectCrashInfo() const
{
    Compiler->CollectCallStack();
}

void CoreImpl::loadCompatibilitySettings(INIFILE &inifile)
{
    using namespace storm;

    std::array<char, 128> strBuffer{};
    inifile.ReadString("compatibility", "target_version", strBuffer.data(), strBuffer.size(), "latest");
    const std::string_view target_engine_version = strBuffer.data();

    targetVersion_ = getTargetEngineVersion(target_engine_version);
    if (targetVersion_ == ENGINE_VERSION::UNKNOWN)
    {
        spdlog::warn("Unknown target version '{}' in engine compatibility settings", target_engine_version);
        targetVersion_ = ENGINE_VERSION::LATEST;
    }
}