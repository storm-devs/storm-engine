//============================================================================================
//	Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
//	WdmIcon
//--------------------------------------------------------------------------------------------
//
//============================================================================================

#ifndef _WdmIcon_H_
#define _WdmIcon_H_

#include "WdmInterfaceObject.h"

class WdmEnemyShip;

class WdmIcon : public WdmInterfaceObject
{
    struct Icon
    {
        float u;
        float alpha;
    };

    // --------------------------------------------------------------------------------------------
    // Construction, destruction
    // --------------------------------------------------------------------------------------------
  public:
    WdmIcon();
    virtual ~WdmIcon();

    // Calculations
    void Update(float dltTime) override;
    // Rendering
    void LRender(VDX9RENDER *rs) override;

    // --------------------------------------------------------------------------------------------
    // Encapsulation
    // --------------------------------------------------------------------------------------------
  private:
    WdmEnemyShip *lastShip;
    long texture;
    Icon icon[2];
    float islandAlpha;
    float skipAlpha;
    float stormAlpha;
};

#endif
