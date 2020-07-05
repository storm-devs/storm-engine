//========= Copyright � 1996-2008, Valve LLC, All rights reserved. ============
//
// Purpose: Main class for the space war game client
//
// $NoKeywords: $
//=============================================================================

#include "stdafx.h"
#include "SpaceWarClient.h"
#include "SpaceWarServer.h"
#include "connectingmenu.h"
#include "MainMenu.h"
#include "QuitMenu.h"
#include "stdlib.h"
#include "time.h"
#include "ServerBrowser.h"
#include "Leaderboards.h"
#include "Friends.h"
#include "musicplayer.h"
#include "clanchatroom.h"
#include "Lobby.h"
#include "p2pauth.h"
#include "voicechat.h"
#include "htmlsurface.h"
#include "Inventory.h"
#include "steam/steamencryptedappticket.h"
#include "RemotePlay.h"
#ifdef WIN32
#include <direct.h>
#else
#define MAX_PATH PATH_MAX
#include <unistd.h>
#define _getcwd getcwd
#endif


CSpaceWarClient *g_pSpaceWarClient = NULL;
CSpaceWarClient* SpaceWarClient() { return g_pSpaceWarClient; }

extern bool ParseCommandLine( const char *pchCmdLine, const char **ppchServerAddress, const char **ppchLobbyID );

#if defined(WIN32)
#define atoll _atoi64
#endif


//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CSpaceWarClient::CSpaceWarClient( IGameEngine *pGameEngine )
{
	Init( pGameEngine );
}



//-----------------------------------------------------------------------------
// Purpose: initialize our client for use
//-----------------------------------------------------------------------------
void CSpaceWarClient::Init( IGameEngine *pGameEngine )
{
	// On PC/OSX we always know the user has a SteamID and is logged in already,
	// as Steam enforces this before game launch.  On PS3 however the game must
	// initiate the logon and we need to check the state here and block the user
	// while Steam connects.
	if ( SteamUser()->BLoggedOn() )
	{
		m_SteamIDLocalUser = SteamUser()->GetSteamID();
		m_eGameState = k_EClientGameMenu;
	}
#ifdef _PS3
	else
	{
		m_eGameState = k_EClientConnectingToSteam;
		SteamUser()->LogOn( true );
	}
#endif

	g_pSpaceWarClient = this;
	m_pGameEngine = pGameEngine;
	m_uPlayerWhoWonGame = 0;
	m_ulStateTransitionTime = m_pGameEngine->GetGameTickCount();
	m_ulLastNetworkDataReceivedTime = 0;
	m_pServer = NULL;
	m_uPlayerShipIndex = 0;
	m_eConnectedStatus = k_EClientNotConnected;
	m_bTransitionedGameState = true;
	m_rgchErrorText[0] = 0;
	m_unServerIP = 0;
	m_usServerPort = 0;
	m_ulPingSentTime = 0;
	m_bSentWebOpen = false;

	for( uint32 i = 0; i < MAX_PLAYERS_PER_SERVER; ++i )
	{
		m_rguPlayerScores[i] = 0;
		m_rgpShips[i] = NULL;
	}

	// Seed random num generator
	srand( (uint32)time( NULL ) );

	m_hHUDFont = pGameEngine->HCreateFont( HUD_FONT_HEIGHT, FW_BOLD, false, "Arial" );
	if ( !m_hHUDFont )
		OutputDebugString( "HUD font was not created properly, text won't draw\n" );

	m_hInstructionsFont = pGameEngine->HCreateFont( INSTRUCTIONS_FONT_HEIGHT, FW_BOLD, false, "Arial" );
	if ( !m_hInstructionsFont )
		OutputDebugString( "instruction font was not created properly, text won't draw\n" );

	m_hInGameStoreFont = pGameEngine->HCreateFont( INSTRUCTIONS_FONT_HEIGHT, FW_BOLD, false, "Courier New" );
	if ( !m_hInGameStoreFont )
		OutputDebugString( "in-game store font was not created properly, text won't draw\n" );

	// Initialize starfield
	m_pStarField = new CStarField( pGameEngine );

	// Initialize main menu
	m_pMainMenu = new CMainMenu( pGameEngine );

	// Initialize connecting menu
	m_pConnectingMenu = new CConnectingMenu( pGameEngine );

	// Initialize pause menu
	m_pQuitMenu = new CQuitMenu( pGameEngine );

	// Initialize sun
	m_pSun = new CSun( pGameEngine );

	m_nNumWorkshopItems = 0;
	for (uint32 i = 0; i < MAX_WORKSHOP_ITEMS; ++i)
	{
		m_rgpWorkshopItems[i] = NULL;
	}

	// initialize P2P auth engine
	m_pP2PAuthedGame = new CP2PAuthedGame( m_pGameEngine );

	// Create matchmaking menus
	m_pServerBrowser = new CServerBrowser( m_pGameEngine );
	m_pLobbyBrowser = new CLobbyBrowser( m_pGameEngine );
	m_pLobby = new CLobby( m_pGameEngine );


	// Init stats
	m_pStatsAndAchievements = new CStatsAndAchievements( pGameEngine );
	m_pLeaderboards = new CLeaderboards( pGameEngine );
	m_pFriendsList = new CFriendsList( pGameEngine );
	m_pMusicPlayer = new CMusicPlayer( pGameEngine );
	m_pClanChatRoom = new CClanChatRoom( pGameEngine );

	// Remote Play session list
	m_pRemotePlayList = new CRemotePlayList( pGameEngine );

	// Remote Storage page
	m_pRemoteStorage = new CRemoteStorage( pGameEngine );

	// P2P voice chat 
	m_pVoiceChat = new CVoiceChat( pGameEngine );

	// HTML Surface page
	m_pHTMLSurface = new CHTMLSurface(pGameEngine);

	LoadWorkshopItems();

	LoadItemsWithPrices();
}


//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CSpaceWarClient::~CSpaceWarClient()
{
	DisconnectFromServer();

	if ( m_pP2PAuthedGame )
	{
		m_pP2PAuthedGame->EndGame();
		delete m_pP2PAuthedGame;
		m_pP2PAuthedGame = NULL;
	}

	if ( m_pServer )
	{
		delete m_pServer;
		m_pServer = NULL; 
	}

	if ( m_pStarField )
		delete m_pStarField;

	if ( m_pMainMenu )
		delete m_pMainMenu;

	if ( m_pConnectingMenu )
		delete m_pConnectingMenu;

	if ( m_pQuitMenu ) 
		delete m_pQuitMenu;

	if ( m_pSun )
		delete m_pSun;

	if ( m_pStatsAndAchievements )
		delete m_pStatsAndAchievements;

	if ( m_pServerBrowser )
		delete m_pServerBrowser; 

	if ( m_pVoiceChat )
		delete m_pVoiceChat;

	if ( m_pHTMLSurface )
		delete m_pHTMLSurface;

	for( uint32 i = 0; i < MAX_PLAYERS_PER_SERVER; ++i )
	{
		if ( m_rgpShips[i] )
		{
			delete m_rgpShips[i];
			m_rgpShips[i] = NULL;
		}
	}
	
	for (uint32 i = 0; i < MAX_WORKSHOP_ITEMS; ++i)
	{
		if ( m_rgpWorkshopItems[i] )
		{
			delete m_rgpWorkshopItems[i];
			m_rgpWorkshopItems[i] = NULL;
		}
	}
}


//-----------------------------------------------------------------------------
// Purpose: Tell the connected server we are disconnecting (if we are connected)
//-----------------------------------------------------------------------------
void CSpaceWarClient::DisconnectFromServer()
{
	if ( m_eConnectedStatus != k_EClientNotConnected )
	{
#ifdef USE_GS_AUTH_API
		if ( m_hAuthTicket != k_HAuthTicketInvalid )
			SteamUser()->CancelAuthTicket( m_hAuthTicket );
		m_hAuthTicket = k_HAuthTicketInvalid;
#else
		SteamUser()->AdvertiseGame( k_steamIDNil, 0, 0 );
#endif

		MsgClientLeavingServer_t msg;
		BSendServerData( &msg, sizeof(msg) );
		m_eConnectedStatus = k_EClientNotConnected;
	}
	if ( m_pP2PAuthedGame )
	{
		m_pP2PAuthedGame->EndGame();
	}

	if ( m_pVoiceChat )
	{
		m_pVoiceChat->StopVoiceChat();
	}

	// forget the game server ID
	if ( m_steamIDGameServer.IsValid() )
	{
		SteamNetworking()->CloseP2PSessionWithUser( m_steamIDGameServer );
		m_steamIDGameServer = CSteamID();
	}
}


//-----------------------------------------------------------------------------
// Purpose: Receive basic server info from the server after we initiate a connection
//-----------------------------------------------------------------------------
void CSpaceWarClient::OnReceiveServerInfo( CSteamID steamIDGameServer, bool bVACSecure, const char *pchServerName )
{
	m_eConnectedStatus = k_EClientConnectedPendingAuthentication;
	m_pQuitMenu->SetHeading( pchServerName );
	m_steamIDGameServer = steamIDGameServer;

	// look up the servers IP and Port from the connection
	P2PSessionState_t p2pSessionState;
	SteamNetworking()->GetP2PSessionState( steamIDGameServer, &p2pSessionState );
	m_unServerIP = p2pSessionState.m_nRemoteIP;
	m_usServerPort = p2pSessionState.m_nRemotePort;

	// set how to connect to the game server, using the Rich Presence API
	// this lets our friends connect to this game via their friends list
	UpdateRichPresenceConnectionInfo();

	MsgClientBeginAuthentication_t msg;
#ifdef USE_GS_AUTH_API
	char rgchToken[1024];
	uint32 unTokenLen = 0;
	m_hAuthTicket = SteamUser()->GetAuthSessionTicket( rgchToken, sizeof( rgchToken ), &unTokenLen );
	msg.SetToken( rgchToken, unTokenLen );

#else
	// When you aren't using Steam auth you can still call AdvertiseGame() so you can communicate presence data to the friends
	// system. Make sure to pass k_steamIDNonSteamGS
	uint32 unTokenLen = SteamUser()->AdvertiseGame( k_steamIDNonSteamGS, m_unServerIP, m_usServerPort );
	msg.SetSteamID( SteamUser()->GetSteamID().ConvertToUint64() );
#endif

	Steamworks_TestSecret();

	if ( msg.GetTokenLen() < 1 )
		OutputDebugString( "Warning: Looks like GetAuthSessionTicket didn't give us a good ticket\n" );

	BSendServerData( &msg, sizeof(msg) );
}


//-----------------------------------------------------------------------------
// Purpose: Receive an authentication response from the server
//-----------------------------------------------------------------------------
void CSpaceWarClient::OnReceiveServerAuthenticationResponse( bool bSuccess, uint32 uPlayerPosition )
{
	if ( !bSuccess )
	{
		SetConnectionFailureText( "Connection failure.\nMultiplayer authentication failed\n" );
		SetGameState( k_EClientGameConnectionFailure );
		DisconnectFromServer();
	}
	else
	{
		// Is this a duplicate message? If so ignore it...
		if ( m_eConnectedStatus == k_EClientConnectedAndAuthenticated && m_uPlayerShipIndex == uPlayerPosition )
			return;

		m_uPlayerShipIndex = uPlayerPosition;
		m_eConnectedStatus = k_EClientConnectedAndAuthenticated;

		// set information so our friends can join the lobby
		UpdateRichPresenceConnectionInfo();

		// send a ping, to measure round-trip time
		m_ulPingSentTime = m_pGameEngine->GetGameTickCount();
		MsgClientPing_t msg;
		BSendServerData( &msg, sizeof( msg ) );
	}
}


//-----------------------------------------------------------------------------
// Purpose: Handles receiving a state update from the game server
//-----------------------------------------------------------------------------
void CSpaceWarClient::OnReceiveServerUpdate( ServerSpaceWarUpdateData_t *pUpdateData )
{
	// Update our client state based on what the server tells us
	
	switch( pUpdateData->GetServerGameState() )
	{
	case k_EServerWaitingForPlayers:
		if ( m_eGameState == k_EClientGameQuitMenu )
			break;
		else if (m_eGameState == k_EClientGameMenu )
			break;
		else if ( m_eGameState == k_EClientGameExiting )
			break;

		SetGameState( k_EClientGameWaitingForPlayers );
		break;
	case k_EServerActive:
		if ( m_eGameState == k_EClientGameQuitMenu )
			break;
		else if (m_eGameState == k_EClientGameMenu )
			break;
		else if ( m_eGameState == k_EClientGameExiting )
			break;

		SetGameState( k_EClientGameActive );
		break;
	case k_EServerDraw:
		if ( m_eGameState == k_EClientGameQuitMenu )
			break;
		else if ( m_eGameState == k_EClientGameMenu )
			break;
		else if ( m_eGameState == k_EClientGameExiting )
			break;

		SetGameState( k_EClientGameDraw );
		break;
	case k_EServerWinner:
		if ( m_eGameState == k_EClientGameQuitMenu )
			break;
		else if ( m_eGameState == k_EClientGameMenu )
			break;
		else if ( m_eGameState == k_EClientGameExiting )
			break;

		SetGameState( k_EClientGameWinner );
		break;
	case k_EServerExiting:
		if ( m_eGameState == k_EClientGameExiting )
			break;

		SetGameState( k_EClientGameMenu );
		break;
	}

	// Update scores
	for( int i=0; i < MAX_PLAYERS_PER_SERVER; ++i )
	{
		m_rguPlayerScores[i] = pUpdateData->GetPlayerScore(i);
	}

	// Update who won last
	m_uPlayerWhoWonGame = pUpdateData->GetPlayerWhoWon();

	if ( m_pP2PAuthedGame )
	{
		// has the player list changed?
		if ( m_pServer )
		{
			// if i am the server owner i need to auth everyone who wants to play
			// assume i am in slot 0, so start at slot 1
			for( uint32 i=1; i < MAX_PLAYERS_PER_SERVER; ++i )
			{
				CSteamID steamIDNew( pUpdateData->GetPlayerSteamID(i) );
				if ( steamIDNew == SteamUser()->GetSteamID() )
				{
					OutputDebugString( "Server player slot 0 is not server owner.\n" );
				}
				else if ( steamIDNew != m_rgSteamIDPlayers[i] )
				{
					if ( m_rgSteamIDPlayers[i].IsValid() )
					{
						m_pP2PAuthedGame->PlayerDisconnect( i );
					}
					if ( steamIDNew.IsValid() )
					{
						m_pP2PAuthedGame->RegisterPlayer( i, steamIDNew );
					}
				}
			}
		}
		else
		{
			// i am just a client, i need to auth the game owner ( slot 0 )
			CSteamID steamIDNew( pUpdateData->GetPlayerSteamID( 0 ) );
			if ( steamIDNew == SteamUser()->GetSteamID() )
			{
				OutputDebugString( "Server player slot 0 is not server owner.\n" );
			}
			else if ( steamIDNew != m_rgSteamIDPlayers[0] )
			{
				if ( m_rgSteamIDPlayers[0].IsValid() )
				{
					OutputDebugString( "Server player slot 0 has disconnected - but thats the server owner.\n" );
					m_pP2PAuthedGame->PlayerDisconnect( 0 );
				}
				if ( steamIDNew.IsValid() )
				{
					m_pP2PAuthedGame->StartAuthPlayer( 0, steamIDNew );
				}
			}
		}
	}

	// update all players that are active
	if ( m_pVoiceChat )
		m_pVoiceChat->MarkAllPlayersInactive();

	// Update the players
	for( uint32 i=0; i < MAX_PLAYERS_PER_SERVER; ++i )
	{
		// Update steamid array with data from server
		m_rgSteamIDPlayers[i].SetFromUint64( pUpdateData->GetPlayerSteamID( i ) );

		if ( pUpdateData->GetPlayerActive( i ) )
		{
			// Check if we have a ship created locally for this player slot, if not create it
			if ( !m_rgpShips[i] )
			{
				ServerShipUpdateData_t *pShipData = pUpdateData->AccessShipUpdateData( i );
				m_rgpShips[i] = new CShip( m_pGameEngine, false, pShipData->GetXPosition(), pShipData->GetYPosition(), g_rgPlayerColors[i] );
				if ( i == m_uPlayerShipIndex )
				{
					// If this is our local ship, then setup key bindings appropriately
					m_rgpShips[i]->SetVKBindingLeft( 0x41 ); // A key
					m_rgpShips[i]->SetVKBindingRight( 0x44 ); // D key
					m_rgpShips[i]->SetVKBindingForwardThrusters( 0x57 ); // W key
					m_rgpShips[i]->SetVKBindingReverseThrusters( 0x53 ); // S key
					m_rgpShips[i]->SetVKBindingFire( VK_SPACE ); 
				}
			}

			if ( i == m_uPlayerShipIndex )
				m_rgpShips[i]->SetIsLocalPlayer( true );
			else
				m_rgpShips[i]->SetIsLocalPlayer( false );

			m_rgpShips[i]->OnReceiveServerUpdate( pUpdateData->AccessShipUpdateData( i ) );			

			if ( m_pVoiceChat )
				m_pVoiceChat->MarkPlayerAsActive( m_rgSteamIDPlayers[i] );
		}
		else
		{
			// Make sure we don't have a ship locally for this slot
			if ( m_rgpShips[i] )
			{
				delete m_rgpShips[i];
				m_rgpShips[i] = NULL;
			}
		}
	}
}


//-----------------------------------------------------------------------------
// Purpose: Used to transition game state
//-----------------------------------------------------------------------------
void CSpaceWarClient::SetGameState( EClientGameState eState )
{
	if ( m_eGameState == eState )
		return;

	m_bTransitionedGameState = true;
	m_ulStateTransitionTime = m_pGameEngine->GetGameTickCount();
	m_eGameState = eState;

	// Let the stats handler check the state (so it can detect wins, losses, etc...)
	m_pStatsAndAchievements->OnGameStateChange( eState );

	// update any rich presence state
	UpdateRichPresenceConnectionInfo();
}


//-----------------------------------------------------------------------------
// Purpose: set the error string to display in the UI
//-----------------------------------------------------------------------------
void CSpaceWarClient::SetConnectionFailureText( const char *pchErrorText )
{
	sprintf_safe( m_rgchErrorText, "%s", pchErrorText );
}


//-----------------------------------------------------------------------------
// Purpose: Send data to the current server
//-----------------------------------------------------------------------------
bool CSpaceWarClient::BSendServerData( const void *pData, uint32 nSizeOfData )
{
	if ( !SteamNetworking()->SendP2PPacket( m_steamIDGameServer, pData, nSizeOfData, k_EP2PSendUnreliable ) )
	{
		OutputDebugString( "Failed sending data to server\n" );
		return false;
	}
	return true;
}


//-----------------------------------------------------------------------------
// Purpose: Initiates a connection to a server
//-----------------------------------------------------------------------------
void CSpaceWarClient::InitiateServerConnection( uint32 unServerAddress, const int32 nPort )
{
	if ( m_eGameState == k_EClientInLobby && m_steamIDLobby.IsValid() )
	{
		SteamMatchmaking()->LeaveLobby( m_steamIDLobby );
	}

	SetGameState( k_EClientGameConnecting );

	// Update when we last retried the connection, as well as the last packet received time so we won't timeout too soon,
	// and so we will retry at appropriate intervals if packets drop
	m_ulLastNetworkDataReceivedTime = m_ulLastConnectionAttemptRetryTime = m_pGameEngine->GetGameTickCount();

	// ping the server to find out what it's steamID is
	m_unServerIP = unServerAddress;
	m_usServerPort = (uint16)nPort;
	m_GameServerPing.RetrieveSteamIDFromGameServer( this, m_unServerIP, m_usServerPort );
}


//-----------------------------------------------------------------------------
// Purpose: Initiates a connection to a server via P2P (NAT-traversing) connection
//-----------------------------------------------------------------------------
void CSpaceWarClient::InitiateServerConnection( CSteamID steamIDGameServer )
{
	if ( m_eGameState == k_EClientInLobby && m_steamIDLobby.IsValid() )
	{
		SteamMatchmaking()->LeaveLobby( m_steamIDLobby );
	}

	SetGameState( k_EClientGameConnecting );

	m_steamIDGameServer = steamIDGameServer;

	// Update when we last retried the connection, as well as the last packet received time so we won't timeout too soon,
	// and so we will retry at appropriate intervals if packets drop
	m_ulLastNetworkDataReceivedTime = m_ulLastConnectionAttemptRetryTime = m_pGameEngine->GetGameTickCount();

	// send the packet to the server
	MsgClientInitiateConnection_t msg;
	BSendServerData( &msg, sizeof( msg ) );
}


//-----------------------------------------------------------------------------
// Purpose: steam callback, triggered when our connection to another client fails
//-----------------------------------------------------------------------------
void CSpaceWarClient::OnP2PSessionConnectFail( P2PSessionConnectFail_t *pCallback )
{
	if ( pCallback->m_steamIDRemote == m_steamIDGameServer )
	{
		// failed, error out
		OutputDebugString( "Failed to make P2P connection, quiting server\n" );
		OnReceiveServerExiting();
	}
}


//-----------------------------------------------------------------------------
// Purpose: Receives incoming network data
//-----------------------------------------------------------------------------
void CSpaceWarClient::ReceiveNetworkData()
{
	char rgchRecvBuf[1024];
	char *pchRecvBuf = rgchRecvBuf;
	uint32 cubMsgSize;
	for (;;)
	{
		// reset the receive buffer
		if ( pchRecvBuf != rgchRecvBuf )
		{
			free( pchRecvBuf );
			pchRecvBuf = rgchRecvBuf;
		}

		// see if there is any data waiting on the socket
		if ( !SteamNetworking()->IsP2PPacketAvailable( &cubMsgSize ) )
			break;

		// not enough space in default buffer
		// alloc custom size and try again
		if ( cubMsgSize > sizeof(rgchRecvBuf) )
		{
			pchRecvBuf = (char *)malloc( cubMsgSize );
		}
		CSteamID steamIDRemote;
		if ( !SteamNetworking()->ReadP2PPacket( pchRecvBuf, cubMsgSize, &cubMsgSize, &steamIDRemote ) )
			break;

		// see if it's from the game server
		if ( steamIDRemote == m_steamIDGameServer )
		{
			m_ulLastNetworkDataReceivedTime = m_pGameEngine->GetGameTickCount();

			// make sure we're connected
			if ( m_eConnectedStatus == k_EClientNotConnected && m_eGameState != k_EClientGameConnecting )
				continue;

			if ( cubMsgSize < sizeof( DWORD ) )
			{
				OutputDebugString( "Got garbage on client socket, too short\n" );
			}

			EMessage eMsg = (EMessage)LittleDWord( *(DWORD*)pchRecvBuf );
			switch ( eMsg )
			{
			case k_EMsgServerSendInfo:
				{
					if ( cubMsgSize != sizeof( MsgServerSendInfo_t ) )
					{
						OutputDebugString ("Bad server info msg\n" );
						continue;
					}
					MsgServerSendInfo_t *pMsg = (MsgServerSendInfo_t*)pchRecvBuf;

					// pull the IP address of the user from the socket
					OnReceiveServerInfo( CSteamID( pMsg->GetSteamIDServer() ), pMsg->GetSecure(), pMsg->GetServerName() );
				}
				break;
			case k_EMsgServerPassAuthentication:
				{
					if ( cubMsgSize != sizeof( MsgServerPassAuthentication_t ) )
					{
						OutputDebugString( "Bad accept connection msg\n" );
						continue;
					}
					MsgServerPassAuthentication_t *pMsg = (MsgServerPassAuthentication_t*)pchRecvBuf;

					// Our game client doesn't really care about whether the server is secure, or what its 
					// steamID is, but if it did we would pass them in here as they are part of the accept message
					OnReceiveServerAuthenticationResponse( true, pMsg->GetPlayerPosition() );
				}
				break;
			case k_EMsgServerFailAuthentication:
				{
					OnReceiveServerAuthenticationResponse( false, 0 );
				}
				break;
			case k_EMsgServerUpdateWorld:
				{
					if ( cubMsgSize != sizeof( MsgServerUpdateWorld_t ) )
					{
						OutputDebugString( "Bad server world update msg\n" );
						continue;
					}

					MsgServerUpdateWorld_t *pMsg = (MsgServerUpdateWorld_t*)pchRecvBuf;
					OnReceiveServerUpdate( pMsg->AccessUpdateData() );
				}
				break;
			case k_EMsgServerExiting:
				{
					if ( cubMsgSize != sizeof( MsgServerExiting_t ) )
					{
						OutputDebugString( "Bad server exiting msg\n" );
					}
					OnReceiveServerExiting();
				}
				break;
			case k_EMsgServerPingResponse:
				{
					uint64 ulTimePassedMS = m_pGameEngine->GetGameTickCount() - m_ulPingSentTime;
					char rgchT[256];
					sprintf_safe( rgchT, "Round-trip ping time to server %d ms\n", (int)ulTimePassedMS );
					rgchT[ sizeof(rgchT) - 1 ] = 0;
					OutputDebugString( rgchT );
					m_ulPingSentTime = 0;
				}
				break;
			default:
				OutputDebugString( "Unhandled message from server\n" );
				break;
			}
		}
		else 
		{
			// the message is from another player
			EMessage eMsg = (EMessage)LittleDWord( *(DWORD*)pchRecvBuf );

			if ( m_pP2PAuthedGame->HandleMessage( eMsg, pchRecvBuf ) )
				continue; // this was a P2P auth message

			if ( m_pVoiceChat->HandleMessage( steamIDRemote, eMsg, pchRecvBuf ) )
				continue;

			// Unhandled message
			OutputDebugString( "Received unknown message on our listen socket\n" );
		}
	}

	// if we're running a server, do that as well
	if ( m_pServer )
	{
		m_pServer->ReceiveNetworkData();
	}
}


//-----------------------------------------------------------------------------
// Purpose: Handle the server telling us it is exiting
//-----------------------------------------------------------------------------
void CSpaceWarClient::OnReceiveServerExiting()
{
	if ( m_pP2PAuthedGame )
		m_pP2PAuthedGame->EndGame();

#ifdef USE_GS_AUTH_API
	if ( m_hAuthTicket != k_HAuthTicketInvalid )
	{
		SteamUser()->CancelAuthTicket( m_hAuthTicket );
	}
	m_hAuthTicket = k_HAuthTicketInvalid;
#else
	SteamUser()->AdvertiseGame( k_steamIDNil, 0, 0 );
#endif

	if ( m_eGameState != k_EClientGameActive )
		return;
	m_eConnectedStatus = k_EClientNotConnected;

	SetConnectionFailureText( "Game server has exited." );
	SetGameState( k_EClientGameConnectionFailure );
}


//-----------------------------------------------------------------------------
// Purpose: Steam is asking us to join a game, based on the user selecting
//			'join game' on a friend in their friends list 
//			the string comes from the "connect" field set in the friends' rich presence
//-----------------------------------------------------------------------------
void CSpaceWarClient::OnGameJoinRequested( GameRichPresenceJoinRequested_t *pCallback )
{
	// parse out the connect 
	const char *pchServerAddress, *pchLobbyID;
	
	if ( ParseCommandLine( pCallback->m_rgchConnect, &pchServerAddress, &pchLobbyID ) )
	{
		// exec
		ExecCommandLineConnect( pchServerAddress, pchLobbyID );
	}
}


//-----------------------------------------------------------------------------
// Purpose: a Steam URL to launch this app was executed while the game is already running, eg steam://run/480//+connect%20127.0.0.1
//      	Anybody can build random Steam URLs	and these extra parameters must be carefully parsed to avoid unintended side-effects
//-----------------------------------------------------------------------------
void CSpaceWarClient::OnNewUrlLaunchParameters( NewUrlLaunchParameters_t *pCallback )
{
	const char *pchServerAddress, *pchLobbyID;
	char szCommandLine[1024] = {};

	if ( SteamApps()->GetLaunchCommandLine( szCommandLine, sizeof(szCommandLine) ) > 0 )
	{
		if ( ParseCommandLine( szCommandLine, &pchServerAddress, &pchLobbyID ) )
		{
			// exec
			ExecCommandLineConnect( pchServerAddress, pchLobbyID );
		}
	}
}


//-----------------------------------------------------------------------------
// Purpose: Finishes up entering a lobby of our own creation
//-----------------------------------------------------------------------------
void CSpaceWarClient::OnLobbyCreated( LobbyCreated_t *pCallback, bool bIOFailure )
{
	if ( m_eGameState != k_EClientCreatingLobby )
		return;

	// record which lobby we're in
	if ( pCallback->m_eResult == k_EResultOK )
	{
		// success
		m_steamIDLobby = pCallback->m_ulSteamIDLobby;
		m_pLobby->SetLobbySteamID( m_steamIDLobby );

		// set the name of the lobby if it's ours
		char rgchLobbyName[256];
		sprintf_safe( rgchLobbyName, "%s's lobby", SteamFriends()->GetPersonaName() );
		SteamMatchmaking()->SetLobbyData( m_steamIDLobby, "name", rgchLobbyName );

		// mark that we're in the lobby
		SetGameState( k_EClientInLobby );
	}
	else
	{
		// failed, show error
		SetConnectionFailureText( "Failed to create lobby (lost connection to Steam back-end servers." );
		SetGameState( k_EClientGameConnectionFailure );
	}
}

//-----------------------------------------------------------------------------
// Purpose: Finishes up entering a lobby
//-----------------------------------------------------------------------------
void CSpaceWarClient::OnLobbyEntered( LobbyEnter_t *pCallback, bool bIOFailure )
{
	if ( m_eGameState != k_EClientJoiningLobby )
		return;

	if ( pCallback->m_EChatRoomEnterResponse != k_EChatRoomEnterResponseSuccess )
	{
		// failed, show error
		SetConnectionFailureText( "Failed to enter lobby" );
		SetGameState( k_EClientGameConnectionFailure );
		return;
	}

	// success

	// move forward the state
	m_steamIDLobby = pCallback->m_ulSteamIDLobby;
	m_pLobby->SetLobbySteamID( m_steamIDLobby );
	SetGameState( k_EClientInLobby );
}


//-----------------------------------------------------------------------------
// Purpose: Joins a game from a lobby
//-----------------------------------------------------------------------------
void CSpaceWarClient::OnLobbyGameCreated( LobbyGameCreated_t *pCallback )
{
	if ( m_eGameState != k_EClientInLobby )
		return;

	// join the game server specified, via whichever method we can
	if ( CSteamID( pCallback->m_ulSteamIDGameServer ).IsValid() )
	{
		InitiateServerConnection( CSteamID( pCallback->m_ulSteamIDGameServer ) );
	}
}


//-----------------------------------------------------------------------------
// Purpose: a large avatar image has been loaded for us
//-----------------------------------------------------------------------------
void CSpaceWarClient::OnAvatarImageLoaded( AvatarImageLoaded_t *pCallback )
{
}


//-----------------------------------------------------------------------------
// Purpose: Handles menu actions in a lobby
//-----------------------------------------------------------------------------
void CSpaceWarClient::OnMenuSelection( LobbyMenuItem_t selection )
{
	if ( selection.m_eCommand == LobbyMenuItem_t::k_ELobbyMenuItemLeaveLobby )
	{
		// leave the lobby
		SteamMatchmaking()->LeaveLobby( m_steamIDLobby );
		m_steamIDLobby = CSteamID();

		// return to main menu
		SetGameState( k_EClientGameMenu );
	}
	else if ( selection.m_eCommand == LobbyMenuItem_t::k_ELobbyMenuItemToggleReadState )
	{
		// update our state
		bool bOldState = ( 1 == atoi( SteamMatchmaking()->GetLobbyMemberData( m_steamIDLobby, SteamUser()->GetSteamID(), "ready" ) ) );
		bool bNewState = !bOldState;
		// publish to everyone
		SteamMatchmaking()->SetLobbyMemberData( m_steamIDLobby, "ready", bNewState ? "1" : "0" );
	}
	else if ( selection.m_eCommand == LobbyMenuItem_t::k_ELobbyMenuItemStartGame )
	{
		// make sure we're not already starting a server
		if ( m_pServer )
			return;

		// broadcast to everyone in the lobby that the game is starting
		SteamMatchmaking()->SetLobbyData( m_steamIDLobby, "game_starting", "1" );
		
		// start a local game server
		m_pServer = new CSpaceWarServer( m_pGameEngine );
		// we'll have to wait until the game server connects to the Steam server back-end 
		// before telling all the lobby members to join (so that the NAT traversal code has a path to contact the game server)
		OutputDebugString( "Game server being created; game will start soon.\n" );
	}
	else if ( selection.m_eCommand == LobbyMenuItem_t::k_ELobbyMenuItemInviteToLobby )
	{
		SteamFriends()->ActivateGameOverlayInviteDialog( selection.m_steamIDLobby );
	}
}


//-----------------------------------------------------------------------------
// Purpose: Handles menu actions when viewing a leaderboard
//-----------------------------------------------------------------------------
void CSpaceWarClient::OnMenuSelection( LeaderboardMenuItem_t selection )
{
	m_pLeaderboards->OnMenuSelection( selection );
}


//-----------------------------------------------------------------------------
// Purpose: Handles menu actions when viewing a leaderboard
//-----------------------------------------------------------------------------
void CSpaceWarClient::OnMenuSelection( FriendsListMenuItem_t selection )
{
	m_pFriendsList->OnMenuSelection( selection );
}


//-----------------------------------------------------------------------------
// Purpose: Handles menu actions when viewing the Remote Play session list
//-----------------------------------------------------------------------------
void CSpaceWarClient::OnMenuSelection( RemotePlayListMenuItem_t selection )
{
	m_pRemotePlayList->OnMenuSelection( selection );
}


//-----------------------------------------------------------------------------
// Purpose: Handles menu actions when viewing the remote storage sync screen
//-----------------------------------------------------------------------------
void CSpaceWarClient::OnMenuSelection( ERemoteStorageSyncMenuCommand selection )
{
	m_pRemoteStorage->OnMenuSelection( selection );
}


//-----------------------------------------------------------------------------
// Purpose: For a player in game, set the appropriate rich presence keys for display
// in the Steam friends list and return the value for steam_display
//-----------------------------------------------------------------------------
const char *CSpaceWarClient::SetInGameRichPresence() const
{
	const char *pchStatus;

	bool bWinning = false;
	uint32 cWinners = 0;
	uint32 uHighScore = m_rguPlayerScores[0];
	uint32 uMyScore = 0;
	for ( uint32 i = 0; i < MAX_PLAYERS_PER_SERVER; ++i )
	{
		if ( m_rguPlayerScores[i] > uHighScore )
		{
			uHighScore = m_rguPlayerScores[i];
			cWinners = 0;
			bWinning = false;
		}

		if ( m_rguPlayerScores[i] == uHighScore )
		{
			cWinners++;
			bWinning = bWinning || (m_rgSteamIDPlayers[i] == m_SteamIDLocalUser);
		}

		if ( m_rgSteamIDPlayers[i] == m_SteamIDLocalUser )
		{
			uMyScore = m_rguPlayerScores[i];
		}
	}

	if ( bWinning && cWinners > 1 )
	{
		pchStatus = "Tied";
	}
	else if ( bWinning )
	{
		pchStatus = "Winning";
	}
	else
	{
		pchStatus = "Losing";
	}

	char rgchBuffer[32];
	sprintf_safe( rgchBuffer, "%2u", uMyScore );
	SteamFriends()->SetRichPresence( "score", rgchBuffer );

	return pchStatus;
}


//-----------------------------------------------------------------------------
// Purpose: does work on transitioning from one game state to another
//-----------------------------------------------------------------------------
void CSpaceWarClient::OnGameStateChanged( EClientGameState eGameStateNew )
{
	const char *pchSteamRichPresenceDisplay = "AtMainMenu";
	bool bDisplayScoreInRichPresence = false;
	if ( m_eGameState == k_EClientFindInternetServers )
	{
		// If we are just opening the find servers screen, then start a refresh
		m_pServerBrowser->RefreshInternetServers();
		SteamFriends()->SetRichPresence( "status", "Finding an internet game" );
		pchSteamRichPresenceDisplay = "WaitingForMatch";
	}
	else if ( m_eGameState == k_EClientFindLANServers )
	{
		m_pServerBrowser->RefreshLANServers();
		SteamFriends()->SetRichPresence( "status", "Finding a LAN game" );
		pchSteamRichPresenceDisplay = "WaitingForMatch";
	}
	else if ( m_eGameState == k_EClientCreatingLobby )
	{
		// start creating the lobby
		if ( !m_SteamCallResultLobbyCreated.IsActive() )
		{
			// ask steam to create a lobby
			SteamAPICall_t hSteamAPICall = SteamMatchmaking()->CreateLobby( k_ELobbyTypePublic /* public lobby, anyone can find it */, 4 );
			// set the function to call when this completes
			m_SteamCallResultLobbyCreated.Set( hSteamAPICall, this, &CSpaceWarClient::OnLobbyCreated );
		}
		SteamFriends()->SetRichPresence( "status", "Creating a lobby" );
		pchSteamRichPresenceDisplay = "WaitingForMatch";
	}
	else if ( m_eGameState == k_EClientInLobby )
	{
		pchSteamRichPresenceDisplay = "WaitingForMatch";
	}
	else if ( m_eGameState == k_EClientFindLobby )
	{
		m_pLobbyBrowser->Refresh();
		SteamFriends()->SetRichPresence( "status", "Main menu: finding lobbies" );
		pchSteamRichPresenceDisplay = "WaitingForMatch";
	}
	else if ( m_eGameState == k_EClientGameMenu )
	{
		// we've switched out to the main menu

		// Tell the server we have left if we are connected
		DisconnectFromServer();

		// shut down any server we were running
		if ( m_pServer )
		{
			delete m_pServer;
			m_pServer = NULL;
		}

		SteamFriends()->SetRichPresence( "status", "Main menu" );

		// Refresh inventory
		SpaceWarLocalInventory()->RefreshFromServer();
	}
	else if ( m_eGameState == k_EClientGameWinner || m_eGameState == k_EClientGameDraw )
	{
		// game over.. update the leaderboard
		m_pLeaderboards->UpdateLeaderboards( m_pStatsAndAchievements );

		// Check if the user is due for an item drop
		SpaceWarLocalInventory()->CheckForItemDrops();

		pchSteamRichPresenceDisplay = SetInGameRichPresence();
		bDisplayScoreInRichPresence = true;
	}
	else if ( m_eGameState == k_EClientLeaderboards )
	{
		// we've switched to the leaderboard menu
		m_pLeaderboards->Show();
		SteamFriends()->SetRichPresence( "status", "Viewing leaderboards" );
	}
	else if ( m_eGameState == k_EClientFriendsList )
	{
		// we've switched to the friends list menu
		m_pFriendsList->Show();
		SteamFriends()->SetRichPresence( "status", "Viewing friends list" );
	}
	else if ( m_eGameState == k_EClientClanChatRoom )
	{
		// we've switched to the leaderboard menu
		m_pClanChatRoom->Show();
		SteamFriends()->SetRichPresence( "status", "Chatting" );
	}
	else if ( m_eGameState == k_EClientGameActive )
	{
		// Load Inventory
		SpaceWarLocalInventory()->RefreshFromServer();

		// start voice chat 
		m_pVoiceChat->StartVoiceChat();
		SteamFriends()->SetRichPresence( "status", "In match" );

		pchSteamRichPresenceDisplay = SetInGameRichPresence();
		bDisplayScoreInRichPresence = true;
	}
	else if ( m_eGameState == k_EClientRemotePlay )
	{
		// we've switched to the remote play menu
		m_pRemotePlayList->Show();
		SteamFriends()->SetRichPresence( "status", "Viewing remote play sessions" );
	}
	else if ( m_eGameState == k_EClientRemoteStorage )
	{
		// we've switched to the remote storage menu
		m_pRemoteStorage->Show();
		SteamFriends()->SetRichPresence( "status", "Viewing remote storage" );
	}
	else if ( m_eGameState == k_EClientMusic )
	{
		// we've switched to the music player menu
		m_pMusicPlayer->Show();
		SteamFriends()->SetRichPresence( "status", "Using music player" );
	}
	else if ( m_eGameState == k_EClientHTMLSurface )
	{
		// we've switched to the html page
		m_pHTMLSurface->Show();
		SteamFriends()->SetRichPresence("status", "Using the web");
	}

	if ( pchSteamRichPresenceDisplay != NULL )
	{
		SteamFriends()->SetRichPresence( "steam_display", bDisplayScoreInRichPresence ? "#StatusWithScore" : "#StatusWithoutScore" );
		SteamFriends()->SetRichPresence( "gamestatus", pchSteamRichPresenceDisplay );
	}

	// steam_player_group defines who the user is playing with.  Set it to the steam ID
	// of the server if we are connected, otherwise blank.
	if ( m_steamIDGameServer.IsValid() )
	{
		char rgchBuffer[32];
		sprintf_safe( rgchBuffer, "%llu", m_steamIDGameServer.ConvertToUint64() );
		SteamFriends()->SetRichPresence( "steam_player_group", rgchBuffer );
	}
	else
	{
		SteamFriends()->SetRichPresence( "steam_player_group", "" );
	}

}

//-----------------------------------------------------------------------------
// Purpose: Handles notification of a steam ipc failure
// we may get multiple callbacks, one for each IPC operation we attempted
// since the actual failure, so protect ourselves from alerting more than once.
//-----------------------------------------------------------------------------
void CSpaceWarClient::OnIPCFailure( IPCFailure_t *failure )
{
	static bool bExiting = false;
	if ( !bExiting )
	{
		OutputDebugString( "Steam IPC Failure, shutting down\n" );
#if defined( _WIN32 )
		::MessageBoxA( NULL, "Connection to Steam Lost, Exiting", "Steam Connection Error", MB_OK );
#endif
		m_pGameEngine->Shutdown();
		bExiting = true;
	}
}

//-----------------------------------------------------------------------------
// Purpose: Handles notification of a Steam shutdown request since a Windows
// user in a second concurrent session requests to play this game. Shutdown
// this process immediately if possible.
//-----------------------------------------------------------------------------
void CSpaceWarClient::OnSteamShutdown( SteamShutdown_t *callback )
{
	static bool bExiting = false;
	if ( !bExiting )
	{
		OutputDebugString( "Steam shutdown request, shutting down\n" );
		m_pGameEngine->Shutdown();
		bExiting = true;
	}
}


//-----------------------------------------------------------------------------
// Purpose: Handles notification that we are now connected to Steam
//-----------------------------------------------------------------------------
void CSpaceWarClient::OnSteamServersConnected( SteamServersConnected_t *callback )
{
	if ( SteamUser()->BLoggedOn() )
		m_eGameState = k_EClientGameMenu;
	else
	{
		OutputDebugString( "Got SteamServersConnected_t, but not logged on?\n" );
	}
}


//-----------------------------------------------------------------------------
// Purpose: Handles notification that we are now connected to Steam
//-----------------------------------------------------------------------------
void CSpaceWarClient::OnSteamServersDisconnected( SteamServersDisconnected_t *callback )
{
	SetGameState( k_EClientConnectingToSteam );
	m_pConnectingMenu->OnConnectFailure();
	OutputDebugString( "Got SteamServersDisconnected_t\n" );
}


//-----------------------------------------------------------------------------
// Purpose: Handles notification that the Steam overlay is shown/hidden, note, this
// doesn't mean the overlay will or will not draw, it may still draw when not active.
// This does mean the time when the overlay takes over input focus from the game.
//-----------------------------------------------------------------------------
void CSpaceWarClient::OnGameOverlayActivated( GameOverlayActivated_t *callback )
{
	if ( callback->m_bActive )	
		OutputDebugString( "Steam overlay now active\n" );
	else
		OutputDebugString( "Steam overlay now inactive\n" );
}


//-----------------------------------------------------------------------------
// Purpose: Handle the callback from the user clicking a steam://gamewebcallback/ link in the overlay browser
//	You can use this to add support for external site signups where you want to pop back into the browser
//  after some web page signup sequence, and optionally get back some detail about that.
//-----------------------------------------------------------------------------
void CSpaceWarClient::OnGameWebCallback( GameWebCallback_t *callback )
{
	m_bSentWebOpen = false;
	char rgchString[256];
	sprintf_safe( rgchString, "User submitted following url: %s\n", callback->m_szURL );
	OutputDebugString( rgchString );
}


//-----------------------------------------------------------------------------
// Purpose: Handles notification that we are failed to connected to Steam
//-----------------------------------------------------------------------------
void CSpaceWarClient::OnSteamServerConnectFailure( SteamServerConnectFailure_t *callback )
{
	char rgchString[256];
	sprintf_safe( rgchString, "SteamServerConnectFailure_t: %d\n", callback->m_eResult );

	m_pConnectingMenu->OnConnectFailure();
}


//-----------------------------------------------------------------------------
// Purpose: Main frame function, updates the state of the world and performs rendering
//-----------------------------------------------------------------------------
void CSpaceWarClient::RunFrame()
{
	// Get any new data off the network to begin with
	ReceiveNetworkData();

	if ( m_eConnectedStatus != k_EClientNotConnected && m_pGameEngine->GetGameTickCount() - m_ulLastNetworkDataReceivedTime > MILLISECONDS_CONNECTION_TIMEOUT )
	{
		SetConnectionFailureText( "Game server connection failure." );
		DisconnectFromServer(); // cleanup on our side, even though server won't get our disconnect msg
		SetGameState( k_EClientGameConnectionFailure );
	}

	// Check if escape has been pressed, we'll use that info in a couple places below
	bool bEscapePressed = false;
	if ( m_pGameEngine->BIsKeyDown( VK_ESCAPE ) ||
		m_pGameEngine->BIsControllerActionActive( eControllerDigitalAction_PauseMenu ) )
	{
		static uint64 m_ulLastESCKeyTick = 0;
		uint64 ulCurrentTickCount = m_pGameEngine->GetGameTickCount();
		if ( ulCurrentTickCount - 250 > m_ulLastESCKeyTick )
		{
			m_ulLastESCKeyTick = ulCurrentTickCount;
			bEscapePressed = true;
		}
	}

	// Run Steam client callbacks
	SteamAPI_RunCallbacks();

	// For now, run stats/achievements every frame
	m_pStatsAndAchievements->RunFrame();

	// if we just transitioned state, perform on change handlers
	if ( m_bTransitionedGameState )
	{
		m_bTransitionedGameState = false;
		OnGameStateChanged( m_eGameState );
	}

	// Update state for everything
	switch ( m_eGameState )
	{
	case k_EClientConnectingToSteam:
		m_pStarField->Render();
		m_pConnectingMenu->RunFrame();
		// Make sure the Steam Controller is in the correct mode.
		m_pGameEngine->SetSteamControllerActionSet( eControllerActionSet_MenuControls );
		break;
	case k_EClientRetrySteamConnection:
#ifdef _PS3
		m_pStarField->Render();
		SteamUser()->LogOn( true );
		m_pConnectingMenu->Reset();
		SetGameState( k_EClientConnectingToSteam );
#else
		OutputDebugString( "Invalidate state k_EClientRetrySteamConnection hit on non-PS3 platform" );
#endif
		break;
	case k_EClientLinkSteamAccount:
#ifdef _PS3
		m_pStarField->Render();
		SteamUser()->LogOnAndLinkSteamAccountToPSN( true, "jmccaskeybeta", "test123" );
		m_pConnectingMenu->Reset();
		SetGameState( k_EClientConnectingToSteam );
#else
		OutputDebugString( "Invalidate state k_EClientLinkSteamAccount hit on non-PS3 platform" );
#endif
		break;
	case k_EClientAutoCreateAccount:
#ifdef _PS3
		m_pStarField->Render();
		m_pConnectingMenu->Reset();
		SteamUser()->LogOnAndCreateNewSteamAccountIfNeeded( true );
		SetGameState( k_EClientConnectingToSteam );
#else
		OutputDebugString( "Invalidate state k_EClientAutoCreateAccount hit on non-PS3 platform" );
#endif
		break;
	case k_EClientGameMenu:
		m_pStarField->Render();
		m_pMainMenu->RunFrame();
		// Make sure the Steam Controller is in the correct mode.
		m_pGameEngine->SetSteamControllerActionSet( eControllerActionSet_MenuControls );
		break;
	case k_EClientFindInternetServers:
	case k_EClientFindLANServers:
		m_pStarField->Render();
		m_pServerBrowser->RunFrame();
		break;
	
	case k_EClientCreatingLobby:
		m_pStarField->Render();
		// draw some text about creating lobby (may take a second or two)
		break;

	case k_EClientInLobby:
		m_pStarField->Render();
		// display the lobby
		m_pLobby->RunFrame();
		
		// see if we have a game server ready to play on
		if ( m_pServer && m_pServer->IsConnectedToSteam() )
		{
			// server is up; tell everyone else to connect
			SteamMatchmaking()->SetLobbyGameServer( m_steamIDLobby, 0, 0, m_pServer->GetSteamID() );
			// start connecting ourself via localhost (this will automatically leave the lobby)
			InitiateServerConnection( m_pServer->GetSteamID() );
		}
		break;

	case k_EClientFindLobby:
		m_pStarField->Render();

		// display the list of lobbies
		m_pLobbyBrowser->RunFrame();
		break;

	case k_EClientJoiningLobby:
		m_pStarField->Render();

		// Draw text telling the user a connection attempt is in progress
		DrawConnectionAttemptText();

		// Check if we've waited too long and should time out the connection
		if ( m_pGameEngine->GetGameTickCount() - m_ulStateTransitionTime > MILLISECONDS_CONNECTION_TIMEOUT )
		{
			SetConnectionFailureText( "Timed out connecting to lobby." );
			SetGameState( k_EClientGameConnectionFailure );
		}
		break;

	case k_EClientGameConnectionFailure:
		m_pStarField->Render();
		DrawConnectionFailureText();

		if ( bEscapePressed )
			SetGameState( k_EClientGameMenu );

		break;
	case k_EClientGameConnecting:
		m_pStarField->Render();

		// Draw text telling the user a connection attempt is in progress
		DrawConnectionAttemptText();

		// Check if we've waited too long and should time out the connection
		if ( m_pGameEngine->GetGameTickCount() - m_ulStateTransitionTime > MILLISECONDS_CONNECTION_TIMEOUT )
		{
			if ( m_pP2PAuthedGame )
				m_pP2PAuthedGame->EndGame();
			if ( m_eConnectedStatus == k_EClientConnectedAndAuthenticated )
			{
				SteamUser()->TerminateGameConnection( m_unServerIP, m_usServerPort );
			}
			m_GameServerPing.CancelPing();
			SetConnectionFailureText( "Timed out connecting to game server" );
			SetGameState( k_EClientGameConnectionFailure );
		}

		break;
	case k_EClientGameQuitMenu:
		m_pStarField->Render();

		// Update all the entities (this is client side interpolation)...
		m_pSun->RunFrame();
		for( uint32 i=0; i<MAX_PLAYERS_PER_SERVER; ++i )
		{
			if ( m_rgpShips[i] )
				m_rgpShips[i]->RunFrame();
		}

		// Now draw the menu
		m_pQuitMenu->RunFrame();

		// Make sure the Steam Controller is in the correct mode.
		m_pGameEngine->SetSteamControllerActionSet( eControllerActionSet_MenuControls );
		break;
	case k_EClientGameInstructions:
		m_pStarField->Render();
		DrawInstructions();

		if ( bEscapePressed )
			SetGameState( k_EClientGameMenu );
		break;
	case k_EClientWorkshop:
		m_pStarField->Render();
		DrawWorkshopItems();

		if (bEscapePressed)
			SetGameState(k_EClientGameMenu);
		break;

	case k_EClientStatsAchievements:
		m_pStarField->Render();
		m_pStatsAndAchievements->Render();

		if ( bEscapePressed )
			SetGameState( k_EClientGameMenu );
		if (m_pGameEngine->BIsKeyDown( 0x31 ) )
		{
			SpaceWarLocalInventory()->DoExchange();
		}
		else if ( m_pGameEngine->BIsKeyDown( 0x32 ) )
		{
			SpaceWarLocalInventory()->ModifyItemProperties();
		}
		break;
	case k_EClientLeaderboards:
		m_pStarField->Render();
		m_pLeaderboards->RunFrame();		

		if ( bEscapePressed )
			SetGameState( k_EClientGameMenu );
		break;

	case k_EClientFriendsList:
		m_pStarField->Render();
		m_pFriendsList->RunFrame();

		if ( bEscapePressed )
			SetGameState( k_EClientGameMenu );
		break;

	case k_EClientClanChatRoom:
		m_pStarField->Render();
		m_pClanChatRoom->RunFrame();		

		if ( bEscapePressed )
			SetGameState( k_EClientGameMenu );
		break;

	case k_EClientRemotePlay:
		m_pStarField->Render();
		m_pRemotePlayList->RunFrame();

		if ( bEscapePressed )
			SetGameState( k_EClientGameMenu );
		break;

	case k_EClientRemoteStorage:
		m_pStarField->Render();
		m_pRemoteStorage->Render();
		break;

	case k_EClientHTMLSurface:
		m_pHTMLSurface->RunFrame();
		m_pHTMLSurface->Render();
		break;


	case k_EClientMinidump:
#ifdef _WIN32
		RaiseException( EXCEPTION_NONCONTINUABLE_EXCEPTION,
			EXCEPTION_NONCONTINUABLE,
			0, NULL );
#endif
		SetGameState( k_EClientGameMenu );
		break;

	case k_EClientGameStartServer:
		m_pStarField->Render();
		if ( !m_pServer )
		{
			m_pServer = new CSpaceWarServer( m_pGameEngine );
		}

		if ( m_pServer && m_pServer->IsConnectedToSteam() )
		{
			// server is ready, connect to it
			InitiateServerConnection( m_pServer->GetSteamID() );
		}
		break;
	case k_EClientGameDraw:
	case k_EClientGameWinner:
	case k_EClientGameWaitingForPlayers:
		m_pStarField->Render();

		// Update all the entities (this is client side interpolation)...
		m_pSun->RunFrame();
		for( uint32 i=0; i<MAX_PLAYERS_PER_SERVER; ++i )
		{
			if ( m_rgpShips[i] )
				m_rgpShips[i]->RunFrame();
		}

		DrawHUDText();
		DrawWinnerDrawOrWaitingText();

		m_pVoiceChat->RunFrame();

		if ( bEscapePressed )
			SetGameState( k_EClientGameQuitMenu );

		break;

	case k_EClientGameActive:
		// Make sure the Steam Controller is in the correct mode.
		m_pGameEngine->SetSteamControllerActionSet( eControllerActionSet_ShipControls );

		m_pStarField->Render();
		
		// SendHeartbeat is safe to call on every frame since the API is internally rate-limited.
		// Ideally you would only call this once per second though, to minimize unnecessary calls.
		SteamInventory()->SendItemDropHeartbeat();

		// Update all the entities...
		m_pSun->RunFrame();
		for( uint32 i=0; i<MAX_PLAYERS_PER_SERVER; ++i )
		{
			if ( m_rgpShips[i] )
				m_rgpShips[i]->RunFrame();
		}

		for (uint32 i = 0; i < MAX_WORKSHOP_ITEMS; ++i)
		{
			if (m_rgpWorkshopItems[i])
				m_rgpWorkshopItems[i]->RunFrame();
		}


		DrawHUDText();

		m_pStatsAndAchievements->RunFrame();
		
		m_pVoiceChat->RunFrame();

		if ( bEscapePressed )
			SetGameState( k_EClientGameQuitMenu );

		break;
	case k_EClientGameExiting:
		DisconnectFromServer();
		m_pGameEngine->Shutdown();
		return;
	case k_EClientWebCallback:
		m_pStarField->Render();

		if ( !m_bSentWebOpen )
		{
			m_bSentWebOpen = true;
#ifndef _PS3
			char szCurDir[MAX_PATH];
			if ( !_getcwd( szCurDir, sizeof(szCurDir) ) )
            {
                strcpy( szCurDir, "." );
            }
			char szURL[MAX_PATH];
			sprintf_safe( szURL, "file:///%s/test.html", szCurDir );
			// load the test html page, it just has a steam://gamewebcallback link in it
			SteamFriends()->ActivateGameOverlayToWebPage( szURL );
			SetGameState( k_EClientGameMenu );
#endif
		}

		break;
	case k_EClientMusic:
		m_pStarField->Render();

		m_pMusicPlayer->RunFrame();		

		if ( bEscapePressed )
		{
			SetGameState( k_EClientGameMenu );
		}
		break;

	case k_EClientInGameStore:
		m_pStarField->Render();
		DrawInGameStore();

		if (bEscapePressed)
			SetGameState(k_EClientGameMenu);
		break;
		
	default:
		OutputDebugString( "Unhandled game state in CSpaceWar::RunFrame\n" );
	}


	// Send an update on our local ship to the server
	if ( m_eConnectedStatus == k_EClientConnectedAndAuthenticated &&  m_rgpShips[ m_uPlayerShipIndex ] )
	{
		MsgClientSendLocalUpdate_t msg;
		msg.SetShipPosition( m_uPlayerShipIndex );

		// If this fails, it probably just means its not time to send an update yet
		if ( m_rgpShips[ m_uPlayerShipIndex ]->BGetClientUpdateData( msg.AccessUpdateData() ) )
			BSendServerData( &msg, sizeof( msg ) );
	}

	if ( m_pP2PAuthedGame )
	{
		if ( m_pServer )
		{
			// Now if we are the owner of the game, lets make sure all of our players are legit.
			// if they are not, we tell the server to kick them off
			// Start at 1 to skip myself
			for ( int i = 1; i < MAX_PLAYERS_PER_SERVER; i++ )
			{
				if ( m_pP2PAuthedGame->m_rgpP2PAuthPlayer[i] && !m_pP2PAuthedGame->m_rgpP2PAuthPlayer[i]->BIsAuthOk() )
				{
					m_pServer->KickPlayerOffServer( m_pP2PAuthedGame->m_rgpP2PAuthPlayer[i]->m_steamID );
				}
			}
		}
		else
		{
			// If we are not the owner of the game, lets make sure the game owner is legit
			// if he is not, we leave the game
			if ( m_pP2PAuthedGame->m_rgpP2PAuthPlayer[0] )
			{
				if ( !m_pP2PAuthedGame->m_rgpP2PAuthPlayer[0]->BIsAuthOk() )
				{
					// leave the game
					SetGameState( k_EClientGameMenu );
				}
			}
		}
	}

	// If we've started a local server run it
	if ( m_pServer )
	{
		m_pServer->RunFrame();
	}

	// Accumulate stats
	for( uint32 i=0; i<MAX_PLAYERS_PER_SERVER; ++i )
	{
		if ( m_rgpShips[i] )
			m_rgpShips[i]->AccumulateStats( m_pStatsAndAchievements );
	}

	// Render everything that might have been updated by the server
	switch ( m_eGameState )
	{
	case k_EClientGameDraw:
	case k_EClientGameWinner:
	case k_EClientGameActive:
		// Now render all the objects
		m_pSun->Render();
		for( uint32 i=0; i<MAX_PLAYERS_PER_SERVER; ++i )
		{
			if ( m_rgpShips[i] )
				m_rgpShips[i]->Render();
		}

		for (uint32 i = 0; i < MAX_WORKSHOP_ITEMS; ++i)
		{
			if ( m_rgpWorkshopItems[i] )
				m_rgpWorkshopItems[i]->Render();
		}

		break;
	default:
		// Any needed drawing was already done above before server updates
		break;
	}
}


//-----------------------------------------------------------------------------
// Purpose: Draws some HUD text indicating game status
//-----------------------------------------------------------------------------
void CSpaceWarClient::DrawHUDText()
{
	// Padding from the edge of the screen for hud elements
#ifdef _PS3
	// Larger padding on PS3, since many of our test HDTVs truncate 
	// edges of the screen and can't be calibrated properly.
	const int32 nHudPaddingVertical = 20;
	const int32 nHudPaddingHorizontal = 35;
#else
	const int32 nHudPaddingVertical = 15;
	const int32 nHudPaddingHorizontal = 15;
#endif


	const int32 width = m_pGameEngine->GetViewportWidth();
	const int32 height = m_pGameEngine->GetViewportHeight();

	const int32 nAvatarWidth = 64;
	const int32 nAvatarHeight = 64;

	const int32 nSpaceBetweenAvatarAndScore = 6;

	LONG scorewidth = LONG((m_pGameEngine->GetViewportWidth() - nHudPaddingHorizontal*2.0f)/4.0f);

	char rgchBuffer[256];
	for( uint32 i=0; i<MAX_PLAYERS_PER_SERVER; ++i )
	{
		// Draw nothing in the spot for an inactive player
		if ( !m_rgpShips[i] )
			continue;


		// We use Steam persona names for our players in-game name.  To get these we 
		// just call SteamFriends()->GetFriendPersonaName() this call will work on friends, 
		// players on the same game server as us (if using the Steam game server auth API) 
		// and on ourself.
		char rgchPlayerName[128];
		CSteamID playerSteamID( m_rgSteamIDPlayers[i] );

		const char *pszVoiceState = m_pVoiceChat->IsPlayerTalking( playerSteamID ) ? "(VoiceChat)" : "";

		if ( m_rgSteamIDPlayers[i].IsValid() )
		{
			sprintf_safe( rgchPlayerName, "%s", SteamFriends()->GetFriendPersonaName( playerSteamID ) );
		}
		else
		{
			sprintf_safe( rgchPlayerName, "Unknown Player" );
		}

		// We also want to use the Steam Avatar image inside the HUD if it is available.
		// We look it up via GetMediumFriendAvatar, which returns an image index we use
		// to look up the actual RGBA data below.
		int iImage = SteamFriends()->GetMediumFriendAvatar( playerSteamID );
		HGAMETEXTURE hTexture = 0;
		if ( iImage != -1 )
			hTexture = GetSteamImageAsTexture( iImage );

		RECT rect;
		switch( i )
		{
		case 0:
			rect.top = nHudPaddingVertical;
			rect.bottom = rect.top+nAvatarHeight;
			rect.left = nHudPaddingHorizontal;
			rect.right = rect.left + scorewidth;

			if ( hTexture )
			{
				m_pGameEngine->BDrawTexturedRect( (float)rect.left, (float)rect.top, (float)rect.left+nAvatarWidth, (float)rect.bottom, 
					0.0f, 0.0f, 1.0, 1.0, D3DCOLOR_ARGB( 255, 255, 255, 255 ), hTexture );
				rect.left += nAvatarWidth + nSpaceBetweenAvatarAndScore;
				rect.right += nAvatarWidth + nSpaceBetweenAvatarAndScore;
			}
			
			sprintf_safe( rgchBuffer, "%s\nScore: %2u %s", rgchPlayerName, m_rguPlayerScores[i], pszVoiceState );
			m_pGameEngine->BDrawString( m_hHUDFont, rect, g_rgPlayerColors[i], TEXTPOS_LEFT|TEXTPOS_VCENTER, rgchBuffer );
			break;
		case 1:

			rect.top = nHudPaddingVertical;
			rect.bottom = rect.top+nAvatarHeight;
			rect.left = width-nHudPaddingHorizontal-scorewidth;
			rect.right = width-nHudPaddingHorizontal;

			if ( hTexture )
			{
				m_pGameEngine->BDrawTexturedRect( (float)rect.right - nAvatarWidth, (float)rect.top, (float)rect.right, (float)rect.bottom, 
					0.0f, 0.0f, 1.0, 1.0, D3DCOLOR_ARGB( 255, 255, 255, 255 ), hTexture );
				rect.right -= nAvatarWidth + nSpaceBetweenAvatarAndScore;
				rect.left -= nAvatarWidth + nSpaceBetweenAvatarAndScore;
			}

			sprintf_safe( rgchBuffer, "%s\nScore: %2u ", rgchPlayerName, m_rguPlayerScores[i] );
			m_pGameEngine->BDrawString( m_hHUDFont, rect, g_rgPlayerColors[i], TEXTPOS_RIGHT|TEXTPOS_VCENTER, rgchBuffer );
			break;
		case 2:
			rect.top = height-nHudPaddingVertical-nAvatarHeight;
			rect.bottom = rect.top+nAvatarHeight;
			rect.left = nHudPaddingHorizontal;
			rect.right = rect.left + scorewidth;

			if ( hTexture )
			{
				m_pGameEngine->BDrawTexturedRect( (float)rect.left, (float)rect.top, (float)rect.left+nAvatarWidth, (float)rect.bottom, 
					0.0f, 0.0f, 1.0, 1.0, D3DCOLOR_ARGB( 255, 255, 255, 255 ), hTexture );
				rect.right += nAvatarWidth + nSpaceBetweenAvatarAndScore;
				rect.left += nAvatarWidth + nSpaceBetweenAvatarAndScore;
			}

			sprintf_safe( rgchBuffer, "%s\nScore: %2u %s", rgchPlayerName, m_rguPlayerScores[i], pszVoiceState );
			m_pGameEngine->BDrawString( m_hHUDFont, rect, g_rgPlayerColors[i], TEXTPOS_LEFT|TEXTPOS_BOTTOM, rgchBuffer );
			break;
		case 3:
			rect.top = height-nHudPaddingVertical-nAvatarHeight;
			rect.bottom = rect.top+nAvatarHeight;
			rect.left = width-nHudPaddingHorizontal-scorewidth;
			rect.right = width-nHudPaddingHorizontal;

			if ( hTexture )
			{
				m_pGameEngine->BDrawTexturedRect( (float)rect.right - nAvatarWidth, (float)rect.top, (float)rect.right, (float)rect.bottom, 
					0.0f, 0.0f, 1.0, 1.0, D3DCOLOR_ARGB( 255, 255, 255, 255 ), hTexture );
				rect.right -= nAvatarWidth + nSpaceBetweenAvatarAndScore;
				rect.left -= nAvatarWidth + nSpaceBetweenAvatarAndScore;
			}

			sprintf_safe( rgchBuffer, "%s\nScore: %2u %s", rgchPlayerName, m_rguPlayerScores[i], pszVoiceState );
			m_pGameEngine->BDrawString( m_hHUDFont, rect, g_rgPlayerColors[i], TEXTPOS_RIGHT|TEXTPOS_BOTTOM, rgchBuffer );
			break;
		default:
			OutputDebugString( "DrawHUDText() needs updating for more players\n" );
			break;
		}
	}

	// Draw a Steam Input tooltip
	if ( m_pGameEngine->BIsSteamInputDeviceActive( ) )
	{
		char rgchHint[128];
		const char *rgchFireOrigin = m_pGameEngine->GetTextStringForControllerOriginDigital( eControllerActionSet_ShipControls, eControllerDigitalAction_FireLasers );

		if ( strcmp( rgchFireOrigin, "None" ) == 0 )
		{
			sprintf_safe( rgchHint, "No Fire action bound." );
		}
		else
		{
			sprintf_safe( rgchHint, "Press '%s' to Fire", rgchFireOrigin );
		}

		RECT rect;
		int nBorder = 30;
		rect.top = m_pGameEngine->GetViewportHeight( ) - nBorder;
		rect.bottom = m_pGameEngine->GetViewportHeight( )*2;
		rect.left = nBorder;
		rect.right = m_pGameEngine->GetViewportWidth( );
		m_pGameEngine->BDrawString( m_hHUDFont, rect, D3DCOLOR_ARGB( 255, 255, 255, 255 ), TEXTPOS_LEFT | TEXTPOS_TOP, rgchHint );
	}

}


//-----------------------------------------------------------------------------
// Purpose: Draws some instructions on how to play the game
//-----------------------------------------------------------------------------
void CSpaceWarClient::DrawInstructions()
{
	const int32 width = m_pGameEngine->GetViewportWidth();

	RECT rect;
	rect.top = 0;
	rect.bottom = m_pGameEngine->GetViewportHeight();
	rect.left = 0;
	rect.right = width;

	char rgchBuffer[256];
#ifdef _PS3
	sprintf_safe( rgchBuffer, "Turn Ship Left: 'Left'\nTurn Ship Right: 'Right'\nForward Thrusters: 'R2'\nReverse Thrusters: 'L2'\nFire Photon Beams: 'Cross'" );
#else
	sprintf_safe( rgchBuffer, "Turn Ship Left: 'A'\nTurn Ship Right: 'D'\nForward Thrusters: 'W'\nReverse Thrusters: 'S'\nFire Photon Beams: 'Space'" );
#endif

	m_pGameEngine->BDrawString( m_hInstructionsFont, rect, D3DCOLOR_ARGB( 255, 25, 200, 25 ), TEXTPOS_CENTER|TEXTPOS_VCENTER, rgchBuffer );

	
	rect.left = 0;
	rect.right = width;
	rect.top = LONG(m_pGameEngine->GetViewportHeight() * 0.7);
	rect.bottom = m_pGameEngine->GetViewportHeight();

	sprintf_safe( rgchBuffer, "Press ESC to return to the Main Menu\n Build ID:%d", SteamApps()->GetAppBuildId() );
	m_pGameEngine->BDrawString( m_hInstructionsFont, rect, D3DCOLOR_ARGB( 255, 25, 200, 25 ), TEXTPOS_CENTER|TEXTPOS_TOP, rgchBuffer );

}

//-----------------------------------------------------------------------------
// Purpose: Draws some text indicating a connection attempt is in progress
//-----------------------------------------------------------------------------
void CSpaceWarClient::DrawConnectionAttemptText()
{
	const int32 width = m_pGameEngine->GetViewportWidth();

	RECT rect;
	rect.top = 0;
	rect.bottom = m_pGameEngine->GetViewportHeight();
	rect.left = 0;
	rect.right = width;

	// Figure out how long we are still willing to wait for success
	uint32 uSecondsLeft = (MILLISECONDS_CONNECTION_TIMEOUT - uint32(m_pGameEngine->GetGameTickCount() - m_ulStateTransitionTime ))/1000;

	char rgchTimeoutString[256];
	if ( uSecondsLeft < 25 )
		sprintf_safe( rgchTimeoutString, ", timeout in %u...\n", uSecondsLeft );
	else
		sprintf_safe( rgchTimeoutString, "...\n" );
		

	char rgchBuffer[256];
	if ( m_eGameState == k_EClientJoiningLobby )
		sprintf_safe( rgchBuffer, "Connecting to lobby%s", rgchTimeoutString );
	else
		sprintf_safe( rgchBuffer, "Connecting to server%s", rgchTimeoutString );

	m_pGameEngine->BDrawString( m_hInstructionsFont, rect, D3DCOLOR_ARGB( 255, 25, 200, 25 ), TEXTPOS_CENTER|TEXTPOS_VCENTER, rgchBuffer );
}


//-----------------------------------------------------------------------------
// Purpose: Draws some text indicating a connection failure
//-----------------------------------------------------------------------------
void CSpaceWarClient::DrawConnectionFailureText()
{
	const int32 width = m_pGameEngine->GetViewportWidth();

	RECT rect;
	rect.top = 0;
	rect.bottom = m_pGameEngine->GetViewportHeight();
	rect.left = 0;
	rect.right = width;

	char rgchBuffer[256];
	sprintf_safe( rgchBuffer, "%s\n", m_rgchErrorText );
	m_pGameEngine->BDrawString( m_hInstructionsFont, rect, D3DCOLOR_ARGB( 255, 25, 200, 25 ), TEXTPOS_CENTER|TEXTPOS_VCENTER, rgchBuffer );

	rect.left = 0;
	rect.right = width;
	rect.top = LONG(m_pGameEngine->GetViewportHeight() * 0.7);
	rect.bottom = m_pGameEngine->GetViewportHeight();

	sprintf_safe( rgchBuffer, "Press ESC to return to the Main Menu" );
	m_pGameEngine->BDrawString( m_hInstructionsFont, rect, D3DCOLOR_ARGB( 255, 25, 200, 25 ), TEXTPOS_CENTER|TEXTPOS_TOP, rgchBuffer );
}


//-----------------------------------------------------------------------------
// Purpose: Draws some text about who just won (or that there was a draw)
//-----------------------------------------------------------------------------
void CSpaceWarClient::DrawWinnerDrawOrWaitingText()
{
	int nSecondsToRestart = ((MILLISECONDS_BETWEEN_ROUNDS - (int)(m_pGameEngine->GetGameTickCount() - m_ulStateTransitionTime) )/1000) + 1;
	if ( nSecondsToRestart < 0 )
		nSecondsToRestart = 0;

	RECT rect;
	rect.top = 0;
	rect.bottom = int(m_pGameEngine->GetViewportHeight()*0.6f);
	rect.left = 0;
	rect.right = m_pGameEngine->GetViewportWidth();

	char rgchBuffer[256];
	if ( m_eGameState == k_EClientGameWaitingForPlayers )
	{
		sprintf_safe( rgchBuffer, "Server is waiting for players.\n\nStarting in %d seconds...", nSecondsToRestart );
		m_pGameEngine->BDrawString( m_hInstructionsFont, rect, D3DCOLOR_ARGB( 255, 25, 200, 25 ), TEXTPOS_CENTER|TEXTPOS_VCENTER, rgchBuffer );
	} 
	else if ( m_eGameState == k_EClientGameDraw )
	{
		sprintf_safe( rgchBuffer, "The round is a draw!\n\nStarting again in %d seconds...", nSecondsToRestart );
		m_pGameEngine->BDrawString( m_hInstructionsFont, rect, D3DCOLOR_ARGB( 255, 25, 200, 25 ), TEXTPOS_CENTER|TEXTPOS_VCENTER, rgchBuffer );
	} 
	else if ( m_eGameState == k_EClientGameWinner )
	{
		if ( m_uPlayerWhoWonGame >= MAX_PLAYERS_PER_SERVER )
		{
			OutputDebugString( "Invalid winner value\n" );
			return;
		}

		char rgchPlayerName[128];
		if ( m_rgSteamIDPlayers[m_uPlayerWhoWonGame].IsValid() )
		{
			sprintf_safe( rgchPlayerName, "%s", SteamFriends()->GetFriendPersonaName( m_rgSteamIDPlayers[m_uPlayerWhoWonGame] ) );
		}
		else
		{
			sprintf_safe( rgchPlayerName, "Unknown Player" );
		}

		sprintf_safe( rgchBuffer, "%s wins!\n\nStarting again in %d seconds...", rgchPlayerName, nSecondsToRestart );
		
		m_pGameEngine->BDrawString( m_hInstructionsFont, rect, D3DCOLOR_ARGB( 255, 25, 200, 25 ), TEXTPOS_CENTER|TEXTPOS_VCENTER, rgchBuffer );
	}

	// Note: GetLastDroppedItem is the result of an async function, this will not render the reward right away. Could wait for it.
	const CSpaceWarItem *pItem = SpaceWarLocalInventory()->GetLastDroppedItem();
	if ( pItem )
	{
		// (We're not really bothering to localize everything else, this is just an example.)
		sprintf_safe( rgchBuffer, "You won a brand new %s!", pItem->GetLocalizedName().c_str() );

		rect.top = 0;
		rect.bottom = int(m_pGameEngine->GetViewportHeight()*0.4f);
		rect.left = 0;
		rect.right = m_pGameEngine->GetViewportWidth();
		m_pGameEngine->BDrawString( m_hInstructionsFont, rect, D3DCOLOR_ARGB( 255, 25, 200, 25 ), TEXTPOS_CENTER|TEXTPOS_VCENTER, rgchBuffer );
	}
}


//-----------------------------------------------------------------------------
// Purpose: Did we win the last game?
//-----------------------------------------------------------------------------
bool CSpaceWarClient::BLocalPlayerWonLastGame()
{
	if ( m_eGameState == k_EClientGameWinner )
	{
		if ( m_uPlayerWhoWonGame >= MAX_PLAYERS_PER_SERVER )
		{
			// ur
			return false;
		}

		if ( m_rgpShips[m_uPlayerWhoWonGame] && m_rgpShips[m_uPlayerWhoWonGame]->BIsLocalPlayer() )
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

//-----------------------------------------------------------------------------
// Purpose: Scale pixel sizes to "real" sizes
//-----------------------------------------------------------------------------
float CSpaceWarClient::PixelsToFeet( float flPixels )
{
	// This game is actual size! (at 72dpi) LOL
	// Those are very tiny ships, and an itty bitty neutron star

	float flReturn = ( flPixels / 72 ) / 12;

	return flReturn;
}


//-----------------------------------------------------------------------------
// Purpose: Get a specific Steam image RGBA as a game texture
//-----------------------------------------------------------------------------
HGAMETEXTURE CSpaceWarClient::GetSteamImageAsTexture( int iImage )
{
	HGAMETEXTURE hTexture = 0;

	// iImage of 0 from steam means no avatar is set
	if ( iImage )
	{
		std::map<int, HGAMETEXTURE>::iterator iter;
		iter = m_MapSteamImagesToTextures.find( iImage );
		if ( iter == m_MapSteamImagesToTextures.end() )
		{
			// We haven't created a texture for this image index yet, do so now

			// Get the image size from Steam, making sure it looks valid afterwards
			uint32 uAvatarWidth, uAvatarHeight;
			SteamUtils()->GetImageSize( iImage, &uAvatarWidth, &uAvatarHeight );
			if ( uAvatarWidth > 0 && uAvatarHeight > 0 )
			{
				// Get the actual raw RGBA data from Steam and turn it into a texture in our game engine
				byte *pAvatarRGBA = new byte[ uAvatarWidth * uAvatarHeight * 4];
				SteamUtils()->GetImageRGBA( iImage, (uint8*)pAvatarRGBA, uAvatarWidth * uAvatarHeight * 4 );
				hTexture = m_pGameEngine->HCreateTexture( pAvatarRGBA, uAvatarWidth, uAvatarHeight );
				delete[] pAvatarRGBA;
				if ( hTexture )
				{
					m_MapSteamImagesToTextures[ iImage ] = hTexture;
				}
			}
		}
		else
		{
			hTexture = iter->second;
		}
	}

	return hTexture;
}


//-----------------------------------------------------------------------------
// Purpose: Request an encrypted app ticket
//-----------------------------------------------------------------------------
uint32 k_unSecretData = 0x5444;
void CSpaceWarClient::RetrieveEncryptedAppTicket()
{	
	SteamAPICall_t hSteamAPICall = SteamUser()->RequestEncryptedAppTicket( &k_unSecretData, sizeof( k_unSecretData ) );
	m_SteamCallResultEncryptedAppTicket.Set( hSteamAPICall, this, &CSpaceWarClient::OnRequestEncryptedAppTicket );
}


//-----------------------------------------------------------------------------
// Purpose: Called when requested app ticket asynchronously completes
//-----------------------------------------------------------------------------
void CSpaceWarClient::OnRequestEncryptedAppTicket( EncryptedAppTicketResponse_t *pEncryptedAppTicketResponse, bool bIOFailure )
{
	if ( bIOFailure )
		return;

	if ( pEncryptedAppTicketResponse->m_eResult == k_EResultOK )
	{
		uint8 rgubTicket[1024];
		uint32 cubTicket;		
		SteamUser()->GetEncryptedAppTicket( rgubTicket, sizeof( rgubTicket), &cubTicket );


#ifdef _WIN32
		// normally at this point you transmit the encrypted ticket to the service that knows the decryption key
		// this code is just to demonstrate the ticket cracking library

		// included is the "secret" key for spacewar. normally this is secret
		const uint8 rgubKey[k_nSteamEncryptedAppTicketSymmetricKeyLen] = { 0xed, 0x93, 0x86, 0x07, 0x36, 0x47, 0xce, 0xa5, 0x8b, 0x77, 0x21, 0x49, 0x0d, 0x59, 0xed, 0x44, 0x57, 0x23, 0xf0, 0xf6, 0x6e, 0x74, 0x14, 0xe1, 0x53, 0x3b, 0xa3, 0x3c, 0xd8, 0x03, 0xbd, 0xbd };		

		uint8 rgubDecrypted[1024];
		uint32 cubDecrypted = sizeof( rgubDecrypted );
		if ( !SteamEncryptedAppTicket_BDecryptTicket( rgubTicket, cubTicket, rgubDecrypted, &cubDecrypted, rgubKey, sizeof( rgubKey ) ) )
		{
			OutputDebugString( "Ticket failed to decrypt\n" );
			return;
		}

		if ( !SteamEncryptedAppTicket_BIsTicketForApp( rgubDecrypted, cubDecrypted, SteamUtils()->GetAppID() ) )
			OutputDebugString( "Ticket for wrong app id\n" );

		CSteamID steamIDFromTicket;
		SteamEncryptedAppTicket_GetTicketSteamID( rgubDecrypted, cubDecrypted, &steamIDFromTicket );
		if ( steamIDFromTicket != SteamUser()->GetSteamID() )
			OutputDebugString( "Ticket for wrong user\n" );

		uint32 cubData;
		uint32 *punSecretData = (uint32 *)SteamEncryptedAppTicket_GetUserVariableData( rgubDecrypted, cubDecrypted, &cubData );
		if ( cubData != sizeof( uint32 ) || *punSecretData != k_unSecretData )
			OutputDebugString( "Failed to retrieve secret data\n" );
#endif
	}
	else if ( pEncryptedAppTicketResponse->m_eResult == k_EResultLimitExceeded )
	{
		OutputDebugString( "Calling RequestEncryptedAppTicket more than once per minute returns this error\n" );
	}
	else if ( pEncryptedAppTicketResponse->m_eResult == k_EResultDuplicateRequest )
	{
		OutputDebugString( "Calling RequestEncryptedAppTicket while there is already a pending request results in this error\n" );
	}
	else if ( pEncryptedAppTicketResponse->m_eResult == k_EResultNoConnection )
	{
		OutputDebugString( "Calling RequestEncryptedAppTicket while not connected to steam results in this error\n" );
	}
}


//-----------------------------------------------------------------------------
// Purpose: Updates what we show to friends about what we're doing and how to connect
//-----------------------------------------------------------------------------
void CSpaceWarClient::UpdateRichPresenceConnectionInfo()
{
	// connect string that will come back to us on the command line	when a friend tries to join our game
	char rgchConnectString[128];
	rgchConnectString[0] = 0;

	if ( m_eConnectedStatus == k_EClientConnectedAndAuthenticated && m_unServerIP && m_usServerPort )
	{
		// game server connection method
		sprintf_safe( rgchConnectString, "+connect %d:%d", m_unServerIP, m_usServerPort );
	}
	else if ( m_steamIDLobby.IsValid() )
	{
		// lobby connection method
		sprintf_safe( rgchConnectString, "+connect_lobby %llu", m_steamIDLobby.ConvertToUint64() );
	}

	SteamFriends()->SetRichPresence( "connect", rgchConnectString );
}


//-----------------------------------------------------------------------------
// Purpose: applies a command-line connect
//-----------------------------------------------------------------------------
void CSpaceWarClient::ExecCommandLineConnect( const char *pchServerAddress, const char *pchLobbyID )
{
	if ( pchServerAddress )
	{
		int32 octet0 = 0, octet1 = 0, octet2 = 0, octet3 = 0;
		int32 uPort = 0;
		int nConverted = sscanf( pchServerAddress, "%d.%d.%d.%d:%d", &octet0, &octet1, &octet2, &octet3, &uPort );
		if ( nConverted == 5 )
		{
			char rgchIPAddress[128];
			sprintf_safe( rgchIPAddress, "%d.%d.%d.%d", octet0, octet1, octet2, octet3 );
			uint32 unIPAddress = ( octet3 ) + ( octet2 << 8 ) + ( octet1 << 16 ) + ( octet0 << 24 );
			InitiateServerConnection( unIPAddress, uPort );
		}
	}

	// if +connect_lobby was used to specify a lobby to join, connect now
	if ( pchLobbyID )
	{
		CSteamID steamIDLobby( (uint64)atoll( pchLobbyID ) );
		if ( steamIDLobby.IsValid() )
		{
			// act just like we had selected it from the menu
			LobbyBrowserMenuItem_t menuItem = { steamIDLobby, k_EClientJoiningLobby };
			OnMenuSelection( menuItem );
		}
	}
}


//-----------------------------------------------------------------------------
// Purpose: parse CWorkshopItem from text file
//-----------------------------------------------------------------------------
CWorkshopItem *CSpaceWarClient::LoadWorkshopItemFromFile( const char *pszFileName )
{
	FILE *file = fopen( pszFileName, "rt");
	if (!file)
		return NULL;

	CWorkshopItem *pItem = NULL;

	char szLine[1024];

	if ( fgets(szLine, sizeof(szLine), file) )
	{
		float flXPos, flYPos, flXVelocity, flYVelocity;
		// initialize object
		if ( sscanf(szLine, "%f %f %f %f", &flXPos, &flYPos, &flXVelocity, &flYVelocity) )
		{
			pItem = new CWorkshopItem( m_pGameEngine, 0 );

			pItem->SetPosition( flXPos, flYPos );
			pItem->SetVelocity( flXVelocity, flYVelocity );

			while (!feof(file))
			{
				float xPos0, yPos0, xPos1, yPos1;
				DWORD dwColor;
				if ( fgets(szLine, sizeof(szLine), file) &&
                     sscanf(szLine, "%f %f %f %f %x", &xPos0, &yPos0, &xPos1, &yPos1, &dwColor) >= 5 )
				{
					// Add a line to the entity
					pItem->AddLine(xPos0, yPos0, xPos1, yPos1, dwColor);
				}
			}
		}
	}

	fclose(file);

	return pItem;
}


//-----------------------------------------------------------------------------
// Purpose: load a Workshop item by PublishFileID
//-----------------------------------------------------------------------------
bool CSpaceWarClient::LoadWorkshopItem( PublishedFileId_t workshopItemID )
{
	if ( m_nNumWorkshopItems == MAX_WORKSHOP_ITEMS )
		return false; // too much

	uint32 unItemState = SteamUGC()->GetItemState( workshopItemID );

	if ( !(unItemState & k_EItemStateInstalled) )
		return false;

	uint32 unTimeStamp = 0;
	uint64 unSizeOnDisk = 0;
	char szItemFolder[1024] = { 0 };
	
	if ( !SteamUGC()->GetItemInstallInfo( workshopItemID, &unSizeOnDisk, szItemFolder, sizeof(szItemFolder), &unTimeStamp ) )
		return false;

	char szFile[1024];
	if( unItemState & k_EItemStateLegacyItem )
	{
		// szItemFolder just points directly to the item for legacy items that were published with the RemoteStorage API.
		_snprintf( szFile, sizeof( szFile ), "%s", szItemFolder );
	}
	else
	{
		_snprintf( szFile, sizeof( szFile ), "%s/workshopitem.txt", szItemFolder );
	}

	CWorkshopItem *pItem = LoadWorkshopItemFromFile( szFile );

	if ( !pItem )
		return false;
	
	pItem->m_ItemDetails.m_nPublishedFileId = workshopItemID;
	m_rgpWorkshopItems[m_nNumWorkshopItems++] = pItem;

	// get Workshop item details
	SteamAPICall_t hSteamAPICall = SteamUGC()->RequestUGCDetails( workshopItemID, 60 );
	pItem->m_SteamCallResultUGCDetails.Set(hSteamAPICall, pItem, &CWorkshopItem::OnUGCDetailsResult);
	
	return true;
}


//-----------------------------------------------------------------------------
// Purpose: load all subscribed workshop items 
//-----------------------------------------------------------------------------
void CSpaceWarClient::LoadWorkshopItems()
{
	// reset workshop Items
	for (uint32 i = 0; i < MAX_WORKSHOP_ITEMS; ++i)
	{
		if ( m_rgpWorkshopItems[i] )
		{
			delete m_rgpWorkshopItems[i];
			m_rgpWorkshopItems[i] = NULL;
		}
	}

	m_nNumWorkshopItems = 0; // load default test item

	PublishedFileId_t vecSubscribedItems[MAX_WORKSHOP_ITEMS];

	int numSubscribedItems = SteamUGC()->GetSubscribedItems( vecSubscribedItems, MAX_WORKSHOP_ITEMS );
	
	if ( numSubscribedItems > MAX_WORKSHOP_ITEMS )
		numSubscribedItems = MAX_WORKSHOP_ITEMS; // crop
	
	// load all subscribed workshop items
	for ( int iSubscribedItem=0; iSubscribedItem<numSubscribedItems; iSubscribedItem++ )
	{
		PublishedFileId_t workshopItemID = vecSubscribedItems[iSubscribedItem];
		LoadWorkshopItem( workshopItemID );
	}

	// load local test item 
	if ( m_nNumWorkshopItems < MAX_WORKSHOP_ITEMS )
	{
		CWorkshopItem *pItem = LoadWorkshopItemFromFile("workshop/workshopitem.txt");

		if ( pItem )
		{
			strncpy( pItem->m_ItemDetails.m_rgchTitle, "Test Item", k_cchPublishedDocumentTitleMax );
			strncpy( pItem->m_ItemDetails.m_rgchDescription, "This is a local test item for debugging", k_cchPublishedDocumentDescriptionMax );
			m_rgpWorkshopItems[m_nNumWorkshopItems++] = pItem;
		}
	}
}


//-----------------------------------------------------------------------------
// Purpose: load all all purchaseable items
//-----------------------------------------------------------------------------
void CSpaceWarClient::LoadItemsWithPrices()
{
	m_vecPurchaseableItems.clear();

	SteamAPICall_t hSteamAPICall = SteamInventory()->RequestPrices();
	m_SteamCallResultRequestPrices.Set( hSteamAPICall, this, &CSpaceWarClient::OnRequestPricesResult );
}

//-----------------------------------------------------------------------------
// Purpose: new Workshop was installed, load it instantly
//-----------------------------------------------------------------------------
void CSpaceWarClient::OnWorkshopItemInstalled( ItemInstalled_t *pParam )
{
	if ( pParam->m_unAppID == SteamUtils()->GetAppID() )
		LoadWorkshopItem( pParam->m_nPublishedFileId );
}

//-----------------------------------------------------------------------------
// Purpose: Request prices from the Steam Inventory Service
//-----------------------------------------------------------------------------
void CSpaceWarClient::OnRequestPricesResult( SteamInventoryRequestPricesResult_t *pParam, bool bIOFailure )
{
	if ( pParam->m_result == k_EResultOK )
	{
		strncpy( m_rgchCurrency, pParam->m_rgchCurrency, sizeof( m_rgchCurrency ) );

		uint32 unItems = SteamInventory()->GetNumItemsWithPrices();
		std::vector<SteamItemDef_t> vecItemDefs;
		vecItemDefs.resize( unItems );
		std::vector<uint64> vecPrices;
		vecPrices.resize( unItems );

		if ( SteamInventory()->GetItemsWithPrices( vecItemDefs.data(), vecPrices.data(), NULL, unItems ) )
		{
			m_vecPurchaseableItems.reserve( unItems );
			for ( uint32 i = 0; i < unItems; ++i )
			{
				PurchaseableItem_t t;
				t.m_nItemDefID = vecItemDefs[i];
				t.m_ulPrice = vecPrices[i];
				m_vecPurchaseableItems.push_back( t );
			}
		}
	}
}


//-----------------------------------------------------------------------------
// Purpose: Draws PublishFileID, title & description for each subscribed Workshop item
//-----------------------------------------------------------------------------
void CSpaceWarClient::DrawWorkshopItems()
{
	const int32 width = m_pGameEngine->GetViewportWidth();

	RECT rect;
	rect.top = 0;
	rect.bottom = 64;
	rect.left = 0;
	rect.right = width;

	char rgchBuffer[1024];
	sprintf_safe(rgchBuffer, "Subscribed Workshop Items");
	m_pGameEngine->BDrawString( m_hInstructionsFont, rect, D3DCOLOR_ARGB(255, 25, 200, 25), TEXTPOS_CENTER |TEXTPOS_VCENTER, rgchBuffer);

	rect.left = 32;
	rect.top = 64;
	rect.bottom = 96;
	
	for (int iSubscribedItem = 0; iSubscribedItem < MAX_WORKSHOP_ITEMS; iSubscribedItem++)
	{
		CWorkshopItem *pItem = m_rgpWorkshopItems[ iSubscribedItem ];

		if ( !pItem )
			continue;

		rect.top += 32;
		rect.bottom += 32;

		sprintf_safe( rgchBuffer, "%u. \"%s\" (%llu) : %s", iSubscribedItem+1,
			pItem->m_ItemDetails.m_rgchTitle, pItem->m_ItemDetails.m_nPublishedFileId, pItem->m_ItemDetails.m_rgchDescription );

		m_pGameEngine->BDrawString( m_hInstructionsFont, rect, D3DCOLOR_ARGB(255, 25, 200, 25), TEXTPOS_LEFT |TEXTPOS_VCENTER, rgchBuffer);
	}
	
	rect.left = 0;
	rect.right = width;
	rect.top = LONG(m_pGameEngine->GetViewportHeight() * 0.8);
	rect.bottom = m_pGameEngine->GetViewportHeight();

	sprintf_safe(rgchBuffer, "Press ESC to return to the Main Menu");
	m_pGameEngine->BDrawString(m_hInstructionsFont, rect, D3DCOLOR_ARGB(255, 25, 200, 25), TEXTPOS_CENTER | TEXTPOS_TOP, rgchBuffer);
}


void CSpaceWarClient::DrawInGameStore()
{
	const int32 width = m_pGameEngine->GetViewportWidth();

	RECT rect;
	rect.top = 0;
	rect.bottom = 64;
	rect.left = 0;
	rect.right = width;

	char rgchBuffer[1024];
	sprintf_safe(rgchBuffer, "In-Game Store");
	m_pGameEngine->BDrawString( m_hInstructionsFont, rect, D3DCOLOR_ARGB(255, 25, 200, 25), TEXTPOS_CENTER |TEXTPOS_VCENTER, rgchBuffer);

	rect.left = 32;
	rect.top = 64;
	rect.bottom = 96;

	for ( uint32 i = 0; i < m_vecPurchaseableItems.size(); ++i )
	{
		const auto &t = m_vecPurchaseableItems[i];
		
		char buf[512];
		uint32 bufSize = sizeof(buf);
		if ( !SteamInventory()->GetItemDefinitionProperty( t.m_nItemDefID, "name", buf, &bufSize ) && bufSize <= sizeof(buf) )
		{
			continue;
		}
		uint32 unQuantity = SpaceWarLocalInventory()->GetNumOf( t.m_nItemDefID );

		rect.top += 32;
		rect.bottom += 32;

		sprintf_safe( rgchBuffer, "%u. Purchase %-25s    %s %0.2f    (own %u)", i+1, buf, m_rgchCurrency, float(t.m_ulPrice)/100, unQuantity );

		m_pGameEngine->BDrawString( m_hInGameStoreFont, rect, D3DCOLOR_ARGB(255, 25, 200, 25), TEXTPOS_LEFT |TEXTPOS_VCENTER, rgchBuffer);

		// if the user presses a key, let them purchase the item
		uint32 key = 0x30 + i + 1;
		if ( m_pGameEngine->BIsKeyDown( key ) )
		{
			uint32 rgQuantity[1] = {1};
			SteamInventory()->StartPurchase( &t.m_nItemDefID, rgQuantity, 1 );
		}
	}
	
	rect.left = 0;
	rect.right = width;
	rect.top = LONG(m_pGameEngine->GetViewportHeight() * 0.8);
	rect.bottom = m_pGameEngine->GetViewportHeight();

	sprintf_safe(rgchBuffer, "Press ESC to return to the Main Menu");
	m_pGameEngine->BDrawString(m_hInstructionsFont, rect, D3DCOLOR_ARGB(255, 25, 200, 25), TEXTPOS_CENTER | TEXTPOS_TOP, rgchBuffer);
}
