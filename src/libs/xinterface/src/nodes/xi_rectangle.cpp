#include "xi_rectangle.h"

CXI_RECTANGLE::CXI_RECTANGLE()
{
    m_nNodeType = NODETYPE_RECTANGLE;
}

CXI_RECTANGLE::~CXI_RECTANGLE()
{
    ReleaseAll();
}

void CXI_RECTANGLE::Draw(bool bSelected, uint32_t Delta_Time)
{
    if (m_bUse)
    {
        m_rs->TextureSet(0, 0);
        m_rs->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, XI_NOTEX_FVF, 2, m_pVert, sizeof(XI_NOTEX_VERTEX), "iRectangle");
        if (m_bBorder)
        {
            RS_LINE pLines[8];
            for (auto i = 0; i < 8; i++)
            {
                pLines[i].vPos.z = 1.f;
                pLines[i].dwColor = m_dwBorderColor;
            }
            pLines[0].vPos.x = pLines[1].vPos.x = pLines[2].vPos.x = pLines[7].vPos.x = static_cast<float>(m_rect.left);
            pLines[1].vPos.y = pLines[2].vPos.y = pLines[3].vPos.y = pLines[4].vPos.y = static_cast<float>(m_rect.top);
            pLines[3].vPos.x = pLines[4].vPos.x = pLines[5].vPos.x = pLines[6].vPos.x =
                static_cast<float>(m_rect.right);
            pLines[0].vPos.y = pLines[5].vPos.y = pLines[6].vPos.y = pLines[7].vPos.y =
                static_cast<float>(m_rect.bottom);
            m_rs->DrawLines(pLines, 4, "iRectangle");
        }
    }
}

bool CXI_RECTANGLE::Init(INIFILE *ini1, const char *name1, INIFILE *ini2, const char *name2, VDX9RENDER *rs,
                         XYRECT &hostRect, XYPOINT &ScreenSize)
{
    if (!CINODE::Init(ini1, name1, ini2, name2, rs, hostRect, ScreenSize))
        return false;
    return true;
}

void CXI_RECTANGLE::ReleaseAll()
{
}

int CXI_RECTANGLE::CommandExecute(int wActCode)
{
    return -1;
}

void CXI_RECTANGLE::LoadIni(INIFILE *ini1, const char *name1, INIFILE *ini2, const char *name2)
{
    // fill vertex positions
    for (auto i = 0; i < 4; i++)
        m_pVert[i].pos.z = 1.f;
    m_pVert[0].pos.x = static_cast<float>(m_rect.left);
    m_pVert[0].pos.y = static_cast<float>(m_rect.top);
    m_pVert[1].pos.x = static_cast<float>(m_rect.left);
    m_pVert[1].pos.y = static_cast<float>(m_rect.bottom);
    m_pVert[2].pos.x = static_cast<float>(m_rect.right);
    m_pVert[2].pos.y = static_cast<float>(m_rect.top);
    m_pVert[3].pos.x = static_cast<float>(m_rect.right);
    m_pVert[3].pos.y = static_cast<float>(m_rect.bottom);

    // Get rectangle left colors
    m_dwLeftColor = GetIniARGB(ini1, name1, ini2, name2, "leftColor", 0);

    // Get rectangle top colors
    m_dwTopColor = GetIniARGB(ini1, name1, ini2, name2, "topColor", 0);

    // Get rectangle right colors
    m_dwRightColor = GetIniARGB(ini1, name1, ini2, name2, "rightColor", 0);

    // Get rectangle bottom colors
    m_dwBottomColor = GetIniARGB(ini1, name1, ini2, name2, "bottomColor", 0);

    // Get bounder parameters
    m_dwBorderColor = GetIniARGB(ini1, name1, ini2, name2, "borderColor", 0);
    m_bBorder = ALPHA(m_dwBorderColor) != 0;

    UpdateColors();
}

void CXI_RECTANGLE::UpdateColors()
{
    int alpha, red, green, blue;

    // set left top vertex color
    alpha = (ALPHA(m_dwLeftColor) * ALPHA(m_dwTopColor)) >> 8L;
    red = (RED(m_dwLeftColor) * RED(m_dwTopColor)) >> 8L;
    green = (GREEN(m_dwLeftColor) * GREEN(m_dwTopColor)) >> 8L;
    blue = (BLUE(m_dwLeftColor) * BLUE(m_dwTopColor)) >> 8L;
    m_pVert[0].color = ARGB(alpha, red, green, blue);

    // set left bottom vertex color
    alpha = (ALPHA(m_dwLeftColor) * ALPHA(m_dwBottomColor)) >> 8L;
    red = (RED(m_dwLeftColor) * RED(m_dwBottomColor)) >> 8L;
    green = (GREEN(m_dwLeftColor) * GREEN(m_dwBottomColor)) >> 8L;
    blue = (BLUE(m_dwLeftColor) * BLUE(m_dwBottomColor)) >> 8L;
    m_pVert[1].color = ARGB(alpha, red, green, blue);

    // set right top vertex color
    alpha = (ALPHA(m_dwRightColor) * ALPHA(m_dwTopColor)) >> 8L;
    red = (RED(m_dwRightColor) * RED(m_dwTopColor)) >> 8L;
    green = (GREEN(m_dwRightColor) * GREEN(m_dwTopColor)) >> 8L;
    blue = (BLUE(m_dwRightColor) * BLUE(m_dwTopColor)) >> 8L;
    m_pVert[2].color = ARGB(alpha, red, green, blue);

    // set right bottom vertex color
    alpha = (ALPHA(m_dwRightColor) * ALPHA(m_dwBottomColor)) >> 8L;
    red = (RED(m_dwRightColor) * RED(m_dwBottomColor)) >> 8L;
    green = (GREEN(m_dwRightColor) * GREEN(m_dwBottomColor)) >> 8L;
    blue = (BLUE(m_dwRightColor) * BLUE(m_dwBottomColor)) >> 8L;
    m_pVert[3].color = ARGB(alpha, red, green, blue);
}

bool CXI_RECTANGLE::IsClick(int buttonID, int32_t xPos, int32_t yPos)
{
    if (!m_bClickable)
        return false;
    if (xPos >= m_rect.left && xPos <= m_rect.right && yPos >= m_rect.top && yPos <= m_rect.bottom)
        return true;
    return false;
}

void CXI_RECTANGLE::ChangePosition(XYRECT &rNewPos)
{
    m_rect = rNewPos;

    m_pVert[0].pos.x = static_cast<float>(m_rect.left);
    m_pVert[0].pos.y = static_cast<float>(m_rect.top);
    m_pVert[1].pos.x = static_cast<float>(m_rect.left);
    m_pVert[1].pos.y = static_cast<float>(m_rect.bottom);
    m_pVert[2].pos.x = static_cast<float>(m_rect.right);
    m_pVert[2].pos.y = static_cast<float>(m_rect.top);
    m_pVert[3].pos.x = static_cast<float>(m_rect.right);
    m_pVert[3].pos.y = static_cast<float>(m_rect.bottom);
}

void CXI_RECTANGLE::SaveParametersToIni()
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

uint32_t CXI_RECTANGLE::MessageProc(int32_t msgcode, MESSAGE &message)
{
    switch (msgcode)
    {
    case 0: // Change the position of the rectangle
    {
        XYRECT newRect;
        newRect.left = message.Long();
        newRect.top = message.Long();
        newRect.right = message.Long();
        newRect.bottom = message.Long();
        ChangePosition(newRect);
    }
    break;
    case 1: // Change rectangle and border color
    {
        m_dwTopColor = m_dwBottomColor = ARGB(255, 255, 255, 255);
        m_dwLeftColor = m_dwRightColor = message.Long();
        m_dwBorderColor = message.Long();
        m_bBorder = ALPHA(m_dwBorderColor) != 0;
        UpdateColors();
    }
    break;
    }

    return 0;
}
