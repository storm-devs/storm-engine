#include "net.h"
#include "Server.h"
#include "Client.h"
#include "nurbs.h"
#include "NetFile\NetFile.h"
#include "Entities\Chat\Say.h"
#include "Entities\Sea\Sea.h"
#include "Entities\Ship\Ship.h"
#include "Entities\Ship\Track.h"
#include "Entities\Camera\DeckCamera.h"
#include "Entities\Camera\ShipCamera.h"
#include "Entities\Balls\NetBalls.h"
#include "Entities\Weather\Weather.h"
#include "Entities\Rigging\rope.h"
#include "Entities\Rigging\sail.h"
#include "Entities\Rigging\flag.h"
#include "Entities\Rigging\vant.h"
#include "Entities\Rigging\mast.h"
#include "Entities\Island\Island.h"
#include "Entities\Island\Foam.h"
#include "Entities\Touch\Touch.h"
#include "Entities\Fort\Fort.h"
#include "Entities\LightPillar\Pillar.h"

// 
class NET_SCRIPT_LIBRIARY : public SCRIPT_LIBRIARY
{
public:
	static VDX8RENDER * pRS;

	NET_SCRIPT_LIBRIARY() {};
	~NET_SCRIPT_LIBRIARY() {};
	virtual bool Init();
};

VDX8RENDER * NET_SCRIPT_LIBRIARY::pRS = null;

#include "Script\NetFile.inl"
#include "Script\NetMessage.inl"
#include "Script\NetSocket.inl"
#include "Script\NetInet.inl"

INTERFACE_FUNCTION
CREATE_CLASS(Net)
CREATE_CLASS(NetServer)
CREATE_CLASS(NetClient)
CREATE_CLASS(NetSea)
CREATE_CLASS(NetBalls)
CREATE_CLASS(NetWeather)
CREATE_CLASS(NetRain)
CREATE_CLASS(NetSunGlow)
CREATE_CLASS(NetLightning)
CREATE_CLASS(NetSky)
CREATE_CLASS(NetShip)
CREATE_CLASS(NetShipLights)
CREATE_CLASS(NetShipTracks)
CREATE_CLASS(NetDeckCamera)
CREATE_CLASS(NetShipCamera)
CREATE_CLASS(NetSail)
CREATE_CLASS(NetFlag)
CREATE_CLASS(NetRope)
CREATE_CLASS(NetVant)
CREATE_CLASS(NetMast)
CREATE_CLASS(NetSay)
CREATE_CLASS(NetIsland)
CREATE_CLASS(NetCoastFoam)
CREATE_CLASS(NetTouch)
CREATE_CLASS(NetFort)
CREATE_CLASS(NetLightPillar)
CREATE_SCRIPTLIBRIARY(NET_SCRIPT_LIBRIARY)

dword _cdecl NetServer_DelClient(VS_STACK * pS)
{
	word wClientID = word(((VDATA*)pS->Pop())->GetLong());
	NetServer::pServer->DelClient(wClientID);
	return IFUNCRESULT_OK;
}

dword _cdecl NetServer_AddClient(VS_STACK * pS)
{
	word wPort = word(((VDATA*)pS->Pop())->GetLong());
	dword dwIP = dword(((VDATA*)pS->Pop())->GetLong());
	word wClientID = word(((VDATA*)pS->Pop())->GetLong());
	NetServer::pServer->AddClient(wClientID, dwIP, wPort);
	return IFUNCRESULT_OK;
}

dword _cdecl Net_GetTickCount(VS_STACK * pS)
{
	((VDATA*)pS->Push())->Set( (long)GetTickCount() );
	return IFUNCRESULT_OK;
}

bool NET_SCRIPT_LIBRIARY::Init()
{
	IFUNCINFO fi;

	pRS = (VDX8RENDER *)api->CreateService("dx8render"); Assert(pRS);

	// ===================================
	// Net native function
	// ===================================
	fi.nArguments = 3; fi.pFuncName = "Net_SaveFile"; fi.pReturnValueName = "void"; fi.pFuncAddress = Net_SaveFile; api->SetScriptFunction(&fi);
	fi.nArguments = 3; fi.pFuncName = "Net_LoadFile"; fi.pReturnValueName = "void"; fi.pFuncAddress = Net_LoadFile; api->SetScriptFunction(&fi);
	fi.nArguments = 0; fi.pFuncName = "Net_GetTickCount"; fi.pReturnValueName = "int"; fi.pFuncAddress = Net_GetTickCount; api->SetScriptFunction(&fi);
	fi.nArguments = 6; fi.pFuncName = "Net_Sort2i"; fi.pReturnValueName = "void"; fi.pFuncAddress = Net_Sort2i; api->SetScriptFunction(&fi);

	// ===================================
	// Net INI native function
	// ===================================
	fi.nArguments = 3; fi.pFuncName = "NetIni_GetLong"; fi.pReturnValueName = "int"; fi.pFuncAddress = NetIni_GetLong; api->SetScriptFunction(&fi);
	fi.nArguments = 3; fi.pFuncName = "NetIni_GetString"; fi.pReturnValueName = "string"; fi.pFuncAddress = NetIni_GetString; api->SetScriptFunction(&fi);

	// ===================================
	// NetServer/NetClient native function
	// ===================================
	fi.nArguments = 3; fi.pFuncName = "NetServer_AddClient"; fi.pReturnValueName = "void"; fi.pFuncAddress = NetServer_AddClient; api->SetScriptFunction(&fi);

	fi.nArguments = 1; fi.pFuncName = "NetServer_DelClient"; fi.pReturnValueName = "void"; fi.pFuncAddress = NetServer_DelClient; api->SetScriptFunction(&fi);

	fi.nArguments = 3; fi.pFuncName = "NetServer_SendMessage"; fi.pReturnValueName = "void"; fi.pFuncAddress = NetServer_SendMessage; api->SetScriptFunction(&fi);
	fi.nArguments = 3; fi.pFuncName = "NetServer_SendMessageAllExcept"; fi.pReturnValueName = "void"; fi.pFuncAddress = NetServer_SendMessageAllExcept; api->SetScriptFunction(&fi);
	fi.nArguments = 3; fi.pFuncName = "NetServer_SendMessageDirect"; fi.pReturnValueName = "void"; fi.pFuncAddress = NetServer_SendMessageDirect; api->SetScriptFunction(&fi);
	fi.nArguments = 3; fi.pFuncName = "NetServer_SendMasterServerMessage"; fi.pReturnValueName = "void"; fi.pFuncAddress = NetServer_SendMasterServerMessage; api->SetScriptFunction(&fi);
	fi.nArguments = 2; fi.pFuncName = "NetServer_GarantedDeliveryCallback"; fi.pReturnValueName = "void"; fi.pFuncAddress = NetServer_GarantedDeliveryCallback; api->SetScriptFunction(&fi);
	fi.nArguments = 0; fi.pFuncName = "NetServer_ClearPacketsQueue"; fi.pReturnValueName = "void"; fi.pFuncAddress = NetServer_ClearPacketsQueue; api->SetScriptFunction(&fi);

	fi.nArguments = 2; fi.pFuncName = "NetClient_SendMessage"; fi.pReturnValueName = "void"; fi.pFuncAddress = NetClient_SendMessage; api->SetScriptFunction(&fi);
	fi.nArguments = 1; fi.pFuncName = "NetClient_GarantedDeliveryCallback"; fi.pReturnValueName = "void"; fi.pFuncAddress = NetClient_GarantedDeliveryCallback; api->SetScriptFunction(&fi);
	fi.nArguments = 1; fi.pFuncName = "NetClient_SendMessageDirect"; fi.pReturnValueName = "void"; fi.pFuncAddress = NetClient_SendMessageDirect; api->SetScriptFunction(&fi);
	fi.nArguments = 3; fi.pFuncName = "NetClient_SendMessageDirectToIP"; fi.pReturnValueName = "void"; fi.pFuncAddress = NetClient_SendMessageDirectToIP; api->SetScriptFunction(&fi);
	fi.nArguments = 1; fi.pFuncName = "NetClient_SendBroadcastMessage"; fi.pReturnValueName = "void"; fi.pFuncAddress = NetClient_SendBroadcastMessage; api->SetScriptFunction(&fi);
	fi.nArguments = 3; fi.pFuncName = "NetClient_SendMasterServerMessage"; fi.pReturnValueName = "void"; fi.pFuncAddress = NetClient_SendMasterServerMessage; api->SetScriptFunction(&fi);
	fi.nArguments = 0; fi.pFuncName = "NetClient_ClearPacketsQueue"; fi.pReturnValueName = "void"; fi.pFuncAddress = NetClient_ClearPacketsQueue; api->SetScriptFunction(&fi);

	// ===================================
	// NetMessages native function
	// ===================================
	fi.nArguments = 0; fi.pFuncName = "NMCreate"; fi.pReturnValueName = "int"; fi.pFuncAddress = NMCreate; api->SetScriptFunction(&fi);
	fi.nArguments = 1; fi.pFuncName = "NMDelete"; fi.pReturnValueName = "void"; fi.pFuncAddress = NMDelete; api->SetScriptFunction(&fi);

	fi.nArguments = 1; fi.pFuncName = "NMNotEmpty"; fi.pReturnValueName = "int"; fi.pFuncAddress = NMNotEmpty; api->SetScriptFunction(&fi);

	fi.nArguments = 1; fi.pFuncName = "NMGetByte"; fi.pReturnValueName = "int"; fi.pFuncAddress = NMGetByte; api->SetScriptFunction(&fi);
	fi.nArguments = 1; fi.pFuncName = "NMGetWord"; fi.pReturnValueName = "int"; fi.pFuncAddress = NMGetWord; api->SetScriptFunction(&fi);
	fi.nArguments = 1; fi.pFuncName = "NMGetDword"; fi.pReturnValueName = "int"; fi.pFuncAddress = NMGetDword; api->SetScriptFunction(&fi);
	fi.nArguments = 1; fi.pFuncName = "NMGet24"; fi.pReturnValueName = "int"; fi.pFuncAddress = NMGet24; api->SetScriptFunction(&fi);
	fi.nArguments = 1; fi.pFuncName = "NMGetFloat"; fi.pReturnValueName = "float"; fi.pFuncAddress = NMGetFloat; api->SetScriptFunction(&fi);
	fi.nArguments = 1; fi.pFuncName = "NMGetString"; fi.pReturnValueName = "string"; fi.pFuncAddress = NMGetString; api->SetScriptFunction(&fi);

	fi.nArguments = 2; fi.pFuncName = "NMAddByte"; fi.pReturnValueName = "void"; fi.pFuncAddress = NMAddByte; api->SetScriptFunction(&fi);
	fi.nArguments = 2; fi.pFuncName = "NMAddWord"; fi.pReturnValueName = "void"; fi.pFuncAddress = NMAddWord; api->SetScriptFunction(&fi);
	fi.nArguments = 2; fi.pFuncName = "NMAddDword"; fi.pReturnValueName = "void"; fi.pFuncAddress = NMAddDword; api->SetScriptFunction(&fi);
	fi.nArguments = 2; fi.pFuncName = "NMAdd24"; fi.pReturnValueName = "void"; fi.pFuncAddress = NMAdd24; api->SetScriptFunction(&fi);
	fi.nArguments = 2; fi.pFuncName = "NMAddFloat"; fi.pReturnValueName = "void"; fi.pFuncAddress = NMAddFloat; api->SetScriptFunction(&fi);
	fi.nArguments = 3; fi.pFuncName = "NMAddString"; fi.pReturnValueName = "void"; fi.pFuncAddress = NMAddString; api->SetScriptFunction(&fi);
	fi.nArguments = 3; fi.pFuncName = "NMAddData"; fi.pReturnValueName = "void"; fi.pFuncAddress = NMAddData; api->SetScriptFunction(&fi);

	// ===================================
	// Inet native function
	// ===================================
	fi.nArguments = 1; fi.pFuncName = "inet_ntoa"; fi.pReturnValueName = "string"; fi.pFuncAddress = __inet_ntoa; api->SetScriptFunction(&fi);
	fi.nArguments = 1; fi.pFuncName = "inet_ntoax"; fi.pReturnValueName = "string"; fi.pFuncAddress = __inet_ntoax; api->SetScriptFunction(&fi);
	fi.nArguments = 1; fi.pFuncName = "ntohs"; fi.pReturnValueName = "int"; fi.pFuncAddress = __ntohs; api->SetScriptFunction(&fi);

	fi.nArguments = 4; fi.pFuncName = "NFFindFiles"; fi.pReturnValueName = "int"; fi.pFuncAddress = NFFindFiles; api->SetScriptFunction(&fi);

	// ===================================
	// NetFileSend/NetFileRecv native function
	// ===================================
	fi.nArguments = 1; fi.pFuncName = "NFExist"; fi.pReturnValueName = "int"; fi.pFuncAddress = NFExist; api->SetScriptFunction(&fi);

	fi.nArguments = 4; fi.pFuncName = "NFRCreateFile"; fi.pReturnValueName = "int"; fi.pFuncAddress = NFRCreateFile; api->SetScriptFunction(&fi);
	fi.nArguments = 1; fi.pFuncName = "NFRDeleteFile"; fi.pReturnValueName = "void"; fi.pFuncAddress = NFRDeleteFile; api->SetScriptFunction(&fi);
	fi.nArguments = 3; fi.pFuncName = "NFRReceiveData"; fi.pReturnValueName = "void"; fi.pFuncAddress = NFRReceiveData; api->SetScriptFunction(&fi);
	fi.nArguments = 3; fi.pFuncName = "NFRIsEqualFile"; fi.pReturnValueName = "int"; fi.pFuncAddress = NFRIsEqualFile; api->SetScriptFunction(&fi);
	fi.nArguments = 2; fi.pFuncName = "NFRSetSendHandle"; fi.pReturnValueName = "void"; fi.pFuncAddress = NFRSetSendHandle; api->SetScriptFunction(&fi);

	fi.nArguments = 3; fi.pFuncName = "NFSCreateFile"; fi.pReturnValueName = "int"; fi.pFuncAddress = NFSCreateFile; api->SetScriptFunction(&fi);
	fi.nArguments = 1; fi.pFuncName = "NFSDeleteFile"; fi.pReturnValueName = "void"; fi.pFuncAddress = NFSDeleteFile; api->SetScriptFunction(&fi);
	fi.nArguments = 1; fi.pFuncName = "NFSReceiverReady"; fi.pReturnValueName = "void"; fi.pFuncAddress = NFSReceiverReady; api->SetScriptFunction(&fi);
	fi.nArguments = 2; fi.pFuncName = "NFSDataACK"; fi.pReturnValueName = "void"; fi.pFuncAddress = NFSDataACK; api->SetScriptFunction(&fi);

	// ===================================
	// Load/Delete Texture
	// ===================================
	fi.nArguments = 2; fi.pFuncName = "NFGetTexture"; fi.pReturnValueName = "int"; fi.pFuncAddress = NFGetTexture; api->SetScriptFunction(&fi);
	fi.nArguments = 2; fi.pFuncName = "NFDelTexture"; fi.pReturnValueName = "void"; fi.pFuncAddress = NFDelTexture; api->SetScriptFunction(&fi);
	fi.nArguments = 1; fi.pFuncName = "NFDelGroupTextures"; fi.pReturnValueName = "void"; fi.pFuncAddress = NFDelGroupTextures; api->SetScriptFunction(&fi);

	return true;
}

Net::Net()
{
}

Net::~Net()
{
	NetServer::pServer = null;
	NetClient::pClient = null;

	api->SetNetActive(false);
	WSACleanup();
}

bool Net::Init()
{
	if (!WSAStartup(1, 1)) return false;

	VDX8RENDER * pRS = (VDX8RENDER *)api->CreateService("dx8render"); Assert(pRS);
	
	NetServer::pRS = pRS;
	NetClient::pRS = pRS;

	api->SetNetActive(true);

	return true;
}

bool Net::WSAStartup(dword dwVerHi, dword dwVerLo)
{
   	long iStatus = ::WSAStartup(MAKEWORD(dwVerHi,dwVerLo), &WSAData);
	return true;
}


bool Net::WSACleanup()
{
	::WSACleanup();
	return true;
}

void Net::Execute(dword dwDeltaTime)
{
	float fDeltaTime = float(dwDeltaTime) * 0.001f;

	if (NetServer::pServer) NetServer::pServer->Execute(fDeltaTime);
	if (NetClient::pClient) NetClient::pClient->Execute(fDeltaTime);

	for (long i=0; i<NetFileSend::aNFSend; i++)
		NetFileSend::aNFSend[i]->Execute(fDeltaTime);
}

void Net::Realize(dword dwDeltaTime)
{
	float fDeltaTime = float(dwDeltaTime) * 0.001f;

	if (NetServer::pServer) NetServer::pServer->Realize(fDeltaTime);
	if (NetClient::pClient) NetClient::pClient->Realize(fDeltaTime);
}

dword _cdecl Net::ProcessMessage(MESSAGE & message)
{
	switch (message.Long())
	{
		case SD_SERVERMESSAGE:
		{
			dword wParam = message.Dword();
			dword lParam = message.Dword();
			NetServer::pServer->ReceiveMessage((SOCKET)wParam, lParam);
		}
		break;
		case SD_CLIENTMESSAGE:
		{
			dword wParam = message.Dword();
			dword lParam = message.Dword();
			NetClient::pClient->ReceiveMessage((SOCKET)wParam, lParam);
		}
		break;
	}
	return 0;
}

dword Net::AttributeChanged(ATTRIBUTES * pA)
{
	return 0;
}

