#include "xi_strcollection.h"
#include <stdio.h>

CXI_STRCOLLECTION::CXI_STRCOLLECTION()
{
    m_rs = nullptr;

    m_pStrDescr = nullptr;
    m_nStr = 0;
    m_nNodeType = NODETYPE_STRINGCOLLECTION;

    m_nEditIndex = -1;
}

CXI_STRCOLLECTION::~CXI_STRCOLLECTION()
{
    ReleaseAll();
}

int CXI_STRCOLLECTION::CommandExecute(int wActCode)
{
    return -1;
}

void CXI_STRCOLLECTION::Draw(bool bSelected, uint32_t Delta_Time)
{
    if (m_bUse)
    {
        for (auto i = 0; i < m_nStr; i++)
            if (m_pStrDescr[i].strStr != nullptr)
                m_rs->ExtPrint(m_pStrDescr[i].nFontNum, m_pStrDescr[i].foreColor, m_pStrDescr[i].backColor,
                               m_pStrDescr[i].wAlignment, m_pStrDescr[i].bShadow, m_pStrDescr[i].fScale, m_screenSize.x,
                               m_screenSize.y, m_pStrDescr[i].scrPos.x, m_pStrDescr[i].scrPos.y, "%s",
                               m_pStrDescr[i].strStr);
            else
                m_rs->ExtPrint(m_pStrDescr[i].nFontNum, m_pStrDescr[i].foreColor, m_pStrDescr[i].backColor,
                               m_pStrDescr[i].wAlignment, m_pStrDescr[i].bShadow, m_pStrDescr[i].fScale, m_screenSize.x,
                               m_screenSize.y, m_pStrDescr[i].scrPos.x, m_pStrDescr[i].scrPos.y, "%s",
                               pStringService->GetString(m_pStrDescr[i].strNum));
    }
}

bool CXI_STRCOLLECTION::Init(INIFILE *ini1, const char *name1, INIFILE *ini2, const char *name2, VDX9RENDER *rs,
                             XYRECT &hostRect, XYPOINT &ScreenSize)
{
    if (!CINODE::Init(ini1, name1, ini2, name2, rs, hostRect, ScreenSize))
        return false;
    // screen position for that is host screen position
    memcpy(&m_rect, &m_hostRect, sizeof(m_hostRect));
    SetGlowCursor(false);
    return true;
}

void CXI_STRCOLLECTION::LoadIni(INIFILE *ini1, const char *name1, INIFILE *ini2, const char *name2)
{
    int i;
    char param[256];

    const auto bRelativeRect = !GetIniLong(ini1, name1, ini2, name2, "bAbsoluteRectangle", 0);

    auto *ini = ini1;
    const auto *name = name1;
    if (!ini1)
    {
        ini = ini2;
        name = name2;
    }

    // get string quantity
    m_nStr = 0;
    if (ini && ini->ReadString(name, "string", param, sizeof(param) - 1, ""))
        do
        {
            m_nStr++;
        } while (ini->ReadStringNext(name, "string", param, sizeof(param) - 1));

    if (m_nStr)
    {
        m_pStrDescr = new STRINGDESCR[m_nStr];

        // Set strings
        int a_fc, r_fc, g_fc, b_fc;
        int a_bc, r_bc, g_bc, b_bc;
        char strName[sizeof(param)];
        char strState[sizeof(param)];
        char fontName[sizeof(param)];
        ini->ReadString(name, "string", param, sizeof(param) - 1, "");
        for (i = 0; i < m_nStr; i++)
        {
            // set all parameters to zero
            PZERO(&m_pStrDescr[i], sizeof(STRINGDESCR));
            m_pStrDescr[i].nFontNum = -1;

            // read same parameters
            strState[0] = '\0';
            sscanf(param, "%[^,],font:%[^,],pos:{%d,%d},fc:{%d,%d,%d,%d},bc:{%d,%d,%d,%d},scale:%f,state:{%[^}]}",
                   strName, fontName, &m_pStrDescr[i].scrPos.x, &m_pStrDescr[i].scrPos.y, &a_fc, &r_fc, &g_fc, &b_fc,
                   &a_bc, &r_bc, &g_bc, &b_bc, &m_pStrDescr[i].fScale, strState);
            DublicateString(m_pStrDescr[i].sFontName, fontName);
            m_pStrDescr[i].nFontNum = m_rs->LoadFont(fontName);
            if (bRelativeRect)
            {
                m_pStrDescr[i].scrPos.x += m_hostRect.left;
                m_pStrDescr[i].scrPos.y += m_hostRect.top;
            }

            // set foreground & background colors
            m_pStrDescr[i].foreColor = ARGB(a_fc, r_fc, g_fc, b_fc);
            m_pStrDescr[i].backColor = ARGB(a_bc, r_bc, g_bc, b_bc);

            // set states
            for (int k = strlen(strState); k >= 0; k--)
            {
                if (strState[k] == 'C' || strState[k] == 'c')
                    m_pStrDescr[i].wAlignment = PR_ALIGN_CENTER;
                if (strState[k] == 'R' || strState[k] == 'r')
                    m_pStrDescr[i].wAlignment = PR_ALIGN_RIGHT;
                if (strState[k] == 'S' || strState[k] == 's')
                    m_pStrDescr[i].bShadow = true;
            }

            m_pStrDescr[i].strNum = -1;
            if (strName[0] == '#')
                DublicateString(m_pStrDescr[i].strStr, &strName[1]);
            else
                m_pStrDescr[i].strNum = pStringService->GetStringNum(strName);

            ini->ReadStringNext(name, "string", param, sizeof(param) - 1);
        }
    }
}

void CXI_STRCOLLECTION::ReleaseAll()
{
    for (auto i = 0; i < m_nStr; i++)
    {
        STORM_DELETE(m_pStrDescr[i].strID);
        STORM_DELETE(m_pStrDescr[i].strStr);
        STORM_DELETE(m_pStrDescr[i].sFontName);
        FONT_RELEASE(m_rs, m_pStrDescr[i].nFontNum);
    }
    STORM_DELETE(m_pStrDescr);
    m_nStr = 0;
}

bool CXI_STRCOLLECTION::IsClick(int buttonID, long xPos, long yPos)
{
    return false;
}

bool CXI_STRCOLLECTION::GetInternalNameList(std::vector<std::string> &aStr)
{
    aStr.clear();
    // aStr.Add();
    // aStr[0] = "All";
    aStr.push_back("All");
    for (long n = 0; n < m_nStr; n++)
    {
        if (m_pStrDescr[n].strID)
            continue;
        // aStr.Add();
        aStr.push_back(std::string{});
        char param[512];
        sprintf_s(param, sizeof(param), "%d - %s", n + 1, pStringService->GetStringName(m_pStrDescr[n].strNum));
        aStr[n + 1] = param;
    }
    return aStr.size() > 1;
}

void CXI_STRCOLLECTION::SetInternalName(std::string &sName)
{
    if (sName.empty())
        return;
    if (sName == "All")
        m_nEditIndex = -1;
    else
    {
        sscanf(sName.c_str(), "%d -", &m_nEditIndex);
        m_nEditIndex--;
    }
}

void CXI_STRCOLLECTION::ChangePosition(XYRECT &rNewPos)
{
    const auto nX = rNewPos.left - m_rect.left;
    const auto nY = rNewPos.top - m_rect.top;
    if (nX != 0 || nY != 0)
    {
        if (m_nEditIndex >= 0 && m_nEditIndex < m_nStr)
        {
            m_pStrDescr[m_nEditIndex].scrPos.x += nX;
            m_pStrDescr[m_nEditIndex].scrPos.y += nY;
            return;
        }
        for (long n = 0; n < m_nStr; n++)
        {
            m_pStrDescr[n].scrPos.x += nX;
            m_pStrDescr[n].scrPos.y += nY;
        }
    }
}

void CXI_STRCOLLECTION::SaveParametersToIni()
{
    //    char pcWriteParam[2048];

    auto pIni = fio->OpenIniFile(ptrOwner->m_sDialogFileName.c_str());
    if (!pIni)
    {
        core.Trace("Warning! Can`t open ini file name %s", ptrOwner->m_sDialogFileName.c_str());
        return;
    }

    pIni->DeleteSection(m_nodeName);
    // save position
    char param[2048];
    for (long n = 0; n < m_nStr; n++)
    {
        if (m_pStrDescr[n].strID)
            continue; // dynamically added string

        char pcState[3];
        switch (m_pStrDescr[n].wAlignment)
        {
        case PR_ALIGN_CENTER:
            pcState[0] = 'C';
            break;
        case PR_ALIGN_RIGHT:
            pcState[0] = 'R';
            break;
        default:
            pcState[0] = 'L';
        }
        if (m_pStrDescr[n].bShadow)
        {
            pcState[1] = 'S';
            pcState[2] = 0;
        }
        else
            pcState[1] = 0;

        if (m_pStrDescr[n].strStr)
            sprintf_s(param, sizeof(param),
                      "#%s,font:%s,pos:{%d,%d},fc:{%d,%d,%d,%d},bc:{%d,%d,%d,%d},scale:%.2f,state:{%s}",
                      m_pStrDescr[n].strStr,                            // strID
                      m_pStrDescr[n].sFontName,                         // font name
                      m_pStrDescr[n].scrPos.x, m_pStrDescr[n].scrPos.y, // pos
                      ALPHA(m_pStrDescr[n].foreColor), RED(m_pStrDescr[n].foreColor), GREEN(m_pStrDescr[n].foreColor),
                      BLUE(m_pStrDescr[n].foreColor), ALPHA(m_pStrDescr[n].backColor), RED(m_pStrDescr[n].backColor),
                      GREEN(m_pStrDescr[n].backColor), BLUE(m_pStrDescr[n].backColor), m_pStrDescr[n].fScale, pcState);
        else
            sprintf_s(param, sizeof(param),
                      "%s,font:%s,pos:{%d,%d},fc:{%d,%d,%d,%d},bc:{%d,%d,%d,%d},scale:%.2f,state:{%s}",
                      pStringService->GetStringName(m_pStrDescr[n].strNum), // strID
                      m_pStrDescr[n].sFontName,                             // font name
                      m_pStrDescr[n].scrPos.x, m_pStrDescr[n].scrPos.y,     // pos
                      ALPHA(m_pStrDescr[n].foreColor), RED(m_pStrDescr[n].foreColor), GREEN(m_pStrDescr[n].foreColor),
                      BLUE(m_pStrDescr[n].foreColor), ALPHA(m_pStrDescr[n].backColor), RED(m_pStrDescr[n].backColor),
                      GREEN(m_pStrDescr[n].backColor), BLUE(m_pStrDescr[n].backColor), m_pStrDescr[n].fScale, pcState);
        pIni->AddString(m_nodeName, "string", param);
    }
}

uint32_t CXI_STRCOLLECTION::MessageProc(long msgcode, MESSAGE &message)
{
    switch (msgcode)
    {
    case 0: // Add dinamic string
    {
        // string ID & data
        const std::string &paramID = message.String();
        const std::string &paramStr = message.String();
        auto *pstr = CreateNewDinamicString(paramID.c_str(), paramStr.c_str());
        if (pstr == nullptr)
            return -1;
        // string font
        const std::string &fontName = message.String();
        pstr->nFontNum = m_rs->LoadFont(fontName.c_str());
        // string pos.x&y, color front&back, alignment, shadow, scale
        pstr->scrPos.x = message.Long() + m_hostRect.left; // msg
        pstr->scrPos.y = message.Long() + m_hostRect.top;  // msg
        pstr->foreColor = message.Long();                  // msg
        pstr->backColor = message.Long();                  // msg
        switch (message.Long())                            // msg
        {
        case SCRIPT_ALIGN_RIGHT:
            pstr->wAlignment = PR_ALIGN_RIGHT;
            break;
        case SCRIPT_ALIGN_CENTER:
            pstr->wAlignment = PR_ALIGN_CENTER;
            break;
        case SCRIPT_ALIGN_LEFT:
            pstr->wAlignment = PR_ALIGN_LEFT;
            break;
        }
        pstr->bShadow = message.Long() != 0; // msg
        pstr->fScale = message.Float();      // msg
        // required width
        const int nWidth = message.Long(); // msg
        if (nWidth > 0)
        {
            const int realWidth = m_rs->StringWidth(paramStr.c_str(), pstr->nFontNum, pstr->fScale);
            if (realWidth > nWidth)
                pstr->fScale *= static_cast<float>(nWidth - 1) / realWidth;
        }
    }
    break;
    case 1: // change line by number
    {
        const auto n = message.Long() - 1;
        const std::string &param = message.String();
        ChangeString(n, param.c_str());
    }
    break;
    case 2: // copy a line from one place to another (the first line is number 1)
    {
        const auto nDst = message.Long() - 1;
        const auto nSrc = message.Long() - 1;
        if (nDst >= 0 && nSrc >= 0 && nDst < m_nStr && nSrc < m_nStr)
        {
            m_pStrDescr[nDst].strNum = -1;
            STORM_DELETE(m_pStrDescr[nDst].strStr);
            if (m_pStrDescr[nSrc].strStr)
                DublicateString(m_pStrDescr[nDst].strStr, m_pStrDescr[nSrc].strStr);
            else
                m_pStrDescr[nDst].strNum = m_pStrDescr[nSrc].strNum;
        }
    }
    break;
    case 3: // change line color
    {
        const long nStr = message.Long() - 1;
        const uint32_t nColor = message.Long();
        ChangeStringColor(nStr, nColor);
    }
    break;
    }
    return -1;
}

void CXI_STRCOLLECTION::ChangeString(long num, const char *sValue) const
{
    if (num >= 0 && num < m_nStr)
    {
        m_pStrDescr[num].strNum = -1;
        STORM_DELETE(m_pStrDescr[num].strStr);
        if (sValue)
        {
            if (sValue[0] == '#')
                DublicateString(m_pStrDescr[num].strStr, &sValue[1]);
            else
                m_pStrDescr[num].strNum = pStringService->GetStringNum(sValue);
        }
    }
}

void CXI_STRCOLLECTION::ChangeStringColor(long num, uint32_t dwColor) const
{
    if (num >= 0 && num < m_nStr)
        m_pStrDescr[num].foreColor = dwColor;
}

void CXI_STRCOLLECTION::ChangeStringPos(long num, XYPOINT &pntNewPos) const
{
    if (num >= 0 && num < m_nStr)
        m_pStrDescr[num].scrPos = pntNewPos;
}

CXI_STRCOLLECTION::STRINGDESCR *CXI_STRCOLLECTION::CreateNewDinamicString(const char *strID, const char *strStr)
{
    if (strID == nullptr || strID[0] == 0)
        return nullptr;
    int i;
    for (i = 0; i < m_nStr; i++)
    {
        if (m_pStrDescr[i].strID != nullptr && _stricmp(m_pStrDescr[i].strID, strID) == 0)
            break;
    }
    if (i < m_nStr)
    {
        if (strStr == nullptr || strStr[0] == 0)
        {
            STORM_DELETE(m_pStrDescr[i].strID);
            STORM_DELETE(m_pStrDescr[i].strStr);
            FONT_RELEASE(m_rs, m_pStrDescr[i].nFontNum);
            m_nStr--;
            if (m_nStr > i)
                memcpy(&m_pStrDescr[i], &m_pStrDescr[i + 1], sizeof(STRINGDESCR) * (m_nStr - i));
            return nullptr;
        }
        FONT_RELEASE(m_rs, m_pStrDescr[i].nFontNum);
        STORM_DELETE(m_pStrDescr[i].strStr);
        const auto len = strlen(strStr) + 1;
        m_pStrDescr[i].strStr = new char[len];
        if (m_pStrDescr[i].strStr == nullptr)
        {
            throw std::runtime_error("allocate memory error");
        }
        memcpy(m_pStrDescr[i].strStr, strStr, len);
        return &m_pStrDescr[i];
    }
    if (strStr == nullptr || strStr[0] == 0)
        return nullptr;
    STRINGDESCR *pOld = m_pStrDescr;
    m_nStr++;
    m_pStrDescr = new STRINGDESCR[m_nStr];
    if (m_pStrDescr == nullptr)
    {
        throw std::runtime_error("allocate memory error");
    }
    if (pOld && i)
        memcpy(m_pStrDescr, pOld, sizeof(STRINGDESCR) * i);
    delete pOld;
    PZERO(&m_pStrDescr[i], sizeof(STRINGDESCR));
    m_pStrDescr[i].nFontNum = -1;
    const auto len1 = strlen(strID) + 1;
    const auto len2 = strlen(strStr) + 1;
    m_pStrDescr[i].strID = new char[len1];
    m_pStrDescr[i].strStr = new char[len2];
    if (m_pStrDescr[i].strID == nullptr || m_pStrDescr[i].strStr == nullptr)
    {
        throw std::runtime_error("allocate memory error");
    }
    memcpy(m_pStrDescr[i].strID, strID, len1);
    memcpy(m_pStrDescr[i].strStr, strStr, len2);
    return &m_pStrDescr[i];
}
