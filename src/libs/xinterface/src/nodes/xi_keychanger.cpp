#include "xi_keychanger.h"
#include <stdio.h>

#include "core.h"

#include "v_file_service.h"

#define KEYPRESS_DELAY 500

CXI_KEYCHANGER::CXI_KEYCHANGER()
{
    m_rs = nullptr;
    m_pControlsID = nullptr;
    m_pbControlsStick = nullptr;
    m_nNodeType = NODETYPE_KEYCHANGER;
    m_bKeyCheck = false;
    m_keysQuantity = 0;
}

CXI_KEYCHANGER::~CXI_KEYCHANGER()
{
    ReleaseAll();
}

void CXI_KEYCHANGER::Draw(bool bSelected, uint32_t Delta_Time)
{
    if (m_bUse)
    {
        int i;
        CONTROL_STATE cs;

        for (i = 0; i < m_keysQuantity; i++)
        {
            core.Controls->GetControlState(m_pControlsID[i], cs);
            if (m_bKeyCheck)
            {
                if ((m_pbControlsStick[i] && cs.state == CST_INACTIVATED) ||
                    (!m_pbControlsStick[i] && (cs.fValue > 1.f || cs.fValue < -1.f)))
                {
                    auto bAllowChange = false;
                    auto *pdat = core.Event("evntKeyChoose", "ll", i, cs.fValue > 0);
                    if (pdat != nullptr)
                        bAllowChange = pdat->GetInt() != 0;
                    if (bAllowChange)
                    {
                        m_bUse = false;
                        m_bKeyCheck = false;
                        return;
                    }
                }
            }
            else if (cs.state == CST_ACTIVE || cs.state == CST_ACTIVATED || cs.fValue > 1.f || cs.fValue < -1.f)
                return;
        }
        m_bKeyCheck = true;
    }
}

void CXI_KEYCHANGER::ReleaseAll()
{
    STORM_DELETE(m_pControlsID);
    STORM_DELETE(m_pbControlsStick);
}

void CXI_KEYCHANGER::ChangePosition(XYRECT &rNewPos)
{
    // no this action
}

void CXI_KEYCHANGER::SaveParametersToIni()
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

uint32_t CXI_KEYCHANGER::MessageProc(int32_t msgcode, MESSAGE &message)
{
    switch (msgcode)
    {
    case 0: {
        auto *const pA = message.AttributePointer();
        SetChoosingControls(pA);
    }
    break;
    }

    return 0;
}

void CXI_KEYCHANGER::SetChoosingControls(ATTRIBUTES *pA)
{
    if (pA == nullptr)
        return;

    STORM_DELETE(m_pControlsID);
    STORM_DELETE(m_pbControlsStick);
    m_keysQuantity = pA->GetAttributesNum();
    if (m_keysQuantity <= 0)
        return;

    m_pControlsID = new int32_t[m_keysQuantity];
    m_pbControlsStick = new bool[m_keysQuantity];
    if (m_pControlsID == nullptr || m_pbControlsStick == nullptr)
    {
        throw std::runtime_error("Allocate memory error");
    }

    char contrlName[128];
    for (auto i = 0; i < m_keysQuantity; i++)
    {
        sprintf_s(contrlName, "cntrl_%d", i);
        m_pbControlsStick[i] = false;
        m_pControlsID[i] = core.Controls->CreateControl(contrlName);
        const char *keyCode = pA->GetAttribute(i);
        if (keyCode != nullptr)
        {
            core.Controls->MapControl(m_pControlsID[i], atoi(keyCode));
        }
        auto *pAttr = pA->GetAttributeClass(i);
        if (pAttr != nullptr)
            if (pAttr->GetAttributeAsDword("stick", 0) != 1)
                m_pbControlsStick[i] = true;
    }
}

bool CXI_KEYCHANGER::Init(INIFILE *ini1, const char *name1, INIFILE *ini2, const char *name2, VDX9RENDER *rs,
                          XYRECT &hostRect, XYPOINT &ScreenSize)
{
    SetGlowCursor(false);
    return true;
}
