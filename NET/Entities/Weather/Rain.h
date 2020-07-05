#ifndef WEATHER_RAIN_HPP
#define WEATHER_RAIN_HPP

#include "..\..\common.h"

#define D3DRAINVERTEX_FORMAT		(D3DFVF_XYZ|D3DFVF_DIFFUSE)

typedef struct
{
	CVECTOR		vPos;
	DWORD		dwColor;
} RAINVERTEX;

typedef struct
{
	float		fWindFlaw,fWindSpeedJitter;
	DWORD		dwTime;
	CVECTOR		vPos;
	CVECTOR		vAng;
} rainblock_t;

class NetRain : public ENTITY
{
private:
	ATTRIBUTES * pAWeather;
	dword		dwRainMaxBlend, dwRainColor, dwRainR, dwRainG, dwRainB;
	float		fDropLength, fRainWindSpeed, fRainSpeed, fWindPower, fWindAngle,
				fRainJitter, fRainWindSpeedJitter;
	
	bool		bRainbowEnable;
	long		iRainbowTex;
	string		sRainbowTexture;

	dword		dwRainTimeBlend;
	dword		dwNumRainBlocks;
	rainblock_t	*pRainBlocks;

	CVECTOR		vCamPos,vCamAng;

	float		fRainHeight,fRainRadius;
	dword		dwNumDrops;
	VDX8RENDER	* pRS;

	long		iVertexBuffer;

	void	GenerateRandomDrop(CVECTOR *vPos);
	void	GenerateRain();
	void	InitialSomeBlockParameters(long iIdx);
	void	Release();

public:
	NetRain();
	~NetRain();

	void	SetDevice();
	bool	Init();
	void	Realize(dword Delta_Time);
	void	Execute(dword Delta_Time);
	bool	CreateState(ENTITY_STATE_GEN * state_gen);
	bool	LoadState(ENTITY_STATE * state);
	void	ProcessMessage(dword iMsg,dword wParam,dword lParam);
	dword	AttributeChanged(ATTRIBUTES * pAttribute);
};

#endif