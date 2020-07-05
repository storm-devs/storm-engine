#include <stdio.h>
#include "xservice.h"
#include "..\\common_h\\dx8render.h"

#define ERROR_MUL	1.0f

static const char *	LISTS_INIFILE = "resource\\ini\\interfaces\\pictures.ini";

XSERVICE::XSERVICE()
{
	m_dwListQuantity = 0;
	m_dwImageQuantity = 0;
	m_pList = NULL;
	m_pImage = NULL;

	m_pRS = NULL;
}

XSERVICE::~XSERVICE()
{
}

void XSERVICE::Init(VDX8RENDER* pRS,long lWidth,long lHeight)
{
	m_pRS = pRS;

	// получим размер окна вывода
/*	D3DVIEWPORT8 vp;
	m_pRS->GetViewport(&vp);
	m_fWAdd = m_fWScale = (float)vp.Width/lWidth;
	m_fHAdd = m_fHScale = (float)vp.Height/lHeight;

	while(m_fWAdd>1.f) m_fWAdd-=1.f;
	while(m_fWAdd<0.f) m_fWAdd+=1.f;
	while(m_fHAdd>1.f) m_fHAdd-=1.f;
	while(m_fHAdd<0.f) m_fHAdd+=1.f;
	if(m_fWAdd>0.5f) m_fWAdd = 2.f-m_fWAdd*2.f;
	else	m_fWAdd *= 2.f;
	if(m_fHAdd>0.5f) m_fHAdd = 2.f-m_fHAdd*2.f;
	else	m_fHAdd *= 2.f;
	m_fWAdd *= ERROR_MUL;
	m_fHAdd *= ERROR_MUL;
*/
	m_fWAdd = 0.5f;
	m_fHAdd = 0.5f;

	LoadAllPicturesInfo();
}

long XSERVICE::GetTextureID(const char* sImageListName)
{
	if(sImageListName!=NULL)
	{
		for(int i=0; i<m_dwListQuantity; i++)
			if( !stricmp(m_pList[i].sImageListName,sImageListName) )
			{
				if(m_pList[i].textureQuantity<=0)
				{
					char sTexName[256];
					sprintf(sTexName,"INTERFACES\\%s",m_pList[i].sTextureName);
					m_pList[i].textureID = m_pRS->TextureCreate(sTexName);
					m_pList[i].textureQuantity = 1;
				} else
					m_pList[i].textureQuantity++;
				return m_pList[i].textureID;
			}
	}

	return -1;
}

long XSERVICE::FindGroup(const char* sImageListName)
{
	if( !sImageListName ) return -1;
	for(int n=0; n<m_dwListQuantity; n++)
		if( !stricmp(m_pList[n].sImageListName,sImageListName) )
			return n;
	return -1;
}

bool XSERVICE::ReleaseTextureID(const char* sImageListName)
{
	if(sImageListName==NULL) return false;

	for(int i=0; i<m_dwListQuantity; i++)
		if( !stricmp(m_pList[i].sImageListName,sImageListName) )
			if( --m_pList[i].textureQuantity == 0 )
			{
				m_pRS->TextureRelease(m_pList[i].textureID);
				return true;
			}

	return false;
}

bool XSERVICE::GetTexturePos(long pictureNum, FXYRECT &texRect)
{
	if(pictureNum>=0 && pictureNum<m_dwImageQuantity)
	{
        // find picture group
        for(int gn=0; gn<m_dwListQuantity; gn++)
            if(pictureNum>=m_pList[gn].pictureStart && pictureNum<m_pList[gn].pictureStart+m_pList[gn].pictureQuantity)
                break;
        if(gn<m_dwListQuantity)
        {
			texRect.left = (float)(m_pImage[pictureNum].pTextureRect.left+m_fWAdd)/m_pList[gn].textureWidth;
			texRect.right = (float)(m_pImage[pictureNum].pTextureRect.right-m_fWAdd)/m_pList[gn].textureWidth;
			texRect.top = (float)(m_pImage[pictureNum].pTextureRect.top+m_fHAdd)/m_pList[gn].textureHeight;
			texRect.bottom = (float)(m_pImage[pictureNum].pTextureRect.bottom-m_fHAdd)/m_pList[gn].textureHeight;
    		return true;
        }
	}

	ZeroMemory(&texRect,sizeof(texRect));
	return false;
}

bool XSERVICE::GetTexturePos(long pictureNum, XYRECT &texRect)
{
	if(pictureNum>=0 && pictureNum<m_dwImageQuantity)
	{
		memcpy(&texRect,&m_pImage[pictureNum].pTextureRect,sizeof(XYRECT));
    	return true;
	}

	ZeroMemory(&texRect,sizeof(texRect));
	return false;
}

bool XSERVICE::GetTexturePos(const char* sImageListName,const char* sImageName, FXYRECT &texRect)
{
	return GetTexturePos( GetImageNum(sImageListName, sImageName), texRect );
}

bool XSERVICE::GetTexturePos(const char* sImageListName,const char* sImageName, XYRECT &texRect)
{
	return GetTexturePos( GetImageNum(sImageListName, sImageName), texRect );
}

bool XSERVICE::GetTexturePos(int nTextureModify,long pictureNum, FXYRECT &texRect)
{
    FXYRECT rectTmp;

	if(pictureNum>=0 && pictureNum<m_dwImageQuantity)
	{
        // find picture group
        for(int gn=0; gn<m_dwListQuantity; gn++)
            if((pictureNum>=m_pList[gn].pictureStart) && (pictureNum<m_pList[gn].pictureStart+m_pList[gn].pictureQuantity))
                break;
        if(gn<m_dwListQuantity)
        {
			rectTmp.left = (float)m_pImage[pictureNum].pTextureRect.left;
			rectTmp.top = (float)m_pImage[pictureNum].pTextureRect.top;
			rectTmp.right = (float)m_pImage[pictureNum].pTextureRect.right;
			rectTmp.bottom = (float)m_pImage[pictureNum].pTextureRect.bottom;
            if(nTextureModify&TEXTURE_MODIFY_HORZFLIP)
            {
                float tmp = rectTmp.left+m_fWAdd*2.f;
                rectTmp.left = rectTmp.right-m_fWAdd*2.f;
                rectTmp.right = tmp;
            }
            if(nTextureModify&TEXTURE_MODIFY_VERTFLIP)
            {
                float tmp = rectTmp.top+m_fHAdd*2.f;
                rectTmp.top = rectTmp.bottom-m_fHAdd*2.f;
                rectTmp.bottom = tmp;
            }
			texRect.left = (rectTmp.left + m_fWAdd) / m_pList[gn].textureWidth;
			texRect.right = (float)(rectTmp.right - m_fWAdd) / m_pList[gn].textureWidth;
			texRect.top = (rectTmp.top + m_fHAdd) / m_pList[gn].textureHeight;
			texRect.bottom = (float)(rectTmp.bottom - m_fHAdd) / m_pList[gn].textureHeight;
    		return true;
        }
	}

	ZeroMemory(&texRect,sizeof(texRect));
	return false;
}

bool XSERVICE::GetTexturePos(int nTextureModify,const char* sImageListName, const char* sImageName, FXYRECT &texRect)
{
	return GetTexturePos( nTextureModify, GetImageNum(sImageListName, sImageName), texRect );
}

void XSERVICE::GetTextureCutForSize( const char* pcImageListName, const FXYPOINT &pntLeftTopUV, const XYPOINT &pntSize, long nSrcWidth, long nSrcHeight, FXYRECT &outUV )
{
	long n = FindGroup( pcImageListName );
	if( n>=0 )
	{
		if( nSrcWidth<m_pList[n].textureWidth ) nSrcWidth = m_pList[n].textureWidth;
		if( nSrcHeight<m_pList[n].textureHeight ) nSrcHeight = m_pList[n].textureHeight;
	}
	float fW = 1.f;
	if( nSrcWidth > 0 )
		fW = (float)pntSize.x / nSrcWidth + pntLeftTopUV.x;
	float fH = 1.f;
	if( nSrcHeight > 0 )
		fH = (float)pntSize.y / nSrcHeight + pntLeftTopUV.y;
	if( fW > 1.f ) fW = 1.f;
	if( fH > 1.f ) fH = 1.f;
	outUV.left = pntLeftTopUV.x;
	outUV.top = pntLeftTopUV.y;
	outUV.right = fW;
	outUV.bottom = fH;
}

void XSERVICE::LoadAllPicturesInfo()
{
	char	section[255];
	char	param[255];
	INIFILE * ini;

	// initialize ini file
	WIN32_FIND_DATA	wfd;
	HANDLE h = _CORE_API->fio->_FindFirstFile(LISTS_INIFILE,&wfd);
	if (INVALID_HANDLE_VALUE != h)
		_CORE_API->fio->_FindClose(h);
	ini = _CORE_API->fio->OpenIniFile((char*)LISTS_INIFILE);
	if(!ini) THROW("ini file not found!");

	m_dwListQuantity=0;
	m_dwImageQuantity=0;

	// calculate lists quantity
	if( ini->GetSectionName(section,sizeof(section)-1) )
		do m_dwListQuantity++; while(ini->GetSectionNameNext(section,sizeof(section)-1));
	// create list pointers array
	if(m_dwListQuantity>0)
	{
		m_pList = NEW IMAGELISTDESCR[m_dwListQuantity];
		if(m_pList==NULL)
			_THROW("memory allocate error")
	}

	// fill lists
	if( ini->GetSectionName(section,sizeof(section)-1) )
		for(int i=0; true; i++)
		{
			m_pList[i].textureQuantity = 0;
			m_pList[i].textureID = -1L;

			// get list name
			m_pList[i].sImageListName = NEW char[strlen(section)+1];
			strcpy(m_pList[i].sImageListName,section);
			// get texture name
			ini->ReadString(section,"sTextureName",param,sizeof(param)-1,"");
			m_pList[i].sTextureName = NEW char[strlen(param)+1];
			strcpy(m_pList[i].sTextureName,param);

			// get texture width & height
			m_pList[i].textureWidth = ini->GetLong(section,"wTextureWidth",1024);
			m_pList[i].textureHeight = ini->GetLong(section,"wTextureHeight",1024);

			m_pList[i].pictureStart = m_dwImageQuantity;
			// get pictures quantity
			m_pList[i].pictureQuantity = 0;
			if( ini->ReadString(section,"picture",param,sizeof(param)-1,"") )
				do m_pList[i].pictureQuantity++; while(ini->ReadStringNext(section,"picture",param,sizeof(param)-1));

			// resize image list
			PICTUREDESCR *oldpImage = m_pImage;
			m_pImage = NEW PICTUREDESCR[m_dwImageQuantity + m_pList[i].pictureQuantity];
			if(m_pImage==NULL)
				_THROW("allocate memory error")
			if(oldpImage!=NULL)
			{
				memcpy(m_pImage,oldpImage,m_dwImageQuantity*sizeof(PICTUREDESCR));
				delete oldpImage;
			}
			m_dwImageQuantity += m_pList[i].pictureQuantity;

			// set pictures
			char picName[sizeof(param)];
			ini->ReadString(section,"picture",param,sizeof(param)-1,"");
			for(int j=m_pList[i].pictureStart; j<m_dwImageQuantity; j++)
			{
				// get texture coordinates
				int nLeft,nTop,nRight,nBottom;

				sscanf(param,"%[^,],%d,%d,%d,%d", picName, &nLeft,	&nTop, &nRight,	&nBottom);
				m_pImage[j].pTextureRect.left   = nLeft;
				m_pImage[j].pTextureRect.top    = nTop;
				m_pImage[j].pTextureRect.right  = nRight;
				m_pImage[j].pTextureRect.bottom = nBottom;

				m_pImage[j].sPictureName = NEW char[strlen(picName)+1];
				strcpy(m_pImage[j].sPictureName,picName);

				ini->ReadStringNext(section,"picture",param,sizeof(param)-1);
			}

			if( !ini->GetSectionNameNext(section,sizeof(section)-1) ) break;
		}

	delete ini;
}

void XSERVICE::ReleaseAll()
{
	if(m_pList!=NULL)
	{
		for(int i=0; i<m_dwListQuantity; i++)
		{
			if(m_pList[i].textureQuantity!=0)
				m_pRS->TextureRelease(m_pList[i].textureID);

			if(m_pList[i].sImageListName!=NULL)
				delete m_pList[i].sImageListName;
			if(m_pList[i].sTextureName!=NULL)
				delete m_pList[i].sTextureName;
		}

		delete m_pList;
	}

	if(m_pImage!=NULL)
	{
		for(int i=0; i<m_dwImageQuantity; i++)
		{
			if(m_pImage[i].sPictureName!=NULL)
				delete m_pImage[i].sPictureName;
		}

		delete m_pImage;
	}

	m_dwListQuantity = 0;
	m_dwImageQuantity = 0;
}

long XSERVICE::GetImageNum(const char* sImageListName, const char* sImageName)
{
	long retVal=-1;

	if(sImageName!=NULL)
		if(sImageListName!=NULL)
		{
			for(int i=0; i<m_dwListQuantity; i++)
				if( !stricmp(m_pList[i].sImageListName,sImageListName) )
				{
					for(int j=m_pList[i].pictureStart; j<m_pList[i].pictureStart+m_pList[i].pictureQuantity; j++)
						if( !stricmp(m_pImage[j].sPictureName,sImageName) )
						{
							retVal=j; break;
						}
					break;
				}
		}
		else
		{
			for(int i=0; i<m_dwImageQuantity; i++)
				if( !stricmp(m_pImage[i].sPictureName,sImageName) )
				{
					retVal = i; break;
				}
		}

	return retVal;
}


//============================================================================
//	COMBINE STRINGS CLASS
//============================================================================

ComboString::ComboString()
{
}

ComboString::~ComboString()
{
}

void ComboString::AddToCombo(char * fontName, const XYPOINT & posStrStart, char * str)
{
}

void ComboString::AddToCombo(XYRECT posPic, char * picTexName, FXYRECT picUV)
{
}

void ComboString::PrintComboString(int comboStrID)
{
}

int ComboString::GetComboString(int align, int x,int y, int needWidth, int needHeight, int * allHeight, char * formatStr, char * fontlist)
{
	return -1;
}

void ComboString::ComboStringRelease(int comboStrID)
{
}
