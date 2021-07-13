#pragma once

#include "../BaseManager.h"

class BI_ManagerBase;

class BI_BaseNode : public BI_ManagerNodeBase
{
  public:
    BI_BaseNode(BI_ManagerBase *pManager);
    virtual ~BI_BaseNode();

    void Update() override;

    long Event(const char *pcEventName) override
    {
        return 0;
    }

    bool IsPointInside(float fX, float fY) override
    {
        return false;
    }
    bool IsMouseClickReaction() override
    {
        return false;
    }
    bool IsMouseMoveReaction() override
    {
        return false;
    }

    BI_ManagerBase *Manager() const
    {
        return m_pManager;
    }
    VDX9RENDER *Render() const
    {
        return m_pRS;
    }

  protected:
    BI_ManagerBase *m_pManager;
    VDX9RENDER *m_pRS;
};
