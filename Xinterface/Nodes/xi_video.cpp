#include <stdio.h>
#include "xi_video.h"

CXI_VIDEO::CXI_VIDEO()
{
	m_rs=0;
	pTex=0;
	m_nNodeType = NODETYPE_VIDEO;
}

CXI_VIDEO::~CXI_VIDEO()
{
	ReleaseAll();
}

DWORD vid_counter=0;
void CXI_VIDEO::Draw(bool bSelected,dword Delta_Time)
{
	if(m_bUse)
	{
		// Create rectangle
		XI_ONETEX_VERTEX v[4];
		v[0].pos.x = (float)m_rect.left;  v[0].pos.y = (float)m_rect.top;	 v[0].tu=m_rectTex.left; v[0].tv=m_rectTex.bottom;
		v[1].pos.x = (float)m_rect.left;  v[1].pos.y = (float)m_rect.bottom; v[1].tu=m_rectTex.left; v[1].tv=m_rectTex.top;
		v[2].pos.x = (float)m_rect.right; v[2].pos.y = (float)m_rect.top;    v[2].tu=m_rectTex.right; v[2].tv=m_rectTex.bottom;
		v[3].pos.x = (float)m_rect.right, v[3].pos.y = (float)m_rect.bottom; v[3].tu=m_rectTex.right; v[3].tv=m_rectTex.top;
		for(int i=0; i<4; i++)
		{
			v[i].color = m_dwColor;
			v[i].pos.z = 1.f;
		}

		m_rs->SetTexture(0,pTex ? pTex->m_pTexture : 0);
		if(vid_counter++>10)
			m_rs->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,XI_ONETEX_FVF,2,v,sizeof(XI_ONETEX_VERTEX),"iVideo");
	}
}

bool CXI_VIDEO::Init(INIFILE *ini1,char *name1, INIFILE *ini2,char *name2, VDX8RENDER *rs, XYRECT &hostRect, XYPOINT &ScreenSize)
{
	if( !CINODE::Init(ini1,name1, ini2,name2, rs,hostRect,ScreenSize) ) return false;
	SetGlowCursor(false);
	return true;
}

void CXI_VIDEO::LoadIni(INIFILE *ini1,char *name1, INIFILE *ini2,char *name2)
{
	char param[255];

	if(pTex) m_rs->ReleaseVideoTexture(pTex);
	if( ReadIniString(ini1,name1, ini2,name2, "sTexture", param, sizeof(param),"") )
		pTex = m_rs->GetVideoTexture(param);

	m_rectTex = GetIniFloatRect(ini1,name1, ini2,name2, "textureRect", FXYRECT(0.f,0.f,1.f,1.f));

	m_dwColor = GetIniARGB(ini1,name1, ini2,name2, "color", 0xFFFFFFFF);
}

void CXI_VIDEO::ReleaseAll()
{
	VIDEOTEXTURE_RELEASE(m_rs,pTex);
}

int CXI_VIDEO::CommandExecute(int wActCode)
{
	return -1;
}

bool CXI_VIDEO::IsClick(int buttonID,long xPos,long yPos)
{
	return false;
}

void CXI_VIDEO::ChangePosition( XYRECT &rNewPos )
{
	m_rect = rNewPos;
}

void CXI_VIDEO::SaveParametersToIni()
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
