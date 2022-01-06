#pragma once

#include "ball_splash_defines.h"
#include "c_vector.h"
#include "t_splash.h"
#include "dx9render.h"
#include "geometry.h"
#include "sea_base.h"

///////////////////////////////////////////////////////////////////
// CLASS DEFINITION
///////////////////////////////////////////////////////////////////

class BALLSPLASH : public Entity
{
  public:
    BALLSPLASH();
    ~BALLSPLASH() override;

    bool Init() override;
    uint64_t ProcessMessage(MESSAGE &message) override;
    virtual void Realize(uint32_t dTime);
    virtual void Execute(uint32_t dTime);

    void BALLSPLASH::ProcessStage(Stage stage, uint32_t delta) override
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
    void InitializeSplashes();
    TSplash *TryToAddSplash(const CVECTOR &_pos, const CVECTOR &_dir);

    TSplash splashes[MAX_SPLASHES];
    VDX9RENDER *renderer;
    SEA_BASE *sea;
};
