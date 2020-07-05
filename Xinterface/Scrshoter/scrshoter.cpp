#include "..\\..\\engine\\program\\interface\\messages.h"
#include "..\\defines.h"
#include "scrshoter.h"

#define SS_TEXTURE_WIDTH		128
#define SS_TEXTURE_HEIGHT		128
#define SS_TEXTURE_FONECOLOR	0xFF000000

DWORD GetA8R8G8B8_FromFMT(void *p, DWORD fmt)
{
	DWORD retVal;

#ifdef _XBOX
	if(fmt==D3DFMT_R5G6B5 || fmt==D3DFMT_LIN_R5G6B5)
#else
	if(fmt==D3DFMT_R5G6B5)
#endif
	{
		retVal = 0xFF000000 |
			( ((DWORD)( *(WORD*)(p) & 0xF800 )) << 8 ) |
			( ((DWORD)( *(WORD*)(p) & 0x7E0)) << 5 ) |
			( ((DWORD)( *(WORD*)(p) & 0x1F)) << 3 );
	}
	else
	{
		retVal = 0xFF000000 | (*(DWORD*)(p));
	}

	return retVal;
}

SCRSHOTER::SCRSHOTER()
{
	m_pScrShotTex = null;
	m_list = null;
}

SCRSHOTER::~SCRSHOTER()
{
	if(m_pScrShotTex!=null && rs!=null) rs->Release(m_pScrShotTex);
	m_pScrShotTex = null;
	SAVETEXTURES * pst;
	while(m_list)
	{
		pst = m_list;
		m_list = pst->next;
		if(pst->fileName!=null) delete pst->fileName;
		if(pst->m_pTex!=null && rs!=null) rs->Release(pst->m_pTex);
		delete pst;
	}
}

bool SCRSHOTER::Init()
{
	GUARD(SCRSHOTER::Init())
	SetDevice();
	UNGUARD
	return true;
}

void SCRSHOTER::SetDevice()
{
    // получить сервис рендера
	rs = (VDX8RENDER *)_CORE_API->CreateService("dx8render");
	if(!rs){_THROW("No service: dx8render")}

}

void SCRSHOTER::Execute(dword Delta_Time)
{
}

void SCRSHOTER::Realize(dword Delta_Time)
{
	if(m_pScrShotTex==null)
		if(!MakeScreenShot())
			{api->Trace("ERROR!!! screen shot create error"); api->Event("makescrshot",0);}
		else
			api->Event("makescrshot",0);
}

bool SCRSHOTER::MakeScreenShot()
{
	int n;
	D3DLOCKED_RECT inRect,outRect;

	HRESULT hr = D3D_OK;

	// «аставим видео карту отрисовать все незакоченные задани€
	hr = rs->EndScene();
	if(hr!=D3D_OK) {
		api->Trace("ERROR!!! Can`t EndScene");
		return false;
	}
	hr = rs->BeginScene();
	if(hr!=D3D_OK) {
		api->Trace("ERROR!!! Can`t BeginScene");
		return false;
	}

	// удалим старый скрин шот
	if(m_pScrShotTex!=null && rs!=null) rs->Release(m_pScrShotTex);
	m_pScrShotTex = null;

	// получим данные о старой поверхности рендера
	D3DSURFACE_DESC desc;
	IDirect3DSurface8 * pOldRenderTarg = null;
	if( hr==D3D_OK ) hr = rs->GetRenderTarget(&pOldRenderTarg);
	if( hr==D3D_OK ) hr = pOldRenderTarg->GetDesc(&desc);

	// получим копию рендер буфера
	IDirect3DSurface8 * pRenderTarg = null;
	if( hr==D3D_OK ) hr = rs->CreateImageSurface(desc.Width, desc.Height, desc.Format, &pRenderTarg);
	if( hr==D3D_OK ) hr = rs->CopyRects( pOldRenderTarg, null, 0, pRenderTarg, null );
	if( pOldRenderTarg!=null ) pOldRenderTarg->Release();

	// создадим новый скрин шот
	if( hr==D3D_OK ) hr = rs->CreateTexture(SS_TEXTURE_WIDTH,SS_TEXTURE_HEIGHT,1,0,D3DFMT_A8R8G8B8,D3DPOOL_MANAGED,&m_pScrShotTex);

	// получим буфер дл€ копии поверхности рендеринга
	void * pIn = null;
	if( hr==D3D_OK ) hr = pRenderTarg->LockRect(&inRect,null,0);
	if( hr==D3D_OK ) pIn = inRect.pBits;
	// получим буфер дл€ текстуры
	void * pOut = null;
	if( hr==D3D_OK ) hr = m_pScrShotTex->LockRect(0,&outRect,null,0);
	if( hr==D3D_OK ) pOut = outRect.pBits;

	// заполним эту текстуру из копии нашего рендер буфера
	if( hr==D3D_OK )
	{
		// —оздать набор отступов по ординатам
		int * pHorzOff = NEW int[SS_TEXTURE_WIDTH];
		int * pVertOff = NEW int[SS_TEXTURE_HEIGHT];
		if(!pHorzOff || !pVertOff) {
			_THROW("allocate memory error");
		}
		int nHorzSize, nVertSize;
		if( (float)desc.Width/desc.Height < (float)SS_TEXTURE_WIDTH/SS_TEXTURE_HEIGHT )
		{
			nHorzSize = desc.Width;
			nVertSize = desc.Width * SS_TEXTURE_HEIGHT / SS_TEXTURE_WIDTH;
		}
		else
		{
			nHorzSize = desc.Height * SS_TEXTURE_WIDTH / SS_TEXTURE_HEIGHT;
			nVertSize = desc.Height;
		}
		// «аполним горизонтальные смещени€
		for(n=0; n<SS_TEXTURE_WIDTH; n++)
			pHorzOff[n] = (n * desc.Width / SS_TEXTURE_WIDTH) * (inRect.Pitch / desc.Width);
		// «аполним вертикальные смещени€
		for(n=0; n<SS_TEXTURE_HEIGHT; n++)
			pVertOff[n] = n * desc.Height / SS_TEXTURE_HEIGHT;

		int vi, hi;
		for ( vi=0; vi<SS_TEXTURE_HEIGHT; vi++ )
		{
			BYTE * pInPxl = (BYTE*)pIn + inRect.Pitch * pVertOff[vi];
			DWORD * pOutPxl = (DWORD*) ( (BYTE*)pOut + outRect.Pitch * vi );
			for( hi=0; hi<SS_TEXTURE_WIDTH; hi++ )
			{
				pOutPxl[hi] = GetA8R8G8B8_FromFMT( &pInPxl[pHorzOff[hi]], desc.Format );
			}
		}

		delete pHorzOff;
		delete pVertOff;
	}

	// закрываем открытые буфера
	if(pIn!=null) pRenderTarg->UnlockRect();

#ifdef _XBOX
	// ƒелаем перевод в смешанную текстуру
	DWORD dwTmp = outRect.Pitch*SS_TEXTURE_HEIGHT;
	//DWORD dwPixelSize = XGBytesPerPixelFromFormat( D3DFMT_A8R8G8B8 );
	if( (pIn=NEW char[dwTmp]) == null ) { _THROW("allocate memory error") }
	memcpy(pIn,outRect.pBits,dwTmp);
    XGSwizzleRect( pIn, 0, NULL, outRect.pBits,
				   SS_TEXTURE_WIDTH, SS_TEXTURE_HEIGHT,
				   NULL, XGBytesPerPixelFromFormat( D3DFMT_A8R8G8B8 ) );
	delete pIn;
#endif

	// закрываем открытые буфера
	if(pOut!=null) m_pScrShotTex->UnlockRect(0);

	// ”далим ненужную уже копию экрана
	if(pRenderTarg!=null) pRenderTarg->Release();

	// Ќаложим на шот текстуру с рамкой
	int nTextureID = rs->TextureCreate("interfaces\\EmptyBorder.tga");
	if(nTextureID>=0)
	{
		IDirect3DTexture8 * pScrShotTex = null;
		if( D3D_OK == rs->CreateTexture(SS_TEXTURE_WIDTH,SS_TEXTURE_HEIGHT,1,D3DUSAGE_RENDERTARGET,D3DFMT_A8R8G8B8,D3DPOOL_DEFAULT,&pScrShotTex) )
		{
			DWORD BI_SCRSHOTER_VERTEX_FORMAT = (D3DFVF_XYZRHW|D3DFVF_TEX1|D3DFVF_TEXTUREFORMAT2);
			struct BI_SCRSHOTER_VERTEX
			{
				CVECTOR pos;
				float w;
				float tu, tv;
			} vert[4];
			for(int i=0; i<4; i++) {vert[i].pos.z = 1.f; vert[i].w = 0.5f;}
			vert[1].pos.x = vert[0].pos.x = 0;
			vert[2].pos.y = vert[0].pos.y = 0;
			vert[3].pos.x = vert[2].pos.x = SS_TEXTURE_WIDTH;
			vert[3].pos.y = vert[1].pos.y = SS_TEXTURE_HEIGHT;
			vert[1].tu = vert[0].tu = 0;
			vert[2].tv = vert[0].tv = 0;
			vert[3].tu = vert[2].tu = 1.f;
			vert[3].tv = vert[1].tv = 1.f;

			pOldRenderTarg = null;
			pRenderTarg = null;
			if( rs->GetRenderTarget(&pOldRenderTarg)==S_OK )
			{
				IDirect3DSurface8 * pStencil = null;
				rs->GetDepthStencilSurface(&pStencil);
				pScrShotTex->GetSurfaceLevel(0,&pRenderTarg);
				if( rs->SetRenderTarget(pRenderTarg,NULL) == S_OK )
				{
					rs->SetTexture(0,m_pScrShotTex);
						rs->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,BI_SCRSHOTER_VERTEX_FORMAT,2,vert,sizeof(BI_SCRSHOTER_VERTEX),"battle_icons");
					rs->TextureSet(0,nTextureID);
						rs->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,BI_SCRSHOTER_VERTEX_FORMAT,2,vert,sizeof(BI_SCRSHOTER_VERTEX),"battle_icons");
					rs->SetRenderTarget(pOldRenderTarg,pStencil);
				}
				if(pRenderTarg)	pRenderTarg->Release();
				if(pStencil) pStencil->Release();
				if(pOldRenderTarg) pOldRenderTarg->Release();
			}
			IDirect3DSurface8 *pSurf1=null, *pSurf2=null;
			rs->GetSurfaceLevel(m_pScrShotTex,0,&pSurf1);
			rs->GetSurfaceLevel(pScrShotTex,0,&pSurf2);
			rs->CopyRects(pSurf2,null,0,pSurf1,null);
			if(pSurf1) rs->Release(pSurf1);
			if(pSurf2) rs->Release(pSurf2);
			rs->Release(pScrShotTex);
		}

		rs->TextureRelease(nTextureID);
	}

	return hr==D3D_OK;
}

dword _cdecl SCRSHOTER::ProcessMessage(MESSAGE & message)
{
	switch(message.Long())
	{
	case MSG_SCRSHOT_MAKE:	return (dword)m_pScrShotTex;	break;
	case MSG_SCRSHOT_READ:
		{
			char param[512],param2[256];
			VDATA * pvdat;

			message.String(sizeof(param)-1,param);
			message.String(sizeof(param2)-1,param2);
			pvdat = message.ScriptVariablePointer();

			IDirect3DTexture8 * pRetTex = AddSaveTexture(param,param2);
			char * strDat = FindSaveData(param2);
			if(pvdat)
				if(!strDat) pvdat->Set("\0");
				else pvdat->Set(strDat);
			return (dword)pRetTex;
		}
		break;
	case MSG_SCRSHOT_RELEASE:
		{
			char param[512];
			message.String(sizeof(param)-1,param);
			DelSaveTexture(param);
		}
		break;
	}
	return 0;
}

IDirect3DTexture8 * SCRSHOTER::FindSaveTexture(char * fileName)
{
	if(!fileName) return null;
	SAVETEXTURES * ps = m_list;
	while(ps)
		if(ps->fileName && stricmp(fileName,ps->fileName)==0) return ps->m_pTex;
		else ps = ps->next;
	return null;
}

char * SCRSHOTER::FindSaveData(char * fileName)
{
	if(!fileName) return null;
	SAVETEXTURES * ps = m_list;
	while(ps)
		if(ps->fileName && stricmp(fileName,ps->fileName)==0) return ps->dataString;
		else ps = ps->next;
	return null;
}

IDirect3DTexture8 * SCRSHOTER::AddSaveTexture(char * dirName, char * fileName)
{
	if(fileName==null) return null;
	IDirect3DTexture8 * rval = FindSaveTexture(fileName);
	if(rval) return rval;
	if(stricmp(fileName,"newsave")==0) return m_pScrShotTex;
	SAVETEXTURES * ps = NEW SAVETEXTURES;
	if(ps==null)	{_THROW("Allocate memory error");}
	ps->dataString = null;
	ps->next = m_list;
	m_list = ps;
	m_list->fileName = NEW char[strlen(fileName)+1];
	if(m_list->fileName==null)	{_THROW("Allocate memory error");}
	strcpy(m_list->fileName,fileName);
	char param[1024];
	if(dirName==null || dirName[0] == 0)	sprintf(param,"%s",fileName);
	else	sprintf(param,"%s\\%s",dirName,fileName);
	m_list->m_pTex = GetTexFromSave(param, &ps->dataString);
	return m_list->m_pTex;
}

void SCRSHOTER::DelSaveTexture(char * fileName)
{
	if(!fileName) return;
	SAVETEXTURES * oldps = null;
	SAVETEXTURES * ps = m_list;
	while(ps)
		if(ps->fileName && stricmp(fileName,ps->fileName)==0)
		{
			if(oldps) oldps->next = ps->next;
			else m_list = ps->next;
			if(ps->fileName!=null)	delete ps->fileName;
			if(ps->dataString!=null)	delete ps->dataString;
			if(ps->m_pTex!=null && rs!=null) rs->Release(ps->m_pTex);
			delete ps;
			return;
		}
		else
		{
			oldps = ps;
			ps = ps->next;
		}
}

IDirect3DTexture8 * SCRSHOTER::GetTexFromSave(char * fileName, char **pDatStr)
{
	HRESULT hr = D3D_OK;
	D3DLOCKED_RECT outRect;
	IDirect3DTexture8 * pt = null;
	*pDatStr = 0;

	long datSize = 0;
	char * pdat = null;
	pdat = (char*)api->GetSaveData(fileName,datSize);
	long startIdx = 0;
	long texSize = 0;
	if( pdat!=null && datSize>sizeof(SAVE_DATA_HANDLE) )
	{
		startIdx = ((SAVE_DATA_HANDLE*)pdat)->StringDataSize + sizeof(SAVE_DATA_HANDLE);
		texSize = ((SAVE_DATA_HANDLE*)pdat)->SurfaceDataSize;
	}
	if( datSize>sizeof(SAVE_DATA_HANDLE) && datSize==startIdx+texSize && texSize==4*SS_TEXTURE_WIDTH*SS_TEXTURE_HEIGHT )
	{
		hr = rs->CreateTexture(SS_TEXTURE_WIDTH,SS_TEXTURE_HEIGHT,1,0,D3DFMT_A8R8G8B8,D3DPOOL_MANAGED,&pt);
		if( hr==D3D_OK ) hr = pt->LockRect(0,&outRect,null,0);
		if( hr==D3D_OK )
		{
			memcpy( outRect.pBits, &pdat[startIdx], texSize );
			pt->UnlockRect(0);
		}
		if(pDatStr)
		{
			int strLen = startIdx-sizeof(SAVE_DATA_HANDLE);
			*pDatStr = NEW char[strLen+1];
			if(!*pDatStr) {_THROW("allocate memory error");}
			strncpy(*pDatStr, &pdat[sizeof(SAVE_DATA_HANDLE)], strLen);
			(*pDatStr)[strLen] = 0;
		}
	}
/*	else
	{
		int idx=0;
		for(int i=0; i<SS_TEXTURE_HEIGHT; i++)
			for(int j=0; j<SS_TEXTURE_WIDTH; j++)
			{
				((DWORD*)outRect.pBits)[idx] = 0xFF000000;
				idx++;
			}
	}*/
	PTR_DELETE(pdat);

	if( hr==D3D_OK ) return pt;
	if(*pDatStr) delete (*pDatStr); *pDatStr = 0;
	if(pt && rs) rs->Release(pt);
	return null;
}
