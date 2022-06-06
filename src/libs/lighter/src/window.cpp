//============================================================================================
//    Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
//    Window
//--------------------------------------------------------------------------------------------
//
//============================================================================================

#include "window.h"
#include "entity.h"
#include "core.h"
#include "shared/messages.h"
#include <cstdint>

// ============================================================================================
// Construction, destruction
// ============================================================================================

Window::Window()
{
    rs = nullptr;
    sw = sh = 0.0f;
    isNeedInit = false;
    isNoPrepared = false;
    isVisible = false;
    isTraceShadows = false;
    isSmoothShadows = false;
    smoothNorm = true;
    isBlurLight = false;
    isResetBlurLight = false;
    isTraceBlur = true;
    isLockCtrl = false;
    font = FONT_DEFAULT;
    list = nullptr;
    numElements = 0;
    selected = -1;
    listPos = 0.0f;
    isList = false;
    isPikerActive = false;
    listWait = 0.0f;
    pikerWait = 0.0f;
    pickerTexture = nullptr;
    tracePrc = 0.0f;
    smoothPrc = 0.0f;
    smoothRad = 0.2f;
    blurPrc = 0.0f;
    blurRad = 5.0f;
    blurAtt = 2.0f;
    blurCos = 0.5f;
    kBlur = 0.3f;
    isActiveMouseState = false;
    isOldMouseState = false;
    isUpdateLight = false;
    updateLight = -1;
    updateLightCos = updateLightAtt = updateLightSdw = true;
    isSaveLight = false;
    isSuccessful = 0.0f;
    isFailed = 0.0f;
    isFailedInit = false;
    isSmallSlider = true;
    lastPreset = -1;
    prsComment[0] = 0;
    slidDltX = 0.0f;
    strcpy_s(ver, "Location lighter v1.03");
}

Window::~Window()
{
    if (pickerTexture)
        pickerTexture->Release();
    if (rs && font != FONT_DEFAULT)
        rs->UnloadFont("Lighter");
    if (list)
    {
        for (int32_t i = 0; i < numElements; i++)
            delete list[i].name;
        delete list;
    }
}

bool Window::Init(VDX9RENDER *rs)
{
    this->rs = rs;
    Assert(rs);
    font = rs->LoadFont("Lighter");
    if (font < 0)
        font = FONT_DEFAULT;
    fontHeight = static_cast<float>(rs->CharHeight(font));
    // Get the current vp sizes
    D3DVIEWPORT9 vp;
    rs->GetViewport(&vp);
    sw = static_cast<float>(vp.Width);
    sh = static_cast<float>(vp.Height);
    cursx = sw * 0.5f;
    cursy = sh * 0.5f;
    winw = 300.0f;
    winh = 180.0f;
    winx = sw - winw - 5.0f;
    winy = 5.0f;
    sldLen = 180.0f;
    bkgColor = 0xa0000000;
    frmColor = 0xff00a0a0;
    textColor = 0xffffff8f;
    selColor = 0x00c0c000;
    // Picker texture
    if (rs->CreateTexture(256, 256, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &pickerTexture) == D3D_OK && pickerTexture)
    {
        D3DLOCKED_RECT lockedRect;
        if (rs->LockRect(pickerTexture, 0, &lockedRect, nullptr, 0) == D3D_OK)
        {
            auto *pnt = static_cast<uint8_t *>(lockedRect.pBits);
            for (int32_t y = 0; y < 256; y++)
                for (int32_t x = 0; x < 256; x++, pnt += 4)
                {
                    auto r = static_cast<float>(x);
                    auto g = static_cast<float>(y);
                    const auto b = static_cast<float>(255 - x);
                    auto k = r > g ? r : g;
                    if (k < b)
                        k = b;
                    k = 255.0f / k;
                    pnt[0] = static_cast<uint8_t>(b * k);
                    pnt[1] = static_cast<uint8_t>(g * k);
                    pnt[2] = static_cast<uint8_t>(r * k);
                    pnt[3] = 255;
                }
            rs->UnlockRect(pickerTexture, 0);
        }
    }

    return true;
}

void Window::InitList(LighterLights &ls)
{
    const auto maxSize = ls.Num();
    numElements = 7;
    list = new ListElement[numElements + maxSize];
    memset(list, 0, (numElements + maxSize) * sizeof(ListElement));
    list[0].name = new char[sizeof "Quick process"];
    strcpy_s(list[0].name, sizeof "Quick process", "Quick process");
    list[0].type = ListElement::t_quick;
    list[0].h = 200.0f;
    list[1].name = new char[sizeof "Trace shadows"];
    strcpy_s(list[1].name, sizeof "Trace shadows", "Trace shadows");
    list[1].type = ListElement::t_trace;
    list[2].name = new char[sizeof "Smooth shadows"];
    strcpy_s(list[2].name, sizeof "Smooth shadows", "Smooth shadows");
    list[2].type = ListElement::t_smooth;
    list[3].name = new char[sizeof "Blur light"];
    strcpy_s(list[3].name, sizeof "Blur light", "Blur light");
    list[3].type = ListElement::t_blur;
    list[3].h = 100.0f;
    list[4].name = new char[sizeof "Save presets"];
    strcpy_s(list[4].name, sizeof "Save presets", "Save presets");
    list[4].type = ListElement::t_save;
    list[4].h = 180.0f;
    list[5].name = new char[sizeof "Load presets"];
    strcpy_s(list[5].name, sizeof "Load presets", "Load presets");
    list[5].type = ListElement::t_load;
    list[5].h = 180.0f;
    list[6].name = new char[sizeof "Save lights"];
    strcpy_s(list[6].name, sizeof "Save lights", "Save lights");
    list[6].type = ListElement::t_savelight;
    for (int32_t i = 0; i < maxSize; i++)
        ls[i].isMark = false;
    for (int32_t i = 0; i < maxSize; i++)
    {
        if (ls[i].isMark)
            continue;
        switch (ls[i].type)
        {
        case Light::t_none:
            break;
        case Light::t_amb: {
            list[numElements].name = new char[sizeof "Ambient light"];
            strcpy_s(list[numElements].name, sizeof "Ambient light", "Ambient light");
            list[numElements].type = ListElement::t_amb;
            list[numElements].c = ls[i].color;
            list[numElements].st = 0.0f;
            list[numElements].color = &ls[i].color;
            ls[i].color = 0.0f;
            list[numElements].cosine = &ls[i].cosine;
            list[numElements].shadow = &ls[i].shadow;
            list[numElements].bright = &ls[i].bright;
            list[numElements].contr = &ls[i].contr;
            list[numElements].gamma = &ls[i].gamma;
            list[numElements].isOn = &ls[i].isOn;
            list[numElements].litIndex = i;
            numElements++;
            ls[i].isMark = true;
        }
        break;
        case Light::t_sun: {
            list[numElements].name = new char[sizeof "Sun light"];
            strcpy_s(list[numElements].name, sizeof "Sun light", "Sun light");
            list[numElements].type = ListElement::t_light;
            list[numElements].c = ls[i].color;
            list[numElements].st = 0.0f;
            list[numElements].color = &ls[i].color;
            ls[i].color = 0.0f;
            list[numElements].cosine = &ls[i].cosine;
            list[numElements].shadow = &ls[i].shadow;
            list[numElements].bright = &ls[i].bright;
            list[numElements].contr = &ls[i].contr;
            list[numElements].gamma = &ls[i].gamma;
            list[numElements].isOn = &ls[i].isOn;
            list[numElements].litIndex = i;
            numElements++;
            ls[i].isMark = true;
        }
        break;
        case Light::t_sky: {
            list[numElements].name = new char[sizeof "Sky light"];
            strcpy_s(list[numElements].name, sizeof "Sky light", "Sky light");
            list[numElements].type = ListElement::t_light;
            list[numElements].c = ls[i].color;
            list[numElements].st = 0.0f;
            list[numElements].color = &ls[i].color;
            ls[i].color = 0.0f;
            list[numElements].cosine = &ls[i].cosine;
            list[numElements].shadow = &ls[i].shadow;
            list[numElements].bright = &ls[i].bright;
            list[numElements].contr = &ls[i].contr;
            list[numElements].gamma = &ls[i].gamma;
            list[numElements].isOn = &ls[i].isOn;
            list[numElements].litIndex = i;
            numElements++;
            ls[i].isMark = true;
        }
        break;
        case Light::t_group: {
            const auto len = strlen(ls[i].group) + sizeof "Group ";
            list[numElements].name = new char[len];
            list[numElements].name[0] = 0;
            strcat_s(list[numElements].name, len, "Group ");
            strcat_s(list[numElements].name, len, ls[i].group);
            list[numElements].type = ListElement::t_glight;
            list[numElements].c = ls[i].color;
            list[numElements].st = 0.0f;
            list[numElements].color = &ls[i].color;
            ls[i].color = 0.0f;
            list[numElements].cosine = &ls[i].cosine;
            list[numElements].shadow = &ls[i].shadow;
            list[numElements].bright = &ls[i].bright;
            list[numElements].contr = &ls[i].contr;
            list[numElements].gamma = &ls[i].gamma;
            list[numElements].att0 = &ls[i].att0;
            list[numElements].att1 = &ls[i].att1;
            list[numElements].att2 = &ls[i].att2;
            list[numElements].range = &ls[i].range;
            list[numElements].isOn = &ls[i].isOn;
            list[numElements].litIndex = i;
            list[numElements].h = 80;
            numElements++;
            ls[i].isMark = true;
        }
        break;
        }
    }
    selected = 0;
    UpdateColors();
    UpdateLight(-1, true, true, true);
}

void Window::Draw(float dltTime)
{
    float x, y;
    if (pikerWait > 0.0f)
        pikerWait -= dltTime;
    // loading message box
    if (isFailedInit)
    {
        if (isVisible)
        {
            x = sw * 0.5f;
            y = sh * 0.5f;
            DrawLRect(x - 200.0f, y - 50.0f, x + 200.0f, y + 50.0f, bkgColor, frmColor);
            Print(0xffff0000, x - 200.0f, x + 200.0f, y - fontHeight * 0.5f, 1.0f, true, "Init is failed");
        }
        return;
    }
    if (isNeedInit)
    {
        x = sw * 0.5f;
        y = sh * 0.5f;
        DrawLRect(x - 150.0f, y - 50.0f, x + 150.0f, y + 50.0f, bkgColor, frmColor);
        Print(textColor, x - 150.0f, x + 150.0f, y - fontHeight * 0.5f, 1.0f, true, "Preparing data");
        Print(textColor, x - 150.0f, x + 150.0f, y - fontHeight * 0.5f + 40.0f, 0.5f, true, ver);
    }
    if (isNoPrepared)
    {
        x = sw * 0.5f;
        y = sh * 0.5f;
        DrawLRect(x - 200.0f, y - 50.0f, x + 200.0f, y + 50.0f, bkgColor, frmColor);
        Print(textColor, x - 200.0f, x + 200.0f, y - fontHeight * 0.5f, 1.0f, true, "Data is not preparing");
    }
    if (isSuccessful > 0.0f)
    {
        x = sw * 0.5f;
        y = sh * 0.5f;
        DrawLRect(x - 200.0f, y - 50.0f, x + 200.0f, y + 50.0f, bkgColor, frmColor);
        Print(0xff00ff00, x - 200.0f, x + 200.0f, y - fontHeight * 0.5f, 1.0f, true, "Processed is successful");
        isSuccessful -= dltTime;
        if (cursx >= x - 200.0f && cursx <= x + 200.0f)
        {
            if (cursy >= y - 50.0f && cursy <= y + 50.0f)
            {
                if (core.Controls->GetAsyncKeyState(VK_LBUTTON) < 0)
                    isSuccessful = 0.0f;
            }
        }
    }
    if (isFailed > 0.0f)
    {
        x = sw * 0.5f;
        y = sh * 0.5f;
        DrawLRect(x - 200.0f, y - 50.0f, x + 200.0f, y + 50.0f, bkgColor, frmColor);
        Print(0xffff0000, x - 200.0f, x + 200.0f, y - fontHeight * 0.5f, 1.0f, true, "Processed is failed");
        isFailed -= dltTime;
        if (cursx >= x - 200.0f && cursx <= x + 200.0f)
        {
            if (cursy >= y - 50.0f && cursy <= y + 50.0f)
            {
                if (core.Controls->GetAsyncKeyState(VK_LBUTTON) < 0)
                    isFailed = 0.0f;
            }
        }
    }
    // Drawing the interface
    if (!isVisible)
        return;
    if (core.Controls->GetAsyncKeyState(VK_CONTROL) < 0)
    {
        if (core.Controls->GetAsyncKeyState(VK_SUBTRACT) < 0)
        {
            x = sw * 0.5f;
            y = sh * 0.5f;
            DrawLRect(x - 230.0f, y - 50.0f, x + 230.0f, y + 50.0f, bkgColor, frmColor);
            Print(textColor, x - 200.0f, x + 200.0f, y - fontHeight * 0.5f, 1.0f, true, ver);
        }
        else if (core.Controls->GetAsyncKeyState(VK_ADD) < 0)
        {
            selected = 0;
        }
        else if (core.Controls->GetAsyncKeyState(VK_NUMPAD1) < 0)
        {
            selected = 1;
        }
        else if (core.Controls->GetAsyncKeyState(VK_NUMPAD2) < 0)
        {
            selected = 2;
        }
        else if (core.Controls->GetAsyncKeyState(VK_NUMPAD3) < 0)
        {
            selected = 3;
        }
        else if (core.Controls->GetAsyncKeyState(VK_NUMPAD4) < 0)
        {
            selected = 4;
        }
        else if (core.Controls->GetAsyncKeyState(VK_NUMPAD5) < 0)
        {
            selected = 5;
        }
        else if (core.Controls->GetAsyncKeyState(VK_NUMPAD6) < 0)
        {
            selected = 6;
        }
        else if (core.Controls->GetAsyncKeyState(VK_NUMPAD7) < 0)
        {
            selected = 7;
        }
        else if (core.Controls->GetAsyncKeyState(VK_NUMPAD8) < 0)
        {
            selected = 8;
        }
        else if (core.Controls->GetAsyncKeyState(VK_NUMPAD9) < 0)
        {
            selected = 9;
        }
    }
    isMouseDown =
        !isLockCtrl && isSuccessful <= 0.0f && isFailed <= 0.0f && (core.Controls->GetAsyncKeyState(VK_LBUTTON) < 0);
    if (!isMouseDown)
        slidID = -1;
    if (!isActiveMouseState)
    {
        if (!isOldMouseState)
        {
            isActiveMouseState = isMouseDown;
            isOldMouseState = isMouseDown;
        }
        else
            isOldMouseState = isMouseDown;
    }
    else
        isActiveMouseState = false;
    // Update cursor position
    CONTROL_STATE cs;
    core.Controls->GetControlState("Turn H", cs);
    cursx += cs.lValue * 3.0f;
    if (cursx < 0.0f)
        cursx = 0.0f;
    if (cursx > sw - 5.0f)
        cursx = sw - 5.0f;
    core.Controls->GetControlState("Turn V", cs);
    cursy += cs.lValue * 3.0f;
    if (cursy < 0.0f)
        cursy = 0.0f;
    if (cursy > sh - 5.0f)
        cursy = sh - 5.0f;
    // Background
    DrawLRect(winx, winy, winx + winw, winy + winh + (selected >= 0 ? list[selected].h : 0), bkgColor, frmColor);
    // Flap button
    const auto clsx = winx + winw - 20.0f;
    const auto clsy = winy + 5.0f;
    const auto clsw = 15.0f;
    const auto clsh = 15.0f;
    auto c = selColor;
    auto cl = frmColor;
    if (cursx >= clsx && cursx < clsx + clsw && cursy >= clsy && cursy < clsy + clsh)
    {
        c |= 0xff000000;
        cl = 0xff000000;
        if (isActiveMouseState)
            Reset(false);
    }
    DrawLRect(clsx, clsy, clsx + clsw, clsy + clsh, c, frmColor);
    DrawLine(clsx + 2.0f, clsy + 2.0f, clsx + clsw - 1.0f, clsy + clsh - 1.0f, cl);
    DrawLine(clsx + clsw - 2.0f, clsy + 2.0f, clsx + 1.0f, clsy + clsh - 1.0f, cl);
    // List header
    const auto lstx = winx + 5.0f;
    auto lsty = winy + 5.0f;
    const auto lstw = 240.0f;
    auto lsth = 22.0f;
    const auto lstbx = lstx + lstw;
    const auto lstbw = lsth;
    c = selColor;
    cl = frmColor;
    if (listWait <= 0.0f)
    {
        if (!isList && cursx >= lstx && cursx < lstbx + lstbw && cursy >= lsty && cursy < lsty + lsth)
        {
            c |= 0xff000000;
            cl = 0xff000000;
            if (isActiveMouseState)
            {
                isPikerActive = false;
                isList = !isList;
            }
        }
    }
    else
        listWait -= dltTime;
    DrawLRect(lstx, lsty, lstx + lstw + lsth, lsty + lsth, c, frmColor);
    if (selected >= 0)
    {
        Print(textColor, lstx + 5.0f, lstx + 5.0f, lsty + (lsth - fontHeight * 0.6f) * 0.5f, 0.6f, false,
              list[selected].name);
    }
    // DrawLRect(lstbx, lsty, lstbx + lstbw, lsty + lsth, c, frmColor);
    // DrawLine(lstbx + 6.0f, lsty + 6.0f, lstbx + lstbw - 6.0f, lsty + 6.0f, cl);
    // DrawLine(lstbx + 6.0f, lsty + 6.0f, lstbx + lstbw/2 + 1, lsty + lsth - 6.0f, cl);
    // DrawLine(lstbx + lstbw - 6.0f, lsty + 6.0f, lstbx + lstbw/2 - 1, lsty + lsth - 6.0f, cl);
    // Controls
    int32_t prs;
    if (selected >= 0)
    {
        auto &le = list[selected];
        bool isOn;
        updateLightCos = updateLightAtt = updateLightSdw = false;
        switch (le.type)
        {
        case ListElement::t_quick:
            prs = SelPreset();
            DrawLRect(winx + 15.0f, winy + winh + list[selected].h - 30.0f,
                      winx + 15.0f + (winw - 30.0f) * (tracePrc + smoothPrc) * 0.5f,
                      winy + winh + list[selected].h - 10.0f, isLockCtrl ? 0xff000080 : 0xff0000c0, 0);
            DrawLRect(winx + 15.0f, winy + winh + list[selected].h - 30.0f, winx + winw - 15.0f,
                      winy + winh + list[selected].h - 10.0f, 0, frmColor);
            if (prs > 0)
            {
                LoadPreset(prs);
                if (tracePrc < 1.0f)
                    isTraceShadows = true;
                if (smoothPrc < 1.0f)
                    isSmoothShadows = true;
            }
            break;
        case ListElement::t_trace:
            DrawLRect(winx + 15.0f, winy + winh * 0.5f - 10.0f, winx + 15.0f + (winw - 30.0f) * tracePrc,
                      winy + winh * 0.5f + 10.0f, isLockCtrl ? 0xff000080 : 0xff0000c0, 0);
            DrawLRect(winx + 15.0f, winy + winh * 0.5f - 10.0f, winx + winw - 15.0f, winy + winh * 0.5f + 10.0f, 0,
                      frmColor);
            isTraceShadows = Button((winw - 150.0f) * 0.5f, winh - 40.0f, 150.0f, 30.0f, "Trace shadows");
            break;
        case ListElement::t_smooth:
            Slider(0, 50.0f, "Rad", smoothRad, 0.0f, 2.0f, true);
            isSmoothShadows = Button((winw - 150.0f) * 0.5f, winh - 40.0f, 150.0f, 30.0f, "Smooth shadows");
            Checker(80.0f, 75.0f, "Check normals", smoothNorm);
            DrawLRect(winx + 15.0f, winy + 110.0f, winx + 15.0f + (winw - 30.0f) * smoothPrc, winy + 130.0f,
                      isLockCtrl ? 0xff000080 : 0xff0000c0, 0);
            DrawLRect(winx + 15.0f, winy + 110.0f, winx + winw - 15.0f, winy + 130.0f, 0, frmColor);
            break;
        case ListElement::t_blur:
            Slider(0, 50.0f, "Rad", blurRad, 0.2f, 20.0f, true);
            Slider(1, 70.0f, "Att", blurAtt, 0.2f, 4.0f, true);
            Slider(2, 90.0f, "Cos", blurCos, 0.0f, 1.0f, true);
            Checker(65.0f, 115.0f, "Trace shadows", isTraceBlur);
            DrawLRect(winx + 15.0f, winy + 150.0f, winx + 15.0f + (winw - 30.0f) * blurPrc, winy + 170.0f,
                      isLockCtrl ? 0xff000080 : 0xff0000c0, 0);
            DrawLRect(winx + 15.0f, winy + 150.0f, winx + winw - 15.0f, winy + 170.0f, 0, frmColor);
            if (Slider(3, 200.0f, "Blr", kBlur, 0.0f, 1.0f, true))
                UpdateLight(-1, false, false, false);
            isBlurLight = Button(20.0f, winh + list[selected].h - 40.0f, winw * 0.5f - 30.0f, 30.0f, "Blur light");
            isResetBlurLight =
                Button(winw * 0.5f + 10.0f, winh + list[selected].h - 40.0f, winw * 0.5f - 30.0f, 30.0f, "Reset blur");
            break;
        case ListElement::t_save:
            prs = SelPreset();
            if (prs > 0)
                SavePreset(prs);
            break;
        case ListElement::t_load:
            prs = SelPreset();
            if (prs > 0)
                LoadPreset(prs);
            break;
        case ListElement::t_savelight:
            Print(textColor, winx, winx + winw, 60.0f, 0.6f, true, "Save lighting data");
            if (Button((winw - 150.0f) * 0.5f, 95.0f, 150.0f, 30.0f, "Save"))
                isSaveLight = true;
            break;
        case ListElement::t_amb:
            if (Slider(0, 50.0f, "Clr", le.st))
                UpdateLight(le.litIndex, false, false, false);
            isOn = *le.isOn;
            Checker(65.0f + sldLen, 120.0f, "On", *le.isOn);
            if (isOn != *le.isOn)
            {
                isUpdateLight = true;
                updateLight = le.litIndex;
            }
            if (ColorPicker(6, 50.0f, le.c, le.st, *le.color))
                UpdateLight(le.litIndex, false, false, false);
            break;
        case ListElement::t_glight:
            if (Slider(6, 170.0f, "Att0", *le.att0, 0.001f, 3.0f, true))
                UpdateLight(le.litIndex, false, true, false);
            if (Slider(7, 190.0f, "Att1", *le.att1, 0.001f, 3.0f, true))
                UpdateLight(le.litIndex, false, true, false);
            if (Slider(8, 210.0f, "Att2", *le.att2, 0.001f, 3.0f, true))
                UpdateLight(le.litIndex, false, true, false);
            if (Slider(9, 230.0f, "Rng", *le.range, 1.0f, 50.0f, true))
                UpdateLight(le.litIndex, false, true, false);
        case ListElement::t_light:
            if (Slider(0, 50.0f, "Clr", le.st))
                UpdateLight(le.litIndex, false, false, false);
            if (Slider(1, 70.0f, "Cos", *le.cosine))
                UpdateLight(le.litIndex, true, false, false);
            if (Slider(2, 90.0f, "Shw", *le.shadow))
                UpdateLight(le.litIndex, false, false, true);
            if (Slider(3, 110.0f, "Brt", *le.bright))
                UpdateLight(le.litIndex, false, false, true);
            if (Slider(4, 130.0f, "Cnt", *le.contr))
                UpdateLight(le.litIndex, false, false, true);
            if (Slider(5, 150.0f, "Gmm", *le.gamma))
                UpdateLight(le.litIndex, false, false, true);
            isOn = *le.isOn;
            Checker(65.0f + sldLen, 120.0f, "On", *le.isOn);
            if (isOn != *le.isOn)
            {
                isUpdateLight = true;
                updateLight = le.litIndex;
            }
            if (ColorPicker(6, 50.0f, le.c, le.st, *le.color))
                UpdateLight(le.litIndex, false, false, false);
            break;
        }
    }
    // List
    if (isList)
    {
        const int32_t numLines = 16;
        lsty += lsth;
        lsth = lsth * numLines;
        DrawLRect(lstx, lsty, lstx + lstw + lstbw, lsty + lsth, 0xe0000000, frmColor);
        DrawLine(lstbx, lsty, lstbx, lsty + lsth, frmColor);
        const auto isCheck = (cursx >= lstbx && cursx < lstbx + lstbw && cursy >= lsty && cursy < lsty + lsth);
        c = selColor;
        cl = frmColor;
        if (isCheck && cursy <= lsty + lstbw)
        {
            c |= 0xff000000;
            cl = 0xff000000;
            if (isMouseDown)
            {
                if (core.Controls->GetAsyncKeyState(VK_SHIFT) < 0)
                    listPos -= 3.0f;
                else
                    listPos -= 1.0f;
            }
        }
        DrawLRect(lstbx, lsty, lstbx + lstbw, lsty + lstbw, c, frmColor);
        DrawLine(lstbx + lstbw * 0.5f, lsty + 6.0f, lstbx + 6.0f, lsty + lstbw - 6.0f, cl);
        DrawLine(lstbx + lstbw * 0.5f, lsty + 6.0f, lstbx + lstbw - 6.0f, lsty + lstbw - 6.0f, cl);
        DrawLine(lstbx + 6.0f, lsty + lstbw - 6.0f, lstbx + lstbw - 4.0f, lsty + lstbw - 6.0f, cl);
        c = selColor;
        cl = frmColor;
        if (isCheck && cursy >= lsty + lsth - lstbw)
        {
            c |= 0xff000000;
            cl = 0xff000000;
            if (isMouseDown)
            {
                if (core.Controls->GetAsyncKeyState(VK_SHIFT) < 0)
                    listPos += 3.0f;
                else
                    listPos += 1.0f;
            }
        }
        DrawLRect(lstbx, lsty + lsth - lstbw, lstbx + lstbw, lsty + lsth, c, frmColor);
        DrawLine(lstbx + lstbw * 0.5f, lsty + lsth - 6.0f, lstbx + 6.0f, lsty + lsth - lstbw + 6.0f, cl);
        DrawLine(lstbx + lstbw * 0.5f, lsty + lsth - 6.0f, lstbx + lstbw - 6.0f, lsty + lsth - lstbw + 6.0f, cl);
        DrawLine(lstbx + 6.0f, lsty + lsth - lstbw + 6.0f, lstbx + lstbw - 4.0f, lsty + lsth - lstbw + 6.0f, cl);
        // Rendering loop
        if (listPos > numElements - numLines / 2)
            listPos = static_cast<float>(numElements - numLines / 2);
        if (listPos < 0.0f)
            listPos = 0.0f;
        const auto str = static_cast<int32_t>(listPos);
        int32_t sel = -1;
        if (cursx >= lstx && cursx < lstx + lstw && cursy >= lsty && cursy < lsty + lsth)
        {
            // Looking for the selected area
            sel = static_cast<int32_t>((cursy - lsty) / lstbw);
            if (sel < 0)
                sel = 0;
            if (sel >= numLines)
                sel = numLines;
        }
        int32_t actItem = -1;
        y = lsty;
        for (int32_t i = str, cnt = 0; i < numElements && cnt < numLines; i++, cnt++)
        {
            const auto clr = textColor;
            if (cnt == sel)
            {
                DrawLRect(lstx + 3, y, lstx + lstw - 3, y + lstbw, 0xff808080, 0);
                actItem = i;
            }
            if (i == selected)
            {
                DrawLRect(lstx + 3, y, lstx + lstw - 3, y + lstbw, 0, 0xff800000);
            }
            Print(clr, lstx + 5.0f, lstx + 5.0f, y + (lstbw - fontHeight * 0.6f) * 0.5f, 0.6f, false, list[i].name);
            y += lstbw;
        }
        if (actItem >= 0 && isMouseDown)
        {
            selected = actItem;
            isList = false;
        }
    }
    // Draw the cursor
    DrawCursor();
}

void Window::Reset(bool isActive)
{
    isList = false;
    isVisible = isActive;
    entid_t loc;
    core.Send_Message(core.GetEntityId("location"), "ll", MSG_LOCATION_PAUSE, static_cast<int32_t>(isActive));
    slidID = -1;
    isPikerActive = false;
    isActiveMouseState = false;
    isMouseDown = false;
    isOldMouseState = false;
}

void Window::DrawRect(float x1, float y1, float x2, float y2, uint32_t color)
{
    Vertex v[4];
    v[0].x = x1;
    v[0].y = y1;
    v[0].z = 0.5f;
    v[0].rhw = 2.0f;
    v[1].x = x2;
    v[1].y = y1;
    v[1].z = 0.5f;
    v[1].rhw = 2.0f;
    v[2].x = x1;
    v[2].y = y2;
    v[2].z = 0.5f;
    v[2].rhw = 2.0f;
    v[3].x = x2;
    v[3].y = y2;
    v[3].z = 0.5f;
    v[3].rhw = 2.0f;
    rs->SetRenderState(D3DRS_TEXTUREFACTOR, color);
    rs->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, D3DFVF_XYZRHW, 2, v, sizeof(Vertex), "DbgDraw2DTFColor");
}

void Window::DrawLine(float x1, float y1, float x2, float y2, uint32_t color)
{
    Vertex v[2];
    v[0].x = x1;
    v[0].y = y1;
    v[0].z = 0.5f;
    v[0].rhw = 2.0f;
    v[1].x = x2;
    v[1].y = y2;
    v[1].z = 0.5f;
    v[1].rhw = 2.0f;
    rs->SetRenderState(D3DRS_TEXTUREFACTOR, color);
    rs->DrawPrimitiveUP(D3DPT_LINELIST, D3DFVF_XYZRHW, 1, v, sizeof(Vertex), "DbgDraw2DTFColor");
}

void Window::DrawLRect(float x1, float y1, float x2, float y2, uint32_t bkgColor, uint32_t lnColor)
{
    if (bkgColor & 0xff000000)
        DrawRect(x1, y1, x2, y2, bkgColor);
    if (lnColor & 0xff000000)
    {
        DrawLine(x1, y1, x2, y1, lnColor);
        DrawLine(x1, y1, x1, y2, lnColor);
        DrawLine(x1, y2, x2 + 1.0f, y2, lnColor);
        DrawLine(x2, y1, x2, y2 + 1.0f, lnColor);
    }
}

void Window::Print(int32_t color, float xleft, float xright, float y, float scale, bool isAlign, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    vsnprintf(stringBuffer, sizeof(stringBuffer), format, args);
    va_end(args);
    auto x = xleft;
    if (isAlign)
    {
        const auto strw = rs->StringWidth(stringBuffer, font) * scale;
        x = (xright + xleft - strw) * 0.5f;
    }
    rs->ExtPrint(font, color, 0, 0, false, scale, 0, 0, static_cast<int32_t>(x), static_cast<int32_t>(y), stringBuffer);
}

void Window::DrawCursor()
{
    Vertex v[3];
    const auto p1x = cursx + 15.0f, p1y = cursy + 20.0f;
    const auto p2x = cursx + 15.0f * 1.2f - 20.0f * 0.4f, p2y = cursy + 20.0f * 1.2f + 15.0f * 0.4f;
    const float p3x = cursx + 15.0f * 1.2f + 20.0f * 0.4f, p3y = cursy + 20.0f * 1.2f - 15.0f * 0.4f;
    v[0].x = cursx;
    v[0].y = cursy;
    v[0].z = 0.5f;
    v[0].rhw = 2.0f;
    v[1].x = p1x;
    v[1].y = p1y;
    v[1].z = 0.5f;
    v[1].rhw = 2.0f;
    v[2].x = p2x;
    v[2].y = p2y;
    v[2].z = 0.5f;
    v[2].rhw = 2.0f;
    rs->SetRenderState(D3DRS_TEXTUREFACTOR, 0xff9f9f9f);
    rs->DrawPrimitiveUP(D3DPT_TRIANGLELIST, D3DFVF_XYZRHW, 1, v, sizeof(Vertex), "DbgDraw2DTFColor");
    v[2].x = p3x;
    v[2].y = p3y;
    rs->SetRenderState(D3DRS_TEXTUREFACTOR, 0xffcfcfcf);
    rs->DrawPrimitiveUP(D3DPT_TRIANGLELIST, D3DFVF_XYZRHW, 1, v, sizeof(Vertex), "DbgDraw2DTFColor");
    DrawLine(cursx, cursy, p2x, p2y, 0xcf404040);
    DrawLine(p2x, p2y, p1x, p1y, 0xcf404040);
    DrawLine(p1x, p1y, p3x, p3y, 0xcf404040);
    DrawLine(p3x, p3y, cursx, cursy, 0xcf404040);
}

bool Window::Slider(int32_t id, float y, const char *text, float &value, float min, float max, bool drawVal)
{
    bool isUpdate = false;
    float v = (value - min) / (max - min);
    if (v < 0.0f)
        v = 0.0f;
    if (v > 1.0f)
        v = 1.0f;
    // Text
    float x = winx + 5.0f;
    Print(textColor, x, x, y, 0.5f, false, text);
    // Strip
    x += 40.0f;
    y += 1.0f;
    DrawLRect(x, winy + y, x + sldLen, winy + y + 5.0f, 0, frmColor);
    const float szx = isSmallSlider ? 3.0f : 8.0f;
    const float szy = isSmallSlider ? 3.0f : 5.0f;
    // Engine
    uint32_t c = 0xcc000000;
    const float oldv = v;
    if (!isPikerActive && !isList)
    {
        if (isActiveMouseState)
        {
            if (cursy >= winy + y - szy && cursy <= winy + y + szy + 5.0f)
            {
                if (cursx >= x + v * sldLen - szx && cursx <= x + v * sldLen + szx)
                {
                    slidDltX = (x + v * sldLen) - cursx;
                    slidID = id;
                }
            }
            if (slidID != id && cursy >= winy + y && cursy <= winy + y + 5.0f)
            {
                if (cursx >= x && cursx <= x + sldLen)
                {
                    v = (cursx - x) / sldLen;
                }
            }
        }
        else if (isMouseDown)
        {
            if (slidID == id)
            {
                c = 0xff000000 | selColor;
                v = (cursx - x + slidDltX) / sldLen;
            }
        }
    }
    if (v < 0.0f)
        v = 0.0f;
    if (v > 1.0f)
        v = 1.0f;
    isUpdate = oldv != v;
    DrawLRect(x + v * sldLen - szx, winy + y - szy, x + v * sldLen + szx, winy + y + szy + 5.0f, c, frmColor);
    if (!isSmallSlider)
        DrawLine(x + v * sldLen, winy + y - 3.0f, x + v * sldLen, winy + y + 8.0f, frmColor);
    if (isUpdate)
        value = min + (max - min) * v;
    if (drawVal)
        Print(textColor, x + sldLen + 5.0f, x + sldLen + 5.0f, y + 1.0f, 0.5f, false, "%.3f", value);
    return isUpdate;
}

bool Window::ColorPicker(int32_t id, float y, CVECTOR &ref, float st, CVECTOR &res)
{
    bool isChange = false;
    // calculate the color
    UpdateColors();
    // Draw a rectangle
    const float x = winx + 60.0f + sldLen;
    const float w = 50.0f;
    const float h = 50.0f;
    if (res.x < 0.0f)
        res.x = 0.0f;
    if (res.y < 0.0f)
        res.y = 0.0f;
    if (res.z < 0.0f)
        res.z = 0.0f;
    float knrm = res.x > res.y ? res.x : res.y;
    if (knrm < res.z)
        knrm = res.z;
    if (knrm <= 1.0f)
        knrm = 1.0f;
    knrm = 255.0f / knrm;
    const uint32_t color = (static_cast<uint32_t>(res.x * knrm) << 16) | (static_cast<uint32_t>(res.y * knrm) << 8) |
                           (static_cast<uint32_t>(res.z * knrm) << 0);
    DrawLRect(x, winy + y, x + w, winy + y + h, 0xff000000 | color, frmColor);
    // draw a picker
    if (isPikerActive)
    {
        // Background
        DrawLRect(x - 257.0f - 6.0f, winy + y, x - 1.0f, winy + y + 256.0f + 6.0f, 0xff000000, 0);
        // Color square
        float x1 = x - 256.0f - 4.0f;
        float y1 = winy + y + 3.0f;
        const float x2 = x - 4.0f;
        const float y2 = winy + y + 256.0f + 3.0f;
        struct Vrt
        {
            float x, y;
            float z, rhw;
            float u, v;
        } v[4];
        v[0].x = x1;
        v[0].y = y1;
        v[0].z = 0.5f;
        v[0].rhw = 2.0f;
        v[0].u = 0.0f;
        v[0].v = 0.0f;
        v[1].x = x2;
        v[1].y = y1;
        v[1].z = 0.5f;
        v[1].rhw = 2.0f;
        v[1].u = 1.0f;
        v[1].v = 0.0f;
        v[2].x = x1;
        v[2].y = y2;
        v[2].z = 0.5f;
        v[2].rhw = 2.0f;
        v[2].u = 0.0f;
        v[2].v = 1.0f;
        v[3].x = x2;
        v[3].y = y2;
        v[3].z = 0.5f;
        v[3].rhw = 2.0f;
        v[3].u = 1.0f;
        v[3].v = 1.0f;
        rs->TextureSet(0, -1);
        rs->SetTexture(0, pickerTexture);
        rs->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, D3DFVF_XYZRHW | D3DFVF_TEX1, 2, v, sizeof(v[0]), "DbgDraw2DTexture");
        rs->SetTexture(0, nullptr);
        //
        if (pikerWait < 0.0f && isMouseDown)
        {
            if (cursy >= y1 && cursy <= y2 && cursx >= x1 && cursx <= x2)
            {
                x1 = (cursx - x1) / (x2 - x1);
                y1 = (cursy - y1) / (y2 - y1);
                if (x1 < 0.0f)
                    x1 = 0.0f;
                if (x1 > 1.0f)
                    x1 = 1.0f;
                if (y1 < 0.0f)
                    y1 = 0.0f;
                if (y1 > 1.0f)
                    y1 = 1.0f;
                ref.x = x1;
                ref.y = y1;
                ref.z = 1.0f - x1;
                float k = ref.x > ref.y ? ref.x : ref.y;
                if (k < ref.z)
                    k = ref.z;
                ref *= 1.0f / k;
                UpdateColors();
            }
            isPikerActive = false;
            isChange = true;
        }
    }
    else
    {
        if (!isList && slidID < 0 && isMouseDown)
        {
            if (cursx >= x && cursx <= x + w)
            {
                if (cursy >= winy + y && cursy <= winy + y + h)
                {
                    isPikerActive = true;
                    pikerWait = 0.2f;
                }
            }
        }
    }
    return isChange;
}

bool Window::Button(float x, float y, float w, float h, const char *text, int32_t *act, int32_t init)
{
    bool isInside = false;
    uint32_t c = selColor;
    if (cursx >= winx + x && cursx <= winx + x + w)
    {
        if (cursy >= winy + y && cursy <= winy + y + h)
        {
            isInside = true;
            c |= 0xff000000;
        }
    }
    DrawLRect(winx + x, winy + y, winx + x + w, winy + y + h, c, frmColor);
    Print(textColor, winx + x, winx + x + w, winy + y + (h - fontHeight * 0.5f) * 0.5f, 0.5f, true, text);
    if (isInside && act)
        *act = init;
    return isActiveMouseState && isInside && !isList;
}

void Window::Checker(float x, float y, const char *text, bool &res)
{
    uint32_t c = selColor;
    const float s = 10.0f;
    const float h = 20.0f;
    // check for a change
    if (cursx >= winx + x && cursx <= winx + x + s)
    {
        if (cursy >= winy + y + h * 0.5f - s * 0.5f && cursy <= winy + y + h * 0.5f + s * 0.5f)
        {
            c |= 0xff000000;
            if (isActiveMouseState)
                res = !res;
        }
    }
    DrawLRect(winx + x, winy + y + h * 0.5f - s * 0.5f, winx + x + s, winy + y + h * 0.5f + s * 0.5f, c, frmColor);
    if (res)
    {
        DrawLine(winx + x, winy + y + h * 0.5f - s * 0.5f - 2.0f, winx + x + s * 0.5f,
                 winy + y + h * 0.5f + s * 0.5f - 2.0f, frmColor);
        DrawLine(winx + x + s + 1.0f, winy + y + h * 0.5f - s * 0.5f - 5.0f, winx + x + s * 0.5f,
                 winy + y + h * 0.5f + s * 0.5f - 3.0f, frmColor);
    }
    Print(textColor, winx + x + s + 10.0f, winx + x + s + 10.0f, winy + y + (h - fontHeight * 0.5f) * 0.5f, 0.5f, false,
          text);
}

int32_t Window::SelPreset()
{
    int32_t prs = -1, ins = -1;
    float vl = 40.0f;
    Print(textColor, winx, winx + winw, vl, 0.6f, true, "Hours Outside");
    if (Button(5.0f, vl + 20.0f, 20.0f, 20.0f, "1", &ins, 1))
        prs = 1;
    if (Button(35.0f, vl + 20.0f, 20.0f, 20.0f, "2", &ins, 2))
        prs = 2;
    if (Button(65.0f, vl + 20.0f, 20.0f, 20.0f, "3", &ins, 3))
        prs = 3;
    if (Button(95.0f, vl + 20.0f, 20.0f, 20.0f, "4", &ins, 4))
        prs = 4;
    if (Button(125.0f, vl + 20.0f, 20.0f, 20.0f, "5", &ins, 5))
        prs = 5;
    if (Button(155.0f, vl + 20.0f, 20.0f, 20.0f, "6", &ins, 6))
        prs = 6;
    if (Button(185.0f, vl + 20.0f, 20.0f, 20.0f, "7", &ins, 7))
        prs = 7;
    if (Button(215.0f, vl + 20.0f, 20.0f, 20.0f, "8", &ins, 8))
        prs = 8;
    if (Button(245.0f, vl + 20.0f, 20.0f, 20.0f, "9", &ins, 9))
        prs = 9;
    if (Button(275.0f, vl + 20.0f, 20.0f, 20.0f, "10", &ins, 10))
        prs = 10;
    // vl += 50.0f;
    // Print(textColor, winx, winx + winw, vl, 0.6f, true, "Day");
    if (Button(5.0f, vl + 50.0f, 20.0f, 20.0f, "11", &ins, 11))
        prs = 11;
    if (Button(35.0f, vl + 50.0f, 20.0f, 20.0f, "12", &ins, 12))
        prs = 12;
    if (Button(65.0f, vl + 50.0f, 20.0f, 20.0f, "13", &ins, 13))
        prs = 13;
    if (Button(95.0f, vl + 50.0f, 20.0f, 20.0f, "14", &ins, 14))
        prs = 14;
    if (Button(125.0f, vl + 50.0f, 20.0f, 20.0f, "15", &ins, 15))
        prs = 15;
    if (Button(155.0f, vl + 50.0f, 20.0f, 20.0f, "16", &ins, 16))
        prs = 16;
    if (Button(185.0f, vl + 50.0f, 20.0f, 20.0f, "17", &ins, 17))
        prs = 17;
    if (Button(215.0f, vl + 50.0f, 20.0f, 20.0f, "18", &ins, 18))
        prs = 18;
    if (Button(245.0f, vl + 50.0f, 20.0f, 20.0f, "19", &ins, 19))
        prs = 19;
    if (Button(275.0f, vl + 50.0f, 20.0f, 20.0f, "20", &ins, 20))
        prs = 20;
    // vl += 50.0f;
    // Print(textColor, winx, winx + winw, vl, 0.6f, true, "Evening");
    if (Button(5.0f, vl + 80.0f, 20.0f, 20.0f, "20", &ins, 21))
        prs = 21;
    if (Button(35.0f, vl + 80.0f, 20.0f, 20.0f, "21", &ins, 22))
        prs = 22;
    if (Button(65.0f, vl + 80.0f, 20.0f, 20.0f, "22", &ins, 23))
        prs = 23;
    if (Button(95.0f, vl + 80.0f, 20.0f, 20.0f, "23", &ins, 24))
        prs = 24;
    if (Button(125.0f, vl + 80.0f, 20.0f, 20.0f, "24", &ins, 25))
        prs = 25;
    /*if(Button(155.0f, vl + 20.0f, 20.0f, 20.0f, "6", &ins, 26)) prs = 26;
    if(Button(185.0f, vl + 20.0f, 20.0f, 20.0f, "7", &ins, 27)) prs = 27;
    if(Button(215.0f, vl + 20.0f, 20.0f, 20.0f, "8", &ins, 28)) prs = 28;
    if(Button(245.0f, vl + 20.0f, 20.0f, 20.0f, "9", &ins, 29)) prs = 29;
    if(Button(275.0f, vl + 20.0f, 20.0f, 20.0f, "10", &ins, 30)) prs = 30;
    */
    vl += 130.0f;
    Print(textColor, winx, winx + winw, vl, 0.6f, true, "Storm Outside");
    if (Button(5.0f, vl + 20.0f, 20.0f, 20.0f, "1", &ins, 31))
        prs = 26;
    if (Button(35.0f, vl + 20.0f, 20.0f, 20.0f, "2", &ins, 32))
        prs = 27;
    if (Button(65.0f, vl + 20.0f, 20.0f, 20.0f, "3", &ins, 33))
        prs = 28;
    if (Button(95.0f, vl + 20.0f, 20.0f, 20.0f, "4", &ins, 34))
        prs = 29;
    /*
    if(Button(125.0f, vl + 20.0f, 20.0f, 20.0f, "5", &ins, 35)) prs = 35;
    if(Button(155.0f, vl + 20.0f, 20.0f, 20.0f, "6", &ins, 36)) prs = 36;
    if(Button(185.0f, vl + 20.0f, 20.0f, 20.0f, "7", &ins, 37)) prs = 37;
    if(Button(215.0f, vl + 20.0f, 20.0f, 20.0f, "8", &ins, 38)) prs = 38;
    if(Button(245.0f, vl + 20.0f, 20.0f, 20.0f, "9", &ins, 39)) prs = 39;
    if(Button(275.0f, vl + 20.0f, 20.0f, 20.0f, "10", &ins, 40)) prs = 40;
    */
    vl += 50.0f;
    Print(textColor, winx, winx + winw, vl, 0.6f, true, "Hours Inside");
    if (Button(5.0f, vl + 20.0f, 20.0f, 20.0f, "1", &ins, 41))
        prs = 30;
    if (Button(35.0f, vl + 20.0f, 20.0f, 20.0f, "2", &ins, 42))
        prs = 31;
    if (Button(65.0f, vl + 20.0f, 20.0f, 20.0f, "3", &ins, 43))
        prs = 32;
    if (Button(95.0f, vl + 20.0f, 20.0f, 20.0f, "4", &ins, 44))
        prs = 33;
    if (Button(125.0f, vl + 20.0f, 20.0f, 20.0f, "5", &ins, 45))
        prs = 34;
    if (Button(155.0f, vl + 20.0f, 20.0f, 20.0f, "6", &ins, 46))
        prs = 35;
    if (Button(185.0f, vl + 20.0f, 20.0f, 20.0f, "7", &ins, 47))
        prs = 36;
    if (Button(215.0f, vl + 20.0f, 20.0f, 20.0f, "8", &ins, 48))
        prs = 37;
    if (Button(245.0f, vl + 20.0f, 20.0f, 20.0f, "9", &ins, 49))
        prs = 38;
    if (Button(275.0f, vl + 20.0f, 20.0f, 20.0f, "10", &ins, 50))
        prs = 39;
    if (Button(5.0f, vl + 50.0f, 20.0f, 20.0f, "11", &ins, 51))
        prs = 40;
    if (Button(35.0f, vl + 50.0f, 20.0f, 20.0f, "12", &ins, 52))
        prs = 41;
    if (Button(65.0f, vl + 50.0f, 20.0f, 20.0f, "13", &ins, 53))
        prs = 42;
    if (Button(95.0f, vl + 50.0f, 20.0f, 20.0f, "14", &ins, 54))
        prs = 43;
    if (Button(125.0f, vl + 50.0f, 20.0f, 20.0f, "15", &ins, 55))
        prs = 44;
    if (Button(155.0f, vl + 50.0f, 20.0f, 20.0f, "16", &ins, 56))
        prs = 45;
    if (Button(185.0f, vl + 50.0f, 20.0f, 20.0f, "17", &ins, 57))
        prs = 46;
    if (Button(215.0f, vl + 50.0f, 20.0f, 20.0f, "18", &ins, 58))
        prs = 47;
    if (Button(245.0f, vl + 50.0f, 20.0f, 20.0f, "19", &ins, 59))
        prs = 48;
    if (Button(275.0f, vl + 50.0f, 20.0f, 20.0f, "20", &ins, 60))
        prs = 49;
    if (Button(5.0f, vl + 80.0f, 20.0f, 20.0f, "21", &ins, 57))
        prs = 50;
    if (Button(35.0f, vl + 80.0f, 20.0f, 20.0f, "22", &ins, 58))
        prs = 51;
    if (Button(65.0f, vl + 80.0f, 20.0f, 20.0f, "23", &ins, 59))
        prs = 52;
    if (Button(95.0f, vl + 80.0f, 20.0f, 20.0f, "24", &ins, 60))
        prs = 53;
    if (ins > 0)
    {
        if (lastPreset != ins)
        {
            // Load the name
            auto ini = fio->OpenIniFile("resource\\ini\\loclighter.ini");
            if (ini)
            {
                char sect[32];
                sprintf_s(sect, "prs%i", ins);
                prsComment[0] = 0;
                if (!ini->ReadString(sect, "comment", prsComment, 64, ""))
                    prsComment[0] = 0;
                prsComment[63] = 0;
            }
            else
                prsComment[0] = 0;
        }
        if (prsComment[0])
            Print(textColor, winx, winx + winw, vl + 88.0f, 0.5f, true, "%.35s", prsComment);
    }
    else
        prsComment[0] = 0;
    lastPreset = ins;
    return prs;
}

void Window::SavePreset(int32_t prs)
{
    if (prs < 0)
        return;
    // Checking if able to work
    auto ini = fio->OpenIniFile("resource\\ini\\loclighter.ini");
    if (!ini)
        return;
    char sect[32];
    sprintf_s(sect, "prs%i", prs);
    for (int32_t i = 0; i < numElements; i++)
    {
        switch (list[i].type)
        {
        case ListElement::t_smooth:
            ini->WriteLong(sect, "smoothUseNormals", static_cast<int32_t>(smoothNorm));
            ini->WriteDouble(sect, "smoothRadius", smoothRad);
            break;
        case ListElement::t_blur:
            ini->WriteLong(sect, "blurTrace", static_cast<int32_t>(isTraceBlur));
            ini->WriteDouble(sect, "blurRadius", blurRad);
            ini->WriteDouble(sect, "blurAtt", blurAtt);
            ini->WriteDouble(sect, "blurCos", blurCos);
            ini->WriteDouble(sect, "blurKf", kBlur);
            break;
        case ListElement::t_amb:
            ini->WriteDouble(sect, "ambient_intens", list[i].st);
            ini->WriteDouble(sect, "ambient_clrR", list[i].c.x);
            ini->WriteDouble(sect, "ambient_clrG", list[i].c.y);
            ini->WriteDouble(sect, "ambient_clrB", list[i].c.z);
            break;
        case ListElement::t_light:
            ini->WriteDouble(sect, GenerateName(list[i].name, "_intens"), list[i].st);
            ini->WriteDouble(sect, GenerateName(list[i].name, "_clrR"), list[i].c.x);
            ini->WriteDouble(sect, GenerateName(list[i].name, "_clrG"), list[i].c.y);
            ini->WriteDouble(sect, GenerateName(list[i].name, "_clrB"), list[i].c.z);
            ini->WriteDouble(sect, GenerateName(list[i].name, "_cosine"), *list[i].cosine);
            ini->WriteDouble(sect, GenerateName(list[i].name, "_shadow"), *list[i].shadow);
            ini->WriteDouble(sect, GenerateName(list[i].name, "_bright"), *list[i].bright);
            ini->WriteDouble(sect, GenerateName(list[i].name, "_contr"), *list[i].contr);
            ini->WriteDouble(sect, GenerateName(list[i].name, "_gamma"), *list[i].gamma);
            ini->WriteLong(sect, GenerateName(list[i].name, "_isOn"), static_cast<int32_t>(*list[i].isOn));
            break;
        case ListElement::t_glight:
            ini->WriteDouble(sect, GenerateName(list[i].name, "_intens"), list[i].st);
            ini->WriteDouble(sect, GenerateName(list[i].name, "_clrR"), list[i].c.x);
            ini->WriteDouble(sect, GenerateName(list[i].name, "_clrG"), list[i].c.y);
            ini->WriteDouble(sect, GenerateName(list[i].name, "_clrB"), list[i].c.z);
            ini->WriteDouble(sect, GenerateName(list[i].name, "_cosine"), *list[i].cosine);
            ini->WriteDouble(sect, GenerateName(list[i].name, "_shadow"), *list[i].shadow);
            ini->WriteDouble(sect, GenerateName(list[i].name, "_bright"), *list[i].bright);
            ini->WriteDouble(sect, GenerateName(list[i].name, "_contr"), *list[i].contr);
            ini->WriteDouble(sect, GenerateName(list[i].name, "_gamma"), *list[i].gamma);
            ini->WriteDouble(sect, GenerateName(list[i].name, "_att0"), *list[i].att0);
            ini->WriteDouble(sect, GenerateName(list[i].name, "_att1"), *list[i].att1);
            ini->WriteDouble(sect, GenerateName(list[i].name, "_att2"), *list[i].att2);
            ini->WriteDouble(sect, GenerateName(list[i].name, "_range"), *list[i].range);
            ini->WriteLong(sect, GenerateName(list[i].name, "_isOn"), static_cast<int32_t>(*list[i].isOn));
            break;
        }
    }
}

void Window::LoadPreset(int32_t prs)
{
    if (prs < 0)
        return;
    // Checking if able to work
    auto ini = fio->OpenIniFile("resource\\ini\\loclighter.ini");
    if (!ini)
        return;
    char sect[32];
    sprintf_s(sect, "prs%i", prs);
    for (int32_t i = 0; i < numElements; i++)
    {
        switch (list[i].type)
        {
        case ListElement::t_smooth:
            smoothNorm = ini->GetInt(sect, "smoothUseNormals", static_cast<int32_t>(smoothNorm)) != 0;
            smoothRad = static_cast<float>(ini->GetDouble(sect, "smoothRadius", smoothRad));
            break;
        case ListElement::t_blur:
            isTraceBlur = ini->GetInt(sect, "blurTrace", static_cast<int32_t>(isTraceBlur)) != 0;
            blurRad = static_cast<float>(ini->GetDouble(sect, "blurRadius", blurRad));
            blurAtt = static_cast<float>(ini->GetDouble(sect, "blurAtt", blurAtt));
            blurCos = static_cast<float>(ini->GetDouble(sect, "blurCos", blurCos));
            kBlur = static_cast<float>(ini->GetDouble(sect, "blurKf", kBlur));
            break;
        case ListElement::t_amb:
            list[i].st = static_cast<float>(ini->GetDouble(sect, "ambient_intens", list[i].st));
            list[i].c.x = static_cast<float>(ini->GetDouble(sect, "ambient_clrR", list[i].c.x));
            list[i].c.y = static_cast<float>(ini->GetDouble(sect, "ambient_clrG", list[i].c.y));
            list[i].c.z = static_cast<float>(ini->GetDouble(sect, "ambient_clrB", list[i].c.z));
            break;
        case ListElement::t_light:
            list[i].st = static_cast<float>(ini->GetDouble(sect, GenerateName(list[i].name, "_intens"), list[i].st));
            list[i].c.x = static_cast<float>(ini->GetDouble(sect, GenerateName(list[i].name, "_clrR"), list[i].c.x));
            list[i].c.y = static_cast<float>(ini->GetDouble(sect, GenerateName(list[i].name, "_clrG"), list[i].c.y));
            list[i].c.z = static_cast<float>(ini->GetDouble(sect, GenerateName(list[i].name, "_clrB"), list[i].c.z));
            *list[i].cosine =
                static_cast<float>(ini->GetDouble(sect, GenerateName(list[i].name, "_cosine"), *list[i].cosine));
            *list[i].shadow =
                static_cast<float>(ini->GetDouble(sect, GenerateName(list[i].name, "_shadow"), *list[i].shadow));
            *list[i].bright =
                static_cast<float>(ini->GetDouble(sect, GenerateName(list[i].name, "_bright"), *list[i].bright));
            *list[i].contr =
                static_cast<float>(ini->GetDouble(sect, GenerateName(list[i].name, "_contr"), *list[i].contr));
            *list[i].gamma =
                static_cast<float>(ini->GetDouble(sect, GenerateName(list[i].name, "_gamma"), *list[i].gamma));
            *list[i].isOn =
                ini->GetInt(sect, GenerateName(list[i].name, "_isOn"), static_cast<int32_t>(*list[i].isOn)) != 0;
            break;
        case ListElement::t_glight:
            list[i].st = static_cast<float>(ini->GetDouble(sect, GenerateName(list[i].name, "_intens"), list[i].st));
            list[i].c.x = static_cast<float>(ini->GetDouble(sect, GenerateName(list[i].name, "_clrR"), list[i].c.x));
            list[i].c.y = static_cast<float>(ini->GetDouble(sect, GenerateName(list[i].name, "_clrG"), list[i].c.y));
            list[i].c.z = static_cast<float>(ini->GetDouble(sect, GenerateName(list[i].name, "_clrB"), list[i].c.z));
            *list[i].cosine =
                static_cast<float>(ini->GetDouble(sect, GenerateName(list[i].name, "_cosine"), *list[i].cosine));
            *list[i].shadow =
                static_cast<float>(ini->GetDouble(sect, GenerateName(list[i].name, "_shadow"), *list[i].shadow));
            *list[i].bright =
                static_cast<float>(ini->GetDouble(sect, GenerateName(list[i].name, "_bright"), *list[i].bright));
            *list[i].contr =
                static_cast<float>(ini->GetDouble(sect, GenerateName(list[i].name, "_contr"), *list[i].contr));
            *list[i].gamma =
                static_cast<float>(ini->GetDouble(sect, GenerateName(list[i].name, "_gamma"), *list[i].gamma));
            *list[i].att0 =
                static_cast<float>(ini->GetDouble(sect, GenerateName(list[i].name, "_att0"), *list[i].att0));
            *list[i].att1 =
                static_cast<float>(ini->GetDouble(sect, GenerateName(list[i].name, "_att1"), *list[i].att1));
            *list[i].att2 =
                static_cast<float>(ini->GetDouble(sect, GenerateName(list[i].name, "_att2"), *list[i].att2));
            *list[i].range =
                static_cast<float>(ini->GetDouble(sect, GenerateName(list[i].name, "_range"), *list[i].range));
            *list[i].isOn =
                ini->GetInt(sect, GenerateName(list[i].name, "_isOn"), static_cast<int32_t>(*list[i].isOn)) != 0;
            break;
        }
    }
    UpdateColors();
    UpdateLight(-1, true, true, true);
}

char *Window::GenerateName(const char *f, const char *n)
{
    strcpy_s(stringBuffer, f);
    strcat_s(stringBuffer, n);
    for (int32_t i = 0; stringBuffer[i]; i++)
        if (stringBuffer[i] == ' ')
            stringBuffer[i] = '_';
    return stringBuffer;
}

void Window::UpdateColors()
{
    for (int32_t i = 0; i < numElements; i++)
    {
        switch (list[i].type)
        {
        case ListElement::t_amb:
        case ListElement::t_light:
        case ListElement::t_glight:
            //*list[i].color = list[i].c*(2.0f*powf(list[i].st, 2.0f));
            *list[i].color = list[i].c * (list[i].st * 2.0f);
            break;
        }
    }
}

void Window::UpdateLight(int32_t idxLight, bool isCos, bool isAtt, bool isSdw)
{
    if (updateLight == -1)
    {
        isUpdateLight = true;
        updateLight = idxLight;
        updateLightCos |= isCos;
        updateLightAtt |= isAtt;
        updateLightSdw |= isSdw;
        if (updateLight < 0)
            updateLight = -2;
    }
    else
    {
        isUpdateLight = true;
        updateLight = -1;
        updateLightCos = updateLightAtt = updateLightSdw = true;
    }
}
