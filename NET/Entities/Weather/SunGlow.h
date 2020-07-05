#ifndef SUN_GLOW_HPP
#define SUN_GLOW_HPP

#include "..\..\common.h"

class NetSunGlow : public ENTITY
{
public:

	NetSunGlow();
	~NetSunGlow();

	void	SetDevice();
	bool	Init();
	void	Realize(dword Delta_Time);
	void	Execute(dword Delta_Time);
	dword	AttributeChanged(ATTRIBUTES * pAttribute);
	dword	_cdecl ProcessMessage(MESSAGE & message);

private:
	enum 
	{
		SUNGLOWVERTEX_FORMAT	= D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1,
		SUNGLOW_FORCE_DWORD		= 0x7FFFFFFF
	};

	struct SUNGLOWVERTEX 
	{
		CVECTOR vPos;
		dword	dwColor;
		float	tu,tv;
	};

	struct sunglow_t
	{
		float				fSize;
		float				fDist;
		float				fDecayTime;
		float				fRotateSpeed;
		dword				dwColor;
		string				sTexture, sTechniqueZ, sTechniqueNoZ;
	};

	struct flare_t 
	{
		float	fDist;			// distance from far
		float	fSize;			// size
		dword	dwColor;		// color
		dword	dwSubTexIndex;	// subtexture index 
	};

	struct sunflares_t
	{
		sunflares_t() : aFlares(_FL_) {}

		float				fSize;
		float				fDist;
		float				fFlareScale;
		dword				dwTexSizeX, dwTexSizeY;
		string				sTexture, sTechnique;
		array<flare_t>		aFlares;
	};

	struct overflow_t
	{
		string				sTexture, sTechnique;
		float				fSize;
		float				fStart;
		dword				dwColor;
	};

	struct reflection_t
	{
		string			sTexture, sTechnique;
		float			fSize;
		float			fDist;
		dword			dwColor;
	};

	ATTRIBUTES		* pAWeather;
	CVECTOR			vSunPos;

	bool			bSimpleSea;

	overflow_t		Overflow;
	sunglow_t		Glow;
	sunflares_t		Flares;
	reflection_t	Reflection;
	COLLIDE			* pCollide;
	VDX8RENDER		* pRS;
	VIDWALKER		* pVWSunTrace, * pVWSailsTrace;

	array<RS_RECT>	aRSR;

	long			iNumFlares;
	float			fAlpha, fAlphaFlare, fAlphaOverflow, fMaxOverflowAlphaValue;
	long			iGlowTex, iFlareTex, iOverflowTex, iReflTexture;
	bool			bVisible, bVisibleFlare;
	bool			bHaveGlow, bHaveFlare, bHaveOverflow, bHaveReflection;
	float			fMinAlphaValue;

	float	LayerTrace(CVECTOR & vSrc, VIDWALKER * pVW);
	void	GenerateSunGlow();
	void	DrawReflection();
	void	Release();

};

#endif