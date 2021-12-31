#include "base_node.h"

BI_BaseNode::BI_BaseNode(BI_ManagerBase *pManager) : m_pRS(nullptr)
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
