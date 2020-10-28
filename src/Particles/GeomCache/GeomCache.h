#ifndef _GEOMETRY_CACHE_
#define _GEOMETRY_CACHE_

#include "geometry.h"
#include "geos.h"
#include "templates.h"
#include "vapi.h"

class IGMXScene;

class GeomCache
{

    VGEOMETRY *pGS;

    struct CachedGeometry
    {
        GEOS *pGeom;
        string FileName;
    };

    array<CachedGeometry> Cache;

  public:
    //Конструктор/деструктор
    GeomCache();
    ~GeomCache();

    //Положить модель в кэш
    void CacheModel(const char *FileName);

    //Сбросить кэш
    void ResetCache();

    //Взять модель из кэша
    GEOS *GetModel(const char *FileName);

    //Проверить существует ли такая модель в кэше
    bool ValidatePointer(GEOS *pModel);
};

#endif