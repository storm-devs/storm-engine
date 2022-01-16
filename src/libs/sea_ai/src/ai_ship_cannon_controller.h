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
    AIShip *pOurAIShip{};

    ATTRIBUTES *pAShip{};

    bool bReload;         // we must start reload at next frame
    bool bNotEnoughBalls; // if we haven't enough balls

    bool debugDrawToggle{false};
    std::vector<std::tuple<CVECTOR, uint32_t, float>> debugFirePositions;

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
        }

        [[nodiscard]] bool isCharged() const
        {
            return fChargePercent >= 1.0f;
        }

        [[nodiscard]] bool isBortDamaged() const
        {
            return dwNumDamagedCannons == aCannons.size();
        }

        AISHIP_BORT()
            : fOurBortFireHeight(0.0f)
        {
            ClearCharge();
        }

        int operator==(const char *pStr) const
        {
            Assert(pStr && pStr[0]);
            return (storm::iEquals(sName, pStr));
        }
    };

    // borts container
    std::vector<AISHIP_BORT> aShipBorts;

    // update borts parameters
    bool UpdateParameters();

    bool ScanShipForCannons();

    bool Fire2Position(AISHIP_BORT &bort, const CVECTOR &vFirePos, float fFireHeight);

    [[nodiscard]] float GetSpeedV0() const;

  public:
    [[nodiscard]] uint32_t GetCannonsNum() const;

    [[nodiscard]] float GetFireDistance(bool bMaxFireDistance) const;

    // bort section
    [[nodiscard]] decltype(aShipBorts)::iterator GetFirstFireBort(const CVECTOR &vFirePos,
                                                                  float *pfZapasDistance = nullptr);
    [[nodiscard]] decltype(aShipBorts)::iterator GetNextFireBort(decltype(aShipBorts)::iterator bortIt,
                                                                 const CVECTOR &vFirePos,
                                                   float *pfZapasDistance = nullptr);
    [[nodiscard]] bool IsValid(decltype(aShipBorts)::const_iterator bortIt) const;

    [[nodiscard]] float GetBortHeightAngle(const AISHIP_BORT &bort) const;
    [[nodiscard]] bool isCanFireBort(const AISHIP_BORT &bort, const CVECTOR &vFirePos,
                                     float *pfZapasDistance = nullptr) const;
    [[nodiscard]] bool isHaveEnoughBallsForBort(const AISHIP_BORT &bort) const;

    [[nodiscard]] CVECTOR GetBortDirection(const AISHIP_BORT &bort) const;
    [[nodiscard]] decltype(aShipBorts)::iterator GetBestFireBortOnlyDistance(CVECTOR vFirePos,
                                                                             float fZapasDistance);
    CVECTOR GetFirePos(const CVECTOR &vFireDir);
    CVECTOR GetFirePos(const CVECTOR &vFireDir, float fDistance);
    [[nodiscard]] uint32_t GetBortIntactCannonsNum(const AISHIP_BORT &bort) const;
    [[nodiscard]] uint32_t GetBortDisabledCannonsNum(const AISHIP_BORT &bort) const;

    // fire test
    bool isCanFirePos(const CVECTOR &vFirePos);                 // is we can fire to position
    bool isCanFire(AIShip *pEnemy);                             // is we can fire to enemy ship
    [[nodiscard]] bool isCanFire(const CVECTOR &vCamDir) const; // is we can fire to camera direction

    // fire section
    bool Fire(const CVECTOR &vFirePos);                             // fire to position
    bool Fire(AIShip *pEnemy);                                      // fire to ship
    bool Fire(const CVECTOR &vFireCamPos, const CVECTOR &vFireDir); // manual fire

    // reload section
    void Unload(); // unload cannons
    void Reload(); // set flag to reload
    
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

    [[nodiscard]] AIShip *GetAIShip() const
    {
        return pOurAIShip;
    }

    // default constructor/destructor
    AIShipCannonController(AIShip *);

    static float fMaxCannonDamageDistance;
    static float fMaxCannonDamageRadiusPoint;

    void Save(CSaveLoad *pSL);
    void Load(CSaveLoad *pSL);
};