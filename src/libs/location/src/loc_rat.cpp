//============================================================================================
//    Spirenkov Maxim, 2003
//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
//    LocRat
//--------------------------------------------------------------------------------------------
//
//============================================================================================

#include "loc_rat.h"

//============================================================================================

LocRat::LocRat()
    : timeout(0)
{
    nextActionTime = 1.0f + rand() * (3.0f / RAND_MAX);
    kSpeed = 1.2f;
    speed = 1.6f;
    moveTime = 0.0f;
    isPanic = false;
}

LocRat::~LocRat()
{
}

//============================================================================================

const char *LocRat::GetModelName()
{
    return "Animals\\rat";
}

const char *LocRat::GetAniName()
{
    return "rat";
}

bool LocRat::PostInit(Animation *ani)
{
    ani->SetEvent(ae_end, 0, this);
    if (!ani->Player(0).SetAction("idle_stand"))
        return false;
    if (!ani->Player(0).Play())
        return false;
    return true;
}

void LocRat::IdleProcess(Animation *ani, float dltTime)
{
    if (!IsNearPlayer(1.0f))
    {
        if (nextActionTime > 0.0f)
        {
            nextActionTime -= dltTime;
            if (nextActionTime <= 0.0f)
                StartNewAction(ani);
        }
        else
        {
            if (timeout > 0.0f)
            {
                timeout -= dltTime;
            }
            else
            {
                nextActionTime = 1.0f + rand() * (2.0f / RAND_MAX);
            }
        }
    }
    else
    {
        isPanic = true;
        StartMove();
    }
}

void LocRat::MoveProcess(Animation *ani, float dltTime)
{
    moveTime -= dltTime;
    if (moveTime > 0.0f)
    {
        if (isPanic)
        {
            if (!IsNearPlayer(4.0f))
            {
                isPanic = false;
                kSpeed = 1.0;
                ani->Player(0).SetAction("run");
                ani->Player(0).Play();
            }
        }
        else
        {
            if (IsNearPlayer(2.0f))
            {
                isPanic = true;
                kSpeed = 2.2f;
                ani->Player(0).SetAction("panic_run");
                ani->Player(0).Play();
                moveTime += 2.0f + rand() * (3.0f / RAND_MAX);
            }
        }
    }
    else
        StopMove();
}

void LocRat::IsStartMove(Animation *ani)
{
    moveTime = 3.0f + rand() * (10.0f / RAND_MAX);
    if (isPanic)
    {
        ani->Player(0).SetAction("panic_run");
        ani->Player(0).Play();
        ani->Player(0).SetSpeed(kSpeed);
        kSpeed = 2.2f;
    }
    else
    {
        ani->Player(0).SetAction("run");
        ani->Player(0).Play();
        ani->Player(0).SetSpeed(kSpeed);
        kSpeed = 1.0;
    }
}

void LocRat::IsStopMove(Animation *ani)
{
    moveTime = 0.0f;
    nextActionTime = 0.5f + rand() * (1.0f / RAND_MAX);
    ani->Player(0).Pause();
    isPanic = false;
    kSpeed = 1.0f;
}

void LocRat::Event(Animation *animation, int32_t index, int32_t eventID, AnimationEvent event)
{
    if (moveTime > 0.0f)
        return;
    if (!animation || index != 0 || event != ae_end)
        return;
    animation->Player(0).Pause();
    nextActionTime = 1.0f + rand() * (2.0f / RAND_MAX);
}

void LocRat::StartNewAction(Animation *ani)
{
    const auto p = rand() * (1.0f / RAND_MAX);
    if (p < 0.4f)
    {
        ani->Player(0).SetAction("idle");
        ani->Player(0).Play();
        ani->Player(0).SetSpeed(kSpeed);
    }
    else if (p < 0.6f)
    {
        ani->Player(0).SetAction("eat");
        ani->Player(0).Play();
        ani->Player(0).SetSpeed(kSpeed);
    }
    else if (p < 0.7f)
    {
        ani->Player(0).SetAction("idle_stand");
        ani->Player(0).Play();
        ani->Player(0).SetSpeed(kSpeed);
    }
    else
    {
        isPanic = false;
        StartMove();
    }
}
