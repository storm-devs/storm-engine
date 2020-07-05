#ifndef _SKY_H_
#define _SKY_H_

#include "..\..\common.h"

class NetSky : public ENTITY
{
public:
	 NetSky();
	~NetSky();

	bool	Init();
	void	SetDevice();
	void	Realize(dword Delta_Time);
	bool	CreateState(ENTITY_STATE_GEN * state_gen);
	bool	LoadState(ENTITY_STATE * state);
	dword	AttributeChanged(ATTRIBUTES * pAttribute);
	dword _cdecl ProcessMessage(MESSAGE & message);

private:
	enum 
	{
		FOGVERTEX_FORMAT	= D3DFVF_XYZ | D3DFVF_DIFFUSE,
		SKYVERTEX_FORMAT	= D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1 | D3DFVF_TEXTUREFORMAT2,
		SKY_NUM_TEXTURES	= 5,
		SKY_NUM_VERTEX		= 40,

		ENUMSKY_FORCE_DWORD	= 0x7FFFFFFF
	};

	struct SKYVERTEX
	{
		CVECTOR		pos;
		dword		diffuse;
		float		tu, tv;
	};

	struct FOGVERTEX
	{
		CVECTOR		pos;
		dword		diffuse;
	};

	string		sSkyDir;
	dword		dwSkyColor;
	float		fSkySpeedRotate, fSkySize, fAngleY, fSkyAngle;

	VDX8RENDER	* pRS;
	long		TexturesID[SKY_NUM_TEXTURES];

	long		iSkyVertsID, iSkyIndexID;
	long		iFogVertsID, iFogIndexID, iFogNumVerts, iFogNumTrgs;

	// sky section
	void	GenerateSky();
	void	LoadTextures();
	void	Release();
	
	// fog section
	void	CreateFogSphere();
	dword	CalcFogDiffuse(CVECTOR & vPos);
};

#endif