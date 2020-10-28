#ifndef AISHIPSPEEDCONTROLLER_HPP
#define AISHIPSPEEDCONTROLLER_HPP

#include "Script_Defines.h"
#include "character.h"
#include "sd2_h\CannonTrace.h"
#include "sd2_h\VAI_ObjBase.h"
#include "templates\array.h"
#include "templates\string.h"
#include "vmodule_api.h"

// ============================================================================
// master class AIShipSpeedController
// Contain base virtual functions
// ============================================================================
class AIShip;
class AIShipSpeedController
{
  private:
    AIShip *pOurAIShip; // our AI ship pointer

    dword dwSpeedNum;
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
    AIShip *GetAIShip()
    {
        return pOurAIShip;
    }

    AIShipSpeedController(AIShip *pShip);
    virtual ~AIShipSpeedController();

    void Save(CSaveLoad *pSL);
    void Load(CSaveLoad *pSL);
};

#endif