#pragma once
#include "Sd2_h/CannonTrace.h"

class NODE;

//=================================================
//=================================================
class SAILONE_BASE
{
  public:
    virtual void SetAllHole(uint32_t holeData) = 0;
    virtual void CalculateMirrorSailIndex() = 0;
};

class SAIL_BASE : public CANNON_TRACE_BASE
{
  public:
    // Trace function
    virtual entid_t GetShipID() = 0;
    virtual SAILONE_BASE *FindSailForCharacter(int chrIdx, char *nodeName, int grNum) = 0;
};

//==================================================
//==================================================
class ROPE_BASE : public Entity
{
  public:
    // service function
    virtual void GetEndPoint(CVECTOR *cv, int ropenum, entid_t mdl_id) = 0;
    virtual bool IsAbsentRope(entid_t mdl_id, int ropenum) = 0;
    virtual void DoDeleteUntie(entid_t mdl_id, NODE *rnod, int gNum) = 0;
};
