//============================================================================================
//    Spirenkov Maxim, 2003
//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
//    LocRat
//--------------------------------------------------------------------------------------------
//
//============================================================================================

#pragma once

#include "loc_life.h"

class LocRat : public LocLife
{
  public:
    LocRat();
    ~LocRat() override;

  private:
    const char *GetModelName() override;
    const char *GetAniName() override;
    bool PostInit(Animation *ani) override;

    void IdleProcess(Animation *ani, float dltTime) override;
    void MoveProcess(Animation *ani, float dltTime) override;
    void IsStartMove(Animation *ani) override;
    void IsStopMove(Animation *ani) override;

    void Event(Animation *animation, int32_t index, int32_t eventID, AnimationEvent event) override;

    void StartNewAction(Animation *ani);

  private:
    float nextActionTime;
    float timeout;
    float moveTime;
    bool isPanic;
};
