#pragma once

#include "BaseGroup.h"

class BI_SeaGroup : public BI_BaseGroup
{
  public:
    BI_SeaGroup(BI_ManagerBase *pManager);
    virtual ~BI_SeaGroup();

    void Init() override;

    void Update() override
    {
    }

    long Event(const char *pcEventName) override
    {
        return 0;
    }

  protected:
};
