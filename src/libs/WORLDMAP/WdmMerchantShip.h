//============================================================================================
//    Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
//    WdmMerchantShip
//--------------------------------------------------------------------------------------------
//
//============================================================================================

#pragma once

#include "WdmEnemyShip.h"

class WdmMerchantShip : public WdmEnemyShip
{
    // --------------------------------------------------------------------------------------------
    // Construction, destruction
    // --------------------------------------------------------------------------------------------
  public:
    WdmMerchantShip();
    virtual ~WdmMerchantShip();

    void Goto(float x, float z, float rad);

  protected:
    // Find the pulling force in the desired direction
    void FindMoveForce() override;
    // Completion check
    bool KillTest() override;

    // Updating stored data
    void UpdateSaveData() override;
    // Setting parameters
    void SetSaveAttribute(ATTRIBUTES *save) override;

    // Setting parameters
    bool Load(const char *modelName) override;

    // --------------------------------------------------------------------------------------------
    // Encapsulation
    // --------------------------------------------------------------------------------------------
  private:
    CVECTOR gotoPos;
    float gotoRad;
    bool isEnableSetDir;
};
