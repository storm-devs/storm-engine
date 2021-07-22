//============================================================================================
//    Spirenkov Maxim, 2003
//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
//    Fader
//--------------------------------------------------------------------------------------------
//
//============================================================================================

#pragma once

#include "dx9render.h"
#include "vmodule_api.h"

class Fader : public Entity
{
    // --------------------------------------------------------------------------------------------
    // Construction, destruction
    // --------------------------------------------------------------------------------------------
  public:
    Fader();
    ~Fader() override;

    // Initialization
    bool Init() override;
    // Messages
    uint64_t ProcessMessage(MESSAGE &message) override;

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

    // Work
    void Execute(uint32_t delta_time);
    void Realize(uint32_t delta_time);

    // --------------------------------------------------------------------------------------------
    // Encapsulation
    // --------------------------------------------------------------------------------------------
  private:
    VDX9RENDER *rs;
    IDirect3DSurface9 *renderTarget;
    IDirect3DSurface9 *surface;

    bool isWork;
    bool haveFrame;
    bool fadeIn;
    bool isStart;
    bool isAutodelete;
    bool endFade;
    float fadeSpeed;
    float alpha;
    float w, h;

    bool eventStart;
    bool eventEnd;
    long deleteMe;

    long textureID;
    long textureBackID;
    long tipsID;

  public:
    static long numberOfTips;
    static long currentTips;
};
