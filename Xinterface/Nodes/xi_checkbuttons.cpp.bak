#include <stdio.h>
#include "xi_checkbuttons.h"

#define PicName(bDisable,bSelect) ( (bDisable && !m_sDisablePicture.IsEmpty()) ? m_sDisablePicture : ((bSelect) ? m_sSelectPicture : m_sNormalPicture) )
#define PicColor(bDisable,bSelect) ( (bDisable) ? m_dwDisablePicColor : ((bSelect) ? m_dwSelectPicColor : m_dwNormalPicColor) )

CXI_CHECKBUTTONS::CXI_CHECKBUTTONS() :
	m_aButton(_FL)
{
	m_bClickable = true;
	m_nNodeType = NODETYPE_CHECKBUTTONS;
	m_nFontAlignment = ALIGN_LEFT;
	m_nFontNum = -1;
	m_bClickIntoTextActive = false;
	m_bIndividualPos = false;
	m_nEditableSectionIndex = -1;
}

CXI_CHECKBUTTONS::~CXI_CHECKBUTTONS()
{
	ReleaseAll();
}

void CXI_CHECKBUTTONS::Draw(bool bSelected,dword Delta_Time)
{
	float fX,fY;

	// Напечатать строки
	fX = (float)m_rect.left;
	fY = (float)m_rect.top;
	for( long n=0; n<m_aButton; n++ )
	{
		// определяем цвет строки
		dword dwColor = m_dwNormalFontColor;
		if( m_aButton[n]->bChoose ) dwColor = m_dwSelectFontColor;
		if( m_aButton[n]->bDisable ) dwColor = m_dwDisableFontColor;

		if( m_bIndividualPos && m_aButton[n]->bSetPos ) {
			fX = m_aButton[n]->pos.x;
			fY = m_aButton[n]->pos.y;
		}

		if( m_aButton[n]->pImg )
		{
			m_aButton[n]->pImg->SetDifferentPosition( (long)(fX+m_fpIconOffset.x), (long)(fY+m_fpIconOffset.y),
				(long)m_fpIconSize.x, (long)m_fpIconSize.y );
			m_aButton[n]->pImg->Draw();
		}

		// выведем все строки
		for( long i=0; i<m_aButton[n]->aStr; i++ )
		{
			m_rs->ExtPrint( m_nFontNum, dwColor, 0,ALIGN_LEFT,true,m_fFontScale, m_screenSize.x,m_screenSize.y,
						(long)(fX+m_frTextOffset.left+m_aButton[n]->aStr[i].fX), (long)(fY+m_frTextOffset.top),
						"%s", m_aButton[n]->aStr[i].str.GetBuffer() );
			fY += m_fTextLineHeight;
		}

		// отступ между секциями (одной кнопки от другой)
		fY += m_fTextSectionInterval;
	}
}

bool CXI_CHECKBUTTONS::Init(INIFILE *ini1,char *name1, INIFILE *ini2,char *name2, VDX8RENDER *rs, XYRECT &hostRect, XYPOINT &ScreenSize)
{
	if( !CINODE::Init(ini1,name1, ini2,name2, rs,hostRect,ScreenSize) ) return false;
	return true;
}

void CXI_CHECKBUTTONS::LoadIni(INIFILE *ini1,char *name1, INIFILE *ini2,char *name2)
{
	char param[2048];
	XYPOINT tmpLPnt;

	// Выбор только одного элемента или нет
	m_bExclusiveChoose = GetIniBool( ini1,name1, ini2,name2, "exclusiveChoose", true );

	// Выбор активности текста на клик мыши
	m_bClickIntoTextActive = GetIniBool( ini1,name1, ini2,name2, "textClickable", false );

	// get font number
	if( ReadIniString(ini1,name1, ini2,name2, "font", param, sizeof(param),"") )
	{
		if( (m_nFontNum=m_rs->LoadFont(param))==-1 )
			_CORE_API->Trace("can not load font:'%s'",param);
	}
	m_fFontScale = GetIniFloat( ini1,name1, ini2,name2, "fontScale", 1.f );

	// get normal font color
	m_dwNormalFontColor = GetIniARGB(ini1,name1, ini2,name2, "normalFontColor", 0xFFFFFFFF);
	// selected font color
	m_dwSelectFontColor = GetIniARGB(ini1,name1, ini2,name2, "selectFontColor", m_dwNormalFontColor);
	// disable font color
	m_dwDisableFontColor = GetIniARGB(ini1,name1, ini2,name2, "disableFontColor", m_dwNormalFontColor);

	//
	m_frTextOffset.left = m_frTextOffset.top = m_frTextOffset.right = m_frTextOffset.bottom = 0.f;
	m_frTextOffset = GetIniFloatRect( ini1,name1, ini2,name2, "rect_textoffset", m_frTextOffset );

	m_fTextLineHeight = GetIniFloat( ini1,name1, ini2,name2, "lineheight", (float)m_rs->CharHeight(m_nFontNum) );
	m_fTextSectionInterval = GetIniFloat( ini1,name1, ini2,name2, "sectioninterval", (float)m_rs->CharHeight(m_nFontNum) );

	m_fpIconSize = GetIniFloatPoint( ini1,name1, ini2,name2, "iconsize", m_fpIconSize );
	if( ReadIniString(ini1,name1, ini2,name2, "icongroup", param, sizeof(param),"") ) m_sIconGroupName = param;
	//
	if( ReadIniString(ini1,name1, ini2,name2, "normalpicture", param, sizeof(param),"") ) m_sNormalPicture = param;
	m_dwNormalPicColor = GetIniARGB( ini1,name1, ini2,name2, "normalpiccolor", 0xFFFFFFFF );
	//
	if( ReadIniString(ini1,name1, ini2,name2, "selectpicture", param, sizeof(param),"") ) m_sSelectPicture = param;
	else m_sSelectPicture = m_sNormalPicture;
	m_dwSelectPicColor = GetIniARGB( ini1,name1, ini2,name2, "selectpiccolor", m_dwNormalPicColor );
	//
	if( ReadIniString(ini1,name1, ini2,name2, "disablepicture", param, sizeof(param),"") ) m_sDisablePicture = param;
	else m_sDisablePicture = m_sNormalPicture;
	m_dwDisablePicColor = GetIniARGB( ini1,name1, ini2,name2, "disablepiccolor", m_dwNormalPicColor );

	m_fpIconOffset = GetIniFloatPoint( ini1,name1, ini2,name2, "iconoffset", m_fpIconOffset );

	m_bIndividualPos = GetIniBool( ini1,name1, ini2,name2, "individualpos", false );

	// зачитаем по очереди все секции
	char pcKeyName[128];
	for( long n=0; n<100; n++ )
	{
		sprintf( pcKeyName, "section%d", n+1 );
		if( !ReadIniString(ini1,name1, ini2,name2, pcKeyName, param, sizeof(param),"") ) break;
		char* pTmpChar = param;
		bool bSelect = CXI_UTILS::StringGetLong( pTmpChar ) != 0;
		bool bDisable = CXI_UTILS::StringGetLong( pTmpChar ) != 0;
		AddButton( pTmpChar, bDisable, bSelect );
	}

	// special positions for sections
	if( m_bIndividualPos ) {
		for( n=0; n<m_aButton; n++ ) {
			sprintf( pcKeyName, "pos%d", n+1 );
			if( ReadIniString(ini1,name1, ini2,name2, pcKeyName, param, sizeof(param),"") ) {
				char* pTmpChar = param;
				m_aButton[n]->bSetPos = true;
				m_aButton[n]->pos.x = (float)CXI_UTILS::StringGetLong( pTmpChar );
				m_aButton[n]->pos.y = (float)CXI_UTILS::StringGetLong( pTmpChar );
			}
		}
	}

	m_nFontAlignment = ALIGN_LEFT;
	if( ReadIniString(ini1,name1, ini2,name2, "alignment", param, sizeof(param),"") )
	{
		if( stricmp(param,"center")==0 ) m_nFontAlignment = ALIGN_CENTER;
		if( stricmp(param,"right")==0 ) m_nFontAlignment = ALIGN_RIGHT;
	}

	if( m_bExclusiveChoose )
	{
		for( n=0; n<m_aButton; n++ )
			if( m_aButton[n]->bChoose ) break;
		if( n == m_aButton ) // ни ондного не установлено
			SetButtonOn(0); // по умолчанию первый вариант
	}
	for( n=0; n<m_aButton; n++ )
		WriteToAttributeButtonState( n );

	UpdateAllTextInfo();
}

void CXI_CHECKBUTTONS::ReleaseAll()
{
	FONT_RELEASE(m_rs,m_nFontNum);
	m_aButton.DelAllWithPointers();
}

int CXI_CHECKBUTTONS::CommandExecute(int wActCode)
{
	if(m_bUse)
	{
		switch(wActCode)
		{
		case ACTION_ACTIVATE:
			break;
		//case ACTION_MOUSEDBLCLICK:
		case ACTION_MOUSECLICK:
			CheckMouseClick( ptrOwner->GetMousePoint() );
			break;
		}
	}
	return -1;
}

bool CXI_CHECKBUTTONS::IsClick(int buttonID,long xPos,long yPos)
{
	for( long n=0; n<m_aButton; n++ )
		if( m_aButton[n]->pImg && m_aButton[n]->pImg->IsPointInside(xPos,yPos) )
			return true;
	if( !m_bUse || !m_bClickable ||
		xPos<m_rect.left || xPos>m_rect.right ||
		yPos<m_rect.top  || yPos>m_rect.bottom )
		return false;
	return m_bClickIntoTextActive;
}

void CXI_CHECKBUTTONS::ChangePosition( XYRECT &rNewPos )
{
	m_rect = rNewPos;
	UpdateAllTextInfo();
}

void CXI_CHECKBUTTONS::SaveParametersToIni()
{
	char pcWriteKeyName[128];
	char pcWriteParam[2048];

	INIFILE * pIni = api->fio->OpenIniFile( (char*)ptrOwner->m_sDialogFileName.GetBuffer() );
	if( !pIni ) {
		api->Trace( "Warning! Can`t open ini file name %s", ptrOwner->m_sDialogFileName.GetBuffer() );
		return;
	}

	// save position
	_snprintf( pcWriteParam, sizeof(pcWriteParam), "%d,%d,%d,%d", m_rect.left, m_rect.top, m_rect.right, m_rect.bottom );
	pIni->WriteString( m_nodeName, "position", pcWriteParam );

	if( m_bIndividualPos ) {
		for( long n=0; n<m_aButton; n++ ) {
			sprintf( pcWriteKeyName, "pos%d", n+1 );
			if( m_aButton[n]->bSetPos ) {
				_snprintf( pcWriteParam, sizeof(pcWriteParam), "%d,%d", m_aButton[n]->pos.x, m_aButton[n]->pos.y );
				pIni->WriteString( m_nodeName, pcWriteKeyName, pcWriteParam );
			} else {
				pIni->DeleteKey( m_nodeName, pcWriteKeyName );
			}
		}
	}

	delete pIni;
}

dword _cdecl CXI_CHECKBUTTONS::MessageProc(long msgcode, MESSAGE & message)
{
	switch(msgcode)
	{
	case 0: // добавить кнопку
		{
			char param[1024];
			message.String( sizeof(param), param );
			bool bSelect = (message.Long() != 0);
			bool bDisable = (message.Long() != 0);
			AddButton( param, bDisable, bSelect );
			UpdateTextInfo( m_aButton.Size()-1 );
		}
	break;

	case 1: // установить новый текст на кнопку
		{
			long nButtonNum = message.Long()-1;
			char param[1024];
			message.String( sizeof(param), param );
			ChangeText( nButtonNum, param );
			UpdateTextInfo( nButtonNum );
		}
	break;

	case 2: // установить выбор на кнопку
		{
			long nButtonNum = message.Long()-1;
			bool bChoose = (message.Long() != 0);
			SetCheckToButton( nButtonNum, bChoose );
		}
	break;

	case 3: // получить сосотояние кнопки
		{
			long nButtonNum = message.Long()-1;
			if( nButtonNum<0 || nButtonNum>=m_aButton ) return 0;
			return m_aButton[nButtonNum]->bChoose;
		}
	break;

	case 4: // получить выбранную кнопку (для не эксклюзивного выбора выдает номер первой выбранной)
		{
			for( long n=0; n<m_aButton; n++ )
				if( m_aButton[n]->bChoose )
					return n;
		}
	break;

	case 5: // запретить/разрешить кнопку
		{
			long nButtonNum = message.Long()-1;
			bool bDisable = (message.Long() != 0);
			if( nButtonNum<0 || nButtonNum>=m_aButton ) return 0;
			m_aButton[nButtonNum]->bDisable = bDisable;
			m_aButton[nButtonNum]->pImg->LoadFromBase( m_sIconGroupName,
				PicName( bDisable, m_aButton[nButtonNum]->bChoose ), false );
		}
	break;
	}

	return 0;
}

bool CXI_CHECKBUTTONS::GetInternalNameList( array<string>& aStr )
{
	aStr.DelAll();
	if( m_bIndividualPos ) {
		string sTmp = "all";
		aStr.Add( sTmp );
		for( long n=0; n<m_aButton; n++ )
			if( m_aButton[n]->bSetPos ) {
				sTmp = "btn";
				sTmp += (n+1);
				aStr.Add( sTmp );
			}
		return true;
	}
	return false;
}

void CXI_CHECKBUTTONS::SetInternalName( string& sName )
{
	if( sName == "all" ) {
		m_nEditableSectionIndex = -1;
	} else {
		m_nEditableSectionIndex = -1;
		if( strnicmp(sName.GetBuffer(),"btn",3) == 0 ) {
			m_nEditableSectionIndex = atoi( &sName.GetBuffer()[3] ) - 1;
		}
	}
}

void CXI_CHECKBUTTONS::AddButton( const char* pcText, bool bDisable, bool bSelect )
{
	ButtonDescribe * pBD = NEW ButtonDescribe;
	Assert( pBD );
	pBD->aStr.Add();
	if( pcText && pcText[0]=='#' )
		pBD->aStr[0].str = &pcText[1];
	else
		pBD->aStr[0].str = pStringService->GetString( pStringService->GetStringNum(pcText) );
	pBD->aStr[0].fX = 0;
	pBD->bDisable = bDisable;
	pBD->bChoose = bSelect;
	pBD->pImg = NEW CXI_IMAGE();
	Assert( pBD->pImg );
	pBD->pImg->LoadFromBase( m_sIconGroupName, PicName(bDisable,bSelect) );
	pBD->pImg->SetColor( PicColor(bDisable,bSelect) );
	pBD->pImg->SetSize( (long)m_fpIconSize.x, (long)m_fpIconSize.y );

	m_aButton.Add( pBD );
}

void CXI_CHECKBUTTONS::ChangeText( long nButtonNum, const char* pcText )
{
	if( nButtonNum<0 || nButtonNum>=m_aButton ) return;
	m_aButton[nButtonNum]->aStr.Empty();
	m_aButton[nButtonNum]->aStr.Add();
	m_aButton[nButtonNum]->aStr[0].fX = 0.f;
	m_aButton[nButtonNum]->aStr[0].str = pcText;
}

void CXI_CHECKBUTTONS::CheckMouseClick( const FXYPOINT & pntMouse )
{
	if( m_bClickIntoTextActive )
	{
		if( !m_bIndividualPos )
		{
			if( pntMouse.x < m_rect.left || pntMouse.x > m_rect.right ) return;
			long nY = m_rect.top;
			for( long n=0; n<m_aButton; n++ )
			{
				long nHeight = (long)(m_aButton[n]->aStr.Size() * m_fTextLineHeight);
				if( !m_aButton[n]->bDisable )
				{
					if( pntMouse.y >= nY && pntMouse.y <= nY+nHeight )
					{
						if( m_bExclusiveChoose )
						{
							if( !m_aButton[n]->bChoose )
								SetCheckToButton( n, true );
						} else
							SetCheckToButton( n, !m_aButton[n]->bChoose );
						break;
					}
				}
				nY += nHeight + (long)m_fTextSectionInterval;
			}
			if( n < m_aButton ) return;
		}
	}

	for( long n=0; n<m_aButton; n++ )
	{
		if( m_aButton[n]->bDisable ) continue;
		if( !m_aButton[n]->pImg ) continue;
		if( m_aButton[n]->pImg->IsPointInside((long)pntMouse.x, (long)pntMouse.y) )
		{
			if( m_bExclusiveChoose ) {
				if( !m_aButton[n]->bChoose )
					SetCheckToButton( n, true );
			} else {
				SetCheckToButton( n, !m_aButton[n]->bChoose );
			}
			break;
		}
	}
}

void CXI_CHECKBUTTONS::SetButtonOn( long nButtonNum )
{
	if( nButtonNum < 0 || nButtonNum >= m_aButton ) return;
	if( m_aButton[nButtonNum]->bChoose ) return;
	m_aButton[nButtonNum]->bChoose = true;
	m_aButton[nButtonNum]->pImg->LoadFromBase( m_sIconGroupName, m_sSelectPicture, false );
	WriteToAttributeButtonState( nButtonNum );
}

void CXI_CHECKBUTTONS::SetButtonOff( long nButtonNum )
{
	if( nButtonNum < 0 || nButtonNum >= m_aButton ) return;
	if( !m_aButton[nButtonNum]->bChoose ) return;
	m_aButton[nButtonNum]->bChoose = false;
	m_aButton[nButtonNum]->pImg->LoadFromBase( m_sIconGroupName, m_sNormalPicture, false );
	WriteToAttributeButtonState( nButtonNum );
}

void CXI_CHECKBUTTONS::SetAllButtonsToOff()
{
	for( long n=0; n<m_aButton; n++ )
		if( !m_aButton[n]->bDisable && m_aButton[n]->bChoose )
			SetButtonOff( n );
}

void CXI_CHECKBUTTONS::SetCheckToButton( long nButtonNum, bool bCheck )
{
	if( nButtonNum<0 || nButtonNum>=m_aButton ) return; // нет такой кнопки
	if( m_aButton[nButtonNum]->bChoose == bCheck ) return; // уже установлено это состояние

	if( m_bExclusiveChoose )
	{
		if( bCheck ) SetAllButtonsToOff();
		else return; // не можем снять флаг со всех кнопок
	}

	if( bCheck ) SetButtonOn( nButtonNum );
	else SetButtonOff( nButtonNum );

	api->Event( "CheckButtonChange", "sll", m_nodeName, nButtonNum+1, (bCheck?1:0) );
}

void CXI_CHECKBUTTONS::UpdateAllTextInfo()
{
	for( long n=0; n<m_aButton; n++ )
		UpdateTextInfo( n );
}

void CXI_CHECKBUTTONS::UpdateTextInfo( long nButtonNum )
{
	// получим полный текст
	string sAllText;
	for( long n=0; n<m_aButton[nButtonNum]->aStr; n++ )
		sAllText += m_aButton[nButtonNum]->aStr[n].str;

	long nWidth = (m_rect.right - m_rect.left) - (long)( m_frTextOffset.right + m_frTextOffset.left);
	if( nWidth < 10 ) nWidth = 10;
	array<string> asOutStr(_FL);
	CXI_UTILS::SplitStringByWidth( sAllText, m_nFontNum, m_fFontScale, nWidth, asOutStr );

	m_aButton[nButtonNum]->aStr.DelAll();
	for( n=0; n<asOutStr; n++ )
	{
		m_aButton[nButtonNum]->aStr.Add();
		m_aButton[nButtonNum]->aStr[n].str = asOutStr[n];
		long nOffset = m_rs->StringWidth( (char*)asOutStr[n].GetBuffer(), m_nFontNum, m_fFontScale, 0 );
		switch( m_nFontAlignment )
		{
		case ALIGN_LEFT: m_aButton[nButtonNum]->aStr[n].fX = 0.f; break;
		case ALIGN_CENTER: m_aButton[nButtonNum]->aStr[n].fX = (float)((nWidth - nOffset) / 2); break;
		case ALIGN_RIGHT: m_aButton[nButtonNum]->aStr[n].fX = (float)(nWidth - nOffset); break;
		}
	}
}

void CXI_CHECKBUTTONS::WriteToAttributeButtonState( long nButtonIndex )
{
	if( nButtonIndex < 0 || nButtonIndex >= m_aButton ) return;
	char atrName[128];
	_snprintf( atrName, sizeof(atrName), "state%d", nButtonIndex+1 );
	ATTRIBUTES* pA = ptrOwner->AttributesPointer->GetAttributeClass( m_nodeName );
	if( !pA ) pA = ptrOwner->AttributesPointer->CreateSubAClass( ptrOwner->AttributesPointer, m_nodeName );
	if( pA ) pA->SetAttributeUseDword( atrName, m_aButton[nButtonIndex]->bChoose ? 1 : 0 );
}
