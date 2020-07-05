#include "image.h"
#include "material.h"
#include "imgrender.h"

BIImage::BIImage( VDX8RENDER* rs, BIImageMaterial* pMaterial ) :
	m_aRelPos(_FL)
{
	m_pRS = rs;
	m_pMaterial = pMaterial;
	m_nVertexQuantity = 4;
	m_nTriangleQuantity = 2;
	m_nPrioritet = ImagePrioritet_DefaultValue;
}

BIImage::~BIImage()
{
	Release();
}

void BIImage::FillBuffers( BI_IMAGE_VERTEX* pV, word* pT, long& nV, long& nT )
{
	long n;
	long ni = nT * 3;
	long nv = nV;

	// index buffer
	if( m_eType == BIType_square ) {
		for( n=2; n<m_aRelPos; n++ ) // квадратик делается по принципу Triangle Strip
		{
			pT[ni++] = (word)(nv + n-2);
			pT[ni++] = (word)(nv + n-1);
			pT[ni++] = (word)(nv + n);
		}
		nT += n-2;
	}
	else if( m_eType == BIType_clocksquare ) { // квадратные "часы" делаются по принципу Triangle Fan
		for( n=2; n<m_aRelPos; n++ )
		{
			pT[ni++] = (word)(nv);
			pT[ni++] = (word)(nv + n-1);
			pT[ni++] = (word)(nv + n);
		}
		nT += n-2;
	} else return;

	// vertex buffer
	for( n=0; n<m_aRelPos; n++ )
	{
		pV[nv].pos.x = CalculateMidPos( m_BasePos.left, m_BasePos.right, m_aRelPos[n].x );
		pV[nv].pos.y = CalculateMidPos( m_BasePos.top, m_BasePos.bottom, m_aRelPos[n].y );
		pV[nv].pos.z = 1.f;
		pV[nv].w = 0.5f;
		pV[nv].col = m_dwColor;
		pV[nv].tu = CalculateMidPos( m_BaseUV.left, m_BaseUV.right, m_aRelPos[n].x );
		pV[nv].tv = CalculateMidPos( m_BaseUV.top, m_BaseUV.bottom, m_aRelPos[n].y );
		nv++;
	}
	nV += n;
}

void BIImage::SetColor( dword color )
{
	m_dwColor = color;
	m_pMaterial->UpdateFlagOn();
}

void BIImage::SetPosition( long nLeft, long nTop, long nRight, long nBottom )
{
	m_pMaterial->GetImgRender()->TranslateBasePosToRealPos((float)nLeft,(float)nTop, m_BasePos.left,m_BasePos.top);
	m_pMaterial->GetImgRender()->TranslateBasePosToRealPos((float)nRight,(float)nBottom, m_BasePos.right,m_BasePos.bottom);
	m_pMaterial->UpdateFlagOn();
}

void BIImage::Set3DPosition( const CVECTOR& vPos, float fWidth, float fHeight )
{
}

void BIImage::SetUV( const FRECT& uv )
{
	m_BaseUV = uv;
	m_pMaterial->UpdateFlagOn();
}

void BIImage::SetType( BIImageType type )
{
	m_eType = type;
	if( type == BIType_square ) {
		m_nVertexQuantity = 4;
		m_nTriangleQuantity = 2;
		CutSide(0.f,0.f,0.f,0.f);
	}
	if( type == BIType_clocksquare ) {
		m_nVertexQuantity = 3;
		m_nTriangleQuantity = 1;
		CutClock(0.f,1.f,0.5f);
	}
	m_pMaterial->UpdateFlagOn();
}

void BIImage::CutSide( float fleft, float fright, float ftop, float fbottom )
{
	if( m_eType != BIType_square ) return;
	m_aRelPos.DelAll();
	FPOINT fp;
	fp.x = fleft;		fp.y = 1.f-fbottom;		m_aRelPos.Add( fp );
	fp.x = fleft;		fp.y = ftop;			m_aRelPos.Add( fp );
	fp.x = 1.f-fright;	fp.y = 1.f-fbottom;		m_aRelPos.Add( fp );
	fp.x = 1.f-fright;	fp.y = ftop;			m_aRelPos.Add( fp );
	m_pMaterial->UpdateFlagOn();
}

void BIImage::CutClock( float fBegin, float fEnd, float fFactor )
{
	if( m_eType != BIType_clocksquare ) return;
	m_aRelPos.DelAll();
	FPOINT fp;
	fp.x = 0.5f;		fp.y = 0.5f;			m_aRelPos.Add( fp );
	float fEndAng = fBegin + (fEnd-fBegin) * fFactor;
	// первая/начальная точка на часах
	m_aRelPos.Add( GetClockPoint( fBegin, fp ) );
	// следующие углы
	if( fBegin < fEndAng )
		for( float fAng=GetNextClockCorner(fBegin); fAng<fEndAng; fAng=GetNextClockCorner(fAng) )
			m_aRelPos.Add( GetClockPoint( fAng, fp ) );
	else if( fBegin > fEndAng )
		for( float fAng=GetPrevClockCorner(fBegin); fAng>fEndAng; fAng=GetPrevClockCorner(fAng) )
			m_aRelPos.Add( GetClockPoint( fAng, fp ) );
	// последняя/конечная точка на часах
	m_aRelPos.Add( GetClockPoint( fEndAng, fp ) );
	m_nVertexQuantity = m_aRelPos.Size();
	m_nTriangleQuantity = m_nVertexQuantity - 2;
	m_pMaterial->UpdateFlagOn();
}

FPOINT& BIImage::GetClockPoint( float fAng, FPOINT &fp )
{
	if( fAng < -1.f ) {
		fp.x = 0.5f; fp.y = 0.f;
	} else if( fAng <= -0.875f ) {
		fp.x = (1.125f + fAng) * 4.f;
		fp.y = 0.f;
	} else if( fAng <= -0.625f ) {
		fp.x = 1.f;
		fp.y = (0.875f + fAng) * 4.f;
	} else if( fAng <= -0.375f ) {
		fp.x = (-0.375f - fAng) * 4.f;
		fp.y = 1.f;
	} else if( fAng <= -0.125f ) {
		fp.x = 0.f;
		fp.y = (-0.125f - fAng) * 4.f;
	} else if( fAng <= 0.125f ) {
		fp.x = (0.125f + fAng) * 4.f;
		fp.y = 0.f;
	} else if( fAng <= 0.375f ) {
		fp.x = 1.f;
		fp.y = (-0.125f + fAng) * 4.f;
	} else if( fAng <= 0.625f ) {
		fp.x = (0.625f - fAng) * 4.f;
		fp.y = 1.f;
	} else if( fAng <= 0.875f ) {
		fp.x = 0.f;
		fp.y = (0.875f - fAng) * 4.f;
	} else if( fAng <= 1.f ) {
		fp.x = (fAng - 0.875f) * 4.f;
		fp.y = 0.f;
	} else {
		fp.x = 0.5f; fp.y = 0.f;
	}

	return fp;
}

float BIImage::GetNextClockCorner( float fAng )
{
	for( float f=-0.875f; f<1.f; f+=0.25f )
		if( fAng < f ) return f;
	return fAng + .25f;
}

float BIImage::GetPrevClockCorner( float fAng )
{
	for( float f=0.875f; f>-1.f; f-=0.25f )
		if( fAng > f ) return f;
	return fAng - .25f;
}

void BIImage::Release()
{
	if( m_pMaterial )
		m_pMaterial->DeleteImage( this );
}
