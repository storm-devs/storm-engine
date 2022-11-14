#pragma once

#include "../battle_command.h"

class WMShipCommandList : public BICommandList
{
  public:
    WMShipCommandList(entid_t eid, ATTRIBUTES *pA, VDX9RENDER *rs);
    ~WMShipCommandList() override;

    void FillIcons() override;
    void Init() override;

  protected:
    void Release();

    int32_t CommandAdding();
};
