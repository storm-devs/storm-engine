#include "xi_button.h"

#include "string_compare.hpp"

CXI_BUTTON::CXI_BUTTON()
{
    m_rs = nullptr;
    m_sGroupName = nullptr;
    m_idTex = -1;
    m_pTex = nullptr;

    fXShadow = 0.f;
    fYShadow = 0.f;

    nPressedDelay = 0;
    nMaxDelay = 100;

    m_nFontNum = -1;

    m_bClickable = true;
    m_nNodeType = NODETYPE_BUTTON;

    m_fBlindSpeed = -1.f;
}

CXI_BUTTON::~CXI_BUTTON()
{
    ReleaseAll();
}

void CXI_BUTTON::Draw(bool bSelected, uint32_t Delta_Time)
{
    if (nPressedDelay > 0)
        nPressedDelay--;

    if (m_bUse)
    {
        // Create rectangle
        XI_ONETEX_VERTEX vFace[4];
        XI_ONETEX_VERTEX vShadow[4];

        auto dwFaceColor = m_dwFaceColor;
        if (bSelected && m_fBlindSpeed > 0.f)
        {
            dwFaceColor = ColorInterpolate(m_dwDarkColor, m_dwLightColor, m_fCurBlind);
            if (m_bUpBlind)
                m_fCurBlind += m_fBlindSpeed * Delta_Time;
            else
                m_fCurBlind -= m_fBlindSpeed * Delta_Time;
            if (m_fCurBlind < 0.f)
            {
                m_fCurBlind = 0.f;
                m_bUpBlind = true;
            }
            if (m_fCurBlind > 1.f)
            {
                m_fCurBlind = 1.f;
                m_bUpBlind = false;
            }
        }

        for (auto i = 0; i < 4; i++)
        {
            vFace[i].color = dwFaceColor;
            vFace[i].pos.z = 1.f;
            vShadow[i].color = m_dwShadowColor;
            vShadow[i].pos.z = 1.f;
        }

        vFace[0].tu = vShadow[0].tu = m_tRect.left;
        vFace[0].tv = vShadow[0].tv = m_tRect.top;
        vFace[1].tu = vShadow[1].tu = m_tRect.right;
        vFace[1].tv = vShadow[1].tv = m_tRect.top;
        vFace[2].tu = vShadow[2].tu = m_tRect.left;
        vFace[2].tv = vShadow[2].tv = m_tRect.bottom;
        vFace[3].tu = vShadow[3].tu = m_tRect.right;
        vFace[3].tv = vShadow[3].tv = m_tRect.bottom;

        vFace[0].pos.x = static_cast<float>(m_rect.left);
        vFace[0].pos.y = static_cast<float>(m_rect.top);
        vFace[1].pos.x = static_cast<float>(m_rect.right);
        vFace[1].pos.y = static_cast<float>(m_rect.top);
        vFace[2].pos.x = static_cast<float>(m_rect.left);
        vFace[2].pos.y = static_cast<float>(m_rect.bottom);
        vFace[3].pos.x = static_cast<float>(m_rect.right);
        vFace[3].pos.y = static_cast<float>(m_rect.bottom);

        for (auto i = 0; i < 4; i++)
        {
            if (nPressedDelay > 0)
            {
                vFace[i].pos.x += fXDeltaPress;
                vFace[i].pos.y += fYDeltaPress;
                vShadow[i].pos.x = vFace[i].pos.x + fXShadowPress;
                vShadow[i].pos.y = vFace[i].pos.y + fYShadowPress;
            }
            else
            {
                vShadow[i].pos.x = vFace[i].pos.x + fXShadow;
                vShadow[i].pos.y = vFace[i].pos.y + fYShadow;
            }
        }

        if (m_idTex != -1)
            m_rs->TextureSet(0, m_idTex);
        else
            m_rs->SetTexture(0, m_pTex ? m_pTex->m_pTexture : nullptr);

        if (m_idTex >= 0 || m_pTex != nullptr)
        {
            m_rs->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, XI_ONETEX_FVF, 2, vShadow, sizeof(XI_ONETEX_VERTEX), "iShadow");
            if (m_bClickable && m_bSelected)
                m_rs->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, XI_ONETEX_FVF, 2, vFace, sizeof(XI_ONETEX_VERTEX),
                                      "iButton");
            else
            {
                m_rs->SetRenderState(D3DRS_TEXTUREFACTOR, m_argbDisableColor);
                m_rs->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, XI_ONETEX_FVF, 2, vFace, sizeof(XI_ONETEX_VERTEX),
                                      "iDisabledNode");
            }
        }

        if (m_idString != -1L)
            if (nPressedDelay > 0)
            {
                m_rs->ExtPrint(m_nFontNum, m_dwFontColor, 0, PR_ALIGN_CENTER, false, 1.f, m_screenSize.x,
                               m_screenSize.y, (m_rect.left + m_rect.right) / 2 + static_cast<int>(fXDeltaPress),
                               m_rect.top + m_dwStrOffset + static_cast<int>(fYDeltaPress), "%s",
                               pStringService->GetString(m_idString));
            }
            else
            {
                m_rs->ExtPrint(m_nFontNum, m_dwFontColor, 0, PR_ALIGN_CENTER, false, 1.f, m_screenSize.x,
                               m_screenSize.y, (m_rect.left + m_rect.right) / 2, m_rect.top + m_dwStrOffset, "%s",
                               pStringService->GetString(m_idString));
            }
    }
}

bool CXI_BUTTON::Init(INIFILE *ini1, const char *name1, INIFILE *ini2, const char *name2, VDX9RENDER *rs,
                      XYRECT &hostRect, XYPOINT &ScreenSize)
{
    if (!CINODE::Init(ini1, name1, ini2, name2, rs, hostRect, ScreenSize))
        return false;
    return true;
}

void CXI_BUTTON::LoadIni(INIFILE *ini1, const char *name1, INIFILE *ini2, const char *name2)
{
    char param[255];
    XYPOINT tmpLPnt;

    // get font number
    if (ReadIniString(ini1, name1, ini2, name2, "font", param, sizeof(param), ""))
    {
        if ((m_nFontNum = m_rs->LoadFont(param)) == -1)
            core.Trace("can not load font:'%s'", param);
    }

    // get face color
    m_dwFaceColor = GetIniARGB(ini1, name1, ini2, name2, "faceColor", 0xFFFFFFFF);

    // selected color (light)
    m_dwLightColor = GetIniARGB(ini1, name1, ini2, name2, "lightColor", 0xFFFFFFFF);

    // (dark)
    m_dwDarkColor = GetIniARGB(ini1, name1, ini2, name2, "darkColor", 0xFFFFFFFF);

    // blinking speed
    m_fBlindSpeed = GetIniFloat(ini1, name1, ini2, name2, "blindTimeSec", -1.f);
    if (m_fBlindSpeed <= 0.f)
        m_fBlindSpeed = 1.f;
    else
        m_fBlindSpeed = .001f / m_fBlindSpeed;
    m_fCurBlind = 1.f;
    m_bUpBlind = false;

    // get disable color
    m_argbDisableColor = GetIniARGB(ini1, name1, ini2, name2, "disableColor", ARGB(255, 128, 128, 128));

    // get shadow color
    m_dwShadowColor = GetIniARGB(ini1, name1, ini2, name2, "shadowColor", ARGB(255, 0, 0, 0));

    // get font color
    m_dwFontColor = GetIniARGB(ini1, name1, ini2, name2, "fontColor", 0xFFFFFFFF);

    // get group name and get texture for this
    if (ReadIniString(ini1, name1, ini2, name2, "group", param, sizeof(param), ""))
    {
        m_idTex = pPictureService->GetTextureID(param);
        const auto len = strlen(param) + 1;
        m_sGroupName = new char[len];
        if (m_sGroupName == nullptr)
            throw std::runtime_error("allocate memory error");
        memcpy(m_sGroupName, param, len);

        // get button picture name
        if (ReadIniString(ini1, name1, ini2, name2, "picture", param, sizeof(param), ""))
            pPictureService->GetTexturePos(m_sGroupName, param, m_tRect);
    }
    else
    {
        if (ReadIniString(ini1, name1, ini2, name2, "videoTexture", param, sizeof(param), ""))
            m_pTex = m_rs->GetVideoTexture(param);
        m_tRect.left = 0.f;
        m_tRect.top = 0.f;
        m_tRect.right = 1.f;
        m_tRect.bottom = 1.f;
    }

    // get offset button image in case pressed button
    tmpLPnt = GetIniLongPoint(ini1, name1, ini2, name2, "pressPictureOffset", XYPOINT(0, 0));
    fXDeltaPress = static_cast<float>(tmpLPnt.x);
    fYDeltaPress = static_cast<float>(tmpLPnt.y);

    // get offset button shadow in case pressed button
    tmpLPnt = GetIniLongPoint(ini1, name1, ini2, name2, "shadowOffset", XYPOINT(0, 0));
    fXShadow = static_cast<float>(tmpLPnt.x);
    fYShadow = static_cast<float>(tmpLPnt.y);

    // get offset button shadow in case not pressed button
    tmpLPnt = GetIniLongPoint(ini1, name1, ini2, name2, "pressShadowOffset", XYPOINT(0, 0));
    fXShadowPress = static_cast<float>(tmpLPnt.x);
    fYShadowPress = static_cast<float>(tmpLPnt.y);

    // get press delay
    nMaxDelay = GetIniLong(ini1, name1, ini2, name2, "pressDelay", 20);

    m_dwStrOffset = GetIniLong(ini1, name1, ini2, name2, "strOffset", 0);

    m_idString = -1;
    if (ReadIniString(ini1, name1, ini2, name2, "group", param, sizeof(param), ""))
        m_idString = pStringService->GetStringNum(param);
}

void CXI_BUTTON::ReleaseAll()
{
    PICTURE_TEXTURE_RELEASE(pPictureService, m_sGroupName, m_idTex);
    STORM_DELETE(m_sGroupName);
    FONT_RELEASE(m_rs, m_nFontNum);
    VIDEOTEXTURE_RELEASE(m_rs, m_pTex);
}

int CXI_BUTTON::CommandExecute(int wActCode)
{
    if (m_bUse)
    {
        switch (wActCode)
        {
        case ACTION_ACTIVATE:
            nPressedDelay = nMaxDelay;
            break;
            // case ACTION_MOUSEDBLCLICK:
        case ACTION_MOUSECLICK:
            if (m_bClickable && m_bSelected)
                nPressedDelay = nMaxDelay;
            break;
        }
    }
    return -1;
}

bool CXI_BUTTON::IsClick(int buttonID, int32_t xPos, int32_t yPos)
{
    if (xPos >= m_rect.left && xPos <= m_rect.right && yPos >= m_rect.top && yPos <= m_rect.bottom && m_bClickable &&
        m_bSelected && m_bUse)
        return true;

    return false;
}

void CXI_BUTTON::ChangePosition(XYRECT &rNewPos)
{
    m_rect = rNewPos;
}

void CXI_BUTTON::SaveParametersToIni()
{
    char pcWriteParam[2048];

    auto pIni = fio->OpenIniFile(ptrOwner->m_sDialogFileName.c_str());
    if (!pIni)
    {
        core.Trace("Warning! Can`t open ini file name %s", ptrOwner->m_sDialogFileName.c_str());
        return;
    }

    // save position
    sprintf_s(pcWriteParam, sizeof(pcWriteParam), "%d,%d,%d,%d", m_rect.left, m_rect.top, m_rect.right, m_rect.bottom);
    pIni->WriteString(m_nodeName, "position", pcWriteParam);
}

void CXI_BUTTON::SetUsing(bool bUsing)
{
    m_bUse = bUsing;
    nPressedDelay = 0;
}

uint32_t CXI_BUTTON::MessageProc(int32_t msgcode, MESSAGE &message)
{
    switch (msgcode)
    {
    case 0: // change the position of the button
        m_rect.left = message.Long();
        m_rect.top = message.Long();
        m_rect.right = message.Long();
        m_rect.bottom = message.Long();
        GetAbsoluteRect(m_rect, message.Long());
        break;

    case 1: // change texture coordinates
        m_tRect.left = message.Float();
        m_tRect.top = message.Float();
        m_tRect.right = message.Float();
        m_tRect.bottom = message.Float();
        break;

    case 2: // change picture
    {
        const std::string &param = message.String();
        const auto len = param.size() + 1;
        if (len == 1)
            break;

        if (m_sGroupName == nullptr || !storm::iEquals(m_sGroupName, param))
        {
            PICTURE_TEXTURE_RELEASE(pPictureService, m_sGroupName, m_idTex);
            STORM_DELETE(m_sGroupName);

            m_sGroupName = new char[len];
            if (m_sGroupName == nullptr)
                throw std::runtime_error("allocate memory error");
            memcpy(m_sGroupName, param.c_str(), len);
            m_idTex = pPictureService->GetTextureID(m_sGroupName);
        }

        const std::string &param2 = message.String();
        pPictureService->GetTexturePos(m_sGroupName, param2.c_str(), m_tRect);
    }
    break;
    }

    return 0;
}
