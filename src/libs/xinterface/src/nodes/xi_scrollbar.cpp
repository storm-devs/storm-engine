#include "xi_scrollbar.h"

#define CLICK_TYPE_CENTER 0
#define CLICK_TYPE_LEFT 1
#define CLICK_TYPE_RIGHT 2

CXI_SCROLLBAR::CXI_SCROLLBAR()
{
    m_idTex = -1;
    m_sGroupName = nullptr;
    m_rs = nullptr;

    m_fXShadow = 0.f;
    m_fYShadow = 0.f;

    m_nPressedDelay = 0;
    m_nMaxDelay = 100;

    m_nFontID = -1;

    m_idVBuf = -1L;
    m_idIBuf = -1L;
    m_nVert = 0;
    m_nIndx = 0;

    m_bClickable = true;
    m_nNodeType = NODETYPE_SCROLLBAR;

    m_MouseClickParam.bExecute = false;
    m_MouseClickParam.fDelayMouseTime = 0.5f;
    m_MouseClickParam.fNextClickTime = 0.1f;
}

CXI_SCROLLBAR::~CXI_SCROLLBAR()
{
    ReleaseAll();
}

void CXI_SCROLLBAR::Draw(bool bSelected, uint32_t Delta_Time)
{
    if (m_nPressedDelay > 0)
        m_nPressedDelay--;

    if (m_MouseClickParam.bExecute)
    {
        CONTROL_STATE cs;
        core.Controls->GetControlState("ILClick", cs);
        if ((cs.state == CST_ACTIVE || cs.state == CST_ACTIVATED) &&
            (m_MouseClickParam.OldMousePoint.x == ptrOwner->GetMousePoint().x &&
             m_MouseClickParam.OldMousePoint.y == ptrOwner->GetMousePoint().y))
        {
            m_MouseClickParam.fCurMouseTime += .001f * core.GetRDeltaTime();
            if (m_MouseClickParam.fCurMouseTime > m_MouseClickParam.fNextClickTime)
            {
                m_MouseClickParam.fCurMouseTime = m_MouseClickParam.fDelayMouseTime;
                ChangeValue(m_bRightPress, false);
            }
        }
        else
            m_MouseClickParam.bExecute = false;
    }

    if (m_bUse)
    {
        if (bSelected ^ m_bPrevSelectStatus)
        {
            auto *pVert = static_cast<XI_ONETEX_VERTEX *>(m_rs->LockVertexBuffer(m_idVBuf));
            if (pVert != nullptr)
            {
                m_bPrevSelectStatus = bSelected;
                if (bSelected)
                    for (auto idx = 0; idx < 6 * 12; idx += 12)
                    {
                        pVert[idx + 4].tu = m_rectSelectCenterTex.left;
                        pVert[idx + 4].tv = m_rectSelectCenterTex.top;
                        pVert[idx + 5].tu = m_rectSelectCenterTex.right;
                        pVert[idx + 5].tv = m_rectSelectCenterTex.top;
                        pVert[idx + 6].tu = m_rectSelectCenterTex.left;
                        pVert[idx + 6].tv = m_rectSelectCenterTex.bottom;
                        pVert[idx + 7].tu = m_rectSelectCenterTex.right;
                        pVert[idx + 7].tv = m_rectSelectCenterTex.bottom;
                    }
                else
                    for (auto idx = 0; idx < 6 * 12; idx += 12)
                    {
                        pVert[idx + 4].tu = m_rectCenterTex.left;
                        pVert[idx + 4].tv = m_rectCenterTex.top;
                        pVert[idx + 5].tu = m_rectCenterTex.right;
                        pVert[idx + 5].tv = m_rectCenterTex.top;
                        pVert[idx + 6].tu = m_rectCenterTex.left;
                        pVert[idx + 6].tv = m_rectCenterTex.bottom;
                        pVert[idx + 7].tu = m_rectCenterTex.right;
                        pVert[idx + 7].tv = m_rectCenterTex.bottom;
                    }

                m_rs->UnLockVertexBuffer(m_idVBuf);
            }
        }

        m_rs->TextureSet(0, m_idTex);
        // show shadow
        if (m_nPressedDelay > 0)
            if (m_bRightPress)
                m_rs->DrawBuffer(m_idVBuf, sizeof(XI_ONETEX_VERTEX), m_idIBuf, 60, 12, 0, m_nIndx, "iShadow");
            else
                m_rs->DrawBuffer(m_idVBuf, sizeof(XI_ONETEX_VERTEX), m_idIBuf, 36, 12, 0, m_nIndx, "iShadow");
        else
            m_rs->DrawBuffer(m_idVBuf, sizeof(XI_ONETEX_VERTEX), m_idIBuf, 12, 12, 0, m_nIndx, "iShadow");

        // show button
        if (m_nPressedDelay > 0)
            if (m_bRightPress)
                m_rs->DrawBuffer(m_idVBuf, sizeof(XI_ONETEX_VERTEX), m_idIBuf, 48, 12, 0, m_nIndx, "iButton");
            else
                m_rs->DrawBuffer(m_idVBuf, sizeof(XI_ONETEX_VERTEX), m_idIBuf, 24, 12, 0, m_nIndx, "iButton");
        else
            m_rs->DrawBuffer(m_idVBuf, sizeof(XI_ONETEX_VERTEX), m_idIBuf, 0, 12, 0, m_nIndx, "iButton");
    }

    if (m_bShowString)
    {
        auto *pA = ptrOwner->AttributesPointer;
        if (pA)
        {
            pA = pA->GetAttributeClass(m_nodeName);
            if (pA)
            {
                auto *const pcStr = pA->GetAttribute("str");
                if (pcStr)
                {
                    m_rs->ExtPrint(m_nFontID, m_dwFontColor, 0, PR_ALIGN_CENTER, true, m_fFontScale, m_screenSize.x,
                                   m_screenSize.y, (m_rect.left + m_rect.right) / 2 + m_pntFontOffset.x,
                                   m_rect.top + m_pntFontOffset.y, "%s", pcStr);
                }
            }
        }
    }
}

bool CXI_SCROLLBAR::Init(INIFILE *ini1, const char *name1, INIFILE *ini2, const char *name2, VDX9RENDER *rs,
                         XYRECT &hostRect, XYPOINT &ScreenSize)
{
    if (!CINODE::Init(ini1, name1, ini2, name2, rs, hostRect, ScreenSize))
        return false;
    return true;
}

void CXI_SCROLLBAR::LoadIni(INIFILE *ini1, const char *name1, INIFILE *ini2, const char *name2)
{
    char param[255];
    FXYPOINT fPnt;

    // get font number
    m_nFontID = -1;
    if (ReadIniString(ini1, name1, ini2, name2, "fontID", param, sizeof(param), ""))
        if ((m_nFontID = m_rs->LoadFont(param)) == -1)
            core.Trace("can not load font:'%s'", param);
    // get font color
    m_dwFontColor = GetIniARGB(ini1, name1, ini2, name2, "fontColor", 0xFFFFFFFF);
    // get font scale
    m_fFontScale = GetIniFloat(ini1, name1, ini2, name2, "fontScale", 1.f);
    // get font offset
    m_pntFontOffset = GetIniLongPoint(ini1, name1, ini2, name2, "fontOffset", XYPOINT(0, 0));

    // get face color
    m_dwFaceColor = GetIniARGB(ini1, name1, ini2, name2, "faceColor", 0xFFFFFFFF);

    // get shadow color
    m_dwShadowColor = GetIniARGB(ini1, name1, ini2, name2, "shadowColor", ARGB(255, 0, 0, 0));

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

    // get offset button image in case pressed button
    fPnt = GetIniFloatPoint(ini1, name1, ini2, name2, "pressPictureOffset", FXYPOINT(0.f, 0.f));
    m_fXDeltaPress = fPnt.x;
    m_fYDeltaPress = fPnt.y;

    // get offset button shadow in case pressed button
    fPnt = GetIniFloatPoint(ini1, name1, ini2, name2, "shadowOffset", FXYPOINT(0.f, 0.f));
    m_fXShadow = fPnt.x;
    m_fYShadow = fPnt.y;

    // get offset button shadow in case not pressed button
    fPnt = GetIniFloatPoint(ini1, name1, ini2, name2, "pressShadowOffset", FXYPOINT(0.f, 0.f));
    m_fXShadowPress = fPnt.x;
    m_fYShadowPress = fPnt.y;

    // get press delay
    m_nMaxDelay = GetIniLong(ini1, name1, ini2, name2, "pressDelay", 20);

    m_nVert = 12 * 6;    //
    m_nIndx = 3 * 2 * 3; // 3 rectangle * 2 treangle into rectangle * 3 vertex into triangle
    m_idIBuf = m_rs->CreateIndexBuffer(m_nIndx * 2);
    m_idVBuf = m_rs->CreateVertexBuffer(XI_ONETEX_FVF, m_nVert * sizeof(XI_ONETEX_VERTEX), D3DUSAGE_WRITEONLY);

    // Lock buffers for write
    auto *pVert = static_cast<XI_ONETEX_VERTEX *>(m_rs->LockVertexBuffer(m_idVBuf));
    auto *pIndx = static_cast<uint16_t *>(m_rs->LockIndexBuffer(m_idIBuf));
    if (pVert == nullptr || pIndx == nullptr)
        throw std::runtime_error("can not create the index&vertex buffers");

    // fill triangles buffer
    auto i = 0;
    for (auto tidx = 0; tidx < 3; tidx++)
    {
        pIndx[i + 0] = tidx * 4;
        pIndx[i + 1] = tidx * 4 + 1;
        pIndx[i + 2] = tidx * 4 + 2;
        pIndx[i + 3] = tidx * 4 + 2;
        pIndx[i + 4] = tidx * 4 + 1;
        pIndx[i + 5] = tidx * 4 + 3;
        i += 6;
    }

    // fill vertex buffer
    for (i = 0; i < m_nVert; i++)
        pVert[i].pos.z = 1.f;
    for (i = 0; i < 12; i++)
        pVert[i].color = m_dwFaceColor;
    for (; i < 12 * 2; i++)
        pVert[i].color = m_dwShadowColor;
    for (; i < 12 * 3; i++)
        pVert[i].color = m_dwFaceColor;
    for (; i < 12 * 4; i++)
        pVert[i].color = m_dwShadowColor;
    for (; i < 12 * 5; i++)
        pVert[i].color = m_dwFaceColor;
    for (; i < 12 * 6; i++)
        pVert[i].color = m_dwShadowColor;

    m_rs->UnLockVertexBuffer(m_idVBuf);
    m_rs->UnLockIndexBuffer(m_idIBuf);

    if (ReadIniString(ini1, name1, ini2, name2, "leftPicture", param, sizeof(param), ""))
        pPictureService->GetTexturePos(m_sGroupName, param, m_frLeftTex);
    if (ReadIniString(ini1, name1, ini2, name2, "rightPicture", param, sizeof(param), ""))
        pPictureService->GetTexturePos(m_sGroupName, param, m_frRightTex);
    if (ReadIniString(ini1, name1, ini2, name2, "centerPicture", param, sizeof(param), ""))
        pPictureService->GetTexturePos(m_sGroupName, param, m_rectCenterTex);
    if (ReadIniString(ini1, name1, ini2, name2, "selectCenterPicture", param, sizeof(param), ""))
        pPictureService->GetTexturePos(m_sGroupName, param, m_rectSelectCenterTex);
    m_bPrevSelectStatus = false;
    m_nBarWidth = GetIniLong(ini1, name1, ini2, name2, "barWidth", -1);
    m_nSideWidth = GetIniLong(ini1, name1, ini2, name2, "sideWidth", -1);
    if (m_nSideWidth >= 0)
        m_nBarWidth = -1;
    if (m_nBarWidth < 0 && m_nSideWidth < 0)
        m_nSideWidth = (m_rect.bottom - m_rect.top);

    m_nIndx /= 3;
    UpdatePosition();

    m_bShowString = GetIniBool(ini1, name1, ini2, name2, "valueShow", false);
    m_fMinValue = GetIniFloat(ini1, name1, ini2, name2, "valueMin", 0.f);
    m_fMaxValue = GetIniFloat(ini1, name1, ini2, name2, "valueMax", 10.f);
    m_fStartValue = GetIniFloat(ini1, name1, ini2, name2, "valueStart", 10.f);
    m_fStepValue = GetIniFloat(ini1, name1, ini2, name2, "valueStep", 1.f);
    m_fSpeedMultiplay = GetIniFloat(ini1, name1, ini2, name2, "valueStepMultiply", 10.f);
    m_fCurValue = m_fStartValue;
    auto *pA = ptrOwner->AttributesPointer;
    if (pA)
        pA = pA->GetAttributeClass(m_nodeName);
    if (pA)
        m_fCurValue = pA->GetAttributeAsFloat("str", m_fStartValue);
    WriteDataToAttribute();
}

void CXI_SCROLLBAR::ReleaseAll()
{
    PICTURE_TEXTURE_RELEASE(pPictureService, m_sGroupName, m_idTex);
    STORM_DELETE(m_sGroupName);
    VERTEX_BUFFER_RELEASE(m_rs, m_idVBuf);
    INDEX_BUFFER_RELEASE(m_rs, m_idIBuf);
    FONT_RELEASE(m_rs, m_nFontID);
}

int CXI_SCROLLBAR::CommandExecute(int wActCode)
{
    if (m_bUse)
    {
        switch (wActCode)
        {
        case ACTION_RIGHTSTEP:
            m_nPressedDelay = m_nMaxDelay;
            m_bRightPress = true;
            ChangeValue(true, false);
            break;
        case ACTION_SPEEDRIGHT:
            m_nPressedDelay = m_nMaxDelay;
            m_bRightPress = true;
            ChangeValue(true, true);
            break;

        case ACTION_LEFTSTEP:
            m_nPressedDelay = m_nMaxDelay;
            m_bRightPress = false;
            ChangeValue(false, false);
            break;
        case ACTION_SPEEDLEFT:
            m_nPressedDelay = m_nMaxDelay;
            m_bRightPress = false;
            ChangeValue(false, true);
            break;

        case ACTION_MOUSECLICK:
            if (m_clickType == CLICK_TYPE_LEFT)
                m_bRightPress = false;
            if (m_clickType == CLICK_TYPE_RIGHT)
                m_bRightPress = true;
            if (m_clickType != CLICK_TYPE_CENTER)
            {
                m_nPressedDelay = m_nMaxDelay;
                m_MouseClickParam.bExecute = true;
                m_MouseClickParam.fCurMouseTime = 0;
                m_MouseClickParam.OldMousePoint = ptrOwner->GetMousePoint();
            }

            if (m_nPressedDelay > 0)
            {
                if (m_bRightPress)
                    return ACTION_RIGHTSTEP;
                return ACTION_LEFTSTEP;
            }
            break;
        }
    }
    return -1;
}

bool CXI_SCROLLBAR::IsClick(int buttonID, int32_t xPos, int32_t yPos)
{
    if (m_bClickable && m_bUse)
        if (xPos >= m_rect.left && xPos <= m_rect.right && yPos >= m_rect.top && yPos <= m_rect.bottom)
        {
            m_clickType = CLICK_TYPE_CENTER;
            int sideWidth = m_nSideWidth;
            if (sideWidth < 0)
                sideWidth = (m_rect.right - m_rect.left - m_nBarWidth) / 2;
            if (xPos < m_rect.left + sideWidth)
                m_clickType = CLICK_TYPE_LEFT;
            else if (xPos > m_rect.right - sideWidth)
                m_clickType = CLICK_TYPE_RIGHT;
            return true;
        }

    return false;
}

void CXI_SCROLLBAR::ChangePosition(XYRECT &rNewPos)
{
    m_rect = rNewPos;
    UpdatePosition();
}

void CXI_SCROLLBAR::SaveParametersToIni()
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

XYRECT CXI_SCROLLBAR::GetCursorRect()
{
    auto retRect = m_rect;
    retRect.right += static_cast<int32_t>(m_fXShadow);
    retRect.bottom += static_cast<int32_t>(m_fYShadow);
    return retRect;
}

uint32_t CXI_SCROLLBAR::MessageProc(int32_t msgcode, MESSAGE &message)
{
    switch (msgcode)
    {
    case 0: // set minimum value
        m_fMinValue = message.Float();
        break;
    case 1: // set maximum value
        m_fMaxValue = message.Float();
        break;
    case 2: // change value
        m_fCurValue = message.Float();
        WriteDataToAttribute();
        break;
    }

    return 0;
}

void CXI_SCROLLBAR::UpdatePosition() const
{
    auto *pVert = static_cast<XI_ONETEX_VERTEX *>(m_rs->LockVertexBuffer(m_idVBuf));

    auto idx = 0;
    int sideWidth = m_nSideWidth;
    if (sideWidth < 0)
        sideWidth = (m_rect.right - m_rect.left - m_nBarWidth) / 2;
    // fill normal picture
    //============================================================================
    // fill face (left,center,right)
    pVert[idx].tu = pVert[idx + 2].tu = m_frLeftTex.left;
    pVert[idx + 1].tu = pVert[idx + 3].tu = m_frLeftTex.right;
    pVert[idx].tv = pVert[idx + 1].tv = m_frLeftTex.top;
    pVert[idx + 2].tv = pVert[idx + 3].tv = m_frLeftTex.bottom;
    pVert[idx].pos.x = pVert[idx + 2].pos.x = static_cast<float>(m_rect.left);
    pVert[idx + 1].pos.x = pVert[idx + 3].pos.x = static_cast<float>(m_rect.left + sideWidth);
    pVert[idx].pos.y = pVert[idx + 1].pos.y = static_cast<float>(m_rect.top);
    pVert[idx + 2].pos.y = pVert[idx + 3].pos.y = static_cast<float>(m_rect.bottom);
    pVert[idx + 4].tu = pVert[idx + 6].tu = m_rectCenterTex.left;
    pVert[idx + 5].tu = pVert[idx + 7].tu = m_rectCenterTex.right;
    pVert[idx + 4].tv = pVert[idx + 5].tv = m_rectCenterTex.top;
    pVert[idx + 6].tv = pVert[idx + 7].tv = m_rectCenterTex.bottom;
    pVert[idx + 4].pos.x = pVert[idx + 6].pos.x = static_cast<float>(m_rect.left + sideWidth);
    pVert[idx + 5].pos.x = pVert[idx + 7].pos.x = static_cast<float>(m_rect.right - sideWidth);
    pVert[idx + 4].pos.y = pVert[idx + 5].pos.y = static_cast<float>(m_rect.top);
    pVert[idx + 6].pos.y = pVert[idx + 7].pos.y = static_cast<float>(m_rect.bottom);
    pVert[idx + 8].tu = pVert[idx + 10].tu = m_frRightTex.left;
    pVert[idx + 9].tu = pVert[idx + 11].tu = m_frRightTex.right;
    pVert[idx + 8].tv = pVert[idx + 9].tv = m_frRightTex.top;
    pVert[idx + 10].tv = pVert[idx + 11].tv = m_frRightTex.bottom;
    pVert[idx + 8].pos.x = pVert[idx + 10].pos.x = static_cast<float>(m_rect.right - sideWidth);
    pVert[idx + 9].pos.x = pVert[idx + 11].pos.x = static_cast<float>(m_rect.right);
    pVert[idx + 8].pos.y = pVert[idx + 9].pos.y = static_cast<float>(m_rect.top);
    pVert[idx + 10].pos.y = pVert[idx + 11].pos.y = static_cast<float>(m_rect.bottom);
    idx += 12;
    // fill shadow
    pVert[idx].tu = pVert[idx + 2].tu = m_frLeftTex.left;
    pVert[idx + 1].tu = pVert[idx + 3].tu = m_frLeftTex.right;
    pVert[idx].tv = pVert[idx + 1].tv = m_frLeftTex.top;
    pVert[idx + 2].tv = pVert[idx + 3].tv = m_frLeftTex.bottom;
    pVert[idx].pos.x = pVert[idx + 2].pos.x = static_cast<float>(m_rect.left) + m_fXShadow;
    pVert[idx + 1].pos.x = pVert[idx + 3].pos.x = static_cast<float>(m_rect.left + sideWidth) + m_fXShadow;
    pVert[idx].pos.y = pVert[idx + 1].pos.y = static_cast<float>(m_rect.top) + m_fYShadow;
    pVert[idx + 2].pos.y = pVert[idx + 3].pos.y = static_cast<float>(m_rect.bottom) + m_fYShadow;
    pVert[idx + 4].tu = pVert[idx + 6].tu = m_rectCenterTex.left;
    pVert[idx + 5].tu = pVert[idx + 7].tu = m_rectCenterTex.right;
    pVert[idx + 4].tv = pVert[idx + 5].tv = m_rectCenterTex.top;
    pVert[idx + 6].tv = pVert[idx + 7].tv = m_rectCenterTex.bottom;
    pVert[idx + 4].pos.x = pVert[idx + 6].pos.x = static_cast<float>(m_rect.left + sideWidth) + m_fXShadow;
    pVert[idx + 5].pos.x = pVert[idx + 7].pos.x = static_cast<float>(m_rect.right - sideWidth) + m_fXShadow;
    pVert[idx + 4].pos.y = pVert[idx + 5].pos.y = static_cast<float>(m_rect.top) + m_fYShadow;
    pVert[idx + 6].pos.y = pVert[idx + 7].pos.y = static_cast<float>(m_rect.bottom) + m_fYShadow;
    pVert[idx + 8].tu = pVert[idx + 10].tu = m_frRightTex.left;
    pVert[idx + 9].tu = pVert[idx + 11].tu = m_frRightTex.right;
    pVert[idx + 8].tv = pVert[idx + 9].tv = m_frRightTex.top;
    pVert[idx + 10].tv = pVert[idx + 11].tv = m_frRightTex.bottom;
    pVert[idx + 8].pos.x = pVert[idx + 10].pos.x = static_cast<float>(m_rect.right - sideWidth) + m_fXShadow;
    pVert[idx + 9].pos.x = pVert[idx + 11].pos.x = static_cast<float>(m_rect.right) + m_fXShadow;
    pVert[idx + 8].pos.y = pVert[idx + 9].pos.y = static_cast<float>(m_rect.top) + m_fYShadow;
    pVert[idx + 10].pos.y = pVert[idx + 11].pos.y = static_cast<float>(m_rect.bottom) + m_fYShadow;
    idx += 12;

    // fill left press picture
    //============================================================================
    // fill face (left,center,right)
    pVert[idx].tu = pVert[idx + 2].tu = m_frLeftTex.left;
    pVert[idx + 1].tu = pVert[idx + 3].tu = m_frLeftTex.right;
    pVert[idx].tv = pVert[idx + 1].tv = m_frLeftTex.top;
    pVert[idx + 2].tv = pVert[idx + 3].tv = m_frLeftTex.bottom;
    pVert[idx].pos.x = pVert[idx + 2].pos.x = static_cast<float>(m_rect.left) + m_fXDeltaPress;
    pVert[idx + 1].pos.x = pVert[idx + 3].pos.x = static_cast<float>(m_rect.left + sideWidth) + m_fXDeltaPress;
    pVert[idx].pos.y = pVert[idx + 1].pos.y = static_cast<float>(m_rect.top) + m_fYDeltaPress;
    pVert[idx + 2].pos.y = pVert[idx + 3].pos.y = static_cast<float>(m_rect.bottom) + m_fYDeltaPress;
    pVert[idx + 4].tu = pVert[idx + 6].tu = m_rectCenterTex.left;
    pVert[idx + 5].tu = pVert[idx + 7].tu = m_rectCenterTex.right;
    pVert[idx + 4].tv = pVert[idx + 5].tv = m_rectCenterTex.top;
    pVert[idx + 6].tv = pVert[idx + 7].tv = m_rectCenterTex.bottom;
    pVert[idx + 4].pos.x = pVert[idx + 6].pos.x = static_cast<float>(m_rect.left + sideWidth);
    pVert[idx + 5].pos.x = pVert[idx + 7].pos.x = static_cast<float>(m_rect.right - sideWidth);
    pVert[idx + 4].pos.y = pVert[idx + 5].pos.y = static_cast<float>(m_rect.top);
    pVert[idx + 6].pos.y = pVert[idx + 7].pos.y = static_cast<float>(m_rect.bottom);
    pVert[idx + 8].tu = pVert[idx + 10].tu = m_frRightTex.left;
    pVert[idx + 9].tu = pVert[idx + 11].tu = m_frRightTex.right;
    pVert[idx + 8].tv = pVert[idx + 9].tv = m_frRightTex.top;
    pVert[idx + 10].tv = pVert[idx + 11].tv = m_frRightTex.bottom;
    pVert[idx + 8].pos.x = pVert[idx + 10].pos.x = static_cast<float>(m_rect.right - sideWidth);
    pVert[idx + 9].pos.x = pVert[idx + 11].pos.x = static_cast<float>(m_rect.right);
    pVert[idx + 8].pos.y = pVert[idx + 9].pos.y = static_cast<float>(m_rect.top);
    pVert[idx + 10].pos.y = pVert[idx + 11].pos.y = static_cast<float>(m_rect.bottom);
    idx += 12;
    // fill shadow
    pVert[idx].tu = pVert[idx + 2].tu = m_frLeftTex.left;
    pVert[idx + 1].tu = pVert[idx + 3].tu = m_frLeftTex.right;
    pVert[idx].tv = pVert[idx + 1].tv = m_frLeftTex.top;
    pVert[idx + 2].tv = pVert[idx + 3].tv = m_frLeftTex.bottom;
    pVert[idx].pos.x = pVert[idx + 2].pos.x = static_cast<float>(m_rect.left) + m_fXDeltaPress + m_fXShadowPress;
    pVert[idx + 1].pos.x = pVert[idx + 3].pos.x =
        static_cast<float>(m_rect.left + sideWidth) + m_fXDeltaPress + m_fXShadowPress;
    pVert[idx].pos.y = pVert[idx + 1].pos.y = static_cast<float>(m_rect.top) + m_fYDeltaPress + m_fYShadowPress;
    pVert[idx + 2].pos.y = pVert[idx + 3].pos.y = static_cast<float>(m_rect.bottom) + m_fYDeltaPress + m_fYShadowPress;
    pVert[idx + 4].tu = pVert[idx + 6].tu = m_rectCenterTex.left;
    pVert[idx + 5].tu = pVert[idx + 7].tu = m_rectCenterTex.right;
    pVert[idx + 4].tv = pVert[idx + 5].tv = m_rectCenterTex.top;
    pVert[idx + 6].tv = pVert[idx + 7].tv = m_rectCenterTex.bottom;
    pVert[idx + 4].pos.x = pVert[idx + 6].pos.x = static_cast<float>(m_rect.left + sideWidth) + m_fXShadow;
    pVert[idx + 5].pos.x = pVert[idx + 7].pos.x = static_cast<float>(m_rect.right - sideWidth) + m_fXShadow;
    pVert[idx + 4].pos.y = pVert[idx + 5].pos.y = static_cast<float>(m_rect.top) + m_fYShadow;
    pVert[idx + 6].pos.y = pVert[idx + 7].pos.y = static_cast<float>(m_rect.bottom) + m_fYShadow;
    pVert[idx + 8].tu = pVert[idx + 10].tu = m_frRightTex.left;
    pVert[idx + 9].tu = pVert[idx + 11].tu = m_frRightTex.right;
    pVert[idx + 8].tv = pVert[idx + 9].tv = m_frRightTex.top;
    pVert[idx + 10].tv = pVert[idx + 11].tv = m_frRightTex.bottom;
    pVert[idx + 8].pos.x = pVert[idx + 10].pos.x = static_cast<float>(m_rect.right - sideWidth) + m_fXShadow;
    pVert[idx + 9].pos.x = pVert[idx + 11].pos.x = static_cast<float>(m_rect.right) + m_fXShadow;
    pVert[idx + 8].pos.y = pVert[idx + 9].pos.y = static_cast<float>(m_rect.top) + m_fYShadow;
    pVert[idx + 10].pos.y = pVert[idx + 11].pos.y = static_cast<float>(m_rect.bottom) + m_fYShadow;
    idx += 12;

    // fill right press picture
    //============================================================================
    // fill face (left,center,right)
    pVert[idx].tu = pVert[idx + 2].tu = m_frLeftTex.left;
    pVert[idx + 1].tu = pVert[idx + 3].tu = m_frLeftTex.right;
    pVert[idx].tv = pVert[idx + 1].tv = m_frLeftTex.top;
    pVert[idx + 2].tv = pVert[idx + 3].tv = m_frLeftTex.bottom;
    pVert[idx].pos.x = pVert[idx + 2].pos.x = static_cast<float>(m_rect.left);
    pVert[idx + 1].pos.x = pVert[idx + 3].pos.x = static_cast<float>(m_rect.left + sideWidth);
    pVert[idx].pos.y = pVert[idx + 1].pos.y = static_cast<float>(m_rect.top);
    pVert[idx + 2].pos.y = pVert[idx + 3].pos.y = static_cast<float>(m_rect.bottom);
    pVert[idx + 4].tu = pVert[idx + 6].tu = m_rectCenterTex.left;
    pVert[idx + 5].tu = pVert[idx + 7].tu = m_rectCenterTex.right;
    pVert[idx + 4].tv = pVert[idx + 5].tv = m_rectCenterTex.top;
    pVert[idx + 6].tv = pVert[idx + 7].tv = m_rectCenterTex.bottom;
    pVert[idx + 4].pos.x = pVert[idx + 6].pos.x = static_cast<float>(m_rect.left + sideWidth);
    pVert[idx + 5].pos.x = pVert[idx + 7].pos.x = static_cast<float>(m_rect.right - sideWidth);
    pVert[idx + 4].pos.y = pVert[idx + 5].pos.y = static_cast<float>(m_rect.top);
    pVert[idx + 6].pos.y = pVert[idx + 7].pos.y = static_cast<float>(m_rect.bottom);
    pVert[idx + 8].tu = pVert[idx + 10].tu = m_frRightTex.left;
    pVert[idx + 9].tu = pVert[idx + 11].tu = m_frRightTex.right;
    pVert[idx + 8].tv = pVert[idx + 9].tv = m_frRightTex.top;
    pVert[idx + 10].tv = pVert[idx + 11].tv = m_frRightTex.bottom;
    pVert[idx + 8].pos.x = pVert[idx + 10].pos.x = static_cast<float>(m_rect.right - sideWidth) + m_fXDeltaPress;
    pVert[idx + 9].pos.x = pVert[idx + 11].pos.x = static_cast<float>(m_rect.right) + m_fXDeltaPress;
    pVert[idx + 8].pos.y = pVert[idx + 9].pos.y = static_cast<float>(m_rect.top) + m_fYDeltaPress;
    pVert[idx + 10].pos.y = pVert[idx + 11].pos.y = static_cast<float>(m_rect.bottom) + m_fYDeltaPress;
    idx += 12;
    // fill shadow
    pVert[idx].tu = pVert[idx + 2].tu = m_frLeftTex.left;
    pVert[idx + 1].tu = pVert[idx + 3].tu = m_frLeftTex.right;
    pVert[idx].tv = pVert[idx + 1].tv = m_frLeftTex.top;
    pVert[idx + 2].tv = pVert[idx + 3].tv = m_frLeftTex.bottom;
    pVert[idx].pos.x = pVert[idx + 2].pos.x = static_cast<float>(m_rect.left) + m_fXShadow;
    pVert[idx + 1].pos.x = pVert[idx + 3].pos.x = static_cast<float>(m_rect.left + sideWidth) + m_fXShadow;
    pVert[idx].pos.y = pVert[idx + 1].pos.y = static_cast<float>(m_rect.top) + m_fYShadow;
    pVert[idx + 2].pos.y = pVert[idx + 3].pos.y = static_cast<float>(m_rect.bottom) + m_fYShadow;
    pVert[idx + 4].tu = pVert[idx + 6].tu = m_rectCenterTex.left;
    pVert[idx + 5].tu = pVert[idx + 7].tu = m_rectCenterTex.right;
    pVert[idx + 4].tv = pVert[idx + 5].tv = m_rectCenterTex.top;
    pVert[idx + 6].tv = pVert[idx + 7].tv = m_rectCenterTex.bottom;
    pVert[idx + 4].pos.x = pVert[idx + 6].pos.x = static_cast<float>(m_rect.left + sideWidth) + m_fXShadow;
    pVert[idx + 5].pos.x = pVert[idx + 7].pos.x = static_cast<float>(m_rect.right - sideWidth) + m_fXShadow;
    pVert[idx + 4].pos.y = pVert[idx + 5].pos.y = static_cast<float>(m_rect.top) + m_fYShadow;
    pVert[idx + 6].pos.y = pVert[idx + 7].pos.y = static_cast<float>(m_rect.bottom) + m_fYShadow;
    pVert[idx + 8].tu = pVert[idx + 10].tu = m_frRightTex.left;
    pVert[idx + 9].tu = pVert[idx + 11].tu = m_frRightTex.right;
    pVert[idx + 8].tv = pVert[idx + 9].tv = m_frRightTex.top;
    pVert[idx + 10].tv = pVert[idx + 11].tv = m_frRightTex.bottom;
    pVert[idx + 8].pos.x = pVert[idx + 10].pos.x =
        static_cast<float>(m_rect.right - sideWidth) + m_fXDeltaPress + m_fXShadowPress;
    pVert[idx + 9].pos.x = pVert[idx + 11].pos.x = static_cast<float>(m_rect.right) + m_fXDeltaPress + m_fXShadowPress;
    pVert[idx + 8].pos.y = pVert[idx + 9].pos.y = static_cast<float>(m_rect.top) + m_fYDeltaPress + m_fYShadowPress;
    pVert[idx + 10].pos.y = pVert[idx + 11].pos.y =
        static_cast<float>(m_rect.bottom) + m_fYDeltaPress + m_fYShadowPress;
    idx += 12;

    m_rs->UnLockVertexBuffer(m_idVBuf);
}

void CXI_SCROLLBAR::WriteDataToAttribute() const
{
    auto *pRoot = ptrOwner->AttributesPointer;
    if (!pRoot)
        return;
    ATTRIBUTES *pA = pRoot->GetAttributeClass(m_nodeName);
    if (!pA)
        pA = pRoot->CreateSubAClass(pRoot, m_nodeName);
    if (!pA)
        return;
    pA->SetAttributeUseFloat("str", m_fCurValue);
}

void CXI_SCROLLBAR::ChangeValue(bool bGrowing, bool bMultiply)
{
    float fVal = m_fCurValue;
    float fStep = m_fStepValue;
    if (bMultiply)
        fStep *= m_fSpeedMultiplay;
    if (bGrowing)
        fVal += fStep;
    else
        fVal -= fStep;
    if (fVal < m_fMinValue)
        fVal = m_fMinValue;
    if (fVal > m_fMaxValue)
        fVal = m_fMaxValue;
    if (fVal != m_fCurValue)
    {
        m_fCurValue = fVal;
        WriteDataToAttribute();
    }
}
