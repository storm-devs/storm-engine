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

#include "Animation.h"

#define ANITM_PLAYERSMAX ((ANI_MAX_ACTIONS + 31) >> 5)

class AnimationImp;

class AnimationTimerImp : public AnimationTimer
{
    // --------------------------------------------------------------------------------------------
    // Construction, destruction
    // --------------------------------------------------------------------------------------------
  public:
    AnimationTimerImp();
    virtual ~AnimationTimerImp();

    // Set animation
    void SetAnimation(AnimationImp *animation);

    //--------------------------------------------------------------------------------------------
    // AnimationTimer
    //--------------------------------------------------------------------------------------------
  public:
    // Start timer (time in milliseconds)
    virtual void Start(float time, float startTime = 0.0f);
    // Stop timer
    virtual void Stop();
    // Reset timer state
    virtual void ResetTimer();
    // Find out if the timer is running
    virtual bool IsWork();
    // Get the current value
    virtual float GetCurrent() const;
    // Get the specified timer running time
    virtual float GetTime() const;
    // Assign ActionPlayer for blending (isInverse == false -> kBlend = [0..1])
    virtual void SetPlayer(long playerIndex, bool isInverse = false);
    virtual void ResetPlayer(long playerIndex);
    // Find out if ActionPlayer is being used
    virtual bool IsUsedPlayer(long playerIndex, bool *isInverse = nullptr);
    // Get the blending value for the player (if not used then 1.0f)
    virtual float GetPlayerValue(long playerIndex);

    //--------------------------------------------------------------------------------------------
    // AnimationTimerImp
    //--------------------------------------------------------------------------------------------
  public:
    // Execute
    void Execute(long dltTime);

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
