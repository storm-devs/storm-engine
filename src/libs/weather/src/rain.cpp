#include "rain.h"

#include "core.h"
#include "math_inlines.h"

RAIN::RAIN()
{
    aRects.reserve(512);
    aDrops.reserve(512);
    aSeaDrops.reserve(512);
    aShips.reserve(16);

    pWeather = nullptr;
    pRainBlocks = nullptr;
    dwNumDrops = 0;
    iVertexBuffer = -1;
    iRainbowTex = -1;
    iRainDropsTexture = -1;
    bRainbowEnable = false;
    bShow = true;

    fDropsDeltaTime = 0.0f;

    dwDropsNearNum = 0;
    dwDropsFarNum = 0;

    fDropsNearRadius = 25.0f;
    fDropsFarRadius = 75.0f;

    fDropsLifeTime = 0.5f;
    fDropsSize = 0.1f;
    dwDropsColor = 0x3FFFFFFF;

    iIBSeaDrops = -1;
    iVBSeaDrops = -1;
    iSeaDropTex = -1;

    sDropsTexture = "weather\\rain_drops.tga";
    sSeaDropsTexture = "weather\\sea_rain_drops.tga";
}

RAIN::~RAIN()
{
    Release();
}

void RAIN::Release()
{
    STORM_DELETE(pRainBlocks);
    if (iVertexBuffer >= 0)
        rs->ReleaseVertexBuffer(iVertexBuffer);
    if (iRainbowTex >= 0)
        rs->TextureRelease(iRainbowTex);
    if (iRainDropsTexture >= 0)
        rs->TextureRelease(iRainDropsTexture);
    if (iSeaDropTex >= 0)
        rs->TextureRelease(iSeaDropTex);
    iVertexBuffer = -1;
    iRainbowTex = -1;
    iSeaDropTex = -1;

    if (iIBSeaDrops >= 0)
        rs->ReleaseIndexBuffer(iIBSeaDrops);
    if (iVBSeaDrops >= 0)
        rs->ReleaseVertexBuffer(iVBSeaDrops);

    iIBSeaDrops = -1;
    iVBSeaDrops = -1;
}

void RAIN::GenerateRandomDrop(CVECTOR *vPos) const
{
    const auto fDist = 1.5f + FRAND(fRainRadius);
    const auto fAngle = FRAND(PIm2);
    vPos->x = fDist * cosf(fAngle);
    vPos->z = fDist * sinf(fAngle);
    vPos->y = fRainHeight - FRAND(fRainHeight * 2.0f);
}

void RAIN::SetDevice()
{
}

void RAIN::GenerateRain()
{
    uint32_t i;

    entid_t ent;
    if (!(ent = core.GetEntityId("weather")))
        throw std::runtime_error("No found WEATHER entity!");
    pWeather = static_cast<WEATHER_BASE *>(core.GetEntityPointer(ent));
    Assert(pWeather);

    Release();

    /*STORM_DELETE(pRainBlocks);
    if (iVertexBuffer>=0) RS->ReleaseVertexBuffer(iVertexBuffer);
    iVertexBuffer = -1;*/

    // fRainHeight = Weather->GetFloat(whf_rain_height);
    // fRainRadius = Weather->GetFloat(whf_rain_radius);
    // float fRainRadiusD2 = fRainRadius / 2.0f;
    // dwNumDrops = Weather->GetInt(whi_rain_drops_num);

    if (dwNumDrops == 0)
        return;

    dwNumRainBlocks = 4;

    dwRainR = (dwRainColor >> 0x10) & 0xFF;
    dwRainG = (dwRainColor >> 0x8) & 0xFF;
    dwRainB = (dwRainColor)&0xFF;

    fWindPower = pWeather->GetFloat(whf_wind_speed) / 18.0f;
    fWindAngle = pWeather->GetFloat(whf_wind_angle);
    fRainWindSpeed = fWindPower * 0.2f;

    pRainBlocks = new rainblock_t[dwNumRainBlocks];

    pRainBlocks[0].vPos = CVECTOR(0.0f, 0.0f, 0.0f);
    pRainBlocks[1].vPos = CVECTOR(0.0f, fRainHeight, 0.0f);
    pRainBlocks[2].vPos = CVECTOR(0.0f, fRainHeight / 2.0f, 0.0f);
    pRainBlocks[3].vPos = CVECTOR(0.0f, fRainHeight + fRainHeight / 2.0f, 0.0f);

    for (i = 0; i < dwNumRainBlocks; i++)
    {
        InitialSomeBlockParameters(i);

        pRainBlocks[i].dwTime = dwRainTimeBlend;
        pRainBlocks[i].vAng.y = FRAND(PIm2);

        const auto jitter = fRainWindSpeedJitter; // Weather->GetFloat(whf_rain_wind_speed_jitter);
        pRainBlocks[i].fWindSpeedJitter = FRAND(jitter) - jitter / 2.0f;
    }

    iVertexBuffer =
        rs->CreateVertexBuffer(D3DRAINVERTEX_FORMAT, dwNumDrops * 2 * sizeof(RAINVERTEX), D3DUSAGE_WRITEONLY);
    if (iVertexBuffer < 0)
        return;

    auto *const pVertBuf = static_cast<RAINVERTEX *>(rs->LockVertexBuffer(iVertexBuffer));
    if (!pVertBuf)
        return;

    for (i = 0; i < dwNumDrops; i++)
    {
        GenerateRandomDrop(&pVertBuf[i * 2 + 0].vPos);
        pVertBuf[i * 2 + 0].dwColor = 0xFFFFFFFF;

        pVertBuf[i * 2 + 1].vPos = pVertBuf[i * 2 + 0].vPos;
        pVertBuf[i * 2 + 1].vPos.y += fDropLength;
        pVertBuf[i * 2 + 1].dwColor = 0xFFFFFF;
    }
    rs->UnLockVertexBuffer(iVertexBuffer);

    iIBSeaDrops =
        rs->CreateIndexBuffer(NUM_SEA_DROPS * 2 * 3 * sizeof(uint16_t), D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY);
    iVBSeaDrops = rs->CreateVertexBuffer(D3DSEADROPVERTEX_FORMAT, NUM_SEA_DROPS * 4 * sizeof(SEADROPVERTEX),
                                         D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY);

    auto *pI = static_cast<uint16_t *>(rs->LockIndexBuffer(iIBSeaDrops));
    if (pI)
    {
        for (int32_t i = 0; i < NUM_SEA_DROPS; i++)
        {
            *pI++ = static_cast<uint16_t>(i * 4 + 0);
            *pI++ = static_cast<uint16_t>(i * 4 + 3);
            *pI++ = static_cast<uint16_t>(i * 4 + 2);

            *pI++ = static_cast<uint16_t>(i * 4 + 0);
            *pI++ = static_cast<uint16_t>(i * 4 + 2);
            *pI++ = static_cast<uint16_t>(i * 4 + 1);
        }
        rs->UnLockIndexBuffer(iIBSeaDrops);
    }

    // rainbow
    if (bRainbowEnable)
    {
        iRainbowTex = rs->TextureCreate(sRainbowTexture.c_str());
    }

    iRainDropsTexture = rs->TextureCreate(sDropsTexture.c_str());

    iSeaDropTex = rs->TextureCreate(sSeaDropsTexture.c_str());

    fDropsDeltaTime = 0.0f;
}

bool RAIN::Init()
{
    rs = static_cast<VDX9RENDER *>(core.GetService("dx9render"));
    cs = static_cast<COLLIDE *>(core.GetService("coll"));

    SetDevice();

    return true;
}

bool RAIN::CreateState(ENTITY_STATE_GEN *state_gen)
{
    return true;
}

bool RAIN::LoadState(ENTITY_STATE *state)
{
    return true;
}

void RAIN::Execute(uint32_t Delta_Time)
{
}

void RAIN::InitialSomeBlockParameters(int32_t iIdx) const
{
    const auto fDist = 6.0f * 5.4f * fWindPower;
    pRainBlocks[iIdx].vPos.x = -fDist * sinf(fWindAngle);
    pRainBlocks[iIdx].vPos.z = -fDist * cosf(fWindAngle);
    pRainBlocks[iIdx].fWindFlaw = 0.0f;
}

void RAIN::RealizeDrops(uint32_t Delta_Time)
{
    auto fDeltaTime = static_cast<float>(Delta_Time) * 0.001f;

    static auto dwShipName = MakeHashValue("SHIP");

    CMatrix mView;
    rs->GetTransform(D3DTS_VIEW, mView);
    mView.Transposition();

    float fFov;
    rs->GetCamera(vCamPos, vCamAng, fFov);

    vCamPos = mView.Pos();

    entid_t sea_id;
    SEA_BASE *pSea = nullptr;
    if (sea_id = core.GetEntityId("sea"))
        pSea = static_cast<SEA_BASE *>(core.GetEntityPointer(sea_id));

    fDropsDeltaTime += fDeltaTime;

    auto iNumNewDrops1 = static_cast<int32_t>(fDropsDeltaTime * static_cast<float>(dwDropsNearNum));
    auto iNumNewDrops2 = static_cast<int32_t>(fDropsDeltaTime * static_cast<float>(dwDropsFarNum));
    fDropsDeltaTime -= static_cast<float>(static_cast<double>(iNumNewDrops1 + iNumNewDrops2) /
                                          static_cast<double>(dwDropsNearNum + dwDropsFarNum));
    if (fDropsDeltaTime < 0.0f)
        fDropsDeltaTime = 0.0f;

    if (auto && entities = core.GetEntityIds(RAIN_DROPS); !entities.empty())
    {
        for (int32_t i = 0; i < iNumNewDrops1 + iNumNewDrops2; i++)
        {
            SHIP_BASE *pShip = nullptr;
            float fA, fS, fR;
            CVECTOR vSrc, vDst;

            fS = 0.0f;
            fR = fDropsNearRadius;
            if (i >= iNumNewDrops1)
            {
                fS = fDropsNearRadius;
                fR = fDropsFarRadius;
            }

            fA = FRAND(PIm2);
            fR = fS + FRAND(fR);

            vSrc = CVECTOR(vCamPos.x + fR * sinf(fA), vCamPos.y + 75.0f, vCamPos.z + fR * cosf(fA));
            vDst = CVECTOR(vSrc.x, vCamPos.y - 75.0f, vSrc.z);

            auto fTest1 = cs->Trace(entities, vSrc, vDst, nullptr, 0);
            auto fTest2 = 2.0f;

            if (pSea)
            {
                float y = pSea->WaveXZ(vSrc.x, vSrc.z);
                if (y >= vDst.y && y <= vSrc.y)
                    fTest2 = (vSrc.y - y) / (vSrc.y - vDst.y);
            }
            float fTest = fTest2;
            if (fTest1 <= 1.0f && fTest1 < fTest2)
            {
                fTest = fTest1;

                // check - if it's a ship
                entid_t eid = cs->GetObjectID();
                if (core.GetClassCode(eid) == dwShipName)
                {
                    pShip = static_cast<SHIP_BASE *>(core.GetEntityPointer(eid));
                }
            }
            else if (fTest2 <= 1.0f)
            {
                // seadrop_t & sea_drop = aSeaDrops[aSeaDrops.Add()];
                // sea_drop.vPos = vSrc + fTest * (vDst - vSrc);
                // sea_drop.fTime = 1.0f;
                // sea_drop.fLifeTime = 1.0f;
                aSeaDrops.push_back(seadrop_t{vSrc + fTest * (vDst - vSrc), 1.0f, 1.0f});
            }

            if (fTest <= 1.0f)
            {
                // Add a drop
                aDrops.push_back(drop_t{});
                // drop_t & drop = aDrops[aDrops.Add()];
                drop_t &drop = aDrops.back();
                drop.vPos = vSrc + fTest * (vDst - vSrc);
                drop.fLifeTime = fDropsLifeTime + fDeltaTime;
                drop.iShip = -1;
                if (pShip)
                {
                    int32_t k;
                    for (k = 0; k < aShips.size(); k++)
                        if (aShips[k].pShip == pShip)
                            break;
                    if (k == aShips.size())
                    {
                        // ship_t & ship = aShips[aShips.Add()];
                        // ship.eid = pShip->GetId();
                        // ship.pShip = pShip;
                        aShips.push_back(ship_t{pShip->GetId(), pShip});
                    }
                    CMatrix mShip = *pShip->GetMatrix();
                    mShip.Transposition();
                    drop.iShip = k;
                    drop.vPos = mShip * drop.vPos;
                }
            }
        }
    }

    aRects.clear();

    for (int32_t i = 0; i < aShips.size(); i++)
    {
        if (!core.GetEntityPointer(aShips[i].eid))
        {
            aShips[i].pShip = nullptr;
        }
    }

    for (int32_t i = 0; i < aDrops.size(); i++)
    {
        drop_t &drop = aDrops[i];

        drop.fLifeTime -= fDeltaTime;
        if (drop.fLifeTime <= 0.0f || (drop.iShip >= 0 && !aShips[drop.iShip].pShip))
        {
            // aDrops.ExtractNoShift(i);
            aDrops[i] = aDrops.back();
            aDrops.pop_back();
            i--;
            continue;
        }

        // RS_RECT & r = aRects[aRects.Add()];
        RS_RECT r;
        if (drop.iShip >= 0)
            r.vPos = (*aShips[drop.iShip].pShip->GetMatrix()) * drop.vPos;
        else
            r.vPos = drop.vPos;

        float fDistance = sqrtf(SQR(r.vPos.x - vCamPos.x) + SQR(r.vPos.z - vCamPos.z));
        float fK = Bring2Range(0.4f, 2.0f, 0.0f, 30.0f, fDistance);

        r.vPos.y += fK * fDropsSize;
        r.fAngle = 0.0f;
        r.fSize = fK * fDropsSize;
        r.dwColor = dwDropsColor;
        r.dwSubTexture = static_cast<uint32_t>(8.0f * drop.fLifeTime / fDropsLifeTime);

        aRects.push_back(r);
    }

    rs->TextureSet(0, iRainDropsTexture);
    // rs->SetTexture(0, null);
    rs->DrawRects(aRects.data(), aRects.size(), "rain_drops", 8, 1);

    // draw circles on the water
    CMatrix IMatrix;
    IMatrix.SetIdentity();
    rs->SetWorld(IMatrix);
    rs->TextureSet(0, iSeaDropTex);

    auto pVSeaDropBuffer = static_cast<SEADROPVERTEX *>(rs->LockVertexBuffer(iVBSeaDrops, D3DLOCK_DISCARD));
    int32_t n = 0;
    if (pVSeaDropBuffer)
        for (int32_t i = 0; i < aSeaDrops.size(); i++)
        {
            seadrop_t &drop = aSeaDrops[i];

            drop.fTime -= fDeltaTime;
            if (drop.fTime <= 0.0f)
            {
                // aSeaDrops.ExtractNoShift(i);
                aSeaDrops[i] = aSeaDrops.back();
                aSeaDrops.pop_back();
                i--;
                continue;
            }

            CVECTOR v = drop.vPos;
            v.y = pSea->WaveXZ(v.x, v.z) + 0.015f;

            SEADROPVERTEX *pV = &pVSeaDropBuffer[n * 4];

            float fSize = 0.15f;
            int32_t frame = static_cast<int32_t>((1.0f - drop.fTime / drop.fLifeTime) * 8.0f);
            float du = 1.0f / 8.0f;
            float u = static_cast<float>(frame) * du;

            pV[0].vPos = v + CVECTOR(-fSize, 0.0f, -fSize);
            pV[0].dwColor = 0xFFFFFFFF;
            pV[0].tu = u;
            pV[0].tv = 0.0f;

            pV[1].vPos = v + CVECTOR(fSize, 0.0f, -fSize);
            pV[1].dwColor = 0xFFFFFFFF;
            pV[1].tu = u + du;
            pV[1].tv = 0.0f;

            pV[2].vPos = v + CVECTOR(fSize, 0.0f, fSize);
            pV[2].dwColor = 0xFFFFFFFF;
            pV[2].tu = u + du;
            pV[2].tv = 1.0f;

            pV[3].vPos = v + CVECTOR(-fSize, 0.0f, fSize);
            pV[3].dwColor = 0xFFFFFFFF;
            pV[3].tu = u;
            pV[3].tv = 1.0f;

            n++;

            if (n >= NUM_SEA_DROPS)
            {
                rs->UnLockVertexBuffer(iVBSeaDrops);
                rs->DrawBuffer(iVBSeaDrops, sizeof(SEADROPVERTEX), iIBSeaDrops, 0, n * 4, 0, n * 2, "sea_rain_drops");

                n = 0;
                pVSeaDropBuffer = static_cast<SEADROPVERTEX *>(rs->LockVertexBuffer(iVBSeaDrops, D3DLOCK_DISCARD));
            }
        }

    if (n > 0)
    {
        rs->UnLockVertexBuffer(iVBSeaDrops);
        rs->DrawBuffer(iVBSeaDrops, sizeof(SEADROPVERTEX), iIBSeaDrops, 0, n * 4, 0, n * 2, "sea_rain_drops");
    }
}

void RAIN::Realize(uint32_t Delta_Time)
{
    if (!bShow)
        return;

    uint32_t i;
    float fFov;

    rs->GetCamera(vCamPos, vCamAng, fFov);

    if (iVertexBuffer >= 0)
    {
        bool bDraw = rs->TechniqueExecuteStart("rain");

        if (bDraw)
            for (i = 0; i < dwNumRainBlocks; i++)
            {
                auto dwAlpha = static_cast<uint32_t>(255.0f * static_cast<float>(pRainBlocks[i].dwTime) /
                                                     static_cast<float>(dwRainTimeBlend));
                if (dwAlpha > dwRainMaxBlend)
                    dwAlpha = dwRainMaxBlend;
                uint32_t dwColor = ARGB(dwAlpha, dwRainR, dwRainG, dwRainB);
                rs->SetRenderState(D3DRS_TEXTUREFACTOR, dwColor);

                CMatrix mY1, mX, mY2, mWorld;

                mWorld.BuildPosition(vCamPos.x + pRainBlocks[i].vPos.x, pRainBlocks[i].vPos.y,
                                     vCamPos.z + pRainBlocks[i].vPos.z);

                mY1.BuildRotateY(pRainBlocks[i].vAng.y);
                mX.BuildRotateX(fWindPower + pRainBlocks[i].fWindSpeedJitter);
                mY2.BuildRotateY(PI + fWindAngle);

                mWorld = mY1 * mX * mY2 * mWorld;

                pRainBlocks[i].vPos.y -= fRainSpeed * 0.2f * static_cast<float>(Delta_Time) / 60.0f;
                pRainBlocks[i].fWindFlaw += FRAND(fRainJitter) - fRainJitter / 2.0f;
                float f = 0.0f; // pRainBlocks[i].fWindFlaw * float(Delta_Time) / 50.0f;
                pRainBlocks[i].vPos.x +=
                    (fRainSpeed * fRainWindSpeed + f) * static_cast<float>(Delta_Time) / 60.0f * sinf(fWindAngle);
                pRainBlocks[i].vPos.z +=
                    (fRainSpeed * fRainWindSpeed + f) * static_cast<float>(Delta_Time) / 60.0f * cosf(fWindAngle);
                pRainBlocks[i].dwTime += Delta_Time;
                if (pRainBlocks[i].vPos.y < vCamPos.y - fRainHeight)
                {
                    InitialSomeBlockParameters(i);
                    pRainBlocks[i].vPos.y += fRainHeight * 2.0f - 1.0f;
                    pRainBlocks[i].vAng.y = FRAND(PIm2);
                    pRainBlocks[i].dwTime = 0;
                }

                rs->SetTransform(D3DTS_WORLD, mWorld);

                rs->DrawPrimitive(D3DPT_LINELIST, iVertexBuffer, sizeof(RAINVERTEX), 0, dwNumDrops);
            }
        if (bDraw)
            while (rs->TechniqueExecuteNext())
            {
            }

        RealizeDrops(Delta_Time);
    }
    // draw rainbow
    if (bRainbowEnable)
    {
        RS_RECT rs_rect;
        CVECTOR vPos = vCamPos + CVECTOR(0.32f, 0.0f, 0.72f) * 1900.0f;
        vPos.y = 350.0f;
        rs_rect.dwColor = 0x0F0F0F;
        rs_rect.vPos = vPos;
        rs_rect.fSize = 1600.0f;
        rs_rect.fAngle = 0.0f;

        rs->TextureSet(0, iRainbowTex);
        rs->DrawRects(&rs_rect, 1, "rainbow");
    }
}

uint64_t RAIN::ProcessMessage(MESSAGE &message)
{
    switch (message.Long())
    {
    case 1:
        bShow = (0 != message.Long());
        break;
    }
    return 0;
}

uint32_t RAIN::AttributeChanged(ATTRIBUTES *pAttribute)
{
    if (*pAttribute == "Clear")
    {
        Release();
        bRainbowEnable = false;
        dwNumDrops = 0;
        fDropsDeltaTime = 0.0f;
        return 0;
    }

    if (*pAttribute == "isDone")
    {
        GenerateRain();
        return 0;
    }

    ATTRIBUTES *pParent = pAttribute->GetParent();
    if (*pParent == "RainDrops")
    {
        if (*pAttribute == "DropsNearNum")
        {
            dwDropsNearNum = pAttribute->GetAttributeAsDword();
            return 0;
        }
        if (*pAttribute == "DropsFarNum")
        {
            dwDropsFarNum = pAttribute->GetAttributeAsDword();
            return 0;
        }
        if (*pAttribute == "DropsNearRadius")
        {
            fDropsNearRadius = pAttribute->GetAttributeAsFloat();
            return 0;
        }
        if (*pAttribute == "DropsFarRadius")
        {
            fDropsFarRadius = pAttribute->GetAttributeAsFloat();
            return 0;
        }
        if (*pAttribute == "DropsLifeTime")
        {
            fDropsLifeTime = pAttribute->GetAttributeAsFloat();
            return 0;
        }
        if (*pAttribute == "DropsSize")
        {
            fDropsSize = pAttribute->GetAttributeAsFloat();
            return 0;
        }
        if (*pAttribute == "DropsColor")
        {
            dwDropsColor = pAttribute->GetAttributeAsDword();
            return 0;
        }
        if (*pAttribute == "DropsTexture")
        {
            sDropsTexture = to_string(pAttribute->GetThisAttr());
            return 0;
        }
    }
    else if (*pParent == "Rainbow")
    {
        if (*pAttribute == "Enable")
        {
            bRainbowEnable = pAttribute->GetAttributeAsDword() == 1;
            return 0;
        }
        if (*pAttribute == "Texture")
        {
            sRainbowTexture = to_string(pAttribute->GetThisAttr());
            return 0;
        }
    }
    else
    {
        if (*pAttribute == "color")
        {
            dwRainColor = pAttribute->GetAttributeAsDword();
            return 0;
        }
        if (*pAttribute == "NumDrops")
        {
            dwNumDrops = pAttribute->GetAttributeAsDword();
            return 0;
        }
        if (*pAttribute == "DropLength")
        {
            fDropLength = pAttribute->GetAttributeAsFloat();
            return 0;
        }
        if (*pAttribute == "Height")
        {
            fRainHeight = pAttribute->GetAttributeAsFloat();
            return 0;
        }
        if (*pAttribute == "Radius")
        {
            fRainRadius = pAttribute->GetAttributeAsFloat();
            return 0;
        }
        if (*pAttribute == "Speed")
        {
            fRainSpeed = pAttribute->GetAttributeAsFloat();
            return 0;
        }
        if (*pAttribute == "Jitter")
        {
            fRainJitter = pAttribute->GetAttributeAsFloat();
            return 0;
        }
        if (*pAttribute == "WindSpeedJitter")
        {
            fRainWindSpeedJitter = pAttribute->GetAttributeAsFloat();
            return 0;
        }
        if (*pAttribute == "TimeBlend")
        {
            dwRainTimeBlend = pAttribute->GetAttributeAsDword();
            return 0;
        }
        if (*pAttribute == "MaxBlend")
        {
            dwRainMaxBlend = pAttribute->GetAttributeAsDword();
            return 0;
        }
    }

    return 0;
}
