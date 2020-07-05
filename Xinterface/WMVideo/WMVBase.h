//#ifndef _WMVBASE_H
//#define _WMVBASE_H

#include <xtl.h>
#include <xgraphics.h>
#include <xmv.h>
#include "..\base_video.h"

#define NUM_PACKETS 64
#define PACKET_SIZE 2048 * 2 * 2

//-----------------------------------------------------------------------------
// Name: class VideoToTexture
// Desc: play video into texture
//-----------------------------------------------------------------------------
class WMVBase
{
public:
	 WMVBase();
	~WMVBase();

    bool Initialize(IDirect3DDevice8* pd3dDev,const char* cWMVfileName);
	bool FileOpen(const char* cWMVfileName);
	void Release();
    bool FrameUpdate();
	bool IsReady();			// Ready to switch?
	HRESULT DecodeNext();	// Decode next frame

	void SetShowParameters(float ftl,float ftt,float ftr,float ftb, float fpl,float fpt,float fpr,float fpb);

protected:
	IDirect3DDevice8*	m_pd3dDevice;
    XMVDecoder *		m_pWMVDecoder;	// WMV Decoder
	XMVVIDEO_DESC		m_wmvVideoInfo; // Video format info
	XMVAUDIO_DESC		m_wfx;			// Audio format info

	REFERENCE_TIME		m_rtStartTime;	// Start time
	BOOL				m_bFirstFrame;	// BOOL to track first frame
	BOOL				m_bVideoGoing;

	DWORD				m_rtCurVideoTime;	// Current Video time

    IDirectSound8 *		m_pDSound;						// DirectSound object
    IDirectSoundStream*	m_pStream;						// Stream for playback

    LPDIRECT3DTEXTURE8  m_pOverlay[2];  // Overlay textures
    LPDIRECT3DSURFACE8  m_pSurface[2];  // Overlay Surfaces
    DWORD               m_dwCurrent;    // Current surface

	LPDIRECT3DSURFACE8  m_pTmpSurface;

	RECT				m_SrcRect;
    RECT				m_DestRect;

	D3DVIEWPORT8		m_vp;
};

//#endif
