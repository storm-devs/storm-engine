#include "xi_lr_changer.h"

void SetOneTextureCoordinate(XI_ONETEX_VERTEX v[4], const FXYRECT &tr)
{
    v[0].tu = tr.left;
    v[0].tv = tr.top;
    v[1].tu = tr.right;
    v[1].tv = tr.top;
    v[2].tu = tr.left;
    v[2].tv = tr.bottom;
    v[3].tu = tr.right;
    v[3].tv = tr.bottom;
}

void SetRectanglePosition(XI_ONETEX_VERTEX v[4], const FXYRECT &pr)
{
    v[0].pos.x = static_cast<float>(pr.left);
    v[0].pos.y = static_cast<float>(pr.top);
    v[1].pos.x = static_cast<float>(pr.right);
    v[1].pos.y = static_cast<float>(pr.top);
    v[2].pos.x = static_cast<float>(pr.left);
    v[2].pos.y = static_cast<float>(pr.bottom);
    v[3].pos.x = static_cast<float>(pr.right);
    v[3].pos.y = static_cast<float>(pr.bottom);
}

CXI_LRCHANGER::CXI_LRCHANGER()
{
    m_idTex = -1;
    m_rs = nullptr;

    m_ShadowShift.x = m_ShadowShift.y = 0.f;
    m_PressShadowShift.x = m_PressShadowShift.y = 0.f;

    nPressedDelay = 0;
    nMaxDelay = 100;

    m_bClickable = true;
    m_nNodeType = NODETYPE_LRCHANGER;
}

CXI_LRCHANGER::~CXI_LRCHANGER()
{
    ReleaseAll();
}

void CXI_LRCHANGER::Draw(bool bSelected, uint32_t Delta_Time)
{
    if (m_bUse)
    {
        if (nPressedDelay > 0)
            nPressedDelay--;

        // Create rectangle
        XI_ONETEX_VERTEX vFace[4];
        XI_ONETEX_VERTEX vShadow[4];

        // calculate face color
        uint32_t curCol;
        if (m_bBlindIncrement)
            m_dwCurBlindState += Delta_Time;
        else
            m_dwCurBlindState -= Delta_Time;
        if (m_dwCurBlindState < 0)
        {
            m_dwCurBlindState = 0;
            m_bBlindIncrement = true;
        }
        if (m_dwCurBlindState > m_dwBlindDelay)
        {
            m_dwCurBlindState = m_dwBlindDelay;
            m_bBlindIncrement = false;
        }
        if (bSelected)
            curCol = ColorInterpolate(m_dwDarkSelCol, m_dwLightSelCol,
                                      static_cast<float>(m_dwCurBlindState) / m_dwBlindDelay);
        else
            curCol = m_dwFaceColor;

        for (auto i = 0; i < 4; i++)
        {
            vFace[i].color = curCol;
            vFace[i].pos.z = 1.f;
            vShadow[i].color = m_dwShadowColor;
            vShadow[i].pos.z = 1.f;
        }

        m_rs->TextureSet(0, m_idTex);

        // show left button
        SetOneTextureCoordinate(vFace, m_tLRect);
        SetOneTextureCoordinate(vShadow, m_tLRect);
        if (nPressedDelay > 0 && m_bLeftPress)
        {
            SetRectanglePosition(vFace, m_posLRect + m_PressShift);
            SetRectanglePosition(vShadow, m_posLRect + m_PressShift + m_PressShadowShift);
        }
        else
        {
            SetRectanglePosition(vFace, m_posLRect);
            SetRectanglePosition(vShadow, m_posLRect + m_ShadowShift);
        }
        m_rs->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, XI_ONETEX_FVF, 2, vShadow, sizeof(XI_ONETEX_VERTEX), "iShadow");
        m_rs->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, XI_ONETEX_FVF, 2, vFace, sizeof(XI_ONETEX_VERTEX), "iIcon");
        // show right button
        SetOneTextureCoordinate(vFace, m_tRRect);
        SetOneTextureCoordinate(vShadow, m_tRRect);
        if (nPressedDelay > 0 && !m_bLeftPress)
        {
            SetRectanglePosition(vFace, m_posRRect + m_PressShift);
            SetRectanglePosition(vShadow, m_posRRect + m_PressShift + m_PressShadowShift);
        }
        else
        {
            SetRectanglePosition(vFace, m_posRRect);
            SetRectanglePosition(vShadow, m_posRRect + m_ShadowShift);
        }
        m_rs->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, XI_ONETEX_FVF, 2, vShadow, sizeof(XI_ONETEX_VERTEX), "iShadow");
        m_rs->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, XI_ONETEX_FVF, 2, vFace, sizeof(XI_ONETEX_VERTEX), "iIcon");
    }
}

bool CXI_LRCHANGER::Init(INIFILE *ini1, const char *name1, INIFILE *ini2, const char *name2, VDX9RENDER *rs,
                         XYRECT &hostRect, XYPOINT &ScreenSize)
{
    if (!CINODE::Init(ini1, name1, ini2, name2, rs, hostRect, ScreenSize))
        return false;
    return true;
}

void CXI_LRCHANGER::LoadIni(INIFILE *ini1, const char *name1, INIFILE *ini2, const char *name2)
{
    char param[255];

    // set buttons position
    const auto nSpaceWidth = GetIniLong(ini1, name1, ini2, name2, "spaceWidth", 0);
    m_posLRect.left = static_cast<float>(m_rect.left);
    m_posLRect.top = static_cast<float>(m_rect.top);
    m_posLRect.right = static_cast<float>(m_rect.left) + (m_rect.right - m_rect.left - nSpaceWidth) / 2.f;
    m_posLRect.bottom = static_cast<float>(m_rect.bottom);
    m_posRRect.left = static_cast<float>(m_rect.left) + (m_rect.right - m_rect.left + nSpaceWidth) / 2.f;
    m_posRRect.top = static_cast<float>(m_rect.top);
    m_posRRect.right = static_cast<float>(m_rect.right);
    m_posRRect.bottom = static_cast<float>(m_rect.bottom);

    // get face color
    m_dwFaceColor = GetIniARGB(ini1, name1, ini2, name2, "faceColor", 0xFFFFFFFF);

    // get shadow color
    m_dwShadowColor = GetIniARGB(ini1, name1, ini2, name2, "shadowColor", ARGB(255, 0, 0, 0));

    // get light select color
    m_dwLightSelCol = GetIniARGB(ini1, name1, ini2, name2, "lightSelectColor", ARGB(255, 138, 138, 138));

    // get dark select color
    m_dwDarkSelCol = GetIniARGB(ini1, name1, ini2, name2, "darkSelectColor", ARGB(255, 108, 108, 108));

    // get blind delay
    m_dwBlindDelay = GetIniLong(ini1, name1, ini2, name2, "blindDelay", 0);

    // get group name and get texture for this
    m_idTex = -1;
    m_sGroupName = nullptr;
    if (ReadIniString(ini1, name1, ini2, name2, "group", param, sizeof(param), ""))
    {
        const auto len = strlen(param) + 1;
        m_sGroupName = new char[len];
        if (m_sGroupName == nullptr)
            throw std::runtime_error("allocate memory error");
        memcpy(m_sGroupName, param, len);
        m_idTex = pPictureService->GetTextureID(param);
    }

    // get buttons picture name
    if (ReadIniString(ini1, name1, ini2, name2, "lpicture", param, sizeof(param), ""))
        pPictureService->GetTexturePos(m_sGroupName, param, m_tLRect);
    if (ReadIniString(ini1, name1, ini2, name2, "rpicture", param, sizeof(param), ""))
        pPictureService->GetTexturePos(m_sGroupName, param, m_tRRect);

    // get offset button image in case pressed button
    m_PressShift = GetIniFloatPoint(ini1, name1, ini2, name2, "pressPictureOffset", FXYPOINT(0.f, 0.f));

    // get offset button shadow in case not pressed button
    m_ShadowShift = GetIniFloatPoint(ini1, name1, ini2, name2, "shadowOffset", FXYPOINT(0.f, 0.f));

    // get offset button shadow in case pressed button
    m_PressShadowShift = GetIniFloatPoint(ini1, name1, ini2, name2, "pressShadowOffset", FXYPOINT(0.f, 0.f));

    // get press delay
    nMaxDelay = GetIniLong(ini1, name1, ini2, name2, "pressDelay", 20);
}

void CXI_LRCHANGER::ReleaseAll()
{
    PICTURE_TEXTURE_RELEASE(pPictureService, m_sGroupName, m_idTex);
    STORM_DELETE(m_sGroupName);
}

int CXI_LRCHANGER::CommandExecute(int wActCode)
{
    if (m_bUse)
    {
        switch (wActCode)
        {
        case ACTION_ACTIVATE:
        case ACTION_MOUSECLICK:
            nPressedDelay = nMaxDelay;
            core.Event("IEvnt_MouseClick", "sl", m_nodeName, m_bLeftPress ? 0 : 1);
            break;
        case ACTION_LEFTSTEP:
        case ACTION_SPEEDLEFT:
            nPressedDelay = nMaxDelay;
            m_bLeftPress = true;
            break;
        case ACTION_RIGHTSTEP:
        case ACTION_SPEEDRIGHT:
            nPressedDelay = nMaxDelay;
            m_bLeftPress = false;
            break;
        }
    }
    return -1;
}

bool CXI_LRCHANGER::IsClick(int buttonID, int32_t xPos, int32_t yPos)
{
    if (xPos >= m_rect.left && xPos <= m_rect.right && yPos >= m_rect.top && yPos <= m_rect.bottom && m_bClickable &&
        m_bUse)
    {
        // check left button
        if (xPos <= m_posLRect.right)
        {
            m_bLeftPress = true;
            return true;
        }
        if (xPos >= m_posRRect.left)
        {
            m_bLeftPress = false;
            return true;
        }
    }
    return false;
}

void CXI_LRCHANGER::ChangePosition(XYRECT &rNewPos)
{
    const auto nSpaceWidth = static_cast<int32_t>((m_rect.right - m_rect.left) - (m_posLRect.right - m_posLRect.left) -
                                               (m_posRRect.right - m_posRRect.left));

    m_rect = rNewPos;

    // set buttons position
    m_posLRect.left = static_cast<float>(m_rect.left);
    m_posLRect.top = static_cast<float>(m_rect.top);
    m_posLRect.right = static_cast<float>(m_rect.left) + (m_rect.right - m_rect.left - nSpaceWidth) / 2.f;
    m_posLRect.bottom = static_cast<float>(m_rect.bottom);
    m_posRRect.left = static_cast<float>(m_rect.left) + (m_rect.right - m_rect.left + nSpaceWidth) / 2.f;
    m_posRRect.top = static_cast<float>(m_rect.top);
    m_posRRect.right = static_cast<float>(m_rect.right);
    m_posRRect.bottom = static_cast<float>(m_rect.bottom);
}

void CXI_LRCHANGER::SaveParametersToIni()
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

int32_t CXI_LRCHANGER::GetClickState()
{
    if (m_bLeftPress)
        return 1;
    return 2;
}
