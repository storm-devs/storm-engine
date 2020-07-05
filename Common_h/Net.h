#ifndef NET_BASE_HPP
#define NET_BASE_HPP

#include <stdio.h>

#include "vmodule_api.h"
#include "defines.h"

#include "ship_base.h"
#include "collide.h"
#include "dx8render.h"
#include "model.h"
#include "Island_Base.h"
#include "geos.h"
#include "sea_base.h"
#include "character.h"
#include "vdata.h"
#include "mast_msg.h"
#include "geometry.h"
#include "vparticle_system.h"
#include "object.h"
#include "animation.h"
#include "rands.h"
#include "sail_base.h"
#include "mast_msg.h"
#include "tga.h"

#include "SD2_H\VAI_ObjBase.h"

#include "templates\string.h"
#include "templates\array.h"
#include "templates\dtimer.h"
#include "templates\htable.h"
#include "templates\stack.h"
#include "templates\map.h"

#include "..\common_h\CVECTOR4.h"

#include "..\ENGINE\PROGRAM\NET\net_defines.h"

#include "..\common_h\math3d.h"
#include "..\common_h\ShipLights.h"

__forceinline bool NetFindClass(bool bServer, ENTITY_ID * pEID, const char * pClassName)
{
	Assert(pEID);
	bool bFind = api->FindClass(pEID, (char *)pClassName, 0);
	if (bFind) do
	{
		ENTITY * pE = (ENTITY *)pEID->pointer;
		if (pE->IsServer() == bServer) return true;
	} while (api->FindClassNext(pEID));

	return false;
}

__forceinline bool NetFindClassNext(bool bServer, ENTITY_ID * pEID)
{
	Assert(pEID);
	while (api->FindClassNext(pEID))
	{
		ENTITY * pE = (ENTITY *)pEID->pointer;
		if (pE->IsServer() == bServer) return true;
	}
	return false;
}

class NET_BASE
{
public:
	NET_BASE()
	{
		pGeoService = null;
		pRenderService = null;
		pCollideService = null;
	}

	~NET_BASE() {};

	virtual void SetServer(bool bServer) { this->bServer = bServer; };
	virtual bool IsServer() { return bServer; };
	virtual bool IsClient() { return !IsServer(); };

	virtual VDX8RENDER & Render() 
	{ 
		if (!pRenderService) pRenderService = (VDX8RENDER *)api->CreateService("dx8render");	
		return *pRenderService; 
	};
	virtual VGEOMETRY & Geometry() 
	{ 
		if (!pGeoService) pGeoService = (VGEOMETRY *)api->CreateService("geometry");
		return *pGeoService; 
	};
	virtual COLLIDE & Collide() 
	{ 
		if (!pCollideService) pCollideService = (COLLIDE *)api->CreateService("coll");
		return *pCollideService; 
	};

private:
	bool bServer;
	VGEOMETRY * pGeoService;
	VDX8RENDER * pRenderService;
	COLLIDE * pCollideService;
};

#endif