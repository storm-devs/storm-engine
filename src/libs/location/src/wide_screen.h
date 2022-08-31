//============================================================================================
//    Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
//    WideScreen
//--------------------------------------------------------------------------------------------
//
//============================================================================================

#pragma once

#include "dx9render.h"
#include "vma.hpp"

class WideScreen : public Entity
{
    // --------------------------------------------------------------------------------------------
    // Construction, destruction
    // --------------------------------------------------------------------------------------------
  public:
    WideScreen();
    ~WideScreen() override;

    // Initialization
    bool Init() override;
    // Messages
    uint64_t ProcessMessage(MESSAGE &message) override;
    // Work
    void Realize(uint32_t delta_time);

    void ProcessStage(Stage stage, uint32_t delta) override
    {
        switch (stage)
        {
            // case Stage::execute:
            //    Execute(delta); break;
        case Stage::realize:
            Realize(delta);
            break;
            /*case Stage::lost_render:
              LostRender(delta); break;
            case Stage::restore_render:
              RestoreRender(delta); break;*/
        }
    }

    // --------------------------------------------------------------------------------------------
    // Encapsulation
    // --------------------------------------------------------------------------------------------
  private:
    VDX9RENDER *rs; //
    float w, h;     // Screen dimensions
    float state;    // Screen state
    float dlt;      // Screen change direction
};
