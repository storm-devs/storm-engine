#include "Net\Server\OnCamera.c"
#include "Net\Server\OnBall.c"
#include "Net\Server\OnCommand.c"
#include "Net\Server\OnConnect.c"
#include "Net\Server\OnNetMessage.c"
#include "Net\Server\OnSay.c"
#include "Net\Server\OnShip.c"
#include "Net\Server\OnPing.c"
#include "Net\Server\OnStart.c"
#include "Net\Server\Utils.c"
#include "Net\Server\Rigging.c"
#include "Net\Server\Island.c"
#include "Net\Server\Touch.c"
#include "Net\Server\Time.c"
#include "Net\Server\File.c"
#include "Net\Server\Buy.c"
#include "Net\Server\Fort.c"
#include "Net\Server\ServerInfo.c"
#include "Net\Server\LocalServer.c"
#include "Net\Server\DedicatedServer.c"
#include "Net\Server\Weather\Weather.c"
#include "Net\Server\Games\Convoy.c"
#include "Net\Server\Games\DeathMatch.c"
#include "Net\Server\Games\Fort.c"
#include "Net\Server\Games\Naval.c"

#define NET_TOTALCLIENTS			255		// total massive
#define NET_MAXCLIENTS				16		// absolute maximum is 254
#define NET_FORTCLIENT				253

#define MAX_TOPPLAYERS				100

#define MASTERSERVER_UPDATETIME		60000

int		NSSortedPlayers[MAX_TOPPLAYERS];
object	NSClients[NET_TOTALCLIENTS];	// current game clients
object	NSBanList;						// server ban list

object NSPlayers;						// All players in server for last month

int iServerTime;
int iPingTime;
bool bNetServerIsStarted = false;

void Net_DeleteServer()
{
	DelEventHandler("NetServer_UpdateMasterServerInfo", "NetServer_UpdateMasterServerInfo");

	DeleteClass(&NetServer);

	if (!NetServer_IsStarted()) { return; }

	if (sti(NetServer.InetServer))
	{
		int iSMsg = NMCreate();
		NMAddByte(iSMsg, NC_MASTERSERVER);
		NMAddByte(iSMsg, NSC_MASTERSERVER_DELSERVER);
		NetServer_SendMasterServerMessage("VANOSUPERPC", 29029, iSMsg);
		NMDelete(iSMsg);
	}

	NetServer_EndGame(false);

	// save global server massives
	Net_SaveFile(true, &NSBanList, "BanList.nsv");
	Net_SaveFile(true, &NSPlayers, "Players.nsv");

	// clear all server massives
	DeleteAttribute(&NSBanList, "");
	DeleteAttribute(&NSClients, "");
	DeleteAttribute(&NSPlayers, "");

	DelEventHandler("ExitApplication", "NetServer_ExitApplication");
	DelEventHandler("NetServer_OnNetMessage", "NetServer_OnNetMessage");
	DelEventHandler("frame", "NetServer_OneSecondTick");

	iSecondsToStartGame = -1;
	bNetServerIsStarted = false;
	NetServer.StopServer = "";
}

void Net_ServerFirstInit()
{
	NetServer.ServerName = "Default Server Name";
	NetServer.ServerPort = NET_DEFAULT_SERVER_PORT;
	NetServer.MaxClients = NET_MAXCLIENTS;
	NetServer.ServerPassword = "";
	NetServer.AdminPassword = "yupi";
	NetServer.LanServer = true;
	NetServer.InetServer = false;
	NetServer.UpdateMasterServer = true;
	NetServer.MaxFps = 100;
	NetServer.WeatherID = "Night_sky_minor_clouds";
	NetServer.IslandID = "";
	NetServer.GameType = NETGAME_DEATHMATCH;
	NetServer.AllowDownload = false;
	NetServer.AllowUpload = false;
	NetServer.GameSpeed = 0;
}

void Net_CreateServer(ref rSrvParams)
{
	iServerTime = 0;
	iPingTime = 0;

	for (int i=0; i<NET_TOTALCLIENTS; i++)
	{
		DeleteAttribute(&NSClients[i], "");
		NSClients[i].index = i;
		NSClients[i].id = i;
		NSClients[i].Use = false;
		NSClients[i].BuyReady = false;
		NSClients[i].StartReady = false;
		NSClients[i].Server = true;			// netserver clients
		NSClients[i].Team = 0;
		NSClients[i].LastPacketIndex = 0;

		NSClients[i].LastPingTime = iPingTime;

		NSClients[i].Stat.Sunk = 0;
		NSClients[i].Stat.Wreck = 0;
		NSClients[i].Stat.Misses = 0;
		NSClients[i].Stat.Hits = 0;
		NSClients[i].Stat.Won = 0;
		NSClients[i].Stat.Lost = 0;
		NSClients[i].Stat.DamageInflicted = 0;
	}

	NetServer.ServerName = rSrvParams.ServerName;
	NetServer.ServerPort = sti(rSrvParams.ServerPort);
	NetServer.ServerPassword = rSrvParams.ServerPassword;
	//NetServer.AdminPassword = "yupi";
	NetServer.Dedicated = sti(rSrvParams.Dedicated);
	NetServer.MaxClients = sti(rSrvParams.MaxPlayers);
	NetServer.LanServer = sti(rSrvParams.LanServer);
	NetServer.InetServer = sti(rSrvParams.InetServer);
	NetServer.UpdateMasterServer = sti(rSrvParams.UpdateMasterServer);
	NetServer.MaxFps = sti(rSrvParams.MaxFps);
	NetServer.WeatherID = rSrvParams.WeatherID;
	NetServer.IslandID = rSrvParams.IslandID;
	NetServer.GameType = sti(rSrvParams.GameType);
	NetServer.NumTeams = sti(rSrvParams.NumTeams);
	NetServer.AllowDownload = sti(rSrvParams.AllowDownload);
	NetServer.AllowUpload = sti(rSrvParams.AllowUpload);
	NetServer.MaxShipClass = sti(rSrvParams.MaxShipClass);
	NetServer.Credit = sti(rSrvParams.Credit);
	NetServer.StartPosition = sti(rSrvParams.StartPosition);
	NetServer.TimeLimit = sti(rSrvParams.TimeLimit);
	NetServer.GameSpeed = sti(rSrvParams.GameSpeed);

	NetServer.GameOver = false;
	
	NetServer.ConvoyShipID = DST_INVALID;
	NetServer.ConvoyPoint.x = 0.0;
	NetServer.ConvoyPoint.z = 0.0;

	CreateEntity(&NetServer, "NetServer");
	NetServer.StartServer = "";

	Net_LoadFile(true, &NSBanList, "BanList.nsv");
	Net_LoadFile(true, &NSPlayers, "Players.nsv");
	
	SetEventHandler("NetServer_OnNetMessage", "NetServer_OnNetMessage", 0);
	SetEventHandler("ExitApplication", "NetServer_ExitApplication", 0);
	SetEventHandler("frame", "NetServer_OneSecondTick", 0);

	bNetServerIsStarted = true;

	NetServer.wWinnerClientID = DST_INVALID;
	NetServer.iWinnerTeam = -1;

	// sort NSPlayers
	NetServer_SortPlayers();

	// send to master server message
	if (sti(NetServer.InetServer))
	{
		int iSMsg = NMCreate();
		NMAddByte(iSMsg, NC_MASTERSERVER);
		NMAddByte(iSMsg, NSC_MASTERSERVER_ADDSERVER);
		NMAddByte(iSMsg, NET_SCRIPT_VERSION);
		NetServer_SendMasterServerMessage(sMasterServerAddress, iMasterServerPort, iSMsg);
		NMDelete(iSMsg);

		SetEventHandler("NetServer_UpdateMasterServerInfo", "NetServer_UpdateMasterServerInfo", 0);
		PostEvent("NetServer_UpdateMasterServerInfo", MASTERSERVER_UPDATETIME);
	}
}

void NetServer_UpdateMasterServerInfo()
{
	int iSMsg = NMCreate();
	NMAddByte(iSMsg, NC_MASTERSERVER);
	NMAddByte(iSMsg, NSC_MASTERSERVER_UPDATESERVER);
	NetServer_SendMasterServerMessage(sMasterServerAddress, iMasterServerPort, iSMsg);
	NMDelete(iSMsg);

	PostEvent("NetServer_UpdateMasterServerInfo", MASTERSERVER_UPDATETIME);
}

void NetServer_OneSecondTick()
{
	int iFullDeltaTime = MakeInt(fHighPrecisionDeltaTime * 1000000.0);
	int iDeltaTime = iFullDeltaTime / 1000;

	int iNewPingTime = iPingTime + iDeltaTime;

	if (!bServerGameStarted)
	{
		if (iNewPingTime / 1000 != iPingTime / 1000)
		{
			for (int i=0; i<NET_MAXCLIENTS; i++)
			{
				if (!sti(NSClients[i].Use)) { continue; }
				
				if (iPingTime - sti(NSClients[i].LastPingTime) >= 15 * 1000)
				{
					NetServer_DropClient(sti(NSClients[i].ID));
				}
			}
		}
	}

	iPingTime = iNewPingTime;
}

void NetServer_ExitApplication()
{
	Net_DeleteServer();
}

bool NetServer_IsStarted()
{
	return bNetServerIsStarted;
}

int NetServer_GetClientsNum()
{
	int iNumClients = 0;
	for (int i=0; i<NET_MAXCLIENTS; i++) { if (sti(NSClients[i].Use)) { iNumClients++; } }
	return iNumClients;
}

void NetServer_SortPlayers()
{
	int i;
	aref arPlayers; makearef(arPlayers, NSPlayers.Players);
	int iNumPlayers = GetAttributesNum(arPlayers);
	
	int iRealDays = iRealYear * (iRealMonth * 31 + iRealDay);

	// kill players who offline > 30 days, fake counting ofcourse, but who care?
	for (i=0; i<iNumPlayers; i++)
	{
		aref arPlayer = GetAttributeN(arPlayers, i);

		int iYear = sti(arPlayer.LastLogin.Year);
		int iMonth = sti(arPlayer.LastLogin.Month);
		int iDay = sti(arPlayer.LastLogin.Day);
		
		if (iYear * (iMonth * 31 + iDay) - iRealDays > 31)
		{
			DeleteAttribute(arPlayers, GetAttributeName(arPlayer));
			i = i - 1;
			continue;
		}
	}

	iNumPlayers = GetAttributesNum(arPlayers);
	for (i=0; i<iNumPlayers; i++) 
	{ 
		if (i >= MAX_TOPPLAYERS) { break; }
		NSSortedPlayers[i] = i; 
	}

	// sort players using rank and (second condition) Ship Sunks
	Net_Sort2i(arPlayers, &NSSortedPlayers, 1, 100, "Rank", "ShipsSunk");
}

// update global player statistics
void NetServer_UpdateGlobalPlayerStatistics(ref rPlayer)
{
	aref arPlayers; makearef(arPlayers, NSPlayers.Players);
	aref arPlayer;

	string sNickName = rPlayer.NickName;
	string sNickPassword = rPlayer.NickPassword;
	
	// add new player if not exist
	if (!CheckAttribute(arPlayers, sNickName)) 
	{ 
		arPlayers.(sNickName) = ""; 

		arPlayers.(sNickName).Nickname = sNickName;
		arPlayers.(sNickName).NickPassword = sNickPassword;
		arPlayers.(sNickName).FaceImage = rPlayer.UserData.Face;
		arPlayers.(sNickName).Hits = 0;
		arPlayers.(sNickName).Misses = 0;
		arPlayers.(sNickName).Won = 0;
		arPlayers.(sNickName).Lost = 0;
		arPlayers.(sNickName).ShipsSunk = 0;
		arPlayers.(sNickName).Shipwrecks = 0;
	}

	makearef(arPlayer, arPlayers.(sNickName));

	int iHits = sti(arPlayer.Hits) + sti(rPlayer.Stat.Hits);
	int iMisses = sti(arPlayer.Misses) + sti(rPlayer.Stat.Misses);
	int iWon = sti(arPlayer.Won) + sti(rPlayer.Stat.Won);
	int iLost = sti(arPlayer.Lost) + sti(rPlayer.Stat.Lost);
	int iSunks = sti(arPlayer.ShipsSunk) + sti(rPlayer.Stat.Sunk);
	int iWrecks = sti(arPlayer.Shipwrecks) + sti(rPlayer.Stat.Wreck)

	arPlayer.Hits = iHits;
	arPlayer.Misses = iMisses;
	arPlayer.Won = iWon;
	arPlayer.Lost = iLost;
	arPlayer.Shipwrecks = iWrecks;
	arPlayer.ShipsSunk = iSunks;
	arPlayer.TotalDamage = sti(arPlayer.TotalDamage) + sti(rPlayer.Stat.DamageInflicted); 

	arPlayer.LastLogin.Year = iRealYear;
	arPlayer.LastLogin.Month = iRealMonth;
	arPlayer.LastLogin.Day = iRealDay;

	if (iHits + iMisses > 0) 
		{ arPlayer.Accuracy = makeint((iHits * 100) / (iHits + iMisses)); }
	else 
		{ arPlayer.Accuracy = 0; }

	arPlayer.Rank = Net_GetRankNum(iSunks, iWon);

	arPlayer.Rewards.Accuracy = Net_GetRewardAccuracy(iHits, iMisses);
	arPlayer.Rewards.Vitality = Net_GetRewardVitality(iSunks, iWrecks);
	arPlayer.Rewards.Victorious = Net_GetRewardVictorious(iWon, iLost);
}