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

#include "Animation.h"

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
    void SetAnimation(AnimationImp *animation, long index);

    //--------------------------------------------------------------------------------------------
    // ActionPlayer
    //--------------------------------------------------------------------------------------------
  public:
    // Set current action
    virtual bool SetAction(const char *actionName);
    virtual const char *GetAction() const;
    // Play control
    virtual bool Play();
    virtual void Pause();
    virtual void Stop();
    virtual bool IsPlaying() const;
    virtual bool IsPause() const;
    // Auto stop when timer expires
    virtual bool SetAutoStop(bool isStop);
    virtual bool IsAutoStop() const;
    // Current playing position
    virtual float SetPosition(float position);
    virtual float GetPosition() const;
    // Play type
    virtual void SetType(AnimationType atype);
    virtual AnimationType GetType() const;
    // Playback speed coefficient
    virtual float SetSpeed(float kSpeed = 1.0f);
    virtual float GetSpeed() const;
    virtual float GetDefSpeed() const;
    // Get duration of action in frames
    virtual long GetFrames() const;
    // Set blending coefficient 0..1
    virtual void SetBlend(float k);
    // Get a blending coefficient 0..1
    virtual float GetBlend();
    // Get user data for this action
    virtual const char *GetData(const char *dataName) const;

    //--------------------------------------------------------------------------------------------
    // ActionPlayerImp
    //--------------------------------------------------------------------------------------------
  public:
    // Take a step in time
    void Execute(long dltTime);
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
    long playerIndex;
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
