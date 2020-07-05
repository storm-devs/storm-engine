//============================================================================================
//	Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//	
//--------------------------------------------------------------------------------------------
//	Lights
//--------------------------------------------------------------------------------------------
//	
//============================================================================================

#ifndef _Lights_H_
#define _Lights_H_

#include "LTypes.h"

class Lights  
{
//--------------------------------------------------------------------------------------------
//Конструирование, деструктурирование
//--------------------------------------------------------------------------------------------
public:
	Lights();
	virtual ~Lights();

	void AddAmbient(const CVECTOR & color);
	void AddWeaterLights(const CVECTOR & color, const CVECTOR & dir);
	void AddPointLight(const CVECTOR & color, const CVECTOR & pos, float att0, float att1, float att2, float range, const char * group);
	void PostInit();
	void UpdateLights(long lit);

	long Num() const;
	Light & operator [] (long i);

//--------------------------------------------------------------------------------------------
//Инкапсуляция
//--------------------------------------------------------------------------------------------
private:
	void SetDefLightParam(long i);

private:
	Light * light;
	long numLights;
	long maxLights;
};

inline long Lights::Num() const
{
	return numLights;
}

inline Light & Lights::operator [] (long i)
{
	Assert(i >= 0 && i < numLights);
	return light[i];
}



#endif

