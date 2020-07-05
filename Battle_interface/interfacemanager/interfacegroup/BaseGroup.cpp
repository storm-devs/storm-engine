#include "BaseGroup.h"

BI_BaseGroup::BI_BaseGroup( BI_ManagerBase* pManager ) :
	m_aNodes(_FL)
{
	Assert(pManager);
	m_pManager = pManager;
	Init();
}

BI_BaseGroup::~BI_BaseGroup()
{
	for(long n=0; n<m_aNodes; n++)
		m_pManager->DeleteNode( m_aNodes[n] );
	m_aNodes.DelAllWithPointers();
}
