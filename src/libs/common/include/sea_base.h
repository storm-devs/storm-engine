#pragma once

#include "sd2_h/cannon_trace.h"
#include "v_module_api.h"

class SEA_BASE : public CANNON_TRACE_BASE
{
  public:
    virtual float WaveXZ(float x, float z, CVECTOR *vNormal = nullptr) = 0;
};
