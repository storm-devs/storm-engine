// ============================================================================================
// Spirenkov Maxim aka Sp-Max Shaman, 2001
// --------------------------------------------------------------------------------------------
// Storm engine v2.00
// --------------------------------------------------------------------------------------------
// Animation
// --------------------------------------------------------------------------------------------
// Interface classes for working with animation
// ============================================================================================

#pragma once

//============================================================================================

#include "Matrix.h"
#include "service.h"

//============================================================================================

class AnimationPlayer;        // Action player
class AnimationTimer;         // Animation blending class
class Animation;              // A class that controls animations for one model
class AnimationEventListener; // A class that accepts internal animation events
class AnimationService;       // Animation service

// ============================================================================================
// Constants
// ============================================================================================

// Possible ways to play action animation
enum AnimationType
{
    at_static,
    // Frames do not change
    at_normal,
    // From start to finish
    at_reverse,
    // From end to start
    at_pingpong,
    // From start to finish, back again
    at_rpingpong,
    // From end to start, back again
};

// Events set from code to help control animation
enum AnimationEvent
{
    // Internal AnimationPlayer Events
    ae_setnewaction,
    // A new action has been set for the player
    ae_play,
    // Action playback enabled
    ae_pauseon,
    // Paused playing an action
    ae_pauseoff,
    // Continued playing action
    ae_stop,
    // Stop playing an action
    ae_start,
    // When the action starts
    ae_end,
    // When the action ends
    ae_change,
    // When changing the direction of playback (pingpong)
    // AnimationTimer
    ae_timerstop,
    // Timer has ended or has been stopped

    // Internal
    ae_numevents,
    // Number of events
};

#define ANI_MAX_ACTIONS 8 // Number of ActionPlayers and AnimationTimers for one model
#define ANI_MAX_EVENTS 8 // Number of events per action

// ============================================================================================
// The class that plays the action
// ============================================================================================
// Action or action animation - a fragment of a complete animation (walking, running, jumping, ...)
// containing all the animation of the character.
//
// Autostop is on by default and always on when a new action is set
// ============================================================================================

class ActionPlayer
{
  public:
    ~ActionPlayer()
    {
    }

    // Set current action
    virtual bool SetAction(const char *actionName) = 0;
    virtual const char *GetAction() const = 0;
    // Play control
    virtual bool Play() = 0;
    virtual void Pause() = 0;
    virtual void Stop() = 0;
    virtual bool IsPlaying() const = 0;
    virtual bool IsPause() const = 0;
    // Auto stop when timer expires
    virtual bool SetAutoStop(bool isStop) = 0;
    virtual bool IsAutoStop() const = 0;
    // Current playing position
    virtual float SetPosition(float position) = 0;
    virtual float GetPosition() const = 0;
    // Play type
    virtual void SetType(AnimationType atype) = 0;
    virtual AnimationType GetType() const = 0;
    // Playback speed coefficient
    virtual float SetSpeed(float kSpeed = 1.0f) = 0;
    virtual float GetSpeed() const = 0;
    virtual float GetDefSpeed() const = 0;
    // Get duration of action in frames
    virtual long GetFrames() const = 0;
    // Set blending coefficient 0..1
    virtual void SetBlend(float k) = 0;
    // Get a blending coefficient of 0..1
    virtual float GetBlend() = 0;
    // Get user data for this action
    virtual const char *GetData(const char *dataName) const = 0;
};

// ============================================================================================
// Animation blending class
// ============================================================================================
// This class allows to scale the coefficient
// blending in ActionPlayer
// The normal value ranges from 0 to 1,
// inverse from 1 to 0
// ============================================================================================

class AnimationTimer
{
  public:
    virtual ~AnimationTimer()
    {
    }

    // Start timer (time in seconds)
    virtual void Start(float time, float startTime = 0.0f) = 0;
    // Stop timer
    virtual void Stop() = 0;
    // Reset timer state
    virtual void ResetTimer() = 0;
    // Find out if the timer is running
    virtual bool IsWork() = 0;
    // Get the current value 0..1
    virtual float GetCurrent() const = 0;
    // Get the specified timer running time
    virtual float GetTime() const = 0;
    // Assign ActionPlayer for blending (isInverse == false --> kBlend = [0..1])
    virtual void SetPlayer(long playerIndex, bool isInverse = false) = 0;
    virtual void ResetPlayer(long playerIndex) = 0;
    // Find out if ActionPlayer is being used
    virtual bool IsUsedPlayer(long playerIndex, bool *isInverse = nullptr) = 0;
    // Get the blending value for the player (if not used then 1.0f)
    virtual float GetPlayerValue(long playerIndex) = 0;
};

// ============================================================================================
// A class that controls animations for one model
// ============================================================================================
// This class contains ANI_MAX_ACTIONS ActionPlayer and AnimationTimer objects
// ActionPlayer and AnimationTimer can be used arbitrarily, at will
// ============================================================================================

class Animation
{
  public:
    virtual ~Animation(){};
    // Access the action player
    virtual ActionPlayer &Player(long index) = 0;
    // Access the animation timer
    virtual AnimationTimer &Timer(long index) = 0;
    // Events
    // Set an internal event handler
    virtual long SetEvent(AnimationEvent event, long index, AnimationEventListener *ael) = 0;
    // Remove internal event handler
    virtual void DelEvent(long eventID) = 0;
    // Set an external event handler
    virtual void SetEventListener(AnimationEventListener *ael = nullptr) = 0;
    // Access to bones
    // Get the number of bones in a skeleton
    virtual long GetNumBones() const = 0;
    // Get animation matrix for bone
    virtual CMatrix &GetAnimationMatrix(long iBone) const = 0;
    // miscellanea
    // Get custom data for animation
    virtual const char *GetData(const char *dataName) const = 0;
    // Copy the state of one player to another
    virtual void CopyPlayerState(long indexSrc, long indexDst, bool copyTimerState = false) = 0;
    // Get animation speed
    virtual float GetFPS() = 0;
    // Set blending modes
    // Automatic normalization of blending coefficients
    virtual bool SetAutoNormalize(bool isNormalize = true) = 0;
    virtual bool GetAutoNormalize() = 0;
    // Allow custom blending coefficients in ActionPlayer
    virtual bool UserBlend(bool isBlend = true) = 0;
    virtual bool IsUserBlend() = 0;
};

// ============================================================================================
// A class that accepts internal animation events
// ============================================================================================

class AnimationEventListener
{
  public:
    virtual ~AnimationEventListener(){};
    // Accept event
    virtual void Event(Animation *animation, long index, long eventID, AnimationEvent event)
    {
    }

    // Accept event
    virtual void Event(Animation *animation, long playerIndex, const char *eventName)
    {
    }
};

// ============================================================================================
// Animation service
// ============================================================================================

class AnimationService : public SERVICE
{
  public:
    ~AnimationService() override
    {
    }

    // Create animation for the model, delete with "delete"
    virtual Animation *CreateAnimation(const char *animationName) = 0;
};

//============================================================================================
