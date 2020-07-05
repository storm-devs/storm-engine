#ifndef BI_IMAGE_H
#define BI_IMAGE_H

#include "defines.h"

class BIImageMaterial;

class BIImage : public IBIImage
{
public:
	BIImage( VDX8RENDER* rs, BIImageMaterial* pMaterial );
	~BIImage();

	long GetVertexQuantity() {return m_nVertexQuantity;}
	long GetTriangleQuantity() {return m_nTriangleQuantity;}
	void FillBuffers( BI_IMAGE_VERTEX* pV, word* pT, long& nV,long& nT );

	void SetColor( dword color );
	void SetPosition( long nLeft, long nTop, long nRight, long nBottom );
	void Set3DPosition( const CVECTOR& vPos, float fWidth, float fHeight );
	void SetUV( const FRECT& uv );
	void SetType( BIImageType type );

	void CutSide( float fleft, float fright, float ftop, float fbottom );
	void CutClock( float fBegin, float fEnd, float fFactor );
	FPOINT& GetClockPoint( float fAng, FPOINT &fp );
	float GetNextClockCorner( float fAng );
	float GetPrevClockCorner( float fAng );

	long GetPrioritet() {return m_nPrioritet;}
	void SetPrioritet(long nPrior) {m_nPrioritet=nPrior;}

protected:
	float CalculateMidPos( float fMin, float fMax, float fK ) {return fMin + fK * (fMax - fMin);}
	void Release();

	VDX8RENDER* m_pRS;
	BIImageMaterial* m_pMaterial;

	long m_nVertexQuantity;
	long m_nTriangleQuantity;

	FRECT m_BasePos;
	FRECT m_BaseUV;
	dword m_dwColor;

	array<FPOINT> m_aRelPos;
	BIImageType m_eType;

	long m_nPrioritet;
};

#endif
