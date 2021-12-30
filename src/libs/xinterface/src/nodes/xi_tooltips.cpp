#include "xi_tooltips.h"
#include "../xinterface.h"
#include "strutils.h"

CXI_ToolTip::CXI_ToolTip(VXSERVICE *pPicService, VSTRSERVICE *pStrService, XYPOINT &pntScrSize)
    : m_pntScreenSize(pntScrSize)
{
    m_pPicService = pPicService;
    m_pStrService = pStrService;

    m_rs = XINTERFACE::GetRenderService();
    if (!m_rs)
    {
        throw std::runtime_error("No service: dx9render");
    }

    m_nTextureID = -1;
    m_pV = nullptr;
    m_pI = nullptr;
    m_nSquareQ = 0;
    m_dwBackColor = ARGB(255, 128, 128, 128);

    m_nFontID = -1;

    m_bDisableDraw = true;
    m_fTurnOnDelay = 2.f;
    m_fCurTimeLeft = 2.f;
    m_nMouseX = 0;
    m_nMouseY = 0;
}

CXI_ToolTip::~CXI_ToolTip()
{
    ReleaseAll();
}

void CXI_ToolTip::Draw()
{
    if (m_bDisableDraw)
        return;
    if (m_sText.empty())
        return; // there is nothing

    if (m_nSquareQ > 0)
    {
        m_rs->TextureSet(0, m_nTextureID);
        m_rs->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, m_nSquareQ * 4, m_nSquareQ * 2, m_pI, D3DFMT_INDEX16, m_pV,
                                     sizeof(XI_ONETEX_VERTEX), "iVideo");
    }

    const auto nX = (m_rPos.left + m_rPos.right) / 2;
    auto nY = m_rPos.top + m_pntTextOffset.y;
    for (long n = 0; n < m_aSubText.size(); n++)
    {
        m_rs->ExtPrint(m_nFontID, m_dwFontColor, 0, PR_ALIGN_CENTER, true, m_fFontScale, m_pntScreenSize.x,
                       m_pntScreenSize.y, nX, nY, "%s", m_aSubText[n].c_str());
        nY += static_cast<long>(m_rs->CharHeight(m_nFontID) * m_fFontScale);
    }
}

void CXI_ToolTip::SetByFormatString(XYRECT &rectOwner, INIFILE *pDefIni, const char *pFmtStr)
{
    if (!pFmtStr)
        return;
    char tokenID[256];
    char tokenString[2048];
    char pcToolTipType[128];
    char param[512];
    long n;

    m_rActiveZone = rectOwner;

    sprintf_s(pcToolTipType, sizeof(pcToolTipType), "ToolTip");
    // TODO: figure out why this was done:
    long m_nMaxStrWidth = -1; //~!~

    for (auto *pcParam = (char *)pFmtStr; pcParam && pcParam[0];)
    {
        if (nullptr == CXI_UTILS::StringGetTokenID(pcParam, tokenID, sizeof(tokenID)))
            break;
        const auto nTokenCode = CXI_UTILS::StringGetTokenCode(tokenID);
        if (CXI_UTILS::StringGetTokenString(pcParam, tokenString, sizeof(tokenString)))
        {
            const char *pStr = tokenString;
            switch (nTokenCode)
            {
            case InterfaceToken_text:
                if (tokenString[0] == '#')
                    m_sText = &tokenString[1];
                else
                    m_sText = m_pStrService->GetString(m_pStrService->GetStringNum(tokenString));
                break;
            case InterfaceToken_class:
                sprintf_s(pcToolTipType, sizeof(pcToolTipType), "%s", tokenString);
                break;
            case InterfaceToken_width:
                m_nMaxStrWidth = CXI_UTILS::StringGetLong(pStr);
            }
        }
    }

    //
    m_nFontID = -1;
    m_fFontScale = 1.f;
    m_dwFontColor = 0xFFFFFFFF;
    m_dwBackColor = ARGB(255, 128, 128, 128);
    m_nLeftSideWidth = m_nRightSideWidth = 0;
    m_uvBackLeft.left = m_uvBackRight.left = m_uvBackMiddle.left = 0.f;
    m_uvBackLeft.top = m_uvBackRight.top = m_uvBackMiddle.top = 0.f;
    m_uvBackLeft.right = m_uvBackRight.right = m_uvBackMiddle.right = 1.f;
    m_uvBackLeft.bottom = m_uvBackRight.bottom = m_uvBackMiddle.bottom = 1.f;
    m_pntTextOffset.x = 6;
    m_pntTextOffset.y = 4;
    m_sGroupName = "";
    m_nPicIndex_Left = m_nPicIndex_Right = m_nPicIndex_Middle = -1;
    //
    if (pDefIni)
    {
        if (pDefIni->ReadString(pcToolTipType, "font_id", param, sizeof(param), ""))
            m_nFontID = m_rs->LoadFont(param);
        m_fFontScale = pDefIni->GetFloat(pcToolTipType, "font_scale", m_fFontScale);
        m_dwFontColor = CINODE::GetIniARGB(pDefIni, pcToolTipType, nullptr, nullptr, "font_color", m_dwFontColor);
        if (m_nMaxStrWidth <= 0)
            m_nMaxStrWidth = pDefIni->GetLong(pcToolTipType, "str_width", m_pntScreenSize.x);
        m_pntTextOffset =
            CINODE::GetIniLongPoint(pDefIni, pcToolTipType, nullptr, nullptr, "str_offset", m_pntTextOffset);

        // read back info
        m_dwBackColor = CINODE::GetIniARGB(pDefIni, pcToolTipType, nullptr, nullptr, "back_color", m_dwBackColor);
        m_nLeftSideWidth = pDefIni->GetLong(pcToolTipType, "back_leftwidth", m_nLeftSideWidth);
        m_nRightSideWidth = pDefIni->GetLong(pcToolTipType, "back_rightwidth", m_nRightSideWidth);
        if (pDefIni->ReadString(pcToolTipType, "back_imagegroup", param, sizeof(param), ""))
        {
            m_sGroupName = param;
            m_nTextureID = m_pPicService->GetTextureID(m_sGroupName.c_str());
            if (pDefIni->ReadString(pcToolTipType, "back_imageleft", param, sizeof(param), ""))
            {
                m_nPicIndex_Left = m_pPicService->GetImageNum(m_sGroupName.c_str(), param);
                m_pPicService->GetTexturePos(m_nPicIndex_Left, m_uvBackLeft);
            }
            if (pDefIni->ReadString(pcToolTipType, "back_imageright", param, sizeof(param), ""))
            {
                m_nPicIndex_Right = m_pPicService->GetImageNum(m_sGroupName.c_str(), param);
                m_pPicService->GetTexturePos(m_nPicIndex_Right, m_uvBackRight);
            }
            if (pDefIni->ReadString(pcToolTipType, "back_imagemiddle", param, sizeof(param), ""))
            {
                m_nPicIndex_Middle = m_pPicService->GetImageNum(m_sGroupName.c_str(), param);
                m_pPicService->GetTexturePos(m_nPicIndex_Middle, m_uvBackMiddle);
            }
        }
        m_fTurnOnDelay = pDefIni->GetFloat(pcToolTipType, "turnondelay", m_fTurnOnDelay);
        m_nXRectangleOffset = pDefIni->GetLong(pcToolTipType, "horzcursoroffset", 0);
        m_nYRectangleOffsetUp = pDefIni->GetLong(pcToolTipType, "vertupcursoroffset", 0);
        m_nYRectangleOffsetDown = pDefIni->GetLong(pcToolTipType, "vertdowncursoroffset", 0);
    }
    if (m_nMaxStrWidth <= 0)
        m_nMaxStrWidth = m_pntScreenSize.x;

    //
    CXI_UTILS::SplitStringByWidth(m_sText.c_str(), m_nFontID, m_fFontScale, m_nMaxStrWidth, m_aSubText);
    m_nUseWidth = 0;
    for (n = 0; n < m_aSubText.size(); n++)
    {
        // m_aSubText[n].TrimLeft();
        // m_aSubText[n].TrimRight();
        TOREMOVE::trim(m_aSubText[n]);
        TOREMOVE::rtrim(m_aSubText[n]);

        const auto nW = m_rs->StringWidth((char *)m_aSubText[n].c_str(), m_nFontID, m_fFontScale, 0);
        if (nW > m_nUseWidth)
            m_nUseWidth = nW;
    }
    m_nUseWidth += m_pntTextOffset.x * 2;
    m_nUseHeight =
        m_aSubText.size() * static_cast<long>(m_rs->CharHeight(m_nFontID) * m_fFontScale) + 2 * m_pntTextOffset.x;

    m_nSquareQ = 3;
    CreateIndexBuffer();
    CreateVertexBuffer();
    UpdateIndexBuffer();

    ReplaceRectangle(rectOwner.right, rectOwner.bottom);
}

void CXI_ToolTip::MousePos(float fDeltaTime, long nX, long nY)
{
    if (m_nMouseX != nX || m_nMouseY != nY || nX < m_rActiveZone.left || nX > m_rActiveZone.right ||
        nY < m_rActiveZone.top || nY > m_rActiveZone.bottom)
    {
        m_nMouseX = nX;
        m_nMouseY = nY;
        m_bDisableDraw = true;
        m_fCurTimeLeft = m_fTurnOnDelay;
        return;
    }

    if (m_fCurTimeLeft >= 0.f)
    {
        m_fCurTimeLeft -= fDeltaTime;
        if (m_fCurTimeLeft <= 0.f)
        {
            m_bDisableDraw = false;
            ReplaceRectangle(nX, nY);
        }
    }
}

void CXI_ToolTip::ReleaseAll()
{
    PICTURE_TEXTURE_RELEASE(m_pPicService, m_sGroupName.c_str(), m_nTextureID);
    STORM_DELETE(m_pV);
    STORM_DELETE(m_pI);
    m_nSquareQ = 0;
    m_bDisableDraw = true;
    FONT_RELEASE(m_rs, m_nFontID);
}

void CXI_ToolTip::CreateIndexBuffer()
{
    if (m_nSquareQ > 0)
    {
        m_pI = new uint16_t[m_nSquareQ * 6];
        Assert(m_pI);
    }
}

void CXI_ToolTip::CreateVertexBuffer()
{
    if (m_nSquareQ > 0)
    {
        m_pV = new XI_ONETEX_VERTEX[m_nSquareQ * 4];
        Assert(m_pV);
    }
}

void CXI_ToolTip::UpdateIndexBuffer() const
{
    if (!m_pI)
        return;
    for (long n = 0; n < m_nSquareQ; n++)
    {
        m_pI[n * 6 + 0] = static_cast<uint16_t>(n * 4 + 0);
        m_pI[n * 6 + 1] = static_cast<uint16_t>(n * 4 + 1);
        m_pI[n * 6 + 2] = static_cast<uint16_t>(n * 4 + 2);

        m_pI[n * 6 + 3] = static_cast<uint16_t>(n * 4 + 1);
        m_pI[n * 6 + 4] = static_cast<uint16_t>(n * 4 + 3);
        m_pI[n * 6 + 5] = static_cast<uint16_t>(n * 4 + 2);
    }
}

void CXI_ToolTip::UpdateVertexBuffer()
{
    if (!m_pV)
        return;
    CXI_UTILS::WriteSquareToVertexBuffer(&m_pV[0], m_dwBackColor, m_uvBackMiddle, m_rPos.left + m_nLeftSideWidth,
                                         m_rPos.top, m_rPos.right - m_nRightSideWidth, m_rPos.bottom);
    CXI_UTILS::WriteSquareToVertexBuffer(&m_pV[4], m_dwBackColor, m_uvBackLeft, m_rPos.left, m_rPos.top,
                                         m_rPos.left + m_nLeftSideWidth, m_rPos.bottom);
    CXI_UTILS::WriteSquareToVertexBuffer(&m_pV[8], m_dwBackColor, m_uvBackRight, m_rPos.right - m_nRightSideWidth,
                                         m_rPos.top, m_rPos.right, m_rPos.bottom);
}

void CXI_ToolTip::ReplaceRectangle(long x, long y)
{
    auto top = y + m_nYRectangleOffsetUp;
    auto bottom = y + m_nYRectangleOffsetDown;
    if (top > m_rActiveZone.top)
        top = m_rActiveZone.top;
    if (bottom < m_rActiveZone.bottom)
        bottom = m_rActiveZone.bottom;

    // can we display info below the control?
    if (m_pntScreenSize.y - bottom >= m_nUseHeight)
    {
        m_rPos.top = bottom;
        m_rPos.bottom = m_rPos.top + m_nUseHeight;
    }
    else
    {
        if (top >= m_nUseHeight) // fits at the top
        {
            m_rPos.bottom = top;
            m_rPos.top = m_rPos.bottom - m_nUseHeight;
        }
        else
        {
            // put below anyway
            m_rPos.top = bottom;
            m_rPos.bottom = m_rPos.top + m_nUseHeight;
        }
    }

    // horizontal coordinates
    m_rPos.right = x + m_nUseWidth + m_nXRectangleOffset;
    if (m_rPos.right > m_pntScreenSize.x)
        m_rPos.right = m_pntScreenSize.x;
    m_rPos.left = m_rPos.right - m_nUseWidth;

    UpdateVertexBuffer();
}
