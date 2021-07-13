#pragma once

#include "Entity.h"
#include <string>
#include <vector>

class IParticleService;
class IParticleManager;
class IParticleSystem;
class PARTICLE_SYSTEM;

class PARTICLES : public Entity
{
    bool CreationCapture;
    bool bSystemDelete;

    struct SystemInfo
    {
        std::string FileName;
        PARTICLE_SYSTEM *pSystem;
        uint32_t LifeTime;
        uint32_t PassedTime;

        SystemInfo()
        {
            PassedTime = 0;
            pSystem = nullptr;
            LifeTime = 0xFFFFFF00;
        }
    };

    std::vector<SystemInfo> CreatedSystems;

    IParticleService *pService;
    IParticleManager *pManager;

    PARTICLE_SYSTEM *CreateSystem(const char *pFileName, uint32_t LifeTime);
    void DeleteSystem(uintptr_t SystemID);
    void DeleteAll();

    void PauseAllActive(bool bPaused);

    std::vector<uintptr_t> CaptureBuffer;

    void DeleteCaptured();

  public:
    void DeleteResource(PARTICLE_SYSTEM *pResource);

  public:
    PARTICLES();
    ~PARTICLES();

    bool Init() override;

    uint64_t ProcessMessage(MESSAGE &message) override;

    void Realize(uint32_t Delta_Time);
    void Execute(uint32_t Delta_Time);

    void ProcessStage(Stage stage, uint32_t delta) override
    {
        switch (stage)
        {
        case Stage::execute:
            Execute(delta);
            break;
        case Stage::realize:
            Realize(delta);
            break;
            /*case Stage::lost_render:
              LostRender(delta); break;
            case Stage::restore_render:
              RestoreRender(delta); break;*/
        }
    }
};
