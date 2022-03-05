//============================================================================================
//    Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
//    WdmWarringShip
//--------------------------------------------------------------------------------------------
//
//============================================================================================

#pragma once

#include "wdm_enemy_ship.h"

class WdmWarringShip : public WdmEnemyShip
{
    struct MoveInfo
    {
        CVECTOR v;
        float dAng;
        float time;
        float kTime;
    };

    // --------------------------------------------------------------------------------------------
    // Construction, destruction
    // --------------------------------------------------------------------------------------------
  public:
    WdmWarringShip();
    ~WdmWarringShip() override;

    // Calculations
    void Update(float dltTime) override;

    // Rendering
    void LRender(VDX9RENDER *rs) override;

  protected:
    // All move calculations together
    void Move(float dltTime) override{};

    // --------------------------------------------------------------------------------------------
    // Encapsulation
    // --------------------------------------------------------------------------------------------
  private:
    // Particles for cannon smoke
    RS_RECT rect[24];
    MoveInfo move[24];
    int32_t numRects;
    float brnTime;
    int32_t texture;
};
