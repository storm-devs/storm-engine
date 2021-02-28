//============================================================================================
//	Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
//	CharacterAnimationKipper
//--------------------------------------------------------------------------------------------
//
//============================================================================================

#ifndef _CharacterAnimationKipper_H_
#define _CharacterAnimationKipper_H_

#include "Animation.h"
#include "Entity.h"
#include "vmodule_api.h"

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

    long lockTextures[16];
    long numLTextures;
};

#endif
