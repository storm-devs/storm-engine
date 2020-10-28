#ifndef _SHIP_H_
#define _SHIP_H_

#include "Island_Base.h"
#include "character.h"
#include "collide.h"
#include "dx8render.h"
#include "geometry.h"
#include "geos.h"
#include "mast_msg.h"
#include "model.h"
#include "sea_base.h"
#include "ship_base.h"
#include "vdata.h"
#include <stdio.h>

#include "ShipLights.h"
#include "fireplace.h"

#include "templates\array.h"
#include "templates\dtimer.h"
#include "templates\string.h"

#include "sd2_h\SaveLoad.h"

#define DELTA_TIME(x) ((x)*0.001f)
#define DELTA_TIME_ROTATE(x) ((x)*1.0f / 10.0f)

#define MAX_STRENGTH 16
#define STRENGTH_MAIN 0
#define RESERVED_STRENGTH 1

#define MAX_KEEL_POINTS 5

#define MAST_IDENTIFY "mast"
#define MAST_FIRST 1
#define TOPMAST_BEGIN 100 // начало нумерации стеньг (если есть)
#define HULL_IDENTIFY "shatter"

class SHIP : public SHIP_BASE
{
  protected:
    // struct section
    struct mast_t
    {
        NODE *pNode;        // node pointer in model
        CVECTOR vSrc, vDst; // src and dest vectors
        long iMastNum;      // mast number
        bool bBroken;       // if mast is broken then pNode = 0
        float fDamage;
    };

    struct hull_t
    {
        NODE *pNode;        // node pointer in model
        CVECTOR vSrc, vDst; // src and dest vectors
        long iHullNum;      // hull detail number
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
    static string sRealizeLayer, sExecuteLayer;
    char cShipIniName[256];
    long iShipPriorityExecute, iShipPriorityRealize;
    float fGravity;

    float fRockingY, fRockingAZ;

    // entity_id section
    ENTITY_ID model_id, sphere[36];
    ENTITY_ID sail_id, rope_id, flag_id, cannon_id, vant_id, vantl_id, vantz_id, touch_id, sea_id, blots_id;

    static VDX8RENDER *pRS;
    static SEA_BASE *pSea;
    static ISLAND_BASE *pIsland;
    static COLLIDE *pCollide;
    static VGEOMETRY *pGS;

    CMatrix mRoot;
    CVECTOR vSpeed, vSpeedsA;
    float fMinusVolume;
    float fXOffset, fZOffset;
    float fXHeel, fZHeel;

    bool bModelUpperShip;
    MODEL *pModelUpperShip;
    float fUpperShipAY, fUpperShipY;

    // Ships lights
    IShipLights *pShipsLights;

    // Fire places
    array<FirePlace> aFirePlaces;

    // Sound section

    // temporary used
    bool bMassaShow;
    long uniIDX;
    bool bUse;
    ship_point_t ShipPoints[16][16];

    // executed parameters
    CVECTOR vSpeedAccel;
    ship_t SP;
    CVECTOR vPos, vAng;
    float fSailState;
    float fWaterLine;
    bool bDead, bVisible;
    CVECTOR vDeadDir, vCurDeadDir;
    CVECTOR vKeelContour[MAX_KEEL_POINTS];
    long iNumMasts;
    long iNumHulls;
    mast_t *pMasts;
    hull_t *pHulls;
    bool bShip2Strand;
    bool bMounted;
    bool bKeelContour;
    CVECTOR vOldAng, vOldPos;
    bool bSetFixed;
    float fFixedSpeed;

    // fast turn perk parameters
    bool bPerkTurnActive;
    float fInitialPerkAngle, fResultPerkAngle;

    STRENGTH Strength[MAX_STRENGTH];

    DTimer dtMastTrace, dtUpdateParameters;

    // executed functions
    CVECTOR ShipRocking(float fDeltaTime);
    BOOL ApplyStrength(float dtime, BOOL bCollision);
    BOOL CalculateNewSpeedVector(CVECTOR *Speed, CVECTOR *Rotate);
    void CalculateImmersion(); //
    void CheckShip2Strand(float fDeltaTime);
    void MastFall(mast_t *pM);
    //	void 		MastFallChild(mast_t * pM);
    void TestMastFall(long iMast);
    void HullFall(hull_t *pM);

    CMatrix UpdateModelMatrix();
    void RecalculateWorldOffset();

    // init section
    void ScanShipForFirePlaces();
    void LoadPositionFromAttributes();
    BOOL LoadShipParameters();
    void CalcRealBoxSize();

    void SetDead();
    bool isDead()
    {
        return bDead;
    };
    bool isVisible()
    {
        return bVisible;
    };

  public:
    virtual ~SHIP();
    SHIP();

    float GetMaxSpeedZ();
    float GetMaxSpeedY();
    float GetWindAgainst();
    long AddStrength(STRENGTH *strength);
    bool DelStrength(long iIdx);

    BOOL BuildContour(CVECTOR *vContour, long &iNumVContour);
    bool BuildMasts(); // создаем мачты
    bool BuildHulls();

    BOOL Move(DWORD DeltaTime, BOOL bCollision);
    BOOL TouchMove(DWORD DeltaTime, TOUCH_PARAMS *pTPOld, TOUCH_PARAMS *pTPNew);

    void LoadServices();

    // inherit functions SHIP_BASE
    bool bSetLightAndFog;
    dword dwSaveAmbient, dwSaveFogColor;
    D3DLIGHT9 saveLight;

    virtual void SetLightAndFog(bool bSetLight);
    virtual void RestoreLightAndFog();

    virtual void SetSpeed(float fSpeed);
    virtual float GetSpeed();

    virtual void SetRotate(float fRotSpeed);
    virtual float GetRotate();

    virtual float GetBrakingDistance(float *pfTime = 0);
    virtual float GetRotationAngle(float *pfTime = 0);

    virtual float GetCurrentSpeed();

    virtual void SetLights();
    virtual void UnSetLights();
    virtual void Fire(const CVECTOR &vPos);

    virtual void SetFixedSpeed(bool bSetFixed, float fFixedSpeed);

    // inherit functions COLLISION_OBJECT
    virtual float Trace(const CVECTOR &src, const CVECTOR &dst);
    virtual bool Clip(const PLANE *planes, long nplanes, const CVECTOR &center, float radius, ADD_POLYGON_FUNC addpoly)
    {
        return false;
    };

    virtual const char *GetCollideMaterialName()
    {
        return 0;
    };
    virtual bool GetCollideTriangle(Triangle &triangle)
    {
        return false;
    };

    // inherit functions CANNON_TRACE_BASE
    float Cannon_Trace(long iBallOwner, const CVECTOR &src, const CVECTOR &dst);

    // inherit functions VAI_OBJBASE
    void SetACharacter(ATTRIBUTES *pAP);

    CMatrix *GetMatrix();
    void SetMatrix(CMatrix &mtx);
    MODEL *GetModel();
    ENTITY_ID GetModelEID();
    CVECTOR GetPos();
    CVECTOR GetAng();
    CVECTOR GetBoxSize();

    void SetPos(CVECTOR &vNewPos);

    bool Mount(ATTRIBUTES *);

    // inherit functions ENTITY
    bool Init();
    void Realize(dword Delta_Time);
    void Execute(dword Delta_Time);
    dword _cdecl ProcessMessage(MESSAGE &message);
    dword AttributeChanged(ATTRIBUTES *pAttribute);

    void Save(CSaveLoad *pSL);
    void Load(CSaveLoad *pSL);
};

#endif