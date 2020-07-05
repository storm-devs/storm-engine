#include "editor.h"
#include "xinterface.h"
#include "nodes\xi_image.h"

GIEditor::GIEditor(XINTERFACE* pInterface)
{
	m_pGIOwner = pInterface;
	m_bShowMode = false;

	m_pEditableNode = null;
	m_fLastKeyPressTime = -1.f;

	m_pImageBack = NEW CXI_IMAGE;
	m_pImageBack->SetPosition( 100, 60 );
	m_pImageBack->SetSize( 600, 400 );
	m_pImageBack->SetColor( 0xC0606060 );
	m_pImageBack->SetThisRectangleNotUseTexture( true );
}

GIEditor::~GIEditor()
{
	Release();
}

void GIEditor::Release()
{
	m_pGIOwner = 0;
	m_bShowMode = false;
}

void GIEditor::Render()
{
	m_pImageBack->Draw();
}

bool GIEditor::MakeControl()
{
#ifndef _XBOX
	if( !m_bShowMode ) {
		if( GetAsyncKeyState( VK_CONTROL ) < 0 &&
			GetAsyncKeyState( VK_SHIFT ) < 0 &&
			GetAsyncKeyState( 'E' ) < 0 )
		{
			m_bShowMode = true;
			return true;
		}
	}

	if( m_bShowMode )
	{
		if( GetAsyncKeyState( VK_CONTROL ) < 0 &&
			GetAsyncKeyState( VK_SHIFT ) < 0 &&
			GetAsyncKeyState( 'Q' ) < 0 ) m_bShowMode = false;
		return true;
	}

	if( !m_pEditableNode ) return false;

	bool bMove = false;
	bool bSize = false;
	if( GetAsyncKeyState( VK_CONTROL )<0 ) // двигаем
	{
		bMove = true;
		if( GetAsyncKeyState( VK_SHIFT )<0 ) // меняем размер
		{
			bMove = false;
			bSize = true;
		}
	}

	if( GetAsyncKeyState( VK_CONTROL ) < 0 &&
		GetAsyncKeyState( VK_SHIFT ) < 0 &&
		GetAsyncKeyState( 'S' ) < 0 )
	{
		m_pEditableNode->SaveParametersToIni();
	}

	if( bMove || bSize )
	{
		long nHorz = 0;
		if( GetAsyncKeyState(VK_LEFT) < 0 ) nHorz--;
		if( GetAsyncKeyState(VK_RIGHT) < 0 ) nHorz++;
		long nVert = 0;
		if( GetAsyncKeyState(VK_UP) < 0 ) nVert--;
		if( GetAsyncKeyState(VK_DOWN) < 0 ) nVert++;

		if( nHorz!=0 || nVert!=0 )
		{
			if( m_fLastKeyPressTime < 0.f ) m_fLastKeyPressTime = 0.f;
			else
			{
				m_fLastKeyPressTime += api->GetDeltaTime() * .001f;
				if( m_fLastKeyPressTime < 0.8f ) nHorz = nVert = 0;
			}
		} else m_fLastKeyPressTime = -1.f;

		if( nHorz!=0 || nVert!=0 )
		{
			XYRECT rectNew = m_pEditableNode->m_rect;
			if( nHorz!=0 )
			{
				rectNew.right += nHorz;
				if( bMove ) rectNew.left += nHorz;

				if( rectNew.right < 0 ) rectNew.right = 0;
				if( rectNew.left > (long)m_pGIOwner->GetScreenWidth() ) rectNew.left = m_pGIOwner->GetScreenWidth();
				if( rectNew.right < rectNew.left ) rectNew.right = rectNew.left;
			}
			if( nVert!=0 )
			{
				rectNew.bottom += nVert;
				if( bMove ) rectNew.top += nVert;

				if( rectNew.bottom < 0 ) rectNew.bottom = 0;
				if( rectNew.top > (long)m_pGIOwner->GetScreenHeight() ) rectNew.top = m_pGIOwner->GetScreenHeight();
				if( rectNew.bottom < rectNew.top ) rectNew.bottom = rectNew.top;
			}
			m_pEditableNode->ChangePosition( rectNew );
		}
		return true;
	}
#endif
	return false;
}

bool GIEditor::IsShowMode()
{
	return m_bShowMode;
}

void GIEditor::MakeShowMode(bool bShow)
{
	m_bShowMode = bShow;
}

void GIEditor::SetEditNode(CINODE* pNode)
{
	m_pEditableNode = pNode;
}
