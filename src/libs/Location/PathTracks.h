#ifndef _PathTracks_h_
#define _PathTracks_h_

#include "dx9render.h"
#include "math3D.h"
#include "math3d/Quaternion.h"

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
    //Загрузить трек в PathTracks
    bool Load(const char *fileName);
    //Нарисовать трек
    void Draw(VDX9RENDER *render);
    //Получить точку трека
    bool GetPoint(float index, Vector &cp, Quaternion &cq) const;

    //--------------------------------------------------------------------------------------------
  private:
    Point *point;
    long numPoints;
};

#endif
