//========= Copyright 1996-2008, Valve LLC, All rights reserved. ============
//
// Purpose: Class for tracking stats and achievements
//
// $NoKeywords: $
//=============================================================================

#include "stdafx.h"
#include "RemoteStorage.h"
#include "remotestoragesync.h"
#include <assert.h>

#define CLOUDDISP_FONT_HEIGHT 20
#define CLOUDDISP_COLUMN_WIDTH 600
#define CLOUDDISP_TEXT_HEIGHT 20
#define CLOUDDISP_VERT_SPACING 4

#define MESSAGE_FILE_NAME "message.dat"


//-----------------------------------------------------------------------------
// NOTE
//
// The Steam program is normally responsible for synchronizing an App's files 
// to the Steam Cloud before launch and after the program exits. 
//
// This means that, if you build this example app and run it directly, 
// the Remote Storage page may appear to work (it will save the file changes
// to disk, locally), however nothing will actually get pulled down from
// or sent up to the Steam Cloud.
//
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Purpose: CRemoteStorage implementation
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CRemoteStorage::CRemoteStorage( IGameEngine *pGameEngine ) : m_pGameEngine( pGameEngine ), m_pRemoteStorageSync(NULL)
{
	m_pRemoteStorageScreen = new CRemoteStorageScreen( pGameEngine );

#ifdef _PS3
	m_pRemoteStorageSync = new CRemoteStorageSync( pGameEngine );
#endif
}


//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CRemoteStorage::~CRemoteStorage()
{
	if ( m_pRemoteStorageSync )
	{
		assert( m_pRemoteStorageSync->BFinished() );
		delete m_pRemoteStorageSync;
	}

	delete m_pRemoteStorageScreen;
}


//-----------------------------------------------------------------------------
// Purpose: Called when the user selects view remote storage files
//-----------------------------------------------------------------------------
void CRemoteStorage::Show()
{
	m_eState = k_ERemoteStorageStateIdle;
	CheckState();
}


//-----------------------------------------------------------------------------
// Purpose: Check's if we have finished our current step and should advance to the next
//
// NOTE:	These steps are not necessary for platforms where the Steam client is running. On those platforms,
//			the Steam client will take care of synchronization and m_pRemoteStorageSync will be NULL
//-----------------------------------------------------------------------------
void CRemoteStorage::CheckState()
{
	switch( m_eState )
	{
	default:
	case k_ERemoteStorageStateIdle:
		if ( m_pRemoteStorageSync )
			m_pRemoteStorageSync->SynchronizeToClient();

		// advance to next state
		m_eState = k_ERemoteStorageStateSyncToClient;

		// fall through

	case k_ERemoteStorageStateSyncToClient:
		if ( m_pRemoteStorageSync && !m_pRemoteStorageSync->BFinished() )
			break;

		// advance to next state
		m_eState = k_ERemoteStorageStateDisplayMessage;
		m_pRemoteStorageScreen->Show();
		break;

	case k_ERemoteStorageStateDisplayMessage:
		if ( !m_pRemoteStorageScreen->BFinished() )
			break;

		// advance to next state
		m_eState = k_ERemoteStorageStateSyncToServer;
		if ( m_pRemoteStorageSync )
			m_pRemoteStorageSync->SynchronizeToServer();

		// fall through

	case k_ERemoteStorageStateSyncToServer:
		if ( m_pRemoteStorageSync && !m_pRemoteStorageSync->BFinished() )
			break;

		// complete. Return to the main menu
		m_eState = k_ERemoteStorageStateIdle;
		SpaceWarClient()->SetGameState( k_EClientGameMenu );

		break;
	}
}


//-----------------------------------------------------------------------------
// Purpose: Called once per frame
//-----------------------------------------------------------------------------
void CRemoteStorage::Render()
{
	if ( m_pRemoteStorageSync && (m_eState == k_ERemoteStorageStateSyncToClient || m_eState == k_ERemoteStorageStateSyncToServer) )
		m_pRemoteStorageSync->Render();

	if ( m_eState == k_ERemoteStorageStateDisplayMessage )
		m_pRemoteStorageScreen->Render();

	CheckState();
}


//-----------------------------------------------------------------------------
// Purpose: A sync menu item has been selected
//-----------------------------------------------------------------------------
void CRemoteStorage::OnMenuSelection( ERemoteStorageSyncMenuCommand selection )
{
	if ( m_pRemoteStorageSync )
		m_pRemoteStorageSync->OnMenuSelection( selection );
}


//-----------------------------------------------------------------------------
// Purpose: CRemoteStorageScreen implementation
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CRemoteStorageScreen::CRemoteStorageScreen( IGameEngine *pGameEngine ) : m_pGameEngine( pGameEngine )
{
	m_rgchGreeting[0] = 0;
	strncpy( m_rgchGreeting, "<none>", sizeof( m_rgchGreeting ) );
	m_rgchGreetingNext[0] = 0;

	m_pSteamRemoteStorage = SteamRemoteStorage();

	m_hDisplayFont = pGameEngine->HCreateFont( CLOUDDISP_FONT_HEIGHT, FW_MEDIUM, false, "Arial" );
	if ( !m_hDisplayFont )
		OutputDebugString( "RemoteStorage font was not created properly, text won't draw\n" );

	GetFileStats();	
}


//-----------------------------------------------------------------------------
// Purpose: Load the user's saved message
//-----------------------------------------------------------------------------
void CRemoteStorageScreen::LoadMessage()
{
	if ( !m_pSteamRemoteStorage->FileExists( MESSAGE_FILE_NAME ) )
		return;

	int32 cubFile = m_pSteamRemoteStorage->GetFileSize( MESSAGE_FILE_NAME );
	if ( cubFile >= sizeof( m_rgchGreeting ) )
	{
		// ?? too big, nuke it
		char c = 0;
		OutputDebugString( "RemoteStorage: File was larger than expected. . .\n" );
		m_pSteamRemoteStorage->FileWrite( MESSAGE_FILE_NAME, &c, 1 );
	}
	else
	{
		int32 cubRead = m_pSteamRemoteStorage->FileRead( MESSAGE_FILE_NAME, m_rgchGreeting, sizeof( m_rgchGreeting ) - 1 );
		m_rgchGreeting[cubRead] = 0; // null-terminate
	}
}


//-----------------------------------------------------------------------------
// Purpose: Update stats on our files in the Cloud
//-----------------------------------------------------------------------------
void CRemoteStorageScreen::GetFileStats()
{
	m_ulBytesQuota = 0;
	m_ulAvailableBytes = 0;
	m_nNumFilesInCloud = m_pSteamRemoteStorage->GetFileCount();
	m_pSteamRemoteStorage->GetQuota( &m_ulBytesQuota, &m_ulAvailableBytes );
}


//-----------------------------------------------------------------------------
// Purpose: Called when the user selects view remote storage files
//-----------------------------------------------------------------------------
void CRemoteStorageScreen::Show()
{
	GetFileStats();
	LoadMessage();
}


//-----------------------------------------------------------------------------
// Purpose: Render the Remote Storage page
//-----------------------------------------------------------------------------
void CRemoteStorageScreen::Render()
{
	m_bFinished = false;

	// Update key press information

	int nGreetingNextLength = (int) strlen( m_rgchGreetingNext );

	DWORD dwVKDown = 0;

	bool bQuotaExceeded = nGreetingNextLength > m_ulBytesQuota;

	while ( m_pGameEngine->BGetFirstKeyDown( &dwVKDown ) )
	{
		if ( VK_ESCAPE == dwVKDown )
		{
			// cancel
			m_rgchGreetingNext[0] = 0;

			m_bFinished = true;
			return;
		}
		else if ( VK_RETURN == dwVKDown )
		{
			if ( !bQuotaExceeded )
			{
				uint64 ulCurrentTickCount = m_pGameEngine->GetGameTickCount();
				if ( ulCurrentTickCount - 150 > g_ulLastReturnKeyTick )
				{
					// global from BaseMenu.h!
					g_ulLastReturnKeyTick = ulCurrentTickCount;

					// Do it
					{
						m_bFinished = true;

						strncpy( m_rgchGreeting, m_rgchGreetingNext, sizeof( m_rgchGreeting ) );
						m_rgchGreetingNext[0] = 0;

						// Note: not writing the NULL termination, so won't read it back later either.
						bool bRet = m_pSteamRemoteStorage->FileWrite( MESSAGE_FILE_NAME, m_rgchGreeting, (int) strlen( m_rgchGreeting ) );
						
						// Update our stats on stuff
						GetFileStats();

						if ( !bRet )
						{
							OutputDebugString( "RemoteStorage: Failed to write file!\n" );
						}

						return;
					}
				}
			}
		}
		else if ( VK_BACK == dwVKDown )
		{
			if ( nGreetingNextLength )
			{
				m_rgchGreetingNext[--nGreetingNextLength] = 0;
			}
		}
		else if ( ( dwVKDown >= 0x30 && dwVKDown <= 0x39 )
			|| ( dwVKDown >= 0x41 && dwVKDown <= 0x5A )
			|| dwVKDown == VK_SPACE )
		{
			// Add the key pressed
			if ( nGreetingNextLength + 1 < sizeof( m_rgchGreetingNext ) )
			{
				m_rgchGreetingNext[nGreetingNextLength++] = (char) dwVKDown;
				m_rgchGreetingNext[nGreetingNextLength] = 0;
			}
		}
	}

	const int32 width = m_pGameEngine->GetViewportWidth();
	//const int32 height = m_pGameEngine->GetViewportHeight();

	const int32 pxColumn1Left = width / 2 - CLOUDDISP_COLUMN_WIDTH / 2;

	RECT rect;
	{
		int32 pxVertOffset = 8 * ( CLOUDDISP_TEXT_HEIGHT );
		rect.top = pxVertOffset;
		rect.bottom = rect.top + CLOUDDISP_TEXT_HEIGHT;
		rect.left = pxColumn1Left;
		rect.right = rect.left + CLOUDDISP_COLUMN_WIDTH;
		pxVertOffset = rect.bottom + CLOUDDISP_VERT_SPACING;

		char rgchBuffer[256];
		sprintf_safe( rgchBuffer, "Num Files In Cloud: %d", m_nNumFilesInCloud );
		m_pGameEngine->BDrawString( m_hDisplayFont, rect, D3DCOLOR_ARGB( 255, 25, 200, 25 ), TEXTPOS_CENTER|TEXTPOS_VCENTER, rgchBuffer );

		rect.top = pxVertOffset;
		rect.bottom = rect.top + CLOUDDISP_TEXT_HEIGHT;
		rect.left = pxColumn1Left;
		rect.right = rect.left + CLOUDDISP_COLUMN_WIDTH;
		pxVertOffset = rect.bottom + CLOUDDISP_TEXT_HEIGHT + CLOUDDISP_VERT_SPACING;

		sprintf_safe( rgchBuffer, "Quota: %llu bytes, %llu bytes remaining", m_ulBytesQuota, m_ulAvailableBytes );
		m_pGameEngine->BDrawString( m_hDisplayFont, rect, D3DCOLOR_ARGB( 255, 25, 200, 25 ), TEXTPOS_CENTER|TEXTPOS_VCENTER, rgchBuffer );

		rect.top = pxVertOffset;
		rect.bottom = rect.top + CLOUDDISP_TEXT_HEIGHT;
		rect.left = pxColumn1Left;
		rect.right = rect.left + CLOUDDISP_COLUMN_WIDTH;
		pxVertOffset = rect.bottom + CLOUDDISP_VERT_SPACING;

		sprintf_safe( rgchBuffer, "Current Message:" );
		m_pGameEngine->BDrawString( m_hDisplayFont, rect, D3DCOLOR_ARGB( 255, 25, 200, 25 ), TEXTPOS_CENTER|TEXTPOS_VCENTER, rgchBuffer );

		rect.top = pxVertOffset;
		rect.bottom = rect.top + CLOUDDISP_TEXT_HEIGHT;
		rect.left = pxColumn1Left;
		rect.right = rect.left + CLOUDDISP_COLUMN_WIDTH;
		pxVertOffset = rect.bottom + CLOUDDISP_TEXT_HEIGHT + CLOUDDISP_VERT_SPACING;

		m_pGameEngine->BDrawString( m_hDisplayFont, rect, D3DCOLOR_ARGB( 255, 25, 200, 25 ), TEXTPOS_CENTER|TEXTPOS_VCENTER, m_rgchGreeting );

		rect.top = pxVertOffset;
		rect.bottom = rect.top + CLOUDDISP_TEXT_HEIGHT;
		rect.left = pxColumn1Left;
		rect.right = rect.left + CLOUDDISP_COLUMN_WIDTH;
		pxVertOffset = rect.bottom + CLOUDDISP_VERT_SPACING;

		sprintf_safe( rgchBuffer, "Type in a new message below:" );
		m_pGameEngine->BDrawString( m_hDisplayFont, rect, D3DCOLOR_ARGB( 255, 25, 200, 25 ), TEXTPOS_CENTER|TEXTPOS_VCENTER, rgchBuffer );

		rect.top = pxVertOffset;
		rect.bottom = rect.top + CLOUDDISP_TEXT_HEIGHT;
		rect.left = pxColumn1Left;
		rect.right = rect.left + CLOUDDISP_COLUMN_WIDTH;
		pxVertOffset = rect.bottom + CLOUDDISP_TEXT_HEIGHT + CLOUDDISP_VERT_SPACING;

		m_pGameEngine->BDrawString( m_hDisplayFont, rect, D3DCOLOR_ARGB( 255, 25, 200, 25 ), TEXTPOS_CENTER|TEXTPOS_VCENTER, m_rgchGreetingNext );

		rect.top = pxVertOffset;
		rect.bottom = rect.top + CLOUDDISP_TEXT_HEIGHT;
		rect.left = pxColumn1Left;
		rect.right = rect.left + CLOUDDISP_COLUMN_WIDTH;
		pxVertOffset = rect.bottom + CLOUDDISP_TEXT_HEIGHT + CLOUDDISP_VERT_SPACING;

		sprintf_safe( rgchBuffer, "Hit <ENTER> to save, <ESC> to cancel" );
		m_pGameEngine->BDrawString( m_hDisplayFont, rect, D3DCOLOR_ARGB( 255, 25, 200, 25 ), TEXTPOS_CENTER|TEXTPOS_VCENTER, rgchBuffer );

		if ( bQuotaExceeded )
		{
			rect.top = pxVertOffset;
			rect.bottom = rect.top + CLOUDDISP_TEXT_HEIGHT;
			rect.left = pxColumn1Left;
			rect.right = rect.left + CLOUDDISP_COLUMN_WIDTH;
			pxVertOffset = rect.bottom + CLOUDDISP_TEXT_HEIGHT + CLOUDDISP_VERT_SPACING;

			sprintf_safe( rgchBuffer, "!! QUOTA EXCEEDED !!" );
			m_pGameEngine->BDrawString( m_hDisplayFont, rect, D3DCOLOR_ARGB( 255, 25, 200, 25 ), TEXTPOS_CENTER|TEXTPOS_VCENTER, rgchBuffer );
		}
	}
}
