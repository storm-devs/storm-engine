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

    long CommandAdding();
    long UserIconsAdding();
    long AbilityAdding();
    long AddCancelIcon();
};
