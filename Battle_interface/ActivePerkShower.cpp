#include "ActivePerkShower.h"
#include "defines.h"

ActivePerkShower::ActivePerkShower()
{
	rs = null;

	m_idVBuf = -1;
	m_idIBuf = -1;

	m_nTextureQ = 0;
	m_pTexDescr = null;

	m_nShowPlaceQ = 0;
	m_pShowPlaces = null;

	m_nIShowQ = 0;
	m_pIconsList = null;
}

ActivePerkShower::~ActivePerkShower()
{
	ReleaseAll();
}

bool ActivePerkShower::Init()
{
	if( (rs=(VDX8RENDER *)_CORE_API->CreateService("dx8render")) == NULL )
	{
		_THROW("Can`t create render service");
	}

	if( AttributesPointer==null ) return false;
	if( !CreateTextures(AttributesPointer->GetAttributeClass("Textures")) ) return false;
	if( !CreateShowPlaces(AttributesPointer->GetAttributeClass("ShowParam")) )
	{
		ReleaseAll();
		return false;
	}
	if( !InitIconsList(AttributesPointer->FindAClass(AttributesPointer,"PerkList.list")) )
	{
		ReleaseAll();
		return false;
	}
	return true;
}

void ActivePerkShower::Execute(dword delta_time)
{
}

void ActivePerkShower::Realize(dword delta_time)
{
	if(m_pTexDescr==null) return;
	rs->MakePostProcess();

	for(int i=0; i<m_nTextureQ; i++)
	{
		if( m_pTexDescr[i].m_nPicsQ==0 ) continue;
		rs->TextureSet( 0, m_pTexDescr[i].m_idTexture );
		rs->DrawBuffer( m_idVBuf, sizeof(BI_ONETEXTURE_VERTEX), m_idIBuf,
			m_pTexDescr[i].m_nVertStart, m_pTexDescr[i].m_nPicsQ*4,
			m_pTexDescr[i].m_nIndxStart, m_pTexDescr[i].m_nPicsQ*2,
			"battle_rectangle");
	}
}

dword _cdecl ActivePerkShower::ProcessMessage(MESSAGE & message)
{
	switch(message.Long())
	{
	case MSG_ACTIVE_PERK_LIST_REFRESH:
		{
			char param[256];
			message.String(sizeof(param),param);
			ATTRIBUTES * pA = message.AttributePointer();
			if( stricmp(param,"add")==0 )	AddIconToList(pA);
			else if( stricmp(param,"del")==0 ) DelIconFromList(pA);
		}
	break;
	}
	return 0;
}

bool ActivePerkShower::CreateTextures(ATTRIBUTES * pATextureRoot)
{
	if(pATextureRoot==null) return false;

	int q = pATextureRoot->GetAttributesNum();
	if(q<=0) return false;

	m_pTexDescr = NEW _TEXTURE_DESCR[q];
	if(m_pTexDescr==null)
	{	THROW("allocate memory error");
	}

	for(int i=0; i<q; i++)
	{
		ATTRIBUTES * pA = pATextureRoot->GetAttributeClass(i);
		if(pA==null)
		{
			m_pTexDescr[i].m_idTexture = -1;
			m_pTexDescr[i].m_nCol = 1;
			m_pTexDescr[i].m_nRow = 1;
		}
		else
		{
			m_pTexDescr[i].m_idTexture = rs->TextureCreate(pA->GetAttribute("file"));
			m_pTexDescr[i].m_nCol = pA->GetAttributeAsDword("horz",1);
			m_pTexDescr[i].m_nRow = pA->GetAttributeAsDword("vert",1);
		}
		m_pTexDescr[i].m_nPicsQ = 0;
		m_pTexDescr[i].m_nVertStart = 0;
		m_pTexDescr[i].m_nIndxStart = 0;
	}

	m_nTextureQ = q;
	return true;
}

bool ActivePerkShower::CreateShowPlaces(ATTRIBUTES * pAPlacesRoot)
{
	if(pAPlacesRoot==null) return false;
	ATTRIBUTES * pAttr;

	m_nIconWidth = 64;
	m_nIconHeight = 64;
	pAttr = pAPlacesRoot->GetAttributeClass("IconSize");
	if(pAttr!=null)
	{
		m_nIconWidth = pAttr->GetAttributeAsDword("horz",64);
		m_nIconHeight = pAttr->GetAttributeAsDword("vert",64);
	}

	m_nSpaceHorz = 4;
	m_nSpaceVert = 4;
	pAttr = pAPlacesRoot->GetAttributeClass("IconSpace");
	if(pAttr!=null)
	{
		m_nSpaceHorz = pAttr->GetAttributeAsDword("horz",4);
		m_nSpaceVert = pAttr->GetAttributeAsDword("vert",4);
	}

	RECT rectBound;
	rectBound.left		= 488;
	rectBound.top		= 192;
	rectBound.right		= 624;
	rectBound.bottom	= 464;
	pAttr = pAPlacesRoot->GetAttributeClass("PosRect");
	if(pAttr!=null)
	{
		rectBound.left		= pAttr->GetAttributeAsDword("left", rectBound.left);
		rectBound.top		= pAttr->GetAttributeAsDword("top", rectBound.top);
		rectBound.right		= pAttr->GetAttributeAsDword("right", rectBound.right);
		rectBound.bottom	= pAttr->GetAttributeAsDword("bottom", rectBound.bottom);
	}

	int nHorzQ = (rectBound.right-rectBound.left) / (m_nIconWidth+m_nSpaceHorz);
	int nVertQ = (rectBound.bottom-rectBound.top) / (m_nIconHeight+m_nSpaceVert);
	if(nHorzQ<=0) nHorzQ = 1;
	if(nVertQ<=0) nVertQ = 1;

	m_nShowPlaceQ = nHorzQ*nVertQ;
	m_pShowPlaces = NEW _SHOW_PLACE[m_nShowPlaceQ];
	if(m_pShowPlaces==null)
	{	THROW("allocate memory error");
	}

	for(int ih=0; ih<nHorzQ; ih++)
	{
		for(int iv=0; iv<nVertQ; iv++)
		{
			int idx = iv + ih*nVertQ;
			m_pShowPlaces[idx].right = (float)(rectBound.right - ih*(m_nIconWidth+m_nSpaceHorz));
			m_pShowPlaces[idx].left = (float)(m_pShowPlaces[idx].right - m_nIconWidth);
			m_pShowPlaces[idx].top = (float)(rectBound.top + iv*(m_nIconHeight+m_nSpaceVert));
			m_pShowPlaces[idx].bottom = (float)(m_pShowPlaces[idx].top + m_nIconHeight);
		}
	}

	return InitCommonBuffers();
}

bool ActivePerkShower::InitIconsList(ATTRIBUTES * pAIconsRoot)
{
	if( pAIconsRoot==null ) return true;

	int q = pAIconsRoot->GetAttributesNum();
	m_nIShowQ = q;
	if(m_nIShowQ==0) return true;
	m_pIconsList = NEW _PICTURE_DESCR[q];
	if(m_pIconsList==null)
	{	THROW("allocate memory error");
	}
	for(int i=0; i<q; i++)
	{
		m_pIconsList[i].m_nPicNum = 0;
		m_pIconsList[i].m_nPicTexIdx = 0;
		ATTRIBUTES * pA = pAIconsRoot->GetAttributeClass(i); 
		if(pA!=null)
		{
			m_pIconsList[i].m_nPicNum = pA->GetAttributeAsDword("texture",0);
			m_pIconsList[i].m_nPicTexIdx = pA->GetAttributeAsDword("pic_idx",0);
		}
	}

	FillVIBuffers();
	return true;
}

void ActivePerkShower::AddIconToList(ATTRIBUTES * pAItemDescr)
{
	if(pAItemDescr==null) return;
	int picNum = pAItemDescr->GetAttributeAsDword("pic_idx");
	int texNum = pAItemDescr->GetAttributeAsDword("texture");

	for(int i=0; i<m_nIShowQ; i++)
	{
		if(texNum==m_pIconsList[i].m_nPicTexIdx && picNum==m_pIconsList[i].m_nPicNum)
			return; // уже есть такая абилити
	}

	m_nIShowQ++;
	if(m_pIconsList==null)	m_pIconsList = NEW _PICTURE_DESCR[m_nIShowQ];
	else
	{
		_PICTURE_DESCR * old_pIconsList = m_pIconsList;
		m_pIconsList = NEW _PICTURE_DESCR[m_nIShowQ];
		if(m_pIconsList!=null)
		{	memcpy(m_pIconsList,old_pIconsList,sizeof(_PICTURE_DESCR)*(m_nIShowQ-1));
		}
		delete old_pIconsList;
	}
	if(m_pIconsList==null)	{THROW("allocate memory error");}
	m_pIconsList[m_nIShowQ-1].m_nPicTexIdx = texNum;
	m_pIconsList[m_nIShowQ-1].m_nPicNum = picNum;

	FillVIBuffers();
}

void ActivePerkShower::DelIconFromList(ATTRIBUTES * pAIconDescr)
{
	if(pAIconDescr==null) return;
	int picNum = pAIconDescr->GetAttributeAsDword("pic_idx");
	int texNum = pAIconDescr->GetAttributeAsDword("texture");

	int del_idx = m_nIShowQ;
	for(int i=0; i<m_nIShowQ; i++)
	{
		if(i>del_idx)
		{
			m_pIconsList[i-1].m_nPicTexIdx = m_pIconsList[i].m_nPicTexIdx;
			m_pIconsList[i-1].m_nPicNum = m_pIconsList[i].m_nPicNum;
			continue;
		}
		if(texNum==m_pIconsList[i].m_nPicTexIdx && picNum==m_pIconsList[i].m_nPicNum)
			del_idx = i;
	}
	if(del_idx==m_nIShowQ) return;

	m_nIShowQ--;
	FillVIBuffers();
}

void ActivePerkShower::FillVIBuffers()
{
	int pi,ti,start_idx;

	BI_ONETEXTURE_VERTEX * pvb = (BI_ONETEXTURE_VERTEX*)rs->LockVertexBuffer(m_idVBuf);
	if(pvb==null) return;

	start_idx = 0;
	for(ti=0; ti<m_nTextureQ; ti++)
	{
		m_pTexDescr[ti].m_nPicsQ = 0;
		m_pTexDescr[ti].m_nVertStart = start_idx*4;
		for(pi=0; pi<m_nIShowQ && start_idx<m_nShowPlaceQ; pi++)
		{
			if(m_pIconsList[pi].m_nPicTexIdx!=ti) continue;
			m_pTexDescr[ti].m_nPicsQ++;
			FillRectData( &pvb[start_idx*4], m_pShowPlaces[pi],
				GetTextureRect(ti,m_pIconsList[pi].m_nPicNum) );
			start_idx++;
		}
	}

	rs->UnLockVertexBuffer(m_idVBuf);
}

void ActivePerkShower::FillRectData(void * vbuf,FRECT & rectPos,FRECT & rectTex)
{
	if(vbuf==null) return;
	BI_ONETEXTURE_VERTEX * ptmp = (BI_ONETEXTURE_VERTEX*)vbuf;
	ptmp[0].pos.x = rectPos.left;		ptmp[0].pos.y = rectPos.top;
	ptmp[1].pos.x = rectPos.left;		ptmp[1].pos.y = rectPos.bottom;
	ptmp[2].pos.x = rectPos.right;		ptmp[2].pos.y = rectPos.top;
	ptmp[3].pos.x = rectPos.right;		ptmp[3].pos.y = rectPos.bottom;

	ptmp[0].tu = rectTex.left;		ptmp[0].tv = rectTex.top;
	ptmp[1].tu = rectTex.left;		ptmp[1].tv = rectTex.bottom;
	ptmp[2].tu = rectTex.right;		ptmp[2].tv = rectTex.top;
	ptmp[3].tu = rectTex.right;		ptmp[3].tv = rectTex.bottom;
}

FRECT ActivePerkShower::GetTextureRect(int textIdx, int picIdx)
{
	FRECT retRect;

	int vIdx = picIdx / m_pTexDescr[textIdx].m_nCol;
	int hIdx = picIdx - vIdx*m_pTexDescr[textIdx].m_nCol;

	retRect.left = (float)hIdx/m_pTexDescr[textIdx].m_nCol;
	retRect.top = (float)vIdx/m_pTexDescr[textIdx].m_nRow;
	retRect.right = (float)(hIdx+1.f)/m_pTexDescr[textIdx].m_nCol;
	retRect.bottom = (float)(vIdx+1.f)/m_pTexDescr[textIdx].m_nRow;

	return retRect;
}

bool ActivePerkShower::InitCommonBuffers()
{
	m_idVBuf = rs->CreateVertexBuffer(BI_ONETEX_VERTEX_FORMAT,m_nShowPlaceQ*4*sizeof(BI_ONETEXTURE_VERTEX),D3DUSAGE_WRITEONLY);
	m_idIBuf = rs->CreateIndexBuffer(m_nShowPlaceQ*6*2);
	if(m_idIBuf==-1 || m_idVBuf==-1) return false;

	int i;
	WORD * pibuf = (WORD*)rs->LockIndexBuffer(m_idIBuf);
	for(i=0; i<m_nShowPlaceQ; i++)
	{
		pibuf[i*6+0] = i*4 + 0;
		pibuf[i*6+1] = i*4 + 1;
		pibuf[i*6+2] = i*4 + 2;
		pibuf[i*6+3] = i*4 + 2;
		pibuf[i*6+4] = i*4 + 1;
		pibuf[i*6+5] = i*4 + 3;
	}
	rs->UnLockIndexBuffer(m_idIBuf);

	BI_ONETEXTURE_VERTEX * pvbuf = (BI_ONETEXTURE_VERTEX*)rs->LockVertexBuffer(m_idVBuf);
	for(i=0; i<m_nShowPlaceQ*4; i++)
	{
		pvbuf[i].pos.z = 1.f;
		pvbuf[i].w = .5f;
	}
	rs->UnLockVertexBuffer(m_idVBuf);

	return true;
}

void ActivePerkShower::ReleaseAll()
{
	int i;

	VERTEX_BUFFER_RELEASE(rs,m_idVBuf);
	INDEX_BUFFER_RELEASE(rs,m_idIBuf);

	for(i=0; i<m_nTextureQ; i++)	TEXTURE_RELEASE(rs,m_pTexDescr[i].m_idTexture);
	DELETE(m_pTexDescr);
	m_nTextureQ = 0;

	DELETE(m_pShowPlaces);
	m_nShowPlaceQ = 0;

	DELETE(m_pIconsList);
	m_nIShowQ = 0;
}
