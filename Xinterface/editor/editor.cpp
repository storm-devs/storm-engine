#include "editor.h"
#include "..\xinterface.h"
#include "..\nodes\xi_image.h"
#include "list.h"
#include "defines.h"

GIEditor::GIEditor(XINTERFACE* pInterface)
{
	m_pGIOwner = pInterface;
	m_pRS = pInterface->RenderService();

	m_bShowMode = false;

	m_pEditableNode = null;
	m_fLastKeyPressTime = -1.f;

	m_pImageBack = NEW CXI_IMAGE;
	Assert( m_pImageBack );
	m_pImageBack->SetPosition( 100, 60 );
	m_pImageBack->SetSize( 600, 400 );
	m_pImageBack->SetColor( 0xD0606060 );
	m_pImageBack->SetThisRectangleNotUseTexture( true );

	m_pNodeList = NEW GIEditorList( this );
	Assert( m_pNodeList );
	m_pNodeList->SetPosition( 120,80, 360, 380 );
	LinkEvent( m_pNodeList->m_pChangeSelected, (GIEditorEvent)ChangeNodeName );

	m_bSubNameOn = false;
	m_pSubNameList = NEW GIEditorList( this );
	Assert( m_pSubNameList );
	m_pSubNameList->SetPosition( 380,80, 500,380 );
	LinkEvent( m_pSubNameList->m_pChangeSelected, (GIEditorEvent)ChangeSubNodeName );
}

GIEditor::~GIEditor()
{
	Release();
}

void GIEditor::Release()
{
	m_pGIOwner = 0;
	m_bShowMode = false;

	DELETE( m_pImageBack );
	DELETE( m_pNodeList );
	DELETE( m_pSubNameList );
}

void GIEditor::Render()
{
	if( !m_bShowMode ) return;

	FXYPOINT pntMouse = m_pGIOwner->GetMousePoint();
	m_pNodeList->CheckMouseInside( pntMouse.x, pntMouse.y );
	if( m_bSubNameOn ) m_pSubNameList->CheckMouseInside( pntMouse.x, pntMouse.y );

	CONTROL_STATE cs;
	api->Controls->GetControlState(INTERFACE_CONTROL_LCLICK,cs);
	if( cs.state == CST_ACTIVATED )
	{
		m_pNodeList->MakeMouseClick( pntMouse.x, pntMouse.y );
		if( m_bSubNameOn ) m_pSubNameList->MakeMouseClick( pntMouse.x, pntMouse.y );
	}

	if( m_bSubNameOn ) m_pSubNameList->DoKeyChecking();
	else m_pNodeList->DoKeyChecking();

	m_pImageBack->Draw();
	m_pNodeList->Render();
	if( m_bSubNameOn ) m_pSubNameList->Render();
}

bool GIEditor::ProcessControl()
{
#ifndef _XBOX
	if( !m_bShowMode ) {
		if( api->Controls->GetDebugAsyncKeyState( VK_CONTROL ) < 0 &&
			api->Controls->GetDebugAsyncKeyState( VK_SHIFT ) < 0 &&
			api->Controls->GetDebugAsyncKeyState( 'E' ) < 0 )
		{
			m_bShowMode = true;
			m_bSubNameOn = false;
			return true;
		}
	}

	if( m_bShowMode )
	{
		if( api->Controls->GetDebugAsyncKeyState( VK_CONTROL ) < 0 &&
			api->Controls->GetDebugAsyncKeyState( VK_SHIFT ) < 0 &&
			api->Controls->GetDebugAsyncKeyState( 'Q' ) < 0 ) m_bShowMode = false;

		CONTROL_STATE cs;
		api->Controls->GetControlState( "IStartButton", cs );
		if( cs.state == CST_INACTIVATED )
		{
			m_bShowMode = false;
			if( !m_bSubNameOn )
			{
				array<string> aStr( _FL );
				if( m_pEditableNode && m_pEditableNode->GetInternalNameList(aStr) )
				{
					m_bShowMode = true;
					m_bSubNameOn = false;
					m_pSubNameList->RemoveAllStrings();
					for( long n=0; n<aStr; n++ )
						m_pSubNameList->AddString( aStr[n] );
					m_bSubNameOn = true;
					m_pSubNameList->SetSelectIndex(0);
					aStr.DelAll();
				}
			}
		}

		if( m_bSubNameOn && api->Controls->GetDebugAsyncKeyState( VK_ESCAPE ) < 0 ) {
			m_bSubNameOn = false;
		}

		return true;
	}

	if( !m_pEditableNode ) return false;

	bool bMove = 0;
	bool bSize = false;
	if( api->Controls->GetDebugAsyncKeyState( VK_CONTROL )<0 ) // двигаем
	{
		bMove = true;
		if( api->Controls->GetDebugAsyncKeyState( VK_SHIFT )<0 ) // меняем размер
		{
			bMove = false;
			bSize = true;
		}
	}

	if( api->Controls->GetDebugAsyncKeyState( VK_CONTROL ) < 0 &&
		api->Controls->GetDebugAsyncKeyState( VK_SHIFT ) < 0 &&
		api->Controls->GetDebugAsyncKeyState( 'S' ) < 0 )
	{
		m_pEditableNode->SaveParametersToIni();
	}

	if( bMove || bSize )
	{
		long nHorz = 0;
		if( api->Controls->GetDebugAsyncKeyState(VK_LEFT) < 0 )
			if( api->Controls->GetDebugAsyncKeyState(VK_MENU) < 0 ) nHorz-=10;
			else nHorz--;
		if( api->Controls->GetDebugAsyncKeyState(VK_RIGHT) < 0 )
			if( api->Controls->GetDebugAsyncKeyState(VK_MENU) < 0 ) nHorz+=10;
			else nHorz++;
		long nVert = 0;
		if( api->Controls->GetDebugAsyncKeyState(VK_UP) < 0 )
			if( api->Controls->GetDebugAsyncKeyState(VK_MENU) < 0 ) nVert-=10;
			else nVert--;
		if( api->Controls->GetDebugAsyncKeyState(VK_DOWN) < 0 )
			if( api->Controls->GetDebugAsyncKeyState(VK_MENU) < 0 ) nVert+=10;
			else nVert++;

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
/*
				if( rectNew.right < 0 ) rectNew.right = 0;
				if( rectNew.left > (long)m_pGIOwner->GetScreenWidth() ) rectNew.left = m_pGIOwner->GetScreenWidth();
				if( rectNew.right < rectNew.left ) rectNew.right = rectNew.left;*/
			}
			if( nVert!=0 )
			{
				rectNew.bottom += nVert;
				if( bMove ) rectNew.top += nVert;

/*				if( rectNew.bottom < 0 ) rectNew.bottom = 0;
				if( rectNew.top > (long)m_pGIOwner->GetScreenHeight() ) rectNew.top = m_pGIOwner->GetScreenHeight();
				if( rectNew.bottom < rectNew.top ) rectNew.bottom = rectNew.top;*/
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

void GIEditor::ReCreate()
{
	m_pNodeList->RemoveAllStrings();
	string sStr = "Nothing";
	m_pNodeList->AddString( sStr );
	m_pNodeList->SetSelectIndex( 0 );
}

void GIEditor::AddNode(CINODE* pNode)
{
	if( !pNode ) return;
	string sStr = pNode->m_nodeName;
	m_pNodeList->AddString( sStr );
}

void GIEditor::DelNode(CINODE* pNode)
{
	if( !pNode ) return;
	m_pNodeList->RemoveString( string(pNode->m_nodeName) );
}

void GIEditor::DrawSizeBox()
{
	if( !m_pEditableNode ) return;
	if( api->Controls->GetDebugAsyncKeyState( VK_CONTROL )<0 ) // показываем
	{
		RS_LINE rsl[8];
		for( long n=0; n<8; n++ )
		{
			rsl[n].dwColor = 0xFFFFFFFF;
			rsl[n].vPos.z = 1.f;
		}
		rsl[0].vPos.x = (float)m_pEditableNode->m_rect.left;		rsl[0].vPos.y = (float)m_pEditableNode->m_rect.top;
		rsl[1].vPos.x = (float)m_pEditableNode->m_rect.right;		rsl[1].vPos.y = (float)m_pEditableNode->m_rect.top;

		rsl[2].vPos.x = (float)m_pEditableNode->m_rect.right;		rsl[2].vPos.y = (float)m_pEditableNode->m_rect.top;
		rsl[3].vPos.x = (float)m_pEditableNode->m_rect.right;		rsl[3].vPos.y = (float)m_pEditableNode->m_rect.bottom;

		rsl[4].vPos.x = (float)m_pEditableNode->m_rect.right;		rsl[4].vPos.y = (float)m_pEditableNode->m_rect.bottom;
		rsl[5].vPos.x = (float)m_pEditableNode->m_rect.left;		rsl[5].vPos.y = (float)m_pEditableNode->m_rect.bottom;

		rsl[6].vPos.x = (float)m_pEditableNode->m_rect.left;		rsl[6].vPos.y = (float)m_pEditableNode->m_rect.bottom;
		rsl[7].vPos.x = (float)m_pEditableNode->m_rect.left;		rsl[7].vPos.y = (float)m_pEditableNode->m_rect.top;
		m_pRS->DrawLines( rsl, 4, "AILine" );
		// boal -->
		// idFont, dwFCol, dwBCol, align, shadow, scale, sxs, sys,
									//left, top, "%s", str
		long m_fontID = m_pGIOwner->GetRenderService()->LoadFont("interface_normal");
		m_pGIOwner->GetRenderService()->ExtPrint( m_fontID, 0xFFFFFFFF, 0, ALIGN_LEFT, false, 1.0, 800, 600,
									10, 10, "(%d, %d) - (%d, %d) W - %d H - %d",
									 (int)m_pEditableNode->m_rect.left,
									 (int)m_pEditableNode->m_rect.top,
									 (int)m_pEditableNode->m_rect.right,
									 (int)m_pEditableNode->m_rect.bottom,
									 ((int)m_pEditableNode->m_rect.right - (int)m_pEditableNode->m_rect.left),
									 ((int)m_pEditableNode->m_rect.bottom - (int)m_pEditableNode->m_rect.top));
		m_pGIOwner->GetRenderService()->UnloadFont(m_fontID);
		// boal <--
	}
}

void GIEditor::ChangeNodeName()
{
	if( m_pNodeList->GetSelectString()=="Nothing" || m_pNodeList->GetSelectString()=="" )
	{
		m_pEditableNode = null;
		return;
	}

	m_pEditableNode = m_pGIOwner->FindNode( m_pNodeList->GetSelectString().GetBuffer(), 0 );
}

void GIEditor::ChangeSubNodeName()
{
	if( !m_bSubNameOn ) return;
	if( !m_pEditableNode ) return;

	m_pEditableNode->SetInternalName( m_pSubNameList->GetSelectString() );
}
