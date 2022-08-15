#pragma once

#include "c_vector.h"
#include "vai_objbase.h"
#include "vma.hpp"

struct SHIP_STATE
{
    CVECTOR vPos, vAng;
    CVECTOR vSpeed, vInert;
    CVECTOR vRotate, vAngInert;
    CVECTOR vBoxSize, vRealBoxSize;
    CVECTOR vInertiaAccel, vInertiaBrake;

    float fInertiaFactor, fMassInertia, fWaterResis;
    float fWeight; // current weight of ship in cwt

    float fShipSpeedZ;    // maximum speed of ship
    float fShipSpeedY;    // maximum rotate speed of ship
    float fLoad;          // current load status
    float fShipImmersion; // current ship immersion(depends on goods)
};

struct STRENGTH
{
    BOOL bUse;       // true if use
    BOOL bInertia;   // inertia(true) or impulse(false)
    CVECTOR vSpeed;  // speed power
    CVECTOR vRotate; // rotate power
};

struct TOUCH_PARAMS
{
    CVECTOR vSpeed;  // speed vector
    CVECTOR vRotate; // rotate speed vector
    CVECTOR vPos, vAng;
};

class SHIP_BASE : public VAI_OBJBASE
{
  protected:
    ATTRIBUTES *pAShip;

  public:
    SHIP_STATE State;

    ~SHIP_BASE() override = default;

    virtual ATTRIBUTES *GetAShip()
    {
        return pAShip;
    }

    virtual BOOL BuildContour(CVECTOR *vContour, int32_t &iNumVContour) = 0;

    virtual BOOL TouchMove(uint32_t DeltaTime, TOUCH_PARAMS *pTPOld, TOUCH_PARAMS *pTPNew) = 0;
    virtual int32_t AddStrength(STRENGTH *strength) = 0;
    virtual bool DelStrength(int32_t iIdx) = 0;

    virtual float GetMaxSpeedZ() = 0;
    virtual float GetMaxSpeedY() = 0;
    virtual float GetWindAgainst() = 0;

    virtual void SetSpeed(float fSpeed) = 0;
    virtual float GetSpeed() = 0;

    virtual void SetRotate(float fRotSpeed) = 0;
    virtual float GetRotate() = 0;

    virtual float GetBrakingDistance(float *pfTime = nullptr) = 0;
    virtual float GetRotationAngle(float *pfTime = nullptr) = 0;

    virtual float GetCurrentSpeed() = 0;

    virtual void SetDead() = 0;
    virtual bool isDead() = 0;

    virtual void SetLightAndFog(bool bSetLight) = 0;
    virtual void RestoreLightAndFog() = 0;

    // set ship lights
    virtual void SetLights() = 0;
    virtual void UnSetLights() = 0;
    void Fire(const CVECTOR &vPos) override = 0;

    virtual void SetFixedSpeed(bool bSetFixed, float fFixedSpeed) = 0;
};
