#pragma once

#include "../object.h"

class CANNON_TRACE_BASE : public COLLISION_OBJECT
{
  public:
    virtual ~CANNON_TRACE_BASE() = default;

    virtual float Cannon_Trace(long iBallOwner, const CVECTOR &src, const CVECTOR &dst) = 0;
};
