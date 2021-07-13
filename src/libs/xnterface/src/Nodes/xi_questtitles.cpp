#include "xi_questtitles.h"

void SubRightWord(char *buf, int fontNum, int width, VDX9RENDER *rs)
{
    if (buf == nullptr)
        return;
    const long bufSize = strlen(buf);
    for (auto *pEnd = buf + bufSize; pEnd > buf; pEnd--)
    {
        if (*pEnd == ' ')
        {
            *pEnd = 0;
            if (rs->StringWidth(buf, fontNum) <= width)
                return;
        }
    }
}

bool CXI_QUESTTITLE::GetLineNext(int fontNum, char *&pInStr, char *buf, int bufSize) const
{
    if (pInStr == nullptr || buf == nullptr)
        return false;
    auto *const pStart = pInStr;
    auto bYesEOL = false;
    while (*pInStr != 0)
    {
        if (*pInStr == '\r' || *pInStr == '\n')
            bYesEOL = true;
        else if (bYesEOL)
            break;
        pInStr++;
    }
    const size_t lineSize = pInStr - pStart;
    if (lineSize == 0)
        return false;

    strncpy_s(buf, bufSize, pStart, lineSize);
    buf[lineSize] = 0;
    const auto strWidth = m_rs->StringWidth(buf, fontNum);
    const auto needWidth = m_rect.right - m_rect.left - m_iconWidth;
    if (strWidth <= needWidth)
        return true;

    SubRightWord(buf, fontNum, needWidth, m_rs);
    pInStr = pStart + strlen(buf);

    // remove leading spaces
    while (*pInStr != 0 && static_cast<unsigned>(*pInStr) <= ' ')
        pInStr++;

    return true;
}

CXI_QUESTTITLE::CXI_QUESTTITLE()
{
    m_nNodeType = NODETYPE_QTITLE;

    m_idFont = -1;
    m_dwNonCompleteColor = ARGB(255, 255, 255, 255);
    m_dwCompleteColor = ARGB(255, 128, 128, 128);
    m_dwSelectRectangleColor = ARGB(255, 0, 128, 128);

    m_stringQuantity = 0;
    m_allStrings = 0;

    m_strList = nullptr;
    m_curIdx = 0;
    m_selectOffset = 8;
    m_fontOffset = 4;
    m_bSelected = true;
    m_iconGroupName = nullptr;

    m_nCommonQuantity = 0;
}

CXI_QUESTTITLE::~CXI_QUESTTITLE()
{
    ReleaseAll();
}

void CXI_QUESTTITLE::Draw(bool bSelected, uint32_t Delta_Time)
{
    int i, j;
    if (!m_bUse)
        return;

    auto curY = m_rect.top;
    auto lineNum = 0;
    for (i = 0; i < m_stringQuantity && lineNum < m_allStrings; i++)
    {
        // display selection
        if (bSelected && i == m_curIdx)
        {
            XI_NOTEX_VERTEX selV[4];
            for (j = 0; j < 4; j++)
            {
                selV[j].pos.z = 1.f;
                selV[j].color = m_dwSelectRectangleColor;
            }
            selV[0].pos.x = selV[1].pos.x = static_cast<float>(m_rect.left) - m_selectOffset;
            selV[2].pos.x = selV[3].pos.x = static_cast<float>(m_rect.right);
            selV[0].pos.y = selV[2].pos.y = static_cast<float>(curY);
            selV[1].pos.y = selV[3].pos.y = static_cast<float>(curY) + m_strList[i].lineQuantity * m_vertOffset;
            m_rs->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, XI_NOTEX_FVF, 2, selV, sizeof(XI_NOTEX_VERTEX), "iRectangle");
        }

        // display progress icon
        XI_ONLYONETEX_VERTEX v[4];
        v[0].pos.z = v[1].pos.z = v[2].pos.z = v[3].pos.z = 1.f;
        v[0].pos.x = v[1].pos.x = static_cast<float>(m_rect.right) - m_iconWidth;
        v[2].pos.x = v[3].pos.x = static_cast<float>(m_rect.right);
        v[0].pos.y = v[2].pos.y = static_cast<float>(curY) + m_iconVOffset;
        v[1].pos.y = v[3].pos.y = static_cast<float>(curY) + m_iconVOffset + m_iconHeight;
        if (m_strList[i].complete)
        {
            v[0].tu = v[1].tu = m_texComplete.left;
            v[2].tu = v[3].tu = m_texComplete.right;
            v[0].tv = v[2].tv = m_texComplete.top;
            v[1].tv = v[3].tv = m_texComplete.bottom;
        }
        else
        {
            v[0].tu = v[1].tu = m_texNonComplete.left;
            v[2].tu = v[3].tu = m_texNonComplete.right;
            v[0].tv = v[2].tv = m_texNonComplete.top;
            v[1].tv = v[3].tv = m_texNonComplete.bottom;
        }
        m_rs->TextureSet(0, m_texId);
        m_rs->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, XI_ONLYONETEX_FVF, 2, v, sizeof(XI_ONLYONETEX_VERTEX),
                              "iDinamicPictures");

        // display lines
        auto curColor = m_strList[i].complete ? m_dwCompleteColor : m_dwNonCompleteColor;
        if (m_strList[i].dwSpecColor != 0)
            curColor = m_strList[i].dwSpecColor;
        for (j = 0; j < m_strList[i].lineQuantity; j++)
        {
            m_rs->ExtPrint(m_idFont, curColor, 0, PR_ALIGN_LEFT, true, 1.f, m_screenSize.x, m_screenSize.y, m_rect.left,
                           curY + m_fontOffset, "%s", m_strList[i].name[j]);
            curY += m_vertOffset;
            lineNum++;
            if (lineNum >= m_allStrings)
                break;
        }
    }
}

bool CXI_QUESTTITLE::Init(INIFILE *ini1, const char *name1, INIFILE *ini2, const char *name2, VDX9RENDER *rs,
                          XYRECT &hostRect, XYPOINT &ScreenSize)
{
    if (!CINODE::Init(ini1, name1, ini2, name2, rs, hostRect, ScreenSize))
        return false;
    SetGlowCursor(false);
    return true;
}

void CXI_QUESTTITLE::ReleaseAll()
{
    FONT_RELEASE(m_rs, m_idFont);
    if (m_strList != nullptr)
        for (auto i = 0; i < m_stringQuantity; i++)
            for (auto j = 0; j < m_strList[i].lineQuantity; j++)
                STORM_DELETE(m_strList[i].name[j]);
    STORM_DELETE(m_strList);
    STORM_DELETE(m_iconGroupName);
    m_stringQuantity = 0;
}

int CXI_QUESTTITLE::CommandExecute(int wActCode)
{
    if (m_bUse)
    {
        switch (wActCode)
        {
        case ACTION_UPSTEP:
            if (m_curIdx > 0)
                m_curIdx--;
            else
                core.Event("QuestTopChange", "l", -1);
            break;
        case ACTION_DOWNSTEP:
            if (m_curIdx < m_stringQuantity - 1)
                m_curIdx++;
            else
                core.Event("QuestTopChange", "l", 1);
            break;
        case ACTION_SPEEDUP:
            core.Event("QuestTopChange", "l", -m_allStrings);
            break;
        case ACTION_SPEEDDOWN:
            core.Event("QuestTopChange", "l", m_allStrings);
            break;
        case ACTION_MOUSECLICK:
        case ACTION_ACTIVATE:
            core.Event("QuestActivate", "l", m_curIdx);
            break;
        }
    }
    return -1;
}

bool CXI_QUESTTITLE::IsClick(int buttonID, long xPos, long yPos)
{
    if (!m_bUse)
        return false;
    if (xPos < (m_rect.left - m_selectOffset) || xPos > m_rect.right)
        return false;

    long top, bottom;
    top = m_rect.top;
    for (auto i = 0; i < m_stringQuantity; i++)
    {
        bottom = top + m_strList[i].lineQuantity * m_vertOffset;
        if (i == m_curIdx)
        {
            if (yPos >= top && yPos <= bottom)
                return true;
            return false;
        }
        top = bottom;
    }
    return false;
}

void CXI_QUESTTITLE::ChangePosition(XYRECT &rNewPos)
{
    m_rect = rNewPos;
}

void CXI_QUESTTITLE::SaveParametersToIni()
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

void CXI_QUESTTITLE::LoadIni(INIFILE *ini1, const char *name1, INIFILE *ini2, const char *name2)
{
    char param[255];

    //
    m_selectOffset = GetIniLong(ini1, name1, ini2, name2, "selectOffset", 8);

    // get line space
    m_vertOffset = GetIniLong(ini1, name1, ini2, name2, "lineSpace", 30);
    if (m_vertOffset == 0)
        m_vertOffset = 10;

    // counting the number of lines displayed on the screen
    m_allStrings = (m_rect.bottom - m_rect.top) / m_vertOffset;

    // get golors
    m_dwCompleteColor = GetIniARGB(ini1, name1, ini2, name2, "completeColor", ARGB(255, 128, 128, 128));
    m_dwNonCompleteColor = GetIniARGB(ini1, name1, ini2, name2, "noncompleteColor", ARGB(255, 255, 255, 255));
    m_dwSelectRectangleColor = GetIniARGB(ini1, name1, ini2, name2, "selectColor", ARGB(255, 255, 255, 255));

    // get font
    m_idFont = -1;
    if (ReadIniString(ini1, name1, ini2, name2, "font", param, sizeof(param), ""))
        m_idFont = m_rs->LoadFont(param);
    m_fontOffset = GetIniLong(ini1, name1, ini2, name2, "fontOffset", 4);

    // get image info
    m_iconWidth = GetIniLong(ini1, name1, ini2, name2, "iconWidth", 32);
    m_iconHeight = GetIniLong(ini1, name1, ini2, name2, "iconHeight", 32);
    m_iconVOffset = m_vertOffset / 2 - m_iconHeight / 2;
    if (ReadIniString(ini1, name1, ini2, name2, "iconGroup", param, sizeof(param), ""))
    {
        const auto len = strlen(param) + 1;
        m_iconGroupName = new char[len];
        if (m_iconGroupName == nullptr)
        {
            throw std::runtime_error("allocate memory error");
        }
        memcpy(m_iconGroupName, param, len);
    }
    else
        m_iconGroupName = nullptr;
    m_texId = ptrOwner->PictureService()->GetTextureID(m_iconGroupName);

    if (ReadIniString(ini1, name1, ini2, name2, "completeIcon", param, sizeof(param), ""))
    {
        ptrOwner->PictureService()->GetTexturePos(m_iconGroupName, param, m_texComplete);
    }
    else
    {
        m_texComplete.left = m_texComplete.top = 0.f;
        m_texComplete.right = m_texComplete.bottom = 1.f;
    }

    if (ReadIniString(ini1, name1, ini2, name2, "noncompleteIcon", param, sizeof(param), ""))
    {
        ptrOwner->PictureService()->GetTexturePos(m_iconGroupName, param, m_texNonComplete);
    }
    else
    {
        m_texNonComplete.left = m_texNonComplete.top = 0.f;
        m_texNonComplete.right = m_texNonComplete.bottom = 1.f;
    }
}

void CXI_QUESTTITLE::SetNewTopQuest(ATTRIBUTES *pA, int topNum)
{
    int i;
    m_nCommonQuantity = 0;
    // delete old lines
    if (m_strList != nullptr)
    {
        for (i = 0; i < m_stringQuantity; i++)
            for (auto j = 0; j < m_strList[i].lineQuantity; j++)
                STORM_DELETE(m_strList[i].name[j]);
        STORM_DELETE(m_strList);
        m_stringQuantity = 0;
    } // boal moved up, otherwise it does not delete if there are no quests, but there were already

    if (pA == nullptr)
        return;
    const long aq = pA->GetAttributesNum();
    if (topNum < 0 || topNum >= aq)
    {
        core.Trace("quest number out of range");
        return;
    }
    m_nCommonQuantity = aq;

    std::string param;
    // if(ptrOwner->QuestFileReader())
    {
        // ptrOwner->QuestFileReader()->InitQuestsQuery();

        // calculating the number of lines to display
        m_stringQuantity = aq - topNum;
        if (m_stringQuantity > m_allStrings)
            m_stringQuantity = m_allStrings;
        if (m_curIdx >= m_stringQuantity)
            m_curIdx = m_stringQuantity - 1;

        // creating an array of strings
        if (m_stringQuantity <= 0)
            return;
        if ((m_strList = new STRING_DESCRIBER[m_stringQuantity]) == nullptr)
        {
            throw std::runtime_error("allocate memory error");
        }
        // and filling these strings
        auto lineNum = 0;
        for (i = 0; i < m_stringQuantity; i++)
        {
            m_strList[i].lineQuantity = 0;
            m_strList[i].dwSpecColor = 0;
            auto *pAttr = pA->GetAttributeClass(topNum + i);
            if (pAttr == nullptr)
            {
                m_strList[i].complete = false;
                m_strList[i].lineQuantity = 0;
                continue;
            }
            m_strList[i].dwSpecColor = pAttr->GetAttributeAsDword("color", 0);
            m_strList[i].complete = pAttr->GetAttributeAsDword("Complete", 0) != 0;
            const char *pTmpQuestRecordID = pAttr->GetAttribute("LogName");
            if (!pTmpQuestRecordID)
                pTmpQuestRecordID = pAttr->GetThisName();
            if (ptrOwner->QuestFileReader()->GetQuestTitle(pTmpQuestRecordID, pAttr->GetThisName(), param))
            {
                const size_t titleSize = param.size();
                if (titleSize == 0)
                    m_strList[i].lineQuantity = 0;
                else
                {
                    std::string lineName(titleSize, '\0');
                    auto *pstr = param.data();
                    int ln = 0;
                    /// @todo There is no need to get size here, instead there
                    /// will be better to use std::vector in GetLineNext
                    while (GetLineNext(m_idFont, pstr, lineName.data(), lineName.size()))
                    {
                        const auto len = lineName.size() + 1;
                        m_strList[i].name[ln] = new char[len];
                        memcpy(m_strList[i].name[ln], lineName.data(), len);
                        ln++;
                    }
                    m_strList[i].lineQuantity = ln;
                    lineNum += ln;
                    if (lineNum >= m_allStrings)
                        m_stringQuantity = i + 1;
                }
            }
        }

        // ptrOwner->QuestFileReader()->CloseQuestsQuery();
    }
}

float CXI_QUESTTITLE::GetLineStep() const
{
    if (m_nCommonQuantity <= 0)
        return 0.f;
    return 1.f / static_cast<float>(m_nCommonQuantity);
}

void CXI_QUESTTITLE::ScrollerChanged(float fPos)
{
}

void CXI_QUESTTITLE::MouseThis(float fX, float fY)
{
    const float left = static_cast<float>(m_rect.left) - m_selectOffset;
    const auto right = static_cast<float>(m_rect.right);
    if (fX < left || fX > right)
        return;

    float top, bottom;
    top = static_cast<float>(m_rect.top);
    for (int i = 0; i < m_stringQuantity; i++)
    {
        bottom = top + m_strList[i].lineQuantity * m_vertOffset;
        if (fY >= top && fY <= bottom)
        {
            if (i != m_curIdx)
                m_curIdx = i;
            break;
        }
        top = bottom;
    }
}
