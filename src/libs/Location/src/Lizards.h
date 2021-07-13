//============================================================================================
//    Spirenkov Maxim
//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
//    Lizards
//--------------------------------------------------------------------------------------------
//
//============================================================================================

#pragma once

#include "dx9render.h"
#include "vmodule_api.h"

#include "Lizard.h"

class Lizards : public Entity
{
  public:
    Lizards();
    virtual ~Lizards();

    // Initialization
    bool Init() override;
    // Update
    void Execute(uint32_t delta_time);
    // Rendering
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

    Lizard lizard[8];
    long num;
};
