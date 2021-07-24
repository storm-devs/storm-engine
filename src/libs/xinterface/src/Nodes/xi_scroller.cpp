#include "xi_scroller.h"
#include "../xinterface.h"
#include "Entity.h"
#include "xi_formttext.h"
#include "xi_questtexts.h"
#include "xi_questtitles.h"
#include "xi_table.h"
#include <stdio.h>

void SetVertexRectangleTex(XI_ONLYONETEX_VERTEX *pv, FXYRECT &texRect)
{
    pv[0].tu = texRect.left;
    pv[0].tv = texRect.top;
    pv[1].tu = texRect.left;
    pv[1].tv = texRect.bottom;
    pv[2].tu = texRect.right;
    pv[2].tv = texRect.top;
    pv[3].tu = texRect.right;
    pv[3].tv = texRect.bottom;
}

void SetVertexRectanglePos(XI_ONLYONETEX_VERTEX *pv, FXYRECT &posRect)
{
    pv[0].pos.x = posRect.left;
    pv[0].pos.y = posRect.top;
    pv[1].pos.x = posRect.left;
    pv[1].pos.y = posRect.bottom;
    pv[2].pos.x = posRect.right;
    pv[2].pos.y = posRect.top;
    pv[3].pos.x = posRect.right;
    pv[3].pos.y = posRect.bottom;
}

CXI_SCROLLER::CXI_SCROLLER()
{
    m_rs = nullptr;
    m_nNodeType = NODETYPE_SCROLLER;

    m_idBaseTex = -1;
    m_idRollerTex = -1;
    m_idVBuf = -1;
    m_bSelected = true;

    m_bDragRoll = false;
}

CXI_SCROLLER::~CXI_SCROLLER()
{
    ReleaseAll();
}

void CXI_SCROLLER::Draw(bool bSelected, uint32_t Delta_Time)
{
    if (m_bUse)
    {
        if (m_bDragRoll)
        {
            CONTROL_STATE cs;
            core.Controls->GetControlState("ILClick", cs);
            if (cs.state == CST_INACTIVE || cs.state == CST_INACTIVATED)
                m_bDragRoll = false;
            if (m_bDragRoll)
                MouseMove();
        }

        m_rs->TextureSet(0, m_idBaseTex);
        m_rs->DrawPrimitive(D3DPT_TRIANGLESTRIP, m_idVBuf, sizeof(XI_ONLYONETEX_VERTEX), 0, 2, "iStatusLine");
        m_rs->DrawPrimitive(D3DPT_TRIANGLESTRIP, m_idVBuf, sizeof(XI_ONLYONETEX_VERTEX), 4, 2, "iStatusLine");
        m_rs->DrawPrimitive(D3DPT_TRIANGLESTRIP, m_idVBuf, sizeof(XI_ONLYONETEX_VERTEX), 8, 2, "iStatusLine");
        m_rs->TextureSet(0, m_idRollerTex);
        m_rs->DrawPrimitive(D3DPT_TRIANGLESTRIP, m_idVBuf, sizeof(XI_ONLYONETEX_VERTEX), 12, 2, "iStatusLine");
    }
}

bool CXI_SCROLLER::Init(INIFILE *ini1, const char *name1, INIFILE *ini2, const char *name2, VDX9RENDER *rs,
                        XYRECT &hostRect, XYPOINT &ScreenSize)
{
    if (!CINODE::Init(ini1, name1, ini2, name2, rs, hostRect, ScreenSize))
        return false;
    // screen position for that is host screen position
    // memcpy(&m_rect,&m_hostRect,sizeof(m_hostRect));
    SetGlowCursor(false);
    return true;
}

void CXI_SCROLLER::ReleaseAll()
{
    m_bUse = false;
    TEXTURE_RELEASE(m_rs, m_idBaseTex);
    TEXTURE_RELEASE(m_rs, m_idRollerTex);
    VERTEX_BUFFER_RELEASE(m_rs, m_idVBuf);
    m_asOwnedNodes.clear();
}

int CXI_SCROLLER::CommandExecute(int wActCode)
{
    if (m_bUse)
    {
        switch (wActCode)
        {
        case ACTION_MOUSECLICK:
            if (m_upButtonPos.left <= m_curMousePos.x && m_upButtonPos.right >= m_curMousePos.x &&
                m_upButtonPos.top <= m_curMousePos.y && m_downButtonPos.bottom >= m_curMousePos.y)
            {
                if (m_upButtonPos.bottom >= m_curMousePos.y)
                {
                    // top button
                    UpPress();
                    break;
                }
                if (m_downButtonPos.top <= m_curMousePos.y)
                {
                    // lower button
                    DownPress();
                    break;
                }
                // middle
                CONTROL_STATE cs;
                core.Controls->GetControlState("ILClick", cs);
                if (cs.state == CST_ACTIVATED)
                    m_bDragRoll = true;
                MouseMove();
            }
            break;
        }
    }
    return -1;
}

void CXI_SCROLLER::LoadIni(INIFILE *ini1, const char *name1, INIFILE *ini2, const char *name2)
{
    char param[2048];

    if (ReadIniString(ini1, name1, ini2, name2, "OwnedControl", param, sizeof(param), ""))
    {
        CXI_UTILS::StringFillStringArray(param, m_asOwnedNodes);
    }

    // Get texture name and load that texture
    m_idBaseTex = -1;
    if (ReadIniString(ini1, name1, ini2, name2, "baseTexture", param, sizeof(param), ""))
        m_idBaseTex = m_rs->TextureCreate(param);

    m_idRollerTex = -1;
    if (ReadIniString(ini1, name1, ini2, name2, "rollerTexture", param, sizeof(param), ""))
        m_idRollerTex = m_rs->TextureCreate(param);

    // Set buffers
    m_fOffTexHeight = GetIniFloat(ini1, name1, ini2, name2, "begEndTexSize", 0.f);
    m_fOffHeight = GetIniFloat(ini1, name1, ini2, name2, "begEndBaseSize", 0.f);
    m_rollerHeight = GetIniFloat(ini1, name1, ini2, name2, "rollerHeight", 0.f);

    m_rollerPlace.left = static_cast<float>(m_rect.left);
    m_rollerPlace.right = static_cast<float>(m_rect.right);
    m_rollerPlace.top = m_rect.top + m_fOffHeight;
    m_rollerPlace.bottom = m_rect.bottom - m_fOffHeight;

    m_ScrollTexRect = GetIniFloatRect(ini1, name1, ini2, name2, "scrollTexPos", FXYRECT(0.f, 0.f, 1.f, 1.f));
    m_RollTexRect = GetIniFloatRect(ini1, name1, ini2, name2, "rollTexPos", FXYRECT(0.f, 0.f, 1.f, 1.f));

    m_idVBuf = m_rs->CreateVertexBuffer(XI_ONLYONETEX_FVF, 16 * sizeof(XI_ONLYONETEX_VERTEX), D3DUSAGE_WRITEONLY);
    FillVertexBuffer();
}

void CXI_SCROLLER::MakeOwnedControl()
{
    for (long n = 0; n < static_cast<long>(m_asOwnedNodes.size()); n++)
    {
        auto *const pNode = static_cast<XINTERFACE *>(EntityManager::GetEntityPointer(g_idInterface))
                                ->FindNode(m_asOwnedNodes[n].c_str(), nullptr);
        if (!pNode)
            continue;
        switch (pNode->m_nNodeType)
        {
        case NODETYPE_FORMATEDTEXTS:
            static_cast<CXI_FORMATEDTEXT *>(pNode)->SetPointer(m_fPos);
            break;
        case NODETYPE_TABLE:
            static_cast<CXI_TABLE *>(pNode)->ScrollerChanged(m_fPos);
            break;
        case NODETYPE_QTEXTS:
            static_cast<CXI_QUESTTEXTS *>(pNode)->ScrollerChanged(m_fPos);
            break;
        case NODETYPE_QTITLE:
            static_cast<CXI_QUESTTITLE *>(pNode)->ScrollerChanged(m_fPos);
            break;

        default:
            core.Trace("Warning! Control %s owned not legal type of control (%s).", m_nodeName, pNode->m_nodeName);
        }
    }
}

void CXI_SCROLLER::UpPress()
{
    core.Event("ScrollTopChange", "l", -1);
    const auto fDelta = GetOwnedStep();
    if (fDelta != 0.f)
    {
        SetRollerPos(m_fPos - fDelta);
        MakeOwnedControl();
    }
}

void CXI_SCROLLER::DownPress()
{
    core.Event("ScrollTopChange", "l", 1);
    const auto fDelta = GetOwnedStep();
    if (fDelta != 0.f)
    {
        SetRollerPos(m_fPos + fDelta);
        MakeOwnedControl();
    }
}

float CXI_SCROLLER::GetOwnedStep()
{
    CINODE *pNode = nullptr;
    for (long n = 0; n < static_cast<long>(m_asOwnedNodes.size()); n++)
    {
        pNode = static_cast<XINTERFACE *>(EntityManager::GetEntityPointer(g_idInterface))
                    ->FindNode(m_asOwnedNodes[n].c_str(), nullptr);
        if (pNode)
            break;
    }
    if (!pNode)
        return 0.f;
    switch (pNode->m_nNodeType)
    {
    case NODETYPE_FORMATEDTEXTS:
        return static_cast<CXI_FORMATEDTEXT *>(pNode)->GetLineStep();
        break;
    case NODETYPE_TABLE:
        return static_cast<CXI_TABLE *>(pNode)->GetLineStep();
        break;
    case NODETYPE_QTEXTS:
        return static_cast<CXI_QUESTTEXTS *>(pNode)->GetLineStep();
        break;
    case NODETYPE_QTITLE:
        return static_cast<CXI_QUESTTITLE *>(pNode)->GetLineStep();
        break;

    default:
        core.Trace("Warning! Control %s owned not legal type of control (%s).", m_nodeName, pNode->m_nodeName);
    }
    return 0.f;
}

bool CXI_SCROLLER::IsClick(int buttonID, long xPos, long yPos)
{
    if (xPos >= m_rect.left && xPos <= m_rect.right && yPos >= m_rect.top && yPos <= m_rect.bottom)
    {
        if (!m_bDragRoll)
        {
            m_curMousePos.x = static_cast<float>(xPos);
            m_curMousePos.y = static_cast<float>(yPos);
        }
        return true;
    }
    return false;
}

void CXI_SCROLLER::ChangePosition(XYRECT &rNewPos)
{
    m_rect = rNewPos;

    m_rollerPlace.left = static_cast<float>(m_rect.left);
    m_rollerPlace.right = static_cast<float>(m_rect.right);
    m_rollerPlace.top = m_rect.top + m_fOffHeight;
    m_rollerPlace.bottom = m_rect.bottom - m_fOffHeight;

    FillVertexBuffer();
}

void CXI_SCROLLER::SaveParametersToIni()
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

void CXI_SCROLLER::SetRollerPos(float pos)
{
    if (pos < 0.f)
        pos = 0.f;
    if (pos > 1.f)
        pos = 1.f;
    m_fPos = pos;

    m_rollerCur.top = m_rollerPlace.top + m_fPos * (m_rollerPlace.bottom - m_rollerPlace.top - m_rollerHeight);
    m_rollerCur.bottom = m_rollerCur.top + m_rollerHeight;

    auto *pV = static_cast<XI_ONLYONETEX_VERTEX *>(m_rs->LockVertexBuffer(m_idVBuf));
    if (pV != nullptr)
    {
        SetVertexRectanglePos(&pV[12], m_rollerCur);
        m_rs->UnLockVertexBuffer(m_idVBuf);
    }
}

void CXI_SCROLLER::LinkNodeChanged(float fPos)
{
    if (m_fPos == fPos)
        return;
    SetRollerPos(fPos);
    core.Event("ScrollPosChange", "fs", fPos, m_nodeName);
}

void CXI_SCROLLER::FillVertexBuffer()
{
    auto *pV = static_cast<XI_ONLYONETEX_VERTEX *>(m_rs->LockVertexBuffer(m_idVBuf));
    if (pV != nullptr)
    {
        for (auto i = 0; i < 16; i++)
            pV[i].pos.z = 1.f;

        FXYRECT texRect;
        texRect.left = m_ScrollTexRect.left;
        texRect.top = m_ScrollTexRect.top + m_fOffTexHeight;
        texRect.right = m_ScrollTexRect.right;
        texRect.bottom = m_ScrollTexRect.bottom - m_fOffTexHeight;
        SetVertexRectangleTex(&pV[0], texRect);

        texRect.left = m_ScrollTexRect.left;
        texRect.top = m_ScrollTexRect.top;
        texRect.right = m_ScrollTexRect.right;
        texRect.bottom = m_ScrollTexRect.top + m_fOffTexHeight;
        SetVertexRectangleTex(&pV[4], texRect);

        texRect.left = m_ScrollTexRect.left;
        texRect.top = m_ScrollTexRect.bottom - m_fOffTexHeight;
        texRect.right = m_ScrollTexRect.right;
        texRect.bottom = m_ScrollTexRect.bottom;
        SetVertexRectangleTex(&pV[8], texRect);

        texRect.left = m_RollTexRect.left;
        texRect.top = m_RollTexRect.top;
        texRect.right = m_RollTexRect.right;
        texRect.bottom = m_RollTexRect.bottom;
        SetVertexRectangleTex(&pV[12], texRect);

        FXYRECT baseRect;
        baseRect.left = static_cast<float>(m_rect.left);
        baseRect.top = static_cast<float>(m_rect.top) + m_fOffHeight;
        baseRect.right = static_cast<float>(m_rect.right);
        baseRect.bottom = static_cast<float>(m_rect.bottom) - m_fOffHeight;
        SetVertexRectanglePos(&pV[0], baseRect);

        m_upButtonPos.left = static_cast<float>(m_rect.left);
        m_upButtonPos.top = static_cast<float>(m_rect.top);
        m_upButtonPos.right = static_cast<float>(m_rect.right);
        m_upButtonPos.bottom = static_cast<float>(m_rect.top) + m_fOffHeight;
        SetVertexRectanglePos(&pV[4], m_upButtonPos);

        m_downButtonPos.left = static_cast<float>(m_rect.left);
        m_downButtonPos.top = static_cast<float>(m_rect.bottom) - m_fOffHeight;
        m_downButtonPos.right = static_cast<float>(m_rect.right);
        m_downButtonPos.bottom = static_cast<float>(m_rect.bottom);
        SetVertexRectanglePos(&pV[8], m_downButtonPos);

        m_rollerCur.left = m_rollerPlace.left;
        m_rollerCur.top = m_rollerPlace.top;
        m_rollerCur.right = m_rollerPlace.right;
        m_rollerCur.bottom = m_rollerPlace.top + m_rollerHeight;
        SetVertexRectanglePos(&pV[12], m_rollerCur);

        m_rs->UnLockVertexBuffer(m_idVBuf);
    }
}

void CXI_SCROLLER::MouseMove()
{
    const auto pntMouse = ptrOwner->GetMousePoint();
    if (pntMouse.y != m_curMousePos.y)
    {
        m_curMousePos = pntMouse;

        auto fY = pntMouse.y;
        if (fY < m_rollerPlace.top + m_rollerHeight * .5f)
            fY = m_rollerPlace.top + m_rollerHeight * .5f;
        if (fY > m_rollerPlace.bottom - m_rollerHeight * .5f)
            fY = m_rollerPlace.bottom - m_rollerHeight * .5f;

        const float newPos = (fY - m_rollerPlace.top - m_rollerHeight * .5f) /
                             (m_rollerPlace.bottom - m_rollerPlace.top - m_rollerHeight);
        SetRollerPos(newPos);
        core.Event("ScrollPosChange", "fs", newPos, m_nodeName);
        MakeOwnedControl();
    }
}
