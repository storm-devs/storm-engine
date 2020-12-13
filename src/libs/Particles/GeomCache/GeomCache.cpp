#include "GeomCache.h"

//Конструктор/деструктор
GeomCache::GeomCache()
{
    pGS = static_cast<VGEOMETRY *>(api->CreateService("geometry"));
    Assert(pGS);
}

GeomCache::~GeomCache()
{
    ResetCache();
}

//Положить модель в кэш
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

//Сбросить кэш
void GeomCache::ResetCache()
{
    for (auto n = 0; n < Cache.size(); n++)
    {
        delete Cache[n].pGeom;
    }

    Cache.clear();
}

//Взять модель из кэша
GEOS *GeomCache::GetModel(const char *FileName)
{
    for (auto n = 0; n < Cache.size(); n++)
    {
        if (Cache[n].FileName == FileName)
            return Cache[n].pGeom;
    }

    return nullptr;
}

//Проверить существует ли такая модель в кэше
bool GeomCache::ValidatePointer(GEOS *pModel)
{
    for (auto n = 0; n < Cache.size(); n++)
    {
        if (Cache[n].pGeom == pModel)
            return true;
    }

    return false;
}
