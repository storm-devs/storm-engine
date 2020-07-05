#ifndef WATERFLARE_HPP
#define WATERFLARE_HPP

#include "Typedef.h"
#include "..\common_h\Weather_base.h"

class WATERFLARE : public ENTITY
{
	RS_RECT			*pRSRect;
	float			*pfAlpha;
	long			iFlareTex,iFlaresNum;
	WEATHER_BASE	*pWeather;
	VDX8RENDER		*RS;


	void	GenerateFlares();

public:
	WATERFLARE();
	~WATERFLARE();

	void SetDevice();
	bool Init();
	void Realize(dword Delta_Time);
	void Execute(dword Delta_Time);
	bool CreateState(ENTITY_STATE_GEN * state_gen);
	bool LoadState(ENTITY_STATE * state);
	void ProcessMessage(dword iMsg,dword wParam,dword lParam);
};

#endif