#include "FirePlace.h"
#include "..\..\..\engine\program\sound.h"

ENTITY_ID	FirePlace::eidSound;

FirePlace::FirePlace()
{
	SetShip(null);
	SetActive(false);
	pParticleSmoke = null;
	pParticleFire = null;
	iSoundID = 0;
}

FirePlace::~FirePlace()
{
	Stop();
}

bool FirePlace::CreateParticle(const char * pParticleSmokeName, const char * pParticleFireName)
{
	ENTITY_ID eidParticle;
	if (api->FindClass(&eidParticle, "particles", 0))
	{
		CVECTOR vPos = GetPos();
		pParticleSmoke = (VPARTICLE_SYSTEM*)api->Send_Message(eidParticle, "lsffffffl", PS_CREATE_RIC, pParticleSmokeName, vPos.x, vPos.y, vPos.z, 0.0f, 1.0f, 0.0f, 0);
		pParticleFire = (VPARTICLE_SYSTEM*)api->Send_Message(eidParticle, "lsffffffl", PS_CREATE_RIC, pParticleFireName, vPos.x, vPos.y, vPos.z, 0.0f, 1.0f, 0.0f, 0);
		return true;
	}
	return false;
}

void FirePlace::DeleteParticle()
{
	ENTITY_ID eidParticle;
	if (api->FindClass(&eidParticle, "particles", 0))
	{
		if (pParticleSmoke) pParticleSmoke->Pause(true);//>StopEmitter();
		if (pParticleFire)	pParticleFire->Pause(true);
	}
	/*if (pParticleSmoke) pParticleSmoke->StopEmitter();
	if (pParticleFire)	pParticleFire->StopEmitter();
	DELETE(pParticleSmoke);
	DELETE(pParticleFire);*/
	pParticleSmoke = 0;
	pParticleFire = 0;
}

void FirePlace::Run(const char * pParticleSmokeName, const char * pParticleFireName, const char * pSoundName, float _fRunTime)
{
	if (isActive()) return;
	if (!CreateParticle(pParticleSmokeName, pParticleFireName)) return;
	CVECTOR vPos = GetPos();
	fRunTime = _fRunTime;
	iSoundID = api->Send_Message(eidSound, "lsllllllfff", MSG_SOUND_PLAY, pSoundName, SOUND_WAV_3D, VOLUME_FX, false, true, false, 0, vPos.x, vPos.y, vPos.z);
	SetActive(true);
	sParticleSmokeName = pParticleSmokeName;
	sParticleFireName = pParticleFireName;
	sSoundName = pSoundName;
}

void FirePlace::StopSound(long _iSoundID)
{
	if (_iSoundID != 0) api->Send_Message(eidSound, "lll", MSG_SOUND_STOP, _iSoundID, 0);
}

void FirePlace::Stop()
{
	if (!isActive()) return;
	DeleteParticle();
	StopSound(iSoundID);
	iSoundID = 0;

	SetActive(false);
}

void FirePlace::Execute(float fDeltaTime)
{
	if (!isActive()) return;

	CVECTOR vCurPos = GetPos();
	
	fRunTime -= fDeltaTime;
	if (fRunTime < 0)// || pSea->WaveXZ(vCurPos.x, vCurPos.z) > vCurPos.y)
	{
		Stop();
		return;
	}
	if (pParticleSmoke) pParticleSmoke->SetEmitter(vCurPos, CVECTOR(0.0f, 1.0f, 0.0f));
	if (pParticleFire)	pParticleFire->SetEmitter(vCurPos, CVECTOR(0.0f, 1.0f, 0.0f));
	api->Send_Message(eidSound, "lllfff", MSG_SOUND_SET_3D_PARAM, iSoundID, SOUND_PARAM_POSITION, vCurPos.x, vCurPos.y, vCurPos.z);
}

void FirePlace::Init(SEA_BASE * _pSea, SHIP_BASE * _pShip, GEOS::LABEL & label)
{
	SetShip(_pShip);
	pSea = _pSea;

	CMatrix m;
	memcpy(&m[0][0], &label.m[0][0], sizeof(m));

	SetPos(m.Pos());
}
