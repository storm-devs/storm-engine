#include "xi_window.h"
#include <stdio.h>

CXI_WINDOW::CXI_WINDOW()
{
    m_nNodeType = NODETYPE_WINDOW;

    m_bActive = true;
    m_bShow = true;
}

CXI_WINDOW::~CXI_WINDOW()
{
}

bool CXI_WINDOW::Init(INIFILE *ini1, const char *name1, INIFILE *ini2, const char *name2, VDX9RENDER *rs,
                      XYRECT &hostRect, XYPOINT &ScreenSize)
{
    if (!CINODE::Init(ini1, name1, ini2, name2, rs, hostRect, ScreenSize))
        return false;
    SetGlowCursor(false);
    return true;
}

void CXI_WINDOW::ChangePosition(XYRECT &rNewPos)
{
    if (m_rect.right - m_rect.left != rNewPos.right - rNewPos.left)
        rNewPos.right += rNewPos.left + m_rect.right - m_rect.left;
    if (m_rect.bottom - m_rect.top != rNewPos.bottom - rNewPos.top)
        rNewPos.bottom += rNewPos.top + m_rect.bottom - m_rect.top;
    if (rNewPos.top == m_rect.top && rNewPos.left == m_rect.left)
        return; // nothing to change - the same position
    const auto nXAdd = rNewPos.left - m_rect.left;
    const auto nYAdd = rNewPos.top - m_rect.top;
    m_rect = rNewPos;

    for (int32_t n = 0; n < m_aNodeNameList.size(); n++)
    {
        auto *pNod = ptrOwner->FindNode(m_aNodeNameList[n].c_str(), nullptr);
        if (pNod)
        {
            auto r = pNod->m_rect;
            r.left += nXAdd;
            r.right += nXAdd;
            r.top += nYAdd;
            r.bottom += nYAdd;
            pNod->ChangePosition(r);
        }
    }
}

void CXI_WINDOW::SaveParametersToIni()
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

    for (int32_t n = 0; n < m_aNodeNameList.size(); n++)
    {
        auto *pNod = ptrOwner->FindNode(m_aNodeNameList[n].c_str(), nullptr);
        if (pNod)
            pNod->SaveParametersToIni();
    }
}

void CXI_WINDOW::SetShow(bool bShow)
{
    if (bShow == m_bShow)
        return;
    if (!bShow && m_bActive)
        SetActive(false);
    m_bShow = bShow;

    // walk through all nodes and turn them on / off
    for (int32_t n = 0; n < m_aNodeNameList.size(); n++)
    {
        auto *const pNod = ptrOwner->FindNode(m_aNodeNameList[n].c_str(), nullptr);
        if (pNod)
        {
            pNod->m_bUse = bShow;
            if (pNod->m_nNodeType == NODETYPE_WINDOW)
                static_cast<CXI_WINDOW *>(pNod)->SetShow(bShow);
        }
    }
}

void CXI_WINDOW::SetActive(bool bActive)
{
    if (m_bActive == bActive)
        return;
    m_bActive = bActive;

    // pass through all nodes and lock / unlock them
    for (int32_t n = 0; n < m_aNodeNameList.size(); n++)
    {
        auto *const pNod = ptrOwner->FindNode(m_aNodeNameList[n].c_str(), nullptr);
        if (pNod)
        {
            pNod->m_bLockedNode = !bActive;
            if (pNod->m_nNodeType == NODETYPE_WINDOW)
                static_cast<CXI_WINDOW *>(pNod)->SetActive(bActive);
        }
    }
}

void CXI_WINDOW::AddNode(const char *pcNodeName)
{
    auto *pNod = ptrOwner->FindNode(pcNodeName, nullptr);
    if (!pNod)
    {
        core.Trace("Warning! CXI_WINDOW::AddNode(%s) : Node not found", pcNodeName);
        return;
    }
    m_aNodeNameList.push_back(pcNodeName);

    pNod->m_bUse = m_bShow;
    pNod->m_bLockedNode = !m_bActive;

    XYRECT r = pNod->m_rect;
    r.left += m_rect.left;
    r.right += m_rect.left;
    r.top += m_rect.top;
    r.bottom += m_rect.top;
    pNod->ChangePosition(r);
}

void CXI_WINDOW::LoadIni(INIFILE *ini1, const char *name1, INIFILE *ini2, const char *name2)
{
    char param[1024];

    // get nodes list
    // if( ReadIniString(ini1,name1, ini2,name2, "nodelist", param, sizeof(param),"") )
    if (ini1 && ini1->ReadString(name1, "nodelist", param, sizeof(param), ""))
        do
        {
            const char *pcStr = param;
            char subparam[256];
            while (pcStr && pcStr[0])
            {
                pcStr = GetSubStr(pcStr, subparam, sizeof(subparam));
                m_aNodeNameList.push_back(subparam);
            }
        } while (ini1->ReadStringNext(name1, "nodelist", param, sizeof(param)));

    // get active value
    SetActive(GetIniBool(ini1, name1, ini2, name2, "active", true));

    // get show value
    SetShow(GetIniBool(ini1, name1, ini2, name2, "show", true));
}
