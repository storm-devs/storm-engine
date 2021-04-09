#pragma once

#include "geos.h"
#include "service.h"
#include "vmodule_api.h"

class ANIMATION
{
  public:
    virtual ~ANIMATION(){};
    virtual float SetTime(float time) = 0;
    virtual void SetMatrix() = 0;
};

typedef void *(*VERTEX_TRANSFORM)(void *vb, long startVrt, long nVerts, long totVerts);

class VGEOMETRY : public SERVICE
{
  public:
    class ANIMATION_VB
    {
      public:
        long nvertices;
        long fvf;
        long stride;
        void *buff;
    };

    virtual ~VGEOMETRY(){};
    virtual GEOS *CreateGeometry(const char *file_name, const char *light_file_name, long flags,
                                 const char *lmPath = nullptr) = 0;
    virtual void DeleteGeometry(GEOS *) = 0;
    virtual ANIMATION *LoadAnimation(const char *anim) = 0;
    virtual void SetTechnique(const char *name) = 0;
    virtual void SetVBConvertFunc(VERTEX_TRANSFORM _transform_func) = 0;
    virtual ANIMATION_VB GetAnimationVBDesc(long avb) = 0;

    virtual const char *GetTexturePath() = 0;
    virtual void SetTexturePath(const char *) = 0;

    virtual void SetCausticMode(bool bSet = false) = 0;
};
