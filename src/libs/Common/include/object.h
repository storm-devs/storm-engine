#pragma once

#include "math3d/Plane.h"
#include "dx9render.h"
#include "Entity.h"

struct TRIANGLE
{
    Vector vrt[3];
};

using ADD_POLYGON_FUNC = bool (*)(const Vector *v, long nv);

class COLLISION_OBJECT : public Entity
{
  public:
    bool Init() override
    {
        return true;
    }

    ~COLLISION_OBJECT() override{};
    Matrix mtx;

    virtual float Trace(const Vector &src, const Vector &dst) = 0;
    virtual bool Clip(const Plane *planes, long nplanes, const Vector &center, float radius,
                      ADD_POLYGON_FUNC addpoly) = 0;

    virtual const char *GetCollideMaterialName() = 0;
    virtual bool GetCollideTriangle(TRIANGLE &triangle) = 0;
};
