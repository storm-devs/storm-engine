#ifndef _BATTLE_SHIPCOMMAND_H_
#define _BATTLE_SHIPCOMMAND_H_

#include "..\battle_command.h"

class BIShipCommandList : public BICommandList
{
public:
	BIShipCommandList( ENTITY_ID& eid, ATTRIBUTES* pA, VDX8RENDER* rs );
	virtual ~BIShipCommandList();

	virtual void FillIcons();
	virtual void Init();

protected:
	void Release();

	long ShipAdding( bool allLabel, bool bMyShip, bool bEnemy, bool bNeutral, bool bFriend );
	long FortAdding(bool allLabel, bool bFriend, bool bNeutral, bool bEnemy );
	long LandAdding(bool allLabel);
	long CommandAdding();
	long ChargeAdding();
	long UserIconsAdding();
	long AbilityAdding();
	long AddCancelIcon();
	long TownAdding( bool allLabel, bool bDiseased,bool bNotDiseased, bool bEnemy,bool bNeutral,bool bFriend );

	void AddFlagPictureToIcon( long nCharIdx );

	long m_nChargeTextureNum;
	long m_nCommandTextureNum;

	array<long> m_aChargeQuantity;
};

#endif
