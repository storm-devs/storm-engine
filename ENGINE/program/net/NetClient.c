#include "Net\Client\Cameras.c"
#include "Net\Client\ShipLights.c"
#include "Net\Client\Rigging.c"
#include "Net\Client\Island.c"
#include "Net\Client\Time.c"
#include "Net\Client\Fort.c"

#include "Net\Client\Utils.c"
#include "Net\Client\File.c"

#include "Net\Client\OnNetMessage.c"
#include "Net\Client\OnConnect.c"
#include "Net\Client\OnBall.c"
#include "Net\Client\OnChat.c"
#include "Net\Client\OnShip.c"
#include "Net\Client\OnPing.c"
#include "Net\Client\OnStart.c"
#include "Net\Client\OnWeather.c"
#include "Net\Client\OnClient.c"
#include "Net\Client\ServerInfo.c"
#include "Net\Client\PlayerInfo.c"

#include "Net\Client\Weather\Weather.c"

#include "Net\Interfaces\Interface.c"
#include "Net\Interfaces\BattleInterface\BattleInterface.c"

int wClientID;

object NCBalls;
object NCConsole;
object NCIsland, NCCoastFoam;
object NCSail, NCVant, NCRope, NCFlag;
object NCSay;
object NCShipTracks;
object NCLightPillar;

object NCServer;

object NCClients[NET_TOTALCLIENTS];
object NCProfiles;
object NCFavorites;
object NCInetServers;

int iLangNetClient;

string sClientNetCamera = "DeckCamera";
int iClientTime, iClientServerTime, iClientDeltaTime;
int iTestDeltaTime;

void Net_ClientFirstInit()
{
	NetClient.NickName = "default";
	NetClient.Password = "";
	NetClient.ShipName = "noname";
	NetClient.FaceImage = "_emptyface_";
	NetClient.SailImage = "_emptysail_";
	NetClient.FlagImage = "_emptyflag_";
	NetClient.SailColor = argb(255, 255, 255, 255);

	NetClient.ClientLastPacketIndex = 0;
}

void Net_DeleteClient()
{
	DelEventHandler("ExitApplication", "NetClient_ExitApplication");
	DelEventHandler("NetClient_OnNetMessage", "NetClient_OnNetMessage");
	DelEventHandler("NetClient_Ping", "NetClient_Ping");

	NetClient_OnEndGame();

	DeleteClass(&NetClient);
	DeleteClass(&NCLightPillar);

	LanguageCloseFile(iLangNetClient); iLangNetClient  = -1;

	Net_SaveFile(false, &NCInetServers, "InetServers.nsv");
}

void Net_CreateClient()
{
	for (int i=0; i<NET_TOTALCLIENTS; i++)
	{
		DeleteAttribute(&NCClients[i], "");
		NCClients[i].index = i;
		NCClients[i].ID = i;
		NCClients[i].Use = false;
		NCClients[i].Server = false;			// netclient clients
		
		NCClients[i].BuyReady = false;
		NCClients[i].StartReady = false;
		NCClients[i].Kick = false;

		NCClients[i].Team = 0;

		NCClients[i].Dead = false;

		NCClients[i].Stat.DamageInflicted = 0;
		NCClients[i].Stat.Accuracy = 0;

		NCClients[i].Stat.Kills = "";

		NCClients[i].Skills.Repair.Active = 0;
		NCClients[i].Skills.Repair.TotalCooldownTime = 0;
		NCClients[i].Skills.Repair.CurrentCooldownTime = 0;
	}

	iClientTime = 0;
	iClientServerTime = 0;
	iClientDeltaTime = 0;
	iTestDeltaTime = 0;

	CreateEntity(&NetClient, "NetClient");

	NetClient.ServerPassword = "";
	NetClient.ServerAddr = "";
	NetClient.ServerPort = NET_DEFAULT_SERVER_PORT;
	NetClient.Start = "";
	NetClient.AllowDownload = true;
	NetClient.AllowUpload = true;

	NetClient.ConvoyShipID = DST_INVALID;
	NetClient.ConvoyPoint.x = 0.0;
	NetClient.ConvoyPoint.z = 0.0;

	NetClient_PingInit();

	SetEventHandler("ExitApplication", "NetClient_ExitApplication", 0);
	SetEventHandler("NetClient_OnNetMessage", "NetClient_OnNetMessage", 0);
	SetEventHandler("NetClient_Ping", "NetClient_Ping", 0);

	// init NCServer object with fake values
	NCServer.NumClients = 0;
	NCServer.MaxClients = NET_MAXCLIENTS;
	NCServer.NumTeams = 4;
	NCServer.IslandID = "";
	NCServer.WeatherID = "";

	NetClient.ClientLastPacketIndex = 0;

	iLangNetClient = LanguageOpenFile("NetSection.txt");

	// load client profiles
	DeleteAttribute(&NCProfiles, "");
	Net_LoadFile(false, &NCProfiles, "Profiles.nsv");

	if (CheckAttribute(&NCProfiles, "LastProfile"))
	{
		NetClient.SailColor = NCProfiles.LastProfile.SailColor;
		NetClient.NickName = NCProfiles.LastProfile.NickName;
		NetClient.ShipName = NCProfiles.LastProfile.ShipName;
		NetClient.Password = NCProfiles.LastProfile.Password;
		NetClient.FaceImage = NCProfiles.LastProfile.FaceImage;
		NetClient.FlagImage = NCProfiles.LastProfile.FlagImage;
		NetClient.SailImage = NCProfiles.LastProfile.SailImage;
	}

	// load client inet servers list
	DeleteAttribute(&NCInetServers, "");
	Net_LoadFile(false, &NCInetServers, "InetServers.nsv");
}

void NetClient_ExitApplication()
{
}

void NetClient_FindLocalServers()
{
	int iSMsg = NMCreate();
	NMAddClientID(iSMsg, DST_INVALID);
	NMAddByte(iSMsg, NC_BROADCAST);
	NMAddByte(iSMsg, NSC_BROADCAST_LOCAL);
	NMAddDword(iSMsg, Net_GetTickCount());
	NetClient_SendBroadcastMessage(iSMsg);
	NMDelete(iSMsg);
}

void NetClient_TryConnect(string sServerIP, int wPort)
{
	NetClient.ServerAddr = sServerIP;
	NetClient.ServerPort = wPort;

	int iSMsg = NMCreate();
	NMAddClientID(iSMsg, DST_INVALID);
	NMAddByte(iSMsg, NC_CONNECT);
	NMAdd24(iSMsg, NET_TEST_DWORD);
	NMAddByte(iSMsg, NET_SCRIPT_VERSION);
	NMAddString(iSMsg, NetClient.NickName, 24);			// nick name
	NMAddString(iSMsg, NetClient.Password, 12);			// nick password
	NMAddString(iSMsg, NetClient.ServerPassword, 12);	// nick password
	NMAddString(iSMsg, NetClient.ShipName, 24);			// ship name
	NMAddString(iSMsg, NetClient.FaceImage, 12);		// client face
	NMAddString(iSMsg, NetClient.FlagImage, 12);		// client flag
	NMAddString(iSMsg, NetClient.SailImage, 12);		// client sail emblem
	NMAddDword(iSMsg, sti(NetClient.SailColor));		// client sail color
	NetClient_SendMessage(iSMsg, false);
	NMDelete(iSMsg);
}

void NetClient_SelectProfile(string sProfileName)
{
	NetClient.SailColor = sti(SailsColors[sti(NCProfiles.Profiles.(sProfileName).ColorSail)].Color);
	
	NetClient.NickName = NCProfiles.Profiles.(sProfileName).Name;
	NetClient.ShipName = NCProfiles.Profiles.(sProfileName).ShipName;
	NetClient.Password = NCProfiles.Profiles.(sProfileName).Password;

	NetClient.FaceImage = NCProfiles.Profiles.(sProfileName).ImageFace;
	NetClient.FlagImage = NCProfiles.Profiles.(sProfileName).ImageFlag;
	NetClient.SailImage = NCProfiles.Profiles.(sProfileName).ImageSail;
}