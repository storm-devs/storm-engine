//============================================================================================
//    Spirenkov Maxim, 2005
//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
//    WdmWindUI
//--------------------------------------------------------------------------------------------
//
//============================================================================================

#include "WdmWindUI.h"

#include "core.h"

#include "WdmPlayerShip.h"
#include "WorldMap.h"

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
    txNationFlag = wdmObjects->rs->TextureCreate("WorldMap\\Interfaces\\WorldMapEnsigns.tga");
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
    if (txNationFlag >= 0)
        wdmObjects->rs->TextureRelease(txNationFlag);
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
        auto *s = ap->GetAttribute("font");
        if (s && s[0])
            dateFont = wdmObjects->wm->GetRS()->LoadFont(s);
        const auto *coordinate = ap->GetAttribute("coordinate");
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
            for (long i = 1; i <= 12; i++)
            {
                if (i < 10)
                    buf[1] = '0';
                else
                    buf[1] = '1';
                buf[2] = static_cast<char>('0' + (i % 10));
                s = a->GetAttribute(buf);
                if (s && s[0])
                {
                    long sl = strlen(s) + 1;
                    if (sl > 128)
                        sl = 128;
                    memcpy(month[i - 1], s, sl);
                    month[i - 1][127] = 0;
                }
            }
        }
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
            food = static_cast<long>(data->GetFloat() + 0.5f);
            if (food < 0)
                food = 0;
        }
        data = core.Event("WorldMap_GetRum");
        if (data)
        {
            rum = static_cast<long>(data->GetFloat() + 0.5f);
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
    // Screen parameters
    float w, h;
    wdmObjects->GetVPSize(w, h);
    auto kDef = rs->GetHeightDeformator();
    // Center of the thing
    // float cx = (w - 128.0f - 16.0f) + 64.0f;
    float cx = w - (128.0f) * resizeRatio - 16.0f + 64.0f;
    // float cy = (-40.0f) + 128.0f;
    float cy = -40.0f + 128.0f * resizeRatio;
    // Buffer for drawing
    Vertex buf[(3 * 2) * 2];
    // Sky
    rs->TextureSet(0, txSky);
    rs->TextureSet(1, txSkyMask);

    // FillRectCoord(buf, cx - 64.0f, cy - 64.0f, 128.0f, 128.0f);
    FillRectCoord(buf, cx - (64.0f * resizeRatio), cy - (64.0f * resizeRatio), 128.0f * resizeRatio,
                  128.0f * resizeRatio);

    FillRectUV(buf, (wdmObjects->wm->hour * (1.0f / 24.0f) - 0.125f), 0.0f, 0.25f, 1.0f);
    FillRectUV1(buf, 0.0f, 0.0f, 1.0f, 1.0f);
    FillRectColor(buf, 0x80ffffff);
    DrawRects(buf, 1, "WdmInterfaceDrawSky");
    // Direction of the wind
    rs->TextureSet(0, txWindPointer);

    // FillRectCoord(buf, cx - 16.0f, cy - 64.0f, 32.0f, 128.0f, ang);
    FillRectCoord(buf, cx - (16.0f * resizeRatio), cy - (64.0f * resizeRatio), 32.0f * resizeRatio,
                  128.0f * resizeRatio, ang);

    FillRectUV(buf, 0.0f, 0.0f, 1.0f, 1.0f);
    FillRectColor(buf, 0xffffffff);
    DrawRects(buf, 1, "WdmDrawMapBlend");
    // Wind force
    rs->TextureSet(0, txBar);
    rs->TextureSet(1, txBarMask);

    // FillRectCoord(buf, cx - 64.0f, cy, 128.0f, 128.0f);
    FillRectCoord(buf, cx - (64.0f * resizeRatio), cy, 128.0f * resizeRatio, 128.0f * resizeRatio);

    FillRectUV(buf, 0.0f, 0.0f, 1.0f, 1.0f);
    FillRectUV1(buf, -0.5f, 0.0f, 1.0f, 1.0f);
    CMatrix rot(0.0f, 1.8f * (1.0f - widForce), 0.0f, 0.5f, 0.0f, 0.0f);
    for (long i = 0; i < 6; i++)
    {
        CVECTOR v(buf[i].tu1, 0.0f, buf[i].tv1);
        const auto vrot = rot * v;
        buf[i].tu1 = vrot.x;
        buf[i].tv1 = vrot.z;
    }
    FillRectColor(buf, 0xffffffff);
    DrawRects(buf, 1, "WdmInterfaceDrawSky");
    // Frame
    rs->TextureSet(0, txBack);

    // FillRectCoord(buf, cx - 64.0f, cy - 128.0f, 128.0f, 256.0f);
    FillRectCoord(buf, cx - 64.0f * resizeRatio, cy - 128.0f * resizeRatio, 128.0f * resizeRatio, 256.0f * resizeRatio);

    FillRectUV(buf, 0.0f, 0.0f, 1.0f, 1.0f);
    FillRectColor(buf, 0xffffffff);
    DrawRects(buf, 1, "WdmDrawMapBlend");
    // writing a date
    char tbuf[128];
    sprintf_s(tbuf, sizeof(tbuf) - 1, "%i %s %i", wdmObjects->wm->day, month[wdmObjects->wm->mon - 1],
              wdmObjects->wm->year);
    tbuf[sizeof(tbuf) - 1] = 0;
    long font = dateFont >= 0 ? dateFont : FONT_DEFAULT;
    long fw = rs->StringWidth(tbuf, font);
    long fh = rs->CharHeight(font);

    // rs->Print(font, 0xffffffff, long(cx - fw*0.5f), long(cy + 98.0f - fh*0.5f), tbuf);
    rs->ExtPrint(font, 0xffffffff, 0x00000000, PR_ALIGN_CENTER, true, resizeRatio, 0, 0, long(cx),
                 long(cy + (98.0f - fh * 0.5f) * resizeRatio), tbuf);

    // Centre
    // cy += 128.0f + 32.0f;
    cy += 128.0f * resizeRatio + 32.0f;
    // Drawing a moral bar
    rs->TextureSet(0, txMoraleBar);
    rs->TextureSet(1, txMoraleMask);

    // FillRectCoord(buf, cx - 64.0f, cy - 32.0f, 128.0f, 64.0f);
    FillRectCoord(buf, cx - 64.0f * resizeRatio, cy - 32.0f * resizeRatio, 128.0f * resizeRatio, 64.0f * resizeRatio);

    FillRectUV(buf, morale * 0.28f, 0.0f, 1.0f, 1.0f);
    FillRectUV1(buf, 0.0f, 0.0f, 1.0f, 1.0f);
    FillRectColor(buf, 0xffffffff);
    DrawRects(buf, 1, "WdmInterfaceDrawSky");
    // Drawing a moral block
    rs->TextureSet(0, txMorale);

    // FillRectCoord(buf, cx - 64.0f, cy - 32.0f, 128.0f, 64.0f);
    FillRectCoord(buf, cx - 64.0f * resizeRatio, cy - 32.0f * resizeRatio, 128.0f * resizeRatio, 64.0f * resizeRatio);

    FillRectUV(buf, 0.0f, 0.0f, 1.0f, 1.0f);
    FillRectColor(buf, 0xffffffff);
    DrawRects(buf, 1, "WdmDrawMapBlend");

    // write the number of supplies
    float foodRumSpacing = rum ? 24.0f : 0.0f;

    sprintf_s(tbuf, sizeof(tbuf) - 1, "%i%s", food > 99999 ? 99999 : food, food > 99999 ? "+" : "");
    tbuf[sizeof(tbuf) - 1] = 0;
    fw = rs->StringWidth(tbuf, font, resizeRatio, static_cast<long>(w));

    rs->ExtPrint(font, 0xffffffff, 0x00000000, PR_ALIGN_CENTER, true, resizeRatio, 0, 0,
                 long(cx - foodRumSpacing * resizeRatio), long(cy + 30.0f * resizeRatio), tbuf);

    // write the amount of rum
    if (rum)
    {
        _snprintf(tbuf, sizeof(tbuf) - 1, "%i", rum.value());
        tbuf[sizeof(tbuf) - 1] = 0;
        fw = rs->StringWidth(tbuf, font, resizeRatio, static_cast<long>(w));

        rs->ExtPrint(font, 0xffffffff, 0x00000000, PR_ALIGN_CENTER, true, resizeRatio, 0, 0,
                     long(cx + foodRumSpacing * resizeRatio), long(cy + 30.0f * resizeRatio), tbuf);
    }

    if (!wdmObjects->coordinate.empty())
    {
        // Coordinate frame
        rs->TextureSet(0, txCoord);

        FillRectCoord(buf, cx - 64.0f * resizeRatio, cy + 64.0f * resizeRatio, 128.0f * resizeRatio,
                      64.0f * resizeRatio);

        FillRectUV(buf, 0.0f, 0.0f, 1.0f, 1.0f);
        FillRectColor(buf, 0xffffffff);
        DrawRects(buf, 1, "WdmDrawMapBlend");

        // display a line with coordinates
        _snprintf(tbuf, sizeof(tbuf) - 1, "%s", wdmObjects->coordinate.c_str());
        tbuf[sizeof(tbuf) - 1] = 0;
        fw = rs->StringWidth(tbuf, font, resizeRatio, static_cast<long>(w));
        fh = rs->CharHeight(font);

        rs->ExtPrint(font, 0xffffffff, 0x00000000, PR_ALIGN_CENTER, true, resizeRatio, 0, 0, long(cx),
                     long(cy + (64.0f + 32.0f) * resizeRatio), tbuf);

        _snprintf(tbuf, sizeof(tbuf) - 1, "%s", wdmObjects->stCoordinate);
        tbuf[sizeof(tbuf) - 1] = 0;
        fw = rs->StringWidth(tbuf, font, resizeRatio, static_cast<long>(w));
        fh = rs->CharHeight(font);

        rs->ExtPrint(font, 0xffffffff, 0x00000000, PR_ALIGN_CENTER, true, resizeRatio, 0, 0, long(cx),
                     long(cy + (64.0f + 13.0f) * resizeRatio), tbuf);
    }

    if (wdmObjects->nationFlagIndex)
    {
        // National flag
        float addtu = 0.125;
        rs->TextureSet(0, txNationFlag);

        FillRectCoord(buf, cx - 24.0f * resizeRatio, cy + 150.0f * resizeRatio, 48.0f * resizeRatio,
                      48.0f * resizeRatio);

        FillRectUV(buf, wdmObjects->nationFlagIndex.value() * addtu, 0.0f, addtu, 1.0f);
        FillRectColor(buf, 0xffffffff);
        DrawRects(buf, 1, "WdmDrawMapBlend");
    }
}
