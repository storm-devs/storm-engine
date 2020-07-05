#include "battle_mancommand.h"

BIManCommandList::BIManCommandList( ENTITY_ID& eid, ATTRIBUTES* pA, VDX8RENDER* rs ) :
	BICommandList(eid,pA,rs)
{
	Init();
}

BIManCommandList::~BIManCommandList()
{
	Release();
}

void BIManCommandList::FillIcons()
{
	long nIconsQuantity = 0;

	if( m_nCurrentCommandMode & BI_COMMODE_COMMAND_SELECT )
		nIconsQuantity += CommandAdding();

	if( m_nCurrentCommandMode & BI_COMMODE_USER_ICONS )
		nIconsQuantity += UserIconsAdding();

	if( m_nCurrentCommandMode & BI_COMMODE_ABILITY_ICONS )
		nIconsQuantity += AbilityAdding();

	nIconsQuantity += AddCancelIcon();

	/*if( nIconsQuantity == 0 )
		AddCancelIcon();*/
}

void BIManCommandList::Init()
{
	BICommandList::Init();

	m_nIconShowMaxQuantity = 5;
	// boal -->
	ATTRIBUTES* pA = m_pARoot;
	if( pA ) pA = pA->GetAttributeClass( "CommandList" );
	if( pA )
	{
		m_nIconShowMaxQuantity = pA->GetAttributeAsDword("CommandMaxIconQuantity",m_nIconShowMaxQuantity);
	}
	// boal <--
}

void BIManCommandList::Release()
{
}

long BIManCommandList::CommandAdding()
{
	api->Event("BI_SetPossibleCommands","l",m_nCurrentCommandCharacterIndex);
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

long BIManCommandList::UserIconsAdding()
{
	long retVal = 0;
	ATTRIBUTES * pAttr = m_pARoot->GetAttributeClass("UserIcons");
	if( !pAttr ) return 0;
	long attrQuant = pAttr->GetAttributesNum();

	for(long i=0; i<attrQuant; i++)
	{
		ATTRIBUTES * pA = pAttr->GetAttributeClass(i);
		if(pA==null) continue; // нет такого атрибута
		if(pA->GetAttributeAsDword("enable",0)==0) continue; // команда недоступна
		long pictureNum = pA->GetAttributeAsDword("pic",0);
		long selPictureNum = pA->GetAttributeAsDword("selpic",0);
		long textureNum = pA->GetAttributeAsDword("tex",-1);
		char * eventName = pA->GetAttribute("event");
		retVal += AddToIconList( textureNum, pictureNum, selPictureNum, -1, -1, eventName, i+1, pA->GetAttribute("name"), pA->GetAttribute("note") );
	}

	return retVal;
}

long BIManCommandList::AbilityAdding()
{
	api->Event("evntSetUsingAbility","l",m_nCurrentCommandCharacterIndex);
	long retVal = 0;
	ATTRIBUTES * pAttr = m_pARoot->GetAttributeClass( "AbilityIcons" );
	if( !pAttr ) return 0;
	long attrQuant = pAttr->GetAttributesNum();

	for(long i=0; i<attrQuant; i++)
	{
		ATTRIBUTES * pA = pAttr->GetAttributeClass(i);
		if(pA==null) continue; // нет такого атрибута
		if(pA->GetAttributeAsDword("enable",0)==0) continue; // команда недоступна
		long pictureNum = pA->GetAttributeAsDword("picNum",0);
		long selPictureNum = pA->GetAttributeAsDword("selPicNum",0);
		long textureNum = pA->GetAttributeAsDword("texNum",-1);
		//retVal += AddToIconList( textureNum, pictureNum, selPictureNum, -1, -1, pA->GetThisName(), i, null, pA->GetAttribute("note") );
		char * eventName = pA->GetAttribute("event");
		retVal += AddToIconList( textureNum, pictureNum, selPictureNum, -1,
			-1, eventName, -1, null, pA->GetAttribute("note") );
	}

	return retVal;
}

long BIManCommandList::AddCancelIcon()
{
	ATTRIBUTES * pA = m_pARoot->GetAttributeClass( "Commands" );
	if( pA ) pA = pA->GetAttributeClass( "Cancel" );
	if( !pA ) return 0;
	long pictureNum = pA->GetAttributeAsDword("picNum",0);
	long selPictureNum = pA->GetAttributeAsDword("selPicNum",0);
	long textureNum = pA->GetAttributeAsDword("texNum",-1);
	return AddToIconList( textureNum, pictureNum, selPictureNum, -1, -1, pA->GetAttribute("event"), -1, null, pA->GetAttribute("note") );
}