#include "InfoHandler.h"

InfoHandler::InfoHandler()
{
	m_pSurface = null;
	m_pRenderTarget = null;
}

InfoHandler::~InfoHandler()
{
	if(m_pSurface) m_rs->Release(m_pSurface);
	if(m_pRenderTarget) m_rs->Release(m_pRenderTarget);
}

bool InfoHandler::Init()
{
    // получить сервис рендера
	m_rs = (VDX8RENDER *)api->CreateService("dx8render");
	if(!m_rs)
	{
		api->Trace("No service: dx8render");
		return false;
	}
	if( m_rs->IsInsideScene() ) {
		m_rs->MakePostProcess();
	}

	if( m_rs->GetRenderTarget(&m_pRenderTarget)!=D3D_OK )
	{
		api->Trace("Can`t get render target");
		return false;
	}

	bool isOk = false;
	D3DSURFACE_DESC desc;
	if(m_pRenderTarget->GetDesc(&desc) == D3D_OK)
	{			
		if(m_rs->CreateImageSurface(desc.Width, desc.Height, desc.Format, &m_pSurface) == D3D_OK)
		{
			if( DoPreOut() )
			{
				if(m_rs->CopyRects(m_pRenderTarget, null, 0, m_pSurface, null) == D3D_OK)
				{
					isOk = true;
				}
			}
		}
	}
	if(!isOk)
	{
		api->Trace("Screen shot for info shower not created!");
		if(m_pSurface)
		{
			m_rs->Release(m_pSurface); m_pSurface=0;
		}
		if(m_pRenderTarget)
		{
			m_rs->Release(m_pRenderTarget); m_pRenderTarget=0;
		}
		return false;
	}

	return true;
}

void InfoHandler::Execute(dword delta_time)
{
}

void InfoHandler::Realize(dword delta_time)
{
	if(m_pSurface==null || m_pRenderTarget==null) return;
	m_rs->MakePostProcess();
	// Поддерживаем постоянный экран
	if(m_rs->CopyRects(m_pSurface, null, 0, m_pRenderTarget, null) != D3D_OK)
	{
		api->Trace("Can't copy fader screen shot to render target!");
	}
}

dword _cdecl InfoHandler::ProcessMessage(MESSAGE & message)
{
	long nMsgCode = message.Long();
	switch( nMsgCode )
	{
	case 1: Realize(0); break;
	}
	return 0;
}

bool InfoHandler::DoPreOut()
{
	if( AttributesPointer == null ) return false;

	bool isOK = false;
	DWORD dwBCol, dwFCol;
	char *inStrStart;
	char outStr[1048];
	D3DSURFACE_DESC desc;
	float fScale;
	int	nOutOffset, nOutWidth, nBorderWidth, nInsideRectWidth, nInsideRectHeight;
	int nRowQ;

	inStrStart = AttributesPointer->GetAttribute("infoStr");
	if(inStrStart!=null)
	{
		dwBCol = AttributesPointer->GetAttributeAsDword("backColor",0);
		dwFCol = AttributesPointer->GetAttributeAsDword("foreColor",0);
		nBorderWidth = AttributesPointer->GetAttributeAsDword("borderWidth",0);
		fScale = AttributesPointer->GetAttributeAsFloat("scale",1.f);
		nOutOffset = AttributesPointer->GetAttributeAsDword("offset",m_rs->CharHeight(0));
	}
	char * picTexureFile = AttributesPointer->GetAttribute("picfilename");

	DWORD TMP_VERTEX_FORMAT = (D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_TEX1|D3DFVF_TEXTUREFORMAT2);
	struct TMP_VERTEX
	{
		CVECTOR pos;
		float w;
		DWORD col;
		float tu,tv;
	} pV[4];

	pV[0].pos.z = 1.f;		pV[0].w = .5f;
	pV[1].pos.z = 1.f;		pV[1].w = .5f;
	pV[2].pos.z = 1.f;		pV[2].w = .5f;
	pV[3].pos.z = 1.f;		pV[3].w = .5f;

	if( m_pRenderTarget->GetDesc(&desc) != D3D_OK ) return false;

	int ntmp = 0;
	char * ps = null;
	if(inStrStart)
	{
		ntmp = m_rs->StringWidth(inStrStart,0,fScale);
		nOutWidth = (int)(sqrtf(4.f*ntmp*nOutOffset)+.9f); // ширина записи приблизительно больше высоты в четыре раза
		if(nOutWidth>(int)desc.Width) nOutWidth = desc.Width;
		nInsideRectWidth = nOutWidth + nBorderWidth*2;
		if(nInsideRectWidth>(int)desc.Width) nInsideRectWidth = desc.Width;

		nRowQ = 0;
		for(ps=inStrStart; ps!=null && *ps;)
		{
			ps = GetCutString(ps,nOutWidth,fScale);
			nRowQ++;
		}
		if( nRowQ*nOutOffset > (int)desc.Height ) nRowQ = desc.Height/nOutOffset;
		nInsideRectHeight = nRowQ*nOutOffset + nBorderWidth*2;
		if(nInsideRectHeight > (int)desc.Height ) nInsideRectHeight = desc.Height;
	}

	isOK = m_rs->IsInsideScene();
	bool bMakeEndScene = false;
	if( !isOK ) {
		bMakeEndScene = true;
		isOK = (m_rs->BeginScene() == D3D_OK);
	}

	if( isOK )
	{
		// show picture
		if(picTexureFile!=null)
		{
			int picID = m_rs->TextureCreate(picTexureFile);
			if(picID>=0)
			{
				m_rs->TextureSet(0,picID);
				pV[0].col = pV[1].col = pV[2].col = pV[3].col = 0xFFFFFFFF;
				pV[0].pos.x = 0.f;					pV[0].pos.y = 0.f;
				pV[1].pos.x = 0.f;					pV[1].pos.y = (float)desc.Height;
				pV[2].pos.x = (float)desc.Width;	pV[2].pos.y = 0.f;
				pV[3].pos.x = (float)desc.Width;	pV[3].pos.y = (float)desc.Height;
				pV[0].tu = 0.f;		pV[0].tv = 0.f;
				pV[1].tu = 0.f;		pV[1].tv = 1.f;
				pV[2].tu = 1.f;		pV[2].tv = 0.f;
				pV[3].tu = 1.f;		pV[3].tv = 1.f;
				m_rs->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,TMP_VERTEX_FORMAT,2,&pV,sizeof(TMP_VERTEX),"iInfoShowerPic");
				m_rs->TextureRelease(picID);

				m_rs->SetProgressImage( picTexureFile );
			}
		}

		if(inStrStart)
		{
			// show back
			pV[0].col = pV[1].col = pV[2].col = pV[3].col = dwBCol;
			pV[0].pos.x = 0.f;					pV[0].pos.y = 0.f;
			pV[1].pos.x = 0.f;					pV[1].pos.y = (float)desc.Height;
			pV[2].pos.x = (float)desc.Width;	pV[2].pos.y = 0.f;
			pV[3].pos.x = (float)desc.Width;	pV[3].pos.y = (float)desc.Height;
			m_rs->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,TMP_VERTEX_FORMAT,2,&pV,sizeof(TMP_VERTEX),"iInfoShower");

			// show fore
			pV[0].col = pV[1].col = pV[2].col = pV[3].col = dwFCol;
			pV[1].pos.x = pV[0].pos.x = (float)(desc.Width - nInsideRectWidth)/2;
			pV[3].pos.x = pV[2].pos.x = pV[0].pos.x + nInsideRectWidth;
			pV[2].pos.y = pV[0].pos.y = (float)(desc.Height - nInsideRectHeight)/2;
			pV[3].pos.y = pV[1].pos.y = pV[0].pos.y + nInsideRectHeight;
			m_rs->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,TMP_VERTEX_FORMAT,2,&pV,sizeof(TMP_VERTEX),"iInfoShower");

			// show strings
			ntmp = 0;
			int topY = (desc.Height - nRowQ*nOutOffset)/2;
			for(ps=inStrStart; ps!=null && *ps;)
			{
				char * oldps = ps;
				ps = GetCutString(ps,nOutWidth,fScale);
				if(!ps || ps==oldps) break;
				StringToBufer(outStr,sizeof(outStr),oldps,ps-oldps);
				m_rs->ExtPrint( 0,0xFFFFFFFF,0,ALIGN_CENTER,true,fScale,0,0,
					desc.Width/2,topY+ntmp*nOutOffset, outStr);
				ntmp++;
				if(ntmp>=nRowQ) break;
			}
		}

		if( bMakeEndScene ) m_rs->EndScene();
	}

	return isOK;
}

char * InfoHandler::GetCutString( char * pstr, int nOutWidth, float fScale )
{
	bool spaceWait = false;
	char param[1024];

	// удаляем первые переходы на новую строку
	while(pstr && (*pstr==0x0A || *pstr==0x0D || *pstr==32) )	pstr++;

	char * oldps = null;
	for(char *ps=pstr; ps && *ps; ps++)
	{
		if( *ps==0x0a || *ps==0x0d ) break;

		if( *ps==32 )
		{
			if(spaceWait) continue;
			spaceWait = true;

			int n = ps-pstr;
			if(n==0) continue;
			if(n>=1023) break;

			strncpy(param,pstr,n);
			param[n] = 0;
			int j = m_rs->StringWidth(param,0,fScale);
			if( j<nOutWidth )
			{
				oldps = ps;
				continue;
			}
			else
				if(oldps)	return oldps;
				else return ps;
		}
		spaceWait = false;
	}

	int nt = ps-pstr;
	if(nt<=0) return null;
	if(nt>=1023)
	{
		nt = 1023;
		ps = pstr+nt;
	}

	strncpy(param,pstr,nt);
	param[nt] = 0;
	int jt = m_rs->StringWidth(param,0,fScale);
	if( jt<nOutWidth )
		return ps;
	else
		if(oldps)
			return oldps;

	return ps;
}

void InfoHandler::StringToBufer(char * outStr, int sizeBuf, char * inStr, int copySize)
{
	if(outStr==null || sizeBuf<=0) return;
	outStr[0] = 0;
	if(inStr==null) return;
	while(*inStr && (*inStr==0x0A || *inStr==0x0D || *inStr==32)) inStr++;

	int n = strlen(inStr);
	if(n>copySize) n = copySize;
	if(n>sizeBuf-1) n = sizeBuf-1;

	if(n<1) return;

	strncpy(outStr,inStr,n);
	outStr[n] = 0;
}
