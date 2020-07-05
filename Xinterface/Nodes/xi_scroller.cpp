#include <stdio.h>
#include "xi_scroller.h"
#include "..\vxservice.h"
#include "..\xinterface.h"
#include "xi_formttext.h"
#include "xi_table.h"
#include "xi_questtexts.h"
#include "xi_questtitles.h"

void SetVertexRectangleTex(XI_ONLYONETEX_VERTEX * pv,FXYRECT & texRect)
{
	pv[0].tu = texRect.left;		pv[0].tv = texRect.top;
	pv[1].tu = texRect.left;		pv[1].tv = texRect.bottom;
	pv[2].tu = texRect.right;		pv[2].tv = texRect.top;
	pv[3].tu = texRect.right;		pv[3].tv = texRect.bottom;
}

void SetVertexRectanglePos(XI_ONLYONETEX_VERTEX * pv,FXYRECT & posRect)
{
	pv[0].pos.x = posRect.left;		pv[0].pos.y = posRect.top;
	pv[1].pos.x = posRect.left;		pv[1].pos.y = posRect.bottom;
	pv[2].pos.x = posRect.right;	pv[2].pos.y = posRect.top;
	pv[3].pos.x = posRect.right;	pv[3].pos.y = posRect.bottom;
}

CXI_SCROLLER::CXI_SCROLLER() :
	m_asOwnedNodes(_FL)
{
	m_rs=NULL;
	m_nNodeType = NODETYPE_SCROLLER;

	m_idBaseTex = -1;
	m_idRollerTex = -1;
	m_idVBuf = -1;
	m_bSelected = true;

	m_bDragRoll = false;
}

CXI_SCROLLER::~CXI_SCROLLER()
{
	ReleaseAll();
}

void CXI_SCROLLER::Draw(bool bSelected,dword Delta_Time)
{
	if(m_bUse)
	{
		if( m_bDragRoll )
		{
			CONTROL_STATE cs;
			api->Controls->GetControlState("ILClick",cs);
			if(cs.state==CST_INACTIVE || cs.state==CST_INACTIVATED)
				m_bDragRoll = false;
			if( m_bDragRoll ) MouseMove();
		}

		m_rs->TextureSet(0,m_idBaseTex);
		m_rs->DrawPrimitive(D3DPT_TRIANGLESTRIP,m_idVBuf,sizeof(XI_ONLYONETEX_VERTEX),0,2,"iStatusLine");
		m_rs->DrawPrimitive(D3DPT_TRIANGLESTRIP,m_idVBuf,sizeof(XI_ONLYONETEX_VERTEX),4,2,"iStatusLine");
		m_rs->DrawPrimitive(D3DPT_TRIANGLESTRIP,m_idVBuf,sizeof(XI_ONLYONETEX_VERTEX),8,2,"iStatusLine");
		m_rs->TextureSet(0,m_idRollerTex);
		m_rs->DrawPrimitive(D3DPT_TRIANGLESTRIP,m_idVBuf,sizeof(XI_ONLYONETEX_VERTEX),12,2,"iStatusLine");
	}
}

bool CXI_SCROLLER::Init(INIFILE *ini1,char *name1, INIFILE *ini2,char *name2, VDX8RENDER *rs, XYRECT &hostRect, XYPOINT &ScreenSize)
{
	if( !CINODE::Init(ini1,name1, ini2,name2, rs,hostRect,ScreenSize) ) return false;
	// screen position for that is host screen position
	//memcpy(&m_rect,&m_hostRect,sizeof(m_hostRect));
	SetGlowCursor(false);
	return true;
}

void CXI_SCROLLER::ReleaseAll()
{
	m_bUse=false;
	TEXTURE_RELEASE(m_rs,m_idBaseTex);
	TEXTURE_RELEASE(m_rs,m_idRollerTex);
	VERTEX_BUF_RELEASE(m_rs,m_idVBuf);
	m_asOwnedNodes.DelAll();
}

int CXI_SCROLLER::CommandExecute(int wActCode)
{
	if(m_bUse)
	{
		switch(wActCode)
		{
		case ACTION_MOUSECLICK:
			if( m_upButtonPos.left<=m_curMousePos.x && m_upButtonPos.right>=m_curMousePos.x &&
				m_upButtonPos.top<=m_curMousePos.y && m_downButtonPos.bottom>=m_curMousePos.y )
			{
				if( m_upButtonPos.bottom>=m_curMousePos.y )
				{ // верхн€€ кнопка
					UpPress();
					break;
				} else if( m_downButtonPos.top<=m_curMousePos.y )
				{ // нижн€€ кнопка
					DownPress();
					break;
				} else
				{ // середина
					CONTROL_STATE cs;
					api->Controls->GetControlState("ILClick",cs);
					if(cs.state==CST_ACTIVATED) m_bDragRoll = true;
					MouseMove();
				}
			}
		break;
		}
	}
	return -1;
}

void CXI_SCROLLER::LoadIni(INIFILE *ini1,char *name1, INIFILE *ini2,char *name2)
{
	char param[2048];

	if( ReadIniString(ini1,name1, ini2,name2, "OwnedControl", param, sizeof(param),"") )
	{
		CXI_UTILS::StringFillStringArray(param,m_asOwnedNodes);
	}

	// Get texture name and load that texture
	m_idBaseTex = -1;
	if( ReadIniString(ini1,name1, ini2,name2, "baseTexture", param, sizeof(param),"") )
		m_idBaseTex = m_rs->TextureCreate(param);

	m_idRollerTex = -1;
	if( ReadIniString(ini1,name1, ini2,name2, "rollerTexture", param, sizeof(param),"") )
		m_idRollerTex = m_rs->TextureCreate(param);

	// Set buffers
	m_fOffTexHeight = GetIniFloat(ini1,name1, ini2,name2, "begEndTexSize", 0.f);
	m_fOffHeight = GetIniFloat(ini1,name1, ini2,name2, "begEndBaseSize", 0.f);
	m_rollerHeight = GetIniFloat(ini1,name1, ini2,name2, "rollerHeight", 0.f);

	m_rollerPlace.left = (float)m_rect.left;
	m_rollerPlace.right = (float)m_rect.right;
	m_rollerPlace.top = m_rect.top + m_fOffHeight;
	m_rollerPlace.bottom = m_rect.bottom - m_fOffHeight;

	m_ScrollTexRect = GetIniFloatRect(ini1,name1, ini2,name2, "scrollTexPos", FXYRECT(0.f,0.f,1.f,1.f));
	m_RollTexRect = GetIniFloatRect(ini1,name1, ini2,name2, "rollTexPos", FXYRECT(0.f,0.f,1.f,1.f));

	m_idVBuf = m_rs->CreateVertexBuffer(XI_ONLYONETEX_FVF,16*sizeof(XI_ONLYONETEX_VERTEX),D3DUSAGE_WRITEONLY);
	FillVertexBuffer();
}

void CXI_SCROLLER::MakeOwnedControl()
{
	for( long n=0; n<(long)m_asOwnedNodes.Size(); n++ )
	{
		CINODE * pNode = ((XINTERFACE*)g_idInterface.pointer)->FindNode( m_asOwnedNodes[n], 0 );
		if( !pNode ) continue;
		switch( pNode->m_nNodeType )
		{
		case NODETYPE_FORMATEDTEXTS:
			((CXI_FORMATEDTEXT*)pNode)->SetPointer( m_fPos );
		break;
		case NODETYPE_TABLE:
			((CXI_TABLE*)pNode)->ScrollerChanged( m_fPos );
		break;
		case NODETYPE_QTEXTS:
			((CXI_QUESTTEXTS*)pNode)->ScrollerChanged( m_fPos );
		break;
		case NODETYPE_QTITLE:
			((CXI_QUESTTITLE*)pNode)->ScrollerChanged( m_fPos );
		break;

		default:
			api->Trace("Warning! Control %s owned not legal type of control (%s).", m_nodeName, pNode->m_nodeName);
		}
	}
}

void CXI_SCROLLER::UpPress()
{
	api->Event("ScrollTopChange","l",-1);
	float fDelta = GetOwnedStep();
	if( fDelta!=0.f ) {
		SetRollerPos( m_fPos - fDelta );
		MakeOwnedControl();
	}
}

void CXI_SCROLLER::DownPress()
{
	api->Event("ScrollTopChange","l",1);
	float fDelta = GetOwnedStep();
	if( fDelta!=0.f ) {
		SetRollerPos( m_fPos + fDelta );
		MakeOwnedControl();
	}
}

float CXI_SCROLLER::GetOwnedStep()
{
	CINODE * pNode = 0;
	for(long n=0; n<(long)m_asOwnedNodes.Size(); n++ )
	{
		pNode = ((XINTERFACE*)g_idInterface.pointer)->FindNode( m_asOwnedNodes[n], 0 );
		if( pNode ) break;
	}
	if( !pNode ) return 0.f;
	switch( pNode->m_nNodeType )
	{
	case NODETYPE_FORMATEDTEXTS:
		return ((CXI_FORMATEDTEXT*)pNode)->GetLineStep();
	break;
	case NODETYPE_TABLE:
		return ((CXI_TABLE*)pNode)->GetLineStep();
	break;
	case NODETYPE_QTEXTS:
		return ((CXI_QUESTTEXTS*)pNode)->GetLineStep();
	break;
	case NODETYPE_QTITLE:
		return ((CXI_QUESTTITLE*)pNode)->GetLineStep();
	break;

	default:
		api->Trace("Warning! Control %s owned not legal type of control (%s).",m_nodeName,pNode->m_nodeName);
	}
	return 0.f;
}

bool CXI_SCROLLER::IsClick(int buttonID,long xPos,long yPos)
{
	if( xPos>=m_rect.left && xPos<=m_rect.right &&
		yPos>=m_rect.top && yPos<=m_rect.bottom )
	{
		if( !m_bDragRoll )
		{
			m_curMousePos.x = (float)xPos;
			m_curMousePos.y = (float)yPos;
		}
		return true;
	}
	return false;
}

void CXI_SCROLLER::ChangePosition( XYRECT &rNewPos )
{
	m_rect = rNewPos;

	m_rollerPlace.left = (float)m_rect.left;
	m_rollerPlace.right = (float)m_rect.right;
	m_rollerPlace.top = m_rect.top + m_fOffHeight;
	m_rollerPlace.bottom = m_rect.bottom - m_fOffHeight;

	FillVertexBuffer();
}

void CXI_SCROLLER::SaveParametersToIni()
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

void CXI_SCROLLER::SetRollerPos(float pos)
{
	if(pos<0.f) pos=0.f;
	if(pos>1.f) pos=1.f;
	m_fPos = pos;

	m_rollerCur.top = m_rollerPlace.top + m_fPos*(m_rollerPlace.bottom - m_rollerPlace.top - m_rollerHeight);
	m_rollerCur.bottom = m_rollerCur.top+m_rollerHeight;

	XI_ONLYONETEX_VERTEX * pV = (XI_ONLYONETEX_VERTEX *)m_rs->LockVertexBuffer(m_idVBuf);
	if(pV!=NULL)
	{
		SetVertexRectanglePos(&pV[12],m_rollerCur);
		m_rs->UnLockVertexBuffer(m_idVBuf);
	}
}

void CXI_SCROLLER::LinkNodeChanged(float fPos)
{
	if( m_fPos == fPos ) return;
	SetRollerPos( fPos );
	api->Event("ScrollPosChange","fs",fPos,m_nodeName);
}

void CXI_SCROLLER::FillVertexBuffer()
{
	XI_ONLYONETEX_VERTEX * pV = (XI_ONLYONETEX_VERTEX *)m_rs->LockVertexBuffer(m_idVBuf);
	if(pV!=NULL)
	{
		for(int i=0;i<16;i++)
			pV[i].pos.z = 1.f;

		FXYRECT texRect;
		texRect.left = m_ScrollTexRect.left;	texRect.top = m_ScrollTexRect.top + m_fOffTexHeight;
		texRect.right = m_ScrollTexRect.right;	texRect.bottom = m_ScrollTexRect.bottom - m_fOffTexHeight;
		SetVertexRectangleTex(&pV[0],texRect);

		texRect.left = m_ScrollTexRect.left;	texRect.top = m_ScrollTexRect.top;
		texRect.right = m_ScrollTexRect.right;	texRect.bottom = m_ScrollTexRect.top + m_fOffTexHeight;
		SetVertexRectangleTex(&pV[4],texRect);

		texRect.left = m_ScrollTexRect.left;	texRect.top = m_ScrollTexRect.bottom - m_fOffTexHeight;
		texRect.right = m_ScrollTexRect.right;	texRect.bottom = m_ScrollTexRect.bottom;
		SetVertexRectangleTex(&pV[8],texRect);

		texRect.left = m_RollTexRect.left;		texRect.top = m_RollTexRect.top;
		texRect.right = m_RollTexRect.right;	texRect.bottom = m_RollTexRect.bottom;
		SetVertexRectangleTex(&pV[12],texRect);

		FXYRECT baseRect;
		baseRect.left = (float)m_rect.left;		baseRect.top = (float)m_rect.top + m_fOffHeight;
		baseRect.right = (float)m_rect.right;	baseRect.bottom = (float)m_rect.bottom - m_fOffHeight;
		SetVertexRectanglePos(&pV[0],baseRect);

		m_upButtonPos.left = (float)m_rect.left;	m_upButtonPos.top = (float)m_rect.top;
		m_upButtonPos.right = (float)m_rect.right;	m_upButtonPos.bottom = (float)m_rect.top + m_fOffHeight;
		SetVertexRectanglePos(&pV[4],m_upButtonPos);

		m_downButtonPos.left = (float)m_rect.left;		m_downButtonPos.top = (float)m_rect.bottom - m_fOffHeight;
		m_downButtonPos.right = (float)m_rect.right;	m_downButtonPos.bottom = (float)m_rect.bottom;
		SetVertexRectanglePos(&pV[8],m_downButtonPos);

		m_rollerCur.left = m_rollerPlace.left;		m_rollerCur.top = m_rollerPlace.top;
		m_rollerCur.right = m_rollerPlace.right;	m_rollerCur.bottom = m_rollerPlace.top + m_rollerHeight;
		SetVertexRectanglePos(&pV[12],m_rollerCur);

		m_rs->UnLockVertexBuffer(m_idVBuf);
	}
}

void CXI_SCROLLER::MouseMove()
{
	FXYPOINT pntMouse = ptrOwner->GetMousePoint();
	if( pntMouse.y != m_curMousePos.y )
	{
		m_curMousePos = pntMouse;

		float fY = pntMouse.y;
		if( fY < m_rollerPlace.top+m_rollerHeight*.5f )
			fY = m_rollerPlace.top + m_rollerHeight * .5f;
		if( fY > m_rollerPlace.bottom-m_rollerHeight*.5f )
			fY = m_rollerPlace.bottom - m_rollerHeight * .5f;

		float newPos = (fY-m_rollerPlace.top-m_rollerHeight*.5f)/(m_rollerPlace.bottom-m_rollerPlace.top-m_rollerHeight);
		SetRollerPos(newPos);
		api->Event("ScrollPosChange","fs",newPos,m_nodeName);
		MakeOwnedControl();
	}
}
