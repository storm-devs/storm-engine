#pragma once

#include "TSeagulls.h"
#include "geometry.h"
//#include "TSharks.h"
#include "TButterflies.h"
#include "TFishSchools.h"

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
    virtual ~ANIMALS();

    virtual bool Init();
    virtual uint64_t ProcessMessage(MESSAGE &message);
    virtual void Realize(uint32_t dTime);
    virtual void Execute(uint32_t dTime);
    uint32_t AttributeChanged(ATTRIBUTES *pA);

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
