#include "lightning.h"

#include "core.h"

LIGHTNING::LIGHTNING()
{
    iLightningTexture = -1;
    iFlashTexture = -1;
}

LIGHTNING::~LIGHTNING()
{
    Release();
}

void LIGHTNING::Release() const
{
    pRS->TextureRelease(iLightningTexture);
    pRS->TextureRelease(iFlashTexture);
}

bool LIGHTNING::Init()
{
    SetDevice();

    return true;
}

void LIGHTNING::SetDevice()
{
    pRS = static_cast<VDX9RENDER *>(core.GetService("dx9render"));
    Assert(pRS);
    pCollide = static_cast<COLLIDE *>(core.GetService("COLL"));
    Assert(pCollide);
}

bool LIGHTNING::CreateState(ENTITY_STATE_GEN *state_gen)
{
    return true;
}

bool LIGHTNING::LoadState(ENTITY_STATE *state)
{
    return true;
}

void LIGHTNING::Execute(uint32_t Delta_Time)
{
    const auto fDeltaTime = static_cast<float>(Delta_Time) * 0.001f;

    for (uint32_t i = 0; i < aLightnings.size(); i++)
    {
        auto *pL = &aLightnings[i];
        if (pL->fTime > 0.0f)
        {
            pL->fAlpha = (static_cast<uint32_t>(pL->fTime * 1000.0f) & pL->dwFlickerTime) ? 1.0f : 0.0f;
            pL->fTime -= fDeltaTime;
            if (pL->fTime <= 0.0f)
            {
                pL->fAlpha = 1.0f;
            }
        }
        if (pL->fTime < 0.0f)
        {
            pL->fAlpha -= static_cast<float>(Delta_Time) * 0.01f;
            if (pL->fAlpha < 0.0f)
            {
                // aLightnings.ExtractNoShift(i);
                aLightnings[i] = aLightnings.back();
                aLightnings.pop_back();
                i--;
            }
        }
    }
}

void LIGHTNING::Realize(uint32_t Delta_Time)
{
    uint32_t i;
    RS_RECT rs_rect;

    if (iLightningTexture >= 0)
    {
        pRS->TextureSet(0, iLightningTexture);
        for (i = 0; i < aLightnings.size(); i++)
        {
            auto *const pL = &aLightnings[i];
            auto *pR = &rs_rect;

            const auto dwAlpha = static_cast<uint32_t>(255.0f * pL->fAlpha);
            pR->dwSubTexture = pL->dwSubTexture;
            pR->dwColor = makeRGB(dwAlpha, dwAlpha, dwAlpha);
            pR->vPos = pL->vPos;
            pR->fSize = pL->fSize;
            pR->fAngle = 0.0f;
            pRS->DrawRects(pR, 1, pL->sTechnique.c_str(), dwSubTexX, dwSubTexY, pL->fScaleX, pL->fScaleY);
        }
    }

    if (iFlashTexture >= 0)
    {
        pRS->TextureSet(0, iFlashTexture);
        for (i = 0; i < aLightnings.size(); i++)
        {
            auto *const pL = &aLightnings[i];
            auto *pR = &rs_rect;

            const auto dwAlpha = static_cast<uint32_t>(255.0f * pL->fAlpha * pL->fPower);
            pR->dwColor = makeRGB(dwAlpha, dwAlpha, dwAlpha);
            pR->vPos = pL->vPos;
            pR->fSize = pL->Flash.fSize;
            pR->fAngle = 0.0f;
            pRS->DrawRects(pR, 1, pL->Flash.sTechnique.c_str());
        }
    }
}

uint64_t LIGHTNING::ProcessMessage(MESSAGE &message)
{
    switch (message.Long())
    {
    case MSG_SEA_REFLECTION_DRAW:
        Realize(0);
        break;
    case MSG_WHR_LIGHTNING_ADD: {
        // add new lightning
        aLightnings.push_back(lightning_t{});
        // lightning_t * pL = &aLightnings[aLightnings.Add()];
        auto *pL = &aLightnings.back();

        pL->dwSubTexture = message.Long();
        pL->sTechnique = message.String();
        pL->fTime = message.Float();
        pL->dwFlickerTime = static_cast<uint32_t>(message.Long());
        pL->fSize = message.Float();
        pL->fScaleX = message.Float();
        pL->fScaleY = message.Float();
        pL->vPos.x = message.Float();
        pL->vPos.y = message.Float();
        pL->vPos.z = message.Float();

        pL->Flash.sTechnique = message.String();
        pL->Flash.fSize = message.Float();
        pL->Flash.fTime = message.Float();

        CalcFlashPower(pL);
    }
    break;
    }

    return 0;
}

void LIGHTNING::CalcFlashPower(lightning_t *pL) const
{
    CVECTOR vCamPos, vCamAng, vTrace[3];
    float fFov;

    pRS->GetCamera(vCamPos, vCamAng, fFov);

    vTrace[0] = CVECTOR(pL->vPos.x, pL->vPos.y + pL->fSize * 0.9f, pL->vPos.z);
    vTrace[1] = pL->vPos;
    vTrace[2] = CVECTOR(pL->vPos.x, pL->vPos.y - pL->fSize * 0.9f, pL->vPos.z);

    auto fPower = 1.0f;

    for (uint32_t i = 0; i < 3; i++)
    {
        const auto fRes =
            pCollide->Trace(core.GetEntityIds(SUN_TRACE), vCamPos, vTrace[i], nullptr, 0);
        if (fRes <= 1.0f)
            fPower -= 0.31f;
    }
    pL->fPower = fPower;
}

uint32_t LIGHTNING::AttributeChanged(ATTRIBUTES *pAttribute)
{
    // std::string sTextureName;

    auto *const pParent = pAttribute->GetParent();

    if (*pAttribute == "Clear")
    {
        aLightnings.clear();
        return 0;
    }

    if (*pParent == "Flash")
    {
        if (*pAttribute == "Texture")
        {
            if (iFlashTexture >= 0)
                pRS->TextureRelease(iFlashTexture);
            iFlashTexture = pRS->TextureCreate(pAttribute->GetThisAttr());
            return 0;
        }
        return 0;
    }

    if (*pAttribute == "Texture")
    {
        if (iLightningTexture >= 0)
            pRS->TextureRelease(iLightningTexture);
        iLightningTexture = pRS->TextureCreate(pAttribute->GetThisAttr());
        return 0;
    }

    if (*pAttribute == "SubTexX")
    {
        dwSubTexX = pAttribute->GetAttributeAsDword();
        return 0;
    }
    if (*pAttribute == "SubTexY")
    {
        dwSubTexY = pAttribute->GetAttributeAsDword();
        return 0;
    }
    if (*pAttribute == "isDone")
        return 0;
    return 0;
}
