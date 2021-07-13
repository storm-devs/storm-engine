//============================================================================================
//    Spirenkov Maxim, 2001
//--------------------------------------------------------------------------------------------
//    Sea Dogs II
//--------------------------------------------------------------------------------------------
//    WdmPlayerShip
//--------------------------------------------------------------------------------------------
//
//============================================================================================

#pragma once

#include "WdmShip.h"

class WdmPlayerShip : public WdmShip
{
    // --------------------------------------------------------------------------------------------
    // Construction, destruction
    // --------------------------------------------------------------------------------------------
  public:
    WdmPlayerShip();
    virtual ~WdmPlayerShip();

    void PushOutFromIsland();

    void SetActionRadius(float radius);

    // Calculations
    void Update(float dltTime) override;
    void LRender(VDX9RENDER *rs) override;

    // Check when exiting the map
    bool ExitFromMap();
    // Checking for a ship in a storm
    long TestInStorm() const;

    float GetAy() const;

    bool canSkip;

  protected:
    // Move the ship
    virtual void Move(float dltTime);

    void Collide() override;

    // --------------------------------------------------------------------------------------------
    // Encapsulation
    // --------------------------------------------------------------------------------------------
  private:
    bool goForward;
    float actionRadius;
    float stormEventTime;
};

inline float WdmPlayerShip::GetAy() const
{
    return ay;
}
