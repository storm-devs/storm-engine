//============================================================================================
//	Spirenkov Maxim, 2001
//--------------------------------------------------------------------------------------------
//	Sea Dogs II
//--------------------------------------------------------------------------------------------
//	WdmPlayerShip
//--------------------------------------------------------------------------------------------
//	
//============================================================================================

#ifndef _WdmPlayerShip_H_
#define _WdmPlayerShip_H_

#include "WdmShip.h"

class WdmPlayerShip : public WdmShip  
{
//--------------------------------------------------------------------------------------------
//Конструирование, деструктурирование
//--------------------------------------------------------------------------------------------
public:
	WdmPlayerShip();
	virtual ~WdmPlayerShip();

	void PushOutFromIsland();

	void SetActionRadius(float radius);

	//Расчёты
	virtual void Update(float dltTime);
	virtual void LRender(VDX8RENDER * rs);

	//Проверка при выходе из карты
	bool ExitFromMap();
	//Проверка на нахождении кораблика в шторме
	long TestInStorm();

	float GetAy();

	bool canSkip;

protected:
	//Переместить кораблик
	virtual void Move(float dltTime);

	virtual void Collide();

//--------------------------------------------------------------------------------------------
//Инкапсуляция
//--------------------------------------------------------------------------------------------
private:
	bool goForward;
	float actionRadius;	
	float stormEventTime;
};

inline float WdmPlayerShip::GetAy()
{
	return ay;
}

#endif

