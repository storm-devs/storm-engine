#pragma once

#include "BallSplashDefines.h"
#include "TSplash.h"
#include "dx9render.h"
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
        }
    }

  private:
    void InitializeSplashes();
    TSplash *TryToAddSplash(const Vector &_pos, const Vector &_dir);

    TSplash splashes[MAX_SPLASHES];
    VDX9RENDER *renderer;
    SEA_BASE *sea;
};
