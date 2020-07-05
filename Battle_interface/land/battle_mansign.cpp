#include "battle_mansign.h"
#include "battle_mancommand.h"
#include "..\utils.h"

BIManSign::BIManSign( ENTITY_ID& BIEntityID, VDX8RENDER* pRS ) :
	m_aChargeProgress(_FL)
{
	Assert( pRS );

	m_idHostEntity = BIEntityID;
	m_pRS = pRS;
	m_pCommandList = 0;
	m_nManQuantity = 0;

	m_nVBufID = -1;
	m_nIBufID = -1;
	m_nBackTextureID = -1;
	m_nManStateTextureID = -1;
	m_nGunChargeTextureID = -1;
	m_nBackSquareQ = 0;
	m_nManStateSquareQ = 0;
	m_nGunChargeSquareQ = 0;

	m_nMaxSquareQ = 0;
	m_nSquareQ = 0;

	m_bMakeUpdate = true;
	m_bMakeVertexFill = false;
	m_nCommandMode = 0;

	m_nCommandListVerticalOffset = -48;

	m_nCurrentManIndex = 0;

	for(long n=0; n<MAX_MAN_QUANTITY; n++)
		m_Man[n].nTexture = -1;

	m_bIsAlarmOn = false;
	m_nAlarmTextureID = -1;
	m_fAlarmTime = 0.f;
	m_fAlarmUpSpeed = 1.f;
	m_fAlarmDownSpeed = 1.f;
}

BIManSign::~BIManSign()
{
	Release();
}

void BIManSign::Draw()
{
	// update
	if( m_bMakeUpdate )
	{
		Recollect();
		CheckDataChange();
		m_bMakeUpdate = false;

		UpdateCommandList();
		m_bMakeVertexFill = true;
	} else {
		// проверка измеенения каках либо данных
		CheckDataChange();
	}

	// перезаполнение буфера
	if( m_bMakeVertexFill )
	{
		FillVertexBuffer();
		m_bMakeVertexFill = false;
	}

	if( m_nVBufID!=-1 && m_nIBufID!=-1 )
	{
		long nStartV = 0;
		long nStartI = 0;

		for(long n=0; n<m_nManQuantity; n++)
		{
			m_pRS->TextureSet( 0, m_Man[n].nTexture );
			m_pRS->DrawBuffer( m_nVBufID,sizeof(BI_COLOR_VERTEX), m_nIBufID, nStartV,4, nStartI,2, "battle_colorRectangle" );
			nStartV += 4;
		}

		//
		if( m_nBackTextureID && m_nBackSquareQ>0 )
		{
			m_pRS->TextureSet( 0, m_nBackTextureID );
			m_pRS->DrawBuffer( m_nVBufID,sizeof(BI_COLOR_VERTEX), m_nIBufID, nStartV,m_nBackSquareQ*4, nStartI,m_nBackSquareQ*2, "battle_colorRectangle" );
		}
		nStartV += m_nBackSquareQ*4;

		if( m_bIsAlarmOn && m_nManQuantity>0 )
		{
			m_pRS->TextureSet( 0, m_nAlarmTextureID );
			m_pRS->DrawBuffer( m_nVBufID,sizeof(BI_COLOR_VERTEX), m_nIBufID, nStartV,4, nStartI,2, "battle_tex_col_Rectangle" );
			nStartV += 4;
		}

		//
		if( m_nManStateTextureID && m_nManStateSquareQ>0 )
		{
			m_pRS->TextureSet( 0, m_nManStateTextureID );
			m_pRS->DrawBuffer( m_nVBufID,sizeof(BI_COLOR_VERTEX), m_nIBufID, nStartV,m_nManStateSquareQ*4, nStartI,m_nManStateSquareQ*2, "battle_colorRectangle" );
		}
		nStartV += m_nManStateSquareQ*4;

		//
		if( m_nGunChargeTextureID && m_nGunChargeSquareQ>0 )
		{
			m_pRS->TextureSet( 0, m_nGunChargeTextureID );
			m_pRS->DrawBuffer( m_nVBufID,sizeof(BI_COLOR_VERTEX), m_nIBufID, nStartV,m_nGunChargeSquareQ*4, nStartI,m_nGunChargeSquareQ*2, "battle_alphacutcolor" );
		}
		nStartV += m_nGunChargeSquareQ*4;
	}

	if( m_pCommandList )
		m_pCommandList->Draw();
}

void BIManSign::Init( ATTRIBUTES* pRoot, ATTRIBUTES* pA )
{
	long n;
	float fTmp;
	char * pcTmp;
	char param[256];

	m_pARoot = pRoot;

	m_pCommandList = NEW BIManCommandList( m_idHostEntity, pRoot, m_pRS );

	// default value
	m_nBackTextureID = -1;
	m_dwBackColor = ARGB(255,128,128,128);
	FULLRECT( m_rBackUV );
	ZERROPOINT( m_pntBackOffset );
	FILLPOINT( m_pntBackIconSize, 128,128 );

	m_bIsAlarmOn = false;
	m_nAlarmTextureID = -1;
	m_dwAlarmHighColor = ARGB(255,168,28,28);
	m_dwAlarmLowColor = ARGB(64,168,28,28);
	FULLRECT( m_rAlarmUV );
	ZERROPOINT( m_pntAlarmOffset );
	FILLPOINT( m_pntAlarmIconSize, 128,128 );
	m_fAlarmTime = 0.f;
	m_fAlarmUpSpeed = 2.f;
	m_fAlarmDownSpeed = 0.666f;

	m_nManStateTextureID = -1;
	m_dwManStateColor = ARGB(255,128,128,128);
	FULLRECT( m_rManHPUV );
	ZERROPOINT( m_pntManHPOffset );
	FILLPOINT( m_pntManHPIconSize, 128,128 );
	FULLRECT( m_rManEnergyUV );
	ZERROPOINT( m_pntManEnergyOffset );
	FILLPOINT( m_pntManEnergyIconSize, 128,128 );

	m_nGunChargeTextureID = -1;
	m_dwGunChargeColor = ARGB(255,128,128,128);
	m_dwGunChargeBackColor = ARGB(255,128,64,64);
	FULLRECT( m_rGunChargeUV );
	ZERROPOINT( m_pntGunChargeOffset );
	FILLPOINT( m_pntGunChargeIconSize, 128,128 );

	m_dwManFaceColor = ARGB(255,128,128,128);
	ZERROPOINT(m_pntManPicOffset);
	FILLPOINT(m_pntManPicIconSize,64,64);

	m_nCommandListVerticalOffset = -48;

	for( n=0; n<MAX_MAN_QUANTITY; n++ )
	{
		m_Man[n].pntPos.x = 20.f;
		m_Man[n].pntPos.y = 20.f + (m_pntBackIconSize.y + 10.f) * n;
	}

	if( pA )
	{
		pcTmp = pA->GetAttribute( "backtexturename" );
		if( pcTmp ) m_nBackTextureID = m_pRS->TextureCreate( pcTmp );
		m_dwBackColor = pA->GetAttributeAsDword( "backcolor", m_dwBackColor );
		pcTmp = pA->GetAttribute( "backuv" );
		if( pcTmp ) sscanf( pcTmp, "%f,%f,%f,%f", &m_rBackUV.left,&m_rBackUV.top, &m_rBackUV.right,&m_rBackUV.bottom );
		pcTmp = pA->GetAttribute( "backoffset" );
		if( pcTmp ) sscanf( pcTmp, "%f,%f", &m_pntBackOffset.x, &m_pntBackOffset.y );
		pcTmp = pA->GetAttribute( "backiconsize" );
		if( pcTmp ) sscanf( pcTmp, "%f,%f", &m_pntBackIconSize.x, &m_pntBackIconSize.y );

		pcTmp = pA->GetAttribute( "alarmtexturename" );
		if( pcTmp ) m_nAlarmTextureID = m_pRS->TextureCreate( pcTmp );
		m_dwAlarmHighColor = pA->GetAttributeAsDword( "alarmhighcolor", m_dwAlarmHighColor );
		m_dwAlarmLowColor = pA->GetAttributeAsDword( "alarmlowcolor", m_dwAlarmLowColor );
		pcTmp = pA->GetAttribute( "alarmuv" );
		if( pcTmp ) sscanf( pcTmp, "%f,%f,%f,%f", &m_rAlarmUV.left,&m_rAlarmUV.top, &m_rAlarmUV.right,&m_rAlarmUV.bottom );
		pcTmp = pA->GetAttribute( "alarmoffset" );
		if( pcTmp ) sscanf( pcTmp, "%f,%f", &m_pntAlarmOffset.x, &m_pntAlarmOffset.y );
		pcTmp = pA->GetAttribute( "alarmiconsize" );
		if( pcTmp ) sscanf( pcTmp, "%f,%f", &m_pntAlarmIconSize.x, &m_pntAlarmIconSize.y );
		fTmp = pA->GetAttributeAsFloat( "alarmuptime", 0.f );
		if( fTmp > 0.f ) m_fAlarmUpSpeed = 1.f / fTmp;
		fTmp = pA->GetAttributeAsFloat( "alarmdowntime", 0.f );
		if( fTmp > 0.f ) m_fAlarmDownSpeed = 1.f / fTmp;

		pcTmp = pA->GetAttribute( "manstatetexturename" );
		if( pcTmp ) m_nManStateTextureID = m_pRS->TextureCreate( pcTmp );
		m_dwManStateColor = pA->GetAttributeAsDword( "manstatecolor", m_dwManStateColor );
		pcTmp = pA->GetAttribute( "manhpuv" );
		if( pcTmp ) sscanf( pcTmp, "%f,%f,%f,%f", &m_rManHPUV.left,&m_rManHPUV.top, &m_rManHPUV.right,&m_rManHPUV.bottom );
		pcTmp = pA->GetAttribute( "manhpoffset" );
		if( pcTmp ) sscanf( pcTmp, "%f,%f", &m_pntManHPOffset.x, &m_pntManHPOffset.y );
		pcTmp = pA->GetAttribute( "manhpiconsize" );
		if( pcTmp ) sscanf( pcTmp, "%f,%f", &m_pntManHPIconSize.x, &m_pntManHPIconSize.y );
		pcTmp = pA->GetAttribute( "manenegryuv" );
		if( pcTmp ) sscanf( pcTmp, "%f,%f,%f,%f", &m_rManEnergyUV.left,&m_rManEnergyUV.top, &m_rManEnergyUV.right,&m_rManEnergyUV.bottom );
		pcTmp = pA->GetAttribute( "manenegryoffset" );
		if( pcTmp ) sscanf( pcTmp, "%f,%f", &m_pntManEnergyOffset.x, &m_pntManEnergyOffset.y );
		pcTmp = pA->GetAttribute( "manenergyiconsize" );
		if( pcTmp ) sscanf( pcTmp, "%f,%f", &m_pntManEnergyIconSize.x, &m_pntManEnergyIconSize.y );

		pcTmp = pA->GetAttribute( "gunchargetexturename" );
		if( pcTmp ) m_nGunChargeTextureID = m_pRS->TextureCreate( pcTmp );
		m_dwGunChargeColor = pA->GetAttributeAsDword( "gunchargecolor", m_dwGunChargeColor );
		m_dwGunChargeBackColor = pA->GetAttributeAsDword( "gunchargebackcolor", m_dwGunChargeBackColor );
		pcTmp = pA->GetAttribute( "gunchargeuv" );
		if( pcTmp ) sscanf( pcTmp, "%f,%f,%f,%f", &m_rGunChargeUV.left,&m_rGunChargeUV.top, &m_rGunChargeUV.right,&m_rGunChargeUV.bottom );
		pcTmp = pA->GetAttribute( "gunchargeoffset" );
		if( pcTmp ) sscanf( pcTmp, "%f,%f", &m_pntGunChargeOffset.x, &m_pntGunChargeOffset.y );
		pcTmp = pA->GetAttribute( "gunchargeiconsize" );
		if( pcTmp ) sscanf( pcTmp, "%f,%f", &m_pntGunChargeIconSize.x, &m_pntGunChargeIconSize.y );
		pcTmp = pA->GetAttribute( "gunchargeprogress" );
		if( pcTmp ) {
			do {
				m_aChargeProgress.Add( BIUtils::GetFromStr_Float(pcTmp,0.f) );
			} while( pcTmp[0] );
		}

		m_dwManFaceColor = pA->GetAttributeAsDword( "manfacecolor", m_dwManFaceColor );
		pcTmp = pA->GetAttribute( "manfaceoffset" );
		if( pcTmp ) sscanf( pcTmp, "%f,%f", &m_pntManPicOffset.x, &m_pntManPicOffset.y );
		pcTmp = pA->GetAttribute( "manfaceiconsize" );
		if( pcTmp ) sscanf( pcTmp, "%f,%f", &m_pntManPicIconSize.x, &m_pntManPicIconSize.y );

		m_nCommandListVerticalOffset = pA->GetAttributeAsDword("commandlistverticaloffset");

		for( n=0; n<MAX_MAN_QUANTITY; n++ )
		{
			_snprintf( param,sizeof(param), "iconoffset%d", n+1 );
			pcTmp = pA->GetAttribute( param );
			if( pcTmp ) sscanf( pcTmp,"%f,%f", &m_Man[n].pntPos.x, &m_Man[n].pntPos.y );
		}
	}

	m_bMakeUpdate = true;
}

long BIManSign::AddTexture( const char* pcTextureName, long nCols, long nRows )
{
	if( m_pCommandList ) return m_pCommandList->AddTexture( pcTextureName,nCols,nRows );
	return -1;
}

void BIManSign::Recollect()
{
	long n = CalculateManQuantity();
	UpdateBuffers( n );
}

bool BIManSign::IsActive()
{
	if(!m_pCommandList) return false;
	return m_pCommandList->GetActive();
}

void BIManSign::SetActive(bool bActive)
{
	bool bSameActive = (IsActive()==bActive);
	if( m_pCommandList ) m_pCommandList->SetActive( bActive );
	if( bSameActive ) return;
	m_nCommandMode = BI_COMMODE_COMMAND_SELECT;
	m_bMakeUpdate = true;
}

void BIManSign::MakeControl()
{
	CONTROL_STATE cs;

	api->Controls->GetControlState( BI_COMMANDS_CONFIRM, cs );
	if( cs.state == CST_ACTIVATED )
		ExecuteCommand( BI_MSG_COMMAND_ACTIVATE );

	api->Controls->GetControlState(BI_COMMANDS_LEFTSTEP,cs);
	if(cs.state == CST_ACTIVATED)
		ExecuteCommand( BI_MSG_COMMAND_LEFT );

	api->Controls->GetControlState( BI_COMMANDS_RIGHTSTEP,cs);
	if(cs.state == CST_ACTIVATED)
		ExecuteCommand( BI_MSG_COMMAND_RIGHT );

	api->Controls->GetControlState( BI_COMMANDS_UPSTEP,cs);
	if(cs.state == CST_ACTIVATED)
		ExecuteCommand( BI_MSG_COMMAND_UP );

	api->Controls->GetControlState( BI_COMMANDS_DOWNSTEP,cs);
	if(cs.state == CST_ACTIVATED)
		ExecuteCommand( BI_MSG_COMMAND_DOWN );

	api->Controls->GetControlState(BI_COMMANDS_CANCEL,cs);
	if(cs.state == CST_ACTIVATED)
		ExecuteCommand( BI_MSG_COMMAND_DEACTIVATE );
}

void BIManSign::ExecuteCommand( long command )
{
	switch( command )
	{
	case BI_MSG_COMMAND_ACTIVATE:
		if( m_pCommandList ) {
			long nTmp = m_pCommandList->ExecuteConfirm();
			if( nTmp != -1 ) m_nCommandMode = nTmp;
		}
		if( m_nCommandMode == 0 )
			SetActive( false );
	break;

	case BI_MSG_COMMAND_LEFT:
		if( m_pCommandList )
			m_pCommandList->ExecuteLeft();
	break;

	case BI_MSG_COMMAND_RIGHT:
		if( m_pCommandList )
			m_pCommandList->ExecuteRight();
	break;

	case BI_MSG_COMMAND_UP:
	break;

	case BI_MSG_COMMAND_DOWN:
	break;

	case BI_MSG_COMMAND_DEACTIVATE:
		m_nCommandMode = m_pCommandList->ExecuteCancel();
		if( m_nCommandMode == 0 )
			SetActive( false );
		else UpdateCommandList();
	break;

	default:
		api->Trace( "Warning! Unknown executing command: %d", command );
	}
}

void BIManSign::Release()
{
	DELETE( m_pCommandList );
	TEXTURE_RELEASE( m_pRS, m_nBackTextureID );
	//TEXTURE_RELEASE( m_pRS, m_nShipTextureID );
	TEXTURE_RELEASE( m_pRS, m_nManStateTextureID );
	TEXTURE_RELEASE( m_pRS, m_nGunChargeTextureID );
	VERTEX_BUFFER_RELEASE( m_pRS, m_nVBufID );
	INDEX_BUFFER_RELEASE( m_pRS, m_nIBufID );

	m_nMaxSquareQ = 0;
	m_nSquareQ = 0;
	m_nBackSquareQ = 0;
	m_nManStateSquareQ = 0;
	m_nGunChargeSquareQ = 0;
}

long BIManSign::CalculateManQuantity()
{
	long n;

	// сброс всех
	m_nManQuantity = 0;
	for( n=0; n<MAX_MAN_QUANTITY; n++ )
	{
		m_Man[n].nCharacterIndex = -1;
		m_Man[n].nSlotIndex = -1;
		m_Man[n].fHealth = 0.f;
		m_Man[n].fEnergy = 0.f;
		m_Man[n].bAlarm = false;
		m_Man[n].nShootMax = 4;
		m_Man[n].nShootCurrent = 0;
	}

	ATTRIBUTES* pAttr = m_pARoot ? m_pARoot->GetAttributeClass("data") : 0;
	if( pAttr ) pAttr = pAttr->GetAttributeClass("icons");
	char attrname[128];
	if( pAttr ) for(n=0; n<MAX_MAN_QUANTITY; n++) {
		_snprintf( attrname, sizeof(attrname), "id%d", n );
		ATTRIBUTES* pA = pAttr->GetAttributeClass(attrname);
		if( !pA ) continue;
		m_Man[m_nManQuantity].nCharacterIndex = -1;
		m_Man[m_nManQuantity].nSlotIndex = n;
		m_nManQuantity++;
	}

	return m_nManQuantity;
}

void BIManSign::UpdateBuffers( long nShipQ )
{
	m_nBackSquareQ = nShipQ;
	m_nManStateSquareQ = nShipQ*2;
	m_nGunChargeSquareQ = nShipQ*2;
	long nManSquareQ = nShipQ;
	long nAlarmSquareQ = (m_bIsAlarmOn&&(nShipQ>0))?1:0;

	long nMaxSquareQ = BIUtils::GetMaxFromFourLong(m_nBackSquareQ,m_nManStateSquareQ,m_nGunChargeSquareQ,nManSquareQ);
	if( m_nMaxSquareQ != nMaxSquareQ )
	{
		m_nMaxSquareQ = nMaxSquareQ;
		INDEX_BUFFER_RELEASE( m_pRS, m_nIBufID );
		if( m_nMaxSquareQ > 0 )
			m_nIBufID = m_pRS->CreateIndexBuffer( m_nMaxSquareQ * 6 * sizeof(word) );
		FillIndexBuffer();
	}

	if( (m_nBackSquareQ + m_nManStateSquareQ + m_nGunChargeSquareQ + nManSquareQ + nAlarmSquareQ) != m_nSquareQ )
	{
		m_nSquareQ = m_nBackSquareQ + m_nManStateSquareQ + m_nGunChargeSquareQ + nManSquareQ + nAlarmSquareQ;
		long n = 6*4 + 1;
		VERTEX_BUFFER_RELEASE( m_pRS, m_nVBufID );
		m_nVBufID = m_pRS->CreateVertexBuffer( BI_COLOR_VERTEX_FORMAT, n * 4 * sizeof(BI_COLOR_VERTEX), D3DUSAGE_WRITEONLY );
	}
	//FillVertexBuffer();
}

void BIManSign::FillIndexBuffer()
{
	if( m_nIBufID < 0 ) return;
	word* pI = (word*)m_pRS->LockIndexBuffer( m_nIBufID );
	if( pI )
	{
		for( long n=0; n<m_nMaxSquareQ; n++ )
		{
			pI[n*6 + 0] = (word)(n*4 + 0);
			pI[n*6 + 1] = (word)(n*4 + 1);
			pI[n*6 + 2] = (word)(n*4 + 2);

			pI[n*6 + 3] = (word)(n*4 + 2);
			pI[n*6 + 4] = (word)(n*4 + 1);
			pI[n*6 + 5] = (word)(n*4 + 3);
		}
		m_pRS->UnLockIndexBuffer( m_nIBufID );
	}
}

void BIManSign::FillVertexBuffer()
{
	long n;
	if( m_nVBufID < 0 ) return;
	BI_COLOR_VERTEX* pV = (BI_COLOR_VERTEX*)m_pRS->LockVertexBuffer( m_nVBufID );
	if( pV )
	{
		long vn = 0;

		// face
		for( n=0; n<m_nManQuantity; n++ )
			vn += WriteSquareToVBuff( &pV[vn], m_Man[n].rUV, m_dwManFaceColor, m_Man[n].pntPos+m_pntManPicOffset, m_pntManPicIconSize );

		// back icon
		for( n=0; n<m_nManQuantity; n++ )
			vn += WriteSquareToVBuff( &pV[vn], m_rBackUV, m_dwBackColor, m_Man[n].pntPos+m_pntBackOffset, m_pntBackIconSize );

		// alarm icon
		if( m_bIsAlarmOn && m_nManQuantity>0 )
		{
			vn += WriteSquareToVBuff( &pV[vn], m_rAlarmUV, GetColorByFactor(m_dwAlarmLowColor,m_dwAlarmHighColor,m_fAlarmTime), m_Man[0].pntPos+m_pntAlarmOffset, m_pntAlarmIconSize );
		}

		// state (HP & Energy)
		for( n=0; n<m_nManQuantity; n++ )
		{
			vn += WriteSquareToVBuffWithProgress( &pV[vn], m_rManHPUV, m_dwManStateColor, m_Man[n].pntPos+m_pntManHPOffset, m_pntManHPIconSize, GetProgressManHP(n), 0.f,0.f,0.f );
			vn += WriteSquareToVBuffWithProgress( &pV[vn], m_rManEnergyUV, m_dwManStateColor, m_Man[n].pntPos+m_pntManEnergyOffset, m_pntManEnergyIconSize, GetProgressManEnergy(n), 0.f,0.f,0.f );
		}

		// Gun charge status
		for( n=0; n<m_nManQuantity; n++ )
		{
			vn += WriteSquareToVBuffWithProgress( &pV[vn], m_rGunChargeUV, m_dwGunChargeBackColor, m_Man[n].pntPos+m_pntGunChargeOffset, m_pntGunChargeIconSize, 0.f,0.f,0.f,GetProgressGunChargeMax(n) );
			vn += WriteSquareToVBuffWithProgress( &pV[vn], m_rGunChargeUV, m_dwGunChargeColor, m_Man[n].pntPos+m_pntGunChargeOffset, m_pntGunChargeIconSize, 0.f,0.f,0.f,GetProgressGunCharge(n) );
		}

		m_pRS->UnLockVertexBuffer( m_nVBufID );
	}
}

long BIManSign::WriteSquareToVBuff( BI_COLOR_VERTEX* pv, FRECT& uv, dword color, BIFPOINT& center, FPOINT& size )
{
	if( !pv ) return 0;

	float fLeft = (float)(center.x - size.x/2);
	float fTop = (float)(center.y - size.y/2);
	float fRight = fLeft + size.x;
	float fBottom = fTop + size.y;

	pv[0].pos.x = fLeft;
	pv[0].pos.y = fTop;
	pv[0].pos.z = 1.f;
	pv[0].w = 0.5f;
	pv[0].col = color;
	pv[0].tu = uv.left;
	pv[0].tv = uv.top;

	pv[1].pos.x = fLeft;
	pv[1].pos.y = fBottom;
	pv[1].pos.z = 1.f;
	pv[1].w = 0.5f;
	pv[1].col = color;
	pv[1].tu = uv.left;
	pv[1].tv = uv.bottom;

	pv[2].pos.x = fRight;
	pv[2].pos.y = fTop;
	pv[2].pos.z = 1.f;
	pv[2].w = 0.5f;
	pv[2].col = color;
	pv[2].tu = uv.right;
	pv[2].tv = uv.top;

	pv[3].pos.x = fRight;
	pv[3].pos.y = fBottom;
	pv[3].pos.z = 1.f;
	pv[3].w = 0.5f;
	pv[3].col = color;
	pv[3].tu = uv.right;
	pv[3].tv = uv.bottom;

	return 4;
}

long BIManSign::WriteSquareToVBuffWithProgress( BI_COLOR_VERTEX* pv, FRECT& uv, dword color, BIFPOINT& center, FPOINT& size, float fClampUp, float fClampDown, float fClampLeft, float fClampRight )
{
	if( !pv ) return 0;

	float fLeft = (float)(center.x - size.x/2);
	float fTop = (float)(center.y - size.y/2);
	float fRight = fLeft + size.x;
	float fBottom = fTop + size.y;

	fLeft += size.x * fClampLeft;
	fRight -= size.x * fClampRight;
	fTop += size.y * fClampUp;
	fBottom += size.y * fClampDown;

	float fLeftUV = uv.left + (uv.right-uv.left) * fClampLeft;
	float fRightUV = uv.right - (uv.right-uv.left) * fClampRight;
	float fTopUV = uv.top + (uv.bottom-uv.top) * fClampUp;
	float fBottomUV = uv.bottom - (uv.bottom-uv.top) * fClampDown;

	pv[0].pos.x = fLeft;
	pv[0].pos.y = fTop;
	pv[0].pos.z = 1.f;
	pv[0].w = 0.5f;
	pv[0].col = color;
	pv[0].tu = fLeftUV;
	pv[0].tv = fTopUV;

	pv[1].pos.x = fLeft;
	pv[1].pos.y = fBottom;
	pv[1].pos.z = 1.f;
	pv[1].w = 0.5f;
	pv[1].col = color;
	pv[1].tu = fLeftUV;
	pv[1].tv = fBottomUV;

	pv[2].pos.x = fRight;
	pv[2].pos.y = fTop;
	pv[2].pos.z = 1.f;
	pv[2].w = 0.5f;
	pv[2].col = color;
	pv[2].tu = fRightUV;
	pv[2].tv = fTopUV;

	pv[3].pos.x = fRight;
	pv[3].pos.y = fBottom;
	pv[3].pos.z = 1.f;
	pv[3].w = 0.5f;
	pv[3].col = color;
	pv[3].tu = fRightUV;
	pv[3].tv = fBottomUV;

	return 4;
}

void BIManSign::UpdateCommandList()
{
	if( m_pCommandList )
		m_pCommandList->Update( GetCurrentCommandTopLine(), GetCurrentCommandCharacterIndex(), GetCurrentCommandMode() );
}

long BIManSign::GetCurrentCommandTopLine()
{
	long n = m_nCurrentManIndex;
	if( n<0 || n>=m_nManQuantity ) n=0;
	return (long)(m_Man[n].pntPos.y) + m_nCommandListVerticalOffset;
}

long BIManSign::GetCurrentCommandCharacterIndex()
{
	long n = m_nCurrentManIndex;
	if( n<0 || n>=m_nManQuantity ) n=0;
	return (long)m_Man[n].nCharacterIndex;
}

long BIManSign::GetCurrentCommandMode()
{
	return m_nCommandMode;
}

float BIManSign::GetProgressManHP(long nIdx)
{
	if( nIdx<0 || nIdx>=m_nManQuantity ) return 1.f;
	return 1.f - m_Man[nIdx].fHealth;
}

float BIManSign::GetProgressManEnergy(long nIdx)
{
	if( nIdx<0 || nIdx>=m_nManQuantity ) return 1.f;
	return 1.f - m_Man[nIdx].fEnergy;
}

float BIManSign::GetProgressGunChargeMax(long nIdx)
{
	if( nIdx<0 || nIdx>=m_nManQuantity ) return 1.f;
	return 1.f - GetGunProgressByIndex( m_Man[nIdx].nShootMax );
}

float BIManSign::GetProgressGunCharge(long nIdx)
{
	if( nIdx<0 || nIdx>=m_nManQuantity ) return 1.f;
	return 1.f - GetGunProgressByIndex( m_Man[nIdx].nShootCurrent );
}

float BIManSign::GetGunProgressByIndex(long nIdx)
{
	if( nIdx<0 || nIdx>=m_aChargeProgress ) return 0.f;
	return m_aChargeProgress[nIdx];
}

void BIManSign::CheckDataChange()
{
	ATTRIBUTES * pAttr = m_pARoot ? m_pARoot->GetAttributeClass("data") : 0;
	if( pAttr && BoolACompare(pAttr,"alarm",m_bIsAlarmOn) ) m_bMakeVertexFill = true;
	if( pAttr ) pAttr = pAttr->GetAttributeClass("icons");
	if( !pAttr ) return;

	char attrname[64];
	for( long n=0; n<MAX_MAN_QUANTITY; n++ )
	{
		if( m_Man[n].nSlotIndex < 0 ) continue;
		_snprintf( attrname,sizeof(attrname), "id%d", m_Man[n].nSlotIndex );
		ATTRIBUTES* pA = pAttr->GetAttributeClass( attrname );
		if( !pA ) continue;

		if( LongACompare(pA,"chrindex",m_Man[n].nCharacterIndex) ) m_bMakeVertexFill = true;
		if( FloatACompare(pA,"health",m_Man[n].fHealth) ) m_bMakeVertexFill = true;
		if( FloatACompare(pA,"energy",m_Man[n].fEnergy) ) m_bMakeVertexFill = true;
		if( LongACompare(pA,"shootMax",m_Man[n].nShootMax) ) m_bMakeVertexFill = true;
		if( LongACompare(pA,"shootCur",m_Man[n].nShootCurrent) ) m_bMakeVertexFill = true;
		if( FRectACompare(pA,"uv",m_Man[n].rUV) ) m_bMakeVertexFill = true;
		if( StringACompare(pA,"texture",m_Man[n].sTexture) ) {
			TEXTURE_RELEASE( m_pRS, m_Man[n].nTexture );
			m_Man[n].nTexture = m_pRS->TextureCreate( m_Man[n].sTexture );
			m_bMakeVertexFill = true;
		}
	}

	if( m_bIsAlarmOn ) {
		m_bMakeVertexFill = true;
		if( m_bAlarmUpDirection ) {
			m_fAlarmTime += api->GetDeltaTime() * .001f * m_fAlarmUpSpeed;
			if( m_fAlarmTime>=1.f ) {
				m_fAlarmTime = 1.f;
				m_bAlarmUpDirection = false;
			}
		} else {
			m_fAlarmTime -= api->GetDeltaTime() * .001f * m_fAlarmDownSpeed;
			if( m_fAlarmTime<=0.f ) {
				m_fAlarmTime = 0.f;
				m_bAlarmUpDirection = true;
			}
		}
	}
}
