#include <stdio.h>
#include "xi_editbox.h"

#define WIDTH_SCALE_USED	0.9f
#define HEIGHT_SCALE_USED	0.9f

CXI_EDITBOX::CXI_EDITBOX()
{
	m_nAlphaQuantity = 0;
	m_nCurAlphaNum = -1;
	m_nFirstChar = 0;
	m_nHorz = m_nVert = 1;
	m_nMaxSize = -1;

	m_idBtnTex = -1;
	m_dwEditBoxColor = 0;
	m_dwBorderColor = 0xFFFFFFFF;

	m_nChrFontNum = -1;
	m_nStrFontNum = -1;
	m_fChrScale = 1.f;
	m_fStrScale = 1.f;
	m_bUpChrRegistrOffset = 0;

	m_idVBRect = -1;
	m_idVB = -1;
	m_idIB = -1;
	m_nVQ = 0;
	m_nIQ = 0;

	m_nNodeType = NODETYPE_EDITBOX;
	m_bSelected = true;
	m_bOldSelected = false;
}

CXI_EDITBOX::~CXI_EDITBOX()
{
	ReleaseAll();
}

void CXI_EDITBOX::Draw(bool bSelected,dword Delta_Time)
{
	if(!bSelected && m_bOldSelected)
	{
		m_bUse = false;
		api->Event("editbox_exit");
	}
	m_bOldSelected = bSelected;
	if(!m_bUse || !bSelected) return;
	if(m_nCurAlphaNum<0) SetNewCurSymbol(0,0);

	// show back rectangle
	if(m_idVBRect>=0) m_rs->DrawPrimitive(D3DPT_TRIANGLESTRIP,m_idVBRect,sizeof(XI_NOTEX_VERTEX),0,2,"iRectangle");
	// show background of the edit string
	if(m_idVBRect>=0) m_rs->DrawPrimitive(D3DPT_TRIANGLESTRIP,m_idVBRect,sizeof(XI_NOTEX_VERTEX),4,2,"iRectangle");
	// show buttons rectangles
	if(m_idVB>=0 && m_idIB>=0 && m_idBtnTex>=0)
	{
		m_rs->TextureSet(0,m_idBtnTex);
		m_rs->DrawBuffer(m_idVB,sizeof(XI_ONLYONETEX_VERTEX),m_idIB, 0,m_nVQ, 0,m_nIQ, "iImageCollection");
	}

	// Print buttons symbol
	int i,j,idx;
	float top = m_nTopOffset + m_fChrTopOffset;
	for(idx=j=0; j<m_nVert; j++)
	{
		float left = float(m_rect.left + m_nLeftOffset)+m_fHAdd*.5f;
		for(i=0; i<m_nHorz; i++)
		{
			switch(m_alpha[m_bUpChrRegistrOffset+idx])
			{
			case '*':
				m_rs->ExtPrint(m_nChrFontNum,0xFFFFFFFF,0,ALIGN_CENTER,true,m_fChrScale*.7f,m_screenSize.x,m_screenSize.y,(long)left,(long)top+3,"Del");
				break;
			case '^': break;
			case '~':
				m_rs->ExtPrint(m_nChrFontNum,0xFFFFFFFF,0,ALIGN_CENTER,true,m_fChrScale*.7f,m_screenSize.x,m_screenSize.y,(long)left,(long)top+3,"Ok");
				break;
			case '`':
				m_rs->ExtPrint(m_nChrFontNum,0xFFFFFFFF,0,ALIGN_CENTER,true,m_fChrScale*.7f,m_screenSize.x,m_screenSize.y,(long)left,(long)top+3,"Esc");
				break;
			case '|':
				m_rs->ExtPrint(m_nChrFontNum,0xFFFFFFFF,0,ALIGN_CENTER,true,m_fChrScale*.6f,m_screenSize.x,m_screenSize.y,(long)left,(long)top+4,"Caps");
				break;
			default:
				m_rs->ExtPrint(m_nChrFontNum,0xFFFFFFFF,0,ALIGN_CENTER,true,m_fChrScale,m_screenSize.x,m_screenSize.y,(long)left,(long)top,"%c",m_alpha[m_bUpChrRegistrOffset+idx]);
			}
			left += m_fHAdd;
			idx++;
			if(idx>=m_nAlphaQuantity) break;
		}
		if(idx>=m_nAlphaQuantity) break;
		top += m_fVAdd;
	}

	// show out string
	ATTRIBUTES * pA = api->Entity_GetAttributeClass(&g_idInterface,m_nodeName);
	char * tmpstr = null;
	if(pA) tmpstr = pA->GetAttribute("strdata");
	if(tmpstr) m_rs->ExtPrint(m_nStrFontNum,0xFFFFFFFF,0,ALIGN_CENTER,true,m_fStrScale,m_screenSize.x,m_screenSize.y,(m_rect.left+m_rect.right)/2,m_nTopStringPos,"%s",tmpstr);
}

bool CXI_EDITBOX::Init(INIFILE *ini1,char *name1, INIFILE *ini2,char *name2, VDX8RENDER *rs, XYRECT &hostRect, XYPOINT &ScreenSize)
{
	if( !CINODE::Init(ini1,name1, ini2,name2, rs,hostRect,ScreenSize) ) return false;
	SetGlowCursor(false);
	return true;
}

void CXI_EDITBOX::ReleaseAll()
{
	m_nAlphaQuantity = 0;
	m_nCurAlphaNum = 0;
	m_nFirstChar = 0;
	m_nHorz = m_nVert = 1;
	m_nMaxSize = 0;

	TEXTURE_RELEASE(m_rs,m_idBtnTex);

	VERTEX_BUF_RELEASE(m_rs,m_idVBRect);
	VERTEX_BUF_RELEASE(m_rs,m_idVB);
	INDEX_BUF_RELEASE(m_rs,m_idIB);

	FONT_RELEASE(m_rs,m_nChrFontNum);
	FONT_RELEASE(m_rs,m_nStrFontNum);
}

int CXI_EDITBOX::CommandExecute(int wActCode)
{
	if(m_bUse)
	{
		int newNum = m_nCurAlphaNum;
		switch(wActCode)
		{
		case ACTION_ACTIVATE:
        case ACTION_MOUSECLICK:
			{
				if(m_nCurAlphaNum<0 || m_nCurAlphaNum>=m_nAlphaQuantity) break;
				ATTRIBUTES * pA = api->Entity_GetAttributeClass(&g_idInterface,m_nodeName);
				if(pA==null) break;
				char param[256];	param[0] = 0;
				char * tmpstr = pA->GetAttribute("strdata");
				switch(m_alpha[m_bUpChrRegistrOffset+m_nCurAlphaNum])
				{
				case '*':
					if(tmpstr) sprintf(param,"%s",tmpstr);
					if(strlen(param)>0) param[strlen(param)-1] = 0;
					pA->SetAttribute("strdata",param);
					return -1;
					break;
				case '^':
					if(tmpstr) sprintf(param,"%s ",tmpstr);
					else sprintf(param," ");
					break;
				case '~':
					api->Event("NodeOk","s",m_nodeName);
					return -1;
					break;
				case '`':
					api->Event("NodeCancel","s",m_nodeName);
					return -1;
					break;
				case '|':
					m_bUpChrRegistrOffset = sizeof(m_alpha)/2 - m_bUpChrRegistrOffset;
					return -1;
					break;
				default:
					if(tmpstr) sprintf(param,"%s%c",tmpstr,m_alpha[m_bUpChrRegistrOffset+m_nCurAlphaNum]);
					else sprintf(param,"%c",m_alpha[m_bUpChrRegistrOffset+m_nCurAlphaNum]);
				}
				int nLimit = pA->GetAttributeAsDword("maxlen",0);
				if( nLimit>0 && (int)strlen(param)>nLimit ) return -1;
				nLimit = pA->GetAttributeAsDword("maxwidth",0);
				if(nLimit>0 && m_rs->StringWidth(param,m_nStrFontNum,m_fStrScale)>nLimit) return -1;
				pA->SetAttribute("strdata",param);
				return -1;
			}
			break;

		case ACTION_UPSTEP:
			if(newNum<0) newNum=0;
			else	newNum -= m_nHorz;
			break;
		case ACTION_DOWNSTEP:
			if(newNum<0) newNum=0;
			else	newNum += m_nHorz;
			break;
		case ACTION_LEFTSTEP:
			if(newNum<0) newNum=0;
			else	newNum--;
			if(newNum<0) newNum = m_nAlphaQuantity-1;
			break;
		case ACTION_RIGHTSTEP:
			if(newNum<0) newNum=0;
			else	newNum++;
			if(newNum>=m_nAlphaQuantity) newNum=0;
			break;
		}
		if(newNum<0)
		{
			newNum+=m_nHorz*m_nVert;
			while(newNum>=m_nAlphaQuantity) newNum-=m_nHorz;
		}
		if(newNum>=m_nAlphaQuantity) newNum -= (newNum/m_nHorz)*m_nHorz;

		if(newNum>=0)	SetNewCurSymbol( newNum-(newNum/m_nHorz)*m_nHorz, newNum/m_nHorz );
	}
	return -1;
}

bool CXI_EDITBOX::IsClick(int buttonID,long xPos,long yPos)
{
	if(xPos<m_rect.left+m_nLeftOffset) return false;
	if(xPos>m_rect.right-m_nLeftOffset) return false;
	if(yPos<m_nTopOffset) return false;
	int y = int((yPos-m_nTopOffset)/m_fVAdd);
	if(y>=m_nVert) return false;
	int x = int((xPos-m_rect.left-m_nLeftOffset)/m_fHAdd);
	if(x>=m_nHorz) return false;
	if(x+y*m_nHorz>=m_nAlphaQuantity) return false;
	return true;
}

void CXI_EDITBOX::MouseThis(float fX, float fY)
{
	if(fX<m_rect.left+m_nLeftOffset) return;
	if(fX>m_rect.right-m_nLeftOffset) return;
	if(fY<m_nTopOffset) return;
	int y = int((fY-m_nTopOffset)/m_fVAdd);
	if(y>=m_nVert) return;
	int x = int((fX-m_rect.left-m_nLeftOffset)/m_fHAdd);
	if(x>=m_nHorz) return;
	if(x+y*m_nHorz>=m_nAlphaQuantity) return;
	SetNewCurSymbol(x,y);
}

void CXI_EDITBOX::ChangePosition( XYRECT &rNewPos )
{
	long i,j,idx;
	m_rect = rNewPos;

	// fills this buffers
	float fBottomOff = (float)m_nTopOffset;
	m_nTopStringPos = m_rect.top + m_nTopOffset;
	XI_NOTEX_VERTEX* pv = (XI_NOTEX_VERTEX*)m_rs->LockVertexBuffer(m_idVBRect);
	pv[0].color = pv[1].color = pv[2].color = pv[3].color = m_dwBorderColor;
	pv[4].color = pv[5].color = pv[6].color = pv[7].color = m_dwEditBoxColor;
	pv[0].pos.z = pv[1].pos.z = pv[2].pos.z = pv[3].pos.z =
		pv[4].pos.z = pv[5].pos.z = pv[6].pos.z = pv[7].pos.z = 1.f;
	pv[0].pos.x = pv[1].pos.x = (float)m_rect.left;
	pv[2].pos.x = pv[3].pos.x = (float)m_rect.right;
	pv[0].pos.y = pv[2].pos.y = (float)m_rect.top;
	pv[1].pos.y = pv[3].pos.y = (float)m_rect.bottom;
	pv[4].pos.x = pv[5].pos.x = (float)m_rect.left+m_nLeftOffset;
	pv[6].pos.x = pv[7].pos.x = (float)m_rect.right-m_nLeftOffset;
	pv[4].pos.y = pv[6].pos.y = (float)m_nTopStringPos;
	m_nTopOffset = long(pv[5].pos.y = pv[7].pos.y = (float)m_rect.top+m_nTopOffset + m_rs->CharHeight(m_nStrFontNum)*1.06f) + 4;
	m_rs->UnLockVertexBuffer(m_idVBRect);

	XI_ONLYONETEX_VERTEX* pvt = (XI_ONLYONETEX_VERTEX*)m_rs->LockVertexBuffer(m_idVB);
	float topButtons = (float)m_nTopOffset;
	idx = 0;
	for(j=0; j<m_nVert; j++)
	{
		float left = float(m_rect.left + m_nLeftOffset);
		for(i=0; i<m_nHorz; i++)
		{
			pvt[idx].pos.z = pvt[idx+1].pos.z = pvt[idx+2].pos.z = pvt[idx+3].pos.z = 1.f;
			pvt[idx].pos.x = pvt[idx+1].pos.x = left;
			pvt[idx+2].pos.x = pvt[idx+3].pos.x = left+m_fHAdd*WIDTH_SCALE_USED;
			pvt[idx].pos.y = pvt[idx+2].pos.y = topButtons;
			pvt[idx+1].pos.y = pvt[idx+3].pos.y = topButtons+m_fVAdd*HEIGHT_SCALE_USED;
			pvt[idx].tu = pvt[idx+1].tu = m_frNormTex.left;
			pvt[idx+2].tu = pvt[idx+3].tu = m_frNormTex.right;
			pvt[idx].tv = pvt[idx+2].tv = m_frNormTex.top;
			pvt[idx+1].tv = pvt[idx+3].tv = m_frNormTex.bottom;
			idx += 4;
			if(idx>=m_nAlphaQuantity*4) break;
			left += m_fHAdd;
		}
		if(idx>=m_nAlphaQuantity*4) break;
		topButtons += m_fVAdd;
	}
	m_rs->UnLockVertexBuffer(m_idVB);
}

void CXI_EDITBOX::SaveParametersToIni()
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

void CXI_EDITBOX::LoadIni(INIFILE *ini1,char *name1, INIFILE *ini2,char *name2)
{
	int i,j,idx;
	char param[512];

	m_frNormTex = GetIniFloatRect(ini1,name1, ini2,name2, "normTexRect", FXYRECT(0.f,0.f,1.f,1.f));
	m_frSelTex = GetIniFloatRect(ini1,name1, ini2,name2, "selTexRect", FXYRECT(0.f,0.f,1.f,1.f));

	// get font number
	if( ReadIniString(ini1,name1, ini2,name2, "chrFont", param, sizeof(param),"") )
		if( (m_nChrFontNum=m_rs->LoadFont(param))==-1 )
			api->Trace("can`t load font:'%s'",param);
	if( ReadIniString(ini1,name1, ini2,name2, "strFont", param, sizeof(param),"") )
		if( (m_nStrFontNum=m_rs->LoadFont(param))==-1 )
			api->Trace("can`t load font:'%s'",param);

	// Get font scale
	m_fChrScale = GetIniFloat(ini1,name1, ini2,name2, "chrScale",1.f);
	m_fStrScale = GetIniFloat(ini1,name1, ini2,name2, "strScale",1.f);

	m_nLeftOffset = GetIniLong(ini1,name1, ini2,name2, "offsLeft");
	m_nTopOffset = GetIniLong(ini1,name1, ini2,name2, "offsTop");

	// get texture
	if( ReadIniString(ini1,name1, ini2,name2, "chrTexture", param, sizeof(param),"") )
		if( (m_idBtnTex=m_rs->TextureCreate(param))==-1 )
			api->Trace("can`t load texture:'%s'",param);

	// Get rectangle color
	m_dwEditBoxColor = GetIniARGB(ini1,name1, ini2,name2, "argbBoxColor", 0);

	// Get rectangle bound color
	m_dwBorderColor = GetIniARGB(ini1,name1, ini2,name2, "argbBoundColor", 0);

	// Get alphabet
	m_nAlphaQuantity = 0;
	m_nCurAlphaNum = 0;
	m_nFirstChar = 0;
	m_bUpChrRegistrOffset = 0;

	if( ReadIniString(ini1,name1, ini2,name2, "alphabet", param, sizeof(param)-1,"") )
	{
		char * pTmpStr = pStringService->GetString(pStringService->GetStringNum(param));
		if(pTmpStr==null) pTmpStr = param;
		int nLen = strlen(pTmpStr);
		if(nLen>sizeof(m_alpha)/2-1) nLen = sizeof(m_alpha)/2-1;
		strncpy(m_alpha,pTmpStr,nLen);
		m_alpha[nLen] = 0;
		m_nAlphaQuantity = strlen(m_alpha);
	}
	if( ReadIniString(ini1,name1, ini2,name2, "alphabetUP", param,sizeof(param)-1,"") )
	{
		char * pTmpStr = pStringService->GetString(pStringService->GetStringNum(param));
		if(pTmpStr==null) pTmpStr = param;
		int nLen = strlen(pTmpStr);
		if(nLen>sizeof(m_alpha)/2-1) nLen = sizeof(m_alpha)/2-1;
		strncpy(&m_alpha[sizeof(m_alpha)/2],pTmpStr,nLen);
		m_alpha[nLen+sizeof(m_alpha)/2] = 0;

		if( (size_t)m_nAlphaQuantity != strlen(&m_alpha[sizeof(m_alpha)/2]) ) {
			api->Trace("WARNING!!! parameters alphabet & alphabetUP is different");
		}
	}

	// Get symbol buttons grate
	m_nHorz = GetIniLong(ini1,name1, ini2,name2, "hGrateSize", 1);
	if(m_nHorz<=0) m_nHorz=1;
	m_nVert = (m_nAlphaQuantity+m_nHorz-1)/m_nHorz;

	// Get max string size for edit string
	m_nMaxSize = GetIniLong(ini1,name1, ini2,name2, "stringLength", -1);

	// Create buffers
	m_idVBRect = m_rs->CreateVertexBuffer(XI_NOTEX_FVF,8*sizeof(XI_NOTEX_VERTEX),D3DUSAGE_WRITEONLY);
	m_idVB = m_rs->CreateVertexBuffer(XI_ONLYONETEX_FVF,sizeof(XI_ONLYONETEX_VERTEX)*m_nAlphaQuantity*4,D3DUSAGE_WRITEONLY);
	m_idIB = m_rs->CreateIndexBuffer(m_nAlphaQuantity*6*sizeof(WORD));
	if( m_idVBRect==-1 || m_idVB==-1 || m_idIB==-1 )
		_THROW("Can't create buffers");

	// fills this buffers
	float fBottomOff = (float)m_nTopOffset;
	m_nTopStringPos = m_rect.top + m_nTopOffset;
	XI_NOTEX_VERTEX* pv = (XI_NOTEX_VERTEX*)m_rs->LockVertexBuffer(m_idVBRect);
	pv[0].color = pv[1].color = pv[2].color = pv[3].color = m_dwBorderColor;
	pv[4].color = pv[5].color = pv[6].color = pv[7].color = m_dwEditBoxColor;
	pv[0].pos.z = pv[1].pos.z = pv[2].pos.z = pv[3].pos.z =
	pv[4].pos.z = pv[5].pos.z = pv[6].pos.z = pv[7].pos.z = 1.f;
	pv[0].pos.x = pv[1].pos.x = (float)m_rect.left;
	pv[2].pos.x = pv[3].pos.x = (float)m_rect.right;
	pv[0].pos.y = pv[2].pos.y = (float)m_rect.top;
	pv[1].pos.y = pv[3].pos.y = (float)m_rect.bottom;
	pv[4].pos.x = pv[5].pos.x = (float)m_rect.left+m_nLeftOffset;
	pv[6].pos.x = pv[7].pos.x = (float)m_rect.right-m_nLeftOffset;
	pv[4].pos.y = pv[6].pos.y = (float)m_nTopStringPos;
	m_nTopOffset = long(pv[5].pos.y = pv[7].pos.y = (float)m_rect.top+m_nTopOffset + m_rs->CharHeight(m_nStrFontNum)*1.06f) + 4;
	m_rs->UnLockVertexBuffer(m_idVBRect);

	// buttons buffers
	m_fVAdd = ((float)m_rect.bottom-m_nTopOffset-fBottomOff)/m_nVert;
	if(m_rs->CharHeight(m_nChrFontNum)*m_fChrScale > m_fVAdd*HEIGHT_SCALE_USED)
		m_fChrScale = m_fVAdd*HEIGHT_SCALE_USED/m_rs->CharHeight(m_nChrFontNum);
	m_fChrTopOffset = .5f*(m_fVAdd*HEIGHT_SCALE_USED-m_rs->CharHeight(m_nChrFontNum)*m_fChrScale);
	m_fHAdd = (float)(m_rect.right-m_rect.left - m_nLeftOffset*2)/m_nHorz;

	XI_ONLYONETEX_VERTEX* pvt = (XI_ONLYONETEX_VERTEX*)m_rs->LockVertexBuffer(m_idVB);
	float topButtons = (float)m_nTopOffset;
	idx=0;
	for(j=0; j<m_nVert; j++)
	{
		float left = float(m_rect.left + m_nLeftOffset);
		for(i=0; i<m_nHorz; i++)
		{
			pvt[idx].pos.z = pvt[idx+1].pos.z = pvt[idx+2].pos.z = pvt[idx+3].pos.z = 1.f;
			pvt[idx].pos.x = pvt[idx+1].pos.x = left;
			pvt[idx+2].pos.x = pvt[idx+3].pos.x = left+m_fHAdd*WIDTH_SCALE_USED;
			pvt[idx].pos.y = pvt[idx+2].pos.y = topButtons;
			pvt[idx+1].pos.y = pvt[idx+3].pos.y = topButtons+m_fVAdd*HEIGHT_SCALE_USED;
			pvt[idx].tu = pvt[idx+1].tu = m_frNormTex.left;
			pvt[idx+2].tu = pvt[idx+3].tu = m_frNormTex.right;
			pvt[idx].tv = pvt[idx+2].tv = m_frNormTex.top;
			pvt[idx+1].tv = pvt[idx+3].tv = m_frNormTex.bottom;
			idx += 4;
			if(idx>=m_nAlphaQuantity*4) break;
			left += m_fHAdd;
		}
		if(idx>=m_nAlphaQuantity*4) break;
		topButtons += m_fVAdd;
	}
	m_rs->UnLockVertexBuffer(m_idVB);

	WORD * pt = (WORD*)m_rs->LockIndexBuffer(m_idIB);
	for(j=i=0; i<idx; i+=4,j+=6)
	{
		pt[j] = i;	pt[j+1] = i+1;	pt[j+2] = i+2;
		pt[j+3] = i+2;	pt[j+4] = i+1;	pt[j+5] = i+3;
	}
	m_rs->UnLockIndexBuffer(m_idIB);

	m_nVQ = idx;
	m_nIQ = j/3;

	m_nCurAlphaNum = -1;
	SetNewCurSymbol(0,0);
}

void CXI_EDITBOX::SetNewCurSymbol(int h, int v)
{
	int newNum = h+v*m_nHorz;
	if(newNum>=m_nAlphaQuantity) newNum = h;
	if(newNum>=m_nAlphaQuantity) newNum = -1;
	if(m_nCurAlphaNum==newNum) return;
	XI_ONLYONETEX_VERTEX* pvt = (XI_ONLYONETEX_VERTEX*)m_rs->LockVertexBuffer(m_idVB);
	int idx;
	if(m_nCurAlphaNum>=0)
	{
		idx = m_nCurAlphaNum*4;
		pvt[idx].tu = pvt[idx+1].tu = m_frNormTex.left;
		pvt[idx+2].tu = pvt[idx+3].tu = m_frNormTex.right;
		pvt[idx].tv = pvt[idx+2].tv = m_frNormTex.top;
		pvt[idx+1].tv = pvt[idx+3].tv = m_frNormTex.bottom;
	}
	m_nCurAlphaNum = newNum;
	if(m_nCurAlphaNum>=0)
	{
		idx = m_nCurAlphaNum*4;
		pvt[idx].tu = pvt[idx+1].tu = m_frSelTex.left;
		pvt[idx+2].tu = pvt[idx+3].tu = m_frSelTex.right;
		pvt[idx].tv = pvt[idx+2].tv = m_frSelTex.top;
		pvt[idx+1].tv = pvt[idx+3].tv = m_frSelTex.bottom;
	}
	m_rs->UnLockVertexBuffer(m_idVB);
}
