#include "battle_shipcommand.h"
#include "image\imgrender.h"
#include "ships_list.h"
#include "island_descr.h"

BIShipCommandList::BIShipCommandList( ENTITY_ID& eid, ATTRIBUTES* pA, VDX8RENDER* rs ) :
	m_aTexture(_FL),
	m_aCommand(_FL),
	m_aUsedCommand(_FL),
	m_aChargeQuantity(_FL)
{
	m_idHostObj = eid;
	m_pARoot = pA;
	m_pRS = rs;

	m_pImgRender = NEW BIImageRender( rs );
	Assert( m_pImgRender );

	m_nStartUsedCommandIndex = 0;
	m_nSelectedCommandIndex = 0;
	m_nIconShowMaxQuantity = 5;

	m_NoteFontID = -1;
	m_NotePos.x = m_NotePos.y = 0;
	m_NoteOffset.x = m_NoteOffset.y = 0;

	Init();
}

BIShipCommandList::~BIShipCommandList()
{
	Release();
}

void BIShipCommandList::Draw()
{
	if( m_pImgRender )
		m_pImgRender->Render();

	if( !m_NoteText.IsEmpty() )
		m_pRS->ExtPrint( m_NoteFontID, m_NoteFontColor, 0, ALIGN_CENTER, true, m_NoteFontScale, 0,0,
			m_NotePos.x, m_NotePos.y, "%s", m_NoteText.GetBuffer() );
}

void BIShipCommandList::Update( long nTopLine, long nCharacterIndex, long nCommandMode )
{
	m_LeftTopPoint.y = nTopLine;
	if( nCharacterIndex != m_nCurrentCommandCharacterIndex ) m_sCurrentCommandName = "";
	m_nCurrentCommandCharacterIndex = nCharacterIndex;
	m_nCurrentCommandMode = nCommandMode;

	m_nStartUsedCommandIndex = 0;
	m_nSelectedCommandIndex = 0;
	m_aUsedCommand.DelAll();

	m_NoteText.Empty();

	long nIconsQuantity = 0;
	if( nCommandMode & BI_COMMODE_MY_SHIP_SELECT )
		nIconsQuantity += ShipAdding( (nCommandMode&BI_COMMODE_ALLLOCATOR_SELECT)!=0, true, false,false,false );
	if( nCommandMode & BI_COMMODE_NEUTRAL_SHIP_SELECT )
		nIconsQuantity += ShipAdding( (nCommandMode&BI_COMMODE_ALLLOCATOR_SELECT)!=0, false, false,true,false );
	if( nCommandMode & BI_COMMODE_FRIEND_SHIP_SELECT )
		nIconsQuantity += ShipAdding( (nCommandMode&BI_COMMODE_ALLLOCATOR_SELECT)!=0, false, false,false,true );
	if( nCommandMode & BI_COMMODE_ENEMY_SHIP_SELECT )
		nIconsQuantity += ShipAdding( (nCommandMode&BI_COMMODE_ALLLOCATOR_SELECT)!=0, false, true,false,false );

	if( nCommandMode & BI_COMMODE_FRIEND_FORT_SELECT )
		nIconsQuantity += FortAdding( (nCommandMode&BI_COMMODE_ALLLOCATOR_SELECT)!=0, true,false,false );
	if( nCommandMode & BI_COMMODE_NEUTRAL_FORT_SELECT )
		nIconsQuantity += FortAdding( (nCommandMode&BI_COMMODE_ALLLOCATOR_SELECT)!=0, false,true,false );
	if( nCommandMode & BI_COMMODE_ENEMY_FORT_SELECT )
		nIconsQuantity += FortAdding( (nCommandMode&BI_COMMODE_ALLLOCATOR_SELECT)!=0, false,false,true );
	if( nCommandMode & BI_COMMODE_LAND_SELECT )
		nIconsQuantity += LandAdding( (nCommandMode&BI_COMMODE_ALLLOCATOR_SELECT)!=0 );

	if( nCommandMode & BI_COMMODE_COMMAND_SELECT )
		nIconsQuantity += CommandAdding();

	if( nCommandMode & BI_COMMODE_CANNON_CHARGE )
		nIconsQuantity += ChargeAdding();

	if( nCommandMode & BI_COMMODE_USER_ICONS )
		nIconsQuantity += UserIconsAdding();

	if( nCommandMode & BI_COMMODE_ABILITY_ICONS )
		nIconsQuantity += AbilityAdding();

	if( nIconsQuantity == 0 )
		AddCancelIcon();

	UpdateShowIcon();
}

long BIShipCommandList::AddTexture( const char* pcTextureName, long nCols, long nRows )
{
	long n = m_aTexture.Add();
	m_aTexture[n].sFileName = pcTextureName;
	m_aTexture[n].nCols = nCols;
	m_aTexture[n].nRows = nRows;
	return n;
}

long BIShipCommandList::ExecuteConfirm()
{
	if( m_nSelectedCommandIndex >= m_aUsedCommand ) return 0; // error!

	long endCode=0;
	long nTargIndex = 0;
	string sLocName;

	if( !m_aUsedCommand[m_nSelectedCommandIndex].sCommandName.IsEmpty() ) {
		m_sCurrentCommandName = m_aUsedCommand[m_nSelectedCommandIndex].sCommandName;
		VDATA * pVD = api->Event("BI_CommandEndChecking","s",m_sCurrentCommandName.GetBuffer());
		if(pVD!=null)	pVD->Get(endCode);
	}
	else
	{
		sLocName = m_aUsedCommand[m_nSelectedCommandIndex].sLocName;
		if( m_aUsedCommand[m_nSelectedCommandIndex].nCharIndex < 0 )
			nTargIndex = m_aUsedCommand[m_nSelectedCommandIndex].nTargetIndex;
		else nTargIndex = m_aUsedCommand[m_nSelectedCommandIndex].nCharIndex;
	}

	switch(endCode)
	{
	case 0:
		api->Event("BI_LaunchCommand","lsls",m_nCurrentCommandCharacterIndex,m_sCurrentCommandName.GetBuffer(),nTargIndex,sLocName.GetBuffer());
		m_sCurrentCommandName = "";
	break;
	default:
		Update( m_LeftTopPoint.y, m_nCurrentCommandCharacterIndex, endCode );
	}
	return endCode;
}

long BIShipCommandList::ExectuteLeft()
{
	if( m_nSelectedCommandIndex > 0 )
	{
		m_nSelectedCommandIndex--;
		if( m_nSelectedCommandIndex < m_nStartUsedCommandIndex )
		{
			m_nStartUsedCommandIndex = m_nSelectedCommandIndex;
		}
		UpdateShowIcon();
	}
	return 0;
}

long BIShipCommandList::ExectuteRight()
{
	if( m_nSelectedCommandIndex < (long)m_aUsedCommand.Size()-1 )
	{
		m_nSelectedCommandIndex++;
		if( m_nSelectedCommandIndex >= m_nStartUsedCommandIndex + m_nIconShowMaxQuantity )
		{
			m_nStartUsedCommandIndex = m_nSelectedCommandIndex - m_nIconShowMaxQuantity + 1;
		}
		UpdateShowIcon();
	}
	return 0;
}

long BIShipCommandList::ExecuteCancel()
{
	if( m_sCurrentCommandName.IsEmpty() ) return 0;
	m_sCurrentCommandName = "";
	return BI_COMMODE_COMMAND_SELECT;
}

void BIShipCommandList::Release()
{
	DELETE( m_pImgRender );
	FONT_RELEASE(m_pRS,m_NoteFontID);
}

void BIShipCommandList::Init()
{
	Assert( m_pImgRender );
	long n,q,i;
	ATTRIBUTES *pAList, *pATextures, *pACommands;

	m_LeftTopPoint.x = 120;
	m_LeftTopPoint.y = 100;
	m_IconSize.x = 64;
	m_IconSize.y = 64;
	m_nIconSpace = 8;

	FONT_RELEASE( m_pRS, m_NoteFontID );
	m_NoteFontColor = ARGB(255,255,255,255);
	m_NoteFontScale = 1.f;
	m_NoteOffset.x = m_NoteOffset.y = 0;

	pAList = 0;
	if( m_pARoot ) pAList = m_pARoot->GetAttributeClass( "ShipIcon" );
	if( pAList ) {
		// get icon parameters
		m_nIconSpace = pAList->GetAttributeAsDword( "CommandIconSpace", 0 );
		m_LeftTopPoint.x = pAList->GetAttributeAsDword( "CommandIconLeft", 100 );
		m_IconSize.x = pAList->GetAttributeAsDword( "CommandIconWidth", m_IconSize.x );
		m_IconSize.y = pAList->GetAttributeAsDword( "CommandIconHeight", m_IconSize.y );

		// get note font parameters
		if( pAList->GetAttribute("CommandNoteFont") )
			m_NoteFontID = m_pRS->LoadFont( pAList->GetAttribute( "CommandNoteFont" ) );
		m_NoteFontColor = pAList->GetAttributeAsDword( "CommandNoteColor", m_NoteFontColor );
		m_NoteFontScale = pAList->GetAttributeAsFloat( "CommandNoteScale", m_NoteFontScale );
		if( pAList->GetAttribute("CommandNoteOffset") )
			sscanf( pAList->GetAttribute("CommandNoteOffset"), "%d,%d", &m_NoteOffset.x, &m_NoteOffset.y );
	}

	pAList = 0;
	if( m_pARoot ) pAList = m_pARoot->GetAttributeClass( "CommandTextures" );

	pATextures = 0;
	if( pAList ) pATextures = pAList->GetAttributeClass( "list" );
	if( pATextures )
	{
		q = pATextures->GetAttributesNum();
		for( n=0; n<q; n++ )
		{
			ATTRIBUTES* pA = pATextures->GetAttributeClass( n );
			if( pA )
			{
				i = m_aTexture.Add();
				m_aTexture[i].sFileName = pA->GetAttribute("name");
				m_aTexture[i].nCols = pA->GetAttributeAsDword("xsize",1);
				m_aTexture[i].nRows = pA->GetAttributeAsDword("ysize",1);

				if( m_aTexture[i].nCols < 1 ) m_aTexture[i].nCols = 1;
				if( m_aTexture[i].nRows < 1 ) m_aTexture[i].nRows = 1;

				BIImageMaterial * pMat = m_pImgRender->CreateMaterial( m_aTexture[i].sFileName );
				Assert( pMat );
			}
		}
	}

	m_nChargeTextureNum =	pAList->GetAttributeAsDword("ChargeTexNum",-1);
	m_nCommandTextureNum =	pAList->GetAttributeAsDword("CommandTexNum",-1);
	m_nIconShowMaxQuantity = 5;

	pACommands = 0;
	if( m_pARoot ) pACommands = m_pARoot->GetAttributeClass( "commands" );
	if( pACommands )
	{
		q = pACommands->GetAttributesNum();
		for( n=0; n<q; n++ )
		{
			ATTRIBUTES* pA = pACommands->GetAttributeClass( n );
			if( pA )
			{
				i = m_aCommand.Add();
				m_aCommand[i].sCommandName = pA->GetThisName();
				m_aCommand[i].nTexture = pA->GetAttributeAsDword( "texnum" );
				m_aCommand[i].sTextureName = m_aTexture[m_aCommand[i].nTexture].sFileName;
				m_aCommand[i].nPicIndex = pA->GetAttributeAsDword( "picnum" );
				GetUVForPictureIntoTexture( m_aCommand[i].nPicIndex, m_aCommand[i].nTexture, m_aCommand[i].rUV );
				m_aCommand[i].sEvent = pA->GetAttribute( "event" );
				m_aCommand[i].sNote = pA->GetAttribute( "note" );
			}
		}
	}
}

long BIShipCommandList::FindTextureIndexByName( const char* pcTextureFieName )
{
	if( !pcTextureFieName ) return -1;
	for( long n=0; n<m_aTexture; n++ )
		if( m_aTexture[n].sFileName == pcTextureFieName )
			return n;
	return -1;
}

void BIShipCommandList::GetUVForPictureIntoTexture( long nPicIndex, long nTextureIndex, FRECT& rUV )
{
	if( nTextureIndex<0 || nPicIndex<0 || nTextureIndex>=m_aTexture ||
		nPicIndex >= m_aTexture[nTextureIndex].nRows * m_aTexture[nTextureIndex].nCols )
	{
		rUV.left = rUV.top = 0.f;
		rUV.right = rUV.bottom = 1.f;
		return;
	}
}

long BIShipCommandList::ShipAdding( bool allLabel, bool bMyShip, bool bEnemy, bool bNeutral, bool bFriend )
{
	long retVal=0;

	// список кораблей
	SHIP_DESCRIBE_LIST::SHIP_DESCR	*sd = g_ShipList.GetShipRoot();
	if(sd==null) return 0;

	// дистанция отсечения корабля из списка
	float sqrRadius = -1.f;
	if( m_pARoot && !m_sCurrentCommandName.IsEmpty() )
	{
		ATTRIBUTES * pA = m_pARoot->GetAttributeClass( "commands" );
		if( pA ) pA = pA->GetAttributeClass( m_sCurrentCommandName );
		if( pA ) sqrRadius = pA->GetAttributeAsFloat("EffectRadius", -1.f);
	}
	// отрицательная дистанция не ограничивает дальность до корабля
	allLabel = sqrRadius<0.f;

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
		retVal += AddToIconList( main_sd->textureNum, main_sd->pictureNum, main_sd->selectPictureNum,
			main_sd->characterIndex, null, -1, null, null );
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
				retVal += AddToIconList( sd->textureNum, sd->pictureNum, sd->selectPictureNum,
					sd->characterIndex, null, -1, null, null );
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

	float sqrRadius = pL->r;
	char * stmp = "Commands";
	if(m_nCurrentCommandMode & BI_COMMODE_ABILITY_ICONS) stmp = "AbilityIcons";
	ATTRIBUTES * pA = m_pARoot->GetAttributeClass(stmp);
	if( pA && !m_sCurrentCommandName.IsEmpty() ) pA = pA->GetAttributeClass( m_sCurrentCommandName );
	if( pA ) sqrRadius = pA->GetAttributeAsFloat("EffectRadius",sqrRadius);
	allLabel = sqrRadius<0.f;
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
			retVal += AddToIconList( pL->texIdx, pL->picIdx, pL->selPicIdx,
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

	float sqrRadius = pL->r;
	char * stmp = "Commands";
	if( m_nCurrentCommandMode & BI_COMMODE_ABILITY_ICONS ) stmp = "AbilityIcons";
	ATTRIBUTES * pA = m_pARoot->GetAttributeClass(stmp);
	if( pA && !m_sCurrentCommandName.IsEmpty() ) pA = pA->GetAttributeClass( m_sCurrentCommandName );
	if( pA ) sqrRadius = pA->GetAttributeAsFloat("EffectRadius",sqrRadius);
	allLabel = sqrRadius<0.f;
	sqrRadius *= sqrRadius;

	// Определим координаты принимающего команды корабля
	long selectedCharacter = -1;
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
		retVal += AddToIconList( pL->texIdx, pL->picIdx, pL->selPicIdx,
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
		long texNum = pA->GetAttributeAsDword("texNum",m_nCommandTextureNum);
		char * eventName = pA->GetAttribute("event");
		retVal += AddToIconList( texNum, pictureNum, selPictureNum,
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
		retVal += AddToIconList( m_nChargeTextureNum, nNormalPicIndex, nSelectPicIndex, -1, null, i+1, null, null );
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
		retVal += AddToIconList( textureNum, pictureNum, selPictureNum, -1, null, i+1, pA->GetAttribute("name"), pA->GetAttribute("note") );
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
		char * eventName = pA->GetAttribute("event");
		retVal += AddToIconList( textureNum, pictureNum, selPictureNum, -1, eventName, -1, null, pA->GetAttribute("note") );
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
	return AddToIconList( textureNum, pictureNum, selPictureNum, -1, pA->GetAttribute("event"), -1, null, pA->GetAttribute("note") );
}

long BIShipCommandList::AddToIconList( long nTextureNum, long nNormPictureNum, long nSelPictureNum,
				   long nCharacterIndex, const char* pcCommandName, long nTargetIndex, const char* pcLocName,
				   const char* pcNoteName )
{
	long n = m_aUsedCommand.Add();
	m_aUsedCommand[n].nCharIndex = nCharacterIndex;
	m_aUsedCommand[n].nNormPictureIndex = nNormPictureNum;
	m_aUsedCommand[n].nSelPictureIndex = nSelPictureNum;
	m_aUsedCommand[n].nTargetIndex = nTargetIndex;
	m_aUsedCommand[n].nTextureIndex = nTextureNum;
	m_aUsedCommand[n].sCommandName = pcCommandName;
	m_aUsedCommand[n].sLocName = pcLocName;
	m_aUsedCommand[n].sNote = pcNoteName;
	return 1;
}

long BIShipCommandList::IconAdd( long nPictureNum, long nTextureNum )
{
	if( nTextureNum<0 || nTextureNum>=m_aTexture ||
		nPictureNum<0 || nPictureNum>=m_aTexture[nTextureNum].nCols*m_aTexture[nTextureNum].nRows )
		return 0;

	FRECT uv;
	RECT rpos;
	m_pImgRender->CreateImage( BIType_square, m_aTexture[nTextureNum].sFileName, 0xFF808080,
		GetPictureUV(nTextureNum, nPictureNum, uv), GetCurrentPos(rpos) );
	return 1;
}

FRECT& BIShipCommandList::GetPictureUV( long nTextureNum, long nPictureNum, FRECT& uv )
{
	if( nTextureNum<0 || nTextureNum>=m_aTexture ||
		nPictureNum<0 || nPictureNum>=m_aTexture[nTextureNum].nCols*m_aTexture[nTextureNum].nRows )
	{
		uv.left = uv.top = 0.f;
		uv.right = uv.bottom = 1.f;
	}
	else
	{
		float fDU = 1.f / m_aTexture[nTextureNum].nCols;
		float fDV = 1.f / m_aTexture[nTextureNum].nRows;
		long ny = nPictureNum / m_aTexture[nTextureNum].nCols;
		long nx = nPictureNum - ny * m_aTexture[nTextureNum].nCols;
		uv.left = nx * fDU;	uv.right = uv.left + fDU;
		uv.top = ny * fDV;	uv.bottom = uv.top + fDV;
	}
	return uv;
}

RECT& BIShipCommandList::GetCurrentPos(RECT& rpos)
{
	rpos.left = m_LeftTopPoint.x + m_pImgRender->GetImageQuantity() * (m_IconSize.x + m_nIconSpace);
	rpos.right = rpos.left + m_IconSize.x;
	rpos.top = m_LeftTopPoint.y;
	rpos.bottom = rpos.top + m_IconSize.y;
	return rpos;
}

void BIShipCommandList::UpdateShowIcon()
{
	m_pImgRender->ReleaseAllImages();

	m_bLeftArrow = m_nStartUsedCommandIndex>0;
	m_bRightArrow = false;

	long i = 0;
	for(long n=m_nStartUsedCommandIndex; n<m_aUsedCommand && i<m_nIconShowMaxQuantity; n++,i++)
	{
		if( n==m_nSelectedCommandIndex ) {
			RECT rPos;
			GetCurrentPos(rPos);
			IconAdd( m_aUsedCommand[n].nSelPictureIndex, m_aUsedCommand[n].nTextureIndex );
			SetNote( m_aUsedCommand[n].sNote, (rPos.left+rPos.right)/2, (rPos.top+rPos.bottom)/2 );
		} else
			IconAdd( m_aUsedCommand[n].nNormPictureIndex, m_aUsedCommand[n].nTextureIndex );
	}
}

void BIShipCommandList::SetNote( const char* pcNote, long nX, long nY )
{
	m_NoteText = pcNote;
	m_NotePos.x = nX + m_NoteOffset.x;
	m_NotePos.y = nY + m_NoteOffset.y;
}