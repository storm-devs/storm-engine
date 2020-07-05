#include "battle_shipcommand.h"
#include "ships_list.h"
#include "island_descr.h"

BIShipCommandList::BIShipCommandList( ENTITY_ID& eid, ATTRIBUTES* pA, VDX8RENDER* rs ) :
	BICommandList(eid,pA,rs),
	m_aChargeQuantity(_FL)
{
	Init();
}

BIShipCommandList::~BIShipCommandList()
{
	Release();
}

void BIShipCommandList::FillIcons()
{
	long nIconsQuantity = 0;

	if( m_nCurrentCommandMode & BI_COMMODE_MY_SHIP_SELECT )
		nIconsQuantity += ShipAdding( (m_nCurrentCommandMode&BI_COMMODE_ALLLOCATOR_SELECT)!=0, true, false,false,false );
	if( m_nCurrentCommandMode & BI_COMMODE_NEUTRAL_SHIP_SELECT )
		nIconsQuantity += ShipAdding( (m_nCurrentCommandMode&BI_COMMODE_ALLLOCATOR_SELECT)!=0, false, false,true,false );
	if( m_nCurrentCommandMode & BI_COMMODE_FRIEND_SHIP_SELECT )
		nIconsQuantity += ShipAdding( (m_nCurrentCommandMode&BI_COMMODE_ALLLOCATOR_SELECT)!=0, false, false,false,true );
	if( m_nCurrentCommandMode & BI_COMMODE_ENEMY_SHIP_SELECT )
		nIconsQuantity += ShipAdding( (m_nCurrentCommandMode&BI_COMMODE_ALLLOCATOR_SELECT)!=0, false, true,false,false );

	if( m_nCurrentCommandMode & BI_COMMODE_FRIEND_FORT_SELECT )
		nIconsQuantity += FortAdding( (m_nCurrentCommandMode&BI_COMMODE_ALLLOCATOR_SELECT)!=0, true,false,false );
	if( m_nCurrentCommandMode & BI_COMMODE_NEUTRAL_FORT_SELECT )
		nIconsQuantity += FortAdding( (m_nCurrentCommandMode&BI_COMMODE_ALLLOCATOR_SELECT)!=0, false,true,false );
	if( m_nCurrentCommandMode & BI_COMMODE_ENEMY_FORT_SELECT )
		nIconsQuantity += FortAdding( (m_nCurrentCommandMode&BI_COMMODE_ALLLOCATOR_SELECT)!=0, false,false,true );
	if( m_nCurrentCommandMode & BI_COMMODE_LAND_SELECT )
		nIconsQuantity += LandAdding( (m_nCurrentCommandMode&BI_COMMODE_ALLLOCATOR_SELECT)!=0 );

	if( m_nCurrentCommandMode & BI_COMMODE_COMMAND_SELECT )
		nIconsQuantity += CommandAdding();

	if( m_nCurrentCommandMode & BI_COMMODE_CANNON_CHARGE )
		nIconsQuantity += ChargeAdding();

	if( m_nCurrentCommandMode & BI_COMMODE_USER_ICONS )
		nIconsQuantity += UserIconsAdding();

	if( m_nCurrentCommandMode & BI_COMMODE_ENEMY_TOWN )
		nIconsQuantity += TownAdding( (m_nCurrentCommandMode&BI_COMMODE_ALLLOCATOR_SELECT)!=0, true,true, true,false,false );

	if( m_nCurrentCommandMode & BI_COMMODE_DISEASED_TOWN )
		nIconsQuantity += TownAdding( (m_nCurrentCommandMode&BI_COMMODE_ALLLOCATOR_SELECT)!=0, true,false, true,true,true );

	if( m_nCurrentCommandMode & BI_COMMODE_NOTDISEASED_TOWN )
		nIconsQuantity += TownAdding( (m_nCurrentCommandMode&BI_COMMODE_ALLLOCATOR_SELECT)!=0, false,true, true,true,true );

	nIconsQuantity += AddCancelIcon();
}

void BIShipCommandList::Init()
{
	BICommandList::Init();

	m_nChargeTextureNum = -1;
	m_nCommandTextureNum = -1;
	m_nIconShowMaxQuantity = 8; // boal

	ATTRIBUTES* pA = m_pARoot;
	if( pA ) pA = pA->GetAttributeClass( "CommandTextures" );
	if( pA ) {
		m_nChargeTextureNum = pA->GetAttributeAsDword("ChargeTexNum",m_nChargeTextureNum);
		m_nCommandTextureNum = pA->GetAttributeAsDword("CommandTexNum",m_nCommandTextureNum);
	}

	pA = m_pARoot;
	if( pA ) pA = pA->GetAttributeClass( "CommandList" );
	if( pA )
	{
		m_nIconShowMaxQuantity = pA->GetAttributeAsDword("CommandMaxIconQuantity",m_nIconShowMaxQuantity);
	}
}

void BIShipCommandList::Release()
{
}

long BIShipCommandList::ShipAdding( bool allLabel, bool bMyShip, bool bEnemy, bool bNeutral, bool bFriend )
{
	long n;
	long retVal=0;

	// список кораблей
	SHIP_DESCRIBE_LIST::SHIP_DESCR	*sd = g_ShipList.GetShipRoot();
	if(sd==null) return 0;

	// дистанция отсечения корабля из списка
	ATTRIBUTES * pA = GetCurrentCommandAttribute();
	float sqrRadius = -1.f;
	if( pA ) sqrRadius = pA->GetAttributeAsFloat("EffectRadius",sqrRadius);
	if( sqrRadius<0.f ) allLabel=true;
	sqrRadius *= sqrRadius;

	SHIP_DESCRIBE_LIST::SHIP_DESCR	*selShip = g_ShipList.FindShip( m_nCurrentCommandCharacterIndex );
	float selX,selZ;
	if( selShip==null )
	{
		allLabel = true;
		selX = 0;
		selZ = 0;
	}
	else
	{
		selX = selShip->pShip->GetPos().x;
		selZ = selShip->pShip->GetPos().z;
	}

	SHIP_DESCRIBE_LIST::SHIP_DESCR	*main_sd = g_ShipList.GetMainCharacterShip();
	if( bMyShip && main_sd!=selShip )
	{
		n = AddToIconList( main_sd->textureNum, main_sd->pictureNum, main_sd->selectPictureNum, -1,
			main_sd->characterIndex, null, -1, null, main_sd->pAttr->GetAttribute("name") );
		if( n > 0 ) {
			retVal += n;
			AddFlagPictureToIcon( main_sd->characterIndex );
		}
	}

	for(; sd!=null; sd=sd->next)
	{
		if(sd!=selShip && sd!=main_sd)
		{
			if( bMyShip && sd->isMyShip ||
				!bMyShip && bEnemy && !sd->isMyShip && sd->relation==BI_RELATION_ENEMY ||
				!bMyShip && bFriend && !sd->isMyShip && sd->relation==BI_RELATION_FRIEND ||
				!bMyShip && bNeutral && !sd->isMyShip && sd->relation==BI_RELATION_NEUTRAL )
			{
				// подходит под расстояние ?
				if(!allLabel)
				{
					CVECTOR cv=sd->pShip->GetPos();
					if(SQR(selX-cv.x)+SQR(selZ-cv.z) > sqrRadius)	continue;
				}
				// проверка на допустимость корабля из скрипта
				if( !m_sCurrentCommandName.IsEmpty() )
				{
					VDATA * pvdat = api->Event("evntCheckEnableShip","sl",m_sCurrentCommandName.GetBuffer(),sd->characterIndex);
					if(pvdat!=null && pvdat->GetLong()==0)	continue;
				}
				n = AddToIconList( sd->textureNum, sd->pictureNum, sd->selectPictureNum, -1,
					sd->characterIndex, null, -1, null, sd->pAttr->GetAttribute("name") );
				if( n>0 ) {
					retVal += n;
					AddFlagPictureToIcon( sd->characterIndex );
				}
			}
		}
	}

	return retVal;
}

long BIShipCommandList::FortAdding(bool allLabel, bool bFriend, bool bNeutral, bool bEnemy )
{
	ISLAND_DESCRIBER::LOCATOR_DESCR * pL = g_IslandDescr.GetFirstFort();
	if(pL==null) return 0;
	long retVal=0;

	ATTRIBUTES * pA = GetCurrentCommandAttribute();
	float sqrRadius = pL->r;
	if( pA ) sqrRadius = pA->GetAttributeAsFloat("EffectRadius",sqrRadius);
	if( sqrRadius<0.f ) allLabel=true;
	sqrRadius *= sqrRadius;

	// Определим координаты принимающего команды корабля
	long selectedCharacter = m_nCurrentCommandCharacterIndex;
	SHIP_DESCRIBE_LIST::SHIP_DESCR	*selShip = g_ShipList.FindShip( selectedCharacter );
	float selX,selZ;
	if(selShip==null)
	{
		allLabel=true;
		selX=0;
		selZ=0;
	}
	else
	{
		selX=selShip->pShip->GetPos().x;
		selZ=selShip->pShip->GetPos().z;
	}

	do
	{
		if( bFriend && pL->relation==BI_RELATION_FRIEND ||
			bNeutral && pL->relation==BI_RELATION_NEUTRAL ||
			bEnemy && pL->relation==BI_RELATION_ENEMY )
		{
			if(!allLabel)
				if( SQR(pL->x-selX) + SQR(pL->z-selZ) > sqrRadius ) continue;
			VDATA * pvdat = api->Event("evntCheckEnableLocator","sa",m_sCurrentCommandName.GetBuffer(),pL->pA);
			if(pvdat!=null && pvdat->GetLong()==0) continue;
			char * pLocName = null;
			if( pL->pA != null ) pLocName = pL->pA->GetAttribute("name");
			retVal += AddToIconList( pL->texIdx, pL->picIdx, pL->selPicIdx, -1,
				pL->characterIndex, null, -1, pL->pA->GetAttribute("name"), pL->pchr_note );
		}
	}while((pL=g_IslandDescr.GetNext())!=null);

	return retVal;

	return 0;
}

long BIShipCommandList::LandAdding(bool allLabel)
{
	ISLAND_DESCRIBER::LOCATOR_DESCR * pL = g_IslandDescr.GetFirstLand();
	if(pL==null) return 0;
	long retVal=0;

	ATTRIBUTES * pA = GetCurrentCommandAttribute();
	float sqrRadius = pL->r;
	if( pA ) sqrRadius = pA->GetAttributeAsFloat("EffectRadius",sqrRadius);
	if( sqrRadius<0.f ) allLabel=true;
	sqrRadius *= sqrRadius;

	// Определим координаты принимающего команды корабля
	long selectedCharacter = m_nCurrentCommandCharacterIndex;
	SHIP_DESCRIBE_LIST::SHIP_DESCR	*selShip = g_ShipList.FindShip(selectedCharacter);
	float selX,selZ;
	if(selShip==null)
	{
		allLabel=true;
		selX=0;
		selZ=0;
	}
	else
	{
		selX=selShip->pShip->GetPos().x;
		selZ=selShip->pShip->GetPos().z;
	}

	do
	{
		if(!allLabel)
			if( SQR(pL->x-selX) + SQR(pL->z-selZ) > sqrRadius ) continue;
		VDATA * pvdat = api->Event("evntCheckEnableLocator","sa",m_sCurrentCommandName.GetBuffer(),pL->pA);
		if(pvdat!=null && pvdat->GetLong()==0) continue;
		char * pLocName = null;
		if( pL->pA != null ) pLocName = pL->pA->GetAttribute("name");
		retVal += AddToIconList( pL->texIdx, pL->picIdx, pL->selPicIdx, -1,
			pL->characterIndex, null, -1, pLocName, pL->pchr_note );
	}while((pL=g_IslandDescr.GetNext())!=null);
	return retVal;
}

long BIShipCommandList::CommandAdding()
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
		long cooldownPictureNum = pA->GetAttributeAsDword("cooldownPicNum",-1);
		long texNum = pA->GetAttributeAsDword("texNum",m_nCommandTextureNum);
		char * eventName = pA->GetAttribute("event");
		retVal += AddToIconList( texNum, pictureNum, selPictureNum, cooldownPictureNum,
			-1, eventName, -1, null, pA->GetAttribute("note") );
	}

	return retVal;
}

long BIShipCommandList::ChargeAdding()
{
	// Определим количество каждого заряда на борту
	VDATA * tmpDat = api->Event("BI_GetChargeQuantity","l",m_nCurrentCommandCharacterIndex);
	if(tmpDat==null) return 0;
	long lIdx=0; // количество типов заряда
	tmpDat->Get(lIdx,0);
	if(lIdx<=0) return 0;

	ATTRIBUTES* pAList = m_pARoot->GetAttributeClass( "charge" );
	long retVal=0;
	m_aChargeQuantity.DelAll();
	for(int i=0;i<lIdx;i++)
	{
		m_aChargeQuantity.Add(0);
		tmpDat->Get(m_aChargeQuantity[i],i+1);

		char param[128];
		_snprintf( param,sizeof(param), "charge%d",i+1 );
		ATTRIBUTES* pA = pAList ? pAList->GetAttributeClass(param) : 0;
		long nNormalPicIndex = pA ? pA->GetAttributeAsDword("picNum",-1) : -1;
		long nSelectPicIndex = pA ? pA->GetAttributeAsDword("selPicNum",-1) : -1;
		retVal += AddToIconList( m_nChargeTextureNum, nNormalPicIndex, nSelectPicIndex, -1, -1, null, i+1, null, null );
	}
	return retVal;
}

long BIShipCommandList::UserIconsAdding()
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
		retVal += AddToIconList( textureNum, pictureNum, selPictureNum, -1, -1, null, i+1, pA->GetAttribute("name"), pA->GetAttribute("note") );
	}

	return retVal;
}

long BIShipCommandList::AbilityAdding()
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
		long cooldownPictureNum = pA->GetAttributeAsDword("cooldownPicNum",-1);
		char * eventName = pA->GetAttribute("event");
		retVal += AddToIconList( textureNum, pictureNum, selPictureNum, cooldownPictureNum,
			-1, eventName, -1, null, pA->GetAttribute("note") );
	}

	return retVal;
}

long BIShipCommandList::AddCancelIcon()
{
	ATTRIBUTES * pA = m_pARoot->GetAttributeClass( "Commands" );
	if( pA ) pA = pA->GetAttributeClass( "Cancel" );
	if( !pA ) return 0;
	long pictureNum = pA->GetAttributeAsDword("picNum",0);
	long selPictureNum = pA->GetAttributeAsDword("selPicNum",0);
	long textureNum = pA->GetAttributeAsDword("texNum",-1);
	return AddToIconList( textureNum, pictureNum, selPictureNum, -1, -1, pA->GetAttribute("event"), -1, null, pA->GetAttribute("note") );
}

long BIShipCommandList::TownAdding( bool allLabel, bool bDiseased,bool bNotDiseased, bool bEnemy,bool bNeutral,bool bFriend )
{
	ISLAND_DESCRIBER::LOCATOR_DESCR * pL = g_IslandDescr.GetFirstLocator();
	if(pL==null) return 0;
	long retVal=0;

	// определяем радиус действия команды (все что не входит в него - не показывается)
	ATTRIBUTES * pA = GetCurrentCommandAttribute();
	float sqrRadius = pL->r;
	if( pA ) sqrRadius = pA->GetAttributeAsFloat("EffectRadius",sqrRadius);
	if( sqrRadius<0.f ) allLabel=true;
	sqrRadius *= sqrRadius;

	// Определим координаты принимающего команды корабля
	long selectedCharacter = m_nCurrentCommandCharacterIndex;
	SHIP_DESCRIBE_LIST::SHIP_DESCR	*selShip = g_ShipList.FindShip(selectedCharacter);
	float selX,selZ;
	if(selShip==null)
	{
		allLabel=true;
		selX=0;
		selZ=0;
	}
	else
	{
		selX=selShip->pShip->GetPos().x;
		selZ=selShip->pShip->GetPos().z;
	}

	do
	{
		if( pL->locatorType != ISLAND_LOCATOR_TOWN ) continue; // проверка по типу - должен быть городом
		// проверка по отношению (враг,нейтрал,друг)
		if( pL->relation==BI_RELATION_ENEMY && !bEnemy ) continue;
		if( pL->relation==BI_RELATION_NEUTRAL && !bNeutral ) continue;
		if( pL->relation==BI_RELATION_FRIEND && !bFriend ) continue;
		if( pL->bDiseased && !bDiseased ) continue;
		if( !pL->bDiseased && !bNotDiseased ) continue;
		if(!allLabel)
			if( SQR(pL->x-selX) + SQR(pL->z-selZ) > sqrRadius ) continue;
		VDATA * pvdat = api->Event("evntCheckEnableLocator","sa",m_sCurrentCommandName.GetBuffer(),pL->pA);
		if(pvdat!=null && pvdat->GetLong()==0) continue;
		char * pLocName = null;
		if( pL->pA != null ) pLocName = pL->pA->GetAttribute("name");
		retVal += AddToIconList( pL->texIdx, pL->picIdx, pL->selPicIdx, -1,
			pL->characterIndex, null, -1, pLocName, pL->pchr_note );
	}while((pL=g_IslandDescr.GetNext())!=null);
	return retVal;
}

void BIShipCommandList::AddFlagPictureToIcon( long nCharIdx )
{
	VDATA* pvdat = api->Event("evntGetSmallFlagData","l",nCharIdx);
	if( !pvdat ) return;
	long nTex,nPic,nBackPic;
	pvdat->Get(nTex,0);
	pvdat->Get(nPic,1);
	pvdat->Get(nBackPic,2);
	AddAdditiveToIconList( nTex,nBackPic, 2.f, 32.f,32.f );
	AddAdditiveToIconList( nTex,nPic, -32.f, 32.f,32.f );
}