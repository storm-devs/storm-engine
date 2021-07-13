#pragma once

//#include <windows.h>
#include "Cvector.h"

#define ATTRACT_FACTOR 0.9f // 0.01f
#define DEFLECT_FACTOR 10.5f
#define MOVEMENT_FACTOR 0.012f // 0.015f

//--------------------------------------------------------------------
class TDynamicObject
{
  public:
    TDynamicObject();
    virtual ~TDynamicObject();
    void Initialize(const CVECTOR &_center, float _radius);
    void Calculate(TDynamicObject **a, int aCount, TDynamicObject **d, int dCount, float _k = 1.0f);
    void SetXYZ(const CVECTOR &_pos);
    float GetAngle()
    {
        return (float)atan2(velocity.z, velocity.x);
    }
    CVECTOR GetDirection()
    {
        return velocity;
    }
    CVECTOR GetXYZ()
    {
        return pos;
    }

    // void Draw(HDC _dc, DWORD _color);
  private:
    CVECTOR pos, velocity;
    // float ang;
};
//--------------------------------------------------------------------
