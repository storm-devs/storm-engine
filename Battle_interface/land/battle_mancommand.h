#ifndef _BATTLE_MANCOMMAND_H_
#define _BATTLE_MANCOMMAND_H_

#include "..\battle_command.h"

class BIManCommandList : public BICommandList
{
public:
	BIManCommandList( ENTITY_ID& eid, ATTRIBUTES* pA, VDX8RENDER* rs );
	virtual ~BIManCommandList();

	virtual void FillIcons();
	virtual void Init();

protected:
	void Release();

	long CommandAdding();
	long UserIconsAdding();
	long AbilityAdding();
	long AddCancelIcon();
};

#endif
