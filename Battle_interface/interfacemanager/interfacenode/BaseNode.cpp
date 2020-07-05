#include "BaseNode.h"

BI_BaseNode::BI_BaseNode( BI_ManagerBase* pManager )
{
	Assert(pManager);
	m_pManager = pManager;
}

BI_BaseNode::~BI_BaseNode()
{
}

void BI_BaseNode::Update()
{
}
