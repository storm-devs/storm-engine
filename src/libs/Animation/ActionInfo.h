// ============================================================================================
// Spirenkov Maxim aka Sp-Max Shaman, 2001
// --------------------------------------------------------------------------------------------
// Storm engine v2.00
// --------------------------------------------------------------------------------------------
// ActionInfo
// --------------------------------------------------------------------------------------------
// Information about one action
// ============================================================================================

#pragma once

#include "Animation.h"
#include "Bone.h"
#include <string>
#include <unordered_map>

enum ExtAnimationEventType
{
    eae_always, // Always
    eae_normal, // When playing forward
    eae_reverse // Reverse playback
};

class ActionPlayerImp;

class ActionInfo final
{
    friend ActionPlayerImp;

    struct Event
    {
        char name[64];               // Event name
        float time;                  // Relative time of the event
        ExtAnimationEventType event; // Event type
    };

    // --------------------------------------------------------------------------------------------
    // Construction, destruction
    // --------------------------------------------------------------------------------------------
  public:
    ActionInfo(const char *aname, long startframe, long endframe);
    // Set playback speed ratio
    void SetRate(float rate);
    // Set animation type
    void SetAnimationType(AnimationType t);
    // Looped animation playback
    void SetLoop(bool isloop);
    // Add event
    bool AddEvent(const char *ename, float frame, ExtAnimationEventType event);

    // --------------------------------------------------------------------------------------------
    // Working with action
    // --------------------------------------------------------------------------------------------
  public:
    // Compare with current name
    bool operator==(const char *actionName) const;
    // Get action name
    const char *GetName();
    // Get the number of events
    long GetNumEvents();
    // check if the event generation condition is satisfied,
    // direction == true for normal direction
    bool CheckEvent(long index, float time, bool direction);
    // Get message name
    const char *EventName(long index);
    // Get duration in frames
    long GetFrames();
    // Access to user data
    std::unordered_map<std::string, std::string> &GetUserData();

    // --------------------------------------------------------------------------------------------
    // Encapsulation
    // --------------------------------------------------------------------------------------------
  private:
    char name[64]; // Action name

    long startFrame; // Starting position in the whole animation
    long endFrame;   // End position in the whole animation

    float kRate;        // Playback speed ratio
    AnimationType type; // Animation type
    bool isLoop;        // Looped animation playback

    uint32_t bonesMask[8]; // Bones used in animation

    Event event[ANI_MAX_EVENTS]; // Events
    long numEvents;              // Number of events

    std::unordered_map<std::string, std::string> userData; // User data
};

//============================================================================================
// inline
//============================================================================================

// Set playback speed ratio
inline void ActionInfo::SetRate(float rate)
{
    if (rate < 0.0f)
        rate = 0.0f;
    if (rate > 10.0f)
        rate = 10.0f;
    kRate = rate;
}

// Set animation type
inline void ActionInfo::SetAnimationType(AnimationType t)
{
    type = t;
}

// Looped animation playback
inline void ActionInfo::SetLoop(bool isloop)
{
    isLoop = isloop;
}

// --------------------------------------------------------------------------------------------
// Working with action
// --------------------------------------------------------------------------------------------

// Get action name
inline const char *ActionInfo::GetName()
{
    return name;
}

// Get the number of events
inline long ActionInfo::GetNumEvents()
{
    return numEvents;
}

// Get duration in frames
inline long ActionInfo::GetFrames()
{
    return endFrame - startFrame;
}

// Access to user data
inline std::unordered_map<std::string, std::string> &ActionInfo::GetUserData()
{
    return userData;
}
