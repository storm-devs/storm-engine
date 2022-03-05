//============================================================================================
//    Spirenkov Maxim, 2001
//--------------------------------------------------------------------------------------------
//    Sea Dogs II
//--------------------------------------------------------------------------------------------
//    WdmSea
//--------------------------------------------------------------------------------------------
//
//============================================================================================

#include "wdm_sea.h"
#include "matrix.h"
#include "wdm_ship.h"
#include "defines.h"
#include "storm_assert.h"

//============================================================================================

#define WDM_SEA_SECTIONS_X 16
#define WDM_SEA_SECTIONS_Z 16

#define WDM_SEA_NT (WDM_SEA_SECTIONS_X * WDM_SEA_SECTIONS_Z * 2)
#define WDM_SEA_NV ((WDM_SEA_SECTIONS_X + 1) * (WDM_SEA_SECTIONS_Z + 1))

//============================================================================================

#define WDM_SEA_MBR 0x20 // Brightness of reflections in the sea

#define WDM_SEA_ANIALPHA 0x80   // Animated texture mixing coefficient
#define WDM_SEA_ANIFPS 8        // Animated texture frame rate
#define WDM_SEA_ANITILING 90.0f // Number of tiled textures per side with a world size of 2000

//============================================================================================

#define WDM_SEA_CLR(a, c) (((uint32_t(a)) << 24) | ((uint32_t(c)) << 16) | ((uint32_t(c)) << 8) | ((uint32_t(c)) << 0))

// ============================================================================================
// Construction, destruction
// ============================================================================================

WdmSea::WdmSea()
{
    seaSizeX = wdmObjects->worldSizeX * 3.0f;
    seaSizeZ = wdmObjects->worldSizeZ * 3.0f;
    aniTiling =
        sqrtf(wdmObjects->worldSizeX * wdmObjects->worldSizeX + wdmObjects->worldSizeZ * wdmObjects->worldSizeZ);
    aniTiling *= WDM_SEA_ANITILING / (2000.0f * 1.414f);
    ib = -1;
    vb = -1;
    for (int32_t i = 0; i < sizeof(wh) / sizeof(WhiteHorses); i++)
        wh[i].textureIndex = -1;
    for (int32_t i = 0; i < sizeof(aniTextures) / sizeof(int32_t); i++)
        aniTextures[i] = -1;
    aniFrame = 0.0f;
    baseTexture = -1;
    for (int32_t i = 0; i < sizeof(whiteHorses) / sizeof(int32_t); i++)
        whiteHorses[i] = -1;
    // Init
    // Create a buffer for indexes
    ib = wdmObjects->rs->CreateIndexBuffer(WDM_SEA_NT * 3 * 2);
    // Create a vertex buffer
    vb = wdmObjects->rs->CreateVertexBuffer(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1,
                                            (WDM_SEA_NV + 4) * sizeof(Vertex), D3DUSAGE_WRITEONLY);
    Assert(ib >= 0 && vb >= 0);
    // Indexes
    auto *triangle = static_cast<Triangle *>(wdmObjects->rs->LockIndexBuffer(ib));
    Assert(triangle);
    for (int32_t j = 0, p = 0; j < WDM_SEA_SECTIONS_Z; j++)
    {
        auto *const trg = triangle + j * WDM_SEA_SECTIONS_X * 2;
        const auto base = j * (WDM_SEA_SECTIONS_X + 1);
        for (int32_t i = 0; i < WDM_SEA_SECTIONS_X; i++)
        {
            trg[i * 2 + 0].index[0] = static_cast<uint16_t>(base + i);
            trg[i * 2 + 0].index[1] = static_cast<uint16_t>(base + WDM_SEA_SECTIONS_X + 1 + i + 1);
            trg[i * 2 + 0].index[2] = static_cast<uint16_t>(base + WDM_SEA_SECTIONS_X + 1 + i);
            trg[i * 2 + 1].index[0] = static_cast<uint16_t>(base + i);
            trg[i * 2 + 1].index[1] = static_cast<uint16_t>(base + i + 1);
            trg[i * 2 + 1].index[2] = static_cast<uint16_t>(base + WDM_SEA_SECTIONS_X + 1 + i + 1);
        }
    }
    wdmObjects->rs->UnLockIndexBuffer(ib);
    // Vertices
    auto *vertex = static_cast<Vertex *>(wdmObjects->rs->LockVertexBuffer(vb));
    Assert(vertex);
    // The sea
    for (int32_t z = 0, p = 0; z <= WDM_SEA_SECTIONS_Z; z++)
    {
        for (int32_t x = 0; x <= WDM_SEA_SECTIONS_X; x++, p++)
        {
            auto &v = vertex[p];
            v.x = (x / static_cast<float>(WDM_SEA_SECTIONS_X) - 0.5f) * seaSizeX;
            v.y = 0.0f;
            v.z = (0.5f - z / static_cast<float>(WDM_SEA_SECTIONS_Z)) * seaSizeZ;
            v.color = 0xffffffff;
            v.tu = x / static_cast<float>(WDM_SEA_SECTIONS_X);
            v.tv = z / static_cast<float>(WDM_SEA_SECTIONS_Z);
        }
    }
    // For waves
    vertex[WDM_SEA_NV + 0].x = -0.5f;
    vertex[WDM_SEA_NV + 0].y = 0.0f;
    vertex[WDM_SEA_NV + 0].z = 0.5f;
    vertex[WDM_SEA_NV + 0].color = 0xffffffff;
    vertex[WDM_SEA_NV + 0].tu = 0.0f;
    vertex[WDM_SEA_NV + 0].tv = 0.0f;
    vertex[WDM_SEA_NV + 1].x = 0.5f;
    vertex[WDM_SEA_NV + 1].y = 0.0f;
    vertex[WDM_SEA_NV + 1].z = 0.5f;
    vertex[WDM_SEA_NV + 1].color = 0xffffffff;
    vertex[WDM_SEA_NV + 1].tu = 1.0f;
    vertex[WDM_SEA_NV + 1].tv = 0.0f;
    vertex[WDM_SEA_NV + 3].x = 0.5f;
    vertex[WDM_SEA_NV + 3].y = 0.0f;
    vertex[WDM_SEA_NV + 3].z = -0.5f;
    vertex[WDM_SEA_NV + 3].color = 0xffffffff;
    vertex[WDM_SEA_NV + 3].tu = 0.0f;
    vertex[WDM_SEA_NV + 3].tv = 1.0f;
    vertex[WDM_SEA_NV + 2].x = -0.5f;
    vertex[WDM_SEA_NV + 2].y = 0.0f;
    vertex[WDM_SEA_NV + 2].z = -0.5f;
    vertex[WDM_SEA_NV + 2].color = 0xffffffff;
    vertex[WDM_SEA_NV + 2].tu = 1.0f;
    vertex[WDM_SEA_NV + 2].tv = 1.0f;
    wdmObjects->rs->UnLockVertexBuffer(vb);
    // Loading textures
    char buf[256];
    baseTexture = wdmObjects->rs->TextureCreate("\\WorldMap\\Sea\\sea.tga");
    for (int32_t i = 0; i < sizeof(aniTextures) / sizeof(int32_t); i++)
    {
        sprintf_s(buf, "\\WorldMap\\Sea\\Ani\\wave_%.4i.tga", i);
        aniTextures[i] = wdmObjects->rs->TextureCreate(buf);
    }
    for (int32_t i = 0; i < sizeof(whiteHorses) / sizeof(int32_t); i++)
    {
        sprintf_s(buf, "\\WorldMap\\Sea\\WhiteHorses\\wh%.3i.tga", i);
        whiteHorses[i] = -1; // wdmObjects->rs->TextureCreate(buf);
    }
    for (int32_t i = 0; i < sizeof(flare) / sizeof(flare[0]); i++)
    {
        flare[i].index = -1;
    }
    flareTexture = wdmObjects->rs->TextureCreate("\\WorldMap\\Sea\\flare.tga");
    flareCount = 0;
    flareCerateCounter = 0.0f;
}

WdmSea::~WdmSea()
{
    if (ib >= 0)
        wdmObjects->rs->ReleaseIndexBuffer(ib);
    if (vb >= 0)
        wdmObjects->rs->ReleaseVertexBuffer(vb);
    for (int32_t i = 0; i < sizeof(aniTextures) / sizeof(int32_t); i++)
        if (aniTextures[i] >= 0)
            wdmObjects->rs->TextureRelease(aniTextures[i]);
    if (baseTexture >= 0)
        wdmObjects->rs->TextureRelease(baseTexture);
    if (flareTexture >= 0)
        wdmObjects->rs->TextureRelease(flareTexture);
    ib = -1;
    vb = -1;
    for (int32_t i = 0; i < sizeof(wh) / sizeof(WhiteHorses); i++)
        wh[i].textureIndex = -1;
    for (int32_t i = 0; i < sizeof(aniTextures) / sizeof(int32_t); i++)
        aniTextures[i] = -1;
    aniFrame = 0.0f;
    baseTexture = -1;
    for (int32_t i = 0; i < sizeof(whiteHorses) / sizeof(int32_t); i++)
        whiteHorses[i] = -1;
}

void WdmSea::Update(float dltTime)
{
    // Animated texture
    aniFrame += dltTime * WDM_SEA_ANIFPS;
    const float maxAni = sizeof(aniTextures) / sizeof(int32_t);
    aniFrame /= maxAni;
    aniFrame = (aniFrame - static_cast<int32_t>(aniFrame)) * maxAni;
    Assert(aniFrame < maxAni);
    // waves
    for (int32_t i = 0; i < sizeof(wh) / sizeof(WhiteHorses); i++)
    {
        if (wh[i].textureIndex >= 0)
        {
            // 1 Move spawned
            wh[i].curTime += wh[i].kTime * dltTime;
            if (wh[i].curTime >= 1.0f)
            {
                wh[i].textureIndex = -1;
                continue;
            }
            wh[i].x += wh[i].speed * dltTime * sinf(wh[i].angle);
            wh[i].z += wh[i].speed * dltTime * cosf(wh[i].angle);
        }
        else
        {
            // 2 spawn new waves
            if (rand() & 255)
                continue;
            // Texture
            wh[i].textureIndex = rand() % (sizeof(whiteHorses) / sizeof(int32_t));
            // Position
            wh[i].x = seaSizeX * (0.5f - rand() * (1.0f / RAND_MAX));
            wh[i].z = seaSizeZ * (0.5f - rand() * (1.0f / RAND_MAX));
            // Direction
            wh[i].angle = 3.1415f * 2.0f * rand() * (1.0f / RAND_MAX);
            // Speed
            wh[i].speed = 0.01f * (seaSizeX + seaSizeZ) * 0.5f * (0.001f + rand() * (0.01f / RAND_MAX));
            // The size
            wh[i].size = (seaSizeX + seaSizeZ) * 0.5f * (0.1f + rand() * (0.3f / RAND_MAX));
            // Lifetime
            wh[i].curTime = 0.0f;
            wh[i].kTime = 1.0f / (10.0f + rand() * (30.0f / RAND_MAX));
        }
    }
    // Flares
    int32_t i;
    for (i = 0; i < sizeof(flare) / sizeof(flare[0]); i++)
    {
        auto &f = flare[i];
        if (f.index < 0)
            continue;
        f.time += dltTime * f.k;
        if (f.time >= 1.0f)
        {
            // delete
            flareCount--;
            for (auto j = f.index; j < flareCount; j++)
            {
                flare[flareRect[j + 1].dwColor & 0xffff].index--;
                flareRect[j] = flareRect[j + 1];
            }
            f.index = -1;
            continue;
        }
        // Updating parameters
        auto &r = flareRect[f.index];
        const auto k = 1.0f - (f.time - 0.5f) * (f.time - 0.5f) * 4.0f;
        r.fSize = k * 0.3f;
        r.dwColor = (r.dwColor & 0xffff) | (static_cast<int32_t>(k * k * k * k * k * k * 255.0f) << 24);
        r.fAngle += dltTime * 8.0f * sinf(f.phase + f.time * 6.0f);
    }
    Assert(wdmObjects->playerShip);
    float playerX, playerZ, playerAng;
    wdmObjects->playerShip->GetPosition(playerX, playerZ, playerAng);
    const auto flareBurnTime = 0.0005f;
    for (flareCerateCounter += dltTime; flareCerateCounter > flareBurnTime; flareCerateCounter -= flareBurnTime)
    {
        if (rand() & 1)
        {
            // Add new
            unsigned int i;
            for (i = 0; i < sizeof(flare) / sizeof(flare[0]); i++)
            {
                if (flare[i].index < 0)
                    break;
            }
            if (i >= sizeof(flare) / sizeof(flare[0]))
                break;
            Assert(flareCount < sizeof(flareRect) / sizeof(flareRect[0]));
            // Fill in with new parameters
            auto &f = flare[i];
            f.index = flareCount++;
            f.time = 0.0f;
            f.k = 1.0f / (0.8f + 0.5f * rand() * (1.0f / RAND_MAX));
            f.phase = rand() * (2.0f * PI / RAND_MAX);
            auto &r = flareRect[f.index];
            const auto pang = rand() * (2.0f * PI / RAND_MAX);
            const auto prad = rand() * (300.0f / RAND_MAX);
            r.vPos.x = playerX + prad * sinf(pang);
            r.vPos.y = 0.0f;
            r.vPos.z = playerZ + prad * cosf(pang);
            r.fSize = 0.0f;
            r.fAngle = rand() * (6.28f / RAND_MAX);
            r.dwColor = i; // Only alpha is used, in color the index of the control structure
            r.dwSubTexture = 0;
        }
    }
}

void WdmSea::PRender(VDX9RENDER *rs)
{
    // Preliminary background rendering
    // Textures
    rs->TextureSet(0, baseTexture);
    PresetMain(rs);
    // Render
    Render(rs, "WdmSeaDraw1");
}

void WdmSea::LRender(VDX9RENDER *rs)
{
    // Further background rendering
    // Textures
    rs->TextureSet(0, baseTexture);
    PresetMain(rs);
    // RenderState
    rs->SetRenderState(D3DRS_TEXTUREFACTOR, WDM_SEA_CLR(0xff - WDM_SEA_MBR, 0xff));
    // Render
    Render(rs, "WdmSeaDraw2");
    // Drawing an animated texture
    // Determine a pair of frames and the blending coefficient between them
    const auto curFrame = static_cast<int32_t>(aniFrame);
    auto nextFrame = curFrame + 1;
    if (nextFrame >= sizeof(aniTextures) / sizeof(int32_t))
        nextFrame = 0;
    auto k = 255.0f * (aniFrame - static_cast<int32_t>(aniFrame));
    if (k < 0.0f)
        k = 0.0f;
    if (k > 255.0f)
        k = 255.0f;
    // Textures
    rs->TextureSet(0, aniTextures[curFrame]);
    rs->TextureSet(1, aniTextures[nextFrame]);
    // Matrix for textures
    CMatrix mtx;
    mtx.m[0][0] = aniTiling;
    mtx.m[1][1] = aniTiling;
    mtx.m[2][2] = aniTiling;
    mtx.Pos() = 0.0f;
    rs->SetTransform(D3DTS_TEXTURE0, mtx);
    rs->SetTransform(D3DTS_TEXTURE1, mtx);
    rs->SetRenderState(D3DRS_TEXTUREFACTOR, WDM_SEA_CLR(WDM_SEA_ANIALPHA, k));
    Render(rs, "WdmSeaDraw3");
    // draw flares
    if (flareCount > 0)
    {
        CMatrix view;
        rs->GetTransform(D3DTS_VIEW, view);
        view.Transposition();
        auto y = view.Pos().y;
        const auto maxy = 130.0f;
        const auto miny = 80.0f;
        if (y < maxy)
        {
            y = (y - miny) / (maxy - miny);
            if (y < 0.0f)
                y = 0.0f;
            y = (1.0f - y) * (1.0f - y);
            const auto c = static_cast<int32_t>(y * 255.0f) << 24;
            rs->TextureSet(0, flareTexture);
            rs->SetRenderState(D3DRS_TEXTUREFACTOR, (c << 24) | (c << 16) | (c << 8) | c);
            rs->DrawRects(flareRect, flareCount, "WdmSeaDrawFlare");
        }
    }
}

void WdmSea::Render(VDX9RENDER *rs, const char *tech) const
{
    static CMatrix identity;
    if (vb < 0 || ib < 0)
        return;
    rs->SetTransform(D3DTS_WORLD, identity);
    rs->DrawBuffer(vb, sizeof(Vertex), ib, 0, WDM_SEA_NV, 0, WDM_SEA_NT, (char *)tech);
}

// Adjust texture coordinate transformations
void WdmSea::PresetMain(VDX9RENDER *rs) const
{
    // Matrix for textures
    CMatrix mtx;
    mtx.m[0][0] = seaSizeX / wdmObjects->worldSizeX;
    mtx.m[1][1] = mtx.m[0][0];
    mtx.m[2][0] = (1.0f - mtx.m[0][0]) * 0.5f;
    mtx.m[2][1] = (1.0f - mtx.m[0][0]) * 0.5f;
    rs->SetTransform(D3DTS_TEXTURE0, mtx);
}
