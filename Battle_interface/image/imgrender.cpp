#include "imgrender.h"
#include "string.h"

BIImageRender::BIImageRender(VDX8RENDER* pRS) :
	m_apMaterial(_FL),
	m_apStrings(_FL)
{
	m_pRS = pRS;
	m_fHScale = m_fVScale = 1.f;
	m_fHOffset = m_fVOffset = 0.f;
}

BIImageRender::~BIImageRender()
{
	Release();
}

void BIImageRender::Render()
{
	if( GetFirstPrioritetRange() ) do {
		CutPrioritetRangeByStrings();

		for( long n=0; n<m_apMaterial; n++ )
			m_apMaterial[n]->Render(m_nBeginOutputPrioritet, m_nEndOutputPrioritet);

		for( n=0; n<m_apStrings; n++ )
			((BIString*)m_apStrings[n])->RenderPrioritetRange(m_nBeginOutputPrioritet, m_nEndOutputPrioritet);

	} while( GetNextPrioritetRange() );
}

dword _cdecl BIImageRender::ProcessMessage(MESSAGE & message)
{
	return 0;
}

IBIImage* BIImageRender::CreateImage( BIImageType type, const char* pcTextureName, dword color, FRECT& uv, long nLeft,long nTop, long nRight,long nBottom, long nPrior, const char* pcTechniqueName )
{
	BIImageMaterial* pMaterial = CreateMaterial( pcTextureName, pcTechniqueName );
	if( pMaterial )
		return (IBIImage*)pMaterial->CreateImage( type, color, uv, nLeft,nTop, nRight,nBottom, nPrior );
	return 0;
}

IBIImage* BIImageRender::CreateImage( BIImageType type, const char* pcTextureName, dword color, FRECT& uv, RECT& pos, long nPrior, const char* pcTechniqueName )
{
	BIImageMaterial* pMaterial = CreateMaterial( pcTextureName, pcTechniqueName );
	if( pMaterial )
		return (IBIImage*)pMaterial->CreateImage( type, color, uv, pos.left,pos.top, pos.right,pos.bottom, nPrior );
	return 0;
}

BIImageMaterial* BIImageRender::FindMaterial( const char* pcTextureName, const char* pcTechniqueName )
{
	for( long n=0; n<m_apMaterial; n++ )
		if( m_apMaterial[n]->IsUseTexture(pcTextureName) &&
			m_apMaterial[n]->IsUseTechnique(pcTechniqueName) )
			return m_apMaterial[n];
	return null;
}

BIImageMaterial* BIImageRender::CreateMaterial( const char* pcTextureName, const char* pcTechniqueName )
{
	BIImageMaterial* pMaterial = FindMaterial( pcTextureName, pcTechniqueName?pcTechniqueName:"battle_tex_col_Rectangle" );
	if( !pMaterial )
	{
		pMaterial = NEW BIImageMaterial( m_pRS, this );
		Assert( pMaterial );
		pMaterial->SetTexture( pcTextureName );
		pMaterial->SetTechnique( pcTechniqueName );
		pMaterial->UpdateFlagOn();
		for( long n=0; n<m_apMaterial; n++ )
			if( m_apMaterial[n]->GetMinPrioritet() > pMaterial->GetMinPrioritet() )
				break;
		m_apMaterial.Insert(n);
		m_apMaterial[n] = pMaterial;
	}
	return pMaterial;
}

void BIImageRender::DeleteMaterial( BIImageMaterial* pMat )
{
	long n = m_apMaterial.Find( pMat );
	if( n >= 0 )
		m_apMaterial.DelIndex( n );
}

void BIImageRender::ReleaseAllImages()
{
	for( long n=0; n<m_apMaterial; n++ )
		m_apMaterial[n]->ReleaseAllImages();
}

long BIImageRender::GetImageQuantity()
{
	long nRetVal = 0;
	for( long n=0; n<m_apMaterial; n++ )
		nRetVal += m_apMaterial[n]->GetImageQuantity();
	return nRetVal;
}

void BIImageRender::MaterialSorting()
{
	for( bool bContinue=true; bContinue; )
	{
		bContinue = false;
		for( long n=1; n<m_apMaterial; n++ )
		{
			if( m_apMaterial[n]->GetMinPrioritet() < m_apMaterial[n-1]->GetMinPrioritet() )
			{
				m_apMaterial.Swap(n-1, n);
				bContinue = true;
			}
		}
	}
}

void BIImageRender::ChangeMaterialPosByPrioritet(BIImageMaterial* pMat)
{
	MaterialSorting();
}

IBIString* BIImageRender::CreateString( const char* text, const char* font_name, float font_scale, dword font_color, long valign, long halign, long nLeft, long nTop, long nRight, long nBottom, long nPrior)
{
	BIString* pStr = NEW BIString(this,m_pRS);
	if( !pStr ) return pStr;

	pStr->SetFont( font_name );
	pStr->SetColor( font_color );
	pStr->SetAlign( halign, valign );
	pStr->SetPosition( nLeft, nTop, nRight, nBottom );
	pStr->SetPrioritet( nPrior );
	pStr->SetScale( font_scale );
	pStr->SetString( text );

	return pStr;
}

IBIString* BIImageRender::CreateString( const char* text, const char* font_name, float font_scale, dword font_color, long valign, long halign, RECT& pos, long nPrior)
{
	return CreateString(text,font_name,font_scale,font_color,valign,halign,pos.left,pos.top,pos.right,pos.bottom,nPrior);
}

void BIImageRender::DeleteString( IBIString* str )
{
	long n = m_apStrings.Find( str );
	if( n >= 0 )
		m_apStrings.DelIndex( n );
}

void BIImageRender::ReleaseAllStrings()
{
	while( m_apStrings.Size()>0 ){
		delete( m_apStrings[0] );
	}
}

void BIImageRender::CutPrioritetRangeByStrings()
{
	for( long n=0; n<m_apStrings; n++ )
	{
		long iprior = ((BIString*)m_apStrings[n])->GetPrioritet();
		if( iprior < m_nBeginOutputPrioritet ) continue;
		if( iprior > m_nEndOutputPrioritet ) continue;
		m_nEndOutputPrioritet = iprior;
	}
}

void BIImageRender::SetBaseScreenSize(long nHSize, long nVSize, long nHOffset, long nVOffset)
{
	D3DVIEWPORT8 vp;
	m_pRS->GetViewport( &vp );

	if( vp.Width==nHSize && nHOffset==0 ) {
		m_fHScale = 1.f;
		m_fHOffset = 0.f;
	} else {
		m_fHScale = (float)vp.Width / (nHSize + 2*nHOffset);
	}

	if( vp.Height==nVSize && nVOffset==0 ) {
		m_fVScale = 1.f;
		m_fVOffset = 0.f;
	} else {
		m_fVScale = (float)vp.Height / (nVSize + 2*nVOffset);
	}
}

void BIImageRender::Release()
{
	while( m_apMaterial.Size()>0 ){
		delete( m_apMaterial[0] );
	}

	ReleaseAllStrings();
}

bool BIImageRender::GetFirstPrioritetRange()
{
	if( m_apMaterial.Size() == 0 ) return false;
	m_nBeginOutputPrioritet = m_apMaterial[0]->GetMinPrioritet();
	m_nEndOutputPrioritet = m_apMaterial[0]->GetMaxPrioritet();

	if( m_apMaterial.Size() > 1 && m_nEndOutputPrioritet>m_apMaterial[1]->GetMinPrioritet() )
		m_nEndOutputPrioritet = m_apMaterial[1]->GetMinPrioritet();
	return true;
}

bool BIImageRender::GetNextPrioritetRange()
{
	m_nBeginOutputPrioritet = ++m_nEndOutputPrioritet;
	for(long n=0; n<m_apMaterial; n++)
	{
		if( m_apMaterial[n]->GetMaxPrioritet() >= m_nBeginOutputPrioritet )
		{
			m_nEndOutputPrioritet = m_apMaterial[n]->GetMaxPrioritet();
			for(long i=n+1;i<m_apMaterial;i++)
				if( m_nBeginOutputPrioritet < m_apMaterial[i]->GetMinPrioritet() )
					m_nEndOutputPrioritet = m_apMaterial[i]->GetMinPrioritet();
			break;
		}
	}
	return (n<m_apMaterial);
}
