#include "foam.h"
#include "core.h"
#include "math_inlines.h"
#include "math3d/plane.h"
#include "dx9render.h"
#include "math3d.h"

CoastFoam::CoastFoam()
{
    pSea = nullptr;
    iVBuffer = -1;
    iIBuffer = -1;
    iCursorTex = -1;

    bEditMode = false;
    bCanEdit = false;
    bMoved = false;

    fMaxFoamDistance = 1000.0f;
    fFoamDeltaY = 0.2f;
    iFoamDivides = 4;

    fCursorPosX = 400.0f;
    fCursorPosY = 300.0f;
}

CoastFoam::~CoastFoam()
{
    Save();
    clear();

    if (iVBuffer >= 0)
        rs->ReleaseVertexBuffer(iVBuffer);
    iVBuffer = -1;
    if (iIBuffer >= 0)
        rs->ReleaseIndexBuffer(iIBuffer);
    iIBuffer = -1;
}

bool CoastFoam::Init()
{
    rs = static_cast<VDX9RENDER *>(core.GetService("dx9render"));

    iVBuffer = rs->CreateVertexBuffer(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1 | D3DFVF_TEXTUREFORMAT2,
                                      sizeof(FoamVertex) * 5000, D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY);
    iIBuffer = rs->CreateIndexBuffer(10000 * 2 * 3, D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY);

    iCursorTex = rs->TextureCreate("cursor.tga");

    Load();

    bCanEdit = AttributesPointer->GetAttributeAsDword("edit", 0) != 0;
    return true;
}

void CoastFoam::Execute(uint32_t Delta_Time)
{
    bEditMode = (bCanEdit) ? (core.Controls->GetDebugKeyState(VK_NUMLOCK) < 0) : false;
    auto fDeltaTime = static_cast<float>(Delta_Time) * 0.001f;
}

void CoastFoam::ExtractRay(const D3DVIEWPORT9 &viewport, float fCursorX, float fCursorY, CVECTOR &raystart,
                           CVECTOR &rayend)
{
    auto matProj = rs->GetProjection();
    CVECTOR v;
    v.x = (((2.0f * fCursorX) / viewport.Width) - 1) / matProj.m[0][0];
    v.y = -(((2.0f * fCursorY) / viewport.Height) - 1) / matProj.m[1][1];
    v.z = 1.0f;

    CMatrix mView3x3;
    auto mView = rs->GetView();
    mView.Transposition();
    mView.Get3X3(&mView3x3);

    CVECTOR raydir;
    CVECTOR rayorig;
    raydir = mView3x3 * v;
    rayorig = mView.Pos();

    raystart = rayorig;
    rayend = (rayorig + (raydir * 5000.f));
}

void CoastFoam::Realize(uint32_t Delta_Time)
{
    pFrustumPlanes = rs->GetPlanes();

    if (pSea == nullptr)
    {
        pSea = static_cast<SEA_BASE *>(core.GetEntityPointer(core.GetEntityId("sea")));
        if (pSea == nullptr)
            return;
    }

    auto fDeltaTime = static_cast<float>(Delta_Time) * 0.001f;
    D3DVIEWPORT9 vp;
    rs->GetViewport(&vp);

    CMatrix mWorld;
    mWorld.SetIdentity(); // rs->GetWorld();
    auto mView = rs->GetView();
    auto mProjection = rs->GetProjection();

    CMatrix mWV, mWVP;
    mWV.EqMultiply(mWorld, mView);
    mWVP.EqMultiply(mWV, mProjection);

    uint64_t dw1;
    dwNumPenasExecuted = 0;
    RDTSC_B(dw1);
    for (int32_t i = 0; i < aFoams.size(); i++)
    {
        auto pF = aFoams[i];

        if (pF->Type == FOAM_TYPE_1)
            ExecuteFoamType1(pF, fDeltaTime);
        else
            ExecuteFoamType2(pF, fDeltaTime);
    }
    RDTSC_E(dw1);

    // rs->Print(0, 300, "dwNumPenasExecuted = %d, ticks = %d", dwNumPenasExecuted, dw1);

    if (!bCanEdit)
        return;

    std::vector<RS_LINE> aLines;
    std::vector<RS_RECT> aRects;

    if (bEditMode)
        for (int32_t i = 0; i < aFoams.size(); i++)
        {
            aLines.clear();
            aRects.clear();

            auto pF = aFoams[i];
            for (int32_t j = 0; j < pF->aFoamParts.size(); j++)
            {
                RS_LINE rl, r2;
                rl.dwColor = 0xFFFFFFFF;
                r2.dwColor = 0xFFFFFFFF;

                rl.vPos = pF->aFoamParts[j].v[0];
                r2.vPos = pF->aFoamParts[j].v[1];
                aLines.push_back(rl);
                aLines.push_back(r2);

                if (j != pF->aFoamParts.size() - 1)
                {
                    rl.vPos = pF->aFoamParts[j].v[0];
                    r2.vPos = pF->aFoamParts[j + 1].v[0];
                    aLines.push_back(rl);
                    aLines.push_back(r2);

                    rl.vPos = pF->aFoamParts[j].v[1];
                    r2.vPos = pF->aFoamParts[j + 1].v[1];
                    aLines.push_back(rl);
                    aLines.push_back(r2);
                }

                rl.vPos = pF->aFoamParts[j].v[0];
                r2.vPos = pF->aFoamParts[j].v[1];
                aLines.push_back(rl);
                aLines.push_back(r2);

                if (bEditMode)
                {
                    auto v1 = pF->aFoamParts[j].v[0];
                    auto v2 = pF->aFoamParts[j].v[1];
                    MTX_PRJ_VECTOR vP1, vP2;
                    mWVP.Projection(&v1, &vP1, 1, static_cast<float>(vp.Width) * 0.5f,
                                    static_cast<float>(vp.Height) * 0.5f, sizeof(CVECTOR), sizeof(MTX_PRJ_VECTOR));
                    mWVP.Projection(&v2, &vP2, 1, static_cast<float>(vp.Width) * 0.5f,
                                    static_cast<float>(vp.Height) * 0.5f, sizeof(CVECTOR), sizeof(MTX_PRJ_VECTOR));
                    auto fR1 = 0.15f;
                    auto fR2 = 0.15f;
                    if (SQR(vP1.x - fCursorPosX) + SQR(vP1.y - fCursorPosY) < 100.0f)
                        fR1 = 0.3f;
                    if (SQR(vP2.x - fCursorPosX) + SQR(vP2.y - fCursorPosY) < 100.0f)
                        fR2 = 0.3f;

                    RS_RECT r1, r2;
                    r1.vPos = v1;
                    r1.dwColor = 0xFFFFFF00;
                    r1.fAngle = 0.0f;
                    r1.fSize = fR1;
                    r2.vPos = v2;
                    r2.dwColor = 0xFF00FF00;
                    r2.fAngle = 0.0f;
                    r2.fSize = fR2;
                    aRects.push_back(r1);
                    aRects.push_back(r2);
                    // RS_RECT & r1 = aRects[aRects.Add()];
                    // r1.vPos = v1; r1.dwColor = 0xFFFFFF00; r1.fAngle = 0.0f; r1.fSize = fR1;
                    // RS_RECT & r2 = aRects[aRects.Add()];
                    // r2.vPos = v2; r2.dwColor = 0xFF00FF00; r2.fAngle = 0.0f; r2.fSize = fR2;
                }
            }

            CMatrix mI;
            mI.SetIdentity();
            rs->SetWorld(mI);
            if (bEditMode)
                rs->DrawLines(aLines.data(), aLines.size() / 2, "Line");

            rs->DrawRects(aRects.data(), aRects.size(), "FoamPoints");
        }

    auto bShift = core.Controls->GetDebugAsyncKeyState(VK_SHIFT) < 0;
    auto bMenu = core.Controls->GetDebugAsyncKeyState(VK_MENU) < 0;
    if (bShift && core.Controls->GetDebugAsyncKeyState('L') < 0)
        Load();
    if (bShift && core.Controls->GetDebugAsyncKeyState('S') < 0)
        Save();

    if (bEditMode)
    {
        RS_SPRITE spr[4];

        auto fMinDistance = 1e10f;
        if (!bMoved)
        {
            iEditFoam = -1;
            iEditFoamPart = -1;
            iEditFoamVertex = -1;
            for (int32_t i = 0; i < aFoams.size(); i++)
            {
                auto *pF = aFoams[i];
                for (int32_t j = 0; j < pF->aFoamParts.size(); j++)
                {
                    for (int32_t k = 0; k < 2; k++)
                    {
                        MTX_PRJ_VECTOR vP;
                        auto v = pF->aFoamParts[j].v[k];

                        mWVP.Projection(&v, &vP, 1, static_cast<float>(vp.Width) * 0.5f,
                                        static_cast<float>(vp.Height) * 0.5f, sizeof(CVECTOR), sizeof(MTX_PRJ_VECTOR));
                        auto fD = SQR(vP.x - fCursorPosX) + SQR(vP.y - fCursorPosY);
                        if (fD < 100.0f && fD < fMinDistance)
                        {
                            fMinDistance = fD;

                            iEditFoam = i;
                            iEditFoamPart = j;
                            iEditFoamVertex = k;
                        }
                    }
                }
            }
        }
        auto x = fCursorPosX, dx = 32.0f;
        auto y = fCursorPosY, dy = 32.0f;
        FillSpriteVertex(spr[0], x, y, 0.1f, 0xFFFFFFFF, 0.0f, 0.0f);
        FillSpriteVertex(spr[1], x, y + dy, 0.1f, 0xFFFFFFFF, 0.0f, 1.0f);
        FillSpriteVertex(spr[2], x + dx, y + dy, 0.1f, 0xFFFFFFFF, 1.0f, 1.0f);
        FillSpriteVertex(spr[3], x + dx, y, 0.1f, 0xFFFFFFFF, 1.0f, 0.0f);

        rs->TextureSet(0, iCursorTex);
        rs->DrawSprites(spr, 1, "Sprite");

        auto bSelected = iEditFoam >= 0 && iEditFoamPart >= 0 && iEditFoamVertex >= 0;

        CONTROL_STATE cs, csH, csV;

        core.Controls->GetControlState("Turn H", csH);
        fCursorPosX += csH.lValue;
        core.Controls->GetControlState("Turn V", csV);
        fCursorPosY += csV.lValue;

        fCursorPosX = Max(0.0f, Min(fCursorPosX, static_cast<float>(vp.Width)));
        fCursorPosY = Max(0.0f, Min(fCursorPosY, static_cast<float>(vp.Height)));

        core.Controls->GetControlState("CoastFoamLB", cs);

        if (cs.state == CST_ACTIVE && !bMoved)
        {
            bMoved = true;
        }

        if (cs.state == CST_INACTIVE && bMoved)
        {
            bMoved = false;
        }
        CVECTOR vStart, vEnd;
        ExtractRay(vp, fCursorPosX, fCursorPosY, vStart, vEnd);

        if (bMoved && (csH.lValue || csV.lValue))
        {
            if (bSelected && (vStart.y > 0.0f && vStart.y > vEnd.y))
            {
                auto *pFP = &aFoams[iEditFoam]->aFoamParts[iEditFoamPart];
                Plane pln(Vector(0.0f, 1.0f, 0.0f), Vector(0.0f));
                auto vS1 = Vector(vStart.x, vStart.y, vStart.z);
                auto vE1 = Vector(vEnd.x, vEnd.y, vEnd.z);
                Vector vR1;
                if (pln.Intersection(vS1, vE1, vR1))
                {
                    auto vDiff = pFP->v[1] - pFP->v[0];
                    auto vNewPos = CVECTOR(vR1.x, vR1.y, vR1.z);
                    auto vDir = vNewPos - pFP->v[iEditFoamVertex];
                    if (bMenu)
                    {
                        for (int32_t i = 0; i < aFoams[iEditFoam]->aFoamParts.size(); i++)
                        {
                            aFoams[iEditFoam]->aFoamParts[i].v[0] += vDir;
                            aFoams[iEditFoam]->aFoamParts[i].v[1] += vDir;
                        }
                    }
                    else
                    {
                        pFP->v[iEditFoamVertex] = vNewPos;

                        if (iEditFoamVertex == 0 && bShift)
                            pFP->v[1] = pFP->v[0] + vDiff;
                    }

                    RecalculateFoam(iEditFoam);
                }
            }
        }

        CONTROL_STATE csIns, csDel, csCopy;
        core.Controls->GetControlState("CoastFoamINS", csIns);
        core.Controls->GetControlState("CoastFoamDEL", csDel);
        core.Controls->GetControlState("CoastFoamCopy", csCopy);

        if (bShift && csCopy.state == CST_ACTIVATED && bSelected)
        {
            auto *pF = new Foam();
            *pF = *aFoams[iEditFoam];

            for (int32_t i = 0; i < pF->aFoamParts.size(); i++)
            {
                pF->aFoamParts[i].v[0] += CVECTOR(10.0f, 0.0f, 10.0f);
                pF->aFoamParts[i].v[1] += CVECTOR(10.0f, 0.0f, 10.0f);
            }

            // RecalculateFoam( aFoams.Add(pF) );
            aFoams.push_back(pF);
            RecalculateFoam(aFoams.size() - 1);
        }

        if (csIns.state == CST_ACTIVATED)
        {
            if (bShift)
            {
                // insert new foam
                auto mIView = rs->GetView();
                mIView.Transposition();
                auto *pF = new Foam();

                InitNewFoam(pF);

                auto vPos = mIView.Pos();
                vPos.y = 0.0f;
                auto vZ = mIView.Vz();
                vZ.y = 0.0;
                vZ = !vZ;
                auto vX = mIView.Vx();
                vX.y = 0.0;
                vX = !vX;

                FoamPart foam1, foam2;
                foam1.v[0] = vPos;
                foam1.v[1] = vPos + vZ * 6.0f;
                foam2.v[0] = vPos + vX * 3.0f;
                foam2.v[1] = vPos + vX * 3.0f + vZ * 6.0f;

                pF->aFoamParts.emplace_back(foam1);
                pF->aFoamParts.emplace_back(foam2);

                // RecalculateFoam( aFoams.Add(pF) );
                aFoams.push_back(pF);
                RecalculateFoam(aFoams.size() - 1);
            }
            else if (bSelected)
            {
                auto *pF = aFoams[iEditFoam];
                if (iEditFoamPart == pF->aFoamParts.size() - 1)
                {
                    const auto foam1 = pF->aFoamParts.end() - 2, foam2 = pF->aFoamParts.end() - 1;

                    FoamPart foam;
                    foam.v[0] = 2.0f * foam2->v[0] - foam1->v[0];
                    foam.v[1] = 2.0f * foam2->v[1] - foam1->v[1];
                    pF->aFoamParts.push_back(foam);

                    // FoamPart * pFP = &pF->aFoamParts[pF->aFoamParts.Add()];
                    // FoamPart * pFP1 = &pF->aFoamParts[pF->aFoamParts.Last() - 2];
                    // FoamPart * pFP2 = &pF->aFoamParts[pF->aFoamParts.Last() - 1];
                    // pFP->v[0] = 2.0f * pFP2->v[0] - pFP1->v[0];
                    // pFP->v[1] = 2.0f * pFP2->v[1] - pFP1->v[1];
                }
                else
                {
                    pF->aFoamParts.insert(pF->aFoamParts.begin() + iEditFoamPart, FoamPart{});
                    pF->aFoamParts[iEditFoamPart] = pF->aFoamParts[iEditFoamPart + 1];
                    pF->aFoamParts[iEditFoamPart + 1].v[0] =
                        0.5f * (pF->aFoamParts[iEditFoamPart].v[0] + pF->aFoamParts[iEditFoamPart + 2].v[0]);
                    pF->aFoamParts[iEditFoamPart + 1].v[1] =
                        0.5f * (pF->aFoamParts[iEditFoamPart].v[1] + pF->aFoamParts[iEditFoamPart + 2].v[1]);
                }
                RecalculateFoam(iEditFoam);
            }
        }

        if (csDel.state == CST_ACTIVATED && bSelected)
        {
            if (bShift)
                aFoams.erase(aFoams.begin() + iEditFoam);
            else
            {
                aFoams[iEditFoam]->aFoamParts.erase(aFoams[iEditFoam]->aFoamParts.begin() + iEditFoamPart);
                if (aFoams[iEditFoam]->aFoamParts.size() <= 1)
                    aFoams.erase(aFoams.begin() + iEditFoam);
                else
                    RecalculateFoam(iEditFoam);
            }
        }
    }
}

void CoastFoam::InitNewFoam(Foam *pF)
{
    pF->fAlphaMin = 148.0f;
    pF->fAlphaMax = 196.0f;
    pF->fAppearMax = 0.0f;
    pF->fAppearMin = 0.0f;
    pF->fSpeedMin = 0.2f;
    pF->fSpeedMax = 0.25f;
    pF->fTexScaleX = 0.1f;
    pF->Type = FOAM_TYPE_2;
    pF->sTexture = "foam.tga";
    pF->iNumFoams = 2;

    pF->iTexture = rs->TextureCreate(("weather\\coastfoam\\" + pF->sTexture).c_str());
}

void CoastFoam::ExecuteFoamType2(Foam *pF, float fDeltaTime)
{
    const int32_t iLen = pF->aWorkParts.size();
    if (!iLen)
        return;

    CVECTOR vCamPos, vCamAng;
    float fPerspective;
    rs->GetCamera(vCamPos, vCamAng, fPerspective);

    const auto fDistance = sqrtf(~(pF->aWorkParts[0].v[0] - vCamPos));
    if (fDistance > fMaxFoamDistance)
        return;

    if (IsClipped(pF))
        return;

    dwNumPenasExecuted++;

    for (int32_t k = 0; k < pF->iNumFoams; k++)
    {
        const int32_t kk = (k == 1) ? 0 : 1;

        if (pF->fMove[k] < 0.0f)
        {
            pF->fMove[k] += fDeltaTime;
            if (pF->fMove[k] < 0.0f)
                continue;
            pF->fMove[k] = 0.0f;
        }

        switch (pF->iMode[k])
        {
        case 0:
            pF->fAlpha[k] = Clampf(pF->fAlpha[k] + fDeltaTime * 0.15f);
            if (pF->fMove[k] > 0.6f)
                pF->iMode[k] = 1;
            break;
        case 1:
            pF->fAlpha[k] = Clampf(pF->fAlpha[k] + fDeltaTime * 0.15f);
            pF->fSpeed[k] *= (1.0f - Clampf(fDeltaTime * 0.8f));
            if (pF->fSpeed[k] < 0.01f && pF->fMove[kk] < 0.0f)
            {
                pF->fSX[kk] = Rnd(100.0f);
                pF->fAlpha[kk] = 0.0f;
                pF->iMode[kk] = 0;
                pF->fSpeed[kk] = RRnd(pF->fSpeedMin, pF->fSpeedMax);
                pF->fMove[kk] = 0.0f;
                pF->fAlphaColor[k] = RRnd(pF->fAlphaMin, pF->fAlphaMax) / 255.0f;
            }
            if (pF->fSpeed[k] < 0.01f)
            {
                pF->iMode[k] = 2;
            }
            break;
        case 2:
            pF->fSpeed[k] -= fDeltaTime * 0.035f;
            pF->fSpeed[k] = Max(-0.04f, pF->fSpeed[k]);
            pF->fAlpha[k] = Clampf(pF->fAlpha[k] - fDeltaTime * 0.2f);
            if (pF->fAlpha[k] <= 0.0f)
            {
                // pF->fMove[k] = -10000.0f;
                pF->fSX[k] = Rnd(100.0f);
                pF->fAlpha[k] = 0.0f;
                pF->iMode[k] = 0;
                pF->fSpeed[k] = RRnd(pF->fSpeedMin, pF->fSpeedMax);
                pF->fMove[k] = -RRnd(pF->fAppearMin, pF->fAppearMax);
                pF->fAlphaColor[k] = RRnd(pF->fAlphaMin, pF->fAlphaMax) / 255.0f;
            }
            break;
        }

        pF->fMove[k] += pF->fSpeed[k] * fDeltaTime;

        auto *pFV = static_cast<FoamVertex *>(rs->LockVertexBuffer(iVBuffer, D3DLOCK_DISCARD));
        auto *pI = static_cast<uint16_t *>(rs->LockIndexBuffer(iIBuffer, D3DLOCK_DISCARD));

        int32_t iNumVertices = 0;

        for (int32_t y = 0; y < 8; y++)
        {
            const auto dy = y / 7.0f;
            const auto fAlpha = pF->fAlpha[k] * Clampf(2.5f * (1.0f - dy) * dy);
            for (int32_t x = 0; x < iLen; x++)
            {
                auto *pWP = &pF->aWorkParts[x];

                auto fAlpha1 = 1.0f;
                if (x <= 4)
                    fAlpha1 = static_cast<float>(x) / 4.0f;
                if (x >= iLen - 5)
                    fAlpha1 = static_cast<float>((iLen - 1) - x) / 4.0f;
                const auto dwColor =
                    ARGB(static_cast<uint32_t>(pF->fAlphaColor[k] * fAlpha * fAlpha1 * 255.0f), 255, 255, 255);

                auto vPos = pWP->v[0] + (pF->fMove[k] * static_cast<float>(y) / 7.0f) * (pWP->v[1] - pWP->v[0]);
                vPos.y = fFoamDeltaY + pSea->WaveXZ(vPos.x, vPos.z);
                // vPos.y += 3.0f * fAmp * sinf(float(y) / 7.0f * PI);
                pFV[x + y * iLen].vPos = vPos;
                pFV[x + y * iLen].dwColor = dwColor;
                pFV[x + y * iLen].tu = pF->fSX[k] + pWP->tu;
                pFV[x + y * iLen].tv = 1.0f - dy;
                iNumVertices++;
            }
        }

        // setup ibuffer
        for (int32_t y = 0; y < 7; y++)
        {
            for (int32_t x = 0; x < iLen - 1; x++)
            {
                *pI++ = static_cast<uint16_t>((y + 0) * iLen + x);
                *pI++ = static_cast<uint16_t>((y + 1) * iLen + x);
                *pI++ = static_cast<uint16_t>((y + 0) * iLen + x + 1);

                *pI++ = static_cast<uint16_t>((y + 1) * iLen + x);
                *pI++ = static_cast<uint16_t>((y + 1) * iLen + x + 1);
                *pI++ = static_cast<uint16_t>((y + 0) * iLen + x + 1);
            }
        }

        rs->UnLockIndexBuffer(iIBuffer);
        rs->UnLockVertexBuffer(iVBuffer);

        CMatrix mI;
        mI.SetIdentity();
        rs->SetWorld(mI);
        rs->TextureSet(0, pF->iTexture);
        rs->DrawBuffer(iVBuffer, sizeof(FoamVertex), iIBuffer, 0, iNumVertices, 0, 7 * 2 * (iLen - 1), "CoastFoam");
    }
}

bool CoastFoam::IsClipped(Foam *pF)
{
    if (!pF)
        return true;

    float fPerspective;
    CVECTOR vCamPos, vCamAng;
    rs->GetCamera(vCamPos, vCamAng, fPerspective);

    CVECTOR vP[4];
    uint32_t dwPlanesPoints[4];
    const uint32_t dwSize = pF->aFoamParts.size();

    for (uint32_t k = 0; k < 4; k++)
    {
        dwPlanesPoints[k] = 0;
        vP[k] = CVECTOR(pFrustumPlanes[k].Nx, pFrustumPlanes[k].Ny, pFrustumPlanes[k].Nz);
    }

    for (uint32_t i = 0; i < dwSize; i++)
    {
        auto v0 = pF->aFoamParts[i].v[0];
        auto v1 = pF->aFoamParts[i].v[1];

        for (uint32_t j = 0; j < 4; j++)
        {
            const auto fD1 = (v0 | vP[j]) - pFrustumPlanes[j].D;
            const auto fD2 = (v1 | vP[j]) - pFrustumPlanes[j].D;

            if (fD1 < 0.0f)
                dwPlanesPoints[j]++;
            if (fD2 < 0.0f)
                dwPlanesPoints[j]++;
        }
    }

    for (uint32_t z = 0; z < 4; z++)
        if (dwPlanesPoints[z] == dwSize * 2)
            return true;

    return false;
}

void CoastFoam::ExecuteFoamType1(Foam *pF, float fDeltaTime)
{
    const int32_t iLen = pF->aWorkParts.size();
    if (!iLen)
        return;

    CVECTOR vCamPos, vCamAng;
    float fPerspective;
    rs->GetCamera(vCamPos, vCamAng, fPerspective);

    const auto fDistance = sqrtf(~(pF->aWorkParts[0].v[0] - vCamPos));
    if (fDistance > fMaxFoamDistance)
        return;

    if (IsClipped(pF))
        return;

    auto *pFV = static_cast<FoamVertex *>(rs->LockVertexBuffer(iVBuffer, D3DLOCK_DISCARD));
    auto *pI = static_cast<uint16_t *>(rs->LockIndexBuffer(iIBuffer, D3DLOCK_DISCARD));

    int32_t iNumVertices = 0;

    dwNumPenasExecuted++;

    for (int32_t y = 0; y < 8; y++)
    {
        const auto dy = y / 7.0f;
        const auto fAlpha = Clampf(2.5f * (1.0f - dy) * dy);
        auto dwColor = ARGB(static_cast<uint32_t>(fAlpha * 255.0f), 255, 255, 255);
        for (int32_t x = 0; x < iLen; x++)
        {
            auto *pWP = &pF->aWorkParts[x];

            pWP->p[y].fPos += pWP->p[y].fSpeed * fDeltaTime * 0.1f;

            auto fAlpha1 = fAlpha;
            if (pWP->p[y].fPos > 0.9f)
            {
                fAlpha1 = fAlpha * Clampf(1.0f - pWP->p[y].fPos) / 0.1f;
            }
            if (x <= 4)
                fAlpha1 *= static_cast<float>(x) / 4.0f;

            if (x >= iLen - 4)
                fAlpha1 *= static_cast<float>((iLen - 1) - x) / 4.0f;

            const auto dwColor = ARGB(static_cast<uint32_t>(fAlpha1 * 255.0f), 255, 255, 255);

            auto fAmp = (1.0f - pWP->p[y].fPos) / 4.0f;
            if (y == 0 && pWP->p[y].fPos >= 0.6f)
            {
                // pWP->p[y].fSpeed *= 0.95f;
                for (int32_t k = 0; k < 8; k++)
                    pWP->p[k].fSpeed *= (1.0f - Clampf(fDeltaTime * 2.0f));
            }

            auto vPos = pWP->v[0] + pWP->p[y].fPos * (pWP->v[1] - pWP->v[0]);
            vPos.y = fFoamDeltaY + pSea->WaveXZ(vPos.x, vPos.z);
            // vPos.y += 3.0f * fAmp * sinf(float(y) / 7.0f * PI);
            pFV[x + y * iLen].vPos = vPos;
            pFV[x + y * iLen].dwColor = dwColor;
            pFV[x + y * iLen].tu = pWP->tu;
            pFV[x + y * iLen].tv = dy * 2.0f;
            iNumVertices++;
        }
    }

    // setup ibuffer
    for (int32_t y = 0; y < 7; y++)
    {
        for (int32_t x = 0; x < iLen - 1; x++)
        {
            *pI++ = static_cast<uint16_t>((y + 0) * iLen + x);
            *pI++ = static_cast<uint16_t>((y + 1) * iLen + x);
            *pI++ = static_cast<uint16_t>((y + 0) * iLen + x + 1);

            *pI++ = static_cast<uint16_t>((y + 1) * iLen + x);
            *pI++ = static_cast<uint16_t>((y + 1) * iLen + x + 1);
            *pI++ = static_cast<uint16_t>((y + 0) * iLen + x + 1);
        }
    }

    rs->UnLockIndexBuffer(iIBuffer);
    rs->UnLockVertexBuffer(iVBuffer);

    CMatrix mI;
    mI.SetIdentity();
    rs->SetWorld(mI);
    rs->TextureSet(0, pF->iTexture);
    rs->DrawBuffer(iVBuffer, sizeof(FoamVertex), iIBuffer, 0, iNumVertices, 0, 7 * 2 * (iLen - 1), "CoastFoam");
}

void CoastFoam::RecalculateFoam(int32_t iFoam)
{
    auto *pF = aFoams[iFoam];

    pF->aWorkParts.clear();

    pF->fMove[0] = -RRnd(pF->fAppearMin, pF->fAppearMax);
    pF->fMove[1] = -100000.0f; // RRnd(pF->fAppearMin, pF->fAppearMax);
    pF->fSpeed[0] = RRnd(pF->fSpeedMin, pF->fSpeedMax);
    pF->fSpeed[1] = RRnd(pF->fSpeedMin, pF->fSpeedMax);

    pF->fAlphaColor[0] = RRnd(pF->fAlphaMin, pF->fAlphaMax) / 255.0f;
    pF->fAlphaColor[1] = RRnd(pF->fAlphaMin, pF->fAlphaMax) / 255.0f;

    pF->fSX[0] = Rnd(100.0f);
    pF->fSX[1] = Rnd(100.0f);
    pF->fAlpha[0] = 0.0f;
    pF->fAlpha[1] = 0.0f;
    pF->iMode[0] = 0;
    pF->iMode[1] = 0;

    auto sx = 0.0f;
    auto ii = 0.0f;

    if (!pF->aFoamParts.empty())
    {
        for (size_t i = 0; i < pF->aFoamParts.size() - 1; i++)
        {
            auto *pF1 = &pF->aFoamParts[i];
            auto *pF2 = &pF->aFoamParts[i + 1];

            const auto dx = (pF1->v[0] - pF2->v[0]).GetLength() / static_cast<float>(iFoamDivides);
            for (int32_t j = 0; j < iFoamDivides; j++)
            {
                if (j == 0 && i != 0)
                    continue;

                // WorkPart * pWP = &pF->aWorkParts[pF->aWorkParts.Add()];
                pF->aWorkParts.push_back(WorkPart{});
                auto *pWP = &pF->aWorkParts.back();
                pWP->tu = sx * pF->fTexScaleX;
                pWP->v[0] =
                    pF1->v[1] + static_cast<float>(j) / static_cast<float>(iFoamDivides - 1) * (pF2->v[1] - pF1->v[1]);
                pWP->v[1] =
                    pF1->v[0] + static_cast<float>(j) / static_cast<float>(iFoamDivides - 1) * (pF2->v[0] - pF1->v[0]);

                const auto fStartPos = sinf(ii / 14.0f * PI) * 0.1f;
                for (int32_t k = 0; k < 8; k++)
                {
                    pWP->p[k].fPos = fStartPos + (static_cast<float>(k) / 7.0f) * 0.4f;
                    pWP->p[k].fSpeed = 2.0f; // RRnd(pF->fSpeedMin, pF->fSpeedMax);
                    // pWP->p[k].fA = 0.0f;
                }

                sx += dx;
                ii++;
            }
        }
    }
}

void CoastFoam::Save()
{
    if (!bCanEdit)
        return;

    char cKey[128], cSection[128], cTemp[1024];
    const auto sID = std::string("resource\\foam\\locations\\") + to_string(AttributesPointer->GetAttribute("id")) + ".ini";
    fio->_DeleteFile(sID.c_str());

    auto pI = fio->CreateIniFile(sID.c_str(), false);
    if (!pI)
        return;

    pI->WriteLong(nullptr, "NumFoams", aFoams.size());
    pI->WriteDouble(nullptr, "MaxFoamDistance", fMaxFoamDistance);
    pI->WriteDouble(nullptr, "FoamDeltaY", fFoamDeltaY);
    pI->WriteLong(nullptr, "FoamDivides", iFoamDivides);

    for (int32_t i = 0; i < aFoams.size(); i++)
    {
        auto *pF = aFoams[i];
        sprintf_s(cSection, "foam_%d", i);
        pI->WriteLong(cSection, "NumParts", pF->aFoamParts.size());

        sprintf_s(cTemp, "%.0f, %.0f", pF->fAlphaMin, pF->fAlphaMax);
        pI->WriteString(cSection, "Alpha", cTemp);

        sprintf_s(cTemp, "%.3f, %.3f", pF->fSpeedMin, pF->fSpeedMax);
        pI->WriteString(cSection, "Speed", cTemp);

        sprintf_s(cTemp, "%.3f, %.3f", pF->fBrakingMin, pF->fBrakingMax);
        pI->WriteString(cSection, "Braking", cTemp);

        sprintf_s(cTemp, "%.3f, %.3f", pF->fAppearMin, pF->fAppearMax);
        pI->WriteString(cSection, "Appear", cTemp);

        sprintf_s(cTemp, "%.3f", pF->fTexScaleX);
        pI->WriteString(cSection, "TexScaleX", cTemp);

        pI->WriteLong(cSection, "NumFoams", pF->iNumFoams);

        pI->WriteString(cSection, "Texture", (char *)pF->sTexture.c_str());
        pI->WriteLong(cSection, "Type", pF->Type);

        for (int32_t j = 0; j < pF->aFoamParts.size(); j++)
        {
            auto *pFP = &pF->aFoamParts[j];
            sprintf_s(cKey, "key_%d", j);
            sprintf_s(cTemp, "%.4f, %.4f, %.4f, %.4f", pFP->v[0].x, pFP->v[0].z, pFP->v[1].x, pFP->v[1].z);
            pI->WriteString(cSection, cKey, cTemp);
        }
    }
#ifdef _WIN32 // FIX_LINUX _flushall
    _flushall();
#endif
}

void CoastFoam::clear()
{
    for (auto &foam : aFoams)
    {
        if (foam->iTexture >= 0)
            rs->TextureRelease(foam->iTexture);
        delete foam;
    }
}

void CoastFoam::Load()
{
    char cSection[256], cKey[256], cTemp[1024];

    const auto sID = std::string("resource\\foam\\locations\\") + to_string(AttributesPointer->GetAttribute("id")) + ".ini";
    auto pI = fio->OpenIniFile(sID.c_str());
    if (!pI)
        return;

    clear();
    const auto iNumFoams = pI->GetInt(nullptr, "NumFoams", 0);
    fMaxFoamDistance = pI->GetFloat(nullptr, "MaxFoamDistance", 1000.0f);
    fFoamDeltaY = pI->GetFloat(nullptr, "FoamDeltaY", 0.2f);
    iFoamDivides = pI->GetInt(nullptr, "FoamDivides", 4);
    for (int32_t i = 0; i < iNumFoams; i++)
    {
        // Foam * pF = aFoams[aFoams.Add(new Foam)];
        aFoams.push_back(new Foam);
        auto *pF = aFoams.back();

        sprintf_s(cSection, "foam_%d", i);

        const int32_t iNumParts = pI->GetInt(cSection, "NumParts", 0);

        pI->ReadString(cSection, "Alpha", cTemp, sizeof(cTemp), "148, 196");
        sscanf(cTemp, "%f, %f", &pF->fAlphaMin, &pF->fAlphaMax);

        pI->ReadString(cSection, "Speed", cTemp, sizeof(cTemp), "0.200, 0.30");
        sscanf(cTemp, "%f, %f", &pF->fSpeedMin, &pF->fSpeedMax);

        pI->ReadString(cSection, "Braking", cTemp, sizeof(cTemp), "0.000, 0.000");
        sscanf(cTemp, "%f, %f", &pF->fBrakingMin, &pF->fBrakingMax);

        pI->ReadString(cSection, "Appear", cTemp, sizeof(cTemp), "0.000, 0.200");
        sscanf(cTemp, "%f, %f", &pF->fAppearMin, &pF->fAppearMax);

        pI->ReadString(cSection, "TexScaleX", cTemp, sizeof(cTemp), "0.050");
        sscanf(cTemp, "%f", &pF->fTexScaleX);

        pF->iNumFoams = (pI->GetInt(cSection, "NumFoams", 2) == 2) ? 2 : 1;

        pI->ReadString(cSection, "Texture", cTemp, sizeof(cTemp), "foam.tga");
        pF->sTexture = cTemp;
        pF->iTexture = rs->TextureCreate((std::string("weather\\coastfoam\\") + cTemp).c_str());
        pF->Type = static_cast<FOAMTYPE>(pI->GetInt(cSection, "Type", FOAM_TYPE_2));

        for (int32_t j = 0; j < ((iNumParts) ? iNumParts : 100000); j++)
        {
            sprintf_s(cKey, "key_%d", j);
            CVECTOR v1, v2;
            v1.y = v2.y = 0.0f;
            pI->ReadString(cSection, cKey, cTemp, sizeof(cTemp), "");
            if (!cTemp[0])
                break;
            sscanf(cTemp, "%f, %f, %f, %f", &v1.x, &v1.z, &v2.x, &v2.z);

            FoamPart foam;
            foam.v[0] = v1;
            foam.v[1] = v2;
            pF->aFoamParts.push_back(foam);
            // pF->aFoamParts.Add();
            // pF->aFoamParts.LastE().v[0] = v1;
            // pF->aFoamParts.LastE().v[1] = v2;
        }
        pF->fMove[0] = -RRnd(pF->fAppearMin, pF->fAppearMax);
        pF->fMove[1] = -100000.0f; // RRnd(pF->fAppearMin, pF->fAppearMax);
        pF->fSpeed[0] = RRnd(pF->fSpeedMin, pF->fSpeedMax);
        pF->fSpeed[1] = RRnd(pF->fSpeedMin, pF->fSpeedMax);
        pF->fSX[0] = Rnd(100.0f);
        pF->fSX[1] = Rnd(100.0f);
        // pF->fBraking[0] = RRnd(pF->fBrakingMin, pF->fBrakingMax);
        // pF->fBraking[1] = RRnd(pF->fBrakingMin, pF->fBrakingMax);
        pF->fAlphaColor[0] = RRnd(pF->fAlphaMin, pF->fAlphaMax) / 255.0f;
        pF->fAlphaColor[1] = RRnd(pF->fAlphaMin, pF->fAlphaMax) / 255.0f;
        pF->fAlpha[0] = 0.0f;
        pF->fAlpha[1] = 0.0f;
        pF->iMode[0] = 0;
        pF->iMode[1] = 0;

        RecalculateFoam(i);
    }
}

uint32_t CoastFoam::AttributeChanged(ATTRIBUTES *pA)
{
    return 0;
}
