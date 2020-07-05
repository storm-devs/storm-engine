#ifndef ISHIPLIGHTS_HPP
#define ISHIPLIGHTS_HPP

#include "ship_base.h"

#include "templates\string.h"
#include "templates\array.h"
#include "templates\dtimer.h"

class IShipLights : public ENTITY
{
public:
	IShipLights() {};
	virtual ~IShipLights() {};

	virtual void Release(VAI_OBJBASE * pObject) = 0;

	virtual void AddLights(VAI_OBJBASE * pObject, MODEL * pModel, bool bLights, bool bFlares) = 0;
	virtual void KillMast(VAI_OBJBASE * pObject, NODE * pNode, bool bNow) = 0;
	virtual void AddDynamicLights(VAI_OBJBASE * pObject, const CVECTOR & vPos) = 0;
	virtual void SetLights(VAI_OBJBASE * pObject) = 0;
	virtual void UnSetLights(VAI_OBJBASE * pObject) = 0;
	virtual void SetLightsOff(VAI_OBJBASE * pObject, float fTime, bool bLights, bool bFlares, bool bNow) = 0;

	virtual void SetDead(VAI_OBJBASE * pObject) = 0;
};


#endif