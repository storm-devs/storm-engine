#pragma once

#include "../object.h"

class CANNON_TRACE_BASE : public COLLISION_OBJECT
{
  public:
    ~CANNON_TRACE_BASE() override = default;

    virtual float Cannon_Trace(long iBallOwner, const Vector &src, const Vector &dst) = 0;
};
