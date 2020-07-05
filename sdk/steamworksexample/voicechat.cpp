//========= Copyright © 1996-2010, Valve LLC, All rights reserved. ============
//
// Purpose:Class for P2P voice chat
//
// $NoKeywords: $
//=============================================================================

#include "stdafx.h"
#include "voicechat.h"


CVoiceChat::CVoiceChat( IGameEngine *pGameEngine ) : 
	m_CallbackP2PSessionRequest( this, &CVoiceChat::OnP2PSessionRequest ),
	m_CallbackP2PSessionConnectFail( this, &CVoiceChat::OnP2PSessionConnectFail )
{
	m_pGameEngine = pGameEngine;
	m_bIsActive = false;
	m_ulLastTimeTalked = 0;
	m_hVoiceLoopback = 0;
}


CVoiceChat::~CVoiceChat()
{
	m_pGameEngine = NULL;
}

//-----------------------------------------------------------------------------
// Purpose: another user has sent us a packet - do we accept?
//-----------------------------------------------------------------------------
void CVoiceChat::OnP2PSessionRequest( P2PSessionRequest_t *pP2PSessionRequest )
{
	std::map< uint64, VoiceChatConnection_t >::iterator iter;
	iter = m_MapConnections.find( pP2PSessionRequest->m_steamIDRemote.ConvertToUint64() );
	if ( iter != m_MapConnections.end() )
	{
		// we play with this user
		iter->second.eLastError = k_EP2PSessionErrorNone; // our connection is now working
	
		// the packet itself will come through when you call SteamNetworking()->ReadP2PPacket()
		SteamNetworking()->AcceptP2PSessionWithUser( pP2PSessionRequest->m_steamIDRemote );
	}
}


//-----------------------------------------------------------------------------
// Purpose: we send a packet to another user but it failed
//-----------------------------------------------------------------------------
void CVoiceChat::OnP2PSessionConnectFail( P2PSessionConnectFail_t *pP2PSessionConnectFail )
{
	std::map< uint64, VoiceChatConnection_t >::iterator iter;
	iter = m_MapConnections.find( pP2PSessionConnectFail->m_steamIDRemote.ConvertToUint64() );
	if ( iter != m_MapConnections.end() )
	{
		// connection to that user failed
		iter->second.eLastError = (EP2PSessionError) pP2PSessionConnectFail->m_eP2PSessionError; 
	}
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CVoiceChat::RunFrame()
{
	if ( m_bIsActive )
	{
		// read local microphone input
		uint32 nBytesAvailable = 0;
		EVoiceResult res = SteamUser()->GetAvailableVoice( &nBytesAvailable, NULL, 0 );

		if ( res == k_EVoiceResultOK && nBytesAvailable > 0 )
		{	
			uint32 nBytesWritten = 0;
			MsgVoiceChatData_t msg;

			// don't send more then 1 KB at a time
			uint8 buffer[ 1024+sizeof(msg) ];
			
			res = SteamUser()->GetVoice( true, buffer+sizeof(msg), 1024, &nBytesWritten, false, NULL, 0, NULL, 0 );
			
			if ( res == k_EVoiceResultOK && nBytesWritten > 0 )
			{
				// assemble message
				msg.SetDataLength( nBytesWritten );
				memcpy( buffer, &msg, sizeof(msg) );
								
				SendMessageToAll( buffer, sizeof(msg)+nBytesWritten );

				m_ulLastTimeTalked = m_pGameEngine->GetGameTickCount();

				// if local voice loopback is enabled, play it back now
				if ( m_hVoiceLoopback !=  0 )
				{
					// Uncompress the voice data, buffer holds up to 1 second of data
					uint32 numUncompressedBytes = 0; 
					const uint8* pVoiceData = (const uint8*) buffer;
					pVoiceData += sizeof(MsgVoiceChatData_t);

					res = SteamUser()->DecompressVoice( pVoiceData , nBytesWritten,
						m_ubUncompressedVoice, sizeof( m_ubUncompressedVoice ), &numUncompressedBytes, VOICE_OUTPUT_SAMPLE_RATE );

					if ( res == k_EVoiceResultOK && numUncompressedBytes > 0 )
					{
						m_pGameEngine->AddVoiceData( m_hVoiceLoopback, m_ubUncompressedVoice, numUncompressedBytes );
					}
				}
			}
		}		

		CheckConnections();
	}
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CVoiceChat::HandleMessage( CSteamID fromSteamID, EMessage eMsg, const void *pMessage )
{
	if ( eMsg != k_EMsgVoiceChatPing && eMsg != k_EMsgVoiceChatData )
		return false; // we don't handle these messages

	std::map< uint64, VoiceChatConnection_t >::iterator iter;
	iter = m_MapConnections.find( fromSteamID.ConvertToUint64() );
	if ( iter != m_MapConnections.end() )
	{
		VoiceChatConnection_t &chatClient = iter->second;
		chatClient.eLastError = k_EP2PSessionErrorNone; // our connection is working
				
		if ( eMsg == k_EMsgVoiceChatData )
		{
			const MsgVoiceChatData_t *pMsgVoiceData = (const MsgVoiceChatData_t *) pMessage; 
			chatClient.ulLastReceiveVoiceTime = m_pGameEngine->GetGameTickCount();

			// Uncompress the voice data, buffer holds up to 1 second of data
			uint8 pbUncompressedVoice[ VOICE_OUTPUT_SAMPLE_RATE * BYTES_PER_SAMPLE ]; 
			uint32 numUncompressedBytes = 0; 
			const uint8* pVoiceData = (const uint8*) pMessage;
			pVoiceData += sizeof(MsgVoiceChatData_t);
			
			EVoiceResult res = SteamUser()->DecompressVoice( pVoiceData , pMsgVoiceData->GetDataLength(),
				pbUncompressedVoice, sizeof( pbUncompressedVoice ), &numUncompressedBytes, VOICE_OUTPUT_SAMPLE_RATE );

			if ( res == k_EVoiceResultOK && numUncompressedBytes > 0 )
			{
				// play it again Sam
				if ( chatClient.hVoiceChannel == 0 )
				{
					chatClient.hVoiceChannel = m_pGameEngine->HCreateVoiceChannel();
				}

				m_pGameEngine->AddVoiceData( chatClient.hVoiceChannel, pbUncompressedVoice, numUncompressedBytes );
			}
		}
	}

	return true;
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CVoiceChat::MarkAllPlayersInactive()
{
	std::map< uint64, VoiceChatConnection_t >::iterator iter;
	for( iter = m_MapConnections.begin(); iter != m_MapConnections.end(); ++iter )
	{
		iter->second.bActive = false;
	}
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CVoiceChat::MarkPlayerAsActive( CSteamID steamID )
{
	if ( !m_bIsActive )
		return;

	if ( m_SteamIDLocalUser == steamID )
		return;

	std::map< uint64, VoiceChatConnection_t >::iterator iter;
	iter = m_MapConnections.find( steamID.ConvertToUint64() );
	if ( iter != m_MapConnections.end() )
	{
		// player already has a session object, no new object created
		iter->second.bActive = true;
		return;
	}

	/*char szText[100];
	sprintf_safe(szText, "CVoiceChat::AddPlayerToSession: %s.\n", SteamFriends()->GetFriendPersonaName( steamID ) );
	OutputDebugString( szText ); */

	VoiceChatConnection_t session;
	session.eLastError = k_EP2PSessionErrorTimeout;
	session.ulLastReceiveVoiceTime = 0;
	session.ulLastSentTime = 0;
	session.hVoiceChannel = 0;
	session.bActive = true;

	m_MapConnections[ steamID.ConvertToUint64() ] = session;

	return;
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CVoiceChat::IsPlayerTalking( CSteamID steamID )
{

	if ( steamID == m_SteamIDLocalUser )
	{
		// thats ourself
		if ( m_ulLastTimeTalked + 250 >  m_pGameEngine->GetGameTickCount() )
			return true;
	}
	else
	{
		std::map< uint64, VoiceChatConnection_t >::iterator iter;
		iter = m_MapConnections.find( steamID.ConvertToUint64() );
		if ( iter != m_MapConnections.end() )
		{
			if ( iter->second.eLastError == k_EP2PSessionErrorNone )
			{
				if ( (iter->second.ulLastReceiveVoiceTime + 250) >  m_pGameEngine->GetGameTickCount() )
				{
					// user talked less then 250msec ago, assume still active
					return true;
				}
			}
		}
	}

	return false;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CVoiceChat::StartVoiceChat()
{
	if ( !m_bIsActive )
	{
		m_SteamIDLocalUser = SteamUser()->GetSteamID();

		SteamUser()->StartVoiceRecording();

		m_bIsActive = true;

		// here you can enable optional local voice loopback:		
		// m_hVoiceLoopback = m_pGameEngine->HCreateVoiceChannel();
	}

	return true;
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CVoiceChat::StopVoiceChat()
{
	if ( m_bIsActive )
	{
		std::map< uint64, VoiceChatConnection_t >::iterator iter;
		for( iter = m_MapConnections.begin(); iter != m_MapConnections.end(); ++iter )
		{
			CSteamID steamID( iter->first );
			m_pGameEngine->DestroyVoiceChannel( iter->second.hVoiceChannel );
			SteamNetworking()->CloseP2PSessionWithUser( steamID );
		}

		m_MapConnections.clear();

		if ( m_hVoiceLoopback )
		{
			m_pGameEngine->DestroyVoiceChannel( m_hVoiceLoopback );
			m_hVoiceLoopback = 0;
		}

		SteamUser()->StopVoiceRecording();

		m_bIsActive = false;
	}
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CVoiceChat::CheckConnections()
{
	const uint64 ulTimeNow = m_pGameEngine->GetGameTickCount();
	const uint64 ulTimeoutLimit = 10 * 1000; // 10 seconds
	// 
	std::map< uint64, VoiceChatConnection_t >::iterator iter;
	for( iter = m_MapConnections.begin(); iter != m_MapConnections.end(); ++iter )
	{
		if ( iter->second.bActive )
		{
			const EP2PSessionError eLastError = iter->second.eLastError;
			CSteamID steamID( iter->first );

			if ( eLastError == k_EP2PSessionErrorNone || eLastError == k_EP2PSessionErrorTimeout )
			{
				if ( iter->second.ulLastSentTime + ulTimeoutLimit < ulTimeNow )
				{
					// time for a ping
					MsgVoiceChatPing_t msg;
					SteamNetworking()->SendP2PPacket( steamID, &msg, sizeof(msg), k_EP2PSendUnreliable );
					iter->second.ulLastSentTime = ulTimeNow;
				}
			}
		}
		else
		{
			// found inactive connection, destroy
			CSteamID steamID( iter->first );
			SteamNetworking()->CloseP2PSessionWithUser( steamID );
			m_pGameEngine->DestroyVoiceChannel( iter->second.hVoiceChannel );

			m_MapConnections.erase( iter );
			break; // break since the iterator became bad
		}
	}
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CVoiceChat::SendMessageToAll( const void *pubData, uint32 cubData )
{
	const uint64 ulTimeNow = m_pGameEngine->GetGameTickCount();
	// 
	std::map< uint64, VoiceChatConnection_t >::iterator iter;
	for( iter = m_MapConnections.begin(); iter != m_MapConnections.end(); ++iter )
	{
		const EP2PSessionError eLastError = iter->second.eLastError;
		CSteamID steamID( iter->first );

		if ( eLastError == k_EP2PSessionErrorNone )
		{
			SteamNetworking()->SendP2PPacket( steamID, pubData, cubData, k_EP2PSendUnreliableNoDelay );
			iter->second.ulLastSentTime = ulTimeNow;
		}
	}
}
