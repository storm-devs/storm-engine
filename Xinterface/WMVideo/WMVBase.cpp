#include <assert.h>
#include "WMVBase.h"
#include "..\\..\\common_h\\memop.h"

#pragma comment(lib,"Xmv.lib")

#define VIDEO_DIRECTORY		"d:\\Resource\\Videos"
#define FAILRETURN(str,hr)	if((hr)!=D3D_OK) { /*api->Trace(str);*/ return false;}

WMVBase::WMVBase()
{
    m_pWMVDecoder   = NULL;
    m_pDSound       = NULL;
    m_pStream       = NULL;

	m_pOverlay[0] = NULL;
	m_pOverlay[1] = NULL;
	m_pSurface[0] = NULL;
	m_pSurface[1] = NULL;

	m_pTmpSurface = NULL;

	m_bVideoGoing = FALSE;
}

WMVBase::~WMVBase()
{
	Release();
}

float g_fWMVAspectRatioK = 1.f;
bool WMVBase::Initialize(IDirect3DDevice8* pd3dDev,const char* cWMVfileName)
{
	HRESULT hr = D3D_OK;

	m_pd3dDevice = pd3dDev;
	assert(m_pd3dDevice);

    // Create DirectSound
	FAILRETURN("Can`t create DirectSound",DirectSoundCreate( NULL, &m_pDSound, NULL ));

	if( !FileOpen(cWMVfileName) ) return false;

	m_SrcRect.left    = 0;
    m_SrcRect.top     = 0;
    m_SrcRect.right   = m_wmvVideoInfo.Width;
    m_SrcRect.bottom  = m_wmvVideoInfo.Height;

	m_pd3dDevice->GetViewport(&m_vp);

	float fAspect = (m_wmvVideoInfo.Width*3.f) / (4.f*m_wmvVideoInfo.Height);

	if(fAspect<=1.f)
	{
		m_DestRect.left   = (long)(m_vp.Width*(1.f - fAspect)*.5f);
		m_DestRect.top    = 0;
		m_DestRect.right  = m_DestRect.left + (long)(m_vp.Width*fAspect);
		m_DestRect.bottom = m_vp.Height;
	}
	else
	{
		fAspect = 1.f/fAspect;
		m_DestRect.left   = 0;
		m_DestRect.top    = (long)(m_vp.Height*(1.f - fAspect)*.5f);
		m_DestRect.right  = m_vp.Width;
		m_DestRect.bottom = m_DestRect.top + (long)(m_vp.Height*fAspect);
	}

/*	api->Trace("========================================");
	api->Trace("Screen Size %d x %d", m_vp.Width, m_vp.Height);
	api->Trace("Video %s", cWMVfileName);
	api->Trace("Source Size %d x %d", m_wmvVideoInfo.Width, m_wmvVideoInfo.dwHeight);
	api->Trace("OutRect %d,%d - %d,%d", m_DestRect.left, m_DestRect.top, m_DestRect.right, m_DestRect.bottom);
	api->Trace("========================================");
*/
    // Enable the Direct3D overlay surface
    m_pd3dDevice->EnableOverlay(true);

	m_pd3dDevice->CreateImageSurface(m_wmvVideoInfo.Width,m_wmvVideoInfo.Height,D3DFMT_YUY2,&m_pTmpSurface);

    for( int i = 0; i < 2; i++ )
    {
        m_pd3dDevice->CreateTexture( m_wmvVideoInfo.Width,
                                     m_wmvVideoInfo.Height,
                                     1,
                                     0,
                                     D3DFMT_YUY2,
                                     0,
                                     &m_pOverlay[i] );
        m_pOverlay[i]->GetSurfaceLevel( 0, &m_pSurface[i] );
    }
    m_dwCurrent = 0;

/*	XMVRESULT xr = XMV_NOFRAME;
	while ( xr != XMV_ENDOFFILE && xr != XMV_FAIL )
	{
		m_pWMVDecoder->GetNextFrame( m_pSurface[m_dwCurrent], &xr, NULL );
		
		if ( !m_pd3dDevice->GetOverlayUpdateStatus() )
		{
			m_pd3dDevice->BlockUntilVerticalBlank();
		}
		if(xr==XMV_NEWFRAME)
		{
			m_pd3dDevice->UpdateOverlay( m_pSurface[m_dwCurrent], &m_SrcRect, &m_DestRect, FALSE, 0 );
		}			   
		m_dwCurrent ^= 1;
	}
*/
	return true;
	//return DecodeNext()==S_OK;
}

bool WMVBase::FileOpen(const char* cWMVfileName)
{
    HRESULT hr;
    
	hr = XMVDecoder_CreateDecoderForFile( 0,
										 cWMVfileName, &m_pWMVDecoder );
	FAILRETURN("Can`t create XMV decoder",hr);

	// Get information about the video
	m_pWMVDecoder->GetVideoDescriptor( &m_wmvVideoInfo );
	api->Trace("Audio stream quantity = %d", m_wmvVideoInfo.AudioStreamCount);
//	m_pWMVDecoder->GetAudioDescriptor( 0, &m_wfx );

//	m_pWMVDecoder->SetSynchronizationStream(0);
	m_pWMVDecoder->EnableAudioStream( 0, 0, NULL, &m_pStream );
//    m_pStream->Pause( DSSTREAMPAUSE_PAUSE );

//    m_bFirstFrame = TRUE;
//    m_rtStartTime = 0;

    return true;
}

HRESULT WMVBase::DecodeNext()
{
	if(!m_pWMVDecoder) return S_FALSE;

    HRESULT hr;
//    bool    bGotVideo = false;

	XMVRESULT wmvResult;
	hr = m_pWMVDecoder->GetNextFrame(m_pTmpSurface, &wmvResult, &m_rtCurVideoTime);
	if( hr == S_FALSE )
	{
		// Tell the stream we're done
		//m_pStream->Discontinuity();
		return S_FALSE;
	}

	if( wmvResult == XMV_NEWFRAME )
	{
		m_dwCurrent ^= 1;
		m_pd3dDevice->CopyRects(m_pTmpSurface, NULL, 0, m_pSurface[m_dwCurrent], NULL);
		m_bVideoGoing = true;
	}

	if( wmvResult == XMV_FAIL || wmvResult == XMV_ENDOFFILE ) return S_FALSE;

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: FrameUpdate
// Desc: Performs per-frame updates
//-----------------------------------------------------------------------------
bool WMVBase::FrameUpdate()
{
	XMVRESULT xr = XMV_NOFRAME;
//	while ( xr != XMV_ENDOFFILE && xr != XMV_FAIL )
	{
		m_pWMVDecoder->GetNextFrame( m_pSurface[m_dwCurrent], &xr, NULL );
		
		if ( !m_pd3dDevice->GetOverlayUpdateStatus() )
		{
			m_pd3dDevice->BlockUntilVerticalBlank();
		}
		if(xr==XMV_NEWFRAME)
		{
			m_pd3dDevice->UpdateOverlay( m_pSurface[m_dwCurrent], &m_SrcRect, &m_DestRect, FALSE, 0 );
			m_dwCurrent ^= 1;
		}			   
	}

	if( xr == XMV_ENDOFFILE || xr == XMV_FAIL ) return false;
	return true;

//    if( m_bFirstFrame )
//    {
//        m_bFirstFrame = FALSE;
//        m_pStream->Pause( DSSTREAMPAUSE_RESUME );
//        m_pDSound->GetTime( &m_rtStartTime );
//    }

    // Decode the next frame

    // If we're ready for the next frame...
//	if(!IsReady()) return true;
	if(DecodeNext() == S_FALSE) return false;

    // Update the overlay surface to use this texture
	//if( m_bVideoGoing )
	if ( !m_pd3dDevice->GetOverlayUpdateStatus() )
	{
		m_pd3dDevice->BlockUntilVerticalBlank();
	}
	m_pd3dDevice->UpdateOverlay( m_pSurface[m_dwCurrent], &m_SrcRect, &m_DestRect, FALSE, 0 );

    return true;
}

void WMVBase::Release()
{
	if(	m_pSurface[0] != NULL )
	{
		m_pSurface[0]->Release();
		m_pSurface[0] = NULL;
	}
	if(	m_pSurface[1] != NULL )
	{
		m_pSurface[1]->Release();
		m_pSurface[1] = NULL;
	}
	if(	m_pOverlay[0] != NULL )
	{
		m_pOverlay[0]->Release();
		m_pOverlay[0] = NULL;
	}
	if(	m_pOverlay[1] != NULL )
	{
		m_pOverlay[1]->Release();
		m_pOverlay[1] = NULL;
	}

	if( m_pTmpSurface != NULL )
	{
		m_pTmpSurface->Release();
		m_pTmpSurface = NULL;
	}

    if( m_pStream )
    {	m_pStream->Pause( DSSTREAMPAUSE_RESUME );
        m_pStream->Release();
        m_pStream = NULL;
    }

	m_pd3dDevice->EnableOverlay(false);
    if( m_pWMVDecoder )
    {	m_pWMVDecoder->CloseDecoder();
        m_pWMVDecoder = NULL;
    }

    if( m_pDSound )
	{	m_pDSound->Release();
		m_pDSound = NULL;
	}

}

bool WMVBase::IsReady()
{
    assert( m_pWMVDecoder );
    DirectSoundDoWork();

    // Calculate current time
//    REFERENCE_TIME rtCurrent;
//    m_pDSound->GetTime( &rtCurrent );
//    rtCurrent -= m_rtStartTime;

    // If we're ready to display next frame, then let the caller know
	return true;
//    if( rtCurrent >= m_rtCurVideoTime )	return true;
    
//    return false;
}

void WMVBase::SetShowParameters(float ftl,float ftt,float ftr,float ftb, float fpl,float fpt,float fpr,float fpb)
{
    m_SrcRect.left   = (long)(m_wmvVideoInfo.Width * ftl);
    m_SrcRect.top  = (long)(m_wmvVideoInfo.Height * ftt);
    m_SrcRect.right   = (long)(m_wmvVideoInfo.Width * ftr);
    m_SrcRect.bottom  = (long)(m_wmvVideoInfo.Height * ftb);

    m_DestRect.left = (long)(m_vp.Width * fpl);
	m_DestRect.top = (long)(m_vp.Height * fpt);
    m_DestRect.right = (long)(m_vp.Width * fpr);
	m_DestRect.bottom = (long)(m_vp.Height * fpb);
}
