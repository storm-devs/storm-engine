//============================================================================================
//    Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
//    WdmEnemyShip
//--------------------------------------------------------------------------------------------
//
//============================================================================================

#pragma once

#include "WdmShip.h"
#include "attributes.h"

class WdmEnemyShip : public WdmShip
{
    // --------------------------------------------------------------------------------------------
    // Construction, destruction
    // --------------------------------------------------------------------------------------------
  public:
    WdmEnemyShip();
    ~WdmEnemyShip() override;

    // Calculations
    void Update(float dltTime) override;

    // Rendering debug information
    void LRender(VDX9RENDER *rs) override;

    // Find the position for the ship relative to the player
    static bool GeneratePosition(float objRadius, float brnDltAng, float &x, float &z);

    // Set life time
    void SetLiveTime(float time);
    // Get life time
    float GetLiveTime() const;

    bool isEnableKill;
    bool isEnemy; // If set, then attacks us
    bool isEntryPlayer;

    uint32_t type;
    WdmEnemyShip *attack;

    EnemyShipType shipType;

    bool canSkip;

    // Get attribute name
    const char *GetAttributeName() const;

  protected:
    // Find the pulling force in the desired direction
    virtual void FindMoveForce();
    // Find the force that repels from the islands
    virtual void FindIslandForce();
    // Find the force that repels from the ships
    virtual void FindShipsForce();
    // All move calculations together
    virtual void Move(float dltTime);
    // Completion check
    virtual bool KillTest();
    // Updating stored data
    virtual void UpdateSaveData();

  public:
    // Setting parameters
    virtual void SetSaveAttribute(ATTRIBUTES *save);
    void DeleteUpdate();

    // --------------------------------------------------------------------------------------------
    // Encapsulation
    // --------------------------------------------------------------------------------------------

  protected:
    float mx, mz; // Direction vector where to travel
    float ix, iz; // Vector of repulsion from islands
    float sx, sz; // Vector of repulsion from ships
    float dx, dz; // Vector of the resulting direction

    // Whether should pay attention when pushing to the player
    bool isLookOnPlayer;

    ATTRIBUTES *saveAttribute;

  private:
    float brnAlpha;
    float deleteAlpha;
    float liveTime;
    float slowingAlfa;
};

// Get life time
inline float WdmEnemyShip::GetLiveTime() const
{
    if (!isEnableKill)
        return -1.0f;
    return liveTime >= 0.0f ? liveTime : 0.0f;
}
