#include "GeomCache.h"

#include "core.h"

#include "storm_assert.h"

// Constructor / destructor
GeomCache::GeomCache()
{
    pGS = static_cast<VGEOMETRY *>(core.CreateService("geometry"));
    Assert(pGS);
}

GeomCache::~GeomCache()
{
    ResetCache();
}

// Put model into cache
void GeomCache::CacheModel(const char *FileName)
{
    if (GetModel(FileName))
        return;

    auto *const pGeom = pGS->CreateGeometry(FileName, "", 0);
    if (!pGeom)
        return;

    CachedGeometry CacheEntry;
    CacheEntry.FileName = FileName;
    CacheEntry.pGeom = pGeom;
    Cache.push_back(CacheEntry);
}

// Reset cache
void GeomCache::ResetCache()
{
    for (auto n = 0; n < Cache.size(); n++)
    {
        delete Cache[n].pGeom;
    }

    Cache.clear();
}

// Get model from cache
GEOS *GeomCache::GetModel(const char *FileName)
{
    for (auto n = 0; n < Cache.size(); n++)
    {
        if (Cache[n].FileName == FileName)
            return Cache[n].pGeom;
    }

    return nullptr;
}

// Check if such a model exists in the cache
bool GeomCache::ValidatePointer(GEOS *pModel)
{
    for (auto n = 0; n < Cache.size(); n++)
    {
        if (Cache[n].pGeom == pModel)
            return true;
    }

    return false;
}
