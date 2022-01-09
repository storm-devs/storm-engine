#pragma once

#include "base_group.h"

class BI_SeaGroup : public BI_BaseGroup
{
  public:
    BI_SeaGroup(BI_ManagerBase *pManager);
    ~BI_SeaGroup() override;

    void Init() override;

    void Update() override
    {
    }

    int32_t Event(const char *pcEventName) override
    {
        return 0;
    }

  protected:
};
