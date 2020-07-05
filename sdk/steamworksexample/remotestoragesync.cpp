//========= Copyright © 1996-2008, Valve LLC, All rights reserved. ============
//
// Purpose: Class for tracking stats and achievements
//
// $NoKeywords: $
//=============================================================================

#include "stdafx.h"
#include "remotestoragesync.h"
#include "BaseMenu.h"

//-----------------------------------------------------------------------------
// NOTE
//
// The following code is for synchronizing files while the game is running
// This is only necessary when on the Playstation 3, as the Steam client handles
// these operations when on PC/Mac.
//
//-----------------------------------------------------------------------------

#ifdef _PS3
//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CRemoteStorageSyncMenu::CRemoteStorageSyncMenu( IGameEngine *pGameEngine ) : CBaseMenu<ERemoteStorageSyncMenuCommand>( pGameEngine )
{		
}

//-----------------------------------------------------------------------------
// Purpose: Shows the synchronization progress menu
//-----------------------------------------------------------------------------
void CRemoteStorageSyncMenu::ShowSyncProgress( ESyncOperation eSyncOperation, double dPercentComplete )
{
	if ( eSyncOperation == k_ESyncOperationIdle )
		return;

	PushSelectedItem();
	ClearMenuItems();

	// Create progress text
	char buffer[260];
	const char *pchFormat = (eSyncOperation == k_ESyncOperationToClient ) ? "Syncing to client (%d%%)" : "Syncing to server(%d%%)";
	sprintf( buffer, pchFormat, (int)dPercentComplete );
	AddMenuItem( CRemoteStorageSyncMenu::MenuItem_t( buffer, k_EMenuCommandProgress ) );

	PopSelectedItem();
}


//-----------------------------------------------------------------------------
// Purpose: Shows the synchronization progress menu
//-----------------------------------------------------------------------------
void CRemoteStorageSyncMenu::ShowSyncComplete( ESyncOperation eSyncOperation, bool bSuccess )
{
	if ( eSyncOperation == k_ESyncOperationIdle )
		return;

	PushSelectedItem();
	ClearMenuItems();

	// Create progress text
	char buffer[260];
	const char *pchResult = bSuccess ? "complete" : "failed";
	const char *pchFormat = (eSyncOperation == k_ESyncOperationToClient ) ? "Sync to client %s.. press X" : "Sync to server %s.. press X";	
	sprintf( buffer, pchFormat, pchResult );
	AddMenuItem( CRemoteStorageSyncMenu::MenuItem_t( buffer, k_EMenuCommandSyncComplete ) );

	PopSelectedItem();
}


//-----------------------------------------------------------------------------
// Purpose: Shows the conflict menu
//-----------------------------------------------------------------------------
void CRemoteStorageSyncMenu::ShowConflict()
{
	PushSelectedItem();
	ClearMenuItems();

	// Description
	AddMenuItem( CRemoteStorageSyncMenu::MenuItem_t( "There is a conflict between the local & server versions of your files:", k_EMenuCommandNone ) );

	PopSelectedItem();
}


//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CRemoteStorageSync::CRemoteStorageSync( IGameEngine *pGameEngine ) : 
	m_eSyncOperation( k_ESyncOperationIdle ),
	m_menu( pGameEngine ),
	m_callbackAppSyncClientComplete( this, &CRemoteStorageSync::OnAppSyncClientComplete ),
	m_callbackAppSyncServerComplete( this, &CRemoteStorageSync::OnAppSyncServerComplete ),
	m_callbackAppSyncProgress( this, &CRemoteStorageSync::OnAppSyncProgress )
{
}


//-----------------------------------------------------------------------------
// Purpose: Start synchronizing files to the client
//-----------------------------------------------------------------------------
void CRemoteStorageSync::SynchronizeToClient()
{
	if ( m_eSyncOperation != k_ESyncOperationIdle )
		return;

	if ( !SteamRemoteStorage()->SynchronizeToClient() )
		return;

	m_eSyncOperation = k_ESyncOperationToClient;
	m_menu.ShowSyncProgress( m_eSyncOperation, 0.0 );
}


//-----------------------------------------------------------------------------
// Purpose: Start synchronizing files to the server
//-----------------------------------------------------------------------------
void CRemoteStorageSync::SynchronizeToServer()
{
	if ( m_eSyncOperation != k_ESyncOperationIdle )
		return;

	if ( !SteamRemoteStorage()->SynchronizeToServer() )
		return;

	m_eSyncOperation = k_ESyncOperationToServer;
	m_menu.ShowSyncProgress( m_eSyncOperation, 0.0 );
}


//-----------------------------------------------------------------------------
// Purpose: Called each frame. Draw's menu
//-----------------------------------------------------------------------------
void CRemoteStorageSync::Render()
{
	m_menu.RunFrame();
}


//-----------------------------------------------------------------------------
// Purpose: Returns true if we are done synchronizing
//-----------------------------------------------------------------------------
bool CRemoteStorageSync::BFinished()
{
	return m_eSyncOperation == k_ESyncOperationIdle;
}


//-----------------------------------------------------------------------------
// Purpose: Called periodically by Steam when a synchronization is in progress
//-----------------------------------------------------------------------------
void CRemoteStorageSync::OnAppSyncProgress( RemoteStorageAppSyncProgress_t *pParam )
{
	// make sure the update is for us
	if ( pParam->m_nAppID != SteamUtils()->GetAppID() )
		return;

	// make sure the update is in the direction we expect
	if ( m_eSyncOperation == k_ESyncOperationIdle || (pParam->m_bUploading && m_eSyncOperation != k_ESyncOperationToServer) )
		return;

	m_menu.ShowSyncProgress( m_eSyncOperation, pParam->m_dAppPercentComplete );
}


//-----------------------------------------------------------------------------
// Purpose: Called by Steam when a synchronization to the client is complete
//-----------------------------------------------------------------------------
void CRemoteStorageSync::OnAppSyncClientComplete( RemoteStorageAppSyncedClient_t *pParam )
{
	// make sure the update is for us
	if ( pParam->m_nAppID != SteamUtils()->GetAppID() )
		return;

	// make sure the update is in the direction we expect
	if ( m_eSyncOperation != k_ESyncOperationToClient )
		return;

	// check for errors
	if ( pParam->m_eResult == k_EResultRemoteFileConflict )
	{
		m_menu.ShowConflict();
		return;
	}
	
	m_menu.ShowSyncComplete( m_eSyncOperation, pParam->m_eResult == k_EResultOK );
}


//-----------------------------------------------------------------------------
// Purpose: Called by Steam when a synchronization to the server is complete
//-----------------------------------------------------------------------------
void CRemoteStorageSync::OnAppSyncServerComplete( RemoteStorageAppSyncedServer_t *pParam )
{
	// make sure the update is for us
	if ( pParam->m_nAppID != SteamUtils()->GetAppID() )
		return;

	// make sure the update is in the direction we expect
	if ( m_eSyncOperation != k_ESyncOperationToServer )
		return;

	// check for errors
	if ( pParam->m_eResult == k_EResultRemoteFileConflict )
	{
		m_menu.ShowConflict();
		return;
	}
	
	m_menu.ShowSyncComplete( m_eSyncOperation, pParam->m_eResult == k_EResultOK );
}


//-----------------------------------------------------------------------------
// Purpose: A sync menu item has been selected
//-----------------------------------------------------------------------------
void CRemoteStorageSync::OnMenuSelection( ERemoteStorageSyncMenuCommand selection )
{
	switch( selection )
	{
	case k_EMenuCommandSyncComplete:
		m_eSyncOperation = k_ESyncOperationIdle;
		break;
	}
}

#endif