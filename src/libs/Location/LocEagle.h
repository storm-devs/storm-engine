//============================================================================================
//    Spirenkov Maxim, 2003
//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
//    LocEagle
//--------------------------------------------------------------------------------------------
//
//============================================================================================

#pragma once

#include "model.h"

class LocEagle : public Entity
{
  public:
    LocEagle();
    virtual ~LocEagle();

    // Initialization
    bool Init() override;
    // Execution
    void Execute(uint32_t delta_time);

    void ProcessStage(Stage stage, uint32_t delta) override
    {
        switch (stage)
        {
        case Stage::execute:
            Execute(delta);
            break;
            // case Stage::realize:
            //    Realize(delta); break;
            /*case Stage::lost_render:
              LostRender(delta); break;
            case Stage::restore_render:
              RestoreRender(delta); break;*/
        }
    }

    entid_t mdl;
    CVECTOR cnt;
    float kTime, time;
    float y, dltY;
    float kRad, dltRad;
    float ay;
    float kAy, timeAy;
};
