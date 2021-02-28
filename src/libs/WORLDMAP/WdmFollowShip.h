//============================================================================================
//    Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
//    WdmFollowShip
//--------------------------------------------------------------------------------------------
//
//============================================================================================

#ifndef _WdmFollowShip_H_
#define _WdmFollowShip_H_

#include "WdmEnemyShip.h"

class WdmFollowShip : public WdmEnemyShip
{
    // --------------------------------------------------------------------------------------------
    // construction, destruction
    // --------------------------------------------------------------------------------------------
  public:
    WdmFollowShip();
    virtual ~WdmFollowShip();

  protected:
    // Setting parameters
    bool Load(const char *modelName) override;
    // Find the force pulling in the right direction
    void FindMoveForce() override;

    // --------------------------------------------------------------------------------------------
    // Encapsulation
    // --------------------------------------------------------------------------------------------
  private:
};

#endif
