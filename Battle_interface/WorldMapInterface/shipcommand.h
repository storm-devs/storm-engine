#ifndef _WM_SHIPCOMMAND_H_
#define _WM_SHIPCOMMAND_H_

#include "..\battle_command.h"

class WMShipCommandList : public BICommandList
{
public:
	WMShipCommandList( ENTITY_ID& eid, ATTRIBUTES* pA, VDX8RENDER* rs );
	virtual ~WMShipCommandList();

	virtual void FillIcons();
	virtual void Init();

protected:
	void Release();

	long CommandAdding();
};

#endif
