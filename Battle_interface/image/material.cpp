#include "material.h"
#include "image.h"
#include "imgrender.h"

BIImageMaterial::BIImageMaterial( VDX8RENDER* pRS, BIImageRender* pImgRender ) :
	m_apImage(_FL)
{
	m_pRS = pRS;
	m_pImageRender = pImgRender;
	m_sTechniqueName = "battle_tex_col_Rectangle";

	m_nTextureID = -1;
	m_nVBufID = -1;
	m_nIBufID = -1;
	m_nVertexQuantity = 0;
	m_nTriangleQuantity = 0;

	m_bMakeBufferUpdate = false;
	m_bDeleteEverythink = false;

	m_nMinPrioritet = ImagePrioritet_DefaultValue;
	m_nMaxPrioritet = ImagePrioritet_DefaultValue;
}

BIImageMaterial::~BIImageMaterial()
{
	Release();
}

void BIImageMaterial::Render(long nBegPrior, long nEndPrior)
{
	if( m_bMakeBufferUpdate )
		RemakeBuffers();

	long nStartIndex = 0;
	long nTriangleQuantity = m_nTriangleQuantity;
	if( !GetOutputRangeByPrioritet(nBegPrior,nEndPrior, nStartIndex,nTriangleQuantity) ) return;

	if( m_nTextureID>=0 && m_nVBufID>=0 && m_nIBufID>=0 )
	{
		m_pRS->TextureSet( 0, m_nTextureID );
		m_pRS->DrawBuffer( m_nVBufID, sizeof(BI_IMAGE_VERTEX), m_nIBufID, 0,m_nVertexQuantity, nStartIndex,nTriangleQuantity, m_sTechniqueName );
	}
}

const BIImage* BIImageMaterial::CreateImage( BIImageType type, dword color, FRECT& uv, long nLeft,long nTop, long nRight,long nBottom, long nPrior )
{
	BIImage* pImg = NEW BIImage( m_pRS, this );
	Assert( pImg );
	pImg->SetColor( color );
	pImg->SetPosition( nLeft, nTop, nRight, nBottom );
	pImg->SetUV( uv );
	pImg->SetType( type );
	pImg->SetPrioritet( nPrior );
	InsertImageToList( pImg );
	//m_apImage.Add( pImg );
	m_bMakeBufferUpdate = true;
	RecalculatePrioritetRange();
	return pImg;
}

void BIImageMaterial::DeleteImage( const BIImage* pImg )
{
	if( m_bDeleteEverythink ) return;
	for( long n=0; n<m_apImage; n++ )
		if( m_apImage[n] == pImg )
		{
			m_apImage.DelIndex( n );
			m_bMakeBufferUpdate = true;
			RecalculatePrioritetRange();
			break;
		}
}

void BIImageMaterial::SetTexture( const char* pcTextureName )
{
	if( pcTextureName == m_sTextureName ) return; // уже стоит эта текстура
	m_sTextureName = pcTextureName;
	TEXTURE_RELEASE( m_pRS, m_nTextureID );
	m_nTextureID = m_pRS->TextureCreate( pcTextureName );
}

void BIImageMaterial::ReleaseAllImages()
{
	m_bDeleteEverythink = true;
	m_apImage.DelAllWithPointers();
	m_bDeleteEverythink = false;
	m_bMakeBufferUpdate = true;
}

void BIImageMaterial::Release()
{
	m_bDeleteEverythink = true;
	m_pImageRender->DeleteMaterial( this );
	m_apImage.DelAllWithPointers();

	TEXTURE_RELEASE( m_pRS, m_nTextureID );
	VERTEX_BUFFER_RELEASE( m_pRS, m_nVBufID );
	INDEX_BUFFER_RELEASE( m_pRS, m_nVBufID );

	m_nVertexQuantity = 0;
	m_nTriangleQuantity = 0;
}

void BIImageMaterial::UpdateImageBuffers( long nStartIdx, long nEndIdx )
{
	// fool check
	if( m_nIBufID<0 || m_nVBufID<0 ) return;
	if( nStartIdx >= (long)m_apImage.Size() ) return;
	if( nEndIdx >= (long)m_apImage.Size() ) nEndIdx = m_apImage.Size() - 1;

	word* pT = (word*)m_pRS->LockIndexBuffer( m_nIBufID );
	BI_IMAGE_VERTEX* pV = (BI_IMAGE_VERTEX*)m_pRS->LockVertexBuffer( m_nVBufID );

	// get before
	long nV = 0;
	long nT = 0;
	for( long n=0; n<nStartIdx; n++ )
	{
		nV += m_apImage[n]->GetVertexQuantity();
		nT += m_apImage[n]->GetTriangleQuantity();
	}

	for( ; n<=nEndIdx; n++ )
	{
		m_apImage[n]->FillBuffers( pV,pT, nV,nT );
	}

	m_pRS->UnLockVertexBuffer( m_nVBufID );
	m_pRS->UnLockIndexBuffer( m_nIBufID );
}

void BIImageMaterial::RemakeBuffers()
{
	long nVQ = 0;
	long nTQ = 0;
	for( long n=0; n<m_apImage; n++ )
	{
		nVQ += m_apImage[n]->GetVertexQuantity();
		nTQ += m_apImage[n]->GetTriangleQuantity();
	}
	if( m_nVertexQuantity != nVQ )
	{
		m_pRS->ReleaseVertexBuffer( m_nVBufID );
		m_nVBufID = m_pRS->CreateVertexBuffer( BI_IMAGE_VERTEX_FORMAT, nVQ*sizeof(BI_IMAGE_VERTEX), D3DUSAGE_WRITEONLY );
		m_nVertexQuantity = nVQ;
	}
	if( m_nTriangleQuantity != nTQ )
	{
		m_pRS->ReleaseIndexBuffer( m_nIBufID );
		m_nIBufID = m_pRS->CreateIndexBuffer( nTQ*3 * sizeof(word) );
		m_nTriangleQuantity = nTQ;
	}
	UpdateImageBuffers( 0, m_apImage.Size()-1 );
	m_bMakeBufferUpdate = false;
}

bool BIImageMaterial::GetOutputRangeByPrioritet(long nBegPrior,long nEndPrior, long &nStartIndex,long &nTriangleQuantity)
{
	if( m_apImage.Size()==0 ) return false;
	if( m_apImage[0]->GetPrioritet() > nEndPrior || m_apImage.LastE()->GetPrioritet() < nBegPrior ) return false;

	nStartIndex = 0;
	for( long n=0; n<m_apImage; n++ )
	{
		if( m_apImage[n]->GetPrioritet() >= nBegPrior )
			break;
		nStartIndex += m_apImage[n]->GetTriangleQuantity() * 3;
	}

	nTriangleQuantity = 0;
	for( ; n<m_apImage; n++ )
	{
		if( m_apImage[n]->GetPrioritet() > nEndPrior )
			break;
		nTriangleQuantity += m_apImage[n]->GetTriangleQuantity();
	}

	return (nTriangleQuantity != 0);
}

void BIImageMaterial::RecalculatePrioritetRange()
{
	if( m_apImage.Size()==0 ) return;
	long oldMin = m_nMinPrioritet;
	long oldMax = m_nMaxPrioritet;
	m_nMinPrioritet = m_nMaxPrioritet = m_apImage[0]->GetPrioritet();
	for( long n=1; n<m_apImage; n++ )
	{
		long p = m_apImage[n]->GetPrioritet();
		if( p < m_nMinPrioritet ) m_nMinPrioritet = p;
		else if( p > m_nMaxPrioritet ) m_nMaxPrioritet = p;
	}
	if( oldMin!=m_nMinPrioritet || oldMax!=m_nMaxPrioritet )
		m_pImageRender->MaterialSorting();
}

void BIImageMaterial::InsertImageToList( BIImage* pImg )
{
	Assert(pImg);
	long nPrior = pImg->GetPrioritet();
	for( long n=0; n<m_apImage; n++ )
		if( m_apImage[n]->GetPrioritet() > nPrior )
			break;
	m_apImage.Insert( n );
	m_apImage[n] = pImg;
}
