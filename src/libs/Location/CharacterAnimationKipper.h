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
#include "vmodule_api.h"

class VDX9RENDER;

class CharacterAnimationKipper : public Entity
{
    //--------------------------------------------------------------------------------------------
    //Конструирование, деструктурирование
    //--------------------------------------------------------------------------------------------
  public:
    CharacterAnimationKipper();
    virtual ~CharacterAnimationKipper();

    //Инициализация
    bool Init() override;
    void LockTexture(const char *texture);

    void ProcessStage(Stage, uint32_t) override
    {
    }

    //--------------------------------------------------------------------------------------------
    //Инкапсуляция
    //--------------------------------------------------------------------------------------------
  private:
    VDX9RENDER *rs;

    AnimationService *asr;
    Animation *aniMan;
    Animation *aniWoman;

    long lockTextures[16];
    long numLTextures;
};

#endif
