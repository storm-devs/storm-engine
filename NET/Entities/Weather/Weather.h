#ifndef WEATHER_HPP
#define WEATHER_HPP

#include <stdio.h>
#include "Sky.h"
#include "Rain.h"
#include "SunGlow.h"
#include "Lightning.h"

class NetWeather : public ENTITY
{
private:
	float fFogStart, fFogDensity;
	float fSunX, fSunY, fSunZ;
	dword dwSunColor, dwSunAmbient;
	dword dwFogColor;
	bool bFogEnable;
	VDX8RENDER * pRS;

	void	SetCommonStates();
	void	CleanUP();

public:
	NetWeather();
	virtual ~NetWeather();

	void SetDevice();
	bool Init();
	void Move();
	void Execute(dword Delta_Time);
	void Realize(dword Delta_Time);
	dword AttributeChanged(ATTRIBUTES *pAttribute);
};

#endif