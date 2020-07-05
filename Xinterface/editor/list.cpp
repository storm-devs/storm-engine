#include "list.h"
#include "..\nodes\xi_image.h"
#include "font.h"
#include "defines.h"

GIEditorList::GIEditorList(GIEditor* pEditor) :
	m_aStrings( _FL )
{
	m_pEditor = pEditor;
	Assert( m_pEditor );

	m_pChangeSelected = NEW GIEditorEventHandler;
	Assert( m_pChangeSelected );

	m_pBackImage = NEW CXI_IMAGE;
	Assert( m_pBackImage );
	m_pBackImage->SetThisRectangleNotUseTexture( true );
	m_pBackImage->SetColor( 0xFF8080D0 );

	m_pSelectImage = NEW CXI_IMAGE;
	Assert( m_pSelectImage );
	m_bShowSelect = false;
	m_pSelectImage->SetThisRectangleNotUseTexture( true );
	m_pSelectImage->SetColor( 0xFF80D080 );

	m_pFont = NEW GIFont( pEditor, "interface_normal" );
	Assert( m_pFont );
	m_pFont->SetHeight( 10.f );

	m_nSelectIndex = -1;
	m_nTopIndex = 0;
	m_nLineQuantity = 1;

	m_frStrOffset.bottom = m_frStrOffset.top = 10.f;
	m_frStrOffset.left = m_frStrOffset.right = 10.f;
	m_fStrLineStep = 12.f;

	m_fKeyRepeatDelay = 0.4f;
	m_fKeyRepeatInterval = 0.04f;
}

GIEditorList::~GIEditorList()
{
	Release();
}

void GIEditorList::Release()
{
	DELETE( m_pChangeSelected );
	DELETE( m_pBackImage );
	DELETE( m_pSelectImage );
	DELETE( m_pFont );
}

void GIEditorList::Create()
{
}

void GIEditorList::Render()
{
	if( m_pBackImage ) m_pBackImage->Draw();
	if( m_pSelectImage && m_bShowSelect ) m_pSelectImage->Draw();

	float fY = m_frBackRect.top + m_frStrOffset.top;
	float fYEnd = m_frBackRect.bottom - m_frStrOffset.bottom;
	float fX = m_frBackRect.left + m_frStrOffset.left;
	float fWidth = m_frBackRect.right - m_frStrOffset.right - fX;

	for( long n=m_nTopIndex; n<m_aStrings && fY < fYEnd; n++ )
	{
		m_pFont->Print( fX, fY, "%s", m_aStrings[n].GetBuffer() );
		fY += m_fStrLineStep;
	}
}

void GIEditorList::SetPosition( float fLeft, float fTop, float fRight, float fBottom )
{
	m_frBackRect.left = fLeft;
	m_frBackRect.top = fTop;
	m_frBackRect.right = fRight;
	m_frBackRect.bottom = fBottom;

	m_nLineQuantity = (long)((m_frBackRect.bottom - m_frBackRect.top - m_frStrOffset.top - m_frStrOffset.bottom) / m_fStrLineStep);

	m_pBackImage->SetPosition( (long)m_frBackRect.left, (long)m_frBackRect.top );
	m_pBackImage->SetSize( (long)(m_frBackRect.right-m_frBackRect.left), (long)(m_frBackRect.bottom-m_frBackRect.top) );

	m_frSelectRect.left = fLeft + 4.f;
	m_frSelectRect.right = fRight - 4.f;

	UpdateSelectPosition();
}

void GIEditorList::AddString( string& sNewStr )
{
	m_aStrings.Add( sNewStr );
}

void GIEditorList::RemoveString( string& sStr )
{
	RemoveString( FindString(sStr) );
}

void GIEditorList::RemoveString( long nIndex )
{
	if( nIndex < 0 || nIndex >= m_aStrings ) return;
	m_aStrings.DelIndex( nIndex );
}

void GIEditorList::RemoveAllStrings()
{
	m_aStrings.DelAll();
	m_nTopIndex = 0;
	SetSelectIndex( -1 );
}

long GIEditorList::FindString( string& sStr )
{
	for( long n=0; n<m_aStrings; n++ )
		if( m_aStrings[n] == sStr )
			return n;
	return -1;
}

string& GIEditorList::GetString( long nIndex )
{
	if( nIndex >= 0 && nIndex < m_aStrings ) return m_aStrings[nIndex];
	return m_sEmptyString;
}

void GIEditorList::SetSelectIndex( long nIndex )
{
	m_nSelectIndex = nIndex;

	if( m_nSelectIndex < m_nTopIndex )
	{
		m_nTopIndex = m_nSelectIndex;
		if( m_nSelectIndex < 0 ) m_nTopIndex = 0;
	}

	if( m_nSelectIndex >= m_nTopIndex + m_nLineQuantity )
	{
		m_nTopIndex = m_nSelectIndex - m_nLineQuantity;
		if( m_nTopIndex >= m_aStrings ) m_nTopIndex = m_aStrings.Size()-1;
	}

	UpdateSelectPosition();

	m_pChangeSelected->Execute();
}

bool GIEditorList::CheckMouseInside( float fX, float fY )
{
	if( fX >= m_frBackRect.left && fX <= m_frBackRect.right &&
		fY >= m_frBackRect.top && fY <= m_frBackRect.bottom ) return true;
	return false;
}

void GIEditorList::MakeMouseClick( float fX, float fY )
{
	FXYRECT frString;
	frString.left = m_frBackRect.left + m_frStrOffset.left;
	frString.right = m_frBackRect.right - m_frStrOffset.right;
	frString.top = m_frBackRect.top + m_frStrOffset.top;
	frString.bottom = m_frBackRect.bottom - m_frStrOffset.bottom;

	if( fX >= frString.left && fX <= frString.right &&
		fY >= frString.top && fY <= frString.bottom )
	{
		float fLineOffset = fY - frString.top;
		long nLineNum = 0;
		if( m_fStrLineStep > 0.f ) nLineNum = (long)(fLineOffset / m_fStrLineStep);
		if( nLineNum != m_nSelectIndex ) {
			m_dwStatus |= GIState_ListChange;
			SetSelectIndex( nLineNum );
		}
	}
}

void GIEditorList::DoKeyChecking()
{
	CONTROL_STATE cs;

	api->Controls->GetControlState( INTERFACE_CONTROL_DOWN, cs );
	if( cs.state == CST_ACTIVATED )
	{
		IncrementSelectedLine( true );
		m_fDownPressTime = 0.f;
	}
	else if( cs.state == CST_ACTIVE )
	{
		if( m_fDownPressTime < m_fKeyRepeatDelay ) {
			m_fDownPressTime += api->GetDeltaTime() * .001f;
		} else {
			IncrementSelectedLine( true );
			m_fDownPressTime = m_fKeyRepeatDelay - m_fKeyRepeatInterval;
		}
	}

	api->Controls->GetControlState( INTERFACE_CONTROL_UP, cs );
	if( cs.state == CST_ACTIVATED )
	{
		IncrementSelectedLine( false );
		m_fUpPressTime = 0.f;
	}
	else if( cs.state == CST_ACTIVE )
	{
		if( m_fUpPressTime < m_fKeyRepeatDelay ) {
			m_fUpPressTime += api->GetDeltaTime() * .001f;
		} else {
			IncrementSelectedLine( false );
			m_fUpPressTime = m_fKeyRepeatDelay - m_fKeyRepeatInterval;
		}
	}
}

void GIEditorList::IncrementSelectedLine( bool bIncr )
{
	if( bIncr )
	{
		if( m_nSelectIndex < (long)m_aStrings.Size()-1 )
		{
			SetSelectIndex( m_nSelectIndex+1 );
		}
	}
	else
	{
		if( m_nSelectIndex > 0 )
		{
			SetSelectIndex( m_nSelectIndex-1 );
		}
	}
}

void GIEditorList::UpdateSelectPosition()
{
	m_bShowSelect = false;
	if( m_nSelectIndex < 0 ) return;

	long nIdx = m_nSelectIndex - m_nTopIndex;
	if( nIdx < 0 ) return;

	m_frSelectRect.top = m_frBackRect.top + m_frStrOffset.top - 2.f + nIdx * m_fStrLineStep;
	if( m_frSelectRect.top > m_frBackRect.bottom - m_frStrOffset.bottom - 2.f ) return;

	m_frSelectRect.bottom = m_frSelectRect.top + m_fStrLineStep;
	m_bShowSelect = true;
	m_pSelectImage->SetPosition( (long)m_frSelectRect.left, (long)m_frSelectRect.top );
	m_pSelectImage->SetSize( (long)(m_frSelectRect.right-m_frSelectRect.left), (long)(m_frSelectRect.bottom-m_frSelectRect.top) );
}
