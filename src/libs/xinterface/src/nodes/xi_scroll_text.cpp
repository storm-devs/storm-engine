#include "xi_scroll_text.h"

#define MAX_PICE_RESERV 256
#define MAX_PICE_STRING_SIZE 256

CXI_SCROLLTEXT::CXI_SCROLLTEXT()
    : m_idFont(0)
{
    m_pScroller = nullptr;
    m_nMaxStringes = 0;
    m_dwFontColor = ARGB(255, 255, 255, 255);
    m_pText = nullptr;

    m_nPiceQuantity = 0;
    m_pStrList = nullptr;
    m_nNodeType = NODETYPE_SCROLLTEXT;
}

CXI_SCROLLTEXT::~CXI_SCROLLTEXT()
{
    ReleaseAll();
}

void CXI_SCROLLTEXT::Draw(bool bSelected, uint32_t Delta_Time)
{
    char param[MAX_PICE_STRING_SIZE];
    const auto curX = 0L;
    const auto curY = 0L;
    if (m_bUse)
    {
        for (auto i = 0; i < m_nPiceQuantity; i++)
        {
            switch (m_pStrList[i].type)
            {
            case PICE_TYPE_STRING: {
                const auto chStart = m_pStrList[i].data.strDescr.startOffset;
                auto chQuant = m_pStrList[i].data.strDescr.charQuantity;
                if (chQuant > MAX_PICE_STRING_SIZE - 1)
                    chQuant = MAX_PICE_STRING_SIZE - 1;
                strncpy_s(param, &m_pText[chStart], chQuant);
                param[chQuant] = 0;
                m_rs->Print(curX, curY, "%s", param);
            }
            break;
            }
        }
    }
}

bool CXI_SCROLLTEXT::Init(INIFILE *ini1, const char *name1, INIFILE *ini2, const char *name2, VDX9RENDER *rs,
                          XYRECT &hostRect, XYPOINT &ScreenSize)
{
    if (!CINODE::Init(ini1, name1, ini2, name2, rs, hostRect, ScreenSize))
        return false;
    SetText("proba string ^fnormal continue proba string one, two, three etc. Continue next stringes from full screen "
            "filling");
    SetGlowCursor(false);
    return true;
}

void CXI_SCROLLTEXT::ReleaseAll()
{
    ClearText();
}

void CXI_SCROLLTEXT::ClearText()
{
    m_pScroller = nullptr;
    m_nMaxStringes = 0;
    m_dwFontColor = ARGB(255, 255, 255, 255);
    STORM_DELETE(m_pText);

    m_nPiceQuantity = 0;
    STORM_DELETE(m_pStrList);
}

void CXI_SCROLLTEXT::SetText(const char *newText)
{
    char param[256];
    STRING_PICE spl[MAX_PICE_RESERV];
    ClearText();

    if (newText != nullptr)
    {
        const auto len = strlen(newText) + 1;
        if ((m_pText = new char[len]) == nullptr)
            throw std::runtime_error("allocate memory error");
        memcpy(m_pText, newText, len);

        // get all parts from text
        auto *pCh = m_pText;
        m_idFont = FONT_DEFAULT;
        m_dwFontColor = ARGB(255, 255, 255, 255);
        int32_t idx = 0;
        int32_t nBeg = 0;
        int32_t nEnd = 0;
        int32_t startWidth = 0;
        while (*pCh != 0)
        {
            if (*pCh == '^')
            {
                // place already filled text in a separate piece
                if (nBeg != nEnd)
                    startWidth = FillPices(m_pText, nBeg, nEnd - nBeg, idx, spl, startWidth);

                idx++;
                if (nBeg == nEnd)
                    nBeg += 3;
                nEnd += 2;
                pCh++;
                switch (*pCh)
                {
                case 'f':
                    spl[idx].type = PICE_TYPE_FONTCHANGE;
                    GetStringWord(&pCh[1], param, sizeof(param) - 1);
                    spl[idx].data.fontID = m_idFont = m_rs->LoadFont(param);
                    break;
                case 'c':
                    break;
                case 'a':
                    break;
                }
                idx++;
            }
        }
    }
}

int CXI_SCROLLTEXT::CommandExecute(int wActCode)
{
    return -1;
}

bool CXI_SCROLLTEXT::IsClick(int buttonID, int32_t xPos, int32_t yPos)
{
    return false;
}

void CXI_SCROLLTEXT::ChangePosition(XYRECT &rNewPos)
{
}

void CXI_SCROLLTEXT::SaveParametersToIni()
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

void CXI_SCROLLTEXT::LoadIni(INIFILE *ini1, const char *name1, INIFILE *ini2, const char *name2)
{
    m_pScroller = nullptr;
    m_nMaxStringes = 0;
    m_dwFontColor = ARGB(255, 255, 255, 255);
    m_pText = nullptr;

    m_nPiceQuantity = 0;
    m_pStrList = nullptr;
}

void CXI_SCROLLTEXT::TextDownShift(float)
{
}

void CXI_SCROLLTEXT::TextUpShift()
{
}

int32_t CXI_SCROLLTEXT::FillPices(char *pt, size_t beg, size_t size, int32_t &idx, STRING_PICE *spl, int32_t wid)
{
    int32_t retVal = 0;

    char param[256];
    char resStr[512];
    auto showWidth = m_rect.right - m_rect.left - wid;
    if (showWidth < 0)
        showWidth = m_rect.right - m_rect.left;

    pt[size] = 0; // denote the end of the line
    while (true)
    {
        auto *const pstr = &pt[beg];
        int32_t chQuantity = 0;
        resStr[0] = 0;
        while (true)
        {
            const auto sw = GetStringWord(&pstr[chQuantity], param, sizeof(param) - 1);
            if (sw == 0)
                break;
            chQuantity += sw;
            strncpy_s(resStr, pstr, chQuantity);
            if ((retVal = m_rs->StringWidth(resStr, m_idFont)) > showWidth)
            {
                retVal = 0;
                break;
            }
            if (static_cast<uint32_t>(chQuantity + sw) > size)
                break;
        }
        if (static_cast<uint32_t>(chQuantity) == size)
        {
        }
        else
            break;
        size -= chQuantity;
        spl[idx].type = PICE_TYPE_STRING;
        spl[idx].data.strDescr.startOffset = beg;
    }

    return 0;
}

int32_t CXI_SCROLLTEXT::GetStringWord(char *pstr, char *buff, size_t size)
{
    uint32_t retVal = 0;

    auto bSpace = false;
    while (*pstr != 0)
    {
        if (*pstr == '^')
            break;
        if (retVal >= size)
            break;
        if (*pstr == 32)
            bSpace = true;
        else if (bSpace)
            break;

        *(buff++) = *(pstr++);
        retVal++;
    }

    if (retVal >= size)
    {
        size -= utf8::u8_dec(buff + size - 1);
        buff[size] = 0;
    }
    else
        buff[retVal] = 0;
    return retVal;
}
