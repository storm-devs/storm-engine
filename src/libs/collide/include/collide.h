#pragma once

#include "c_vector.h"

#include "entity.h"
#include "object.h"
#include "service.h"
#include "types3d.h"
#include "vma.hpp"

class LOCAL_COLLIDE
{
  public:
    virtual ~LOCAL_COLLIDE(){};

    // clip faces into the box and returns num of faces
    virtual int32_t SetBox(const CVECTOR &boxSize, const CMatrix &transform, bool testOnly = false) = 0;

    // returns pointer to clipped face and fills num of vertices in face
    virtual const CVECTOR *GetFace(int32_t &numVertices) = 0;

    // trace a ray inside the box
    virtual float Trace(const CVECTOR &src, const CVECTOR &dst) = 0;
};

class COLLIDE : public SERVICE
{
  public:
    ~COLLIDE() override = default;

    // creates LocalCollide object which must be deleted after use
    virtual LOCAL_COLLIDE *CreateLocalCollide(layer_index_t idx) = 0;

    virtual float Trace(entid_t entity, const CVECTOR &src, const CVECTOR &dst) = 0;

    virtual float Trace(entity_container_cref entities, const CVECTOR &src, const CVECTOR &dst,
                        const entid_t *exclude_list, int32_t exclude_num) = 0;

    virtual bool Clip(entity_container_cref entities, const PLANE *planes, int32_t nplanes, const CVECTOR &center,
                      float radius, ADD_POLYGON_FUNC addpoly, const entid_t *exclude_list, int32_t exclude_num) = 0;

    virtual entid_t GetObjectID() = 0;
};
