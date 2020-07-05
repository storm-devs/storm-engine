#ifndef _AVIPLAYER_HPP_
#define _AVIPLAYER_HPP_

#include <ddraw.h>
#include <amstream.h>
#include "..\base_video.h"

#define XI_AVIVIDEO_FVF		(D3DFVF_XYZRHW|D3DFVF_TEX1|D3DFVF_TEXTUREFORMAT2)
struct XI_AVIVIDEO_VERTEX
{
	CVECTOR pos;
	float w;
	float tu,tv;
};

class CAviPlayer : public xiBaseVideo
{
	VDX8RENDER *rs;
	bool m_bShowVideo;

public:
	CAviPlayer();
	~CAviPlayer();
	bool	Init();
	void	Execute(dword delta_time);
	void	Realize(dword delta_time);
    dword _cdecl ProcessMessage(MESSAGE & message);

	void SetShowVideo(bool bShowVideo) {m_bShowVideo=bShowVideo;}
	IDirect3DTexture9 * GetCurrentVideoTexture() {return pTex;}

protected:
	bool	m_bContinue;

    IDirectDraw *pDD;
    IDirectDrawSurface *pPrimarySurface;
    IDirectDrawSurface *pVideoSurface;

	IAMMultiMediaStream		* pAMStream;
	IMediaStream			* pPrimaryVidStream;
	IDirectDrawMediaStream	* pDDStream;
	IDirectDrawStreamSample	* pSample;

	POINT dstPnt;
	RECT lockRect;

	XI_AVIVIDEO_VERTEX v[4];

	IDirect3DSurface9 * pTmpRenderTarget;
	IDirect3DTexture9 * pTex;

	void ReleaseAll();
	bool PlayMedia(char * fileName);
	bool GetInterfaces();
	void CleanupInterfaces();

	bool m_bFirstDraw;
	bool m_bMakeUninitializeDD;
};

#endif
