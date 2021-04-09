#pragma once

#include "../battle_command.h"

class BIShipCommandList : public BICommandList
{
  public:
    BIShipCommandList(entid_t eid, ATTRIBUTES *pA, VDX9RENDER *rs);
    virtual ~BIShipCommandList();

    void FillIcons() override;
    void Init() override;

  protected:
    void Release();

    long ShipAdding(bool allLabel, bool bMyShip, bool bEnemy, bool bNeutral, bool bFriend);
    long FortAdding(bool allLabel, bool bFriend, bool bNeutral, bool bEnemy);
    long LandAdding(bool allLabel);
    long CommandAdding();
    long ChargeAdding();
    long UserIconsAdding();
    long AbilityAdding();
    long AddCancelIcon();
    long TownAdding(bool allLabel, bool bDiseased, bool bNotDiseased, bool bEnemy, bool bNeutral, bool bFriend);

    void AddFlagPictureToIcon(long nCharIdx);

    long m_nChargeTextureNum;
    long m_nCommandTextureNum;

    std::vector<long> m_aChargeQuantity;
};
