#include "track.h"
#include "entity.h"
#include "core.h"
#include "defines.h"
#include "inlines.h"

VDX9RENDER *ShipTracks::ShipTrack::pRS = nullptr;
SEA_BASE *ShipTracks::ShipTrack::pSea = nullptr;
int32_t ShipTracks::ShipTrack::iRefCount = 0;
uint32_t ShipTracks::ShipTrack::dwMaxBufferSize1 = 0, ShipTracks::ShipTrack::dwMaxBufferSize2 = 0;
int32_t ShipTracks::ShipTrack::iVTmpBuffer1 = -1, ShipTracks::ShipTrack::iVTmpBuffer2 = -1;
int32_t ShipTracks::ShipTrack::iITmpBuffer1 = -1, ShipTracks::ShipTrack::iITmpBuffer2 = -1;

ShipTracks::~ShipTracks()
{
    for (const auto &ship : aShips)
        delete ship;
    // aShips.DelAllWithPointers();
}

bool ShipTracks::Init()
{
    entid_t sea_id;

    ShipTrack::pRS = static_cast<VDX9RENDER *>(core.GetService("dx9render"));
    Assert(ShipTrack::pRS);
    if (sea_id = EntityManager::GetEntityId("sea"))
        ShipTrack::pSea = static_cast<SEA_BASE *>(EntityManager::GetEntityPointer(sea_id));
    return true;
}

void ShipTracks::ResetTrack(SHIP_BASE *pShip)
{
    for (int32_t i = 0; i < aShips.size(); i++)
        if (aShips[i]->pShip == pShip)
        {
            aShips[i]->Reset();
            break;
        }
}

void ShipTracks::AddShip(SHIP_BASE *pShip)
{
    auto *pST = new ShipTrack;

    if (pST->Update(pShip))
        aShips.push_back(pST);
    else
        delete pST;
}

void ShipTracks::DelShip(SHIP_BASE *pShip)
{
    for (int32_t i = 0; i < aShips.size(); i++)
        if (aShips[i]->pShip == pShip)
        {
            STORM_DELETE(aShips[i]);
            aShips.erase(aShips.begin() + i);
            break;
        }
}

void ShipTracks::Execute(uint32_t dwDeltaTime)
{
}

void ShipTracks::Realize(uint32_t dwDeltaTime)
{
    for (int32_t i = 0; i < aShips.size(); i++)
    {
        aShips[i]->Execute(static_cast<float>(dwDeltaTime) * 0.001f);
        aShips[i]->Realize(static_cast<float>(dwDeltaTime) * 0.001f);
    }
}

ShipTracks::ShipTrack::ShipTrack()
{
    bFirstExecute = true;
    iTrackTexture1 = -1;
    iTrackTexture2 = -1;
    iRefCount++;
}

ShipTracks::ShipTrack::~ShipTrack()
{
    iRefCount--;
    if (iRefCount <= 0)
    {
        iRefCount = 0;

        pRS->ReleaseVertexBuffer(iVTmpBuffer1);
        iVTmpBuffer1 = -1;
        pRS->ReleaseVertexBuffer(iVTmpBuffer2);
        iVTmpBuffer2 = -1;

        pRS->ReleaseIndexBuffer(iITmpBuffer1);
        iITmpBuffer1 = -1;
        pRS->ReleaseIndexBuffer(iITmpBuffer2);
        iITmpBuffer2 = -1;

        dwMaxBufferSize1 = 0;
        dwMaxBufferSize2 = 0;
    }

    pRS->TextureRelease(iTrackTexture1);
    iTrackTexture1 = -1;
    pRS->TextureRelease(iTrackTexture2);
    iTrackTexture2 = -1;
}

bool ShipTracks::ShipTrack::Update(SHIP_BASE *pShip)
{
    auto *pAChar = pShip->GetACharacter();
    Assert(pAChar);
    auto *pATrack = pAChar->FindAClass(pAChar, "Ship.Track");
    auto *pATrack1 = pAChar->FindAClass(pAChar, "Ship.Track1");
    auto *pATrack2 = pAChar->FindAClass(pAChar, "Ship.Track2");

    if (!pATrack || !pATrack1 || !pATrack2)
        return false;
    if (!pATrack->GetAttributeAsDword("Enable", 0))
        return false;

    fTrackDistance = pATrack->GetAttributeAsFloat("TrackDistance");
    fUP1 = pATrack->GetAttributeAsFloat("WaveHeight1");
    fUP2 = pATrack->GetAttributeAsFloat("WaveHeight2");

    const std::string sTex1 = pATrack1->GetAttribute("Texture");
    fZStart1 = pATrack1->GetAttributeAsFloat("ZStart");
    fLifeTime1 = pATrack1->GetAttributeAsFloat("LifeTime");
    sscanf(pATrack1->GetAttribute("Width"), "%f, %f", &fWidth11, &fWidth12);
    sscanf(pATrack1->GetAttribute("Speed"), "%f, %f", &fSpeed11, &fSpeed12);
    fTrackStep1 = pATrack1->GetAttributeAsFloat("TrackWidthSteps");

    const std::string sTex2 = pATrack2->GetAttribute("Texture");
    fZStart2 = pATrack2->GetAttributeAsFloat("ZStart");
    fLifeTime2 = pATrack2->GetAttributeAsFloat("LifeTime");
    sscanf(pATrack2->GetAttribute("Width"), "%f, %f", &fWidth21, &fWidth22);
    sscanf(pATrack2->GetAttribute("Speed"), "%f, %f", &fSpeed21, &fSpeed22);
    fTrackStep2 = pATrack2->GetAttributeAsFloat("TrackWidthSteps");

    dwTrackStep1 = static_cast<int32_t>(fTrackStep1);
    dwTrackStep2 = static_cast<int32_t>(fTrackStep2);

    pRS->TextureRelease(iTrackTexture1);
    iTrackTexture1 = -1;
    pRS->TextureRelease(iTrackTexture2);
    iTrackTexture2 = -1;

    this->pShip = pShip;
    // TODO: check this
    //this->fTrackDistance = fTrackDistance; //~!~

    if (this->pShip)
    {
        iTrackTexture1 = pRS->TextureCreate(sTex1.c_str());
        iTrackTexture2 = pRS->TextureCreate(sTex2.c_str());

        this->vLastPos = pShip->GetPos();
        this->vLastAng = pShip->GetAng();
        this->fCurTV = 0.0f;
    }

    aTrack1.clear();
    aTrack2.clear();

    return true;
}

bool ShipTracks::ShipTrack::Reserve1(uint32_t dwSize)
{
    const auto dwNewSize = (dwSize / (100) + 1) * (100);

    if (dwMaxBufferSize1 >= dwNewSize)
        return iVTmpBuffer1 != -1 && iITmpBuffer1 != -1;

    dwMaxBufferSize1 = dwNewSize;

    pRS->ReleaseVertexBuffer(iVTmpBuffer1);
    iVTmpBuffer1 = -1;
    pRS->ReleaseIndexBuffer(iITmpBuffer1);
    iITmpBuffer1 = -1;

    iVTmpBuffer1 = pRS->CreateVertexBuffer(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1,
                                           dwMaxBufferSize1 * dwTrackStep1 * sizeof(TrackVertex),
                                           D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC);
    iITmpBuffer1 = pRS->CreateIndexBuffer(dwMaxBufferSize1 * dwTrackStep1 * 6 * sizeof(uint16_t),
                                          D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC);

    if (iVTmpBuffer1 == -1 || iITmpBuffer1 == -1)
    {
        dwMaxBufferSize1 = 0;
        pRS->ReleaseVertexBuffer(iVTmpBuffer1);
        iVTmpBuffer1 = -1;
        pRS->ReleaseIndexBuffer(iITmpBuffer1);
        iITmpBuffer1 = -1;
        return false;
    }

    auto *pI = static_cast<uint16_t *>(pRS->LockIndexBuffer(iITmpBuffer1));
    for (uint32_t y = 0; y < dwNewSize; y++)
        for (uint32_t x = 0; x < dwTrackStep1 - 1; x++)
        {
            *pI++ = static_cast<uint16_t>((y + 0) * dwTrackStep1 + x);
            *pI++ = static_cast<uint16_t>((y + 1) * dwTrackStep1 + x);
            *pI++ = static_cast<uint16_t>((y + 0) * dwTrackStep1 + x + 1);

            *pI++ = static_cast<uint16_t>((y + 1) * dwTrackStep1 + x);
            *pI++ = static_cast<uint16_t>((y + 1) * dwTrackStep1 + x + 1);
            *pI++ = static_cast<uint16_t>((y + 0) * dwTrackStep1 + x + 1);
        }
    pRS->UnLockIndexBuffer(iITmpBuffer1);

    return true;
}

bool ShipTracks::ShipTrack::Reserve2(uint32_t dwSize)
{
    const auto dwNewSize = (dwSize / (20) + 1) * (20);

    if (dwMaxBufferSize2 >= dwNewSize)
        return iVTmpBuffer2 != -1 && iITmpBuffer2 != -1;

    dwMaxBufferSize2 = dwNewSize;

    pRS->ReleaseVertexBuffer(iVTmpBuffer2);
    iVTmpBuffer2 = -1;
    pRS->ReleaseIndexBuffer(iITmpBuffer2);
    iITmpBuffer2 = -1;

    iVTmpBuffer2 = pRS->CreateVertexBuffer(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1,
                                           dwMaxBufferSize2 * dwTrackStep2 * sizeof(TrackVertex),
                                           D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC);
    iITmpBuffer2 = pRS->CreateIndexBuffer(dwMaxBufferSize2 * dwTrackStep2 * 6 * sizeof(uint16_t), D3DUSAGE_WRITEONLY);

    if (iVTmpBuffer2 == -1 || iITmpBuffer2 == -1)
    {
        dwMaxBufferSize2 = 0;
        pRS->ReleaseVertexBuffer(iVTmpBuffer2);
        iVTmpBuffer2 = -1;
        pRS->ReleaseIndexBuffer(iITmpBuffer2);
        iITmpBuffer2 = -1;
        return false;
    }

    auto *pI = static_cast<uint16_t *>(pRS->LockIndexBuffer(iITmpBuffer2));
    for (uint32_t y = 0; y < dwNewSize; y++)
        for (uint32_t x = 0; x < dwTrackStep2 - 1; x++)
        {
            *pI++ = static_cast<uint16_t>((y + 0) * dwTrackStep2 + x);
            *pI++ = static_cast<uint16_t>((y + 1) * dwTrackStep2 + x);
            *pI++ = static_cast<uint16_t>((y + 0) * dwTrackStep2 + x + 1);

            *pI++ = static_cast<uint16_t>((y + 1) * dwTrackStep2 + x);
            *pI++ = static_cast<uint16_t>((y + 1) * dwTrackStep2 + x + 1);
            *pI++ = static_cast<uint16_t>((y + 0) * dwTrackStep2 + x + 1);
        }
    pRS->UnLockIndexBuffer(iITmpBuffer2);

    return true;
}

void ShipTracks::ShipTrack::Execute(float fDeltaTime)
{
    if (!pShip || !pSea)
        return;

    if (bFirstExecute)
    {
        vLastPos = pShip->GetPos();
        vLastAng = pShip->GetAng();
        fCurTV = 0.0f;
        bFirstExecute = false;
    }

    float fFov;
    CVECTOR vCamPos, vCamAng;
    pRS->GetCamera(vCamPos, vCamAng, fFov);
    const auto fCamDist = Clamp(sqrtf(~vCamPos) / 10000.0f);
    const auto fWaveUP = fUP1 + fCamDist * (fUP2 - fUP1);

    const auto vCurPos = pShip->GetPos();
    const auto vCurAng = pShip->GetAng();

    const auto vBoxSize = pShip->GetBoxsize();

    auto vDist = vCurPos - vLastPos;
    vDist.y = 0.0;
    auto fCurrentDistance = sqrtf(~vDist);
    if (fCurrentDistance > 100.0f)
    {
        fCurrentDistance = 0.0f;
        vLastPos = vCurPos;

        aTrack1.clear();
        aTrack2.clear();
    }
    if (fCurrentDistance > fTrackDistance)
    {
        const auto fSpeed = Min(1.0f, pShip->GetCurrentSpeed() / 20.0f);
        for (int32_t i = 0; i < static_cast<int32_t>(fCurrentDistance / fTrackDistance); i++)
        {
            const auto fDistance = (i + 1) * fTrackDistance;

            // ~!~ optimize?
            aTrack1.insert(aTrack1.begin(), Track{});
            auto vDir = (vCurPos - vLastPos);
            vDir.y = 0.0f;
            vDir = !vDir;
            aTrack1[0].vPos = vLastPos + vDir * (vBoxSize.z * fZStart1 + fDistance);
            aTrack1[0].fCos = cosf(vCurAng.y);
            aTrack1[0].fSin = sinf(vCurAng.y);
            aTrack1[0].fTime = 0.0f;
            aTrack1[0].fTV = fCurTV;
            aTrack1[0].fWidth = RRnd(fWidth11, fWidth12);          // 3.0f;
            aTrack1[0].fSpeed = fSpeed * RRnd(fSpeed11, fSpeed12); // 4.5f + (FRAND(1.0f) - 0.5f);
            aTrack1[0].fInitialAlpha = fSpeed;

            aTrack2.insert(aTrack2.begin(), Track{});
            aTrack2[0].vPos = vLastPos + vDir * (vBoxSize.z * fZStart2 + fDistance);
            aTrack2[0].fCos = cosf(vCurAng.y);
            aTrack2[0].fSin = sinf(vCurAng.y);
            aTrack2[0].fTime = 0.0f;
            aTrack2[0].fTV = fCurTV;
            aTrack2[0].fWidth = RRnd(fWidth21, fWidth22);          // 12.0f;
            aTrack2[0].fSpeed = fSpeed * RRnd(fSpeed21, fSpeed22); // 1.5f + (FRAND(0.4f) - 0.2f);
            aTrack2[0].fInitialAlpha = fSpeed;

            fCurTV += 0.05f;
        }
        vLastPos = vCurPos;
        vLastAng = vCurAng;
    }

    for (int32_t i = 0; i < aTrack1.size(); i++)
    {
        auto &T = aTrack1[i];

        T.fTime += fDeltaTime;
        T.fAlpha = T.fInitialAlpha * Clamp(1.0f - T.fTime / fLifeTime1); // 22.0f);
        T.fWidth += fDeltaTime * (T.fSpeed * (1.0f - T.fTime / fLifeTime1));

        if (T.fTime >= fLifeTime1)
        {
            aTrack1.erase(aTrack1.begin() + i);
            i--;
        }
    }

    if (Reserve1(aTrack1.size()))
        if (aTrack1.size() > 1)
        {
            auto *pV = static_cast<TrackVertex *>(pRS->LockVertexBuffer(iVTmpBuffer1, D3DLOCK_DISCARD));
            for (int32_t i = 0; i < aTrack1.size(); i++)
            {
                auto &T = aTrack1[i];
                int32_t xxx = 0;
                for (float xx = 0; xx < fTrackStep1; xx++)
                {
                    const auto k = xx / (fTrackStep1 - 1.0f);
                    auto x = T.fWidth * (k - 0.5f);
                    auto z = 0.0f;
                    RotateAroundY(x, z, T.fCos, T.fSin);
                    x += T.vPos.x;
                    z += T.vPos.z;
                    auto vPos = CVECTOR(x, fWaveUP * (1.4f - fabsf((k * 2.0f) - 1.0f)) + pSea->WaveXZ(x, z), z);
                    pV[i * dwTrackStep1 + xxx].vPos = vPos - vCurPos;
                    pV[i * dwTrackStep1 + xxx].tu = xx / (fTrackStep1 - 1.0f);
                    pV[i * dwTrackStep1 + xxx].tv = T.fTV;
                    pV[i * dwTrackStep1 + xxx].dwColor = ARGB(T.fAlpha * 255.0f, 0, 0, 0);
                    xxx++;
                }
            }
            pRS->UnLockVertexBuffer(iVTmpBuffer1);
        }

    for (int32_t i = 0; i < aTrack2.size(); i++)
    {
        auto &T = aTrack2[i];

        T.fTime += fDeltaTime;
        T.fAlpha = T.fInitialAlpha * Clamp(1.0f - T.fTime / fLifeTime2); // 10.0f);
        T.fWidth -= fDeltaTime * (T.fSpeed * (1.0f - T.fTime / fLifeTime2));

        if (T.fTime >= fLifeTime2)
        {
            aTrack2.erase(aTrack2.begin() + i);
            i--;
        }
    }

    if (Reserve2(aTrack2.size()))
        if (aTrack2.size() > 1)
        {
            auto *pV = static_cast<TrackVertex *>(pRS->LockVertexBuffer(iVTmpBuffer2, D3DLOCK_DISCARD));
            for (int32_t i = 0; i < aTrack2.size(); i++)
            {
                auto &T = aTrack2[i];
                int32_t xxx = 0;
                for (float xx = 0; xx < fTrackStep2; xx++)
                {
                    const auto k = xx / (fTrackStep2 - 1.0f);
                    auto x = T.fWidth * (k - 0.5f);
                    auto z = 0.0f;
                    RotateAroundY(x, z, T.fCos, T.fSin);
                    x += T.vPos.x;
                    z += T.vPos.z;
                    auto vPos = CVECTOR(x, fWaveUP + pSea->WaveXZ(x, z), z);
                    pV[i * dwTrackStep2 + xxx].vPos = vPos - vCurPos;
                    pV[i * dwTrackStep2 + xxx].tu = static_cast<float>(xx) / (fTrackStep2 - 1.0f);
                    pV[i * dwTrackStep2 + xxx].tv = T.fTV * 6.0f;
                    pV[i * dwTrackStep2 + xxx].dwColor = ARGB(T.fAlpha * 255.0f, 0, 0, 0);
                    xxx++;
                }
            }
            pRS->UnLockVertexBuffer(iVTmpBuffer2);
        }
}

void ShipTracks::ShipTrack::Reset()
{
    bFirstExecute = true;

    aTrack1.clear();
    aTrack2.clear();
}

uint32_t ShipTracks::AttributeChanged(ATTRIBUTES *pA)
{
    if (*pA == "ResetTracks")
    {
        for (int32_t i = 0; i < aShips.size(); i++)
        {
            aShips[i]->Reset();
        }
    }
    return 0;
}

void ShipTracks::ShipTrack::Realize(float fDeltaTime)
{
    if (!pShip || !pSea)
        return;

    // pRS->SetTransform(D3DTS_WORLD, CMatrix());
    const CMatrix m;
    m.Pos() = pShip->GetPos();
    pRS->SetTransform(D3DTS_WORLD, m);

    if (aTrack1.size() > 1)
    {
        pRS->TextureSet(0, iTrackTexture1);
        pRS->DrawBuffer(iVTmpBuffer1, sizeof(TrackVertex), iITmpBuffer1, 0, aTrack1.size() * dwTrackStep1, 0,
                        (dwTrackStep1 - 1) * (aTrack1.size() - 1) * 2, "ShipTrack");
    }

    if (aTrack2.size() > 1)
    {
        pRS->TextureSet(0, iTrackTexture2);
        pRS->DrawBuffer(iVTmpBuffer2, sizeof(TrackVertex), iITmpBuffer2, 0, aTrack2.size() * dwTrackStep2, 0,
                        (dwTrackStep2 - 1) * (aTrack2.size() - 1) * 2, "ShipTrack");
    }
}
