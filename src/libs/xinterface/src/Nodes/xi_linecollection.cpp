#include "xi_linecollection.h"
#include <stdio.h>

CXI_LINECOLLECTION::CXI_LINECOLLECTION()
{
    m_rs = nullptr;
    m_nNodeType = NODETYPE_LINECOLLECTION;
}

CXI_LINECOLLECTION::~CXI_LINECOLLECTION()
{
    ReleaseAll();
}

int CXI_LINECOLLECTION::CommandExecute(int wActCode)
{
    return -1;
}

void CXI_LINECOLLECTION::Draw(bool bSelected, uint32_t Delta_Time)
{
    if (m_bUse)
    {
        m_rs->DrawLines(m_aLines.data(), m_aLines.size() / 2, "iLineCollection");
    }
}

bool CXI_LINECOLLECTION::Init(INIFILE *ini1, const char *name1, INIFILE *ini2, const char *name2, VDX9RENDER *rs,
                              XYRECT &hostRect, XYPOINT &ScreenSize)
{
    if (!CINODE::Init(ini1, name1, ini2, name2, rs, hostRect, ScreenSize))
        return false;
    // screen position for that is host screen position
    memcpy(&m_rect, &m_hostRect, sizeof(m_hostRect));
    SetGlowCursor(false);
    return true;
}

void CXI_LINECOLLECTION::LoadIni(INIFILE *ini1, const char *name1, INIFILE *ini2, const char *name2)
{
    char param[256];
    char param1[256];

    // fill lines structure array
    const auto bRelativeRect = !GetIniLong(ini1, name1, ini2, name2, "bAbsoluteRectangle", 0);
    auto nCurLine = 0;
    if (ini1->ReadString(name1, "line", param, sizeof(param) - 1, ""))
        do
        {
            XYRECT scrRect;
            uint32_t dwCol = 0;
            if (GetMidStr(param, param1, sizeof(param1), "(", ")-("))
                GetDataStr(param1, "ll", &scrRect.left, &scrRect.top);
            if (GetMidStr(param, param1, sizeof(param1), ")-(", ")"))
                GetDataStr(param1, "ll", &scrRect.right, &scrRect.bottom);
            if (GetMidStr(param, param1, sizeof(param1), "col:{", "}"))
            {
                dwCol = GetColorFromStr(param1, dwCol);
            }
            if (bRelativeRect)
                GetRelativeRect(scrRect);

            // long n = m_aLines.Add();
            // m_aLines.Add();
            // m_aLines[n].dwColor = m_aLines[n+1].dwColor = dwCol;
            // m_aLines[n].vPos.z = m_aLines[n+1].vPos.z = 1.f;
            // m_aLines[n].vPos.x = (float)scrRect.left; m_aLines[n+1].vPos.x = (float)scrRect.right;
            // m_aLines[n].vPos.y = (float)scrRect.top;  m_aLines[n+1].vPos.y = (float)scrRect.bottom;
            m_aLines.push_back(
                RS_LINE{CVECTOR{static_cast<float>(scrRect.left), static_cast<float>(scrRect.top), 1.f}, dwCol});
            m_aLines.push_back(
                RS_LINE{CVECTOR{static_cast<float>(scrRect.right), static_cast<float>(scrRect.bottom), 1.f}, dwCol});

            nCurLine++;
        } while (ini1->ReadStringNext(name1, "line", param, sizeof(param) - 1));
}

void CXI_LINECOLLECTION::ReleaseAll()
{
    m_aLines.clear();
}

bool CXI_LINECOLLECTION::IsClick(int buttonID, long xPos, long yPos)
{
    return false;
}

void CXI_LINECOLLECTION::ChangePosition(XYRECT &rNewPos)
{
    // no this action
}

void CXI_LINECOLLECTION::SaveParametersToIni()
{
    //    char pcWriteParam[2048];

    auto pIni = fio->OpenIniFile(ptrOwner->m_sDialogFileName.c_str());
    if (!pIni)
    {
        core.Trace("Warning! Can`t open ini file name %s", ptrOwner->m_sDialogFileName.c_str());
        return;
    }

    // save position
    //    sprintf_s( pcWriteParam, sizeof(pcWriteParam), "%d,%d,%d,%d", m_rect.left, m_rect.top, m_rect.right,
    // m_rect.bottom );     pIni->WriteString( m_nodeName, "position", pcWriteParam );
}

uint32_t CXI_LINECOLLECTION::MessageProc(long msgcode, MESSAGE &message)
{
    switch (msgcode)
    {
    case 0: // change color for line with number or all lines (if number = -1)
    {
        const uint32_t dwColor = message.Long();
        const auto nLineNum = message.Long();
        if (nLineNum < 0 || nLineNum >= static_cast<long>(m_aLines.size()) / 2)
        {
            for (long n = 0; n < m_aLines.size(); n++)
                m_aLines[n].dwColor = dwColor;
        }
        else
        {
            m_aLines[nLineNum * 2].dwColor = m_aLines[nLineNum * 2 + 1].dwColor = dwColor;
        }
    }
    break;
    case 1: // add line and return its number
    {
        const uint32_t dwColor = message.Long();
        const auto nLeft = message.Long();
        const auto nTop = message.Long();
        const auto nRight = message.Long();
        const long nBottom = message.Long();
        // long nLineNum = m_aLines.Add() / 2;
        // m_aLines.Add();
        const long nLineNum = m_aLines.size() / 2;
        m_aLines.resize(m_aLines.size() + 2);
        m_aLines[nLineNum * 2].dwColor = m_aLines[nLineNum * 2 + 1].dwColor = dwColor;
        m_aLines[nLineNum * 2].vPos.z = m_aLines[nLineNum * 2 + 1].vPos.z = 1.f;
        m_aLines[nLineNum * 2].vPos.x = static_cast<float>(nLeft);
        m_aLines[nLineNum * 2 + 1].vPos.x = static_cast<float>(nRight);
        m_aLines[nLineNum * 2].vPos.y = static_cast<float>(nTop);
        m_aLines[nLineNum * 2 + 1].vPos.y = static_cast<float>(nBottom);
        return nLineNum;
    }
    break;
    }
    return 0;
}
