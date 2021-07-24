#pragma once

#include "Matrix.h"
#include "dx9render.h"
#include "triangle.h"

using ADD_POLYGON_FUNC = bool (*)(const CVECTOR *v, long nv);

class COLLISION_OBJECT : public Entity
{
  public:
    bool Init() override
    {
        return true;
    }

    ~COLLISION_OBJECT() override{};
    CMatrix mtx;

    virtual float Trace(const CVECTOR &src, const CVECTOR &dst) = 0;
    virtual bool Clip(const PLANE *planes, long nplanes, const CVECTOR &center, float radius,
                      ADD_POLYGON_FUNC addpoly) = 0;

    virtual const char *GetCollideMaterialName() = 0;
    virtual bool GetCollideTriangle(TRIANGLE &triangle) = 0;
};
