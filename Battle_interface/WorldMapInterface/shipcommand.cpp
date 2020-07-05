#include "shipcommand.h"

WMShipCommandList::WMShipCommandList( ENTITY_ID& eid, ATTRIBUTES* pA, VDX8RENDER* rs ) :
	BICommandList(eid,pA,rs)
{
	Init();
}

WMShipCommandList::~WMShipCommandList()
{
	Release();
}

void WMShipCommandList::FillIcons()
{
	long nIconsQuantity = 0;

	if( m_nCurrentCommandMode & BI_COMMODE_COMMAND_SELECT )
		nIconsQuantity += CommandAdding();
}

void WMShipCommandList::Init()
{
	BICommandList::Init();
}

void WMShipCommandList::Release()
{
}

long WMShipCommandList::CommandAdding()
{
	api->Event("WM_SetPossibleCommands","l",m_nCurrentCommandCharacterIndex);
	long retVal = 0;
	ATTRIBUTES * pAttr = m_pARoot->GetAttributeClass( "Commands" );
	if( !pAttr ) return 0;
	long attrQuant = pAttr->GetAttributesNum();

	for(long i=0; i<attrQuant; i++)
	{
		ATTRIBUTES * pA = pAttr->GetAttributeClass(i);
		if(pA==null) continue; // нет такого атрибута
		if(pA->GetAttributeAsDword("enable",0)==0) continue; // команда недоступна
		long pictureNum = pA->GetAttributeAsDword("picNum",0);
		long selPictureNum = pA->GetAttributeAsDword("selPicNum",0);
		long texNum = pA->GetAttributeAsDword("texNum",-1);
		char * eventName = pA->GetAttribute("event");
		retVal += AddToIconList( texNum, pictureNum, selPictureNum, -1,
			-1, eventName, -1, null, pA->GetAttribute("note") );
	}

	return retVal;
}
