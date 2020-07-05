#ifndef BIManager_SeaGroup_H
#define BIManager_SeaGroup_H

#include "BaseGroup.h"

class BI_SeaGroup : public BI_BaseGroup
{
public:
	BI_SeaGroup( BI_ManagerBase* pManager );
	virtual ~BI_SeaGroup();

	virtual void Init();
	virtual void Update() {}

	virtual long Event(const char* pcEventName) {return 0;}

protected:
};

#endif
