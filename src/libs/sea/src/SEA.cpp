#include "sea.h"

#include "core.h"

#include "SSE.h"
#include "inlines.h"
#include "shared/sea_ai/Script_defines.h"
#include "tga.h"
#include "vfile_service.h"

CREATE_CLASS(SEA)

//#define OLD_WORLD_POS

#define NUM_VERTEXS 65500
#define NUM_INDICES 165000

#define FRAMES 64
#define XWIDTH 128
#define YWIDTH 128

#define MIPSLVLS 4

#define GC_CONSTANT 0 // Global constants = {0.0, 1.0, 0.5, 0.0000152590218967 = (0.5 / 32767.5)}
#define GC_CONSTANT2 1 // Global constants 2 = {2.0, -1.0, 0.00036621652552071 = (12 / 32767.5), fog}
#define GC_ANIMATION 2 // Animation frames(0.0-1.0) for 4 stages = {stage0, stage1, stage2, stage3}
#define GC_SHADOW_CONST1 3
#define GC_SHADOW_CONST2 7
#define GC_LIGHT4 11 //
#define GC_LIGHT3 13
#define GC_LIGHT2 15
#define GC_LIGHT1 17
#define GC_LIGHT0 19
#define GC_MATERIAL 21 //
#define GC_CAMERA_POS 23 // Local Camera position = {x, y, z, 0.0}
#define GC_MTX_WVP 24 // c[0] = mWorld * mView * mProjection

#define GC_FREE 28

SEA *SEA::pSea = nullptr;
bool SEA::bIntel = false;
bool SEA::bSSE = false;
bool SEA::bHyperThreading = false;
bool SEA::bSeaDebug = false;
IDirect3DVertexDeclaration9 *SEA::vertexDecl_ = nullptr;

SEA::SEA()
{
    aBlocks.reserve(128);
    aSeaTrash.reserve(512);
    aTrashRects.reserve(512);
    aSeaLights.reserve(512);
    aLightsRects.reserve(512);
    aBumpMaps.reserve(FRAMES);
    aNormals.reserve(FRAMES);

    dwMaxDim = 65536 * 2;
    dwMinDim = 128;

    fMaxSeaHeight = 20.0f;
    fGridStep = 0.06f;
    fLodScale = 0.4f;

    fAmp1 = 1.0f;
    fFrame1 = 0.0f;
    fAnimSpeed1 = 1.0f;
    vMove1 = 0.0f;
    fScale1 = 0.4f;

    fAmp2 = 1.0f;
    fFrame2 = 10.0f;
    fAnimSpeed2 = 1.0f;
    vMove2 = 0.0f;
    fScale2 = 2.0f;

    fFoamV = 3.0f;
    fFoamK = 0.0f;
    fFoamUV = 3.0f;
    fFoamTextureDisturb = 0.7f;
    iFoamTexture = -1;
    bIniFoamEnable = false;
    bFoamEnable = false;

    iSeaLightTexture = -1;
    fLastLightTime = 0.0f;

    fLastTrashTime = 0.0f;
    iSeaTrashTexture = -1;
    bUnderSea = false;
    bUnderSeaStarted = false;
    bUnderSeaEnable = false;
    bStarted = false;
    bSimpleSea = false;

    fPosShift = 2.0f;

    fFogSeaDensity = 0.0f;
    fFogStartDistance = 0.0f;
    vFogColor = 0.0f;
    bFogEnable = false;

    pSeaFrame1 = new float[XWIDTH * YWIDTH];
    pSeaFrame2 = new float[XWIDTH * YWIDTH];
    pSeaNormalsFrame1 = new float[2 * XWIDTH * YWIDTH];
    pSeaNormalsFrame2 = new float[2 * XWIDTH * YWIDTH];

    iVSeaBuffer = -1;
    iISeaBuffer = -1;

    bTempFullMode = false;

    // HyperThreading section
    hEventCalcMaps = CreateEvent(nullptr, false, false, nullptr);

    InitializeCriticalSection(&cs);
    InitializeCriticalSection(&cs1);

    pSea = this;
    bIntel = intel.IsIntelCPU();
    bSSE = intel.IsSSE();

    bStop = false;

    vWorldOffset = 0.f;

    pRenderTargetBumpMap = nullptr;
}

SEA::~SEA()
{
    DeleteCriticalSection(&cs);
    DeleteCriticalSection(&cs1);

    for (long i = 0; i < aThreads.size(); i++)
    {
        if (aThreads[i])
            TerminateThread(aThreads[i], 0);
        if (aEventCalcBlock[i])
            CloseHandle(aEventCalcBlock[i]);
    }

    if (hEventCalcMaps)
        CloseHandle(hEventCalcMaps);

    rs->Release(pReflection);
    rs->Release(pReflectionSunroad);
    rs->Release(pEnvMap);
    rs->Release(pSunRoadMap);
    rs->Release(pZStencil);
    rs->Release(pReflectionSurfaceDepth);
    rs->Release(pVolumeTexture);
    rs->Release(pRenderTargetBumpMap);

    rs->TextureRelease(iSeaTrashTexture);
    rs->TextureRelease(iSeaLightTexture);

    if (iVSeaBuffer >= 0)
        rs->ReleaseVertexBuffer(iVSeaBuffer);
    iVSeaBuffer = -1;
    if (iISeaBuffer >= 0)
        rs->ReleaseIndexBuffer(iISeaBuffer);
    iISeaBuffer = -1;

    if (iFoamTexture >= 0)
        rs->TextureRelease(iFoamTexture);
    iFoamTexture = -1;

    STORM_DELETE(pIndices);
    STORM_DELETE(pVSea);

    for (long i = 0; i < aBumpMaps.size(); i++)
        rs->Release(aBumpMaps[i]);

    for (long i = 0; i < aBumps.size(); i++)
        STORM_DELETE(aBumps[i]);
    for (long i = 0; i < aNormals.size(); i++)
        STORM_DELETE(aNormals[i]);

    STORM_DELETE(pSeaFrame1);
    STORM_DELETE(pSeaFrame2);
    STORM_DELETE(pSeaNormalsFrame1);
    STORM_DELETE(pSeaNormalsFrame2);
}

void SEA::SFLB_CreateBuffers()
{
    iVSeaBuffer = rs->CreateVertexBuffer(0, NUM_VERTEXS * sizeof(SeaVertex), D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC);
    iISeaBuffer = rs->CreateIndexBuffer(NUM_INDICES * 3 * sizeof(uint16_t), D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC);

    pIndices = new uint32_t[NUM_VERTEXS * 3];
    pVSea = new SeaVertex[NUM_VERTEXS];
}

void SEA::CreateVertexDeclaration()
{
    if (vertexDecl_ != nullptr)
        return;

    const D3DVERTEXELEMENT9 VertexElements[] = {
        {0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
        {0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0},
        {0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
        D3DDECL_END()};

    rs->CreateVertexDeclaration(VertexElements, &vertexDecl_);
}

bool SEA::Init()
{
    rs = static_cast<VDX9RENDER *>(core.CreateService("dx9render"));
    CreateVertexDeclaration();
    auto bEnableSSE = false;
    {
        auto pEngineIni = fio->OpenIniFile(core.EngineIniFileName());
        bHyperThreading = (pEngineIni) ? pEngineIni->GetLong(nullptr, "HyperThreading", 1) != 0 : false;
        bIniFoamEnable = (pEngineIni) ? pEngineIni->GetLong("Sea", "FoamEnable", 1) != 0 : false;
        bEnableSSE = (pEngineIni) ? pEngineIni->GetLong(nullptr, "EnableSSE", 0) != 0 : false; // boal
        bSeaDebug = (pEngineIni) ? pEngineIni->GetLong("Sea", "SeaDebug", 1) != 0 : false;
    }
    if (bEnableSSE)
    {
        bSSE = true; // boal
    }

    if (!bIntel)
        bHyperThreading = false;

    if (bHyperThreading)
    {
        uint32_t dwLogicals, dwCores, dwPhysicals;
        intel.CPUCount(&dwLogicals, &dwCores, &dwPhysicals);
        core.Trace("Total logical: %d, Total cores: %d, Total physical: %d", dwLogicals, dwCores, dwPhysicals);

        const auto dwNumThreads = dwLogicals * dwCores - 1;

        for (size_t i = 0; i < dwNumThreads; i++)
        {
            // HANDLE & hEvent = aEventCalcBlock[aEventCalcBlock.Add()];
            // hEvent = CreateEvent(null, false, false, null);
            aEventCalcBlock.push_back(CreateEvent(nullptr, false, false, nullptr));

            aThreads.push_back(HANDLE{});
            auto &hThread = aThreads.back();
            hThread = CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(ThreadExecute),
                                   reinterpret_cast<void *>(i), CREATE_SUSPENDED, nullptr);
            SetThreadPriority(hThread, THREAD_PRIORITY_NORMAL);
            ResumeThread(hThread);

            aThreadsTest.push_back(long{});
        }

        bHyperThreading = dwNumThreads > 0;
    }

    iFoamTexture = rs->TextureCreate("weather\\sea\\pena\\pena.tga");

    rs->CreateTexture(XWIDTH, YWIDTH, MIPSLVLS, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT,
                      &pRenderTargetBumpMap);

    SFLB_CreateBuffers();

    pVolumeTexture = rs->CreateVolumeTexture(XWIDTH, YWIDTH, FRAMES, 4, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED);
    /*pAnimTexture = rs->CreateAnimationTexture(pVolumeTexture, _FL_);
    pAnimTexture->SetAnimSpeed(20.0f);*/

    rs->CreateCubeTexture(128, 1, D3DUSAGE_RENDERTARGET, D3DFMT_R5G6B5, D3DPOOL_DEFAULT, &pEnvMap);
    rs->CreateCubeTexture(128, 1, D3DUSAGE_RENDERTARGET, D3DFMT_R5G6B5, D3DPOOL_DEFAULT, &pSunRoadMap);
    rs->CreateDepthStencilSurface(128, 128, D3DFMT_D24S8, D3DMULTISAMPLE_NONE, &pZStencil);

    rs->CreateTexture(128, 128, 1, D3DUSAGE_RENDERTARGET, D3DFMT_R5G6B5, D3DPOOL_DEFAULT, &pReflection);
    rs->CreateTexture(128, 128, 1, D3DUSAGE_RENDERTARGET, D3DFMT_R5G6B5, D3DPOOL_DEFAULT, &pReflectionSunroad);
    rs->CreateDepthStencilSurface(128, 128, D3DFMT_D24S8, D3DMULTISAMPLE_NONE, &pReflectionSurfaceDepth);

    iSeaTrashTexture = rs->TextureCreate("seatrash.tga");
    iSeaLightTexture = rs->TextureCreate("sealight.tga");

    uint8_t bMin = 0xFF;
    uint8_t bMax = 0;

    std::vector<uint8_t *> aTmpBumps;

    uint32_t i;

    for (i = 0; i < FRAMES; i++)
    {
        char str[256];
        char *pFBuffer = nullptr;
        uint32_t dwSize;
        sprintf_s(str, "resource\\sea\\sea%.4d.tga", i);
        // sprintf_s(str, "resource\\sea\\sea0000.tga", i);
        fio->LoadFile(str, &pFBuffer, &dwSize);
        if (!pFBuffer)
        {
            core.Trace("Sea: Can't load %s", str);
            return false;
        }

        auto *pFB = pFBuffer + sizeof(TGA_H);

        auto *pBuffer = new uint8_t[XWIDTH * YWIDTH];
        aTmpBumps.push_back(pBuffer);

        for (uint32_t y = 0; y < YWIDTH; y++)
            for (uint32_t x = 0; x < XWIDTH; x++)
            {
                const uint8_t bB = (*pFB);
                // bB = byte(float(bB - 79.0f) * 255.0f / (139.0f - 79.0f));
                if (bB < bMin)
                    bMin = bB;
                if (bB > bMax)
                    bMax = bB;
                pBuffer[x + y * XWIDTH] = bB & 0xFF;
                pFB += sizeof(uint32_t);
            }

        STORM_DELETE(pFBuffer);
    }

    for (i = 0; i < FRAMES; i++)
    {
        auto *pBuffer = new uint8_t[XWIDTH * YWIDTH];
        aBumps.push_back(pBuffer);

        for (uint32_t y = 0; y < YWIDTH; y++)
            for (uint32_t x = 0; x < XWIDTH; x++)
            {
                const auto dwAddress = x + y * YWIDTH;
                float b1, b2, b3, b4, b5; // -2 -1 0 1 2

                b1 = 0.08f * static_cast<float>(aTmpBumps[(i - 2) & (FRAMES - 1)][dwAddress]);
                b2 = 0.17f * static_cast<float>(aTmpBumps[(i - 1) & (FRAMES - 1)][dwAddress]);
                b3 = 0.50f * static_cast<float>(aTmpBumps[(i - 0) & (FRAMES - 1)][dwAddress]);
                b4 = 0.17f * static_cast<float>(aTmpBumps[(i + 1) & (FRAMES - 1)][dwAddress]);
                b5 = 0.08f * static_cast<float>(aTmpBumps[(i + 2) & (FRAMES - 1)][dwAddress]);

                pBuffer[dwAddress] = aTmpBumps[(i - 0) & (FRAMES - 1)][dwAddress];
            }
    }

    for (i = 0; i < aTmpBumps.size(); i++)
        STORM_DELETE(aTmpBumps[i]);

    BuildVolumeTexture();

    EditMode_Update();

    core.Trace("Intel CPU: %s, SSE: %s, HyperThreading: %s", (bIntel) ? "Yes" : "No", (bSSE) ? "On" : "Off",
               (bHyperThreading) ? "On" : "Off");

    return true;
}

void SEA::BuildVolumeTexture()
{
    std::vector<CVECTOR *> aVectors;
    uint32_t i, j;

    for (const auto &normal : aNormals)
        delete normal;
    aNormals.clear();

    D3DLOCKED_BOX box[4];

    if (pVolumeTexture)
        for (i = 0; i < 4; i++)
            pVolumeTexture->LockBox(i, &box[i], nullptr, 0);

    for (i = 0; i < aBumpMaps.size(); i++)
        rs->Release(aBumpMaps[i]);
    aBumpMaps.clear();

    uint32_t dwTexelSize = 4;
    auto *pDst = static_cast<char *>(new char[XWIDTH * YWIDTH * dwTexelSize]);

    // build normals

    aVectors.reserve(FRAMES);
    for (i = 0; i < FRAMES; i++)
    {
        auto pBuffer = new uint32_t[XWIDTH * YWIDTH];
        aNormals.push_back(pBuffer);

        auto *pVectors = new CVECTOR[XWIDTH * YWIDTH];
        aVectors.push_back(pVectors);

        for (uint32_t y = 0; y < YWIDTH; y++)
            for (uint32_t x = 0; x < XWIDTH; x++)
            {
#define GET_MASSIVE(dx, dy)                                                                                            \
    (float(pMassive[((x + dx) & (XWIDTH - 1)) + ((y + dy) & (XWIDTH - 1)) * XWIDTH] & 0xFF) / 255.0f)
                uint8_t *pMassive = aBumps[i];

                float fCenter = GET_MASSIVE(0, 0);

                float fLeft = GET_MASSIVE(-1, 0) - fCenter;
                float fRight = GET_MASSIVE(1, 0) - fCenter;
                float fTop = GET_MASSIVE(0, -1) - fCenter;
                float fBottom = GET_MASSIVE(0, 1) - fCenter;

                CVECTOR vRes, vRes1, d0, d1, d2, d3;

                uint32_t dwNums = 0;
                if (fLeft < 0.0f)
                    dwNums++;
                if (fRight < 0.0f)
                    dwNums++;
                if (fTop < 0.0f)
                    dwNums++;
                if (fBottom < 0.0f)
                    dwNums++;

                float d = 1.0f;
                d0 = CVECTOR(-1.f, d * fLeft, 0.f);
                d1 = CVECTOR(0.f, d * fTop, -1.f);
                d2 = CVECTOR(1.f, d * fRight, 0.f);
                d3 = CVECTOR(0.f, d * fBottom, 1.f);

                // res = !((d0^d1) + (d2^d3));

                CVECTOR v1 = (d1 ^ d0);
                CVECTOR v2 = (d3 ^ d2);
                CVECTOR v3 = (d0 ^ d3);
                CVECTOR v4 = (d2 ^ d1);

                vRes = vRes1 = !((d1 ^ d0) + (d3 ^ d2) + (d0 ^ d3) + (d2 ^ d1));
                if (bSimpleSea)
                {
                    vRes1 = !(vRes * CVECTOR(100.0f, 1.0f, 100.0f));
                }
                // CVECTOR vRes1 = !(vRes * CVECTOR(100.0f, 1.0f, 100.0f));
                uint32_t dwRes = MAKELONG(static_cast<short>(vRes.x * 32767.5f), static_cast<short>(vRes.z * 32767.5f));

                aNormals[i][x + y * XWIDTH] = dwRes;
                aVectors[i][x + y * XWIDTH] = vRes1;

                long red = fftol((vRes1.x * 0.5f + 0.5f) * 255.0f);   // FIX-ME no ftol
                long green = fftol((vRes1.y * 0.5f + 0.5f) * 255.0f); // FIX-ME no ftol
                long blue = fftol((vRes1.z * 0.5f + 0.5f) * 255.0f);  // FIX-ME no ftol

                if (pVolumeTexture)
                {
                    if (bSimpleSea)
                        *(uint32_t *)&(
                            static_cast<char *>(box[0].pBits)[i * box[0].SlicePitch + y * box[0].RowPitch + x * 4]) =
                            ARGB(0x80, blue, blue, red);
                    else
                        *(uint32_t *)&(
                            static_cast<char *>(box[0].pBits)[i * box[0].SlicePitch + y * box[0].RowPitch + x * 4]) =
                            ARGB(0x80, blue, green, red);
                }
            }

        if (!pVolumeTexture)
        {
            D3DSURFACE_DESC d3dsd;
            D3DLOCKED_RECT d3dlr;
            HRESULT hr;

            aBumpMaps.push_back(nullptr);
            // pBumpMap = &aBumpMaps[aBumpMaps.Add()];
            IDirect3DTexture9 **pBumpMap = &aBumpMaps.back();
            hr = rs->CreateTexture(XWIDTH, YWIDTH, MIPSLVLS, 0, D3DFMT_X8R8G8B8, D3DPOOL_MANAGED, pBumpMap);

            // generate mip levels for random bump
            for (uint32_t lev = 0; lev < MIPSLVLS; lev++)
            {
                (*pBumpMap)->GetLevelDesc(lev, &d3dsd);
                (*pBumpMap)->LockRect(lev, &d3dlr, nullptr, 0);

                auto *pDstT = (uint32_t *)pDst;
                for (uint32_t y = 0; y < d3dsd.Height; y++)
                {
                    for (uint32_t x = 0; x < d3dsd.Width; x++)
                    {
                        CVECTOR vTmp = 0.0f;
                        long dwMult = 1 << (lev);
                        for (uint32_t y1 = y * dwMult; y1 < (y + 1) * dwMult; y1++)
                            for (uint32_t x1 = x * dwMult; x1 < (x + 1) * dwMult; x1++)
                                vTmp += aVectors[i][(x1 & (XWIDTH - 1)) + (y1 & (YWIDTH - 1)) * XWIDTH];

                        vTmp *= (1.0f / static_cast<float>(dwMult * dwMult));

                        int red = static_cast<int>((vTmp.x * 0.5f + 0.5f) * 255.0f);   // FIX-ME no ftol
                        int green = static_cast<int>((vTmp.y * 0.5f + 0.5f) * 255.0f); // FIX-ME no ftol
                        int blue = static_cast<int>((vTmp.z * 0.5f + 0.5f) * 255.0f);  // FIX-ME no ftol
                        *pDstT++ = ARGB(255, blue, green, red);
                    }
                }

                // simple copy
                auto pDstTemp = static_cast<uint8_t *>(d3dlr.pBits);
                for (uint32_t y = 0; y < d3dsd.Height; y++)
                {
                    memcpy(pDstTemp, &pDst[y * d3dsd.Width * dwTexelSize], d3dsd.Width * dwTexelSize);
                    pDstTemp += static_cast<uint32_t>(d3dlr.Pitch);
                }

                (*pBumpMap)->UnlockRect(lev);
            }
        }
    }

    for (j = 1; j < 4; j++)
    {
        auto *pVectors = new CVECTOR[(XWIDTH >> j) * (YWIDTH >> j)];

        for (i = 0; i < (FRAMES >> j); i++)
        {
            // calculate current pVectors
            for (uint32_t y = 0; y < (YWIDTH >> j); y++)
                for (uint32_t x = 0; x < (XWIDTH >> j); x++)
                {
                    long iNumVectors = 0;
                    CVECTOR vVec = 0.0f;
                    uint32_t dwW = 1 << j;
                    for (uint32_t k = i * dwW; k < (i + 1) * dwW; k++)
                    {
                        for (uint32_t yy = y * dwW; yy < (y + 1) * dwW; yy++)
                            for (uint32_t xx = x * dwW; xx < (x + 1) * dwW; xx++)
                            {
                                iNumVectors++;
                                vVec += aVectors[k][xx + yy * XWIDTH];
                            }
                    }
                    pVectors[x + y * (XWIDTH >> j)] = vVec / static_cast<float>(iNumVectors);
                }

            //
            if (pVolumeTexture)
                for (uint32_t y = 0; y < (YWIDTH >> j); y++)
                    for (uint32_t x = 0; x < (XWIDTH >> j); x++)
                    {
                        long red = fftol((pVectors[x + y * (XWIDTH >> j)].x * 0.5f + 0.5f) * 255.0f); // FIX-ME no ftol
                        long green = fftol((pVectors[x + y * (XWIDTH >> j)].y * 0.5f + 0.5f) * 255.0f);
                        // FIX-ME no ftol
                        long blue = fftol((pVectors[x + y * (XWIDTH >> j)].z * 0.5f + 0.5f) * 255.0f); // FIX-ME no ftol

                        if (bSimpleSea)
                            *(uint32_t *)&(static_cast<char *>(
                                box[j].pBits)[i * box[j].SlicePitch + y * box[j].RowPitch + x * 4]) =
                                ARGB(0x80, blue, blue, red);
                        else
                            *(uint32_t *)&(static_cast<char *>(
                                box[j].pBits)[i * box[j].SlicePitch + y * box[j].RowPitch + x * 4]) =
                                ARGB(0x80, blue, green, red);
                    }
        }
        STORM_DELETE(pVectors);
    }

    if (pVolumeTexture)
        for (i = 0; i < 4; i++)
            pVolumeTexture->UnlockBox(i);

    for (const auto &vector : aVectors)
        delete vector;
    aVectors.clear();
    // aVectors.DelAllWithPointers();
    STORM_DELETE(pDst);
}

bool SEA::EditMode_Update()
{
    // return true;
    v4SeaColor = CVECTOR4(10.0f / 255.0f, 55.0f / 255.0f, 100.0f / 255.0f, 1.0f);
    v4SkyColor = CVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);

    const float fReflection = 0.8f;
    const float fTransparency = 0.7f;
    float fFrenel = 0.75f;
    const float fAttenuation = 0.9f;
    v4SeaParameters = CVECTOR4(fAttenuation, fReflection, fTransparency, 0.0f);

    fAmp1 = _fAmp1 = 8.0f;
    fAnimSpeed1 = 6.0f;
    fScale1 = 0.5f;
    vMoveSpeed1 = CVECTOR(5.0f, 0.0f, 0.0f);

    fAmp2 = _fAmp2 = 1.0f;
    fAnimSpeed2 = 3.0f;
    fScale2 = 2.0f;
    vMoveSpeed2 = CVECTOR(0.0f, 0.0f, 2.0f);

    fBumpScale = 0.1f;
    fBumpSpeed = 1.0f;
    fGridStep = 0.07f;
    fLodScale = 0.5f;

    fPosShift = 1.2f;

    Realize(0);

    return true;
}

long SEA::VisCode(const CVECTOR &vP)
{
    long vc = 0;

    /*
    CVECTOR v = vP - vCamPos;

    CVECTOR vp1 = CVECTOR(pFrustumPlanes[0].Nx, pFrustumPlanes[0].Ny, pFrustumPlanes[0].Nz);
    CVECTOR vp2 = CVECTOR(pFrustumPlanes[1].Nx, pFrustumPlanes[1].Ny, pFrustumPlanes[1].Nz);
    CVECTOR vp3 = CVECTOR(pFrustumPlanes[2].Nx, pFrustumPlanes[2].Ny, pFrustumPlanes[2].Nz);
    CVECTOR vp4 = CVECTOR(pFrustumPlanes[3].Nx, pFrustumPlanes[3].Ny, pFrustumPlanes[3].Nz);

    //if((v | pFrustumPlanes[0].n) < 0 ) vc |= 0x10;
    if ((v | vp1) < 0 ) vc |= 0x01;
    if ((v | vp2) < 0 ) vc |= 0x02;
    if ((v | vp3) < 0 ) vc |= 0x04;
    if ((v | vp4) < 0 ) vc |= 0x08;

    */

    // Max - so the accuracy is much higher, blocks should not disappear sometimes in the distance
    PLANE *p = &pFrustumPlanes[0];
    if (p->Nx * vP.x + p->Ny * vP.y + p->Nz * vP.z < p->D)
        vc |= 0x01;
    p = &pFrustumPlanes[1];
    if (p->Nx * vP.x + p->Ny * vP.y + p->Nz * vP.z < p->D)
        vc |= 0x02;
    p = &pFrustumPlanes[2];
    if (p->Nx * vP.x + p->Ny * vP.y + p->Nz * vP.z < p->D)
        vc |= 0x04;
    p = &pFrustumPlanes[3];
    if (p->Nx * vP.x + p->Ny * vP.y + p->Nz * vP.z < p->D)
        vc |= 0x08;

    return vc;
}

bool SEA::isVisibleBBox(const CVECTOR &vCenter, const CVECTOR &v1, const CVECTOR &v2)
{
    /*CVECTOR vc = vCenter - vCamPos;
    CVECTOR vp1 = v1 - vCamPos;
    CVECTOR vp2 = v2 - vCamPos;
    float fR2 = sqrtf(Sqr((vp1.x - vp2.x) * 0.5f) + Sqr((vp1.z - vp2.z)) * 0.5f);*/

    // if sphere not visible - return
    //    for (uint32_t i=0; i<dwNumFrustumPlanes; i++)
    //        if ((pFrustumPlanes[i].n | vc) - pFrustumPlanes[i].d < -fR2) return false;

    // check box visible
    long vc = 0xFF;
    vc &= VisCode(CVECTOR(v1.x, v1.y, v1.z));
    if (vc == 0)
        return true;
    vc &= VisCode(CVECTOR(v1.x, v2.y, v1.z));
    vc &= VisCode(CVECTOR(v2.x, v1.y, v1.z));
    vc &= VisCode(CVECTOR(v2.x, v2.y, v1.z));
    if (vc == 0)
        return true;
    vc &= VisCode(CVECTOR(v1.x, v1.y, v2.z));
    vc &= VisCode(CVECTOR(v1.x, v2.y, v2.z));
    vc &= VisCode(CVECTOR(v2.x, v1.y, v2.z));
    vc &= VisCode(CVECTOR(v2.x, v2.y, v2.z));

    return vc == 0;
}

inline float SEA::CalcLod(const float &x, const float &y, const float &z)
{
    return Sqr(x - vCamPos.x) + /*Sqr((y - vCamPos.y)) + */ Sqr(z - vCamPos.z);
}

void SEA::CalculateLOD(const CVECTOR &v1, const CVECTOR &v2, long &iMaxLOD, long &iMinLOD)
{
    float fCur, fMax, fMin;

    fCur = CalcLod(v1.x, vSeaCenterPos.y, v1.z);
    fMax = fCur;
    fCur = CalcLod(v2.x, vSeaCenterPos.y, v1.z);
    if (fCur > fMax)
        fMax = fCur;
    fCur = CalcLod(v1.x, vSeaCenterPos.y, v2.z);
    if (fCur > fMax)
        fMax = fCur;
    fCur = CalcLod(v2.x, vSeaCenterPos.y, v2.z);
    if (fCur > fMax)
        fMax = fCur;

    if (vCamPos.x < v1.x)
    {
        if (vCamPos.z < v1.z)
            fMin = CalcLod(v1.x, vSeaCenterPos.y, v1.z);
        else if (vCamPos.z > v2.z)
            fMin = CalcLod(v1.x, vSeaCenterPos.y, v2.z);
        else
            fMin = CalcLod(v1.x, vSeaCenterPos.y, vCamPos.z);
    }
    else if (vCamPos.x > v2.x)
    {
        if (vCamPos.z < v1.z)
            fMin = CalcLod(v2.x, vSeaCenterPos.y, v1.z);
        else if (vCamPos.z > v2.z)
            fMin = CalcLod(v2.x, vSeaCenterPos.y, v2.z);
        else
            fMin = CalcLod(v2.x, vSeaCenterPos.y, vCamPos.z);
    }
    else
    {
        if (vCamPos.z < v1.z)
            fMin = CalcLod(vCamPos.x, vSeaCenterPos.y, v1.z);
        else if (vCamPos.z > v2.z)
            fMin = CalcLod(vCamPos.x, vSeaCenterPos.y, v2.z);
        else
            fMin = CalcLod(vCamPos.x, vSeaCenterPos.y, vCamPos.z);
    }

    iMaxLOD = static_cast<int>(0.5f * logf(fLodScale * fMax) / logf(2));
    if (iMaxLOD < 4)
        iMaxLOD = 4;
    iMinLOD = static_cast<int>(0.5f * logf(fLodScale * fMin) / logf(2));
    if (iMinLOD < 4)
        iMinLOD = 4;
}

void SEA::AddBlock(long iTX, long iTY, long iSize, long iLOD)
{
    aBlocks.push_back(SeaBlock{});
    // SeaBlock * pB = &aBlocks[aBlocks.Add()];
    SeaBlock *pB = &aBlocks.back();

    pB->iTX = iTX;
    pB->iTY = iTY;
    pB->iSize = iSize;
    pB->iLOD = iLOD;

    pB->bInProgress = false;
    pB->bDone = false;

    pB->iSize0 = iSize >> iLOD;
}

void SEA::BuildTree(long iTX, long iTY, long iLev)
{
    long iMaxLOD, iMinLOD;
    const long iSize = static_cast<long>(dwMaxDim >> iLev);
    const float fGSize = fGridStep * iSize;

    const CVECTOR v1 = vSeaCenterPos + CVECTOR(iTX * fGSize, -fMaxSeaHeight / 2.0f, iTY * fGSize);
    const CVECTOR v2 = v1 + CVECTOR(fGSize, fMaxSeaHeight, fGSize);

    if (!isVisibleBBox(vSeaCenterPos, v1, v2))
        return;

    CalculateLOD(v1, v2, iMaxLOD, iMinLOD);

    if (iSize <= static_cast<long>(dwMinDim) || iMaxLOD - iMinLOD <= 1)
    {
        AddBlock(iTX, iTY, iSize, iMinLOD);
        return;
    }

    iTX *= 2;
    iTY *= 2;
    iLev++;

    BuildTree(iTX, iTY, iLev);
    BuildTree(iTX + 1, iTY, iLev);
    BuildTree(iTX, iTY + 1, iLev);
    BuildTree(iTX + 1, iTY + 1, iLev);
}

inline void PrefetchNTA(uint32_t dwAddress)
{
    /*_asm
    {
      mov    eax, dwAddress
      and esi, ~15d
      //add esi, 128d
      prefetchnta [esi]
    }*/
}

// INTEL COMMENT:
// This version of the function takes in 4 at a time, to take advantage of SSE.
// In particular I have converted the normalise and square roots over to SSE.
void SEA::SSE_WaveXZ(SeaVertex **pArray)
{
    CVECTOR vNormal[4];
    float nY1[4];
    float nY2[4];
    bool bDistancePass = false;

    for (int i = 0; i < 4; i++)
    {
        long iX11, iX12, iX21, iX22, iY11, iY12, iY21, iY22;

        float fDistance = Sqr(pArray[i]->vPos.x - vCamPos.x) + Sqr(pArray[i]->vPos.z - vCamPos.z);
        if (fDistance > 1600.0f * 1600.0f)
        {
            vNormal[i].x = 0.0f;
            nY1[i] = 1.0f;
            nY2[i] = 1.0f;
            vNormal[i].y = 1.0f;
            vNormal[i].z = 0.0f;
            pArray[i]->vPos.y = 0.0f;
            continue;
        }

        bDistancePass = true;

        float x1 = (pArray[i]->vPos.x + vMove1.x) * fScale1;
        float z1 = (pArray[i]->vPos.z + vMove1.z) * fScale1;
        iX11 = ffloor(x1 + 0.0f), iX12 = iX11 + 1;
        iY11 = ffloor(z1 + 0.0f), iY12 = iY11 + 1;
        float fX1 = (x1 - iX11);
        float fZ1 = (z1 - iY11);
        iX11 &= (XWIDTH - 1);
        iX12 &= (XWIDTH - 1);
        iY11 &= (XWIDTH - 1);
        iY12 &= (XWIDTH - 1);

        float x2 = (pArray[i]->vPos.x + vMove2.x) * fScale2;
        float z2 = (pArray[i]->vPos.z + vMove2.z) * fScale2;
        iX21 = ffloor(x2 + 0.0f), iX22 = iX21 + 1;
        iY21 = ffloor(z2 + 0.0f), iY22 = iY21 + 1;
        float fX2 = (x2 - iX21);
        float fZ2 = (z2 - iY21);
        iX21 &= (XWIDTH - 1);
        iX22 &= (XWIDTH - 1);
        iY21 &= (XWIDTH - 1);
        iY22 &= (XWIDTH - 1);

        float a1, a2, a3, a4;

        a1 = pSeaFrame1[iX11 + iY11 * XWIDTH];
        a2 = pSeaFrame1[iX12 + iY11 * XWIDTH];
        a3 = pSeaFrame1[iX11 + iY12 * XWIDTH];
        a4 = pSeaFrame1[iX12 + iY12 * XWIDTH];
        pArray[i]->vPos.y = fAmp1 * (a1 + fX1 * (a2 - a1) + fZ1 * (a3 - a1) + fX1 * fZ1 * (a4 + a1 - a2 - a3));

        a1 = pSeaFrame2[iX21 + iY21 * XWIDTH];
        a2 = pSeaFrame2[iX22 + iY21 * XWIDTH];
        a3 = pSeaFrame2[iX21 + iY22 * XWIDTH];
        a4 = pSeaFrame2[iX22 + iY22 * XWIDTH];
        pArray[i]->vPos.y += fAmp2 * (a1 + fX2 * (a2 - a1) + fZ2 * (a3 - a1) + fX2 * fZ2 * (a4 + a1 - a2 - a3));

        float nx1, nx2, nx3, nx4, nz1, nz2, nz3, nz4;

        nx1 = pSeaNormalsFrame1[2 * (iX11 + iY11 * XWIDTH) + 0];
        nz1 = pSeaNormalsFrame1[2 * (iX11 + iY11 * XWIDTH) + 1];
        nx2 = pSeaNormalsFrame1[2 * (iX12 + iY11 * XWIDTH) + 0];
        nz2 = pSeaNormalsFrame1[2 * (iX12 + iY11 * XWIDTH) + 1];
        nx3 = pSeaNormalsFrame1[2 * (iX11 + iY12 * XWIDTH) + 0];
        nz3 = pSeaNormalsFrame1[2 * (iX11 + iY12 * XWIDTH) + 1];
        nx4 = pSeaNormalsFrame1[2 * (iX12 + iY12 * XWIDTH) + 0];
        nz4 = pSeaNormalsFrame1[2 * (iX12 + iY12 * XWIDTH) + 1];

        float nX1 = (nx1 + fX1 * (nx2 - nx1) + fZ1 * (nx3 - nx1) + fX1 * fZ1 * (nx4 + nx1 - nx2 - nx3));
        float nZ1 = (nz1 + fX1 * (nz2 - nz1) + fZ1 * (nz3 - nz1) + fX1 * fZ1 * (nz4 + nz1 - nz2 - nz3));

        nx1 = pSeaNormalsFrame2[2 * (iX21 + iY21 * XWIDTH) + 0];
        nz1 = pSeaNormalsFrame2[2 * (iX21 + iY21 * XWIDTH) + 1];
        nx2 = pSeaNormalsFrame2[2 * (iX22 + iY21 * XWIDTH) + 0];
        nz2 = pSeaNormalsFrame2[2 * (iX22 + iY21 * XWIDTH) + 1];
        nx3 = pSeaNormalsFrame2[2 * (iX21 + iY22 * XWIDTH) + 0];
        nz3 = pSeaNormalsFrame2[2 * (iX21 + iY22 * XWIDTH) + 1];
        nx4 = pSeaNormalsFrame2[2 * (iX22 + iY22 * XWIDTH) + 0];
        nz4 = pSeaNormalsFrame2[2 * (iX22 + iY22 * XWIDTH) + 1];

        float nX2 = (nx1 + fX2 * (nx2 - nx1) + fZ2 * (nx3 - nx1) + fX2 * fZ2 * (nx4 + nx1 - nx2 - nx3));
        float nZ2 = (nz1 + fX2 * (nz2 - nz1) + fZ2 * (nz3 - nz1) + fX2 * fZ2 * (nz4 + nz1 - nz2 - nz3));

        nY1[i] = 1.0f - (Sqr(nX1) + Sqr(nZ1));
        nY2[i] = 1.0f - (Sqr(nX2) + Sqr(nZ2));

        vNormal[i].x = fScale1 * fAmp1 * nX1 + fScale2 * fAmp2 * nX2;
        vNormal[i].z = fScale1 * fAmp1 * nZ1 + fScale2 * fAmp2 * nZ2;
    }

    if (bDistancePass)
    {
        // INTEL COMMENT:
        // xmm variables.
        __m128 m128X, m128Y, m128Z;
        __m128 m128NX, m128NY, m128NZ;
        __m128 m128Y1, m128Y2;
        __m128 m128SY1, m128SY2;

        // INTEL COMMENT:
        // Gather the Y's.
        SSE_GatherFourFloats(&nY1[0], &nY1[1], &nY1[2], &nY1[3], &m128Y1);
        SSE_GatherFourFloats(&nY2[0], &nY2[1], &nY2[2], &nY2[3], &m128Y2);
        m128SY1 = _mm_sqrt_ps(m128Y1);
        m128SY2 = _mm_sqrt_ps(m128Y2);
        m128Y = _mm_add_ps(m128SY1, m128SY2);

        // INTEL COMMENT:
        // Gather the X's and Z's.
        SSE_GatherFourFloats(&vNormal[0].x, &vNormal[1].x, &vNormal[2].x, &vNormal[3].x, &m128X);
        SSE_GatherFourFloats(&vNormal[0].z, &vNormal[1].z, &vNormal[2].z, &vNormal[3].z, &m128Z);

        // INTEL COMMENT:
        // Normalise.
        SSE_Normalise(m128X, m128Y, m128Z, &m128NX, &m128NY, &m128NZ);

        // INTEL COMMENT:
        // Scatter back out.
        SSE_ScatterFourFloats(&pArray[0]->vNormal.x, &pArray[1]->vNormal.x, &pArray[2]->vNormal.x,
                              &pArray[3]->vNormal.x, m128NX);
        SSE_ScatterFourFloats(&pArray[0]->vNormal.y, &pArray[1]->vNormal.y, &pArray[2]->vNormal.y,
                              &pArray[3]->vNormal.y, m128NY);
        SSE_ScatterFourFloats(&pArray[0]->vNormal.z, &pArray[1]->vNormal.z, &pArray[2]->vNormal.z,
                              &pArray[3]->vNormal.z, m128NZ);
    }
    else
    {
        pArray[0]->vNormal = vNormal[0];
        pArray[1]->vNormal = vNormal[1];
        pArray[2]->vNormal = vNormal[2];
        pArray[3]->vNormal = vNormal[3];
    }
}

float SEA::WaveXZ(float x, float z, CVECTOR *pNormal)
{
    long iX11, iX12, iX21, iX22, iY11, iY12, iY21, iY22;

    const float fDistance = Sqr(x - vCamPos.x) + Sqr(z - vCamPos.z);
    if (fDistance > 1600.0f * 1600.0f)
    {
        if (pNormal)
            *pNormal = CVECTOR(0.0f, 1.0f, 0.0f);
        return 0.0f;
    }

    const float x1 = (x + vMove1.x) * fScale1;
    const float z1 = (z + vMove1.z) * fScale1;
    iX11 = ffloor(x1 + 0.0f), iX12 = iX11 + 1;
    iY11 = ffloor(z1 + 0.0f), iY12 = iY11 + 1;
    const float fX1 = (x1 - iX11);
    const float fZ1 = (z1 - iY11);
    iX11 &= (XWIDTH - 1);
    iX12 &= (XWIDTH - 1);
    iY11 &= (XWIDTH - 1);
    iY12 &= (XWIDTH - 1);

    const float x2 = (x + vMove2.x) * fScale2;
    const float z2 = (z + vMove2.z) * fScale2;
    iX21 = ffloor(x2 + 0.0f), iX22 = iX21 + 1;
    iY21 = ffloor(z2 + 0.0f), iY22 = iY21 + 1;
    const float fX2 = (x2 - iX21);
    const float fZ2 = (z2 - iY21);
    iX21 &= (XWIDTH - 1);
    iX22 &= (XWIDTH - 1);
    iY21 &= (XWIDTH - 1);
    iY22 &= (XWIDTH - 1);

    float a1, a2, a3, a4;

    a1 = pSeaFrame1[iX11 + iY11 * XWIDTH];
    a2 = pSeaFrame1[iX12 + iY11 * XWIDTH];
    a3 = pSeaFrame1[iX11 + iY12 * XWIDTH];
    a4 = pSeaFrame1[iX12 + iY12 * XWIDTH];
    float fRes = fAmp1 * (a1 + fX1 * (a2 - a1) + fZ1 * (a3 - a1) + fX1 * fZ1 * (a4 + a1 - a2 - a3));

    a1 = pSeaFrame2[iX21 + iY21 * XWIDTH];
    a2 = pSeaFrame2[iX22 + iY21 * XWIDTH];
    a3 = pSeaFrame2[iX21 + iY22 * XWIDTH];
    a4 = pSeaFrame2[iX22 + iY22 * XWIDTH];
    fRes += fAmp2 * (a1 + fX2 * (a2 - a1) + fZ2 * (a3 - a1) + fX2 * fZ2 * (a4 + a1 - a2 - a3));

    float nx1, nx2, nx3, nx4, nz1, nz2, nz3, nz4;

    nx1 = pSeaNormalsFrame1[2 * (iX11 + iY11 * XWIDTH) + 0];
    nz1 = pSeaNormalsFrame1[2 * (iX11 + iY11 * XWIDTH) + 1];
    nx2 = pSeaNormalsFrame1[2 * (iX12 + iY11 * XWIDTH) + 0];
    nz2 = pSeaNormalsFrame1[2 * (iX12 + iY11 * XWIDTH) + 1];
    nx3 = pSeaNormalsFrame1[2 * (iX11 + iY12 * XWIDTH) + 0];
    nz3 = pSeaNormalsFrame1[2 * (iX11 + iY12 * XWIDTH) + 1];
    nx4 = pSeaNormalsFrame1[2 * (iX12 + iY12 * XWIDTH) + 0];
    nz4 = pSeaNormalsFrame1[2 * (iX12 + iY12 * XWIDTH) + 1];

    const float nX1 = (nx1 + fX1 * (nx2 - nx1) + fZ1 * (nx3 - nx1) + fX1 * fZ1 * (nx4 + nx1 - nx2 - nx3));
    const float nZ1 = (nz1 + fX1 * (nz2 - nz1) + fZ1 * (nz3 - nz1) + fX1 * fZ1 * (nz4 + nz1 - nz2 - nz3));

    nx1 = pSeaNormalsFrame2[2 * (iX21 + iY21 * XWIDTH) + 0];
    nz1 = pSeaNormalsFrame2[2 * (iX21 + iY21 * XWIDTH) + 1];
    nx2 = pSeaNormalsFrame2[2 * (iX22 + iY21 * XWIDTH) + 0];
    nz2 = pSeaNormalsFrame2[2 * (iX22 + iY21 * XWIDTH) + 1];
    nx3 = pSeaNormalsFrame2[2 * (iX21 + iY22 * XWIDTH) + 0];
    nz3 = pSeaNormalsFrame2[2 * (iX21 + iY22 * XWIDTH) + 1];
    nx4 = pSeaNormalsFrame2[2 * (iX22 + iY22 * XWIDTH) + 0];
    nz4 = pSeaNormalsFrame2[2 * (iX22 + iY22 * XWIDTH) + 1];

    const float nX2 = (nx1 + fX2 * (nx2 - nx1) + fZ2 * (nx3 - nx1) + fX2 * fZ2 * (nx4 + nx1 - nx2 - nx3));
    const float nZ2 = (nz1 + fX2 * (nz2 - nz1) + fZ2 * (nz3 - nz1) + fX2 * fZ2 * (nz4 + nz1 - nz2 - nz3));
    // float nX2 = 0.0f;
    // float nZ2 = 0.0f;

    /*float fDistance = sqrt_ss(Sqr(x - vCamPos.x) + Sqr(fRes - vCamPos.y) + Sqr(z - vCamPos.z));
    {
      float distance_mul = 1.0f - fDistance / 1900.0f;
      if (distance_mul < 0.0f) distance_mul = 0.0f;

      fRes *= distance_mul;

      nX1 *= distance_mul;
      nZ1 *= distance_mul;

      nX2 *= distance_mul;
      nZ2 *= distance_mul;
    }*/

    if (pNormal)
    {
        /*pNormal->x = 0.0f;
        pNormal->y = 1.0f;
        pNormal->z = 0.0f;*/
        const float nY1 = sqrtf(1.0f - (Sqr(nX1) + Sqr(nZ1)));
        const float nY2 = sqrtf(1.0f - (Sqr(nX2) + Sqr(nZ2)));

        CVECTOR vNormal;

        vNormal.x = fScale1 * fAmp1 * nX1 + fScale2 * fAmp2 * nX2;
        vNormal.z = fScale1 * fAmp1 * nZ1 + fScale2 * fAmp2 * nZ2;
        vNormal.y = nY1 + nY2;

        // vNormal.x += 2.0f * vNormal.x;
        // vNormal.z += 2.0f * vNormal.z;
        vNormal = !vNormal;

        *pNormal = vNormal;
    }

    return fRes;
}

void SEA::PrepareIndicesForBlock(uint32_t dwBlockIndex)
{
    SeaBlock *pB = &aBlocks[dwBlockIndex];

    const float fStep = fGridStep * static_cast<float>(1 << pB->iLOD);
    float fSize = fGridStep * pB->iSize;
    long x, y, size0 = pB->iSize >> pB->iLOD;

    pB->iIStart = iIStart;

    const float x1 = static_cast<float>(pB->iTX * pB->iSize) * fGridStep;
    const float y1 = static_cast<float>(pB->iTY * pB->iSize) * fGridStep;
    const float x2 = x1 + static_cast<float>(size0) * fStep;
    const float y2 = y1 + static_cast<float>(size0) * fStep;

    pB->iX1 = fftoi(x1 / fGridStep);
    pB->iX2 = fftoi(x2 / fGridStep);
    pB->iY1 = fftoi(y1 / fGridStep);
    pB->iY2 = fftoi(y2 / fGridStep);

    // analysis
    long i, j;
    for (i = 0; i < static_cast<long>(dwBlockIndex); i++) // if (i == 6)
    {
        SeaBlock *pB2 = &aBlocks[i];

        // Test Up & Down
        const bool bTestedUp = pB->iY1 == pB2->iY2;
        const bool bTestedDown = pB->iY2 == pB2->iY1;

        // if (!(GetAsyncKeyState('5')<0))
        if (bTestedUp || bTestedDown)
        {
            const long iAddSrc = pB2->iIStart + ((bTestedUp) ? (pB2->iSize0 + 1) * pB2->iSize0 : 0);
            const long iAddDst = pB->iIStart + ((bTestedUp) ? 0 : (pB->iSize0 + 1) * pB->iSize0);

            if ((pB->iX1 >= pB2->iX1 && pB->iX2 <= pB2->iX2) || (pB->iX1 <= pB2->iX1 && pB->iX2 >= pB2->iX2))
            {
                const long iMinX = Max(pB->iX1, pB2->iX1);
                const long iMaxX = Min(pB->iX2, pB2->iX2);

                const long iStartDstX = pB->iSize0 * (iMinX - pB->iX1) / (pB->iX2 - pB->iX1);
                const long iStartSrcX = pB2->iSize0 * (iMinX - pB2->iX1) / (pB2->iX2 - pB2->iX1);

                const long iEndDstX = pB->iSize0 * (iMaxX - pB->iX1) / (pB->iX2 - pB->iX1);
                long iEndSrcX = pB2->iSize0 * (iMaxX - pB2->iX1) / (pB2->iX2 - pB2->iX1);

                if (pB->iLOD == pB2->iLOD)
                    for (j = iStartDstX; j <= iEndDstX; j++)
                        pIndices[iAddDst + j] = pIndices[iAddSrc + iStartSrcX + (j - iStartDstX)];
                else
                    for (j = iStartDstX; j <= iEndDstX; j++)
                        pIndices[iAddDst + j] = pIndices[iAddSrc + iStartSrcX + (j - iStartDstX) / 2];

                continue;
            }
        }

        // Test Left & Right
        const bool bTestedLeft = pB->iX1 == pB2->iX2;
        const bool bTestedRight = pB->iX2 == pB2->iX1;
        // if ((GetAsyncKeyState('6')<0))
        if (bTestedLeft || bTestedRight)
        {
            const long iAddSrc = pB2->iIStart + ((bTestedLeft) ? (pB2->iSize0) : 0);
            const long iAddDst = pB->iIStart + ((bTestedLeft) ? 0 : (pB->iSize0));

            if ((pB->iY1 >= pB2->iY1 && pB->iY2 <= pB2->iY2) || (pB->iY1 <= pB2->iY1 && pB->iY2 >= pB2->iY2))
            {
                const long iMinY = Max(pB->iY1, pB2->iY1);
                const long iMaxY = Min(pB->iY2, pB2->iY2);

                const long iStartDstY = pB->iSize0 * (iMinY - pB->iY1) / (pB->iY2 - pB->iY1);
                const long iStartSrcY = pB2->iSize0 * (iMinY - pB2->iY1) / (pB2->iY2 - pB2->iY1);

                const long iEndDstY = pB->iSize0 * (iMaxY - pB->iY1) / (pB->iY2 - pB->iY1);
                long iEndSrcY = pB2->iSize0 * (iMaxY - pB2->iY1) / (pB2->iY2 - pB2->iY1);

                if (pB->iLOD == pB2->iLOD)
                    for (j = iStartDstY; j <= iEndDstY; j++)
                        pIndices[iAddDst + j * (pB->iSize0 + 1)] =
                            pIndices[iAddSrc + (iStartSrcY + j - iStartDstY) * (pB2->iSize0 + 1)];
                else
                    for (j = iStartDstY; j <= iEndDstY; j++)
                        pIndices[iAddDst + j * (pB->iSize0 + 1)] =
                            pIndices[iAddSrc + (iStartSrcY + (j - iStartDstY) / 2) * (pB2->iSize0 + 1)];
            }
        }
    }

    long iIFirst = 1000000, iILast = -1000000;

    for (y = 0; y <= size0; y++)
        for (x = 0; x <= size0; x++)
        {
            if (pIndices[iIStart] != static_cast<uint32_t>(-1))
            {
                iIStart++;
                continue;
            }

            if (iVStart < iIFirst)
                iIFirst = iVStart;
            if (iVStart > iILast)
                iILast = iVStart;

            pIndices[iIStart++] = static_cast<uint32_t>(iVStart);
            iVStart++;
        }

    pB->iIFirst = iIFirst;
    pB->iILast = iILast;

    // setup triangles
    long yy, dyy = size0 + 1;
    for (y = 0, yy = 0; y < size0; y++, yy += dyy)
        for (x = 0; x < size0; x++)
        {
            // first triangle
            *pTriangles++ = static_cast<uint16_t>(pIndices[pB->iIStart + static_cast<uint32_t>(x + yy + dyy + 1)]);
            *pTriangles++ = static_cast<uint16_t>(pIndices[pB->iIStart + static_cast<uint32_t>(x + yy + 1)]);
            *pTriangles++ = static_cast<uint16_t>(pIndices[pB->iIStart + static_cast<uint32_t>(x + yy)]);
            // second triangle
            *pTriangles++ = static_cast<uint16_t>(pIndices[pB->iIStart + static_cast<uint32_t>(x + yy + dyy)]);
            *pTriangles++ = static_cast<uint16_t>(pIndices[pB->iIStart + static_cast<uint32_t>(x + yy + dyy + 1)]);
            *pTriangles++ = static_cast<uint16_t>(pIndices[pB->iIStart + static_cast<uint32_t>(x + yy)]);

            iTStart += 2;
        }
}

void SEA::SSE_WaveXZBlock(SeaBlock *pB)
{
    // return;
    if (!pB)
        return;

    SeaVertex *vTmp[4];
    SeaVertex vFake;

    vFake.vPos.x = vCamPos.x + 1e+5f;
    vFake.vPos.z = vCamPos.z + 1e+5f;

    float cx, cz, fStep = fGridStep * static_cast<float>(1 << pB->iLOD);
    float fSize = fGridStep * pB->iSize;
    long x, y, size0 = pB->iSize >> pB->iLOD;

    float x1 = static_cast<float>(pB->iTX * pB->iSize) * fGridStep;
    float y1 = static_cast<float>(pB->iTY * pB->iSize) * fGridStep;
    float x2 = x1 + static_cast<float>(size0) * fStep;
    float y2 = y1 + static_cast<float>(size0) * fStep;

    pB->iX1 = fftoi(x1 / fGridStep);
    pB->iX2 = fftoi(x2 / fGridStep);
    pB->iY1 = fftoi(y1 / fGridStep);
    pB->iY2 = fftoi(y2 / fGridStep);

    x1 += vSeaCenterPos.x;
    x2 += vSeaCenterPos.x;
    y1 += vSeaCenterPos.z;
    y2 += vSeaCenterPos.z;

    // CVECTOR vNormal, vTmp;
    long iCurrentV = 0;

    long iIStart1 = pB->iIStart;
    const long iIFirst = pB->iIFirst;
    const long iILast = pB->iILast;

    // calculate
    for (cz = y1, y = 0; y <= size0; y++, cz += fStep)
    {
        for (cx = x1, x = 0; x <= size0; x++, cx += fStep)
        {
            const uint32_t dwVIndex = pIndices[iIStart1];

            if (static_cast<long>(dwVIndex) < iIFirst || static_cast<long>(dwVIndex) > iILast)
            {
                iIStart1++;
                continue;
            }

            vTmp[iCurrentV] = &pVSea[dwVIndex];
            vTmp[iCurrentV]->vPos.x = cx;
            vTmp[iCurrentV]->vPos.z = cz;

            iCurrentV++;

            if (iCurrentV == 4)
            {
                SSE_WaveXZ(&vTmp[0]);
                iCurrentV = 0;

                for (long i = 0; i < 4; i++)
                {
                    vTmp[i]->tu = (vTmp[i]->vPos.x - vTmp[i]->vNormal.x * 5.3f) * fBumpScale;
                    vTmp[i]->tv = (vTmp[i]->vPos.z - vTmp[i]->vNormal.z * 5.3f) * fBumpScale;
                    vTmp[i]->vPos.x -= vTmp[i]->vNormal.x * fPosShift * 3.0f + vWorldOffset.x;
                    vTmp[i]->vPos.z -= vTmp[i]->vNormal.z * fPosShift * 3.0f + vWorldOffset.z;
                }
            }

            /*vTmp.y = WaveXZ(cx, cz, &vNormal);

            vTmp.x = cx - vNormal.x * fPosShift * 3.0f;
            vTmp.z = cz - vNormal.z * fPosShift * 3.0f;

            pVSea[dwVIndex].vPos = vTmp;
            pVSea[dwVIndex].vNormal = vNormal;
            pVSea[dwVIndex].tu = (cx - vNormal.x * 5.3f) * fBumpScale;
            pVSea[dwVIndex].tv = (cz - vNormal.z * 5.3f) * fBumpScale;*/

            iIStart1++;
        }
    }

    if (iCurrentV != 0)
    {
        for (long i = iCurrentV; i < 4; i++)
            vTmp[i] = &vFake;

        SSE_WaveXZ(&vTmp[0]);
        for (long i = 0; i < iCurrentV; i++)
        {
            vTmp[i]->tu = (vTmp[i]->vPos.x - vTmp[i]->vNormal.x * 5.3f) * fBumpScale;
            vTmp[i]->tv = (vTmp[i]->vPos.z - vTmp[i]->vNormal.z * 5.3f) * fBumpScale;

            vTmp[i]->vPos.x -= vTmp[i]->vNormal.x * fPosShift * 3.0f + vWorldOffset.x;
            vTmp[i]->vPos.z -= vTmp[i]->vNormal.z * fPosShift * 3.0f + vWorldOffset.z;
        }
    }

    pB->bDone = true;

    EnterCriticalSection(&cs1);
    iBlocksDoneNum++;
    LeaveCriticalSection(&cs1);
}

void SEA::WaveXZBlock(SeaBlock *pB)
{
    // return;
    if (!pB)
        return;

    float cx, cz, fStep = fGridStep * static_cast<float>(1 << pB->iLOD);
    float fSize = fGridStep * pB->iSize;
    long x, y, size0 = pB->iSize >> pB->iLOD;

    float x1 = static_cast<float>(pB->iTX * pB->iSize) * fGridStep;
    float y1 = static_cast<float>(pB->iTY * pB->iSize) * fGridStep;
    float x2 = x1 + static_cast<float>(size0) * fStep;
    float y2 = y1 + static_cast<float>(size0) * fStep;

    pB->iX1 = fftoi(x1 / fGridStep);
    pB->iX2 = fftoi(x2 / fGridStep);
    pB->iY1 = fftoi(y1 / fGridStep);
    pB->iY2 = fftoi(y2 / fGridStep);

    x1 += vSeaCenterPos.x;
    x2 += vSeaCenterPos.x;
    y1 += vSeaCenterPos.z;
    y2 += vSeaCenterPos.z;

    CVECTOR vNormal, vTmp;

    long iIStart1 = pB->iIStart;
    const long iIFirst = pB->iIFirst;
    const long iILast = pB->iILast;

    // calculate
    for (cz = y1, y = 0; y <= size0; y++, cz += fStep)
    {
        for (cx = x1, x = 0; x <= size0; x++, cx += fStep)
        {
            const uint32_t dwVIndex = pIndices[iIStart1];

            if (static_cast<long>(dwVIndex) < iIFirst || static_cast<long>(dwVIndex) > iILast)
            {
                iIStart1++;
                continue;
            }

            vTmp.y = WaveXZ(cx, cz, &vNormal);

            vTmp.x = cx - vNormal.x * fPosShift * 3.0f - vWorldOffset.x;
            vTmp.z = cz - vNormal.z * fPosShift * 3.0f - vWorldOffset.z;

            pVSea[dwVIndex].vPos = vTmp;
            pVSea[dwVIndex].vNormal = vNormal;
            pVSea[dwVIndex].tu = (cx - vNormal.x * 5.3f) * fBumpScale;
            pVSea[dwVIndex].tv = (cz - vNormal.z * 5.3f) * fBumpScale;

            iIStart1++;
        }
    }

    pB->bDone = true;

    EnterCriticalSection(&cs1);
    iBlocksDoneNum++;
    LeaveCriticalSection(&cs1);
}

SEA::SeaBlock *SEA::GetUndoneBlock()
{
    SeaBlock *pB = nullptr;
    for (long i = 0; i < aBlocks.size(); i++)
        if (!aBlocks[i].bInProgress)
        {
            pB = &aBlocks[i];
            pB->bInProgress = true;
            return pB;
        }
    return pB;
}

uint32_t SEA::ThreadExecute(long iThreadIndex)
{
    HANDLE hHandles[] = {pSea->hEventCalcMaps, pSea->aEventCalcBlock[iThreadIndex]};

    while (true)
    {
        const uint32_t dwValue = WaitForMultipleObjects(ARRSIZE(hHandles), hHandles, false, INFINITE);

        if (dwValue >= WAIT_OBJECT_0 && dwValue < WAIT_OBJECT_0 + ARRSIZE(hHandles))
        {
            const HANDLE hValue = hHandles[dwValue - WAIT_OBJECT_0];

            if (hValue == pSea->hEventCalcMaps)
            {
            }

            if (hValue == pSea->aEventCalcBlock[iThreadIndex])
            {
                while (true)
                {
                    EnterCriticalSection(&pSea->cs);
                    SeaBlock *pB = pSea->GetUndoneBlock();
                    LeaveCriticalSection(&pSea->cs);

                    if (!pB)
                        break;

                    // if (SEA::bIntel && SEA::bSSE) // fix AMD
                    if (bSSE)
                        pSea->SSE_WaveXZBlock(pB);
                    else
                        pSea->WaveXZBlock(pB);

                    pSea->iB2++;
                    pSea->aThreadsTest[iThreadIndex]++;
                }
            }
        }
    }
}

void SEA::CalculateNormalMap(float fFrame, float fAmplitude, float *pfOut, std::vector<uint32_t *> &aFrames)
{
    const long iFrame1 = fftol(fFrame) % aFrames.size();
    const long iFrame2 = (iFrame1 + 1) % aFrames.size();

    const float fDelta = fFrame - iFrame1;

    uint32_t *pB1 = aFrames[iFrame1];
    uint32_t *pB2 = aFrames[iFrame2];

    for (long y = 0; y < YWIDTH; y++)
        for (long x = 0; x < XWIDTH; x++)
        {
            const uint32_t dw1 = pB1[x + y * XWIDTH];
            const uint32_t dw2 = pB2[x + y * XWIDTH];
            const float nx1 = static_cast<float>(static_cast<short>(dw1)) / 32767.5f;
            const float nx2 = static_cast<float>(static_cast<short>(dw2)) / 32767.5f;
            const float nz1 = static_cast<float>(static_cast<short>(dw1 >> 0x10)) / 32767.5f;
            const float nz2 = static_cast<float>(static_cast<short>(dw2 >> 0x10)) / 32767.5f;

            pfOut[2 * (x + y * XWIDTH) + 0] = (nx1 + (nx2 - nx1) * fDelta);
            pfOut[2 * (x + y * XWIDTH) + 1] = (nz1 + (nz2 - nz1) * fDelta);
        }
}

void SEA::CalculateHeightMap(float fFrame, float fAmplitude, float *pfOut, std::vector<uint8_t *> &aFrames)
{
    const long iFrame1 = fftol(fFrame) % aFrames.size();
    const long iFrame2 = (iFrame1 + 1) % aFrames.size();

    const float fDelta = fFrame - iFrame1;

    uint8_t *pB1 = aFrames[iFrame1];
    uint8_t *pB2 = aFrames[iFrame2];

    for (long y = 0; y < YWIDTH; y++)
        for (long x = 0; x < XWIDTH; x++)
        {
            const float f1 = pB1[x + y * XWIDTH];
            const float f2 = pB2[x + y * XWIDTH];

            pfOut[x + y * XWIDTH] = fAmplitude * (f1 + (f2 - f1) * fDelta);
        }
}

void SEA::Realize(uint32_t dwDeltaTime)
{
    static float fTmp = 0.0f;

    if (core.Controls->GetDebugAsyncKeyState(VK_SHIFT) < 0 && core.Controls->GetDebugAsyncKeyState('S') < 0)
    {
        if (bTempFullMode)
        {
            bTempFullMode = false;
            fGridStep = fTempGridStep;
            fLodScale = fTempLodScale;
        }
        else
        {
            bTempFullMode = true;
            fTempGridStep = fGridStep;
            fTempLodScale = fLodScale;

            fGridStep = 0.07f;
            fLodScale = 0.5f;
        }

        Sleep(100);
    }

    if (bStop)
        return;

    float fDeltaTime = static_cast<float>(dwDeltaTime) * 0.001f;

    vMove1 += fDeltaTime * vMoveSpeed1;
    vMove2 += fDeltaTime * vMoveSpeed2;

    fFrame1 += fDeltaTime * fAnimSpeed1;
    fFrame2 += fDeltaTime * fAnimSpeed2;
    while (fFrame1 >= FRAMES)
        fFrame1 -= FRAMES;
    while (fFrame2 >= FRAMES)
        fFrame2 -= FRAMES;

    // pSeaParameters->SetTexture(0, pAnimTexture);

    uint64_t dwTotalRDTSC;
    RDTSC_B(dwTotalRDTSC);

    CMatrix mView, mIView;
    rs->GetTransform(D3DTS_VIEW, mView);
    mIView = mView;
    mIView.Transposition();

    vCamPos = mIView.Pos();
#ifdef OLD_WORLD_POS
    vWorldOffset = 0.0f;
#else
    vWorldOffset = vCamPos;
    vWorldOffset.y = 0.f;
#endif

    bUnderSea = false;

    if (vCamPos.y < 0.0f)
        bUnderSea = true;

    if (vCamPos.y < WaveXZ(vCamPos.x, vCamPos.z && bStarted))
        bUnderSea = true;

    if (!bUnderSea)
        bUnderSeaStarted = false;

    if (bSimpleSea)
    {
        EnvMap_Render2();
        SunRoad_Render2();
    }
    else
    {
        EnvMap_Render();
        SunRoad_Render();
    }

    pFrustumPlanes = rs->GetPlanes();

    float fBlockSize = 256.0f * fGridStep;
    long iNumBlocks = static_cast<long>(dwMaxDim) / (256 * 2);
    vSeaCenterPos = CVECTOR(fBlockSize * (static_cast<long>(vCamPos.x / fBlockSize) - iNumBlocks), fMaxSeaHeight * 0.5f,
                            fBlockSize * (static_cast<long>(vCamPos.z / fBlockSize) - iNumBlocks));

    iB1 = 0;
    iB2 = 0;

    iVStart = 0;
    iTStart = 0;
    iIStart = 0;

    if (!pVolumeTexture && aBumpMaps.size())
    {
        IDirect3DSurface9 *pFace;

        static float fBumpMapFrame = 0.0f;
        fBumpMapFrame += static_cast<float>(fDeltaTime) * fBumpSpeed * 48.0f;

        uint32_t dw1 = static_cast<long>(fBumpMapFrame) % aBumpMaps.size();
        uint32_t dw2 = static_cast<long>(fBumpMapFrame + 1.0f) % aBumpMaps.size();

        float fAlpha = 255.0f * (fBumpMapFrame - static_cast<float>(static_cast<long>(fBumpMapFrame)));
        rs->SetRenderState(D3DRS_TEXTUREFACTOR, ARGB(fAlpha, 0, 0, 0));

        rs->SetTexture(0, aBumpMaps[dw1]);
        rs->SetTexture(1, aBumpMaps[dw2]);

        rs->EndScene();
        rs->PushRenderTarget();
        for (uint32_t i = 0; i < MIPSLVLS; i++)
        {
            HRESULT hr = pRenderTargetBumpMap->GetSurfaceLevel(i, &pFace);
            rs->SetRenderTarget(pFace, nullptr);
            pFace->Release();
            float w, h;
            w = h = static_cast<float>(XWIDTH >> i);

            rs->BeginScene();

            RS_SPRITE spr[4];
            FillSpriteVertex(spr[0], 0, 0, 0.1f, 0xFFFFFFFF, 0.0f, 0.0f);
            FillSpriteVertex(spr[1], 0, h, 0.1f, 0xFFFFFFFF, 0.0f, 1.0f);
            FillSpriteVertex(spr[2], w, h, 0.1f, 0xFFFFFFFF, 1.0f, 1.0f);
            FillSpriteVertex(spr[3], w, 0, 0.1f, 0xFFFFFFFF, 1.0f, 0.0f);

            rs->DrawSprites(spr, 1, "bump_interpolate");

            rs->EndScene();
        }
        rs->PopRenderTarget();
        rs->BeginScene();
    }

    memset(pIndices, 0xFF, NUM_VERTEXS * sizeof(pIndices[0]) * 3);

    uint64_t dwX;
    RDTSC_B(dwX);
    // SetEvent(hEventCalcMaps);
    CalculateHeightMap(fFrame1, 1.0f / 255.0f, pSeaFrame1, aBumps);
    CalculateNormalMap(fFrame1, 1.0f / 255.0f, pSeaNormalsFrame1, aNormals);

    CalculateHeightMap(fFrame2, 1.0f / 255.0f, pSeaFrame2, aBumps);
    CalculateNormalMap(fFrame2, 1.0f / 255.0f, pSeaNormalsFrame2, aNormals);
    RDTSC_E(dwX);
    // core.Trace("dwX = %d", dwX);

    aBlocks.clear();
    BuildTree(0, 0, 0);
    // aBlocks.QSort(SeaBlock::QSort);
    std::sort(aBlocks.begin(), aBlocks.end(), SeaBlock::QSort);

    uint32_t i;
    long iNumVPoints = 0;
    for (i = 0; i < aBlocks.size(); i++)
    {
        iNumVPoints += aBlocks[i].iSize0 * aBlocks[i].iSize0;
        if (iNumVPoints >= NUM_VERTEXS)
        {
            aBlocks.erase(aBlocks.begin() + i, aBlocks.end());
            break;
        }
    }

    auto pVSea2 = static_cast<SeaVertex *>(rs->LockVertexBuffer(iVSeaBuffer, D3DLOCK_DISCARD | D3DLOCK_NOSYSLOCK));
    pTriangles = static_cast<uint16_t *>(rs->LockIndexBuffer(iISeaBuffer, D3DLOCK_DISCARD | D3DLOCK_NOSYSLOCK));

    for (i = 0; i < aBlocks.size(); i++)
        PrepareIndicesForBlock(i);
    // for (i=0; i<aBlocks(); i++) SetBlock(i);

    iBlocksDoneNum = 0;

    bool bHT = false;

    uint64_t dwBlockRDTSC;
    RDTSC_B(dwBlockRDTSC);

    if (bHyperThreading) // P4 / PRESCOTT Version
    {
        bHT = true;

        for (long i = 0; i < aEventCalcBlock.size(); i++)
        {
            aThreadsTest[i] = 0;
            SetEvent(aEventCalcBlock[i]);
        }

        while (true)
        {
            EnterCriticalSection(&cs);
            SeaBlock *pB = GetUndoneBlock();
            LeaveCriticalSection(&cs);
            if (!pB)
                break;
            // if (bIntel && bSSE) // fix AMD
            if (bSSE)
                SSE_WaveXZBlock(pB);
            else
                WaveXZBlock(pB);
            iB1++;
        }

        while (iBlocksDoneNum < aBlocks.size())
            _mm_pause();
    }
    else
    {
        while (true)
        {
            SeaBlock *pB = GetUndoneBlock();
            if (!pB)
                break;
            // if (bIntel && bSSE) // fix AMD
            if (bSSE)
                SSE_WaveXZBlock(pB);
            else
                WaveXZBlock(pB);
            iB1++;
        }
    }
    RDTSC_E(dwBlockRDTSC);

    if (iVStart && iTStart)
        memcpy(pVSea2, pVSea, iVStart * sizeof(SeaVertex));

    rs->UnLockVertexBuffer(iVSeaBuffer);
    rs->UnLockIndexBuffer(iISeaBuffer);

    if (iVStart && iTStart)
    {
        CMatrix mWorldView, mWorldViewProj;

        CMatrix mView = rs->GetView();
        CMatrix mWorld;
        mWorld.SetIdentity(); // = rs->GetWorld();
        CMatrix mProjection = rs->GetProjection();

#ifndef OLD_WORLD_POS
        mView.MoveInversePosition(-vWorldOffset.x, 0.f, -vWorldOffset.z);
#endif

        mWorldView.EqMultiply(mWorld, mView);
        mWorldViewProj.EqMultiply(mWorldView, mProjection);

        mWorldViewProj.Transposition4x4();

        fTmp += fDeltaTime * fBumpSpeed;
        while (fTmp >= 1.0f)
            fTmp -= 1.0f;

        rs->SetVertexDeclaration(vertexDecl_);

        const auto vec1 = CVECTOR4(0.0f, 1.0f, 0.5f, -0.04f);
        const auto vec2 = CVECTOR4(2.0f, -1.0f, 0.00036621652552071f, (bFogEnable) ? fFogSeaDensity : 0.0f);
        const auto vec3 = CVECTOR4(fFoamV, fFoamK, fFoamUV, 6.0f);
        const auto vec4 = CVECTOR4(fTmp, fTmp, fTmp, fTmp);
        const auto vec5 = CVECTOR4(vCamPos.x - vWorldOffset.x, vCamPos.y, vCamPos.z - vWorldOffset.z, 1.0f);
        const auto vec6 = CVECTOR4(1.0f, 0.0f, 0.0f, 1.0f);
        const auto vec7 = CVECTOR4(fFrenel, 1.0f, 0.5f, 1.0f);
        const auto vec8 = CVECTOR4(1.0f, 0.0f, 0.0f, 1.0f);
        CVECTOR vTmp = !CVECTOR(0.0f, 1.0f, 0.0f);
        const auto vec9 = CVECTOR4(vTmp.x, vTmp.y, vTmp.z, 1.0f);
        rs->SetVertexShaderConstantF(GC_CONSTANT, (const float *)&vec1, 1);
        rs->SetVertexShaderConstantF(GC_CONSTANT2, (const float *)&vec2, 1);
        rs->SetVertexShaderConstantF(GC_SHADOW_CONST1, (const float *)&vec3, 1);
        rs->SetVertexShaderConstantF(GC_ANIMATION, (const float *)&vec4, 1);
        rs->SetVertexShaderConstantF(GC_CAMERA_POS, (const float *)&vec5, 1);
        rs->SetVertexShaderConstantF(GC_MTX_WVP, (const float *)&mWorldViewProj, 4);

        rs->SetVertexShaderConstantF(GC_FREE, (const float *)&v4SeaParameters, 1);
        rs->SetVertexShaderConstantF(GC_FREE + 1, (const float *)&v4SeaColor, 1);
        rs->SetVertexShaderConstantF(GC_FREE + 2, (const float *)&v4SkyColor, 1);

        rs->SetVertexShaderConstantF(GC_FREE + 5, (const float *)&vec6, 1);
        rs->SetVertexShaderConstantF(GC_FREE + 6, (const float *)&vec7, 1);
        // Frenel K, Frenel Max
        rs->SetVertexShaderConstantF(GC_FREE + 7, (const float *)&vec8, 1);
        rs->SetVertexShaderConstantF(GC_FREE + 30, (const float *)&vec9, 1);

        if (bSimpleSea)
        {
            rs->SetVertexShaderConstantF(GC_FREE + 8, (const float *)&mTexProjection, 4); // Matrix!!

            // rs->SetTexture(0, pVolumeTexture);
            rs->SetTexture(0, (pVolumeTexture) ? static_cast<IDirect3DBaseTexture9 *>(pVolumeTexture)
                                               : static_cast<IDirect3DBaseTexture9 *>(pRenderTargetBumpMap));
            rs->SetTexture(1, pReflection);
            // rs->SetTexture(2, pVolumeTexture);
            rs->SetTexture(2, (pVolumeTexture) ? static_cast<IDirect3DBaseTexture9 *>(pVolumeTexture)
                                               : static_cast<IDirect3DBaseTexture9 *>(pRenderTargetBumpMap));
            rs->SetTexture(3, pReflectionSunroad);

            rs->SetTextureStageState(1, D3DTSS_BUMPENVMAT00, F2DW(0.08f));
            rs->SetTextureStageState(1, D3DTSS_BUMPENVMAT10, F2DW(0.0f));
            rs->SetTextureStageState(1, D3DTSS_BUMPENVMAT01, F2DW(0.0f));
            rs->SetTextureStageState(1, D3DTSS_BUMPENVMAT11, F2DW(0.08f));

            rs->SetTextureStageState(3, D3DTSS_BUMPENVMAT00, F2DW(0.05f));
            rs->SetTextureStageState(3, D3DTSS_BUMPENVMAT10, F2DW(0.0f));
            rs->SetTextureStageState(3, D3DTSS_BUMPENVMAT01, F2DW(0.0f));
            rs->SetTextureStageState(3, D3DTSS_BUMPENVMAT11, F2DW(0.05f));

            rs->DrawIndexedPrimitiveNoVShader(D3DPT_TRIANGLELIST, iVSeaBuffer, sizeof(SeaVertex), iISeaBuffer, 0,
                                              iVStart, 0, iTStart, "Sea3");
        }
        else
        {
            const auto vec1 = CMatrix(0.0f, 0.0f, PId2);
            rs->SetVertexShaderConstantF(GC_FREE + 8, (const float *)&vec1, 4); // Matrix!!

            rs->SetTexture(0, (pVolumeTexture) ? static_cast<IDirect3DBaseTexture9 *>(pVolumeTexture)
                                               : static_cast<IDirect3DBaseTexture9 *>(pRenderTargetBumpMap));
            rs->SetTexture(3, pEnvMap);
            rs->DrawIndexedPrimitiveNoVShader(D3DPT_TRIANGLELIST, iVSeaBuffer, sizeof(SeaVertex), iISeaBuffer, 0,
                                              iVStart, 0, iTStart, "Sea2");

            if (fFoamK > 0.0f && bFoamEnable && bIniFoamEnable)
            {
                // Render sea foam
                const auto vec2 = CVECTOR4(fFoamTextureDisturb, 0.0f, 0.0f, 0.0f);
                rs->SetPixelShaderConstantF(0, (const float *)&vec2, 1);

                rs->TextureSet(0, iFoamTexture);
                rs->SetTexture(4, (pVolumeTexture) ? static_cast<IDirect3DBaseTexture9 *>(pVolumeTexture)
                                                   : static_cast<IDirect3DBaseTexture9 *>(pRenderTargetBumpMap));
                rs->DrawIndexedPrimitiveNoVShader(D3DPT_TRIANGLELIST, iVSeaBuffer, sizeof(SeaVertex), iISeaBuffer, 0,
                                                  iVStart, 0, iTStart, "Sea2_Foam");
            }

            rs->SetTexture(0, (pVolumeTexture) ? static_cast<IDirect3DBaseTexture9 *>(pVolumeTexture)
                                               : static_cast<IDirect3DBaseTexture9 *>(pRenderTargetBumpMap));
            rs->SetTexture(3, pSunRoadMap);
            rs->DrawIndexedPrimitiveNoVShader(D3DPT_TRIANGLELIST, iVSeaBuffer, sizeof(SeaVertex), iISeaBuffer, 0,
                                              iVStart, 0, iTStart, "Sea2_SunRoad");
        }
    }

    RDTSC_E(dwTotalRDTSC);
    // rs->Print(50, 300, "Total ticks with rendering %d", /*iVStart, iTStart, */dwTotalRDTSC);
    /*rs->Print(50, 320, "calc blk%s: %d", (bHT) ? " (HT)" : "", dwBlockRDTSC);
    rs->Print(50, 340, "Blocks in 1st thread: %d", iB1);
    for (long i=0; i<aThreadsTest; i++)
      rs->Print(50, 360 + 20 * i, "Blocks in thread %d: %d", i + 1, aThreadsTest[i]);
    */
    // rs->Print(30, 140, "rdtsc = %d", dwBlockRDTSC);
    // rs->Print(30, 160, "Intel CPU: %s, SSE: %s, HyperThreading: %s", (bIntel) ? "Yes" : "No", (bSSE) ? "On" : "Off",
    // (bHyperThreading) ? "On" : "Off");

    /*D3DVIEWPORT9 vp; rs->GetViewport(&vp);
    float w = 256;
    float h = 256;
    RS_SPRITE spr[4];
    FillSpriteVertex(spr[0], 0, 0, 0.1f, 0xFFFFFFFF, 0.0f, 0.0f);
    FillSpriteVertex(spr[1], 0, h, 0.1f, 0xFFFFFFFF, 0.0f, 1.0f);
    FillSpriteVertex(spr[2], w, h, 0.1f, 0xFFFFFFFF, 1.0f, 1.0f);
    FillSpriteVertex(spr[3], w, 0, 0.1f, 0xFFFFFFFF, 1.0f, 0.0f);
    //rs->SetTexture(0, pReflection);
    rs->SetTexture(0, pReflectionSunroad);
    rs->DrawSprites(spr, 1, "Telescope");*/

    if (bUnderSea && bUnderSeaEnable)
    {
        CVECTOR d(20.0f, 5.0f, 20.0f);
        CVECTOR d2(54.0f, 10.0f, 54.0f);
        if (!bUnderSeaStarted)
        {
            // doing a bunch of sea trash for the first time
            aSeaTrash.clear();
            for (long i = 0; i < 1000; i++)
            {
                aSeaTrash.push_back(SeaTrash{});
                // SeaTrash & st = aSeaTrash[aSeaTrash.Add()];
                SeaTrash &st = aSeaTrash.back();
                st.vPos = vCamPos + CVECTOR(RRnd(-d.x, d.x), RRnd(-d.y, d.y), RRnd(-d.z, d.z));
                st.vSpeed = CVECTOR(0.1f, 0.0f, 0.0f);
                st.dwSubTexture = rand() % 4;
                st.fSize = RRnd(0.01f, 0.05f);
                st.fTime = static_cast<float>(i) * 0.02f;
            }

            // making a bunch of light poles for the first time
            aSeaLights.clear();
            for (long i = 0; i < 50; i++)
            {
                aSeaLights.push_back(SeaLight{});
                // SeaLight & sl = aSeaLights[aSeaLights.Add()];
                SeaLight &sl = aSeaLights.back();
                sl.vPos = vCamPos + CVECTOR(RRnd(-d2.x, d2.x), RRnd(-d2.y, d2.y), RRnd(-d2.z, d2.z));
                sl.vSpeed = CVECTOR(0.1f, 0.0f, 0.0f);
                sl.dwSubTexture = rand() % 4;
                sl.fTime = static_cast<float>(i);
            }
        }

        // add 1 item of sea trash every second
        fLastTrashTime += fDeltaTime;
        while (fLastTrashTime >= 0.03f)
        {
            fLastTrashTime -= 0.03f;

            aSeaTrash.push_back(SeaTrash{});
            // SeaTrash & st = aSeaTrash[aSeaTrash.Add()];
            SeaTrash &st = aSeaTrash.back();
            st.vPos = vCamPos + CVECTOR(RRnd(-d.x, d.x), RRnd(-d.y, d.y), RRnd(-d.z, d.z));
            st.vSpeed = CVECTOR(0.1f, 0.0f, 0.0f);
            st.dwSubTexture = rand() % 4;
            st.fTime = 20.0f;
            st.fSize = RRnd(0.01f, 0.05f);
        }

        // Remove the old sea trash
        for (long i = 0; i < aSeaTrash.size(); i++)
        {
            aSeaTrash[i].fTime -= fDeltaTime;
            if (aSeaTrash[i].fTime <= 0.0f)
            {
                // aSeaTrash.ExtractNoShift(i);
                aSeaTrash[i] = aSeaTrash.back();
                aSeaTrash.pop_back();
                i--;
                continue;
            }
            aSeaTrash[i].vPos += aSeaTrash[i].vSpeed * fDeltaTime;
        }
        // Render sea trash
        aTrashRects.clear();
        for (long i = 0; i < aSeaTrash.size(); i++)
        {
            //    RS_RECT & r = aTrashRects[aTrashRects.Add()];
            aTrashRects.push_back(RS_RECT{});
            RS_RECT &r = aTrashRects.back();

            float fAlpha = 1.0f;
            if (aSeaTrash[i].fTime >= 45.0f)
                fAlpha = 1.0f - (aSeaTrash[i].fTime - 45.0f) / 5.0f;
            if (aSeaTrash[i].fTime <= 5.0f)
                fAlpha = 1.0f - (5.0f - aSeaTrash[i].fTime) / 5.0f;

            r.vPos = aSeaTrash[i].vPos;
            r.fSize = aSeaTrash[i].fSize;
            r.fAngle = 0.0f;
            r.dwColor = ARGB(fAlpha * 255.0f, 255, 255, 255);
            r.dwSubTexture = aSeaTrash[i].dwSubTexture;
        }

        if (aTrashRects.size())
        {
            rs->TextureSet(0, iSeaTrashTexture);
            rs->DrawRects(&aTrashRects[0], aTrashRects.size(), "seatrash", 2, 2);
        }

        // Render schools of fish

        // draw light poles
        // add 1 light pole every second
        fLastLightTime += fDeltaTime;
        while (fLastLightTime >= 1.0f)
        {
            fLastLightTime -= 1.0f;

            aSeaLights.push_back(SeaLight{});
            // SeaLight & sl = aSeaLights[aSeaLights.Add()];
            SeaLight &sl = aSeaLights.back();
            sl.vPos = vCamPos + CVECTOR(RRnd(-d2.x, d2.x), RRnd(-d2.y, d2.y), RRnd(-d2.z, d2.z));
            sl.vSpeed = CVECTOR(0.1f, 0.0f, 0.0f);
            sl.dwSubTexture = rand() % 4;
            sl.fTime = 50.0f;
        }

        // Remove the old light poles
        for (long i = 0; i < aSeaLights.size(); i++)
        {
            aSeaLights[i].fTime -= fDeltaTime;
            if (aSeaLights[i].fTime <= 0.0f)
            {
                aSeaLights[i] = aSeaLights.back();
                aSeaLights.pop_back();
                i--;
                continue;
            }
            aSeaLights[i].vPos += aSeaLights[i].vSpeed * fDeltaTime;
        }

        float fAlphaK = (vCamPos.y < 0.0f) ? Max(0.0f, 1.0f + vCamPos.y / 30.0f) : 1.0f;

        // draw light poles
        aLightsRects.clear();
        for (long i = 0; i < aSeaLights.size(); i++)
        {
            aLightsRects.push_back(RS_RECT{});
            // RS_RECT & r = aLightsRects[aLightsRects.Add()];
            RS_RECT &r = aLightsRects.back();
            float fAlpha = 1.0f;
            if (aSeaLights[i].fTime >= 45.0f)
                fAlpha = 1.0f - (aSeaLights[i].fTime - 45.0f) / 5.0f;
            if (aSeaLights[i].fTime <= 5.0f)
                fAlpha = 1.0f - (5.0f - aSeaLights[i].fTime) / 5.0f;

            CVECTOR v = aSeaLights[i].vPos;
            r.vPos = CVECTOR(v.x, WaveXZ(v.x, v.z) - 5.0f, v.z);
            r.fSize = 20.0f;
            r.fAngle = 0.0f;
            r.dwColor = ARGB(0.07f * fAlphaK * fAlpha * 255.0f, 227, 245, 153);
            r.dwSubTexture = aSeaLights[i].dwSubTexture;
        }

        if (aLightsRects.size())
        {
            rs->TextureSet(0, iSeaLightTexture);
            rs->DrawRects(&aLightsRects[0], aLightsRects.size(), "seatrash", 2, 2, 0.5f);
        }

        bUnderSeaStarted = true;
    }

    bStarted = true;
}

float SEA::Trace(const CVECTOR &vSrc, const CVECTOR &vDst)
{
    const long iNumTests = 5;
    float fRes = 2.0f;
    const float fDV = 1.0f / static_cast<float>(iNumTests - 1);

    if (vSrc.y > fMaxSeaHeight && vDst.y > fMaxSeaHeight)
        return 2.0f;

    for (long i = 0; i < iNumTests; i++)
    {
        const CVECTOR vTemp = vSrc + static_cast<float>(i) * fDV * (vDst - vSrc);
        const float fWaveY = WaveXZ(vTemp.x, vTemp.z, nullptr);

        if (fWaveY > vTemp.y)
            return static_cast<float>(i) * fDV;
    }

    return 2.0f;
}

float SEA::Cannon_Trace(long iBallOwner, const CVECTOR &vSrc, const CVECTOR &vDst)
{
    const float fRes = Trace(vSrc, vDst);

    if (fRes <= 1.0f)
    {
        const CVECTOR vTemp = vSrc + fRes * (vDst - vSrc);
        const float fTmpY = WaveXZ(vTemp.x, vTemp.z, nullptr);
        core.Event(BALL_WATER_HIT, "lfff", iBallOwner, vTemp.x, fTmpY, vTemp.z);
    }

    return fRes;
}

uint32_t SEA::AttributeChanged(ATTRIBUTES *pAttribute)
{
    ATTRIBUTES *pParent = pAttribute->GetParent();
    ATTRIBUTES *pParent2 = (pParent) ? pParent->GetParent() : nullptr;

    const char *sName = pAttribute->GetThisName();
    const char *sValue = pAttribute->GetThisAttr();

    if (*pParent == "isDone")
    {
        Realize(0);
    }

    if (*pParent == "Sea2")
    {
        if (*pAttribute == "WaterColor")
        {
            v4SeaColor = COLOR2VECTOR4(pAttribute->GetAttributeAsDword());
            return 0;
        }
        if (*pAttribute == "SkyColor")
        {
            v4SkyColor = COLOR2VECTOR4(pAttribute->GetAttributeAsDword());
            return 0;
        }

        if (*pAttribute == "Amp1")
        {
            _fAmp1 = pAttribute->GetAttributeAsFloat();
            return 0;
        }
        if (*pAttribute == "AnimSpeed1")
        {
            fAnimSpeed1 = pAttribute->GetAttributeAsFloat();
            return 0;
        }
        if (*pAttribute == "Scale1")
        {
            fScale1 = pAttribute->GetAttributeAsFloat();
            return 0;
        }
        if (*pAttribute == "MoveSpeed1")
        {
            sscanf(pAttribute->GetThisAttr(), "%f, %f, %f", &vMoveSpeed1.x, &vMoveSpeed1.y, &vMoveSpeed1.z);
            return 0;
        }

        if (*pAttribute == "Amp2")
        {
            _fAmp2 = pAttribute->GetAttributeAsFloat();
            return 0;
        }
        if (*pAttribute == "AnimSpeed2")
        {
            fAnimSpeed2 = pAttribute->GetAttributeAsFloat();
            return 0;
        }
        if (*pAttribute == "Scale2")
        {
            fScale2 = pAttribute->GetAttributeAsFloat();
            return 0;
        }
        if (*pAttribute == "MoveSpeed2")
        {
            sscanf(pAttribute->GetThisAttr(), "%f, %f, %f", &vMoveSpeed2.x, &vMoveSpeed2.y, &vMoveSpeed2.z);
            return 0;
        }

        if (*pAttribute == "PosShift")
        {
            fPosShift = pAttribute->GetAttributeAsFloat();
            return 0;
        }

        if (*pAttribute == "BumpScale")
        {
            fBumpScale = pAttribute->GetAttributeAsFloat();
            return 0;
        }
        if (*pAttribute == "BumpSpeed")
        {
            fBumpSpeed = pAttribute->GetAttributeAsFloat();
            return 0;
        }
        if (*pAttribute == "LodScale")
        {
            fLodScale = pAttribute->GetAttributeAsFloat();
            return 0;
        }
        if (*pAttribute == "GridStep")
        {
            fGridStep = pAttribute->GetAttributeAsFloat();
            return 0;
        }

        if (*pAttribute == "Reflection")
        {
            v4SeaParameters.y = pAttribute->GetAttributeAsFloat();
            return 0;
        }
        if (*pAttribute == "Transparency")
        {
            v4SeaParameters.z = pAttribute->GetAttributeAsFloat();
            return 0;
        }
        if (*pAttribute == "Attenuation")
        {
            v4SeaParameters.x = pAttribute->GetAttributeAsFloat();
            return 0;
        }

        if (*pAttribute == "Frenel")
        {
            fFrenel = pAttribute->GetAttributeAsFloat();
            return 0;
        }

        if (*pAttribute == "FoamV")
        {
            fFoamV = pAttribute->GetAttributeAsFloat();
            return 0;
        }
        if (*pAttribute == "FoamK")
        {
            fFoamK = pAttribute->GetAttributeAsFloat();
            return 0;
        }
        if (*pAttribute == "FoamUV")
        {
            fFoamUV = pAttribute->GetAttributeAsFloat();
            return 0;
        }
        if (*pAttribute == "FoamTexDisturb")
        {
            fFoamTextureDisturb = pAttribute->GetAttributeAsFloat();
            return 0;
        }
        if (*pAttribute == "FoamEnable")
        {
            bFoamEnable = pAttribute->GetAttributeAsDword() != 0;
            return 0;
        }

        if (*pAttribute == "SimpleSea")
        {
            bSimpleSea = pAttribute->GetAttributeAsDword() != 0;
            BuildVolumeTexture();
            return 0;
        }
        return 0;
    }

    if (*pParent == "fog")
    {
        if (*pAttribute == "Enable")
        {
            bFogEnable = pAttribute->GetAttributeAsDword() != 0;
            return 0;
        }
        if (*pAttribute == "Start")
        {
            fFogStartDistance = pAttribute->GetAttributeAsFloat();
            return 0;
        }
        if (*pAttribute == "Color")
        {
            vFogColor = (1.0f / 255.0f) * COLOR2VECTOR(pAttribute->GetAttributeAsDword());
            return 0;
        }
        if (*pAttribute == "SeaDensity")
        {
            fFogSeaDensity = pAttribute->GetAttributeAsFloat();
            return 0;
        }
        return 0;
    }

    if (*pAttribute == "Stop")
    {
        bStop = AttributesPointer->GetAttributeAsDword("Stop") != 0;
    }

    if (*pAttribute == "UnderWater")
    {
        bUnderSeaEnable = AttributesPointer->GetAttributeAsDword("UnderWater") != 0;
    }

    if (*pAttribute == "MaxSeaHeight")
    {
        fMaxSeaHeight = AttributesPointer->GetAttributeAsFloat("MaxSeaHeight", 50.0f);

        const float fScale = (fMaxSeaHeight >= _fAmp1 + _fAmp2) ? 1.0f : fMaxSeaHeight / (_fAmp1 + _fAmp2);

        fAmp1 = _fAmp1 * fScale;
        fAmp2 = _fAmp2 * fScale;
    }

    return 0;
}

void SEA::LostRender()
{
    rs->Release(pReflection);
    rs->Release(pReflectionSunroad);
    rs->Release(pEnvMap);
    rs->Release(pSunRoadMap);
    rs->Release(pRenderTargetBumpMap);

    rs->Release(pZStencil);
    rs->Release(pReflectionSurfaceDepth);
}

void SEA::RestoreRender()
{
    rs->CreateTexture(XWIDTH, YWIDTH, MIPSLVLS, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT,
                      &pRenderTargetBumpMap);

    rs->CreateCubeTexture(128, 1, D3DUSAGE_RENDERTARGET, D3DFMT_R5G6B5, D3DPOOL_DEFAULT, &pEnvMap);
    rs->CreateCubeTexture(128, 1, D3DUSAGE_RENDERTARGET, D3DFMT_R5G6B5, D3DPOOL_DEFAULT, &pSunRoadMap);
    rs->CreateDepthStencilSurface(128, 128, D3DFMT_D24S8, D3DMULTISAMPLE_NONE, &pZStencil);

    rs->CreateTexture(128, 128, 1, D3DUSAGE_RENDERTARGET, D3DFMT_R5G6B5, D3DPOOL_DEFAULT, &pReflection);
    rs->CreateTexture(128, 128, 1, D3DUSAGE_RENDERTARGET, D3DFMT_R5G6B5, D3DPOOL_DEFAULT, &pReflectionSunroad);
    rs->CreateDepthStencilSurface(128, 128, D3DFMT_D24S8, D3DMULTISAMPLE_NONE, &pReflectionSurfaceDepth);
}
