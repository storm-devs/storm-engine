#pragma once

#include "ship_base.h"

class IShipLights : public Entity
{
  public:
    IShipLights(){};

    ~IShipLights() override{};
    virtual void Release(VAI_OBJBASE *pObject) = 0;

    virtual void AddLights(VAI_OBJBASE *pObject, MODEL *pModel, bool bLights, bool bFlares) = 0;
    virtual void KillMast(VAI_OBJBASE *pObject, NODE *pNode, bool bNow) = 0;
    virtual void AddDynamicLights(VAI_OBJBASE *pObject, const CVECTOR &vPos) = 0;
    virtual void SetLights(VAI_OBJBASE *pObject) = 0;
    virtual void UnSetLights(VAI_OBJBASE *pObject) = 0;
    virtual void SetLightsOff(VAI_OBJBASE *pObject, float fTime, bool bLights, bool bFlares, bool bNow) = 0;
	virtual void ResetLights(VAI_OBJBASE *pObject, bool bLight) = 0;

    virtual void SetDead(VAI_OBJBASE *pObject) = 0;
};
