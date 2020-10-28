#ifndef __COLLIDE_H__
#define __COLLIDE_H__

#include "cvector.h"

#include "object.h"
#include "types3d.h"
#include "vmodule_api.h"

class LOCAL_COLLIDE
{
  public:
    virtual ~LOCAL_COLLIDE(){};

    // clip faces into the box and returns num of faces
    virtual long SetBox(const CVECTOR &boxSize, const CMatrix &transform, bool testOnly = false) = 0;

    // returns pointer to clipped face and fills num of vertices in face
    virtual const CVECTOR *GetFace(long &numVertices) = 0;

    // trace a ray inside the box
    virtual float Trace(const CVECTOR &src, const CVECTOR &dst) = 0;
};

class COLLIDE : public SERVICE
{
  public:
    // creates LocalCollide object which must be deleted after use
    virtual LOCAL_COLLIDE *CreateLocalCollide(const char *layerName) = 0;

    virtual ~COLLIDE(){};
    virtual float Trace(ENTITY_ID &entity, const CVECTOR &src, const CVECTOR &dst) = 0;

    virtual float Trace(VIDWALKER &walker, const CVECTOR &src, const CVECTOR &dst, const ENTITY_ID *exclude_list,
                        long entities) = 0;

    virtual bool Clip(VIDWALKER &walker, const PLANE *planes, long nplanes, const CVECTOR &center, float radius,
                      ADD_POLYGON_FUNC addpoly, const ENTITY_ID *exclude_list, long entities) = 0;

    virtual ENTITY_ID GetObjectID() = 0;
};

#endif