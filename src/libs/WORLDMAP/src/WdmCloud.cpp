//============================================================================================
//    Spirenkov Maxim, 2001
//--------------------------------------------------------------------------------------------
//    Sea Dogs II
//--------------------------------------------------------------------------------------------
//    WdmCloud
//--------------------------------------------------------------------------------------------
//
//============================================================================================

#include "WdmCloud.h"
#include "WdmObjects.h"
#include "defines.h"

#define WdmStormCloudHeight 40.0f
#define WdmStormSizeMin 40.0f
#define WdmStormSizeMax 60.0f

IDirect3DVertexDeclaration9 *WdmCloud::vertexDecl_ = nullptr;

// ============================================================================================
// Construction, destruction
// ============================================================================================

WdmCloud::WdmCloud()
{
    curMove = 0;
    globalAlpha = 1.0f;
    constAlpha = 0.5f;
    dir = 0.0f;
    texture = -1;
    BuildCloud(4 + (rand() & 3));
    // Init
    texture = wdmObjects->rs->TextureCreate("\\WorldMap\\clouds.tga");
    light = wdmObjects->rs->TextureCreate("\\WorldMap\\stormlight.tga");
    lightning = wdmObjects->rs->TextureCreate("\\WorldMap\\lightnings.tga");
    //
    constAlpha = 1.0f;
    lightningWaitTime = 0.0f;
    curLightning = -1;
    // Rain
    for (long i = 0; i < sizeof(rain) / sizeof(rain[0]); i++)
    {
        rain[i].isLive = false;
    }
    rainBurnTime = 0.0f;
}

WdmCloud::~WdmCloud()
{
    if (texture >= 0)
        wdmObjects->rs->TextureRelease(texture);
    if (light >= 0)
        wdmObjects->rs->TextureRelease(light);
    if (lightning >= 0)
        wdmObjects->rs->TextureRelease(lightning);
}

// Calculations
void WdmCloud::Update(float dltTime)
{
    const auto pi2 = 2.0f * 3.14159265358979323846f;
    if (dltTime > 1.0f)
        dltTime = 1.0f;

    if (GetKeyState(VK_NUMLOCK) != 0)
    {
        dltTime = 0.0f;
    }

    // Moving the cloud
    Move(dltTime);
    // Time to move particles inside the cloud
    const auto dlt = dltTime * 0.1f;
    for (long i = 0; i < numRects; i++)
    {
        rect[i].vPos = pos + move[i].pos;
        rect[i].vPos.y = WdmStormCloudHeight;
        // Rotating the Particle
        rect[i].fAngle += move[i].rotSpd * dlt;
        if (rect[i].fAngle > pi2)
            rect[i].fAngle -= pi2;
        if (rect[i].fAngle < -pi2)
            rect[i].fAngle += pi2;
        // Set the color
        auto c = ~move[i].pos * 1.8f;
        if (c > 255.0f)
            c = 255.0f;
        if (c < 40.0f)
            c = 40.0f;
        c *= constAlpha * globalAlpha;
        rect[i].dwColor = (static_cast<long>(c) << 24) | (rect[i].dwColor & 0xffffff);
        // Moving the Particle
        move[i].dTime += dlt;
        move[i].pos += move[i].v * dlt;
    }
    if (curMove >= numRects)
        curMove = 0;
    // Particle speed
    const auto i = curMove++;
    // Center impact
    dltTime = move[i].dTime;
    move[i].dTime = 0.0f;
    auto d = ~move[i].pos;
    if (d < 100.0f)
    {
        move[i].v += (move[i].pos - move[i].cent) * (move[i].kSpd * dltTime);
        move[i].v.x += move[i].dx;
        move[i].v.z += move[i].dz;
    }
    else
    {
        if (d > 250.0f)
        {
            move[i].v -= (move[i].pos - move[i].cent) * (0.5f * move[i].kSpd * dltTime);
        }
        // Decay rate
        d = 0.01f * dltTime;
        if (d > 1.0f)
            d = 1.0f;
        move[i].v -= move[i].v * d;
    }
    // Pushing effect
    auto l = i - 1;
    if (l < 0)
        l = numRects - 1;
    auto v = move[i].pos - move[l].pos;
    v.y = 0.0f;
    d = ~v;
    if (d > 0.0000001f && d < 40.0f)
        move[i].v += v * (0.8f * move[i].kSpd * dltTime / d);
    l = i + 1;
    if (l >= numRects)
        l = 0;
    v = move[i].pos - move[l].pos;
    v.y = 0.0f;
    d = ~v;
    if (d > 0.0000001f && d < 40.0f)
        move[i].v += v * (0.8f * move[i].kSpd * dltTime / d);
    // generate lightning
    if (curLightning < 0)
    {
        lightningWaitTime += dltTime * 0.001f;
        if (lightningWaitTime > 0.001f + rand() * 1.0f / RAND_MAX)
        {
            // time to spawn a new lightning
            curLightning = rand() % numRects;
            lightningTime = 0.3f;
            lastColor = rect[curLightning].dwColor;
            lightningColor = rect[curLightning].dwColor | 0x0000fc00 | ((rand() & 3) << 8);
            rect[curLightning].dwColor = lightningColor;
            flashTime = 0.0f;
        }
    }
    if (curLightning >= 0)
    {
        flashTime += dltTime;
        if (flashTime > 0.05f)
        {
            rect[curLightning].dwColor = (rand() & 1) ? lightningColor : lastColor;
            flashTime = 0.0f;
        }
        lightningTime -= dltTime;
        if (lightningTime < 0.0f)
        {
            rect[curLightning].dwColor = lastColor;
            lightningWaitTime = 0.0f;
            curLightning = -1;
        }
    }
    // Rain
    for (long i = 0; i < sizeof(rain) / sizeof(rain[0]); i++)
    {
        auto &r = rain[i];
        if (!r.isLive)
            continue;
        if (r.pos.y > 3.0f)
        {
            r.vy += dltTime * 60.0f;
            r.pos.y -= r.vy * dltTime;
            r.alpha += dltTime * 1.0f;
            if (r.alpha > 1.0f)
                r.alpha = 1.0f;
        }
        else
        {
            r.alpha -= dltTime * 1.0f;
            r.size += dltTime * 8.0f;
            auto k = dltTime * 2.0f;
            if (k > 1.0f)
                k = 1.0f;
            r.vy -= k * r.vy;
            r.pos.y -= r.vy * dltTime;
            if (r.alpha < 0.0f)
            {
                r.alpha = 0.0f;
                r.isLive = false;
            }
        }
    }
    rainBurnTime -= dltTime;
    if (rainBurnTime <= 0.0f)
    {
        for (long i = 0; i < sizeof(rain) / sizeof(rain[0]); i++)
        {
            auto &r = rain[i];
            if (!r.isLive)
            {
                rainBurnTime += 0.01f;
                const auto p = rand() % numRects;
                r.isLive = true;
                r.pos = rect[p].vPos;
                r.pos.y -= 10.0f;
                r.alpha = 0.001f;
                r.vy = 60.0f;
                r.size = rect[p].fSize * 0.7f;
                r.angle = rand() * (2.0f * PI / RAND_MAX);
                break;
            }
        }
    }
}

void WdmCloud::PRender(VDX9RENDER *rs)
{
    //    LRender(rs);
}

void WdmCloud::LRender(VDX9RENDER *rs)
{
    Render(rs);
}

void WdmCloud::Move(float dltTime)
{
    // Moving the cloud
    pos += dir * dltTime;
}

long WdmCloud::FillRain(RS_RECT *rainRect, long rcnt)
{
    // Draw rain
    for (long i = 0; i < sizeof(rain) / sizeof(rain[0]); i++)
    {
        auto &r = rain[i];
        if (!r.isLive)
            continue;
        auto &rct = rainRect[rcnt];
        rct.vPos = r.pos;
        rct.fSize = r.size;
        rct.fAngle = r.angle;
        rct.dwSubTexture = 0;
        rct.dwColor = (static_cast<uint8_t>(r.alpha * 95.0f * globalAlpha) << 24) | 0x00ffffff;
        rcnt++;
    }
    return rcnt;
}

void WdmCloud::Render(VDX9RENDER *rs)
{
    // Inverse camera matrix
    CMatrix view;
    rs->GetTransform(D3DTS_VIEW, view);
    view.Transposition();
    // Draw lightning if needed
    Vertex lght[4];
    rs->TextureSet(0, lightning);
    const uint32_t lightningColor = (static_cast<uint8_t>(globalAlpha * 255.0f) << 24) | 0x00ffffff;
    for (long i = 0; i < numRects; i++)
    {
        auto &r = rect[i];
        if (r.dwColor & 0x0000ff00)
        {
            if (r.dwColor & 0x00000400)
            {
                const auto kFrames = 1.0f / 4.0f;
                const auto width = 10.0f;
                const auto u = ((r.dwColor >> 8) & 3) * kFrames;
                auto &vx = view.Vx();
                lght[0].pos = r.vPos - vx * width - CVECTOR(0.0f, 3.0f, 0.0f);
                lght[0].c = lightningColor;
                lght[0].u = u;
                lght[0].v = 0.01f;
                lght[1].pos = r.vPos + vx * width - CVECTOR(0.0f, 3.0f, 0.0f);
                lght[1].c = lightningColor;
                lght[1].u = u + kFrames;
                lght[1].v = 0.01f;
                lght[2].pos = lght[0].pos;
                lght[2].pos.y = 0.5f;
                lght[2].c = lightningColor;
                lght[2].u = u;
                lght[2].v = 0.99f;
                lght[3].pos = lght[1].pos;
                lght[3].pos.y = 0.5f;
                lght[3].c = lightningColor;
                lght[3].u = u + kFrames;
                lght[3].v = 0.99f;
                rs->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1, 2, lght,
                                    sizeof(Vertex), "WdmLightning");
            }
            r.dwColor = (r.dwColor & ~0x00000400) | ((rand() & 1) ? 0x00000400 : 0);
        }
    }

    CreateVertexDeclaration(rs);
    rs->SetVertexDeclaration(vertexDecl_);

    // Textures
    rs->TextureSet(0, texture);
    rs->TextureSet(1, light);
    // Constants
    CMatrix prj;
    rs->GetTransform(D3DTS_PROJECTION, prj);
    rs->SetVertexShaderConstantF(0, prj, 4);
    prj.matrix[0] = view.matrix[1];
    prj.matrix[1] = view.matrix[5];
    prj.matrix[2] = view.matrix[9];
    prj.matrix[3] = view.matrix[13] - WdmStormCloudHeight;
    prj.matrix[4] = WdmStormSizeMin * 1.4f;
    prj.matrix[5] = 0.0f;
    prj.matrix[6] = (WdmStormSizeMax - WdmStormSizeMin) * 1.4f;
    prj.matrix[7] = 0.5f;
    prj.matrix[8] = 0.8f;
    prj.matrix[9] = 0.6f;
    prj.matrix[10] = 1.0f;
    prj.matrix[11] = 1.0f;
    rs->SetVertexShaderConstantF(4, prj, 3);
    // Render
    rs->DrawRects(rect, numRects, "WdmClouds", 2, 2);
}

void WdmCloud::BuildCloud(long n)
{
    curMove = 0;
    numRects = 0;
    for (long i = 0; i < n; i++, numRects++)
    {
        FindPartPos(move[i].pos);
        const uint32_t sz = rand() & 0xff;
        rect[i].fSize = WdmStormSizeMin + (WdmStormSizeMax - WdmStormSizeMin) * (sz / 255.0f);
        const uint32_t clr = rand() & 0xf;
        rect[i].dwColor = 0xf0f00000 | (clr << 24) | (clr << 16) | sz;
        rect[i].fAngle = 0.0f;
        rect[i].dwSubTexture = rand() & 3;
        move[i].rotSpd = 0.8f * Rnd();
        move[i].v = 0.0f;
        move[i].kSpd = 5.0f * (0.01f + rand() * 0.4f / RAND_MAX);
        move[i].dx = 0.0001f * (0.1f + rand() * 1.0f / RAND_MAX);
        move[i].dz = 0.0001f * (0.1f + rand() * 1.0f / RAND_MAX);
        move[i].cent = move[i].pos * (0.8f + rand() * 0.4f / RAND_MAX);
        if (rand() & 1)
            move[i].dx = -move[i].dx;
        if (rand() & 1)
            move[i].dz = -move[i].dz;
        move[i].dTime = 0.0f;
    }
    pos = 0.0f;
    dir = 0.0f;
}

void WdmCloud::FindPartPos(CVECTOR &v)
{
    v.x = 20.0f * Rnd();
    v.y = 5.0f * Rnd();
    v.z = 20.0f * Rnd();
}

inline float WdmCloud::Rnd()
{
    auto f = powf(rand() * 1.0f / RAND_MAX, 0.1f);
    if (rand() & 1)
        f = -f;
    return f;
}

void WdmCloud::CreateVertexDeclaration(VDX9RENDER *rs)
{
    if (vertexDecl_ != nullptr)
        return;

    const D3DVERTEXELEMENT9 VertexElements[] = {
        {0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
        {0, 12, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0},
        {0, 16, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
        D3DDECL_END()};

    rs->CreateVertexDeclaration(VertexElements, &vertexDecl_);
}
