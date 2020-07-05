#ifndef WEATHER_HPP
#define WEATHER_HPP

#include <stdio.h>
#include "Typedef.h"
#include "..\common_h\Weather_Base.h"
#include "Rain.h"
#include "SunGlow.h"
#include "Lightning.h"
#include "Sky.h"
#include "WaterFlare.h"
#include "Astronomy.h"

class WEATHER : public WEATHER_BASE
{
private:
	VDX8RENDER			* pRS;

	float				fFloats[MAX_FLOAT_PARAMS];
	long				iLongs[MAX_LONG_PARAMS];
	dword				dwColors[MAX_COLOR_PARAMS];
	CVECTOR				vVectors[MAX_VECTORS_PARAMS];
	char				*pStrings[MAX_STRINGS_PARAMS];

	// timer data
	float	fUpdateFrequence;

	// sun moving data
	bool	bSunPresent;
	float	fSunHeight;
	float	fSunBegAngle;
	float	fSunBegTime;
	float	fSunEndAngle;
	float	fSunEndTime;
	// moon moving data
	bool	bMoonPresent;
	float	fMoonHeight;
	float	fMoonBegAngle;
	float	fMoonBegTime;
	float	fMoonEndAngle;
	float	fMoonEndTime;

	void	SetBeginData();
	void	UpdateSunMoonPos();

	void	SetCommonStates();
	void	CleanUP();

	// Weather_Base function
	long	GetLong(dword dwCode);
	dword	GetColor(dword, CVECTOR*);
	dword	GetColor(dword);
	float	GetFloat(dword);
	void	GetVector(dword,CVECTOR*);

public:
	WEATHER();
	virtual ~WEATHER();

	void SetDevice();
	bool Init();
	void Move();
	void Execute(dword Delta_Time);
	void Realize(dword Delta_Time);
	bool CreateState(ENTITY_STATE_GEN * state_gen);
	bool LoadState(ENTITY_STATE * state);
	dword AttributeChanged(ATTRIBUTES *pAttribute);
};

#endif