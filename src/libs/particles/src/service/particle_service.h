//****************************************************************
//*
//*  Author : Sergey Makeev aka Joker, 2003
//*
//*
//****************************************************************

#pragma once

#include "particles/i_particle_service.h"
#include <string>
#include <vector>

class ParticleManager;

class ParticleService : public IParticleService
{
    IParticleManager *pDefaultManager;

    struct CreatedManager
    {
        ParticleManager *pManager;
        std::string FileName;
        int Line;
    };

    bool sysDelete;
    std::vector<CreatedManager> CreatedManagers;

  public:
    // Constructor / destructor
    ParticleService();
    ~ParticleService() override;

    bool Init() override;

    // Create a particle manager
    IParticleManager *CreateManagerEx(const char *ProjectName, const char *File, int Line) override;

    virtual void RemoveManagerFromList(IParticleManager *pManager);

    uint32_t GetManagersCount() override;
    IParticleManager *GetManagerByIndex(uint32_t Index) override;

    IParticleManager *DefManager() override;
};
