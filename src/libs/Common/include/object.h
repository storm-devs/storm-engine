#pragma once

#include "Matrix.h"
#include "dx9render.h"
#include "triangle.h"

typedef bool (*ADD_POLYGON_FUNC)(const CVECTOR *v, long nv);

class COLLISION_OBJECT : public Entity
{
  public:
    bool Init() override
    {
        return true;
    }

    virtual ~COLLISION_OBJECT(){};
    CMatrix mtx;

    virtual float Trace(const CVECTOR &src, const CVECTOR &dst) = 0;
    virtual bool Clip(const PLANE *planes, long nplanes, const CVECTOR &center, float radius,
                      ADD_POLYGON_FUNC addpoly) = 0;

    virtual const char *GetCollideMaterialName() = 0;
    virtual bool GetCollideTriangle(TRIANGLE &triangle) = 0;
};
