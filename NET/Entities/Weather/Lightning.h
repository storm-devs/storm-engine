#ifndef LIGHTNING_HPP
#define LIGHTNING_HPP

#include "..\..\common.h"

#define MAX_LIGHTNING_TEXTURES		2

class NetLightning : public ENTITY
{
	struct flash_t
	{
		char	sTechnique[128];
		float	fSize;
		float	fTime;
	};
	
	struct lightning_t
	{
		// script parameters
		dword		dwSubTexture;
		dword		dwFlickerTime;
		float		fTime;
		float		fSize, fScaleX, fScaleY;
		CVECTOR		vPos;
		
		// inner parameters
		float		fAlpha;
		float		fPower;

		char		sTechnique[128];
		
		flash_t		Flash;
	};

	array<lightning_t>	aLightnings;

	dword			dwSubTexX, dwSubTexY;
	long			iLightningTexture, iFlashTexture;

	float			fKDist;
	long			iFlickerTime;

	VDX8RENDER		* pRS;
	COLLIDE			* pCollide;
	VIDWALKER		* pVWSunTrace;

	void			Release();
	void			CalcFlashPower(lightning_t * pL);

public:
	NetLightning();
	~NetLightning();

	void	SetDevice();
	bool	Init();
	void	Realize(dword Delta_Time);
	void	Execute(dword Delta_Time);
	bool	CreateState(ENTITY_STATE_GEN * state_gen);
	bool	LoadState(ENTITY_STATE * state);
	dword	_cdecl ProcessMessage(MESSAGE & message);
	dword	AttributeChanged(ATTRIBUTES * pAttributeChanged);
};

#endif