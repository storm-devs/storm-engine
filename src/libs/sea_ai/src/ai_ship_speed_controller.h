#pragma once

#include "vai_objbase.h"
#include "vma.hpp"

// ============================================================================
// master class AIShipSpeedController
// Contain base virtual functions
// ============================================================================
class AIShip;

class AIShipSpeedController
{
  private:
    AIShip *pOurAIShip; // our AI ship pointer

    uint32_t dwSpeedNum;
    float fSpeedSmooth, fSpeed, fSpeedTime, fTopSpeed;
    float fGlobalMultiply;

  public:
    void TopSpeed(float _fSpeed);
    void MulSpeed(float _fSpeed);
    void AddSpeed(float _fSpeed);
    void SetGlobalMultiply(float _fGlobalMul);

    // init/execute/realize section
    virtual bool Init();
    virtual void Execute(float);
    virtual void Realize(float);

    // set our ship pointer
    void SetAIShip(AIShip *pShip)
    {
        pOurAIShip = pShip;
    }

    AIShip *GetAIShip() const
    {
        return pOurAIShip;
    }

    AIShipSpeedController(AIShip *pShip);
    virtual ~AIShipSpeedController();

    void Save(CSaveLoad *pSL) const;
    void Load(CSaveLoad *pSL);
};
