#include <stdio.h>
#include "xi_window.h"

CXI_WINDOW::CXI_WINDOW() :
	m_aNodeNameList( _FL )
{
	m_nNodeType = NODETYPE_WINDOW;

	m_bActive = true;
	m_bShow = true;
}

CXI_WINDOW::~CXI_WINDOW()
{
}

bool CXI_WINDOW::Init(INIFILE *ini1,char *name1, INIFILE *ini2,char *name2, VDX8RENDER *rs, XYRECT &hostRect, XYPOINT &ScreenSize)
{
	if( !CINODE::Init(ini1,name1, ini2,name2, rs,hostRect,ScreenSize) ) return false;
	SetGlowCursor(false);
	return true;
}

void CXI_WINDOW::ChangePosition( XYRECT &rNewPos )
{
	if( m_rect.right - m_rect.left != rNewPos.right - rNewPos.left )
		rNewPos.right += rNewPos.left + m_rect.right - m_rect.left;
	if( m_rect.bottom - m_rect.top != rNewPos.bottom - rNewPos.top )
		rNewPos.bottom += rNewPos.top + m_rect.bottom - m_rect.top;
	if( rNewPos.top == m_rect.top &&
		rNewPos.left == m_rect.left ) return; // нечего менять - та же позиция
	long nXAdd = rNewPos.left - m_rect.left;
	long nYAdd = rNewPos.top - m_rect.top;
	m_rect = rNewPos;

	for( long n=0; n<m_aNodeNameList; n++ )
	{
		CINODE * pNod = ptrOwner->FindNode( m_aNodeNameList[n], 0 );
		if( pNod )
		{
			XYRECT r = pNod->m_rect;
			r.left += nXAdd;
			r.right += nXAdd;
			r.top += nYAdd;
			r.bottom += nYAdd;
			pNod->ChangePosition( r );
		}
	}
}

void CXI_WINDOW::SaveParametersToIni()
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

	for( long n=0; n<m_aNodeNameList; n++ )
	{
		CINODE * pNod = ptrOwner->FindNode( m_aNodeNameList[n], 0 );
		if( pNod )
			pNod->SaveParametersToIni();
	}
}

void CXI_WINDOW::SetShow( bool bShow )
{
	if( bShow == m_bShow ) return;
	if( !bShow && m_bActive ) SetActive( false );
	m_bShow = bShow;

	// проход по всем нодам и включить/выключить их
	for( long n=0; n<m_aNodeNameList; n++ )
	{
		CINODE * pNod = ptrOwner->FindNode( m_aNodeNameList[n], 0 );
		if( pNod )
		{
			pNod->m_bUse = bShow;
			if( pNod->m_nNodeType == NODETYPE_WINDOW ) ((CXI_WINDOW*)pNod)->SetShow( bShow );
		}
	}
}

void CXI_WINDOW::SetActive( bool bActive )
{
	if( m_bActive == bActive ) return;
	m_bActive = bActive;

	// проход по всем нодам и залочить/разлочить их
	for( long n=0; n<m_aNodeNameList; n++ )
	{
		CINODE * pNod = ptrOwner->FindNode( m_aNodeNameList[n], 0 );
		if( pNod )
		{
			pNod->m_bLockedNode = !bActive;
			if( pNod->m_nNodeType == NODETYPE_WINDOW ) ((CXI_WINDOW*)pNod)->SetActive( bActive );
		}
	}
}

void CXI_WINDOW::AddNode( const char* pcNodeName )
{
	CINODE * pNod = ptrOwner->FindNode( pcNodeName, 0 );
	if( !pNod )
	{
		api->Trace("Warning! CXI_WINDOW::AddNode(%s) : Node not found", pcNodeName);
		return;
	}
	m_aNodeNameList.Add( pcNodeName );

	pNod->m_bUse = m_bShow;
	pNod->m_bLockedNode = !m_bActive;

	XYRECT r = pNod->m_rect;
	r.left += m_rect.left;
	r.right += m_rect.left;
	r.top += m_rect.top;
	r.bottom += m_rect.top;
	pNod->ChangePosition( r );
}

void CXI_WINDOW::LoadIni(INIFILE *ini1,char *name1, INIFILE *ini2,char *name2)
{
	char param[1024];

	// get nodes list
	//if( ReadIniString(ini1,name1, ini2,name2, "nodelist", param, sizeof(param),"") )
	if (ini1 && ini1->ReadString(name1, "nodelist", param, sizeof(param), "")) do
	{
		char *pcStr = param;
		char subparam[256];
		while( pcStr && pcStr[0] )
		{
			pcStr = GetSubStr( pcStr, subparam, sizeof(subparam) );
			m_aNodeNameList.Add( subparam );
		}
	} while (ini1->ReadStringNext(name1, "nodelist", param, sizeof(param)));

	// get active value
	SetActive( GetIniBool(ini1,name1, ini2,name2, "active", true) );

	// get show value
	SetShow( GetIniBool(ini1,name1, ini2,name2, "show", true) );
}
