//============================================================================================
//	Spirenkov Maxim, 2003
//--------------------------------------------------------------------------------------------
//	
//--------------------------------------------------------------------------------------------
//	LocRat
//--------------------------------------------------------------------------------------------
//	
//============================================================================================

#ifndef _LocRat_h_
#define _LocRat_h_

#include "LocLife.h"

class LocRat : public LocLife  
{
public:
	LocRat();
	virtual ~LocRat();

private:
	virtual const char * GetModelName();
	virtual const char * GetAniName();
	virtual bool PostInit(Animation * ani);

	virtual void IdleProcess(Animation * ani, float dltTime);
	virtual void MoveProcess(Animation * ani, float dltTime);
	virtual void IsStartMove(Animation * ani);
	virtual void IsStopMove(Animation * ani);

	virtual void Event(Animation * animation, long index, long eventID, AnimationEvent event);

	void StartNewAction(Animation * ani);

private:
	float nextActionTime;
	float timeout;
	float moveTime;
	bool isPanic;
};

#endif

