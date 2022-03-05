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
#include "v_module_api.h"

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
    IDirect3DTexture9 *textureBase;

    bool isWork;
    bool haveFrame;
    bool fadeIn;
    bool isStart;
    bool isAutodelete;
    bool endFade;
    float fadeSpeed;
    float alpha;
    float w, h;

    struct
    {
        float x, y, z, rhw;
        float u, v;
    } drawbuf_base[6];

    struct
    {
        float x, y, z, rhw;
        uint32_t color;
        float u, v;
    } drawbuf_back[6], drawbuf_front[6];

    bool eventStart;
    bool eventEnd;
    int32_t deleteMe;

    int32_t textureID;
    int32_t textureBackID;
    int32_t tipsID;

  public:
    static int32_t numberOfTips;
    static int32_t currentTips;
};
