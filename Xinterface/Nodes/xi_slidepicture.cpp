#include <stdio.h>
#include "xi_slidepicture.h"

void SetTextureCoordinate(XI_ONETEX_VERTEX v[4],FXYRECT tr, float angle)
{
	if(angle==0)
	{
		v[0].tu = v[1].tu = tr.left;
		v[2].tu = v[3].tu = tr.right;
		v[0].tv = v[2].tv = tr.top;
		v[1].tv = v[3].tv = tr.bottom;
	}
	else
	{
		float x = (tr.left+tr.right)*.5f;
		float y = (tr.top+tr.bottom)*.5f;
		float width = tr.right-tr.left;
		float height = tr.bottom-tr.top;
		float ca = cosf(angle);
		float sa = sinf(angle);
		float wca = width/2*ca;
		float wsa = width/2*sa;
		float hca = height/2*ca;
		float hsa = height/2*sa;
		v[0].tu = x + (-wca + hsa);
		v[0].tv = y + (-wsa - hca);
		v[1].tu = x + (-wca - hsa);
		v[1].tv = y + (-wsa + hca);
		v[2].tu = x + (wca + hsa);
		v[2].tv = y + (wsa - hca);
		v[3].tu = x + (wca - hsa);
		v[3].tv = y + (wsa + hca);
	}
}

CXI_SLIDEPICTURE::CXI_SLIDEPICTURE()
{
	nLifeTime = 0;
	m_rs=0;
	m_idTex=-1L;
	m_nNodeType = NODETYPE_SLIDEPICTURE;
	pSlideSpeedList = NULL;
	nSlideListSize = 0;
	strTechniqueName = null;
}

CXI_SLIDEPICTURE::~CXI_SLIDEPICTURE()
{
	ReleaseAll();
}

void CXI_SLIDEPICTURE::Draw(bool bSelected,dword Delta_Time)
{
	if(m_bUse)
	{
		Update(Delta_Time);
		m_rs->TextureSet(0,m_idTex);
		m_rs->SetTextureStageState(0,D3DTSS_ADDRESSU,D3DTADDRESS_WRAP);
		m_rs->SetTextureStageState(0,D3DTSS_ADDRESSV,D3DTADDRESS_WRAP);
		if(strTechniqueName==null)
			m_rs->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,XI_ONETEX_FVF,2,m_v,sizeof(XI_ONETEX_VERTEX),"iVideo");
		else
			m_rs->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,XI_ONETEX_FVF,2,m_v,sizeof(XI_ONETEX_VERTEX),strTechniqueName);
	}
}

bool CXI_SLIDEPICTURE::Init(INIFILE *ini1,char *name1, INIFILE *ini2,char *name2, VDX8RENDER *rs, XYRECT &hostRect, XYPOINT &ScreenSize)
{
	if( !CINODE::Init(ini1,name1, ini2,name2, rs,hostRect,ScreenSize) ) return false;
	SetGlowCursor(false);
	return true;
}

void CXI_SLIDEPICTURE::LoadIni(INIFILE *ini1,char *name1, INIFILE *ini2,char *name2)
{
	int i;
	char param[255];
	FXYPOINT fPos;

	PTR_DELETE(strTechniqueName);
	if( ReadIniString(ini1,name1, ini2,name2, "techniqueName", param, sizeof(param),"") )
	{
		if(strlen(param)>0)
		{	strTechniqueName = NEW char[strlen(param)+1];
			if(strTechniqueName==null)
			{	_THROW("allocate memory error");
			}
			strcpy(strTechniqueName,param);
		}
	}

	m_idTex = -1;
	if( ReadIniString(ini1,name1, ini2,name2, "textureName", param, sizeof(param),"") )
		m_idTex = m_rs->TextureCreate(param);

	m_texRect = GetIniFloatRect(ini1,name1, ini2,name2, "textureRect", FXYRECT(0.f,0.f,1.f,1.f));

	DWORD color = GetIniARGB(ini1,name1, ini2,name2, "color", 0xFFFFFFFF);

	// Create rectangle
	m_v[0].pos.x = (float)m_rect.left;  m_v[0].pos.y = (float)m_rect.top;	 m_v[0].tu=m_texRect.left;  m_v[0].tv=m_texRect.top;
	m_v[1].pos.x = (float)m_rect.left;  m_v[1].pos.y = (float)m_rect.bottom; m_v[1].tu=m_texRect.left;  m_v[1].tv=m_texRect.bottom;
	m_v[2].pos.x = (float)m_rect.right; m_v[2].pos.y = (float)m_rect.top;    m_v[2].tu=m_texRect.right; m_v[2].tv=m_texRect.top;
	m_v[3].pos.x = (float)m_rect.right, m_v[3].pos.y = (float)m_rect.bottom; m_v[3].tu=m_texRect.right; m_v[3].tv=m_texRect.bottom;
	for(i=0; i<4; i++)
	{
		m_v[i].color = color;
		m_v[i].pos.z = 1.f;
	}

	curAngle = 0.f;
	curRotate = 0.f;
	fPos = GetIniFloatPoint(ini1,name1, ini2,name2, "rotate", FXYPOINT(0.f,0.f));
	minRotate = fPos.x;		deltaRotate = fPos.y;

	nLifeTime = 0;
	nCurSlide = 0;
	nSlideListSize = 0;
	pSlideSpeedList = NULL;

	bool bUse1Ini = true;
	// Расчет размера таблицы скоростей
	if( ini1->ReadString(name1,"speed",param,sizeof(param)-1,"") )
	{
		do	nSlideListSize++;
		while(ini1->ReadStringNext(name1,"speed",param,sizeof(param)-1));
	}
	else
	{
		if( ini2->ReadString(name2,"speed",param,sizeof(param)-1,"") )
		{
			bUse1Ini = false;
			do	nSlideListSize++;
			while(ini2->ReadStringNext(name2,"speed",param,sizeof(param)-1));
		}
	}

	if(nSlideListSize>0)
	{
		pSlideSpeedList = NEW SLIDE_SPEED[nSlideListSize];
		if(pSlideSpeedList==null)	{_THROW("allocate memory error");}
	}

	// заполняем таблицу скоростей
	if(bUse1Ini)
	{
		ini1->ReadString(name1,"speed",param,sizeof(param)-1,"");
		for(i=0; i<nSlideListSize; i++)
		{
			pSlideSpeedList[i].time = 0;
			pSlideSpeedList[i].xspeed = 0;
			pSlideSpeedList[i].yspeed = 0;
			GetDataStr(param,"lff",  &pSlideSpeedList[i].time, &pSlideSpeedList[i].xspeed, &pSlideSpeedList[i].yspeed);
			ini1->ReadStringNext(name1,"speed",param,sizeof(param)-1);
		}
	}
	else
	{
		ini2->ReadString(name2,"speed",param,sizeof(param)-1,"");
		for(i=0; i<nSlideListSize; i++)
		{
			pSlideSpeedList[i].time = 0;
			pSlideSpeedList[i].xspeed = 0;
			pSlideSpeedList[i].yspeed = 0;
			GetDataStr(param,"lff",  &pSlideSpeedList[i].time, &pSlideSpeedList[i].xspeed, &pSlideSpeedList[i].yspeed);
			ini2->ReadStringNext(name2,"speed",param,sizeof(param)-1);
		}
	}
}

void CXI_SLIDEPICTURE::ReleaseAll()
{
	TEXTURE_RELEASE(m_rs,m_idTex);
	PTR_DELETE(pSlideSpeedList);
	PTR_DELETE(strTechniqueName);
	nSlideListSize=0;
}

int CXI_SLIDEPICTURE::CommandExecute(int wActCode)
{
	return -1;
}

bool CXI_SLIDEPICTURE::IsClick(int buttonID,long xPos,long yPos)
{
	return false;
}

void CXI_SLIDEPICTURE::ChangePosition( XYRECT &rNewPos )
{
	m_rect = rNewPos;
	m_v[0].pos.x = (float)m_rect.left;  m_v[0].pos.y = (float)m_rect.top;
	m_v[1].pos.x = (float)m_rect.left;  m_v[1].pos.y = (float)m_rect.bottom;
	m_v[2].pos.x = (float)m_rect.right; m_v[2].pos.y = (float)m_rect.top;
	m_v[3].pos.x = (float)m_rect.right, m_v[3].pos.y = (float)m_rect.bottom;
}

void CXI_SLIDEPICTURE::SaveParametersToIni()
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

void CXI_SLIDEPICTURE::SetNewPicture(char * sNewTexName)
{
	if(m_idTex!=-1L)
		m_rs->TextureRelease(m_idTex);
	m_idTex = m_rs->TextureCreate(sNewTexName);
}

void CXI_SLIDEPICTURE::Update(dword Delta_Time)
{
	if(nCurSlide>=nSlideListSize) return;

	nLifeTime -= Delta_Time;
	if(nLifeTime<0)
	{ // переход на другую скорость
		nCurSlide++;
		if(nCurSlide>=nSlideListSize) nCurSlide=0;
		nLifeTime = pSlideSpeedList[nCurSlide].time;
		curRotate = minRotate + rand()*deltaRotate/RAND_MAX;
	}

	float xadd = pSlideSpeedList[nCurSlide].xspeed * (Delta_Time / 1000.f);
	float yadd = pSlideSpeedList[nCurSlide].yspeed * (Delta_Time / 1000.f);

	curAngle += curRotate*Delta_Time/1000.f;

	m_texRect.left += xadd;
	m_texRect.right += xadd;
	m_texRect.top += yadd;
	m_texRect.bottom += yadd;

	while(m_texRect.left < -10)
	{
		m_texRect.left += 10;
		m_texRect.right += 10;
	}
	while(m_texRect.right > 10)
	{
		m_texRect.left -= 10;
		m_texRect.right -= 10;
	}
	while(m_texRect.top < -10)
	{
		m_texRect.top += 10;
		m_texRect.bottom += 10;
	}
	while(m_texRect.top > 10)
	{
		m_texRect.top -= 10;
		m_texRect.bottom -= 10;
	}

	SetTextureCoordinate(m_v,m_texRect,curAngle);
}
