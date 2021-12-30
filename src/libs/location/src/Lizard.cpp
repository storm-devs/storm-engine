//============================================================================================
//    Spirenkov Maxim
//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
//    Lizard
//--------------------------------------------------------------------------------------------
//
//============================================================================================

#include "lizard.h"

Lizard::Lizard()
{
    lastMove = 10.0f + rand() * (200.0f / RAND_MAX);
    kSpeed = 1.2f;
    speed = 0.6f;
}

Lizard::~Lizard()
{
}

const char *Lizard::GetModelName()
{
    return "Animals\\lizard";
}

const char *Lizard::GetAniName()
{
    return "lizard";
}

bool Lizard::PostInit(Animation *ani)
{
    if (!ani->Player(0).SetAction("run"))
        return false;
    if (!ani->Player(0).Play())
        return false;
    ani->Player(0).Pause();
    return true;
}

void Lizard::IdleProcess(Animation *ani, float dltTime)
{
    // Check if need to start moving
    lastMove -= dltTime;
    if (lastMove < 0.0f)
    {
        kSpeed = 1.2f + rand() * (0.8f / RAND_MAX);
        StartMove();
        lastMove = 0.0f;
    }
    else
    {
        // check if close to the player
        if (IsNearPlayer(1.0f))
        {
            kSpeed = 2.4f;
            StartMove();
        }
    }
}

void Lizard::MoveProcess(Animation *ani, float dltTime)
{
}

void Lizard::IsStartMove(Animation *ani)
{
    ani->Player(0).Play();
    ani->Player(0).SetSpeed(kSpeed);
}

void Lizard::IsStopMove(Animation *ani)
{
    ani->Player(0).Pause();
    lastMove = 10.0f + rand() * (200.0f / RAND_MAX);
}
