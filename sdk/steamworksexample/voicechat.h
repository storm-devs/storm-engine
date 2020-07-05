//========= Copyright © 1996-2010, Valve LLC, All rights reserved. ============
//
// Purpose: Class for P2P voice chat
//
// $NoKeywords: $
//=============================================================================

#ifndef VOICE_CHAT_H
#define VOICE_CHAT_H

#include "GameEngine.h"
#include "SpaceWar.h"
#include "Messages.h"

typedef struct VoiceChatConnection_s
{
	EP2PSessionError eLastError; // ErrorNone means we have a session
	uint64 ulLastReceiveVoiceTime;
	uint64 ulLastSentTime;
	HGAMEVOICECHANNEL hVoiceChannel;	// engine voice channel for this player
	bool   bActive;	
}  VoiceChatConnection_t;

class CVoiceChat
{
public:

	CVoiceChat( IGameEngine *pGameEngine );
	~CVoiceChat();

	bool StartVoiceChat();
	void StopVoiceChat();

	// chat control
	void MarkAllPlayersInactive();
	void MarkPlayerAsActive( CSteamID steamID );

	bool IsPlayerTalking( CSteamID steamID );

	// chat engine
	void RunFrame();
	bool HandleMessage( CSteamID fromSteamID, EMessage eMsg, const void *pMessage );
	void SendMessageToAll( const void *pubData, uint32 cubData );
	
private:

	STEAM_CALLBACK( CVoiceChat, OnP2PSessionRequest, P2PSessionRequest_t, m_CallbackP2PSessionRequest );
	STEAM_CALLBACK( CVoiceChat, OnP2PSessionConnectFail, P2PSessionConnectFail_t, m_CallbackP2PSessionConnectFail );

	void CheckConnections();

	// Pointer to engine instance (so we can play sound)
	IGameEngine *m_pGameEngine;
	
	// map of voice chat sessions with other players
	std::map< uint64, VoiceChatConnection_t > m_MapConnections;

	CSteamID m_SteamIDLocalUser; // ourself
	bool m_bIsActive;	// is voice chat system active
	uint64 m_ulLastTimeTalked; // last time we've talked ourself
	HGAMEVOICECHANNEL m_hVoiceLoopback;
	uint8 m_ubUncompressedVoice[ VOICE_OUTPUT_SAMPLE_RATE * BYTES_PER_SAMPLE ]; // too big for the stack
};

#endif
