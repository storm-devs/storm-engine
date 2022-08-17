#include "xi_formt_text.h"
#include "../xinterface.h"
#include "entity.h"
#include "string_compare.hpp"
#include "utf8.h"
#include "xi_scroller.h"

#include <cstdio>

CXI_FORMATEDTEXT::STRING_DESCRIBER::STRING_DESCRIBER(char *ls) : color(0)
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
    next = nullptr;
    strNum = 0;
    prev = nullptr;
    strGroup = 0;
}

CXI_FORMATEDTEXT::STRING_DESCRIBER *CXI_FORMATEDTEXT::STRING_DESCRIBER::Add(char *ls, int groupNum)
{
    if (ls == nullptr)
        return nullptr;
    auto *newSD = new STRING_DESCRIBER(ls);
    if (newSD == nullptr)
    {
        throw std::runtime_error("allocate memory error");
    }

    newSD->prev = this;
    newSD->next = next;
    next = newSD;
    newSD->strNum = strNum + 1;
    newSD->strGroup = groupNum;

    return newSD;
}

CXI_FORMATEDTEXT::CXI_FORMATEDTEXT()
{
    m_nNodeType = NODETYPE_FORMATEDTEXTS;

    m_idFont = -1;
    m_dwColor = ARGB(255, 255, 255, 255);

    m_allStrings = 0;

    m_nAllTextStrings = 0;
    m_listCur = m_listRoot = nullptr;

    m_nStringGroupQuantity = 0;
    m_pVidTex = nullptr;
    m_idVBuf = -1;
    m_nCurGroupNum = 0;

    m_bUpEnable = false;
    m_bDownEnable = false;
    m_idUpEnableTexture = -1;
    m_idUpDisableTexture = -1;
    m_idDownEnableTexture = -1;
    m_idDownDisableTexture = -1;

    m_bBackRectangle = false;
    m_dwBackColor = 0;

    m_nUpRectOffset = 0;

    m_sScrollerName = nullptr;

    m_bSelectableCursor = false;
    m_bUseOneStringAdding = false;
    m_nVAlignmentOffset = 0;

    m_bFrized = false;
}

CXI_FORMATEDTEXT::~CXI_FORMATEDTEXT()
{
    ReleaseAll();
}

void CXI_FORMATEDTEXT::Draw(bool bSelected, uint32_t Delta_Time)
{
    if (!m_bUse)
        return;

    if (m_idVBuf != -1)
    {
        if (m_bBackRectangle)
        {
            m_rs->SetRenderState(D3DRS_TEXTUREFACTOR, m_dwBackColor);
            m_rs->DrawPrimitive(D3DPT_TRIANGLESTRIP, m_idVBuf, sizeof(XI_ONLYONETEX_VERTEX), 12, 2, "iTFRectangle");
        }
        if (m_pVidTex != nullptr)
        {
            m_rs->SetTexture(0, m_pVidTex->m_pTexture);
            m_rs->DrawPrimitive(D3DPT_TRIANGLESTRIP, m_idVBuf, sizeof(XI_ONLYONETEX_VERTEX), 0, 2, "iDinamicPictures");
        }
    }

    if (m_bUpEnable || m_bDownEnable)
    {
        if (m_bUpEnable && m_idUpEnableTexture >= 0)
        {
            m_rs->TextureSet(0, m_idUpEnableTexture);
            m_rs->DrawPrimitive(D3DPT_TRIANGLESTRIP, m_idVBuf, sizeof(XI_ONLYONETEX_VERTEX), 4, 2, "iDinamicPictures");
        }
        if (!m_bUpEnable && m_idUpDisableTexture >= 0)
        {
            m_rs->TextureSet(0, m_idUpDisableTexture);
            m_rs->DrawPrimitive(D3DPT_TRIANGLESTRIP, m_idVBuf, sizeof(XI_ONLYONETEX_VERTEX), 4, 2, "iDinamicPictures");
        }
        if (m_bDownEnable && m_idDownEnableTexture >= 0)
        {
            m_rs->TextureSet(0, m_idDownEnableTexture);
            m_rs->DrawPrimitive(D3DPT_TRIANGLESTRIP, m_idVBuf, sizeof(XI_ONLYONETEX_VERTEX), 8, 2, "iDinamicPictures");
        }
        if (!m_bDownEnable && m_idDownDisableTexture >= 0)
        {
            m_rs->TextureSet(0, m_idDownDisableTexture);
            m_rs->DrawPrimitive(D3DPT_TRIANGLESTRIP, m_idVBuf, sizeof(XI_ONLYONETEX_VERTEX), 8, 2, "iDinamicPictures");
        }
    }

    auto curY = m_rect.top + m_nVAlignmentOffset;
    auto i = 0;
    for (auto *sd = m_listCur; sd != nullptr && i < m_allStrings; sd = sd->next, i++)
    {
        // display lines
        if (sd->lineStr != nullptr && sd->lineStr[0] != 0)
        {
            if (sd->m_tags.size() == 0)
                m_rs->ExtPrint(m_idFont, sd->color, 0, m_nAlignment, true, m_fFontScale, m_screenSize.x, m_screenSize.y,
                               m_nPrintLeftOffset, curY, "%s", sd->lineStr);
            else
            {
                auto nX = m_nPrintLeftOffset;
                std::vector<int32_t> anWidth;
                int32_t nAllWidth = 0;
                for (int32_t n = 0; n < sd->m_tags.size(); n++)
                {
                    auto nCurWidth = m_rs->StringWidth((char *)sd->m_tags[n].str.c_str(), m_idFont, m_fFontScale);
                    anWidth.push_back(nCurWidth);
                    nAllWidth += nCurWidth;
                }
                if (m_nAlignment == PR_ALIGN_CENTER)
                    nX -= nAllWidth / 2;
                if (m_nAlignment == PR_ALIGN_RIGHT)
                    nX -= nAllWidth;
                for (int32_t n = 0; n < sd->m_tags.size(); n++)
                {
                    m_rs->ExtPrint(m_idFont, sd->m_tags[n].dwColor, 0, PR_ALIGN_LEFT, true, m_fFontScale,
                                   m_screenSize.x, m_screenSize.y, nX, curY, "%s", sd->m_tags[n].str.c_str());
                    nX += anWidth[n]; // m_rs->StringWidth( (char*)sd->m_tags[n].str.c_str(), m_idFont, m_fFontScale );
                }
            }
        }
        curY += m_vertOffset;
    }
}

bool CXI_FORMATEDTEXT::Init(INIFILE *ini1, const char *name1, INIFILE *ini2, const char *name2, VDX9RENDER *rs,
                            XYRECT &hostRect, XYPOINT &ScreenSize)
{
    if (!CINODE::Init(ini1, name1, ini2, name2, rs, hostRect, ScreenSize))
        return false;
    // SetGlowCursor(m_bSelectableCursor && m_nStringGroupQuantity>1);
    return true;
}

void CXI_FORMATEDTEXT::ReleaseAll()
{
    FONT_RELEASE(m_rs, m_idFont);
    VIDEOTEXTURE_RELEASE(m_rs, m_pVidTex);
    VERTEX_BUFFER_RELEASE(m_rs, m_idVBuf);
    TEXTURE_RELEASE(m_rs, m_idUpEnableTexture);
    TEXTURE_RELEASE(m_rs, m_idUpDisableTexture);
    TEXTURE_RELEASE(m_rs, m_idDownEnableTexture);
    TEXTURE_RELEASE(m_rs, m_idDownDisableTexture);
    STORM_DELETE(m_sScrollerName);
    ReleaseStringes();
    m_asSyncNodes.clear();
}

int CXI_FORMATEDTEXT::CommandExecute(int wActCode)
{
    if (m_bUse && !m_bFrized)
    {
        if (m_listCur == nullptr)
            return -1;
        auto *const pOldCur = m_listCur;
        if (wActCode == ACTION_MOUSECLICK || wActCode == ACTION_MOUSERCLICK)
        {
            const auto fpntMouse = ptrOwner->GetMousePoint();
            if (m_bUpEnable && m_idUpEnableTexture >= 0)
            {
                if (fpntMouse.x >= m_frUpPos.left && fpntMouse.x <= m_frUpPos.right && fpntMouse.y >= m_frUpPos.top &&
                    fpntMouse.y <= m_frUpPos.bottom)
                {
                    wActCode = ACTION_UPSTEP;
                }
            }
            if (m_bDownEnable && m_idDownEnableTexture >= 0)
            {
                if (fpntMouse.x >= m_frDownPos.left && fpntMouse.x <= m_frDownPos.right &&
                    fpntMouse.y >= m_frDownPos.top && fpntMouse.y <= m_frDownPos.bottom)
                {
                    wActCode = ACTION_DOWNSTEP;
                }
            }
        }
        if (!m_bSelectableCursor)
        {
            // to move through single lines
            switch (wActCode)
            {
            case ACTION_UPSTEP:
                SetCurLine(m_listCur->prev);
                ControlSyncronouseNodes();
                break;
            case ACTION_DOWNSTEP:
                SetCurLine(m_listCur->next);
                ControlSyncronouseNodes();
                break;
            case ACTION_SPEEDUP: {
                auto *pStrDscr = m_listCur;
                for (auto i = 0; i < m_allStrings; i++)
                {
                    if (pStrDscr->prev == nullptr)
                        break;
                    pStrDscr = pStrDscr->prev;
                }
                SetCurLine(pStrDscr);
                ControlSyncronouseNodes();
            }
            break;
            case ACTION_SPEEDDOWN: {
                auto *pStrDscr = m_listCur;
                for (int i = 0; i < m_allStrings; i++)
                {
                    if (pStrDscr->next == nullptr)
                        break;
                    pStrDscr = pStrDscr->next;
                }
                SetCurLine(pStrDscr);
                ControlSyncronouseNodes();
            }
            break;
            }
        }
        else
        {
            // To move through blocks of lines
            switch (wActCode)
            {
            case ACTION_UPSTEP:
                SetVertexToNewGroup(true, FindUpGroup(m_nCurGroupNum - 1), FindDownGroup(m_nCurGroupNum - 1));
                break;
            case ACTION_DOWNSTEP:
                SetVertexToNewGroup(false, FindUpGroup(m_nCurGroupNum + 1), FindDownGroup(m_nCurGroupNum + 1));
                break;
            case ACTION_SPEEDUP:
                SetVertexToNewGroup(true, FindUpGroup(m_nCurGroupNum - 1), FindDownGroup(m_nCurGroupNum - 1));
                break;
            case ACTION_SPEEDDOWN:
                SetVertexToNewGroup(false, FindUpGroup(m_nCurGroupNum + 1), FindDownGroup(m_nCurGroupNum + 1));
                break;
            case ACTION_MOUSEDBLCLICK:
            case ACTION_MOUSECLICK:
            case ACTION_MOUSERCLICK:
                if (!m_bFrized)
                    core.Event("FormatedTextActivate", "sl", m_nodeName, m_nCurGroupNum);
                break;
            case ACTION_ACTIVATE:
                core.Event("FormatedTextActivate", "sl", m_nodeName, m_nCurGroupNum);
                break;
            }
            if (m_nStringGroupQuantity > 1)
                core.Event("SetScrollerPos", "sf", m_nodeName,
                           static_cast<float>(m_nCurGroupNum) / static_cast<float>(m_nStringGroupQuantity - 1));
            else
                core.Event("SetScrollerPos", "sf", m_nodeName, 0.f);
            core.Event("FTChange", "sf", m_nodeName, GetCurPos());
            ScrollerUpdate();
            ControlSyncronouseNodes();
        }
        if (pOldCur != m_listCur)
            CheckScrollButtons();
    }
    return -1;
}

bool CXI_FORMATEDTEXT::IsClick(int buttonID, int32_t xPos, int32_t yPos)
{
    if (m_bFrized)
    {
        if (buttonID == MOUSE_RBUTTON)
            return true;
        return false;
    }
    bool retVal = false;
    if (xPos >= m_rect.left && xPos <= m_rect.right && yPos >= m_rect.top && yPos <= m_rect.bottom)
    {
        // if(buttonID==MOUSE_RBUTTON)    return true;
        retVal = true;
    }
    // if(buttonID!=MOUSE_LBUTTON)    return false;

    if (m_bUpEnable && m_idUpEnableTexture)
        if (xPos >= m_frUpPos.left && xPos <= m_frUpPos.right && yPos >= m_frUpPos.top && yPos <= m_frUpPos.bottom)
            retVal = true;
    if (!retVal && m_bDownEnable && m_idDownEnableTexture)
        if (xPos >= m_frDownPos.left && xPos <= m_frDownPos.right && yPos >= m_frDownPos.top &&
            yPos <= m_frDownPos.bottom)
            retVal = true;

    if (!retVal)
        return false;

    if (!m_bSelectableCursor)
        return true;
    const int32_t nNum = static_cast<int32_t>((yPos - m_rect.top) / m_vertOffset + .1f);
    if (nNum < 0 || nNum >= m_allStrings)
        return true;

    STRING_DESCRIBER *pdescr = m_listCur;
    for (int i = 0; i < nNum && pdescr != nullptr; i++)
        pdescr = pdescr->next;
    if (pdescr != nullptr && m_nCurGroupNum != pdescr->strGroup)
        SetVertexToNewGroup(true, FindUpGroup(pdescr->strGroup), FindDownGroup(pdescr->strGroup));
    if (m_nStringGroupQuantity > 0)
        core.Event("SetScrollerPos", "sf", m_nodeName,
                   static_cast<float>(m_nCurGroupNum) / static_cast<float>(m_nStringGroupQuantity));
    else
        core.Event("SetScrollerPos", "sf", m_nodeName, 0.f);
    core.Event("FTChange", "sf", m_nodeName, GetCurPos());
    ScrollerUpdate();
    ControlSyncronouseNodes();
    return true;
}

void CXI_FORMATEDTEXT::ChangePosition(XYRECT &rNewPos)
{
    m_rect = rNewPos;

    RefreshAlignment();

    m_rectCursorPosition.left = m_rect.left;
    m_rectCursorPosition.right = m_rect.right;

    XI_ONLYONETEX_VERTEX *pv = nullptr;
    if (m_idVBuf != -1)
        pv = static_cast<XI_ONLYONETEX_VERTEX *>(m_rs->LockVertexBuffer(m_idVBuf));
    if (pv != nullptr)
    {
        pv[0].tu = pv[1].tu = 0.f;
        pv[2].tu = pv[3].tu = 1.f;
        pv[0].tv = pv[2].tv = 0.f;
        pv[1].tv = pv[3].tv = 1.f;
        pv[0].pos.z = pv[1].pos.z = pv[2].pos.z = pv[3].pos.z = 1.f;
        pv[0].pos.x = pv[1].pos.x = static_cast<float>(m_rect.left);
        pv[2].pos.x = pv[3].pos.x = static_cast<float>(m_rect.right);
        pv[0].pos.y = pv[2].pos.y = 0;
        pv[1].pos.y = pv[3].pos.y = 0;

        if (m_bBackRectangle)
        {
            pv[12].tu = pv[13].tu = 0.f;
            pv[14].tu = pv[15].tu = 1.f;
            pv[12].tv = pv[14].tv = 0.f;
            pv[13].tv = pv[15].tv = 1.f;
            pv[12].pos.z = pv[13].pos.z = pv[14].pos.z = pv[15].pos.z = 1.f;
            pv[12].pos.x = pv[13].pos.x = static_cast<float>(m_rect.left) - m_rBorderOffset.left;
            pv[14].pos.x = pv[15].pos.x = static_cast<float>(m_rect.right) + m_rBorderOffset.right;
            pv[12].pos.y = pv[14].pos.y = 0;
            pv[13].pos.y = pv[15].pos.y = 0;
        }

        // get up enable rectangle position
        if (m_idUpEnableTexture >= 0 || m_idUpDisableTexture >= 0)
        {
            pv[4].tu = pv[5].tu = m_frUpDisableUV.left;
            pv[6].tu = pv[7].tu = m_frUpDisableUV.right;
            pv[4].tv = pv[6].tv = m_frUpDisableUV.top;
            pv[5].tv = pv[7].tv = m_frUpDisableUV.bottom;
            pv[4].pos.z = pv[5].pos.z = pv[6].pos.z = pv[7].pos.z = 1.f;
            pv[4].pos.x = pv[5].pos.x = static_cast<float>(m_frUpPos.left);
            pv[6].pos.x = pv[7].pos.x = static_cast<float>(m_frUpPos.right);
            pv[4].pos.y = pv[6].pos.y = static_cast<float>(m_frUpPos.top);
            pv[5].pos.y = pv[7].pos.y = static_cast<float>(m_frUpPos.bottom);
        }

        if (m_idDownEnableTexture >= 0 || m_idDownDisableTexture >= 0)
        {
            pv[8].tu = pv[9].tu = m_frDownDisableUV.left;
            pv[10].tu = pv[11].tu = m_frDownDisableUV.right;
            pv[8].tv = pv[10].tv = m_frDownDisableUV.top;
            pv[9].tv = pv[11].tv = m_frDownDisableUV.bottom;
            pv[8].pos.z = pv[9].pos.z = pv[10].pos.z = pv[11].pos.z = 1.f;
            pv[8].pos.x = pv[9].pos.x = static_cast<float>(m_frDownPos.left);
            pv[10].pos.x = pv[11].pos.x = static_cast<float>(m_frDownPos.right);
            pv[8].pos.y = pv[10].pos.y = static_cast<float>(m_frDownPos.top);
            pv[9].pos.y = pv[11].pos.y = static_cast<float>(m_frDownPos.bottom);
        }

        m_rs->UnLockVertexBuffer(m_idVBuf);
    }

    m_allStrings = static_cast<int32_t>(static_cast<float>(m_rect.bottom - m_rect.top) / m_vertOffset);
}

void CXI_FORMATEDTEXT::SaveParametersToIni()
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

void CXI_FORMATEDTEXT::RefreshAlignment()
{
    m_nCompareWidth = m_rect.right - m_rect.left - m_leftOffset;
    if (m_nAlignment == PR_ALIGN_CENTER)
        m_nPrintLeftOffset = m_leftOffset + static_cast<int32_t>((m_rect.left + m_rect.right - m_leftOffset) * .5f);
    else if (m_nAlignment == PR_ALIGN_RIGHT)
        m_nPrintLeftOffset = m_rect.right - m_leftOffset;
    else
        m_nPrintLeftOffset = m_rect.left + m_leftOffset;
}

void CXI_FORMATEDTEXT::LoadIni(INIFILE *ini1, const char *name1, INIFILE *ini2, const char *name2)
{
    char param[2048];

    m_bUseOneStringAdding = GetIniBool(ini1, name1, ini2, name2, "wrapoff", false);
    if (ReadIniString(ini1, name1, ini2, name2, "syncnodes", param, sizeof(param), ""))
    {
        CXI_UTILS::StringFillStringArray(param, m_asSyncNodes);
    }

    const int nPosMorph = GetIniLong(ini1, name1, ini2, name2, "bAbsoluteRectangle");

    m_fFontScale = GetIniFloat(ini1, name1, ini2, name2, "fontScale", 1.f);

    if (ReadIniString(ini1, name1, ini2, name2, "scrollerName", param, sizeof(param), ""))
    {
        const auto len = strlen(param) + 1;
        m_sScrollerName = new char[len];
        Assert(m_sScrollerName);
        memcpy(m_sScrollerName, param, len);
    }

    ReadIniString(ini1, name1, ini2, name2, "alignment", param, sizeof(param), "left");
    if (storm::iEquals(param, "center"))
        m_nAlignment = PR_ALIGN_CENTER;
    else if (storm::iEquals(param, "right"))
        m_nAlignment = PR_ALIGN_RIGHT;
    else
        m_nAlignment = PR_ALIGN_LEFT;

    m_leftOffset = GetIniLong(ini1, name1, ini2, name2, "leftoffset");
    RefreshAlignment();

    m_nUpRectOffset = GetIniLong(ini1, name1, ini2, name2, "upOffset");

    // get videoTexture
    if (ReadIniString(ini1, name1, ini2, name2, "videoName", param, sizeof(param), ""))
        m_pVidTex = m_rs->GetVideoTexture(param);

    // get colors
    m_dwBackColor = GetIniARGB(ini1, name1, ini2, name2, "backColor", 0);
    m_bBackRectangle = ALPHA(m_dwBackColor) != 0;
    if (m_bBackRectangle)
    {
        m_rBorderOffset.left = m_rBorderOffset.top = m_rBorderOffset.right = m_rBorderOffset.bottom = 0;
        m_rBorderOffset = GetIniLongRect(ini1, name1, ini2, name2, "backOffset", m_rBorderOffset);
    }

    if (m_pVidTex || m_bBackRectangle)
        m_bSelectableCursor = true;

    if (m_bSelectableCursor)
        m_bSelected = true;
    m_idVBuf = m_rs->CreateVertexBuffer(XI_ONLYONETEX_FVF, 4 * 4 * sizeof(XI_ONLYONETEX_VERTEX), D3DUSAGE_WRITEONLY);

    m_rectCursorPosition.left = m_rect.left;
    m_rectCursorPosition.right = m_rect.right;

    XI_ONLYONETEX_VERTEX *pv = nullptr;
    if (m_idVBuf != -1)
        pv = static_cast<XI_ONLYONETEX_VERTEX *>(m_rs->LockVertexBuffer(m_idVBuf));
    if (pv != nullptr)
    {
        pv[0].tu = pv[1].tu = 0.f;
        pv[2].tu = pv[3].tu = 1.f;
        pv[0].tv = pv[2].tv = 0.f;
        pv[1].tv = pv[3].tv = 1.f;
        pv[0].pos.z = pv[1].pos.z = pv[2].pos.z = pv[3].pos.z = 1.f;
        pv[0].pos.x = pv[1].pos.x = static_cast<float>(m_rect.left);
        pv[2].pos.x = pv[3].pos.x = static_cast<float>(m_rect.right);
        pv[0].pos.y = pv[2].pos.y = 0;
        pv[1].pos.y = pv[3].pos.y = 0;

        if (m_bBackRectangle)
        {
            pv[12].tu = pv[13].tu = 0.f;
            pv[14].tu = pv[15].tu = 1.f;
            pv[12].tv = pv[14].tv = 0.f;
            pv[13].tv = pv[15].tv = 1.f;
            pv[12].pos.z = pv[13].pos.z = pv[14].pos.z = pv[15].pos.z = 1.f;
            pv[12].pos.x = pv[13].pos.x = static_cast<float>(m_rect.left) - m_rBorderOffset.left;
            pv[14].pos.x = pv[15].pos.x = static_cast<float>(m_rect.right) + m_rBorderOffset.right;
            pv[12].pos.y = pv[14].pos.y = 0;
            pv[13].pos.y = pv[15].pos.y = 0;
        }

        // get up enable rectangle position
        if (ReadIniString(ini1, name1, ini2, name2, "UpEnableTexture", param, sizeof(param), ""))
            m_idUpEnableTexture = m_rs->TextureCreate(param);
        if (ReadIniString(ini1, name1, ini2, name2, "UpDisableTexture", param, sizeof(param), ""))
            m_idUpDisableTexture = m_rs->TextureCreate(param);
        if (m_idUpEnableTexture >= 0 || m_idUpDisableTexture >= 0)
        {
            m_frUpPos = GetIniLongRect(ini1, name1, ini2, name2, "UpEnablePos", XYRECT(0, 0, 0, 0));
            GetAbsoluteRect(m_frUpPos, nPosMorph);
            m_frUpEnableUV = GetIniFloatRect(ini1, name1, ini2, name2, "UpEnableUV", FXYRECT(0.f, 0.f, 1.f, 1.f));
            m_frUpDisableUV = GetIniFloatRect(ini1, name1, ini2, name2, "UpDisableUV", FXYRECT(0.f, 0.f, 1.f, 1.f));
            pv[4].tu = pv[5].tu = m_frUpDisableUV.left;
            pv[6].tu = pv[7].tu = m_frUpDisableUV.right;
            pv[4].tv = pv[6].tv = m_frUpDisableUV.top;
            pv[5].tv = pv[7].tv = m_frUpDisableUV.bottom;
            pv[4].pos.z = pv[5].pos.z = pv[6].pos.z = pv[7].pos.z = 1.f;
            pv[4].pos.x = pv[5].pos.x = static_cast<float>(m_frUpPos.left);
            pv[6].pos.x = pv[7].pos.x = static_cast<float>(m_frUpPos.right);
            pv[4].pos.y = pv[6].pos.y = static_cast<float>(m_frUpPos.top);
            pv[5].pos.y = pv[7].pos.y = static_cast<float>(m_frUpPos.bottom);
        }

        if (ReadIniString(ini1, name1, ini2, name2, "DownEnableTexture", param, sizeof(param), ""))
            m_idDownEnableTexture = m_rs->TextureCreate(param);
        if (ReadIniString(ini1, name1, ini2, name2, "DownDisableTexture", param, sizeof(param), ""))
            m_idDownDisableTexture = m_rs->TextureCreate(param);
        if (m_idDownEnableTexture >= 0 || m_idDownDisableTexture >= 0)
        {
            m_frDownPos = GetIniLongRect(ini1, name1, ini2, name2, "DownEnablePos", XYRECT(0, 0, 0, 0));
            GetAbsoluteRect(m_frDownPos, nPosMorph);
            m_frDownEnableUV = GetIniFloatRect(ini1, name1, ini2, name2, "DownEnableUV", FXYRECT(0.f, 0.f, 1.f, 1.f));
            m_frDownDisableUV = GetIniFloatRect(ini1, name1, ini2, name2, "DownDisableUV", FXYRECT(0.f, 0.f, 1.f, 1.f));
            pv[8].tu = pv[9].tu = m_frDownDisableUV.left;
            pv[10].tu = pv[11].tu = m_frDownDisableUV.right;
            pv[8].tv = pv[10].tv = m_frDownDisableUV.top;
            pv[9].tv = pv[11].tv = m_frDownDisableUV.bottom;
            pv[8].pos.z = pv[9].pos.z = pv[10].pos.z = pv[11].pos.z = 1.f;
            pv[8].pos.x = pv[9].pos.x = static_cast<float>(m_frDownPos.left);
            pv[10].pos.x = pv[11].pos.x = static_cast<float>(m_frDownPos.right);
            pv[8].pos.y = pv[10].pos.y = static_cast<float>(m_frDownPos.top);
            pv[9].pos.y = pv[11].pos.y = static_cast<float>(m_frDownPos.bottom);
        }

        m_rs->UnLockVertexBuffer(m_idVBuf);
    }

    // get line space
    m_vertOffset = GetIniLong(ini1, name1, ini2, name2, "lineSpace", 30);
    if (m_vertOffset == 0)
        m_vertOffset = 10;

    // counting the number of lines displayed on the screen
    m_allStrings = static_cast<int32_t>(static_cast<float>(m_rect.bottom - m_rect.top) / m_vertOffset);

    // get golors
    m_dwColor = GetIniARGB(ini1, name1, ini2, name2, "color", ARGB(255, 128, 128, 128));

    // get font
    if (ReadIniString(ini1, name1, ini2, name2, "font", param, sizeof(param), ""))
        m_idFont = m_rs->LoadFont(param);

    // get strings
    if (ini1 && ini1->ReadString(name1, "string", param, sizeof(param) - 1, ""))
        do
        {
            if (param[0] == '#')
                AddFormatedText(&param[1]);
            else
            {
                char param2[2048];
                AddFormatedText(pStringService->GetString(param, param2, sizeof(param2)));
            }
        } while (ini1->ReadStringNext(name1, "string", param, sizeof(param) - 1));

    //
    VAlignment(GetIniLong(ini1, name1, ini2, name2, "valignment", 0));
}

void CXI_FORMATEDTEXT::ReleaseString(STRING_DESCRIBER *pCur)
{
    if (!pCur)
        return;
    if (m_listCur == pCur)
        m_listCur = pCur->next;
    if (m_listRoot == pCur)
        m_listRoot = pCur->next;
    if (pCur->prev)
        pCur->prev->next = pCur->next;
    if (pCur->next)
        pCur->next->prev = pCur->prev;
    pCur->next = pCur->prev = nullptr;
    m_nAllTextStrings--;
    STORM_DELETE(pCur->lineStr);
    delete pCur;
}

void CXI_FORMATEDTEXT::ReleaseStringes()
{
    while (m_listRoot)
        ReleaseString(m_listRoot);
    m_listCur = nullptr;
    m_nAllTextStrings = 0;
}

bool CXI_FORMATEDTEXT::GetLineNext(int fontNum, const char *&pInStr, char *buf, int bufSize) const
{
    if (pInStr == nullptr || buf == nullptr)
        return false;

    const char *pStart = pInStr;
    int bYesEOL = 0;
    while (*pInStr != 0)
    {
        if (pInStr[0] == '\\' && (pInStr[1] == 'n' || pInStr[1] == 'N'))
            break;
        if (pInStr[0] == '\r' || pInStr[0] == '\n')
            if (bYesEOL < 2)
                bYesEOL++;
            else
                break;
        else if (bYesEOL > 0)
            break;
        pInStr++;
    }
    const size_t lineSize = pInStr - pStart;
    if (lineSize > 0)
    {
        // fill the buffer without tags
        int32_t i, j;
        for (i = 0, j = 0; i < lineSize && i < bufSize - 1; i++)
        {
            if (pStart[i] == '<')
            {
                if (storm::iEquals(&pStart[i], "<color", 6))
                {
                    while (i < lineSize && i < bufSize - 1 && pStart[i] != '>')
                        i++;
                    continue;
                }
                if (storm::iEquals(&pStart[i], "</color>", 8))
                {
                    i += 7;
                    continue;
                }
            }
            buf[j++] = pStart[i];
        }

        if (j == bufSize - 1) // exclude truncated utf8 chars
            j -= utf8::u8_dec(buf + j);
        buf[j] = 0; // zero denotes the end of the line

        // if the string is large, then cut it
        while (m_rs->StringWidth(buf, fontNum, m_fFontScale) > m_nCompareWidth)
        {
            if (auto *rspace = strrchr(buf, ' '); rspace != nullptr)
            {
                *rspace = '\0';
            }
            else
            {
                break;
            }
        }

        const int32_t q = strlen(buf); // this is the length of the line without tags
        for (i = 0, j = 0; j < q; i++)
        {
            if (pStart[i] == '<')
            {
                if (storm::iEquals(&pStart[i], "<color", 6))
                {
                    while (i < lineSize && i < bufSize - 1 && pStart[i] != '>')
                        i++;
                    continue;
                }
                if (storm::iEquals(&pStart[i], "</color>", 8))
                {
                    i += 7;
                    continue;
                }
            }
            j++;
        }
        strncpy_s(buf, bufSize, pStart, i);
        buf[i] = 0;
        pInStr = &pStart[i];
    }
    else
    {
        buf[0] = ' ';
        buf[1] = 0;
    }

    // remove the line break
    if (pInStr[0] == '\\' && (pInStr[1] == 'n' || pInStr[1] == 'N'))
        pInStr += 2;
    // remove spaces (leading)
    while (pInStr[0])
    {
        if (static_cast<unsigned>(*pInStr) <= ' ')
            pInStr++;
        else
            break;
    }

    if (lineSize == 0 && pInStr[0] == 0)
        return false;
    return true;
}

void CXI_FORMATEDTEXT::GetOneLine(int fontNum, const char *pStr, char *buf, int bufSize) const
{
    if (pStr == nullptr || buf == nullptr)
        return;

    const char *pStart = pStr;
    int bYesEOL = 0;
    while (*pStr != 0)
    {
        if (*pStr == '\r' || *pStr == '\n')
            if (bYesEOL < 2)
                bYesEOL++;
            else
                break;
        else if (bYesEOL > 0)
            break;
        pStr++;
    }
    size_t lineSize = pStr - pStart;
    if (lineSize == 0)
        return;
    if (lineSize > bufSize - 1)
        lineSize = bufSize - utf8::u8_dec(pStart + bufSize);

    strncpy_s(buf, bufSize, pStart, lineSize);
    buf[lineSize] = 0;
    int32_t strWidth = m_rs->StringWidth(buf, fontNum, m_fFontScale);

    while (lineSize > 0 && strWidth > m_nCompareWidth)
    {
        lineSize -= utf8::u8_dec(buf + lineSize);
        buf[lineSize] = 0;
        strWidth = m_rs->StringWidth(buf, fontNum, m_fFontScale);
    }
}

void CXI_FORMATEDTEXT::SetFormatedText(const char *str)
{
    // delete old lines
    while (m_listRoot != nullptr)
    {
        m_listCur = m_listRoot;
        m_listRoot = m_listRoot->next;
        STORM_DELETE(m_listCur->lineStr);
        STORM_DELETE(m_listCur);
    }
    m_nStringGroupQuantity = 0;
    m_nAllTextStrings = 0;
    m_nVAlignmentOffset = 0;
    AddFormatedText(str);
    core.Event("SetScrollerPos", "sf", m_nodeName, 0.f);
    core.Event("FTChange", "sf", m_nodeName, GetCurPos());
    ScrollerUpdate();
}

void CXI_FORMATEDTEXT::SetPointer(float fPos)
{
    if (m_bSelectableCursor)
        SetVertexToNewGroup(true, FindUpGroup(static_cast<int32_t>(m_nStringGroupQuantity * fPos)),
                            FindDownGroup(static_cast<int32_t>(m_nStringGroupQuantity * fPos)));
    else
    {
        m_listCur = m_listRoot;
        for (int nq = static_cast<int>(m_nAllTextStrings * fPos); nq > 0; nq--)
            if (m_listCur != nullptr && m_listCur->next != nullptr)
                m_listCur = m_listCur->next;
            else
                break;
    }
    CheckScrollButtons();
    core.Event("FTChange", "sf", m_nodeName, GetCurPos());
}

float CXI_FORMATEDTEXT::GetLineStep() const
{
    if (m_bSelectableCursor)
    {
        if (m_nStringGroupQuantity > 1)
            return 1.f / static_cast<float>(m_nStringGroupQuantity - 1);
        return 0.f;
    }
    if (m_nAllTextStrings > 0)
        return 1.f / static_cast<float>(m_nAllTextStrings);
    return 0.f;
    return 0.f;
}

float CXI_FORMATEDTEXT::GetCurPos() const
{
    if (m_bSelectableCursor)
    {
        if (m_nStringGroupQuantity > 1)
            return static_cast<float>(m_nCurGroupNum) / static_cast<float>(m_nStringGroupQuantity - 1);
        return 0.f;
    }
    if (m_nAllTextStrings > 0)
        return static_cast<float>(m_listCur->strNum) / static_cast<float>(m_nAllTextStrings);
    return 0.f;
    return 0.f;
}

void CXI_FORMATEDTEXT::SetColor(uint32_t dwCol) const
{
    for (STRING_DESCRIBER *dscrTmp = m_listRoot; dscrTmp; dscrTmp = dscrTmp->next)
    {
        dscrTmp->color = dwCol;
    }
}

int32_t CXI_FORMATEDTEXT::GetAllHeight()
{
    int32_t n = 0;
    for (STRING_DESCRIBER *sd = m_listRoot; sd; sd = sd->next, n++)
        ;
    return m_vertOffset * n;
}

int32_t CXI_FORMATEDTEXT::AddFormatedText(const char *str)
{
    if (str == nullptr)
        return 0;
    m_nVAlignmentOffset = 0;

    STRING_DESCRIBER *dscrTmp = nullptr;

    // find last string
    if (m_listRoot != nullptr)
        for (dscrTmp = m_listRoot; dscrTmp->next != nullptr; dscrTmp = dscrTmp->next)
            ;

    char newStr[512];
    auto pstr = str;
    int32_t textQ = 0;
    bool tagState = false;
    uint32_t tagColor = m_dwColor;
    // decompose the resulting string into lines that fit into the output area
    if (m_bUseOneStringAdding)
    {
        GetOneLine(m_idFont, pstr, newStr, sizeof(newStr));
        if (m_listRoot == nullptr)
        {
            if ((dscrTmp = m_listRoot = new STRING_DESCRIBER(newStr)) == nullptr)
            {
                throw std::runtime_error("allocate memory error");
            }
            dscrTmp->color = m_dwColor;
        }
        else
        {
            dscrTmp = dscrTmp->Add(newStr, m_nStringGroupQuantity);
            dscrTmp->color = m_dwColor;
        }
        textQ++;
        MakeTagChecking(tagState, tagColor, m_dwColor, dscrTmp);
    }
    else
    {
        while (GetLineNext(m_idFont, pstr, newStr, sizeof(newStr)))
        {
            if (m_listRoot == nullptr)
            {
                if ((dscrTmp = m_listRoot = new STRING_DESCRIBER(newStr)) == nullptr)
                {
                    throw std::runtime_error("allocate memory error");
                }
                dscrTmp->color = m_dwColor;
            }
            else
            {
                dscrTmp = dscrTmp->Add(newStr, m_nStringGroupQuantity);
                dscrTmp->color = m_dwColor;
            }
            textQ++;
            MakeTagChecking(tagState, tagColor, m_dwColor, dscrTmp);
        }
    }

    m_nAllTextStrings += textQ;
    if (dscrTmp != nullptr)
    {
        m_nStringGroupQuantity++;
        core.Event("DisableScroll", "sl", m_nodeName, m_nAllTextStrings < m_allStrings);
    }

    if (m_listCur == nullptr)
    {
        m_listCur = m_listRoot;
        SetVertexToNewGroup(true, FindUpGroup(0), FindDownGroup(0));
    }

    CheckScrollButtons();

    // SetGlowCursor(m_bSelectableCursor && m_nStringGroupQuantity>1);

    return textQ;
}

void CXI_FORMATEDTEXT::MakeTagChecking(bool &tagState, uint32_t &tagColor, uint32_t normColor,
                                       STRING_DESCRIBER *pStrDescr)
{
    const char *str = pStrDescr->lineStr;
    const char *tagBegin = str;
    size_t q = 0; // ready tag size
    while (str && str[0])
    {
        q = 0;

        if (str[0] == '<')
        {
            if (tagState)
            {
                if (storm::iEquals(str, "</color>", 8))
                {
                    tagState = false;
                    q = str - tagBegin;
                    str += 8;
                }
            }
            else
            {
                if (storm::iEquals(str, "<color=", 7))
                {
                    tagState = true;
                    int32_t a = 255, r = 255, g = 255, b = 255;
                    sscanf(&str[7], "%d,%d,%d,%d", &a, &r, &g, &b);
                    tagColor = ARGB(a, r, g, b);
                    q = str - tagBegin;
                    while (str && str[0] && str[0] != '>')
                        str++;
                    if (str[0] == '>')
                        str++;
                }
            }
        }

        if (q > 0)
        {
            char tmp[512];
            if (q > sizeof(tmp) - 1)
                q = sizeof(tmp) - 1;
            int32_t n;
            for (n = 0; n < q; n++)
            {
                tmp[n] = tagBegin[n];
            }
            tmp[n] = 0;
            // n = pStrDescr->m_tags.Add();
            pStrDescr->m_tags.push_back(STRING_DESCRIBER::TagInfo{tagState ? normColor : tagColor, tmp});

            tagBegin = str;
        }
        else
            str++;
    }

    if (q > 0)
    {
        char tmp[512];
        if (q > sizeof(tmp) - 1)
            q = sizeof(tmp) - 1;
        int32_t n;
        for (n = 0; n < q; n++)
        {
            tmp[n] = tagBegin[n];
        }
        tmp[n] = 0;
        // n = pStrDescr->m_tags.Add();
        // if( tagState ) pStrDescr->m_tags[n].dwColor = normColor;
        // else pStrDescr->m_tags[n].dwColor = tagColor;
        // pStrDescr->m_tags[n].str = tmp;
        pStrDescr->m_tags.push_back(STRING_DESCRIBER::TagInfo{tagState ? normColor : tagColor, tmp});
    }
}

uint32_t CXI_FORMATEDTEXT::MessageProc(int32_t msgcode, MESSAGE &message)
{
    switch (msgcode)
    {
    case 0: // add text to the formatted list and return the number of occupied lines
    {
        VDATA *pvdat = message.ScriptVariablePointer();
        if (pvdat != nullptr)
        {
            const int retVar = AddFormatedText(pvdat->GetString());
            CheckScrollButtons();
            return retVar;
        }
    }
    break;

    case 1: // set the pointer to the specified text in the list (by its number)
    {
        const int32_t nGrNum = message.Long();
        if (m_bSelectableCursor)
            SetVertexToNewGroup(true, FindUpGroup(nGrNum), FindDownGroup(nGrNum));
        CheckScrollButtons();
    }
    break;

    case 2: // set the pointer to the text (by a fractional value from 0 to 1 - from the beginning to the end)
        SetPointer(message.Float());
        break;

    case 3: // fill attributes with text sizes
    {
        ATTRIBUTES *pAttr = message.AttributePointer();
        if (pAttr == nullptr)
            return 0;
        int i = 0;
        int oldgroup = -1;
        int idx = 0;
        if (m_listCur != nullptr && m_listCur->prev != nullptr)
            oldgroup = m_listCur->prev->strGroup;
        for (STRING_DESCRIBER *sd = m_listCur; sd != nullptr && i < m_allStrings; sd = sd->next, i++)
        {
            // display lines
            if (sd->lineStr != nullptr && sd->lineStr[0] != 0 && sd->strGroup != oldgroup)
            {
                oldgroup = sd->strGroup;
                char atrName[128];
                sprintf_s(atrName, "line%d", idx);
                pAttr->SetAttributeUseDword(atrName, i);
                idx++;
            }
            else if (idx == 0)
            {
                pAttr->SetAttributeUseDword("line0", -1);
                idx++;
            }
        }
    }
    break;

    case 4: // fill attributes with Y coordinates of the top of the text
    {
        ATTRIBUTES *pAttr = message.AttributePointer();
        if (pAttr == nullptr)
            return 0;

        int i = 0;
        int oldgroup = -1;
        int idx = 0;
        if (m_listCur != nullptr && m_listCur->prev != nullptr)
            oldgroup = m_listCur->prev->strGroup;
        for (STRING_DESCRIBER *sd = m_listCur; sd != nullptr && i < m_allStrings; sd = sd->next, i++)
        {
            // display lines
            if (sd->lineStr != nullptr && sd->lineStr[0] != 0 && sd->strGroup != oldgroup)
            {
                oldgroup = sd->strGroup;
                char atrName[128];
                sprintf_s(atrName, "line%d", idx);
                pAttr->SetAttributeUseDword(atrName, m_rect.top + m_vertOffset * i - m_hostRect.top);
                idx++;
            }
            else if (idx == 0)
            {
                pAttr->SetAttributeUseDword("line0", -1);
                idx++;
            }
        }
    }
    break;

    case 5: // Center text vertically
        VAlignment(1);
        break;

    case 6: // Get the state of the ability to move text up / down
    {
        if (message.Long())
            return m_bUpEnable;
        return m_bDownEnable;
    }
    break;

    case 7: // set strings to given positions
    {
        ATTRIBUTES *pA = message.AttributePointer();
        if (pA != nullptr)
            SetSpecialStrings(pA);
    }
    break;

    case 8: // Set color for group
    {
        int grNum = message.Long();
        const uint32_t newColor = message.Long();

        if (grNum < 0)
            grNum = m_nStringGroupQuantity - 1;

        for (STRING_DESCRIBER *sd = m_listRoot; sd != nullptr; sd = sd->next)
            if (sd->strGroup == grNum)
                sd->color = newColor;
    }
    break;

    case 9: // Find out the number of lines that a line will take
    {
        VDATA *pvdat = message.ScriptVariablePointer();
        if (pvdat != nullptr)
        {
            const char *tmpStr = pvdat->GetString();
            char buf[512];
            int n;
            for (n = 0; GetLineNext(m_idFont, tmpStr, buf, sizeof(buf)); n++)
                ;
            return n;
        }
    }
    break;

    case 10: {
        const int grNum = message.Long();
        VDATA *pvdat = message.ScriptVariablePointer();
        if (pvdat != nullptr)
            ReplaceString(grNum, pvdat->GetString());
    }
    break;

    case 11: // get the number of the currently selected line
        return m_nCurGroupNum;
        break;

    case 12: // find out the vertical coordinate of the required line
    {
        const int32_t n = message.Long();
        int32_t curY = m_rect.top + m_nVAlignmentOffset;
        int i = 0;
        for (STRING_DESCRIBER *sd = m_listCur; sd != nullptr && i < m_allStrings; sd = sd->next, i++)
        {
            if (sd->strGroup == n)
                return curY;
            curY += m_vertOffset;
        }
        return -1;
    }
    break;

    case 13: // set frized
        m_bFrized = message.Long() != 0;
        break;

    case 14: { // set alignment from scripts
        const auto new_alignment = message.Long(); 
        if (new_alignment < PR_ALIGN_LEFT || new_alignment > PR_ALIGN_CENTER)
        {
            return -1;
        }
        m_nAlignment = new_alignment;
        RefreshAlignment();
        break;
    }
    }

    return 0;
}

int32_t CXI_FORMATEDTEXT::FindUpGroup(int32_t grNum)
{
    STRING_DESCRIBER *dscrTmp = m_listCur;
    if (dscrTmp == nullptr)
        return 0;
    int32_t upPos = 0;
    if (grNum < 0)
        grNum = 0;
    if (grNum >= m_nStringGroupQuantity)
        grNum = m_nStringGroupQuantity - 1;

    if (dscrTmp->strGroup >= grNum)
    {
        for (; dscrTmp != nullptr && dscrTmp->strGroup >= grNum; dscrTmp = dscrTmp->prev)
            upPos--;
        upPos++;
        return upPos;
    }

    for (; dscrTmp != nullptr && dscrTmp->strGroup < grNum; dscrTmp = dscrTmp->next)
        upPos++;
    if (dscrTmp == nullptr)
        upPos--;
    return upPos;
}

int32_t CXI_FORMATEDTEXT::FindDownGroup(int32_t grNum)
{
    STRING_DESCRIBER *dscrTmp = m_listCur;
    if (dscrTmp == nullptr)
        return 0;
    int32_t upPos = 0;
    if (grNum < 0)
        grNum = 0;
    if (grNum >= m_nStringGroupQuantity)
        grNum = m_nStringGroupQuantity - 1;

    if (dscrTmp->strGroup > grNum)
    {
        for (; dscrTmp != nullptr && dscrTmp->strGroup > grNum; dscrTmp = dscrTmp->prev)
            upPos--;
        if (dscrTmp == nullptr)
            upPos++;
        return upPos;
    }

    for (; dscrTmp != nullptr && dscrTmp->strGroup <= grNum; dscrTmp = dscrTmp->next)
        upPos++;
    return upPos - 1;
}

void CXI_FORMATEDTEXT::SetVertexToNewGroup(bool bUpDirect, int32_t upIdx, int32_t downIdx)
{
    int i;

    if (downIdx - upIdx >= m_allStrings)
        if (bUpDirect)
            downIdx = upIdx + m_allStrings - 1;
        else
            upIdx = downIdx - m_allStrings + 1;

    bool bChange = false;

    for (; upIdx < 0 && m_listCur != nullptr; m_listCur = m_listCur->prev)
    {
        bChange = true;
        upIdx++;
        downIdx++;
    }

    for (; downIdx >= m_allStrings && m_listCur != nullptr; m_listCur = m_listCur->next)
    {
        bChange = true;
        upIdx--;
        downIdx--;
    }

    if (upIdx < 0)
        upIdx = 0;
    if (downIdx >= m_allStrings)
        downIdx = m_allStrings - 1;

    STRING_DESCRIBER *tmpDscr = m_listCur;
    for (i = 0; i < upIdx; i++)
    {
        if (tmpDscr == nullptr)
            break;
        tmpDscr = tmpDscr->next;
    }
    if (tmpDscr != nullptr)
        m_nCurGroupNum = tmpDscr->strGroup;
    else
        m_nCurGroupNum = 0;

    XI_ONLYONETEX_VERTEX *pv = nullptr;
    if (m_idVBuf != -1)
        pv = static_cast<XI_ONLYONETEX_VERTEX *>(m_rs->LockVertexBuffer(m_idVBuf));
    if (pv != nullptr)
    {
        m_rectCursorPosition.top = m_rect.top - m_nUpRectOffset + m_vertOffset * upIdx;
        m_rectCursorPosition.bottom = m_rect.top - m_nUpRectOffset + m_vertOffset * (downIdx + 1);
        pv[0].pos.y = pv[2].pos.y = static_cast<float>(m_rect.top - m_nUpRectOffset + m_vertOffset * upIdx);
        pv[1].pos.y = pv[3].pos.y = static_cast<float>(m_rect.top - m_nUpRectOffset + m_vertOffset * (downIdx + 1));
        if (m_bBackRectangle)
        {
            pv[12].pos.y = pv[14].pos.y =
                static_cast<float>(m_rect.top - m_nUpRectOffset - m_rBorderOffset.top + m_vertOffset * upIdx);
            pv[13].pos.y = pv[15].pos.y = static_cast<float>(m_rect.top - m_nUpRectOffset + m_rBorderOffset.bottom +
                                                             m_vertOffset * (downIdx + 1));
        }
        m_rs->UnLockVertexBuffer(m_idVBuf);
    }

    if (bChange)
    {
        core.Event("evntUpdateFormtText", "sll", m_nodeName, m_listCur->strGroup, m_listCur->strNum);
        core.Event("FTChange", "sf", m_nodeName, GetCurPos());
        ScrollerUpdate();
    }
}

void CXI_FORMATEDTEXT::MouseThis(float fX, float fY)
{
    return;
    if (!m_bSelectableCursor)
        return;
    int32_t nNum = static_cast<int32_t>((fY - m_rect.top) / m_vertOffset + .1f);
    if (nNum < 0 || nNum >= m_allStrings)
        return;

    STRING_DESCRIBER *pdescr = m_listCur;
    for (int i = 0; i < nNum && pdescr != nullptr; i++)
        pdescr = pdescr->next;
    if (pdescr != nullptr && m_nCurGroupNum != pdescr->strGroup)
        SetVertexToNewGroup(true, FindUpGroup(pdescr->strGroup), FindDownGroup(pdescr->strGroup));
    if (m_nStringGroupQuantity > 0)
        core.Event("SetScrollerPos", "sf", m_nodeName,
                   static_cast<float>(m_nCurGroupNum) / static_cast<float>(m_nStringGroupQuantity));
    else
        core.Event("SetScrollerPos", "sf", m_nodeName, 0.f);
    core.Event("FTChange", "sf", m_nodeName, GetCurPos());
    ScrollerUpdate();
}

void CXI_FORMATEDTEXT::CheckScrollButtons()
{
    const bool oldUp = m_bUpEnable;
    const bool oldDown = m_bDownEnable;
    if (m_listCur == nullptr)
    {
        m_bUpEnable = m_bDownEnable = false;
    }
    else
    {
        m_bUpEnable = m_listCur->prev != nullptr;
        STRING_DESCRIBER *psd = m_listCur;
        for (int i = 0; i < m_allStrings && psd != nullptr; i++, psd = psd->next)
            ;
        m_bDownEnable = psd != nullptr;
    }

    if (oldUp == m_bUpEnable && oldDown == m_bDownEnable)
        return;
    if (m_idUpEnableTexture == -1 && m_idUpDisableTexture == -1 && m_idDownEnableTexture == -1 &&
        m_idDownDisableTexture == -1)
        return;

    auto *pv = static_cast<XI_ONLYONETEX_VERTEX *>(m_rs->LockVertexBuffer(m_idVBuf));
    if (pv != nullptr)
    {
        if (oldUp != m_bUpEnable)
            if (m_bUpEnable)
            {
                if (m_idUpEnableTexture >= 0)
                {
                    pv[4].tu = pv[5].tu = m_frUpEnableUV.left;
                    pv[6].tu = pv[7].tu = m_frUpEnableUV.right;
                    pv[4].tv = pv[6].tv = m_frUpEnableUV.top;
                    pv[5].tv = pv[7].tv = m_frUpEnableUV.bottom;
                }
            }
            else
            {
                if (m_idUpDisableTexture >= 0)
                {
                    pv[4].tu = pv[5].tu = m_frUpDisableUV.left;
                    pv[6].tu = pv[7].tu = m_frUpDisableUV.right;
                    pv[4].tv = pv[6].tv = m_frUpDisableUV.top;
                    pv[5].tv = pv[7].tv = m_frUpDisableUV.bottom;
                }
            }
        if (oldDown != m_bDownEnable)
            if (m_bDownEnable)
            {
                if (m_idDownEnableTexture >= 0)
                {
                    pv[8].tu = pv[9].tu = m_frDownEnableUV.left;
                    pv[10].tu = pv[11].tu = m_frDownEnableUV.right;
                    pv[8].tv = pv[10].tv = m_frDownEnableUV.top;
                    pv[9].tv = pv[11].tv = m_frDownEnableUV.bottom;
                }
            }
            else
            {
                if (m_idDownDisableTexture >= 0)
                {
                    pv[8].tu = pv[9].tu = m_frDownDisableUV.left;
                    pv[10].tu = pv[11].tu = m_frDownDisableUV.right;
                    pv[8].tv = pv[10].tv = m_frDownDisableUV.top;
                    pv[9].tv = pv[11].tv = m_frDownDisableUV.bottom;
                }
            }
        m_rs->UnLockVertexBuffer(m_idVBuf);
    }
}

void CXI_FORMATEDTEXT::SetSpecialStrings(ATTRIBUTES *pARoot)
{
    if (pARoot == nullptr)
        return;

    // delete old lines
    while (m_listRoot != nullptr)
    {
        m_listCur = m_listRoot;
        m_listRoot = m_listRoot->next;
        STORM_DELETE(m_listCur->lineStr);
        STORM_DELETE(m_listCur);
    }
    m_nStringGroupQuantity = 0;
    m_nAllTextStrings = 0;

    const int q = pARoot->GetAttributesNum();
    for (int i = 0; i < q; i++)
    {
        ATTRIBUTES *pA = pARoot->GetAttributeClass(i);
        if (pA == nullptr)
            continue;
        const char *tmpstr = pA->GetAttribute("str");
        if (tmpstr == nullptr)
            continue;
        const int pos = pA->GetAttributeAsDword("pos", -1);
        while (pos > m_nAllTextStrings)
            AddFormatedText("\n");
        if (pos >= 0)
        {
            const int oldCompareWidth = m_nCompareWidth; // to avoid hyphenation
            m_nCompareWidth = 1000;
            AddFormatedText(tmpstr);
            m_nCompareWidth = oldCompareWidth;
        }
    }
}

void CXI_FORMATEDTEXT::ControlSyncronouseNodes()
{
    for (int32_t n = 0; n < static_cast<int32_t>(m_asSyncNodes.size()); n++)
    {
        CINODE *pNode = static_cast<XINTERFACE *>(core.GetEntityPointer(g_idInterface))
                            ->FindNode(m_asSyncNodes[n].c_str(), nullptr);
        if (!pNode)
            continue;
        switch (pNode->m_nNodeType)
        {
        case NODETYPE_FORMATEDTEXTS:
            static_cast<CXI_FORMATEDTEXT *>(pNode)->SetCurrentGroupNum(GetFirstGroupNum(), m_nCurGroupNum);
            break;

        default:
            core.Trace("Warning! Control %s owned not legal type of control (%s).", m_nodeName, pNode->m_nodeName);
        }
    }
}

int32_t CXI_FORMATEDTEXT::GetFirstGroupNum()
{
    if (!m_listCur)
        return 0;
    return m_listCur->strGroup;
}

void CXI_FORMATEDTEXT::SetCurrentGroupNum(int32_t nFirstNum, int32_t nSelectNum)
{
    STRING_DESCRIBER *pSD;
    for (pSD = m_listRoot; pSD; pSD = pSD->next)
        if (pSD->strGroup == nFirstNum)
            break;
    if (pSD)
        m_listCur = pSD;

    SetVertexToNewGroup(true, FindUpGroup(nSelectNum), FindDownGroup(nSelectNum));
    // m_nCurGroupNum
}

void CXI_FORMATEDTEXT::ReplaceString(int32_t nGrpNum, const char *pSrcStr)
{
    if (nGrpNum < 0)
        return;
    if (nGrpNum > m_nStringGroupQuantity)
        nGrpNum = m_nStringGroupQuantity;

    if (m_listCur && m_listCur->strGroup == nGrpNum)
        m_listCur = nullptr;

    STRING_DESCRIBER *dscrCur;
    for (dscrCur = m_listRoot; dscrCur; dscrCur = dscrCur->next)
        if (dscrCur->strGroup == nGrpNum)
            break;

    if (!dscrCur)
    {
        core.Trace("Can`t find text group %d into control: %s", nGrpNum, m_nodeName);
        return;
    }

    const uint32_t dwOldColor = dscrCur->color;

    while (dscrCur && dscrCur->strGroup == nGrpNum)
    {
        STRING_DESCRIBER *pTmp = dscrCur;
        dscrCur = dscrCur->next;
        ReleaseString(pTmp);
    }

    if (!dscrCur)
    {
        m_nStringGroupQuantity--;
        AddFormatedText(pSrcStr);
        for (dscrCur = m_listRoot; dscrCur; dscrCur = dscrCur->next)
            if (dscrCur->strGroup == nGrpNum)
                dscrCur->color = dwOldColor;
        return;
    }

    // insert new
    InsertStringBefore(dscrCur, pSrcStr, nGrpNum, dwOldColor);
    if (!m_listCur)
    {
        m_listCur = dscrCur;
        SetVertexToNewGroup(false, FindUpGroup(nGrpNum), FindDownGroup(nGrpNum));
    }
}

void CXI_FORMATEDTEXT::InsertStringBefore(STRING_DESCRIBER *pNextDescr, const char *pSrcStr, int32_t nGrpNum,
                                          uint32_t dwColor)
{
    if (!pNextDescr)
        return;

    char newStr[512];
    auto pstr = pSrcStr;
    int32_t textQ = 0;

    // decompose the resulting string into lines that fit into the output area
    while (GetLineNext(m_idFont, pstr, newStr, sizeof(newStr)))
    {
        auto *pNewDescr = new STRING_DESCRIBER(newStr);
        if (!pNewDescr)
        {
            throw std::runtime_error("allocate memory error");
        }

        pNewDescr->strNum = -1;
        pNewDescr->strGroup = nGrpNum;
        pNewDescr->prev = pNextDescr->prev;
        pNewDescr->next = pNextDescr;
        pNewDescr->color = dwColor;

        if (pNextDescr->prev)
            pNextDescr->prev->next = pNewDescr;
        else
            m_listRoot = pNewDescr;
        pNextDescr->prev = pNewDescr;

        textQ++;
    }

    RecalculateStringNumber();
}

void CXI_FORMATEDTEXT::RecalculateStringNumber()
{
    m_nAllTextStrings = 0;
    for (STRING_DESCRIBER *pDescr = m_listRoot; pDescr; pDescr = pDescr->next)
    {
        pDescr->strNum = m_nAllTextStrings;
        m_nAllTextStrings++;
    }
}

XYRECT CXI_FORMATEDTEXT::GetCursorRect()
{
    return m_rectCursorPosition;
}

void CXI_FORMATEDTEXT::SetCurLine(STRING_DESCRIBER *pNewCurLine)
{
    if (!pNewCurLine)
        return;
    m_listCur = pNewCurLine;

    if (!m_bSelectableCursor)
    {
        if (m_nAllTextStrings > 0 && m_listCur != nullptr)
            core.Event("SetScrollerPos", "sf", m_nodeName,
                       static_cast<float>(m_listCur->strNum) / static_cast<float>(m_nAllTextStrings));
        else
            core.Event("SetScrollerPos", "sf", m_nodeName, 0.f);
        core.Event("FTChange", "sf", m_nodeName, GetCurPos());
    }
    ScrollerUpdate();
}

void CXI_FORMATEDTEXT::ScrollerUpdate()
{
    if (!m_sScrollerName)
        return;
    CINODE *pNode =
        static_cast<XINTERFACE *>(core.GetEntityPointer(g_idInterface))->FindNode(m_sScrollerName, nullptr);
    if (!pNode)
        return;

    if (pNode->m_nNodeType != NODETYPE_SCROLLER)
    {
        core.Trace("Warning! ScrollerUpdate call for not scroller control (%s).", pNode->m_nodeName);
        return;
    }
    static_cast<CXI_SCROLLER *>(pNode)->LinkNodeChanged(GetCurPos());
}

void CXI_FORMATEDTEXT::VAlignment(int32_t nAlign)
{
    if (nAlign == 1)
    {
        if (m_nAllTextStrings > 0 && m_nAllTextStrings < m_allStrings)
        {
            m_nVAlignmentOffset = (m_rect.bottom - m_rect.top) - m_nAllTextStrings * m_vertOffset;
            m_nVAlignmentOffset /= 2;
        }
    }
}
