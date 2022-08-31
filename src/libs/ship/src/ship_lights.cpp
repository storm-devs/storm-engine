#include "ship_lights.h"

#include <algorithm>

#include "math3d.h"
#include "shared/messages.h"
#include "ship.h"

VDX9RENDER *ShipLights::pRS = nullptr;
COLLIDE *ShipLights::pCollide = nullptr;

ShipLights::ShipLights()
    : fSunRoadFlareSize(0), dwCoronaSubTexX(0), dwCoronaSubTexY(0)
{
    bLoadLights = false;
    bReflection = false;

    dwMaxD3DLights = 6;

    iFlareSunRoadTex = -1;
    iCoronaTex = -1;

    iMinLight = 1000;
    iMaxLight = -1;

    pSea = nullptr;
}

ShipLights::~ShipLights()
{
    if (iCoronaTex >= 0)
        pRS->TextureRelease(iCoronaTex);
    if (iFlareSunRoadTex >= 0)
        pRS->TextureRelease(iFlareSunRoadTex);

    aLights.clear();
    aSelectedLights.clear();
    aLightTypes.clear();

    bLoadLights = false;
}

bool ShipLights::Init()
{
    pRS = static_cast<VDX9RENDER *>(core.GetService("dx9render"));
    Assert(pRS);
    pCollide = static_cast<COLLIDE *>(core.GetService("coll"));
    Assert(pCollide);
    pSea = static_cast<SEA_BASE *>(core.GetEntityPointer(core.GetEntityId("sea")));
    return true;
}

void ShipLights::Release(VAI_OBJBASE *pObject)
{
}

float ShipLights::GetAttributeAsFloat(ATTRIBUTES *pA, const char *pName, float fDefault)
{
    if (!pName)
        return fDefault;
    auto *pAF = pA->FindAClass(pA, (char *)pName);
    return pAF ? pAF->GetAttributeAsFloat() : fDefault;
}

bool ShipLights::LoadLights()
{
    aLightTypes.clear();

    bLoadLights = false;

    auto *pA = this->AttributesPointer;
    auto *pALights = pA->FindAClass(pA, "LightTypes");
    if (!pALights)
        return false;

    for (uint32_t i = 0; i < pALights->GetAttributesNum(); i++)
    {
        auto *const pAL = pALights->GetAttributeClass(i);

        LightType lightType;
        lightType.sLightType = pAL->GetThisName();
        lightType.cLightColor.r = GetAttributeAsFloat(pAL, "light.r", 1.0f);
        lightType.cLightColor.g = GetAttributeAsFloat(pAL, "light.g", 1.0f);
        lightType.cLightColor.b = GetAttributeAsFloat(pAL, "light.b", 1.0f);
        lightType.cLightColor.a = 1.0f;
        lightType.cCoronaColor.r = GetAttributeAsFloat(pAL, "corona.r", 1.0f);
        lightType.cCoronaColor.g = GetAttributeAsFloat(pAL, "corona.g", 1.0f);
        lightType.cCoronaColor.b = GetAttributeAsFloat(pAL, "corona.b", 1.0f);
        lightType.cCoronaColor.a = 1.0f;
        lightType.fCoronaRange = GetAttributeAsFloat(pAL, "corona.Range", 20.0f);
        lightType.fCoronaSize = GetAttributeAsFloat(pAL, "corona.Size", 1.0f);
        lightType.fRange = GetAttributeAsFloat(pAL, "light.range", 10.0f);
        lightType.fAttenuation0 = GetAttributeAsFloat(pAL, "light.Attenuation0", 1.0f);
        lightType.fAttenuation1 = GetAttributeAsFloat(pAL, "light.Attenuation1", 0.0f);
        lightType.fAttenuation2 = GetAttributeAsFloat(pAL, "light.Attenuation2", 0.0f);
        lightType.fFlicker = GetAttributeAsFloat(pAL, "Oscillator1.Flicker", 0.1f);
        lightType.fFreq = GetAttributeAsFloat(pAL, "Oscillator1.Freq", 15.0f);
        lightType.fFlickerSlow = GetAttributeAsFloat(pAL, "Oscillator2.Flicker", 0.5f);
        lightType.fFreqSlow = GetAttributeAsFloat(pAL, "Oscillator2.Freq", 1.0f);
        lightType.fLifeTime = GetAttributeAsFloat(pAL, "LifeTime", 0.5f);
        lightType.fUpTime = GetAttributeAsFloat(pAL, "UpTime", 0.2f);
        lightType.fSunRoadFlareFadeDistance = GetAttributeAsFloat(pAL, "SunRoadFlareFadeDistance", 100.0f);
        aLightTypes.push_back(lightType);
    }

    dwMaxD3DLights = Min(static_cast<uint32_t>(7), pA->GetAttributeAsDword("MaxD3DLights", 7));

    sCoronaTechnique = (pA->GetAttribute("CoronaTechnique")) ? pA->GetAttribute("CoronaTechnique") : "";
    iCoronaTex = (pA->GetAttribute("CoronaTexture")) ? pRS->TextureCreate(pA->GetAttribute("CoronaTexture")) : -1;
    iFlareSunRoadTex =
        (pA->GetAttribute("FlareSunRoadTexture")) ? pRS->TextureCreate(pA->GetAttribute("FlareSunRoadTexture")) : -1;
    dwCoronaSubTexX = pA->GetAttributeAsDword("CoronaTextureX", 1);
    dwCoronaSubTexY = pA->GetAttributeAsDword("CoronaTextureY", 1);
    fSunRoadFlareSize = pA->GetAttributeAsFloat("SunRoadFlareSize", 4.0);

    bLoadLights = true;
    return true;
}

ShipLights::LightType *ShipLights::FindLightType(std::string sLightType)
{
    for (uint32_t i = 0; i < aLightTypes.size(); i++)
        if (aLightTypes[i].sLightType == sLightType)
            return &aLightTypes[i];

    return nullptr;
}

void ShipLights::AddDynamicLights(VAI_OBJBASE *pObject, const CVECTOR &vPos)
{
    if (!bLoadLights && !LoadLights())
        return;

    uint32_t dwNum = 0;
    for (uint32_t i = 0; i < aLights.size(); i++)
        if (aLights[i].pObject == pObject && aLights[i].bDynamicLight)
            dwNum++;
    if (dwNum >= 4)
        return;

    std::string sLightType = "cannondefault";
    auto *pLT = FindLightType(sLightType);
    if (!pLT)
    {
        core.Trace("Can find ship light \"%s\"", sLightType.c_str());
        return;
    }

    auto light = ShipLight{};
    light.bDynamicLight = true;
    light.pObject = pObject;
    light.vPos = vPos;
    light.bOff = false;
    light.bLightOff = false;
    light.bBrokenTimeOff = false;
    light.fCurTime = 0.0;
    light.fTotalTime = pLT->fLifeTime;
    light.fUpTime = pLT->fUpTime;
    light.vCurPos = vPos;
    light.bCoronaOnly = false;
    light.fFlareAlpha = 0.0f;
    light.fFlareAlphaMax = 1.0f;
    light.bVisible = false;
    light.fBrokenTime = 0.0f;
    light.bDead = false;
    light.fTotalBrokenTime = 0.0f;
    light.Light = {};
    light.Light.Type = D3DLIGHT_POINT;
    light.Light.Diffuse.r = pLT->cLightColor.r;
    light.Light.Diffuse.g = pLT->cLightColor.g;
    light.Light.Diffuse.b = pLT->cLightColor.b;
    light.Light.Diffuse.a = 1.0f;
    light.Light.Range = pLT->fRange;
    light.Light.Attenuation0 = pLT->fAttenuation0;
    light.Light.Attenuation1 = pLT->fAttenuation1;
    light.Light.Attenuation2 = pLT->fAttenuation2;
    light.pLT = pLT;
    aLights.push_back(light);
}

bool ShipLights::SetLabel(ShipLight *pL, MODEL *pModel, const char *pStr)
{
    pL->pNode = pModel->FindNode(pStr);
    if (!pL->pNode)
        return false;

    CMatrix mNode = pL->pNode->glob_mtx;
    mNode.Transposition();
    pL->vPos = mNode * pL->vPos;
    return true;
}

void ShipLights::AddFlare(VAI_OBJBASE *pObject, bool bLight, MODEL *pModel, const GEOS::LABEL &label)
{
    CMatrix m;
    char str[256]{}, str2[256];
    if (!label.name)
        return;
    strcpy_s(str, label.name);
    std::ranges::for_each(str, [](char &c) { c = std::tolower(c); });

    aLights.push_back(ShipLight{});
    ShipLight *pL = &aLights.back();
    memcpy(m, label.m, sizeof(m));

    pL->pNode = nullptr;
    pL->vPos = m.Pos();

    if (str[0] != 'f' && !bLight)
        return;
    if (str[0] != 'l' && bLight)
        return;

    // mast found
    if (str[1] == 'm' || str[1] == 'r')
    {
        if (str[1] == 'm')
        {
            str2[0] = str[2];
            str2[1] = '\0';
        }
        else
        {
            str2[0] = str[2];
            str2[1] = str[3];
            str2[2] = str[4];
            str2[3] = '\0';
        }
        int iMastIndex = atoi(str2);

        sprintf_s(str2, "mast%d", iMastIndex);
        // rey found
        if (str[1] == 'm')
        {
            if (str[3] >= 'a' && str[3] <= 'z')
            {
                sprintf_s(str2, "rey_%c%d", str[3], iMastIndex);
            }
        }
        else
        {
            if (str[5] >= 'a' && str[5] <= 'z')
            {
                sprintf_s(str2, "rey_%c%d", str[5], iMastIndex);
            }
        }

        if (!SetLabel(pL, pModel, str2))
        {
            aLights.pop_back();
            return;
        }
    }

    LightType *pLT = FindLightType((bLight) ? "default" : "flare");

    if (!pLT)
        return;

    pL->bDynamicLight = false;
    pL->pObject = pObject;
    pL->bCoronaOnly = true;
    pL->fFlareAlpha = 0.0f;
    pL->fFlareAlphaMax = 1.0f;
    pL->bVisible = true;
    pL->fBrokenTime = 0.0f;
    pL->fTotalBrokenTime = 0.0f;
    pL->bOff = false;
    pL->bLightOff = true;
    pL->bBrokenTimeOff = false;
    pL->bDead = false;

    if (bLight)
    {
        pL->Light = {};
        pL->bCoronaOnly = false;
        pL->Light.Type = D3DLIGHT_POINT;
        pL->Light.Diffuse.r = pLT->cLightColor.r;
        pL->Light.Diffuse.g = pLT->cLightColor.g;
        pL->Light.Diffuse.b = pLT->cLightColor.b;
        pL->Light.Diffuse.a = 1.0f;
        pL->Light.Range = pLT->fRange;
        pL->Light.Attenuation0 = pLT->fAttenuation0;
        pL->Light.Attenuation1 = pLT->fAttenuation1;
        pL->Light.Attenuation2 = pLT->fAttenuation2;
    }

    pL->Osc[0].fStep = pLT->fFreq;
    pL->Osc[0].fAmp = pLT->fFlicker;
    pL->Osc[0].fOneDivAmp = 1.0f / pL->Osc[0].fAmp;
    pL->Osc[0].fK = 0.0f;
    pL->Osc[0].fOldValue = RRnd(-pL->Osc[0].fAmp, pL->Osc[0].fAmp);
    pL->Osc[0].fNewValue = RRnd(-pL->Osc[0].fAmp, pL->Osc[0].fAmp);

    pL->Osc[1].fStep = pLT->fFreqSlow;
    pL->Osc[1].fAmp = pLT->fFlickerSlow;
    pL->Osc[1].fOneDivAmp = 1.0f / pL->Osc[1].fAmp;
    pL->Osc[1].fK = 0.0f;
    pL->Osc[1].fOldValue = RRnd(-pL->Osc[1].fAmp, pL->Osc[1].fAmp);
    pL->Osc[1].fNewValue = RRnd(-pL->Osc[1].fAmp, pL->Osc[1].fAmp);

    pL->pLT = pLT;
}

void ShipLights::SetLightsOff(VAI_OBJBASE *pObject, float fTime, bool bLights, bool bFlares, bool bNow)
{
    for (uint32_t i = 0; i < aLights.size(); i++)
        if (aLights[i].pObject == pObject && !aLights[i].bOff)
        {
            if (aLights[i].bCoronaOnly)
            {
                if (aLights[i].bOff == bFlares ||
                    (aLights[i].fTotalBrokenTime > 0.0f && aLights[i].bBrokenTimeOff == bFlares))
                    continue;
                aLights[i].bBrokenTimeOff = bFlares;
            }
            else
            {
                if (aLights[i].bOff == bLights ||
                    (aLights[i].fTotalBrokenTime > 0.0f && aLights[i].bBrokenTimeOff == bLights))
                    continue;
                aLights[i].bBrokenTimeOff = bLights;
            }

            aLights[i].fBrokenTime = 0.0f;
            aLights[i].fTotalBrokenTime = Rnd(fTime);

            if (bNow)
            {
                aLights[i].fTotalBrokenTime = 0.0f;
                aLights[i].bOff = aLights[i].bBrokenTimeOff;
            }
        }
}

void ShipLights::KillMast(VAI_OBJBASE *pObject, NODE *pNode, bool bNow)
{
    for (uint32_t i = 0; i < aLights.size(); i++)
        if (aLights[i].pObject == pObject && aLights[i].pNode)
        {
            if (aLights[i].bOff)
                continue;
            if (aLights[i].pNode->parent != pNode && aLights[i].pNode != pNode)
                continue;
            if (bNow)
            {
                aLights[i].pNode = nullptr;
                aLights[i].bOff = true;
                aLights[i].bBrokenTimeOff = true;
            }
            else
            {
                // slow
                aLights[i].fTotalBrokenTime = 0.1f + FRAND(3.0f);
                aLights[i].fBrokenTime = 0.0f;
                aLights[i].bBrokenTimeOff = true;
            }
        }
}

void ShipLights::AddLights(VAI_OBJBASE *pObject, MODEL *pModel, bool bLights, bool bFlares)
{
    if (!bLoadLights && !LoadLights())
        return;

    const std::string sLightType = "default";

    LightType *pLT = FindLightType(sLightType);
    if (!pLT)
    {
        core.Trace("Can't find ship light \"%s\"", sLightType.c_str());
        return;
    }

    GEOS::LABEL label;
    GEOS::INFO info;
    NODE *pNode;

    NODE *pRoot = pModel->GetNode(0);

    const std::string sFlares = "flares";
    const std::string sLights = "lights";
    uint32_t dwIdx = 0;
    while (pNode = pModel->GetNode(dwIdx))
    {
        pNode->geo->GetInfo(info);

        for (uint32_t i = 0; i < static_cast<uint32_t>(info.nlabels); i++)
        {
            pNode->geo->GetLabel(i, label);

            if (sFlares == label.group_name)
            {
                AddFlare(pObject, false, pModel, label);
                continue;
            }

            if (sLights == label.group_name)	
            {
                AddFlare(pObject, true, pModel, label);
            }
        }
        dwIdx++;
    }
}

void ShipLights::SetLights(VAI_OBJBASE *pObject)
{
    if(!bLoadLights) return;

    uint32_t i;
    aSelectedLights.clear();

    for (i = 0; i < aLights.size(); i++)
    {
        if (aLights[i].bOff || aLights[i].bLightOff )
            continue;
        if (!aLights[i].bDynamicLight)
        {
            if (aLights[i].pNode)
            {
                CVECTOR vPos = aLights[i].vPos;
                aLights[i].vCurPos = aLights[i].pNode->glob_mtx * vPos;
            }
            else
                aLights[i].vCurPos = *(aLights[i].pObject->GetMatrix()) * aLights[i].vPos;

            aLights[i].fCurDistance = sqrtf(~(pObject->GetPos() - aLights[i].vCurPos));
        }
        aLights[i].Light.Position.x = aLights[i].vCurPos.x;
        aLights[i].Light.Position.y = aLights[i].vCurPos.y;
        aLights[i].Light.Position.z = aLights[i].vCurPos.z;

        if (pObject == aLights[i].pObject || aLights[i].bCoronaOnly)
            continue;

        aSelectedLights.push_back(SelectedLight{aLights[i].fCurDistance, i});
    }

    // aSelectedLights.Sort();
    std::sort(aSelectedLights.begin(), aSelectedLights.end());

    for (i = 0; i < aLights.size(); i++)
        if (!aLights[i].bOff && !aLights[i].bLightOff && aLights[i].pObject == pObject && !aLights[i].bCoronaOnly)
        {
            aSelectedLights.insert(aSelectedLights.begin(), SelectedLight{aLights[i].fCurDistance, i});
        }

    for (i = 0; i < dwMaxD3DLights; i++)
    {
        if (i >= aSelectedLights.size())
            break;

        ShipLight *pL = &aLights[aSelectedLights[i].dwIndex];

        pRS->SetLight(i + 1, &pL->Light);
        pRS->LightEnable(i + 1, true);

        iMinLight = Min(iMinLight, static_cast<int32_t>(i + 1));
        iMaxLight = Max(iMaxLight, static_cast<int32_t>(i + 1));
    }
}

// update lights/flares parameters
void ShipLights::Execute(uint32_t dwDeltaTime)
{
    const float fDeltaTime = static_cast<float>(dwDeltaTime) * 0.001f;

    float fFov;
    CVECTOR vCamPos, vCamAng;
    pRS->GetCamera(vCamPos, vCamAng, fFov);

    for (uint32_t i = 0; i < aLights.size(); i++)
    {
        ShipLight &L = aLights[i];

        if (L.bOff || L.bLightOff)
            continue;

        if (!L.bDynamicLight)
        {
            if (L.pNode)
                L.vCurPos = L.pNode->glob_mtx * L.vPos;
            else
                L.vCurPos = *(L.pObject->GetMatrix()) * L.vPos;
        }

        if (!L.bOff && L.bDead && L.fTotalBrokenTime <= 0.0f && pSea &&
            pSea->WaveXZ(L.vCurPos.x, L.vCurPos.x) > L.vCurPos.y)
        {
            L.fTotalBrokenTime = RRnd(1.0f, 4.0f);
            L.fBrokenTime = 0.0f;
            L.bBrokenTimeOff = true;
        }

        float fBroken = 1.0f;
        if (L.fTotalBrokenTime > 0.0f)
        {
            L.fBrokenTime += fDeltaTime;
            if (L.fBrokenTime >= L.fTotalBrokenTime)
            {
                L.bOff = L.bBrokenTimeOff;
                L.fTotalBrokenTime = 0.0f;
                continue;
            }

            fBroken = 1.0f - L.fBrokenTime / L.fTotalBrokenTime;
        }

        if (pCollide)
        {
            L.bVisible = true;

            float fDistance =
                pCollide->Trace(core.GetEntityIds(SAILS_TRACE), L.vCurPos, vCamPos, nullptr, 0);
            L.fFlareAlphaMax = (fDistance >= 1.0f) ? 1.0f : 0.2f;

            const auto its = core.GetEntityIds(SUN_TRACE);
            fDistance = pCollide->Trace(its, L.vCurPos, vCamPos, nullptr, 0);
            const float fLen = fDistance * sqrtf(~(vCamPos - L.vCurPos));
            L.bVisible = fDistance >= 1.0f || (fLen < 0.6f);

            if (!L.bOff && !L.bLightOff && L.bVisible)
            {
                const float fDistance = pCollide->Trace(its, vCamPos, L.vCurPos, nullptr, 0);
                const float fLen = (1.0f - fDistance) * sqrtf(~(vCamPos - L.vCurPos));

                L.bVisible = fLen < 0.6f;
            }
        }

        L.fFlareAlpha = Min(Clamp(L.fFlareAlpha + 5.0f * fDeltaTime * ((L.bVisible) ? 1.0f : -1.0f)), L.fFlareAlphaMax);

        if (!L.bDynamicLight)
        {
            float fIntensity = 0.0f;
            float fKAmp = 1.0f;
            for (uint32_t j = 0; j < 2; j++)
            {
                Oscillator &o = L.Osc[j];
                o.fK += o.fStep * fDeltaTime;
                if (o.fK >= 1.0f)
                {
                    if (o.fK < 2.0f)
                    {
                        o.fK -= 1.0f;
                        o.fOldValue = o.fNewValue;
                        o.fNewValue = RRnd(-o.fAmp, o.fAmp);
                    }
                    else
                    {
                        o.fK = 0.0f;
                        o.fOldValue = RRnd(-o.fAmp, o.fAmp);
                        o.fNewValue = RRnd(-o.fAmp, o.fAmp);
                    }
                }
                const float fIns = o.fOldValue + (o.fNewValue - o.fOldValue) * o.fK;
                fIntensity += fIns * fKAmp;
                fKAmp -= fIns * o.fOneDivAmp;
                if (fKAmp < 0.0f)
                    break;
            }

            fIntensity = 1.0f + Clampf(fIntensity, -1.0f, 1.0f);
            L.Light.Diffuse.r = L.pLT->cLightColor.r * fIntensity * fBroken;
            L.Light.Diffuse.g = L.pLT->cLightColor.g * fIntensity * fBroken;
            L.Light.Diffuse.b = L.pLT->cLightColor.b * fIntensity * fBroken;

            L.fCoronaIntensity = fBroken * (0.5f + fIntensity * 0.5f);
        }
        else
        {
            L.fCurTime += fDeltaTime;
            if (L.fCurTime >= L.fTotalTime)
            {
                aLights[i] = aLights.back();
                aLights.pop_back();
                i--;
                continue;
            }

            float fIntensity = (L.fCurTime < L.fUpTime)
                                   ? L.fCurTime / L.fUpTime
                                   : (1.0f - (L.fCurTime - L.fUpTime) / (L.fTotalTime - L.fUpTime));
            fIntensity = 0.5f + Clampf(fIntensity, -0.5f, 0.5f);
            L.Light.Diffuse.r = L.pLT->cLightColor.r * fIntensity;
            L.Light.Diffuse.g = L.pLT->cLightColor.g * fIntensity;
            L.Light.Diffuse.b = L.pLT->cLightColor.b * fIntensity;
        }
    }
}

// draw flares
void ShipLights::Realize(uint32_t dwDeltaTime)
{
    float fDeltaTime = static_cast<float>(dwDeltaTime) * 0.001f;

    float fFov;
    CVECTOR vCamPos, vCamAng;
    pRS->GetCamera(vCamPos, vCamAng, fFov);

    static std::vector<RS_RECT> aRects;
    aRects.clear();

    const float fReflSize = (bReflection) ? fSunRoadFlareSize : 1.0f;

    for (uint32_t i = 0; i < aLights.size(); i++)
    {
        ShipLight &L = aLights[i];

        if (L.bOff || L.bLightOff)
            continue;

        float fDistanceFade = 1.0f;

        if (bReflection)
        {
            const float fDistance = sqrtf(~(L.vCurPos - vCamPos));
            fDistanceFade = 1.0f - Clamp(fDistance / L.pLT->fSunRoadFlareFadeDistance);
        }

        if (L.bDynamicLight && bReflection)
        {
            RS_RECT r;
            r.vPos = L.vCurPos;
            r.fAngle = 0.0f;
            r.dwSubTexture = 0;
            r.fSize = 10.0f;
            r.dwColor = ARGB(255, 255, 255, 255);
            aRects.push_back(r);
            continue;
        }

        RS_RECT r;
        r.vPos = L.vCurPos;
        r.fSize = L.pLT->fCoronaSize * L.fCoronaIntensity * fReflSize;
        r.fAngle = 0.0f;
        r.dwSubTexture = 0;
        Color cColor = L.pLT->cCoronaColor * (L.fCoronaIntensity * fDistanceFade);
        cColor.Normalize();
        cColor = cColor * 255.0f * L.fFlareAlpha;
        cColor.a = Clamp(L.fFlareAlpha * fReflSize);
        r.dwColor = ARGB(cColor.a, cColor.r, cColor.g, cColor.b);
        aRects.push_back(r);
    }

    if (aRects.size())
    {
        pRS->TextureSet(0, (bReflection) ? iFlareSunRoadTex : iCoronaTex);
        pRS->DrawRects(&aRects[0], aRects.size(), sCoronaTechnique.c_str(), dwCoronaSubTexX, dwCoronaSubTexY);
    }
}

void ShipLights::UnSetLights(VAI_OBJBASE *pObject)
{
	if(!bLoadLights) return;
    for (int32_t i = iMinLight; i <= iMaxLight; i++)
        pRS->LightEnable(i, false);

    iMinLight = 1000;
    iMaxLight = -1;
}

void ShipLights::ResetLights(VAI_OBJBASE *pObject, bool bLight)
{
    for (auto &light : aLights)
    {
        if (!light.bOff)
        {
            light.bLightOff = !bLight;
        }
    }
}

uint64_t ShipLights::ProcessMessage(MESSAGE &message)
{
    switch (message.Long())
    {
    case MSG_SEA_SUNROAD_DRAW:
        bReflection = true;
        Realize(0);
        bReflection = false;
        break;
    }

    return 0;
}

void ShipLights::SetDead(VAI_OBJBASE *pObject)
{
    for (uint32_t i = 0; i < aLights.size(); i++)
        if (aLights[i].pObject == pObject)
            aLights[i].bDead = true;
}
