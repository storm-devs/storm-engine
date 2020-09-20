int g_LocLngFileID = -1;

object objLandInterface;
object objFastReloadTable;
object objTownStateTable;
int g_intRetVal;

bool bDisableFastReload = false;

#define	BLI_UPDATE_PERIOD 400
#event_handler("evntBattleCommandSound","procBattleCommandSound");

bool bLandInterfaceStart = false;

void procBattleCommandSound()
{
	string comName = GetEventData();
	if(comName=="deactivate")
	{
		PlaySound("interface\ok.wav");
	}
	if(comName=="activate")
	{
		PlaySound("interface\ok.wav");
	}
	if(comName=="right" || comName=="left")
	{
		PlaySound("interface\ok.wav");
	}
}

void InitBattleLandInterface()
{
	//if( IsEntity(&objLandInterface) ) {
	//	DeleteClass(&objLandInterface);
	//}
	//if(bLandInterfaceStart) {
	if( IsEntity(&objLandInterface) ) {
		EndBattleLandInterface();
		//DeleteEntity(&objLandInterface);
		//return;
	}
	bLandInterfaceStart = false;
	SetEventHandler(EVENT_LOCATION_LOAD,"StartBattleLandInterface",0);
	SetEventHandler(EVENT_LOCATION_UNLOAD,"EndBattleLandInterface",0);
	LoadLIStates();
}

void BLI_EnableShow()
{
	SetLandInterfaceShow( sti(InterfaceStates.BattleShow.Command) );
}

void BLI_DisableShow()
{
	SetLandInterfaceShow(false);
}

void SetLandInterfaceShow(bool commandosShow)
{
	SendMessage(&objLandInterface,"ll",MSG_BATTLE_LAND_SET_SHOW,commandosShow);
}

void StartBattleLandInterface()
{
	if(bLandInterfaceStart)
	{
		//BLI_SetObjectData();
		return;
	}
	bLandInterfaceStart = true;

	BLI_SetObjectData();
	BLI_SetMessageParameters();
	CreateEntity(&objLandInterface,"battle_land_interface");
	LayerAddObject("execute",&objLandInterface,-1);
	LayerAddObject("realize",&objLandInterface,-1);

	SetEventHandler(EVENT_CHANGE_OFFICERS,"BLI_UpdateOfficers",0);
	SetEventHandler(EVENT_DIALOG_START,"BLI_DisableShow",0);
	SetEventHandler(EVENT_DIALOG_EXIT,"BLI_EnableShow",0);

	if(dialogRun) BLI_DisableShow();
	else BLI_EnableShow();

	SetEventHandler("evntBLI_Update","BLI_UpdateObjectData",0);
	SetEventHandler("BI_CommandEndChecking","BLI_CheckCommand",0);
	SetEventHandler("BI_LaunchCommand","BLI_ExecuteCommand",0);
	SetEventHandler("BI_SetPossibleCommands","BLI_SetPossibleCommands",0);
	SetEventHandler("evntFindDialogChar","procFindDialogChar",0);
	SetEventHandler("Location_CharacterEntryToLocator", "BLI_ChrEnterToLocation", 0);
	SetEventHandler("Location_CharacterExitFromLocator", "BLI_ChrExitFromLocation", 0);
	SetEventHandler("evntPerkAgainUsable","BLI_RefreshCommandMenu",0);
	SetEventHandler("Control Activation","LI_ProcessControlPress",0);

	Event("evntBLI_Update");
	Event("evntFindDialogChar");
}

ref BLI_CheckCommand()
{
	string commandName = GetEventData();

	aref uiref;	makearef(uiref,objLandInterface.UserIcons);
	aref ar;
	int nq = GetAttributesNum(uiref);
	int i;

	bool bUsed = false;
	for(i=0; i<nq; i++)
	{
		ar = GetAttributeN(uiref,i);
		ar.enable = false;
	}
	g_intRetVal = BI_COMMODE_USER_ICONS;

	switch(commandName)
	{
	case "cancel":
		g_intRetVal = -1;
	break;

	case "BI_FastReload":
		bUsed = SetReloadIcons();
	break;

	case "BI_ItemsUse":
		bUsed = SetUsedPotionIcons();
	break;

	case "BI_DialogStart":
		g_intRetVal = 0;
	break;

	case "BI_ItemsChange":
		g_intRetVal = 0;
	break;

	case "BI_TakeItem":
		g_intRetVal = 0;
	break;

	case "BI_PlaceItem":
		g_intRetVal = 0;
	break;

	case "BI_OutDoor":
		g_intRetVal = 0;
	break;

	case "BI_UseBox":
		g_intRetVal = 0;
	break;
	// boal
    case "BI_DeadBox":
		g_intRetVal = 0;
	break;
	case "BI_TalkSelf":
		g_intRetVal = 0;
	break;
	
	case "BI_ActivateRush":
		g_intRetVal = 0;
	break;
	
	case "BI_Exit_Deck": //boal
		g_intRetVal = 0;
	break;
	}

	//if(!bUsed)	objLandInterface.UserIcons.cancel.enable = true;
	return &g_intRetVal;
}

void BLI_ExecuteCommand()
{
	int		chrIdx = GetEventData();
	string	commName = GetEventData();
	int		targChrIdx = GetEventData();
	string	evntName = GetEventData();

	aref arFader;
	if( FindClass(arFader,"fader") ) {return;}

	aref	uiref;
	int		curLocIdx;
	int		tmpi;

	if( commName=="cancel" || evntName=="cancel" ) {
		SendMessage(&objLandInterface,"ls",MSG_BATTLE_LAND_MAKE_COMMAND,"cancel");
		return;
	}

	switch(commName)
	{
	case "BI_FastReload":
		{
			curLocIdx = FindLoadedLocation();
			if(curLocIdx==-1) return;
			if( FindUserIcon(evntName,&uiref) )
			{
				string sTravelLocator = "";
				if(CheckAttribute(uiref,"locator")) {sTravelLocator=uiref.locator;}
				if(CheckAttribute(uiref,"location"))
				{
					PlayerFastTravel(curLocIdx, uiref.location, sTravelLocator);
				}
			}
		}
	break;
	case "BI_ItemsUse":
		if( FindUserIcon(evntName,&uiref) )
		{
			if(CheckAttribute(uiref,"potion"))
			{
				aref arItm;
				if( Items_FindItem(uiref.potion,&arItm) >= 0 )
				{
					if( CheckAttribute(arItm,"potion") )
					{
						DoCharacterUsedItem(pchar,arItm.id);
					}
				}
			}
		}
	break;
	case "BI_DialogStart":
		tmpi = SendMessage(pchar,"ls",MSG_CHARACTER_EX_MSG,"FindDialogCharacter");
		if(tmpi>=0)	Event("dlgReady","l",tmpi);
	break;
	case "BI_ItemsChange":
		tmpi = SendMessage(pchar,"ls",MSG_CHARACTER_EX_MSG,"FindDialogCharacter");
		if(tmpi>=0 && isOfficerInShip(GetCharacter(tmpi), true))//fix вот дыра дл€ обмена с матросом
        {
            LaunchCharacterItemChange(GetCharacter(tmpi));
        }
        // boal 22.01.2004 -->
		else
		{
		// обмен с офицером всегда и везде (в абордаже нет диалога!)
		    tmpi = LAi_FindNearestVisCharacter(GetMainCharacter(), 1);
		    if( tmpi>0 && IsOfficer(GetCharacter(tmpi)) && IsCharacterPerkOn(pchar,"SharedExperience") )
			{
			    LaunchCharacterItemChange(GetCharacter(tmpi));
			}
		}
	break;
	case "BI_TakeItem":
		Item_OnPickItem();
	break;
	case "BI_PlaceItem":
		Item_OnUseItem();
	break;
	case "BI_OutDoor":
		Event("Control Activation","s","ChrAction");
	break;
	case "BI_UseBox":
		OpenBoxProcedure();
	break;
	// boal
	case "BI_DeadBox":
		Dead_OpenBoxProcedure();
	break;
	
	case "BI_TalkSelf":
		DoQuestCheckDelay("TalkSelf_Start", 0.2);
	break;
	
	case "BI_ActivateRush":
		ActivateCharacterPerk(pchar,"Rush");
	break;
	// boal
    case "BI_Exit_Deck":
        if (isShipInside(pchar.location))
        {
            Return2SeaAfterCabin();
        }
        else
        {
            int idxLoadLoc = FindLoadedLocation();
            if (idxLoadLoc != -1)
            {
	            UnloadLocation(&Locations[idxLoadLoc]);
            }
            wdmReloadToSea();
        }
    break;
	}

}

void EndBattleLandInterface()
{
	if(!bLandInterfaceStart) return;
	bLandInterfaceStart = false;

	DeleteAttribute( pchar, "boxname" );

	BLI_DisableShow();
	SendMessage(&objLandInterface,"l",MSG_BATTLE_LAND_END);

	DeleteClass(&objLandInterface);

	DelEventHandler(EVENT_CHANGE_OFFICERS,"BLI_UpdateOfficers");
	DelEventHandler(EVENT_DIALOG_START,"BLI_DisableShow");
	DelEventHandler(EVENT_DIALOG_EXIT,"BLI_EnableShow");

	DelEventHandler("evntBLI_Update","BLI_UpdateObjectData");
	DelEventHandler("BI_CommandEndChecking","BLI_CheckCommand");
	DelEventHandler("BI_LaunchCommand","BLI_ExecuteCommand");
	DelEventHandler("BI_SetPossibleCommands","BLI_SetPossibleCommands");
	DelEventHandler("evntFindDialogChar","procFindDialogChar");
	DelEventHandler("Location_CharacterEntryToLocator", "BLI_ChrEnterToLocation");
	DelEventHandler("Location_CharacterExitFromLocator", "BLI_ChrExitFromLocation");
	DelEventHandler("evntPerkAgainUsable","BLI_RefreshCommandMenu");
	DelEventHandler("Control Activation","LI_ProcessControlPress");

	Log_SetActiveAction("Nothing");
}

void BLI_SetObjectData()
{
	DeleteAttribute(&objLandInterface,"");

	objLandInterface.parameters.DoShowCommandos = InterfaceStates.BattleShow.Command;
	objLandInterface.data.Alarm = 0; // нет тревоги

	// персы вместе с нами
	aref ar;
	int i,cn;
	ref curCh;
	string attrName;
	for(i=0; i<4; i++)
	{
		cn = GetOfficersIndex(pchar,i);
		if(cn==-1) continue;
		curCh = GetCharacter(cn);
		attrName = "id"+i;
		makearef(ar,objLandInterface.data.icons.(attrName));
		SetCharacterIconData(cn,ar);
		//ar.picture = curCh.FaceId;
		//ar.health = 0.0;
		//ar.charge = 0.0;
	}
	BLI_SetMessageParameters();
	// текстуры
	int idLngFile = LanguageOpenFile("commands_name.txt");
	objLandInterface.CommandTextures.list.t0.name = "battle_interface\LandCommands.tga";
	objLandInterface.CommandTextures.list.t0.xsize = 16;
	objLandInterface.CommandTextures.list.t0.ysize = 2;
	
 	objLandInterface.CommandTextures.list.t1.name = "battle_interface\LandTarget_SLB.tga";
	objLandInterface.CommandTextures.list.t1.xsize = 16;
	objLandInterface.CommandTextures.list.t1.ysize = 2;
	
	objLandInterface.CommandTextures.list.t2.name = "battle_interface\Cancel.tga";
	objLandInterface.CommandTextures.list.t2.xsize = 2;
	objLandInterface.CommandTextures.list.t2.ysize = 1;
	// boal <--
	objLandInterface.CommandTextures.CommandTexNum = 0;
	//
	DeleteAttribute(&objIconsNote,"");
	objIconsNote.1x0 = LanguageConvertString(idLngFile, "go_store");
	objIconsNote.1x1 = LanguageConvertString(idLngFile, "go_shipyard");
	objIconsNote.1x2 = LanguageConvertString(idLngFile, "go_tavern");
	objIconsNote.1x4 = LanguageConvertString(idLngFile, "go_residence");
	objIconsNote.1x3 = LanguageConvertString(idLngFile, "go_usurer");
	objIconsNote.1x6 = LanguageConvertString(idLngFile, "go_church");
	objIconsNote.1x9 = LanguageConvertString(idLngFile, "go_brothel");
	objIconsNote.1x10 = LanguageConvertString(idLngFile, "go_port");
	objIconsNote.1x8 = LanguageConvertString(idLngFile, "go_portman");
	// список команд
	objLandInterface.Commands.Cancel.enable		= false;
	objLandInterface.Commands.Cancel.picNum		= 1;
	objLandInterface.Commands.Cancel.selPicNum	= 0;
	objLandInterface.Commands.Cancel.texNum		= 2;
	objLandInterface.Commands.Cancel.event		= "Cancel";
	objLandInterface.Commands.Cancel.name		= "Cancel";
	objLandInterface.Commands.Cancel.note		= LanguageConvertString(idLngFile, "sea_cancel");

	objLandInterface.Commands.FastReload.enable		= true;
	objLandInterface.Commands.FastReload.picNum		= 27;
	objLandInterface.Commands.FastReload.selPicNum	= 11;
	objLandInterface.Commands.FastReload.texNum		= 1;
	objLandInterface.Commands.FastReload.event		= "BI_FastReload";
	objLandInterface.Commands.FastReload.note		= LanguageConvertString(idLngFile, "land_FastReload");

	objLandInterface.Commands.OutDoor.enable		= true;
	objLandInterface.Commands.OutDoor.picNum		= 28;
	objLandInterface.Commands.OutDoor.selPicNum		= 12;
	objLandInterface.Commands.OutDoor.texNum		= 1;
	objLandInterface.Commands.OutDoor.event			= "BI_OutDoor";
	objLandInterface.Commands.OutDoor.note			= LanguageConvertString(idLngFile, "land_OutDoor");
	// boal -->
    objLandInterface.Commands.ItemsUse.enable		= true;
 	objLandInterface.Commands.ItemsUse.picNum		= 20;
	objLandInterface.Commands.ItemsUse.selPicNum		= 4;
	objLandInterface.Commands.ItemsUse.texNum		= 0;
	objLandInterface.Commands.ItemsUse.event		= "BI_ItemsUse";
	objLandInterface.Commands.ItemsUse.note			= LanguageConvertString(idLngFile, "land_ItemsUse");
	
	objLandInterface.Commands.DialogStart.enable	= true;
 	objLandInterface.Commands.DialogStart.picNum		= 17;
	objLandInterface.Commands.DialogStart.selPicNum		= 1;
	objLandInterface.Commands.DialogStart.texNum		= 0;
	objLandInterface.Commands.DialogStart.event		= "BI_DialogStart";
	objLandInterface.Commands.DialogStart.note		= LanguageConvertString(idLngFile, "land_DialogStart");
	
	objLandInterface.Commands.ItemsChange.enable	= true;
 	objLandInterface.Commands.ItemsChange.picNum		= 19;
	objLandInterface.Commands.ItemsChange.selPicNum		= 3;
	objLandInterface.Commands.ItemsChange.texNum		= 0;
	objLandInterface.Commands.ItemsChange.event		= "BI_ItemsChange";
	objLandInterface.Commands.ItemsChange.note		= LanguageConvertString(idLngFile, "land_ItemsChange");
	
	objLandInterface.Commands.TakeItem.enable		= true;
 	objLandInterface.Commands.TakeItem.picNum		= 16;
	objLandInterface.Commands.TakeItem.selPicNum		= 0;
	objLandInterface.Commands.TakeItem.texNum		= 0;
	objLandInterface.Commands.TakeItem.event		= "BI_TakeItem";
	objLandInterface.Commands.TakeItem.note			= LanguageConvertString(idLngFile, "land_TakeItem");
	
	objLandInterface.Commands.PlaceItem.enable		= true;
 	objLandInterface.Commands.PlaceItem.picNum		= 16;
	objLandInterface.Commands.PlaceItem.selPicNum		= 0;
	objLandInterface.Commands.PlaceItem.texNum		= 0;
	objLandInterface.Commands.PlaceItem.event		= "BI_PlaceItem";
	objLandInterface.Commands.PlaceItem.note		= LanguageConvertString(idLngFile, "land_PlaceItem");

	objLandInterface.Commands.UseBox.enable			= true;
 	objLandInterface.Commands.UseBox.picNum		    = 16;
	objLandInterface.Commands.UseBox.selPicNum		= 0;
	objLandInterface.Commands.UseBox.texNum		= 0;
	objLandInterface.Commands.UseBox.event			= "BI_UseBox";
	objLandInterface.Commands.UseBox.note			= LanguageConvertString(idLngFile, "land_UseBox");
	
	objLandInterface.Commands.DeadBox.enable			= true;
 	objLandInterface.Commands.DeadBox.picNum		    = 20;
	objLandInterface.Commands.DeadBox.selPicNum		= 4;
	objLandInterface.Commands.DeadBox.texNum		= 0;
	objLandInterface.Commands.DeadBox.event			= "BI_DeadBox";
	objLandInterface.Commands.DeadBox.note			= "ќбыск тела";
	// диалог сам с собой -->
	objLandInterface.Commands.TalkSelf.enable	= true;
 	objLandInterface.Commands.TalkSelf.picNum	 	= 18;
	objLandInterface.Commands.TalkSelf.selPicNum	= 2;
	objLandInterface.Commands.TalkSelf.texNum		= 0;
	objLandInterface.Commands.TalkSelf.event		= "BI_TalkSelf";
	objLandInterface.Commands.TalkSelf.note		= "ћысли вслух";
	// диалог сам с собой <--
	objLandInterface.Commands.ActivateRush.enable	= true;
	objLandInterface.Commands.ActivateRush.picNum	= 28;
	objLandInterface.Commands.ActivateRush.selPicNum		= 12;
	objLandInterface.Commands.ActivateRush.texNum		= 0;
	objLandInterface.Commands.ActivateRush.event	= "BI_ActivateRush";
	objLandInterface.Commands.ActivateRush.note		= LanguageConvertString(idLngFile, "land_ActivateRush");
	
	objLandInterface.Commands.Exit_Deck.enable		= true;
	objLandInterface.Commands.Exit_Deck.picNum		= 30;
	objLandInterface.Commands.Exit_Deck.selPicNum		= 14;
	objLandInterface.Commands.Exit_Deck.texNum		= 0;
	objLandInterface.Commands.Exit_Deck.event		= "BI_Exit_Deck";
	objLandInterface.Commands.Exit_Deck.note		= LanguageConvertString(idLngFile, "land_Exit");
	// boal <--
	// список пользовательских картинок
	LanguageCloseFile(idLngFile);
	// go to icons
	int idxloc = FindLoadedLocation();
	string outGroupName = "";
	RecalculateHireCrew(idxloc);  // to_do убрать на таймер
	if(idxloc>=0 && CheckAttribute(&Locations[idxloc],"fastreload"))
	{
		outGroupName = Locations[idxloc].fastreload;
		CreateReloadPaths(outGroupName);
	}

	objLandInterface.ManSign.backtexturename		= "battle_interface\ShipBackIcon.tga";
	objLandInterface.ManSign.backcolor				= argb(255,128,128,128);
	objLandInterface.ManSign.backuv					= "0.0,0.0,1.0,1.0";
	objLandInterface.ManSign.backoffset				= "0.0,0.0";
	objLandInterface.ManSign.backiconsize			= "128,128";

	objLandInterface.ManSign.alarmtexturename		= "battle_interface\alarmback.tga";
	objLandInterface.ManSign.alarmhighcolor			= argb(255,168,28,28);
	objLandInterface.ManSign.alarmlowcolor			= argb(64,168,28,28);
	objLandInterface.ManSign.alarmuptime			= 0.5;
	objLandInterface.ManSign.alarmdowntime			= 1.5;
	objLandInterface.ManSign.alarmuv				= "0.0,0.0,1.0,1.0";
	objLandInterface.ManSign.alarmoffset			= "0.0,0.0";
	objLandInterface.ManSign.alarmiconsize			= "128,128";

	objLandInterface.ManSign.manstatetexturename	= "battle_interface\ShipState.tga";
	objLandInterface.ManSign.manstatecolor			= argb(255,128,128,128);
	objLandInterface.ManSign.manhpuv				= "0.0,0.109,0.5,0.6875";
	objLandInterface.ManSign.manhpoffset			= "-32,-13";
	objLandInterface.ManSign.manhpiconsize			= "64,74";
	objLandInterface.ManSign.manenegryuv			= "0.5,0.109,1.0,0.6875";
	objLandInterface.ManSign.manenegryoffset		= "32,-13";
	objLandInterface.ManSign.manenergyiconsize		= "64,74";

	objLandInterface.ManSign.gunchargetexturename	= "battle_interface\ShipClass.tga";
	objLandInterface.ManSign.gunchargecolor			= argb(255,168,168,48);
	objLandInterface.ManSign.gunchargebackcolor		= argb(255,188,48,48);
	objLandInterface.ManSign.gunchargeuv			= "0.0,0.0,1.0,1.0";
	objLandInterface.ManSign.gunchargeoffset		= "-14,-50";
	objLandInterface.ManSign.gunchargeiconsize		= "64,16";
	objLandInterface.ManSign.gunchargeprogress		= "0.0625, 0.219, 0.359, 0.5, 0.641, 0.781, 0.983";

	objLandInterface.ManSign.manfacecolor			= argb(255,128,128,128);
	objLandInterface.ManSign.manfaceoffset			= "-14,-12";
	objLandInterface.ManSign.manfaceiconsize		= "64,64";

	objLandInterface.ManSign.commandlistverticaloffset = -40;

	objLandInterface.ManSign.iconoffset1 = "70,70";
	objLandInterface.ManSign.iconoffset2 = "70,180";
	objLandInterface.ManSign.iconoffset3 = "70,290";
	objLandInterface.ManSign.iconoffset4 = "70,400";

	int nLoc = FindLoadedLocation();
	if(nLoc >= 0) {
		int nFile = LanguageOpenFile("LocLables.txt");
		if(nFile >= 0) {
			objLandInterface.textinfo.islandname.font = "interface_normal";
			objLandInterface.textinfo.islandname.scale = 0.9;
			objLandInterface.textinfo.islandname.pos.x = sti(showWindow.right) - RecalculateHIcon(104);
			objLandInterface.textinfo.islandname.pos.y = RecalculateVIcon(20);
			if (CheckAttribute(&locations[nLoc],"islandId"))
			{
				if (locations[nLoc].islandId != "Mein")
				{
					objLandInterface.textinfo.islandname.text = XI_convertString("Island:") + LanguageConvertString(nFile, locations[nLoc].islandId);
				}
				else
				{
					objLandInterface.textinfo.islandname.text = LanguageConvertString(nFile, locations[nLoc].islandId);
				}
			}
			else
			{
				objLandInterface.textinfo.islandname.text = "";//XI_convertString("Open Sea");
			}
			//
			objLandInterface.textinfo.villagename.font = "interface_normal";
			objLandInterface.textinfo.villagename.scale = 0.9;
			objLandInterface.textinfo.villagename.pos.x = sti(showWindow.right) - RecalculateHIcon(104);
			objLandInterface.textinfo.villagename.pos.y = RecalculateVIcon(40);

			objLandInterface.textinfo.locationname.font = "interface_normal";
			objLandInterface.textinfo.locationname.scale = 0.9;
			objLandInterface.textinfo.locationname.pos.x = sti(showWindow.right) - RecalculateHIcon(104);
			objLandInterface.textinfo.locationname.pos.y = RecalculateVIcon(60);
			
            if (!CheckAttribute(&locations[nLoc],"fastreload"))
			{
			    objLandInterface.textinfo.villagename.text = "";
			}
			else
			{
				objLandInterface.textinfo.villagename.text = XI_ConvertString("Colony:") + LanguageConvertString(nFile, locations[nLoc].fastreload + " Town");
			}
			objLandInterface.textinfo.locationname.text = LanguageConvertString(nFile, locations[nLoc].id.label);
			LanguageCloseFile( nFile );
		}
	}
	objLandInterface.textinfo.datatext.font = "interface_normal";
	objLandInterface.textinfo.datatext.scale = 0.9;
	objLandInterface.textinfo.datatext.pos.x = sti(showWindow.right) - RecalculateHIcon(104);
	objLandInterface.textinfo.datatext.pos.y = RecalculateVIcon(85);
	RefreshLandTime();
	//objLandInterface.textinfo.datatext.text = XI_convertString("Date:") + GetQuestBookData(); //GetDataDay()+" "+XI_ConvertString("target_month_" + GetDataMonth())+" "+GetDataYear();
	objLandInterface.textinfo.datatext.refreshable = true;

	/*objLandInterface.imageslist.textinfoback1.texture = "battle_interface\textinfo_back.tga";
	objLandInterface.imageslist.textinfoback1.color = argb(128,64,64,64);
	objLandInterface.imageslist.textinfoback1.uv = "0.0,0.0,1.0,0.125";
	objLandInterface.imageslist.textinfoback1.pos = (sti(showWindow.right) - RecalculateHIcon(188)) + "," +
		RecalculateVIcon(12) + "," +
		(sti(showWindow.right) - RecalculateHIcon(20)) + "," +
		RecalculateVIcon(20);
	objLandInterface.imageslist.textinfoback2.texture = "battle_interface\textinfo_back.tga";
	objLandInterface.imageslist.textinfoback2.color = argb(128,64,64,64);
	objLandInterface.imageslist.textinfoback2.uv = "0.0,0.125,1.0,0.875";
	objLandInterface.imageslist.textinfoback2.pos = (sti(showWindow.right) - RecalculateHIcon(188)) + "," +
		RecalculateVIcon(20) + "," +
		(sti(showWindow.right) - RecalculateHIcon(20)) + "," +
		RecalculateVIcon(120);
	objLandInterface.imageslist.textinfoback3.texture = "battle_interface\textinfo_back.tga";
	objLandInterface.imageslist.textinfoback3.color = argb(128,64,64,64);
	objLandInterface.imageslist.textinfoback3.uv = "0.0,0.875,1.0,1.0";
	objLandInterface.imageslist.textinfoback3.pos = (sti(showWindow.right) - RecalculateHIcon(188)) + "," +
		RecalculateVIcon(120) + "," +
		(sti(showWindow.right) - RecalculateHIcon(20)) + "," +
		RecalculateVIcon(128);
		*/
	//
	/*времени в игре нет
	objLandInterface.textinfo.timetext.font = "interface_normal";
	objLandInterface.textinfo.timetext.scale = 1.0;
	objLandInterface.textinfo.timetext.pos.x = RecalculateHIcon(940);
	objLandInterface.textinfo.timetext.pos.y = RecalculateVIcon(100);
	objLandInterface.textinfo.timetext.text = makeint(GetHour())+":"+makeint(GetMinute());
    */
    
    // Ёто у вас нет, у мен€ все есть ;) - boal
	objLandInterface.CommandList.CommandMaxIconQuantity = 10;
	objLandInterface.CommandList.CommandIconSpace = 1;
	objLandInterface.CommandList.CommandIconLeft = 108;//157;
	objLandInterface.CommandList.CommandIconWidth = RecalculateHIcon(48);
	objLandInterface.CommandList.CommandIconHeight = RecalculateVIcon(48);

	objLandInterface.CommandList.CommandNoteFont = "interface_normal";
	objLandInterface.CommandList.CommandNoteColor = argb(255,255,255,255);
	objLandInterface.CommandList.CommandNoteScale = 1.0;
	objLandInterface.CommandList.CommandNoteOffset = RecalculateHIcon(0) + "," + RecalculateVIcon(-42);

	objLandInterface.CommandList.UDArrow_Texture = "battle_interface\arrowly.tga";
	objLandInterface.CommandList.UDArrow_UV_Up = "0.0,1.0,1.0,0.0";
	objLandInterface.CommandList.UDArrow_UV_Down = "0.0,0.0,1.0,1.0";
	objLandInterface.CommandList.UDArrow_Size = RecalculateHIcon(32) + "," + RecalculateVIcon(32);
	objLandInterface.CommandList.UDArrow_Offset_Up = RecalculateHIcon(-41) + "," + RecalculateVIcon(-30);
	objLandInterface.CommandList.UDArrow_Offset_Down = RecalculateHIcon(-41) + "," + RecalculateVIcon(46);
	
	// дебилы все равно играть не будут, а нормальным лишнее не нужно boal
	/*objLandInterface.CommandList.ActiveIcon_Texture = "battle_interface\enter_list.tga";
	objLandInterface.CommandList.ActiveIcon_Offset = RecalculateHIcon(-49) + ",0";
	objLandInterface.CommandList.ActiveIcon_Size = RecalculateHIcon(48) + "," + RecalculateVIcon(48);
	objLandInterface.CommandList.ActiveIcon_UV1 = "0.5,0.0,1.0,1.0";
	objLandInterface.CommandList.ActiveIcon_UV2 = "0.0,0.0,0.5,1.0";
	objLandInterface.CommandList.ActiveIcon_Note = XI_ConvertString("MenuNote");*/

	SendMessage(&objLandInterface,"l",MSG_BATTLE_LAND_START);
}

bool FindUserIcon(string id,ref uiref)
{
	aref arroot,ar; makearef(arroot,objLandInterface.UserIcons);
	int iMax = GetAttributesNum(arroot); //fix
	for(int i=0; i<iMax; i++)
	{
		ar = GetAttributeN(arroot,i);
		if(ar.name != id) continue;
		uiref = ar;
		return true;
	}
	return false;
}

void BLI_SetMessageParameters()
{
	objLandInterface.MessageIcons.IconWidth = RecalculateHIcon(64);
	objLandInterface.MessageIcons.IconHeight = RecalculateVIcon(24);
	objLandInterface.MessageIcons.IconDist = RecalculateVIcon(2);
	objLandInterface.MessageIcons.IconBottom = sti(showWindow.bottom)-RecalculateHIcon(80+40);
	objLandInterface.MessageIcons.IconMaxQuantity = 4;
	objLandInterface.MessageIcons.BlendTime = 3.0;
	objLandInterface.MessageIcons.FallSpeed = 22.0;
	objLandInterface.MessageIcons.argbHighBlind = argb(255,128,128,128);
	objLandInterface.MessageIcons.argbLowBlind = argb(255,68,68,68);
	objLandInterface.MessageIcons.BlindUpTime = 0.5;
	objLandInterface.MessageIcons.BlindDownTime = 1.0;
	objLandInterface.MessageIcons.texture = "battle_interface\MessageIcons.tga";
	objLandInterface.MessageIcons.TexHSize = 2;
	objLandInterface.MessageIcons.TexVSize = 2;
}

void BLI_UpdateObjectData()
{
	bool bIsRiskAlarm = LAi_group_IsActivePlayerAlarm();
	if(sti(objLandInterface.data.Alarm)!=bIsRiskAlarm) BLI_RefreshCommandMenu();
	objLandInterface.data.Alarm = bIsRiskAlarm;

	aref ar,arItm;
	int i,cn;
	ref curCh;
	string attrName;
	for(i=0; i<4; i++)
	{
		attrName = "id"+i;
		cn = GetOfficersIndex(pchar,i);
		if(cn==-1 || !IsEntity(&Characters[cn]))
		{
			DeleteAttribute(&objLandInterface,"data.icons."+attrName);
			continue;
		}
		curCh = GetCharacter(cn);
		makearef(ar,objLandInterface.data.icons.(attrName));
		SetCharacterIconData(cn,ar);
		//ar.picture = curCh.FaceId;
		//ar.health = LAi_GetCharacterRelHP(curCh);
		//ar.charge = LAi_GetCharacterRelCharge(curCh);
		//ar.shootMax = LAi_GetCharacterChargeQuant(curCh);
		//ar.shootCur = LAi_GetCharacterChargeCur(curCh);
		//ar.poison = LAi_IsPoison(curCh);
	}

	PostEvent("evntBLI_Update",BLI_UPDATE_PERIOD);
}

void BLI_SetPossibleCommands()
{
	int chrMode = SendMessage(pchar,"ls",MSG_CHARACTER_EX_MSG,"IsFightMode");

	bool bTmpBool;
	bool bUseCommand = false;
	int i;
	aref rootcom,curcom;
	makearef(rootcom,objLandInterface.Commands);
	int iMax = GetAttributesNum(rootcom); //fix
	for(i=0; i<iMax; i++)
	{
		curcom = GetAttributeN(rootcom,i);
		curcom.enable = false;
	}

	if(chrMode==0)
	{
		bTmpBool = true;
		if(LAi_group_IsActivePlayerAlarm()) bTmpBool = false;
		if(!LAi_IsCharacterControl(pchar)) bTmpBool = false;
		if(bDisableFastReload)	bTmpBool = false;
		if(!IsEnableFastTravel()) bTmpBool = false;
		// boal запрет всех переходов
		if (chrDisableReloadToLocation) bTmpBool = false;
		if (!CheckAttribute(loadedLocation,"fastreload")) bTmpBool = false;  // в каюте некуда переходить
		if (bTmpBool) // все еще можно переходить, провер€ем город враг
		{
		    i = sti(Colonies[FindColony(loadedLocation.fastreload)].nation);
			if (i > 0)
			{
				bTmpBool = (GetNationRelation2MainCharacter(i) == RELATION_ENEMY) || GetRelation2BaseNation(i) == RELATION_ENEMY;
				if (bTmpBool && (i != PIRATE))// && !CheckAttribute(pchar, "CheckStateOk"))
				{
					bTmpBool = false;
				}
				else
				{
		    		bTmpBool = true;
				}
			}
		}
		objLandInterface.Commands.FastReload.enable	= bTmpBool;
		///// boal -->
		i = -1;
		if (CheckAttribute(pchar,"dialogready")) i=sti(pchar.dialogready);
		if (i>=0)
		{
			objLandInterface.Commands.DialogStart.enable	= true;
			if (isOfficerInShip(&Characters[i], true))
			{
				objLandInterface.Commands.ItemsChange.enable = true;
			}
			// грабеж
			/*
			else
			{
				if( CheckAttribute(&Characters[i],"InterfaceFlags.ItemsChange") &&
					Characters[i].InterfaceFlags.ItemsChange != "0" )
				{
					objLandInterface.Commands.ItemsChange.enable = true;
				}
			}   */
		}
		// boal 22.01.2004 -->
		else
		// обмен с офицером всегда и везде (в абордаже нет диалога!)
		{
		    i = LAi_FindNearestVisCharacter(pchar, 1);
		    if( i>0 && IsOfficer(GetCharacter(i)) && IsCharacterPerkOn(pchar, "SharedExperience"))
			{
				objLandInterface.Commands.ItemsChange.enable = true;
			}
		}
		//////// <--
		bUseCommand = true;
	}
	if (objLandInterface.Commands.DialogStart.enable == false && LAi_IsCharacterControl(pchar) && !LAi_IsFightMode(pchar) && !LAi_grp_alarmactive && !chrDisableReloadToLocation)
	{
    	objLandInterface.Commands.TalkSelf.enable = true;//boal разговор с каюте сам с собой
    	bUseCommand = true;
    }
	if(GetCharacterPerkUsing(pchar,"Rush"))
	{
		objLandInterface.Commands.ActivateRush.enable = true;
		bUseCommand = true;
	}
	

	if( CanBeUseItem(pchar) )
	{
		objLandInterface.Commands.ItemsUse.enable = true;
		bUseCommand = true;
	}

	if(CheckAttribute(pchar,"boxname"))
	{
		objLandInterface.Commands.UseBox.enable	= true;
		bUseCommand = true;
	}

	if( CheckAttribute(pchar,"activeItem") && pchar.activeItem!="" )
	{
		bUseCommand = true;
		if( CheckAttribute(pchar,"activeItem.pick") && pchar.activeItem.pick=="1" )
		{
			objLandInterface.Commands.TakeItem.enable = true;
		}
		else
		{
			objLandInterface.Commands.PlaceItem.enable = true;
		}
	}

	if( chrWaitReloadLocator != "" )
	{
		bUseCommand = true;
		objLandInterface.Commands.OutDoor.enable = true;
	}
    // boal dead can be searched 14.12.2003 -->
    i = Dead_FindCloseBody();
    if (i != -1)
    {
        objLandInterface.Commands.DeadBox.enable	= true;
		bUseCommand = true;
    }
	// boal dead can be searched 14.12.2003 <--
	// boal 20.03.2004 -->
    if (isShipInside(pchar.location) && !chrDisableReloadToLocation)
    {
       bUseCommand = true;
       objLandInterface.Commands.Exit_Deck.enable = true;
    }
    // boal 20.03.2004 <--
    // boal 20.03.2004 <--
    if (pchar.location == "Ship_deck")
    {
		if (CheckQuestAttribute("Munity", "Deads"))
		{
			bUseCommand = true;
			objLandInterface.Commands.Exit_Deck.enable = true;
		}
    }
	//objLandInterface.Commands.OutDoor.enable		= true;
	if(!bUseCommand)
	{
		SendMessage(&objLandInterface,"llllll",BI_IN_SET_COMMAND_MODE,0,-1,-1,-1,-1);
	}
}

/*void BLI_evntRefreshCommandMenu()
{
	DelEventHandler("frame","BLI_evntRefreshCommandMenu");
	BLI_RefreshCommandMenu();
}
void BLI_RefreshCommandMenu()
{
	SendMessage(&objLandInterface,"lllll",BI_IN_SET_COMMAND_MODE,-1,-1,-1,-1,-1);
}*/

void procFindDialogChar()
{
	int dlgChar = -1;
	if(!LAi_IsBoardingProcess())
		dlgChar = SendMessage(pchar,"ls",MSG_CHARACTER_EX_MSG,"FindDialogCharacter");

	int oldDlgChar = -1;
	if(CheckAttribute(pchar,"dialogready")) oldDlgChar = sti(pchar.dialogready);

	if(dlgChar!=oldDlgChar)
	{
		pchar.dialogready = dlgChar;
		//BLI_RefreshCommandMenu();
		LI_CareUpdateCommandList();
	}

	PostEvent("evntFindDialogChar",1000);
}

void BLI_ChrEnterToLocation()
{
	aref loc = GetEventData();	
	aref chr = GetEventData();
	string group = GetEventData();
	string locator = GetEventData();
	aref locList,locCur;
	int i,n;

	if(sti(chr.index) != nMainCharacterIndex) return;

	if(group=="reload")
	{
		makearef(locList,loc.reload);
		n = GetAttributesNum(locList);
		for(i=0;i<n;i++)
		{
			locCur = GetAttributeN(locList,i);
			if(locCur.name == locator) break;
		}
		if(i<n)
		{
			i = FindLocation(locCur.go);
			if(i>=0)
			{
				if(CheckAttribute(&locations[i],"id.label"))
				{
					Log_SetStringToLog( LanguageConvertString(g_LocLngFileID,locations[i].id.label) );
				}
			}
		}
		//SetEventHandler("frame","BLI_evntRefreshCommandMenu",1);
		LI_CareUpdateCommandList();
	}
}

void BLI_ChrExitFromLocation()
{
	//SetEventHandler("frame","BLI_evntRefreshCommandMenu",1);
	LI_CareUpdateCommandList();
}

void LI_Reload()
{
	Event("Control Activation","s","ChrAction");
}

bool SetUsedPotionIcons()
{
	aref arItm, ar, uiref;
	int i, itmIdx, nq;

	bool bUsed = false;

	makearef(uiref,objLandInterface.UserIcons);
	nq = GetAttributesNum(uiref);
	for(i=0; i<nq; i++)
	{
		ar = GetAttributeN(uiref,i);
		if( CheckAttribute(ar,"potion") ) {
			DeleteAttribute(uiref,GetAttributeName(ar));
			nq--; i--;
		}
	}

	string UI_name;
	int UI_idx = 0;

	itmIdx = FindPotionFromChr(pchar, &arItm, 0);
	while(itmIdx>=0)
	{
		if( EnablePotionUsing(pchar,arItm) )
		{
			UI_idx++;
			UI_name = "potion"+UI_idx;
			objLandInterface.UserIcons.(UI_name).enable = true;
			objLandInterface.UserIcons.(UI_name).pic = GetPotionPicture(arItm);
            objLandInterface.UserIcons.(UI_name).selpic = GetPotionPicture(arItm) - 16;
			objLandInterface.UserIcons.(UI_name).tex = GetPotionTexture(arItm);
			objLandInterface.UserIcons.(UI_name).name = UI_name;
			objLandInterface.UserIcons.(UI_name).potion = arItm.id;
			bUsed = true;
		}
		itmIdx = FindPotionFromChr(pchar, &arItm, itmIdx+1);
	}

	return bUsed;
}

bool SetReloadIcons()
{
	int i, nq, locidx;
	aref ar, curloc, locref, locList, uiref;

	bool bUse = false;

	makearef(uiref,objLandInterface.UserIcons);
	nq = GetAttributesNum(uiref);
	for(i=0; i<nq; i++)
	{
		ar = GetAttributeN(uiref,i);
		if( CheckAttribute(ar,"location") ) {
			DeleteAttribute(uiref,GetAttributeName(ar));
			nq--; 
			i--;
		}
	}

	int idxloc = FindLoadedLocation();
	string str1,str2,fastLocName;
	string outGroupName = "";
	if(idxloc>=0 && CheckAttribute(&Locations[idxloc],"fastreload"))
	{
		outGroupName = Locations[idxloc].fastreload;
		
		if( CheckAttribute(&objFastReloadTable,"table."+outGroupName) )
		{
			makearef(locList,objFastReloadTable.table.(outGroupName));
			nq = GetAttributesNum(locList);
			// to port icon
			if( GetCharacterShipType(pchar)!=SHIP_NOTUSED )
			{
				if( CheckFastJump(Locations[idxloc].id, pchar.location.from_sea) )
				{
					objLandInterface.UserIcons.port.enable = true;
					objLandInterface.UserIcons.port.pic = 26;
					objLandInterface.UserIcons.port.selpic = 10;
					objLandInterface.UserIcons.port.tex = 1;
					objLandInterface.UserIcons.port.name = "reloadtoport";
					objLandInterface.UserIcons.port.location = pchar.location.from_sea;
					objLandInterface.UserIcons.port.locator = "reload1";
					objLandInterface.UserIcons.port.note = GetNodeForIcon(1, 10);
					bUse = true;
				}
			}
			for(i=0; i<nq; i++)
			{
				curloc = GetAttributeN(locList,i);
				if(!CheckFastJump(Locations[idxloc].id, curloc.location) ) 
				{
					continue;
				}
				fastLocName = "fr_"+(i+1);
				objLandInterface.UserIcons.(fastLocName).enable = true;
				objLandInterface.UserIcons.(fastLocName).pic = sti(curloc.pic)+16;
				objLandInterface.UserIcons.(fastLocName).selpic = curloc.pic;
				objLandInterface.UserIcons.(fastLocName).tex = 1+sti(curloc.tex);
				objLandInterface.UserIcons.(fastLocName).name = "reload"+i;
				objLandInterface.UserIcons.(fastLocName).location = curloc.location;
				objLandInterface.UserIcons.(fastLocName).note = GetNodeForIcon(1+sti(curloc.tex), sti(curloc.pic));
				bUse = true;
			}
		}
	}
	int idLngFile = LanguageOpenFile("commands_name.txt");

	return bUse;
}

void SetOfficerTexture(int officerIdx)
{
	aref ar; makearef(ar,objLandInterface.Parameters);
	string attrName = "iconTexture"+officerIdx;

	int cn = GetOfficersIndex(pchar,officerIdx);
	if(cn<0) {
		DeleteAttribute(ar,attrName);
		ar.(attrName) = "";
	} else {
		ar.(attrName) = "interfaces\portraits\64\face_"+Characters[cn].FaceID+".tga";
	}
}

void BLI_UpdateOfficers()
{
	SetOfficerTexture(0);
	SetOfficerTexture(1);
	SetOfficerTexture(2);
	SetOfficerTexture(3);

	SendMessage(&objLandInterface, "lls", MSG_BATTLE_LAND_SET_ICONTEX, 0, objLandInterface.Parameters.iconTexture0);
	SendMessage(&objLandInterface, "lls", MSG_BATTLE_LAND_SET_ICONTEX, 1, objLandInterface.Parameters.iconTexture1);
	SendMessage(&objLandInterface, "lls", MSG_BATTLE_LAND_SET_ICONTEX, 2, objLandInterface.Parameters.iconTexture2);
	SendMessage(&objLandInterface, "lls", MSG_BATTLE_LAND_SET_ICONTEX, 3, objLandInterface.Parameters.iconTexture3);
	//Log_TestInfo("BLI_UpdateOfficers");
	BLI_UpdateObjectData();
	LI_CareUpdateCommandList();
	SendMessage(&objLandInterface,"ls",MSG_BATTLE_LAND_MAKE_COMMAND,"cancel");  // фикс иконок офов (рефреш) boal
}

int GetPotionPicture(aref arItm)
{
	if( CheckAttribute(arItm,"potion.pic") ) {
		return sti(arItm.potion.pic);
	}
	trace("Item "+arItm.id+ " hav`t POTION.PIC attribute");
	return 0;
}

int GetPotionTexture(aref arItm)
{
	if( !CheckAttribute(arItm,"potion.tex") ) {
		trace("Item "+arItm.id+ " hav`t POTION.TEX attribute");
		return 0;
	}
	return sti(arItm.potion.tex);
}

// можем ли юзать хоть что то
bool CanBeUseItem(ref chref)
{
	for(int i=0; i<ITEMS_QUANTITY; i++)
	{
		if( CheckAttribute(&Items[i],"potion") && CheckAttribute(chref,"items."+Items[i].id) ) {
			if( EnablePotionUsing(chref,&Items[i]) ) {return true;}
		}
	}
	return false;
}

object objIconsNote;
string GetNodeForIcon(int nTex, int nPic)
{
	string attrName = nTex+"x"+nPic;
	if( CheckAttribute(objIconsNote,attrName) ) return objIconsNote.(attrName);
	return "";
}

void LI_ProcessControlPress()
{
	string ControlName = GetEventData();

	switch(ControlName)
	{
		case "hk_Cure": // TO_DO не работет - у мен€ в др месте
			if( LAi_GetCharacterRelHP(pchar) < 1.0 )
			{
				float dhlt = LAi_GetCharacterMaxHP(pchar) - LAi_GetCharacterHP(pchar);
				string btl = FindHealthForCharacter(pchar, dhlt);
				DoCharacterUsedItem(pchar, btl);
			}
		break;
		
		case "LICommandsActivate": 
			PlaySound("interface\ok.wav"); // boal даешь звуки!
		break;
	}
}

void BLI_RefreshCommandMenu()
{
	SendMessage( &objLandInterface,"l",BI_MSG_CARE_COMMANDLIST_UPDATE );
}
void LI_CareUpdateCommandList()
{
	SendMessage( &objLandInterface,"l",BI_MSG_CARE_COMMANDLIST_UPDATE );
}

void SetCharacterIconData(int chrindex, aref arData)
{
	ref chref = GetCharacter(chrindex);
	arData.chrindex = chrindex;
	arData.health = LAi_GetCharacterRelHP(chref);
	arData.energy = LAi_GetCharacterRelEnergy(chref);
	arData.shootMax = LAi_GetCharacterChargeQuant(chref);
	arData.shootCur = LAi_GetCharacterChargeCur(chref);
	if( CheckAttribute(chref,"FaceId") ) {
		//arData.picture = chref.FaceId;
		arData.texture = "battle_interface\portraits\face_"+chref.FaceID+".tga";
		if( CheckAttribute(chref,"FaceUV") ) {
			arData.uv = chref.FaceUV;
		} else {
			arData.uv = "0.0,0.0,1.0,1.0";
		}
	} else {
		arData.texture = "battle_interface\two.tga";
		if( chrindex == 1 ) {
			arData.uv = "0.5,0.0,1.0,1.0";
		} else {
			arData.uv = "0.0,0.0,0.5,1.0";
		}
	}
	//arData.charge = LAi_GetCharacterRelCharge(chref);
	//arData.poison = LAi_IsPoison(chref);
}