#ifndef _BATTLE_MANCOMMAND_H_
#define _BATTLE_MANCOMMAND_H_

#include "../battle_command.h"

class BIManCommandList : public BICommandList
{
  public:
    BIManCommandList(entid_t eid, ATTRIBUTES *pA, VDX9RENDER *rs);
    virtual ~BIManCommandList();

    void FillIcons() override;
    void Init() override;

  protected:
    void Release();

    long CommandAdding();
    long UserIconsAdding();
    long AbilityAdding();
    long AddCancelIcon();
};

#endif
