#pragma once

#include "math3d.h"

#define ATTRACT_FACTOR 0.9f // 0.01f
#define DEFLECT_FACTOR 10.5f
#define MOVEMENT_FACTOR 0.012f // 0.015f

//--------------------------------------------------------------------
class TDynamicObject
{
  public:
    TDynamicObject();
    virtual ~TDynamicObject();
    void Initialize(const Vector &_center, float _radius);
    void Calculate(TDynamicObject **a, int aCount, TDynamicObject **d, int dCount, float _k = 1.0f);
    void SetXYZ(const Vector &_pos);

    float GetAngle()
    {
        return (float)atan2(velocity.z, velocity.x);
    }

    Vector GetDirection()
    {
        return velocity;
    }

    Vector GetXYZ()
    {
        return pos;
    }

    // void Draw(HDC _dc, DWORD _color);
  private:
    Vector pos, velocity;
    // float ang;
};

//--------------------------------------------------------------------
