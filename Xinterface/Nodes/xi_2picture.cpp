#include <stdio.h>
#include "xi_2picture.h"

void SetRectanglePos(XI_ONETEX_VERTEX v[4],FXYPOINT & center,FXYPOINT &size)
{
	v[0].pos.x = v[1].pos.x = center.x - size.x/2;
	v[2].pos.x = v[3].pos.x = center.x + size.x/2;
	v[0].pos.y = v[2].pos.y = center.y - size.y/2;
	v[1].pos.y = v[3].pos.y = center.y + size.y/2;
}

void SetRectanglePos(XI_NOTEX_VERTEX v[4],FXYPOINT & center,FXYPOINT &size)
{
	v[0].pos.x = v[1].pos.x = center.x - size.x/2;
	v[2].pos.x = v[3].pos.x = center.x + size.x/2;
	v[0].pos.y = v[2].pos.y = center.y - size.y/2;
	v[1].pos.y = v[3].pos.y = center.y + size.y/2;
}

void SetRectangleColor(XI_ONETEX_VERTEX v[4],DWORD color)
{
	for(int i=0;i<4;i++)	v[i].color = color;
}

CXI_TWOPICTURE::CXI_TWOPICTURE()
{
	m_rs=0;
	m_idOneTex=m_idTwoTex = -1L;
	m_nNodeType = NODETYPE_TWOPICTURE;
	m_bSelected = true;
	m_bMouseInsideIndifferent = true;
}

CXI_TWOPICTURE::~CXI_TWOPICTURE()
{
	ReleaseAll();
}

void CXI_TWOPICTURE::Draw(bool bSelected,dword Delta_Time)
{
	if(m_bUse)
	{
		m_rs->TextureSet(0,m_idOneTex);
		m_rs->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,XI_ONETEX_FVF,2,m_vOne,sizeof(XI_ONETEX_VERTEX),"iIcon");
		m_rs->TextureSet(0,m_idTwoTex);
		m_rs->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,XI_ONETEX_FVF,2,m_vTwo,sizeof(XI_ONETEX_VERTEX),"iIcon");
	}
}

bool CXI_TWOPICTURE::Init(INIFILE *ini1,char *name1, INIFILE *ini2,char *name2, VDX8RENDER *rs, XYRECT &hostRect, XYPOINT &ScreenSize)
{
	if( !CINODE::Init(ini1,name1, ini2,name2, rs,hostRect,ScreenSize) ) return false;
	return true;
}

void CXI_TWOPICTURE::LoadIni(INIFILE *ini1,char *name1, INIFILE *ini2,char *name2)
{
	char param[255];

	int pictSpace = GetIniLong(ini1,name1, ini2,name2, "pictureSpace",10);
	m_picSize.x = (m_rect.right - m_rect.left - pictSpace)/2.f;
	m_picSize.y = float(m_rect.bottom - m_rect.top);
	m_leftPicCenter.x = m_rect.left+m_picSize.x/2;
	m_leftPicCenter.y = (m_rect.bottom+m_rect.top)/2.f;
	m_rightPicCenter.x = m_rect.right-m_picSize.x/2;
	m_rightPicCenter.y = (m_rect.bottom+m_rect.top)/2.f;

	m_bMouseInsideIndifferent = (0!=GetIniLong( ini1,name1, ini2,name2, "MouseInsideIndifferent", 1 ));

	// координатные смещения
	m_ShadowOffset = GetIniFloatPoint(ini1,name1, ini2,name2, "offsetShadow", FXYPOINT(4.0,4.0));
	m_PressShadowOffset = GetIniFloatPoint(ini1,name1, ini2,name2, "offsetPressShad", FXYPOINT(4.0,4.0));
	m_PressOffset = GetIniFloatPoint(ini1,name1, ini2,name2, "offsetPress", FXYPOINT(4.0,4.0));

	// текстуры
	if( ReadIniString(ini1,name1, ini2,name2, "oneTexName", param, sizeof(param),"") )
		m_idOneTex = m_rs->TextureCreate(param);
	else	m_idOneTex = -1;

	if( ReadIniString(ini1,name1, ini2,name2, "twoTexName", param, sizeof(param),"") )
		m_idTwoTex = m_rs->TextureCreate(param);
	else	m_idTwoTex = -1;

	// получим используемые цвета
	m_dwSelectColor = GetIniARGB(ini1,name1, ini2,name2, "argbSelectCol", ARGB(255,128,128,128));
	m_dwDarkColor = GetIniARGB(ini1,name1, ini2,name2, "argbDisableCol", ARGB(255,48,48,48));
	m_dwShadowColor = GetIniARGB(ini1,name1, ini2,name2, "argbShadowCol", ARGB(255,48,48,48));

	// текстурные координаты
	FXYRECT texRect = GetIniFloatRect(ini1,name1, ini2,name2, "texPos", FXYRECT(0.f,0.f,1.f,1.f));

	// Create rectangle
	m_vOne[0].tu=m_vTwo[0].tu = texRect.left;	m_vOne[0].tv=m_vTwo[0].tv = texRect.top;
	m_vOne[1].tu=m_vTwo[1].tu = texRect.left;	m_vOne[1].tv=m_vTwo[1].tv = texRect.bottom;
	m_vOne[2].tu=m_vTwo[2].tu = texRect.right;	m_vOne[2].tv=m_vTwo[2].tv = texRect.top;
	m_vOne[3].tu=m_vTwo[3].tu = texRect.right;	m_vOne[3].tv=m_vTwo[3].tv = texRect.bottom;
	for(int i=0; i<4; i++)
	{
		m_vOne[i].pos.z = m_vTwo[i].pos.z = m_vSOne[i].pos.z = m_vSTwo[i].pos.z = 1.f;
		m_vSOne[i].color = m_vSTwo[i].color = m_dwShadowColor;
	}
	m_bLeftSelect = true;

	UpdateRectangles();
}

void CXI_TWOPICTURE::ReleaseAll()
{
	TEXTURE_RELEASE(m_rs,m_idOneTex);
	TEXTURE_RELEASE(m_rs,m_idTwoTex);
}

int CXI_TWOPICTURE::CommandExecute(int wActCode)
{
	if(m_bUse)
	{
		switch(wActCode)
		{
		case ACTION_RIGHTSTEP:
			if(m_bLeftSelect)
			{
				m_bLeftSelect = false;
				UpdateRectangles();
			}
			break;
		case ACTION_LEFTSTEP:
			if(!m_bLeftSelect)
			{
				m_bLeftSelect = true;
				UpdateRectangles();
			}
			break;
		case ACTION_MOUSECLICK:
			if(m_bLeftSelect!=m_leftClick)
			{
				m_bLeftSelect = m_leftClick;
				UpdateRectangles();
			}
			break;
		}
	}
	return -1;
}

bool CXI_TWOPICTURE::IsClick(int buttonID,long xPos,long yPos)
{
	if( xPos>=m_vOne[0].pos.x && xPos<=m_vOne[3].pos.x &&
		yPos>=m_vOne[0].pos.y && yPos<=m_vOne[3].pos.y )
	{
		m_leftClick = true;
		return true;
	}
	if( xPos>=m_vTwo[0].pos.x && xPos<=m_vTwo[3].pos.x &&
		yPos>=m_vTwo[0].pos.y && yPos<=m_vTwo[3].pos.y )
	{
		m_leftClick = false;
		return true;
	}
	return false;
}

void CXI_TWOPICTURE::UpdateRectangles()
{
	ATTRIBUTES * pA = _CORE_API->Entity_GetAttributeClass(&g_idInterface,m_nodeName);
	if(m_bLeftSelect)
	{
		SetRectanglePos(m_vOne,m_leftPicCenter+m_PressOffset,m_picSize);
		SetRectanglePos(m_vSOne,m_leftPicCenter+m_PressOffset+m_PressShadowOffset,m_picSize);

		SetRectanglePos(m_vTwo,m_rightPicCenter,m_picSize);
		SetRectanglePos(m_vSTwo,m_rightPicCenter+m_ShadowOffset,m_picSize);

		SetRectangleColor(m_vOne,m_dwSelectColor);
		SetRectangleColor(m_vTwo,m_dwDarkColor);

		if(pA!=NULL) pA->SetAttributeUseDword("current",0);
	}
	else
	{
		SetRectanglePos(m_vOne,m_leftPicCenter,m_picSize);
		SetRectanglePos(m_vSOne,m_leftPicCenter+m_ShadowOffset,m_picSize);

		SetRectanglePos(m_vTwo,m_rightPicCenter+m_PressOffset,m_picSize);
		SetRectanglePos(m_vSTwo,m_rightPicCenter+m_PressOffset+m_PressShadowOffset,m_picSize);

		SetRectangleColor(m_vOne,m_dwDarkColor);
		SetRectangleColor(m_vTwo,m_dwSelectColor);

		if(pA!=NULL) pA->SetAttributeUseDword("current",1);
	}
}

void CXI_TWOPICTURE::MouseThis(float fX, float fY)
{
	if( m_bMouseInsideIndifferent ) return;
	if( ( fX >= (m_leftPicCenter.x - m_picSize.x/2) ) &&
		( fX <= (m_leftPicCenter.x + m_picSize.x/2) ) &&
		( fY >= (m_leftPicCenter.y - m_picSize.y/2) ) &&
		( fY <= (m_leftPicCenter.y + m_picSize.y/2) )  )
	// указатель мыши над левой картинкой
	{
		if(!m_bLeftSelect)
		{
			m_bLeftSelect = true;
			UpdateRectangles();
		}
		return;
	}

	if( ( fX >= (m_rightPicCenter.x - m_picSize.x/2) ) &&
		( fX <= (m_rightPicCenter.x + m_picSize.x/2) ) &&
		( fY >= (m_rightPicCenter.y - m_picSize.y/2) ) &&
		( fY <= (m_rightPicCenter.y + m_picSize.y/2) )  )
	// указатель мыши над правой картинкой
	{
		if(m_bLeftSelect)
		{
			m_bLeftSelect = false;
			UpdateRectangles();
		}
		return;
	}
}

void CXI_TWOPICTURE::ChangePosition( XYRECT &rNewPos )
{
	float pictSpace = m_rect.right - m_rect.left - 2.f*m_picSize.x;
	if( pictSpace < 0.f ) pictSpace = 0.f;

	m_rect = rNewPos;

	m_picSize.x = (m_rect.right - m_rect.left - pictSpace)/2.f;
	m_picSize.y = float(m_rect.bottom - m_rect.top);
	m_leftPicCenter.x = m_rect.left+m_picSize.x/2;
	m_leftPicCenter.y = (m_rect.bottom+m_rect.top)/2.f;
	m_rightPicCenter.x = m_rect.right-m_picSize.x/2;
	m_rightPicCenter.y = (m_rect.bottom+m_rect.top)/2.f;

	UpdateRectangles();
}

void CXI_TWOPICTURE::SaveParametersToIni()
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
