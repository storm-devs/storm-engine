#include "xi_status_line.h"
#include <stdio.h>

#include "core.h"

#include "primitive_renderer.h"

CXI_STATUSLINE::CXI_STATUSLINE()
    : m_fLineOffset(0)
{
    m_rs = nullptr;
    m_sGroupName = nullptr;
    m_idTex = -1L;
    m_nVert = 0;
    m_nNodeType = NODETYPE_STATUSLINE;
}

CXI_STATUSLINE::~CXI_STATUSLINE()
{
    ReleaseAll();
}

void CXI_STATUSLINE::Draw(bool bSelected, uint32_t Delta_Time)
{
    if (m_bUse)
    {
        //m_rs->TextureSet(0, m_idTex);
        //m_rs->DrawBuffer(m_vBuf, sizeof(XI_ONLYONETEX_VERTEX), m_iBuf, 0, m_nVert, 0, m_nIndx, "iStatusLine");
        auto texture = m_rs->GetBGFXTextureFromID(m_idTex);
        m_rs->GetPrimitiveRenderer()->Texture = texture;

        for (int i = 0; i < m_nVert; i += 4)
        {
            std::vector<VERTEX_POSITION_TEXTURE> vertices;

            auto pV = m_vertices;

            vertices.push_back(
                VERTEX_POSITION_TEXTURE{pV[i + 0].pos.x, pV[i + 0].pos.y, pV[i + 0].pos.z, pV[i + 0].tu, pV[i + 0].tv});
            vertices.push_back(
                VERTEX_POSITION_TEXTURE{pV[i + 2].pos.x, pV[i + 2].pos.y, pV[i + 2].pos.z, pV[i + 2].tu, pV[i + 2].tv});
            vertices.push_back(
                VERTEX_POSITION_TEXTURE{pV[i + 1].pos.x, pV[i + 1].pos.y, pV[i + 1].pos.z, pV[i + 1].tu, pV[i + 1].tv});
            vertices.push_back(
                VERTEX_POSITION_TEXTURE{pV[i + 3].pos.x, pV[i + 3].pos.y, pV[i + 3].pos.z, pV[i + 3].tu, pV[i + 3].tv});
            m_rs->GetPrimitiveRenderer()->PushVertices(vertices);
        }
    }
}

bool CXI_STATUSLINE::Init(INIFILE *ini1, const char *name1, INIFILE *ini2, const char *name2, VDX9RENDER *rs,
                          XYRECT &hostRect, XYPOINT &ScreenSize)
{
    if (!CINODE::Init(ini1, name1, ini2, name2, rs, hostRect, ScreenSize))
        return false;
    SetGlowCursor(false);
    return true;
}

void CXI_STATUSLINE::ReleaseAll()
{
    BGFX_PICTURE_TEXTURE_RELEASE(pPictureService, m_sGroupName, m_idTex);
    STORM_DELETE(m_sGroupName);
}

int CXI_STATUSLINE::CommandExecute(int wActCode)
{
    return -1;
}

void CXI_STATUSLINE::LoadIni(INIFILE *ini1, const char *name1, INIFILE *ini2, const char *name2)
{
    char param[256];

    // Get texture name and load that texture
    if (ReadIniString(ini1, name1, ini2, name2, "groupName", param, sizeof(param), ""))
    {
        const auto len = strlen(param) + 1;
        m_sGroupName = new char[len];
        memcpy(m_sGroupName, param, len);
        m_idTex = pPictureService->BGFXGetTextureID(m_sGroupName);
    }

    // Calculate vertex and index quantity
    m_nVert = 2 * 4;
    // Create vertex and index buffers
    m_vertices.reserve(m_nVert);
    m_vertices.resize(m_nVert);

    // Lock vertex and index buffers and get pointers to this
    auto *const pVBuf = m_vertices.data();

    if (pVBuf != nullptr)
    {
        FXYRECT scrRect1, scrRect2;

        // get lenght of filled status line
        m_fLineOffset = GetIniFloat(ini1, name1, ini2, name2, "lineOffset", 0.f);
        auto fMediumX = static_cast<float>(m_rect.right - m_rect.left) - m_fLineOffset * 2.f;
        auto *pAttr = core.Entity_GetAttributeClass(g_idInterface, "StatusLine");
        if (pAttr != nullptr)
            pAttr = pAttr->GetAttributeClass(m_nodeName);
        if (pAttr != nullptr)
        {
            const auto fMaxValue = pAttr->GetAttributeAsFloat("Max", 0);
            const auto fMinValue = pAttr->GetAttributeAsFloat("Min", 0);
            const auto fCurValue = pAttr->GetAttributeAsFloat("Value", 0);
            if (fMaxValue - fMinValue > 0 && fCurValue >= fMinValue)
                fMediumX *= (fCurValue - fMinValue) / (fMaxValue - fMinValue);
        }
        fMediumX += m_fLineOffset;

        // get screen coordinates
        scrRect1.left = static_cast<float>(m_rect.left);
        scrRect2.left = static_cast<float>(m_rect.left) + fMediumX;
        scrRect1.top = scrRect2.top = static_cast<float>(m_rect.top);
        scrRect1.right = static_cast<float>(m_rect.left) + fMediumX;
        scrRect2.right = static_cast<float>(m_rect.right);
        scrRect1.bottom = scrRect2.bottom = static_cast<float>(m_rect.bottom);

        // get texture coordinates
        fMediumX /= (m_rect.right - m_rect.left);
        if (ReadIniString(ini1, name1, ini2, name2, "filledPicture", param, sizeof(param), ""))
            pPictureService->BGFXGetTexturePos(m_sGroupName, param, m_texRect1);
        FXYRECT texRect1;
        memcpy(&texRect1, &m_texRect1, sizeof(FRECT));
        texRect1.right = m_texRect1.left + (m_texRect1.right - m_texRect1.left) * fMediumX;
        // .. other ..
        if (ReadIniString(ini1, name1, ini2, name2, "emptyPicture", param, sizeof(param), ""))
            pPictureService->BGFXGetTexturePos(m_sGroupName, param, m_texRect2);
        FXYRECT texRect2;
        memcpy(&texRect2, &m_texRect2, sizeof(FRECT));
        texRect2.left = m_texRect2.left + (m_texRect2.right - m_texRect2.left) * fMediumX;

        // fill vertex buffer
        for (auto i = 0; i < m_nVert; i++)
            pVBuf[i].pos.z = 1.f;
        // screen and texture coordinates for filled rectangle
        pVBuf[0].pos.x = scrRect1.left;
        pVBuf[0].pos.y = scrRect1.top;
        pVBuf[0].tu = texRect1.left;
        pVBuf[0].tv = texRect1.top;
        pVBuf[1].pos.x = scrRect1.right;
        pVBuf[1].pos.y = scrRect1.top;
        pVBuf[1].tu = texRect1.right;
        pVBuf[1].tv = texRect1.top;
        pVBuf[2].pos.x = scrRect1.left;
        pVBuf[2].pos.y = scrRect1.bottom;
        pVBuf[2].tu = texRect1.left;
        pVBuf[2].tv = texRect1.bottom;
        pVBuf[3].pos.x = scrRect1.right;
        pVBuf[3].pos.y = scrRect1.bottom;
        pVBuf[3].tu = texRect1.right;
        pVBuf[3].tv = texRect1.bottom;
        // screen and texture coordinates for empty rectangle
        pVBuf[4].pos.x = scrRect2.left;
        pVBuf[4].pos.y = scrRect2.top;
        pVBuf[4].tu = texRect2.left;
        pVBuf[4].tv = texRect2.top;
        pVBuf[5].pos.x = scrRect2.right;
        pVBuf[5].pos.y = scrRect2.top;
        pVBuf[5].tu = texRect2.right;
        pVBuf[5].tv = texRect2.top;
        pVBuf[6].pos.x = scrRect2.left;
        pVBuf[6].pos.y = scrRect2.bottom;
        pVBuf[6].tu = texRect2.left;
        pVBuf[6].tv = texRect2.bottom;
        pVBuf[7].pos.x = scrRect2.right;
        pVBuf[7].pos.y = scrRect2.bottom;
        pVBuf[7].tu = texRect2.right;
        pVBuf[7].tv = texRect2.bottom;
    }
    else
        throw std::runtime_error("Can't vertex or index buffer create");
}

bool CXI_STATUSLINE::IsClick(int buttonID, int32_t xPos, int32_t yPos)
{
    return false;
}

void CXI_STATUSLINE::ChangePosition(XYRECT &rNewPos)
{
    m_rect = rNewPos;
    Refresh();
}

void CXI_STATUSLINE::SaveParametersToIni()
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

uint32_t CXI_STATUSLINE::MessageProc(int32_t msgcode, MESSAGE &message)
{
    switch (msgcode)
    {
    case 0:
        Refresh();
        break;
    }
    return 0;
}

void CXI_STATUSLINE::Refresh()
{
    if (m_vertices.size() == 0)
        return;
    
    auto *pVBuf = m_vertices.data();
    auto *pAttr = core.Entity_GetAttributeClass(g_idInterface, "StatusLine");
    if (pAttr != nullptr)
        pAttr = pAttr->GetAttributeClass(m_nodeName);
    if (pAttr != nullptr)
    {
        auto fMediumX = static_cast<float>(m_rect.right - m_rect.left) - m_fLineOffset * 2.f;
        const auto fMaxValue = pAttr->GetAttributeAsFloat("Max", 0);
        const auto fMinValue = pAttr->GetAttributeAsFloat("Min", 0);
        const float fCurValue = pAttr->GetAttributeAsFloat("Value", 0);
        if (fMaxValue > fMinValue && fCurValue >= fMinValue && fCurValue <= fMaxValue)
            fMediumX *= (fCurValue - fMinValue) / (fMaxValue - fMinValue);

        FXYRECT scrRect1, scrRect2;
        // get screen coordinates
        scrRect1.left = static_cast<float>(m_rect.left);
        scrRect2.left = static_cast<float>(m_rect.left) + fMediumX;
        scrRect1.top = scrRect2.top = static_cast<float>(m_rect.top);
        scrRect1.right = static_cast<float>(m_rect.left) + fMediumX;
        scrRect2.right = static_cast<float>(m_rect.right);
        scrRect1.bottom = scrRect2.bottom = static_cast<float>(m_rect.bottom);

        // get texture coordinates
        fMediumX /= (m_rect.right - m_rect.left);

        FRECT texRect1, texRect2;
        memcpy(&texRect1, &m_texRect1, sizeof(FRECT));
        texRect1.right = m_texRect1.left + (m_texRect1.right - m_texRect1.left) * fMediumX;
        memcpy(&texRect2, &m_texRect2, sizeof(FRECT));
        texRect2.left = m_texRect2.left + (m_texRect2.right - m_texRect2.left) * fMediumX;

        // screen and texture coordinates for filled rectangle
        pVBuf[0].pos.x = scrRect1.left;
        pVBuf[0].pos.y = scrRect1.top;
        pVBuf[0].tu = texRect1.left;
        pVBuf[0].tv = texRect1.top;
        pVBuf[1].pos.x = scrRect1.right;
        pVBuf[1].pos.y = scrRect1.top;
        pVBuf[1].tu = texRect1.right;
        pVBuf[1].tv = texRect1.top;
        pVBuf[2].pos.x = scrRect1.left;
        pVBuf[2].pos.y = scrRect1.bottom;
        pVBuf[2].tu = texRect1.left;
        pVBuf[2].tv = texRect1.bottom;
        pVBuf[3].pos.x = scrRect1.right;
        pVBuf[3].pos.y = scrRect1.bottom;
        pVBuf[3].tu = texRect1.right;
        pVBuf[3].tv = texRect1.bottom;
        // screen and texture coordinates for empty rectangle
        pVBuf[4].pos.x = scrRect2.left;
        pVBuf[4].pos.y = scrRect2.top;
        pVBuf[4].tu = texRect2.left;
        pVBuf[4].tv = texRect2.top;
        pVBuf[5].pos.x = scrRect2.right;
        pVBuf[5].pos.y = scrRect2.top;
        pVBuf[5].tu = texRect2.right;
        pVBuf[5].tv = texRect2.top;
        pVBuf[6].pos.x = scrRect2.left;
        pVBuf[6].pos.y = scrRect2.bottom;
        pVBuf[6].tu = texRect2.left;
        pVBuf[6].tv = texRect2.bottom;
        pVBuf[7].pos.x = scrRect2.right;
        pVBuf[7].pos.y = scrRect2.bottom;
        pVBuf[7].tu = texRect2.right;
        pVBuf[7].tv = texRect2.bottom;
    }
}
