//============================================================================================
//    LocCrabs
//============================================================================================

#pragma once

#include "dx9render.h"
#include "vma.hpp"

#include "loc_crab.h"

class LocCrabs : public Entity
{
  public:
    LocCrabs();
    ~LocCrabs() override;
    //--------------------------------------------------------------------------------------------
  public:
    // Initialization
    bool Init() override;
    // Execution
    void Execute(uint32_t delta_time);
    // Drawing
    void Realize(uint32_t delta_time);

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

    // Messages
    uint64_t ProcessMessage(MESSAGE &message) override;

    //--------------------------------------------------------------------------------------------
  private:
    LocCrab crab[32];
    int32_t num;
};
