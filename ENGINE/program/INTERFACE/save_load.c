#define CONFIRMMODE_PROFILE_DELETE	1
#define CONFIRMMODE_SAVE_DELETE		2
#define CONFIRMMODE_SAVE_OVERWRITE	3
#define CONFIRMMODE_LOAD_GAME		4

#define SAVEIMAGE_UPDATE_TIME	50

bool bThisSave;
bool bIsGameProcessNow;
aref scrshot;
object emptyscrshot;

int g_nLablesFileID = -1;
int g_nInterfaceFileID = -1;
string currentProfile;

int g_nCurrentSaveIndex = 0;
int g_nFirstSaveIndex = -1;
int g_nSaveQuantity = 0;

object g_oSaveList[10];
object g_oSaveContainer;

int g_nConfirmMode;
string g_sConfirmReturnWindow;
bool isMainMenuChecker = false;

void InitInterface_BB(string iniName, bool isSave, bool isMainMenu)
{
	if( CheckAttribute(&PlayerProfile,"name") ) {
		PlayerProfile.old_name = PlayerProfile.name;
	}

	isMainMenuChecker = isMainMenu;
	bThisSave = isSave;
	bIsGameProcessNow = true;
	if(bThisSave) GameInterface.title = "titleSave";
	else GameInterface.title = "titleLoad";

	g_nLablesFileID = LanguageOpenFile("LocLables.txt");
	g_nInterfaceFileID = LanguageOpenFile("interface_strings.txt");

	SendMessage(&GameInterface,"ls",MSG_INTERFACE_INIT,iniName);

	if( bThisSave ) {
		SetSelectable("BTN_PROFILE",false);
		SendMessage( &GameInterface,"lsls",MSG_INTERFACE_MSG_TO_NODE,"BTN_SAVELOAD", 0, "Save" );
	}

	// by default first save is selected
	SendMessage( &GameInterface, "lsll", MSG_INTERFACE_MSG_TO_NODE, "SAVEIMG1", 5, true );
	SendMessage( &GameInterface, "lslll", MSG_INTERFACE_MSG_TO_NODE, "SAVENOTES", 3, 1, argb(255,255,255,255) );
	SendMessage( &GameInterface, "lslll", MSG_INTERFACE_MSG_TO_NODE, "SAVENOTES", 3, 2, argb(255,255,255,255) );
	SendMessage( &GameInterface, "lslll", MSG_INTERFACE_MSG_TO_NODE, "SAVENOTES", 3, 3, argb(255,255,255,255) );

	//FillProfileList();
	
	FillProfileList();
	FindScrshotClass();
	InitSaveObjList();

	SetEventHandler("exitCancel","ProcessCancelExit",0);
	SetEventHandler("eventBtnAction","procBtnAction",0);
	SetEventHandler("eventSelecterActivate","procSelecterActivate",0);
	SetEventHandler("eventSelecterMove","procSelecterMove",0);
	SetEventHandler("eventSaveClick","procSaveClick",0);
	SetEventHandler("evLoadOneSaveInfo","procLoadOneSaveInfo",0);
	SetEventHandler("eProfileBtnAction","procProfileBtnAction",0);
	SetEventHandler("eventConfirm","procConfirm",0);
	SetEventHandler("ScrollPosChange","ScrollPosChange",0);
	SetEventHandler("LoadProfile","LoadProfile",0);
	SetEventHandler("SaveLoad","SaveLoad",0);

	PostEvent( "evLoadOneSaveInfo",1 );

	if(isSave == false && isMainMenu == true && sti(PlayerProfile.profilesQuantity) > 1)
	{
		ProcChooseProfileFromList();
		//SetSelectable("PROFILE_WINDOW_BTN_EXIT", false);
	}
	else
	{
		SetCurrentProfile( GetCurrentProfile() );
	}


	//SetCurrentNode("PROFILE_WINDOW_BTN_CHOOSE");
	//SetSelectable("PROFILE_WINDOW_BTN_CHOOSE", false);
}

void SetCurrentProfile( string sProfileName )
{
	currentProfile = sProfileName;
	PlayerProfile.name = sProfileName;
	// fill save list
	DeleteAttribute( &g_oSaveContainer,"" );
	int nSaveNum = 0;
	string saveName;
	int nSaveSize;
	string attr;
	GameInterface.SavePath = "SAVE\" + currentProfile;
//	if( IsEntity(scrshot) ) {
//		scrshot.SavePath = "SAVE" + "\" + currentProfile;
//	}
	while( SendMessage(&GameInterface,"llee",MSG_INTERFACE_SAVE_FILE_FIND,nSaveNum,&saveName,&nSaveSize)!=0 )
	{
		attr = "s" + nSaveNum;
		g_oSaveContainer.(attr).savename = saveName;
		g_oSaveContainer.(attr).savesize = nSaveSize;
		nSaveNum++;
	}
	g_oSaveContainer.listsize = nSaveNum;
	g_nSaveQuantity = nSaveNum;
	g_nFirstSaveIndex = -1;
	FillSaveList( (g_nCurrentSaveIndex/10) * 10 );
	if( bThisSave ) {
		SelectSaveImage( g_nSaveQuantity );
	} else {
		SelectSaveImage( 0 );
	}
	SetClickable("SAVESCROLL",g_nSaveQuantity>10);
	// show profile name
	SendMessage( &GameInterface,"lslls",MSG_INTERFACE_MSG_TO_NODE, "SAVEINFO", 1, 1, "#"+XI_ConvertString("ProfileName")+": " + currentProfile );
	// read option from profile
	LoadGameOptions();
}

void ProcessCancelExit()
{
	DelEventHandler("ScrollPosChange","ScrollPosChange");
	DelEventHandler("eventConfirm","procConfirm");
	DelEventHandler("evLoadOneSaveInfo","procLoadOneSaveInfo");
	DelEventHandler("eProfileBtnAction","procProfileBtnAction");
	DelEventHandler("eventSaveClick","procSaveClick");
	DelEventHandler("eventSelecterActivate","procSelecterActivate");
	DelEventHandler("eventSelecterMove","procSelecterMove");
	DelEventHandler("eventBtnAction","procBtnAction");
	DelEventHandler("exitCancel","ProcessCancelExit");
	DelEventHandler("LoadProfile","LoadProfile");
	DelEventHandler("SaveLoad","SaveLoad");

	if( CheckAttribute(&PlayerProfile,"old_name") ) {
		PlayerProfile.name = PlayerProfile.old_name;
	}

	if( CheckAttribute(&InterfaceStates,"showGameMenuOnExit") && sti(InterfaceStates.showGameMenuOnExit) == true)
	{	
		IDoExit(RC_INTERFACE_LAUNCH_GAMEMENU);
		return;
	}

	IDoExit(RC_INTERFACE_SAVELOAD_EXIT);

	if( !CheckAttribute(&InterfaceStates,"InstantExit") || sti(InterfaceStates.InstantExit)==false )
	{
		ReturnToMainMenu();
	}
}

void IDoExit(int exitCode)
{
	DeleteAttribute(&PlayerProfile,"old_name");

	GameInterface.SavePath = "SAVE";
	LanguageCloseFile(g_nLablesFileID);
	LanguageCloseFile(g_nInterfaceFileID);

    interfaceResultCommand = exitCode;
	if( CheckAttribute(&InterfaceStates,"InstantExit") && sti(InterfaceStates.InstantExit)==true ) {
		EndCancelInterface(false);
	} else {
		EndCancelInterface(true);
	}
}

void FindScrshotClass()
{
	string layerName;
	if(bSeaActive && !bAbordageStarted) layerName = SEA_REALIZE;
	else layerName = "realize";

	if( !FindClass(&scrshot, "scrshoter") ) {
		makearef(scrshot,emptyscrshot);
	}
}

string GetCurrentProfile()
{
	if( CheckAttribute(&PlayerProfile, "name") && PlayerProfile.name!="" ) {
		return PlayerProfile.name;
	}
	trace("error (save_load.c) : PlayerProfile.name attribute not found.");

	object FolderList;
	XI_FindFoldersWithoutNetsave("SAVE"+ "\*", &FolderList);

	int num = GetAttributesNum(&FolderList);
	if( num > 0 )
		return GetAttributeValue( GetAttributeN(&FolderList, num - 1) );
	return "";
}

void FillProfileList()
{
	PlayerProfile.profilesQuantity = 0;

	object FolderList;
	XI_FindFoldersWithoutNetsave("SAVE\*", &FolderList);

	int num = GetAttributesNum(&FolderList);
	string folderName;

	int i = 0;
	string attr;
	for(i=0; i<num; i++) {
		attr = "profile_" + i;
		PlayerProfile.(attr) = GetAttributeValue( GetAttributeN(&FolderList,i) );
	}

	PlayerProfile.profilesQuantity = num;
}

void procSelecterActivate()
{
	SaveLoadCurrentIntoSlot();
}

void procBtnAction()
{
	int iComIndex = GetEventData();
	string sNodName = GetEventData();

	if( iComIndex == ACTION_RIGHTSTEP ) {
		if( sNodName == "BTN_PROFILE" ) {
			if( GetSelectable("BTN_SAVELOAD") ) {
				SetCurrentNode("BTN_SAVELOAD");
			} else {
				sNodName = "BTN_SAVELOAD";
			}
		}
		if( sNodName == "BTN_SAVELOAD" ) {
			if( GetSelectable("BTN_DELETE") ) {
				SetCurrentNode("BTN_DELETE");
			} else {
				sNodName = "BTN_DELETE";
			}
		}
		if( sNodName == "BTN_DELETE" ) {
			SetCurrentNode("BTN_EXIT");
		}
		return;
	}

	if( iComIndex == ACTION_LEFTSTEP ) {
		if( sNodName == "BTN_EXIT" ) {
			if( GetSelectable("BTN_DELETE") ) {
				SetCurrentNode("BTN_DELETE");
			} else {
				sNodName = "BTN_DELETE";
			}
		}
		if( sNodName == "BTN_DELETE" ) {
			if( GetSelectable("BTN_SAVELOAD") ) {
				SetCurrentNode("BTN_SAVELOAD");
			} else {
				sNodName = "BTN_SAVELOAD";
			}
		}
		if( sNodName == "BTN_SAVELOAD" && !bThisSave ) {
			SetCurrentNode("BTN_PROFILE");
		}
		return;
	}

	if( iComIndex == ACTION_ACTIVATE || iComIndex == ACTION_MOUSECLICK ) {
		if( sNodName == "BTN_PROFILE" ) {
			ProcChooseProfileFromList();
		}
		if( sNodName == "BTN_SAVELOAD" ) {
			SaveLoadCurrentIntoSlot();
		}
		if( sNodName == "BTN_DELETE" ) {
			DoConfirm(CONFIRMMODE_SAVE_DELETE);
		}
	}
}

void SaveLoadCurrentIntoSlot()
{
	if( bThisSave ) {
		// запись
		if( GetCurSaveName()=="" ) { // не перезапись старого файла
			ProcessSave();
		} else {
			DoConfirm(CONFIRMMODE_SAVE_OVERWRITE);
		}
	} else {
		// чтение
		if( g_nCurrentSaveIndex<0 || g_nCurrentSaveIndex>=g_nSaveQuantity ) { // нет такой €чейки с записью
			return;
		}
		if( bIsGameProcessNow ) { // в данный момент уже идет игра?
			DoConfirm(CONFIRMMODE_LOAD_GAME);
		} else {
			ProcessLoad();
		}
	}
}

void ProcChooseProfileFromList()
{
	// disable all windows
	XI_WindowDisable( "MAIN_WINDOW", true );
	// enable profile window
	XI_WindowDisable( "PROFILE_WINDOW", false );
	XI_WindowShow( "PROFILE_WINDOW", true );
	// set current node to profile list
	FillProfileListIntoTable();
	SetCurrentNode( "PROFILE_WINDOW_LIST" );
}

void FillProfileListIntoTable()
{
	// Fill Profile List
	int n, q, nSel;
	string src_attr, dst_attr, sCurProfile;
	q = sti(PlayerProfile.profilesQuantity);
	nSel = 1;
	if( CheckAttribute(&PlayerProfile,"name") ) {
		sCurProfile = PlayerProfile.name;
	} else {
		sCurProfile = "";
	}
	DeleteAttribute( &GameInterface, "profile_window_list" );
	for( n=0; n<q; n++ )
	{
		src_attr = "profile_" + n;
		dst_attr = "tr" + (n+1);
		GameInterface.profile_window_list.(dst_attr).td1.str = PlayerProfile.(src_attr);
		if( PlayerProfile.(src_attr) == sCurProfile ) {nSel = n+1;}
	}
	GameInterface.profile_window_list.select = nSel;
	SendMessage( &GameInterface, "lsl", MSG_INTERFACE_MSG_TO_NODE, "PROFILE_WINDOW_LIST", 0 );
}

void ProcExitProfile()
{
	// disable all windows
	XI_WindowShow( "PROFILE_WINDOW", false );
	// enable main window
	XI_WindowDisable( "MAIN_WINDOW", false );
	// set current node to button for start profile choosing
	SetCurrentNode( "BTN_PROFILE" );
}

void ProcChooseProfile()
{
	string attr = "profile_" + (sti(GameInterface.profile_window_list.select)-1);
	if( CheckAttribute(&PlayerProfile,attr) ) {
		SetCurrentProfile( PlayerProfile.(attr) );
	}

	ProcExitProfile();
}

void ProcDeleteProfile()
{
	int nProfileIdx = sti(GameInterface.profile_window_list.select) - 1;
	// deleting process:
	string attr = "profile_" + nProfileIdx;
	string sThisProfile = PlayerProfile.(attr);
	DeleteProfile( PlayerProfile.(attr) );

	// shifting profile list
	int nProfilesQ = sti(PlayerProfile.profilesQuantity);
	nProfilesQ--;
	string attrSrc, attrDst;
	for( int n=nProfileIdx; n<nProfilesQ; n++ )
	{
		attrDst = "profile_" + n;
		attrSrc = "profile_" + (n+1);
		PlayerProfile.(attrDst) = PlayerProfile.(attrSrc);
	}
	DeleteAttribute( &PlayerProfile, "profile_"+nProfilesQ );
	PlayerProfile.profilesQuantity = nProfilesQ;
	FillProfileListIntoTable();

	if( sThisProfile == currentProfile )
	{
		SetCurrentProfile("");
	}
}

void DeleteProfile(string profileName)
{
	string oldpath = GameInterface.SavePath;
	GameInterface.SavePath = "SAVE\"+profileName;
	// deleting all files from profile folder
	int nSaveNum= 0;
	string saveName;
	int nSaveSize;
	while( SendMessage(&GameInterface,"llee",MSG_INTERFACE_SAVE_FILE_FIND,nSaveNum,&saveName,&nSaveSize)!=0 )
	{
		nSaveNum++;
		SendMessage(&GameInterface,"ls",MSG_INTERFACE_DELETE_SAVE_FILE,saveName);
	}
	XI_DeleteFolder( GameInterface.SavePath );
	GameInterface.SavePath = oldpath;
}

void procSelecterMove()
{
	int iComIndex = GetEventData();

	int nLeft,nTop,nRight,nBottom;
	bool bMakeMove = false;

	switch( iComIndex )
	{
	case ACTION_RIGHTSTEP:
		bMakeMove = GetMoveToOtherSave( g_nCurrentSaveIndex+1, &nLeft,&nTop,&nRight,&nBottom );
	break;
	case ACTION_LEFTSTEP:
		bMakeMove = GetMoveToOtherSave( g_nCurrentSaveIndex-1, &nLeft,&nTop,&nRight,&nBottom );
	break;
	case ACTION_UPSTEP:
		bMakeMove = GetMoveToOtherSave( g_nCurrentSaveIndex-5, &nLeft,&nTop,&nRight,&nBottom );
	break;
	case ACTION_DOWNSTEP:
		bMakeMove = GetMoveToOtherSave( g_nCurrentSaveIndex+5, &nLeft,&nTop,&nRight,&nBottom );
	break;
	}

	if( bMakeMove ) {
		SendMessage( &GameInterface, "lslllll", MSG_INTERFACE_MSG_TO_NODE,"SAVE_SELECTER",0, nLeft,nTop,nRight,nBottom );
	}
}

void SetSelecting(int nSlot,bool bSelect)
{
	string sNodeName;
	int nColor;
	sNodeName = "SAVEIMG"+(nSlot+1);
	if( bSelect ) {
		nColor = argb(255,255,255,255);
	} else {
		nColor = argb(255,148,148,148);
	}
	SendMessage( &GameInterface, "lsll", MSG_INTERFACE_MSG_TO_NODE, sNodeName, 5, bSelect );
	SendMessage( &GameInterface, "lslll", MSG_INTERFACE_MSG_TO_NODE, "SAVENOTES", 3, nSlot*3+1, nColor );
	SendMessage( &GameInterface, "lslll", MSG_INTERFACE_MSG_TO_NODE, "SAVENOTES", 3, nSlot*3+2, nColor );
	SendMessage( &GameInterface, "lslll", MSG_INTERFACE_MSG_TO_NODE, "SAVENOTES", 3, nSlot*3+3, nColor );
}

bool GetMoveToOtherSave( int nNewSaveIndex, ref rLeft, ref rTop, ref rRight, ref rBottom )
{
	if( nNewSaveIndex < 0 ) return false;
	//if( nNewSaveIndex > 9 ) return false;
	if( g_nCurrentSaveIndex == nNewSaveIndex ) return false;

	int nMaxQ = (g_nSaveQuantity + 4)/ 5 * 5 + 5;
	if( nNewSaveIndex > nMaxQ ) return false;

	int nOldIdx = g_nCurrentSaveIndex - g_nFirstSaveIndex;
	int nNewIdx = nNewSaveIndex - g_nFirstSaveIndex;

	if( nNewIdx < 0 ) {
		if( g_nFirstSaveIndex > 0 ) {
			FillSaveList(g_nFirstSaveIndex - 5);
		}
		nNewIdx = nNewSaveIndex - g_nFirstSaveIndex;
	}
	if( nNewIdx > 9 ) {
		FillSaveList(g_nFirstSaveIndex + 5);
		nNewIdx = nNewSaveIndex - g_nFirstSaveIndex;
	}

	string sNodName = "SAVEIMG"+(nNewIdx+1);
	if( nNewIdx>0 && GetSelectable(sNodName)==false ) {
		nNewIdx = g_nCurrentSaveIndex - g_nFirstSaveIndex;
		nNewSaveIndex = g_nCurrentSaveIndex;
		if( nNewIdx == nOldIdx ) {
			ReloadSaveInfo();
			return false; // нельз€ перейти на новый элемент (на первый всегда можно)
		}
	}

	// старое немигает новое подмигивает & старый текст серый новый выделенный - светлый
	if( g_nCurrentSaveIndex>=0 && nOldIdx>=0 && nOldIdx<10 ) {
		SetSelecting(nOldIdx,false);
	}
	SetSelecting(nNewIdx,true);

	g_nCurrentSaveIndex = nNewSaveIndex;
	ReloadSaveInfo();

	int nLeft = 0;
	int nTop = 0;
	int nRight = 0;
	int nBottom = 0;

	switch( nNewIdx )
	{
	case 0: nLeft=32; nTop=60; nRight=160; nBottom=188; break;
	case 1: nLeft=180; nTop=60; nRight=308; nBottom=188; break;
	case 2: nLeft=328; nTop=60; nRight=456; nBottom=188; break;
	case 3: nLeft=476; nTop=60; nRight=604; nBottom=188; break;
	case 4: nLeft=624; nTop=60; nRight=752; nBottom=188; break;
	case 5: nLeft=32; nTop=240; nRight=160; nBottom=368; break;
	case 6: nLeft=180; nTop=240; nRight=308; nBottom=368; break;
	case 7: nLeft=328; nTop=240; nRight=456; nBottom=368; break;
	case 8: nLeft=476; nTop=240; nRight=604; nBottom=368; break;
	case 9: nLeft=624; nTop=240; nRight=752; nBottom=368; break;
	}

	rLeft = nLeft;
	rTop = nTop;
	rRight = nRight;
	rBottom = nBottom;
	return true;
}

void procSaveClick()
{
	int iComIdx = GetEventData();
	string sNodName = GetEventData();

	if( sNodName == "SAVEIMG1" ) { SelectSaveImage( g_nFirstSaveIndex + 0 ); return; }
	if( sNodName == "SAVEIMG2" ) { SelectSaveImage( g_nFirstSaveIndex + 1 ); return; }
	if( sNodName == "SAVEIMG3" ) { SelectSaveImage( g_nFirstSaveIndex + 2 ); return; }
	if( sNodName == "SAVEIMG4" ) { SelectSaveImage( g_nFirstSaveIndex + 3 ); return; }
	if( sNodName == "SAVEIMG5" ) { SelectSaveImage( g_nFirstSaveIndex + 4 ); return; }

	if( sNodName == "SAVEIMG6" ) { SelectSaveImage( g_nFirstSaveIndex + 5 ); return; }
	if( sNodName == "SAVEIMG7" ) { SelectSaveImage( g_nFirstSaveIndex + 6 ); return; }
	if( sNodName == "SAVEIMG8" ) { SelectSaveImage( g_nFirstSaveIndex + 7 ); return; }
	if( sNodName == "SAVEIMG9" ) { SelectSaveImage( g_nFirstSaveIndex + 8 ); return; }
	if( sNodName == "SAVEIMG10" ) { SelectSaveImage( g_nFirstSaveIndex + 9 ); return; }
}

void SelectSaveImage( int nSaveIndex )
{
	int nLeft,nTop,nRight,nBottom;
	bool bMakeMove = GetMoveToOtherSave( nSaveIndex, &nLeft,&nTop,&nRight,&nBottom );
	if( bMakeMove ) {
		SendMessage( &GameInterface, "lslllll", MSG_INTERFACE_MSG_TO_NODE,"SAVE_SELECTER",0, nLeft,nTop,nRight,nBottom );
	}
	SetCurrentNode( "SAVE_SELECTER" );
}

void FillSaveList(int nFirstSaveIndex)
{
	if( nFirstSaveIndex<0 ) nFirstSaveIndex = 0;
	nFirstSaveIndex = nFirstSaveIndex / 5;
	nFirstSaveIndex = nFirstSaveIndex * 5;

	bool bNoRebuildSaveList = (g_nFirstSaveIndex>=0);
	int nDelta = nFirstSaveIndex - g_nFirstSaveIndex;
	g_nFirstSaveIndex = nFirstSaveIndex;

	if( bNoRebuildSaveList && (nDelta==5) ) {
		// перенос нижней линии записей вверх
		MoveSaveLine(1,0);
		// установка нижней линии в ожидание
		FillSaveLine(1,nFirstSaveIndex+5);
		ReloadSaveInfo();
		return;
	}
	if( bNoRebuildSaveList && (nDelta==-5) ) {
		// перенос верхней линии записей вниз
		MoveSaveLine(0,1);
		// установка верхней линии в ожидание
		FillSaveLine(0,nFirstSaveIndex);
		ReloadSaveInfo();
		return;
	}
	// установка всех линий в ожидание
	FillSaveLine(0,nFirstSaveIndex);
	FillSaveLine(1,nFirstSaveIndex+5);
	ReloadSaveInfo();
}

void FillSaveLine(int nLine, int nBegIndex)
{
	int nBeg = nLine * 5;
	int i,n;
	string attr;
	g_nSaveQuantity;
	for(i=0; i<5; i++) {
		n = nBegIndex + i;
		attr = "s" + n;
		if( CheckAttribute(&g_oSaveContainer,attr) ) {
			LoadInfo(nBeg+i, n, g_oSaveContainer.(attr).savename);
		} else {
			FillEmptySaveSlot(nBeg+i);
		}
	}
}

void FillEmptySaveSlot(int nSlot)
{
	ClearSaveInfoByIndex( nSlot );
	g_oSaveList[nSlot].saveidx = -1;
	g_oSaveList[nSlot].savefile = "";
	g_oSaveList[nSlot].loaded = 1;
	ShowDataForSave(nSlot,"empty",0,"");
}

void ShowDataForSave(int nSlot, string picname, int picpointer, string strdata)
{
	string nodname = "SAVEIMG" + (nSlot+1);
	bool bClickable = bThisSave;
	if( picname!="" ) {
		if( picname == "empty" ) {
			SendMessage( &GameInterface, "lslls", MSG_INTERFACE_MSG_TO_NODE, nodname, 2, 0,"interfaces\SaveIcons\SaveEmpty.tga" );
		}
		if( picname == "loading" ) {
			SendMessage( &GameInterface, "lslls", MSG_INTERFACE_MSG_TO_NODE, nodname, 2, 0,"interfaces\SaveIcons\SaveLoading.tga" );
		}
		if( picname == "corrupted" ) {
			SendMessage( &GameInterface, "lslls", MSG_INTERFACE_MSG_TO_NODE, nodname, 2, 0,"interfaces\SaveIcons\SaveCorrupted.tga" );
		}
	}
	if( picpointer!=0 ) {
		SendMessage( &GameInterface, "lsll", MSG_INTERFACE_MSG_TO_NODE, nodname, 7, picpointer );
		bClickable = true;
	}
	SetSelectable( nodname, bClickable );

	string fileSystemTime = "";
	string fileSystemDate = "";
	string sSystemTimeString = "No Time";
	if (CheckAttribute(&g_oSaveList[nSlot], "savefile") && g_oSaveList[nSlot].savefile != "" ) //fix boal
	{
		SendMessage(&GameInterface,"lsee",MSG_INTERFACE_GETTIME, "SAVE\"+currentProfile+"\"+g_oSaveList[nSlot].savefile, &fileSystemTime, &fileSystemDate);
		sSystemTimeString = GetSystemTimeString( fileSystemTime, fileSystemDate );
	}
	SendMessage( &GameInterface,"lslls",MSG_INTERFACE_MSG_TO_NODE, "SAVENOTES", 1, nSlot*3+3, "#"+sSystemTimeString );

	if( strdata != "" )
	{
		string facestr, locName, timeStr, language, playtime;
		if( ParseSaveData(strdata, &facestr, &locName, &timeStr, &language, &playtime) ) {
			SendMessage( &GameInterface,"lslls",MSG_INTERFACE_MSG_TO_NODE, "SAVENOTES", 1, nSlot*3+1, "#"+locName );
			SendMessage( &GameInterface,"lslls",MSG_INTERFACE_MSG_TO_NODE, "SAVENOTES", 1, nSlot*3+2, "#"+timeStr );
			g_oSaveList[nSlot].faceinfo = facestr;
			g_oSaveList[nSlot].playtime = playtime;
		} else {
			SendMessage( &GameInterface,"lslls",MSG_INTERFACE_MSG_TO_NODE, "SAVENOTES", 1, nSlot*3+1, "#Unknown" );
			SendMessage( &GameInterface,"lslls",MSG_INTERFACE_MSG_TO_NODE, "SAVENOTES", 1, nSlot*3+2, "#No Time" );
			g_oSaveList[nSlot].faceinfo = "";
			g_oSaveList[nSlot].playtime = "";
		}
	} else {
		SendMessage( &GameInterface,"lslls",MSG_INTERFACE_MSG_TO_NODE, "SAVENOTES", 1, nSlot*3+1, "#Unknown" );
		SendMessage( &GameInterface,"lslls",MSG_INTERFACE_MSG_TO_NODE, "SAVENOTES", 1, nSlot*3+2, "#No Time" );
		g_oSaveList[nSlot].faceinfo = "";
		g_oSaveList[nSlot].playtime = "";
	}

	if( (g_nCurrentSaveIndex-g_nFirstSaveIndex) == nSlot ) {
		ReloadSaveInfo();
	}
}

string GetSystemTimeString( string systemTime, string systemDate )
{
	return systemTime + " " + systemDate;
}

void MoveSaveLine(int nSrcLine, int nDstLine)
{
	int nSrc = nSrcLine * 5;
	int nDst = nDstLine * 5;
	for( int i=0; i<5; i++ )
	{
		MoveSaveInfo(nSrc+i,nDst+i);
	}
}

void InitSaveObjList()
{
	int i;
	for(i=0; i<10; i++)
	{
		g_oSaveList[i].saveidx = -1;
	}
}

int FindSaveInfoIndex(int iSaveIdx)
{
	int i;
	for(i=0; i<10; i++)
	{
		if( g_oSaveList[i].saveidx == iSaveIdx ) {
			return i;
		}
	}
	return -1;
}

void ClearSaveInfoByIndex(int i)
{
	if( g_oSaveList[i].saveidx == -1 ) {return;}
	DeleteAttribute( &g_oSaveList[i], "" );
	g_oSaveList[i].saveidx = -1;
}

void MoveSaveInfo(int nSrc, int nDst)
{
	CopyAttributes( &g_oSaveList[nDst], &g_oSaveList[nSrc] );
	DeleteAttribute( &g_oSaveList[nSrc], "" );
	g_oSaveList[nSrc].saveidx = -1;

	// copy info from src control to dst control
	string sDstImgNod = "SAVEIMG"+(nDst+1);
	string sSrcImgNod = "SAVEIMG"+(nSrc+1);
	SetSelectable( sDstImgNod, GetSelectable(sSrcImgNod) );
	SendMessage( &GameInterface, "lsls", MSG_INTERFACE_MSG_TO_NODE, sDstImgNod, 8, sSrcImgNod );
	SendMessage( &GameInterface, "lslll", MSG_INTERFACE_MSG_TO_NODE, "SAVENOTES", 2, nDst*3+1, nSrc*3+1 );
	SendMessage( &GameInterface, "lslll", MSG_INTERFACE_MSG_TO_NODE, "SAVENOTES", 2, nDst*3+2, nSrc*3+2 );
	SendMessage( &GameInterface, "lslll", MSG_INTERFACE_MSG_TO_NODE, "SAVENOTES", 2, nDst*3+3, nSrc*3+3 );
	// set src control to empty
	ShowDataForSave( nSrc, "empty", 0, "" );
}

void LoadInfo(int nInfoIdx, int nSaveIdx, string sSaveName)
{
	ClearSaveInfoByIndex( nInfoIdx );
	g_oSaveList[nInfoIdx].saveidx = nSaveIdx;
	g_oSaveList[nInfoIdx].savefile = sSaveName;
	g_oSaveList[nInfoIdx].loaded = 0;
	ShowDataForSave(nInfoIdx,"loading",0,"");
}

void procLoadOneSaveInfo()
{
	int i;
	int pTex;
	string strdata;
	bool bYesScrShoter = IsEntity(scrshot);
	for( i=0; i<10; i++ )
	{
		if( g_oSaveList[i].saveidx != "-1" )
		{
			if( !CheckAttribute(&g_oSaveList[i],"loaded") || g_oSaveList[i].loaded=="0" )
			{
				g_oSaveList[i].loaded = 1;
				strdata = "";
				if( bYesScrShoter ) {
					pTex = SendMessage( scrshot, "lsse", MSG_SCRSHOT_READ, "SAVE\"+currentProfile, g_oSaveList[i].savefile, &strdata );
				}
				if( strdata == "" ) {
					ShowDataForSave(i,"corrupted",0,"");
				} else {
					ShowDataForSave(i,"",pTex,strdata);
				}
				if( bYesScrShoter ) {
					SendMessage( scrshot, "ls", MSG_SCRSHOT_RELEASE, g_oSaveList[i].savefile );
				}
				// только один сейф файл за раз
				break;
			}
		}
	}
	PostEvent( "evLoadOneSaveInfo",SAVEIMAGE_UPDATE_TIME );
}

void procProfileBtnAction()
{
	int iComIndex = GetEventData();
	string sNodName = GetEventData();

	if( iComIndex == ACTION_ACTIVATE || iComIndex == ACTION_MOUSECLICK ) {
		if( sNodName == "PROFILE_WINDOW_BTN_EXIT" ) {
			if(isMainMenuChecker == true && g_nSaveQuantity == 0)
			{
				SetCurrentProfile("");
				ProcExitProfile();
			}
			else
			{
				ProcExitProfile();
			}
		}
		if( sNodName == "PROFILE_WINDOW_BTN_CHOOSE" ) {
			ProcChooseProfile();
		}
		if( sNodName == "PROFILE_WINDOW_BTN_DELETE" ) {
			DoConfirm( CONFIRMMODE_PROFILE_DELETE );
		}
		return;
	}
}

bool ParseSaveData(string fullSaveData, ref facestr, ref locationStr, ref timeStr, ref languageID, ref playtime)
{
	string lastStr;
	if( !GetNextSubStr(fullSaveData, locationStr, &lastStr, "@") ) return false;
	if( !GetNextSubStr(lastStr, facestr, &lastStr, "@") ) return false;
	if( !GetNextSubStr(lastStr, timeStr, &lastStr, "@") ) return false;
	if( !GetNextSubStr(lastStr, playtime, &lastStr, "@") ) return false;
	GetNextSubStr(lastStr, languageID, &lastStr, "@");
	return true;
}

bool GetNextSubStr(string inStr, ref outStr, ref lastStr, string separator)
{
	if(inStr=="")
	{
		outStr="";
		lastStr="";
		return false;
	}
	int strSize = strlen(inStr)-1;
	if(strSize<=0)
	{
		outStr="";
		lastStr="";
		return false;
	}
	int sympos = findsubstr(inStr,separator,0);
	if(sympos==-1)
	{
		outStr = inStr;
		lastStr = "";
		return true;
	}
	if(sympos>0)	outStr = strcut(inStr,0,sympos-1);
	else	outStr = "";
	if(strSize>sympos+1) lastStr = strcut(inStr,sympos+1,strSize);
	else lastStr = "";
	return true;
}

void ProcessDeleteSaveFile()
{
	string curSave = GetCurSaveName();
	if( curSave=="" ) { return; }
	SendMessage(&GameInterface,"ls",MSG_INTERFACE_DELETE_SAVE_FILE,curSave);

	int n, nDst, nSrc;
	string attrDst,attrSrc;
	aref arSrc,arDst;
	g_nSaveQuantity--;
	for( n=g_nCurrentSaveIndex; n<=g_nSaveQuantity; n++ )
	{
		// dst save info
		attrDst = "s" + n;
		makearef(arDst,g_oSaveContainer.(attrDst));
		// src save info (or no info and clear dst)
		if( n<g_nSaveQuantity ) {
			attrSrc = "s" + (n+1);
			makearef(arSrc,g_oSaveContainer.(attrSrc));
			CopyAttributes( arDst, arSrc );
		} else {
			DeleteAttribute( &g_oSaveContainer, attrDst );
		}

		nDst = n - g_nFirstSaveIndex;
		nSrc = nDst + 1;
		if( nDst<10 ) {
			if( nSrc<10 ) {
				MoveSaveInfo(nSrc,nDst);
			}
			else
			{
				if( CheckAttribute(&g_oSaveContainer,attrDst) ) {
					LoadInfo(nDst, n, g_oSaveContainer.(attrDst).savename);
				} else {
					FillEmptySaveSlot(nDst);
				}
			}
		}
	}
	g_oSaveContainer.listsize = g_nSaveQuantity;
	SetClickable("SAVESCROLL",g_nSaveQuantity>10);
	// при интерфейсе загрузки выделение об€зательно оставл€ем на реальном сейве
	attrDst = "s" + g_nCurrentSaveIndex;
	if( !CheckAttribute(&g_oSaveContainer,attrDst) ) {
		if( g_oSaveContainer > g_nFirstSaveIndex ) {
			SelectSaveImage( g_nCurrentSaveIndex - 1 );
		}
	}

	ReloadSaveInfo();
}

void ProcessLoad()
{
	string sCurSave = GetCurSaveName();
	PlayerProfile.name = currentProfile;

	IDoExit(RC_INTERFACE_SAVELOAD_EXIT);
	ResetSound();

	SetEventHandler("evntLoad","LoadGame",1);
	PostEvent( "evntLoad", 0, "s", "SAVE\"+PlayerProfile.name+"\"+sCurSave );
	Event("evntPreLoad");
}

void ProcessSave()
{
	string curSave = GetCurSaveName();
	if( curSave!="" ) {
		SendMessage(&GameInterface,"ls",MSG_INTERFACE_DELETE_SAVE_FILE,curSave);
	}
	curSave = GetNewSaveName();
	string sSaveDescriber = GetSaveDataString();

	SetEventHandler("evntSave","SaveGame",1);
	PostEvent("evntSave",0,"ss", "SAVE\"+currentProfile+"\"+curSave, sSaveDescriber);
	IDoExit(RC_INTERFACE_SAVELOAD_EXIT);
}

string GetNewSaveName()
{
	string curLocName = GetCurLocationName();
	string saveName;
	saveName = GetClampedSaveName(currentProfile+" "+curLocName,0);
	int idx = 0;
	while( SendMessage(&GameInterface,"ls", MSG_INTERFACE_NEW_SAVE_FILE_NAME, saveName) == 1)
	{
		saveName = GetClampedSaveName(currentProfile+" "+curLocName,idx+1);
		idx++;
	}
	return saveName;
}

string GetClampedSaveName(string sInName, int nNumber)
{
	if(nNumber>0) {return sInName + " " + nNumber;}
	else {return sInName;}
}

string GetCurSaveName()
{
	if( g_nCurrentSaveIndex<0 || g_nCurrentSaveIndex>=g_nSaveQuantity ) return "";
	string attr = "s" + g_nCurrentSaveIndex;
	if( CheckAttribute(&g_oSaveContainer,attr+".savename") ) {
		return g_oSaveContainer.(attr).savename;
	}
	return "";
}

void DoConfirm( int nConfirmMode )
{
	g_sConfirmReturnWindow = "MAIN_WINDOW";
	if( XI_IsWindowEnable("PROFILE_WINDOW") ) {
		g_sConfirmReturnWindow = "PROFILE_WINDOW";
	}
	XI_WindowDisable( g_sConfirmReturnWindow, true );
	// enable confirm window
	XI_WindowDisable( "CONFIRM_WINDOW", false );
	XI_WindowShow( "CONFIRM_WINDOW", true );
	SetCurrentNode( "CONFIRM_YES" );

	g_nConfirmMode = nConfirmMode;
	switch( nConfirmMode )
	{
	case CONFIRMMODE_PROFILE_DELETE:
		SetFormatedText( "CONFIRM_TEXT", LanguageConvertString(g_nInterfaceFileID,"Delete profile confirm") );
		break;
	case CONFIRMMODE_SAVE_DELETE:
		SetFormatedText( "CONFIRM_TEXT", LanguageConvertString(g_nInterfaceFileID,"Delete savefile confirm") );
		break;
	case CONFIRMMODE_SAVE_OVERWRITE:
		SetFormatedText( "CONFIRM_TEXT", LanguageConvertString(g_nInterfaceFileID,"Overwrite savefile confirm") );
		break;
	case CONFIRMMODE_LOAD_GAME:
		SetFormatedText( "CONFIRM_TEXT", LanguageConvertString(g_nInterfaceFileID,"Load game confirm") );
		break;
	}
	SendMessage( &GameInterface, "lsl", MSG_INTERFACE_MSG_TO_NODE, "CONFIRM_TEXT", 5 ); // центрируем по вертикали
}

void procConfirm()
{
	int iComIndex = GetEventData();
	string sNodName = GetEventData();

	if( iComIndex==ACTION_ACTIVATE || iComIndex==ACTION_MOUSECLICK ) {
		if( sNodName=="CONFIRM_YES" ) {
			UndoConfirm(true);
		} else {
			UndoConfirm(false);
		}
	}
	if( iComIndex==ACTION_DEACTIVATE ) {
		UndoConfirm(false);
	}
}

void UndoConfirm(bool bPositiveChoose)
{
	// disable confirm
	XI_WindowDisable( "CONFIRM_WINDOW", true );
	XI_WindowShow( "CONFIRM_WINDOW", false );
	// enable window where confirm was started
	XI_WindowDisable( g_sConfirmReturnWindow, false );

	int nTmp = g_nConfirmMode;
	if( bPositiveChoose ) {
		switch( nTmp )
		{
		case CONFIRMMODE_PROFILE_DELETE: ProcDeleteProfile(); break;
		case CONFIRMMODE_SAVE_DELETE: ProcessDeleteSaveFile(); break;
		case CONFIRMMODE_SAVE_OVERWRITE: ProcessSave(); break;
		case CONFIRMMODE_LOAD_GAME: ProcessLoad(); break;
		}
	}
	// возвращаем управление правильной контролке
	switch( nTmp )
	{
	case CONFIRMMODE_PROFILE_DELETE: SetCurrentNode("PROFILE_WINDOW_BTN_DELETE"); break;
	case CONFIRMMODE_SAVE_DELETE:
		if(GetSelectable("BTN_DELETE")) {SetCurrentNode("BTN_DELETE");}
		else {SetCurrentNode("SAVE_SELECTER");}
	break;
	case CONFIRMMODE_SAVE_OVERWRITE: SetCurrentNode("BTN_SAVELOAD"); break;
	case CONFIRMMODE_LOAD_GAME: SetCurrentNode("BTN_SAVELOAD"); break;
	}
}

void ShowCharacterFace(int iPlace, string sFace)
{
	string nodname = "OFFICER" + (iPlace+1);
	string facegroup = sFace;
	if( sFace=="" || sFace=="*" ) {
		facegroup = "face128_fighter";
		switch( iPlace ) {
			case 4: facegroup = "face128_navigator"; break;
			case 5: facegroup = "face128_boatswain"; break;
			case 6: facegroup = "face128_cannoner"; break;
			case 7: facegroup = "face128_doctor"; break;
			case 8: facegroup = "face128_treasurer"; break;
			case 9: facegroup = "face128_carpenter"; break;
		}
	}
	SendMessage( &GameInterface, "lslss", MSG_INTERFACE_MSG_TO_NODE, nodname, 6, facegroup, "face" );
}

void ShowFaceInfo( string facestr )
{
	string sstr = facestr;
	string sdat;
	for( int i=0; i<10; i++ )
	{
		sdat = "";
		if( GetNextSubStr( sstr, &sdat, &sstr, "," ) ) {
			ShowCharacterFace(i,sdat);
		} else {
			ShowCharacterFace(i,"*");
		}
	}
}

void ReloadSaveInfo()
{
	int nSlot = g_nCurrentSaveIndex - g_nFirstSaveIndex;
	string info = "";
	string playtime = "#";
	if( nSlot>=0 && nSlot<10 && CheckAttribute(&g_oSaveList[nSlot],"faceinfo") ) {
		info = g_oSaveList[nSlot].faceinfo;
		if( g_oSaveList[nSlot].playtime != "" ) {
			playtime = "#" + XI_ConvertString("Play Time") + ": " + g_oSaveList[nSlot].playtime;
		}
	}
	ShowFaceInfo( info );
	SendMessage( &GameInterface,"lslls",MSG_INTERFACE_MSG_TO_NODE, "SAVEINFO", 1, 2, playtime );

	if( info == "" ) {
		SetSelectable( "BTN_SAVELOAD", bThisSave );
		SetSelectable( "BTN_DELETE", false );
	} else {
		SetSelectable( "BTN_SAVELOAD", true );
		SetSelectable( "BTN_DELETE", true );
	}
}

void ScrollPosChange()
{
	float fPos = GetEventData();
	string sNod = GetEventData();
	if( sNod == "SAVESCROLL" ) {
		if( g_nCurrentSaveIndex>=0 ) {
			int nOldIdx = g_nCurrentSaveIndex - g_nFirstSaveIndex;
			SetSelecting(nOldIdx,false);
		}
		int nLineQ = g_nSaveQuantity / 5;
		int nLine = makeint( makefloat(nLineQ) * fPos );
		FillSaveList(nLine * 5);
		// старое немигает новое подмигивает & старый текст серый новый выделенный - светлый
		SetSelecting(0,true);
		SelectSaveImage(g_nFirstSaveIndex);
	}
}

void LoadProfile()
{
	ProcChooseProfile();
}

void SaveLoad()
{
	int iComIdx = GetEventData();
	string sNodName = GetEventData();

	if( sNodName == "SAVEIMG1" ) { SelectSaveImage( g_nFirstSaveIndex + 0 ) }
	if( sNodName == "SAVEIMG2" ) { SelectSaveImage( g_nFirstSaveIndex + 1 ) }
	if( sNodName == "SAVEIMG3" ) { SelectSaveImage( g_nFirstSaveIndex + 2 ) }
	if( sNodName == "SAVEIMG4" ) { SelectSaveImage( g_nFirstSaveIndex + 3 ) }
	if( sNodName == "SAVEIMG5" ) { SelectSaveImage( g_nFirstSaveIndex + 4 ) }

	if( sNodName == "SAVEIMG6" ) { SelectSaveImage( g_nFirstSaveIndex + 5 ) }
	if( sNodName == "SAVEIMG7" ) { SelectSaveImage( g_nFirstSaveIndex + 6 ) }
	if( sNodName == "SAVEIMG8" ) { SelectSaveImage( g_nFirstSaveIndex + 7 ) }
	if( sNodName == "SAVEIMG9" ) { SelectSaveImage( g_nFirstSaveIndex + 8 ) }
	if( sNodName == "SAVEIMG10" ) { SelectSaveImage( g_nFirstSaveIndex + 9 ) }

	SaveLoadCurrentIntoSlot();
}