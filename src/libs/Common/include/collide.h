#pragma once

#include "Entity.h"
#include "core.h"
#include "object.h"

class LOCAL_COLLIDE
{
  public:
    virtual ~LOCAL_COLLIDE(){};

    // clip faces into the box and returns num of faces
    virtual long SetBox(const Vector &boxSize, const Matrix &transform, bool testOnly = false) = 0;

    // returns pointer to clipped face and fills num of vertices in face
    virtual const Vector *GetFace(long &numVertices) = 0;

    // trace a ray inside the box
    virtual float Trace(const Vector &src, const Vector &dst) = 0;
};

class COLLIDE : public SERVICE
{
  public:
    ~COLLIDE() override = default;

    // creates LocalCollide object which must be deleted after use
    virtual LOCAL_COLLIDE *CreateLocalCollide(EntityManager::layer_index_t idx) = 0;

    virtual float Trace(entid_t entity, const Vector &src, const Vector &dst) = 0;

    virtual float Trace(EntityManager::LayerIterators its, const Vector &src, const Vector &dst,
                        const entid_t *exclude_list, long entities) = 0;

    virtual bool Clip(EntityManager::LayerIterators its, const Plane *planes, long nplanes, const Vector &center,
                      float radius, ADD_POLYGON_FUNC addpoly, const entid_t *exclude_list, long entities) = 0;

    virtual entid_t GetObjectID() = 0;
};
