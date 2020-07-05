#ifndef ASTRONOMY_HPP
#define ASTRONOMY_HPP

#include "typedef.h"

class Astronomy : public ENTITY
{
public:
	static VDX8RENDER * pRS;
	static VGEOMETRY * pGS;

	Astronomy();
	~Astronomy();

	bool	Init();
	void	SetDevice();
	void	Realize(dword Delta_Time);
	dword	AttributeChanged(ATTRIBUTES * pAttribute);
	ATTRIBUTES * GetRoot(ATTRIBUTES * pA);

private:
	class PLANETS
	{
	public:
		struct Planet
		{
			GEOS	* pGeo;
			CVECTOR vPos;
			long	iTexture;
			float	fDistance, fRealDistance;
			float	fSpeed;
			float	fInclination;
			float	fDiameter;
			float	fScale, fFakeScale, fAngle;
			float	fMagMax, fMagMin;
		};

		array<Planet>	aPlanets;
		float			fPlanetScale;

		float fPlanetFade;
		float fFadeTimeStart;
		float fFadeTime;

		PLANETS();
		~PLANETS();

		void ReleasePlanets();

		void Init(ATTRIBUTES * pAP);
		void Execute(double dDeltaTime, double dHour);
		void Realize(double dDeltaTime, double dHour);
		void TimeUpdate(ATTRIBUTES * pAP);
	};

	class STARS
	{
	public:
		STARS();
		~STARS();

		void Init(ATTRIBUTES * pAP);
		void Execute(double dDeltaTime, double dHour);
		void Realize(double dDeltaTime, double dHour);

		dword AttributeChanged(ATTRIBUTES * pAttribute);
		void TimeUpdate(ATTRIBUTES * pAP);

		inline bool IsEnable() { return bEnable; };

	private:
		string sCatalog, sTexture;
		float fRadius, fSize, fHeightFade, fSunFade;
		float fVisualMagnitude, fTelescopeMagnitude;
		long iTexture;
		bool bEnable;
		long iVertexBuffer, iVertexBufferColors;
		dword dwShader;
		float fPrevFov;

		struct Star
		{
			float fRA;
			float fDec;
			float fMag;
			char cSpectr[2];
			dword dwColor;
			float fAlpha;
			CVECTOR vPos;
		};

		dword Spectr[256];
		array<Star> aStars;

		float fFadeValue;
		float fFadeTimeStart;
		float fFadeTime;
		float m_fTwinklingTime;
	};

	double dHour, dTimeScale;
	STARS Stars;
	PLANETS Planets;

};

#endif
