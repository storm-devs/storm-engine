#ifndef AISHIPMOVECONTROLLER_HPP
#define AISHIPMOVECONTROLLER_HPP

#include "AIFlowGraph.h"
#include "Script_Defines.h"
#include "character.h"
#include "sd2_h\CannonTrace.h"
#include "sd2_h\VAI_ObjBase.h"
#include "templates\array.h"
#include "templates\string.h"
#include "vmodule_api.h"

// ============================================================================
// master class AIShipMoveController
// Contain base virtual functions
// ============================================================================
class AIShip;
class AIShipMoveController
{
  private:
    ENTITY_ID eidSphere;

    AIShip *pOurAIShip; // our AI ship pointer

    bool bStopped;      // if ship stopped
    CVECTOR vDestPoint; // destination point
    CVECTOR vRetardForce, vDeflectForce;

    float fMoveTime;

    dword dwCurPnt;

    AIFlowGraph::VectorPath *pVPath;

  public:
    // init/execute/realize section
    virtual bool Init();
    virtual void Execute(float);
    virtual void Realize(float);

    // move to point (major function!!!)
    virtual void Move(CVECTOR);
    // stop moving/rotating
    virtual void Stop(bool _bStopped = true)
    {
        bStopped = _bStopped;
    };

    virtual void AddDeflectForce(CVECTOR _vDeflectForce);
    virtual void AddRetardForce(CVECTOR _vRetardForce);

    virtual bool isStopped()
    {
        return bStopped;
    };

    // set our ship pointer
    void SetAIShip(AIShip *pShip)
    {
        pOurAIShip = pShip;
    }
    AIShip *GetAIShip()
    {
        return pOurAIShip;
    }

    AIShipMoveController(AIShip *pShip);
    virtual ~AIShipMoveController();

    void Save(CSaveLoad *pSL);
    void Load(CSaveLoad *pSL);
};

#endif