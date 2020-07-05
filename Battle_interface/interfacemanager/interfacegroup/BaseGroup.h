#ifndef BIManager_BaseGroup_H
#define BIManager_BaseGroup_H

#include "..\BaseManager.h"

#define BIImagePrioritet_Group_Beg	15000

class BI_ManagerBase;

class BI_BaseGroup
{
public:
	BI_BaseGroup( BI_ManagerBase* pManager );
	virtual ~BI_BaseGroup();

	virtual void Init() {}
	virtual void Update() {}

	virtual long Event(const char* pcEventName) {return 0;}

	BI_ManagerBase* Manager() {return m_pManager;}
	const array<BI_ManagerNodeBase*> & Nodes() {return m_aNodes;}

protected:
	BI_ManagerBase* m_pManager;
	array<BI_ManagerNodeBase*> m_aNodes;
};

#endif
