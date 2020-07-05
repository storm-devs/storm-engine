//============================================================================================
//	Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//	
//--------------------------------------------------------------------------------------------
//	WdmStormCloud
//--------------------------------------------------------------------------------------------
//	
//============================================================================================

#ifndef _WdmStormCloud_H_
#define _WdmStormCloud_H_

#include "WdmCloud.h"

class WdmStormCloud : public WdmCloud
{
	struct RainVertex
	{
		float x, y, z;
		float tu, tv;
	};

//--------------------------------------------------------------------------------------------
//Конструирование, деструктурирование
//--------------------------------------------------------------------------------------------
public:
	WdmStormCloud();
	virtual ~WdmStormCloud();

	//Расчёты
	virtual void Update(float dltTime);

	//Отрисовка
	virtual void PRender(VDX8RENDER * rs);
	virtual void LRender(VDX8RENDER * rs);

//--------------------------------------------------------------------------------------------
//Инкапсуляция
//--------------------------------------------------------------------------------------------
private:
	virtual void BuildCloud(long n);
	void FillRects();


	//Параметры молнии
	long curLightning;
	float lightningWaitTime;
	float lightningTime;
	float flashTime;
	long lastColor;
	long lightningColor;

	//Дождик
	long rainTexture;
	float curU, curV;
	CVECTOR rainpos[2048];			//Позиция на партикле
	static RainVertex rain[4096];	//Текущий массив для отрисовки
};

#endif

