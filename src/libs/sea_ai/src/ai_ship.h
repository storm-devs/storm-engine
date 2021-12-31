#pragma once

#include "ai_cannon.h"
#include "ai_ship_camera_controller.h"
#include "ai_ship_cannon_controller.h"
#include "ai_ship_move_controller.h"
#include "ai_ship_rotate_controller.h"
#include "ai_ship_speed_controller.h"
#include "ai_ship_task_controller.h"
#include "ai_ship_touch_controller.h"
#include "ship_base.h"
#include "ai_flow_graph.h"
#include "ai_helper.h"

enum AI_OBJTYPE
{
    AIOBJ_SHIP_WAR = 0,
    AIOBJ_SHIP_TRADE,
    AIOBJ_SHIP_BOAT,
    AIOBJ_FORT,
    AIOBJ_UNKNOWN = 0x7FFFFFFF
};

// ============================================================================
// virtual AI Class
// Contain base virtual functions
// ============================================================================

class VAI_INNEROBJ : public AIAttributesHolder
{
  protected:
    AI_OBJTYPE ObjType;
    bool bDead;

  public:
    VAI_INNEROBJ()
    {
        pACharacter = nullptr;
        bDead = false;
        ObjType = AIOBJ_UNKNOWN;
    };

    virtual ~VAI_INNEROBJ(){};

    // check for fire possibility
    virtual bool isCanFire(const CVECTOR &vFirePos) const
    {
        return false;
    }

    // set / get type of AI object
    void SetObjType(AI_OBJTYPE _ObjType)
    {
        ObjType = _ObjType;
    };

    AI_OBJTYPE GetObjType() const
    {
        return ObjType;
    };

    // enemy / friend / neutral / main character
    virtual bool isMainCharacter()
    {
        return GetACharacter()->GetAttributeAsDword("MainCharacter") != 0;
    };

    virtual bool isFriend(ATTRIBUTES *pAOtherCharacter) const
    {
        if (isDead() || isDead(pAOtherCharacter))
            return true;
        return Helper.isFriend(GetACharacter(), pAOtherCharacter);
    }

    virtual bool isEnemy(ATTRIBUTES *pAOtherCharacter) const
    {
        if (isDead() || isDead(pAOtherCharacter))
            return false;
        return Helper.isEnemy(GetACharacter(), pAOtherCharacter);
    };

    virtual bool isNeutral(ATTRIBUTES *pAOtherCharacter) const
    {
        if (isDead() || isDead(pAOtherCharacter))
            return false;
        return Helper.isNeutral(GetACharacter(), pAOtherCharacter);
    };

    virtual bool isFriend(const VAI_INNEROBJ &OtherAIObj) const
    {
        return isFriend(OtherAIObj.GetACharacter());
    };

    virtual bool isEnemy(const VAI_INNEROBJ &OtherAIObj) const
    {
        return isEnemy(OtherAIObj.GetACharacter());
    };

    virtual bool isNeutral(const VAI_INNEROBJ &OtherAIObj) const
    {
        return isNeutral(OtherAIObj.GetACharacter());
    };

    virtual bool isDead() const
    {
        return bDead;
    };

    virtual bool isDead(ATTRIBUTES *pACharacter) const
    {
        auto *const pAIObj = AIHelper::FindAIInnerObj(pACharacter);
        Assert(pAIObj);
        return pAIObj->isDead();
    };

    virtual bool SetDead(bool bNewDead)
    {
        const auto bOldDead = bDead;
        bDead = bNewDead;
        return bOldDead;
    };

    bool operator==(const ATTRIBUTES *pOtherAICharacter) const
    {
        return (GetACharacter() == pOtherAICharacter);
    };

    bool operator==(const VAI_INNEROBJ &OtherAIObj) const
    {
        return GetACharacter() == OtherAIObj.GetACharacter();
    };

    virtual void ReleasePoint(VAI_INNEROBJ *){};
    virtual CVECTOR GetAttackPoint(VAI_INNEROBJ *) = 0;
    virtual CVECTOR GetFollowPoint(VAI_INNEROBJ *) = 0;
    virtual CVECTOR GetAbordagePoint(VAI_INNEROBJ *pOtherObj) = 0;

    virtual float GetMaxFireDistance() = 0;
    virtual float GetMinFireDistance() = 0;

    virtual float GetDistance(const VAI_INNEROBJ &OtherAIObj) const
    {
        return sqrtf(~(GetPos() - OtherAIObj.GetPos()));
    };

    virtual float GetDistance(const CVECTOR &vOtherPos) const
    {
        return sqrtf(~(GetPos() - vOtherPos));
    };
    virtual CVECTOR GetBoxsize() const = 0;
    virtual CVECTOR GetPos() const = 0;
    virtual CVECTOR GetAng() const = 0;

    virtual void SetPos(const CVECTOR &vNewPos) = 0;
    virtual void SetAngleY(float fAngleY) = 0;
};

// ============================================================================
// master class AIShip
// Contain base virtual functions
// ============================================================================
class AIShip : public VAI_INNEROBJ
{
  private:
    struct can_fire_t
    {
        AIShip *pShip;
        AICannon *pFortCannon;
        float fDistance;

        bool operator<(can_fire_t &other) const
        {
            return fDistance < other.fDistance;
        };
    };

    struct AI_POINT
    {
        AI_POINT() = default;

        AI_POINT(VAI_INNEROBJ *_pObj)
            : fAngle(0)
        {
            pObj = _pObj;
        }
        ;

        bool operator==(const AI_POINT &pOtherPoint) const
        {
            return pOtherPoint.pObj == pObj;
        };

        VAI_INNEROBJ *pObj; // attached object
        float fAngle;       // angle of attack
    };

    ATTRIBUTES *pAShipBase;

    AIShipTaskController *pTaskController;
    AIShipMoveController *pMoveController;
    AIShipCannonController *pCannonController;
    AIShipCameraController *pCameraController;
    AIShipTouchController *pTouchController;
    AIShipRotateController *pRotateController;
    AIShipSpeedController *pSpeedController;

    DTimer dtFireTime, dtCheckSituation, dtUpdateSeaAIAttributes;

    std::string sGroupName;
    entid_t eidShip;

    float fAbordageDistance, fFollowDistance, fAttackDistance;

    static std::vector<can_fire_t> aShipFire;
    std::vector<AI_POINT> aFollowPoints, aAttackPoints;

    void SetSeaAIAttributes(ATTRIBUTES *pAAttr, VAI_INNEROBJ *pObj) const;

  public:
    ATTRIBUTES *GetAShip() const
    {
        return GetACharacter()->FindAClass(GetACharacter(), "Ship");
    };

    // some static functions
    static AIShip *FindShip(ATTRIBUTES *pACharacter);

    static bool ShipFire(ATTRIBUTES *pACharacter, bool bCameraOutside);

    static void ShipSetAttack(uint32_t dwPriority, ATTRIBUTES *pACharacter1, ATTRIBUTES *pACharacter2);
    static void ShipSetRunAway(uint32_t dwPriority, ATTRIBUTES *pACharacter1);
    static void ShipSetMove(uint32_t dwPriority, ATTRIBUTES *pACharacter1, ATTRIBUTES *pACharacter2);
    static void ShipSetMove(uint32_t dwPriority, ATTRIBUTES *pACharacter1, CVECTOR &vPnt);
    static void ShipSetDrift(uint32_t dwPriority, ATTRIBUTES *pACharacter1);
    static void ShipSetDefend(uint32_t dwPriority, ATTRIBUTES *pACharacter1, ATTRIBUTES *pACharacter2);
    static void ShipSetBrander(uint32_t dwPriority, ATTRIBUTES *pACharacter1, ATTRIBUTES *pACharacter2);
    static void ShipSetAbordage(uint32_t dwPriority, ATTRIBUTES *pACharacter1, ATTRIBUTES *pACharacter2);

    static void ReloadCannons(ATTRIBUTES *pACharacter);

    // Group section
    void SetGroupName(std::string &_sGroupName)
    {
        sGroupName = _sGroupName;
    };

    std::string &GetGroupName()
    {
        return sGroupName;
    };

    // AI section
    void SwapShips(AIShip *pOtherShip);

    bool isAttack(ATTRIBUTES *pAOtherCharacter) const;
    void CheckSituation();
    float GetPower() const;
    float GetShipHP() const;
    float GetShipBaseHP() const;
    float GetAttackHP(float fDistance);
    float GetDefendHP();

    virtual void GetPrediction(float fTime, CVECTOR *vPos, CVECTOR *vAng);
    //

    // controllers
    AIShipMoveController *GetMoveController() const
    {
        return pMoveController;
    };

    AIShipCannonController *GetCannonController() const
    {
        return pCannonController;
    };

    AIShipTaskController *GetTaskController() const
    {
        return pTaskController;
    };

    AIShipCameraController *GetCameraController() const
    {
        return pCameraController;
    };

    AIShipTouchController *GetTouchController() const
    {
        return pTouchController;
    };

    AIShipRotateController *GetRotateController() const
    {
        return pRotateController;
    };

    AIShipSpeedController *GetSpeedController() const
    {
        return pSpeedController;
    };

    // global ship container, accessible for AIShip, AIGroup and SEA_AI.
    static std::vector<AIShip *> AIShips;

    // inherit functions from VAI_INNEROBJ
    void SetACharacter(ATTRIBUTES *pAP) override;
    void ReleasePoint(VAI_INNEROBJ *) override;

    CVECTOR GetAttackPoint(VAI_INNEROBJ *) override;
    CVECTOR GetFollowPoint(VAI_INNEROBJ *) override;
    CVECTOR GetAbordagePoint(VAI_INNEROBJ *pOtherObj) override;

    float GetMaxFireDistance() override
    {
        return GetCannonController()->GetFireDistance(true);
    };

    float GetMinFireDistance() override
    {
        return GetCannonController()->GetFireDistance(false);
    };

    // entid_t functions
    entid_t GetShipEID() const
    {
        return eidShip;
    };

    Entity *GetShipPointer() const
    {
        return EntityManager::GetEntityPointer(GetShipEID());
    };

    SHIP_BASE *GetShipBasePointer() const
    {
        return static_cast<SHIP_BASE *>(GetShipPointer());
    };

    VAI_OBJBASE *GetAIObjShipPointer() const
    {
        return static_cast<VAI_OBJBASE *>(GetShipPointer());
    };

    // inherit functions from VAI_OBJBASE
    CMatrix *GetMatrix() const
    {
        return GetAIObjShipPointer()->GetMatrix();
    };

    MODEL *GetModel() const
    {
        return GetAIObjShipPointer()->GetModel();
    };

    entid_t GetModelEID() const
    {
        return GetAIObjShipPointer()->GetModelEID();
    };

    CVECTOR GetBoxsize() const override
    {
        return GetAIObjShipPointer()->GetBoxsize();
    };

    CVECTOR GetPos() const override
    {
        return GetAIObjShipPointer()->GetPos();
    };

    CVECTOR GetAng() const override
    {
        return GetAIObjShipPointer()->GetAng();
    };

    void SetPos(const CVECTOR &vNewPos) override
    {
        GetAIObjShipPointer()->SetPos(vNewPos);
    };

    void SetAngleY(float fAngleY) override
    {
        GetShipBasePointer()->State.vAng.y = fAngleY;
    }

    // battle section
    bool isCanFire(const CVECTOR &vFirePos) const override;
    virtual bool Fire(bool bCameraOutside);

    // execute/realize section
    virtual void Execute(float fDeltaTime);
    virtual void Realize(float fDeltaTime);

    // default constructor/destructor
    AIShip(AI_OBJTYPE);
    ~AIShip() override;

    void Unload() const;
    void CreateShip(entid_t _eidShip, ATTRIBUTES *_pACharacter, ATTRIBUTES *_pAShip, CVECTOR *vInitPos);
    void CheckStartPosition() const;
    bool isCanPlace(CVECTOR vNewPos) const;

    void Save(CSaveLoad *pSL) const;
    void Load(CSaveLoad *pSL);
};

// ============================================================================
// child of AIShip : war type ship
// ============================================================================
class AIShipWar : public AIShip
{
  public:
    AIShipWar();
    ~AIShipWar() override;
};

// ============================================================================
// child of AIShip : trade type ship
// ============================================================================
class AIShipTrade : public AIShip
{
  public:
    AIShipTrade();
    ~AIShipTrade() override;
};

// ============================================================================
// child of AIShip : boat type ship
// ============================================================================
class AIShipBoat : public AIShip
{
  public:
    AIShipBoat();
    ~AIShipBoat() override;
};
