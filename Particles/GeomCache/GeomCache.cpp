#include "GeomCache.h"


//Конструктор/деструктор
GeomCache::GeomCache () : Cache(_FL_)
{
	pGS = (VGEOMETRY *)api->CreateService("geometry");
	Assert (pGS);
}

GeomCache::~GeomCache ()
{
	ResetCache ();
}

//Положить модель в кэш
void GeomCache::CacheModel (const char* FileName)
{
	if (GetModel(FileName)) return;
	
	GEOS* pGeom = pGS->CreateGeometry(FileName, "", 0);
	if (!pGeom) return;

	CachedGeometry CacheEntry;
	CacheEntry.FileName = FileName;
	CacheEntry.pGeom = pGeom;
	Cache.Add(CacheEntry);
}

//Сбросить кэш
void GeomCache::ResetCache ()
{
	for (int n = 0; n < Cache; n++)
	{
		delete Cache[n].pGeom;
	}

	Cache.DelAll();
}

//Взять модель из кэша
GEOS* GeomCache::GetModel (const char* FileName)
{
	for (int n = 0; n < Cache; n++)
	{
		if (Cache[n].FileName == FileName) return Cache[n].pGeom;
	}

	return NULL;
}

//Проверить существует ли такая модель в кэше
bool GeomCache::ValidatePointer (GEOS* pModel)
{
	for (int n = 0; n < Cache; n++)
	{
		if (Cache[n].pGeom == pModel) return true;
	}

	return false;
}
