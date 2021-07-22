#pragma once

#include "Cvector.h"

#include "Entity.h"
#include "core.h"
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
    ~COLLIDE() override = default;

    // creates LocalCollide object which must be deleted after use
    virtual LOCAL_COLLIDE *CreateLocalCollide(EntityManager::layer_index_t idx) = 0;

    virtual float Trace(entid_t entity, const CVECTOR &src, const CVECTOR &dst) = 0;

    virtual float Trace(EntityManager::LayerIterators its, const CVECTOR &src, const CVECTOR &dst,
                        const entid_t *exclude_list, long entities) = 0;

    virtual bool Clip(EntityManager::LayerIterators its, const PLANE *planes, long nplanes, const CVECTOR &center,
                      float radius, ADD_POLYGON_FUNC addpoly, const entid_t *exclude_list, long entities) = 0;

    virtual entid_t GetObjectID() = 0;
};
