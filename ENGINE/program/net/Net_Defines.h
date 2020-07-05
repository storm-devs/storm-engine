#ifndef PROGRAM_NET_HPP
#define PROGRAM_NET_HPP

#define NET_DEFAULT_SERVER_PORT			29020

#define NETSHIP_MAST_TOUCH_BALL			0
#define NETSHIP_MAST_TOUCH_ISLAND		1
#define NETSHIP_MAST_TOUCH_SHIP			2

#define NETFILE_FACE					0
#define NETFILE_FLAG					1
#define NETFILE_SAIL					2

#define NETKILL_BY_BALL					0
#define	NETKILL_BY_ISLAND				1
#define NETKILL_BY_TOUCH_DEAD			2
#define NETKILL_BY_TOUCH_ALIVE			3
#define NETKILL_BY_LIGHTNING			4

#define NETSKILL_MAX					10.0

#define MSG_NET_CREATE_SERVER			10000
#define MSG_NET_CREATE_CLIENT			10001
#define MSG_NET_ADD_FORT				10002

// =================================================================================
// Network game types
// =================================================================================
#define NETGAME_DEATHMATCH				0
#define NETGAME_NAVAL					1
#define NETGAME_CONVOY					2
#define NETGAME_FORT					3

// =================================================================================
// Destination address
// =================================================================================
#define DST_INVALID						65534
#define DST_ALL							65535

// =================================================================================
// NET MESSAGES CODES 
// =================================================================================
#define NC_CONNECT						0
#define NC_DISCONNECT					1
#define NC_STARTGAME					2
#define NC_SAY							3
#define NC_BALL							4
#define NC_COMMAND						5
#define NC_SHIP							6
#define NC_FORT							7
#define NC_PING							8
#define NC_WEATHER						9
#define NC_CLIENT						10
#define NC_CAMERA						11
//#define NC_BROADCAST					12		== changed, because some changes implemented to broadcast message
#define NC_MASTERSERVER					13
#define NC_FILE							14
#define NC_BUY							15
#define NC_SERVERINFO					16
#define NC_PLAYERINFO					17
#define NC_STATISTICS					18
#define NC_GARANTED						19
#define NC_GAMEINFO						20
#define NC_BROADCAST					21

// =================================================================================
// SUBCODES
// =================================================================================
// NC_FILE
#define NSC_FILE_DATA					0
#define NSC_FILE_DATA_ACK				1
#define NSC_FILE_GET_FILE				2
#define NSC_FILE_TRANSFER_READY			3
#define NSC_FILE_RECEIVER_READY			4
#define NSC_FILE_CANCEL					5

// NC_CONNECT
#define NSC_CONNECT_REQUEST							0
#define NSC_CONNECT_ACCEPTED						1
#define NSC_CONNECT_REJECTED						2
#define NSC_CONNECT_REJECTED_NEED_CLIENT_PASSWORD	3
#define NSC_CONNECT_REJECTED_SERVER_FULL			4
#define NSC_CONNECT_NEED_PASSWORD					5
#define NSC_CONNECT_BANNED							6
#define NSC_CONNECT_RECONNECT						7
#define NSC_CONNECT_DISCONNECT						8
#define NSC_CONNECT_REJECTED_DIFFERENT_VERSION		9

// NC_CLIENT
#define NSC_CLIENT_CONNECTED			0
#define NSC_CLIENT_LOAD					1
#define NSC_CLIENT_DROP					2

// NC_WEATHER
#define NSC_WEATHER_ADD_LIGHTNING		0

// NC_COMMAND	
#define NSC_COMMAND_STARTGAME			0
#define NSC_COMMAND_RESTARTGAME			1
#define NSC_COMMAND_VOTE_KICK			2

// NC_STARTGAME
#define NSC_STARTGAME_GETBUYINFO		0
#define NSC_STARTGAME_ENVIRONMENT		1
#define NSC_STARTGAME_GAMEOVER			2

// NC_PING
#define NSC_PING						0
#define NSC_PONG						1

// NC_SAY
#define NSC_SAY_CHAT					0
#define NSC_SAY_TEAM					1
#define NSC_SAY_PERSON					2

// NC_BALL
#define NSC_BALL_ADD					0
#define NSC_BALL_DEL					1
#define NSC_BALL_HIT_WATER 				2
#define NSC_BALL_HIT_SAIL 				3
#define NSC_BALL_HIT_HULL 				4
#define NSC_BALL_HIT_MAST 				5
#define NSC_BALL_HIT_FORT 				6
#define NSC_BALL_HIT_ISLAND				7

// NC_SHIP
#define NSC_SHIP_MAST_FALL				0
#define NSC_SHIP_DEAD					1
#define NSC_SHIP_TURNLEFT_ON			2
#define NSC_SHIP_TURNLEFT_OFF			3
#define NSC_SHIP_TURNRIGHT_ON			4
#define NSC_SHIP_TURNRIGHT_OFF			5
#define NSC_SHIP_SAILUP					6
#define NSC_SHIP_SAILDOWN				7
#define NSC_SHIP_SETSAILSTATE			8
#define NSC_SHIP_FIRE					9
#define NSC_SHIP_INFO					10
#define NSC_SHIP_SERIOUS_BOOM			11
#define NSC_SHIP_ACTIVATE_FIREPLACE		12
#define NSC_SHIP_CHANGE_CHARGE			13
#define NSC_SHIP_BORTS_INFO				14
#define NSC_SHIP_SAIL_STATE				15
#define NSC_SHIP_START_REPAIR			16
#define NSC_SHIP_SHIP_COLLISION			17
#define NSC_SHIP_ISLAND_COLLISION		18

// NC_FORT
#define NSC_FORT_CANNON_DESTROY			0

// NC_CAMERA
#define NSC_DECK_CAMERA_UPDATE			0

// NC_BROADCAST
#define NSC_BROADCAST_LOCAL				0
#define NSC_BROADCAST_INET				1
#define NSC_BROADCAST_FAVORITE			2

// NC_MASTERSERVER
#define NSC_MASTERSERVER_CONNECT		0
#define NSC_MASTERSERVER_GETSERVERS		1
#define NSC_MASTERSERVER_UPDATESERVER	2
#define NSC_MASTERSERVER_ADDSERVER		3
#define NSC_MASTERSERVER_DELSERVER		4
#define NSC_MASTERSERVER_DISCONNECT		5

// NC_BUY
#define NSC_BUY_OK						0
#define NSC_BUY_FAILED					1
#define NSC_BUY_READY					2
#define	NSC_BUY_TEAMCHANGE				3
#define	NSC_BUY_SHIPCHANGE				4
#define NSC_BUY_FULLINFO				5

// NC_SERVERINFO
#define NSC_SERVERINFO_GET				0
#define	NSC_SERVERINFO_COMMON			1
#define NSC_SERVERINFO_PLAYER			2
#define NSC_SERVERINFO_SHIP				3
#define NSC_SERVERINFO_CANNONS			4
#define NSC_SERVERINFO_SKILLS			5
#define NSC_SERVERINFO_PERKS			6
#define NSC_SERVERINFO_GOODS			7
#define NSC_SERVERINFO_TOPLIST_PLAYER	8

// NC_PLAYERINFO
#define NSC_PLAYERINFO_SHIPCHANGE		0
#define NSC_PLAYERINFO_TEAMCHANGE		1
#define NSC_PLAYERINFO_BUYREADY			2
#define NSC_PLAYERINFO_STARTREADY		3
#define NSC_PLAYERINFO_STARTCOUNTDOWN	4
#define NSC_PLAYERINFO_STOPCOUNTDOWN	5

// NC_STATISTICS
#define NSC_STATISTICS_TOTAL_PLAYER		0
#endif