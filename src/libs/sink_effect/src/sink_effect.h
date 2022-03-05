#pragma once

#include "c_vector.h"
#include "sink_splash_defines.h"
#include "t_sink.h"
#include "dx9render.h"
#include "geometry.h"
#include "sea_base.h"

///////////////////////////////////////////////////////////////////
// CLASS DEFINITION
///////////////////////////////////////////////////////////////////

class SINKEFFECT : public Entity
{
  public:
    SINKEFFECT();
    ~SINKEFFECT() override;

    bool Init() override;
    uint64_t ProcessMessage(MESSAGE &message) override;
    virtual void Realize(uint32_t dTime);
    virtual void Execute(uint32_t dTime);

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

  private:
    void InitializeSinks();
    TSink *TryToAddSink(const CVECTOR &_pos, float _r);

    TSink sinks[sink_effect::MAX_SINKS];
    VDX9RENDER *renderer;
    SEA_BASE *sea;
};
