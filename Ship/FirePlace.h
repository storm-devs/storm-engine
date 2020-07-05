#ifndef _SHIP_FIRE_PLACE_HPP
#define _SHIP_FIRE_PLACE_HPP

#include "..\common_h\ship_base.h"
#include "..\common_h\sea_base.h"
#include "..\common_h\collide.h"
#include "..\common_h\dx8render.h"
#include "..\common_h\model.h"
#include "..\common_h\vparticle_system.h"

#include "..\common_h\templates\string.h"
#include "..\common_h\templates\array.h"

#include "..\common_h\sd2_h\SaveLoad.h"

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
	void		Run(const char * pParticleSmokeName, const char * pParticleFireName, long _iBallCharacterIndex, const char * pSoundName, float _fRunTime);
	void		Stop();
	void		StopSound(long _iSoundID);
	bool		isActive() { return bActive; }
	float		GetRunTime() { return fRunTime; }
	long		GetBallCharacterIndex() { return iBallCharacterIndex; }

	void		Init(SEA_BASE * _pSea, SHIP_BASE * _pShip, GEOS::LABEL & label);

	void Save(CSaveLoad * pSL);
	void Load(CSaveLoad * pSL);	

	static ENTITY_ID	eidSound;

private:
	
	bool		CreateParticle(const char * pParticleSmokeName, const char * pParticleFireName);
	void		DeleteParticle();

	void		SetActive(bool _bActive) { bActive = _bActive; }

	VPARTICLE_SYSTEM	* pParticleSmoke, * pParticleFire;
	SHIP_BASE			* pShip;
	SEA_BASE			* pSea;
	long				iSoundID;
	long				iBallCharacterIndex;
	CVECTOR				vOrigPos;
	bool				bActive;
	float				fRunTime;
	string				sParticleSmokeName, sParticleFireName, sSoundName;
};

#endif