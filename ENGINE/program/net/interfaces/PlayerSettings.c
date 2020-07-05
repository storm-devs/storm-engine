
void InitInterface(string iniName)
{
	int i;

	GameInterface.title = "titleNetPlayerSettings";

	EnumerateIcons("resource\textures\" + sUserFacesPath, "*.tga.tx", "FACESLIST", 0);
	EnumerateIcons("resource\textures\" + sUserSailsPath, "*.tga.tx", "SAILSLIST", 0);
	EnumerateIcons("resource\textures\" + sUserFlagsPath, "*.tga.tx", "FLAGSLIST", 0);

	SetEventHandler("GetInterfaceTexture", "ScrollGetTexture", 0);
	SetEventHandler("OnSave", "PS_OnSave", 0);
	SetEventHandler("OnDelete", "PS_OnDelete", 0);
	SetEventHandler("OnSelect", "PS_OnSelect", 0);
	SetEventHandler("OnCancel", "PS_OnCancel", 0);

	SetEventHandler("TableSelectChange", "PS_TableSelectChange", 0);	

	SetEventHandler("frame", "PS_CheckButtons", 0);

	// fill colors list
	for (i=0; i<SAILS_COLOR_QUANTITY; i++)
	{
		string attrName = "pic" + (i + 1);
		GameInterface.COLORSLIST.(attrName).name1 = SailsColors[i].icon;
	}
	GameInterface.COLORSLIST.ListSize = SAILS_COLOR_QUANTITY;

	SendMessage(&GameInterface, "ls", MSG_INTERFACE_INIT, iniName);

	PS_FillProfilesTable();
}

void PS_FillProfilesTable()
{
	Table_Clear("TABLE_PROFILES", false, true, false);

	int iCurProfileIndex = -1;
	string sCurProfileName = ""
	if (CheckAttribute(&NCProfiles, "CurProfile")) { sCurProfileName = NCProfiles.CurProfile; }

	// fill profiles table
	if (CheckAttribute(&NCProfiles, "Profiles"))
	{
		aref arProfiles;	makearef(arProfiles, NCProfiles.Profiles);
		int iNumProfiles = GetAttributesNum(arProfiles);
		for (int i=0; i<iNumProfiles; i++)
		{
			aref arProfile; arProfile = GetAttributeN(arProfiles, i);
			string sAttr = "tr" + (i + 1);
			GameInterface.TABLE_PROFILES.(sAttr).td1.str = arProfile.Name;
			if (arProfile.Name == sCurProfileName) { iCurProfileIndex = i; }
		}
	}

	Table_UpdateWindow("TABLE_PROFILES");

	// select current profile
	//if (iCurProfileIndex >= 0) { Event("TableSelectChange", "sl", "TABLE_PROFILES", iCurProfileIndex); }
}

void PS_TableSelectChange()
{
	string sControl = GetEventData();
	int iSelected = GetEventData();

	aref arProfiles;	makearef(arProfiles, NCProfiles.Profiles);
	aref arProfile;		arProfile = GetAttributeN(arProfiles, iSelected - 1);

	GameInterface.EDITBOX_PLAYERNAME.str = arProfile.Name;
	GameInterface.EDITBOX_PLAYERPASSWORD.str = arProfile.Password;
	GameInterface.EDITBOX_PLAYERSHIPNAME.str = arProfile.ShipName;

	// select saved sail color
	ScrollImage_SetPosition("COLORSLIST", sti(arProfile.ColorSail));

	// select saved face
	ScrollImage_SetPosition("FACESLIST", PS_FindIndex("FACESLIST", arProfile.ImageFace));

	// select saved flag
	ScrollImage_SetPosition("FLAGSLIST", PS_FindIndex("FLAGSLIST", arProfile.ImageFlag));

	// select sail
	ScrollImage_SetPosition("SAILSLIST", PS_FindIndex("SAILSLIST", arProfile.ImageSail));
}

int PS_FindIndex(string sControl, string sName)
{
	int iNum = sti(GameInterface.(sControl).ListSize);
	for (int i=0; i<iNum; i++)
	{
		string sRow = "pic" + (i + 1);

		if (GameInterface.(sControl).(sRow).FileName.Name == sName) return i;
	}
	return 0;
}

void PS_CheckButtons()
{
	// check for Save button
	bool bSave = true;
	if (GameInterface.EDITBOX_PLAYERNAME.str == "") { bSave = false; }
	if (GameInterface.EDITBOX_PLAYERSHIPNAME.str == "") { bSave = false; }
	Button_SetEnable("BUTTON_SAVE", bSave);

	// check for Select button
	Button_SetEnable("BUTTON_SELECT", bSave);

	// check for Delete button
	int iSelected = GameInterface.TABLE_PROFILES.select;
	Button_SetEnable("BUTTON_DELETE", iSelected >= 1);
}

void PS_OnSave()
{
	string sNick = GameInterface.EDITBOX_PLAYERNAME.str;

	NCProfiles.Profiles.(sNick).Name = sNick;

	NCProfiles.Profiles.(sNick).Password = GameInterface.EDITBOX_PLAYERPASSWORD.str;
	NCProfiles.Profiles.(sNick).ShipName = GameInterface.EDITBOX_PLAYERSHIPNAME.str;

	string sFacePos = "pic" + (sti(GameInterface.FACESLIST.current) + 1);
	NCProfiles.Profiles.(sNick).ImageFace = GameInterface.FACESLIST.(sFacePos).FileName.Name;

	string sSailPos = "pic" + (sti(GameInterface.SAILSLIST.current) + 1);
	NCProfiles.Profiles.(sNick).ImageSail = GameInterface.SAILSLIST.(sSailPos).FileName.Name; 

	string sFlagPos = "pic" + (sti(GameInterface.FLAGSLIST.current) + 1);
	NCProfiles.Profiles.(sNick).ImageFlag = GameInterface.FLAGSLIST.(sFlagPos).FileName.Name;

	NCProfiles.Profiles.(sNick).ColorSail = GameInterface.COLORSLIST.current;

	PS_FillProfilesTable();
}

void PS_OnDelete()
{
	int iSelected = GameInterface.TABLE_PROFILES.select;
	if (iSelected >= 1)
	{
		string sRow = "tr" + iSelected;
		DeleteAttribute(&NCProfiles, "Profiles." + GameInterface.TABLE_PROFILES.(sRow).td1.str);
		PS_FillProfilesTable();
	}
}

void PS_OnSelect()
{
	string sFacePos = "pic" + (sti(GameInterface.FACESLIST.current) + 1);
	string sSailPos = "pic" + (sti(GameInterface.SAILSLIST.current) + 1);
	string sFlagPos = "pic" + (sti(GameInterface.FLAGSLIST.current) + 1);

	NetClient.SailColor = sti(SailsColors[sti(GameInterface.COLORSLIST.current)].Color);
	NetClient.NickName = GameInterface.EDITBOX_PLAYERNAME.str;
	NetClient.ShipName = GameInterface.EDITBOX_PLAYERSHIPNAME.str;
	NetClient.Password = GameInterface.EDITBOX_PLAYERPASSWORD.str;
	NetClient.FaceImage = GameInterface.FACESLIST.(sFacePos).FileName.Name;
	NetClient.FlagImage = GameInterface.FLAGSLIST.(sFlagPos).FileName.Name;
	NetClient.SailImage = GameInterface.SAILSLIST.(sSailPos).FileName.Name;

	NCProfiles.LastProfile.SailColor = NetClient.SailColor;
	NCProfiles.LastProfile.NickName = NetClient.NickName;
	NCProfiles.LastProfile.ShipName = NetClient.ShipName;
	NCProfiles.LastProfile.Password = NetClient.Password;
	NCProfiles.LastProfile.FaceImage = NetClient.FaceImage;
	NCProfiles.LastProfile.FlagImage = NetClient.FlagImage;
	NCProfiles.LastProfile.SailImage = NetClient.SailImage;

	IDoExit(RC_INTERFACE_NET_FINDGAME);
}

void PS_OnCancel()
{
	IDoExit(RC_INTERFACE_NET_FINDGAME);
}

void IDoExit(int exitCode)
{
	DelEventHandler("GetInterfaceTexture", "ScrollGetTexture");
	DelEventHandler("OnSave", "PS_OnSave");
	DelEventHandler("OnDelete", "PS_OnDelete");
	DelEventHandler("OnSelect", "PS_OnSelect");
	DelEventHandler("OnCancel", "PS_OnCancel");

	DelEventHandler("TableSelectChange", "PS_TableSelectChange");	

	DelEventHandler("frame", "PS_CheckButtons");

	NFDelGroupTextures("FACESLIST");
	NFDelGroupTextures("SAILSLIST");
	NFDelGroupTextures("FLAGSLIST");

	interfaceResultCommand = exitCode;
	
	EndCancelInterface(true);
	
	Net_SaveFile(false, &NCProfiles, "Profiles.nsv");
	//DeleteAttribute(&NCProfiles, "");
}

int ScrollGetTexture()
{
	string sName = GetEventData();
	int iPicIndex = GetEventData();
	string sScrollID = GetEventData();
	int iTexture = NFGetTexture(sScrollID, sName);
	Trace("ScrollGetTexture: " + iTexture + ", " + sName);
	
	return iTexture;
}
