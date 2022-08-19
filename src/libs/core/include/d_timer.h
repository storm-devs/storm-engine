#pragma once
#include "math_inlines.h"

class DTimer
{
  private:
    float fCurrent, fMin, fMax;

  public:
    DTimer()
    {
        Setup(0.0f, 1.0f);
    };

    DTimer(float _fCurrent, float _fMax)
    {
        Setup(_fCurrent, _fMax);
    };

    ~DTimer(){};

    void Setup(float _fCurrent, float _fMin, float _fMax)
    {
        fCurrent = _fCurrent;
        fMin = _fMin;
        fMax = _fMax;
    }

    void Setup(float _fCurrent, float _fMax)
    {
        fCurrent = _fCurrent;
        fMin = _fMax;
        fMax = _fMax;
    }

    bool Update(uint32_t dwDeltaTime)
    {
        return Update(0.001f * static_cast<float>(dwDeltaTime));
    }

    bool Update(float fDeltaTime)
    {
        fCurrent -= fDeltaTime;
        if (fCurrent <= 0.0f)
        {
            fCurrent = fMin + FRAND(fMax - fMin);
            return true;
        }
        return false;
    }
};
