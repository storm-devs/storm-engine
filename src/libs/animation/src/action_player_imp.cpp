//============================================================================================
//    Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//    Storm engine v2.00
//--------------------------------------------------------------------------------------------
//    ActionPlayerImp
//--------------------------------------------------------------------------------------------
//
//============================================================================================

#include "animation_imp.h"

#include "storm/string_compare.hpp"

// ============================================================================================
// Construction, destruction
// ============================================================================================

ActionPlayerImp::ActionPlayerImp()
{
    ani = nullptr;
    playerIndex = -1;
    action = nullptr;
    isPlay = false;
    isPause = false;
    isAutostop = true;
    anitype = at_normal;
    speed = 1.0f;
    kspeed = 1.0f;
    isLoop = false;
    kBlend = 1.0f;
    kBlendTimer = 1.0f;
    ResetEventsMask();
}

ActionPlayerImp::~ActionPlayerImp()
{
}

// Set pointer to animation manager
void ActionPlayerImp::SetAnimation(AnimationImp *animation, int32_t index)
{
    Assert(!ani);
    ani = animation;
    playerIndex = index;
}

//--------------------------------------------------------------------------------------------
// ActionPlayer
//--------------------------------------------------------------------------------------------

// Set current action
bool ActionPlayerImp::SetAction(const char *actionName)
{
    if (actionName && action && storm::iEquals(action->GetName(), actionName))
        return true;
    action = nullptr;
    isPlay = isPause = false;
    isAutostop = true;
    kBlendTimer = 1.0f;
    ResetPosition();
    if (!actionName || actionName[0] == 0)
    {
        ani->ApeSetnewaction(playerIndex);
        return false;
    }
    action = ani->GetActionInfo(actionName);
    if (action == nullptr)
    {
        ani->ApeSetnewaction(playerIndex);
        return false;
    }
    // Reading parameters
    if (action->startFrame != action->endFrame)
        speed = 0.001f * ani->GetFPS() / (action->endFrame - action->startFrame);
    else
        speed = 0.0f;
    kspeed = action->kRate;
    anitype = action->type;
    isLoop = action->isLoop;
    // expose a position
    ResetPosition();
    // inform about the change
    ani->ApeSetnewaction(playerIndex);
    return true;
}

const char *ActionPlayerImp::GetAction() const
{
    if (action)
        return action->GetName();
    return nullptr;
}

// Play control
bool ActionPlayerImp::Play()
{
    if (!action)
        return false;
    if (isPlay && !isPause)
        return true;
    isPlay = true;
    auto p = isPause;
    if (!isPause)
    {
        ani->ApePlay(playerIndex);
        ani->ApeStart(playerIndex);
    }
    else
        ani->ApePauseOff(playerIndex);
    isPause = false;
    return true;
}

void ActionPlayerImp::Pause()
{
    isPause = true;
    ani->ApePauseOn(playerIndex);
}

void ActionPlayerImp::Stop()
{
    ResetPosition();
    isPlay = false;
    isPause = false;
    ani->ApeStop(playerIndex);
}

void ActionPlayerImp::SysStop()
{
    if (isPause)
        return;
    isPlay = false;
    isPause = false;
    ani->ApeStop(playerIndex);
    if (position < 0.0f)
        position = 0.0f;
    if (position > 1.0f)
        position = 1.0f;
    ResetEventsMask();
}

bool ActionPlayerImp::IsPlaying() const
{
    return isPlay;
}

bool ActionPlayerImp::IsPause() const
{
    return isPause;
}

// Auto stop when timer expires
bool ActionPlayerImp::SetAutoStop(bool isStop)
{
    const auto old = isAutostop;
    isAutostop = isStop;
    return old;
}

bool ActionPlayerImp::IsAutoStop() const
{
    return isAutostop;
}

// Current playing position
float ActionPlayerImp::SetPosition(float position)
{
    const auto pos = this->position;
    if (position < 0.0f)
        position = 0.0f;
    if (position > 1.0f)
        position = 1.0f;
    this->position = position;
    return pos;
}

float ActionPlayerImp::GetPosition() const
{
    return position;
}

// Play type
void ActionPlayerImp::SetType(AnimationType atype)
{
    anitype = atype;
}

AnimationType ActionPlayerImp::GetType() const
{
    return anitype;
}

// Playback speed coefficient
float ActionPlayerImp::SetSpeed(float kSpeed)
{
    if (kSpeed < 0.0f)
        kSpeed = 0.0f;
    if (kSpeed > 10.0f)
        kSpeed = 10.0f;
    const auto ks = kspeed;
    kspeed = kSpeed;
    return ks;
}

float ActionPlayerImp::GetSpeed() const
{
    return kspeed;
}

float ActionPlayerImp::GetDefSpeed() const
{
    if (action)
        return action->kRate;
    return 0.0f;
}

// Get the duration of an action in milliseconds
int32_t ActionPlayerImp::GetFrames() const
{
    if (action)
        return action->GetFrames();
    return 0;
}

// Set blending coefficient 0..1
void ActionPlayerImp::SetBlend(float k)
{
    if (k < 0.0f)
        k = 0.0f;
    if (k > 1.0f)
        k = 1.0f;
    kBlend = k;
}

// Get a blending coefficient 0..1
float ActionPlayerImp::GetBlend()
{
    return kBlend;
}

// Get user data for this action
const char *ActionPlayerImp::GetData(const char *dataName) const
{
    if (!action)
        return nullptr;
    const auto &userData = action->GetUserData();
    const auto it = userData.find(dataName);
    return it != userData.end() ? it->second.c_str() : nullptr;
}

//--------------------------------------------------------------------------------------------
// AnimationImp
//--------------------------------------------------------------------------------------------

// Take a step in time
void ActionPlayerImp::Execute(int32_t dltTime)
{
    kBlendTimer = 1.0f;
    if (!action || !isPlay || isPause || anitype == at_static)
        return;
    const auto dlt = dltTime * speed * kspeed;
    switch (anitype)
    {
    case at_static:
        break;
    case at_normal:
        MoveNormal(dlt);
        break;
    case at_reverse:
        MoveReverse(dlt);
        break;
    case at_pingpong:
        MovePingpong(dlt);
        break;
    case at_rpingpong:
        MoveRPingpong(dlt);
        break;
    default:
        throw std::runtime_error("ActionPlayerImp::Execute -> anknow animation type");
    }
    if (isPlay)
        CheckEvents();
    else
        ResetPosition();
}

// Set position to the very beginning
void ActionPlayerImp::ResetPosition()
{
    position = 0.0f;
    dir = true;
    if (action && (anitype == at_reverse || anitype == at_rpingpong))
    {
        position = 1.0f;
        dir = false;
    }
    ResetEventsMask();
}

// Get the current time
float ActionPlayerImp::GetCurrentFrame()
{
    if (!action)
        return 0;
    return action->startFrame + position * (action->endFrame - action->startFrame);
}

// Copy the state of another player
void ActionPlayerImp::CopyState(ActionPlayerImp &from)
{
    // Information about the current action
    action = from.action;
    // Animation playback type
    anitype = from.anitype;
    // Playing
    isPlay = from.isPlay;
    isPause = from.isPause;
    // Automatic stop
    isAutostop = from.isAutostop;
    // Play position
    position = from.position;
    // Direction of movement: true - normal, otherwise - reverse
    dir = from.dir;
    // Play speed
    speed = from.speed;
    // Playback speed coefficient
    kspeed = from.kspeed;
    // loop
    isLoop = from.isLoop;
    // A mask for past events
    for (int32_t i = 0; i < ACTIONPLAYEREVENTFLAGS; i++)
        eventsMask[i] = from.eventsMask[i];
    // Coefficient of blending
    kBlend = from.kBlend;
    // Timer blending scaling factor
    kBlendTimer = from.kBlendTimer;
}

// --------------------------------------------------------------------------------------------
// Encapsulation
// --------------------------------------------------------------------------------------------

// Position movement
void ActionPlayerImp::MoveNormal(float dlt)
{
    position += dlt;
    if (position >= 1.0f)
    {
        ani->ApeEnd(playerIndex);
        if (isLoop)
        {
            ResetEventsMask();
            position -= static_cast<float>(static_cast<int32_t>(position));
            ani->ApeStart(playerIndex);
        }
        else
            SysStop();
    }
}

void ActionPlayerImp::MoveReverse(float dlt)
{
    position -= dlt;
    if (position <= 0.0f)
    {
        ani->ApeEnd(playerIndex);
        if (isLoop)
        {
            ResetEventsMask();
            position -= static_cast<int32_t>(position) - 1.0f;
            ani->ApeStart(playerIndex);
        }
        else
            SysStop();
    }
}

void ActionPlayerImp::MovePingpong(float dlt)
{
    if (dir)
    {
        // Forward movement
        position += dlt;
        if (position >= 1.0f)
        {
            // change direction
            position = 1.0f - (position - static_cast<float>(static_cast<int32_t>(position)));
            dir = false;
            ani->ApeChange(playerIndex);
        }
    }
    else
    {
        // Reverse movement
        position -= dlt;
        if (position < 0.0f)
        {
            ani->ApeEnd(playerIndex);
            if (isLoop)
            {
                // change direction
                ResetEventsMask();
                position = -(position - static_cast<int32_t>(position));
                dir = true;
                ani->ApeStart(playerIndex);
            }
            else
                SysStop();
        }
    }
}

void ActionPlayerImp::MoveRPingpong(float dlt)
{
    if (!dir)
    {
        // Reverse movement
        position -= dlt;
        if (position < 0.0f)
        {
            ResetEventsMask();
            // change direction
            position = -(position - static_cast<int32_t>(position));
            dir = true;
            ani->ApeChange(playerIndex);
        }
    }
    else
    {
        // Direct movement
        position += dlt;
        if (position >= 1.0f)
        {
            ani->ApeEnd(playerIndex);
            if (isLoop)
            {
                ResetEventsMask();
                // change direction
                position = 1.0f - (position - static_cast<float>(static_cast<int32_t>(position)));
                dir = false;
                ani->ApeStart(playerIndex);
            }
            else
                SysStop();
        }
    }
}

// Check events and initiate if necessary
void ActionPlayerImp::CheckEvents()
{
    if (!action)
        return;
    const auto num = action->GetNumEvents();
    for (int32_t i = 0; i < num; i++)
    {
        const int32_t mask = 1 << (i & 31);
        if (eventsMask[i >> 5] & mask)
            continue;
        if (action->CheckEvent(i, position, dir))
        {
            eventsMask[i >> 5] |= mask;
            // send a custom event
            ani->AteExtern(playerIndex, action->EventName(i));
        }
    }
}

// Clearing the flags of the events that have occurred
void ActionPlayerImp::ResetEventsMask()
{
    for (int32_t i = 0; i < ACTIONPLAYEREVENTFLAGS; i++)
        eventsMask[i] = 0;
}
