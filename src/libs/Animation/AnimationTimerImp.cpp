// ============================================================================================
// Spirenkov Maxim aka Sp-Max Shaman, 2001
// --------------------------------------------------------------------------------------------
// Storm engine v2.00
// --------------------------------------------------------------------------------------------
// AnimationTimerImp
// --------------------------------------------------------------------------------------------
// Animation timer implementation
// ============================================================================================

#include "AnimationTimerImp.h"
#include "AnimationImp.h"

// ============================================================================================
// Construction, destruction
// ============================================================================================

AnimationTimerImp::AnimationTimerImp()
{
    ani = nullptr;
    for (long i = 0; i < ANITM_PLAYERSMAX; i++)
    {
        playersMask[i] = 0;
        inverseMask[i] = 0;
    }
    curTime = 0.0f;
    kTime = 0.0f;
    value = 1.0f;
}

AnimationTimerImp::~AnimationTimerImp()
{
}

// Set animation
void AnimationTimerImp::SetAnimation(AnimationImp *animation)
{
    Assert(!ani && animation);
    ani = animation;
}

//--------------------------------------------------------------------------------------------
// AnimationTimer
//--------------------------------------------------------------------------------------------

// Start timer (time in milliseconds)
void AnimationTimerImp::Start(float time, float startTime)
{
    if (startTime < 0)
        startTime = 0;
    if (startTime >= time || time <= 0)
    {
        Stop();
        return;
    }
    curTime = startTime;
    kTime = 1.0f / time;
    value = 0.0f;
    // if(value < 0.0f) value = 0.0f;
    // if(value > 1.0f) value = 1.0f;
    Execute(0);
}

// Stop timer
void AnimationTimerImp::Stop()
{
    curTime = 0.0f;
    kTime = 0.0f;
    value = 1.0f;
    // Set end values and stop animation where asked
    long i;
    for (i = 0; i < ANI_MAX_ACTIONS; i++)
    {
        if (playersMask[i >> 5] & (1 << (i & 31)))
        {
            if (inverseMask[i >> 5] & (1 << (i & 31)))
            {
                static_cast<ActionPlayerImp *>(&ani->Player(i))->TimerBlend() = 0.0f;
                if (ani->Player(i).IsAutoStop())
                    ani->Player(i).Stop();
            }
            else
            {
                static_cast<ActionPlayerImp *>(&ani->Player(i))->TimerBlend() = 1.0f;
            }
        }
    }
    // send an event
    for (i = 0; i < ANI_MAX_ACTIONS; i++)
    {
        if (&ani->Timer(i) == this)
            break;
    }
    Assert(i < ANI_MAX_ACTIONS);
    ani->AteTimerstop(i);
}

// Reset timer state
void AnimationTimerImp::ResetTimer()
{
    curTime = 0.0f;
    kTime = 0.0f;
    value = 0.0f;
    for (long i = 0; i < ANITM_PLAYERSMAX; i++)
    {
        playersMask[i] = 0;
        inverseMask[i] = 0;
    }
}

// Find out if the timer is running
bool AnimationTimerImp::IsWork()
{
    return kTime != 0.0f;
}

// Get the current value
float AnimationTimerImp::GetCurrent() const
{
    return value;
}

// Get the specified timer running time
float AnimationTimerImp::GetTime() const
{
    return (kTime > 0.0f) ? 1.0f / kTime : 0.0f;
}

// Assign ActionPlayer for blending (isInverse == false -> kBlend = [0..1])
void AnimationTimerImp::SetPlayer(long playerIndex, bool isInverse)
{
    Assert(playerIndex >= 0 && playerIndex < ANI_MAX_ACTIONS);
    // set ourselves
    playersMask[playerIndex >> 5] |= 1 << (playerIndex & 31);
    // Determine what to use
    if (isInverse)
    {
        inverseMask[playerIndex >> 5] |= 1 << (playerIndex & 31);
    }
    else
    {
        inverseMask[playerIndex >> 5] &= ~(1 << (playerIndex & 31));
    }
    static_cast<ActionPlayerImp *>(&ani->Player(playerIndex))->TimerBlend() = 1.0f;
}

void AnimationTimerImp::ResetPlayer(long playerIndex)
{
    Assert(playerIndex >= 0 && playerIndex < ANI_MAX_ACTIONS);
    // Uncheck the use box
    playersMask[playerIndex >> 5] &= ~(1 << (playerIndex & 31));
    static_cast<ActionPlayerImp *>(&ani->Player(playerIndex))->TimerBlend() = 1.0f;
}

// Find out if ActionPlayer is being used
bool AnimationTimerImp::IsUsedPlayer(long playerIndex, bool *isInverse)
{
    Assert(playerIndex >= 0 && playerIndex < ANI_MAX_ACTIONS);
    if (playersMask[playerIndex >> 5] & (1 << (playerIndex & 31)))
    {
        if (isInverse)
            *isInverse = (inverseMask[playerIndex >> 5] & (1 << (playerIndex & 31))) != 0;
        return true;
    }
    return false;
}

// Get the blending value for the player (if not used then 1.0f)
float AnimationTimerImp::GetPlayerValue(long playerIndex)
{
    Assert(playerIndex >= 0 && playerIndex < ANI_MAX_ACTIONS);
    if (playersMask[playerIndex >> 5] & (1 << (playerIndex & 31)))
    {
        if ((inverseMask[playerIndex >> 5] & (1 << (playerIndex & 31))) != 0)
        {
            return kTime != 0 ? 1.0f - value : 0.0f;
        }
        return kTime != 0 ? value : 1.0f;
    }
    return 1.0f;
}

//--------------------------------------------------------------------------------------------
// AnimationTimerImp
//--------------------------------------------------------------------------------------------

// Execute
void AnimationTimerImp::Execute(long dltTime)
{
    if (kTime != 0.0f)
    {
        // calculate
        curTime += dltTime * 0.001f;
        value = curTime * kTime;
        if (value > 1.0f)
            value = 1.0f;
        // Arranging blending coefficients
        for (long i = 0; i < ANI_MAX_ACTIONS; i++)
        {
            if (playersMask[i >> 5] & (1 << (i & 31)))
            {
                if (inverseMask[i >> 5] & (1 << (i & 31)))
                {
                    static_cast<ActionPlayerImp *>(&ani->Player(i))->TimerBlend() *= 1.0f - value;
                }
                else
                {
                    static_cast<ActionPlayerImp *>(&ani->Player(i))->TimerBlend() *= value;
                }
            }
        }
        // Maybe it's time to stop
        if (value == 1.0f)
            Stop();
    }
}
