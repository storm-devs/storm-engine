#pragma once

#include "collide.h"

#pragma pack(push)
#pragma pack(1)

class LCOLL : public LOCAL_COLLIDE
{
    EntityManager::layer_index_t layerIndex_;
    PLANE plane[6];
    CVECTOR boxCenter;
    float boxRadius;
    COLLIDE *col;

  public:
    LCOLL(EntityManager::layer_index_t idx);
    virtual ~LCOLL();
    virtual long SetBox(const CVECTOR &boxSize, const CMatrix &transform, bool testOnly = false);
    virtual const CVECTOR *GetFace(long &numVertices);
    virtual float Trace(const CVECTOR &src, const CVECTOR &dst);
};

class COLL : public COLLIDE
{
  public:
    COLL() = default;
    virtual ~COLL() = default;
    virtual LOCAL_COLLIDE *CreateLocalCollide(EntityManager::layer_index_t idx);
    virtual float Trace(entid_t entity, const CVECTOR &src, const CVECTOR &dst);
    virtual float Trace(EntityManager::LayerIterators its, const CVECTOR &src, const CVECTOR &dst,
                        const entid_t *exclude_list, long entities);
    virtual bool Clip(EntityManager::LayerIterators its, const PLANE *planes, long nplanes, const CVECTOR &center,
                      float radius, ADD_POLYGON_FUNC addpoly, const entid_t *exclude_list, long entities);
    virtual entid_t GetObjectID();
};

#pragma pack(pop)

// API_SERVICE_START("collide service")
//    DECLARE_MAIN_SERVICE(COLL)
// API_SERVICE_END(COLL)
