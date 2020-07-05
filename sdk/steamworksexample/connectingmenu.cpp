//========= Copyright © 1996-2008, Valve LLC, All rights reserved. ============
//
// Purpose: Class to define the connecting game menu
//
// $NoKeywords: $
//=============================================================================


#include "stdafx.h"
#include "connectingmenu.h"
#include "SpaceWar.h"

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CConnectingMenu::CConnectingMenu( IGameEngine *pGameEngine ) : CBaseMenu<EClientGameState>( pGameEngine )
{
	m_bInitialLogonFailed = false;
	AddMenuItem( MenuItem_t( "Connecting...", k_EClientConnectingToSteam ) );
}

void CConnectingMenu::Reset()
{
	m_bInitialLogonFailed = false;
	ClearMenuItems();
	AddMenuItem( MenuItem_t( "Connecting...", k_EClientConnectingToSteam ) );
}

void CConnectingMenu::OnConnectFailure()
{
	m_bInitialLogonFailed = true;

	ClearMenuItems();

	AddMenuItem( MenuItem_t( "Retry Connecting", k_EClientRetrySteamConnection ) );
	AddMenuItem( MenuItem_t( "Link Existing Account", k_EClientLinkSteamAccount ) );
	AddMenuItem( MenuItem_t( "Autocreate Account", k_EClientAutoCreateAccount ) );
}