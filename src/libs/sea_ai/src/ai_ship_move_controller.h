#pragma once

#include "ai_flow_graph.h"
#include "vai_objbase.h"
#include "vma.hpp"

// ============================================================================
// master class AIShipMoveController
// Contain base virtual functions
// ============================================================================
class AIShip;

class AIShipMoveController
{
  private:
    entid_t eidSphere;

    AIShip *pOurAIShip; // our AI ship pointer

    bool bStopped;      // if ship stopped
    CVECTOR vDestPoint; // destination point
    CVECTOR vRetardForce, vDeflectForce;

    float fMoveTime;

    uint32_t dwCurPnt;

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

    AIShip *GetAIShip() const
    {
        return pOurAIShip;
    }

    AIShipMoveController(AIShip *pShip);
    virtual ~AIShipMoveController();

    void Save(CSaveLoad *pSL) const;
    void Load(CSaveLoad *pSL);
};
