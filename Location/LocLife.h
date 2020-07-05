//============================================================================================
//	Spirenkov Maxim, 2003
//--------------------------------------------------------------------------------------------
//	
//--------------------------------------------------------------------------------------------
//	LocLife
//--------------------------------------------------------------------------------------------
//	
//============================================================================================

#ifndef _LocLife_h_
#define _LocLife_h_

#include "..\common_h\vmodule_api.h"
#include "..\common_h\animation.h"

class Location;
class Animation;

class LocLife : public AnimationEventListener
{
public:
	LocLife();
	virtual ~LocLife();

//--------------------------------------------------------------------------------------------
public:
	bool Init(Location * loc);
	void Update(float dltTime);

//--------------------------------------------------------------------------------------------
protected:
	virtual const char * GetModelName() = null;
	virtual const char * GetAniName() = null;
	virtual bool PostInit(Animation * ani) = null;

	virtual void IdleProcess(Animation * ani, float dltTime) = null;
	virtual void MoveProcess(Animation * ani, float dltTime) = null;
	virtual void IsStartMove(Animation * ani) = null;
	virtual void IsStopMove(Animation * ani) = null;

//--------------------------------------------------------------------------------------------
protected:
	void StartMove();
	void StopMove();
	bool IsNearPlayer(float radius);

private:
	long FindPos();
	long FindRandomPos(CVECTOR & pos);


//--------------------------------------------------------------------------------------------
protected:
	float kSpeed;
	float speed;
private:
	Location * location;
	ENTITY_ID model;
	long node;
	float ay;
	CVECTOR pos;
	CVECTOR npos;

};

#endif

