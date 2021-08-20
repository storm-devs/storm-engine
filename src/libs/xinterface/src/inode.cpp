#include "inode.h"
#include <cstdarg>

#include "core.h"

CINODE::CINODE()
{
    m_bMouseWeelReaction = false;
    m_bLockedNode = false;
    m_nDoDelay = 0;
    m_nCurrentCommandNumber = -1;
    m_bUse = true;
    m_next = nullptr;
    m_list = nullptr;
    m_bClickable = false;
    m_bSelected = false;
    m_bLockStatus = false;
    m_bBreakPress = false;
    m_bMouseSelect = false;
    m_nodeName = nullptr;
    PZERO(m_pCommands, sizeof(COMMAND_ACTION) * COMMAND_QUANTITY);
    m_bShowGlowCursor = true;
    m_strHelpTextureFile = nullptr;
    m_bUseUserGlowCursor = false;
    m_bUseUserGlowOffset = false;
    m_bInProcessingMessageForThisNode = false;
    m_bDeleting = false;
    m_pToolTip = nullptr;
    m_bMakeActionInDeclick = false;
}

CINODE::~CINODE()
{
    STORM_DELETE(m_strHelpTextureFile);

    STORM_DELETE(m_nodeName);

    if (m_list)
    {
        m_list->ReleaseAll();
        delete m_list;
    }

    for (auto i = 0; i < COMMAND_QUANTITY; i++)
    {
        STORM_DELETE(m_pCommands[i].sRetControl);
        STORM_DELETE(m_pCommands[i].sEventName);

        auto *pContrl = m_pCommands[i].pNextControl;
        while (pContrl != nullptr)
        {
            auto *const pOld = pContrl;
            pContrl = pContrl->next;
            delete pOld;
        }
        m_pCommands[i].pNextControl = nullptr;
    }
    STORM_DELETE(m_pToolTip);
}

void CINODE::FrameProcess(uint32_t DeltaTime)
{
    if (m_nCurrentCommandNumber != -1)
    {
        m_nDoDelay -= DeltaTime;
        if (m_nDoDelay < 0)
            m_nDoDelay = 0;

        if (m_nDoDelay == 0)
        {
            // redirect command to subnodes
            auto *pContrl = m_pCommands[m_nCurrentCommandNumber].pNextControl;
            while (pContrl != nullptr)
            {
                if (pContrl->sControlName)
                {
                    auto *pTmpNod = ptrOwner->FindNode(pContrl->sControlName, nullptr);
                    if (pTmpNod)
                        pTmpNod->CommandExecute(pContrl->command);
                }
                pContrl = pContrl->next;
            }

            if (m_pCommands[m_nCurrentCommandNumber].sEventName != nullptr)
                core.Send_Message(g_idInterface, "lssl", MSG_INTERFACE_SET_EVENT,
                                  m_pCommands[m_nCurrentCommandNumber].sEventName, m_nodeName, m_nCurrentCommandNumber);

            if (m_pCommands[m_nCurrentCommandNumber].sRetControl)
            {
                auto *const pTmpNod = ptrOwner->FindNode(m_pCommands[m_nCurrentCommandNumber].sRetControl, nullptr);
                if (pTmpNod)
                    core.Send_Message(g_idInterface, "lp", MSG_INTERFACE_SET_CURRENT_NODE, pTmpNod);
            }

            m_nCurrentCommandNumber = -1;
        }
    }

    // tooltip update
    if (m_pToolTip && m_bUse)
    {
        m_pToolTip->Draw();
    }
}

CINODE *CINODE::DoAction(int wActCode, bool &bBreakPress, bool bFirstPress)
{
    if (m_nNodeType == NODETYPE_TEXTBUTTON && !m_bSelected)
        return nullptr;
    bBreakPress = m_bBreakPress;
    if (m_bLockStatus)
        return this;

    int i;
    for (i = 0; i < COMMAND_QUANTITY; i++)
        if (pCommandsList[i].code == wActCode)
            break;
    if (i == COMMAND_QUANTITY)
        return this;

    auto n = i;
    if (m_pCommands[i].bUse)
    {
        // if(m_pCommands[i].nSound!=0)
        if (bFirstPress)
            core.Event(ISOUND_EVENT, "l", 1);
        // core.Event(ISOUND_EVENT,"l",m_pCommands[i].nSound);
        // execute command
        while (n != COMMAND_QUANTITY)
        {
            const auto ac = CommandExecute(pCommandsList[n].code);
            if (ac == -1)
                break;

            for (n = 0; n < COMMAND_QUANTITY; n++)
                if (pCommandsList[n].code == ac)
                    break;
        }
        m_nDoDelay = m_pCommands[i].nActionDelay;
        if (n < COMMAND_QUANTITY)
        {
            core.Event("ievnt_command", "ss", pCommandsList[n].sName, m_nodeName);
            m_nCurrentCommandNumber = n;
        }
        else
        {
            core.Event("ievnt_command", "ss", pCommandsList[i].sName, m_nodeName);
            m_nCurrentCommandNumber = i;
        }
    }

    FrameProcess(0);

    // return other or self node
    if (m_nCurrentCommandNumber == -1)
    {
        if (n < COMMAND_QUANTITY)
            return ptrOwner->FindNode(m_pCommands[n].sRetControl, nullptr);
        return ptrOwner->FindNode(m_pCommands[i].sRetControl, nullptr);
    }
    return nullptr;
}

CINODE *CINODE::FindNode(CINODE *pNod, const char *sNodName)
{
    if (!sNodName)
        return nullptr;
    while (pNod)
    {
        if (pNod->m_nodeName && _stricmp(sNodName, pNod->m_nodeName) == 0)
            break;
        if (pNod->m_list)
        {
            auto *const pInsideNod = FindNode(pNod->m_list, sNodName);
            if (pInsideNod)
                return pInsideNod;
        }
        pNod = pNod->m_next;
    }
    return pNod;
}

CINODE *CINODE::FindNode(CINODE *pNod, int nNodType)
{
    while (pNod)
    {
        if (pNod->m_nNodeType == nNodType)
            break;
        if (pNod->m_list)
        {
            auto *const pInsideNod = FindNode(pNod->m_list, nNodType);
            if (pInsideNod)
                return pInsideNod;
        }
        pNod = pNod->m_next;
    }
    return pNod;
}

CINODE *CINODE::FindNode(CINODE *pNod, float x, float y)
{
    while (pNod)
    {
        if ((x >= pNod->m_rect.left) && (x <= pNod->m_rect.right) && (y >= pNod->m_rect.top) &&
            (y <= pNod->m_rect.bottom))
            break;
        if (pNod->m_list)
        {
            auto *const pInsideNod = FindNode(pNod->m_list, x, y);
            if (pInsideNod)
                return pInsideNod;
        }
        pNod = pNod->m_next;
    }
    return pNod;
}

/*CINODE*    CINODE::FindNode(const char* sNodName)
{
    if( sNodName!=null && _stricmp(sNodName,m_nodeName)==0 )
        return this;
    CINODE* retVal;
    if( m_list!=null )
        if( (retVal=m_list->FindNode(sNodName))!=null ) return retVal;
    if( m_next!=null ) return m_next->FindNode(sNodName);
    return null;
}

CINODE* CINODE::FindNode(int nNodType)
{
    if(m_nNodeType==nNodType)
        return this;
    CINODE* retVal;
    if( m_list!=null )
        if( (retVal=m_list->FindNode(nNodType))!=null ) return retVal;
    if( m_next!=null ) return m_next->FindNode(nNodType);
    return null;
}

CINODE* CINODE::FindNode(float x,float y)
{
    if( !m_bLockedNode )
    {
        if( (x>=m_rect.left) && (x<=m_rect.right) && (y>=m_rect.top) && (y<=m_rect.bottom) )
            return this;
        CINODE* retVal;
        if( m_list!=null )
            if( (retVal=m_list->FindNode(x,y))!=null ) return retVal;
    }
    if( m_next!=null ) return m_next->FindNode(x,y);
    return null;
}*/

void CINODE::GetRelativeRect(XYRECT &rect) const
{
    rect.left += m_hostRect.left;
    rect.top += m_hostRect.top;
    rect.right += m_hostRect.left;
    rect.bottom += m_hostRect.top;
}

void CINODE::GetAbsoluteRect(XYRECT &rect, int at) const
{
    if (!(at & ABSOLUTE_LEFT))
        rect.left += m_hostRect.left;
    if (at & ABSOLUTE_RIGHT)
        rect.right += m_screenSize.x - m_hostRect.right + m_hostRect.left;
    else
        rect.right += m_hostRect.left;

    if (!(at & ABSOLUTE_TOP))
        rect.top += m_hostRect.top;
    if (at & ABSOLUTE_BOTTOM)
        rect.bottom += m_screenSize.y - m_hostRect.bottom + m_hostRect.top;
    else
        rect.bottom += m_hostRect.top;
}

void CINODE::GetAbsoluteRectForSave(XYRECT &rect, int at) const
{
    if (!(at & ABSOLUTE_LEFT))
        rect.left -= m_hostRect.left;
    if (at & ABSOLUTE_RIGHT)
        rect.right -= m_screenSize.x - m_hostRect.right + m_hostRect.left;
    else
        rect.right -= m_hostRect.left;

    if (!(at & ABSOLUTE_TOP))
        rect.top -= m_hostRect.top;
    if (at & ABSOLUTE_BOTTOM)
        rect.bottom -= m_screenSize.y - m_hostRect.bottom + m_hostRect.top;
    else
        rect.bottom -= m_hostRect.top;
}

const char *CINODE::GetSubStr(const char *inStr, char *buf, size_t bufSize, char devChar)
{
    if (bufSize <= 0 || buf == nullptr)
        return inStr;
    if (inStr == nullptr)
    {
        buf[0] = 0;
        return nullptr;
    }
    auto curSize = 0;
    const char *curStr;
    for (curStr = inStr; *curStr != 0; curStr++)
    {
        if (*curStr == ' ' && curSize == 0)
            continue;
        if (*curStr == devChar || *curStr == 0)
            break;
        if (curSize < static_cast<int>(bufSize) - 1)
            buf[curSize++] = *curStr;
    }
    buf[curSize] = 0;
    if (*curStr == devChar)
        curStr++;
    return curStr;
}

bool CINODE::GetMidStr(const char *inStr, char *buf, size_t bufSize, const char *begStr, const char *endStr)
{
    if (bufSize <= 0 || buf == nullptr)
        return false;
    if (inStr == nullptr || begStr == nullptr || endStr == nullptr)
    {
        buf[0] = 0;
        return false;
    }
    const int lenIn = strlen(inStr);
    const int lenBeg = strlen(begStr);
    const int lenEnd = strlen(endStr);

    int i;
    auto fcn = -1, lcn = -1;
    for (i = 0; i <= lenIn - lenBeg; i++)
        if (_strnicmp(&inStr[i], begStr, lenBeg) == 0)
        {
            fcn = i;
            break;
        }
    if (fcn < 0)
    {
        buf[0] = 0;
        return false;
    }
    fcn += lenBeg;

    for (i = fcn; i <= lenIn - lenEnd; i++)
        if (_strnicmp(&inStr[i], endStr, lenEnd) == 0)
        {
            lcn = i;
            break;
        }
    if (lcn <= fcn)
    {
        buf[0] = 0;
        return false;
    }

    if (lcn - fcn > static_cast<int>(bufSize) - 1)
        lcn = fcn + bufSize - 1;
    strncpy_s(buf, bufSize, &inStr[fcn], lcn - fcn);
    buf[lcn - fcn] = 0;
    return true;
}

uint32_t CINODE::GetColorFromStr(const char *inStr, uint32_t dwDefColor)
{
    if (inStr)
    {
        int a = ALPHA(dwDefColor);
        int r = RED(dwDefColor);
        int g = GREEN(dwDefColor);
        int b = BLUE(dwDefColor);
        GetDataStr(inStr, "llll", &a, &r, &g, &b);
        dwDefColor = ARGB(a, r, g, b);
    }
    return dwDefColor;
}

bool CINODE::CheckByToolTip(float fX, float fY)
{
    if (m_pToolTip)
    {
        m_pToolTip->MousePos(core.GetDeltaTime() * .001f, static_cast<long>(fX), static_cast<long>(fY));
        return true;
    }
    return false;
}

void CINODE::ShowToolTip() const
{
    if (m_pToolTip)
        m_pToolTip->Draw();
}

const char *CINODE::GetDataStr(const char *inStr, const char *strOrder, ...)
{
    if (inStr == nullptr || strOrder == nullptr)
        return nullptr;
    va_list vl;
    va_start(vl, strOrder);
    char param[256];
    for (auto i = 0; strOrder[i] != 0; i++)
    {
        inStr = GetSubStr(inStr, param, sizeof(param));
        switch (strOrder[i])
        {
        case 'f':
        case 'F':
            *va_arg(vl, float *) = static_cast<float>(atof(param));
            break;
        case 'l':
        case 'L':
            *va_arg(vl, long *) = atol(param);
            break;
        }
    }
    return inStr;
}

uint32_t CINODE::MessageProc(long msgcode, MESSAGE &message)
{
    switch (msgcode)
    {
    case 0: // Execute node command
    {
        const auto commCode = message.Long();
        CommandExecute(commCode);
    }
    break;

    case 1: // Set clickable status
    {
        const auto clickState = message.Long();
        m_bClickable = clickState != 0;
    }
    break;

    case 2: // Get clickable status
        return m_bClickable ? 1 : 0;
        break;

    case 3: // Execute node command for command name
    {
        const std::string &param = message.String();
        const auto commIdx = FindCommand(param.c_str());
        if (commIdx >= 0)
            CommandExecute(pCommandsList[commIdx].code);
    }
    break;

    case 4: // Change position
    {
        XYRECT rect;
        rect.left = message.Long();
        rect.top = message.Long();
        rect.right = message.Long();
        rect.bottom = message.Long();
        ChangePosition(rect);
    }
    break;
    }
    return 0;
}

bool CINODE::CheckCommandUsed(int comCode) const
{
    int i;
    for (i = 0; i < COMMAND_QUANTITY; i++)
        if (pCommandsList[i].code == comCode)
            break;
    if (i == COMMAND_QUANTITY)
        return false;
    return m_pCommands[i].bUse;
}

bool CINODE::Init(INIFILE *ini1, const char *name1, INIFILE *ini2, const char *name2, VDX9RENDER *rs, XYRECT &hostRect,
                  XYPOINT &ScreenSize)
{
    char param[512];
    if (!rs)
        return false;
    m_rs = rs;
    memcpy(&m_screenSize, &ScreenSize, sizeof(ScreenSize));
    memcpy(&m_hostRect, &hostRect, sizeof(hostRect));

    // get position
    m_rect = GetIniLongRect(ini1, name1, ini2, name2, "position", m_hostRect);
    m_nAbsoluteRectVal = GetIniLong(ini1, name1, ini2, name2, "bAbsoluteRectangle", 0);
    GetAbsoluteRect(m_rect, m_nAbsoluteRectVal);

    // glow cursor
    SetGlowCursor(GetIniBool(ini1, name1, ini2, name2, "bShowGlowCursor", true));
    SetGlowCursorToBack(GetIniBool(ini1, name1, ini2, name2, "bGlowCursorToBack", false));

    // start using
    m_bUse = !GetIniBool(ini1, name1, ini2, name2, "bNotUse", false);

    // mouse weel reaction
    m_bMouseWeelReaction = GetIniBool(ini1, name1, ini2, name2, "bWheelUse", false);
    m_bMouseWeelReaction = GetIniBool(ini1, name1, ini2, name2, "bUseWheel", m_bMouseWeelReaction);

    if (ReadIniString(ini1, name1, ini2, name2, "HelpTextureFile", param, sizeof(param) - 1, ""))
    {
        const auto len = strlen(param) + 1;
        m_strHelpTextureFile = new char[len];
        if (m_strHelpTextureFile != nullptr)
        {
            memcpy(m_strHelpTextureFile, param, len);
        }
    }
    m_frectHelpTextureUV = GetIniFloatRect(ini1, name1, ini2, name2, "HelpTextureUV", FXYRECT(0.0, 0.0, 1.0, 1.0));

    if (ReadIniString(ini1, name1, ini2, name2, "GlowRectangle", param, sizeof(param) - 1, ""))
    {
        m_bUseUserGlowCursor = true;
        GetDataStr(param, "llll", &m_rectUserGlowCursor.left, &m_rectUserGlowCursor.top, &m_rectUserGlowCursor.right,
                   &m_rectUserGlowCursor.bottom);
        GetAbsoluteRect(m_rectUserGlowCursor, m_nAbsoluteRectVal);
    }

    if (ReadIniString(ini1, name1, ini2, name2, "GlowOffset", param, sizeof(param) - 1, ""))
    {
        m_bUseUserGlowOffset = true;
        GetDataStr(param, "ff", &m_rectUserGlowOffset.x, &m_rectUserGlowOffset.y);
    }

    if (ReadIniString(ini1, name1, ini2, name2, "tooltip", param, sizeof(param), ""))
    {
        m_pToolTip = new CXI_ToolTip(pPictureService, pStringService, m_screenSize);
        Assert(m_pToolTip);
        m_pToolTip->SetByFormatString(m_rect, ini2, param);
    }

    m_bMakeActionInDeclick = GetIniBool(ini1, name1, ini2, name2, "UseActionByDeclick", false);

    LoadIni(ini1, name1, ini2, name2);
    return true;
}

float CINODE::GetIniFloat(INIFILE *ini1, const char *name1, INIFILE *ini2, const char *name2, const char *keyName,
                          float fDefault)
{
    char param[256];
    auto bYes = false;
    if (ini1 && name1 && ini1->ReadString(name1, keyName, param, sizeof(param), ""))
        bYes = true;
    if (!bYes)
        if (ini2 && name2 && ini2->ReadString(name2, keyName, param, sizeof(param), ""))
            bYes = true;
    if (bYes)
    {
        fDefault = static_cast<float>(atof(param));
    }
    return fDefault;
}

long CINODE::GetIniLong(INIFILE *ini1, const char *name1, INIFILE *ini2, const char *name2, const char *keyName,
                        long iDefault)
{
    char param[256];
    auto bYes = false;
    if (ini1 && name1 && ini1->ReadString(name1, keyName, param, sizeof(param), ""))
        bYes = true;
    if (!bYes)
        if (ini2 && name2 && ini2->ReadString(name2, keyName, param, sizeof(param), ""))
            bYes = true;
    if (bYes)
    {
        iDefault = atol(param);
    }
    return iDefault;
}

bool CINODE::ReadIniString(INIFILE *ini1, const char *name1, INIFILE *ini2, const char *name2, const char *keyName,
                           char *buf, size_t bufSize, const char *strDef)
{
    auto bYes = false;
    if (strDef == nullptr)
        strDef = "";
    if (ini1 && name1 && ini1->ReadString(name1, keyName, buf, bufSize - 1, strDef))
        bYes = true;
    if (!bYes)
        if (ini2 && name2 && ini2->ReadString(name2, keyName, buf, bufSize - 1, strDef))
            bYes = true;
    if (!bYes && strDef != nullptr) //~!
    {
        if (buf)
        {
            int strDefLen = strlen(strDef);
            if (strDefLen > static_cast<int>(bufSize) - 1)
                strDefLen = bufSize - 1;
            if (strDefLen > 0)
                strncpy_s(buf, bufSize, strDef, strDefLen);
            buf[strDefLen] = 0;
        }
    }
    return bYes;
}

bool CINODE::GetIniBool(INIFILE *ini1, const char *name1, INIFILE *ini2, const char *name2, const char *keyName,
                        bool bDefault)
{
    return GetIniLong(ini1, name1, ini2, name2, keyName, bDefault) != 0;
}

XYRECT CINODE::GetIniLongRect(INIFILE *ini1, const char *name1, INIFILE *ini2, const char *name2, const char *keyName,
                              const XYRECT &rectDefault)
{
    char param[256];
    auto outRect = rectDefault;
    if (ReadIniString(ini1, name1, ini2, name2, keyName, param, sizeof(param)))
    {
        GetDataStr(param, "llll", &outRect.left, &outRect.top, &outRect.right, &outRect.bottom);
    }
    return outRect;
}

FXYRECT CINODE::GetIniFloatRect(INIFILE *ini1, const char *name1, INIFILE *ini2, const char *name2, const char *keyName,
                                const FXYRECT &rectDefault)
{
    char param[256];
    auto outRect = rectDefault;
    if (ReadIniString(ini1, name1, ini2, name2, keyName, param, sizeof(param)))
    {
        GetDataStr(param, "ffff", &outRect.left, &outRect.top, &outRect.right, &outRect.bottom);
    }
    return outRect;
}

XYPOINT CINODE::GetIniLongPoint(INIFILE *ini1, const char *name1, INIFILE *ini2, const char *name2, const char *keyName,
                                const XYPOINT &pntDefault)
{
    char param[256];
    auto outPnt = pntDefault;
    if (ReadIniString(ini1, name1, ini2, name2, keyName, param, sizeof(param)))
    {
        GetDataStr(param, "ll", &outPnt.x, &outPnt.y);
    }
    return outPnt;
}

FXYPOINT CINODE::GetIniFloatPoint(INIFILE *ini1, const char *name1, INIFILE *ini2, const char *name2,
                                  const char *keyName, const FXYPOINT &pntDefault)
{
    char param[256];
    auto outPnt = pntDefault;
    if (ReadIniString(ini1, name1, ini2, name2, keyName, param, sizeof(param)))
    {
        GetDataStr(param, "ff", &outPnt.x, &outPnt.y);
    }
    return outPnt;
}

uint32_t CINODE::GetIniARGB(INIFILE *ini1, const char *name1, INIFILE *ini2, const char *name2, const char *keyName,
                            uint32_t dwDefColor)
{
    char param[256];
    if (ReadIniString(ini1, name1, ini2, name2, keyName, param, sizeof(param)))
    {
        long a = ALPHA(dwDefColor);
        long r = RED(dwDefColor);
        long g = GREEN(dwDefColor);
        long b = BLUE(dwDefColor);
        GetDataStr(param, "llll", &a, &r, &g, &b);
        dwDefColor = ARGB(a, r, g, b);
    }
    return dwDefColor;
}
