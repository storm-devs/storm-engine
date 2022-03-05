//============================================================================================
//    Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
//    CharacterAnimationKipper
//--------------------------------------------------------------------------------------------
//
//============================================================================================

#pragma once

#include "animation.h"
#include "entity.h"
#include "v_module_api.h"

class VDX9RENDER;

class CharacterAnimationKipper : public Entity
{
    // --------------------------------------------------------------------------------------------
    // Construction, destruction
    // --------------------------------------------------------------------------------------------
  public:
    CharacterAnimationKipper();
    virtual ~CharacterAnimationKipper();

    // Initialization
    bool Init() override;
    void LockTexture(const char *texture);

    void ProcessStage(Stage, uint32_t) override
    {
    }

    // --------------------------------------------------------------------------------------------
    // Encapsulation
    // --------------------------------------------------------------------------------------------
  private:
    VDX9RENDER *rs;

    AnimationService *asr;
    Animation *aniMan;
    Animation *aniWoman;

    int32_t lockTextures[16];
    int32_t numLTextures;
};
