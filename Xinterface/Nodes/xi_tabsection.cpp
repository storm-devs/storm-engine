#include <stdio.h>
#include "xi_tabsection.h"

CXI_TABSECTION::CXI_TABSECTION()
{
	m_rs = null;
	m_bClickable = true;
	m_nNodeType = NODETYPE_TABSECTION;

	m_idIconTexture = -1;
	m_idVBuf = -1;
	m_idIBuf = -1;
	m_nSubQ = 0;
}

CXI_TABSECTION::~CXI_TABSECTION()
{
	ReleaseAll();
}

void CXI_TABSECTION::Draw(bool bSelected,dword Delta_Time)
{
	// Прорисовка
	if( m_idIconTexture!=-1 && m_idVBuf!=-1 && m_idIBuf!=-1 )
	{
		m_rs->TextureSet( 0, m_idIconTexture );
		m_rs->DrawBuffer( m_idVBuf, sizeof(XI_ONETEX_VERTEX), m_idIBuf, 0, m_nSubQ*4, 0, m_nSubQ*2, "iIcon" );
	}

	// Вывод заголовков
}

bool CXI_TABSECTION::Init(INIFILE *ini1,char *name1, INIFILE *ini2,char *name2, VDX8RENDER *rs, XYRECT &hostRect, XYPOINT &ScreenSize)
{
	if( !CINODE::Init(ini1,name1, ini2,name2, rs,hostRect,ScreenSize) ) return false;
	return true;
}

void CXI_TABSECTION::ReleaseAll()
{
	PICTURE_TEXTURE_RELEASE( pPictureService, m_sIconGroupName, m_idIconTexture );
	VERTEX_BUF_RELEASE( m_rs, m_idVBuf );
	INDEX_BUF_RELEASE( m_rs, m_idIBuf );
	m_nSubQ = 0;
}

int CXI_TABSECTION::CommandExecute(int wActCode)
{
	if(m_bUse)
	{
		switch(wActCode)
		{
		case ACTION_ACTIVATE:
			break;
		case ACTION_MOUSECLICK:
			break;
		}
	}
	return -1;
}

bool CXI_TABSECTION::IsClick(int buttonID,long xPos,long yPos)
{
	if( xPos>=m_rect.left && xPos<=m_rect.right &&
		yPos>=m_rect.top  && yPos<=m_rect.bottom && m_bClickable && m_bSelected && m_bUse)		return true;
	return false;
}

void CXI_TABSECTION::ChangePosition( XYRECT &rNewPos )
{
	m_rect = rNewPos;
}

void CXI_TABSECTION::SaveParametersToIni()
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

dword _cdecl CXI_TABSECTION::MessageProc(long msgcode, MESSAGE & message)
{
	switch(msgcode)
	{
	case 0: //
	break;
	}

	return 0;
}

void CXI_TABSECTION::LoadIni(INIFILE *ini1,char *name1, INIFILE *ini2,char *name2)
{
	SetGlowCursor( false );
}

void CXI_TABSECTION::UpdateImage()
{
}
