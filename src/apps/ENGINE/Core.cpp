#include "core.h"
#include "SteamApi.hpp"
#include "VmaInit.h"
#include "compiler.h"
#include "controls.h"
#include "fs.h"
#include <fstream>

uint32_t dwNumberScriptCommandsExecuted = 0;

typedef struct
{
    uint32_t code;
    void *pointer;
} CODE_AND_POINTER;

void CORE::ResetCore()
{
    Initialized = false;
    bEngineIniProcessed = false;

    ReleaseServices();

    Services_List.Release();

    STORM_DELETE(State_file_name);
}

void CORE::CleanUp()
{
    Initialized = false;
    bEngineIniProcessed = false;
    ReleaseServices();
    Compiler->Release();
    Services_List.Release();
    Services_List.Release();
    delete State_file_name;
}

void CORE::Init()
{
    Initialized = false;
    bEngineIniProcessed = false;
    App_Hwnd = nullptr;
    State_file_name = nullptr;
    Exit_flag = false;
    State_loading = false;
    Memory_Leak_flag = false;
    Controls = nullptr;
    fTimeScale = 1.0f;
    Compiler = new COMPILER;

    /* TODO: place this outside CORE */
    EntityManager::SetLayerType(EXECUTE, EntityManager::Layer::Type::execute);
    EntityManager::SetLayerType(REALIZE, EntityManager::Layer::Type::realize);
    EntityManager::SetLayerType(SEA_EXECUTE, EntityManager::Layer::Type::execute);
    EntityManager::SetLayerType(SEA_REALIZE, EntityManager::Layer::Type::realize);
    EntityManager::SetLayerType(INTERFACE_EXECUTE, EntityManager::Layer::Type::execute);
    EntityManager::SetLayerType(INTERFACE_REALIZE, EntityManager::Layer::Type::realize);
    EntityManager::SetLayerType(FADER_EXECUTE, EntityManager::Layer::Type::execute);
    EntityManager::SetLayerType(FADER_REALIZE, EntityManager::Layer::Type::realize);
    EntityManager::SetLayerType(LIGHTER_EXECUTE, EntityManager::Layer::Type::execute);
    EntityManager::SetLayerType(LIGHTER_REALIZE, EntityManager::Layer::Type::realize);
    EntityManager::SetLayerType(VIDEO_EXECUTE, EntityManager::Layer::Type::execute);
    EntityManager::SetLayerType(VIDEO_REALIZE, EntityManager::Layer::Type::realize);
    EntityManager::SetLayerType(EDITOR_REALIZE, EntityManager::Layer::Type::realize);
    EntityManager::SetLayerType(INFO_REALIZE, EntityManager::Layer::Type::realize);
    EntityManager::SetLayerType(SOUND_DEBUG_REALIZE, EntityManager::Layer::Type::realize);
}

void CORE::InitBase()
{
    LoadClassesTable();
}

void CORE::ReleaseBase()
{
    Compiler->Token.Release();
}

bool CORE::Run()
{
    const auto bDebugWindow = true;
    if (bDebugWindow && core.Controls && core.Controls->GetDebugAsyncKeyState(VK_F7) < 0)
        DumpEntitiesInfo();
    dwNumberScriptCommandsExecuted = 0;

    if (Exit_flag)
        return false; // exit

    Timer.Run(); // calc delta time

    auto *pVCTime = static_cast<VDATA *>(core.GetScriptVariable("iRealDeltaTime"));
    if (pVCTime)
        pVCTime->Set(static_cast<long>(GetRDeltaTime()));

    SYSTEMTIME st;
    GetLocalTime(&st);

    auto *pVYear = static_cast<VDATA *>(core.GetScriptVariable("iRealYear"));
    auto *pVMonth = static_cast<VDATA *>(core.GetScriptVariable("iRealMonth"));
    auto *pVDay = static_cast<VDATA *>(core.GetScriptVariable("iRealDay"));

    if (pVYear)
        pVYear->Set(static_cast<long>(st.wYear));
    if (pVMonth)
        pVMonth->Set(static_cast<long>(st.wMonth));
    if (pVDay)
        pVDay->Set(static_cast<long>(st.wDay));

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
    ProcessExecute(); // transfer control to objects via Execute() function
    ProcessRealize(); // transfer control to objects via Realize() function

    steamapi::SteamApi::getInstance().RunCallbacks();

    if (Controls)
        Controls->Update(Timer.rDelta_Time);

    if (Controls)
        ProcessControls();

    EntityManager::NewLifecycle();

    ProcessRunEnd(SECTION_ALL);

    return true;
}

void CORE::ProcessControls()
{
    CONTROL_STATE cs;
    USER_CONTROL uc;

    if (!Controls)
        return;

    for (long n = 0; n < Controls->GetControlsNum(); n++)
    {
        Controls->GetControlState(n, cs);
        if (cs.state == CST_ACTIVATED)
        {
            Controls->GetControlDesc(n, uc);
            Event("Control Activation", "s", uc.name);
        }
        else if (cs.state == CST_INACTIVATED)
        {
            Controls->GetControlDesc(n, uc);
            Event("Control Deactivation", "s", uc.name);
        }
    }
}

//-------------------------------------------------------------------------------------------------
// internal functions
//-------------------------------------------------------------------------------------------------
bool CORE::Initialize()
{
    ResetCore();

    InitializeCriticalSection(&lock);

    Initialized = true;

    return true;
}

void CORE::ProcessEngineIniFile()
{
    char String[_MAX_PATH];

    bEngineIniProcessed = true;

    auto engine_ini = fio->OpenIniFile(fs::ENGINE_INI_FILE_NAME);
    if (!engine_ini)
        throw std::exception("no 'engine.ini' file");

    auto res = engine_ini->ReadString(nullptr, "program_directory", String, sizeof(String), "");
    if (res)
    {
        Compiler->SetProgramDirectory(String);
    }

    res = engine_ini->ReadString(nullptr, "controls", String, sizeof(String), "");
    if (res)
    {
        core.Controls = static_cast<CONTROLS *>(MakeClass(String));
        if (core.Controls == nullptr)
            core.Controls = static_cast<CONTROLS *>(MakeClass("controls"));
    }
    else
    {
        delete Controls;
        Controls = nullptr;

        core.Controls = new CONTROLS;
    }

    res = engine_ini->ReadString(nullptr, "run", String, sizeof(String), "");
    if (res)
    {
        if (!Compiler->CreateProgram(String))
            throw std::exception("fail to create program");
        if (!Compiler->Run())
            throw std::exception("fail to run program");
        // Script version test
        long iScriptVersion = 0xFFFFFFFF;
        auto *pVScriptVersion = static_cast<VDATA *>(core.GetScriptVariable("iScriptVersion"));
        if (pVScriptVersion)
            pVScriptVersion->Get(iScriptVersion);

        if (iScriptVersion != ENGINE_SCRIPT_VERSION)
        {
            ShowCursor(true);
            MessageBoxA(nullptr, "Wrong script version", "Error", MB_OK);
            Compiler->ExitProgram();
        }
    }
}

bool CORE::LoadClassesTable()
{
    for (auto *c : _pModuleClassRoot)
    {
        const auto hash = MakeHashValue(c->GetName());
        c->SetHash(hash);
    }

    return true;
}

void CORE::CheckAutoExceptions(uint32_t = 0) const
{
    spdlog::warn("exception thrown");
}

void CORE::Exit()
{
    Exit_flag = true;
}

//------------------------------------------------------------------------------------------------
// return application window handle
//
HWND CORE::GetAppHWND()
{
    return App_Hwnd;
}
HINSTANCE CORE::GetAppInstance()
{
    return hInstance;
}

void CORE::SetTimeScale(float _scale)
{
    fTimeScale = _scale;
}

//------------------------------------------------------------------------------------------------
// transfer message arguments and program control to entity, specified by Destination id
//
uint64_t CORE::Send_Message(entid_t Destination, const char *Format, ...)
{
    MESSAGE message;
    auto *const ptr = EntityManager::GetEntityPointer(Destination); // check for valid destination
    if (!ptr)
        return 0;

    message.Reset(Format); // reset message class
    PZERO(&message.Sender_ID, sizeof(entid_t));
    va_start(message.args, Format);
    const auto rc = static_cast<Entity *>(ptr)->ProcessMessage(message); // transfer control
    va_end(message.args);
    return rc;
}

uint32_t CORE::PostEvent(const char *Event_name, uint32_t post_time, const char *Format, ...)
{
    MESSAGE_SCRIPT *pMS;
    MESSAGE message;

    entid_t id;

    if (Format != nullptr)
    {
        pMS = new MESSAGE_SCRIPT;
        va_start(message.args, Format);
        message.Reset(Format);
        pMS->Reset(Format);

        auto bAction = true;
        while (bAction)
        {
            switch (message.GetCurrentFormatType())
            {
                //-------------------------------------
            case 'l':
                long v;
                v = message.Long();
                pMS->Set((char *)&v);
                break;
            case 'f':
                float f;
                f = message.Float();
                pMS->Set((char *)&f);
                break;
            case 'i':
                id = message.EntityID();
                pMS->Set((char *)&id);
                break;
            case 'e':
                VDATA *e;
                e = message.ScriptVariablePointer();
                pMS->Set((char *)&e);
                break;
            case 's':
                char *s;
                s = message.StringPointer();
                pMS->Set(static_cast<char *>(s));
                break;
            case 'a':
                ATTRIBUTES *a;
                a = message.AttributePointer();
                pMS->Set((char *)&a);
                break;

                //-------------------------------------
            default:
                bAction = false;
                break;
            }
        }
        va_end(message.args);
    }
    else
        pMS = nullptr;

    auto *pEM = new S_EVENTMSG(Event_name, pMS, post_time);
    pEM->bProcess = true;
    Compiler->AddPostEvent(pEM);
    return 0;
}

VDATA *CORE::Event(const char *Event_name, const char *Format, ...)
{
    VDATA *pVD = nullptr;
    if (Format == nullptr)
    {
        pVD = Compiler->ProcessEvent(Event_name);
        return pVD;
    }
    MESSAGE message;
    va_start(message.args, Format); // 1
    message.Reset(Format);          // reset message class    // 2
    // ....
    pVD = Compiler->ProcessEvent(Event_name, message);

    va_end(message.args);
    return pVD;
}

void *CORE::MakeClass(const char *class_name)
{
    const long hash = MakeHashValue(class_name);
    for (auto *const c : _pModuleClassRoot)
        if (c->GetHash() == hash && _stricmp(class_name, c->GetName()) == 0)
            return c->CreateClass();

    return nullptr;
}

void CORE::ReleaseServices()
{
    for (auto *const c : _pModuleClassRoot)
        if (c->Service())
            c->Clear();

    Controls = nullptr;
}

VMA *CORE::FindVMA(const char *class_name)
{
    const long hash = MakeHashValue(class_name);
    for (auto *const c : _pModuleClassRoot)
        if (c->GetHash() == hash && _stricmp(class_name, c->GetName()) == 0)
            return c;

    return nullptr;
}

VMA *CORE::FindVMA(long hash)
{
    for (auto *const c : _pModuleClassRoot)
        if (c->GetHash() == hash)
            return c;

    return nullptr;
}

void *CORE::CreateService(const char *service_name)
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

void CORE::Trace(const char *format, ...)
{
    static char buffer_4k[4096];

    va_list args;
    va_start(args, format);
    _vsnprintf_s(buffer_4k, sizeof(buffer_4k) - 4, format, args);
    va_end(args);
    tracelog->info(buffer_4k);
}

//------------------------------------------------------------------------------------------------
// Transfer programm control to objects via Execute() functions
//
void CORE::ProcessExecute()
{
    uint64_t ticks;

    ProcessRunStart(SECTION_EXECUTE);

    const auto deltatime = Timer.GetDeltaTime();
    const auto &entIds = EntityManager::GetEntityIdVector(EntityManager::Layer::Type::execute);
    for (auto id : entIds)
    {
        if (auto *ptr = EntityManager::GetEntityPointer(id))
        {
            ptr->ProcessStage(Entity::Stage::execute, deltatime);
        }
    }

    ProcessRunEnd(SECTION_EXECUTE);
}

void CORE::ProcessRealize()
{
    uint64_t ticks;
    ProcessRunStart(SECTION_REALIZE);

    const auto deltatime = Timer.GetDeltaTime();
    const auto &entIds = EntityManager::GetEntityIdVector(EntityManager::Layer::Type::realize);
    for (auto id : entIds)
    {
        if (auto *ptr = EntityManager::GetEntityPointer(id))
        {
            ptr->ProcessStage(Entity::Stage::realize, deltatime);
        }
    }

    ProcessRunEnd(SECTION_REALIZE);
}

// save core state
bool CORE::SaveState(const char *file_name)
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
bool CORE::InitiateStateLoading(const char *file_name)
{
    auto fileS = fio->_CreateFile(file_name, std::ios::binary | std::ios::in);
    if (!fileS.is_open())
    {
        return false;
    }
    fio->_CloseFile(fileS);
    delete State_file_name;

    const auto len = strlen(file_name) + 1;
    State_file_name = static_cast<char *>(new char[len]);
    strcpy_s(State_file_name, len, file_name);
    return true;
}

void CORE::ProcessStateLoading()
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

    delete State_file_name;
    State_file_name = nullptr;
    State_loading = false;
}

void CORE::ProcessRunStart(uint32_t section_code)
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

void CORE::ProcessRunEnd(uint32_t section_code)
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

uint32_t CORE::EngineFps()
{
    return Timer.fps;
}

void CORE::SetDeltaTime(long delta_time)
{
    Timer.SetDelta(delta_time);
}

uint32_t CORE::GetDeltaTime()
{
    return Timer.GetDeltaTime();
}

uint32_t CORE::GetRDeltaTime()
{
    return Timer.rDelta_Time;
}

ATTRIBUTES *CORE::Entity_GetAttributeClass(entid_t id_PTR, const char *name)
{
    Entity *pE = EntityManager::GetEntityPointer(id_PTR);
    if (pE == nullptr)
        return nullptr;
    if (pE->AttributesPointer == nullptr)
        return nullptr;
    return pE->AttributesPointer->FindAClass(pE->AttributesPointer, name);
}

char *CORE::Entity_GetAttribute(entid_t id_PTR, const char *name)
{
    Entity *pE = EntityManager::GetEntityPointer(id_PTR);
    if (pE == nullptr)
        return nullptr;
    if (pE->AttributesPointer == nullptr)
        return nullptr;
    return pE->AttributesPointer->GetAttribute(name);
}

uint32_t CORE::Entity_GetAttributeAsDword(entid_t id_PTR, const char *name, uint32_t def)
{
    Entity *pE = EntityManager::GetEntityPointer(id_PTR);
    if (pE == nullptr)
        return def;
    if (pE->AttributesPointer == nullptr)
        return def;
    return pE->AttributesPointer->GetAttributeAsDword(name, def);
}

FLOAT CORE::Entity_GetAttributeAsFloat(entid_t id_PTR, const char *name, FLOAT def)
{
    Entity *pE = EntityManager::GetEntityPointer(id_PTR);
    if (pE == nullptr)
        return def;
    if (pE->AttributesPointer == nullptr)
        return def;
    return pE->AttributesPointer->GetAttributeAsFloat(name, def);
}

bool CORE::Entity_SetAttribute(entid_t id_PTR, const char *name, const char *attribute)
{
    Entity *pE = EntityManager::GetEntityPointer(id_PTR);
    if (pE == nullptr)
        return false;
    if (pE->AttributesPointer == nullptr)
        return false;
    return pE->AttributesPointer->SetAttribute(name, attribute);
}

bool CORE::Entity_SetAttributeUseDword(entid_t id_PTR, const char *name, uint32_t val)
{
    Entity *pE = EntityManager::GetEntityPointer(id_PTR);
    if (pE == nullptr)
        return false;
    if (pE->AttributesPointer == nullptr)
        return false;
    return pE->AttributesPointer->SetAttributeUseDword(name, val);
}

bool CORE::Entity_SetAttributeUseFloat(entid_t id_PTR, const char *name, FLOAT val)
{
    Entity *pE = EntityManager::GetEntityPointer(id_PTR);
    if (pE == nullptr)
        return false;
    if (pE->AttributesPointer == nullptr)
        return false;
    return pE->AttributesPointer->SetAttributeUseFloat(name, val);
}

void CORE::Entity_SetAttributePointer(entid_t id_PTR, ATTRIBUTES *pA)
{
    Entity *pE = EntityManager::GetEntityPointer(id_PTR);
    if (pE == nullptr)
        return;
    pE->AttributesPointer = pA;
}

uint32_t CORE::Entity_AttributeChanged(entid_t id_PTR, ATTRIBUTES *pA)
{
    Entity *pE = EntityManager::GetEntityPointer(id_PTR);
    if (pE == nullptr)
        return 0;
    return pE->AttributeChanged(pA);
}

ATTRIBUTES *CORE::Entity_GetAttributePointer(entid_t id_PTR)
{
    Entity *pE = EntityManager::GetEntityPointer(id_PTR);
    if (pE == nullptr)
        return nullptr;
    return pE->AttributesPointer;
}

void CORE::EraseEntities()
{
    EntityManager::EraseAll();
}

void CORE::ClearEvents()
{
    Compiler->ClearEvents();
}

void CORE::AppState(bool state)
{
    if (Controls)
        Controls->AppState(state);
}

uint32_t CORE::MakeHashValue(const char *string)
{
    uint32_t hval = 0;

    while (*string != 0)
    {
        char v = *string++;
        if ('A' <= v && v <= 'Z')
            v += 'a' - 'A';

        hval = (hval << 4) + static_cast<unsigned long>(v);
        const uint32_t g = hval & (static_cast<unsigned long>(0xf) << (32 - 4));
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

void CORE::DumpEntitiesInfo()
{
    /*LARGE_INTEGER li;
    if (!QueryPerformanceFrequency(&li))
      throw std::exception();

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

    Sleep(200);*/
}

void *CORE::GetSaveData(const char *file_name, long &data_size)
{
    return Compiler->GetSaveData(file_name, data_size);
}

bool CORE::SetSaveData(const char *file_name, void *data_ptr, long data_size)
{
    return Compiler->SetSaveData(file_name, data_ptr, data_size);
}

uint32_t CORE::SetScriptFunction(IFUNCINFO *pFuncInfo)
{
    return Compiler->SetScriptFunction(pFuncInfo);
}

const char *CORE::EngineIniFileName()
{
    return fs::ENGINE_INI_FILE_NAME;
}

void *CORE::GetScriptVariable(const char *pVariableName, uint32_t *pdwVarIndex)
{
    VARINFO vi;

    const auto dwVarIndex = Compiler->VarTab.FindVar(pVariableName);
    if (dwVarIndex == INVALID_VAR_CODE || !Compiler->VarTab.GetVar(vi, dwVarIndex))
        return nullptr;

    if (pdwVarIndex)
        *pdwVarIndex = dwVarIndex;

    return vi.pDClass;
}

void CORE::Start_CriticalSection()
{
    EnterCriticalSection(&lock);
};
void CORE::Leave_CriticalSection()
{
    LeaveCriticalSection(&lock);
};
