#include <stdio.h>
#include "xi_scrollimage.h"
#include "..\\vxservice.h"

#define MAXIMAGEQUANTITY 100

IDirect3DTexture8 * GetTexFromEvent(VDATA * vdat)
{
	if(vdat==NULL) return null;
	DWORD dwTmp = vdat->GetLong();
	return (IDirect3DTexture8*)dwTmp;
}

CXI_SCROLLIMAGE::CXI_SCROLLIMAGE()
{
	m_bClickable = true;

	m_texBorder = -1;
	m_nCurImage = 0;
	m_nListSize = 0;
	m_Image = NULL;
	m_pScroll = NULL;
	m_bDoMove = false;
	m_sBorderGroupName = NULL;

	m_nOneStrFont = -1L;
	m_nTwoStrFont = -1L;

	m_nSpeedMul = 5;
	m_nNodeType = NODETYPE_SCROLLIMAGE;

	m_nGroupQuantity = 0;
	m_sGroupName = NULL;
	m_nGroupTex = NULL;
	m_nShowOrder = 100;
	m_nNotUsedQuantity = 0;
	m_sSpecTechniqueName = NULL;
	m_dwSpecTechniqueARGB = 0xFFFFFFFF;

	m_nSlotsQnt = 0;
	m_idBadTexture = null;
	m_idBadPic = null;
	m_pPicOffset = null;
	m_dwNormalColor = null;
	m_dwSelectColor = null;
	m_dwCurColor = null;
}

CXI_SCROLLIMAGE::~CXI_SCROLLIMAGE()
{
	ReleaseAll();
}

void CXI_SCROLLIMAGE::Draw(bool bSelected,dword Delta_Time)
{
	int n;
	if(m_bUse && m_Image!=NULL)
	{
		if(m_bDoMove)
		{
			m_fCurrentDistance += .01f*Delta_Time * m_fDeltaMove;
			if( (m_fCurrentDistance>=m_fMoveDistance && m_fDeltaMove>=0.f) ||
				(m_fCurrentDistance<=m_fMoveDistance && m_fDeltaMove<=0.f) )
				m_fCurrentDistance = m_fMoveDistance;
			float fDelta = ChangeDinamicParameters(m_fCurrentDistance);
			m_fCurrentDistance += fDelta; m_fMoveDistance += fDelta;
			if(m_fMoveDistance == m_fCurrentDistance)
			{
				m_bLockStatus = false;
				m_bDoMove = false;

				// Set new current image
				ATTRIBUTES* tmpAttr = _CORE_API->Entity_GetAttributeClass(&g_idInterface,m_nodeName);
				if(tmpAttr!=NULL)
					tmpAttr->SetAttributeUseDword("current",m_nCurImage);

				ChangeDinamicParameters(0);
			}
			for(n=0; n<m_nSlotsQnt; n++)
				m_dwCurColor[n] = m_dwNormalColor[n];
		}
        else
        {
            if(m_bDoBlind && bSelected)
            {
				if((m_nBlindCounter-=Delta_Time)<=0)
				{
					m_nBlindCounter=m_nMaxBlindCounter;
					m_bColorType = !m_bColorType;
				}

				for(n=0; n<m_nSlotsQnt; n++)
				{
					int ad = ALPHA(m_dwNormalColor[n]);
					int rd = RED(m_dwNormalColor[n]);
					int gd = GREEN(m_dwNormalColor[n]);
					int bd = BLUE(m_dwNormalColor[n]);
					int al = ALPHA(m_dwSelectColor[n]);
					int rl = RED(m_dwSelectColor[n]);
					int gl = GREEN(m_dwSelectColor[n]);
					int bl = BLUE(m_dwSelectColor[n]);
					int a,r,g,b;
					if(m_bColorType)
					{
						a = (al-ad)*m_nBlindCounter/m_nMaxBlindCounter;
						r = (rl-rd)*m_nBlindCounter/m_nMaxBlindCounter;
						g = (gl-gd)*m_nBlindCounter/m_nMaxBlindCounter;
						b = (bl-bd)*m_nBlindCounter/m_nMaxBlindCounter;
						m_dwCurColor[n] = ARGB(ad+a,rd+r,gd+g,bd+b);
					}
					else
					{
						a = (al-ad)*m_nBlindCounter/m_nMaxBlindCounter;
						r = (rl-rd)*m_nBlindCounter/m_nMaxBlindCounter;
						g = (gl-gd)*m_nBlindCounter/m_nMaxBlindCounter;
						b = (bl-bd)*m_nBlindCounter/m_nMaxBlindCounter;
						m_dwCurColor[n] = ARGB(al-a,rl-r,gl-g,bl-b);
					}
				}
            }
            else
			{
				for(n=0; n<m_nSlotsQnt; n++)
					m_dwCurColor[n] = m_dwNormalColor[n];
			}
        }

		// create select border
		XI_ONLYONETEX_VERTEX pV[4];
		FXYRECT textureRect;
		for(int i=0; i<4; i++) pV[i].pos.z = 1.f;
		pPictureService->GetTexturePos(m_nBorderPicture,textureRect);
		pV[0].tu = textureRect.left;	pV[0].tv = textureRect.top;
		pV[1].tu = textureRect.left;	pV[1].tv = textureRect.bottom;
		pV[2].tu = textureRect.right;	pV[2].tv = textureRect.top;
		pV[3].tu = textureRect.right;	pV[3].tv = textureRect.bottom;
		pV[0].pos.x = (float)(m_pCenter.x-m_ImageSize.x/2);
		pV[0].pos.y = (float)(m_pCenter.y-m_ImageSize.y/2);
		pV[1].pos.x = (float)(m_pCenter.x-m_ImageSize.x/2);
		pV[1].pos.y = (float)(m_pCenter.y+m_ImageSize.y/2);
		pV[2].pos.x = (float)(m_pCenter.x+m_ImageSize.x/2);
		pV[2].pos.y = (float)(m_pCenter.y-m_ImageSize.y/2);
		pV[3].pos.x = (float)(m_pCenter.x+m_ImageSize.x/2);
		pV[3].pos.y = (float)(m_pCenter.y+m_ImageSize.y/2);

		// show select border
		if(m_bShowBorder /*&& !m_bLockStatus*/ && m_nShowOrder<0)
		{
			m_rs->TextureSet(0,m_texBorder);
			m_rs->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,XI_ONLYONETEX_FVF,2,pV,sizeof(XI_ONLYONETEX_VERTEX),"iScrollImages_border");
		}

        XI_ONETEX_VERTEX v[4];

		int j=0;
		FXYRECT rectTex;
		SCROLLENTITY * pScroll;
		int curShowOrder = m_nShowOrder;
		//if(m_bLockStatus) curShowOrder = m_nSlotsQnt-1;
		if(curShowOrder>=m_nSlotsQnt) curShowOrder = m_nSlotsQnt-1;
		bool bDoShowBorder = false;

		for(n=0; n<m_nSlotsQnt; n++)
		{
			pScroll = m_pScroll;
			while(pScroll!=null)
			{
				FXYRECT pos;

				if(m_Image[pScroll->imageNum].ptex[n]!=null)
				{
					m_rs->SetTexture(0,m_Image[pScroll->imageNum].ptex[n]);
					rectTex.left = 0.f;		rectTex.top = 0.f;
					rectTex.right = 1.f;	rectTex.bottom = 1.f;
				}
				else
					if(m_Image[pScroll->imageNum].img[n]!=-1)
					{
						// get texture rectangle
						pPictureService->GetTexturePos(m_Image[pScroll->imageNum].img[n],rectTex);
						m_rs->TextureSet(0,m_nGroupTex[m_Image[pScroll->imageNum].tex[n]]);
					}
					else
					{
						if(m_idBadPic[n]!=-1 && m_idBadTexture[n]!=-1) // частичное использование текстуры для "плохой" картинки
						{
							m_rs->TextureSet(0,m_nGroupTex[m_idBadTexture[n]]);
							pPictureService->GetTexturePos(m_idBadPic[n],rectTex);
						}
						else	// "плохая" картинка на всю текстуру
						{
							if(m_idBadTexture[n]!=-1)
							{
								m_rs->TextureSet(0,m_idBadTexture[n]);
								rectTex.left = 0.f;		rectTex.top = 0.f;
								rectTex.right = 1.f;	rectTex.bottom = 1.f;
							}
							else // не показывать несуществующую картинку
							{
								pScroll = pScroll->next;
								continue;
							}
						}
					}

				pos.right = (pos.left = pScroll->pCenter.x-m_ImageSize.x/2.f) + m_ImageSize.x;
				pos.bottom = (pos.top = pScroll->pCenter.y-m_ImageSize.y/2.f+m_pPicOffset[n]) + m_ImageSize.y;
				if(pos.left<m_rect.left)
				{
					if( pos.right<=m_rect.left ) {
						pScroll = pScroll->next;
						continue;
					}
					float texCorrect = (m_rect.left-pos.left)/(pos.right-pos.left);
					rectTex.left += texCorrect*(rectTex.right-rectTex.left);
					pos.left = (float)m_rect.left;
				}
				if(pos.right>m_rect.right)
				{
					if( pos.left>=m_rect.right ) {
						pScroll = pScroll->next;
						continue;
					}
					float texCorrect = (m_rect.right-pos.right)/(pos.right-pos.left);
					rectTex.right += texCorrect*(rectTex.right-rectTex.left);
					pos.right = (float)m_rect.right;
				}
				v[0].pos = CVECTOR((float)pos.left,  (float)pos.top,    1.f);
				v[1].pos = CVECTOR((float)pos.left,  (float)pos.bottom, 1.f);
				v[2].pos = CVECTOR((float)pos.right, (float)pos.top,    1.f);
				v[3].pos = CVECTOR((float)pos.right, (float)pos.bottom, 1.f);
				v[0].tu = v[1].tu = rectTex.left;
				v[2].tu = v[3].tu = rectTex.right;
				v[0].tv = v[2].tv = rectTex.top;
				v[1].tv = v[3].tv = rectTex.bottom;
				if(pScroll==m_pScroll)
					v[0].color = v[1].color = v[2].color = v[3].color = m_dwCurColor[n];
				else
					v[0].color = v[1].color = v[2].color = v[3].color = m_dwNormalColor[n];
				if(m_Image[pScroll->imageNum].bUseSpecTechnique[n])
				{
					m_rs->SetRenderState(D3DRS_TEXTUREFACTOR,m_dwSpecTechniqueARGB);
					m_rs->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,XI_ONETEX_FVF,2,v,sizeof(XI_ONETEX_VERTEX),m_sSpecTechniqueName);
				}
				else
					m_rs->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,XI_ONETEX_FVF,2,v,sizeof(XI_ONETEX_VERTEX),"iScrollImages_main");

				pScroll = pScroll->next;
			}

			if(n==m_nSlotsQnt-1) // последними показать строки
			{
				// out to screen the strings if that needed
				if( m_bUseOneString || m_bUseTwoString )
				{
					pScroll = m_pScroll;
					while(pScroll!=NULL)
					{
						if(m_bUseOneString)
						{
							if(m_Image[pScroll->imageNum].str1!=-1L)
								ptrOwner->PrintIntoWindow( m_rect.left, m_rect.right,
									m_nOneStrFont, m_dwOneStrForeColor,m_dwOneStrBackColor,
									m_nOneStrAlign, true, m_nOneStrScale,  m_screenSize.x,m_screenSize.y,
									long(pScroll->pCenter.x + m_lOneStrX),m_rect.top+m_lOneStrOffset,
									pStringService->GetString(m_Image[pScroll->imageNum].str1), (int)(m_ImageSize.x*pScroll->fCurScale), (int)(-24*m_nOneStrScale) );
							else if(m_Image[pScroll->imageNum].string1!=NULL)
								ptrOwner->PrintIntoWindow( m_rect.left, m_rect.right,
									m_nOneStrFont, m_dwOneStrForeColor,m_dwOneStrBackColor,
									m_nOneStrAlign, true, m_nOneStrScale, m_screenSize.x,m_screenSize.y,
									long(pScroll->pCenter.x + m_lOneStrX),m_rect.top+m_lOneStrOffset,
									m_Image[pScroll->imageNum].string1, (int)(m_ImageSize.x*pScroll->fCurScale), (int)(-24*m_nOneStrScale) );
						}
						if(m_bUseTwoString)
						{
							if(m_Image[pScroll->imageNum].str2!=-1L)
								ptrOwner->PrintIntoWindow( m_rect.left, m_rect.right,
									m_nTwoStrFont, m_dwTwoStrForeColor,m_dwTwoStrBackColor,
									m_nTwoStrAlign, true, m_nTwoStrScale, m_screenSize.x,m_screenSize.y,
									long(pScroll->pCenter.x + m_lTwoStrX),m_rect.top+m_lTwoStrOffset,
									pStringService->GetString(m_Image[pScroll->imageNum].str2), (int)(m_ImageSize.x*pScroll->fCurScale), (int)(-24*m_nTwoStrScale) );
							if(m_Image[pScroll->imageNum].string2!=NULL)
								ptrOwner->PrintIntoWindow( m_rect.left, m_rect.right,
									m_nTwoStrFont, m_dwTwoStrForeColor,m_dwTwoStrBackColor,
									m_nTwoStrAlign, true, m_nTwoStrScale, m_screenSize.x,m_screenSize.y,
									long(pScroll->pCenter.x + m_lTwoStrX),m_rect.top+m_lTwoStrOffset,
									m_Image[pScroll->imageNum].string2, (int)(m_ImageSize.x*pScroll->fCurScale), (int)(-24*m_nTwoStrScale) );
						}

						pScroll = pScroll->next;
					}
				}
			}

			if(m_bShowBorder && n==curShowOrder)
			{
				// show select border
				m_rs->TextureSet(0,m_texBorder);
				m_rs->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,XI_ONLYONETEX_FVF,2,pV,sizeof(XI_ONLYONETEX_VERTEX),"iScrollImages_border");
				bDoShowBorder = true;
			}
		}

		if(m_bShowBorder && !bDoShowBorder)
		{
			// show select border
			m_rs->TextureSet(0,m_texBorder);
			m_rs->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,XI_ONLYONETEX_FVF,2,pV,sizeof(XI_ONLYONETEX_VERTEX),"iScrollImages_border");
		}
	}
}

bool CXI_SCROLLIMAGE::Init(INIFILE *ini1,char *name1, INIFILE *ini2,char *name2, VDX8RENDER *rs, XYRECT &hostRect, XYPOINT &ScreenSize)
{
	if( !CINODE::Init(ini1,name1, ini2,name2, rs,hostRect,ScreenSize) ) return false;
	return true;
}

void CXI_SCROLLIMAGE::LoadIni(INIFILE *ini1,char *name1, INIFILE *ini2,char *name2)
{
	int i,n;
	char param[256];
	char param1[256];
	char * tmpstr;

	// Set space
	m_rAbsolutePosition = GetIniLongRect(ini1,name1, ini2,name2, "position", m_hostRect);
    GetAbsoluteRect(m_rAbsolutePosition,0);
	m_pCenter.x = (m_rAbsolutePosition.right+m_rAbsolutePosition.left)/2;
	m_pCenter.y = (m_rAbsolutePosition.top+m_rAbsolutePosition.bottom)/2;
	m_pntCenterOffset.x = GetIniLong(ini1,name1, ini2,name2, "centerXoffset", m_pCenter.x-m_rAbsolutePosition.left);
	m_pntCenterOffset.y = GetIniLong(ini1,name1, ini2,name2, "centerYoffset", m_pCenter.y-m_rAbsolutePosition.top);
	m_pCenter.x = m_rAbsolutePosition.left + m_pntCenterOffset.x;
	m_pCenter.y = m_rAbsolutePosition.top + m_pntCenterOffset.y;

    // set center of scrolling list
	m_fDeltaMoveBase = GetIniFloat(ini1,name1, ini2,name2, "fMoveDelta", 1.f);
	m_nSpeedMul = GetIniLong(ini1,name1, ini2,name2, "speedMul", 5);

	// set image size
	m_ImageSize = GetIniLongPoint(ini1,name1, ini2,name2, "imageSize", XYPOINT(128,128));

	// set parameters for blend & minimize far images
	m_fScale = GetIniFloat(ini1,name1, ini2,name2, "fBoundScale", 1.f);
	m_lDelta = GetIniLong(ini1,name1, ini2,name2, "wDelta", 0);
	m_dwBlendColor = GetIniARGB(ini1,name1, ini2,name2, "blendColor", 0xFFFFFFFF);

	//
    m_nMaxBlindCounter = GetIniLong(ini1,name1, ini2,name2, "blindDelay", 2000);
    m_bDoBlind = true;
    m_bColorType = true;
    m_nBlindCounter = m_nMaxBlindCounter;

	m_nSlotsQnt = GetIniLong(ini1,name1, ini2,name2, "LayerQuantity", 0);

	if(m_nSlotsQnt>0)
	{
		m_dwCurColor = NEW DWORD[m_nSlotsQnt];
		m_dwNormalColor = NEW DWORD[m_nSlotsQnt];
		m_dwSelectColor = NEW DWORD[m_nSlotsQnt];
		m_pPicOffset = NEW long[m_nSlotsQnt];
		m_idBadTexture = NEW long[m_nSlotsQnt];
		m_idBadPic = NEW long[m_nSlotsQnt];
		if( !m_dwCurColor || !m_dwNormalColor ||
			!m_dwSelectColor || !m_pPicOffset ||
			!m_idBadTexture || !m_idBadPic ) {
			_THROW("allocate memory error");
		}
	}

    // set parameters for blind
	for(i=0; i<m_nSlotsQnt; i++)
	{
		sprintf(param,"dwNormalColorARGB%d",i+1);
		m_dwCurColor[i] = m_dwNormalColor[i] = GetIniARGB(ini1,name1, ini2,name2, param, ARGB(255,128,128,128));
		sprintf(param,"dwSelectColorARGB%d",i+1);
		m_dwSelectColor[i] = GetIniARGB(ini1,name1, ini2,name2, param, ARGB(255,64,64,64));
		sprintf(param,"PicOffset%d",i+1);
		m_pPicOffset[i] = GetIniLong(ini1,name1, ini2,name2, param, 0);
		m_idBadTexture[i] = -1;
		m_idBadPic[i] = -1;
	}

	// set stringes
	m_bUseOneString = GetIniBool( ini1,name1, ini2,name2, "UseOneString", false);
	m_bUseTwoString = GetIniBool( ini1,name1, ini2,name2, "UseTwoString", false);
	if(m_bUseOneString)
	{
		m_nOneStrScale = GetIniFloat(ini1,name1, ini2,name2, "scale1", 1.f);
		if( ReadIniString(ini1,name1, ini2,name2, "font1", param, sizeof(param),"") )
			if( (m_nOneStrFont=m_rs->LoadFont(param))==-1 )
				_CORE_API->Trace("can not load font:'%s'",param);
		m_lOneStrX = GetIniLong(ini1,name1, ini2,name2, "dwXOffset1", 0);
		if(m_lOneStrX>0) m_nOneStrAlign = ALIGN_RIGHT;
		else if(m_lOneStrX<0) m_nOneStrAlign = ALIGN_LEFT;
		else m_nOneStrAlign = ALIGN_CENTER;
		m_lOneStrOffset = GetIniLong(ini1,name1, ini2,name2, "dwYOffset1", 0);
		m_dwOneStrForeColor = GetIniARGB(ini1,name1, ini2,name2, "dwForeColor1", 0xFFFFFFFF);
		m_dwOneStrBackColor = GetIniARGB(ini1,name1, ini2,name2, "dwBackColor1", 0);
	}
	if(m_bUseTwoString)
	{
		m_nTwoStrScale = GetIniFloat(ini1,name1, ini2,name2, "scale2", 1.f);
		if( ReadIniString(ini1,name1, ini2,name2, "font2", param, sizeof(param),"") )
			if( (m_nTwoStrFont=m_rs->LoadFont(param))==-1 )
				_CORE_API->Trace("can not load font:'%s'",param);
		m_lTwoStrX = GetIniLong(ini1,name1, ini2,name2, "dwXOffset2", 0);
		if(m_lTwoStrX>0) m_nTwoStrAlign = ALIGN_RIGHT;
		else if(m_lTwoStrX<0) m_nTwoStrAlign = ALIGN_LEFT;
		else m_nTwoStrAlign = ALIGN_CENTER;
		m_lTwoStrOffset = GetIniLong(ini1,name1, ini2,name2, "dwYOffset2", 0);
		m_dwTwoStrForeColor = GetIniARGB(ini1,name1, ini2,name2, "dwForeColor2", 0xFFFFFFFF);
		m_dwTwoStrBackColor = GetIniARGB(ini1,name1, ini2,name2, "dwBackColor2", 0);
	}

	ATTRIBUTES * pAttribute = api->Entity_GetAttributeClass(&g_idInterface,m_nodeName);
	if(pAttribute!=NULL)
	{
		// get special technique name and color
		m_dwSpecTechniqueARGB = pAttribute->GetAttributeAsDword("SpecTechniqueColor");
		char * sTechnique = pAttribute->GetAttribute("SpecTechniqueName");
		if(sTechnique!=NULL)
		{
			if( (m_sSpecTechniqueName=NEW char[strlen(sTechnique)+1])==NULL )
			{
				_THROW("Allocate memory error");
			}
			strcpy(m_sSpecTechniqueName,sTechnique);
		}
		// get images quantity
		m_nListSize = pAttribute->GetAttributeAsDword("ListSize",0);
		m_nNotUsedQuantity = pAttribute->GetAttributeAsDword("NotUsed",0);
		m_nListSize += m_nNotUsedQuantity;
		// create images array
		if(m_nListSize>0)
			m_Image = NEW IMAGEDESCRIBE[m_nListSize];
		else
		{
			m_Image = NULL;
			return;
		}
		m_nCurImage = pAttribute->GetAttributeAsDword("current",0);
		if(m_nCurImage>=m_nListSize || m_nCurImage<0)
			m_nCurImage = 0;
		pAttribute->SetAttributeUseDword("current",m_nCurImage);

		// get textures
		ATTRIBUTES * pA = pAttribute->GetAttributeClass("ImagesGroup");
		if(pA!=NULL)
		{
			m_nGroupQuantity = pA->GetAttributesNum();
			if(m_nGroupQuantity!=0)
			{
				m_nGroupTex = NEW long[m_nGroupQuantity];
				m_sGroupName = NEW char*[m_nGroupQuantity];
				if( m_nGroupTex==NULL || m_sGroupName==NULL )
				{
					_THROW("allocate memory error");
				}
				for(i=0;i<m_nGroupQuantity;i++)
				{
					char * stmp = pA->GetAttribute(i);
					if(stmp==NULL) continue;
					m_sGroupName[i] = NEW char[strlen(stmp)+1];
					if(m_sGroupName[i]==NULL)
					{
						_THROW("allocate memory error");
					}
					strcpy(m_sGroupName[i],stmp);
					m_nGroupTex[i] = pPictureService->GetTextureID(m_sGroupName[i]);
				}
			}
		}

		// get bad picture
		for(n=0; n<m_nSlotsQnt; n++)
		{
			char * sBadPict;
			sprintf(param,"BadPicture%d",n+1);
			if( (sBadPict=pAttribute->GetAttribute(param)) != NULL)
			{
				m_idBadTexture[n] = m_rs->TextureCreate(sBadPict);
				m_idBadPic[n] = -1;
			}
			else
			{
				sprintf(param,"BadTex%d",n+1);
				m_idBadTexture[n] = pAttribute->GetAttributeAsDword(param,-1);
				if(m_idBadTexture[n]>=0)
				{
					sprintf(param,"BadPic%d",n+1);
					m_idBadPic[n] = pPictureService->GetImageNum(m_sGroupName[m_idBadTexture[n]],pAttribute->GetAttribute(param));
				}
				else	m_idBadPic[n] = -1;
				if(m_idBadPic[n]==-1)	m_idBadTexture[n] = -1;
			}
		}

		// get all scroll entity
		for(i=0; i<m_nListSize; i++)
		{
			char attrName[256];
			char * sStringName;
			sprintf( attrName,"pic%d",i+1);
			ATTRIBUTES * pListEntity = pAttribute->GetAttributeClass(attrName);

			// Fill image descriptor by default value
			//------------------------------------------------------
			m_Image[i].str1 = m_Image[i].str2 = -1;
			m_Image[i].string1 = m_Image[i].string2 = null;
			if(m_nSlotsQnt>0)
			{
				m_Image[i].bUseSpecTechnique =  NEW bool[m_nSlotsQnt];
				m_Image[i].img = NEW long[m_nSlotsQnt];
				m_Image[i].ptex = NEW IDirect3DTexture8*[m_nSlotsQnt];
				m_Image[i].saveName = NEW char*[m_nSlotsQnt];
				m_Image[i].tex = NEW long[m_nSlotsQnt];
				if( !m_Image[i].bUseSpecTechnique || !m_Image[i].img ||
					!m_Image[i].ptex || !m_Image[i].saveName ||
					!m_Image[i].tex )
				{
					_THROW("allocate memory error");
				}
				for(n=0; n<m_nSlotsQnt; n++)
				{
					m_Image[i].bUseSpecTechnique[n] = false;
					m_Image[i].img[n] = -1;
					m_Image[i].ptex[n] = null;
					m_Image[i].saveName[n] = null;
					m_Image[i].tex[n] = -1;
				}
			} else {
				m_Image[i].bUseSpecTechnique = null;
				m_Image[i].img = null;
				m_Image[i].ptex = null;
				m_Image[i].saveName = null;
				m_Image[i].tex = null;
			}

			if(pListEntity!=NULL)
			{
				// set one string
				if(m_bUseOneString)
				{
					sStringName = pListEntity->GetAttribute("str1");
					if(sStringName!=NULL && sStringName[0]=='#')
					{
						m_Image[i].string1 = NEW char[strlen(sStringName)];
						if(m_Image[i].string1==NULL)
							_THROW("allocate memory error")
						strcpy(m_Image[i].string1,&(sStringName[1]));
					}
					else
						m_Image[i].str1 = pStringService->GetStringNum(sStringName);
				}

				// set two string
				if(m_bUseTwoString)
				{
					sStringName = pListEntity->GetAttribute("str2");
					if(sStringName!=NULL && sStringName[0]=='#')
					{
						m_Image[i].string2 = NEW char[strlen(sStringName)];
						if(m_Image[i].string2==NULL)
							_THROW("allocate memory error")
						strcpy(m_Image[i].string2,&(sStringName[1]));
					}
					else
						m_Image[i].str2 = pStringService->GetStringNum(sStringName);
				}

				// set pictures
				char * tmpStr;
				for(n=0; n<m_nSlotsQnt; n++)
				{
					sprintf(param,"name%d",n+1);
					tmpStr=pListEntity->GetAttribute(param);
					if(tmpStr!=null)
					{
						if( (m_Image[i].saveName[n]=NEW char[strlen(tmpStr)+1])==0 )
							{_THROW("allocate memory error");}
						strcpy(m_Image[i].saveName[n],tmpStr);
					}
					sprintf(param,"tex%d",n+1);
					m_Image[i].tex[n] = pListEntity->GetAttributeAsDword(param,-1);
					sprintf(param,"img%d",n+1);
					if(m_Image[i].tex[n]!=-1)
						m_Image[i].img[n] = pPictureService->GetImageNum(m_sGroupName[m_Image[i].tex[n]],pListEntity->GetAttribute(param));
					else	m_Image[i].img[n] = -1;
					sprintf(param,"spec%d",n+1);
					m_Image[i].bUseSpecTechnique[n] = pListEntity->GetAttributeAsDword(param,0)!=0;
				}
			}
		}
	}

	// get border picture
	m_nShowOrder = GetIniLong( ini1,name1, ini2,name2, "borderShowOrder", 100); // boredrShowUp
	if( ReadIniString(ini1,name1, ini2,name2, "border", param, sizeof(param),"") )
	{
		tmpstr = GetSubStr(param, param1, sizeof(param1));
		if( (m_sBorderGroupName = NEW char[strlen(param1)+1])==null )
			_THROW("allocate memory error")
		strcpy(m_sBorderGroupName,param1);
		m_texBorder = pPictureService->GetTextureID(m_sBorderGroupName);
		m_nBorderPicture = pPictureService->GetImageNum(m_sBorderGroupName,tmpstr);
		m_bShowBorder = m_texBorder!=-1;
	}
	else
	{
		m_bShowBorder = false;
		m_texBorder = -1;
		m_sBorderGroupName = null;
	}

	// set images parameters
	ChangeDinamicParameters(0);
}

float CXI_SCROLLIMAGE::ChangeDinamicParameters(float fXDelta)
{
	if(m_Image==null) return 0.f;
	int n;

	float   curScale;
	bool	bIncrement = true;
	float	curXCenter = m_pCenter.x+fXDelta;
	int		curImage = m_nCurImage;
	long	lrBorderDist = m_rect.right-m_pCenter.x;
	long	llBorderDist = m_pCenter.x-m_rect.left;

	int		newCurImage = m_nCurImage;
	float	fNewCurCenter = curXCenter;

	SCROLLENTITY * pPrevScroll = NULL;
	SCROLLENTITY * pScroll = m_pScroll;

	long aDeleteImageIndex[200];
	long nDeleteImageQuantity = 0;

	while(true)
	{
		if( pScroll==NULL )
		{
			pScroll = NEW SCROLLENTITY;
			if(pScroll==NULL)
				_THROW("allocate memory error")

			if(pPrevScroll==NULL)
				m_pScroll = pScroll;
			else
				pPrevScroll->next = pScroll;

			pScroll->next = NULL;
		}

		while(true)
		{
			pScroll->imageNum = curImage;
			for(n=0; n<m_nSlotsQnt; n++)
			{
				if(m_Image[curImage].saveName[n]!=null)
				{
					if(m_Image[curImage].ptex[n]==null)
					{
						m_Image[curImage].ptex[n] = GetTexFromEvent(api->Event("GetInterfaceTexture","sls",m_Image[curImage].saveName[n],curImage, m_nodeName));
						break;
					}
				}
			}
			if( n==m_nSlotsQnt || (DWORD)m_Image[curImage].ptex[n]!=0xFFFFFFFF ) break;

			// delete current save from list
			m_Image[curImage].Release(m_nSlotsQnt);
			if(curImage<m_nListSize-1)
				memcpy( &m_Image[curImage], &m_Image[curImage+1], sizeof(IMAGEDESCRIBE)*(m_nListSize-1-curImage) );
			m_nListSize--;
			// Передвинем все уже используемые картинки
			for(SCROLLENTITY * pSTmp = m_pScroll; pSTmp!=null && pSTmp!=pScroll; pSTmp=pSTmp->next)
				if(pSTmp->imageNum>curImage)
					pSTmp->imageNum--;
			if(!bIncrement)
				curImage--;
			// 
			if(curImage<0 || curImage>=m_nListSize) break;
		}
		if(curImage<0 || curImage>=m_nListSize) break;

		// Calculate color
		if(curXCenter>=m_pCenter.x)
		{
			pScroll->colorMul = float(curXCenter-m_pCenter.x) / lrBorderDist;
		}
		else
		{
			pScroll->colorMul = float(m_pCenter.x-curXCenter) / llBorderDist;
		}

		// Calculate current scale
		if(curXCenter>=m_pCenter.x)
		{
			curScale = 1.f - (curXCenter-m_pCenter.x)/lrBorderDist*(1.f-m_fScale);
		}
		else
		{
			curScale = 1.f - (m_pCenter.x-curXCenter)/llBorderDist*(1.f-m_fScale);
		}

		// set position
		pScroll->fCurScale = curScale;
		pScroll->pCenter.x = curXCenter;
		pScroll->pCenter.y = (float)m_pCenter.y;

		float lpos = curXCenter - m_ImageSize.x*.5f;
		float rpos = curXCenter + m_ImageSize.x*.5f;

		if(m_pCenter.x>=lpos && m_pCenter.x<=rpos)
		{
			newCurImage = curImage;
			fNewCurCenter = curXCenter;
		}

		if(bIncrement && rpos>m_rect.right-m_lDelta) // end pass to right
		{
			// удалим следующую (неиспользуемую текстуру)
			curImage++;
			if(curImage>=m_nListSize) curImage = 0;
			aDeleteImageIndex[nDeleteImageQuantity++] = curImage;
			// продолжим вывод следующей иконки с центра
			curImage = m_nCurImage;
			curXCenter = m_pCenter.x + fXDelta;
			if(curXCenter>=m_pCenter.x)
			{
				curScale = 1.f - (curXCenter-m_pCenter.x)/lrBorderDist*(1.f-m_fScale);
			}
			else
			{
				curScale = 1.f - (m_pCenter.x-curXCenter)/llBorderDist*(1.f-m_fScale);
			}
			lpos = curXCenter - m_ImageSize.x*.5f*curScale;
			bIncrement = false;
		}
		else if(!bIncrement && lpos<m_rect.left+m_lDelta) // end pass to left is all end
		{
			// удалим предыдущую (неиспользуемую текстуру)
			curImage--; if(curImage<0) curImage = m_nListSize-1;
			aDeleteImageIndex[nDeleteImageQuantity++] = curImage;
			break;
		}

		// next image
		if(bIncrement)
		{
			curImage++;	if(curImage>=m_nListSize) curImage = 0;
			curXCenter = rpos + m_lDelta + m_ImageSize.x*0.5f;
		}
		else
		{
			curImage--;	if(curImage<0) curImage = m_nListSize-1;
			curXCenter = lpos - m_lDelta - m_ImageSize.x*0.5f;
		}

		pPrevScroll = pScroll;
		pScroll = pScroll->next;
	}

	if(pScroll->next!=NULL)
	{
		SCROLLENTITY * pScr = pScroll->next;
		pScroll->next = NULL;
		while(pScr!=NULL)
		{
			pScroll = pScr;
			pScr = pScr->next;
			delete pScroll;
		}
	}

	long i;
	pScroll = m_pScroll;
	for( pScroll=m_pScroll; pScroll; pScroll=pScroll->next ) {
		for( i=0; i<nDeleteImageQuantity; i++ )
			if( aDeleteImageIndex[i] == pScroll->imageNum )
				aDeleteImageIndex[i] = -1;
	}
	for( i=0; i<nDeleteImageQuantity; i++ )
	{
		curImage = aDeleteImageIndex[i];
		if( curImage<0 ) continue;
		for(n=0; n<m_nSlotsQnt; n++)
		{
			if(m_Image[curImage].saveName[n]!=null && m_Image[curImage].ptex[n]!=null)
			{
				api->Event("DelInterfaceTexture","ss",m_Image[curImage].saveName[n],m_nodeName);
				m_Image[curImage].ptex[n] = null;
			}
		}
	}

	if(newCurImage!=m_nCurImage)
	{
		m_nCurImage = newCurImage;
		api->Event("ChangeSelectScrollImage","sl",m_nodeName,m_nCurImage);
		return fNewCurCenter - m_pCenter.x - fXDelta;
	}

	return 0;
}

void CXI_SCROLLIMAGE::ReleaseAll()
{
	int i;

	PTR_DELETE(m_sSpecTechniqueName);

	if (m_idBadPic && m_idBadTexture)
		for(i=0; i<m_nSlotsQnt; i++)
		{
			if(m_idBadPic[i]==-1) {
				TEXTURE_RELEASE(m_rs,m_idBadTexture[i]);
			}
			else m_idBadTexture[i] = -1;
			m_idBadPic[i] = -1;
		}

	PTR_DELETE(m_idBadPic);
	PTR_DELETE(m_idBadTexture);

	if(m_Image!=NULL)
	{
		for(i=0; i<m_nListSize; i++) m_Image[i].Release(m_nSlotsQnt);
		PTR_DELETE(m_Image);
	}

	while(m_pScroll!=NULL)
	{
		SCROLLENTITY * rootScroll = m_pScroll;
		m_pScroll = m_pScroll->next;
		delete rootScroll;
	}

	for(i=0;i<m_nGroupQuantity;i++)
	{
		PICTURE_TEXTURE_RELEASE(pPictureService,m_sGroupName[i],m_nGroupTex[i]);
		PTR_DELETE(m_sGroupName[i]);
	}
	PTR_DELETE(m_sGroupName);
	PTR_DELETE(m_nGroupTex);

	PICTURE_TEXTURE_RELEASE(pPictureService,m_sBorderGroupName,m_texBorder);
	PTR_DELETE(m_sBorderGroupName);

	FONT_RELEASE(m_rs,m_nOneStrFont);
	FONT_RELEASE(m_rs,m_nTwoStrFont);

	PTR_DELETE(m_dwCurColor);
	PTR_DELETE(m_dwNormalColor);
	PTR_DELETE(m_dwSelectColor);
	PTR_DELETE(m_pPicOffset);

	m_nGroupQuantity = 0;
	m_nSlotsQnt = 0;
}

int CXI_SCROLLIMAGE::CommandExecute(int wActCode)
{
	int i;
	if(m_bUse && m_Image!=null && m_pScroll!=null)
	{
		if(m_bLockStatus) return -1;

		switch(wActCode)
		{
		case ACTION_RIGHTSTEP:
		case ACTION_SPEEDRIGHT:
			i = GetRightQuantity();
			if(i==0) break;
			if(wActCode==ACTION_RIGHTSTEP) i = 1;
			m_bLockStatus = true;
			m_bDoMove = true;
			m_fCurrentDistance = 0.f;
			m_fMoveDistance = GetShiftDistance(i);
			if(wActCode==ACTION_SPEEDRIGHT)	m_fDeltaMove = -m_fDeltaMoveBase*m_nSpeedMul;
			else	m_fDeltaMove = -m_fDeltaMoveBase;
		break;

		case ACTION_LEFTSTEP:
		case ACTION_SPEEDLEFT:
			i = GetLeftQuantity();
			if(i==0) break;
			if(wActCode==ACTION_LEFTSTEP) i = 1;
			m_bLockStatus = true;
			m_bDoMove = true;
			m_fCurrentDistance = 0.f;
			m_fMoveDistance = GetShiftDistance(-i);
			if(wActCode==ACTION_SPEEDLEFT)	m_fDeltaMove = m_fDeltaMoveBase*m_nSpeedMul;
			else	m_fDeltaMove = m_fDeltaMoveBase;
		break;

		case ACTION_MOUSECLICK:
			if(IsCurrentNode())
			{
				i = FindClickedImageNum();
				if(i==0) return ACTION_ACTIVATE;
				if(i==10000) return -1;
				m_bLockStatus = true;
				m_bDoMove = true;
				m_fCurrentDistance = 0.f;
				m_fMoveDistance = GetShiftDistance(i);
				if(i>0)	m_fDeltaMove = -m_fDeltaMoveBase*m_nSpeedMul;
				else	m_fDeltaMove = m_fDeltaMoveBase*m_nSpeedMul;
			}
		break;
		}
	}
	return -1;
}

bool CXI_SCROLLIMAGE::IsClick(int buttonID,long xPos,long yPos)
{
	if( xPos>=m_rect.left && xPos<=m_rect.right &&
		yPos>=m_rect.top  && yPos<=m_rect.bottom && m_bClickable && m_bUse )
	{
		return true;
	}

	return false;
}

void CXI_SCROLLIMAGE::ChangePosition( XYRECT &rNewPos )
{
	long nLeftOffset = rNewPos.left - m_rect.left;
	long nTopOffset = rNewPos.top - m_rect.top;
	long nRightOffset = rNewPos.right - m_rect.right;
	long nBottomOffset = rNewPos.bottom - m_rect.bottom;

	m_rAbsolutePosition.left += nLeftOffset;
	m_rAbsolutePosition.top += nTopOffset;
	m_rAbsolutePosition.right += nRightOffset;
	m_rAbsolutePosition.bottom += nBottomOffset;

	m_rect = rNewPos;

	m_pCenter.x = (m_rAbsolutePosition.right + m_rAbsolutePosition.left)/2;
	m_pCenter.y = (m_rAbsolutePosition.top + m_rAbsolutePosition.bottom)/2;
	m_pCenter.x = m_rAbsolutePosition.left + m_pntCenterOffset.x;
	m_pCenter.y = m_rAbsolutePosition.top + m_pntCenterOffset.y;

	ChangeDinamicParameters( 0.f );
}

void CXI_SCROLLIMAGE::SaveParametersToIni()
{
	char pcWriteParam[2048];

	INIFILE * pIni = api->fio->OpenIniFile( (char*)ptrOwner->m_sDialogFileName.GetBuffer() );
	if( !pIni ) {
		api->Trace( "Warning! Can`t open ini file name %s", ptrOwner->m_sDialogFileName.GetBuffer() );
		return;
	}

	// save position
	_snprintf( pcWriteParam, sizeof(pcWriteParam), "%d,%d,%d,%d", m_rAbsolutePosition.left, m_rAbsolutePosition.top, m_rAbsolutePosition.right, m_rAbsolutePosition.bottom );
	pIni->WriteString( m_nodeName, "position", pcWriteParam );

	delete pIni;
}

void CXI_SCROLLIMAGE::ChangeScroll(int nScrollItemNum)
{
    ATTRIBUTES * pAttr = _CORE_API->Entity_GetAttributeClass(&g_idInterface,m_nodeName);
    if(pAttr!=NULL)
    {
		// проверим может весь список надо менять
		if( nScrollItemNum==-1 || m_nListSize != long(pAttr->GetAttributeAsDword("NotUsed",0) + pAttr->GetAttributeAsDword("ListSize",0)) )
		{
			RefreshScroll();
			return;
		}

        int nScrollLastNum;
        if( nScrollItemNum!=-1 )
            nScrollLastNum = nScrollItemNum+1;
        else
        {
            nScrollLastNum = m_nListSize;
            nScrollItemNum = 0;
        }

		int  i,n;
        char sAttrName[256];
		char param[256];
        ATTRIBUTES * pAttribute;
		char * sStringName;
        for(i=nScrollItemNum; i<nScrollLastNum; i++)
        {
			m_Image[i].Clear(m_nSlotsQnt);

            sprintf(sAttrName,"pic%d",i+1);
            pAttribute=pAttr->GetAttributeClass(sAttrName);

            if(pAttribute!=null)
            {
				// set one string
				if(m_bUseOneString)
				{
					sStringName = pAttribute->GetAttribute("str1");
					if(sStringName!=NULL && sStringName[0]=='#')
					{
						m_Image[i].string1 = NEW char[strlen(sStringName)];
						if(m_Image[i].string1==NULL)
							_THROW("allocate memory error")
						strcpy(m_Image[i].string1,&(sStringName[1]));
					}
					else
						m_Image[i].str1 = pStringService->GetStringNum(sStringName);
				}

				// set two string
				if(m_bUseTwoString)
				{
					sStringName = pAttribute->GetAttribute("str2");
					if(sStringName!=NULL && sStringName[0]=='#')
					{
						m_Image[i].string2 = NEW char[strlen(sStringName)];
						if(m_Image[i].string2==NULL)
							_THROW("allocate memory error")
						strcpy(m_Image[i].string2,&(sStringName[1]));
					}
					else
						m_Image[i].str2 = pStringService->GetStringNum(sStringName);
				}

				// set pictures
				char * tmpStr;
				for(n=0; n<m_nSlotsQnt; n++)
				{
					sprintf(param,"name%d",n+1);
					tmpStr=pAttribute->GetAttribute(param);
					if(tmpStr!=null)
					{
						if( (m_Image[i].saveName[n]=NEW char[strlen(tmpStr)+1])==0 )
							{_THROW("allocate memory error");}
						strcpy(m_Image[i].saveName[n],tmpStr);
					}
					sprintf(param,"tex%d",n+1);
					m_Image[i].tex[n] = pAttribute->GetAttributeAsDword(param,-1);
					sprintf(param,"img%d",n+1);
					if(m_Image[i].tex[n]!=-1)
						m_Image[i].img[n] = pPictureService->GetImageNum(m_sGroupName[m_Image[i].tex[n]],pAttribute->GetAttribute(param));
					else	m_Image[i].img[n] = -1;
					sprintf(param,"spec%d",n+1);
					m_Image[i].bUseSpecTechnique[n] = pAttribute->GetAttributeAsDword(param,0)!=0;
				}
            }
        }
    }

	if(m_nCurImage>=m_nListSize-m_nNotUsedQuantity) m_nCurImage=m_nListSize-m_nNotUsedQuantity-1;
	if(m_nCurImage<0) m_nCurImage=0;
	ATTRIBUTES * pA = api->Entity_GetAttributeClass(&g_idInterface,m_nodeName);
	if(pA!=NULL) {
		pA->SetAttributeUseDword("current",m_nCurImage);
	}
	ChangeDinamicParameters(0);
}

void CXI_SCROLLIMAGE::DeleteImage(int imgNum)
{
	if(imgNum<0 || imgNum>=m_nListSize) return;
	if(m_nListSize<=m_nNotUsedQuantity) return;
	m_Image[imgNum].Release(m_nSlotsQnt);
	m_nListSize--;
	if(m_nListSize<=0)
	{
		PTR_DELETE(m_Image);
		return;
	}

	IMAGEDESCRIBE * pOldImgs = m_Image;
	m_Image = NEW IMAGEDESCRIBE[m_nListSize];
	if(m_Image==NULL)	{_THROW("memory allocate error")}
	if(imgNum>0)
		memcpy(m_Image,pOldImgs,imgNum*sizeof(IMAGEDESCRIBE));
	if(imgNum<m_nListSize)
		memcpy(&m_Image[imgNum],&pOldImgs[imgNum+1],(m_nListSize-imgNum)*sizeof(IMAGEDESCRIBE));
	delete pOldImgs;

	if(m_nCurImage>=m_nListSize-m_nNotUsedQuantity) m_nCurImage=m_nListSize-m_nNotUsedQuantity-1;
	if(m_nCurImage<0) m_nCurImage=0;
	ATTRIBUTES * pA = _CORE_API->Entity_GetAttributeClass(&g_idInterface,m_nodeName);
	if(pA!=NULL) {
		pA->SetAttributeUseDword("current",m_nCurImage);
	}
	ChangeDinamicParameters(0);
}

void CXI_SCROLLIMAGE::RefreshScroll()
{
	int i,n;
	char param[256];
	//UpdateTexturesGroup();

	PTR_DELETE(m_sSpecTechniqueName);

	for(i=0; i<m_nSlotsQnt; i++)
	{
		if(m_idBadPic[i]==-1) {
			TEXTURE_RELEASE(m_rs,m_idBadTexture[i]);
		}
		else m_idBadTexture[i] = -1;
		m_idBadPic[i] = -1;
	}

	if(m_Image!=NULL)
	{
		for(i=0; i<m_nListSize; i++) m_Image[i].Release(m_nSlotsQnt);
		PTR_DELETE(m_Image);
	}

	while(m_pScroll!=NULL)
	{
		SCROLLENTITY * rootScroll = m_pScroll;
		m_pScroll = m_pScroll->next;
		delete rootScroll;
	}

	// release old groups
	for(i=0;i<m_nGroupQuantity;i++)
	{
		PICTURE_TEXTURE_RELEASE(pPictureService,m_sGroupName[i],m_nGroupTex[i]);
		PTR_DELETE(m_sGroupName[i]);
	}
	PTR_DELETE(m_sGroupName);
	PTR_DELETE(m_nGroupTex);

	m_nListSize=0;	m_nNotUsedQuantity=0;

	ATTRIBUTES * pAttribute = api->Entity_GetAttributeClass(&g_idInterface,m_nodeName);
	if(pAttribute!=NULL)
	{
		// get special technique name and color
		m_dwSpecTechniqueARGB = pAttribute->GetAttributeAsDword("SpecTechniqueColor");
		char * sTechnique = pAttribute->GetAttribute("SpecTechniqueName");
		if(sTechnique!=NULL)
		{
			if( (m_sSpecTechniqueName=NEW char[strlen(sTechnique)+1])==NULL )
			{
				_THROW("Allocate memory error");
			}
			strcpy(m_sSpecTechniqueName,sTechnique);
		}
		// get images quantity
		m_nListSize = pAttribute->GetAttributeAsDword("ListSize",0);
		m_nNotUsedQuantity = pAttribute->GetAttributeAsDword("NotUsed",0);
		m_nListSize += m_nNotUsedQuantity;
		// create images array
		if(m_nListSize>0)
			m_Image = NEW IMAGEDESCRIBE[m_nListSize];
		else
		{
			m_Image = NULL;
			return;
		}
		m_nCurImage = pAttribute->GetAttributeAsDword("current",0);
		if( m_nCurImage>=m_nListSize ) m_nCurImage = m_nListSize-1;
		if( m_nCurImage<0 ) m_nCurImage = 0;

		// get textures
		ATTRIBUTES * pA = pAttribute->GetAttributeClass("ImagesGroup");
		if(pA!=NULL)
		{
			m_nGroupQuantity = pA->GetAttributesNum();
			if(m_nGroupQuantity!=0)
			{
				// set new groups
				m_nGroupTex = NEW long[m_nGroupQuantity];
				m_sGroupName = NEW char*[m_nGroupQuantity];
				if( m_nGroupTex==NULL || m_sGroupName==NULL )
				{
					_THROW("allocate memory error");
				}
				for(i=0;i<m_nGroupQuantity;i++)
				{
					char * stmp = pA->GetAttribute(i);
					if(stmp==NULL) continue;
					m_sGroupName[i] = NEW char[strlen(stmp)+1];
					if(m_sGroupName[i]==NULL)
					{
						_THROW("allocate memory error");
					}
					strcpy(m_sGroupName[i],stmp);
					m_nGroupTex[i] = pPictureService->GetTextureID(m_sGroupName[i]);
				}
			}
		}

		// get bad picture
		for(n=0; n<m_nSlotsQnt; n++)
		{
			char * sBadPict;
			sprintf(param,"BadPicture%d",n+1);
			if( (sBadPict=pAttribute->GetAttribute(param)) != NULL)
			{
				m_idBadTexture[n] = m_rs->TextureCreate(sBadPict);
				m_idBadPic[n] = -1;
			}
			else
			{
				sprintf(param,"BadTex%d",n+1);
				m_idBadTexture[n] = pAttribute->GetAttributeAsDword(param,-1);
				if(m_idBadTexture[n]>=0)
				{
					sprintf(param,"BadPic%d",n+1);
					m_idBadPic[n] = pPictureService->GetImageNum(m_sGroupName[m_idBadTexture[n]],pAttribute->GetAttribute(param));
				}
				else	m_idBadPic[n] = -1;
				if(m_idBadPic[n]==-1)	m_idBadTexture[n] = -1;
			}
		}

		// get all scroll entity
		for(i=0; i<m_nListSize; i++)
		{
			char attrName[256];
			char * sStringName;
			sprintf( attrName,"pic%d",i+1);
			ATTRIBUTES * pListEntity = pAttribute->GetAttributeClass(attrName);

			// Fill image descriptor by default value
			//------------------------------------------------------
			m_Image[i].str1 = m_Image[i].str2 = -1;
			m_Image[i].string1 = m_Image[i].string2 = null;
			if(m_nSlotsQnt>0) {
				m_Image[i].bUseSpecTechnique =  NEW bool[m_nSlotsQnt];
				m_Image[i].img = NEW long[m_nSlotsQnt];
				m_Image[i].ptex = NEW IDirect3DTexture8*[m_nSlotsQnt];
				m_Image[i].saveName = NEW char*[m_nSlotsQnt];
				m_Image[i].tex = NEW long[m_nSlotsQnt];
				if( !m_Image[i].bUseSpecTechnique || !m_Image[i].img ||
					!m_Image[i].ptex || !m_Image[i].saveName ||
					!m_Image[i].tex )
				{
					_THROW("allocate memory error");
				}
				for(n=0; n<m_nSlotsQnt; n++)
				{
					m_Image[i].bUseSpecTechnique[n] = false;
					m_Image[i].img[n] = -1;
					m_Image[i].ptex[n] = null;
					m_Image[i].saveName[n] = null;
					m_Image[i].tex[n] = -1;
				}
			} else {
				m_Image[i].bUseSpecTechnique = null;
				m_Image[i].img = null;
				m_Image[i].ptex = null;
				m_Image[i].saveName = null;
				m_Image[i].tex = null;
			}

			if(pListEntity!=NULL)
			{
				// set one string
				if(m_bUseOneString)
				{
					sStringName = pListEntity->GetAttribute("str1");
					if(sStringName!=NULL && sStringName[0]=='#')
					{
						m_Image[i].string1 = NEW char[strlen(sStringName)];
						if(m_Image[i].string1==NULL)
							_THROW("allocate memory error")
						strcpy(m_Image[i].string1,&(sStringName[1]));
					}
					else
						m_Image[i].str1 = pStringService->GetStringNum(sStringName);
				}

				// set two string
				if(m_bUseTwoString)
				{
					sStringName = pListEntity->GetAttribute("str2");
					if(sStringName!=NULL && sStringName[0]=='#')
					{
						m_Image[i].string2 = NEW char[strlen(sStringName)];
						if(m_Image[i].string2==NULL)
							_THROW("allocate memory error")
						strcpy(m_Image[i].string2,&(sStringName[1]));
					}
					else
						m_Image[i].str2 = pStringService->GetStringNum(sStringName);
				}

				// set pictures
				char * tmpStr;
				for(n=0; n<m_nSlotsQnt; n++)
				{
					sprintf(param,"name%d",n+1);
					tmpStr=pListEntity->GetAttribute(param);
					if(tmpStr!=null)
					{
						if( (m_Image[i].saveName[n]=NEW char[strlen(tmpStr)+1])==0 )
							{_THROW("allocate memory error");}
						strcpy(m_Image[i].saveName[n],tmpStr);
					}
					sprintf(param,"tex%d",n+1);
					m_Image[i].tex[n] = pListEntity->GetAttributeAsDword(param,-1);
					sprintf(param,"img%d",n+1);
					if(m_Image[i].tex[n]!=-1)
						m_Image[i].img[n] = pPictureService->GetImageNum(m_sGroupName[m_Image[i].tex[n]],pListEntity->GetAttribute(param));
					else	m_Image[i].img[n] = -1;
					sprintf(param,"spec%d",n+1);
					m_Image[i].bUseSpecTechnique[n] = pListEntity->GetAttributeAsDword(param,0)!=0;
				}
			}
		}
	}

	if(m_nCurImage>=m_nListSize-m_nNotUsedQuantity) m_nCurImage=m_nListSize-m_nNotUsedQuantity-1;
	if(m_nCurImage<0) m_nCurImage=0;
	ATTRIBUTES * pA = api->Entity_GetAttributeClass(&g_idInterface,m_nodeName);
	if(pA!=NULL) {
		pA->SetAttributeUseDword("current",m_nCurImage);
	}

	ChangeDinamicParameters(0);
}

int CXI_SCROLLIMAGE::FindClickedImageNum()
{
	int n;
	int i = 0;

	FXYPOINT fp = ptrOwner->GetMousePoint();
	for(SCROLLENTITY* pscroll=m_pScroll; pscroll!=null; pscroll = pscroll->next)
	{
		float flx = .5f*pscroll->fCurScale*m_ImageSize.x;
		float frx = pscroll->pCenter.x + flx;
		flx = pscroll->pCenter.x - flx;

		float fty = .5f*pscroll->fCurScale*m_ImageSize.y;
		float fby = pscroll->pCenter.y + fty;
		fty = pscroll->pCenter.y - fty;

		if(pscroll->pCenter.x >= m_pCenter.x) i++;
		else if(i>=0) i = -1;
			 else i--;

		if(fp.x>=flx && fp.x<=frx && fp.y>=fty && fp.y<=fby)
			break;
	}

	if(pscroll==null) return 10000;
	for(n=0; n<m_nSlotsQnt; n++)
	{
/*		if( m_Image[pscroll->imageNum].img[n]!=-1 ||
			m_Image[pscroll->imageNum].saveName!=null ) break;*/
		if( m_Image[pscroll->imageNum].tex[n]!=-1 ||
			m_Image[pscroll->imageNum].ptex[n]!=null ||
			m_Image[pscroll->imageNum].saveName[n]!=null ) break;
	}
	if(n>=m_nSlotsQnt) return 10000;
	if(i<0)	return i;
	if(i==0) return 0;
	return i-1;
}

int CXI_SCROLLIMAGE::GetRightQuantity()
{
	if(m_pScroll==null || m_Image==null) return 0;
	int q=0;
	for(SCROLLENTITY* pscr=m_pScroll; pscr!=null; pscr=pscr->next) q++;

	int i = m_pScroll->imageNum;
	for( int n=0; n<q; n++ )
	{
		for(int j=0; j<m_nSlotsQnt; j++)
		{
			if( m_Image[i].img[j]!=-1 || m_Image[i].saveName[j]!=null ) break;
		}
		if(j>=m_nSlotsQnt) break;
		i++;
		if(i>=m_nListSize) i=0;
		if(i==m_pScroll->imageNum) break;
	}
	if(n==0) return 0;
	return n-1;
}

int CXI_SCROLLIMAGE::GetLeftQuantity()
{
	if(m_pScroll==null || m_Image==null) return 0;
	int q=0;
	for(SCROLLENTITY* pscr=m_pScroll; pscr!=null; pscr=pscr->next) q++;

	int i = m_pScroll->imageNum;
	for( int n=0; n<q; n++ )
	{
		for(int j=0; j<m_nSlotsQnt; j++)
		{
			if( m_Image[i].img[j]!=-1 || m_Image[i].saveName[j]!=null ) break;
		}
		if(j>=m_nSlotsQnt) break;
		i--;
		if(i<0) i = m_nListSize-1;
		if(i==m_pScroll->imageNum) break;
	}
	if(n==0) return 0;
	return n-1;
}

float CXI_SCROLLIMAGE::GetShiftDistance(int shiftIdx)
{
	int i = 0;
	int n = shiftIdx<0 ? -shiftIdx : shiftIdx;

	bool bNoFindRight = true;
	float fright = (float)m_pCenter.x;
	SCROLLENTITY* pprev = m_pScroll;
	for(SCROLLENTITY* pscr=m_pScroll; pscr!=null; pscr=pscr->next)
	{
		if(pscr->pCenter.x >= m_pCenter.x)	pprev = pscr;
		else if(bNoFindRight)
		{
			bNoFindRight = false;
			fright = pprev->pCenter.x;
		}
		if(i==n) break;
		i++;
	}

	if(pscr!=null)
	{
		if(bNoFindRight)	fright -= pscr->pCenter.x;
		else fright = pscr->pCenter.x - fright;
		if(shiftIdx<0) return -fright;
		else return fright;
	}
	return 0.f;
}

XYRECT CXI_SCROLLIMAGE::GetCursorRect()
{
	XYRECT retVal;

	retVal.left = m_pCenter.x-m_ImageSize.x/2;
	retVal.top = m_pCenter.y-m_ImageSize.y/2;
	retVal.right = m_pCenter.x+m_ImageSize.x/2;
	retVal.bottom = m_pCenter.y+m_ImageSize.y/2;

	return retVal;
}

void CXI_SCROLLIMAGE::UpdateTexturesGroup()
{
	//m_sGroupName m_nGroupTex m_nGroupQuantity
	int i;

	char * *pPrevGroup = m_sGroupName;
	long * prevTex = m_nGroupTex;
	int nPrevQ = m_nGroupQuantity;

	// get textures
	ATTRIBUTES * pAttribute = api->Entity_GetAttributeClass(&g_idInterface,m_nodeName);
	if(pAttribute==null) return;

	ATTRIBUTES * pA = pAttribute->GetAttributeClass("ImagesGroup");
	if(pA!=NULL)
	{
		m_nGroupQuantity = pA->GetAttributesNum();
		if(m_nGroupQuantity!=0)
		{
			m_nGroupTex = NEW long[m_nGroupQuantity];
			m_sGroupName = NEW char*[m_nGroupQuantity];
			if( m_nGroupTex==NULL || m_sGroupName==NULL ) {
				_THROW("allocate memory error");
			}
			for(i=0;i<m_nGroupQuantity;i++)
			{
				char * stmp = pA->GetAttribute(i);
				if(stmp==NULL)
				{
					m_sGroupName[i] = NULL;
					m_nGroupTex[i] = -1;
					continue;
				}

				int itmp = FindTexGroupFromOld(pPrevGroup,stmp,nPrevQ);
				if(itmp!=-1)
				{
					m_sGroupName[i] = pPrevGroup[itmp];
					m_nGroupTex[i] = prevTex[itmp];
					pPrevGroup[itmp] = NULL;
					prevTex[itmp] = -1;
				}
				else
				{
					m_sGroupName[i] = NEW char[strlen(stmp)+1];
					if(m_sGroupName[i]==NULL) {
						_THROW("allocate memory error");
					}
					strcpy(m_sGroupName[i],stmp);
					m_nGroupTex[i] = pPictureService->GetTextureID(m_sGroupName[i]);
				}
			}

			// delete old groups
			for(i=0; i<nPrevQ; i++)
			{
				PICTURE_TEXTURE_RELEASE(pPictureService,pPrevGroup[i],prevTex[i]);
				PTR_DELETE(pPrevGroup[i]);
			}
			PTR_DELETE(pPrevGroup);
			PTR_DELETE(prevTex);
		}
	}
}

int CXI_SCROLLIMAGE::FindTexGroupFromOld(char * *pGroupList, char * groupName, int listSize)
{
	if(pGroupList==NULL || groupName==NULL) return -1;
	for(int i=0; i<listSize; i++)
	{
		if( pGroupList[i]!=NULL && stricmp(pGroupList[i],groupName)==0 )
			return i;
	}
	return -1;
}

void CXI_SCROLLIMAGE::IMAGEDESCRIBE::Release(int nQnt)
{
	PTR_DELETE(bUseSpecTechnique);
	PTR_DELETE(tex);
	PTR_DELETE(ptex);
	PTR_DELETE(img);

	for(int i=0; i<nQnt; i++)	PTR_DELETE(saveName[i]);
	PTR_DELETE(saveName);

	str1 = -1;
	str2 = -1;
	PTR_DELETE(string1);
	PTR_DELETE(string2);
}

void CXI_SCROLLIMAGE::IMAGEDESCRIBE::Clear(int nQnt)
{
	for(int i=0; i<nQnt; i++)
	{
		bUseSpecTechnique[i] = false;
		tex[i] = -1;
		ptex[i] = null;
		img[i] = -1;
		PTR_DELETE(saveName[i]);
	}

	str1 = -1;
	str2 = -1;
	PTR_DELETE(string1);
	PTR_DELETE(string2);
}

dword _cdecl CXI_SCROLLIMAGE::MessageProc(long msgcode, MESSAGE & message)
{
	switch(msgcode)
	{
	case 0: // разрешить/запретить показ рамки
		m_bShowBorder = message.Long()!=0;
	break;
	case 1: // установить новую текущую картинку
		{
			m_nCurImage = message.Long();
			if(m_nCurImage>=m_nListSize-m_nNotUsedQuantity) m_nCurImage=m_nListSize-m_nNotUsedQuantity-1;
			if(m_nCurImage<0) m_nCurImage=0;

			m_bLockStatus = false;
			m_bDoMove = false;

			// Set new current image
			ATTRIBUTES* tmpAttr = _CORE_API->Entity_GetAttributeClass(&g_idInterface,m_nodeName);
			if(tmpAttr!=NULL)
				tmpAttr->SetAttributeUseDword("current",m_nCurImage);

			ChangeDinamicParameters(0);
		}
	break;
	case 2:
		return GetMousePointedPictureNum();
	break;
	}

	return 0;
}

long CXI_SCROLLIMAGE::GetMousePointedPictureNum()
{
	FXYPOINT mp = ptrOwner->GetMousePoint();
	if( mp.x<m_rect.left || mp.x>m_rect.right ||
		mp.y<m_rect.top  || mp.y>m_rect.bottom ) {
		return -1;
	}

	float curXCenter = m_pScroll ? m_pScroll->pCenter.x : (m_rect.left+m_rect.right)/2;
	long n = 0;
	if( mp.x < curXCenter ) { // счет на уменьшение номера
		float fLeft = curXCenter - m_ImageSize.x*0.5f;
		for( n=0; mp.x<fLeft; n-- ) {
			fLeft -= m_lDelta + m_ImageSize.x;
		}
	} else { // счет на увеличение номера
		float fRight = curXCenter + m_ImageSize.x*0.5f;
		for( n=0; mp.x>fRight; n++ ) {
			fRight += m_lDelta + m_ImageSize.x;
		}
	}

	n += m_nCurImage;
	if( n >= m_nListSize ) n -= m_nListSize;
	if( n < 0 ) n += m_nListSize;
	return n;
}
