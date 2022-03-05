// ============================================================================================
// Spirenkov Maxim aka Sp-Max Shaman, 2001
// --------------------------------------------------------------------------------------------
// Storm engine v2.00
// --------------------------------------------------------------------------------------------
// AnimationImp
// --------------------------------------------------------------------------------------------
// Implementing the Animation interface
// ============================================================================================

#pragma once

#include "action_player_imp.h"
#include "animation.h"
#include "animation_info.h"
#include "animation_timer_imp.h"

#define ANIIMP_MAXLISTENERS 8

class AnimationServiceImp;

class AnimationImp final : public Animation
{
    // --------------------------------------------------------------------------------------------
    // Construction, destruction
    // --------------------------------------------------------------------------------------------
  public:
    AnimationImp(int32_t id, AnimationInfo *animationInfo);
    ~AnimationImp() override;

    // Set pointer to animation service
    static void SetAnimationService(AnimationServiceImp *animationService);

    //--------------------------------------------------------------------------------------------
    // Animation
    //--------------------------------------------------------------------------------------------
  public:
    // Access the action player
    ActionPlayer &Player(int32_t index) override;
    // Access the animation timer
    AnimationTimer &Timer(int32_t index) override;
    // Events
    // Set internal event
    int32_t SetEvent(AnimationEvent event, int32_t index, AnimationEventListener *ael) override;
    // Delete internal event
    void DelEvent(int32_t eventID) override;
    // Set an external event handler
    void SetEventListener(AnimationEventListener *ael) override;
    // Access to bones
    // Get the number of bones in a skeleton
    int32_t GetNumBones() const override;
    // Get animation matrix for bone
    CMatrix &GetAnimationMatrix(int32_t iBone) const override;
    // misc
    // Get custom data for animation
    const char *GetData(const char *dataName) const override;
    // Copy the state of one player to another
    void CopyPlayerState(int32_t indexSrc, int32_t indexDst, bool copyTimerState = false) override;
    // Get animation speed
    float GetFPS() override;
    // Set blending modes
    // Automatic normalization of blending coefficients
    bool SetAutoNormalize(bool isNormalize = true) override;
    bool GetAutoNormalize() override;
    // Allow custom blending coefficients in ActionPlayer
    bool UserBlend(bool isBlend = true) override;
    bool IsUserBlend() override;
	// Procedural head look
    bool HeadControl(bool isControllable) override;
    bool IsControllableHead() override;
    void RotateHead(float x, float y) override;

    //--------------------------------------------------------------------------------------------
    // AnimationImp
    //--------------------------------------------------------------------------------------------
  public:
    // Get thisID
    int32_t GetThisID();
    // Get pointer to AnimationInfo
    AnimationInfo *GetAnimationInfo();
    // Find action by name
    ActionInfo *GetActionInfo(const char *actionName);
    // Take a step in time
    void Execute(int32_t dltTime);
    // Calculate animation matrices
    void BuildAnimationMatrices();
    // Get a pointer to the animation srvis
    static AnimationServiceImp *GetAniService();
    // AnimationPlayer events
    // A new action has been set for the player
    void ApeSetnewaction(int32_t index);
    // Action playback enabled
    void ApePlay(int32_t index);
    // Paused playing an action
    void ApePauseOn(int32_t index);
    // Continued playing action
    void ApePauseOff(int32_t index);
    // Stop playing an action
    void ApeStop(int32_t index);
    // Generate event on every frame
    void ApeAlways(int32_t index);
    // On the action starts
    void ApeStart(int32_t index);
    // On the action ends
    void ApeEnd(int32_t index);
    // On changing the direction of playback (pingpong)
    void ApeChange(int32_t index);
    // Timer has ended or has been stopped
    void AteTimerstop(int32_t index);

    // External event
    void AteExtern(int32_t plIndex, const char *evt);

  private:
    // Send events
    void SendEvent(AnimationEvent event, int32_t index);

    // --------------------------------------------------------------------------------------------
    // Encapsulation
    // --------------------------------------------------------------------------------------------
  private:
    // identifier
    int32_t thisID;
    // Animation pointer
    AnimationInfo *aniInfo;
    // Animations
    ActionPlayerImp action[ANI_MAX_ACTIONS];
    // Timers
    AnimationTimerImp timer[ANI_MAX_ACTIONS];
    // Auto normalization
    bool isAutoNormalize;
    // Custom blending
    bool isUserBlend;
    // Skeleton matrices
    CMatrix *matrix;
    // Internal event subscribers
    AnimationEventListener *ae_listeners[ae_numevents][ANIIMP_MAXLISTENERS];
    // Subscribers to external events
    AnimationEventListener *ae_listenersExt;
    // Animation Service Pointer
    static AnimationServiceImp *aniService;
	// Procedural head look
    bool isControllableHead;
    int32_t headBoneIndex;
    float customHeadAX;
    float customHeadAY;
};

//============================================================================================
// inline
//============================================================================================

// Set pointer to animation service
inline void AnimationImp::SetAnimationService(AnimationServiceImp *animationService)
{
    Assert(aniService == nullptr);
    aniService = animationService;
}

//--------------------------------------------------------------------------------------------
// AnimationImp
//--------------------------------------------------------------------------------------------

// Get thisID
inline int32_t AnimationImp::GetThisID()
{
    return thisID;
}

// Get pointer to AnimationInfo
inline AnimationInfo *AnimationImp::GetAnimationInfo()
{
    return aniInfo;
}

// Find action by name
inline ActionInfo *AnimationImp::GetActionInfo(const char *actionName)
{
    return aniInfo->FindAction(actionName);
}

// Get a pointer to the animation service
inline AnimationServiceImp *AnimationImp::GetAniService()
{
    return aniService;
}

// AnimationPlayer events
// A new action has been set for the player
inline void AnimationImp::ApeSetnewaction(int32_t index)
{
    SendEvent(ae_setnewaction, index);
}

// Action playback enabled
inline void AnimationImp::ApePlay(int32_t index)
{
    SendEvent(ae_play, index);
}

// Paused playing an action
inline void AnimationImp::ApePauseOn(int32_t index)
{
    SendEvent(ae_pauseon, index);
}

// Continued playing an action
inline void AnimationImp::ApePauseOff(int32_t index)
{
    SendEvent(ae_pauseoff, index);
}

// Stop playing an action
inline void AnimationImp::ApeStop(int32_t index)
{
    SendEvent(ae_stop, index);
}

// When the action starts
inline void AnimationImp::ApeStart(int32_t index)
{
    SendEvent(ae_start, index);
}

// When the action ends
inline void AnimationImp::ApeEnd(int32_t index)
{
    SendEvent(ae_end, index);
}

// When changing the direction of playback (pingpong)
inline void AnimationImp::ApeChange(int32_t index)
{
    SendEvent(ae_change, index);
}

// Timer has ended or has been stopped
inline void AnimationImp::AteTimerstop(int32_t index)
{
    SendEvent(ae_timerstop, index);
}

// External event
inline void AnimationImp::AteExtern(int32_t plIndex, const char *evt)
{
    if (ae_listenersExt)
        ae_listenersExt->Event(this, plIndex, evt);
}
