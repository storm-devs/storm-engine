#pragma once

#include "ai_ship.h"
#include "v_module_api.h"

// ============================================================================
// Master class AIGroup
// Contain base virtual functions
// ============================================================================
class AIGroup
{
  private:
    DTimer dtCheckTask;
    std::string sCommand, sCommandGroup;
    std::string sGroupName;
    std::string sLocationNearOtherGroup;
    std::string sGroupType;
    CVECTOR vInitGroupPos, vMovePoint;
    bool bFirstExecute;
    uint32_t iWarShipsNum, iTradeShipsNum;

    ATTRIBUTES *pACommander;

    // ship container for this group
    std::vector<AIShip *> aGroupShips;

  public:
    AIGroup(){};
    AIGroup(const char *pGroupName);
    virtual ~AIGroup();

    // global group container, accessible for AIGroup and SEA_AI.
    static std::vector<AIGroup *> AIGroups;
    static float fDistanceBetweenGroupShips;
    static float fDistanceBetweenGroupLines;

    // static functions
    static AIGroup *FindGroup(ATTRIBUTES *pACharacter);
    static AIGroup *FindGroup(const char *pGroupName);
    static AIGroup *FindOrCreateGroup(const char *pGroupName);
    static AIGroup *CreateNewGroup(const char *pGroupName);
    static AIGroup *FindMainGroup();
    static float GetAttackHP(const char *pGroupName, float fDistance);
    static void SetXYZ_AY(const char *pGroupName, CVECTOR vPos, float _fAY);
    static void GroupSetMove(const char *pGroupName, CVECTOR &vMovePoint);
    static void GroupSetAttack(const char *cGroupName, const char *cGroupAttackingName);
    static void GroupSetAttack(AIShip *pS1, AIShip *pS2);
    static void GroupSetRunAway(const char *pGroupName);
    static void GroupSetType(const char *pGroupName, const char *cGroupType);
    static void GroupSetCommander(const char *pGroupName, ATTRIBUTES *_pACommander);
    static void GroupSetLocationNearOtherGroup(const char *pGroupName, const char *pOtherGroupName);
    static void ShipChangeGroup(ATTRIBUTES *pACharacter, const char *pGroupName);
    static void SwapCharactersShips(ATTRIBUTES *pACharacter1, ATTRIBUTES *pACharacter2);
    static void SetOfficerCharacter2Ship(ATTRIBUTES *pOfficerCharacter, ATTRIBUTES *pReplacedACharacter);
    static void SailMainGroup(CVECTOR vPos, float fAngleY, ATTRIBUTES *pACharacter);

    static void GroupHelpMe(const char *pGroupName, AIShip *pMe, AIShip *pEnemy);

    // AI section
    bool isAttack(AIGroup *pGroup);
    bool isDead(); // is group dead?
    bool isMainGroup();
    float GetPower();

    // return group name
    std::string &GetName()
    {
        return sGroupName;
    };

    // extract / insert ship group
    AIShip *ExtractShip(ATTRIBUTES *pACharacter);
    void InsertShip(AIShip *pAIShip);

    // add new ship to group
    ATTRIBUTES *GetCommanderACharacter() const;
    AIShip *GetMainShip();
    void AddShip(entid_t _eidShip, ATTRIBUTES *pACharacter, ATTRIBUTES *pAShip);

    void Unload(){};

    // execute/realize function (on each frame)
    void Realize(float fDeltaTime);
    void Execute(float fDeltaTime);

    void Save(CSaveLoad *pSL);
    void Load(CSaveLoad *pSL);
};
