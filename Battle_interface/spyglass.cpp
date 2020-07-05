#include "..\common_h\matrix.h"
#include "spyglass.h"
#include "..\engine\program\battle_interface\msg_control.h"

// для быстрой записи определим:
#define ITEXTURE_RELEASE(rs,id)	if(rs!=NULL && id!=-1) {rs->TextureRelease(id); id=-1;}
#define IFONT_RELEASE(rs,id)	if(rs!=NULL && id!=-1) {rs->UnloadFont(id); id=-1;}
#define IPTR_DELETE(ptr)	if(ptr!=NULL) {delete ptr; ptr=NULL;}
#define IVERTEXBUFFER_RELEASE(rs,id)	if(rs!=NULL && id!=-1) {rs->ReleaseVertexBuffer(id); id=-1;}
#define IINDEXBUFFER_RELEASE(rs,id)	if(rs!=NULL && id!=-1) {rs->ReleaseIndexBuffer(id); id=-1;}

// определим вертексы
#define ISG_VERTEX_FORMAT		(D3DFVF_XYZRHW|D3DFVF_TEX1|D3DFVF_TEXTUREFORMAT2)
struct ISG_VERTEX
{
	CVECTOR pos;
	float w;
	float tu, tv;
};

bool GetFRectFromAttribute ( ATTRIBUTES * pA, FRECT & fr )
{
	if(pA==NULL) return false;
	fr.left = pA->GetAttributeAsFloat("left",0);
	fr.top = pA->GetAttributeAsFloat("top",0);
	fr.right = pA->GetAttributeAsFloat("right",0);
	fr.bottom = pA->GetAttributeAsFloat("bottom",0);
	return true;
}

bool SetVertexToBuffer ( ISG_VERTEX * pv, FRECT & frPos, FRECT & frTex )
{
	if(pv==NULL) return false;
	pv[0].pos.x = frPos.left;		pv[0].pos.y = frPos.top;		pv[0].pos.z = 1.f;
	pv[0].tu = frTex.left;			pv[0].tv = frTex.top;			pv[0].w = .5f;
	pv[1].pos.x = frPos.left;		pv[1].pos.y = frPos.bottom;		pv[1].pos.z = 1.f;
	pv[1].tu = frTex.left;			pv[1].tv = frTex.bottom;		pv[1].w = .5f;
	pv[2].pos.x = frPos.right;		pv[2].pos.y = frPos.top;		pv[2].pos.z = 1.f;
	pv[2].tu = frTex.right;			pv[2].tv = frTex.top;			pv[2].w = .5f;
	pv[3].pos.x = frPos.right;		pv[3].pos.y = frPos.bottom;		pv[3].pos.z = 1.f;
	pv[3].tu = frTex.right;			pv[3].tv = frTex.bottom;		pv[3].w = .5f;
	return true;
}

void SetVertexUV ( ISG_VERTEX * pv, FRECT & frTex )
{
	if(pv==NULL) return;
	pv[0].tu = frTex.left;			pv[0].tv = frTex.top;
	pv[1].tu = frTex.left;			pv[1].tv = frTex.bottom;
	pv[2].tu = frTex.right;			pv[2].tv = frTex.top;
	pv[3].tu = frTex.right;			pv[3].tv = frTex.bottom;
}

ISPYGLASS::ISPYGLASS()
{
	m_bVisible = false;

	m_frNationUV = NULL;
	m_nationQuantity = 0;

	m_idBackTex = -1;
	m_idIconTex = -1;
	m_idNationsTex = -1;

	m_idFontOne = -1;
	m_idFontTwo = -1;

	m_fShipNameScale = m_fShipTypeScale = 1.f;

	m_sShipName = NULL;
	m_sShipType = NULL;
	m_nShipHull = -1;
	m_nShipSail = -1;
	m_nShipCrew = -1;
	m_fShipSpeed = -1.f;
	m_fSailTo   = -1.f; //boal
	m_nShipCannons = -1;
	m_nShipCharge = -1;

	m_fFadeSpeed = 0.f;
	m_fCurAlpha = 255.f;
	m_bFadeOut = true;

	m_idVBuf = -1;
	m_idIBuf = -1;
	m_nPrimitiveQuantity = 0;
	rs=NULL;
}

ISPYGLASS::~ISPYGLASS()
{
	AllRelease();
}

void ISPYGLASS::AllRelease()
{
	m_bVisible = false;

	ITEXTURE_RELEASE(rs,m_idBackTex)
	ITEXTURE_RELEASE(rs,m_idIconTex)
	ITEXTURE_RELEASE(rs,m_idNationsTex)

	IFONT_RELEASE(rs,m_idFontOne)
	IFONT_RELEASE(rs,m_idFontTwo)

	IPTR_DELETE(m_sShipName)
	IPTR_DELETE(m_sShipType)
	IPTR_DELETE(m_frNationUV)

	m_nationQuantity = 0;
	m_nShipHull = -1;
	m_nShipSail = -1;
	m_nShipCrew = -1;
	m_fShipSpeed = -1.f;
	m_fSailTo = -1.f; //boal
	m_nShipCannons = -1;
	m_nShipCharge = -1;

	IVERTEXBUFFER_RELEASE(rs,m_idVBuf)
	IINDEXBUFFER_RELEASE(rs,m_idIBuf)
	m_nPrimitiveQuantity = 0;
}

bool ISPYGLASS::Init()
{
	if( (rs=(VDX8RENDER *)api->CreateService("dx8render")) == NULL )
	{
		_THROW("Can`t create render service");
	}
	if( SetParameters() ) return true;
	AllRelease();
	return false;
}

bool ISPYGLASS::SetParameters()
{
	int i;

	ATTRIBUTES * pAttr = api->Entity_GetAttributeClass(&GetID(),"parameters");
	if(pAttr==NULL)
	{
		api->Trace("Not found attributes \"parameters\" into object ISPYGLASS");
		return false;
	}
	ATTRIBUTES * pA;

	// Скорость фэйда
	m_fFadeSpeed = pAttr->GetAttributeAsFloat("FadeTime");
	if(m_fFadeSpeed>0.001f) m_fFadeSpeed = 1.f / m_fFadeSpeed;
	else m_fFadeSpeed = -1.f;
	m_fCurAlpha = 255.f;

	// Создадим текстуры
	char * texName = pAttr->GetAttribute("BackTextureName");
	if(texName!=NULL)	m_idBackTex = rs->TextureCreate(texName);
	else m_idBackTex = -1;
	texName = pAttr->GetAttribute("IconsTextureName");
	if(texName!=NULL)	m_idIconTex = rs->TextureCreate(texName);
	else m_idIconTex = -1;
	texName = pAttr->GetAttribute("NationsTextureName");
	if(texName!=NULL)	m_idNationsTex = rs->TextureCreate(texName);
	else m_idNationsTex = -1;

	// Создадим буферы
	m_idVBuf = rs->CreateVertexBuffer(ISG_VERTEX_FORMAT,4*(1+1+6) * sizeof(ISG_VERTEX),D3DUSAGE_WRITEONLY); // подложка, 
	m_nPrimitiveQuantity = 5;
	m_idIBuf = rs->CreateIndexBuffer(8 * 6*2); // хотя одновременно рисуется не 8, а только два прямоугольника
	if(m_idVBuf==-1 || m_idIBuf==-1)
	{
		api->Trace("Can`t create vertex or index buffers.");
		return false;
	}

	// позиция подложки
	FRECT gfr;
	if( !GetFRectFromAttribute ( pAttr->GetAttributeClass("globalpos"), gfr ) )
	{
		api->Trace("Not found attributes \"parameters\\globalpos\" into object ISPYGLASS");
		return false;
	}

	// размер иконок
	FPOINT fpIconSize;
	pA = pAttr->GetAttributeClass("iconsize");
	if(pA!=NULL)
	{
		fpIconSize.x = (float)pA->GetAttributeAsDword("x",0);
		fpIconSize.y = (float)pA->GetAttributeAsDword("y",0);
	}
	else
	{
		api->Trace("Not found attributes \"parameters\\iconsize\" into object ISPYGLASS");
		return false;
	}

	// позиция прорисовки текста
	pA = pAttr->GetAttributeClass("ShipText");
	if(pA!=NULL)
	{
		m_idFontOne = rs->LoadFont(pA->GetAttribute("font"));
		m_fShipNameScale = m_fShipTypeScale = m_fFontOneScale = pA->GetAttributeAsFloat("scale",1.f);
		m_nLeftTextShipData = pA->GetAttributeAsDword("left",0);
		m_nTopTextShipName = pA->GetAttributeAsDword("topName",0);
		m_nTopTextShipType = pA->GetAttributeAsDword("topType",0);
	}
	else
	{
		api->Trace("Not found attributes \"parameters\\shiptextpos\" into object ISPYGLASS");
		return false;
	}

	// позиции для вывода данных
	pA = pAttr->GetAttributeClass("DigitData");
	if(pA!=NULL)
	{
		m_idFontTwo = rs->LoadFont(pA->GetAttribute("font"));
		m_fFontTwoScale = pA->GetAttributeAsFloat("scale",1.f);
		m_nTopDigitData = pA->GetAttributeAsDword("topOffset",0);
		m_nLeftTextPos[0] = pA->GetAttributeAsDword("leftHull",0);
		m_nLeftTextPos[1] = pA->GetAttributeAsDword("leftSail",0);
		m_nLeftTextPos[2] = pA->GetAttributeAsDword("leftCrew",0);
		m_nLeftTextPos[3] = pA->GetAttributeAsDword("leftSpeed",0);
		m_nLeftTextPos[4] = pA->GetAttributeAsDword("leftCannons",0);
	}
	else
	{
		api->Trace("Not found attributes \"parameters\\DigitData\" into object ISPYGLASS");
		return false;
	}

	// позиции для иконок
	long	nTopIcon;
	long	nIconPos[7];
	pA = pAttr->GetAttributeClass("Icons");
	if(pA!=NULL)
	{
		nTopIcon = pA->GetAttributeAsDword("topOffset",0);
		nIconPos[0] = pA->GetAttributeAsDword("leftNation",0);
		nIconPos[1] = pA->GetAttributeAsDword("leftHull",0);
		nIconPos[2] = pA->GetAttributeAsDword("leftSail",0);
		nIconPos[3] = pA->GetAttributeAsDword("leftCrew",0);
		nIconPos[4] = pA->GetAttributeAsDword("leftSpeed",0);
		nIconPos[5] = pA->GetAttributeAsDword("leftCannons",0);
		nIconPos[6] = pA->GetAttributeAsDword("leftCharge",0);
	}
	else
	{
		api->Trace("Not found attributes \"parameters\\Icons\" into object ISPYGLASS");
		return false;
	}

	// Размер области под текст
	m_fStringMaxWidth = float(nIconPos[1] - m_nLeftTextShipData) - 4;

	// заполним индекс буфер
	WORD *pibuf = (WORD*)rs->LockIndexBuffer(m_idIBuf);
	if(pibuf==NULL) return false;
	for(i=0; i<8; i++)
	{
		pibuf[i*6+0] = i*4;
		pibuf[i*6+1] = i*4+1;
		pibuf[i*6+2] = i*4+2;
		pibuf[i*6+3] = i*4+1;
		pibuf[i*6+4] = i*4+3;
		pibuf[i*6+5] = i*4+2;
	}
	rs->UnLockIndexBuffer(m_idIBuf);

	// текстура иконок
	pA = pAttr->GetAttributeClass("IconsTexture");
	if(pA!=NULL)
	{
		ISG_VERTEX * pv = (ISG_VERTEX*)rs->LockVertexBuffer(m_idVBuf);
		if(pv==NULL) return false;

		FRECT pos,fr;
		pos.bottom = (pos.top = (float)nTopIcon) + fpIconSize.y;

		fr.left = 0.f;		fr.top = 0.f;
		fr.right = 1.f;		fr.bottom = 1.f;
		SetVertexToBuffer(pv,gfr,fr);

		ATTRIBUTES *pANations = pA->GetAttributeClass("nations");
		if(pANations!=NULL)
		{
			m_nationQuantity = pANations->GetAttributesNum();
			if( m_nationQuantity!=0 && (m_frNationUV=NEW FRECT[m_nationQuantity])==NULL )
				{_THROW("allocate memory error");}
			for(int in=0; in<m_nationQuantity; in++)
				if( !GetFRectFromAttribute(pANations->GetAttributeClass(in),m_frNationUV[in]) )
					m_frNationUV[in] = fr;
		}
		else	m_nationQuantity = 0;
		pos.right = (pos.left = (float)nIconPos[0]) + fpIconSize.x;
		SetVertexToBuffer ( &pv[4], pos, fr );

		pos.right = (pos.left = (float)nIconPos[1]) + fpIconSize.x;
		if ( GetFRectFromAttribute(pA->GetAttributeClass("hull"),fr) )	SetVertexToBuffer ( &pv[8], pos, fr );

		pos.right = (pos.left = (float)nIconPos[2]) + fpIconSize.x;
		if ( GetFRectFromAttribute(pA->GetAttributeClass("sail"),fr) )	SetVertexToBuffer ( &pv[12], pos, fr );

		pos.right = (pos.left = (float)nIconPos[3]) + fpIconSize.x;
		if ( GetFRectFromAttribute(pA->GetAttributeClass("crew"),fr) )	SetVertexToBuffer ( &pv[16], pos, fr );

		pos.right = (pos.left = (float)nIconPos[4]) + fpIconSize.x;
		if ( GetFRectFromAttribute(pA->GetAttributeClass("speed"),fr) )	SetVertexToBuffer ( &pv[20], pos, fr );

		pos.right = (pos.left = (float)nIconPos[5]) + fpIconSize.x;
		if ( GetFRectFromAttribute(pA->GetAttributeClass("cannons"),fr) )	SetVertexToBuffer ( &pv[24], pos, fr );

		pos.right = (pos.left = (float)nIconPos[6]) + fpIconSize.x;
		if ( GetFRectFromAttribute(pA->GetAttributeClass("charge0"),m_frChargeUV[0]) )	SetVertexToBuffer ( &pv[28], pos, m_frChargeUV[0] );
		GetFRectFromAttribute(pA->GetAttributeClass("charge1"),m_frChargeUV[1]);
		GetFRectFromAttribute(pA->GetAttributeClass("charge2"),m_frChargeUV[2]);
		GetFRectFromAttribute(pA->GetAttributeClass("charge3"),m_frChargeUV[3]);

		rs->UnLockVertexBuffer(m_idVBuf);
	}
	else
	{
		api->Trace("Not found attributes \"parameters\\IconsTexture\" into object ISPYGLASS");
		return false;
	}

	return true;
}

void ISPYGLASS::Execute(dword delta_time)
{
	if(m_bVisible)
	{
		if(m_fFadeSpeed<0) m_fCurAlpha = m_bFadeOut ? 256.f:0.f;
		else
			if(m_bFadeOut)
			{	if(m_fCurAlpha<255.9f) m_fCurAlpha += delta_time*.256f*m_fFadeSpeed;	}
			else
			{
				if(m_fCurAlpha>0) m_fCurAlpha -= delta_time*.256f*m_fFadeSpeed;
				if(m_fCurAlpha<=0) m_bVisible=false;
			}
	}
}

void ISPYGLASS::Realize(dword delta_time)
{
	if(m_bVisible)
	{
		CMatrix matw;
		rs->SetTransform(D3DTS_WORLD,(D3DXMATRIX*)&matw);
		DWORD dwcolor = 0xFFFFFFFF;
		if(m_fCurAlpha<256.9f) dwcolor = (((DWORD)m_fCurAlpha)<<24) | 0x00FFFFFF;
		rs->SetRenderState(D3DRS_TEXTUREFACTOR,dwcolor);
		// подложка
		rs->TextureSet(0,m_idBackTex);
		rs->DrawBuffer(m_idVBuf, sizeof(ISG_VERTEX), m_idIBuf, 0,4, 0,2, "spyglass_interface");
		// нации
		if(m_nShipNation>=0)
		{
			rs->TextureSet(0,m_idNationsTex);
			rs->DrawBuffer(m_idVBuf, sizeof(ISG_VERTEX), m_idIBuf, 4,4, 0,2, "spyglass_interface");
		}
		// иконки
		rs->TextureSet(0,m_idIconTex);
		if(m_nShipHull!=-1) //1
		{
			rs->DrawBuffer(m_idVBuf, sizeof(ISG_VERTEX), m_idIBuf, 8, 4, 0,2, "spyglass_interface");
		}
		if(m_nShipSail!=-1) //2
		{
			rs->DrawBuffer(m_idVBuf, sizeof(ISG_VERTEX), m_idIBuf, 12, 4, 0,2, "spyglass_interface");
		}
		if(m_nShipCrew!=-1) //3
		{
			rs->DrawBuffer(m_idVBuf, sizeof(ISG_VERTEX), m_idIBuf, 16, 4, 0,2, "spyglass_interface");
		}
		if(m_fShipSpeed!=-1) //4
		{
			rs->DrawBuffer(m_idVBuf, sizeof(ISG_VERTEX), m_idIBuf, 20, 4, 0,2, "spyglass_interface");
		}

		if(m_nShipCannons!=-1) //5
		{
			rs->DrawBuffer(m_idVBuf, sizeof(ISG_VERTEX), m_idIBuf, 24, 4, 0,2, "spyglass_interface");
		}
		if(m_nShipCharge!=-1) //6
		{
			rs->DrawBuffer(m_idVBuf, sizeof(ISG_VERTEX), m_idIBuf, 28, 4, 0,2, "spyglass_interface");
		}
		
		if(m_fSailTo!=-1) //7 boal
		{
			rs->DrawBuffer(m_idVBuf, sizeof(ISG_VERTEX), m_idIBuf, 32, 4, 0,2, "spyglass_interface");
		}
		// тексты
		if(m_sShipName!=NULL) rs->ExtPrint (m_idFontOne, dwcolor,0, ALIGN_LEFT,true,m_fShipNameScale, 0,0, m_nLeftTextShipData,m_nTopTextShipName,"%s",m_sShipName);
		if(m_sShipType!=NULL) rs->ExtPrint (m_idFontOne, dwcolor,0, ALIGN_LEFT,true,m_fShipTypeScale, 0,0, m_nLeftTextShipData,m_nTopTextShipType,"%s",m_sShipType);
		if(m_nShipHull!=-1) rs->ExtPrint (m_idFontTwo, dwcolor,0, ALIGN_LEFT,true,m_fFontTwoScale, 0,0, m_nLeftTextPos[0],m_nTopDigitData,"%d%%",m_nShipHull);
		if(m_nShipSail!=-1) rs->ExtPrint (m_idFontTwo, dwcolor,0, ALIGN_LEFT,true,m_fFontTwoScale, 0,0, m_nLeftTextPos[1],m_nTopDigitData,"%d%%",m_nShipSail);
		if(m_nShipCrew!=-1) rs->ExtPrint (m_idFontTwo, dwcolor,0, ALIGN_LEFT,true,m_fFontTwoScale, 0,0, m_nLeftTextPos[2],m_nTopDigitData,"%d",m_nShipCrew);
		if(m_fShipSpeed!=-1) rs->ExtPrint (m_idFontTwo, dwcolor,0, ALIGN_LEFT,true,m_fFontTwoScale, 0,0, m_nLeftTextPos[3],m_nTopDigitData,"%.1f",m_fShipSpeed);
		if(m_nShipCannons!=-1) rs->ExtPrint (m_idFontTwo, dwcolor,0, ALIGN_LEFT,true,m_fFontTwoScale, 0,0, m_nLeftTextPos[4],m_nTopDigitData,"%d",m_nShipCannons);
		if(m_fSailTo!=-1)   rs->ExtPrint (m_idFontTwo, dwcolor,0, ALIGN_LEFT,true,m_fFontTwoScale, 0,0, m_nLeftTextPos[5],m_nTopDigitData,"%.1f",m_fSailTo);
	}
}

dword _cdecl ISPYGLASS::ProcessMessage(MESSAGE & message)
{
	switch(message.Long())
	{
	case MSG_ISG_VISIBLE:
		SetVisible(0!=message.Long());
	break;

	case MSG_ISG_UPDATE:
		{
			float ftmp;
			IPTR_DELETE(m_sShipName);
			IPTR_DELETE(m_sShipType);
			char pdat[256];
			// Get shipName
			message.String(sizeof(pdat),pdat);
			if( strlen(pdat)!=0 )
			{
				if( (m_sShipName = NEW char[strlen(pdat)+1])==NULL )
					_THROW("Error! Allocate memory error");
				strcpy(m_sShipName,pdat);
			}
			// Get shipType
			message.String(sizeof(pdat),pdat);
			if( strlen(pdat)!=0 )
			{
				if( (m_sShipType = NEW char[strlen(pdat)+1])==NULL )
					_THROW("Error! Allocate memory error");
				strcpy(m_sShipType,pdat);
			}
			// Set scale
			//if( (ftmp=(float)rs->StringWidth(m_sShipName,m_idFontOne,m_fFontOneScale))>m_fStringMaxWidth )
			//	m_fShipNameScale = m_fFontOneScale*m_fStringMaxWidth/ftmp;
			//else m_fShipNameScale = m_fFontOneScale;
			if( (ftmp=(float)rs->StringWidth(m_sShipType,m_idFontOne,m_fFontOneScale))>m_fStringMaxWidth )
				m_fShipTypeScale = m_fFontOneScale*m_fStringMaxWidth/ftmp;
			else m_fShipTypeScale = m_fFontOneScale;
			// Get next ship data
			m_nShipHull = message.Long();
			m_nShipSail = message.Long();
			m_nShipCrew = message.Long();
			m_fShipSpeed = message.Float();
			m_fSailTo    = message.Float();         //boal
			m_nShipCannons = message.Long();
			m_nShipCharge = message.Long();
			m_nShipNation = message.Long();
			if(m_nShipCharge!=-1) m_nPrimitiveQuantity = 6;
			else	m_nPrimitiveQuantity = 5;
			Update(m_nShipCharge);
		}
	break;
	}
	return 0;
}

void ISPYGLASS::Update(long chargeType)
{
	ISG_VERTEX * pv = (ISG_VERTEX*)rs->LockVertexBuffer(m_idVBuf);
	if(pv==NULL) return;

	switch(chargeType)
	{
	case 0:		SetVertexUV ( &pv[28], m_frChargeUV[0] );	break;
	case 1:		SetVertexUV ( &pv[28], m_frChargeUV[1] );	break;
	case 2:		SetVertexUV ( &pv[28], m_frChargeUV[2] );	break;
	case 3:		SetVertexUV ( &pv[28], m_frChargeUV[3] );	break;
	}

	if(m_nShipNation>=0 && m_nShipNation<m_nationQuantity)
		SetVertexUV ( &pv[4], m_frNationUV[m_nShipNation] );

	rs->UnLockVertexBuffer(m_idVBuf);
}

void ISPYGLASS::SetVisible(bool vflag)
{
	if(!m_bVisible && vflag)
	{
		m_bVisible = true;
		m_fCurAlpha = 0;
		m_bFadeOut = true;
		return;
	}
	if(!vflag && m_bVisible)
	{
		if(m_bFadeOut)
		{
			m_bFadeOut = false;
			m_fCurAlpha = 255;
		}
		return;
	}
}
