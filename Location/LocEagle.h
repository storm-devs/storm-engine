//============================================================================================
//	Spirenkov Maxim, 2003
//--------------------------------------------------------------------------------------------
//	
//--------------------------------------------------------------------------------------------
//	LocEagle
//--------------------------------------------------------------------------------------------
//	
//============================================================================================

#ifndef _LocEagle_H_
#define _LocEagle_H_

#include "..\common_h\model.h"

class LocEagle : public ENTITY  
{
public:
	LocEagle();
	virtual ~LocEagle();

	//Инициализация
	bool Init();
	//Исполнение
	void Execute(dword delta_time);


	ENTITY_ID mdl;
	CVECTOR cnt;
	float kTime, time;
	float y, dltY;
	float kRad, dltRad;
	float ay;
	float kAy, timeAy;
};

#endif
