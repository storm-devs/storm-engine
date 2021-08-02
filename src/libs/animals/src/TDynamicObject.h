#pragma once

#include <vector>
#include <memory>

//#include <windows.h>
#include "Cvector.h"

#define ATTRACT_FACTOR 0.9f // 0.01f
#define DEFLECT_FACTOR 10.5f
#define MOVEMENT_FACTOR 0.012f // 0.015f

//--------------------------------------------------------------------
class TDynamicObject
{
  public:
    TDynamicObject() = default;
    virtual ~TDynamicObject() = default;
    void Initialize(const CVECTOR &_center, const float _radius);
    void Calculate(std::vector<TDynamicObject *> &a, std::vector<TDynamicObject *> &d, const float _k = 1.0f);
    void SetXYZ(const CVECTOR &_pos);

    float GetAngle()
    {
        return static_cast<float>(atan2(velocity.z, velocity.x));
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
