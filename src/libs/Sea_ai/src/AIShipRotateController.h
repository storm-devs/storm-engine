#pragma once

#include "Sd2_h/VAI_ObjBase.h"
#include "vmodule_api.h"

// ============================================================================
// master class AIShipRotateController
// Contain base virtual functions
// ============================================================================
class AIShip;

class AIShipRotateController
{
  private:
    AIShip *pOurAIShip; // our AI ship pointer

    uint32_t dwRotateNum;
    float fRotateMode, fRotateTime, fRotateSmooth, fRotate;
    float fGlobalMultiply;

  public:
    void AddRotate(float _fRotate);
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

    AIShipRotateController(AIShip *pShip);
    virtual ~AIShipRotateController();

    void Save(CSaveLoad *pSL) const;
    void Load(CSaveLoad *pSL);
};
