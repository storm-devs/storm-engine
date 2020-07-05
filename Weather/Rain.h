#ifndef WEATHER_RAIN_HPP
#define WEATHER_RAIN_HPP

#include "Typedef.h"
#include "..\common_h\Weather_base.h"
#include "..\common_h\sea_base.h"
#include "..\common_h\ship_base.h"

#define NUM_SEA_DROPS				500
#define D3DRAINVERTEX_FORMAT		(D3DFVF_XYZ | D3DFVF_DIFFUSE)
#define D3DSEADROPVERTEX_FORMAT		(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1)

typedef struct
{
	CVECTOR		vPos;
	DWORD		dwColor;
} RAINVERTEX;

struct SEADROPVERTEX
{
	CVECTOR		vPos;
	DWORD		dwColor;
	float		tu, tv;
};

typedef struct
{
	float		fWindFlaw,fWindSpeedJitter;
	DWORD		dwTime;
	CVECTOR		vPos;
	CVECTOR		vAng;
} rainblock_t;

class RAIN : public ENTITY
{
struct ship_t
{
	ENTITY_ID	eid;
	SHIP_BASE	* pShip;
};

struct seadrop_t
{
	CVECTOR vPos;
	float	fTime;
	float	fLifeTime;
};

struct drop_t
{
	CVECTOR vPos;
	float	fLifeTime;
	long	iShip;
	//SHIP_BASE * pShip;
};

private:
	long				iRainDropsTexture;
	float				fDropsDeltaTime;
	array<RS_RECT>		aRects;
	array<drop_t>		aDrops;
	array<seadrop_t>	aSeaDrops;
	array<ship_t>		aShips;

	bool				bShow;

	WEATHER_BASE	* pWeather;

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

	long		iVertexBuffer;

	dword		dwDropsColor;
	dword		dwDropsNearNum, dwDropsFarNum;
	float		fDropsNearRadius, fDropsFarRadius;
	float		fDropsLifeTime;
	float		fDropsSize;
	string		sDropsTexture, sSeaDropsTexture;

	long		iSeaDropTex;
	long		iIBSeaDrops;
	long		iVBSeaDrops;

	void	GenerateRandomDrop(CVECTOR *vPos);
	void	GenerateRain();
	void	InitialSomeBlockParameters(long iIdx);
	void	Release();
	void	RealizeDrops(dword Delta_Time);

public:
	RAIN();
	~RAIN();

	void	SetDevice();
	bool	Init();
	void	Realize(dword Delta_Time);
	void	Execute(dword Delta_Time);
	bool	CreateState(ENTITY_STATE_GEN * state_gen);
	bool	LoadState(ENTITY_STATE * state);
	dword _cdecl ProcessMessage(MESSAGE & message);
	dword	AttributeChanged(ATTRIBUTES * pAttribute);
};

#endif