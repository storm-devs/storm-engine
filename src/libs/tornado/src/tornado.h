//============================================================================================
//    Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
//    Tornado
//--------------------------------------------------------------------------------------------
//
//============================================================================================

#pragma once

#include "debris.h"
#include "noise_cloud.h"
#include "tornado_particles.h"

class Tornado : public Entity
{
    // --------------------------------------------------------------------------------------------
    // Construction, destruction
    // --------------------------------------------------------------------------------------------
  public:
    Tornado();
    ~Tornado() override;

    // Initialization
    bool Init() override;
    // Execution
    void Execute(uint32_t delta_time);
    void Realize(uint32_t delta_time);
    uint64_t ProcessMessage(MESSAGE &message) override;

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

    void SetAlpha(float a);

    // --------------------------------------------------------------------------------------------
    // Encapsulation
    // --------------------------------------------------------------------------------------------
  private:
    VDX9RENDER *rs;

    VSoundService *soundService;
    int32_t sID;

    Pillar pillar;
    TornadoParticles particles;
    NoiseCloud noiseCloud;
    Debris debris;

    float eventCounter;

    float liveTime;
    float galhpa;

    int32_t ib, vb;
};
