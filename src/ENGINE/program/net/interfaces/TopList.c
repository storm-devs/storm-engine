
void InitInterface(string iniName)
{
	GameInterface.title = "titleNetTopList";

	GameInterface.TABLE_LIST.hr.td1.str = XI_ConvertString("capTL_Player");
	GameInterface.TABLE_LIST.hr.td1.scale = 0.85;
	GameInterface.TABLE_LIST.hr.td2.str = XI_ConvertString("capTL_Rank");
	GameInterface.TABLE_LIST.hr.td2.scale = 0.85;
	GameInterface.TABLE_LIST.hr.td3.str = XI_ConvertString("capTL_Won");
	GameInterface.TABLE_LIST.hr.td3.scale = 0.85;
	GameInterface.TABLE_LIST.hr.td4.str = XI_ConvertString("capTL_Lost");
	GameInterface.TABLE_LIST.hr.td4.scale = 0.85;
	GameInterface.TABLE_LIST.hr.td5.str = XI_ConvertString("capTL_ShipSunk");
	GameInterface.TABLE_LIST.hr.td5.scale = 0.85;
	GameInterface.TABLE_LIST.hr.td6.str = XI_ConvertString("capTL_ShipWrecks");
	GameInterface.TABLE_LIST.hr.td6.scale = 0.85;
	GameInterface.TABLE_LIST.hr.td7.str = XI_ConvertString("capTL_Accuracy");
	GameInterface.TABLE_LIST.hr.td7.scale = 0.85;
	GameInterface.TABLE_LIST.hr.td8.str = XI_ConvertString("capTL_Damage");
	GameInterface.TABLE_LIST.hr.td8.scale = 0.85;
	GameInterface.TABLE_LIST.hr.td9.str = XI_ConvertString("capTL_Rewards");
	GameInterface.TABLE_LIST.hr.td9.scale = 0.85;

	SendMessage(&GameInterface, "ls", MSG_INTERFACE_INIT, iniName);
	SetEventHandler("OnCancel", "ICancelProcedure", 1);
	SetEventHandler("exitCancel", "ICancelProcedure", 1);
	SetEventHandler("Control Activation", "INetInterfaceControlProcess", 1);

	SetEventHandler("NCEvent_PlayerStatistic", "TL_PlayerStatistic", 0);
	FreezeMainNetLayers(true);

	//
	TL_GetPlayerStatFromServer(0);
}

void TL_GetPlayerStatFromServer(int iPlayerIndex)
{
	int iSMsg = NMCreate();
	NMAddClientID(iSMsg, wClientID);
	NMAddByte(iSMsg, NC_STATISTICS);
	NMAddByte(iSMsg, NSC_STATISTICS_TOTAL_PLAYER);
	NMAddWord(iSMsg, iPlayerIndex);
	NetClient_SendMessage(iSMsg, true);
	NMDelete(iSMsg);
}

void IDoExit(int exitCode)
{
	DelEventHandler("Control Activation", "INetInterfaceControlProcess");
	DelEventHandler("OnCancel", "ICancelProcedure");
	DelEventHandler("NCEvent_PlayerStatistic", "TL_PlayerStatistic");
	FreezeMainNetLayers(false);

	interfaceResultCommand = exitCode;
	EndCancelInterface(true);
}

void ICancelProcedure()
{
	IDoExit( RC_INTERFACE_NET_TOPLIST );
}

void INetInterfaceControlProcess()
{
	string sControl = GetEventData();
	if( sControl == "toplist" ) IDoExit( RC_INTERFACE_NET_TOPLIST );
}

void TL_PlayerStatistic()
{
	int iPIndex = GetEventData();

	if (iPIndex == -1) return;

	string sNickName = GetEventData();
	string sFaceImage = GetEventData();

	int iRank = GetEventData();
	int iRewardAccuracy = GetEventData();
	int iRewardVitality = GetEventData();
	int iRewardVictorious = GetEventData();

	int iWon = GetEventData();
	int iLost = GetEventData();

	int iWrecks = GetEventData();
	int iSunk = GetEventData();

	float fAccuracy = GetEventData();
	int iDamage = GetEventData();

	// TABLE
	string sRow = "tr" + (iPIndex + 1);

	string sFaceTex = sUserFacesPath + sFaceImage + ".tga";
	GameInterface.TABLE_LIST.(sRow).td1.icon.offset = "0,1";
	GameInterface.TABLE_LIST.(sRow).td1.icon.width = 26;
	GameInterface.TABLE_LIST.(sRow).td1.icon.height = 26;
	GameInterface.TABLE_LIST.(sRow).td1.icon.valign = "center";
	GameInterface.TABLE_LIST.(sRow).td1.icon.texturepointer = NFGetTexture("TopList", sFaceTex);
	GameInterface.TABLE_LIST.(sRow).td1.textoffset = "29,0";
	GameInterface.TABLE_LIST.(sRow).td1.valign = "center";
	GameInterface.TABLE_LIST.(sRow).td1.align = "left";
	GameInterface.TABLE_LIST.(sRow).td1.scale = 0.9;
	GameInterface.TABLE_LIST.(sRow).td1.str = sNickName;

	GameInterface.TABLE_LIST.(sRow).td2.str = XI_ConvertString("NR_" + NetRanks[iRank].Rank);

	GameInterface.TABLE_LIST.(sRow).td3.str = iWon;
	GameInterface.TABLE_LIST.(sRow).td4.str = iLost;

	GameInterface.TABLE_LIST.(sRow).td5.str = iSunk;
	GameInterface.TABLE_LIST.(sRow).td6.str = iWrecks;

	GameInterface.TABLE_LIST.(sRow).td7.str = makeint(fAccuracy) + "%";

	GameInterface.TABLE_LIST.(sRow).td8.str = iDamage;

	// rewards icons
	/*string sRewardsGroup = "CANNONS_UPGRADE_3";//"net_rewards";
	GameInterface.TABLE_LIST.(sRow).td9.icon1.offset = "1,2";
	GameInterface.TABLE_LIST.(sRow).td9.icon1.width = 27;
	GameInterface.TABLE_LIST.(sRow).td9.icon1.height = 27;
	GameInterface.TABLE_LIST.(sRow).td9.icon1.group = sRewardsGroup;
	GameInterface.TABLE_LIST.(sRow).td9.icon1.icon = "cannons1";

	GameInterface.TABLE_LIST.(sRow).td9.icon2.offset = "30,2";
	GameInterface.TABLE_LIST.(sRow).td9.icon2.width = 27;
	GameInterface.TABLE_LIST.(sRow).td9.icon2.height = 27;
	GameInterface.TABLE_LIST.(sRow).td9.icon2.group = sRewardsGroup;
	GameInterface.TABLE_LIST.(sRow).td9.icon2.icon = "cannons2";
	
	GameInterface.TABLE_LIST.(sRow).td9.icon3.offset = "59,2";
	GameInterface.TABLE_LIST.(sRow).td9.icon3.width = 27;
	GameInterface.TABLE_LIST.(sRow).td9.icon3.height = 27;
	GameInterface.TABLE_LIST.(sRow).td9.icon3.group = sRewardsGroup;
	GameInterface.TABLE_LIST.(sRow).td9.icon3.icon = "cannons3";*/

	Table_UpdateWindow("TABLE_LIST");

	if (iPIndex < 100)
	{
		TL_GetPlayerStatFromServer(iPIndex + 1);
	}
}