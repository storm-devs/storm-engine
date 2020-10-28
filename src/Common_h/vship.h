#ifndef VSHIP_H
#define VSHIP_H
#include "ship_msg.h"
#include "vcannon.h"

class VSHIP : public ENTITY
{
  public:
    virtual void CauseDamage(float, float, float) = 0;
    virtual float GetHullPercent() = 0;
    virtual void *GetModel() = 0;
    virtual void *GetCannon() = 0;
};

#endif
