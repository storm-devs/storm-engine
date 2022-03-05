//============================================================================================
//    Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
//    WdmIcon
//--------------------------------------------------------------------------------------------
//
//============================================================================================

#include "wdm_icon.h"
#include "wdm_enemy_ship.h"

// ============================================================================================
// Construction, destruction
// ============================================================================================

WdmIcon::WdmIcon()
{
    lastShip = nullptr;
    icon[0].u = -1.0f;
    icon[1].u = -1.0f;
    texture = wdmObjects->rs->TextureCreate("WorldMap\\Interfaces\\BigIcons.tga");
    islandAlpha = 0.0f;
    skipAlpha = 0.0f;
    stormAlpha = 0.0f;
}

WdmIcon::~WdmIcon()
{
    if (texture >= 0)
        wdmObjects->rs->TextureRelease(texture);
}

// Calculations
void WdmIcon::Update(float dltTime)
{
    if (lastShip != wdmObjects->enemyShip)
    {
        lastShip = wdmObjects->enemyShip;
        icon[1] = icon[0];
        if (lastShip != nullptr)
        {
            icon[0].alpha = 0.0f;
            switch (lastShip->shipType)
            {
            case wdmest_unknow:
                icon[0].u = 0.0f;
                break;
            case wdmest_merchant:
                icon[0].u = 1.0f / 8.0f;
                break;
            case wdmest_warring:
                icon[0].u = 2.0f / 8.0f;
                break;
            case wdmest_follow:
                icon[0].u = 3.0f / 8.0f;
                break;
            }
        }
        else
            icon[0].u = -1.0f;
    }
    icon[0].alpha += 2.0f * dltTime;
    if (icon[0].alpha > 1.0f)
        icon[0].alpha = 1.0f;
    icon[1].alpha -= 1.5f * dltTime;
    if (icon[1].alpha < 0.0f)
    {
        icon[1].u = -1.0f;
        icon[1].alpha = 0.0f;
    }
    if (wdmObjects->curIsland)
    {
        islandAlpha += 2.0f * dltTime;
        if (islandAlpha > 1.0f)
            islandAlpha = 1.0f;
    }
    else
    {
        islandAlpha -= 2.0f * dltTime;
        if (islandAlpha < 0.0f)
            islandAlpha = 0.0f;
    }
    if (wdmObjects->enableSkipEnemy)
    {
        skipAlpha += 2.0f * dltTime;
        if (skipAlpha > 1.0f)
            skipAlpha = 1.0f;
    }
    else
    {
        skipAlpha -= 2.0f * dltTime;
        if (skipAlpha < 0.0f)
            skipAlpha = 0.0f;
    }
    // Determine being in a storm
    if (wdmObjects->playarInStorm)
    {
        stormAlpha += 2.0f * dltTime;
        if (stormAlpha > 1.0f)
            stormAlpha = 1.0f;
    }
    else
    {
        stormAlpha -= 2.0f * dltTime;
        if (stormAlpha < 0.0f)
            stormAlpha = 0.0f;
    }
}

// Rendering
void WdmIcon::LRender(VDX9RENDER *rs)
{
    return; //~!~
    Vertex vrt[6];
    rs->TextureSet(0, texture);
    float w, h;
    wdmObjects->GetVPSize(w, h);
    // Icon in the upper corner
    auto kDef = rs->GetHeightDeformator();
    FillRectCoord(vrt, 8.0f, 8.0f, 64.0f, 64.0f * kDef);
    FillRectUV1(vrt, 0.0f, 0.0f, 1.0f, 1.0f);
    int32_t alpha;
    if (icon[1].u >= 0.0f)
    {
        alpha = static_cast<int32_t>(255.0f * icon[1].alpha);
        FillRectUV(vrt, icon[1].u, 0.0f, 1.0f / 8.0f, 1.0f);
        FillRectColor(vrt, (alpha << 24) | 0xffffff);
        DrawRects(vrt, 1, "WdmDrawIcon");
    }
    if (icon[0].u >= 0.0f)
    {
        alpha = static_cast<int32_t>(255.0f * icon[0].alpha);
        FillRectUV(vrt, icon[0].u, 0.0f, 1.0f / 8.0f, 1.0f);
        FillRectColor(vrt, (alpha << 24) | 0xffffff);
        DrawRects(vrt, 1, "WdmDrawIcon");
    }
    // Icon in the bottom corner
    if (islandAlpha > 0.0f)
    {
        FillRectCoord(vrt, 8.0f + 8.0f, h - 64.0f * kDef - 8.0f, 64.0f, 64.0f * kDef);
        FillRectUV(vrt, 0.0f, 0.0f, 1.0f / 8.0f, 1.0f);
        FillRectUV1(vrt, 0.0f, 0.0f, 1.0f, 1.0f);
        FillRectColor(vrt, (static_cast<int32_t>(islandAlpha * 255.0f) << 24) | 0xffffff);
        DrawRects(vrt, 1, "WdmDrawIcon");
    }
    // Encounter skip icon
    if (skipAlpha > 0.0f)
    {
        FillRectCoord(vrt, 8.0f + 64.0f + 8.0f, 8.0f, 64.0f, 64.0f * kDef);
        FillRectUV(vrt, 4.0f / 8.0f, 0.0f, 1.0f / 8.0f, 1.0f);
        FillRectUV1(vrt, 0.0f, 0.0f, 1.0f, 1.0f);
        FillRectColor(vrt, (static_cast<int32_t>(skipAlpha * 255.0f) << 24) | 0xffffff);
        DrawRects(vrt, 1, "WdmDrawIcon");
    }
    // Storm icon
    if (stormAlpha > 0.0f)
    {
        float move = 0.0f, m;
        if (icon[0].u >= 0.0f)
        {
            move = icon[0].alpha * 5.0f;
            if (move > 1.0f)
                move = 1.0f;
        }
        if (icon[1].u >= 0.0f)
        {
            m = icon[1].alpha * 5.0f;
            if (m > 1.0f)
                m = 1.0f;
            if (m > move)
                move = m;
        }
        m = skipAlpha * 5.0f;
        if (m > 1.0f)
            m = 1.0f;
        move += m;
        move *= 64.0f + 8.0f;

        FillRectCoord(vrt, 8.0f + move, 8.0f, 64.0f, 64.0f * kDef);
        FillRectUV(vrt, 5.0f / 8.0f, 0.0f, 1.0f / 8.0f, 1.0f);
        FillRectUV1(vrt, 0.0f, 0.0f, 1.0f, 1.0f);
        FillRectColor(vrt, (static_cast<int32_t>(stormAlpha * 255.0f) << 24) | 0xffffff);
        DrawRects(vrt, 1, "WdmDrawIcon");
    }
}
