#ifndef _WMVIDEO_H
#define _WMVIDEO_H

#include "wmvbase.h"

//-----------------------------------------------------------------------------
// Name: class VideoToTexture
// Desc: play video into texture
//-----------------------------------------------------------------------------
class WMVideoPlay : public xiBaseVideo
{
	VDX8RENDER  *rs;
	WMVBase * pBase;

public:
	 WMVideoPlay();
	~WMVideoPlay();
	bool Init();
    void Execute(dword Delta_Time);
	void Realize(dword Delta_Time);
    dword _cdecl ProcessMessage(MESSAGE & message);

protected:
/*    bool Initialize(const char* cWMVfileName);
	bool FileOpen(const char* cWMVfileName);
	void Release();
    bool FrameUpdate();
	bool IsReady();			// Ready to switch?
	HRESULT DecodeNext();	// Decode next frame
	bool FindFreePacket(DWORD * pdwIndex);
	bool GetTexture(IDirect3DTexture8 * pTexture);

	IDirect3DDevice8*	m_pd3dDevice;
    LPWMVDECODER		m_pWMVDecoder;	// WMV Decoder
	WMVVIDEOINFO		m_wmvVideoInfo;
	WAVEFORMATEX		m_wfx;			// Audio format info
	REFERENCE_TIME		m_rtDuration;	// Video duration

	REFERENCE_TIME		m_rtStartTime;	// Start time
	BOOL				m_bFirstFrame;	// BOOL to track first frame

    VIDEOFRAME			m_videoframe;

    IDirectSound8 *		m_pDSound;						// DirectSound object
    IDirectSoundStream*	m_pStream;						// Stream for playback
    DWORD               m_adwStatus[NUM_PACKETS];       // Packet status
    BYTE*               m_pbSampleData;                 // Audio buffer

    LPDIRECT3DTEXTURE8  m_pOverlay[2];  // Overlay textures
    LPDIRECT3DSURFACE8  m_pSurface[2];  // Overlay Surfaces
    DWORD               m_dwCurrent;    // Current surface

	RECT				m_SrcRect;
    RECT				m_DestRect;
*/
};

#endif