#include <stdio.h>
#include "xi_picture.h"

CXI_PICTURE::CXI_PICTURE()
{
	m_rs = 0;
	m_idTex = -1;
	m_pD3D8Texture = 0;
	m_pTex = null;
	m_nNodeType = NODETYPE_PICTURE;
	m_pcGroupName = null;
	m_bMakeBlind = false;
	m_fCurBlindTime = 0.f;
	m_bBlindUp = true;
	m_fBlindUpSpeed = 0.001f;
	m_fBlindDownSpeed = 0.001f;
	m_dwBlindMin = ARGB(255,128,128,128);
	m_dwBlindMax = ARGB(255,255,255,255);
}

CXI_PICTURE::~CXI_PICTURE()
{
	ReleaseAll();
}

void CXI_PICTURE::Draw(bool bSelected,dword Delta_Time)
{
	if(m_bUse)
	{
		if( m_bMakeBlind )
		{
			if( m_bBlindUp ) {
				m_fCurBlindTime += m_fBlindUpSpeed * Delta_Time;
				if( m_fCurBlindTime >= 1.f )
				{
					m_fCurBlindTime = 1.f;
					m_bBlindUp = false;
				}
			} else {
				m_fCurBlindTime -= m_fBlindDownSpeed * Delta_Time;
				if( m_fCurBlindTime <= 0.f )
				{
					m_fCurBlindTime = 0.f;
					m_bBlindUp = true;
				}
			}
			ChangeColor( ptrOwner->GetBlendColor(m_dwBlindMin,m_dwBlindMax, m_fCurBlindTime) );
		}

		if(m_idTex!=-1 || m_pTex || m_pD3D8Texture)
		{
			if(m_idTex!=-1)	m_rs->TextureSet(0,m_idTex);
			else if( m_pD3D8Texture ) m_rs->SetTexture( 0,m_pD3D8Texture );
			else m_rs->SetTexture(0,m_pTex ? m_pTex->m_pTexture : 0);
			m_rs->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,XI_ONETEX_FVF,2,m_v,sizeof(XI_ONETEX_VERTEX),"iVideo");
		}
	}
}

bool CXI_PICTURE::Init(INIFILE *ini1,char *name1, INIFILE *ini2,char *name2, VDX8RENDER *rs, XYRECT &hostRect, XYPOINT &ScreenSize)
{
	if( !CINODE::Init(ini1,name1, ini2,name2, rs,hostRect,ScreenSize) ) return false;
	return true;
}

void CXI_PICTURE::LoadIni(INIFILE *ini1,char *name1, INIFILE *ini2,char *name2)
{
	char param[255];

	m_idTex = -1;

	FXYRECT texRect = FXYRECT(0.f,0.f,1.f,1.f);

	if( ReadIniString(ini1,name1, ini2,name2, "groupName", param, sizeof(param),"") )
	{
		m_idTex = pPictureService->GetTextureID( param );
		m_pcGroupName = NEW char[strlen(param)+1];
		Assert( m_pcGroupName );
		strcpy( m_pcGroupName, param );

		if( ReadIniString(ini1,name1, ini2,name2, "picName", param, sizeof(param),"") )
			pPictureService->GetTexturePos( m_pcGroupName, param, texRect );
	}
	else
	{
		if( ReadIniString(ini1,name1, ini2,name2, "textureName", param, sizeof(param),"") )
			m_idTex = m_rs->TextureCreate(param);
		texRect = GetIniFloatRect(ini1,name1, ini2,name2, "textureRect", texRect);
	}

	m_pTex = null;
	if( ReadIniString(ini1,name1, ini2,name2, "videoName", param, sizeof(param),"") )
		m_pTex = m_rs->GetVideoTexture(param);

	DWORD color = GetIniARGB(ini1,name1, ini2,name2, "color", ARGB(255,128,128,128));

	// Create rectangle
	ChangePosition( m_rect );
	ChangeUV( texRect );
	for(int i=0; i<4; i++)
	{
		m_v[i].color = color;
		m_v[i].pos.z = 1.f;
	}

	m_bMakeBlind = GetIniBool(ini1,name1, ini2,name2, "blind", false);
	m_fCurBlindTime = 0.f;
	m_bBlindUp = true;
	float fTmp = GetIniFloat(ini1,name1, ini2,name2, "blindUpTime", 1.f);
	if( fTmp>0.f ) m_fBlindUpSpeed = 0.001f / fTmp;
	fTmp = GetIniFloat(ini1,name1, ini2,name2, "blindDownTime", 1.f);
	if( fTmp>0.f ) m_fBlindDownSpeed = 0.001f / fTmp;
	m_dwBlindMin = GetIniARGB(ini1,name1, ini2,name2, "blindMinColor", ARGB(255,128,128,128));
	m_dwBlindMax = GetIniARGB(ini1,name1, ini2,name2, "blindMaxColor", ARGB(255,255,255,255));
}

void CXI_PICTURE::ReleaseAll()
{
	ReleasePicture();
}

int CXI_PICTURE::CommandExecute(int wActCode)
{
	return -1;
}

bool CXI_PICTURE::IsClick(int buttonID,long xPos,long yPos)
{
	if( m_bClickable ) {
		if( xPos>=m_rect.left && xPos<=m_rect.right &&
			yPos>=m_rect.top && yPos<=m_rect.bottom )
			return true;
	}
	return false;
}

void CXI_PICTURE::ChangePosition( XYRECT &rNewPos )
{
	m_rect = rNewPos;
	m_v[0].pos.x = (float)m_rect.left;  	m_v[0].pos.y = (float)m_rect.top;
	m_v[1].pos.x = (float)m_rect.left;  	m_v[1].pos.y = (float)m_rect.bottom;
	m_v[2].pos.x = (float)m_rect.right; 	m_v[2].pos.y = (float)m_rect.top;
	m_v[3].pos.x = (float)m_rect.right; 	m_v[3].pos.y = (float)m_rect.bottom;
}

void CXI_PICTURE::SaveParametersToIni()
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

void CXI_PICTURE::SetNewPicture(bool video, char * sNewTexName)
{
	ReleasePicture();
	if(video)	m_pTex = m_rs->GetVideoTexture(sNewTexName);
	else	m_idTex = m_rs->TextureCreate(sNewTexName);

	FXYRECT uv;
	uv.left = uv.top = 0.f;
	uv.right = uv.bottom = 1.f;
	ChangeUV( uv );
}

void CXI_PICTURE::SetNewPictureFromDir(char * dirName)
{
	int findQ;
	WIN32_FIND_DATA	wfd;
	char param[512];

	sprintf(param,"resource\\textures\\%s\\*.tx",dirName);

	HANDLE h = api->fio->_FindFirstFile(param,&wfd);
	for(findQ=0; h!=INVALID_HANDLE_VALUE;)
	{	findQ++;
		if(!api->fio->_FindNextFile(h,&wfd)) break;
	}
	if(h!=INVALID_HANDLE_VALUE)	api->fio->_FindClose(h);

	if(findQ>0)
	{
		findQ = rand()%findQ;
		h = api->fio->_FindFirstFile(param,&wfd);
		if(h!=INVALID_HANDLE_VALUE)	for(;findQ>0;findQ--)
		{
			if(!api->fio->_FindNextFile(h,&wfd)) break;
		}
		if(h!=INVALID_HANDLE_VALUE)
		{
			api->fio->_FindClose(h);
			sprintf(param,"%s\\%s",dirName,wfd.cFileName);
			int paramlen = strlen(param);
			if(paramlen<sizeof(param) && paramlen>=3)	param[paramlen-3] = 0;
			SetNewPicture(false,param);
		}
	}
}

void CXI_PICTURE::SetNewPictureByGroup( char* groupName, char* picName )
{
	if( !m_pcGroupName || stricmp(m_pcGroupName,groupName)!=0 )
	{
		ReleasePicture();
		if( groupName )
		{
			m_pcGroupName = NEW char[strlen(groupName)+1];
			Assert( m_pcGroupName );
			strcpy( m_pcGroupName, groupName );
			m_idTex = pPictureService->GetTextureID( groupName );
		}
	}

	if( m_pcGroupName && picName )
	{
		FXYRECT texRect;
		pPictureService->GetTexturePos( m_pcGroupName, picName, texRect );
		ChangeUV( texRect );
	}
}

dword _cdecl CXI_PICTURE::MessageProc(long msgcode, MESSAGE & message)
{
	switch(msgcode)
	{
	case 0: // Установить картинку на новую позицию
		{
			m_rect.left = message.Long();	m_rect.top = message.Long();
			m_rect.right = message.Long();	m_rect.bottom = message.Long();
			ChangePosition( m_rect );
		}
		break;

	case 1:	// Установить текстурные координаты картинки
		{
			FXYRECT texRect;
			texRect.left = message.Float();	texRect.right = message.Float();
			texRect.top = message.Float();	texRect.bottom = message.Float();
			ChangeUV( texRect );
		}
		break;

	case 2: // Установить новую картинку или видео картинку
		{
			bool bVideo = message.Long()!=0;
			char param[256];
			message.String(sizeof(param)-1,param);
			SetNewPicture(bVideo, param);
		}
		break;

	case 3:	// Установить случайную картинку из директории
		{
			char param[256];
			message.String(sizeof(param)-1,param);
			SetNewPictureFromDir(param);
		}
		break;

	case 4: // Установить новый цвет
		{
			DWORD color = message.Long();
			for(int i=0; i<4; i++) m_v[i].color = color;
		}
		break;

	case 5: // установить/снять мигание
		{
			bool bBlind = message.Long() != 0;
			if( m_bMakeBlind != bBlind )
			{
				m_bMakeBlind = bBlind;
				if( !m_bMakeBlind )
					ChangeColor( m_dwBlindMin );
				else
				{
					m_fCurBlindTime = 0.f;
					m_bBlindUp = true;
				}
			}
		}
		break;

	case 6: // set new picture by group and picture name
		{
			char groupName[256];
			char picName[256];
			message.String( sizeof(groupName), groupName );
			message.String( sizeof(picName), picName );
			SetNewPictureByGroup( groupName, picName );
		}
		break;

	case 7: // set new picture by pointer to IDirect3DTexture8
		{
			IDirect3DBaseTexture8* pTex = (IDirect3DBaseTexture8*)message.Long();
			SetNewPictureByPointer( pTex );
		}
		break;

	case 8: // remove texture from other picture to this
		{
			char srcNodeName[256];
			message.String( sizeof(srcNodeName), srcNodeName );
			CINODE* pNod = (CINODE*)ptrOwner->FindNode(srcNodeName,0);
			if( pNod->m_nNodeType != NODETYPE_PICTURE ) {
				api->Trace("Warning! XINTERFACE:: node with name %s have not picture type.", srcNodeName);
			} else {
				ReleasePicture();
				CXI_PICTURE* pOtherPic = (CXI_PICTURE*)pNod;
				if( pOtherPic->m_pD3D8Texture ) {
					SetNewPictureByPointer( pOtherPic->m_pD3D8Texture );
				}
				if( pOtherPic->m_pcGroupName ) {
					m_pcGroupName = pOtherPic->m_pcGroupName;
					pOtherPic->m_pcGroupName = 0;
				}
				if( pOtherPic->m_idTex != -1 ) {
					m_idTex = pOtherPic->m_idTex;
					pOtherPic->m_idTex = -1;
				}
				for( long n=0; n<4; n++ )
				{
					m_v[n].tu = pOtherPic->m_v[n].tu;
					m_v[n].tv = pOtherPic->m_v[n].tv;
				}
				pOtherPic->ReleasePicture();
			}
		}
		break;
	}

	return 0;
}

void CXI_PICTURE::ChangeUV( FXYRECT &frNewUV )
{
	m_v[0].tu=frNewUV.left;  m_v[0].tv=frNewUV.top;
	m_v[1].tu=frNewUV.left;  m_v[1].tv=frNewUV.bottom;
	m_v[2].tu=frNewUV.right; m_v[2].tv=frNewUV.top;
	m_v[3].tu=frNewUV.right; m_v[3].tv=frNewUV.bottom;
}

void CXI_PICTURE::ChangeColor( dword dwColor )
{
	m_v[0].color = m_v[1].color = m_v[2].color = m_v[3].color = dwColor;
}

void CXI_PICTURE::SetPictureSize(long& nWidth, long& nHeight)
{
	if( !m_pD3D8Texture && !m_pTex && m_idTex==-1 ) {
		m_bUse = false;
		nWidth = nHeight = 0;
		return;
	}

	if( nWidth <= 0 ) { // найдем реальную ширину
		nWidth = 128;
	}
	if( nHeight <= 0 ) { // найдем реальную высоту
		nHeight = 128;
	}

	if( nWidth<0 || nHeight<0 ) {
		m_bUse = false;
		nWidth = nHeight = 0;
		return;
	}

	XYRECT rNewPos = m_rect;
	if( rNewPos.right - rNewPos.left != nWidth ) {
		rNewPos.left = (m_rect.left + m_rect.right - nWidth) / 2;
		rNewPos.right = rNewPos.left + nWidth;
	}
	if( rNewPos.bottom - rNewPos.top != nHeight ) {
		rNewPos.top = (m_rect.top + m_rect.bottom - nHeight) / 2;
		rNewPos.bottom = rNewPos.top + nHeight;
	}
	ChangePosition( rNewPos );
}

void CXI_PICTURE::SetNewPictureByPointer( IDirect3DBaseTexture8* pTex )
{
	if( pTex ) pTex->AddRef();
	ReleasePicture();
	m_pD3D8Texture = pTex;

	FXYRECT uv;
	uv.left = uv.top = 0.f;
	uv.right = uv.bottom = 1.f;
	ChangeUV( uv );
}

void CXI_PICTURE::ReleasePicture()
{
	PICTURE_TEXTURE_RELEASE( pPictureService, m_pcGroupName, m_idTex );
	if( m_pD3D8Texture ) m_pD3D8Texture->Release(); m_pD3D8Texture = null;
	DELETE( m_pcGroupName );
	TEXTURE_RELEASE( m_rs, m_idTex );
	VIDEOTEXTURE_RELEASE( m_rs, m_pTex );
}
