#include "xi_slideLine.h"

CXI_SLIDELINE::CXI_SLIDELINE()
{
    m_idTexLine = -1;
	m_idTexSelLine = -1;
	m_idTexPointer = -1;
	m_idVBuf = -1;
	m_nNodeType = NODETYPE_SLIDELINE;
	m_bMouseSelect = true;
	m_bDoChangeSlider = false;
}

CXI_SLIDELINE::~CXI_SLIDELINE()
{
	ReleaseAll();
}

void CXI_SLIDELINE::Draw(bool bSelected,dword Delta_Time)
{
	if(m_bUse)
	{
		if(m_idVBuf>=0)
		{
			DoMouseControl();

			dword dwOldTF;
			m_rs->GetRenderState(D3DRS_TEXTUREFACTOR, &dwOldTF);

			if( m_bSelected ) m_rs->SetRenderState(D3DRS_TEXTUREFACTOR, 0xFF808080);
			else m_rs->SetRenderState(D3DRS_TEXTUREFACTOR, m_dwDisableColor);

			if(m_idTexLine>=0 && m_idTexSelLine>=0)
			{
				if(bSelected)	m_rs->TextureSet(0,m_idTexSelLine);
				else	m_rs->TextureSet(0,m_idTexLine);
				m_rs->DrawPrimitive(D3DPT_TRIANGLESTRIP,m_idVBuf,sizeof(XI_ONLYONETEX_VERTEX),0,2,"iBlindPictures");
			}

			if(m_idTexPointer>=0)
			{
				m_rs->TextureSet(0,m_idTexPointer);
				m_rs->DrawPrimitive(D3DPT_TRIANGLESTRIP,m_idVBuf,sizeof(XI_ONLYONETEX_VERTEX),4,2,"iBlindPictures");
			}

			m_rs->SetRenderState(D3DRS_TEXTUREFACTOR, dwOldTF);
		}
	}
}

bool CXI_SLIDELINE::Init(INIFILE *ini1,char *name1, INIFILE *ini2,char *name2, VDX8RENDER *rs, XYRECT &hostRect, XYPOINT &ScreenSize)
{
	if( !CINODE::Init(ini1,name1, ini2,name2, rs,hostRect,ScreenSize) ) return false;
	return true;
}

void CXI_SLIDELINE::ReleaseAll()
{
	TEXTURE_RELEASE(m_rs,m_idTexLine);
	TEXTURE_RELEASE(m_rs,m_idTexSelLine);
	TEXTURE_RELEASE(m_rs,m_idTexPointer);
	VERTEX_BUF_RELEASE(m_rs,m_idVBuf);
}

int CXI_SLIDELINE::CommandExecute(int wActCode)
{
	if(m_bUse)
	{
		switch(wActCode)
		{
		case ACTION_LEFTSTEP:	SetNewValue(m_nCurValue-1); break;
		case ACTION_SPEEDLEFT:
			if(m_nCurValue>=m_nSpeedSlide) SetNewValue(m_nCurValue-m_nSpeedSlide);
			else SetNewValue(0);
			break;
		case ACTION_RIGHTSTEP:	SetNewValue(m_nCurValue+1); break;
		case ACTION_SPEEDRIGHT:
			if(m_nCurValue<m_nGrateQuantity-m_nSpeedSlide)
				SetNewValue(m_nCurValue+m_nSpeedSlide);
			else SetNewValue(m_nGrateQuantity);
			break;
		}
	}
	return -1;
}

bool CXI_SLIDELINE::IsClick(int buttonID,long xPos,long yPos)
{
	//if( buttonID == MOUSE_RBUTTON )
	{
		if( xPos >= m_rect.left && xPos <= m_rect.right &&
			yPos >= m_rect.top && yPos <= m_rect.bottom )
			return true;
	}
	return false;
}

void CXI_SLIDELINE::MouseThis(float fX, float fY)
{
}

void CXI_SLIDELINE::ChangePosition( XYRECT &rNewPos )
{
	m_rect = rNewPos;

	XI_ONLYONETEX_VERTEX *pv = (XI_ONLYONETEX_VERTEX*) m_rs->LockVertexBuffer(m_idVBuf);
	if(pv)
	{
		pv[0].pos.x = pv[1].pos.x = (float)m_rect.left;
		pv[2].pos.x = pv[3].pos.x = (float)m_rect.right;
		pv[0].pos.y = pv[2].pos.y = (float)m_rect.top;
		pv[1].pos.y = pv[3].pos.y = (float)m_rect.bottom;

		float left = (float)(m_rect.left + m_nBaseLeft - m_nPointerLeft);
		float right = (float)(m_rect.right - m_nBaseLeft + m_nPointerLeft - m_nPointerWidth);
		left = left + (right-left)/m_nGrateQuantity*m_nCurValue;

		pv[4].pos.x = pv[5].pos.x = left;
		pv[6].pos.x = pv[7].pos.x = left+m_nPointerWidth;
		pv[4].pos.y = pv[6].pos.y = (float)(m_rect.top+m_rect.bottom-m_nPointerHeight)/2.f;
		pv[5].pos.y = pv[7].pos.y = (float)(m_rect.top+m_rect.bottom+m_nPointerHeight)/2.f;

		m_rs->UnLockVertexBuffer(m_idVBuf);
	}
}

void CXI_SLIDELINE::SaveParametersToIni()
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

void CXI_SLIDELINE::DoMouseControl()
{
	if( !m_bClickable || !m_bSelected || m_bLockedNode ) return;
	CONTROL_STATE cs;
	api->Controls->GetControlState("ILClick",cs);
	FXYPOINT fmp = ptrOwner->GetMousePoint();
	if(cs.state == CST_ACTIVATED)
	{
		if(fmp.x<m_rect.left) return;
		if(fmp.x>m_rect.right) return;

		float ftop = (m_rect.bottom+m_rect.top-m_nPointerHeight)/2.f;
		if(fmp.y<ftop) return;
		if(fmp.y>ftop+m_nPointerHeight) return;

		m_bDoChangeSlider = true;
	}
	if(cs.state == CST_INACTIVATED)
	{
		m_bDoChangeSlider = false;
	}

	if(m_bDoChangeSlider)
	{
		if(fmp.x<m_rect.left+m_nBaseLeft)
			SetNewValue(0);
		else	if(fmp.x>m_rect.right-m_nBaseLeft)
			SetNewValue(m_nGrateQuantity);
		else
			SetNewValue(
			long((fmp.x-m_rect.left-m_nBaseLeft)/(m_rect.right-m_rect.left - m_nBaseLeft - m_nBaseLeft)*m_nGrateQuantity)
			);
	}
}

dword _cdecl CXI_SLIDELINE::MessageProc(long msgcode, MESSAGE & message)
{
	switch(msgcode)
	{
	case 0: // Установить слайд на новое значение
		{
			SetNewValue( (long)(message.Float()*m_nGrateQuantity) );
		}
		break;
	case 1: //
		{
			m_nGrateQuantity = message.Long();
			if( m_nGrateQuantity < 2 ) m_nGrateQuantity = 2;
			ATTRIBUTES* pA = api->Entity_GetAttributeClass(&g_idInterface,"nodes");
			if( pA ) pA = pA->GetAttributeClass(m_nodeName);
			if( pA ) {
				m_nMinValue = pA->GetAttributeAsDword("minLimit",-1);
				m_nMaxValue = pA->GetAttributeAsDword("maxLimit",-1);
			}
		}
		break;
	case 2: // Установить слайд на конкретное значение
		{
			SetNewValue( message.Long() );
		}
		break;
	}

	return 0;
}

void CXI_SLIDELINE::LoadIni(INIFILE *ini1,char *name1, INIFILE *ini2,char *name2)
{
	int i;
	char param[255];

	m_idTexLine = -1;
	if( ReadIniString(ini1,name1, ini2,name2, "baseTexture", param, sizeof(param),"") )
		m_idTexLine = m_rs->TextureCreate(param);

	m_idTexSelLine = -1;
	if( ReadIniString(ini1,name1, ini2,name2, "selectTexture", param, sizeof(param),"") )
		m_idTexSelLine = m_rs->TextureCreate(param);

	m_idTexPointer = -1;
	if( ReadIniString(ini1,name1, ini2,name2, "pointerTexture", param, sizeof(param),"") )
		m_idTexPointer = m_rs->TextureCreate(param);

	m_idVBuf = m_rs->CreateVertexBuffer(XI_ONLYONETEX_FVF,8*sizeof(XI_ONLYONETEX_VERTEX),D3DUSAGE_WRITEONLY);
	if(m_idVBuf==-1) THROW("can not create the vertex buffers");

	m_nPointerWidth = GetIniLong(ini1,name1, ini2,name2, "pointerWidth", 8);
	m_nPointerHeight = GetIniLong(ini1,name1, ini2,name2, "pointerHeight", m_rect.bottom-m_rect.top);

	m_nBaseLeft = GetIniLong(ini1,name1, ini2,name2, "baseLeft", 0);
	m_nPointerLeft = GetIniLong(ini1,name1, ini2,name2, "pointerLeft", 0);

	m_nGrateQuantity = GetIniLong(ini1,name1, ini2,name2, "pitchCounter", m_rect.right-m_rect.left);
	m_nSpeedSlide = GetIniLong(ini1,name1, ini2,name2, "speedSlide", 1);

	m_nCurValue = 0;
	ATTRIBUTES * pA = api->Entity_GetAttributeClass(&g_idInterface,"nodes");
	if(pA!=null) pA = pA->GetAttributeClass(m_nodeName);
	if(pA!=null) m_nCurValue = long(pA->GetAttributeAsFloat("value",0.f)*m_nGrateQuantity);
	m_nMinValue = m_nMaxValue = -1;
	if( pA ) {
		m_nMinValue = pA->GetAttributeAsDword("minLimit",m_nMinValue);
		m_nMaxValue = pA->GetAttributeAsDword("maxLimit",m_nMaxValue);
	}

	m_dwDisableColor = GetIniARGB(ini1,name1, ini2,name2, "disablecolor", 0xA04C4C4C);

	XI_ONLYONETEX_VERTEX *pv = (XI_ONLYONETEX_VERTEX*) m_rs->LockVertexBuffer(m_idVBuf);
	if(pv)
	{
		for(i=0; i<8; i++) pv[i].pos.z = 1.f;

		pv[0].tu = pv[1].tu = pv[4].tu = pv[5].tu = 0.f;
		pv[2].tu = pv[3].tu = pv[6].tu = pv[7].tu = 1.f;
		pv[0].tv = pv[2].tv = pv[4].tv = pv[6].tv = 0.f;
		pv[1].tv = pv[3].tv = pv[5].tv = pv[7].tv = 1.f;

		pv[0].pos.x = pv[1].pos.x = (float)m_rect.left;
		pv[2].pos.x = pv[3].pos.x = (float)m_rect.right;
		pv[0].pos.y = pv[2].pos.y = (float)m_rect.top;
		pv[1].pos.y = pv[3].pos.y = (float)m_rect.bottom;

		float left = (float)(m_rect.left + m_nBaseLeft - m_nPointerLeft);
		float right = (float)(m_rect.right - m_nBaseLeft + m_nPointerLeft - m_nPointerWidth);
		left = left + (right-left)/m_nGrateQuantity*m_nCurValue;

		pv[4].pos.x = pv[5].pos.x = left;
		pv[6].pos.x = pv[7].pos.x = left+m_nPointerWidth;
		pv[4].pos.y = pv[6].pos.y = (float)(m_rect.top+m_rect.bottom-m_nPointerHeight)/2.f;
		pv[5].pos.y = pv[7].pos.y = (float)(m_rect.top+m_rect.bottom+m_nPointerHeight)/2.f;

		m_rs->UnLockVertexBuffer(m_idVBuf);
	}

	m_bSelected = true;
}

void CXI_SLIDELINE::SetNewValue(long newValue)
{
	if( m_nMinValue>=0 && newValue<m_nMinValue ) newValue = m_nMinValue;
	if( m_nMaxValue>=0 && newValue>m_nMaxValue ) newValue = m_nMaxValue;
	if(newValue<0 || m_nCurValue==newValue || newValue>m_nGrateQuantity) return;
	m_nCurValue=newValue;

	float left = (float)(m_rect.left + m_nBaseLeft - m_nPointerLeft);
	float right = (float)(m_rect.right - m_nBaseLeft + m_nPointerLeft - m_nPointerWidth);
	left = left + (right-left)/m_nGrateQuantity*m_nCurValue;

	XI_ONLYONETEX_VERTEX *pv = (XI_ONLYONETEX_VERTEX*) m_rs->LockVertexBuffer(m_idVBuf);
	if(pv)
	{
		pv[4].pos.x = pv[5].pos.x = left;
		pv[6].pos.x = pv[7].pos.x = left+m_nPointerWidth;
		m_rs->UnLockVertexBuffer(m_idVBuf);
	}

	ATTRIBUTES * pA = api->Entity_GetAttributeClass(&g_idInterface,"nodes");
	if(pA!=null) pA = pA->GetAttributeClass(m_nodeName);
	if(pA!=null) pA->SetAttributeUseFloat("value",(float)m_nCurValue/m_nGrateQuantity);
	api->PostEvent("eSlideChange", 0, "slf", m_nodeName, m_nCurValue, (float)m_nCurValue/m_nGrateQuantity);
}
