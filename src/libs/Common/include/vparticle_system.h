#pragma once

#include "Entity.h"
#include "math3d/Vector.h"

class VPARTICLE_SYSTEM
{
  public:
    VPARTICLE_SYSTEM(){};

    virtual ~VPARTICLE_SYSTEM(){};
    virtual void Stop() = 0;
    virtual void SetEmitter(Vector p, Vector a) = 0;
    virtual void LinkToObject(entid_t id, Vector _LinkPos) = 0;
    virtual void SetDelay(long _delay) = 0;
    virtual void SetLifeTime(uint32_t time) = 0;

    virtual void StopEmitter(){};

    virtual void Pause(bool _bPause)
    {
    }
};
