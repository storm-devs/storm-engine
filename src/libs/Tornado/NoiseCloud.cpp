//============================================================================================
//    Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
//    NoiseCloud
//--------------------------------------------------------------------------------------------
//
//============================================================================================

#include "NoiseCloud.h"

//============================================================================================

#define TRND_CLDRADIUS 2000.0f

// ============================================================================================
// Construction, destruction
// ============================================================================================

NoiseCloud::NoiseCloud(Pillar &_pillar) : pillar(_pillar)
{
    for (long i = 0; i < sizeof(rect) / sizeof(Rect); i++)
    {
        rect[i].a = rand() * 2.0f * TRND_PI / RAND_MAX;
        rect[i].r = i * (TRND_CLDRADIUS / static_cast<float>(sizeof(rect) / sizeof(Rect) - 1));
        rect[i].angle = 0.0f;
        rect[i].size = TRND_CLDRADIUS * 0.4f;
        rect[i].alpha = 0.5f;
        rect[i].light = 0.5f;
        rect[i].kLight = -1.0f;
        rect[i].tLight = rand() * 10.0f / RAND_MAX;
    }
    texture = -1;
    lightCnt = 0;
    SetGlobalAlpha(1.0f);
}

NoiseCloud::~NoiseCloud()
{
}

void NoiseCloud::Update(float dltTime)
{
    for (long i = 0; i < sizeof(rect) / sizeof(Rect); i++)
    {
        if (rect[i].r >= TRND_CLDRADIUS)
            rect[i].r = 0.0f;
        const auto k = rect[i].r / TRND_CLDRADIUS;
        auto rot = dltTime * (2.0f - powf(k, 0.3f) * 2.2f);
        if (rot < 0.0f)
            rot = 0.0f;
        rect[i].a += rot;
        if (rect[i].a > 2.0f * TRND_PI)
            rect[i].a -= 2.0f * TRND_PI;
        rect[i].angle += rot;
        if (rect[i].angle > 2.0f * TRND_PI)
            rect[i].angle -= 2.0f * TRND_PI;
        rect[i].r += dltTime * TRND_CLDRADIUS * 0.1f;
        if (rect[i].r < TRND_CLDRADIUS * 0.2f)
            rect[i].alpha = rect[i].r / (TRND_CLDRADIUS * 0.2f);
        if (rect[i].r > TRND_CLDRADIUS * 0.6f)
            rect[i].alpha = 1.0f - (rect[i].r - TRND_CLDRADIUS * 0.6f) / (TRND_CLDRADIUS * 0.4f);
        if (rect[i].alpha < 0.0f)
            rect[i].alpha = 0.0f;
        if (rect[i].alpha > 1.0f)
            rect[i].alpha = 1.0f;
        if (rect[i].kLight < 0.0f)
        {
            rect[i].tLight += dltTime;
            if (lightCnt < 3 && rect[i].r < TRND_CLDRADIUS * 0.4f)
            {
                if (rect[i].tLight > 5.0f && (rand() & 127) == 7)
                {
                    rect[i].kLight = 0.0f;
                    rect[i].tLight = 0.0f;
                    lightCnt++;
                }
            }
        }
        else
        {
            rect[i].kLight += dltTime * 1.1f;
            if (rect[i].kLight < 1.0f)
            {
                rect[i].light = 0.2f + 0.8f * (1.0f - (rect[i].kLight - 0.5f) * (rect[i].kLight - 0.5f) * 4.0f);
            }
            else
            {
                rect[i].tLight = 0.0f;
                rect[i].kLight = -1.0f;
                rect[i].light = 0.5f;
                lightCnt--;
            }
        }
    }
}

void NoiseCloud::Draw(VDX9RENDER *rs)
{
    rs->TextureSet(0, texture);
    for (long i = 0; i < sizeof(rect) / sizeof(Rect); i++)
    {
        // The size
        const auto size = rect[i].size;
        // Position
        const auto sn = sinf(rect[i].angle);
        const auto cs = cosf(rect[i].angle);
        CVECTOR pos(pillar.X() + rect[i].r * sinf(rect[i].a), pillar.GetHeight(),
                    pillar.Z() + rect[i].r * cosf(rect[i].a));
        buf[i * 6 + 0].pos = pos + CVECTOR(size * (-cs + sn), 0.0f, size * (sn + cs));
        buf[i * 6 + 0].u = 0.0f;
        buf[i * 6 + 0].v = 0.0f;
        buf[i * 6 + 1].pos = pos + CVECTOR(size * (-cs - sn), 0.0f, size * (sn - cs));
        buf[i * 6 + 1].u = 0.0f;
        buf[i * 6 + 1].v = 1.0f;
        buf[i * 6 + 2].pos = pos + CVECTOR(size * (cs + sn), 0.0f, size * (-sn + cs));
        buf[i * 6 + 2].u = 1.0f;
        buf[i * 6 + 2].v = 0.0f;
        buf[i * 6 + 3].pos = buf[i * 6 + 2].pos;
        buf[i * 6 + 3].u = 1.0f;
        buf[i * 6 + 3].v = 0.0f;
        buf[i * 6 + 4].pos = buf[i * 6 + 1].pos;
        buf[i * 6 + 4].u = 0.0f;
        buf[i * 6 + 4].v = 1.0f;
        buf[i * 6 + 5].pos = pos + CVECTOR(size * (cs - sn), 0.0f, size * (-sn - cs));
        buf[i * 6 + 5].u = 1.0f;
        buf[i * 6 + 5].v = 1.0f;
        // Colour
        const auto clr = rect[i].light * 200.0f;
        const auto color = (static_cast<long>(clr * 1.0f) << 16) | (static_cast<long>(clr * 0.95f) << 8) |
                           static_cast<long>(clr * 0.9f) | (static_cast<long>(rect[i].alpha * galpha * 0.4f) << 24);
        buf[i * 6 + 0].color = color;
        buf[i * 6 + 1].color = color;
        buf[i * 6 + 2].color = color;
        buf[i * 6 + 3].color = color;
        buf[i * 6 + 4].color = color;
        buf[i * 6 + 5].color = color;
    }
    rs->DrawPrimitiveUP(D3DPT_TRIANGLELIST, D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1, 2 * sizeof(rect) / sizeof(Rect),
                        buf, sizeof(Vertex), "TornadoClouds");
}

void NoiseCloud::SetGlobalAlpha(float a)
{
    if (a < 0.0f)
        a = 0.0f;
    if (a > 1.0f)
        a = 1.0f;
    galpha = a * 255.0f;
}
