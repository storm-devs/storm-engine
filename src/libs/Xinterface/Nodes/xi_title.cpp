#include "xi_title.h"

#include "primitive_renderer.h"


CXI_TITLE::CXI_TITLE()
{
    m_sGroupName = nullptr;
    m_idTex = -1L;

    m_idString = -1L;

    m_nVert = 0;

    pVert = nullptr;

    m_fontID = -1L;
    m_nNodeType = NODETYPE_TITLE;
}

CXI_TITLE::~CXI_TITLE()
{
    ReleaseAll();
}

void CXI_TITLE::Draw(bool bSelected, uint32_t Delta_Time)
{
    if (m_bUse)
    {
        /*m_rs->TextureSet(0, m_idTex);
        m_rs->DrawBuffer(m_idVBuf, sizeof(XI_ONETEX_VERTEX), m_idIBuf, 0, m_nVert, 0, m_nIndx, "iTitle");*/

        auto texture = m_rs->GetBGFXTextureFromID(m_idTex);
        m_rs->GetPrimitiveRenderer()->Texture = texture;

        for (long n = 0; n < m_nVert; n += 4)
        {
            auto pVertices = pVert;
            std::vector<VERTEX_POSITION_TEXTURE_COLOR> vertices;

            vertices.push_back(VERTEX_POSITION_TEXTURE_COLOR{pVertices[n + 0].pos.x, pVertices[n + 0].pos.y,
                                                             pVertices[n + 0].pos.z, pVertices[n + 0].tu,
                                                             pVertices[n + 0].tv, pVertices[n + 0].color});
            vertices.push_back(VERTEX_POSITION_TEXTURE_COLOR{pVertices[n + 2].pos.x, pVertices[n + 2].pos.y,
                                                             pVertices[n + 2].pos.z, pVertices[n + 2].tu,
                                                             pVertices[n + 2].tv, pVertices[n + 2].color});
            vertices.push_back(VERTEX_POSITION_TEXTURE_COLOR{pVertices[n + 1].pos.x, pVertices[n + 1].pos.y,
                                                             pVertices[n + 1].pos.z, pVertices[n + 1].tu,
                                                             pVertices[n + 1].tv, pVertices[n + 1].color});
            vertices.push_back(VERTEX_POSITION_TEXTURE_COLOR{pVertices[n + 3].pos.x, pVertices[n + 3].pos.y,
                                                             pVertices[n + 3].pos.z, pVertices[n + 3].tu,
                                                             pVertices[n + 3].tv, pVertices[n + 3].color});
            m_rs->GetPrimitiveRenderer()->PushVertices(vertices);
        }

        // show title text
        if (m_idString != -1L)
            m_rs->ExtPrint(m_fontID, m_fontColor, m_backColor, PR_ALIGN_CENTER, true, m_fontScale, m_screenSize.x,
                           m_screenSize.y, m_StringCenter.x, m_StringCenter.y, "%s",
                           pStringService->GetString(m_idString));
    }
}

bool CXI_TITLE::Init(INIFILE *ini1, const char *name1, INIFILE *ini2, const char *name2, VDX9RENDER *rs,
                     XYRECT &hostRect, XYPOINT &ScreenSize)
{
    if (!CINODE::Init(ini1, name1, ini2, name2, rs, hostRect, ScreenSize))
        return false;
    SetGlowCursor(false);
    return true;
}

void CXI_TITLE::ReleaseAll()
{
    BGFX_PICTURE_TEXTURE_RELEASE(pPictureService, m_sGroupName, m_idTex);
    STORM_DELETE(m_sGroupName);
    m_idString = -1L;

    m_nVert = 0;

    delete[] pVert;

    //FONT_RELEASE(m_rs, m_fontID);

    if (m_rs != NULL && m_fontID != -1)
    {
        m_rs->UnloadFont(m_fontID);
        m_fontID = -1;
    }
}

int CXI_TITLE::CommandExecute(int wActCode)
{
    return -1;
}

bool CXI_TITLE::IsClick(int buttonID, long xPos, long yPos)
{
    return false;
}

void CXI_TITLE::ChangePosition(XYRECT &rNewPos)
{
    m_rect = rNewPos;
    FillVertexBuffer();
}

void CXI_TITLE::SaveParametersToIni()
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

void CXI_TITLE::LoadIni(INIFILE *ini1, const char *name1, INIFILE *ini2, const char *name2)
{
    int i;
    char param[256];

    // Get image color
    const auto imgColor = GetIniARGB(ini1, name1, ini2, name2, "imageColor", 0xFFFFFFFF);

    // Get font color
    m_fontColor = GetIniARGB(ini1, name1, ini2, name2, "fontColor", 0xFFFFFFFF);

    // get back font color
    m_backColor = GetIniARGB(ini1, name1, ini2, name2, "backFontColor", 0xFFFFFFFF);

    // get font number
    if (ReadIniString(ini1, name1, ini2, name2, "font", param, sizeof(param), ""))
        if ((m_fontID = m_rs->LoadFont(param)) == -1)
            core.Trace("can not load font:'%s'", param);

    // get font scale
    m_fontScale = GetIniFloat(ini1, name1, ini2, name2, "fontScale", 1.f);

    // get string offset
    m_StringCenter.x = (m_rect.left + m_rect.right) / 2;
    m_StringCenter.y = m_rect.top + GetIniLong(ini1, name1, ini2, name2, "stringOffset", 0);

    // get title string
    auto *const pChar = core.Entity_GetAttribute(g_idInterface, "title");
    if (pChar != nullptr && pChar[0] != '#')
        m_idString = pStringService->GetStringNum(pChar);
    else
        m_idString = -1;

    m_nStringWidth = GetIniLong(ini1, name1, ini2, name2, "stringWidth", 0);
    if (m_nStringWidth == 0)
    {
        if (pChar != nullptr && pChar[0] == '#')
            m_nStringWidth = m_rs->StringWidth(&pChar[1], m_fontID, m_fontScale, m_screenSize.x);
        else
            m_nStringWidth =
                m_rs->StringWidth(pStringService->GetString(m_idString), m_fontID, m_fontScale, m_screenSize.x);
    }

    // get title image group name
    if (ReadIniString(ini1, name1, ini2, name2, "imgGroupName", param, sizeof(param), ""))
    {
        const auto len = strlen(param) + 1;
        m_sGroupName = new char[len];
        if (m_sGroupName == nullptr)
            throw std::runtime_error("allocate memory error");
        memcpy(m_sGroupName, param, len);
        m_idTex = pPictureService->BGFXGetTextureID(m_sGroupName);
    }
    else
    {
        m_sGroupName = nullptr;
        m_idTex = -1;
    }

    FXYRECT centerRect, tiledRect, mediumRect;
    // get title images
    if (ReadIniString(ini1, name1, ini2, name2, "titleCenter", param, sizeof(param), ""))
        pPictureService->BGFXGetTexturePos(m_sGroupName, param, centerRect);
    else
        PZERO(&centerRect, sizeof(centerRect));
    if (ReadIniString(ini1, name1, ini2, name2, "titleMedium", param, sizeof(param), ""))
    {
        pPictureService->BGFXGetTexturePos(m_sGroupName, param, mediumRect);
        pPictureService->BGFXGetTexturePos(m_sGroupName, param, m_mRect);
    }
    else
    {
        PZERO(&mediumRect, sizeof(mediumRect));
        PZERO(&m_mRect, sizeof(m_mRect));
    }
    if (ReadIniString(ini1, name1, ini2, name2, "titleTiled", param, sizeof(param), ""))
    {
        pPictureService->BGFXGetTexturePos(m_sGroupName, param, tiledRect);
        pPictureService->BGFXGetTexturePos(m_sGroupName, param, m_tRect);
    }
    else
    {
        PZERO(&tiledRect, sizeof(tiledRect));
        PZERO(&m_tRect, sizeof(m_tRect));
    }

    // create vertex buffer and index buffer for title image
    m_nTiledQuantity = 0;
    if (m_tRect.right != m_tRect.left)
        m_nTiledQuantity = ((m_StringCenter.x - m_rect.left) - m_nStringWidth / 2 - (m_mRect.right - m_mRect.left)) /
                               (m_tRect.right - m_tRect.left) +
                           1;
    if (m_nTiledQuantity < 0)
        m_nTiledQuantity = 0;
    const int rectangleQuantity = 1 + 2 + 2 * m_nTiledQuantity;
    m_nVert = 4 * rectangleQuantity;

    pVert = new XI_ONETEX_VERTEX[m_nVert];

    for (i = 0; i < m_nVert; i++)
    {
        pVert[i].color = imgColor;
        pVert[i].pos.z = 1.f;
    }
    // fill center rectangle (were string is showing)
    pVert[0].pos.x = static_cast<float>(m_StringCenter.x - m_nStringWidth / 2);
    pVert[0].pos.y = static_cast<float>(m_rect.top);
    pVert[1].pos.x = static_cast<float>(m_StringCenter.x + m_nStringWidth / 2);
    pVert[1].pos.y = static_cast<float>(m_rect.top);
    pVert[2].pos.x = static_cast<float>(m_StringCenter.x - m_nStringWidth / 2);
    pVert[2].pos.y = static_cast<float>(m_rect.bottom);
    pVert[3].pos.x = static_cast<float>(m_StringCenter.x + m_nStringWidth / 2);
    pVert[3].pos.y = static_cast<float>(m_rect.bottom);
    pVert[0].tu = pVert[2].tu = centerRect.left;
    pVert[1].tu = pVert[3].tu = centerRect.right;
    pVert[0].tv = pVert[1].tv = centerRect.top;
    pVert[2].tv = pVert[3].tv = centerRect.bottom;
    // fill two medium rectangles
    const int tmp1 = m_nStringWidth / 2;
    const int tmp2 = m_nStringWidth / 2 + (m_mRect.right - m_mRect.left);
    pVert[4].pos.y = pVert[5].pos.y = pVert[8].pos.y = pVert[9].pos.y = static_cast<float>(m_rect.top);
    pVert[6].pos.y = pVert[7].pos.y = pVert[10].pos.y = pVert[11].pos.y = static_cast<float>(m_rect.bottom);
    pVert[4].pos.x = pVert[6].pos.x = static_cast<float>(m_StringCenter.x - tmp2);
    pVert[5].pos.x = pVert[7].pos.x = static_cast<float>(m_StringCenter.x - tmp1);
    pVert[8].pos.x = pVert[10].pos.x = static_cast<float>(m_StringCenter.x + tmp2);
    pVert[9].pos.x = pVert[11].pos.x = static_cast<float>(m_StringCenter.x + tmp1);
    pVert[4].tu = pVert[6].tu = pVert[8].tu = pVert[10].tu = mediumRect.left;
    pVert[5].tu = pVert[7].tu = pVert[9].tu = pVert[11].tu = mediumRect.right;
    pVert[4].tv = pVert[5].tv = pVert[8].tv = pVert[9].tv = mediumRect.top;
    pVert[6].tv = pVert[7].tv = pVert[10].tv = pVert[11].tv = mediumRect.bottom;
    int idx = 12;
    // fill left tiled rectangles
    auto xpos = static_cast<float>(m_StringCenter.x - tmp2);
    float xposDelta = (xpos - static_cast<float>(m_rect.left)) / m_nTiledQuantity;
    for (i = 0; i < m_nTiledQuantity; i++)
    {
        pVert[idx + 0].pos.x = pVert[idx + 2].pos.x = xpos - xposDelta;
        pVert[idx + 1].pos.x = pVert[idx + 3].pos.x = xpos;
        pVert[idx + 0].pos.y = pVert[idx + 1].pos.y = static_cast<float>(m_rect.top);
        pVert[idx + 2].pos.y = pVert[idx + 3].pos.y = static_cast<float>(m_rect.bottom);
        pVert[idx + 0].tu = pVert[idx + 2].tu = tiledRect.left;
        pVert[idx + 1].tu = pVert[idx + 3].tu = tiledRect.right;
        pVert[idx + 0].tv = pVert[idx + 1].tv = tiledRect.top;
        pVert[idx + 2].tv = pVert[idx + 3].tv = tiledRect.bottom;
        idx += 4;
        xpos -= xposDelta;
    }
    // fill right tiled rectangles
    xpos = static_cast<float>(m_StringCenter.x + tmp2);
    xposDelta = (static_cast<float>(m_rect.right) - xpos) / m_nTiledQuantity;
    for (i = 0; i < m_nTiledQuantity; i++)
    {
        pVert[idx + 0].pos.x = pVert[idx + 2].pos.x = xpos + xposDelta;
        pVert[idx + 1].pos.x = pVert[idx + 3].pos.x = xpos;
        pVert[idx + 0].pos.y = pVert[idx + 1].pos.y = static_cast<float>(m_rect.top);
        pVert[idx + 2].pos.y = pVert[idx + 3].pos.y = static_cast<float>(m_rect.bottom);
        pVert[idx + 0].tu = pVert[idx + 2].tu = tiledRect.left;
        pVert[idx + 1].tu = pVert[idx + 3].tu = tiledRect.right;
        pVert[idx + 0].tv = pVert[idx + 1].tv = tiledRect.top;
        pVert[idx + 2].tv = pVert[idx + 3].tv = tiledRect.bottom;
        idx += 4;
        xpos += xposDelta;
    }
}

void CXI_TITLE::FillVertexBuffer()
{
    long i;
    if (pVert != nullptr)
    {
        // fill center rectangle (were string is showing)
        pVert[0].pos.x = static_cast<float>(m_StringCenter.x - m_nStringWidth / 2);
        pVert[0].pos.y = static_cast<float>(m_rect.top);
        pVert[1].pos.x = static_cast<float>(m_StringCenter.x + m_nStringWidth / 2);
        pVert[1].pos.y = static_cast<float>(m_rect.top);
        pVert[2].pos.x = static_cast<float>(m_StringCenter.x - m_nStringWidth / 2);
        pVert[2].pos.y = static_cast<float>(m_rect.bottom);
        pVert[3].pos.x = static_cast<float>(m_StringCenter.x + m_nStringWidth / 2);
        pVert[3].pos.y = static_cast<float>(m_rect.bottom);

        // fill two medium rectangles
        const int tmp1 = m_nStringWidth / 2;
        const int tmp2 = m_nStringWidth / 2 + (m_mRect.right - m_mRect.left);
        pVert[4].pos.y = pVert[5].pos.y = pVert[8].pos.y = pVert[9].pos.y = static_cast<float>(m_rect.top);
        pVert[6].pos.y = pVert[7].pos.y = pVert[10].pos.y = pVert[11].pos.y = static_cast<float>(m_rect.bottom);
        pVert[4].pos.x = pVert[6].pos.x = static_cast<float>(m_StringCenter.x - tmp2);
        pVert[5].pos.x = pVert[7].pos.x = static_cast<float>(m_StringCenter.x - tmp1);
        pVert[8].pos.x = pVert[10].pos.x = static_cast<float>(m_StringCenter.x + tmp2);
        pVert[9].pos.x = pVert[11].pos.x = static_cast<float>(m_StringCenter.x + tmp1);

        int idx = 12;
        // fill left tiled rectangles
        auto xpos = static_cast<float>(m_StringCenter.x - tmp2);
        float xposDelta = (xpos - static_cast<float>(m_rect.left)) / m_nTiledQuantity;
        for (i = 0; i < m_nTiledQuantity; i++)
        {
            pVert[idx + 0].pos.x = pVert[idx + 2].pos.x = xpos - xposDelta;
            pVert[idx + 1].pos.x = pVert[idx + 3].pos.x = xpos;
            pVert[idx + 0].pos.y = pVert[idx + 1].pos.y = static_cast<float>(m_rect.top);
            pVert[idx + 2].pos.y = pVert[idx + 3].pos.y = static_cast<float>(m_rect.bottom);
            idx += 4;
            xpos -= xposDelta;
        }

        // fill right tiled rectangles
        xpos = static_cast<float>(m_StringCenter.x + tmp2);
        xposDelta = (static_cast<float>(m_rect.right) - xpos) / m_nTiledQuantity;
        for (i = 0; i < m_nTiledQuantity; i++)
        {
            pVert[idx + 0].pos.x = pVert[idx + 2].pos.x = xpos + xposDelta;
            pVert[idx + 1].pos.x = pVert[idx + 3].pos.x = xpos;
            pVert[idx + 0].pos.y = pVert[idx + 1].pos.y = static_cast<float>(m_rect.top);
            pVert[idx + 2].pos.y = pVert[idx + 3].pos.y = static_cast<float>(m_rect.bottom);
            idx += 4;
            xpos += xposDelta;
        }
    }
}
