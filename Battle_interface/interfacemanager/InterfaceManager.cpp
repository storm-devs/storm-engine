#include "InterfaceManager.h"
#include "..\image\imgrender.h"
#include "..\image\image.h"
#include "..\utils.h"
#include "MousePointer.h"
#include "..\..\engine\program\BIManager\messages.h"
#include "InterfaceNode\AllNodes.h"

#include "InterfaceGroup\BaseGroup.h"
#include "InterfaceGroup\SeaGroup.h"

BI_InterfaceManager::BI_InterfaceManager() :
	m_aNodes(_FL)
{
	m_pRS = 0;
	m_pImgRender = 0;
	m_pMouse = 0;
	m_pInterfaceSheet = 0;
}

BI_InterfaceManager::~BI_InterfaceManager()
{
	DELETE( m_pInterfaceSheet );
	m_aNodes.DelAllWithPointers();
	DELETE( m_pMouse );
	DELETE( m_pImgRender );
}

bool BI_InterfaceManager::Init()
{
	m_pRS = (VDX8RENDER*)api->CreateService("DX8RENDER"); Assert(m_pRS);
	m_pImgRender = NEW BIImageRender(m_pRS); Assert(m_pImgRender);
	m_pMouse = NEW MousePointer(this,AttributesPointer); Assert(m_pMouse);

	long nBaseWidth = 800;
	long nBaseHeight = 600;
	long nBaseXOffset = 0;
	long nBaseYOffset = 0;
	if( AttributesPointer ) {
		ATTRIBUTES* pA = AttributesPointer->GetAttributeClass("BaseWindow");
		if( pA ) {
			nBaseWidth = pA->GetAttributeAsDword("width",nBaseWidth);
			nBaseHeight = pA->GetAttributeAsDword("height",nBaseHeight);
			nBaseXOffset = pA->GetAttributeAsDword("xoffset",nBaseXOffset);
			nBaseYOffset = pA->GetAttributeAsDword("yoffset",nBaseYOffset);
		}
	}
	m_pImgRender->SetBaseScreenSize( nBaseWidth,nBaseHeight, nBaseXOffset,nBaseYOffset );

	return true;
}

void BI_InterfaceManager::Execute(dword delta_time)
{
}

void BI_InterfaceManager::Realize(dword delta_time)
{
	if( m_pInterfaceSheet )
		m_pInterfaceSheet->Update();

	for( long n=0; n<m_aNodes; n++ )
		m_aNodes[n]->Update();

	m_pMouse->Update();

	m_pImgRender->Render();
}

dword _cdecl BI_InterfaceManager::ProcessMessage(MESSAGE & message)
{
	switch( message.Long() )
	{
	case MSG_BIMANAGER_DELETE_SHEET: DELETE(m_pInterfaceSheet); break;

	case MSG_BIMANAGER_LOAD_SHEET: return MsgLoadSheet( message ); break;

	case MSG_BIMANAGER_EVENT: return MsgEvent( message ); break;

	case MSG_BIMANAGER_CREATE_IMAGE: return MsgCreateImage( message ); break;

	case MSG_BIMANAGER_DELETE_IMAGE: return MsgDeleteNode( message ); break;

	case MSG_BIMANAGER_CREATE_STRING: return MsgCreateString( message ); break;

	case MSG_BIMANAGER_DELETE_STRING: return MsgDeleteNode( message ); break;
	}
	return 0;
}

BI_ManagerNodeBase* BI_InterfaceManager::CreateImageNode(const char* texture, const FRECT& uv, const RECT& pos, dword color, long nPrioritet)
{
	BI_ManagerNodeBase* pNod = NEW BI_ImageNode(this, texture, uv, pos, color, nPrioritet);
	return pNod;
}

BI_ManagerNodeBase* BI_InterfaceManager::CreateStringNode(const char* text, const char* font, dword color, float scale, const RECT& pos, long nHAlign, long nVAlign, long prioritet)
{
	BI_ManagerNodeBase* pNod = NEW BI_StringNode(this, text, font, color, scale, pos, nHAlign, nVAlign, prioritet);
	return pNod;
}

void BI_InterfaceManager::DeleteNode(BI_ManagerNodeBase* pNod)
{
	long n = m_aNodes.Find( pNod );
	if( n<0 ) return;
	m_aNodes.DelIndex( n );
}

long BI_InterfaceManager::MsgLoadSheet(MESSAGE & message)
{
	// удаляем старый интерфейс
	DELETE( m_pInterfaceSheet );

	char param[512];
	message.String( sizeof(param), param );
	if( stricmp(param,"sea")==0 )
	{ // грузим морской интерфейс
		m_pInterfaceSheet = NEW BI_SeaGroup(this);
		if( m_pInterfaceSheet ) {
			m_pInterfaceSheet->Init();
		}
	}else
	if( stricmp(param,"land")==0 )
	{ // грузим земной интерфейс
	}
	return 0;
}

long BI_InterfaceManager::MsgCreateImage(MESSAGE & message)
{
	/*char texture[MAX_PATH];	message.String( sizeof(texture), texture );
	FRECT uv;
	RECT pos;
	dword color;
	long nPrioritet;

	return (long)CreateImageNode(texture,uv,pos,color,nPrioritet);*/
	return 0;
}

long BI_InterfaceManager::MsgCreateString(MESSAGE & message)
{
	//return (long)CreateStringNode();
	return 0;
}

long BI_InterfaceManager::MsgDeleteNode(MESSAGE & message)
{
	BI_ManagerNodeBase* pNod = (BI_ManagerNodeBase*)message.Long();
	if( !pNod ) return 0;
	if( m_aNodes.Find(pNod) >=0 ) {
		DELETE(pNod);
	}
	return 0;
}

long BI_InterfaceManager::MsgEvent(MESSAGE & message)
{
	return 0;
}
