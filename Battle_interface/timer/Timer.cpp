#include "timer.h"
#include "..\image\image.h"
#include "..\image\imgrender.h"
#include "..\utils.h"

BITimer::BITimer()
{
	SetBeginData();
}

BITimer::~BITimer()
{
	Release();
}

bool BITimer::Init()
{
	if( !ReadAndCreate() ) return false;
	return true;
}

void BITimer::Realize(dword delta_time)
{
	if( m_fCurTimerCounter>0.f && m_fMaxTimerCounter>0.f ) {
		m_fCurTimerCounter -= delta_time * 0.001f;
		if( m_fCurTimerCounter > 0.f ) {
			if( m_pForeImage ) m_pForeImage->CutSide( 0.f, 1.f-m_fCurTimerCounter/m_fMaxTimerCounter, 0.f, 0.f );
			if( m_pImgRndr ) {
				m_pRender->MakePostProcess();
				m_pImgRndr->Render();
			}
		} else
			CloseTimer( true );
	}
}

dword _cdecl BITimer::ProcessMessage(MESSAGE & message)
{
	long nMsgCod = message.Long();
	switch( nMsgCod )
	{
	case 0:
		{
			char eventName[512];
			float fTime = message.Float();
			message.String( sizeof(eventName)-1, eventName );
			OpenTimer( fTime, eventName );
		}
	break;
	case 1:
		CloseTimer(false);
	break;
	}
	return 0;
}

bool BITimer::ReadAndCreate()
{
	if( !m_pImgRndr ) return false;

	// read position
	RECT rBackPos,rForePos;
	rBackPos.left = rBackPos.top = rBackPos.right = rBackPos.bottom = 0;
	rForePos.left = rForePos.top = rForePos.right = rForePos.bottom = 0;
	BIUtils::ReadRectFromAttr( AttributesPointer, "timerpos", rBackPos, rBackPos );
	BIUtils::ReadRectFromAttr( AttributesPointer, "timeroffset", rForePos, rForePos );
	rForePos.left = rBackPos.left + rForePos.left;
	rForePos.top = rBackPos.top + rForePos.top;
	rForePos.right = rBackPos.right - rForePos.right;
	rForePos.bottom = rBackPos.bottom - rForePos.bottom;

	// read uv
	FRECT rBackUV,rForeUV;
	rBackUV.left = rBackUV.top = rForeUV.left = rForeUV.top = 0.f;
	rBackUV.right = rBackUV.bottom = rForeUV.right = rForeUV.bottom = 1.f;
	BIUtils::ReadRectFromAttr( AttributesPointer, "timerbackuv", rBackUV, rBackUV );
	BIUtils::ReadRectFromAttr( AttributesPointer, "timerforeuv", rForeUV, rForeUV );

	// read texture & color
	char* pcBackTexture = AttributesPointer ? AttributesPointer->GetAttribute("timerbacktexture") : 0;
	char* pcForeTexture = AttributesPointer ? AttributesPointer->GetAttribute("timerforetexture") : 0;
	dword dwColorBack = AttributesPointer ? AttributesPointer->GetAttributeAsDword("timerbackcolor",0xFFFFFFFF) : 0xFFFFFFFF;
	dword dwColorFore = AttributesPointer ? AttributesPointer->GetAttributeAsDword("timerforecolor",0xFFFFFFFF) : 0xFFFFFFFF;

	// create
	if( pcBackTexture )
		m_pBackImage = m_pImgRndr->CreateImage(BIType_square,pcBackTexture, dwColorBack, rBackUV, rBackPos );
	if( pcForeTexture )
		m_pForeImage = m_pImgRndr->CreateImage(BIType_square,pcForeTexture, dwColorFore, rForeUV, rForePos );
	return true;
}

void BITimer::SetBeginData()
{
	m_fCurTimerCounter = 0.f;
	m_fMaxTimerCounter = 0.f;

	m_pRender = (VDX8RENDER*)api->CreateService("dx8render");
	Assert( m_pRender );
	m_pImgRndr = NEW BIImageRender( m_pRender );
	m_pBackImage = 0;
	m_pForeImage = 0;
}

void BITimer::Release()
{
	DELETE( m_pBackImage );
	DELETE( m_pForeImage );
	DELETE( m_pImgRndr );
}

void BITimer::OpenTimer( float fTime, const char* pcEventName )
{
	m_fCurTimerCounter = m_fMaxTimerCounter = fTime;
	m_sEventName = pcEventName;
}

void BITimer::CloseTimer( bool bTimeOut )
{
	m_fCurTimerCounter = 0.f;
	m_pForeImage->CutSide( 0.f, 0.f, 0.f, 0.f );
	if( bTimeOut ) {
		api->Event( (char*)m_sEventName.GetBuffer(), 0 );
	}
}
