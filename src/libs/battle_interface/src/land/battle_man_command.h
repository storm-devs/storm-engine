#pragma once

#include "../battle_command.h"

class BIManCommandList : public BICommandList
{
  public:
    BIManCommandList(entid_t eid, ATTRIBUTES *pA, VDX9RENDER *rs);
    ~BIManCommandList() override;

    void FillIcons() override;
    void Init() override;

  protected:
    void Release();

    int32_t CommandAdding();
    int32_t UserIconsAdding();
    int32_t AbilityAdding();
    int32_t AddCancelIcon();
};
