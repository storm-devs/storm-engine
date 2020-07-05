#ifndef SAIL_BASE_H
#define SAIL_BASE_H
#include "..\common_h\object.h"
#include "sd2_h\CannonTrace.h"

class NODE;
//=================================================
//=================================================
class SAILONE_BASE
{
	public:

	virtual void SetAllHole(DWORD holeData) = 0;
	virtual void CalculateMirrorSailIndex() = 0;
};

class SAIL_BASE : public CANNON_TRACE_BASE
{
	public:
        // Trace function
	virtual ENTITY_ID GetShipID() = 0;
	virtual SAILONE_BASE * FindSailForCharacter(int chrIdx,char* nodeName,int grNum) = 0;
};

//==================================================
//==================================================
class ROPE_BASE : public ENTITY
{
	public:
        // service function
    virtual void GetEndPoint(CVECTOR* cv,int ropenum,ENTITY_ID &mdl_id)=0;
    virtual bool IsAbsentRope(ENTITY_ID &mdl_id, int ropenum)=0;
    virtual void DoDeleteUntie(ENTITY_ID &mdl_id, NODE *rnod, int gNum)=0;
};

#endif