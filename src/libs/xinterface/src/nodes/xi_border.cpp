#include "xi_border.h"

#include "core.h"

#include "v_file_service.h"
#include "xi_image.h"

CXI_BORDER::CXI_BORDER()
{
    m_rs = nullptr;

    m_idTex = -1;

    m_idVBuf = -1;
    m_idIBuf = -1;

    m_nSquareQ = 0;
    m_nNodeType = NODETYPE_BORDER;
    m_pBackImage = nullptr;
    m_pCaptionImage = nullptr;
}

CXI_BORDER::~CXI_BORDER()
{
    ReleaseAll();
}

int CXI_BORDER::CommandExecute(int wActCode)
{
    return -1;
}

void CXI_BORDER::Draw(bool bSelected, uint32_t Delta_Time)
{
    if (m_bUse)
    {
        if (m_pBackImage)
            m_pBackImage->Draw();
        if (m_pCaptionImage)
            m_pCaptionImage->Draw();

        if (m_idTex >= 0)
        {
            m_rs->TextureSet(0, m_idTex);
            if (m_idVBuf >= 0 && m_idIBuf >= 0)
                m_rs->DrawBuffer(m_idVBuf, sizeof(XI_ONETEX_VERTEX), m_idIBuf, 0, m_nSquareQ * 4, 0, m_nSquareQ * 2,
                                 "iBounder");
        }
    }
}

bool CXI_BORDER::Init(INIFILE *ini1, const char *name1, INIFILE *ini2, const char *name2, VDX9RENDER *rs,
                      XYRECT &hostRect, XYPOINT &ScreenSize)
{
    if (!CINODE::Init(ini1, name1, ini2, name2, rs, hostRect, ScreenSize))
        return false;
    return true;
}

void CXI_BORDER::ReleaseAll()
{
    m_bUse = false;
    PICTURE_TEXTURE_RELEASE(pPictureService, m_sGroupName.c_str(), m_idTex);
    VERTEX_BUFFER_RELEASE(m_rs, m_idVBuf);
    INDEX_BUFFER_RELEASE(m_rs, m_idIBuf);
    STORM_DELETE(m_pBackImage);
    STORM_DELETE(m_pCaptionImage);
}

bool CXI_BORDER::IsClick(int buttonID, int32_t xPos, int32_t yPos)
{
    return false;
}

void CXI_BORDER::ChangePosition(XYRECT &rNewPos)
{
    m_rect = rNewPos;
    // move the background picture
    if (m_pBackImage)
        m_pBackImage->SetPosition(m_rect);
    // move the caption image
    if (m_pCaptionImage && m_nCaptionHeight > 0)
    {
        auto rCapRect = m_rCapRect;
        rCapRect.top += m_rect.top;
        rCapRect.bottom = rCapRect.top + m_nCaptionHeight - rCapRect.bottom;
        rCapRect.left += m_rect.left;
        rCapRect.right = m_rect.right - rCapRect.right;
        m_pCaptionImage->SetPosition(rCapRect);
    }

    FillVertexBuffers();
}

void CXI_BORDER::SaveParametersToIni()
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

void CXI_BORDER::LoadIni(INIFILE *ini1, const char *name1, INIFILE *ini2, const char *name2)
{
    char param[256];

    // get back image
    if (ReadIniString(ini1, name1, ini2, name2, "backimage", param, sizeof(param), ""))
    {
        m_pBackImage = new CXI_IMAGE;
        if (m_pBackImage)
        {
            m_pBackImage->LoadAccordingToString(param);
            m_pBackImage->SetPosition(m_rect);
        }
    }

    // get caption rectangle
    m_nCaptionHeight = GetIniLong(ini1, name1, ini2, name2, "captionheight", 0);
    m_mCaptionDividerHeight = GetIniLong(ini1, name1, ini2, name2, "captiondividerheight", 2);
    if (m_nCaptionHeight > 0 && ReadIniString(ini1, name1, ini2, name2, "captionimage", param, sizeof(param), ""))
    {
        m_pCaptionImage = new CXI_IMAGE;
        if (m_pCaptionImage)
        {
            m_pCaptionImage->LoadAccordingToString(param);
        }
    }

    // get show color
    m_dwColor = GetIniARGB(ini1, name1, ini2, name2, "color", 0xFFFFFFFF);
    // Get texture name and load that texture
    m_sGroupName = "";
    if (ReadIniString(ini1, name1, ini2, name2, "groupName", param, sizeof(param), ""))
    {
        m_sGroupName = param;
        m_idTex = pPictureService->GetTextureID(m_sGroupName.c_str());
    }

    // create index and vertex buffers
    m_nSquareQ = 4 + 4 + 1; // 4 edge & 4 angle & 1 caption
    m_idVBuf = m_rs->CreateVertexBuffer(XI_ONETEX_FVF, m_nSquareQ * 4 * sizeof(XI_ONETEX_VERTEX), D3DUSAGE_WRITEONLY);
    m_idIBuf = m_rs->CreateIndexBuffer(m_nSquareQ * 6 * 2);
    if (m_nCaptionHeight == 0 || !m_pCaptionImage)
        m_nSquareQ--; // don`t show caption

    // get pictures
    // left top corner
    m_nLeftTopPicture = -1;
    if (ReadIniString(ini1, name1, ini2, name2, "lefttop_pic", param, sizeof(param), ""))
        m_nLeftTopPicture = pPictureService->GetImageNum(m_sGroupName.c_str(), param);
    if (m_nLeftTopPicture < 0)
    {
        m_frLeftTopUV.left = m_frLeftTopUV.top = 0.f;
        m_frLeftTopUV.right = m_frLeftTopUV.bottom = 1.f;
    }
    else
        pPictureService->GetTexturePos(m_nLeftTopPicture, m_frLeftTopUV);
    m_pntLeftTopSize = GetIniLongPoint(ini1, name1, ini2, name2, "lefttop_size", XYPOINT(9, 9));

    // right top corner
    m_nRightTopPicture = -1;
    if (ReadIniString(ini1, name1, ini2, name2, "righttop_pic", param, sizeof(param), ""))
        m_nRightTopPicture = pPictureService->GetImageNum(m_sGroupName.c_str(), param);
    if (m_nRightTopPicture < 0)
    {
        m_frRightTopUV.left = m_frRightTopUV.top = 0.f;
        m_frRightTopUV.right = m_frRightTopUV.bottom = 1.f;
    }
    else
        pPictureService->GetTexturePos(m_nRightTopPicture, m_frRightTopUV);
    m_pntRightTopSize = GetIniLongPoint(ini1, name1, ini2, name2, "righttop_size", XYPOINT(9, 9));

    // left bottom corner
    m_nLeftBottomPicture = -1;
    if (ReadIniString(ini1, name1, ini2, name2, "leftbottom_pic", param, sizeof(param), ""))
        m_nLeftBottomPicture = pPictureService->GetImageNum(m_sGroupName.c_str(), param);
    if (m_nLeftBottomPicture < 0)
    {
        m_frLeftBottomUV.left = m_frLeftBottomUV.top = 0.f;
        m_frLeftBottomUV.right = m_frLeftBottomUV.bottom = 1.f;
    }
    else
        pPictureService->GetTexturePos(m_nLeftBottomPicture, m_frLeftBottomUV);
    m_pntLeftBottomSize = GetIniLongPoint(ini1, name1, ini2, name2, "leftbottom_size", XYPOINT(9, 9));

    // left bottom corner
    m_nRightBottomPicture = -1;
    if (ReadIniString(ini1, name1, ini2, name2, "rightbottom_pic", param, sizeof(param), ""))
        m_nRightBottomPicture = pPictureService->GetImageNum(m_sGroupName.c_str(), param);
    if (m_nRightBottomPicture < 0)
    {
        m_frRightBottomUV.left = m_frRightBottomUV.top = 0.f;
        m_frRightBottomUV.right = m_frRightBottomUV.bottom = 1.f;
    }
    else
        pPictureService->GetTexturePos(m_nRightBottomPicture, m_frRightBottomUV);
    m_pntRightBottomSize = GetIniLongPoint(ini1, name1, ini2, name2, "rightbottom_size", XYPOINT(9, 9));

    // top edge
    m_nTopLinePicture = -1;
    if (ReadIniString(ini1, name1, ini2, name2, "topline_pic", param, sizeof(param), ""))
        m_nTopLinePicture = pPictureService->GetImageNum(m_sGroupName.c_str(), param);
    if (m_nTopLinePicture < 0)
    {
        m_frTopLineUV.left = m_frTopLineUV.top = 0.f;
        m_frTopLineUV.right = m_frTopLineUV.bottom = 1.f;
    }
    else
        pPictureService->GetTexturePos(m_nTopLinePicture, m_frTopLineUV);
    m_nTopLineHeight = GetIniLong(ini1, name1, ini2, name2, "topline_height", 5);

    // bottom edge
    m_nBottomLinePicture = -1;
    if (ReadIniString(ini1, name1, ini2, name2, "bottomline_pic", param, sizeof(param), ""))
        m_nBottomLinePicture = pPictureService->GetImageNum(m_sGroupName.c_str(), param);
    if (m_nBottomLinePicture < 0)
    {
        m_frBottomLineUV.left = m_frBottomLineUV.top = 0.f;
        m_frBottomLineUV.right = m_frBottomLineUV.bottom = 1.f;
    }
    else
        pPictureService->GetTexturePos(m_nBottomLinePicture, m_frBottomLineUV);
    m_nBottomLineHeight = GetIniLong(ini1, name1, ini2, name2, "bottomline_height", 5);

    // left edge
    m_nLeftLinePicture = -1;
    if (ReadIniString(ini1, name1, ini2, name2, "leftline_pic", param, sizeof(param), ""))
        m_nLeftLinePicture = pPictureService->GetImageNum(m_sGroupName.c_str(), param);
    if (m_nLeftLinePicture < 0)
    {
        m_frLeftLineUV.left = m_frLeftLineUV.top = 0.f;
        m_frLeftLineUV.right = m_frLeftLineUV.bottom = 1.f;
    }
    else
        pPictureService->GetTexturePos(m_nLeftLinePicture, m_frLeftLineUV);
    m_nLeftLineWidth = GetIniLong(ini1, name1, ini2, name2, "leftline_width", 5);

    // right edge
    m_nRightLinePicture = -1;
    if (ReadIniString(ini1, name1, ini2, name2, "rightline_pic", param, sizeof(param), ""))
        m_nRightLinePicture = pPictureService->GetImageNum(m_sGroupName.c_str(), param);
    if (m_nRightLinePicture < 0)
    {
        m_frRightLineUV.left = m_frRightLineUV.top = 0.f;
        m_frRightLineUV.right = m_frRightLineUV.bottom = 1.f;
    }
    else
        pPictureService->GetTexturePos(m_nRightLinePicture, m_frRightLineUV);
    m_nRightLineWidth = GetIniLong(ini1, name1, ini2, name2, "rightline_width", 5);

    if (m_pCaptionImage && m_nCaptionHeight > 0)
    {
        XYRECT rCapRect;
        ZERO(rCapRect);
        m_rCapRect = GetIniLongRect(ini1, name1, ini2, name2, "captionoffset", rCapRect);
        rCapRect = m_rCapRect;
        rCapRect.top += m_rect.top;
        rCapRect.bottom = rCapRect.top + m_nCaptionHeight - rCapRect.bottom;
        rCapRect.left += m_rect.left;
        rCapRect.right = m_rect.right - rCapRect.right;
        m_pCaptionImage->SetPosition(rCapRect);
    }

    // Fill buffers
    FillIndexBuffers();
    FillVertexBuffers();
}

void CXI_BORDER::FillIndexBuffers() const
{
    if (m_idIBuf < 0)
        return;
    auto *pI = static_cast<uint16_t *>(m_rs->LockIndexBuffer(m_idIBuf));

    for (int32_t n = 0; n < m_nSquareQ; n++)
    {
        pI[n * 6 + 0] = static_cast<uint16_t>(n * 4 + 0);
        pI[n * 6 + 1] = static_cast<uint16_t>(n * 4 + 1);
        pI[n * 6 + 2] = static_cast<uint16_t>(n * 4 + 2);

        pI[n * 6 + 3] = static_cast<uint16_t>(n * 4 + 1);
        pI[n * 6 + 4] = static_cast<uint16_t>(n * 4 + 3);
        pI[n * 6 + 5] = static_cast<uint16_t>(n * 4 + 2);
    }

    m_rs->UnLockIndexBuffer(m_idIBuf);
}

void CXI_BORDER::FillVertexBuffers()
{
    if (m_idVBuf < 0)
        return;
    auto *pV = static_cast<XI_ONETEX_VERTEX *>(m_rs->LockVertexBuffer(m_idVBuf));

    for (int32_t n = 0; n < m_nSquareQ * 4; n++)
    {
        pV[n].color = m_dwColor;
        pV[n].pos.z = 1.f;
    }

    // top line
    WriteVertexForSquare(&pV[0], m_frTopLineUV, m_dwColor, m_rect.left + m_pntLeftTopSize.x, m_rect.top,
                         m_rect.right - m_pntRightTopSize.x, m_rect.top + m_nTopLineHeight);
    // bottom line
    WriteVertexForSquare(&pV[4], m_frBottomLineUV, m_dwColor, m_rect.left + m_pntLeftTopSize.x,
                         m_rect.bottom - m_nBottomLineHeight, m_rect.right - m_pntRightTopSize.x, m_rect.bottom);
    // left line
    WriteVertexForSquare(&pV[8], m_frLeftLineUV, m_dwColor, m_rect.left, m_rect.top + m_pntLeftTopSize.y,
                         m_rect.left + m_nLeftLineWidth, m_rect.bottom - m_pntLeftBottomSize.y);
    // right line
    WriteVertexForSquare(&pV[12], m_frRightLineUV, m_dwColor, m_rect.right - m_nRightLineWidth,
                         m_rect.top + m_pntRightTopSize.y, m_rect.right, m_rect.bottom - m_pntRightBottomSize.y);
    // left top corner
    WriteVertexForSquare(&pV[16], m_frLeftTopUV, m_dwColor, m_rect.left, m_rect.top, m_rect.left + m_pntLeftTopSize.x,
                         m_rect.top + m_pntLeftTopSize.y);
    // right top corner
    WriteVertexForSquare(&pV[20], m_frRightTopUV, m_dwColor, m_rect.right - m_pntRightTopSize.x, m_rect.top,
                         m_rect.right, m_rect.top + m_pntRightTopSize.y);
    // left bottom corner
    WriteVertexForSquare(&pV[24], m_frLeftBottomUV, m_dwColor, m_rect.left, m_rect.bottom - m_pntLeftBottomSize.y,
                         m_rect.left + m_pntLeftBottomSize.x, m_rect.bottom);
    // right bottom corner
    WriteVertexForSquare(&pV[28], m_frRightBottomUV, m_dwColor, m_rect.right - m_pntRightBottomSize.x,
                         m_rect.bottom - m_pntRightBottomSize.y, m_rect.right, m_rect.bottom);

    // caption
    if (m_nCaptionHeight > 0 && m_pCaptionImage)
    {
        // caption line
        WriteVertexForSquare(&pV[32], m_frTopLineUV, m_dwColor, m_rect.left + m_nLeftLineWidth,
                             m_rect.top + m_nCaptionHeight, m_rect.right - m_nRightLineWidth,
                             m_rect.top + m_nCaptionHeight + m_mCaptionDividerHeight);
    }

    m_rs->UnLockVertexBuffer(m_idVBuf);
}

void CXI_BORDER::WriteVertexForSquare(XI_ONETEX_VERTEX *pV, FXYRECT &UVRect, uint32_t dwColor, int32_t left, int32_t top,
                                      int32_t right, int32_t bottom)
{
    pV[0].color = dwColor;
    pV[0].pos.x = static_cast<float>(left);
    pV[0].pos.y = static_cast<float>(top);
    pV[0].pos.z = 1.f;
    pV[0].tu = UVRect.left;
    pV[0].tv = UVRect.top;

    pV[1].color = dwColor;
    pV[1].pos.x = static_cast<float>(left);
    pV[1].pos.y = static_cast<float>(bottom);
    pV[1].pos.z = 1.f;
    pV[1].tu = UVRect.left;
    pV[1].tv = UVRect.bottom;

    pV[2].color = dwColor;
    pV[2].pos.x = static_cast<float>(right);
    pV[2].pos.y = static_cast<float>(top);
    pV[2].pos.z = 1.f;
    pV[2].tu = UVRect.right;
    pV[2].tv = UVRect.top;

    pV[3].color = dwColor;
    pV[3].pos.x = static_cast<float>(right);
    pV[3].pos.y = static_cast<float>(bottom);
    pV[3].pos.z = 1.f;
    pV[3].tu = UVRect.right;
    pV[3].tv = UVRect.bottom;
}

uint32_t CXI_BORDER::MessageProc(int32_t msgcode, MESSAGE &message)
{
    switch (msgcode)
    {
    case 0: // change the position of the button
        m_dwColor = message.Long();
        FillVertexBuffers();
        break;
    }
    return 0;
}
