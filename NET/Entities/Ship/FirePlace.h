#ifndef _SHIP_FIRE_PLACE_HPP
#define _SHIP_FIRE_PLACE_HPP

#include "..\..\common.h"

class FirePlace
{
public:
	FirePlace();
	~FirePlace();

	CVECTOR		GetPos() { Assert(GetShip()); return *GetShip()->GetMatrix() * vOrigPos; };
	void		SetPos(CVECTOR & vPos) { vOrigPos = vPos; };
	float		GetDistance(CVECTOR & vOtherPos) { return sqrtf(~(GetPos() - vOtherPos)); };

	void		SetShip(SHIP_BASE * _pShip) { pShip = _pShip; }
	SHIP_BASE	* GetShip() { return pShip; }

	void		Execute(float fDeltaTime);
	void		Run(const char * pParticleSmokeName, const char * pParticleFireName, const char * pSoundName, float _fRunTime);
	void		Stop();
	void		StopSound(long _iSoundID);
	bool		isActive() { return bActive; }
	float		GetRunTime() { return fRunTime; }

	void		Init(SEA_BASE * _pSea, SHIP_BASE * _pShip, GEOS::LABEL & label);

	static ENTITY_ID	eidSound;

private:
	
	bool		CreateParticle(const char * pParticleSmokeName, const char * pParticleFireName);
	void		DeleteParticle();

	void		SetActive(bool _bActive) { bActive = _bActive; }

	VPARTICLE_SYSTEM	* pParticleSmoke, * pParticleFire;
	SHIP_BASE			* pShip;
	SEA_BASE			* pSea;
	long				iSoundID;
	CVECTOR				vOrigPos;
	bool				bActive;
	float				fRunTime;
	string				sParticleSmokeName, sParticleFireName, sSoundName;
};

#endif