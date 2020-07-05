#ifndef SUN_GLOW_HPP
#define SUN_GLOW_HPP

#include "Typedef.h"
#include "..\common_h\Weather_base.h"

class SKY;

class SUNGLOW : public ENTITY 
{
public:

	SUNGLOW();
	~SUNGLOW();

	void	SetDevice();
	bool	Init();
	void	Realize(dword Delta_Time);
	void	Execute(dword Delta_Time);
	dword	AttributeChanged(ATTRIBUTES * pAttribute);
	dword	_cdecl ProcessMessage(MESSAGE & message);

	void	DrawSunMoon();

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
		float				fSunSize;
		float				fMoonSize;
		float				fGlowSize;
		float				fDist;
		float				fDecayTime;
		float				fRotateSpeed;
		dword				dwColor;
		string				sSunTexture,sMoonTexture,sGlowTexture, sTechniqueZ, sTechniqueNoZ;
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
		string			sTexture, sTechnique;
		float			fSize;
		float			fStart;
		dword			dwColor;
	};

	struct reflection_t
	{
		string			sTexture, sTechnique;
		float			fSize;
		float			fDist;
		dword			dwColor;
	};

	overflow_t		Overflow;
	sunglow_t		Glow;
	sunflares_t		Flares;
	reflection_t	Reflection;
	COLLIDE			* pCollide;
	WEATHER_BASE	* pWeather;
	SKY				* pSky;
	VDX8RENDER		* pRS;
	VIDWALKER		* pVWSunTrace, * pVWSailsTrace;
	long			idRectBuf;

	array<RS_RECT>	aRSR;

	bool			bSimpleSea;
	long			iNumFlares;
	float			fFadeout, fAlpha, fAlphaFlare, fAlphaOverflow, fMaxOverflowAlphaValue;
	long			iSunTex,iMoonTex,iSunGlowTex, iFlareTex, iOverflowTex, iReflTexture;
	bool			bVisible, bVisibleFlare;
	bool			bHaveGlow, bHaveFlare, bHaveOverflow, bHaveReflection;
	bool			bMoon;
	float			fMinAlphaValue;

	float			fBottomClip;

	float	LayerTrace(CVECTOR & vSrc, VIDWALKER * pVW);
	void	GenerateSunGlow();
	void	DrawReflection();
	void	Release();

	void	DrawRect(dword dwColor,const CVECTOR& pos,float fSize,float fAngle,const char* pcTechnique, float fBottomClip);

	float	GetSunFadeoutFactor(const CVECTOR& vSunPos,float fSunSize);
};

#endif