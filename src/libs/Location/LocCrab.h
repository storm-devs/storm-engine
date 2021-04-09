//============================================================================================
//    LocCrab
//============================================================================================

#pragma once

#include "LocLife.h"

class LocCrab : public LocLife
{
  public:
    LocCrab();
    virtual ~LocCrab();

  private:
    virtual const char *GetModelName();
    virtual const char *GetAniName();
    virtual bool PostInit(Animation *ani);

    virtual void IdleProcess(Animation *ani, float dltTime);
    virtual void MoveProcess(Animation *ani, float dltTime);
    virtual void IsStartMove(Animation *ani);
    virtual void IsStopMove(Animation *ani);

    virtual void Event(Animation *animation, long index, long eventID, AnimationEvent event);

    void StartNewAction(Animation *ani);

  private:
    float nextActionTime;
    float timeout;
    float moveTime;
    bool isPanic;
};
