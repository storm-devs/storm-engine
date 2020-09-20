
void NetClient_OnClient(int iMsg)
{
	int iSubCode = NMGetByte(iMsg);

	switch (iSubCode)
	{
		case NSC_CLIENT_CONNECTED: 
			NetClient_Connect(iMsg);
		break;
		case NSC_CLIENT_LOAD: 
			NetClient_Load(iMsg);
		break;
		case NSC_CLIENT_DROP:
			NetClient_Drop(iMsg);
		break;
	}
}

void NetClient_Drop(int iMsg)
{
	int wNetClientID = NMGetClientID(iMsg);	// ClientID
	ref rClient = NetClient_GetClient(wNetClientID);

	rClient.Use = false;

	//DeleteClass(rClient);

	Event("NCEvent_ClientDrop", "l", wNetClientID);
	Event("NCEvent_ShipDelete", "l", wNetClientID);
}

void NetClient_Connect(int iMsg)
{
	int wNetClientID = NMGetClientID(iMsg);

	string sNickName = NMGetString(iMsg);
	string sShipName = NMGetString(iMsg);
	string sFaceImage = NMGetString(iMsg);
	string sFlagImage = NMGetString(iMsg);
	string sSailImage = NMGetString(iMsg);

	int iSailColor = NMGetDword(iMsg);
	int bBuyReady = NMGetByte(iMsg);

	ref rClient = NetClient_GetClient(wNetClientID);

	rClient.Use = true;
	rClient.BuyReady = bBuyReady;
	rClient.StartReady = false;
	rClient.Kick = false;
	rClient.NickName = sNickName;
	rClient.Ship.Name = sShipName;
	rClient.Ship.Sails.Color = iSailColor;

	string sFaceTex = sUserFacesPath + sFaceImage + ".tga";
	string sFlagTex = sUserFlagsPath + sFlagImage + ".tga";
	string sSailTex = sUserSailsPath + sSailImage + ".tga";

	rClient.UserData.Face = sFaceImage;
	rClient.UserData.Face.Texture = NFGetTexture("UserDataImages", sFaceTex);

	rClient.UserData.Sail = sSailImage;
	rClient.UserData.Sail.Texture = NFGetTexture("UserDataImages", sSailTex);

	rClient.UserData.Flag = sFlagImage;
	rClient.UserData.Flag.Texture = NFGetTexture("UserDataImages", sFlagTex);

	rClient.Ship.Type = -1;

	if (!sti(rClient.UserData.Face.Texture))
	{
		rClient.UserData.Face.Texture = NFGetTexture("UserDataImages", sUserFacesPath + "_emptyface_.tga");
		//NetClient_DownloadServerFile("resource\textures\" + sUserFacesPath + sFaceImage + ".tga.tx", NETFILE_FACE, wNetClientID);
	}

	if (!sti(rClient.UserData.Sail.Texture))
	{
		rClient.UserData.Sail.Texture = NFGetTexture("UserDataImages", sUserSailsPath + "_emptysail_.tga");
		//NetClient_DownloadServerFile("resource\textures\" + sUserSailsPath + sSailImage + ".tga.tx", NETFILE_SAIL, wNetClientID);
	}

	if (!sti(rClient.UserData.Flag.Texture))
	{
		rClient.UserData.Flag.Texture = NFGetTexture("UserDataImages", sUserFlagsPath + "_emptyflag_.tga");
		//NetClient_DownloadServerFile("resource\textures\" + sUserFlagsPath + sFlagImage + ".tga.tx", NETFILE_FLAG, wNetClientID);
	}

	Event("NCEvent_Client_Connect", "l", wNetClientID);
}

void NetClient_Load(int iMsg)
{
	int i;

	int wNetClientID = NMGetClientID(iMsg);	// ClientID

	// create battle interface
	if (wNetClientID == NetClient_GetThisClientID())
	{
		// create battle interface
		NetClient_CreateBattleInterface();
		NetClient_CreateInterfaceEnvironment();
	}

	ref rClient = NetClient_GetClient(wNetClientID);

	rClient.Use = true;

	rClient.NickName = NMGetString(iMsg);
	rClient.Dead = false;

	rClient.Ship.Type = NMGetWord(iMsg);
	rClient.Ship.HP = NMGetFloat(iMsg);
	rClient.Ship.MaxHP = rClient.Ship.HP;
	rClient.Ship.Upgrades.Hull = NMGetByte(iMsg);
	rClient.Ship.Upgrades.Sails = NMGetByte(iMsg);

	rClient.Ship.Cannons.Type = NMGetByte(iMsg);
	rClient.Ship.Upgrades.Cannons = NMGetByte(iMsg);
	rClient.Ship.Crew.Quantity = NMGetWord(iMsg);

	rClient.Skill.Accuracy = NMGetByte(iMsg);
	rClient.Skill.Cannons = NMGetByte(iMsg);
	rClient.Skill.Sailing = NMGetByte(iMsg);
	rClient.Skill.Defence = NMGetByte(iMsg);
	rClient.Skill.Repair = NMGetByte(iMsg);

	rClient.Ship.Pos.x = NMGetFloat(iMsg);
	rClient.Ship.Pos.y = NMGetFloat(iMsg);
	rClient.Ship.Pos.z = NMGetFloat(iMsg);
	rClient.Ship.Ang.y = NMGetFloat(iMsg);

	// goods
	for (i=0; i<iNetGoodsNum; i++)
	{
		ref rGood = Net_GetGoodByIndex(i);
		string sGoodName = rGood.Name;

		rClient.Ship.Goods.(sGoodName) = NMGetWord(iMsg);
	}
	
	rClient.Ship.SailState = 0.0;

	rClient.Ship.Cannons.Charge.Type = 0;

	rClient.Ship.MaxSpeedZ = 12.0;
	rClient.Ship.MaxSpeedY = 0.6;
	rClient.Ship.SP = 100.0;

	// init borts with default values
	rClient.Ship.Cannons.Borts = "";
	for (i=0; i<4; i++)
	{
		aref arBorts; makearef(arBorts, rClient.Ship.Cannons.Borts);
		string sCurBort = sNetBortNames[i];

		arBorts.(sCurBort).MaxFireDistance = 1.0;
		arBorts.(sCurBort).ChargeRatio = 1.0;
		arBorts.(sCurBort).DamageRatio = 0.0;
	}

	rClient.Ship.Flares = sti(NCSea.Lights);
	rClient.Ship.Lights = sti(NCSea.Lights);

	ref rShip = Net_GetShipByIndex(sti(rClient.Ship.Type));
	rClient.Model = rShip.Name + "1";

	NetClient_SetTrackSettings(rClient);

	string sPath = "ships\" + rClient.Model + "\" + NetShipHullUpgrades[sti(rClient.Ship.Upgrades.Hull) - 1].str + "\";
	SetTexturePath(0, sPath);
	CreateEntity(rClient, "NetShip");
	SendMessage(rClient, "la", MSG_SHIP_CREATE, rShip);
	SetTexturePath(0, "");

	// create water sound
	float fX = stf(rClient.Ship.Pos.x);
	float fY = stf(rClient.Ship.Pos.y);
	float fZ = stf(rClient.Ship.Pos.z);
	rClient.Ship.Sounds.WaterID.x = 0.0;
	rClient.Ship.Sounds.WaterID.y = 0.0;
	rClient.Ship.Sounds.WaterID.z = 0.12;
	rClient.Ship.Sounds.WaterID = Play3DSoundComplex("ship_water_" + rShip.Soundtype, fX, fY, fZ, true, false);
	if (sti(rClient.Ship.Sounds.WaterID) > 0) { Sound_SetVolume(sti(rClient.Ship.Sounds.WaterID), 0.0); }
	rClient.Ship.Sounds.WaterSpeed = 30.0;

	// create sails sound
	rClient.Ship.Sounds.SailsID.x = 0.0;
	rClient.Ship.Sounds.SailsID.y = 0.5;
	rClient.Ship.Sounds.SailsID.z = 0.0;
	rClient.Ship.Sounds.SailsID = Play3DSoundComplex("sails_up_" + rShip.Soundtype, fX, fY, fZ, true, false);
	if (sti(rClient.Ship.Sounds.SailsID) > 0) { Sound_SetVolume(sti(rClient.Ship.Sounds.SailsID), 0.0); }
	rClient.Ship.Sounds.SailsMove = 0.0;

	// create turn sound
	rClient.Ship.Sounds.TurnID.x = 0.0;
	rClient.Ship.Sounds.TurnID.y = 0.0;
	rClient.Ship.Sounds.TurnID.z = 0.0;
	rClient.Ship.Sounds.TurnID = Play3DSoundComplex("ship_turn_" + rShip.Soundtype, fX, fY, fZ, true, false);
	if (sti(rClient.Ship.Sounds.TurnID) > 0) { Sound_SetVolume(sti(rClient.Ship.Sounds.TurnID), 0.0); }
	rClient.Ship.Sounds.TurnSpeed = 0.05;

	// 
	LayerAddObject("net_execute", rClient, iClientShipPriorityExecute);
	LayerAddObject("net_realize", rClient, iClientShipPriorityRealize);
	LayerAddObject("sea_reflection", rClient, 100);

	Event("NCEvent_ShipCreate", "l", wNetClientID);
	PostEvent("NetClient_ShipUpdateSounds", 1000, "l", wNetClientID);

	RefreshBattleInterface();

	// add ship to foam 
	SeaFoam.AddNetShip = wNetClientID;

	NetBI_SelectShipPointer(wNetClientID);
}

void NetClient_SetTrackSettings(ref rClient)
{
	ref rShip = Net_GetShipByIndex(sti(rClient.Ship.Type));

	rClient.Ship.Track.TrackDistance = 1.0;

	rClient.Ship.Track.WaveHeight1 = 0.2;
	rClient.Ship.Track.WaveHeight2 = 0.9;

	rClient.Ship.Track1.Texture = "ships\trailShip.tga";
	rClient.Ship.Track1.TrackWidthSteps = 12.0;

	rClient.Ship.Track1.ZStart = rShip.Track1.ZStart;
	rClient.Ship.Track1.LifeTime = rShip.Track1.LifeTime;
	rClient.Ship.Track1.Width = rShip.Track1.Width;
	rClient.Ship.Track1.Speed = rShip.Track1.Speed;

	rClient.Ship.Track2.Texture = "ships\trailKeel.tga";
	rClient.Ship.Track2.TrackWidthSteps = 4.0;

	rClient.Ship.Track2.ZStart = rShip.Track2.ZStart;
	rClient.Ship.Track2.LifeTime = rShip.Track2.LifeTime;
	rClient.Ship.Track2.Width = rShip.Track2.Width;
	rClient.Ship.Track2.Speed = rShip.Track2.Speed;
}

void NetClient_DownloadServerFile(string sFileName, int iEventID, int iEventSubID)
{
	if (!sti(NetClient.AllowDownload)) { return; }

	int iFileID1 = NFRCreateFile(-1, sFileName, iEventID, iEventSubID);

	Trace("NetClient trying download server file: " + sFileName);

	int iSMsg = NMCreate();
	NMAddClientID(iSMsg, wClientID);
	NMAddByte(iSMsg, NC_FILE);
	NMAddByte(iSMsg, NSC_FILE_GET_FILE);
	NMAddDword(iSMsg, 0);
	NMAddDword(iSMsg, iFileID1);
	NMAddString(iSMsg, sFileName, 128);
	NetClient_SendMessage(iSMsg, true);
	NMDelete(iSMsg);
}