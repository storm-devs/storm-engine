#include <stdio.h>
#include "xi_pceditbox.h"
#include "xi_image.h"

#define WIDTH_SCALE_USED	0.9f
#define HEIGHT_SCALE_USED	0.9f

CXI_PCEDITBOX::CXI_PCEDITBOX()
{
	m_nFontID = -1;
	m_dwFontColor = 0xFFFFFFFF;
	m_pntFontOffset.x = 0;
	m_pntFontOffset.y = 0;
	m_fFontScale = 0;

	m_pLeftImage = 0;
	m_pRightImage = 0;
	m_pMiddleImage = 0;

	m_nEditPos = -1;
	m_nFirstShowCharacterIndex = 0;
	m_bWaitKeyRelease = true;
	m_bDisguiseString = false;
}

CXI_PCEDITBOX::~CXI_PCEDITBOX()
{
	ReleaseAll();
}

void CXI_PCEDITBOX::ReleaseAll()
{
	FONT_RELEASE( m_rs, m_nFontID );
	PTR_DELETE( m_pLeftImage );
	PTR_DELETE( m_pRightImage );
	PTR_DELETE( m_pMiddleImage );
}

void CXI_PCEDITBOX::Draw(bool bSelected,dword Delta_Time)
{
	if( m_pLeftImage ) m_pLeftImage->Draw();
	if( m_pRightImage ) m_pRightImage->Draw();
	if( m_pMiddleImage ) m_pMiddleImage->Draw();

	long x = m_rect.left + m_pntFontOffset.x;
	if( m_nStringAlign==ALIGN_CENTER ) {
		x = (m_rect.left + m_rect.right) / 2;
	} else if( m_nStringAlign==ALIGN_RIGHT ) {
		x = m_rect.right - m_pntFontOffset.x;
	}

	// show out string
	string sString;
	UpdateString( sString );
	if( !sString.IsEmpty() ) //m_rs->ExtPrint( m_nFontID,m_dwFontColor,0,m_nStringAlign,true,m_fFontScale,m_screenSize.x,m_screenSize.y,m_rect.left+m_pntFontOffset.x,m_rect.top+m_pntFontOffset.y,"%s",sString.GetBuffer()+m_nFirstShowCharacterIndex);
	{
		CXI_UTILS::PrintTextIntoWindow( m_rs, m_nFontID, m_dwFontColor, m_nStringAlign, true, m_fFontScale,
			m_screenSize.x,	m_screenSize.y, x, m_rect.top+m_pntFontOffset.y,
			&sString[m_nFirstShowCharacterIndex],
			m_rect.left, m_rect.top, m_rect.right-m_rect.left, m_rect.bottom-m_rect.top );
	}

	// show cursor position
	if( IsEditMode() ) ShowCursorPosition( sString );

	CONTROL_STATE cs;
	api->Controls->GetControlState("IStartButton",cs);
	if( cs.state == CST_INACTIVATED )
		api->Event("editexit","s",m_nodeName);
}

bool CXI_PCEDITBOX::Init(INIFILE *ini1,char *name1, INIFILE *ini2,char *name2, VDX8RENDER *rs, XYRECT &hostRect, XYPOINT &ScreenSize)
{
	if( !CINODE::Init(ini1,name1, ini2,name2, rs,hostRect,ScreenSize) ) return false;
	SetGlowCursor(false);
	return true;
}

int CXI_PCEDITBOX::CommandExecute(int wActCode)
{
	return -1;
}

bool CXI_PCEDITBOX::IsClick(int buttonID,long xPos,long yPos)
{
	if( xPos < m_rect.left ) return false;
	if( xPos > m_rect.right ) return false;
	if( yPos < m_rect.top ) return false;
	if( yPos > m_rect.bottom ) return false;
	return true;
}

void CXI_PCEDITBOX::ChangePosition( XYRECT &rNewPos )
{
	m_rect = rNewPos;

	//m_pntFontOffset.x += m_rect.left;
	//m_pntFontOffset.y += m_rect.top;

	// update position
	long nMiddleLeft = m_rect.left;
	long nMiddleRight = m_rect.right;
	long nHeight = m_rect.bottom - m_rect.top;
	if( m_pLeftImage ) {
		if( m_pLeftImage->IsImagePresent() )
		{
			m_pLeftImage->SetSize( m_pLeftImage->GetWidth(), nHeight );
			m_pLeftImage->SetPosition( m_rect.left, m_rect.top, IPType_LeftTop );
			nMiddleLeft += m_pLeftImage->GetWidth();
		}
	}
	if( m_pRightImage ) {
		if( m_pRightImage->IsImagePresent() )
		{
			m_pRightImage->SetSize( m_pRightImage->GetWidth(), nHeight );
			m_pRightImage->SetPosition( m_rect.right, m_rect.top, IPType_RightTop );
			nMiddleRight -= m_pRightImage->GetWidth();
		}
	}
	if( m_pMiddleImage )
	{
		if( m_pMiddleImage->IsImagePresent() )
		{
			m_pMiddleImage->SetSize( nMiddleRight - nMiddleLeft, nHeight );
			m_pMiddleImage->SetPosition( nMiddleLeft, m_rect.top, IPType_LeftTop );
		}
	}
}

void CXI_PCEDITBOX::SaveParametersToIni()
{
	char pcWriteParam[2048];

	INIFILE * pIni = api->fio->OpenIniFile( (char*)ptrOwner->m_sDialogFileName.GetBuffer() );
	if( !pIni ) {
		api->Trace( "Warning! Can`t open ini file name %s", ptrOwner->m_sDialogFileName.GetBuffer() );
		return;
	}

	// save position
	_snprintf( pcWriteParam, sizeof(pcWriteParam), "%d,%d,%d,%d", m_rect.left, m_rect.top, m_rect.right, m_rect.bottom );
	pIni->WriteString( m_nodeName, "position", pcWriteParam );

	delete pIni;
}

void CXI_PCEDITBOX::LoadIni(INIFILE *ini1,char *name1, INIFILE *ini2,char *name2)
{
	char param[2048];

	// get font number
	if( ReadIniString(ini1,name1, ini2,name2, "strFont", param, sizeof(param),"") )
		if( (m_nFontID=m_rs->LoadFont(param))==-1 )
			api->Trace("can`t load font:'%s'",param);

	// Get font scale
	m_fFontScale = GetIniFloat(ini1,name1, ini2,name2, "fontScale",1.f);

	m_bDisguiseString = GetIniBool(ini1,name1, ini2,name2, "disguisestring",false);

	// Get font color
	m_dwFontColor = GetIniARGB(ini1,name1, ini2,name2, "argbFontColor", 0);

	// Get max string size for edit string
	m_nMaxSize = GetIniLong(ini1,name1, ini2,name2, "stringLength", -1);
	m_nMaxWidth = GetIniLong(ini1,name1, ini2,name2, "stringWidth", -1);

	m_pntFontOffset = GetIniLongPoint(ini1,name1, ini2,name2, "stringoffset", m_pntFontOffset);
	m_nStringAlign = ALIGN_LEFT;
	if( ReadIniString(ini1,name1, ini2,name2, "stringalign", param, sizeof(param), "center") )
	{
		if( stricmp(param,"center") == 0 ) m_nStringAlign = ALIGN_CENTER;
		else if( stricmp(param,"right") == 0 ) m_nStringAlign = ALIGN_RIGHT;
	}
	//m_pntFontOffset.x += m_rect.left;
	//m_pntFontOffset.y += m_rect.top;

	// read images
	if( ReadIniString(ini1,name1, ini2,name2, "leftImage", param, sizeof(param),"") )
	{
		m_pLeftImage = NEW CXI_IMAGE;
		if( m_pLeftImage )
			m_pLeftImage->LoadAccordingToString( param );
	}
	if( ReadIniString(ini1,name1, ini2,name2, "RightImage", param, sizeof(param),"") )
	{
		m_pRightImage = NEW CXI_IMAGE;
		if( m_pRightImage )
			m_pRightImage->LoadAccordingToString( param );
	}
	if( ReadIniString(ini1,name1, ini2,name2, "MiddleImage", param, sizeof(param),"") )
	{
		m_pMiddleImage = NEW CXI_IMAGE;
		if( m_pMiddleImage )
			m_pMiddleImage->LoadAccordingToString( param );
	}

	if( ReadIniString(ini1,name1, ini2,name2, "excludechars", param, sizeof(param),"") )
		m_sExcludeChars = param;
	else m_sExcludeChars = "";

	// update position
	long nMiddleLeft = m_rect.left;
	long nMiddleRight = m_rect.right;
	long nHeight = m_rect.bottom - m_rect.top;
	if( m_pLeftImage ) {
		if( m_pLeftImage->IsImagePresent() )
		{
			m_pLeftImage->SetSize( m_pLeftImage->GetWidth(), nHeight );
			m_pLeftImage->SetPosition( m_rect.left, m_rect.top, IPType_LeftTop );
			nMiddleLeft += m_pLeftImage->GetWidth();
		} else {
			PTR_DELETE( m_pLeftImage );
		}
	}
	if( m_pRightImage ) {
		if( m_pRightImage->IsImagePresent() )
		{
			m_pRightImage->SetSize( m_pRightImage->GetWidth(), nHeight );
			m_pRightImage->SetPosition( m_rect.right, m_rect.top, IPType_RightTop );
			nMiddleRight -= m_pRightImage->GetWidth();
		} else {
			PTR_DELETE( m_pRightImage );
		}
	}
	if( nMiddleLeft >= nMiddleRight ) {PTR_DELETE( m_pMiddleImage );}
	if( m_pMiddleImage )
	{
		if( m_pMiddleImage->IsImagePresent() )
		{
			m_pMiddleImage->SetSize( nMiddleRight - nMiddleLeft, nHeight );
			m_pMiddleImage->SetPosition( nMiddleLeft, m_rect.top, IPType_LeftTop );
		} else {
			PTR_DELETE( m_pMiddleImage );
		}
	}

	ATTRIBUTES* pA = ptrOwner->AttributesPointer->GetAttributeClass( m_nodeName );
	if( !pA ) pA = ptrOwner->AttributesPointer->CreateSubAClass( ptrOwner->AttributesPointer, m_nodeName );
	if( pA && !pA->GetAttribute( "str" ) ) pA->CreateAttribute( "str", "" );
}

void CXI_PCEDITBOX::UpdateString( string & str )
{
	str = "";
	m_nFirstShowCharacterIndex=0;

	ATTRIBUTES * pA = api->Entity_GetAttributeClass(&g_idInterface,m_nodeName);
	if( !pA )
	{
		api->Entity_SetAttribute(&g_idInterface,m_nodeName,"");
		pA = api->Entity_GetAttributeClass(&g_idInterface,m_nodeName);
	}
	if( !pA ) return;
	str = pA->GetAttribute("str");
	if( m_nEditPos < 0 ) m_nEditPos = str.Len();

	if( IsEditMode() )
	{
		if( m_bWaitKeyRelease && api->Controls->GetKeyBufferLength()==0 ) m_bWaitKeyRelease = false;

		if( !m_bWaitKeyRelease ) {
			const KeyDescr* pKeys = api->Controls->GetKeyBuffer();
			for( long n=0; n<api->Controls->GetKeyBufferLength(); n++ ) {
				if( pKeys[n].bSystem ) {
					switch( pKeys[n].ucVKey )
					{
						// символы управления
					case VK_BACK:
						if( m_nEditPos>0 ) {
							m_nEditPos--;
							str[m_nEditPos] = 0;
						} break;
					case VK_END:	m_nEditPos = str.Size(); break;
					case VK_HOME:	m_nEditPos = 0; break;
					case VK_DELETE: if( m_nEditPos < (long)str.Size() )	str.Delete( m_nEditPos, 1 ); break;
					case VK_LEFT: if( m_nEditPos > 0 ) m_nEditPos--; break;
					case VK_RIGHT: if( m_nEditPos < (long)str.Len() ) m_nEditPos++; break;
					}
				} else InsertSymbol( str, pKeys[n].ucVKey );
			}
			if( pA ) pA->SetAttribute( "str", (char*)str.GetBuffer() );
			/*char chr = GetInputSymbol();
			if( chr )
			{
				switch( chr )
				{
					// символы управления
				case SpecSymbol_back:
					if( m_nEditPos>0 ) {
						m_nEditPos--;
						str.Delete(m_nEditPos,1);
					}
					break;
				case SpecSymbol_end:	m_nEditPos = str.Size(); break;
				case SpecSymbol_home:	m_nEditPos = 0; break;
				case SpecSymbol_delete:
					if( m_nEditPos < (long)str.Size() )
						str.Delete( m_nEditPos, 1 );
					break;
				case SpecSymbol_left: if( m_nEditPos > 0 ) m_nEditPos--; break;
				case SpecSymbol_right: if( m_nEditPos < (long)str.Len() ) m_nEditPos++; break;

					// пропускаем ненужные символы
				case SpecSymbol_up:
				case SpecSymbol_down:
				case SpecSymbol_tab:
				case SpecSymbol_return:
				case SpecSymbol_escape:
					break;

					// а это то что мы вводим
				default:
					InsertSymbol( str, chr );
				}
				if( pA ) pA->SetAttribute( "str", (char*)str.GetBuffer() );
			}*/

			if( m_bDisguiseString ) DisguiseString( str );

			// определение первого выводимого символа
			char param[2048];
			param[sizeof(param)-1] = 0;
			_snprintf( param,sizeof(param)-1, "%s",str.GetBuffer() );
			for( m_nFirstShowCharacterIndex=0; m_nFirstShowCharacterIndex<m_nEditPos; m_nFirstShowCharacterIndex++ )
			{
				if( m_rs->StringWidth( &param[m_nFirstShowCharacterIndex], m_nFontID, m_fFontScale ) <= (m_rect.right - m_rect.left - 2*m_pntFontOffset.x) )
					break;
			}
		} else {
			if( m_bDisguiseString ) DisguiseString( str );
		}
	} else {
		m_bWaitKeyRelease = true;
		if( m_bDisguiseString ) DisguiseString( str );
	}
}

char CXI_PCEDITBOX::GetInputSymbol()
{
	char chr = CXI_UTILS::GetKeyInput();
	return chr;
}

void CXI_PCEDITBOX::ShowCursorPosition( string & str )
{
	if( m_nEditPos < 0 ) m_nEditPos = 0;
	if( m_nEditPos > (long)str.Len() ) m_nEditPos = (long)str.Len();
	string strForPosCalculate = str;
	strForPosCalculate.Delete( m_nEditPos, str.Len() );
	if( m_nFirstShowCharacterIndex<0 || m_nFirstShowCharacterIndex>m_nEditPos ) return;
	long nPos = m_rs->StringWidth( (char*)strForPosCalculate.GetBuffer()+m_nFirstShowCharacterIndex, m_nFontID, m_fFontScale );
	if( m_nStringAlign == ALIGN_CENTER )
		nPos -= m_rs->StringWidth( (char*)str.GetBuffer()+m_nFirstShowCharacterIndex, m_nFontID, m_fFontScale ) / 2;
	else if( m_nStringAlign == ALIGN_RIGHT )
		nPos -= m_rs->StringWidth( (char*)str.GetBuffer()+m_nFirstShowCharacterIndex, m_nFontID, m_fFontScale );

	long x = m_rect.left + m_pntFontOffset.x;
	if( m_nStringAlign==ALIGN_CENTER ) {
		x = (m_rect.left + m_rect.right) / 2;
	} else if( m_nStringAlign==ALIGN_RIGHT ) {
		x = m_rect.right - m_pntFontOffset.x;
	}

	m_rs->ExtPrint( m_nFontID,m_dwFontColor,0,ALIGN_LEFT,true,m_fFontScale,m_screenSize.x,m_screenSize.y,x+nPos,m_rect.top+m_pntFontOffset.y,"_");
}

void CXI_PCEDITBOX::InsertSymbol( string & str, char chr )
{
	if( (unsigned char)chr < 32 ) return;
	if( IsExcludeChar(chr) ) return;

	char chrInsert[2];
	chrInsert[0] = chr;
	chrInsert[1] = 0;
	// проверка на максимальное число символов
	if( m_nMaxSize >= 0 && (long)str.Len() >= m_nMaxSize ) return;
	// проверка на максимальную ширину строки
	if( m_nMaxWidth >= 0 )
	{
		string strResult = str;
		strResult.Insert( m_nEditPos, chrInsert );
		if( m_rs->StringWidth( (char*)strResult.GetBuffer(), m_nFontID, m_fFontScale ) > m_nMaxWidth ) return;
	}
	str.Insert( m_nEditPos, chrInsert );
	m_nEditPos++;
	if( m_nEditPos > (long)str.Len() ) m_nEditPos = str.Len();
}

void CXI_PCEDITBOX::DisguiseString( string &str )
{
	long q = str.Len();
	for( long n=0; n<q; n++ )
		str[n] = '*';
}

bool CXI_PCEDITBOX::IsExcludeChar( char chr )
{
	char fndStr[2];
	fndStr[0] = chr;
	fndStr[1] = 0;

	if( m_sExcludeChars.FindSubStr(fndStr)>=0 ) return true;
	return false;
}
