#ifndef BIManager_BaseNode_H
#define BIManager_BaseNode_H

#include "..\BaseManager.h"

class BI_ManagerBase;

class BI_BaseNode : public BI_ManagerNodeBase
{
public:
	BI_BaseNode( BI_ManagerBase* pManager );
	virtual ~BI_BaseNode();

	virtual void Update();

	virtual long Event(const char* pcEventName) {return 0;}

	virtual bool IsPointInside(float fX, float fY) {return false;}
	virtual bool IsMouseClickReaction() {return false;}
	virtual bool IsMouseMoveReaction() {return false;}

	BI_ManagerBase* Manager() {return m_pManager;}
	VDX8RENDER* Render() {return m_pRS;}

protected:
	BI_ManagerBase* m_pManager;
	VDX8RENDER* m_pRS;
};

#endif
