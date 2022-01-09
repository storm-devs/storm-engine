// ============================================================================================
// Spirenkov Maxim aka Sp-Max Shaman, 2001
// --------------------------------------------------------------------------------------------
// Storm engine v2.00
// --------------------------------------------------------------------------------------------
// ActionInfo
// --------------------------------------------------------------------------------------------
// Information about one action
// ============================================================================================

#include "action_info.h"
#include "storm_assert.h"
#include "storm/string_compare.hpp"

// ============================================================================================
// Construction, destruction
// ============================================================================================

ActionInfo::ActionInfo(const char *aname, int32_t startframe, int32_t endframe)
{
    Assert(aname);
    Assert(aname[0] != '\0');
    Assert(strlen(aname) < 64);
    Assert(startframe >= 0);
    Assert(startframe <= endframe);
    strcpy_s(name, aname);
    startFrame = startframe;
    endFrame = endframe;
    kRate = 1.0f;
    type = at_normal;
    isLoop = false;
    for (auto i = 0; i < 8; i++)
        bonesMask[0] = 0xffffffff;
    numEvents = 0;
}

// Add event
bool ActionInfo::AddEvent(const char *ename, float frame, ExtAnimationEventType eventType)
{
    Assert(ename);
    Assert(strlen(ename) < 64);
    if (numEvents >= ANI_MAX_EVENTS || ename[0] == 0)
        return false;
    // calculate the relative time
    if (frame > static_cast<float>(endFrame))
        frame = static_cast<float>(endFrame);
    auto t = static_cast<float>(frame - startFrame);
    if (t < 0.0f)
        t = 0.0f;
    if (t > 0.0f)
        t /= endFrame - startFrame;
    if (t > 1.0f)
        t = 1.0f;
    // fill in the structure
    strcpy_s(event[numEvents].name, ename);
    event[numEvents].time = t;
    event[numEvents].event = eventType;
    numEvents++;
    return true;
}

// --------------------------------------------------------------------------------------------
// Working with action
// --------------------------------------------------------------------------------------------
// Compare with current name
bool ActionInfo::operator==(const char *actionName) const
{
    return storm::iEquals(actionName, name);
}

// check if the event generation condition is satisfied
bool ActionInfo::CheckEvent(int32_t index, float time, bool direction)
{
    Assert(index >= 0 && index < numEvents);
    switch (event[index].event)
    {
    case eae_always:
        if (direction)
            return time >= event[index].time;
        return time <= event[index].time;
    case eae_normal:
        if (!direction)
            return false;
        return time >= event[index].time;
    case eae_reverse:
        if (direction)
            return false;
        return time <= event[index].time;
    default:
        Assert(false);
    }
    return false;
}

// Get message name
const char *ActionInfo::EventName(int32_t index)
{
    Assert(index >= 0 && index < numEvents);
    return event[index].name;
}
