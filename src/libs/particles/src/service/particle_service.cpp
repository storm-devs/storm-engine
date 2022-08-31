#include "particle_service.h"

#include "core.h"

#include "../k2_wrapper/particles.h"
#include "../manager/particle_manager.h"
#include "vma.hpp"

CREATE_SERVICE(ParticleService)

CREATE_CLASS(PARTICLES)

ParticleService::ParticleService()
{
    pDefaultManager = nullptr;
    sysDelete = false;
}

ParticleService::~ParticleService()
{
    if (pDefaultManager)
        pDefaultManager->Release();
    sysDelete = true;

    if (!CreatedManagers.empty())
    {
        core.Trace("Unreleased particles managers found !\n");
    }
    for (auto n = 0; n < CreatedManagers.size(); n++)
    {
        core.Trace("Manager created in %s, Line %d\n", CreatedManagers[n].FileName.c_str(), CreatedManagers[n].Line);
        CreatedManagers[n].pManager->Release();
    }
}

IParticleManager *ParticleService::CreateManagerEx(const char *ProjectName, const char *File, int Line)
{
    auto *pManager = new ParticleManager(this);

    CreatedManager manager;
    manager.pManager = pManager;
    manager.Line = Line;
    manager.FileName = File;
    CreatedManagers.push_back(manager);

    if (ProjectName != nullptr)
    {
        pManager->OpenProject(ProjectName);
    }
    return pManager;
}

void ParticleService::RemoveManagerFromList(IParticleManager *pManager)
{
    if (sysDelete)
        return;
    for (auto n = 0; n < CreatedManagers.size(); n++)
    {
        if (CreatedManagers[n].pManager == pManager)
        {
            // CreatedManagers.ExtractNoShift(n);
            CreatedManagers[n] = CreatedManagers.back();
            CreatedManagers.pop_back();
            return;
        }
    }
}

uint32_t ParticleService::GetManagersCount()
{
    return CreatedManagers.size();
}

IParticleManager *ParticleService::GetManagerByIndex(uint32_t Index)
{
    return CreatedManagers[Index].pManager;
}

bool ParticleService::Init()
{
    pDefaultManager = CreateManagerEx(nullptr, __FILE__, __LINE__);
    Assert(pDefaultManager);
    pDefaultManager->OpenDefaultProject();
    return true;
}

IParticleManager *ParticleService::DefManager()
{
    return pDefaultManager;
}
