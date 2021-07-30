#pragma once

#include "collide.h"

#pragma pack(push)
#pragma pack(1)

class LCOLL : public LOCAL_COLLIDE
{
    EntityManager::layer_index_t layerIndex_;
    Plane plane[6];
    Vector boxCenter;
    float boxRadius;
    COLLIDE *col;

  public:
    LCOLL(EntityManager::layer_index_t idx);
    ~LCOLL() override;
    long SetBox(const Vector &boxSize, const Matrix &transform, bool testOnly = false) override;
    const Vector *GetFace(long &numVertices) override;
    float Trace(const Vector &src, const Vector &dst) override;
};

class COLL : public COLLIDE
{
  public:
    COLL() = default;
    ~COLL() override = default;
    LOCAL_COLLIDE *CreateLocalCollide(EntityManager::layer_index_t idx) override;
    float Trace(entid_t entity, const Vector &src, const Vector &dst) override;
    float Trace(EntityManager::LayerIterators its, const Vector &src, const Vector &dst, const entid_t *exclude_list,
                long entities) override;
    bool Clip(EntityManager::LayerIterators its, const Plane *planes, long nplanes, const Vector &center, float radius,
              ADD_POLYGON_FUNC addpoly, const entid_t *exclude_list, long entities) override;
    entid_t GetObjectID() override;
};

#pragma pack(pop)

// API_SERVICE_START("collide service")
//    DECLARE_MAIN_SERVICE(COLL)
// API_SERVICE_END(COLL)
