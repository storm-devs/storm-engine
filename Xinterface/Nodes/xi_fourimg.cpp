#include <stdio.h>
#include <assert.h>
#include "xi_fourimg.h"
#include "..\\defines.h"

CXI_FOURIMAGE::CXI_FOURIMAGE()
{
	m_rs = NULL;
	m_bClickable = true;
	m_oneStrFont = -1;
	m_twoStrFont = -1;
	m_nNodeType = NODETYPE_FOURIMAGE;
	m_oneBadTexture = m_twoBadTexture = -1;
	vBuf = -1;
	m_nTexturesQuantity = 0;
	m_nTextureId = NULL;
	m_sGroupName = NULL;
}

CXI_FOURIMAGE::~CXI_FOURIMAGE()
{
	ReleaseAll();
}

int CXI_FOURIMAGE::CommandExecute(int wActCode)
{
	int retVal = -1;
	if(m_bUse)
	{
		int newSelectItem = m_nSelectItem;

		switch(wActCode)
		{
		case ACTION_RIGHTSTEP:
			newSelectItem++;
			break;
		case ACTION_LEFTSTEP:
			newSelectItem--;
			break;
		case ACTION_UPSTEP:
			{
				VDATA * pvdat = api->Event("FI_UpCom","l",m_nSelectItem);
				if(pvdat==null || pvdat->GetLong()==0)	newSelectItem-=2;
			}
			break;
		case ACTION_DOWNSTEP:
			newSelectItem+=2;
			break;
		case ACTION_ACTIVATE:
			break;
		case ACTION_SPEEDRIGHT:
			newSelectItem++;
			break;
		case ACTION_SPEEDLEFT:
			newSelectItem--;
			break;
		case ACTION_DEACTIVATE:
			break;
		case ACTION_MOUSECLICK:
			for(int i=0; i<4; i++)
				if( m_MousePoint.x >= m_imgRect[i].left && m_MousePoint.x <= m_imgRect[i].right &&
					m_MousePoint.y >= m_imgRect[i].top  && m_MousePoint.y <= m_imgRect[i].bottom )
					break;
			if(i<4)
			{
				newSelectItem = i;
				if(m_nSelectItem==newSelectItem && IsCurrentNode())
				{
					/*api->Event("ievnt_command","ss","activate",m_nodeName);
					for(int n=0; n<COMMAND_QUANTITY; n++)
						if(pCommandsList[n].code==wActCode) break;
					if(n<COMMAND_QUANTITY) m_nCurrentCommandNumber = n;*/
					retVal = ACTION_ACTIVATE;
				}
			}
			break;
		}

		while(newSelectItem<0) newSelectItem+=4;
		while(newSelectItem>3) newSelectItem-=4;

		if(m_bUsed[newSelectItem])
			m_nSelectItem = newSelectItem;

		// set new current item
		ATTRIBUTES * tmpAttr = _CORE_API->Entity_GetAttributeClass(&g_idInterface,"FourImage");
		tmpAttr->SetAttributeUseDword("current",m_nSelectItem);
	}
	return retVal;
}

void CXI_FOURIMAGE::Draw(bool bSelected,dword Delta_Time)
{
	GUARD(void CXI_FOURIMAGE::Draw())

	if(m_bUse)
	{
		Update(bSelected,Delta_Time);

		// draw one picture
		for(int i=0; i<4; i++)
		{
			if(m_oneTexID[i]==-1 || m_oneTexID[i]>=m_nTexturesQuantity) continue;
			if(m_oneImgID[i]==-1) m_rs->TextureSet(0,m_oneBadTexture);
			else m_rs->TextureSet(0,m_nTextureId[m_oneTexID[i]]);
			m_rs->DrawPrimitive(D3DPT_TRIANGLESTRIP,vBuf,sizeof(XI_ONETEX_VERTEX),i*4,2,"iFourImages_main");
		}

		// create select border
        if(m_bShowBorder && bSelected)
		{
			XI_ONLYONETEX_VERTEX pV[4];
			FXYRECT textureRect;
			for(int i=0; i<4; i++) pV[i].pos.z = 1.f;
			pPictureService->GetTexturePos(m_nBorderPicture,textureRect);
			pV[0].tu = textureRect.left;	pV[0].tv = textureRect.top;
			pV[1].tu = textureRect.left;	pV[1].tv = textureRect.bottom;
			pV[2].tu = textureRect.right;	pV[2].tv = textureRect.top;
			pV[3].tu = textureRect.right;	pV[3].tv = textureRect.bottom;
			pV[0].pos.x = (float)m_imgRect[m_nSelectItem].left;		pV[0].pos.y = (float)m_imgRect[m_nSelectItem].top;
			pV[1].pos.x = (float)m_imgRect[m_nSelectItem].left;		pV[1].pos.y = (float)m_imgRect[m_nSelectItem].bottom;
			pV[2].pos.x = (float)m_imgRect[m_nSelectItem].right;	pV[2].pos.y = (float)m_imgRect[m_nSelectItem].top;
			pV[3].pos.x = (float)m_imgRect[m_nSelectItem].right;	pV[3].pos.y = (float)m_imgRect[m_nSelectItem].bottom;
			// show select border
			m_rs->TextureSet(0,m_texBorder);
			m_rs->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,XI_ONLYONETEX_FVF,2,pV,sizeof(XI_ONLYONETEX_VERTEX),"iFourImages_border");
		}

		// draw two picture
		for(i=0; i<4; i++)
		{
			if(m_twoTexID[i]==-1 || m_twoTexID[i]>=m_nTexturesQuantity) continue;
			if(m_twoImgID[i]==-1) m_rs->TextureSet(0,m_twoBadTexture);
			else m_rs->TextureSet(0,m_nTextureId[m_twoTexID[i]]);
			m_rs->DrawPrimitive(D3DPT_TRIANGLESTRIP,vBuf,sizeof(XI_ONETEX_VERTEX),16+i*4,2,"iFourImages_main");
		}

		// out to screen the strings if that needed
		if( bUseOneString || bUseTwoString )
		{
			int nAlignment1=ALIGN_CENTER,nAlignment2=ALIGN_CENTER;
			if(m_xOneOffset>0) nAlignment1=ALIGN_RIGHT;
			else if(m_xOneOffset<0) nAlignment1=ALIGN_LEFT;
			if(m_xTwoOffset>0) nAlignment2=ALIGN_RIGHT;
			else if(m_xTwoOffset<0) nAlignment2=ALIGN_LEFT;

			for(int i=0; i<4; i++)
			{
				long posX,posY;
				posX = (m_imgRect[i].left + m_imgRect[i].right)/2;
				DWORD color = 0xFFFFFFFF;
				/*if(i==m_nSelectItem) color=m_dwCurSelectColor;
				else color=m_dwBaseColor;*/

				if(bUseOneString)
				{
					posY = m_imgRect[i].top + m_nOneStrOffset;

					if(m_pOneStr[i]!=NULL)
						m_rs->ExtPrint(m_oneStrFont,color,0, nAlignment1, false, 1.f, m_screenSize.x,
							m_screenSize.y,	posX+m_xOneOffset,posY,"%s",m_pOneStr[i]);
					else if(m_oneStr[i]!=-1)
						m_rs->ExtPrint(m_oneStrFont,color,0, nAlignment1, false, 1.f, m_screenSize.x,
							m_screenSize.y,	posX+m_xOneOffset,posY,"%s",pStringService->GetString(m_oneStr[i]) );
				}
				if(bUseTwoString)
				{
					posY = m_imgRect[i].top + m_nTwoStrOffset;
					if(m_pTwoStr[i]!=NULL)
						m_rs->ExtPrint(m_twoStrFont,color,0, nAlignment2, false, 1.f, m_screenSize.x,
							m_screenSize.y,	posX+m_xTwoOffset,posY,"%s",m_pTwoStr[i]);
					else if(m_twoStr[i]!=-1)
						m_rs->ExtPrint(m_twoStrFont,color,0, nAlignment2, false, 1.f, m_screenSize.x,
							m_screenSize.y,	posX+m_xTwoOffset,posY,"%s",pStringService->GetString(m_twoStr[i]) );
				}
			}
		}
	}

	UNGUARD
}

bool CXI_FOURIMAGE::Init(INIFILE *ini1,char *name1, INIFILE *ini2,char *name2, VDX8RENDER *rs, XYRECT &hostRect, XYPOINT &ScreenSize)
{
	if( !CINODE::Init(ini1,name1, ini2,name2, rs,hostRect,ScreenSize) ) return false;
	// screen position for that is host screen position
	memcpy(&m_rect,&m_hostRect,sizeof(m_hostRect));
	FillVertex();
	return true;
}

void CXI_FOURIMAGE::LoadIni(INIFILE *ini1,char *name1, INIFILE *ini2,char *name2)
{
	int i;
	char param[256];
	char param1[256];
	char * tmpstr;

	// get base color
	m_dwBaseColor = GetIniARGB(ini1,name1, ini2,name2, "commonColor", ARGB(255,128,128,128));

	// get color for light select image
	m_dwLightSelectColor = GetIniARGB(ini1,name1, ini2,name2, "lightSelectCol", ARGB(255,255,255,255));

	// get color for dark select image
	m_dwDarkSelectColor = GetIniARGB(ini1,name1, ini2,name2, "darkSelectCol", ARGB(255,150,150,150));

	// get blind parameters
	m_bColorType = true; // select item used select color
	m_nBlindCounter = 0; // currend blind counter
	m_nMaxBlindCounter = GetIniLong(ini1,name1, ini2,name2, "wBlindDelay");
	if(m_nMaxBlindCounter>0) m_bDoBlind = true;
	else m_bDoBlind = false;
	m_dwCurSelectColor = m_dwLightSelectColor;

	// get one string parameters
	bUseOneString = ReadIniString(ini1,name1, ini2,name2, "oneString", param, sizeof(param),"");
	if(bUseOneString)
	{
		m_oneStrFont = -1;
		if(	GetMidStr(param, param1, sizeof(param1), "font:", ",") )
			if( (m_oneStrFont=m_rs->LoadFont(param1)) == -1 )
				api->Trace("can not load font:'%s'",param1);

		m_xOneOffset = m_nOneStrOffset = 0;
		if(	GetMidStr(param, param1, sizeof(param1), "off:(", ")") )
			GetDataStr(param1,"ll", &m_xOneOffset, &m_nOneStrOffset);

		m_foreColOneStr = ARGB(255,255,255,255);
		if(	GetMidStr(param, param1, sizeof(param1), "fc:{", "}") )
		{
			m_foreColOneStr = GetColorFromStr(param1, m_foreColOneStr);
		}

		m_backColOneStr = ARGB(0,0,0,0);
		if(	GetMidStr(param, param1, sizeof(param1), "bc:{", "}") )
		{
			m_backColOneStr = GetColorFromStr(param1, m_backColOneStr);
		}
	}
	else
	{
		m_nOneStrOffset = 0;
		m_foreColOneStr = ARGB(255,255,255,255);
		m_backColOneStr = ARGB(0,0,0,0);
		m_xOneOffset = 0;
	}

	// get two string parameters
	bUseTwoString = ReadIniString(ini1,name1, ini2,name2, "twoString", param, sizeof(param),"");
	if(bUseTwoString)
	{
		m_twoStrFont = -1;
		if(	GetMidStr(param, param1, sizeof(param1), "font:", ",") )
			if( (m_twoStrFont=m_rs->LoadFont(param1)) == -1 )
				api->Trace("can not load font:'%s'",param1);

		m_xTwoOffset = m_nTwoStrOffset = 0;
		if(	GetMidStr(param, param1, sizeof(param1), "off:(", ")") )
			GetDataStr(param1,"ll", &m_xTwoOffset, &m_nTwoStrOffset);

		m_foreColTwoStr = ARGB(255,255,255,255);
		if(	GetMidStr(param, param1, sizeof(param1), "fc:{", "}") )
		{
			m_foreColTwoStr = GetColorFromStr(param1, m_foreColTwoStr);
		}

		m_backColTwoStr = ARGB(0,0,0,0);
		if(	GetMidStr(param, param1, sizeof(param1), "bc:{", "}") )
		{
			m_backColTwoStr = GetColorFromStr(param1, m_backColTwoStr);
		}
	}
	else
	{
		m_nTwoStrOffset = 0;
		m_foreColTwoStr = ARGB(255,255,255,255);
		m_backColTwoStr = ARGB(0,0,0,0);
		m_xTwoOffset = 0;
	}

	ATTRIBUTES * pAttribute = _CORE_API->Entity_GetAttributeClass(&g_idInterface,"FourImage");
	if(pAttribute!=NULL)
	{
		m_nSelectItem = pAttribute->GetAttributeAsDword("current",0);
		// get bad picture
		char * sBadPict;
		if( (sBadPict=pAttribute->GetAttribute("BadOnePicture")) != NULL)
			m_oneBadTexture = m_rs->TextureCreate(sBadPict);
		else	m_oneBadTexture = -1;
		if( (sBadPict=pAttribute->GetAttribute("BadTwoPicture")) != NULL)
			m_twoBadTexture = m_rs->TextureCreate(sBadPict);
		else	m_twoBadTexture = -1;

		// get textures
		ATTRIBUTES * pA = pAttribute->GetAttributeClass("ImagesGroup");
		if(pA==0) m_nTexturesQuantity=0;
		else
		{
			m_nTexturesQuantity = pA->GetAttributesNum();
			m_nTextureId = NEW long[m_nTexturesQuantity];
			m_sGroupName = NEW char*[m_nTexturesQuantity];
			if(m_sGroupName==NULL || m_nTextureId==NULL)
			{
				_THROW("Allocate memory error");
			}
			for(i=0; i<m_nTexturesQuantity; i++)
			{
				char * stmp = pA->GetAttribute(i);
				if(stmp==NULL) m_sGroupName[i]=NULL;
				else
				{
					if( (m_sGroupName[i]=NEW char[strlen(stmp)+1])==NULL )
					{
						_THROW("Allocate memory error");
					}
					strcpy(m_sGroupName[i],stmp);
				}
			}
		}
		for(i=0; i<m_nTexturesQuantity; i++)
			m_nTextureId[i] = pPictureService->GetTextureID(m_sGroupName[i]);

		for(i=0; i<4; i++)
		{
			sprintf(param,"pic%d",i+1);
			ATTRIBUTES * pAttrTmp = pAttribute->GetAttributeClass(param);
			if(pAttrTmp!=NULL)
			{
				m_bUsed[i] = pAttrTmp->GetAttributeAsDword("selected",0)!=0;
				m_oneTexID[i] = pAttrTmp->GetAttributeAsDword("tex1",-1);
				m_twoTexID[i] = pAttrTmp->GetAttributeAsDword("tex2",-1);
				if(m_oneTexID[i]!=-1 && m_oneTexID[i]<m_nTexturesQuantity) m_oneImgID[i] = pPictureService->GetImageNum(m_sGroupName[m_oneTexID[i]],pAttrTmp->GetAttribute("img1"));
				else m_oneImgID[i] = -1;
				if(m_twoTexID[i]!=-1 && m_twoTexID[i]<m_nTexturesQuantity) m_twoImgID[i] = pPictureService->GetImageNum(m_sGroupName[m_twoTexID[i]],pAttrTmp->GetAttribute("img2"));
				else m_twoImgID[i] = -1;
				char * tmps = pAttrTmp->GetAttribute("str1");
				if(tmps!=NULL && *tmps=='#')
				{
					if( (m_pOneStr[i]=NEW char[strlen(tmps)]) == NULL )
						_THROW("allocate memory error");
					strcpy(m_pOneStr[i],&tmps[1]);
					m_oneStr[i] = -1L;
				}
				else
				{
					m_pOneStr[i] = NULL;
					m_oneStr[i] = pStringService->GetStringNum(tmps);
				}
				tmps = pAttrTmp->GetAttribute("str2");
				if(tmps!=NULL && *tmps=='#')
				{
					if( (m_pTwoStr[i]=NEW char[strlen(tmps)]) == NULL )
						_THROW("allocate memory error");
					strcpy(m_pTwoStr[i],&tmps[1]);
					m_twoStr[i] = -1L;
				}
				else
				{
					m_pTwoStr[i] = NULL;
					m_twoStr[i] = pStringService->GetStringNum(tmps);
				}
			}
			else
			{
				m_oneImgID[i] = -1L;
				m_twoImgID[i] = -1L;
				m_oneStr[i] = -1L;
				m_twoStr[i] = -1L;
				m_pOneStr[i] = NULL;
				m_pTwoStr[i] = NULL;
				if(m_oneBadTexture==-1) m_oneTexID[i] = -1;
				else m_oneTexID[i] = 0;
				if(m_twoBadTexture==-1) m_twoTexID[i] = -1;
				else m_twoTexID[i] = 0;
			}
		}
	}

	bool bRelativeRect = !GetIniLong(ini1,name1, ini2,name2, "bAbsoluteRectangle");
	for(i=0; i<4; i++)
	{
		sprintf(param,"position%d",i+1);
		m_imgRect[i] = GetIniLongRect(ini1,name1, ini2,name2, param, XYRECT(0,0,0,0));
        if(bRelativeRect)
            GetRelativeRect(m_imgRect[i]);
	}

	// get border picture
	if( ReadIniString(ini1,name1, ini2,name2, "border", param, sizeof(param),"") )
	{
		tmpstr = GetSubStr(param, param1, sizeof(param1));
		if( (m_sBorderGroupName = NEW char[strlen(param1)+1])==null )
			_THROW("allocate memory error")
		strcpy(m_sBorderGroupName,param1);
		m_texBorder = pPictureService->GetTextureID(m_sBorderGroupName);
		m_nBorderPicture = pPictureService->GetImageNum(m_sBorderGroupName,tmpstr);
		m_bShowBorder = m_texBorder!=-1 && m_nBorderPicture!=-1;
	}
	else
	{
		m_bShowBorder = false;
		m_texBorder = -1;
		m_sBorderGroupName = null;
	}

	// create index & vertex buffers
    vBuf = m_rs->CreateVertexBuffer(XI_ONETEX_FVF,4*4*2*sizeof(XI_ONETEX_VERTEX),D3DUSAGE_WRITEONLY);
}

void CXI_FOURIMAGE::ReleaseAll()
{
	int i;
	VERTEX_BUF_RELEASE(m_rs,vBuf);

	TEXTURE_RELEASE(m_rs,m_oneBadTexture);
	TEXTURE_RELEASE(m_rs,m_twoBadTexture);
	// Release all used textures
	for(i=0; i<m_nTexturesQuantity; i++)
	{
		PICTURE_TEXTURE_RELEASE(pPictureService,m_sGroupName[i],m_nTextureId[i]);
		PTR_DELETE(m_sGroupName[i]);
	}
	PTR_DELETE(m_sGroupName);
	PTR_DELETE(m_nTextureId);
	PICTURE_TEXTURE_RELEASE(pPictureService,m_sBorderGroupName,m_texBorder);

	for(i=0; i<4; i++)
	{
		PTR_DELETE(m_pOneStr[i]);
		PTR_DELETE(m_pTwoStr[i]);
	}

	// release all image list names
	PTR_DELETE(m_sBorderGroupName);

	FONT_RELEASE(m_rs,m_oneStrFont);
	FONT_RELEASE(m_rs,m_twoStrFont);

	m_nTexturesQuantity = 0;
}

void CXI_FOURIMAGE::FillVertex()
{
	XI_ONETEX_VERTEX* pVert = (XI_ONETEX_VERTEX*)m_rs->LockVertexBuffer(vBuf);
	if(pVert!=NULL)
	{
		FXYRECT textRect1,textRect2;

		for(int i=0; i<4; i++)
		{
			int idx = i*4;
			// get texture rectangles
			if(m_oneImgID[i]!=-1)
				pPictureService->GetTexturePos(m_oneImgID[i],textRect1);
			else
			{
				textRect1.left = 0.f;	textRect1.top = 0.f;
				textRect1.right = 1.f;	textRect1.bottom = 1.f;
			}
			if(m_twoImgID[i]!=-1)
				pPictureService->GetTexturePos(m_twoImgID[i],textRect2);
			else
			{
				textRect2.left = 0.f;	textRect2.top = 0.f;
				textRect2.right = 1.f;	textRect2.bottom = 1.f;
			}

			// left top vertex
			pVert[idx].pos.x		= pVert[16+idx].pos.x	= (float)m_imgRect[i].left;
			pVert[idx].pos.y		= pVert[16+idx].pos.y	= (float)m_imgRect[i].top;
			pVert[idx].tu			= textRect1.left;
			pVert[idx].tv			= textRect1.top;
			pVert[16+idx].tu		= textRect2.left;
			pVert[16+idx].tv		= textRect2.top;
			// left bottom vertex	
			pVert[idx+1].pos.x		= pVert[17+idx].pos.x	= (float)m_imgRect[i].left;
			pVert[idx+1].pos.y		= pVert[17+idx].pos.y	= (float)m_imgRect[i].bottom;
			pVert[idx+1].tu			= textRect1.left;
			pVert[idx+1].tv			= textRect1.bottom;
			pVert[17+idx].tu		= textRect2.left;
			pVert[17+idx].tv		= textRect2.bottom;
			// right top vertex		
			pVert[idx+2].pos.x		= pVert[18+idx].pos.x	= (float)m_imgRect[i].right;
			pVert[idx+2].pos.y		= pVert[18+idx].pos.y	= (float)m_imgRect[i].top;
			pVert[idx+2].tu			= textRect1.right;
			pVert[idx+2].tv			= textRect1.top;
			pVert[18+idx].tu		= textRect2.right;
			pVert[18+idx].tv		= textRect2.top;
			// right bottom vertex
			pVert[idx+3].pos.x		= pVert[19+idx].pos.x	= (float)m_imgRect[i].right;
			pVert[idx+3].pos.y		= pVert[19+idx].pos.y	= (float)m_imgRect[i].bottom;
			pVert[idx+3].tu			= textRect1.right;
			pVert[idx+3].tv			= textRect1.bottom;
			pVert[19+idx].tu		= textRect2.right;
			pVert[19+idx].tv		= textRect2.bottom;

			for(int j=0; j<4; j++,idx++)
			{
				if(i==m_nSelectItem) pVert[16+idx].color = m_dwCurSelectColor;
				else pVert[16+idx].color = m_dwBaseColor;
				pVert[idx].color = m_dwBaseColor;
				pVert[idx].pos.z = pVert[16+idx].pos.z = 1.f;
			}
		}

		m_rs->UnLockVertexBuffer(vBuf);
	}
}

void CXI_FOURIMAGE::Update(bool bSelected,dword DeltaTime)
{
	//
	XI_ONETEX_VERTEX* pVert = (XI_ONETEX_VERTEX*)m_rs->LockVertexBuffer(vBuf);
	if(pVert!=NULL)
	{
		int idx = 0;
		DWORD setColor = 0xFFFFFFFF;
		for(int i=0; i<4; i++)
		{
			if(i==m_nSelectItem)
			{
				if(m_bDoBlind && bSelected)
				{
					if((m_nBlindCounter-=DeltaTime)<=0)
					{
						m_nBlindCounter=m_nMaxBlindCounter;
						m_bColorType = !m_bColorType;
					}

                    DWORD ad = ALPHA(m_dwDarkSelectColor);
                    DWORD rd = RED(m_dwDarkSelectColor);
                    DWORD gd = GREEN(m_dwDarkSelectColor);
                    DWORD bd = BLUE(m_dwDarkSelectColor);
                    DWORD al = ALPHA(m_dwLightSelectColor);
                    DWORD rl = RED(m_dwLightSelectColor);
                    DWORD gl = GREEN(m_dwLightSelectColor);
                    DWORD bl = BLUE(m_dwLightSelectColor);
                    DWORD a,r,g,b;
					if(m_bColorType)
                    {
                        a = (al-ad)*m_nBlindCounter/m_nMaxBlindCounter;
                        r = (rl-rd)*m_nBlindCounter/m_nMaxBlindCounter;
                        g = (gl-gd)*m_nBlindCounter/m_nMaxBlindCounter;
                        b = (bl-bd)*m_nBlindCounter/m_nMaxBlindCounter;
                        m_dwCurSelectColor = ARGB(ad+a,rd+r,gd+g,bd+b);
                    }
					else
                    {
                        a = (al-ad)*m_nBlindCounter/m_nMaxBlindCounter;
                        r = (rl-rd)*m_nBlindCounter/m_nMaxBlindCounter;
                        g = (gl-gd)*m_nBlindCounter/m_nMaxBlindCounter;
                        b = (bl-bd)*m_nBlindCounter/m_nMaxBlindCounter;
                        m_dwCurSelectColor = ARGB(al-a,rl-r,gl-g,bl-b);
                    }
				}
				else m_dwCurSelectColor=m_dwLightSelectColor;
				setColor = m_dwCurSelectColor;
			}
			else setColor = m_dwBaseColor;

			for(int j=0; j<4; j++,idx++)
				if(m_twoTexID[i]!=-1) pVert[16+idx].color = setColor;
				else pVert[idx].color = setColor;
		}

		m_rs->UnLockVertexBuffer(vBuf);
	}
}

bool CXI_FOURIMAGE::IsClick(int buttonID,long xPos,long yPos)
{
	for(int i=0; i<4; i++)
		if( xPos>=m_imgRect[i].left && xPos<=m_imgRect[i].right &&
			yPos>=m_imgRect[i].top  && yPos<=m_imgRect[i].bottom && m_bClickable && m_bUse )
		{
			m_MousePoint.x = xPos;
			m_MousePoint.y = yPos;

			return true;
		}

	return false;
}

void CXI_FOURIMAGE::ChangePosition( XYRECT &rNewPos )
{
	long nXOffset = rNewPos.left - m_rect.left;
	long nYOffset = rNewPos.top - m_rect.top;
	long nXToGrow = rNewPos.right - m_rect.right;
	long nYToGrow = rNewPos.bottom - m_rect.bottom;

	m_rect = rNewPos;

	for( long n=0; n<4; n++ )
	{
		m_imgRect[n].left += nXOffset;
		m_imgRect[n].top += nYOffset;
		m_imgRect[n].right += nXOffset;
		m_imgRect[n].bottom += nYOffset;

		// находим является ли позиция правой или нижней
		long nXLessMore = 0;
		long nYLessMore = 0;
		for( long i=0; i<4; i++ ) {
			if( i==n ) continue;
			else {
				if( m_imgRect[i].left < m_imgRect[n].left ) nXLessMore++;
				if( m_imgRect[i].top < m_imgRect[n].top ) nYLessMore++;
			}
		}
		if( nXLessMore > 1 ) {
			m_imgRect[n].left += nXToGrow;
			m_imgRect[n].right += nXToGrow;
		}
		if( nYLessMore > 1 ) {
			m_imgRect[n].top += nYToGrow;
			m_imgRect[n].bottom += nYToGrow;
		}
	}

	FillVertex();
}

void CXI_FOURIMAGE::SaveParametersToIni()
{
	char pcWriteParam[2048];

	INIFILE * pIni = api->fio->OpenIniFile( (char*)ptrOwner->m_sDialogFileName.GetBuffer() );
	if( !pIni ) {
		api->Trace( "Warning! Can`t open ini file name %s", ptrOwner->m_sDialogFileName.GetBuffer() );
		return;
	}

	// save position
	char pcWriteKeyName[256];
	for( long n=0; n<4; n++ )
	{
		_snprintf( pcWriteParam, sizeof(pcWriteParam), "%d,%d,%d,%d",
			m_imgRect[n].left, m_imgRect[n].top, m_imgRect[n].right, m_imgRect[n].bottom );
		_snprintf( pcWriteKeyName, sizeof(pcWriteKeyName), "position%d", n );
		pIni->WriteString( m_nodeName, pcWriteKeyName, pcWriteParam );
	}

	delete pIni;
}

void CXI_FOURIMAGE::ChangeItem(int nItemNum)
{
    char param[256];
	ATTRIBUTES * pAttribute = _CORE_API->Entity_GetAttributeClass(&g_idInterface,"FourImage");
	if(pAttribute!=NULL)
	{
		for(int i=(nItemNum==-1?0:nItemNum);i<(nItemNum==-1?4:nItemNum+1);i++)
		{
			if(m_pOneStr[i]!=NULL)
			{
				delete m_pOneStr[i];
				m_pOneStr[i] = NULL;
			}
			if(m_pTwoStr[i]!=NULL)
			{
				delete m_pTwoStr[i];
				m_pTwoStr[i] = NULL;
			}
			sprintf(param,"pic%d",i+1);
			ATTRIBUTES * pAttrTmp = pAttribute->GetAttributeClass(param);
			if(pAttrTmp!=NULL)
			{
				char * sptr;
				m_bUsed[i] = pAttrTmp->GetAttributeAsDword("selected",0)!=0;
				m_oneTexID[i] = pAttrTmp->GetAttributeAsDword("tex1",-1);
				m_twoTexID[i] = pAttrTmp->GetAttributeAsDword("tex2",-1);
				if(m_oneTexID[i]==-1) m_oneImgID[i] = -1;
				else m_oneImgID[i] = pPictureService->GetImageNum(m_sGroupName[m_oneTexID[i]],pAttrTmp->GetAttribute("img1"));
				if(m_twoTexID[i]==-1) m_twoImgID[i] = -1;
				else m_twoImgID[i] = pPictureService->GetImageNum(m_sGroupName[m_twoTexID[i]],pAttrTmp->GetAttribute("img2"));
				sptr = pAttrTmp->GetAttribute("str1");
				if(sptr!=NULL && *sptr=='#')
					if( (m_pOneStr[i]=NEW char[strlen(sptr)]) == NULL )
					{
						_THROW("allocate memory error")
					}
					else	strcpy(m_pOneStr[i],&sptr[1]);
				else	m_oneStr[i] = pStringService->GetStringNum(sptr);

				sptr = pAttrTmp->GetAttribute("str2");
				if(sptr!=NULL && *sptr=='#')
					if( (m_pTwoStr[i]=NEW char[strlen(sptr)]) == NULL )
					{
						_THROW("allocate memory error")
					}
					else	strcpy(m_pTwoStr[i],&sptr[1]);
				else	m_twoStr[i] = pStringService->GetStringNum(sptr);
			}
			else
			{
				m_oneImgID[i] = -1L;
				m_twoImgID[i] = -1L;
				m_oneStr[i] = -1L;
				m_twoStr[i] = -1L;
				m_pOneStr[i] = NULL;
				m_pTwoStr[i] = NULL;
			}
		}

        FillVertex();
	}
}

XYRECT CXI_FOURIMAGE::GetCursorRect()
{
	if(m_nSelectItem<0 || m_nSelectItem>3) return m_rect;
	return m_imgRect[m_nSelectItem];
}
