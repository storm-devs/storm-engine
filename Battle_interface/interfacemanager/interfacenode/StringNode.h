#ifndef BIManager_StringNode_H
#define BIManager_StringNode_H

#include "BaseNode.h"

class IBIString;

class BI_StringNode : public BI_BaseNode
{
public:
	BI_StringNode(BI_ManagerBase* pManager, const char* text, const char* font, dword color, float scale, const RECT& pos, long nHAlign, long nVAlign, long prioritet);
	virtual ~BI_StringNode();

	virtual void Update();

protected:
	IBIString* m_pStr;
};

#endif
