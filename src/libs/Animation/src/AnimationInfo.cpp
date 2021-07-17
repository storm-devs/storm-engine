// ============================================================================================
// Spirenkov Maxim aka Sp-Max Shaman, 2001
// --------------------------------------------------------------------------------------------
// Storm engine v2.00
// --------------------------------------------------------------------------------------------
// AnimationInfo
// --------------------------------------------------------------------------------------------
// This is where animation information is stored, loaded 1 time
// ============================================================================================

#include "AnimationInfo.h"

// ============================================================================================
// Construction, destruction
// ============================================================================================

AnimationInfo::AnimationInfo(const char *animationName)
{
    Assert(strlen(animationName) < 64);
    strcpy_s(name, animationName);
    bone = nullptr;
    numBones = 0;
    refCounter = 0;
    downtime = 0;
    fps = 15.0f;
}

AnimationInfo::~AnimationInfo()
{
    delete[] bone;
}

// Create bones
void AnimationInfo::CreateBones(long numbones)
{
    Assert(bone == nullptr || numBones == 0);
    Assert(numbones > 0 && numbones <= 256);
    numBones = numbones;
    bone = new Bone[numBones];
}

// Create action
ActionInfo *AnimationInfo::AddAction(const char *anctionName, long startframe, long endframe)
{
    Assert(anctionName);
    // Looking for repetition
    for (const auto &action : actions)
        if (action == anctionName)
            return nullptr;

    // It's okay - new action
    return &actions.emplace_back(anctionName, startframe, endframe);
}

// --------------------------------------------------------------------------------------------
// Working with animation
// --------------------------------------------------------------------------------------------

// Compare with current name
bool AnimationInfo::operator==(const char *animationName)
{
    return _stricmp(animationName, name) == 0;
}

// Find action by name
ActionInfo *AnimationInfo::FindAction(const char *actionName)
{
    for (auto &action : actions)
        if (action == actionName)
            return &action;
    return nullptr;
}
