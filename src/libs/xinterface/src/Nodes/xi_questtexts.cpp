#include "xi_questtexts.h"

CXI_QUESTTEXTS::STRING_DESCRIBER::STRING_DESCRIBER(const char *ls)
{
    const auto len = strlen(ls) + 1;
    if (len == 1)
        lineStr = nullptr;
    else
    {
        if ((lineStr = new char[len]) == nullptr)
        {
            throw std::runtime_error("allocate memory error");
        }
        memcpy(lineStr, ls, len);
    }
    complete = false;
    next = nullptr;
    strNum = 0;
    prev = nullptr;
}

CXI_QUESTTEXTS::STRING_DESCRIBER *CXI_QUESTTEXTS::STRING_DESCRIBER::Add(const char *ls, bool complete)
{
    if (ls == nullptr)
        return nullptr;
    auto *newSD = new STRING_DESCRIBER(ls);
    if (newSD == nullptr)
    {
        throw std::runtime_error("allocate memory error");
    }

    newSD->complete = complete;
    newSD->prev = this;
    newSD->next = next;
    next = newSD;
    newSD->strNum = strNum + 1;

    return newSD;
}

bool GetNextIdFromList(char *&sptr, char *bufQuestID, size_t nSizeBufQuestID, char *buf, size_t bufSize, char *dataBuf)
{
    if (sptr == nullptr)
        return false;
    auto *sstart = sptr;
    long idSize = 0;
    while (*sptr != 0)
    {
        if (*sptr == ',')
        {
            sptr++;
            break;
        }
        sptr++;
        idSize++;
    }
    if (idSize == 0)
        return false;
    if (sstart[0] == '@')
    {
        sstart++;
        idSize--;
        int n;
        if (sstart[0] < '0' || sstart[0] > '9')
        {
            // not a date
            n = 0;
            while (sstart[0] && sstart[0] != '@')
            {
                if (n < static_cast<long>(nSizeBufQuestID))
                    bufQuestID[n++] = sstart[0];
                sstart++;
                idSize--;
            }
            bufQuestID[n] = '\0';
            sstart++;
            idSize--;
        }
        else
        {
            bufQuestID[0] = '\0';
        }

        // date
        n = 0;
        while (sstart[0] && sstart[0] != '@')
        {
            dataBuf[n] = sstart[0];
            sstart++;
            idSize--;
            n++;
        }
        dataBuf[n] = '\0';
        sstart++;
        idSize--;
    }
    else
    {
        dataBuf[0] = '\0';
        bufQuestID[0] = '\0';
    }
    if (idSize == 0)
        return false;
    if (static_cast<size_t>(idSize) >= bufSize)
        idSize = bufSize - 1;
    strncpy_s(buf, bufSize, sstart, idSize);
    buf[idSize] = 0;
    return true;
}

static void SubRightWord(char *buf, int fontNum, int width, VDX9RENDER *rs)
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

void CXI_QUESTTEXTS::ReleaseStringes()
{
    while (m_listRoot != nullptr)
    {
        m_listCur = m_listRoot;
        m_listRoot = m_listRoot->next;
        STORM_DELETE(m_listCur->lineStr);
        STORM_DELETE(m_listCur);
    }
    m_listCur = nullptr;
    m_nAllTextStrings = 0;
}

bool CXI_QUESTTEXTS::GetLineNext(int fontNum, char *&pInStr, char *buf, int bufSize) const
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
    size_t lineSize = pInStr - pStart;
    if (lineSize == 0)
        return false;
    if (lineSize > bufSize - 1)
    {
        // rewind back to the last utf8 char that fits in the buffer
        lineSize = bufSize - 1 - utf8::u8_dec(pStart + bufSize - 1);
    }

    strncpy_s(buf, bufSize, pStart, lineSize);
    buf[lineSize] = 0;
    const auto strWidth = m_rs->StringWidth(buf, fontNum);
    if (strWidth <= m_rect.right - m_rect.left)
        return true;

    SubRightWord(buf, fontNum, m_rect.right - m_rect.left, m_rs);
    pInStr = pStart + strlen(buf);

    // remove leading spaces
    while (*pInStr != 0 && static_cast<unsigned>(*pInStr) <= ' ')
        pInStr++;

    return true;
}

CXI_QUESTTEXTS::CXI_QUESTTEXTS()
    : m_vertOffset(0)
{
    m_nNodeType = NODETYPE_QTEXTS;

    m_idFont = -1;
    m_dwNonCompleteColor = ARGB(255, 255, 255, 255);
    m_dwCompleteColor = ARGB(255, 128, 128, 128);

    m_allStrings = 0;

    m_nAllTextStrings = 0;
    m_listCur = m_listRoot = nullptr;
}

CXI_QUESTTEXTS::~CXI_QUESTTEXTS()
{
    ReleaseAll();
}

void CXI_QUESTTEXTS::Draw(bool bSelected, uint32_t Delta_Time)
{
    if (!m_bUse)
        return;

    auto curY = m_rect.top;
    auto i = 0;
    for (auto *sd = m_listCur; sd != nullptr && i < m_allStrings; sd = sd->next, i++)
    {
        // display lines
        const auto curColor = sd->complete ? m_dwCompleteColor : m_dwNonCompleteColor;
        if (sd->lineStr != nullptr && sd->lineStr[0] != 0)
            m_rs->ExtPrint(m_idFont, curColor, 0, PR_ALIGN_LEFT, true, 1.f, m_screenSize.x, m_screenSize.y, m_rect.left,
                           curY, "%s", sd->lineStr);
        curY += m_vertOffset;
    }
}

bool CXI_QUESTTEXTS::Init(INIFILE *ini1, const char *name1, INIFILE *ini2, const char *name2, VDX9RENDER *rs,
                          XYRECT &hostRect, XYPOINT &ScreenSize)
{
    if (!CINODE::Init(ini1, name1, ini2, name2, rs, hostRect, ScreenSize))
        return false;
    SetGlowCursor(false);
    return true;
}

void CXI_QUESTTEXTS::ReleaseAll()
{
    FONT_RELEASE(m_rs, m_idFont);
    ReleaseStringes();
}

int CXI_QUESTTEXTS::CommandExecute(int wActCode)
{
    if (m_bUse)
    {
        if (m_listCur == nullptr)
            return -1;
        switch (wActCode)
        {
        case ACTION_UPSTEP:
            if (m_listCur->prev != nullptr)
                m_listCur = m_listCur->prev;
            break;
        case ACTION_DOWNSTEP:
            if (m_listCur->next != nullptr)
                m_listCur = m_listCur->next;
            break;
        case ACTION_SPEEDUP: {
            for (auto i = 0; i < m_allStrings; i++)
            {
                if (m_listCur->prev == nullptr)
                    break;
                m_listCur = m_listCur->prev;
            }
        }
        break;
        case ACTION_SPEEDDOWN: {
            for (auto i = 0; i < m_allStrings; i++)
            {
                if (m_listCur->next == nullptr)
                    break;
                m_listCur = m_listCur->next;
            }
        }
        break;
        }
        core.Event("SetScrollerPos", "sf", m_nodeName,
                   static_cast<float>(m_listCur->strNum) / static_cast<float>(m_nAllTextStrings));
    }
    return -1;
}

bool CXI_QUESTTEXTS::IsClick(int buttonID, long xPos, long yPos)
{
    return false;
}

void CXI_QUESTTEXTS::ChangePosition(XYRECT &rNewPos)
{
    m_rect = rNewPos;
}

void CXI_QUESTTEXTS::SaveParametersToIni()
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

void CXI_QUESTTEXTS::LoadIni(INIFILE *ini1, const char *name1, INIFILE *ini2, const char *name2)
{
    char param[255];

    // get line space
    m_vertOffset = GetIniLong(ini1, name1, ini2, name2, "lineSpace", 30);
    if (m_vertOffset == 0)
        m_vertOffset = 10;

    // counting the number of lines displayed on the screen
    m_allStrings = (m_rect.bottom - m_rect.top) / m_vertOffset;

    // get colors
    m_dwCompleteColor = GetIniARGB(ini1, name1, ini2, name2, "completeColor", ARGB(255, 128, 128, 128));
    m_dwNonCompleteColor = GetIniARGB(ini1, name1, ini2, name2, "noncompleteColor", ARGB(255, 255, 255, 255));

    // get font
    m_idFont = -1;
    if (ReadIniString(ini1, name1, ini2, name2, "font", param, sizeof(param), ""))
        m_idFont = m_rs->LoadFont(param);
}

void CXI_QUESTTEXTS::StartQuestShow(ATTRIBUTES *pA, int qn)
{
    if (pA == nullptr)
        return;
    const long aq = pA->GetAttributesNum();
    if (qn < 0)
        qn = 0;
    if (qn >= aq)
        qn = aq - 1;

    // delete old lines
    while (m_listRoot != nullptr)
    {
        m_listCur = m_listRoot;
        m_listRoot = m_listRoot->next;
        STORM_DELETE(m_listCur->lineStr);
        STORM_DELETE(m_listCur);
    }

    auto *pAttr = pA->GetAttributeClass(qn);
    if (pAttr == nullptr)
        return;

    const auto cFlag = pAttr->GetAttributeAsDword("Complete", 0) != 0;
    auto *pATextList = pAttr->GetAttributeClass("Text");
    const char *questLogName = pAttr->GetAttribute("LogName");
    if (!questLogName)
        questLogName = pAttr->GetThisName();

    std::vector<std::string> asStringList;
    if (ptrOwner->QuestFileReader() && pATextList)
    {
        const long q = pATextList->GetAttributesNum();
        for (auto n = q - 1; n >= 0; n--)
        {
            ATTRIBUTES *pAttr = pATextList->GetAttributeClass(n);
            if (!pAttr)
                continue;

            // space for aparting records
            if (m_listCur != nullptr)
                m_listCur = m_listCur->Add("", cFlag);

            // whole string list for one record
            asStringList.clear();
            GetStringListForQuestRecord(asStringList, pAttr->GetThisAttr(), pAttr->GetAttribute("UserData"));
            for (long i = 0; i < asStringList.size(); i++)
            {
                // decompose the resulting string into lines that fit into the output area
                auto pcStrPtr = (char *)asStringList[i].c_str();
                char newStr[512];
                while (GetLineNext(m_idFont, pcStrPtr, newStr, sizeof(newStr)))
                {
                    if (m_listCur == nullptr)
                    {
                        if ((m_listCur = m_listRoot = new STRING_DESCRIBER(newStr)) == nullptr)
                        {
                            throw std::runtime_error("allocate memory error");
                        }
                        m_listCur->complete = cFlag;
                    }
                    else
                        m_listCur = m_listCur->Add(newStr, cFlag);
                }
            }
        }
    }
    m_nAllTextStrings = m_listCur == nullptr ? 0 : m_listCur->strNum;
    m_listCur = m_listRoot;
    core.Event("SetScrollerPos", "sf", m_nodeName, 0.f);
}

float CXI_QUESTTEXTS::GetLineStep() const
{
    if (m_nAllTextStrings < 1)
        return 0.f;
    return 1.f / static_cast<float>(m_nAllTextStrings);
}

void CXI_QUESTTEXTS::ScrollerChanged(float fPos)
{
    if (m_listRoot == nullptr || m_nAllTextStrings <= 0)
        return;
    long topStrNum = static_cast<long>(fPos * m_nAllTextStrings);
    if (topStrNum > m_nAllTextStrings)
        topStrNum = m_nAllTextStrings;
    if (topStrNum < 0)
        topStrNum = 0;
    m_listCur = m_listRoot;
    for (int i = 0; i < topStrNum; i++)
        if (m_listCur->next != nullptr)
            m_listCur = m_listCur->next;
}

uint32_t CXI_QUESTTEXTS::MessageProc(long msgcode, MESSAGE &message)
{
    switch (msgcode)
    {
    case 0: // set the top display line to a specific number
    {
        long topStrNum = message.Long();
        if (m_listRoot == nullptr || m_nAllTextStrings <= 0)
            return 0;
        if (topStrNum >= m_nAllTextStrings)
            topStrNum = m_nAllTextStrings - 1;
        if (topStrNum < 0)
            topStrNum = 0;
        m_listCur = m_listRoot;
        for (int i = 0; i < topStrNum; i++)
            if (m_listCur != nullptr)
                m_listCur = m_listCur->next;
        return 0;
    }
    break;

    case 1: // set the top display line to a relative number
    {
        ScrollerChanged(message.Float());
        return 0;
    }
    break;
    }
    return -1;
}

void CXI_QUESTTEXTS::GetStringListForQuestRecord(std::vector<std::string> &stringList, const char *recText,
                                                 const char *userData) const
{
    char pcQuestID[1024];
    char pcTextID[1024];
    char pcDate[256];

    auto pcTmp = (char *)recText;

    if (GetNextIdFromList(pcTmp, pcQuestID, sizeof(pcQuestID), pcTextID, sizeof(pcTextID) - 1, pcDate))
    {
        stringList.clear();
        if (pcDate[0])
            stringList.push_back(pcDate);
        stringList.push_back(ptrOwner->QuestFileReader()->GetRecordText(pcQuestID, pcTextID, userData ? userData : ""));
    }
}
