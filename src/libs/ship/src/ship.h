#pragma once

#include <array>
#include <vector>

#include "collide.h"
#include "d_timer.h"
#include "dx9render.h"
#include "fire_place.h"
#include "geometry.h"
#include "island_base.h"
#include "model.h"
#include "save_load.h"
#include "sea_base.h"
#include "ship_base.h"
#include "ship_msg.h"

#define DELTA_TIME(x) ((x)*0.001f)
#define DELTA_TIME_ROTATE(x) ((x)*1.0f / 10.0f)

#define MAX_STRENGTH 16
#define STRENGTH_MAIN 0
#define RESERVED_STRENGTH 1

#define MAX_KEEL_POINTS 5

#define MAST_IDENTIFY "mast"
#define MAST_FIRST 1
#define TOPMAST_BEGIN 100 // start of topmast numbering (if any)
#define HULL_IDENTIFY "shatter"

class SHIP : public SHIP_BASE
{
  protected:
    // struct section
    struct mast_t
    {
        NODE *pNode;        // node pointer in model
        CVECTOR vSrc, vDst; // src and dest vectors
        int32_t iMastNum;   // mast number
        bool bBroken;       // if mast is broken then pNode = 0
        float fDamage;
    };

    struct hull_t
    {
        NODE *pNode;        // node pointer in model
        CVECTOR vSrc, vDst; // src and dest vectors
        int32_t iHullNum;   // hull detail number
        bool bBroken;       // if hull detail is broken then pNode = 0
        float fDamage;
    };

    struct ship_point_t
    {
        float fSpeed;
        float fGravity; // acceleration
        float fExpulsiveForce;
        bool bUnderWater;

        float fY;
        float fA;
        float fV;
    };

    // init parameters
    inline static layer_index_t RealizeLayer, ExecuteLayer;
    char cShipIniName[256];
    int32_t iShipPriorityExecute, iShipPriorityRealize;
    float fGravity;

    float fRockingY, fRockingAZ;

    // entid_t section
    entid_t model_id, sphere[36];
    entid_t sail_id, rope_id, flag_id, cannon_id, vant_id, vantl_id, vantz_id, touch_id, sea_id, blots_id;

    static VDX9RENDER *pRS;
    static SEA_BASE *pSea;
    static ISLAND_BASE *pIsland;
    static COLLIDE *pCollide;
    static VGEOMETRY *pGS;

    CMatrix mRoot;
    CVECTOR vSpeed, vSpeedsA;
    float fMinusVolume;
    float fXOffset, fZOffset;

    bool bModelUpperShip;
    MODEL *pModelUpperShip;
    float fUpperShipAY, fUpperShipY;

    // Ships lights
    entid_t shipLights;
    entid_t flagEntity_{};

    // Fire places
    std::vector<FirePlace> aFirePlaces;

    // Sound section

    // temporary used
    bool bMassaShow;
    int32_t uniIDX;
    bool bUse;
    std::array<std::array<ship_point_t, 16>, 16> ShipPoints;

    // executed parameters
    CVECTOR vSpeedAccel;
    ship_t SP;
    CVECTOR vPos, vAng;
    float fSailState;
    float fWaterLine;
    bool bDead, bVisible;
    CVECTOR vDeadDir, vCurDeadDir;
    CVECTOR vKeelContour[MAX_KEEL_POINTS];
    int32_t iNumMasts;
    int32_t iNumHulls;
    std::vector<mast_t> pMasts;
    std::vector<hull_t> pHulls;
    bool bShip2Strand;
    bool bMounted;
    bool bKeelContour;
    CVECTOR vOldAng, vOldPos;
    bool bSetFixed;
    float fFixedSpeed;

    // fast turn perk parameters
    bool bPerkTurnActive;
    float fInitialPerkAngle, fResultPerkAngle;

    std::array<STRENGTH, MAX_STRENGTH> Strength;

    DTimer dtMastTrace, dtUpdateParameters;

    // executed functions
    CVECTOR ShipRocking(float fDeltaTime);
    BOOL ApplyStrength(float dtime, BOOL bCollision);
    BOOL CalculateNewSpeedVector(CVECTOR *Speed, CVECTOR *Rotate);
    void CalculateImmersion(); //
    void CheckShip2Strand(float fDeltaTime);
    void MastFall(mast_t *pM);
    void HullFall(hull_t *pM);
    void FakeFire(const char *sBort, float fRandTime);

    CMatrix UpdateModelMatrix();
    void RecalculateWorldOffset();

    // init section
    void ScanShipForFirePlaces();
    void LoadPositionFromAttributes();
    BOOL LoadShipParameters();
    void CalcRealBoxsize();

    void SetDead() override;

    bool isDead() override
    {
        return bDead;
    };

    bool isVisible()
    {
        return bVisible;
    };

  public:
    ~SHIP() override;
    SHIP();

    float GetMaxSpeedZ() override;
    float GetMaxSpeedY() override;
    float GetWindAgainst() override;
    int32_t AddStrength(STRENGTH *strength) override;
    bool DelStrength(int32_t iIdx) override;

    BOOL BuildContour(CVECTOR *vContour, int32_t &iNumVContour) override;
    bool BuildMasts();
    bool BuildHulls();
    BOOL Move(uint32_t DeltaTime, BOOL bCollision);
    BOOL TouchMove(uint32_t DeltaTime, TOUCH_PARAMS *pTPOld, TOUCH_PARAMS *pTPNew) override;

    void LoadServices();

    // inherit functions SHIP_BASE
    bool bSetLightAndFog;
    uint32_t dwSaveAmbient, dwSaveFogColor;
    D3DLIGHT9 saveLight;

    void SetLightAndFog(bool bSetLight) override;
    void RestoreLightAndFog() override;

    void SetSpeed(float fSpeed) override;
    float GetSpeed() override;

    void SetRotate(float fRotSpeed) override;
    float GetRotate() override;

    float GetBrakingDistance(float *pfTime = nullptr) override;
    float GetRotationAngle(float *pfTime = nullptr) override;

    float GetCurrentSpeed() override;

    void SetLights() override;
    void UnSetLights() override;
    void Fire(const CVECTOR &vPos) override;

    void SetFixedSpeed(bool bSetFixed, float fFixedSpeed) override;

    // inherit functions COLLISION_OBJECT
    float Trace(const CVECTOR &src, const CVECTOR &dst) override;

    bool Clip(const PLANE *planes, int32_t nplanes, const CVECTOR &center, float radius,
              ADD_POLYGON_FUNC addpoly) override
    {
        return false;
    };

    const char *GetCollideMaterialName() override
    {
        return nullptr;
    };

    bool GetCollideTriangle(TRIANGLE &triangle) override
    {
        return false;
    };

    // inherit functions CANNON_TRACE_BASE
    float Cannon_Trace(int32_t iBallOwner, const CVECTOR &src, const CVECTOR &dst) override;

    // inherit functions VAI_OBJBASE
    void SetACharacter(ATTRIBUTES *pAP) override;

    CMatrix *GetMatrix() override;
    void SetMatrix(CMatrix &mtx);
    MODEL *GetModel() const override;
    entid_t GetModelEID() const override;
    CVECTOR GetPos() const override;
    CVECTOR GetAng() const override;
    CVECTOR GetBoxsize() const override;
    CVECTOR GetRealBoxsize() const override;

    void SetPos(const CVECTOR &vNewPos) override;

    bool Mount(ATTRIBUTES *) override;

    // inherit functions Entity
    bool Init() override;
    void Realize(uint32_t Delta_Time);
    void Execute(uint32_t Delta_Time);
    uint64_t ProcessMessage(MESSAGE &message) override;
    uint32_t AttributeChanged(ATTRIBUTES *pAttribute) override;

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

    void Save(CSaveLoad *pSL) override;
    void Load(CSaveLoad *pSL) override;
};
