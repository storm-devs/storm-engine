#ifndef _SKY_H_
#define _SKY_H_

#include "typedef.h"

class SKY : public ENTITY
{
public:
	 SKY();
	~SKY();

	bool	Init();
	void	SetDevice();
	void	Realize(dword Delta_Time);
	bool	CreateState(ENTITY_STATE_GEN * state_gen);
	bool	LoadState(ENTITY_STATE * state);
	dword	AttributeChanged(ATTRIBUTES * pAttribute);
	dword _cdecl ProcessMessage(MESSAGE & message);

	float	CalculateAlphaForSun(const CVECTOR& vSunPos, float fSunSize);

private:
	enum 
	{
		FOGVERTEX_FORMAT	= D3DFVF_XYZ | D3DFVF_DIFFUSE,
		SKYVERTEX_FORMAT	= D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX2 | D3DFVF_TEXTUREFORMAT2,
		SKY_NUM_TEXTURES	= 5,
		SKY_NUM_VERTEX		= 40,

		ENUMSKY_FORCE_DWORD	= 0x7FFFFFFF
	};

	struct SKYVERTEX
	{
		CVECTOR		pos;
		dword		diffuse;
		float		tu, tv;
		float		tu2, tv2;
	};

	struct FOGVERTEX
	{
		CVECTOR		pos;
		dword		diffuse;
	};

	//string		sSkyDir;
	array<string>	aSkyDirArray;
	dword		dwSkyColor;
	float		fSkySpeedRotate, fSkySize, fAngleY, fSkyAngle;

	VDX8RENDER	* pRS;
	long		TexturesID[SKY_NUM_TEXTURES];
	long		TexturesNextID[SKY_NUM_TEXTURES];
	float		fTimeFactor;

	long		iSkyVertsID, iSkyIndexID;
	long		iFogVertsID, iFogIndexID, iFogNumVerts, iFogNumTrgs;

	ENTITY*		pAstronomy;
	ENTITY*		pSunGlow;

	// sky section
	void	GenerateSky();
	void	LoadTextures();
	void	Release();
	
	// fog section
	void	CreateFogSphere();
	void	UpdateFogSphere();
	dword	CalcFogDiffuse(CVECTOR & vPos);

	void FillSkyDirArray(ATTRIBUTES * pAttribute);
	void GetSkyDirStrings(string & sSkyDir, string & sSkyDirNext);
	void UpdateTimeFactor();

	DWORD GetPixelColor(IDirect3DTexture8* pTex, float fu, float fv);
};

#endif