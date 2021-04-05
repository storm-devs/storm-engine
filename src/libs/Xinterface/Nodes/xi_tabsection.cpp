#include "xi_tabsection.h"
#include <stdio.h>

CXI_TABSECTION::CXI_TABSECTION()
{
    m_rs = nullptr;
    m_bClickable = true;
    m_nNodeType = NODETYPE_TABSECTION;

    m_idIconTexture = -1;
    m_idVBuf = -1;
    m_idIBuf = -1;
    m_nSubQ = 0;
}

CXI_TABSECTION::~CXI_TABSECTION()
{
    ReleaseAll();
}

void CXI_TABSECTION::Draw(bool bSelected, uint32_t Delta_Time)
{
    // Drawing
    if (m_idIconTexture != -1 && m_idVBuf != -1 && m_idIBuf != -1)
    {
        m_rs->TextureSet(0, m_idIconTexture);
        m_rs->DrawBuffer(m_idVBuf, sizeof(XI_ONETEX_VERTEX), m_idIBuf, 0, m_nSubQ * 4, 0, m_nSubQ * 2, "iIcon");
    }

    // Output headers
}

bool CXI_TABSECTION::Init(INIFILE *ini1, const char *name1, INIFILE *ini2, const char *name2, VDX9RENDER *rs,
                          XYRECT &hostRect, XYPOINT &ScreenSize)
{
    if (!CINODE::Init(ini1, name1, ini2, name2, rs, hostRect, ScreenSize))
        return false;
    return true;
}

void CXI_TABSECTION::ReleaseAll()
{
    PICTURE_TEXTURE_RELEASE(pPictureService, m_sIconGroupName.c_str(), m_idIconTexture);
    VERTEX_BUFFER_RELEASE(m_rs, m_idVBuf);
    INDEX_BUFFER_RELEASE(m_rs, m_idIBuf);
    m_nSubQ = 0;
}

int CXI_TABSECTION::CommandExecute(int wActCode)
{
    if (m_bUse)
    {
        switch (wActCode)
        {
        case ACTION_ACTIVATE:
            break;
        case ACTION_MOUSECLICK:
            break;
        }
    }
    return -1;
}

bool CXI_TABSECTION::IsClick(int buttonID, long xPos, long yPos)
{
    if (xPos >= m_rect.left && xPos <= m_rect.right && yPos >= m_rect.top && yPos <= m_rect.bottom && m_bClickable &&
        m_bSelected && m_bUse)
        return true;
    return false;
}

void CXI_TABSECTION::ChangePosition(XYRECT &rNewPos)
{
    m_rect = rNewPos;
}

void CXI_TABSECTION::SaveParametersToIni()
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

uint32_t CXI_TABSECTION::MessageProc(long msgcode, MESSAGE &message)
{
    switch (msgcode)
    {
    case 0: //
        break;
    }

    return 0;
}

void CXI_TABSECTION::LoadIni(INIFILE *ini1, const char *name1, INIFILE *ini2, const char *name2)
{
    SetGlowCursor(false);
}

void CXI_TABSECTION::UpdateImage()
{
}
