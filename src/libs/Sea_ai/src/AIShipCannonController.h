#pragma once

#define INVALID_BORT_INDEX 0xFFFFFFFF

class AIShip;
class VAI_INNEROBJ;

// ============================================================================
// master class AIShipCannonController
// Contain base virtual functions
// ============================================================================
class AIShipCannonController
{
  private:
    AIShip *pOurAIShip;

    ATTRIBUTES *pAShip;

    bool bReload;         // we must start reload at next frame
    bool bNotEnoughBalls; // if we haven't enough balls
    bool bTempFlag;

    RS_RECT rs;

  private:
    struct AISHIP_BORT
    {
        std::string sName;              // bort name(for compare with label name)
        float fFireZone;                // bort fire angle
        float fFireAngMin, fFireAngMax; // bort height fire angle
        float fFireDir;                 // bort fire direction
        float fOurBortFireHeight;       // bort fire height
        float fChargePercent;           // Recharge percent
        float fCosFireZone;

        uint32_t dwNumDamagedCannons;

        // each frame parameters
        float fSpeedV0;         // current fire speed
        float fMaxFireDistance; // maximum fire distance
        // CVECTOR                vAngleVectors[2];        // fire cone (2d(x,z))
        CVECTOR vDirection;

        std::vector<AICannon> aCannons; // cannons container for this bort

        void ClearCharge()
        {
            fChargePercent = 0.0f;
        };

        bool isCharged() const
        {
            return fChargePercent >= 1.0f;
        };

        bool isBortDamaged() const
        {
            return dwNumDamagedCannons == aCannons.size();
        }

        AISHIP_BORT()
        {
            ClearCharge();
            fOurBortFireHeight = 0.0f;
        };

        int operator==(const char *pStr) const
        {
            Assert(pStr && pStr[0]);
            return (_stricmp(sName.c_str(), pStr) == 0);
        }
    };

    // borts container
    std::vector<AISHIP_BORT> aShipBorts;

    // update borts parameters
    bool UpdateParameters();

    bool ScanShipForCannons();

    bool Fire2Position(uint32_t dwBort, const CVECTOR &vFirePos, float fFireHeight);

    float GetSpeedV0();

  public:
    uint32_t GetCannonsNum();

    float GetFireDistance(bool bMaxFireDistance);

    // bort section
    float GetBortHeightAngle(long iBortIndex);
    bool isCanFireBort(uint32_t dwBort, const CVECTOR &vFirePos, float *pfZapasDistance = nullptr);
    uint32_t GetFirstFireBort(const CVECTOR &vFirePos, float *pfZapasDistance = nullptr);
    bool isHaveEnoughtBallsForBort(uint32_t dwBortIdx);
    uint32_t GetNextFireBort(uint32_t dwPrevBort, const CVECTOR &vFirePos, float *pfZapasDistance = nullptr);
    CVECTOR GetBortDirection(uint32_t dwBort);
    uint32_t GetBestFireBortOnlyDistance(CVECTOR vFirePos, float fZapasDistance);
    CVECTOR GetFirePos(const CVECTOR &vFireDir);
    CVECTOR GetFirePos(const CVECTOR &vFireDir, float fDistance);
    uint32_t GetBortIntactCannonsNum(uint32_t dwBortIdx);
    uint32_t GetBortDisabledCannonsNum(uint32_t dwBortIdx);

    // fire test
    bool isCanFirePos(const CVECTOR &vFirePos); // is we can fire to position
    bool isCanFire(AIShip *pEnemy);             // is we can fire to enemy ship
    bool isCanFire(const CVECTOR &vCamDir);     // is we can fire to camera direction

    // fire section
    bool Fire(const CVECTOR &vFirePos);                             // fire to position
    bool Fire(AIShip *pEnemy);                                      // fire to ship
    bool Fire(const CVECTOR &vFireCamPos, const CVECTOR &vFireDir); // manual fire

    // reload section
    void Unload(); // unload cannons
    void Reload(); // set flag to reload

    // temp
    void AddTrg(CVECTOR *pVerts, uint32_t dwColor);
    // temp

    // Cannon boom check
    void CheckCannonsBoom(float fTmpCannonDamage, const CVECTOR &vPnt);
    void ResearchCannons(); // boal 08.08.06 method of recalculating guns on a ship

    // execute/realize section
    void Execute(float fDeltaTime);
    void Realize(float fDeltaTime);

    // init section
    bool Init(ATTRIBUTES *_pAShip);

    void SetAIShip(AIShip *pShip)
    {
        pOurAIShip = pShip;
    }

    AIShip *GetAIShip() const
    {
        return pOurAIShip;
    }

    // default constructor/destructor
    AIShipCannonController(AIShip *);
    ~AIShipCannonController();

    static float fMaxCannonDamageDistance;
    static float fMaxCannonDamageRadiusPoint;

    void Save(CSaveLoad *pSL);
    void Load(CSaveLoad *pSL);
};
