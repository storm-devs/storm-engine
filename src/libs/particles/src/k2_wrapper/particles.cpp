#include "particles.h"
#include "core.h"
#include "defines.h"
#include "entity.h"
#include "particles/i_particle_manager.h"
#include "particles/i_particle_service.h"
#include "particles/i_particle_system.h"
#include "p_system.h"
#include "shared/messages.h"

PARTICLES::PARTICLES()
{
    bSystemDelete = false;
    pService = nullptr;
    pManager = nullptr;
    CreationCapture = false;
}

PARTICLES::~PARTICLES()
{
    bSystemDelete = true;
    DeleteAll();
}

bool PARTICLES::Init()
{
    EntityManager::AddToLayer(REALIZE, GetId(), 0xfffff);
    EntityManager::AddToLayer(EXECUTE, GetId(), 0);

    pService = static_cast<IParticleService *>(core.GetService("ParticleService"));
    Assert(pService);
    pManager = pService->DefManager();
    Assert(pManager);
    return true;
}

uint64_t PARTICLES::ProcessMessage(MESSAGE &message)
{
    const auto code = message.Long();

    CVECTOR pos, angles;
    int32_t lifetime;

    switch (code)
    {
        // Pause all particles
        // new not paused ones are emitted ...
    case PS_PAUSEALL: {
        PauseAllActive(message.Long() != 0);
        break;
    }

        // Delete a specific system
    case PS_DELETE: {
        DeleteSystem(message.Long());
        break;
    }

    case PS_CREATIONCAPTURE_BEG: {
        CreationCapture = true;
        break;
    }
    case PS_CREATIONCAPTURE_END: {
        CreationCapture = false;
        break;
    }
    case PS_CLEAR_CAPTURED: {
        DeleteCaptured();
        break;
    }

        // delete everything
    case PS_CLEARALL: {
        DeleteAll();
        break;
    }
        // create system (string name, float x, y, z position, float rx, ry, rz rotation, float life_time lifetime)
    case PS_CREATE_RIC: {
        const std::string &ps_name = message.String();
        pos.x = message.Float();
        pos.y = message.Float();
        pos.z = message.Float();

        angles.x = message.Float();
        angles.y = message.Float();
        angles.z = message.Float();
        lifetime = message.Long();

        auto *pSystem = CreateSystem(ps_name.c_str(), lifetime);
        if (!pSystem)
            return 0;

        pSystem->SetEmitter(pos, angles);
        pSystem->SetDelay(0);
        pSystem->SetLifeTime(lifetime);
        return (uintptr_t)pSystem;

        break;
    }
    case PS_CREATEX_RIC: {
        throw std::runtime_error("Unsupported particle manager command !!!");
    }

        // create a system
    case PS_CREATE: {
        const std::string &ps_name = message.String();
        pos.x = message.Float();
        pos.y = message.Float();
        pos.z = message.Float();

        angles.x = message.Float();
        angles.y = message.Float();
        angles.z = message.Float();
        lifetime = message.Long();

        auto *pSystem = CreateSystem(ps_name.c_str(), lifetime);
        if (!pSystem)
            return 0;

        pSystem->SetEmitter(pos, angles);
        pSystem->SetDelay(0);
        pSystem->SetLifeTime(lifetime);
        return (uintptr_t)pSystem;

        break;
    }
        // create a system
    case PS_CREATEX: {
        const std::string &ps_name = message.String();
        pos.x = message.Float();
        pos.y = message.Float();
        pos.z = message.Float();

        Vector normal;
        normal.x = message.Float();
        normal.y = message.Float();
        normal.z = message.Float();
        const auto fLen = static_cast<double>(normal.Normalize());

        if (fLen)
        {
            angles.y = normal.GetAY();
            auto fDiv = -(normal.y / fLen);
            fDiv = Min(Max(fDiv, -1.0), 1.0);
            angles.x = static_cast<float>(asin(fDiv));
        }
        else
        {
            angles.x = 0.0f;
            angles.y = 0.0f;
        }

        angles.z = 0.0f;

        lifetime = message.Long();

        auto *pSystem = CreateSystem(ps_name.c_str(), lifetime);
        if (!pSystem)
            return 0;

        pSystem->SetEmitter(pos, angles);
        pSystem->SetDelay(0);
        pSystem->SetLifeTime(lifetime);
        return (uintptr_t)pSystem;

        break;
    }
    case PS_ADDTRACKPOINT: {
        throw std::runtime_error("Unsupported particle manager command !!!");
    }
    case PS_USESURFACE: {
        throw std::runtime_error("Unsupported particle manager command !!!");
    }
    case PS_VALIDATE_PARTICLE: {
        auto *const SystemID = reinterpret_cast<PARTICLE_SYSTEM *>(message.Pointer());
        for (uint32_t n = 0; n < CreatedSystems.size(); n++)
            if (CreatedSystems[n].pSystem == SystemID)
                return 1;
        return 0;
        break;
    }
    }
    return 0;
}

PARTICLE_SYSTEM *PARTICLES::CreateSystem(const char *pFileName, uint32_t LifeTime)
{
    // std::string pFullFileName;
    // pFullFileName = "resource\\particles\\";
    // pFullFileName += pFileName;
    // pFullFileName.AddExtention(".xps");
    //__debugbreak(); //~!~
    auto path = std::filesystem::path() / "resource" / "particles" / pFileName;
    std::string pathStr = path.extension().string();
    if (!storm::iEquals(pathStr, ".xps"))
        path += ".xps";
    pathStr = path.string();
    // MessageBoxA(NULL, (LPCSTR)path.c_str(), "", MB_OK); //~!~

    // core.Trace("K2 Particles Wrapper: Create system '%s'", pFileName);
    IParticleSystem *pSys = pManager->CreateParticleSystemEx(pathStr.c_str(), __FILE__, __LINE__);
    if (!pSys)
    {
        // core.Trace("Can't create particles system '%s'", pFileName);
        return nullptr;
    }

    pSys->AutoDelete(false);

    auto *pNewPS = new PARTICLE_SYSTEM(pSys);
    pNewPS->SetManager(this);

    // core.Trace("PSYS Created ok");

    SystemInfo Info;
    Info.pSystem = pNewPS;
    Info.LifeTime = LifeTime;
    Info.FileName = pathStr;
    CreatedSystems.push_back(Info);

    if (CreationCapture)
    {
        CaptureBuffer.push_back((uintptr_t)pNewPS);
    }

    return pNewPS;
}

void PARTICLES::DeleteSystem(uintptr_t SystemID)
{
    bSystemDelete = true;
    for (uint32_t n = 0; n < CreatedSystems.size(); n++)
    {
        if (CreatedSystems[n].pSystem == (PARTICLE_SYSTEM *)SystemID)
        {
            // core.Trace("Delete particles system with name '%s'", CreatedSystems[n].FileName.c_str());
            delete CreatedSystems[n].pSystem;
            // CreatedSystems.ExtractNoShift(n);
            CreatedSystems[n] = CreatedSystems.back();
            CreatedSystems.pop_back();
            bSystemDelete = false;
            return;
        }
    }

    // core.Trace("Can't delete particle system with GUID %d", SystemID);

    bSystemDelete = false;
}

void PARTICLES::DeleteAll()
{
    bSystemDelete = true;
    for (uint32_t n = 0; n < CreatedSystems.size(); n++)
    {
        delete CreatedSystems[n].pSystem;
    }

    CreatedSystems.clear();
    bSystemDelete = false;
}

void PARTICLES::DeleteResource(PARTICLE_SYSTEM *pResource)
{
    if (bSystemDelete)
        return;

    for (uint32_t n = 0; n < CreatedSystems.size(); n++)
    {
        if (CreatedSystems[n].pSystem == pResource)
        {
            // CreatedSystems.ExtractNoShift(n);
            CreatedSystems[n] = CreatedSystems.back();
            CreatedSystems.pop_back();
            return;
        }
    }
}

void PARTICLES::Realize(uint32_t Delta_Time)
{
    bSystemDelete = true;
    const float fDeltaTime = static_cast<float>(Delta_Time) * 0.001f;
    pManager->Execute(fDeltaTime);

    // If it's time, pause emission
    // when all particles die, the system will retire by itself ...
    for (uint32_t n = 0; n < CreatedSystems.size(); n++)
    {
        CreatedSystems[n].PassedTime += Delta_Time;

        if (CreatedSystems[n].LifeTime == 0)
            continue;

        if (CreatedSystems[n].PassedTime > CreatedSystems[n].LifeTime)
        {
            // CreatedSystems[n].pSystem->Pause(true);
            CreatedSystems[n].pSystem->Stop();
        }
    }

    // Removing dead systems ...
    for (uint32_t n = 0; n < CreatedSystems.size(); n++)
    {
        if (!CreatedSystems[n].pSystem->GetSystem()->IsAlive())
        {
            delete CreatedSystems[n].pSystem;
            // CreatedSystems.ExtractNoShift(n);
            CreatedSystems[n] = CreatedSystems.back();
            CreatedSystems.pop_back();
            n--;
        }
    }

    bSystemDelete = false;
}

void PARTICLES::Execute(uint32_t Delta_Time)
{
}

void PARTICLES::PauseAllActive(bool bPaused)
{
    for (uint32_t n = 0; n < CreatedSystems.size(); n++)
    {
        CreatedSystems[n].pSystem->GetSystem()->Restart(0);
        CreatedSystems[n].pSystem->Pause(bPaused);
    }
}

void PARTICLES::DeleteCaptured()
{
    for (uint32_t n = 0; n < CaptureBuffer.size(); n++)
    {
        DeleteSystem(CaptureBuffer[n]);
    }

    CaptureBuffer.clear();
}
