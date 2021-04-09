#pragma once

#include "geometry.h"
#include "geos.h"
#include <string>
#include <vector>

class IGMXScene;

class GeomCache
{
    VGEOMETRY *pGS;

    struct CachedGeometry
    {
        GEOS *pGeom;
        std::string FileName;
    };

    std::vector<CachedGeometry> Cache;

  public:
    // Constructor / destructor
    GeomCache();
    ~GeomCache();

    // Put model into cache
    void CacheModel(const char *FileName);

    // Reset cache
    void ResetCache();

    // Get model from cache
    GEOS *GetModel(const char *FileName);

    // Check if such a model exists in the cache
    bool ValidatePointer(GEOS *pModel);
};
