#pragma once

#include "t_seagulls.h"
#include "t_butterflies.h"
#include "t_fish_schools.h"

///////////////////////////////////////////////////////////////////
// DEFINES & TYPES
///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
// CLASS DEFINITION
///////////////////////////////////////////////////////////////////
class ANIMALS : public Entity
{
  public:
    ANIMALS();
    ~ANIMALS() override;

    bool Init() override;
    uint64_t ProcessMessage(MESSAGE &message) override;
    virtual void Realize(uint32_t dTime);
    virtual void Execute(uint32_t dTime);
    uint32_t AttributeChanged(ATTRIBUTES *pA) override;

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
    TSeagulls *seagulls;
    // TSharks      *sharks;
    TFishSchools *fishSchools;
    TButterflies *butterflies;
};
