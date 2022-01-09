//============================================================================================
//    Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
//    CharacterAnimationKipper
//--------------------------------------------------------------------------------------------
//
//============================================================================================

#include "character_animation_kipper.h"
#include "entity.h"
#include "core.h"
#include "dx9render.h"

// ============================================================================================
// Construction, destruction
// ============================================================================================

CharacterAnimationKipper::CharacterAnimationKipper()
{
    asr = nullptr;
    aniMan = nullptr;
    aniWoman = nullptr;
    for (int32_t i = 0; i < sizeof(lockTextures) / sizeof(int32_t); i++)
        lockTextures[i] = -1;
    numLTextures = 0;
    rs = nullptr;
}

CharacterAnimationKipper::~CharacterAnimationKipper()
{
    delete aniMan;
    delete aniWoman;
    if (rs)
    {
        for (int32_t i = 0; i < sizeof(lockTextures) / sizeof(int32_t); i++)
            if (lockTextures[i] >= 0)
                rs->TextureRelease(lockTextures[i]);
    }
}

// Initialization
bool CharacterAnimationKipper::Init()
{
    // check that the it's the only one
    const auto &entities = EntityManager::GetEntityIdVector("CharacterAnimationKipper");
    for (auto eid : entities)
    {
        if (EntityManager::GetEntityPointer(eid) == this)
            continue;

        core.Trace("CharacterAnimationKipper::Init() -> CharacterAnimationKipper already created");
        return false;
    }

    rs = static_cast<VDX9RENDER *>(core.GetService("dx9render"));
    if (!rs)
        throw std::runtime_error("No service: dx9render");
    auto *asr = static_cast<AnimationService *>(core.GetService("AnimationServiceImp"));
    if (!asr)
        throw std::runtime_error("Anumation service not created!");
    aniMan = asr->CreateAnimation("man");
    aniWoman = asr->CreateAnimation("towngirl");
    // LockTexture("dialog\dialog.tga");
    return true;
}

void CharacterAnimationKipper::LockTexture(const char *texture)
{
    if (numLTextures >= sizeof(lockTextures) / sizeof(int32_t))
        return;
    lockTextures[numLTextures++] = rs->TextureCreate(texture);
}
