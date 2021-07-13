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

#include "Debris.h"
#include "NoiseCloud.h"
#include "TornadoParticles.h"

class Tornado : public Entity
{
    // --------------------------------------------------------------------------------------------
    // Construction, destruction
    // --------------------------------------------------------------------------------------------
  public:
    Tornado();
    virtual ~Tornado();

    // Initialization
    bool Init();
    // Execution
    void Execute(uint32_t delta_time);
    void Realize(uint32_t delta_time);
    uint64_t ProcessMessage(MESSAGE &message);
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
    long sID;

    Pillar pillar;
    TornadoParticles particles;
    NoiseCloud noiseCloud;
    Debris debris;

    float eventCounter;

    float liveTime;
    float galhpa;

    long ib, vb;
};
