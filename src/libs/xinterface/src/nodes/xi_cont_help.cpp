#include "xi_cont_help.h"

CXI_CONTEXTHELP::CXI_CONTEXTHELP()
{
    m_dwColor = 0;
    m_dwBorderColor = 0;
    m_dwFontColor = 0;

    m_bBorder = false;
    m_offset = 0;
    m_nHelpWidth = 0;

    m_helpQuantity = 0;
    m_pHelpList = nullptr;
    m_curHelp = nullptr;
    m_defaultString = -1;

    m_nMaxDelayCounter = 0;
    m_nCurDelayCounter = 0;
    m_idTempString = -1;
    m_sTempString = nullptr;

    m_idFont = -1;
    m_nNodeType = NODETYPE_CONTEXTHELP;
    m_fCurScale = m_fMaxScale = 0.8f;
}

CXI_CONTEXTHELP::~CXI_CONTEXTHELP()
{
    ReleaseAll();
}

void CXI_CONTEXTHELP::Draw(bool bSelected, uint32_t Delta_Time)
{
    if (m_bUse)
    {
        // outputting the context help rectangle
        m_rs->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, XI_NOTEX_FVF, 2, m_pVert, sizeof(XI_NOTEX_VERTEX), "iRectangle");
        if (m_bBorder)
            m_rs->DrawLines(m_pLines, 4, "iRectangle");

        // help line output
        m_rs->ExtPrint(m_idFont, m_dwFontColor, m_dwColor, PR_ALIGN_CENTER, true, m_fCurScale, m_screenSize.x,
                       m_screenSize.y, (m_rect.left + m_rect.right) / 2, m_rect.top + m_offset, "%s",
                       GetCurrentHelpString(Delta_Time));
    }
}

bool CXI_CONTEXTHELP::Init(INIFILE *ini1, const char *name1, INIFILE *ini2, const char *name2, VDX9RENDER *rs,
                           XYRECT &hostRect, XYPOINT &ScreenSize)
{
    if (!CINODE::Init(ini1, name1, ini2, name2, rs, hostRect, ScreenSize))
        return false;
    return true;
}

void CXI_CONTEXTHELP::ReleaseAll()
{
    if (m_pHelpList != nullptr)
        for (auto i = 0; i < m_helpQuantity; i++)
            STORM_DELETE(m_pHelpList[i].nodeName);
    STORM_DELETE(m_pHelpList);
    m_helpQuantity = 0;
    m_curHelp = nullptr;

    FONT_RELEASE(m_rs, m_idFont);
    STORM_DELETE(m_sTempString);
    m_nCurDelayCounter = 0;
}

int CXI_CONTEXTHELP::CommandExecute(int wActCode)
{
    return -1;
}

void CXI_CONTEXTHELP::LoadIni(INIFILE *ini1, const char *name1, INIFILE *ini2, const char *name2)
{
    int i;
    char param[256];

    m_nHelpWidth = m_rect.right - m_rect.left - 4;
    if (m_nHelpWidth < 0)
        m_nHelpWidth = 0;

    // Get rectangle color
    m_dwColor = GetIniARGB(ini1, name1, ini2, name2, "color", ARGB(255, 255, 255, 255));

    // Get bounder parameters
    m_dwBorderColor = GetIniARGB(ini1, name1, ini2, name2, "borderColor", m_dwColor);
    m_bBorder = m_dwBorderColor != m_dwColor;

    // Get help string  parameters
    m_nMaxDelayCounter = GetIniLong(ini1, name1, ini2, name2, "delay", 0);
    m_dwFontColor = GetIniARGB(ini1, name1, ini2, name2, "fontColor", ARGB(255, 255, 255, 255));

    if (ReadIniString(ini1, name1, ini2, name2, "font", param, sizeof(param), ""))
        if ((m_idFont = m_rs->LoadFont(param)) == -1)
            core.Trace("can not load font:'%s'", param);
    m_offset = GetIniLong(ini1, name1, ini2, name2, "offset", 0);

    // Get help strings quantity
    m_helpQuantity = 0;
    if (ini1->ReadString(name1, "helpstr", param, sizeof(param) - 1, ""))
        do
        {
            m_helpQuantity++;
        } while (ini1->ReadStringNext(name1, "helpstr", param, sizeof(param) - 1));

    // Get default help string
    if (ini1->ReadString(name1, "defhelp", param, sizeof(param) - 1, ""))
        m_defaultString = pStringService->GetStringNum(param);
    else
        m_defaultString = -1;
    // Create help stringes array
    if (m_helpQuantity > 0)
    {
        if ((m_pHelpList = new HELPEntity[m_helpQuantity]) == nullptr)
            throw std::runtime_error("allocate memory error");
        std::memset(m_pHelpList, 0, sizeof(HELPEntity) * m_helpQuantity);
        ini1->ReadString(name1, "helpstr", param, sizeof(param) - 1, "");
        char nodeName[sizeof(param)], stringName[sizeof(param)];
        for (i = 0; i < m_helpQuantity; i++)
        {
            sscanf(param, "%[^,],%[^,]", nodeName, stringName);
            if (nodeName[0] != 0)
            {
                const auto len = strlen(nodeName) + 1;
                if ((m_pHelpList[i].nodeName = new char[len]) == nullptr)
                    throw std::runtime_error("allocate memory error");
                memcpy(m_pHelpList[i].nodeName, nodeName, len);
                m_pHelpList[i].idHelpString = pStringService->GetStringNum(stringName);
            }
            ini1->ReadStringNext(name1, "helpstr", param, sizeof(param) - 1);
        }
    }
    else
    {
        m_helpQuantity = 0;
        m_pHelpList = nullptr;
    }

    // fill vertex parameters
    for (i = 0; i < 4; i++)
    {
        m_pVert[i].pos.z = 1.f;
        m_pVert[i].color = m_dwColor;
    }
    m_pVert[0].pos.x = static_cast<float>(m_rect.left);
    m_pVert[0].pos.y = static_cast<float>(m_rect.top);
    m_pVert[1].pos.x = static_cast<float>(m_rect.left);
    m_pVert[1].pos.y = static_cast<float>(m_rect.bottom);
    m_pVert[2].pos.x = static_cast<float>(m_rect.right);
    m_pVert[2].pos.y = static_cast<float>(m_rect.top);
    m_pVert[3].pos.x = static_cast<float>(m_rect.right);
    m_pVert[3].pos.y = static_cast<float>(m_rect.bottom);

    // fill lines parameters
    if (m_bBorder)
    {
        for (auto i = 0; i < 8; i++)
        {
            m_pLines[i].vPos.z = 1.f;
            m_pLines[i].dwColor = m_dwBorderColor;
        }
        m_pLines[0].vPos.x = m_pLines[1].vPos.x = m_pLines[2].vPos.x = m_pLines[7].vPos.x =
            static_cast<float>(m_rect.left);
        m_pLines[1].vPos.y = m_pLines[2].vPos.y = m_pLines[3].vPos.y = m_pLines[4].vPos.y =
            static_cast<float>(m_rect.top);
        m_pLines[3].vPos.x = m_pLines[4].vPos.x = m_pLines[5].vPos.x = m_pLines[6].vPos.x =
            static_cast<float>(m_rect.right);
        m_pLines[0].vPos.y = m_pLines[5].vPos.y = m_pLines[6].vPos.y = m_pLines[7].vPos.y =
            static_cast<float>(m_rect.bottom);
    }
}

bool CXI_CONTEXTHELP::IsClick(int buttonID, int32_t xPos, int32_t yPos)
{
    return false;
}

void CXI_CONTEXTHELP::ChangePosition(XYRECT &rNewPos)
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

    // fill lines parameters
    if (m_bBorder)
    {
        m_pLines[0].vPos.x = m_pLines[1].vPos.x = m_pLines[2].vPos.x = m_pLines[7].vPos.x =
            static_cast<float>(m_rect.left);
        m_pLines[1].vPos.y = m_pLines[2].vPos.y = m_pLines[3].vPos.y = m_pLines[4].vPos.y =
            static_cast<float>(m_rect.top);
        m_pLines[3].vPos.x = m_pLines[4].vPos.x = m_pLines[5].vPos.x = m_pLines[6].vPos.x =
            static_cast<float>(m_rect.right);
        m_pLines[0].vPos.y = m_pLines[5].vPos.y = m_pLines[6].vPos.y = m_pLines[7].vPos.y =
            static_cast<float>(m_rect.bottom);
    }
}

void CXI_CONTEXTHELP::SaveParametersToIni()
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

void CXI_CONTEXTHELP::ChangeNode(CINODE *pNode)
{
    if (m_curHelp != nullptr)
        if (m_curHelp->pNode == pNode)
            return;

    if (m_pHelpList != nullptr)
    {
        int i;
        for (i = 0; i < m_helpQuantity; i++)
            if (m_pHelpList[i].pNode == pNode)
                break;
        if (i < m_helpQuantity)
            m_curHelp = &m_pHelpList[i];
        else
            m_curHelp = nullptr;
    }
}

void CXI_CONTEXTHELP::SetTempHelp(const char *pStr)
{
    if (pStr == nullptr)
        return;

    // remove the old temporary line

    delete m_sTempString;
    m_sTempString = nullptr;

    int32_t nCurStrWidth = 0;
    if (pStr[0] == '#')
    {
        // get the help line directly
        const auto len = strlen(pStr);
        if ((m_sTempString = new char[len]) == nullptr)
            throw std::runtime_error("allocate memory error");
        memcpy(m_sTempString, &pStr[1], len);
        nCurStrWidth = m_rs->StringWidth(m_sTempString, m_idFont, m_fMaxScale, m_screenSize.x);
    }
    else // or the name of the string in the localization list
    {
        m_idTempString = pStringService->GetStringNum(pStr);
        if (m_idTempString != -1)
            nCurStrWidth = m_rs->StringWidth(pStringService->GetString(m_idTempString), m_idFont, m_fMaxScale, 0);
        // m_screenSize.x);
    }

    if (nCurStrWidth > m_nHelpWidth)
        m_fCurScale = m_fMaxScale * m_nHelpWidth / nCurStrWidth;
    else
        m_fCurScale = m_fMaxScale;

    m_nCurDelayCounter = m_nMaxDelayCounter; // set the delay counter
}

char *CXI_CONTEXTHELP::GetCurrentHelpString(uint32_t deltaTime)
{
    if (static_cast<uint32_t>(m_nCurDelayCounter) > deltaTime)
        m_nCurDelayCounter -= deltaTime;
    else
        m_nCurDelayCounter = 0;

    if (m_nCurDelayCounter > 0)
    {
        if (m_sTempString != nullptr)
            return m_sTempString;
        if (m_idTempString != -1)
            return pStringService->GetString(m_idTempString);
    }

    if (m_curHelp != nullptr)
        return pStringService->GetString(m_curHelp->idHelpString);

    return pStringService->GetString(m_defaultString);
}

uint32_t CXI_CONTEXTHELP::MessageProc(int32_t msgcode, MESSAGE &message)
{
    switch (msgcode)
    {
    case 0: // remove the old temporary line
    {
        delete m_sTempString;
        m_sTempString = nullptr;
        m_idTempString = -1;
    }
    break;

    case 1: // get temporary string
    {
        auto *pvdat = message.ScriptVariablePointer();
        if (pvdat == nullptr)
            return 0;
        if (m_sTempString == nullptr && m_idTempString == -1)
            return 0;

        if (m_sTempString != nullptr)
        {
            pvdat->Set(m_sTempString);
            return 1;
        }

        if (m_idTempString >= 0)
        {
            auto *const pstr = pStringService->GetStringName(m_idTempString);
            if (pstr != nullptr)
            {
                pvdat->Set(pstr);
                return 1;
            }
        }
    }
    break;
    }
    return 0;
}
