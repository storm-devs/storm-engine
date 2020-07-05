#include <stdio.h>
#include "xi_keychanger.h"

#define KEYPRESS_DELAY	500

CXI_KEYCHANGER::CXI_KEYCHANGER()
{
	m_rs = null;
	m_pControlsID = null;
	m_pbControlsStick = null;
	m_nNodeType = NODETYPE_KEYCHANGER;
	m_bKeyCheck = false;
	m_keysQuantity = 0;
}

CXI_KEYCHANGER::~CXI_KEYCHANGER()
{
	ReleaseAll();
}

void CXI_KEYCHANGER::Draw(bool bSelected,dword Delta_Time)
{
	if(m_bUse)
	{
		int i;
		CONTROL_STATE cs;

		for(i=0; i<m_keysQuantity; i++)
		{
			api->Controls->GetControlState(m_pControlsID[i],cs);
			if(m_bKeyCheck)
			{
				if( (m_pbControlsStick[i] && cs.state == CST_INACTIVATED) ||
					(!m_pbControlsStick[i] && (cs.fValue>1.f || cs.fValue<-1.f) ) )
				{
					bool bAllowChange = false;
					VDATA * pdat = api->Event("evntKeyChoose","ll",i,cs.fValue>0);
					if(pdat!=null) bAllowChange = pdat->GetLong()!=0;
					if(bAllowChange)
					{
						m_bUse = false;
						m_bKeyCheck = false;
						return;
					}
				}
			}
			else
				if(cs.state == CST_ACTIVE || cs.state == CST_ACTIVATED || cs.fValue>1.f || cs.fValue<-1.f)
					return;
		}
		m_bKeyCheck = true;
	}
}

void CXI_KEYCHANGER::ReleaseAll()
{
	PTR_DELETE(m_pControlsID);
	PTR_DELETE(m_pbControlsStick);
}

void CXI_KEYCHANGER::ChangePosition( XYRECT &rNewPos )
{
	// no this action
}

void CXI_KEYCHANGER::SaveParametersToIni()
{
	char pcWriteParam[2048];

	INIFILE * pIni = api->fio->OpenIniFile( (char*)ptrOwner->m_sDialogFileName.GetBuffer() );
	if( !pIni ) {
		api->Trace( "Warning! Can`t open ini file name %s", ptrOwner->m_sDialogFileName.GetBuffer() );
		return;
	}

	// save position
	_snprintf( pcWriteParam, sizeof(pcWriteParam), "%d,%d,%d,%d", m_rect.left, m_rect.top, m_rect.right, m_rect.bottom );
	pIni->WriteString( m_nodeName, "position", pcWriteParam );

	delete pIni;
}

dword _cdecl CXI_KEYCHANGER::MessageProc(long msgcode, MESSAGE & message)
{
	switch(msgcode)
	{
	case 0:
		{
			ATTRIBUTES * pA = message.AttributePointer();
			SetChoosingControls(pA);
		}
	break;
	}

	return 0;
}

void CXI_KEYCHANGER::SetChoosingControls(ATTRIBUTES * pA)
{
	if(pA==null) return;

	PTR_DELETE(m_pControlsID);
	PTR_DELETE(m_pbControlsStick);
	m_keysQuantity = pA->GetAttributesNum();
	if(m_keysQuantity<=0) return;

	m_pControlsID = NEW long[m_keysQuantity];
	m_pbControlsStick = NEW bool[m_keysQuantity];
	if(m_pControlsID==null || m_pbControlsStick==null)
	{
		_THROW("Allocate memory error");
	}

	char contrlName[128];
	for(int i=0; i<m_keysQuantity; i++)
	{
		sprintf(contrlName,"cntrl_%d",i);
		m_pbControlsStick[i] = false;
		m_pControlsID[i] = api->Controls->CreateControl(contrlName);
		char * keyCode = pA->GetAttribute(i);
		if(keyCode!=null)
		{
			api->Controls->MapControl(m_pControlsID[i],atoi(keyCode));
		}
		ATTRIBUTES * pAttr = pA->GetAttributeClass(i);
		if(pAttr!=null)
			if( pAttr->GetAttributeAsDword("stick",0) != 1 )
				m_pbControlsStick[i] = true;
	}
}

bool CXI_KEYCHANGER::Init(INIFILE *ini1,char *name1, INIFILE *ini2,char *name2, VDX8RENDER *rs, XYRECT &hostRect, XYPOINT &ScreenSize)
{
	SetGlowCursor(false);
	return true;
}
