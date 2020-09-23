//====== Copyright Valve Corporation, All rights reserved. ====================

#ifndef ISTEAMNETWORKINGMESSAGES
#define ISTEAMNETWORKINGMESSAGES
#pragma once

#include "steamnetworkingtypes.h"

//-----------------------------------------------------------------------------
/// The non-connection-oriented interface to send and receive messages
/// (whether they be "clients" or "servers").
///
/// ISteamNetworkingSockets is connection-oriented (like TCP), meaning you
/// need to listen and connect, and then you send messages using a connection
/// handle.  ISteamNetworkingMessages is more like UDP, in that you can just send
/// messages to arbitrary peers at any time.  The underlying connections are
/// established implicitly.
///
/// Under the hood ISteamNetworkingMessages works on top of the ISteamNetworkingSockets
/// code, so you get the same routing and messaging efficiency.  The difference is
/// mainly in your responsibility to explicitly establish a connection and
/// the type of feedback you get about the state of the connection.  Both
/// interfaces can do "P2P" communications, and both support both unreliable
/// and reliable messages, fragmentation and reassembly.
///
/// The primary purpose of this interface is to be "like UDP", so that UDP-based code
/// can be ported easily to take advantage of relayed connections.  If you find
/// yourself needing more low level information or control, or to be able to better
/// handle failure, then you probably need to use ISteamNetworkingSockets directly.
/// Also, note that if your main goal is to obtain a connection between two peers
/// without concerning yourself with assigning roles of "client" and "server",
/// you may find the symmetric connection mode of ISteamNetworkingSockets useful.
/// (See k_ESteamNetworkingConfig_SymmetricConnect.)
///
class ISteamNetworkingMessages
{
  public:
    /// Sends a message to the specified host.  If we don't already have a session with that user,
    /// a session is implicitly created.  There might be some handshaking that needs to happen
    /// before we can actually begin sending message data.  If this handshaking fails and we can't
    /// get through, an error will be posted via the callback SteamNetworkingMessagesSessionFailed_t.
    /// There is no notification when the operation succeeds.  (You should have the peer send a reply
    /// for this purpose.)
    ///
    /// Sending a message to a host will also implicitly accept any incoming connection from that host.
    ///
    /// nSendFlags is a bitmask of k_nSteamNetworkingSend_xxx options
    ///
    /// nRemoteChannel is a routing number you can use to help route message to different systems.
    /// You'll have to call ReceiveMessagesOnChannel() with the same channel number in order to retrieve
    /// the data on the other end.
    ///
    /// Using different channels to talk to the same user will still use the same underlying
    /// connection, saving on resources.  If you don't need this feature, use 0.
    /// Otherwise, small integers are the most efficient.
    ///
    /// It is guaranteed that reliable messages to the same host on the same channel
    /// will be be received by the remote host (if they are received at all) exactly once,
    /// and in the same order that they were send.
    ///
    /// NO other order guarantees exist!  In particular, unreliable messages may be dropped,
    /// received out of order with respect to each other and with respect to reliable data,
    /// or may be received multiple times.  Messages on different channels are *not* guaranteed
    /// to be received in the order they were sent.
    ///
    /// A note for those familiar with TCP/IP ports, or converting an existing codebase that
    /// opened multiple sockets:  You might notice that there is only one channel, and with
    /// TCP/IP each endpoint has a port number.  You can think of the channel number as the
    /// *destination* port.  If you need each message to also include a "source port" (so the
    /// recipient can route the reply), then just put that in your message.  That is essentially
    /// how UDP works!
    ///
    /// Returns:
    /// - k_EREsultOK on success.
    /// - k_EResultNoConnection will be returned if the session has failed or was closed by the peer,
    ///   and k_nSteamNetworkingSend_AutoRestartBrokwnSession is not used.  (You can use
    ///   GetSessionConnectionInfo to get the details.)  In order to acknowledge the broken session
    ///   and start a new one, you must call CloseSessionWithUser
    /// - See SendMessageToConnection::SendMessageToConnection for more
    virtual EResult SendMessageToUser(const SteamNetworkingIdentity &identityRemote, const void *pubData,
                                      uint32 cubData, int nSendFlags, int nRemoteChannel) = 0;

    /// Reads the next message that has been sent from another user via SendMessageToUser() on the given channel.
    /// Returns number of messages returned into your list.  (0 if no message are available on that channel.)
    ///
    /// When you're done with the message object(s), make sure and call Release()!
    virtual int ReceiveMessagesOnChannel(int nLocalChannel, SteamNetworkingMessage_t **ppOutMessages,
                                         int nMaxMessages) = 0;

    /// AcceptSessionWithUser() should only be called in response to a SteamP2PSessionRequest_t callback
    /// SteamP2PSessionRequest_t will be posted if another user tries to send you a message, and you haven't
    /// tried to talk to them.  If you don't want to talk to them, just ignore the request.
    /// If the user continues to send you messages, SteamP2PSessionRequest_t callbacks will continue to
    /// be posted periodically.  This may be called multiple times for a single user.
    ///
    /// Calling SendMessage() on the other user, this implicitly accepts any pending session request.
    virtual bool AcceptSessionWithUser(const SteamNetworkingIdentity &identityRemote) = 0;

    /// Call this when you're done talking to a user to immediately free up resources under-the-hood.
    /// If the remote user tries to send data to you again, another P2PSessionRequest_t callback will
    /// be posted.
    ///
    /// Note that sessions that go unused for a few minutes are automatically timed out.
    virtual bool CloseSessionWithUser(const SteamNetworkingIdentity &identityRemote) = 0;

    /// Call this  when you're done talking to a user on a specific channel.  Once all
    /// open channels to a user have been closed, the open session to the user will be
    /// closed, and any new data from this user will trigger a SteamP2PSessionRequest_t
    /// callback
    virtual bool CloseChannelWithUser(const SteamNetworkingIdentity &identityRemote, int nLocalChannel) = 0;

    /// Returns information about the latest state of a connection, if any, with the given peer.
    /// Primarily intended for debugging purposes, but can also be used to get more detailed
    /// failure information.  (See SendMessageToUser and k_nSteamNetworkingSend_AutoRestartBrokwnSession.)
    ///
    /// Returns the value of SteamNetConnectionInfo_t::m_eState, or k_ESteamNetworkingConnectionState_None
    /// if no connection exists with specified peer.  You may pass nullptr for either parameter if
    /// you do not need the corresponding details.  Note that sessions time out after a while,
    /// so if a connection fails, or SendMessageToUser returns SendMessageToUser, you cannot wait
    /// indefinitely to obtain the reason for failure.
    virtual ESteamNetworkingConnectionState GetSessionConnectionInfo(
        const SteamNetworkingIdentity &identityRemote, SteamNetConnectionInfo_t *pConnectionInfo,
        SteamNetworkingQuickConnectionStatus *pQuickStatus) = 0;
};
#define STEAMNETWORKINGMESSAGES_VERSION "SteamNetworkingMessages002"

//
// Callbacks
//

#pragma pack(push, 1)

/// Posted when a remote host is sending us a message, and we do not already have a session with them
struct SteamNetworkingMessagesSessionRequest_t
{
    enum
    {
        k_iCallback = k_iSteamNetworkingMessagesCallbacks + 1
    };
    SteamNetworkingIdentity m_identityRemote; // user who wants to talk to us
};

/// Posted when we fail to establish a connection, or we detect that communications
/// have been disrupted it an unusual way.  There is no notification when a peer proactively
/// closes the session.  ("Closed by peer" is not a concept of UDP-style communications, and
/// SteamNetworkingMessages is primarily intended to make porting UDP code easy.)
///
/// Remember: callbacks are asynchronous.   See notes on SendMessageToUser,
/// and k_nSteamNetworkingSend_AutoRestartBrokwnSession in particular.
///
/// Also, if a session times out due to inactivity, no callbacks will be posted.  The only
/// way to detect that this is happening is that querying the session state may return
/// none, connecting, and findingroute again.
struct SteamNetworkingMessagesSessionFailed_t
{
    enum
    {
        k_iCallback = k_iSteamNetworkingMessagesCallbacks + 2
    };

    /// Detailed info about the connection.  This will include the
    SteamNetConnectionInfo_t m_info;
};

#pragma pack(pop)

//
// Global accessor
//

#if defined(STEAMNETWORKINGSOCKETS_PARTNER)

// Standalone lib.  Use different symbol name, so that we can dynamically switch between steamclient.dll
// and the standalone lib
STEAMNETWORKINGSOCKETS_INTERFACE ISteamNetworkingMessages *SteamNetworkingMessages_Lib();
STEAMNETWORKINGSOCKETS_INTERFACE ISteamNetworkingMessages *SteamGameServerNetworkingMessages_Lib();
inline ISteamNetworkingMessages *SteamNetworkingMessages()
{
    return SteamNetworkingMessages_Lib();
}
inline ISteamNetworkingMessages *SteamGameServerNetworkingMessages()
{
    return SteamGameServerNetworkingMessages_Lib();
}

#elif defined(STEAMNETWORKINGSOCKETS_OPENSOURCE)

// Opensource GameNetworkingSockets
STEAMNETWORKINGSOCKETS_INTERFACE ISteamNetworkingMessages *SteamNetworkingMessages();

#else

// Steamworks SDK
inline ISteamNetworkingMessages *SteamNetworkingMessages();
STEAM_DEFINE_USER_INTERFACE_ACCESSOR(ISteamNetworkingMessages *, SteamNetworkingMessages,
                                     STEAMNETWORKINGMESSAGES_VERSION);
inline ISteamNetworkingMessages *SteamGameServerNetworkingMessages();
STEAM_DEFINE_GAMESERVER_INTERFACE_ACCESSOR(ISteamNetworkingMessages *, SteamGameServerNetworkingMessages,
                                           STEAMNETWORKINGMESSAGES_VERSION);
#endif

#endif // ISTEAMNETWORKINGMESSAGES
