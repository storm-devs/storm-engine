#include "xi_bounder.h"
#include <stdio.h>

#include "core.h"

#include "v_file_service.h"

CXI_BOUNDER::CXI_BOUNDER()
{
    m_rs = nullptr;

    m_idTex = -1L;
    m_sGroupName = nullptr;

    m_idVBuf = -1L;
    m_idIBuf = -1L;
    m_nVert = 0;
    m_nIndx = 0;
    m_nNodeType = NODETYPE_BOUNDER;
}

CXI_BOUNDER::~CXI_BOUNDER()
{
    ReleaseAll();
}

int CXI_BOUNDER::CommandExecute(int wActCode)
{
    return -1;
}

void CXI_BOUNDER::Draw(bool bSelected, uint32_t Delta_Time)
{
    if (m_bUse)
    {
        m_rs->TextureSet(0, m_idTex);
        m_rs->DrawBuffer(m_idVBuf, sizeof(XI_ONETEX_VERTEX), m_idIBuf, 0, m_nVert, 0, m_nIndx, "iBounder");
    }
}

bool CXI_BOUNDER::Init(INIFILE *ini1, const char *name1, INIFILE *ini2, const char *name2, VDX9RENDER *rs,
                       XYRECT &hostRect, XYPOINT &ScreenSize)
{
    if (!CINODE::Init(ini1, name1, ini2, name2, rs, hostRect, ScreenSize))
        return false;
    return true;
}

void CXI_BOUNDER::ReleaseAll()
{
    m_bUse = false;

    PICTURE_TEXTURE_RELEASE(pPictureService, m_sGroupName, m_idTex);
    STORM_DELETE(m_sGroupName);

    VERTEX_BUFFER_RELEASE(m_rs, m_idVBuf);
    INDEX_BUFFER_RELEASE(m_rs, m_idIBuf);
}

void CXI_BOUNDER::LoadIni(INIFILE *ini1, const char *name1, INIFILE *ini2, const char *name2)
{
    int i;
    char param[256];
    char param2[256];
    const char *tmpstr;

    // get show color
    m_dwColor = GetIniARGB(ini1, name1, ini2, name2, "color", 0xFFFFFFFF);

    // Get texture name and load that texture
    m_sGroupName = nullptr;
    if (ReadIniString(ini1, name1, ini2, name2, "groupName", param, sizeof(param), ""))
    {
        const auto len = strlen(param) + 1;
        m_sGroupName = new char[len];
        memcpy(m_sGroupName, param, len);
        m_idTex = pPictureService->GetTextureID(m_sGroupName);
    }

    // get pictures name
    m_idAngle = -1;
    m_idHorzLine = -1;
    if (ReadIniString(ini1, name1, ini2, name2, "pictures", param, sizeof(param), ""))
    {
        tmpstr = param;
        tmpstr = GetSubStr(tmpstr, param2, sizeof(param2) - 1);
        m_idAngle = pPictureService->GetImageNum(m_sGroupName, param2);
        m_idHorzLine = pPictureService->GetImageNum(m_sGroupName, tmpstr);
    }

    // get picture width & height
    XYRECT rectTmp;
    pPictureService->GetTexturePos(m_idAngle, rectTmp);
    m_fAngleWidth = static_cast<float>(rectTmp.right - rectTmp.left + 1);
    m_fAngleHeight = static_cast<float>(rectTmp.bottom - rectTmp.top + 1);
    pPictureService->GetTexturePos(m_idHorzLine, rectTmp);
    auto fLineWidth = static_cast<float>(rectTmp.right - rectTmp.left + 1);

    auto fBoxWidth = static_cast<float>(m_rect.right - m_rect.left) - m_fAngleWidth * 2.f;
    auto fBoxHeight = static_cast<float>(m_rect.bottom - m_rect.top) - m_fAngleHeight * 2.f;

    m_nHorzLineQuantity = static_cast<int>(fBoxWidth / fLineWidth);
    if (m_nHorzLineQuantity * fLineWidth < fBoxWidth)
        m_nHorzLineQuantity++;
    auto fHorzLineWidth = fBoxWidth / m_nHorzLineQuantity;

    m_nVertLineQuantity = static_cast<int>(fBoxHeight / fLineWidth);
    if (m_nVertLineQuantity * fLineWidth < fBoxHeight)
        m_nVertLineQuantity++;
    auto fVertLineWidth = fBoxHeight / m_nVertLineQuantity;

    // create index and vertex buffers
    m_nVert = 4 * 4 + m_nHorzLineQuantity * 4 * 2 + m_nVertLineQuantity * 4 * 2;
    m_nIndx = 2 * 4 + 2 * m_nHorzLineQuantity * 2 + 2 * m_nVertLineQuantity * 2;
    m_idVBuf = m_rs->CreateVertexBuffer(XI_ONETEX_FVF, m_nVert * sizeof(XI_ONETEX_VERTEX), D3DUSAGE_WRITEONLY);
    m_idIBuf = m_rs->CreateIndexBuffer(m_nIndx * 3 * 2);

    // Fill buffers
    auto *pVert = static_cast<XI_ONETEX_VERTEX *>(m_rs->LockVertexBuffer(m_idVBuf));
    auto *pIndx = static_cast<uint16_t *>(m_rs->LockIndexBuffer(m_idIBuf));
    if (pVert == nullptr || pIndx == nullptr)
        throw std::runtime_error("can not create the index&vertex buffers");

    for (i = 0; i < m_nVert; i++)
    {
        pVert[i].color = m_dwColor;
        pVert[i].pos.z = 1.f;
    }

    auto inum = 0;
    auto vnum = 0;
    // fill angle rectangles
    FXYRECT texRectTmp;
    FXYRECT fRectTmp;
    for (i = 0; i < 4; i++)
    {
        pIndx[inum++] = vnum;
        pIndx[inum++] = vnum + 1;
        pIndx[inum++] = vnum + 2;
        pIndx[inum++] = vnum + 2;
        pIndx[inum++] = vnum + 1;
        pIndx[inum++] = vnum + 3;

        switch (i)
        {
        case 0:
            fRectTmp.left = static_cast<float>(m_rect.left);
            fRectTmp.right = static_cast<float>(m_rect.left) + m_fAngleWidth;
            fRectTmp.top = static_cast<float>(m_rect.top);
            fRectTmp.bottom = static_cast<float>(m_rect.top) + m_fAngleHeight;
            pPictureService->GetTexturePos(m_idAngle, texRectTmp);
            break;
        case 1:
            fRectTmp.left = static_cast<float>(m_rect.right) - m_fAngleWidth;
            fRectTmp.right = static_cast<float>(m_rect.right);
            fRectTmp.top = static_cast<float>(m_rect.top);
            fRectTmp.bottom = static_cast<float>(m_rect.top) + m_fAngleHeight;
            pPictureService->GetTexturePos(TEXTURE_MODIFY_HORZFLIP, m_idAngle, texRectTmp);
            break;
        case 2:
            fRectTmp.left = static_cast<float>(m_rect.left);
            fRectTmp.right = static_cast<float>(m_rect.left) + m_fAngleWidth;
            fRectTmp.top = static_cast<float>(m_rect.bottom) - m_fAngleHeight;
            fRectTmp.bottom = static_cast<float>(m_rect.bottom);
            pPictureService->GetTexturePos(TEXTURE_MODIFY_VERTFLIP, m_idAngle, texRectTmp);
            break;
        case 3:
            fRectTmp.left = static_cast<float>(m_rect.right) - m_fAngleWidth;
            fRectTmp.right = static_cast<float>(m_rect.right);
            fRectTmp.top = static_cast<float>(m_rect.bottom) - m_fAngleHeight;
            fRectTmp.bottom = static_cast<float>(m_rect.bottom);
            pPictureService->GetTexturePos(TEXTURE_MODIFY_HORZFLIP | TEXTURE_MODIFY_VERTFLIP, m_idAngle, texRectTmp);
            break;
        }

        pVert[vnum].tu = texRectTmp.left;
        pVert[vnum].tv = texRectTmp.top;
        pVert[vnum + 1].tu = texRectTmp.right;
        pVert[vnum + 1].tv = texRectTmp.top;
        pVert[vnum + 2].tu = texRectTmp.left;
        pVert[vnum + 2].tv = texRectTmp.bottom;
        pVert[vnum + 3].tu = texRectTmp.right;
        pVert[vnum + 3].tv = texRectTmp.bottom;

        pVert[vnum].pos.x = fRectTmp.left;
        pVert[vnum].pos.y = fRectTmp.top;
        pVert[vnum + 1].pos.x = fRectTmp.right;
        pVert[vnum + 1].pos.y = fRectTmp.top;
        pVert[vnum + 2].pos.x = fRectTmp.left;
        pVert[vnum + 2].pos.y = fRectTmp.bottom;
        pVert[vnum + 3].pos.x = fRectTmp.right;
        pVert[vnum + 3].pos.y = fRectTmp.bottom;

        vnum += 4;
    }

    // fill horizontal & vertical line rectangles
    FXYRECT tmpRect1, tmpRect2;
    pPictureService->GetTexturePos(m_idHorzLine, tmpRect1);
    pPictureService->GetTexturePos(TEXTURE_MODIFY_VERTFLIP, m_idHorzLine, tmpRect2);
    auto fXTop = m_rect.left + m_fAngleWidth;
    auto fYTop = m_rect.bottom - m_fAngleHeight;
    auto lineType = 0; // top horizontal line
    for (i = 0; i < m_nHorzLineQuantity * 2 + m_nVertLineQuantity * 2; i++)
    {
        if (i == m_nHorzLineQuantity)
        {
            lineType = 1; // bottom horizontal line
            fXTop = m_rect.left + m_fAngleWidth;
        }
        if (i == m_nHorzLineQuantity * 2)
        {
            lineType = 2; // left vertical line
            fYTop = m_rect.bottom - m_fAngleHeight;
        }
        if (i == m_nHorzLineQuantity * 2 + m_nVertLineQuantity)
        {
            lineType = 3; // right vertical line
            fYTop = m_rect.bottom - m_fAngleHeight;
        }

        pIndx[inum++] = vnum;
        pIndx[inum++] = vnum + 1;
        pIndx[inum++] = vnum + 2;
        pIndx[inum++] = vnum + 2;
        pIndx[inum++] = vnum + 1;
        pIndx[inum++] = vnum + 3;

        //
        if (lineType == 0 || lineType == 2)
        {
            pVert[vnum].tu = tmpRect1.left;
            pVert[vnum].tv = tmpRect1.top;
            pVert[vnum + 1].tu = tmpRect1.right;
            pVert[vnum + 1].tv = tmpRect1.top;
            pVert[vnum + 2].tu = tmpRect1.left;
            pVert[vnum + 2].tv = tmpRect1.bottom;
            pVert[vnum + 3].tu = tmpRect1.right;
            pVert[vnum + 3].tv = tmpRect1.bottom;
        }
        else
        {
            pVert[vnum].tu = tmpRect2.left;
            pVert[vnum].tv = tmpRect2.top;
            pVert[vnum + 1].tu = tmpRect2.right;
            pVert[vnum + 1].tv = tmpRect2.top;
            pVert[vnum + 2].tu = tmpRect2.left;
            pVert[vnum + 2].tv = tmpRect2.bottom;
            pVert[vnum + 3].tu = tmpRect2.right;
            pVert[vnum + 3].tv = tmpRect2.bottom;
        }

        switch (lineType)
        {
        case 0:
            fRectTmp.left = fXTop;
            fRectTmp.right = fXTop + fHorzLineWidth;
            fRectTmp.top = static_cast<float>(m_rect.top);
            fRectTmp.bottom = static_cast<float>(m_rect.top) + m_fAngleHeight;
            break;
        case 1:
            fRectTmp.left = fXTop;
            fRectTmp.right = fXTop + fHorzLineWidth;
            fRectTmp.top = static_cast<float>(m_rect.bottom) - m_fAngleHeight;
            fRectTmp.bottom = static_cast<float>(m_rect.bottom);
            break;
        case 2:
            fRectTmp.left = static_cast<float>(m_rect.left);
            fRectTmp.right = static_cast<float>(m_rect.left) + m_fAngleWidth;
            fRectTmp.top = fYTop - fVertLineWidth;
            fRectTmp.bottom = fYTop;
            break;
        case 3:
            fRectTmp.left = static_cast<float>(m_rect.right) - m_fAngleWidth;
            fRectTmp.right = static_cast<float>(m_rect.right);
            fRectTmp.top = fYTop - fVertLineWidth;
            fRectTmp.bottom = fYTop;
            break;
        }

        if (lineType < 2)
        {
            pVert[vnum].pos.x = fRectTmp.left;
            pVert[vnum].pos.y = fRectTmp.top;
            vnum++;
            pVert[vnum].pos.x = fRectTmp.right;
            pVert[vnum].pos.y = fRectTmp.top;
            vnum++;
            pVert[vnum].pos.x = fRectTmp.left;
            pVert[vnum].pos.y = fRectTmp.bottom;
            vnum++;
            pVert[vnum].pos.x = fRectTmp.right;
            pVert[vnum].pos.y = fRectTmp.bottom;
            vnum++;
        }
        else
        {
            pVert[vnum].pos.x = fRectTmp.left;
            pVert[vnum].pos.y = fRectTmp.bottom;
            vnum++;
            pVert[vnum].pos.x = fRectTmp.left;
            pVert[vnum].pos.y = fRectTmp.top;
            vnum++;
            pVert[vnum].pos.x = fRectTmp.right;
            pVert[vnum].pos.y = fRectTmp.bottom;
            vnum++;
            pVert[vnum].pos.x = fRectTmp.right;
            pVert[vnum].pos.y = fRectTmp.top;
            vnum++;
        }

        fXTop += fHorzLineWidth;
        fYTop -= fVertLineWidth;
    }

    m_rs->UnLockVertexBuffer(m_idVBuf);
    m_rs->UnLockIndexBuffer(m_idIBuf);
}

bool CXI_BOUNDER::IsClick(int buttonID, int32_t xPos, int32_t yPos)
{
    return false;
}

void CXI_BOUNDER::ChangePosition(XYRECT &rNewPos)
{
    // Fill buffers
    auto *pVert = static_cast<XI_ONETEX_VERTEX *>(m_rs->LockVertexBuffer(m_idVBuf));

    if (pVert == nullptr)
        throw std::runtime_error("can not create the index&vertex buffers");

    int32_t i;

    m_rect = rNewPos;

    const auto fBoxWidth = static_cast<float>(m_rect.right - m_rect.left) - m_fAngleWidth * 2.f;
    const auto fBoxHeight = static_cast<float>(m_rect.bottom - m_rect.top) - m_fAngleHeight * 2.f;
    const auto fHorzLineWidth = fBoxWidth / m_nHorzLineQuantity;

    const auto fVertLineWidth = fBoxHeight / m_nVertLineQuantity;

    for (i = 0; i < m_nVert; i++)
    {
        pVert[i].color = m_dwColor;
        pVert[i].pos.z = 1.f;
    }

    auto vnum = 0;
    // fill angle rectangles
    FXYRECT texRectTmp;
    FXYRECT fRectTmp;
    for (i = 0; i < 4; i++)
    {
        switch (i)
        {
        case 0:
            fRectTmp.left = static_cast<float>(m_rect.left);
            fRectTmp.right = static_cast<float>(m_rect.left) + m_fAngleWidth;
            fRectTmp.top = static_cast<float>(m_rect.top);
            fRectTmp.bottom = static_cast<float>(m_rect.top) + m_fAngleHeight;
            pPictureService->GetTexturePos(m_idAngle, texRectTmp);
            break;
        case 1:
            fRectTmp.left = static_cast<float>(m_rect.right) - m_fAngleWidth;
            fRectTmp.right = static_cast<float>(m_rect.right);
            fRectTmp.top = static_cast<float>(m_rect.top);
            fRectTmp.bottom = static_cast<float>(m_rect.top) + m_fAngleHeight;
            pPictureService->GetTexturePos(TEXTURE_MODIFY_HORZFLIP, m_idAngle, texRectTmp);
            break;
        case 2:
            fRectTmp.left = static_cast<float>(m_rect.left);
            fRectTmp.right = static_cast<float>(m_rect.left) + m_fAngleWidth;
            fRectTmp.top = static_cast<float>(m_rect.bottom) - m_fAngleHeight;
            fRectTmp.bottom = static_cast<float>(m_rect.bottom);
            pPictureService->GetTexturePos(TEXTURE_MODIFY_VERTFLIP, m_idAngle, texRectTmp);
            break;
        case 3:
            fRectTmp.left = static_cast<float>(m_rect.right) - m_fAngleWidth;
            fRectTmp.right = static_cast<float>(m_rect.right);
            fRectTmp.top = static_cast<float>(m_rect.bottom) - m_fAngleHeight;
            fRectTmp.bottom = static_cast<float>(m_rect.bottom);
            pPictureService->GetTexturePos(TEXTURE_MODIFY_HORZFLIP | TEXTURE_MODIFY_VERTFLIP, m_idAngle, texRectTmp);
            break;
        }

        pVert[vnum].tu = texRectTmp.left;
        pVert[vnum].tv = texRectTmp.top;
        pVert[vnum + 1].tu = texRectTmp.right;
        pVert[vnum + 1].tv = texRectTmp.top;
        pVert[vnum + 2].tu = texRectTmp.left;
        pVert[vnum + 2].tv = texRectTmp.bottom;
        pVert[vnum + 3].tu = texRectTmp.right;
        pVert[vnum + 3].tv = texRectTmp.bottom;

        pVert[vnum].pos.x = fRectTmp.left;
        pVert[vnum].pos.y = fRectTmp.top;
        pVert[vnum + 1].pos.x = fRectTmp.right;
        pVert[vnum + 1].pos.y = fRectTmp.top;
        pVert[vnum + 2].pos.x = fRectTmp.left;
        pVert[vnum + 2].pos.y = fRectTmp.bottom;
        pVert[vnum + 3].pos.x = fRectTmp.right;
        pVert[vnum + 3].pos.y = fRectTmp.bottom;

        vnum += 4;
    }

    // fill horizontal & vertical line rectangles
    FXYRECT tmpRect1, tmpRect2;
    pPictureService->GetTexturePos(m_idHorzLine, tmpRect1);
    pPictureService->GetTexturePos(TEXTURE_MODIFY_VERTFLIP, m_idHorzLine, tmpRect2);
    auto fXTop = m_rect.left + m_fAngleWidth;
    auto fYTop = m_rect.bottom - m_fAngleHeight;
    auto lineType = 0; // top horizontal line
    for (i = 0; i < m_nHorzLineQuantity * 2 + m_nVertLineQuantity * 2; i++)
    {
        if (i == m_nHorzLineQuantity)
        {
            lineType = 1; // bottom horizontal line
            fXTop = m_rect.left + m_fAngleWidth;
        }
        if (i == m_nHorzLineQuantity * 2)
        {
            lineType = 2; // left vertical line
            fYTop = m_rect.bottom - m_fAngleHeight;
        }
        if (i == m_nHorzLineQuantity * 2 + m_nVertLineQuantity)
        {
            lineType = 3; // right vertical line
            fYTop = m_rect.bottom - m_fAngleHeight;
        }

        //
        if (lineType == 0 || lineType == 2)
        {
            pVert[vnum].tu = tmpRect1.left;
            pVert[vnum].tv = tmpRect1.top;
            pVert[vnum + 1].tu = tmpRect1.right;
            pVert[vnum + 1].tv = tmpRect1.top;
            pVert[vnum + 2].tu = tmpRect1.left;
            pVert[vnum + 2].tv = tmpRect1.bottom;
            pVert[vnum + 3].tu = tmpRect1.right;
            pVert[vnum + 3].tv = tmpRect1.bottom;
        }
        else
        {
            pVert[vnum].tu = tmpRect2.left;
            pVert[vnum].tv = tmpRect2.top;
            pVert[vnum + 1].tu = tmpRect2.right;
            pVert[vnum + 1].tv = tmpRect2.top;
            pVert[vnum + 2].tu = tmpRect2.left;
            pVert[vnum + 2].tv = tmpRect2.bottom;
            pVert[vnum + 3].tu = tmpRect2.right;
            pVert[vnum + 3].tv = tmpRect2.bottom;
        }

        switch (lineType)
        {
        case 0:
            fRectTmp.left = fXTop;
            fRectTmp.right = fXTop + fHorzLineWidth;
            fRectTmp.top = static_cast<float>(m_rect.top);
            fRectTmp.bottom = static_cast<float>(m_rect.top) + m_fAngleHeight;
            break;
        case 1:
            fRectTmp.left = fXTop;
            fRectTmp.right = fXTop + fHorzLineWidth;
            fRectTmp.top = static_cast<float>(m_rect.bottom) - m_fAngleHeight;
            fRectTmp.bottom = static_cast<float>(m_rect.bottom);
            break;
        case 2:
            fRectTmp.left = static_cast<float>(m_rect.left);
            fRectTmp.right = static_cast<float>(m_rect.left) + m_fAngleWidth;
            fRectTmp.top = fYTop - fVertLineWidth;
            fRectTmp.bottom = fYTop;
            break;
        case 3:
            fRectTmp.left = static_cast<float>(m_rect.right) - m_fAngleWidth;
            fRectTmp.right = static_cast<float>(m_rect.right);
            fRectTmp.top = fYTop - fVertLineWidth;
            fRectTmp.bottom = fYTop;
            break;
        }

        if (lineType < 2)
        {
            pVert[vnum].pos.x = fRectTmp.left;
            pVert[vnum].pos.y = fRectTmp.top;
            vnum++;
            pVert[vnum].pos.x = fRectTmp.right;
            pVert[vnum].pos.y = fRectTmp.top;
            vnum++;
            pVert[vnum].pos.x = fRectTmp.left;
            pVert[vnum].pos.y = fRectTmp.bottom;
            vnum++;
            pVert[vnum].pos.x = fRectTmp.right;
            pVert[vnum].pos.y = fRectTmp.bottom;
            vnum++;
        }
        else
        {
            pVert[vnum].pos.x = fRectTmp.left;
            pVert[vnum].pos.y = fRectTmp.bottom;
            vnum++;
            pVert[vnum].pos.x = fRectTmp.left;
            pVert[vnum].pos.y = fRectTmp.top;
            vnum++;
            pVert[vnum].pos.x = fRectTmp.right;
            pVert[vnum].pos.y = fRectTmp.bottom;
            vnum++;
            pVert[vnum].pos.x = fRectTmp.right;
            pVert[vnum].pos.y = fRectTmp.top;
            vnum++;
        }

        fXTop += fHorzLineWidth;
        fYTop -= fVertLineWidth;
    }

    m_rs->UnLockVertexBuffer(m_idVBuf);
}

void CXI_BOUNDER::SaveParametersToIni()
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
