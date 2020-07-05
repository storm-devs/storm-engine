//========= Copyright © 1996-2004, Valve LLC, All rights reserved. ============
//
// Purpose:
//
// $NoKeywords: $
//=============================================================================



const int k_cMaxSockets = 16;
class CP2PAuthPlayer;
class CNetworkTransport;


//-----------------------------------------------------------------------------
// Purpose: one player p2p auth process state machine
//-----------------------------------------------------------------------------
class CP2PAuthPlayer
{
public:
	CP2PAuthPlayer( IGameEngine *pGameEngine, CNetworkTransport *pNetworkTransport );
	~CP2PAuthPlayer();
	void EndGame();
	void InitPlayer( CSteamID steamID );
	void StartAuthPlayer();
	bool BIsAuthOk();
	bool HandleMessage( EMessage eMsg, void *pMessage );

	CSteamID GetSteamID();

	STEAM_CALLBACK( CP2PAuthPlayer, OnBeginAuthResponse, ValidateAuthTicketResponse_t, m_CallbackBeginAuthResponse );

	CSteamID m_steamID;
private:
	uint64 GetGameTimeInSeconds() 
	{ 
		return m_pGameEngine->GetGameTickCount()/1000;
	}
	bool m_bSentTicket;
	bool m_bSubmittedHisTicket;
	bool m_bHaveAnswer;
	uint64 m_ulConnectTime;
	uint64 m_ulTicketTime;
	uint64 m_ulAnswerTime;
	uint32 m_cubTicketIGaveThisUser;
	uint8 m_rgubTicketIGaveThisUser[1024];
	uint32 m_cubTicketHeGaveMe;
	uint8 m_rgubTicketHeGaveMe[1024];
	HAuthTicket m_hAuthTicketIGaveThisUser;
	EBeginAuthSessionResult m_eBeginAuthSessionResult;
	EAuthSessionResponse m_eAuthSessionResponse;

	CNetworkTransport *m_pNetworkTransport;
	IGameEngine *m_pGameEngine;
};

//-----------------------------------------------------------------------------
// Purpose: simple wrapper for multiple players
//-----------------------------------------------------------------------------
class CP2PAuthedGame
{
public:
	CP2PAuthedGame( IGameEngine *pGameEngine );
	void PlayerDisconnect( int iSlot );
	void EndGame();
	void StartAuthPlayer( int iSlot, CSteamID steamID );
	void RegisterPlayer( int iSlot, CSteamID steamID );
	bool HandleMessage( EMessage eMsg, void *pMessage );
	CSteamID GetSteamID();
	void InternalInitPlayer( int iSlot, CSteamID steamID, bool bStartAuthProcess );

	CP2PAuthPlayer *m_rgpP2PAuthPlayer[MAX_PLAYERS_PER_SERVER];
	MsgP2PSendingTicket_t *m_rgpQueuedMessage[MAX_PLAYERS_PER_SERVER];
	IGameEngine *m_pGameEngine;
	CNetworkTransport *m_pNetworkTransport;
};


//-----------------------------------------------------------------------------
// Purpose: trivial wrapper on network messaging 
//-----------------------------------------------------------------------------
class CNetworkTransport
{
public:
	CNetworkTransport();
	bool SendTicket( CSteamID steamIDFrom, CSteamID steamIDTo, uint32 cubTicket, uint8 *pubTicket );
	void CloseConnection( CSteamID steamID );

	bool SendNetMessage( CSteamID steamIDTo, void *pMessage, int cubMessage );

	STEAM_CALLBACK( CNetworkTransport, OnP2PSessionRequest, P2PSessionRequest_t, m_CallbackP2PSessionRequest );
	STEAM_CALLBACK( CNetworkTransport, OnP2PSessionConnectFail, P2PSessionConnectFail_t, m_CallbackP2PSessionConnectFail );
};
