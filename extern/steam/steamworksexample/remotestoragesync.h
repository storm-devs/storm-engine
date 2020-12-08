//========= Copyright � 1996-2008, Valve LLC, All rights reserved. ============
//
// Purpose: Class for tracking stats and achievements
//
// $NoKeywords: $
//=============================================================================

#ifndef REMOTE_STORAGE_SYNC_H
#define REMOTE_STORAGE_SYNC_H

#include "SpaceWar.h"
#include "GameEngine.h"
#include "BaseMenu.h"

//-----------------------------------------------------------------------------
// NOTE
//
// The following code is for synchronizing files while the game is running
// This is only necessary when on the Playstation 3, as the Steam client handles
// these operations when on PC/Mac.
//
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Purpose: Interface for sync screen
//-----------------------------------------------------------------------------
class IRemoteStorageSync
{
public:
	virtual ~IRemoteStorageSync() {}

	virtual void SynchronizeToClient() = 0;
	virtual void SynchronizeToServer() = 0;
	virtual void Render() = 0;
	virtual bool BFinished() = 0;
	virtual void OnMenuSelection( ERemoteStorageSyncMenuCommand selection ) = 0;
};

#ifdef _PS3
enum ESyncOperation
{
	k_ESyncOperationIdle,
	k_ESyncOperationToClient,
	k_ESyncOperationToServer
};

//-----------------------------------------------------------------------------
// Purpose: Menu shown for synchronization progress and prompting user to resolve a conflict
//-----------------------------------------------------------------------------
class CRemoteStorageSyncMenu : public CBaseMenu< ERemoteStorageSyncMenuCommand >
{
public:
	CRemoteStorageSyncMenu( IGameEngine *pGameEngine );
	void ShowSyncProgress( ESyncOperation eSyncOperation, double dPercentComplete );
	void ShowSyncComplete( ESyncOperation eSyncOperation, bool bSuccess );
	void ShowConflict();	
};


//-----------------------------------------------------------------------------
// Purpose: Synchronizes files with Steam
//-----------------------------------------------------------------------------
class CRemoteStorageSync : public IRemoteStorageSync
{
public:

	// Constructor
	CRemoteStorageSync( IGameEngine *pGameEngine );

	// IRemoteStorageSync implementations
	virtual void SynchronizeToClient();
	virtual void SynchronizeToServer();
	virtual void Render();
	virtual bool BFinished();
	virtual void OnMenuSelection( ERemoteStorageSyncMenuCommand selection );

private:
	// synchronization updates
	STEAM_CALLBACK( CRemoteStorageSync, OnAppSyncClientComplete, RemoteStorageAppSyncedClient_t, m_callbackAppSyncClientComplete );
	STEAM_CALLBACK( CRemoteStorageSync, OnAppSyncServerComplete, RemoteStorageAppSyncedServer_t, m_callbackAppSyncServerComplete );
	STEAM_CALLBACK( CRemoteStorageSync, OnAppSyncProgress, RemoteStorageAppSyncProgress_t, m_callbackAppSyncProgress );
	STEAM_CALLBACK( CRemoteStorageSync, OnConflictResolution, RemoteStorageConflictResolution_t, m_callbackConflictResolution );

	CRemoteStorageSyncMenu m_menu;
	ESyncOperation m_eSyncOperation;
};
#endif // !_PS3

#endif
