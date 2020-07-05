#include "battle_land.h"
#include "battle_mansign.h"
#include "..\defines.h"
#include "..\utils.h"
#include "..\..\engine\program\battle_interface\msg_control.h"

BATTLE_LAND_INTERFACE::BATTLE_LAND_INTERFACE() :
	m_TextInfo(_FL)
{
	m_pRS = null;
	m_pManSign = null;
}

BATTLE_LAND_INTERFACE::~BATTLE_LAND_INTERFACE()
{
	Release();
}

bool BATTLE_LAND_INTERFACE::Init()
{
	m_pRS=(VDX8RENDER *)api->CreateService("dx8render");
	if( !m_pRS ) {
		_THROW("Can`t create render service");
	}

	SetShowParameters();
	return true;
}

void BATTLE_LAND_INTERFACE::Execute(dword delta_time)
{
	CONTROL_STATE cs;

	if( m_bShowCommandos && m_pManSign )
	{
		if( !m_pManSign->IsActive() )
		{
			api->Controls->GetControlState(BI_COMMANDS_ACTIVATE_LAND,cs);
			if( cs.state == CST_ACTIVATED )
			{
				m_pManSign->SetActive( true );
			}
		}
		else
		{
			m_pManSign->MakeControl();
		}
	}
}

void BATTLE_LAND_INTERFACE::Realize(dword delta_time)
{
	if( m_bShowCommandos ) {
		m_pRS->MakePostProcess();
		if( m_pManSign )
			m_pManSign->Draw();

		m_Images.Draw();
		BIUtils::PrintTextInfoArray( m_TextInfo );
	}
}

dword _cdecl BATTLE_LAND_INTERFACE::ProcessMessage(MESSAGE & message)
{
	switch (message.Long())
	{
	case MSG_BATTLE_LAND_START:
		if( m_pManSign ) m_pManSign->SetUpdate();
	break;

	case MSG_BATTLE_LAND_END:
		EndShow();
	break;

	case BI_IN_SET_COMMAND_MODE:
		if(m_bShowCommandos && m_pManSign)
		{
			long comMode = message.Long();
			long startTextureNumber = message.Long();
			long startPictureNumber = message.Long();
			long characterNum = message.Long();
		}
	break;

	case MSG_BATTLE_LAND_SET_SHOW:
		m_bShowCommandos = (message.Long()!=0);
	break;

	case MSG_BATTLE_LAND_SET_MSGICONS:
		EnableMessageIcons(message.ScriptVariablePointer());
	break;

	case BI_MSG_ADD_NEWTEXTURE:
		{
			char param[256];	message.String(sizeof(param)-1,param);
			int hQ = message.Long();
			int vQ = message.Long();
			if(m_pManSign) return m_pManSign->AddTexture( param, hQ, vQ );
			return -1;
		}
	break;

	case MSG_BATTLE_LAND_MAKE_COMMAND:
		{
			char param[256];
			message.String( sizeof(param)-1, param );
			if( stricmp(param,"cancel")==0 ) {
				if( m_pManSign ) m_pManSign->ExecuteCommand( BI_MSG_COMMAND_DEACTIVATE );
			}
		}
	break;

	case BI_MSG_CARE_COMMANDLIST_UPDATE:
		m_pManSign->SetUpdate();
	break;
	}
	return 0;
}

void BATTLE_LAND_INTERFACE::SetParameters()
{
	UpdateCommandos();
	UpdateAlarm();
}

void BATTLE_LAND_INTERFACE::Release()
{
	m_pRS = NULL;
	DELETE( m_pManSign );

	m_TextInfo.DelAll();
}

void BATTLE_LAND_INTERFACE::EndShow()
{
}

void BATTLE_LAND_INTERFACE::SetShowParameters()
{
	ATTRIBUTES * pA = AttributesPointer ? AttributesPointer->GetAttributeClass("Parameters") : null;
	m_bShowCommandos = 0!=BIUtils::GetLongFromAttr(pA, "DoShowCommandos", true);

	m_pManSign = NEW BIManSign( GetID(), m_pRS );
	Assert(m_pManSign);
	m_pManSign->Init( AttributesPointer, (AttributesPointer?AttributesPointer->GetAttributeClass("ManSign"):0) );

	BIUtils::FillTextInfoArray( m_pRS, AttributesPointer?AttributesPointer->GetAttributeClass("textinfo"):0, m_TextInfo );

	m_Images.Init( m_pRS, AttributesPointer?AttributesPointer->GetAttributeClass("imageslist"):0 );
}

void BATTLE_LAND_INTERFACE::UpdateCommandos()
{
	ATTRIBUTES * pA = AttributesPointer ? AttributesPointer->GetAttributeClass("data") : null;
	if( pA ) pA = pA->GetAttributeClass("icons");
	if( !pA ) return;
}

void BATTLE_LAND_INTERFACE::UpdateAlarm()
{
}

void BATTLE_LAND_INTERFACE::EnableMessageIcons(VDATA * pvdat)
{
}

void BATTLE_LAND_INTERFACE::DisableMessageIcons()
{
}

void BATTLE_LAND_INTERFACE::SetTextData()
{
}
