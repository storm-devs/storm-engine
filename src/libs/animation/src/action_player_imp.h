//============================================================================================
//    Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//    Storm engine v2.00
//--------------------------------------------------------------------------------------------
//    ActionPlayerImp
//--------------------------------------------------------------------------------------------
//
//============================================================================================

#pragma once

#include "animation.h"

class AnimationImp;
class ActionInfo;

#define ACTIONPLAYEREVENTFLAGS ((ANI_MAX_EVENTS + 31) >> 5)

class ActionPlayerImp : public ActionPlayer
{
    // --------------------------------------------------------------------------------------------
    // Construction, destruction
    // --------------------------------------------------------------------------------------------
  public:
    ActionPlayerImp();
    virtual ~ActionPlayerImp();

    // Set pointer to animation
    void SetAnimation(AnimationImp *animation, int32_t index);

    //--------------------------------------------------------------------------------------------
    // ActionPlayer
    //--------------------------------------------------------------------------------------------
  public:
    // Set current action
    bool SetAction(const char *actionName) override;
    const char *GetAction() const override;
    // Play control
    bool Play() override;
    void Pause() override;
    void Stop() override;
    bool IsPlaying() const override;
    bool IsPause() const override;
    // Auto stop when timer expires
    bool SetAutoStop(bool isStop) override;
    bool IsAutoStop() const override;
    // Current playing position
    float SetPosition(float position) override;
    float GetPosition() const override;
    // Play type
    void SetType(AnimationType atype) override;
    AnimationType GetType() const override;
    // Playback speed coefficient
    float SetSpeed(float kSpeed = 1.0f) override;
    float GetSpeed() const override;
    float GetDefSpeed() const override;
    // Get duration of action in frames
    int32_t GetFrames() const override;
    // Set blending coefficient 0..1
    void SetBlend(float k) override;
    // Get a blending coefficient 0..1
    float GetBlend() override;
    // Get user data for this action
    const char *GetData(const char *dataName) const override;

    //--------------------------------------------------------------------------------------------
    // ActionPlayerImp
    //--------------------------------------------------------------------------------------------
  public:
    // Take a step in time
    void Execute(int32_t dltTime);
    // Set position to the very beginning
    void ResetPosition();
    // Get the current time
    float GetCurrentFrame();
    // Quick access to blending ratios
    float &Blend();
    float &TimerBlend();
    // Copy the state of another player
    void CopyState(ActionPlayerImp &from);

  public:
    // Current blending ratio
    float kBlendCurrent;

    // --------------------------------------------------------------------------------------------
    // Encapsulation
    // --------------------------------------------------------------------------------------------
  private:
    // Position movement
    void MoveNormal(float dlt);
    void MoveReverse(float dlt);
    void MovePingpong(float dlt);
    void MoveRPingpong(float dlt);
    // Check events and initiate if necessary
    void CheckEvents();
    // Clearing the flags of the events that have occurred
    void ResetEventsMask();
    // System stop
    void SysStop();

  private:
    // Animation for this action
    AnimationImp *ani;
    // Index of this player
    int32_t playerIndex;
    // Information about the current action
    ActionInfo *action;
    // Animation playback type
    AnimationType anitype;
    // Playing
    bool isPlay, isPause;
    // Automatic stop
    bool isAutostop;
    // Play position
    float position;
    // Direction of movement: true - normal, otherwise - reverse
    bool dir;
    // Play speed
    float speed;
    // Playback speed coefficient
    float kspeed;
    // loop
    bool isLoop;
    // A mask for past events
    uint32_t eventsMask[ACTIONPLAYEREVENTFLAGS];
    // Coefficient of blending
    float kBlend;
    // Timer blending scaling factor
    float kBlendTimer;
};

// Quick access to the blending factor
inline float &ActionPlayerImp::Blend()
{
    return kBlend;
}

inline float &ActionPlayerImp::TimerBlend()
{
    return kBlendTimer;
}
