//============================================================================================
//	Spirenkov Maxim
//--------------------------------------------------------------------------------------------
//	
//--------------------------------------------------------------------------------------------
//	Lizard
//--------------------------------------------------------------------------------------------
//	
//============================================================================================

#ifndef _Lizard_h_
#define _Lizard_h_

#include "LocLife.h"

class Lizard : public LocLife
{
public:
	Lizard();
	virtual ~Lizard();

private:
	virtual const char * GetModelName();
	virtual const char * GetAniName();
	virtual bool PostInit(Animation * ani);

	virtual void IdleProcess(Animation * ani, float dltTime);
	virtual void MoveProcess(Animation * ani, float dltTime);
	virtual void IsStartMove(Animation * ani);
	virtual void IsStopMove(Animation * ani);

private:
	float lastMove;
};

#endif
