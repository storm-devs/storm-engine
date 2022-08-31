#pragma once

#include <c_vector.h>
#include <entity.h>

class VPARTICLE_SYSTEM
{
  public:
    VPARTICLE_SYSTEM(){};

    virtual ~VPARTICLE_SYSTEM(){};
    virtual void Stop() = 0;
    virtual void SetEmitter(CVECTOR p, CVECTOR a) = 0;
    virtual void LinkToObject(entid_t id, CVECTOR _LinkPos) = 0;
    virtual void SetDelay(int32_t _delay) = 0;
    virtual void SetLifeTime(uint32_t time) = 0;

    virtual void StopEmitter(){};

    virtual void Pause(bool _bPause)
    {
    }
};
