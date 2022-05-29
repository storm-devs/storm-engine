#pragma once

#include "collide.h"
#include "dx9render.h"
#include "model.h"
#include "ship_base.h"
#include "v_module_api.h"

#include "sea_base.h"

const float RUN_SPEED = 0.30f;
const float MOVE_SPEED = 0.15f;
const float CLIMB_SPEED = 0.065f;

const float JUMP_SPEED_X_MASTFALL = 3.0f;
const float JUMP_SPEED_Y_EXPLOSION = 30.0f;

#include "sailors_way_points.h"
#include "shared/sea_ai/sea_people.h"

#define CREW2VISIBLE(n) 5.0f * logf(n) //???
#define SHIP_BLACK_PEARL 164

enum ManMode
{
    MAN_JUMP,
    MAN_SWIM,
    MAN_OFF,
    MAN_WALK,
    MAN_RUN,
    MAN_STAY,
    MAN_CANNONRELOAD,
    MAN_TURNLEFT,
    MAN_TURNRIGHT,
    MAN_CLIMB_UP,
    MAN_CLIMB_DOWN
};

enum ManMoveTo
{
    MOVE_TO_POINT,
    MOVE_TO_CANNON,
    MOVE_TO_TOP
};

struct ShipState
{
    uint32_t mode;
    bool dead;
    SEA_BASE *sea;

    ShipState()
        : sea(nullptr)
    {
        mode = SHIP_SAIL;
        dead = false;
    }
    ;
};

class ShipMan
{
  public:
    entid_t modelID;

    CVECTOR pos, ang; // current position
    CVECTOR ptTo, angTo, dir;

    CVECTOR spos; // Bypassing each other
    // float sang; // Bypass each other

    float dieTime;
    bool inWater;
    float jumpSpeedX, jumpSpeedY;

    Path path; // Current path

    ManMode mode, lastMode; // Mode
    int newWayPoint, lastWayPoint, targetWayPoint, lastTargetPoint;

    ManMoveTo moveTo; // Current point type

    float manSpeed;
    float rotSpeed;

    ShipMan();

    void SetPos(MODEL *ship, SHIP_BASE *ship_base, uint32_t &dltTime, ShipState &shipState);
    void FindNextPoint(SailorsPoints &sailorsPoints, ShipState &shipState);
    int FindRandomPoint(SailorsPoints &sailorsPoints, ShipState &shipState);
    int FindRandomPointWithoutType(SailorsPoints &sailorsPoints) const;
    void ApplyTargetPoint(CVECTOR pt, bool randomWalk);

    void UpdatePos(uint32_t &dltTime, SailorsPoints &sailorsPoints, ShipState &shipState);

    void SetAnimation(uint32_t dltTime, ShipState &shipState);

    bool MoveToPosition(uint32_t &dltTime, SailorsPoints &sailorsPoints, ShipState &shipState);
    bool RotateToAngle(uint32_t &dltTime, SailorsPoints &sailorsPoints);
    bool Stay(uint32_t &dltTime, SailorsPoints &sailorsPoints) const;
    bool Turn(uint32_t &dltTime, SailorsPoints &sailorsPoints);
    bool Swim(uint32_t &dltTime, SailorsPoints &sailorsPoints, ShipState &shipState);
    bool Jump(uint32_t &dltTime, SailorsPoints &sailorsPoints, ShipState &shipState);

    void NewAction(SailorsPoints &sailorsPoints, ShipState &shipState, uint32_t &dltTime);
    void Free() const;
    int GetNearestEmptyCannon(SailorsPoints &sailorsPoints) const;
};

class ShipWalk
{
  public:
    SHIP_BASE *ship;
    MODEL *shipModel;
    int crewCount; // Number of people

    bool bHide;

    entid_t shipID;

    SailorsPoints sailorsPoints; // Points

    void ReloadCannons(int bort);
    ShipState shipState; // Ship state

    std::vector<ShipMan> shipMan;
    std::vector<std::string> shipManModels_ = {"Lowcharacters\\Lo_Man_1", "Lowcharacters\\Lo_Man_2",
                                              "Lowcharacters\\Lo_Man_3", "Lowcharacters\\Lo_Man_Kamzol_1",
                                              "Lowcharacters\\Lo_Man_Kamzol_2", "Lowcharacters\\Lo_Man_Kamzol_3"};
    void CreateNewMan(SailorsPoints &sailorsPoints);
    void DeleteMan(int Index);

    bool Init(entid_t _shipID, int editorMode, const char *shipType, std::vector<std::string> &&shipManModels);
    void CheckPosition(uint32_t &dltTime);
    void SetMastBroken(int iMastIndex);
    void OnHullHit(const CVECTOR &v);
    void Reset();

    void Free();
};

class Sailors : public Entity
{
  public:
    VDX9RENDER *rs;

    std::vector<ShipWalk> shipWalk;
    bool editorMode;
    int shipsCount;
    bool disabled;

    Sailors();
    ~Sailors() override;

    bool Init() override;
    virtual void Realize(uint32_t dltTime);

    uint64_t ProcessMessage(MESSAGE &message) override;
    uint32_t AttributeChanged(ATTRIBUTES *_newAttr) override;

    void ProcessStage(Stage stage, uint32_t delta) override
    {
        switch (stage)
        {
        case Stage::realize:
            Realize(delta);
            break;
        }
    }

    int IsOnDeck;

    void DeleteShip(int i);
};
