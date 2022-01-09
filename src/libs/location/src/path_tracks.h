#pragma once

#include "dx9render.h"
#include "math3d.h"
#include "math3d/quaternion.h"

class PathTracks
{
#pragma pack(push, 1)
    struct Point
    {
        Quaternion q;
        Vector p;
        Vector s;
    };
#pragma pack(pop)

    //--------------------------------------------------------------------------------------------
  public:
    PathTracks();
    virtual ~PathTracks();

    //--------------------------------------------------------------------------------------------
  public:
    // Upload track to PathTracks
    bool Load(const char *fileName);
    // Draw track
    void Draw(VDX9RENDER *render);
    // Get track point
    bool GetPoint(float index, Vector &cp, Quaternion &cq) const;

    //--------------------------------------------------------------------------------------------
  private:
    Point *point;
    int32_t numPoints;
};
