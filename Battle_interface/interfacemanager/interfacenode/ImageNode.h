#ifndef BIManager_ImageNode_H
#define BIManager_ImageNode_H

#include "BaseNode.h"

class IBIImage;

class BI_ImageNode : public BI_BaseNode
{
public:
	BI_ImageNode( BI_ManagerBase* pManager, const char* texture, const FRECT& uv, const RECT& pos, dword color, long nPrioritet );
	virtual ~BI_ImageNode();

	virtual void Update();

protected:
	IBIImage* m_pImage;
};

#endif
