#include <stdio.h>
#include "aviplayer.h"
#include "..\\..\\common_h\\defines.h"
#include "..\\..\\engine\\program\\interface\\messages.h"

#pragma comment(lib,"amstrmid.lib")
#pragma comment(lib,"ddraw.lib")

#define IRELEASE(i)	{if(i) i->Release(); i=NULL;}

#define VIDEO_DIRECTORY		"resource\\videos"

long AVI_GetTextureSize(long width)
{
	long i=2;
	while((width>>=1L)>0)
		i<<=1L;
	return i;
}

CAviPlayer::CAviPlayer()
{
	m_bContinue = true;
	m_bShowVideo = true;

    pDD = NULL;
    pPrimarySurface = NULL;
	pVideoSurface	= NULL;
	pAMStream = NULL;
	pPrimaryVidStream = NULL;
	pDDStream = NULL;
	pSample = NULL;

	pTmpRenderTarget = NULL;
	pTex = NULL;
	m_bFirstDraw = true;
	m_bMakeUninitializeDD = false;
}

CAviPlayer::~CAviPlayer()
{
	ReleaseAll();
}

bool CAviPlayer::Init()
{
	if( (rs=(VDX8RENDER *)_CORE_API->CreateService("dx8render")) == NULL )
	{
		_THROW("Can`t create render service");
	}

	api->LayerCreate("vRealize",true,false);
	api->LayerSetRealize("vRealize",true);
	api->LayerAdd("vRealize",GetID(),-1);

	api->LayerCreate("vExecute",true,false);
	api->LayerSetExecute("vExecute",true);
	api->LayerAdd("vExecute",GetID(),1);

	if(!GetInterfaces())
	{
		m_bContinue = false;
		CleanupInterfaces();
	}

	return true;
}

void CAviPlayer::Execute(dword delta_time)
{
	if(m_bContinue==false)
	{
		if(pAMStream!=NULL)
			pAMStream->SetState(STREAMSTATE_STOP);
		CleanupInterfaces();
		api->Event("ievntEndVideo");
		return;
	}
}

void CAviPlayer::Realize(dword delta_time)
{
	int i;
	HRESULT hr;
	DDSURFACEDESC ddsd;
	ddsd.dwSize = sizeof(ddsd);
	if(pSample==NULL)
	{
		m_bContinue = false;
		return;
	}

	if(m_bFirstDraw)
	{
		rs->Clear( 0,NULL,D3DCLEAR_TARGET,0,0,0);
		m_bFirstDraw = false;
	}

	if( (hr=pSample->Update(0, NULL, NULL, NULL)) == S_OK )
	{
		hr = pVideoSurface->Lock( NULL, &ddsd, 0, NULL );
		if(hr!=S_OK) return;

		D3DLOCKED_RECT d3dlkRect;
		hr = pTex->LockRect(0,&d3dlkRect,&lockRect,0);
		if(hr!=S_OK) return;

		int nPitch = d3dlkRect.Pitch;
		char * pOutData = (char*)d3dlkRect.pBits;
		char * pInData = (char*)ddsd.lpSurface;
		long copySize = ddsd.lPitch<d3dlkRect.Pitch ? ddsd.lPitch : d3dlkRect.Pitch;
		for(i=0; i<(int)ddsd.dwHeight; i++)
		{
			memcpy(pOutData,pInData,copySize);
			pInData += ddsd.lPitch;
			pOutData += d3dlkRect.Pitch;
		}

		pTex->UnlockRect(0);
		pVideoSurface->Unlock(NULL);

		if(m_bShowVideo)
		{
			rs->SetTexture(0,pTex);
			rs->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,XI_AVIVIDEO_FVF,2,v,sizeof(XI_AVIVIDEO_VERTEX),"battle_icons");
		}
    }
	else
	{
		m_bContinue=false;
	}
}

dword _cdecl CAviPlayer::ProcessMessage(MESSAGE & message)
{
	switch(message.Long())
	{
	case MSG_SET_VIDEO_PLAY:
		{
			char param[256];
			char vidName[256];
			message.String(sizeof(param)-1,param);
			sprintf(vidName,"%s\\%s",VIDEO_DIRECTORY,param);
			if( !PlayMedia(vidName) )
			{
				CleanupInterfaces();
				api->PostEvent("ievntEndVideo",1,null);
			}
		}
		break;
	}
	return 0;
}

bool CAviPlayer::PlayMedia(char * fileName)
{
	HRESULT hr = S_OK;
    DDSURFACEDESC ddsd;

	WCHAR    wPath[MAX_PATH];        // wide (32-bit) string name
	MultiByteToWideChar(CP_ACP, 0, fileName, -1, wPath,
		sizeof(wPath)/sizeof(wPath[0]));    

	if(pAMStream==null) return false;

	hr = pAMStream->OpenFile(wPath, 0);
    if (FAILED(hr)) {
		api->Trace("Video Error!!!(0x%8x) Can`t load video file = %s",hr,fileName);
        return false;
    }

    hr = pAMStream->GetMediaStream(MSPID_PrimaryVideo, &pPrimaryVidStream);
    if (FAILED(hr)) {
		api->Trace("Video Error!!! Can`t get media stream");
        return false;
    }
    hr = pPrimaryVidStream->QueryInterface(IID_IDirectDrawMediaStream, (void **)&pDDStream);
    if (FAILED(hr)) {
		api->Trace("Video Error!!! Can`t query interface DirectDrawMediaStream");
        return false;
    }
    ddsd.dwSize = sizeof(ddsd);
    hr = pDDStream->GetFormat(&ddsd, NULL, NULL, NULL);
    if (FAILED(hr)) {
		api->Trace("Video Error!!! Can`t get stream format");
        return false;
    }
	long srcWidth = ddsd.dwWidth;
	long srcHeight = ddsd.dwHeight;
	hr = pDD->CreateSurface(&ddsd, &pVideoSurface, NULL);
    if (FAILED(hr)) {
		api->Trace("Video Error!!! Can`t create surface for video imaging");
        return false;
    }

	lockRect.left = 0;			lockRect.top = 0;
	lockRect.right = srcWidth;	lockRect.bottom = srcHeight;

	hr = pDDStream->CreateSample((IDirectDrawSurface*)pVideoSurface, NULL, 0, &pSample);
    if (FAILED(hr)) {
		api->Trace("Video Error!!! Can`t create sample for this video");
        return false;
    }

	RECT dstRect;
	GetWindowRect(api->GetAppHWND(),&dstRect);
	long dstWidth = dstRect.right - dstRect.left;
	long dstHeight = dstRect.bottom - dstRect.top;

	float horzK = (float)dstWidth/srcWidth;
	float vertK = (float)dstHeight/srcHeight;
	if(horzK<vertK)	vertK=horzK;
	else			horzK=vertK;

	dstRect.left = dstRect.top = 0;
	dstRect.right = (long)(srcWidth*horzK+.5f);
	dstRect.bottom = (long)(srcHeight*vertK+.5f);

	dstRect.left += (dstWidth-dstRect.right)/2;
	dstRect.top += (dstHeight-dstRect.bottom)/2;
	dstRect.right += dstRect.left;
	dstRect.bottom += dstRect.top;

	D3DSURFACE_DESC d3d8surf_desc;

	IDirect3DSurface9 * pd3dsurf = null;
	rs->GetRenderTarget(&pd3dsurf);
	pd3dsurf->GetDesc(&d3d8surf_desc);
	pd3dsurf->Release();

	hr = rs->CreateTexture(AVI_GetTextureSize(srcWidth), AVI_GetTextureSize(srcHeight), 1, 0, d3d8surf_desc.Format, D3DPOOL_MANAGED, &pTex);
	if (FAILED(hr)) {
		api->Trace("Video Error!!! Can`t create texture for this video");
		return false;
	}

	for(int i=0; i<4; i++)
	{
		v[i].w = .5f;
		v[i].pos.z = 1.f;
	}
	v[0].pos.x = (float)dstRect.left;  v[0].pos.y = (float)dstRect.top;
	v[0].tu=0.f; v[0].tv=0.f;
	v[1].pos.x = (float)dstRect.left;  v[1].pos.y = (float)dstRect.bottom;
	v[1].tu=0.f; v[1].tv = (float)lockRect.bottom/AVI_GetTextureSize(srcHeight);
	v[2].pos.x = (float)dstRect.right; v[2].pos.y = (float)dstRect.top;
	v[2].tu = (float)lockRect.right/AVI_GetTextureSize(srcWidth); v[2].tv=0.f;
	v[3].pos.x = (float)dstRect.right, v[3].pos.y = (float)dstRect.bottom;
	v[3].tu = (float)lockRect.right/AVI_GetTextureSize(srcWidth); v[3].tv = (float)lockRect.bottom/AVI_GetTextureSize(srcHeight);

    hr = pAMStream->SetState(STREAMSTATE_RUN);
    if (FAILED(hr)) {
		api->Trace("Video Error!!! Can`t run media stream");
        return false;
    }

    return true;
}

bool CAviPlayer::GetInterfaces()
{
    HRESULT hr = S_OK;

    // Initialize COM
    if (FAILED(hr = CoInitialize(NULL)))
        return false;
	m_bMakeUninitializeDD = true;

	hr = DirectDrawCreate(NULL, &pDD, NULL);
    if (FAILED(hr)) {
		api->Trace("Video Error!!! Can`t create DirectDraw interface");
        return false;
    }
	hr = pDD->SetCooperativeLevel(api->GetAppHWND(), DDSCL_NORMAL);
    if (FAILED(hr)) {
		api->Trace("Video Error!!! Can`t SetCooperativeLevel for DirectDraw");
        return false;
    }
	hr = CoCreateInstance(CLSID_AMMultiMediaStream, NULL, CLSCTX_INPROC_SERVER,
		IID_IAMMultiMediaStream, (void **)&pAMStream);
    if (FAILED(hr)) {
		api->Trace("Video Error!!! Can`t create interface AMMultiMediaStream");
        return false;
    }

	hr = pAMStream->Initialize(STREAMTYPE_READ, AMMSF_NOGRAPHTHREAD, NULL);
    if (FAILED(hr)) {
		api->Trace("Video Error!!! Can`t initialize interface AMMultiMediaStream");
        return false;
    }
	hr = pAMStream->AddMediaStream(pDD, &MSPID_PrimaryVideo, 0, NULL);
    if (FAILED(hr)) {
		api->Trace("Video Error!!! Can`t add video stream");
        return false;
    }
	hr = pAMStream->AddMediaStream(NULL, &MSPID_PrimaryAudio, AMMSF_ADDDEFAULTRENDERER, NULL);
    if (FAILED(hr)) {
		api->Trace("Video Error!!! Can`t add audio stream");
        return false;
    }

    return true;
}

void CAviPlayer::CleanupInterfaces()
{
	IRELEASE(pSample);
	IRELEASE(pDDStream);
	IRELEASE(pPrimaryVidStream);

	IRELEASE(pAMStream);
	IRELEASE(pVideoSurface);
    IRELEASE(pPrimarySurface);
    IRELEASE(pDD);

	IRELEASE(pTmpRenderTarget);

	if(pTex!=NULL && rs!=NULL)	rs->Release(pTex);
	pTex=NULL;

	if( m_bMakeUninitializeDD ) CoUninitialize();
	m_bMakeUninitializeDD = false;
}

void CAviPlayer::ReleaseAll()
{
	CleanupInterfaces();
}
