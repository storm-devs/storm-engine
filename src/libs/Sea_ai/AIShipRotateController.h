#ifndef AISHIPROTATECONTROLLER_HPP
#define AISHIPROTATECONTROLLER_HPP

#include "Script_Defines.h"
#include "character.h"
#include "sd2_h\CannonTrace.h"
#include "sd2_h\VAI_ObjBase.h"
#include "templates\array.h"
#include "templates\string.h"
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

    dword dwRotateNum;
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
    AIShip *GetAIShip()
    {
        return pOurAIShip;
    }

    AIShipRotateController(AIShip *pShip);
    virtual ~AIShipRotateController();

    void Save(CSaveLoad *pSL);
    void Load(CSaveLoad *pSL);
};

#endif