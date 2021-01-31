#ifndef _WM_SHIPCOMMAND_H_
#define _WM_SHIPCOMMAND_H_

#include "../battle_command.h"

class WMShipCommandList : public BICommandList
{
  public:
    WMShipCommandList(entid_t eid, ATTRIBUTES *pA, VDX9RENDER *rs);
    virtual ~WMShipCommandList();

    void FillIcons() override;
    void Init() override;

  protected:
    void Release();

    long CommandAdding();
};

#endif
