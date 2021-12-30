#pragma once

#include "dx9render.h"
#include "entity.h"

///////////////////////////////////////////////////////////////////
// DEFINES & TYPES
///////////////////////////////////////////////////////////////////
// SoundPlay (..., type, ...)
#define SOUND_WAV_3D 1
#define SOUND_MP3_STEREO 2
#define SOUND_MP3_3D 3
#define SOUND_WAV_STEREO 4
// SoundSet3DParam(..., type, ...)
#define SOUND_PARAM_MAX_DISTANCE 1
#define SOUND_PARAM_MIN_DISTANCE 2
#define SOUND_PARAM_POSITION 3
#define SOUND_STRING_SIZE 512

///////////////////////////////////////////////////////////////////
// CLASS DEFINITION
///////////////////////////////////////////////////////////////////
class SOUND : public Entity
{
  public:
    SOUND();
    ~SOUND() override;

    bool Init() override;
    uint64_t ProcessMessage(MESSAGE &message) override;
    virtual void Realize(uint32_t dTime);

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

  private:
    VSoundService *soundService;
    VDX9RENDER *renderer;
};
