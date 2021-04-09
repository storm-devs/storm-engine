// ============================================================================================
// Spirenkov Maxim aka Sp-Max Shaman, 2001
// --------------------------------------------------------------------------------------------
// Storm engine v2.00
// --------------------------------------------------------------------------------------------
// AnimationInfo
// --------------------------------------------------------------------------------------------
// This is where animation information is stored, loaded 1 time
// ============================================================================================

#pragma once

#include "ActionInfo.h"
#include "Bone.h"
#include "storm_assert.h"
#include <string>
#include <vector>

class AnimationInfo final
{
    // --------------------------------------------------------------------------------------------
    // Construction, destruction
    // --------------------------------------------------------------------------------------------
  public:
    AnimationInfo(const char *animationName);
    ~AnimationInfo();
    // Set the number of frames in the animation
    void SetNumFrames(long _numFrames);
    // Create bones
    void CreateBones(long numbones);
    // Create action
    ActionInfo *AddAction(const char *anctionName, long startframe, long endframe);
    // Set execution speed
    void SetFPS(float _fps);

    // --------------------------------------------------------------------------------------------
    // Working with animation
    // --------------------------------------------------------------------------------------------
  public:
    // Number of bones
    long NumBones();
    // Access to the bone
    Bone &GetBone(long iBone);
    // Compare with current name
    bool operator==(const char *animationName);
    // Increment reference count
    void AddRef();
    // Decrement reference count
    void RelRef();
    // Add downtime
    void AddDowntime(long dltTime);
    // Get downtime
    long GetDowntime();
    // Get animation name
    const char *GetName();
    // Find action by name
    ActionInfo *FindAction(const char *actionName);
    // Get animation time
    long GetAniNumFrames();
    // Access to user data
    std::unordered_map<std::string, std::string> &GetUserData();
    // Get animation speed
    float GetFPS();

    // --------------------------------------------------------------------------------------------
    // Encapsulation
    // --------------------------------------------------------------------------------------------
  private:
    char name[64]; // Animation name

    long numFrames; // The number of frames in the entire animation
    float fps;      // Frames per second

    Bone *bone;    // Bones with animation keys
    long numBones; // The number of bones in the skeleton

    std::vector<ActionInfo> actions; // Actions

    long refCounter; // Reference counter for this animation
    long downtime;   // Downtime

    std::unordered_map<std::string, std::string> userData; // User data
};

//============================================================================================
// inline
//============================================================================================

// Set the number of frames in the animation
inline void AnimationInfo::SetNumFrames(long _numFrames)
{
    if (_numFrames < 0)
        _numFrames = 0;
    numFrames = _numFrames;
}

// Set execution speed
inline void AnimationInfo::SetFPS(float _fps)
{
    if (_fps < 0.0f)
        fps = 0.0f;
    if (_fps > 1000000.0f)
        fps = 1000000.0f;
    fps = _fps;
}

// Number of bones
inline long AnimationInfo::NumBones()
{
    return numBones;
}

// Access to the bone
inline Bone &AnimationInfo::GetBone(long iBone)
{
    Assert(iBone >= 0 && iBone < numBones);
    return bone[iBone];
}

// Increment reference count
inline void AnimationInfo::AddRef()
{
    refCounter++;
}

// Decrement reference count
inline void AnimationInfo::RelRef()
{
    refCounter--;
    Assert(refCounter >= 0);
}

// Add downtime
inline void AnimationInfo::AddDowntime(long dltTime)
{
    if (refCounter == 0)
    {
        if (dltTime <= 10)
            dltTime = 10;
        downtime += dltTime;
    }
    else
        downtime = 0;
    if (downtime < 0)
        downtime = 0;
}

// Get downtime
inline long AnimationInfo::GetDowntime()
{
    return downtime;
}

// Get animation name
inline const char *AnimationInfo::GetName()
{
    return name;
}

// Get animation time
inline long AnimationInfo::GetAniNumFrames()
{
    return numFrames;
}

// Access to user data
inline std::unordered_map<std::string, std::string> &AnimationInfo::GetUserData()
{
    return userData;
}

// Get animation speed
inline float AnimationInfo::GetFPS()
{
    return fps;
}
