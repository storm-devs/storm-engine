#pragma once

#include "dx9render.h"
#include "sea_base.h"
#include "ship_base.h"
#include <queue>

#define ACTION_TIME 1000
#define TIME_BETWEEN_ACTIONS 1000
#define IDLE_ACTION_SEED 500

class SEA_OPERATOR;

//--------------------------------------------------------------------
struct tAction
{
    int32_t timePassed;
    float timeK;
    int32_t actionTime;
    CVECTOR destination, direction;
    SHIP_BASE *attackerShip;
    int hitIntoShip;
    void (SEA_OPERATOR::*actionMethod)(tAction *_action);
};

// typedef TFIFOBuffer<tAction> TActionBuffer;

using TActionBuffer = std::queue<tAction>;

enum BORT_TYPE
{
    BORT_FRONT,
    BORT_LEFT,
    BORT_RIGHT,
    BORT_REAR
};

//--------------------------------------------------------------------
class SEA_OPERATOR : public Entity
{
  public:
    SEA_OPERATOR();
    ~SEA_OPERATOR() override;

    bool Init() override;
    uint64_t ProcessMessage(MESSAGE &message) override;
    virtual void Realize(uint32_t dTime);
    virtual void Execute(uint32_t dTime);
    uint32_t AttributeChanged(ATTRIBUTES *_newAttr) override;

    void ProcessStage(Stage stage, uint32_t delta) override
    {
        switch (stage)
        {
        case Stage::execute:
            Execute(delta);
            break;
        case Stage::realize:
            Realize(delta);
            break;
            /*case Stage::lost_render:
                LostRender(delta); break;
            case Stage::restore_render:
                RestoreRender(delta); break;*/
        }
    }

  protected:
    void ShowAttackerBort(tAction *_action);
    void ShowFromBall(tAction *_action);
    void ShowAroundPoint(tAction *_action);
    void ShowBallAtMyShip(tAction *_action);
    void ShowMyShipFromPoint(tAction *_action);

  private:
    void FirstInit();
    bool IsTimeToActivate(bool _testControls = true);
    void SetIfMyShip(entid_t _shipID);
    void HandleShipFire(entid_t _shipID, const char *_bortName, const CVECTOR &_destination, const CVECTOR &_direction);
    void HandleShipHit();
    void HandleShipIdle();
    void StartNewAction();
    void ProcessActions(uint32_t dTime);

    bool enabled, active;
    SEA_BASE *sea;
    VDX9RENDER *renderer;
    SHIP_BASE *myShip, *enemyShip;
    int32_t actionTime;
    bool ballTracked;
    CVECTOR ballPosition, lastBallPosition, finalBallPosition;

    uint32_t idleTime, sinceLastActionTime;
    TActionBuffer actionBuffer;
    CVECTOR cameraPos, cameraTargetPos;
};

//--------------------------------------------------------------------
