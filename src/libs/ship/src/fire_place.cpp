#include "fire_place.h"
#include "entity.h"
#include "save_load.h"
#include "core.h"
#include "shared/messages.h"
#include "shared/sound.h"

entid_t FirePlace::eidSound;

FirePlace::FirePlace()
{
    SetShip(nullptr);
    SetActive(false);
    pParticleSmoke = nullptr;
    pParticleFire = nullptr;
    iBallCharacterIndex = -1;
    iSoundID = 0;
}

FirePlace::~FirePlace()
{
    Stop();
}

bool FirePlace::CreateParticle(const char *pParticleSmokeName, const char *pParticleFireName)
{
    if (const auto eidParticle = core.GetEntityId("particles"))
    {
        const auto vPos = GetPos();
        pParticleSmoke = (VPARTICLE_SYSTEM *)core.Send_Message(
            eidParticle, "lsffffffl", PS_CREATE_RIC, pParticleSmokeName, vPos.x, vPos.y, vPos.z, 0.0f, 1.0f, 0.0f, 0);
        pParticleFire = (VPARTICLE_SYSTEM *)core.Send_Message(
            eidParticle, "lsffffffl", PS_CREATE_RIC, pParticleFireName, vPos.x, vPos.y, vPos.z, 0.0f, 1.0f, 0.0f, 0);
        return true;
    }
    return false;
}

void FirePlace::DeleteParticle()
{
    if (const auto eidParticle = core.GetEntityId("particles"))
    {
        if (pParticleSmoke && core.Send_Message(eidParticle, "lp", PS_VALIDATE_PARTICLE, pParticleSmoke))
        {
            pParticleSmoke->Pause(true); //>StopEmitter();
        }
        if (pParticleFire && core.Send_Message(eidParticle, "lp", PS_VALIDATE_PARTICLE, pParticleFire))
        {
            pParticleFire->Pause(true);
        }
        /*if(pParticleSmoke)
        {
          pParticleSmoke->Pause(true);//>StopEmitter();
        }
        if(pParticleFire)
        {
          pParticleFire->Pause(true);
        }*/
    }

    // STORM_DELETE(pParticleSmoke);
    // STORM_DELETE(pParticleFire);
    pParticleSmoke = nullptr;
    pParticleFire = nullptr;
}

void FirePlace::Run(const char *pParticleSmokeName, const char *pParticleFireName, int32_t _iBallCharacterIndex,
                    const char *pSoundName, float _fRunTime)
{
    if (isActive())
        return;
    if (!CreateParticle(pParticleSmokeName, pParticleFireName))
        return;
    const auto vPos = GetPos();
    fRunTime = _fRunTime;
    iSoundID = core.Send_Message(eidSound, "lsllllllfff", MSG_SOUND_PLAY, pSoundName, SOUND_WAV_3D, VOLUME_FX, false,
                                 true, false, 0, vPos.x, vPos.y, vPos.z);
    SetActive(true);
    sParticleSmokeName = pParticleSmokeName;
    sParticleFireName = pParticleFireName;
    sSoundName = pSoundName;
    iBallCharacterIndex = _iBallCharacterIndex;
}

void FirePlace::StopSound(int32_t _iSoundID)
{
    if (_iSoundID != 0)
        core.Send_Message(eidSound, "lll", MSG_SOUND_STOP, _iSoundID, 0);
}

void FirePlace::Stop()
{
    if (!isActive())
        return;
    DeleteParticle();
    StopSound(iSoundID);
    iSoundID = 0;
    iBallCharacterIndex = -1;

    SetActive(false);
}

void FirePlace::Execute(float fDeltaTime)
{
    if (!isActive())
        return;

    const auto vCurPos = GetPos();

    fRunTime -= fDeltaTime;
    if (fRunTime < 0) // || pSea->WaveXZ(vCurPos.x, vCurPos.z) > vCurPos.y)
    {
        Stop();
        return;
    }
    if (pParticleSmoke)
        pParticleSmoke->SetEmitter(vCurPos, CVECTOR(0.0f, 1.0f, 0.0f));
    if (pParticleFire)
        pParticleFire->SetEmitter(vCurPos, CVECTOR(0.0f, 1.0f, 0.0f));
    core.Send_Message(eidSound, "lllfff", MSG_SOUND_SET_3D_PARAM, iSoundID, SOUND_PARAM_POSITION, vCurPos.x, vCurPos.y,
                      vCurPos.z);
}

void FirePlace::Init(SEA_BASE *_pSea, SHIP_BASE *_pShip, GEOS::LABEL &label)
{
    SetShip(_pShip);
    pSea = _pSea;

    CMatrix m;
    memcpy(&m.m[0][0], &label.m[0][0], sizeof(m));

    SetPos(m.Pos());
}

void FirePlace::Save(CSaveLoad *pSL)
{
    pSL->SaveVector(vOrigPos);
    pSL->SaveDword(bActive);
    pSL->SaveFloat(fRunTime);
    pSL->SaveLong(iBallCharacterIndex);
    pSL->SaveString(sParticleSmokeName);
    pSL->SaveString(sParticleFireName);
    pSL->SaveString(sSoundName);
}

void FirePlace::Load(CSaveLoad *pSL)
{
    vOrigPos = pSL->LoadVector();
    bActive = pSL->LoadDword() != 0;
    fRunTime = pSL->LoadFloat();
    iBallCharacterIndex = pSL->LoadLong();
    sParticleSmokeName = pSL->LoadString();
    sParticleFireName = pSL->LoadString();
    sSoundName = pSL->LoadString();

    if (bActive)
    {
        bActive = false;
        Run(sParticleSmokeName.c_str(), sParticleFireName.c_str(), iBallCharacterIndex, sSoundName.c_str(), fRunTime);
    }
}
