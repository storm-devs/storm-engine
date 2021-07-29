#include "ControlTree.h"

#include "core.h"

ControlTree::ControlChild::ControlChild()
    : bWaitReleaseControl(false)
{
    index = -1;
    sControlName = "";
    bActive = false;
    fTimeOut = 0.05f; // boal fix  // 0.5f;
    fCurTime = 0.f;
    sOutControlName = "";
    nLayer = 0;
}

ControlTree::ControlChild::~ControlChild()
{
    index = -1;
    bActive = false;
    fCurTime = 0.f;
    aChild.clear();
}

void ControlTree::ControlChild::Process(float fDeltaTime, ControlTree *pControlTree)
{
    long n;

    // no basic control - always considered active
    // go directly to the child branches
    if (sControlName.empty())
    {
        bActive = true;
        for (n = 0; n < aChild.size(); n++)
            aChild[n].Process(fDeltaTime, pControlTree);
        return;
    }

    CONTROL_STATE cs;
    core.Controls->GetControlState((char *)sControlName.c_str(), cs);

    // if the node is already activated, then calculate the timeout
    if (bActive)
    {
        if (fCurTime <= fTimeOut)
        {
            fCurTime += fDeltaTime;
        }
        /*else
        {
            bActive = false; // boal to allow deactivation
        } */
    }
    else
    // if (! bActive) // boal if not already
    // if the control has not been activated, then check it for activation
    {
        // nothing is pressed - leave
        if (cs.state == CST_INACTIVATED || cs.state == CST_INACTIVE)
            return;

        // there was the first click - activate and set the timeout to the beginning
        bActive = true;
        bWaitReleaseControl = false;
        fCurTime = 0.f;

        for (n = 0; n < aChild.size(); n++)
        {
            aChild[n].bActive = false;
            aChild[n].bWaitReleaseControl = false;
        }

        pControlTree->ControlInAction(sControlName.c_str(), nLayer);
    }

    // timeout passed - turn on the output control
    if ((!bWaitReleaseControl) && fCurTime >= fTimeOut)
    {
        bActive =
            pControlTree->AddOutControl(sOutControlName.c_str(), cs.state == CST_ACTIVATED || cs.state == CST_ACTIVE);
        return;
    }

    // go to child branches
    auto bChildActive = false;
    for (n = 0; n < aChild.size(); n++)
    {
        aChild[n].Process(fDeltaTime, pControlTree);
        if (aChild[n].bActive)
            bChildActive = true;
    }
    // as soon as the child is activated, then the current branch cannot issue control by timeout
    if (bChildActive)
        bWaitReleaseControl = true;

    if (bWaitReleaseControl && !bChildActive)
    {
        if (cs.state == CST_INACTIVE || cs.state == CST_INACTIVATED)
            bActive = false;
    }
}

bool ControlTree::ControlChild::ControlInAction(ControlTree *pControlTree, const char *pcControlName, long _nLayer)
{
    if (_nLayer <= nLayer)
        return false;

    if (sControlName == pcControlName)
    {
        pControlTree->ExcludeControlFromActive(sOutControlName.c_str());
        bWaitReleaseControl = true;
        bActive = true;
        fCurTime = 0.f;
    }

    if (bActive)
    {
        for (long n = 0; n < aChild.size(); n++)
        {
            if (aChild[n].ControlInAction(pControlTree, pcControlName, _nLayer))
                bWaitReleaseControl = true;
        }
    }

    return bWaitReleaseControl;
}

ControlTree::ControlTree()
{
    Init();
}

ControlTree::~ControlTree()
{
    Release();
}

void ControlTree::Process()
{
    long n;

    // all controls that have been deactivated become inactive
    // and all activated become active
    for (n = 0; n < m_aOutControlList.size(); n++)
    {
        if (m_aOutControlList[n].state == CST_INACTIVATED)
            m_aOutControlList[n].state = CST_INACTIVE;
        if (m_aOutControlList[n].state == CST_ACTIVATED)
            m_aOutControlList[n].state = CST_ACTIVE;
    }

    // process the tree
    m_RootControl.Process(core.GetDeltaTime() * 0.001f, this);

    // remove deactivated controls
    for (n = 0; n < m_aOutControlList.size(); n++)
    {
        if (m_aOutControlList[n].state == CST_INACTIVE)
        {
            m_aOutControlList.erase(m_aOutControlList.begin() + n);
            n--;
        }
    }
}

long ControlTree::AddControlChild(long nParentIdx, const char *pcControlName, const char *pcOutControlName,
                                  float fTimeOut)
{
    auto *pParent = FindControlChild(nParentIdx);
    if (!pParent)
        return -1;
    ControlChild child;
    child.bActive = false;
    child.fCurTime = 0.f;
    child.fTimeOut = fTimeOut;
    child.index = m_nControlsNum;
    child.sControlName = pcControlName;
    child.sOutControlName = pcOutControlName;
    child.nLayer = pParent->nLayer + 1;
    pParent->aChild.push_back(child);

    m_nControlsNum++;
    return m_nControlsNum - 1;
}

CONTROL_STATE_TYPE ControlTree::CheckControlActivated(const char *pcControlName)
{
    if (!pcControlName)
        return CST_INACTIVE;
    for (long n = 0; n < m_aOutControlList.size(); n++)
        if (m_aOutControlList[n].sControlName == pcControlName)
            return m_aOutControlList[n].state;
    return CST_INACTIVE;
}

void ControlTree::Init()
{
    m_nControlsNum = 0;
}

void ControlTree::Release()
{
}

ControlTree::ControlChild *ControlTree::FindControlChild(long idx)
{
    if (idx < 0 || idx >= m_nControlsNum)
        return &m_RootControl;
    auto *const pCC = FindControlChild(idx, &m_RootControl);
    if (pCC)
        return pCC;
    return &m_RootControl;
}

ControlTree::ControlChild *ControlTree::FindControlChild(long idx, ControlChild *pParent)
{
    if (!pParent)
        return nullptr;
    if (pParent->index == idx)
        return pParent;
    for (long n = 0; n < pParent->aChild.size(); n++)
    {
        auto *const pCC = FindControlChild(idx, &pParent->aChild[n]);
        if (pCC)
            return pCC;
    }
    return nullptr;
}

bool ControlTree::AddOutControl(const char *pcOutControlName, bool isActive)
{
    if (!pcOutControlName || !pcOutControlName[0])
        return false;

    // looking for a control in the list
    long n;
    for (n = 0; n < m_aOutControlList.size(); n++)
        if (m_aOutControlList[n].sControlName == pcOutControlName)
            break;

    // there was no such control - add with the previous state = unpressed
    if (n >= m_aOutControlList.size())
    {
        OutControlInfo info;
        info.sControlName = pcOutControlName;
        info.state = CST_INACTIVE;
        m_aOutControlList.push_back(info);
        isActive = true;
    }

    // control pressed
    const auto cs_prev = m_aOutControlList[n].state;
    if (isActive)
    {
        if (cs_prev == CST_INACTIVE || cs_prev == CST_INACTIVATED)
        {
            m_aOutControlList[n].state = CST_ACTIVATED;
        }
        else
        {
            m_aOutControlList[n].state = CST_ACTIVE;
        }
    }
    else
    {
        if (cs_prev == CST_ACTIVE || cs_prev == CST_ACTIVATED)
        {
            m_aOutControlList[n].state = CST_INACTIVATED;
        }
        else
        {
            m_aOutControlList[n].state = CST_INACTIVE;
        }
    }

    return (m_aOutControlList[n].state == CST_ACTIVE || m_aOutControlList[n].state == CST_ACTIVATED);
}

void ControlTree::ControlInAction(const char *pcControlName, long nLayer)
{
    m_RootControl.ControlInAction(this, pcControlName, nLayer);
}

bool ControlTree::ExcludeControlFromActive(const char *pcControlName)
{
    for (long n = 0; n < m_aOutControlList.size(); n++)
    {
        if (m_aOutControlList[n].sControlName == pcControlName)
        {
            if (m_aOutControlList[n].state == CST_ACTIVATED)
            {
                m_aOutControlList.erase(m_aOutControlList.begin() + n);
                return true;
            }
            return false;
        }
    }
    return true;
}
