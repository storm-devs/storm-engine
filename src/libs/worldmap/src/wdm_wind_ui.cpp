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
    dateFont = -1;
    morale = 0.0f;
    resizeRatio = 1.0f;
}

WdmWindUI::~WdmWindUI()
{
    if (frameTx >= 0)
        wdmObjects->rs->TextureRelease(frameTx);
    if (skyTx >= 0)
        wdmObjects->rs->TextureRelease(skyTx);
    if (skyMaskTx >= 0)
        wdmObjects->rs->TextureRelease(skyMaskTx);
    if (windBarTx >= 0)
        wdmObjects->rs->TextureRelease(windBarTx);
    if (windBarMaskTx >= 0)
        wdmObjects->rs->TextureRelease(windBarMaskTx);
    if (windPointerTx >= 0)
        wdmObjects->rs->TextureRelease(windPointerTx);
    if (moraleTx >= 0)
        wdmObjects->rs->TextureRelease(moraleTx);
    if (moraleMaskTx >= 0)
        wdmObjects->rs->TextureRelease(moraleMaskTx);
    if (moraleBarTx >= 0)
        wdmObjects->rs->TextureRelease(moraleBarTx);
    if (coordTx >= 0)
        wdmObjects->rs->TextureRelease(coordTx);
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
    const char *txPath;
    const char *barTxPath;
    const char *maskTxPath;
    // Screen parameters
    float w, h;
    wdmObjects->GetVPSize(w, h);
    // Center of the thing
    float cx = w - 128.0f * resizeRatio - 16.0f + 64.0f;
    float cy = -40.0f + 128.0f * resizeRatio;

    txPath = "WorldMap\\Interfaces\\sky.tga";
    maskTxPath = "WorldMap\\Interfaces\\sky_mask.tga";
    skyLeftPos = cx - 64.0f * resizeRatio;
    skyTopPos = cy - 64.0f * resizeRatio;
    skyWidth = 128.0f * resizeRatio;
    skyHeight = 128.0f * resizeRatio;
    skyColor = 0x80ffffff;
    ap = apnt->FindAClass(apnt, "sky");
    if (ap)
    {
        txPath = BIUtils::GetStringFromAttr(ap, "texture", txPath);
        maskTxPath = BIUtils::GetStringFromAttr(ap, "maskTexture", maskTxPath);
        skyLeftPos = ap->GetAttributeAsFloat("leftPos", skyLeftPos);
        skyTopPos = ap->GetAttributeAsFloat("topPos", skyTopPos);
        skyWidth = ap->GetAttributeAsFloat("width", skyWidth);
        skyHeight = ap->GetAttributeAsFloat("height", skyHeight);
        skyColor = ap->GetAttributeAsDword("color", skyColor);
    }
    skyTx = wdmObjects->rs->TextureCreate(txPath);
    skyMaskTx = wdmObjects->rs->TextureCreate(maskTxPath);

    txPath = "WorldMap\\Interfaces\\wind_pointer.tga";
    windPointerLeftPos = cx - 16.0f * resizeRatio;
    windPointerTopPos = cy - 64.0f * resizeRatio;
    windPointerWidth = 32.0f * resizeRatio;
    windPointerHeight = 128.0f * resizeRatio;
    windPointerColor = 0xffffffff;
    ap = apnt->FindAClass(apnt, "windPointer");
    if (ap)
    {
        txPath = BIUtils::GetStringFromAttr(ap, "texture", txPath);
        windPointerLeftPos = ap->GetAttributeAsFloat("leftPos", windPointerLeftPos);
        windPointerTopPos = ap->GetAttributeAsFloat("topPos", windPointerTopPos);
        windPointerWidth = ap->GetAttributeAsFloat("width", windPointerWidth);
        windPointerHeight = ap->GetAttributeAsFloat("height", windPointerHeight);
        windPointerColor = ap->GetAttributeAsDword("color", windPointerColor);
    }
    windPointerTx = wdmObjects->rs->TextureCreate(txPath);

    txPath = "WorldMap\\Interfaces\\bar.tga";
    maskTxPath = "WorldMap\\Interfaces\\bar_mask.tga";
    windBarLeftPos = cx - 64.0f * resizeRatio;
    windBarTopPos = cy;
    windBarWidth = 128.0f * resizeRatio;
    windBarHeight = 128.0f * resizeRatio;
    windBarColor = 0xffffffff;
    ap = apnt->FindAClass(apnt, "windBar");
    if (ap)
    {
        txPath = BIUtils::GetStringFromAttr(ap, "texture", txPath);
        maskTxPath = BIUtils::GetStringFromAttr(ap, "maskTexture", maskTxPath);
        windBarLeftPos = ap->GetAttributeAsFloat("leftPos", windBarLeftPos);
        windBarTopPos = ap->GetAttributeAsFloat("topPos", windBarTopPos);
        windBarWidth = ap->GetAttributeAsFloat("width", windBarWidth);
        windBarHeight = ap->GetAttributeAsFloat("height", windBarHeight);
        windBarColor = ap->GetAttributeAsDword("color", windBarColor);
    }
    windBarTx = wdmObjects->rs->TextureCreate(txPath);
    windBarMaskTx = wdmObjects->rs->TextureCreate(maskTxPath);

    txPath = "WorldMap\\Interfaces\\back.tga";
    frameLeftPos = cx - 64.0f * resizeRatio;
    frameTopPos = cy - 128.0f * resizeRatio;
    frameWidth = 128.0f * resizeRatio;
    frameHeight = 256.0f * resizeRatio;
    frameColor = 0xffffffff;
    ap = apnt->FindAClass(apnt, "frame");
    if (ap)
    {
        txPath = BIUtils::GetStringFromAttr(ap, "texture", txPath);
        frameLeftPos = ap->GetAttributeAsFloat("leftPos", frameLeftPos);
        frameTopPos = ap->GetAttributeAsFloat("topPos", frameTopPos);
        frameWidth = ap->GetAttributeAsFloat("width", frameWidth);
        frameHeight = ap->GetAttributeAsFloat("height", frameHeight);
        frameColor = ap->GetAttributeAsDword("color", frameColor);
    }
    frameTx = wdmObjects->rs->TextureCreate(txPath);

    int32_t font = dateFont >= 0 ? dateFont : FONT_DEFAULT;
    ap = apnt->FindAClass(apnt, "dateText");
    if (ap)
    {
        dateText.Init(wdmObjects->rs, ap);
    }
    else
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
    cy = cy + 128.0f * resizeRatio + 32.0f;

    txPath     = "WorldMap\\Interfaces\\morale.tga";
    barTxPath  = "WorldMap\\Interfaces\\morale_bar.tga";
    maskTxPath = "WorldMap\\Interfaces\\morale_mask.tga";
    moraleLeftPos = cx - 64.0f * resizeRatio;
    moraleTopPos = cy - 32.0f * resizeRatio;
    moraleWidth = 128.0f * resizeRatio;
    moraleHeight = 64.0f * resizeRatio;
    moraleColor = 0xffffffff;
    ap = apnt->FindAClass(apnt, "morale");
    if (ap)
    {
        txPath = BIUtils::GetStringFromAttr(ap, "texture", txPath);
        barTxPath = BIUtils::GetStringFromAttr(ap, "barTexture", barTxPath);
        maskTxPath = BIUtils::GetStringFromAttr(ap, "maskTexture", maskTxPath);
        moraleLeftPos = ap->GetAttributeAsFloat("leftPos", moraleLeftPos);
        moraleTopPos = ap->GetAttributeAsFloat("topPos", moraleTopPos);
        moraleWidth = ap->GetAttributeAsFloat("width", moraleWidth);
        moraleHeight = ap->GetAttributeAsFloat("height", moraleHeight);
        moraleColor = ap->GetAttributeAsDword("color", moraleColor);
    }
    moraleTx = wdmObjects->rs->TextureCreate(txPath);
    moraleBarTx = wdmObjects->rs->TextureCreate(barTxPath);
    moraleMaskTx = wdmObjects->rs->TextureCreate(maskTxPath);

    float foodRumSpacing = rum ? 24.0f : 0.0f;
    ap = apnt->FindAClass(apnt, "foodText");
    if (ap)
    {
        foodText.Init(wdmObjects->rs, ap);
    }
    else
    {
        foodText.pRS = wdmObjects->rs;
        foodText.pARefresh = nullptr;
        foodText.nFont = font;
        foodText.fScale = resizeRatio;
        foodText.dwColor = 0xffffffff;
        foodText.pos.x = int32_t(cx - foodRumSpacing * resizeRatio);
        foodText.pos.y = int32_t(cy + 30.0f * resizeRatio);
    }

    ap = apnt->FindAClass(apnt, "rumText");
    if (ap)
    {
        rumText.Init(wdmObjects->rs, ap);
    }
    else if (rum)
    {
        rumText.pRS = wdmObjects->rs;
        rumText.pARefresh = nullptr;
        rumText.nFont = font;
        rumText.fScale = resizeRatio;
        rumText.dwColor = 0xffffffff;
        rumText.pos.x = int32_t(cx + foodRumSpacing * resizeRatio);
        rumText.pos.y = int32_t(cy + 30.0f * resizeRatio);
    }

    txPath = "WorldMap\\Interfaces\\coord.tga";
    coordLeftPos = cx - 64.0f * resizeRatio;
    coordTopPos = cy + 64.0f * resizeRatio;
    coordWidth = 128.0f * resizeRatio;
    coordHeight = 64.0f * resizeRatio;
    coordColor = 0xffffffff;
    ap = apnt->FindAClass(apnt, "coord");
    if (ap)
    {
        txPath = BIUtils::GetStringFromAttr(ap, "texture", txPath);
        coordLeftPos = ap->GetAttributeAsFloat("leftPos", coordLeftPos);
        coordTopPos = ap->GetAttributeAsFloat("topPos", coordTopPos);
        coordWidth = ap->GetAttributeAsFloat("width", coordWidth);
        coordHeight = ap->GetAttributeAsFloat("height", coordHeight);
        coordColor = ap->GetAttributeAsDword("color", coordColor);
    }
    coordTx = wdmObjects->rs->TextureCreate(txPath);

    ap = apnt->FindAClass(apnt, "stCoordText");
    if (ap)
    {
        stCoordText.Init(wdmObjects->rs, ap);
    }
    else
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

    ap = apnt->FindAClass(apnt, "coordText");
    if (ap)
    {
        coordText.Init(wdmObjects->rs, ap);
    }
    else
    {
        coordText.pRS = wdmObjects->rs;
        coordText.pARefresh = nullptr;
        coordText.nFont = font;
        coordText.fScale = resizeRatio;
        coordText.dwColor = 0xffffffff;
        coordText.pos.x = int32_t(cx);
        coordText.pos.y = int32_t(cy + (64.0f + 32.0f) * resizeRatio);
    }

    txPath = "WorldMap\\Interfaces\\WorldMapEnsigns.tga";
    nationFlagWidth = 48.0f * resizeRatio;
    nationFlagHeight = 48.0f * resizeRatio;
    nationFlagLeftPos = cx - nationFlagWidth / 2.0f;
    nationFlagTopPos = cy + 150.0f * resizeRatio;
    nationFlagColor = 0xffffffff;
    ap = apnt->FindAClass(apnt, "nationFlag");
    if (ap)
    {
        txPath = BIUtils::GetStringFromAttr(ap, "texture", txPath);
        nationFlagCount = ap->GetAttributeAsDword("count", nationFlagCount);
        nationFlagLeftPos = ap->GetAttributeAsFloat("leftPos", nationFlagLeftPos);
        nationFlagTopPos = ap->GetAttributeAsFloat("topPos", nationFlagTopPos);
        nationFlagWidth = ap->GetAttributeAsFloat("width", nationFlagWidth);
        nationFlagHeight = ap->GetAttributeAsFloat("height", nationFlagHeight);
        nationFlagColor = ap->GetAttributeAsDword("color", nationFlagColor);
    }
    nationFlagTx = wdmObjects->rs->TextureCreate(txPath);
}

// Rendering
void WdmWindUI::LRender(VDX9RENDER *rs)
{
    if (!wdmObjects->showWindUI)
        return;

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
    rs->TextureSet(0, skyTx);
    rs->TextureSet(1, skyMaskTx);
    FillRectCoord(buf, skyLeftPos, skyTopPos, skyWidth, skyHeight);

    FillRectUV(buf, (wdmObjects->wm->hour * (1.0f / 24.0f) - 0.125f), 0.0f, 0.25f, 1.0f);
    FillRectUV1(buf, 0.0f, 0.0f, 1.0f, 1.0f);
    FillRectColor(buf, skyColor);
    DrawRects(buf, 1, "WdmInterfaceDrawSky");

    // Direction of the wind
    rs->TextureSet(0, windPointerTx);
    FillRectCoord(buf, windPointerLeftPos, windPointerTopPos, windPointerWidth, windPointerHeight, ang);

    FillRectUV(buf, 0.0f, 0.0f, 1.0f, 1.0f);
    FillRectColor(buf, windPointerColor);
    DrawRects(buf, 1, "WdmDrawMapBlend");

    // Wind force
    rs->TextureSet(0, windBarTx);
    rs->TextureSet(1, windBarMaskTx);
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
    rs->TextureSet(0, frameTx);
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
    rs->TextureSet(0, moraleBarTx);
    rs->TextureSet(1, moraleMaskTx);
    FillRectCoord(buf, moraleLeftPos, moraleTopPos, moraleWidth, moraleHeight);

    FillRectUV(buf, morale * 0.28f, 0.0f, 1.0f, 1.0f);
    FillRectUV1(buf, 0.0f, 0.0f, 1.0f, 1.0f);
    FillRectColor(buf, moraleColor);
    DrawRects(buf, 1, "WdmInterfaceDrawSky");

    // Drawing a moral block
    rs->TextureSet(0, moraleTx);
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
        rs->TextureSet(0, coordTx);
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
