#ifndef _GEOMETRY_CACHE_
#define _GEOMETRY_CACHE_

#include "..\..\common_h\vapi.h"
#include "..\..\common_h\geometry.h"
#include "..\..\common_h\geos.h"
#include "..\..\common_h\templates.h"


class IGMXScene;

class GeomCache
{
	
	VGEOMETRY* pGS;

	struct CachedGeometry
	{
		GEOS* pGeom;
		string FileName;
	};

	array<CachedGeometry> Cache;

public:

//Конструктор/деструктор
	GeomCache ();
	~GeomCache ();

//Положить модель в кэш
	void CacheModel (const char* FileName);

//Сбросить кэш
	void ResetCache ();

//Взять модель из кэша
	GEOS* GetModel (const char* FileName);

//Проверить существует ли такая модель в кэше
	bool ValidatePointer (GEOS* pModel);

};


#endif