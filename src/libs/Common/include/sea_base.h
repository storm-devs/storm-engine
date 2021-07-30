#pragma once

#include "Sd2_h/CannonTrace.h"

class SEA_BASE : public CANNON_TRACE_BASE
{
  public:
    virtual float WaveXZ(float x, float z, Vector *vNormal = nullptr) = 0;
};
