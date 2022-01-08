//============================================================================================
//    Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
//    AnimationTimerImp
//--------------------------------------------------------------------------------------------
//
//============================================================================================

#pragma once

#include "animation.h"

#define ANITM_PLAYERSMAX ((ANI_MAX_ACTIONS + 31) >> 5)

class AnimationImp;

class AnimationTimerImp : public AnimationTimer
{
    // --------------------------------------------------------------------------------------------
    // Construction, destruction
    // --------------------------------------------------------------------------------------------
  public:
    AnimationTimerImp();
    ~AnimationTimerImp() override;

    // Set animation
    void SetAnimation(AnimationImp *animation);

    //--------------------------------------------------------------------------------------------
    // AnimationTimer
    //--------------------------------------------------------------------------------------------
  public:
    // Start timer (time in milliseconds)
    void Start(float time, float startTime = 0.0f) override;
    // Stop timer
    void Stop() override;
    // Reset timer state
    void ResetTimer() override;
    // Find out if the timer is running
    bool IsWork() override;
    // Get the current value
    float GetCurrent() const override;
    // Get the specified timer running time
    float GetTime() const override;
    // Assign ActionPlayer for blending (isInverse == false -> kBlend = [0..1])
    void SetPlayer(int32_t playerIndex, bool isInverse = false) override;
    void ResetPlayer(int32_t playerIndex) override;
    // Find out if ActionPlayer is being used
    bool IsUsedPlayer(int32_t playerIndex, bool *isInverse = nullptr) override;
    // Get the blending value for the player (if not used then 1.0f)
    float GetPlayerValue(int32_t playerIndex) override;

    //--------------------------------------------------------------------------------------------
    // AnimationTimerImp
    //--------------------------------------------------------------------------------------------
  public:
    // Execute
    void Execute(int32_t dltTime);

    // --------------------------------------------------------------------------------------------
    // Encapsulation
    // --------------------------------------------------------------------------------------------
  private:
    // Animation
    AnimationImp *ani;
    // Timer options
    float curTime;
    float kTime;
    float value;
    // Bitmask showing what to set for whom
    uint32_t playersMask[ANITM_PLAYERSMAX];
    uint32_t inverseMask[ANITM_PLAYERSMAX];
};
