#ifndef NETSHIPLIGHTS_HPP
#define NETSHIPLIGHTS_HPP

#include "..\..\common.h"
#include "..\..\..\common_h\ShipLights.h"

class NetShipLights : public IShipLights
{
private:
	struct Color
	{
		float	r, g, b, a;

		friend Color operator * (const Color & c, float fValue)
		{
			Color cc;
			
			cc.r = c.r * fValue;
			cc.g = c.g * fValue;
			cc.b = c.b * fValue;
			cc.a = c.a;

			return cc;
		}

		void Normalize()
		{
			r = Clamp(r);
			g = Clamp(g);
			b = Clamp(b);
			a = Clamp(a);
		}

		Color & operator *= (float fValue)
		{
			r *= fValue;
			g *= fValue;
			b *= fValue;

			return *this;
		}
	};

	struct LightType
	{
		string	sLightType;
		Color	cLightColor;
		Color	cCoronaColor;
		float	fRange;
		float	fAttenuation0, fAttenuation1, fAttenuation2;
		float	fFlicker, fFreq, fFlickerSlow, fFreqSlow;
		float	fLifeTime, fUpTime;
		float	fCoronaRange, fCoronaSize;
		float	fSunRoadFlareFadeDistance;
	};

	struct Oscillator
	{
		float fStep;		
		float fAmp;			
		float fOneDivAmp;	
		float fNewValue;
		float fOldValue;
		float fK;
	};

	struct ShipLight
	{
		VAI_OBJBASE	* pObject;
		NODE		* pNode;
		bool		bDynamicLight;
		CVECTOR		vPos, vCurPos;
		D3DLIGHT8	Light;
		float		fCoronaIntensity;
		float		fCurDistance;
		float		fCurTime, fTotalTime, fUpTime;
		float		fFlareAlpha, fFlareAlphaMax;
		Oscillator	Osc[2];
		bool		bCoronaOnly;
		bool		bVisible;
		float		fTotalBrokenTime, fBrokenTime;
		bool		bOff, bBrokenTimeOff;
		bool		bDead;
		LightType	* pLT;
	};

	struct SelectedLight
	{
		float	fDistance;
		dword	dwIndex;

		bool operator < (const SelectedLight & other) const { return fDistance < other.fDistance; };
	};

	array<ShipLight>		aLights;
	array<SelectedLight>	aSelectedLights;
	array<LightType>		aLightTypes;
	long					iMinLight, iMaxLight;
	dword					dwMaxD3DLights;
	bool					bLoadLights;
	bool					bReflection;
	float					fSunRoadFlareSize;

	long					iCoronaTex, iFlareSunRoadTex;
	string					sCoronaTechnique;
	dword					dwCoronaSubTexX, dwCoronaSubTexY;
	SEA_BASE				* pSea;

	bool LoadLights();
	LightType * FindLightType(string sLightType);
	float GetAttributeAsFloat(ATTRIBUTES * pA, const char * pName, float fDefault);
	void AddFlare(VAI_OBJBASE * pObject, bool bLight, MODEL * pModel, const GEOS::LABEL & label);
	bool SetLabel(ShipLight * pL, MODEL * pModel, const char * pStr);

public:
	static VDX8RENDER * pRS;
	static COLLIDE * pCollide;

	NetShipLights();
	virtual ~NetShipLights();

	virtual void Release(VAI_OBJBASE * pObject);

	virtual void AddLights(VAI_OBJBASE * pObject, MODEL * pModel, bool bLights, bool bFlares);
	virtual void SetLightsOff(VAI_OBJBASE * pObject, float fTime, bool bLights, bool bFlares, bool bNow);
	virtual void KillMast(VAI_OBJBASE * pObject, NODE * pNode, bool bNow);
	virtual void AddDynamicLights(VAI_OBJBASE * pObject, const CVECTOR & vPos);
	virtual void SetLights(VAI_OBJBASE * pObject);
	virtual void UnSetLights(VAI_OBJBASE * pObject);

	virtual void SetDead(VAI_OBJBASE * pObject);

	bool Init();
	void Execute(dword dwDeltaTime);
	void Realize(dword dwDeltaTime);
	dword _cdecl ProcessMessage(MESSAGE & message);
};

#endif