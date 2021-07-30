#include "vcollide.h"

LCOLL::LCOLL(EntityManager::layer_index_t idx) : boxRadius(0)
{
    layerIndex_ = idx;
    col = static_cast<COLLIDE *>(core.CreateService("coll"));
    if (!col)
        throw std::runtime_error("No service: collide");
}

LCOLL::~LCOLL()
{
}

#define REALLOC_QUANT 1024 // mast be power of to due to AND

long addedFaces;
long *sVrt = nullptr;
Vector *addVerts = nullptr;

bool AddPolyColl(const Vector *vr, long nverts)
{
    // start vertex of face, max faces is REALLOC_QUANT
    if ((addedFaces & (REALLOC_QUANT - 1)) == 0)
        sVrt = static_cast<long *>(realloc(sVrt, sizeof(long) * (addedFaces + REALLOC_QUANT)));

    if (addedFaces == 0)
        sVrt[addedFaces] = nverts;
    else
        sVrt[addedFaces] = sVrt[addedFaces - 1] + nverts;

    // F0(v0,v1,v2), F1(v0,v1,v2,v3)...
    if (addedFaces == 0 || (sVrt[addedFaces - 1] & (REALLOC_QUANT - 1)) + nverts > REALLOC_QUANT)
        addVerts = static_cast<Vector *>(
            realloc(addVerts, sizeof(long) * (sVrt[addedFaces] / REALLOC_QUANT + REALLOC_QUANT)));

    for (long v = 0; v < nverts; v++)
    {
        addVerts[sVrt[addedFaces] - nverts + v].x = vr[v].x;
        addVerts[sVrt[addedFaces] - nverts + v].y = vr[v].x;
        addVerts[sVrt[addedFaces] - nverts + v].z = vr[v].x;
    }

    addedFaces++;
    return true;
}

long LCOLL::SetBox(const Vector &boxSize, const Matrix &transform, bool testOnly)
{
    // create box
    Plane clip_p[6];
    memset(clip_p, 0, sizeof(plane));
    clip_p[0].n.x = clip_p[2].n.y = clip_p[4].n.z = 1.0f;
    clip_p[1].n.x = clip_p[3].n.y = clip_p[5].n.z = -1.0f;
    clip_p[0].D = boxSize.x;
    clip_p[1].D = -boxSize.x;
    clip_p[2].D = boxSize.y;
    clip_p[3].D = -boxSize.y;
    clip_p[4].D = boxSize.z;
    clip_p[5].D = -boxSize.z;

    // transform planes
    for (long p = 0; p < 6; p++)
    {
        const auto x = clip_p[p].D * clip_p[p].n.x - transform.m[3][0];
        const auto y = clip_p[p].D * clip_p[p].n.y - transform.m[3][1];
        const auto z = clip_p[p].D * clip_p[p].n.z - transform.m[3][2];
        const auto Nx =
            transform.m[0][0] * clip_p[p].n.x + transform.m[0][1] * clip_p[p].n.y + transform.m[0][2] * clip_p[p].n.z;
        const auto Ny =
            transform.m[1][0] * clip_p[p].n.x + transform.m[1][1] * clip_p[p].n.y + transform.m[1][2] * clip_p[p].n.z;
        const auto Nz =
            transform.m[2][0] * clip_p[p].n.x + transform.m[2][1] * clip_p[p].n.y + transform.m[2][2] * clip_p[p].n.z;
        const auto lx = transform.m[0][0] * x + transform.m[0][1] * y + transform.m[0][2] * z;
        const auto ly = transform.m[1][0] * x + transform.m[1][1] * y + transform.m[1][2] * z;
        const auto lz = transform.m[2][0] * x + transform.m[2][1] * y + transform.m[2][2] * z;
        plane[p].n.x = Nx;
        plane[p].n.y = Ny;
        plane[p].n.z = Nz;
        plane[p].D = (Nx * lx + Ny * ly + Nz * lz) * transform.m[3][3];
    }

    boxCenter = transform.pos;
    boxRadius = sqrtf(~boxSize);

    // added faces
    addedFaces = 0;
    // start vertex of face
    sVrt = nullptr;
    // F0(v0,v1,v2), F1(v0,v1,v2,v3)...
    addVerts = nullptr;

    const auto its = EntityManager::GetEntityIdIterators(layerIndex_);
    col->Clip(its, &plane[0], 6, boxCenter, boxRadius, AddPolyColl, nullptr, 0);
    return 0;
}

const Vector *LCOLL::GetFace(long &numVertices)
{
    numVertices = 0;
    return nullptr;
}

float LCOLL::Trace(const Vector &src, const Vector &dst)
{
    return 2.0f;
}
