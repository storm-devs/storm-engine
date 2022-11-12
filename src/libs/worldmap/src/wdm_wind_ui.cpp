//============================================================================================
//    Spirenkov Maxim, 2005
//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
//    WdmWindUI
//--------------------------------------------------------------------------------------------
//
//============================================================================================

#include "wdm_wind_ui.h"

#include "core.h"

#include "wdm_player_ship.h"
#include "world_map.h"

// ============================================================================================
// Construction, destruction
// ============================================================================================

WdmWindUI::WdmWindUI()
{
    strcpy_s(month[0], "january");
    strcpy_s(month[1], "february");
    strcpy_s(month[2], "march");
    strcpy_s(month[3], "april");
    strcpy_s(month[4], "may");
    strcpy_s(month[5], "june");
    strcpy_s(month[6], "july");
    strcpy_s(month[7], "august");
    strcpy_s(month[8], "september");
    strcpy_s(month[9], "october");
    strcpy_s(month[10], "november");
    strcpy_s(month[11], "december");
    txBack = wdmObjects->rs->TextureCreate("WorldMap\\Interfaces\\back.tga");
    txSky = wdmObjects->rs->TextureCreate("WorldMap\\Interfaces\\sky.tga");
    txSkyMask = wdmObjects->rs->TextureCreate("WorldMap\\Interfaces\\sky_mask.tga");
    txBar = wdmObjects->rs->TextureCreate("WorldMap\\Interfaces\\bar.tga");
    txBarMask = wdmObjects->rs->TextureCreate("WorldMap\\Interfaces\\bar_mask.tga");
    txWindPointer = wdmObjects->rs->TextureCreate("WorldMap\\Interfaces\\wind_pointer.tga");
    txMorale = wdmObjects->rs->TextureCreate("WorldMap\\Interfaces\\morale.tga");
    txMoraleMask = wdmObjects->rs->TextureCreate("WorldMap\\Interfaces\\morale_mask.tga");
    txMoraleBar = wdmObjects->rs->TextureCreate("WorldMap\\Interfaces\\morale_bar.tga");
    txCoord = wdmObjects->rs->TextureCreate("WorldMap\\Interfaces\\coord.tga");
    nationFlagTx = wdmObjects->rs->TextureCreate("WorldMap\\Interfaces\\WorldMapEnsigns.tga");
    dateFont = -1;
    morale = 0.0f;
    resizeRatio = 1.0f;
}

WdmWindUI::~WdmWindUI()
{
    if (txBack >= 0)
        wdmObjects->rs->TextureRelease(txBack);
    if (txSky >= 0)
        wdmObjects->rs->TextureRelease(txSky);
    if (txSkyMask >= 0)
        wdmObjects->rs->TextureRelease(txSkyMask);
    if (txBar >= 0)
        wdmObjects->rs->TextureRelease(txBar);
    if (txBarMask >= 0)
        wdmObjects->rs->TextureRelease(txBarMask);
    if (txWindPointer >= 0)
        wdmObjects->rs->TextureRelease(txWindPointer);
    if (txMorale >= 0)
        wdmObjects->rs->TextureRelease(txMorale);
    if (txMoraleMask >= 0)
        wdmObjects->rs->TextureRelease(txMoraleMask);
    if (txMoraleBar >= 0)
        wdmObjects->rs->TextureRelease(txMoraleBar);
    if (txCoord >= 0)
        wdmObjects->rs->TextureRelease(txCoord);
    if (nationFlagTx >= 0)
        wdmObjects->rs->TextureRelease(nationFlagTx);
}

//============================================================================================

//============================================================================================

// Read font name
void WdmWindUI::SetAttributes(ATTRIBUTES *apnt)
{
    if (!apnt)
        return;
    auto *ap = apnt->FindAClass(apnt, "date");
    if (ap)
    {
        // Font
        const char *s = ap->GetAttribute("font");
        if (s && s[0])
            dateFont = wdmObjects->wm->GetRS()->LoadFont(s);
        const char *coordinate = ap->GetAttribute("coordinate");
        if (coordinate != nullptr)
        {
            strcpy_s(wdmObjects->stCoordinate, coordinate);
        }
        auto *a = ap->FindAClass(ap, "monthnames");
        if (a)
        {
            char buf[4];
            buf[0] = 'm';
            buf[3] = 0;
            for (int32_t i = 1; i <= 12; i++)
            {
                if (i < 10)
                    buf[1] = '0';
                else
                    buf[1] = '1';
                buf[2] = static_cast<char>('0' + (i % 10));
                s = a->GetAttribute(buf);
                if (s && s[0])
                {
                    int32_t sl = strlen(s) + 1;
                    if (sl > 128)
                        sl = 128;
                    memcpy(month[i - 1], s, sl);
                    month[i - 1][127] = 0;
                }
            }
        }
    }

    resizeRatio = wdmObjects->resizeRatio;
    // Screen parameters
    float w, h;
    wdmObjects->GetVPSize(w, h);
    // Center of the thing
    float cx = w - 128.0f * resizeRatio - 16.0f + 64.0f;
    float cy = -40.0f + 128.0f * resizeRatio;

    skyLeftPos = cx - 64.0f * resizeRatio;
    skyTopPos = cy - 64.0f * resizeRatio;
    skyWidth = 128.0f * resizeRatio;
    skyHeight = 128.0f * resizeRatio;
    skyColor = 0x80ffffff;

    windPointerLeftPos = cx - 16.0f * resizeRatio;
    windPointerTopPos = cy - 64.0f * resizeRatio;
    windPointerWidth = 32.0f * resizeRatio;
    windPointerHeight = 128.0f * resizeRatio;
    windPointerColor = 0xffffffff;

    windBarLeftPos = cx - 64.0f * resizeRatio;
    windBarTopPos = cy;
    windBarWidth = 128.0f * resizeRatio;
    windBarHeight = 128.0f * resizeRatio;
    windBarColor = 0xffffffff;

    frameLeftPos = cx - 64.0f * resizeRatio;
    frameTopPos = cy - 128.0f * resizeRatio;
    frameWidth = 128.0f * resizeRatio;
    frameHeight = 256.0f * resizeRatio;
    frameColor = 0xffffffff;

    int32_t font = dateFont >= 0 ? dateFont : FONT_DEFAULT;
    {
        int32_t fh = wdmObjects->rs->CharHeight(font);
        dateText.pRS = wdmObjects->rs;
        dateText.pARefresh = nullptr;
        dateText.nFont = font;
        dateText.fScale = resizeRatio;
        dateText.dwColor = 0xffffffff;
        dateText.pos.x = int32_t(cx);
        dateText.pos.y = int32_t(cy + (98.0f - fh * 0.5f) * resizeRatio);
    }

    // Centre
    cy += 128.0f * resizeRatio + 32.0f;

    moraleLeftPos = cx - 64.0f * resizeRatio;
    moraleTopPos = cy - 32.0f * resizeRatio;
    moraleWidth = 128.0f * resizeRatio;
    moraleHeight = 64.0f * resizeRatio;
    moraleColor = 0xffffffff;

    float foodRumSpacing = rum ? 24.0f : 0.0f;
    {
        foodText.pRS = wdmObjects->rs;
        foodText.pARefresh = nullptr;
        foodText.nFont = font;
        foodText.fScale = resizeRatio;
        foodText.dwColor = 0xffffffff;
        foodText.pos.x = int32_t(cx - foodRumSpacing * resizeRatio);
        foodText.pos.y = int32_t(cy + 30.0f * resizeRatio);
    }

    if (rum)
    {
        rumText.pRS = wdmObjects->rs;
        rumText.pARefresh = nullptr;
        rumText.nFont = font;
        rumText.fScale = resizeRatio;
        rumText.dwColor = 0xffffffff;
        rumText.pos.x = int32_t(cx + foodRumSpacing * resizeRatio);
        rumText.pos.y = int32_t(cy + 30.0f * resizeRatio);
    }

    coordLeftPos = cx - 64.0f * resizeRatio;
    coordTopPos = cy + 64.0f * resizeRatio;
    coordWidth = 128.0f * resizeRatio;
    coordHeight = 64.0f * resizeRatio;
    coordColor = 0xffffffff;

    {
        stCoordText.pRS = wdmObjects->rs;
        stCoordText.pARefresh = nullptr;
        stCoordText.nFont = font;
        stCoordText.fScale = resizeRatio;
        stCoordText.dwColor = 0xffffffff;
        stCoordText.pos.x = int32_t(cx);
        stCoordText.pos.y = int32_t(cy + (64.0f + 13.0f) * resizeRatio);
        stCoordText.sText = wdmObjects->stCoordinate;
    }

    {
        coordText.pRS = wdmObjects->rs;
        coordText.pARefresh = nullptr;
        coordText.nFont = font;
        coordText.fScale = resizeRatio;
        coordText.dwColor = 0xffffffff;
        coordText.pos.x = int32_t(cx);
        coordText.pos.y = int32_t(cy + (64.0f + 32.0f) * resizeRatio);
    }

    nationFlagWidth = 48.0f * resizeRatio;
    nationFlagHeight = 48.0f * resizeRatio;
    nationFlagLeftPos = cx - nationFlagWidth / 2.0f;
    nationFlagTopPos = cy + 150.0f * resizeRatio;
    nationFlagColor = 0xffffffff;
    ap = apnt->FindAClass(apnt, "nationFlag");
    if (ap)
    {
        const char *texName = ap->GetAttribute("texName");
        if (texName)
        {
            const auto newTxNationFlag = wdmObjects->rs->TextureCreate(texName);
            if (newTxNationFlag >= 0)
            {
                wdmObjects->rs->TextureRelease(nationFlagTx);
                nationFlagTx = newTxNationFlag;
            }
        }
        nationFlagCount = ap->GetAttributeAsDword("count", nationFlagCount);
        nationFlagWidth = ap->GetAttributeAsFloat("width", nationFlagWidth);
        nationFlagHeight = ap->GetAttributeAsFloat("height", nationFlagHeight);
    }
}

// Rendering
void WdmWindUI::LRender(VDX9RENDER *rs)
{
    if (wdmObjects->isNextDayUpdate)
    {
        auto *data = core.Event("WorldMap_GetMoral");
        if (data)
        {
            morale = data->GetFloat() * 0.02f - 1.0f;
        }
        data = core.Event("WorldMap_GetFood");
        if (data)
        {
            food = static_cast<int32_t>(data->GetFloat() + 0.5f);
            if (food < 0)
                food = 0;
        }
        data = core.Event("WorldMap_GetRum");
        if (data)
        {
            rum = static_cast<int32_t>(data->GetFloat() + 0.5f);
            if (rum < 0)
                rum = 0;
        }
        if (morale < -1.0f)
            morale = -1.0f;
        if (morale > 1.0f)
            morale = 1.0f;
    }
    // Player's wind parameters
    float x, y, ay;
    wdmObjects->playerShip->GetPosition(x, y, ay);
    CVECTOR windDir;
    resizeRatio = wdmObjects->resizeRatio;
    const auto widForce = wdmObjects->GetWind(x, y, windDir);
    const auto ang = static_cast<float>(atan2(windDir.x, windDir.z));
    // Buffer for drawing
    Vertex buf[(3 * 2) * 2];

    // Sky
    rs->TextureSet(0, txSky);
    rs->TextureSet(1, txSkyMask);
    FillRectCoord(buf, skyLeftPos, skyTopPos, skyWidth, skyHeight);

    FillRectUV(buf, (wdmObjects->wm->hour * (1.0f / 24.0f) - 0.125f), 0.0f, 0.25f, 1.0f);
    FillRectUV1(buf, 0.0f, 0.0f, 1.0f, 1.0f);
    FillRectColor(buf, skyColor);
    DrawRects(buf, 1, "WdmInterfaceDrawSky");

    // Direction of the wind
    rs->TextureSet(0, txWindPointer);
    FillRectCoord(buf, windPointerLeftPos, windPointerTopPos, windPointerWidth, windPointerHeight, ang);

    FillRectUV(buf, 0.0f, 0.0f, 1.0f, 1.0f);
    FillRectColor(buf, windPointerColor);
    DrawRects(buf, 1, "WdmDrawMapBlend");

    // Wind force
    rs->TextureSet(0, txBar);
    rs->TextureSet(1, txBarMask);
    FillRectCoord(buf, windBarLeftPos, windBarTopPos, windBarWidth, windBarHeight);

    FillRectUV(buf, 0.0f, 0.0f, 1.0f, 1.0f);
    FillRectUV1(buf, -0.5f, 0.0f, 1.0f, 1.0f);
    CMatrix rot(0.0f, 1.8f * (1.0f - widForce), 0.0f, 0.5f, 0.0f, 0.0f);
    for (int32_t i = 0; i < 6; i++)
    {
        CVECTOR v(buf[i].tu1, 0.0f, buf[i].tv1);
        const auto vrot = rot * v;
        buf[i].tu1 = vrot.x;
        buf[i].tv1 = vrot.z;
    }
    FillRectColor(buf, windBarColor);
    DrawRects(buf, 1, "WdmInterfaceDrawSky");

    // Frame
    rs->TextureSet(0, txBack);
    FillRectCoord(buf, frameLeftPos, frameTopPos, frameWidth, frameHeight);

    FillRectUV(buf, 0.0f, 0.0f, 1.0f, 1.0f);
    FillRectColor(buf, 0xffffffff);
    DrawRects(buf, 1, "WdmDrawMapBlend");

    // writing a date
    char tbuf[128];
    sprintf_s(tbuf, sizeof(tbuf) - 1, "%i %s %i", wdmObjects->wm->day, month[wdmObjects->wm->mon - 1],
              wdmObjects->wm->year);
    tbuf[sizeof(tbuf) - 1] = 0;
    dateText.Print(tbuf);

    // Drawing a moral bar
    rs->TextureSet(0, txMoraleBar);
    rs->TextureSet(1, txMoraleMask);
    FillRectCoord(buf, moraleLeftPos, moraleTopPos, moraleWidth, moraleHeight);

    FillRectUV(buf, morale * 0.28f, 0.0f, 1.0f, 1.0f);
    FillRectUV1(buf, 0.0f, 0.0f, 1.0f, 1.0f);
    FillRectColor(buf, moraleColor);
    DrawRects(buf, 1, "WdmInterfaceDrawSky");

    // Drawing a moral block
    rs->TextureSet(0, txMorale);
    FillRectCoord(buf, moraleLeftPos, moraleTopPos, moraleWidth, moraleHeight);

    FillRectUV(buf, 0.0f, 0.0f, 1.0f, 1.0f);
    FillRectColor(buf, moraleColor);
    DrawRects(buf, 1, "WdmDrawMapBlend");

    // write the number of supplies
    sprintf_s(tbuf, sizeof(tbuf) - 1, "%i%s", food > 99999 ? 99999 : food, food > 99999 ? "+" : "");
    tbuf[sizeof(tbuf) - 1] = 0;
    foodText.Print(tbuf);

    // write the amount of rum
    if (rum)
    {
        snprintf(tbuf, sizeof(tbuf) - 1, "%i", rum.value());
        tbuf[sizeof(tbuf) - 1] = 0;
        rumText.Print(tbuf);
    }

    if (!wdmObjects->coordinate.empty())
    {
        // Coordinate frame
        rs->TextureSet(0, txCoord);
        FillRectCoord(buf, coordLeftPos, coordTopPos, coordWidth, coordHeight);

        FillRectUV(buf, 0.0f, 0.0f, 1.0f, 1.0f);
        FillRectColor(buf, coordColor);
        DrawRects(buf, 1, "WdmDrawMapBlend");

        // display a line with coordinates
        stCoordText.Print(); // static label, should be set in scripts

        snprintf(tbuf, sizeof(tbuf) - 1, "%s", wdmObjects->coordinate.c_str());
        tbuf[sizeof(tbuf) - 1] = 0;
        coordText.Print(tbuf);
    }

    if (wdmObjects->nationFlagIndex)
    {
        // National flag
        rs->TextureSet(0, nationFlagTx);
        FillRectCoord(buf, nationFlagLeftPos, nationFlagTopPos, nationFlagWidth, nationFlagHeight);

        const float addtu = 1.0f / static_cast<float>(nationFlagCount);
        FillRectUV(buf, wdmObjects->nationFlagIndex.value() * addtu, 0.0f, addtu, 1.0f);
        FillRectColor(buf, nationFlagColor);
        DrawRects(buf, 1, "WdmDrawMapBlend");
    }
}
