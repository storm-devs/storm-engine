//****************************************************************
//*
//*  Author : Sergey Makeev aka Joker, 2003
//*
//*
//****************************************************************

#ifndef PARTICLES_SERVICE_IMPLEMENTATION
#define PARTICLES_SERVICE_IMPLEMENTATION

#include "particles/iparticlesservice.h"
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
    //Конструктор / деструктор
    ParticleService();
    virtual ~ParticleService();

    bool Init() override;

    //Создать менеджер партиклов
    IParticleManager *CreateManagerEx(const char *ProjectName, const char *File, int Line) override;

    virtual void RemoveManagerFromList(IParticleManager *pManager);

    uint32_t GetManagersCount() override;
    IParticleManager *GetManagerByIndex(uint32_t Index) override;

    IParticleManager *DefManager() override;
};

#endif
