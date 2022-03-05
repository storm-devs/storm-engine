#pragma once

#include "../base_manager.h"

class BI_ManagerBase;

class BI_BaseNode : public BI_ManagerNodeBase
{
  public:
    BI_BaseNode(BI_ManagerBase *pManager);
    ~BI_BaseNode() override;

    void Update() override;

    int32_t Event(const char *pcEventName) override
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
