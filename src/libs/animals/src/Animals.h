#pragma once

#include <vector>
#include <memory>

#include "TSeagulls.h"
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
    ~ANIMALS() = default;

    bool Init() override;
    uint64_t ProcessMessage(MESSAGE &message) override;
    virtual void Realize(const uint32_t dTime);
    virtual void Execute(const uint32_t dTime);
    uint32_t AttributeChanged(ATTRIBUTES *pA) override;

    void ProcessStage(const Stage stage, const uint32_t delta) override
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
    std::unique_ptr<TSeagulls> seagulls;
    std::unique_ptr<TFishSchools> fishSchools;
    std::unique_ptr<TButterflies> butterflies;
    // std::shared_ptr<TSharks> sharks;
};
