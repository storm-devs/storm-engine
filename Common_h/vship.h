#ifndef VSHIP_H
#define VSHIP_H
#include "..\common_h\ship_msg.h"
#include "..\common_h\vcannon.h"

class VSHIP : public ENTITY
	{
	public:

	virtual void CauseDamage(float, float, float) = 0;
	virtual float GetHullPercent() = 0;
	virtual void* GetModel() = 0;
	virtual void*  GetCannon() = 0;
	};

#endif
