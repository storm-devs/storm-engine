#include "SKY.h"
#include "SunGlow.h"
#include "weather_base.h"

SKY::SKY() : aSkyDirArray(_FL)
{
    iSkyVertsID = -1;
    iSkyIndexID = -1;
    iFogVertsID = -1;
    iFogIndexID = -1;
    fAngleY = 0.0f;
    pRS = null;
    memset(TexturesID, -1, sizeof(TexturesID));
    memset(TexturesNextID, -1, sizeof(TexturesNextID));
    pAstronomy = 0;
    pSunGlow = 0;
}

SKY::~SKY()
{
    Release();
}

void SKY::Release()
{
    for (long i = 0; i < SKY_NUM_TEXTURES; i++)
    {
        if (TexturesID[i] >= 0)
        {
            pRS->TextureRelease(TexturesID[i]);
            TexturesID[i] = -1;
        }
        if (TexturesNextID[i] >= 0)
        {
            pRS->TextureRelease(TexturesNextID[i]);
            TexturesNextID[i] = -1;
        }
    }

    if (iSkyVertsID >= 0)
        pRS->ReleaseVertexBuffer(iSkyVertsID);
    iSkyVertsID = -1;
    if (iSkyIndexID >= 0)
        pRS->ReleaseIndexBuffer(iSkyIndexID);
    iSkyIndexID = -1;

    if (iFogVertsID >= 0)
        pRS->ReleaseVertexBuffer(iFogVertsID);
    iFogVertsID = -1;
    if (iFogIndexID >= 0)
        pRS->ReleaseIndexBuffer(iFogIndexID);
    iFogIndexID = -1;
}

bool SKY::Init()
{
    SetDevice();
    return true;
}

void SKY::SetDevice()
{
    pRS = (VDX8RENDER *)_CORE_API->CreateService("dx8render");
    Assert(pRS);
}

dword SKY::CalcFogDiffuse(CVECTOR &vPos)
{
    float fAlpha = vPos.y / 500.0f;
    fAlpha = CLAMP(fAlpha);
    dword dwAlpha = 255 - dword(fAlpha * 255.0f);
    return ARGB(dwAlpha, 167, 153, 196);
}

void SKY::CreateFogSphere()
{
    long x, y, iNumLevels, iNumAngles, idx = 0;

    float R = 2000.0f;
    iNumLevels = 64;
    iNumAngles = 8;
    iFogNumVerts = iNumAngles * iNumLevels + 1;
    FOGVERTEX *pVerts = NEW FOGVERTEX[iFogNumVerts];
    iFogVertsID =
        pRS->CreateVertexBufferManaged(FOGVERTEX_FORMAT, iFogNumVerts * sizeof(SKYVERTEX), D3DUSAGE_WRITEONLY);
    FOGVERTEX *pVertBuf = (FOGVERTEX *)pRS->LockVertexBuffer(iFogVertsID);

    iFogNumTrgs = 3 * (iNumAngles + (iNumLevels - 1) * iNumAngles * 2);
    iFogIndexID = pRS->CreateIndexBufferManaged(iFogNumTrgs * 2);

    WORD *pTrgs = (WORD *)pRS->LockIndexBuffer(iFogIndexID);
    for (y = 0; y < iNumLevels; y++)
    {
        float h = y * R / iNumLevels;
        float R1 = sqrtf(SQR(R) - SQR(h));
        for (x = 0; x < iNumAngles; x++)
        {
            float fCos = cosf(float(x) / float(iNumAngles) * PIm2);
            float fSin = sinf(float(x) / float(iNumAngles) * PIm2);
            CVECTOR vPos = CVECTOR(R1 * fCos, h, R1 * fSin);
            pVerts[idx].pos = vPos;
            VDATA *pvData = api->Event(WEATHER_CALC_FOG_COLOR, "fff", vPos.x, vPos.y, vPos.z);
            Assert(pvData);
            pVerts[idx].diffuse = (dword)pvData->GetLong();
            // pVerts[idx].diffuse = CalcFogDiffuse(pVerts[idx].pos);

            if (y == iNumLevels - 1)
            {
                *pTrgs++ = WORD(iFogNumVerts - 1);
                *pTrgs++ = WORD(idx);
                *pTrgs++ = WORD((x == iNumAngles - 1) ? idx - (iNumAngles - 1) : idx + 1);
            }
            else
            {
                *pTrgs++ = WORD(idx);
                *pTrgs++ = WORD((x == iNumAngles - 1) ? idx - (iNumAngles - 1) : idx + 1);
                *pTrgs++ = WORD(idx + iNumAngles);

                *pTrgs++ = WORD((x == iNumAngles - 1) ? idx - (iNumAngles - 1) : idx + 1);
                *pTrgs++ = WORD(idx + iNumAngles);
                *pTrgs++ = WORD((x == iNumAngles - 1) ? idx + 1 : idx + iNumAngles + 1);
            }
            idx++;
        }
    }
    CVECTOR vPos = CVECTOR(0.0f, R, 0.0f);
    pVerts[idx].pos = vPos;
    VDATA *pvData = api->Event(WEATHER_CALC_FOG_COLOR, "fff", vPos.x, vPos.y, vPos.z);
    Assert(pvData);
    pVerts[idx].diffuse = (dword)pvData->GetLong();
    if (pVerts)
        memcpy(pVertBuf, pVerts, iFogNumVerts * sizeof(FOGVERTEX));
    pRS->UnLockVertexBuffer(iFogVertsID);
    pRS->UnLockIndexBuffer(iFogIndexID);

    delete pVerts;
}

void SKY::UpdateFogSphere()
{
    if (iFogVertsID == -1)
        return;

    long x, y, iNumLevels, iNumAngles, idx = 0;

    CVECTOR vPos;
    float R = 2000.0f;
    iNumLevels = 64;
    iNumAngles = 8;
    iFogNumVerts = iNumAngles * iNumLevels + 1;
    FOGVERTEX *pVertBuf = (FOGVERTEX *)pRS->LockVertexBuffer(iFogVertsID);

    for (y = 0; y < iNumLevels; y++)
    {
        float h = y * R / iNumLevels;
        float R1 = sqrtf(SQR(R) - SQR(h));
        for (x = 0; x < iNumAngles; x++)
        {
            float fCos = cosf(float(x) / float(iNumAngles) * PIm2);
            float fSin = sinf(float(x) / float(iNumAngles) * PIm2);
            vPos = CVECTOR(R1 * fCos, h, R1 * fSin);
            pVertBuf[idx].pos = vPos;
            VDATA *pvData = api->Event(WEATHER_CALC_FOG_COLOR, "fff", vPos.x, vPos.y, vPos.z);
            Assert(pvData);
            pVertBuf[idx].diffuse = (dword)pvData->GetLong();

            idx++;
        }
    }
    vPos = CVECTOR(0.0f, R, 0.0f);
    pVertBuf[idx].pos = vPos;
    VDATA *pvData = api->Event(WEATHER_CALC_FOG_COLOR, "fff", vPos.x, vPos.y, vPos.z);
    Assert(pvData);
    pVertBuf[idx].diffuse = (dword)pvData->GetLong();
    pRS->UnLockVertexBuffer(iFogVertsID);
}

void SKY::GenerateSky()
{
    SKYVERTEX Verts[SKY_NUM_VERTEX];
    long i;
    SKYVERTEX v[8];

    Release();

    v[0].pos = CVECTOR(-1.0f, 0.0f, -1.0f);
    v[1].pos = CVECTOR(1.0f, 0.0f, -1.0f);
    v[2].pos = CVECTOR(1.0f, 0.0f, 1.0f);
    v[3].pos = CVECTOR(-1.0f, 0.0f, 1.0f);
    v[4].pos = CVECTOR(-1.0f, 1.0f, -1.0f);
    v[5].pos = CVECTOR(1.0f, 1.0f, -1.0f);
    v[6].pos = CVECTOR(1.0f, 1.0f, 1.0f);
    v[7].pos = CVECTOR(-1.0f, 1.0f, 1.0f);

    float fpdelta = 1.0f / 1024.0f;
    float fpdx = 1.0f - fpdelta; //.25f - fpdelta;
    float fp0 = fpdelta;
    float fp1 = 0.25f + fpdelta;
    float fp2 = 0.5f + fpdelta;
    float fp3 = 0.75f + fpdelta;
    float fp4 = 1.0f - fpdelta;

    Verts[0] = v[1];
    Verts[0].tu = fp4;
    Verts[0].tv = fp0 + fpdx;
    Verts[1] = v[5];
    Verts[1].tu = fp4;
    Verts[1].tv = fp0;
    Verts[2] = v[4];
    Verts[2].tu = fp0;
    Verts[2].tv = fp0;
    Verts[3] = v[0];
    Verts[3].tu = fp0;
    Verts[3].tv = fp0 + fpdx;

    Verts[4] = v[2];
    Verts[4].tu = fp4;
    Verts[4].tv = fp0 + fpdx;
    Verts[5] = v[6];
    Verts[5].tu = fp4;
    Verts[5].tv = fp0;
    Verts[6] = v[5];
    Verts[6].tu = fp0;
    Verts[6].tv = fp0;
    Verts[7] = v[1];
    Verts[7].tu = fp0;
    Verts[7].tv = fp0 + fpdx;

    Verts[8] = v[3];
    Verts[8].tu = fp4;
    Verts[8].tv = fp0 + fpdx;
    Verts[9] = v[7];
    Verts[9].tu = fp4;
    Verts[9].tv = fp0;
    Verts[10] = v[6];
    Verts[10].tu = fp0;
    Verts[10].tv = fp0;
    Verts[11] = v[2];
    Verts[11].tu = fp0;
    Verts[11].tv = fp0 + fpdx;

    Verts[12] = v[0];
    Verts[12].tu = fp4;
    Verts[12].tv = fp0 + fpdx;
    Verts[13] = v[4];
    Verts[13].tu = fp4;
    Verts[13].tv = fp0;
    Verts[14] = v[7];
    Verts[14].tu = fp0;
    Verts[14].tv = fp0;
    Verts[15] = v[3];
    Verts[15].tu = fp0;
    Verts[15].tv = fp0 + fpdx;

    Verts[16] = v[5];
    Verts[16].tu = fp0;
    Verts[16].tv = fp0;
    Verts[17] = v[6];
    Verts[17].tu = fp0;
    Verts[17].tv = fp4;
    Verts[18] = v[7];
    Verts[18].tu = fp4;
    Verts[18].tv = fp4;
    Verts[19] = v[4];
    Verts[19].tu = fp4;
    Verts[19].tv = fp0;

    for (i = 0; i < 20; i++)
    {
        Verts[i].diffuse = 0xFFFFFFFF;
        Verts[i].pos *= fSkySize;
        Verts[i + 20] = Verts[i];
        Verts[i + 20].pos.y = -Verts[i + 20].pos.y;
        Verts[i].tu2 = Verts[i].tu;
        Verts[i].tv2 = Verts[i].tv;
    }
    iSkyVertsID =
        pRS->CreateVertexBufferManaged(SKYVERTEX_FORMAT, SKY_NUM_VERTEX * sizeof(SKYVERTEX), D3DUSAGE_WRITEONLY);
    iSkyIndexID = pRS->CreateIndexBufferManaged(20 * 3 * 2);
    SKYVERTEX *pVertBuf = (SKYVERTEX *)pRS->LockVertexBuffer(iSkyVertsID);
    if (pVertBuf)
        memcpy(pVertBuf, &Verts[0], sizeof(Verts));
    pRS->UnLockVertexBuffer(iSkyVertsID);

    WORD *pTrgs = (WORD *)pRS->LockIndexBuffer(iSkyIndexID);
    for (i = 0; i < 10; i++)
    {
        *pTrgs++ = (word)(i * 4) + 0;
        *pTrgs++ = (word)(i * 4) + 1;
        *pTrgs++ = (word)(i * 4) + 2;
        *pTrgs++ = (word)(i * 4) + 0;
        *pTrgs++ = (word)(i * 4) + 2;
        *pTrgs++ = (word)(i * 4) + 3;
    }
    pRS->UnLockIndexBuffer(iSkyIndexID);

    LoadTextures();
    CreateFogSphere();
}

void SKY::LoadTextures()
{
    char str[256], *names[SKY_NUM_TEXTURES] = {"sky_fr.tga", "sky_lf.tga", "sky_bk.tga", "sky_rt.tga", "sky_up.tga"};

    string sSkyDir, sSkyDirNext;
    GetSkyDirStrings(sSkyDir, sSkyDirNext);

    for (long i = 0; i < SKY_NUM_TEXTURES; i++)
    {
        sprintf(str, "%s%s", (const char *)sSkyDir, names[i]);
        TexturesID[i] = pRS->TextureCreate(str);

        if (aSkyDirArray.Size() > 1)
        {
            sprintf(str, "%s%s", (const char *)sSkyDirNext, names[i]);
            TexturesNextID[i] = pRS->TextureCreate(str);
        }
    }

    fAngleY = 0.0f;
}

bool SKY::CreateState(ENTITY_STATE_GEN *state_gen)
{
    return true;
}

bool SKY::LoadState(ENTITY_STATE *state)
{
    SetDevice();
    return true;
}

void SKY::Realize(dword Delta_Time)
{
    fAngleY += float(Delta_Time) * 0.001f * fSkySpeedRotate;

    float fFov;
    CVECTOR vPos, vAng;
    D3DXMATRIX pMatWorld, pMatTranslate, pMatRotate;

    pRS->GetCamera(vPos, vAng, fFov);

    D3DXMatrixIdentity(&pMatWorld);

    D3DXMatrixTranslation(&pMatTranslate, vPos.x, vPos.y / 6.0f, vPos.z);
    D3DXMatrixMultiply(&pMatWorld, &pMatWorld, &pMatTranslate);
    D3DXMatrixRotationY(&pMatRotate, fAngleY + fSkyAngle);
    D3DXMatrixMultiply(&pMatWorld, &pMatRotate, &pMatWorld);
    pRS->SetTransform(D3DTS_WORLD, &pMatWorld);

    if (aSkyDirArray.Size() > 1)
    {
        UpdateTimeFactor();
        float fBlendFactor = fTimeFactor - (long)fTimeFactor;
        if (fBlendFactor < 0.f)
            fBlendFactor = 0.f;
        if (fBlendFactor > 1.f)
            fBlendFactor = 1.f;
        dword dwColor = (dwSkyColor & 0x00FFFFFF) | (((long)(0xFF000000 * fBlendFactor)) & 0xFF000000);
        pRS->SetRenderState(D3DRS_TEXTUREFACTOR, dwColor);

        if (pRS->TechniqueExecuteStart("skyblend"))
            do
            {
                for (long i = 0; i < SKY_NUM_TEXTURES; i++)
                {
                    pRS->TextureSet(0, TexturesID[i]);
                    pRS->TextureSet(1, TexturesNextID[i]);
                    pRS->DrawBuffer(iSkyVertsID, sizeof(SKYVERTEX), iSkyIndexID, 0, 20, i * 6, 2);
                    if (Delta_Time == 0)
                        pRS->DrawBuffer(iSkyVertsID, sizeof(SKYVERTEX), iSkyIndexID, 0, 40, 30 + i * 6, 2);
                }
            } while (pRS->TechniqueExecuteNext());

        if (Delta_Time != 0)
        {
            ENTITY_ID eid;

            if (!pAstronomy)
                if (api->FindClass(&eid, "Astronomy", 0))
                    pAstronomy = (ENTITY *)eid.pointer;

            if (!pSunGlow)
                if (api->FindClass(&eid, "SUNGLOW", 0))
                    pSunGlow = (ENTITY *)eid.pointer;

            if (pAstronomy || pSunGlow)
            {
                if (pAstronomy)
                    pAstronomy->Realize(Delta_Time);
                if (pSunGlow)
                    ((SUNGLOW *)pSunGlow)->DrawSunMoon();

                pRS->SetTransform(D3DTS_WORLD, &pMatWorld);
                if (pRS->TechniqueExecuteStart("skyblend_alpha"))
                    do
                    {
                        for (long i = 0; i < SKY_NUM_TEXTURES; i++)
                        {
                            pRS->TextureSet(0, TexturesID[i]);
                            pRS->TextureSet(1, TexturesNextID[i]);
                            pRS->DrawBuffer(iSkyVertsID, sizeof(SKYVERTEX), iSkyIndexID, 0, 20, i * 6, 2);
                            if (Delta_Time == 0)
                                pRS->DrawBuffer(iSkyVertsID, sizeof(SKYVERTEX), iSkyIndexID, 0, 40, 30 + i * 6, 2);
                        }
                    } while (pRS->TechniqueExecuteNext());
            }
        }

        string sSkyPrev, sSkyNext;
        GetSkyDirStrings(sSkyPrev, sSkyNext);
        // debug pRS->Print( 10,10, "time = %.2f(%.2f), skydir = %s,%s", fTimeFactor,fBlendFactor, (const
        // char*)sSkyPrev,(const char*)sSkyNext);
    }
    else
    {
        pRS->SetRenderState(D3DRS_TEXTUREFACTOR, dwSkyColor);

        if (pRS->TechniqueExecuteStart("sky"))
            do
            {
                for (long i = 0; i < SKY_NUM_TEXTURES; i++)
                {
                    pRS->TextureSet(0, TexturesID[i]);
                    pRS->DrawBuffer(iSkyVertsID, sizeof(SKYVERTEX), iSkyIndexID, 0, 20, i * 6, 2);
                    if (Delta_Time == 0)
                        pRS->DrawBuffer(iSkyVertsID, sizeof(SKYVERTEX), iSkyIndexID, 0, 40, 30 + i * 6, 2);
                }
            } while (pRS->TechniqueExecuteNext());
    }

    D3DXMatrixIdentity(&pMatWorld);
    D3DXMatrixTranslation(&pMatTranslate, vPos.x, vPos.y / 6.0f, vPos.z);
    D3DXMatrixMultiply(&pMatWorld, &pMatWorld, &pMatTranslate);
    pRS->SetTransform(D3DTS_WORLD, &pMatWorld);
    pRS->DrawBuffer(iFogVertsID, sizeof(FOGVERTEX), iFogIndexID, 0, iFogNumVerts, 0, iFogNumTrgs / 3, "SkyFog");
}

dword SKY::AttributeChanged(ATTRIBUTES *pAttribute)
{
    // if (*pAttribute == "dir")			{ sSkyDir = pAttribute->GetThisAttr(); return 0; }
    if (*pAttribute == "dir")
    {
        FillSkyDirArray(pAttribute);
        return 0;
    }
    if (*pAttribute == "color")
    {
        dwSkyColor = pAttribute->GetAttributeAsDword();
        return 0;
    }
    if (*pAttribute == "RotateSpeed")
    {
        fSkySpeedRotate = pAttribute->GetAttributeAsFloat();
        return 0;
    }
    if (*pAttribute == "Size")
    {
        fSkySize = pAttribute->GetAttributeAsFloat();
        return 0;
    }
    if (*pAttribute == "Angle")
    {
        fSkyAngle = pAttribute->GetAttributeAsFloat();
        return 0;
    }

    if (*pAttribute == "isDone")
    {
        GenerateSky();
        return 0;
    }

    if (*pAttribute == "TimeUpdate")
    {
        UpdateFogSphere();
        return 0;
    }

    return 0;
}

dword _cdecl SKY::ProcessMessage(MESSAGE &message)
{
    if (message.Long() == MSG_SEA_REFLECTION_DRAW)
        Realize(0);
    return 0;
}

void SKY::FillSkyDirArray(ATTRIBUTES *pAttribute)
{
    aSkyDirArray.DelAll();
    long q = pAttribute->GetAttributesNum();
    if (q < 1)
        return;

    aSkyDirArray.AddElements(q);
    if (q > 1)
    {
        for (long n = 0; n < q; n++)
        {
            char *attrName = pAttribute->GetAttributeName(n);
            if (attrName && attrName[0] == 'd' && attrName[1] >= '0' && attrName[1] <= '9')
            {
                long i = atol(&attrName[1]);
                if (i < q)
                    aSkyDirArray[i] = pAttribute->GetAttribute(n);
            }
        }
    }
    else
    {
        aSkyDirArray[0] = pAttribute->GetAttribute((DWORD)0);
    }
    fTimeFactor = (float)atof(pAttribute->GetThisAttr());
    if (fTimeFactor < 0.f || fTimeFactor > 24.f)
        fTimeFactor = 0.f;
    fTimeFactor *= (1.f / 24.f) * aSkyDirArray.Size();
}

void SKY::GetSkyDirStrings(string &sSkyDir, string &sSkyDirNext)
{
    long n1 = (long)fTimeFactor;
    if (n1 >= 0)
    {
        if (n1 >= (long)aSkyDirArray)
            n1 -= aSkyDirArray.Size();
        long n2 = n1 + 1;
        if (n2 >= (long)aSkyDirArray)
            n2 -= aSkyDirArray.Size();

        sSkyDir = aSkyDirArray[n1];
        sSkyDirNext = aSkyDirArray[n2];
        return;
    }
    sSkyDir = sSkyDirNext = "";
}

void SKY::UpdateTimeFactor()
{
    long nPrev = (long)fTimeFactor;

    // fTimeFactor += api->GetDeltaTime() * 0.00005f;
    ENTITY_ID eid;
    if (!api->FindClass(&eid, "Weather", 0))
        return;
    fTimeFactor = ((WEATHER_BASE *)eid.pointer)->GetFloat(whf_time_counter);
    fTimeFactor *= (1.f / 24.f) * aSkyDirArray.Size();

    if ((long)fTimeFactor >= (long)aSkyDirArray)
        fTimeFactor -= aSkyDirArray.Size();
    long nNext = (long)fTimeFactor;
    if (nPrev != nNext)
    {
        // перечитываем текстуры

        char str[256],
            *names[SKY_NUM_TEXTURES] = {"sky_fr.tga", "sky_lf.tga", "sky_bk.tga", "sky_rt.tga", "sky_up.tga"};

        string sSkyDir, sSkyDirNext;
        GetSkyDirStrings(sSkyDir, sSkyDirNext);

        for (long i = 0; i < SKY_NUM_TEXTURES; i++)
        {
            if (TexturesID[i] >= 0)
                pRS->TextureRelease(TexturesID[i]);
            TexturesID[i] = TexturesNextID[i];

            sprintf(str, "%s%s", (const char *)sSkyDirNext, names[i]);
            TexturesNextID[i] = pRS->TextureCreate(str);
        }
    }
}

float SKY::CalculateAlphaForSun(const CVECTOR &vSunPos, float fSunSize)
{
    // get Sky
    float fFov;
    CVECTOR vPos, vAng;
    pRS->GetCamera(vPos, vAng, fFov);

    CMatrix mtxWorld;
    mtxWorld.BuildRotateY(fAngleY + fSkyAngle);
    mtxWorld.Move(vPos.x, vPos.y / 6.0f, vPos.z);

    CVECTOR vLocalSunPos;
    mtxWorld.MulToInv(vSunPos, vLocalSunPos);

    if (vLocalSunPos.y < 0.f)
        vLocalSunPos.y = 0.f;

    // считаем куда мы попадаем
    if (fSkySize > 0.f)
    {
        long nTexNum = -1;
        float fu, fv, fk;

        // проверим попадание в верхнюю сторону
        if (vLocalSunPos.y > 0.f)
        {
            fk = fSkySize / vLocalSunPos.y;
            fu = -vLocalSunPos.x * fk;
            fv = vLocalSunPos.z * fk;

            if (fu <= fSkySize && fu >= -fSkySize && fv <= fSkySize && fv >= -fSkySize)
            {
                nTexNum = 4;
                fu = (fu + fSkySize) * 0.5f / fSkySize;
                fv = (fv + fSkySize) * 0.5f / fSkySize;
            }
        }

        // проверим попадание в переднюю сторону
        if (nTexNum == -1)
        {
            if (vLocalSunPos.z < 0.f)
            {
                fk = -fSkySize / vLocalSunPos.z;
                fu = vLocalSunPos.x * fk;
                fv = vLocalSunPos.y * fk;

                if (fu <= fSkySize && fu >= -fSkySize && fv <= fSkySize && fv >= 0.f)
                {
                    nTexNum = 0;
                    fu = (fu + fSkySize) * 0.5f / fSkySize;
                    fv = (fSkySize - fv) / fSkySize;
                }
            }
        }

        // проверим попадание в заднюю сторону
        if (nTexNum == -1)
        {
            if (vLocalSunPos.z > 0.f)
            {
                fk = fSkySize / vLocalSunPos.z;
                fu = -vLocalSunPos.x * fk;
                fv = vLocalSunPos.y * fk;

                if (fu <= fSkySize && fu >= -fSkySize && fv <= fSkySize && fv >= 0.f)
                {
                    nTexNum = 2;
                    fu = (fu + fSkySize) * 0.5f / fSkySize;
                    fv = (fSkySize - fv) / fSkySize;
                }
            }
        }

        // проверим попадание в левую сторону
        if (nTexNum == -1)
        {
            if (vLocalSunPos.x > 0.f)
            {
                fk = fSkySize / vLocalSunPos.x;
                fu = vLocalSunPos.z * fk;
                fv = vLocalSunPos.y * fk;

                if (fu <= fSkySize && fu >= -fSkySize && fv <= fSkySize && fv >= 0.f)
                {
                    nTexNum = 1;
                    fu = (fSkySize + fu) * 0.5f / fSkySize;
                    fv = (fSkySize - fv) / fSkySize;
                }
            }
        }

        // проверим попадание в правую сторону
        if (nTexNum == -1)
        {
            if (vLocalSunPos.x < 0.f)
            {
                fk = -fSkySize / vLocalSunPos.x;
                fu = -vLocalSunPos.z * fk;
                fv = vLocalSunPos.y * fk;

                if (fu <= fSkySize && fu >= -fSkySize && fv <= fSkySize && fv >= 0.f)
                {
                    nTexNum = 3;
                    fu = (fSkySize + fu) * 0.5f / fSkySize;
                    fv = (fSkySize - fv) / fSkySize;
                }
            }
        }

        // ищем альфу в текстуре
        if (nTexNum != -1)
        {
            DWORD dwCol1 = GetPixelColor((IDirect3DTexture9 *)pRS->GetTextureFromID(TexturesID[nTexNum]), fu, fv);
            DWORD dwCol2 = GetPixelColor((IDirect3DTexture9 *)pRS->GetTextureFromID(TexturesNextID[nTexNum]), fu, fv);

            float fK = fTimeFactor - (long)fTimeFactor;
            float fAlpha = (1.f - fK) * (dwCol1 >> 24) / 255.f + fK * (dwCol2 >> 24) / 255.f;
            return fAlpha;
        }
    }

    return 1.f;
}

DWORD SKY::GetPixelColor(IDirect3DTexture9 *pTex, float fu, float fv)
{
    HRESULT hok;
    DWORD dwCol = 0xFFFFFFFF;

    if (!pTex)
        return dwCol;

    D3DSURFACE_DESC texdesc;
    pRS->GetLevelDesc(pTex, 0, &texdesc);
    long x = (long)(texdesc.Width * fu);
    long y = (long)(texdesc.Height * fv);

    D3DLOCKED_RECT lockRect;
    if ((hok = pRS->LockRect(pTex, 0, &lockRect, 0, D3DLOCK_READONLY)) == D3D_OK)
    {
        DWORD *pLine = (DWORD *)((DWORD)lockRect.pBits + y * lockRect.Pitch);
        dwCol = pLine[x];
        pRS->UnlockRect(pTex, 0);
    }

    return dwCol;
}