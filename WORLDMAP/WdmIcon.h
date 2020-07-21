//============================================================================================
//	Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//	
//--------------------------------------------------------------------------------------------
//	WdmIcon
//--------------------------------------------------------------------------------------------
//	
//============================================================================================

#ifndef _WdmIcon_H_
#define _WdmIcon_H_

#include "WdmInterfaceObject.h"

class WdmEnemyShip;

class WdmIcon : public WdmInterfaceObject  
{
	struct Icon
	{
		float u;
		float alpha;
	};

//--------------------------------------------------------------------------------------------
//Конструирование, деструктурирование
//--------------------------------------------------------------------------------------------
public:
	WdmIcon();
	virtual ~WdmIcon();

	//Расчёты
	virtual void Update(float dltTime);
	//Отрисовка
	virtual void LRender(VDX8RENDER * rs);

//--------------------------------------------------------------------------------------------
//Инкапсуляция
//--------------------------------------------------------------------------------------------
private:
	WdmEnemyShip * lastShip;
	long texture;
	Icon icon[2];
	float islandAlpha;
	float skipAlpha;
	float stormAlpha;
};

#endif

