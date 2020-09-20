#include "Net\Common.h"
#include "Net\Extern.h"
#include "Net\Net_Defines.h"
#include "Net\NetServer.c"
#include "Net\NetClient.c"
#include "Net\NetFile.c"
#include "Net\Interfaces\Interface.h"

bool bNetActive = false;

object Net;
object NetServer;
object NetClient;

string	sMasterServerAddress = "192.168.0.15";
int		iMasterServerPort = 29029;

extern void NetInterface_AddLocalServerToList(string sName, string sIP, int wPort, bool bSecure, string sGameType, int iCredit, int iMaxClass, string sWeatherID, string sIslandID, int iNumClients, int iMaxClients, int iPing);
extern void NetInterface_AddInternetServerToList(string sName, string sIP, int wPort, bool bSecure, string sGameType, int iCredit, int iMaxClass, string sWeatherID, string sIslandID, int iNumClients, int iMaxClients, int iPing);

int NMGetClientID(int iMsg)
{
	return NMGetByte(iMsg);
}

void NMAddClientID(int iSMsg, int iClientID)
{
	NMAddByte(iSMsg, iClientID);
}

bool IsNetActive() 
{
	return bNetActive;
}

void Net_Stop()
{
	bNetActive = false;

	Net_DeleteServer();
	Net_DeleteClient();

	DeleteClass(&Net);

	DelEventHandler("NetFileSend_SendData", "NetFileSend_SendData");
	DelEventHandler("NetFileSend_TransferReady", "NetFileSend_TransferReady");
}

void Net_Init()
{
	// load favorites list
	//Net_LoadFile(false, &INet_FavoritesServersList, "Favorites.srv");

	// First interface parameters init
	iCurrentServersList = NET_FG_LAN;
	
	iNumLanServers = 0;
	iNumInternetServers = 0;

	iNumLanServersList = 0;
	iNumInternetServersList = 0;

	NetModes[NETGAME_DEATHMATCH].Name = "Deathmatch";
	NetModes[NETGAME_NAVAL].Name = "TeamDeathmatch";
	NetModes[NETGAME_CONVOY].Name = "DefendTheConvoy";
	NetModes[NETGAME_FORT].Name = "CaptureTheFort";

	// team colors
	NetTeamColor[0] = argb(255, 255, 0, 0);
	NetTeamColor[1] = argb(255, 0, 255, 0);
	NetTeamColor[2] = argb(255, 0, 0, 255);
	NetTeamColor[3] = argb(255, 255, 165, 243);

	NetTeamName[0] = "red";
	NetTeamName[1] = "green";
	NetTeamName[2] = "blue";
	NetTeamName[3] = "pink";

	sMasterServerAddress = NetIni_GetString("engine.ini", "Network", "MasterServerAddr");
	iMasterServerPort = NetIni_GetLong("engine.ini", "Network", "MasterServerPort");
}

void Net_Start()
{
	if (bNetActive) 
	{ 
		if (!IsEntity(&NetClient)) { Net_CreateClient(); }
		return; 
	}

	bNetActive = true;

	LayerCreate("net_realize", 1);	
	LayerSetRealize("net_realize", 1);
	LayerCreate("net_execute", 1);	
	LayerSetExecute("net_execute", 1);

	LayerFreeze("net_realize", false);
	LayerFreeze("net_execute", false);

	if (!IsEntity(&Net))
	{
		CreateEntity(&Net, "Net");
		LayerAddObject("net_execute", &Net, -1);
		LayerAddObject("net_realize", &Net, -1);
	}

	Net_InitShips();
	Net_InitGoods();
	Net_InitCannons();
	Net_InitSkills();
	Net_InitIslands();
	Net_InitPerks();
	Net_InitRewards();
	Net_InitUpgrades();

	if (!IsEntity(&NetClient)) { Net_CreateClient(); }

	SetEventHandler("NetFileSend_SendData", "NetFileSend_SendData", 0);
	SetEventHandler("NetFileSend_TransferReady", "NetFileSend_TransferReady", 0);
}

string Net_GetControlGroup()
{
	if( sti(InterfaceStates.Launched) == true ) return "MainInterface";

	if( CheckAttribute(&NetBInterface,"ComState") && sti(NetBInterface.ComState) != 0 )
		return "BattleInterfaceControls";

	return "NetShipControls";
}

void Net_StartServer()
{
	int iSMsg = NMCreate();
	NMAddClientID(iSMsg, wClientID);
	NMAddByte(iSMsg, NC_COMMAND);
	NMAddByte(iSMsg, NSC_COMMAND_STARTGAME);
	NetClient_SendMessage(iSMsg, true);
	NMDelete(iSMsg);
}

void ntrace(string sTrace)
{
	Trace(sTrace);
}