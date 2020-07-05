#include <stdio.h>
#include "..\\..\\common_h\\vmodule_api.h"
#include "..\\..\\common_h\\defines.h"
#include "..\\..\\common_h\\matrix.h"
#include "TextureSequence.h"

#define FILE_PATH "TextureSequence\\%s.tga"
static const char *INI_FILENAME = "resource\\ini\\TextureSequence.ini";

#define TS_VERTEX_FRMT		(D3DFVF_XYZRHW|D3DFVF_TEX2|D3DFVF_TEXTUREFORMAT2)
struct TS_VERTEX
{
	CVECTOR pos;
	float w;
	float tu1, tv1;
	float tu2, tv2;
};

void GetRectFromNum(FRECT & texRect,int xq,int yq,int curNum, bool bHorzFlip,bool bVertFlip)
{
	int y = curNum/xq;
	if(y>=yq) y=0;
	int x = curNum - y*xq;
	if(x>=xq) x=0;

	float width = 1.f/xq;
	float height = 1.f/yq;

	if(bHorzFlip)	texRect.left = width + (texRect.right = x*width);
	else	texRect.right = width + (texRect.left = x*width);
	if(bVertFlip)	texRect.top = height + (texRect.bottom = y*height);
	else	texRect.bottom = height + (texRect.top = y*height);
}

TextureSequence::TextureSequence() 
{
	m_pTexture=NULL;
	m_AllTex=-1;
	m_bHorzFlip = false;
	m_bVertFlip = false;
}

TextureSequence::~TextureSequence()
{
	Release();
}

IDirect3DTexture8* TextureSequence::Initialize(VDX8RENDER *pRS, const char* cTSfileName, bool bCicled)
{
	m_bCicled = bCicled;
	if(pRS==null || cTSfileName==null) return null;
	m_pRS = pRS;

	// open ini file
	INIFILE * ini = api->fio->OpenIniFile((char*)INI_FILENAME);
	if(!ini)
	{
		_CORE_API->Trace("ini file %s not found!",INI_FILENAME);
		return null;
	}
	m_dwDeltaTime = ini->GetLong((char*)cTSfileName,"timeDelay",128);
	if(m_dwDeltaTime==0) {delete ini; return null;}
	m_texWidth = ini->GetLong((char*)cTSfileName,"width",128);
	m_texHeight = ini->GetLong((char*)cTSfileName,"height",128);
	m_xQuantity = ini->GetLong((char*)cTSfileName,"horzQ",1);
	m_yQuantity = ini->GetLong((char*)cTSfileName,"vertQ",1);
	m_maxCurNum = m_xQuantity*m_yQuantity;
	if(m_maxCurNum==0) {delete ini; return null;}

	// load sequence texture
	char fullName[256];
	if( !ini->ReadString((char*)cTSfileName,"TextureFile",fullName,sizeof(fullName)-1,"") )
		sprintf(fullName,FILE_PATH,cTSfileName);
	m_AllTex = m_pRS->TextureCreate(fullName);
	if(m_AllTex==-1)
		{delete ini; return null;}

	m_bHorzFlip = ini->GetLong((char*)cTSfileName,"flipH",0)!=0;
	m_bVertFlip = ini->GetLong((char*)cTSfileName,"flipV",0)!=0;

	m_pTexture=null;
	// create output texture
	if( S_OK != m_pRS->CreateTexture(m_texWidth,m_texHeight,1,D3DUSAGE_RENDERTARGET,D3DFMT_A8R8G8B8,D3DPOOL_DEFAULT,&m_pTexture) )
	{
		m_pTexture=null;
		_CORE_API->Trace("Can`t create texture");
		delete ini; return null;
	}

	// first render
	m_curNum = 0;
	m_dwCurDeltaTime = 0;
	//ToTextureRender(0.f);

	delete ini;
    return m_pTexture;
}

//-----------------------------------------------------------------------------
// Name: FrameUpdate
// Desc: Performs per-frame updates
//-----------------------------------------------------------------------------
bool TextureSequence::FrameUpdate()
{
	m_dwCurDeltaTime += api->GetRDeltaTime();
	while(m_dwCurDeltaTime>m_dwDeltaTime)
	{
		m_dwCurDeltaTime -= m_dwDeltaTime;
		m_curNum++;
		if(m_curNum>=m_maxCurNum) m_curNum=0;
	}

	ToTextureRender( (float)m_dwCurDeltaTime/(float)m_dwDeltaTime );
    return true;
}

void TextureSequence::ToTextureRender(float blendValue)
{
	DWORD newTFactor = ARGB(long(255.f*blendValue),long(255.f*blendValue),long(255.f*blendValue),long(255.f*blendValue));

	// set texture as render target
	IDirect3DSurface8 *pRenderTarg=NULL, *pOldRenderTarg=NULL;
	if( m_pRS->GetRenderTarget(&pOldRenderTarg)==S_OK )
	{
		if( m_pRS->GetSurfaceLevel(m_pTexture,0,&pRenderTarg) == S_OK )
		{
			IDirect3DSurface8 * pStencil;
			m_pRS->GetDepthStencilSurface(&pStencil);
			if( m_pRS->SetRenderTarget(pRenderTarg,NULL) == S_OK )
			{
				CMatrix matw;
				m_pRS->SetTransform(D3DTS_WORLD,(D3DMATRIX*)&matw);
				m_pRS->TextureSet(0,m_AllTex);
				m_pRS->TextureSet(1,m_AllTex);
				
				FRECT m_rectTex;
				TS_VERTEX v[4];
				for(int i=0; i<4; i++)
				{
					v[i].w = 0.5f;
					v[i].pos.z = 1.f;
				}
				v[0].pos.x = 0;					v[0].pos.y = 0;
				v[1].pos.x = 0;					v[1].pos.y = (float)m_texHeight;
				v[2].pos.x = (float)m_texWidth;	v[2].pos.y = 0;
				v[3].pos.x = (float)m_texWidth;	v[3].pos.y = (float)m_texHeight;

				GetRectFromNum(m_rectTex, m_xQuantity, m_yQuantity, m_curNum, m_bHorzFlip, m_bVertFlip);
				v[0].tu1=m_rectTex.left;	v[0].tv1=m_rectTex.bottom;
				v[1].tu1=m_rectTex.left;	v[1].tv1=m_rectTex.top;
				v[2].tu1=m_rectTex.right;	v[2].tv1=m_rectTex.bottom;
				v[3].tu1=m_rectTex.right;	v[3].tv1=m_rectTex.top;

				GetRectFromNum(m_rectTex, m_xQuantity, m_yQuantity, m_curNum+1, m_bHorzFlip, m_bVertFlip);
				v[0].tu2=m_rectTex.left;	v[0].tv2=m_rectTex.bottom;
				v[1].tu2=m_rectTex.left;	v[1].tv2=m_rectTex.top;
				v[2].tu2=m_rectTex.right;	v[2].tv2=m_rectTex.bottom;
				v[3].tu2=m_rectTex.right;	v[3].tv2=m_rectTex.top;

				// draw rectangle
				m_pRS->SetRenderState(D3DRS_TEXTUREFACTOR,newTFactor);
				m_pRS->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,TS_VERTEX_FRMT,2,v,sizeof(TS_VERTEX),"TextureSequence");

				m_pRS->SetRenderTarget(pOldRenderTarg,pStencil);
			}
			pStencil->Release();
			pRenderTarg->Release();
		}
		pOldRenderTarg->Release();
	}
}

void TextureSequence::Release()
{
	if(m_pTexture!=NULL && m_pRS!=NULL)
	{
		m_pRS->Release(m_pTexture);
		m_pTexture=NULL;
	}
	if(m_pRS!=NULL && m_AllTex!=-1)
	{
		m_pRS->TextureRelease(m_AllTex);
		m_AllTex=-1;
	}
}

void TextureSequence::LostRender()
{
	m_pRS->Release(m_pTexture);
}

void TextureSequence::RestoreRender()
{
	m_pRS->CreateTexture(m_texWidth,m_texHeight,1,D3DUSAGE_RENDERTARGET,D3DFMT_A8R8G8B8,D3DPOOL_DEFAULT,&m_pTexture);
}
