
#define MODE_NO		0
#define	MODE_SHOW	1
#define MODE_MAIN	2

#define MAIN_WINDOW_FIRST	0
#define MAIN_WINDOW_SECOND	1

object obj_CM_StoreData;

int iFFMode;

int iShowMode;
int curCntrlIdx;

string sColony;
int iUpgradeNum;

int iColony;

/////////////////////////////////////////////////////////////////
/// NEW INTERFACE
/////////////////////////////////////////////////////////////////
int nCurScrollNum, nCurPassengerScrollNum;
/////////////////////////////////////////////////////////////////
/// END NEW INTERFACE
/////////////////////////////////////////////////////////////////


ref xi_refCharacter;

void InitInterface(string iniName)
{
	iFFMode = MAIN_WINDOW_FIRST;
	iShowMode = MODE_NO;

	int i;
	xi_refCharacter = pchar;

	GameInterface.title = "titleColonyManagement";
	
	//GameInterface.StatusLine.EXP.Max = CalculateExperienceFromRank(sti(xi_refCharacter.rank)+1);
	//GameInterface.StatusLine.EXP.Min = CalculateExperienceFromRank(sti(xi_refCharacter.rank));
	//GameInterface.StatusLine.EXP.Value = xi_refCharacter.Experience;
	
	/////////////////////////////////////////////////////////////////
	/// NEW INTERFACE
	/////////////////////////////////////////////////////////////////
	
	FillColoniesScroll();
	FillPassengerScroll();

	/////////////////////////////////////////////////////////////////
	/// END NEW INTERFACE
	/////////////////////////////////////////////////////////////////
	
	SendMessage(&GameInterface,"ls",MSG_INTERFACE_INIT,iniName);

	SetDescription();
	FillUpgradesImages();
	
	SendMessage(&GameInterface,"lslsssllllllfl", MSG_INTERFACE_MSG_TO_NODE,"MAIN_WINDOW_STRINGS",0,
		"Passengers",XI_ConvertString("Colonies"), FONT_CAPTION,75,57, COLOR_NORMAL,0, 
		SCRIPT_ALIGN_CENTER, true, 1.0, 420);

	SendMessage(&GameInterface,"lslsssllllllfl", MSG_INTERFACE_MSG_TO_NODE,"MAIN_WINDOW_STRINGS",0,
		"BuildingsText",XI_ConvertString("Buildings"), FONT_CAPTION,378,57, COLOR_NORMAL,0, 
		SCRIPT_ALIGN_LEFT, true, 1.0, 420);
	
	string sText;
	sText = " " + XI_ConvertString("Hovernor");
	SendMessage(&GameInterface,"lsle",MSG_INTERFACE_MSG_TO_NODE,"CHARACTER_INFO_TEXT", 0,&sText);
	sText = " ";
	SendMessage(&GameInterface,"lsle",MSG_INTERFACE_MSG_TO_NODE,"CHARACTER_INFO", 0,&sText);

	sText = " " + XI_ConvertString("Colonylevel");
	SendMessage(&GameInterface,"lsle",MSG_INTERFACE_MSG_TO_NODE,"CHARACTER_INFO_TEXT", 0,&sText);
	sText = " ";
	SendMessage(&GameInterface,"lsle",MSG_INTERFACE_MSG_TO_NODE,"CHARACTER_INFO", 0,&sText);

	sText = " " + XI_ConvertString("Population");
	SendMessage(&GameInterface,"lsle",MSG_INTERFACE_MSG_TO_NODE,"CHARACTER_INFO_TEXT", 0,&sText);
	sText = " ";
	SendMessage(&GameInterface,"lsle",MSG_INTERFACE_MSG_TO_NODE,"CHARACTER_INFO", 0,&sText);

	sText = " " + XI_ConvertString("Gold");
	SendMessage(&GameInterface,"lsle",MSG_INTERFACE_MSG_TO_NODE,"CHARACTER_INFO_TEXT", 0,&sText);
	sText = " ";
	SendMessage(&GameInterface,"lsle",MSG_INTERFACE_MSG_TO_NODE,"CHARACTER_INFO", 0,&sText);

	sText = " " + XI_ConvertString("Morale");
	SendMessage(&GameInterface,"lsle",MSG_INTERFACE_MSG_TO_NODE,"CHARACTER_INFO_TEXT", 0,&sText);
	sText = " ";
	SendMessage(&GameInterface,"lsle",MSG_INTERFACE_MSG_TO_NODE,"CHARACTER_INFO", 0,&sText);

	sText = " " + XI_ConvertString("Loyality");
	SendMessage(&GameInterface,"lsle",MSG_INTERFACE_MSG_TO_NODE,"CHARACTER_INFO_TEXT", 0,&sText);
	sText = " ";
	SendMessage(&GameInterface,"lsle",MSG_INTERFACE_MSG_TO_NODE,"CHARACTER_INFO", 0,&sText);

	sText = " " + XI_ConvertString("SalaryQuantity");
	SendMessage(&GameInterface,"lsle",MSG_INTERFACE_MSG_TO_NODE,"CHARACTER_INFO_TEXT", 0,&sText);
	sText = " ";
	SendMessage(&GameInterface,"lsle",MSG_INTERFACE_MSG_TO_NODE,"CHARACTER_INFO", 0,&sText);

	sText = " " + XI_ConvertString("Garrison");
	SendMessage(&GameInterface,"lsle",MSG_INTERFACE_MSG_TO_NODE,"CHARACTER_INFO_TEXT", 0,&sText);
	sText = " ";
	SendMessage(&GameInterface,"lsle",MSG_INTERFACE_MSG_TO_NODE,"CHARACTER_INFO", 0,&sText);

	sText = " " + XI_ConvertString("Agressor");
	SendMessage(&GameInterface,"lsle",MSG_INTERFACE_MSG_TO_NODE,"CHARACTER_INFO_TEXT", 0,&sText);
	sText = " ";
	SendMessage(&GameInterface,"lsle",MSG_INTERFACE_MSG_TO_NODE,"CHARACTER_INFO", 0,&sText);

	sText = " " + XI_ConvertString("Disease");
	SendMessage(&GameInterface,"lsle",MSG_INTERFACE_MSG_TO_NODE,"CHARACTER_INFO_TEXT", 0,&sText);
	sText = " ";
	SendMessage(&GameInterface,"lsle",MSG_INTERFACE_MSG_TO_NODE,"CHARACTER_INFO", 0,&sText);


	SetEventHandler("InterfaceBreak","ProcessCancelExit",0);
	SetEventHandler("exitCancel","ProcessCancelExit",0);
	SetEventHandler("ievnt_command","ProcessCommandExecute",0);
	SetEventHandler("frame","ProcessFrame",1);
	SetEventHandler("SetDescription","SetDescription",1);
	SetEventHandler("Upgrade", "Upgrade",0);
	SetEventHandler("UpgradeBuy", "UpgradeBuy",0);
	SetEventHandler("UpgradeShow", "UpgradeShow",0);
	SetEventHandler("Build", "Build",0);
	SetEventHandler("HideBuildConfirm", "HideBuildConfirm",0);	
	SetEventHandler("ConfirmBuildYes", "ConfirmBuildYes",0);
	SetEventHandler("MouseRClickUP", "HideAllWindows",0);
	SetEventHandler("ShowColonyInfo", "ShowColonyInfo",0);
	SetEventHandler("FormatedTextActivate", "FormatedTextActivate", 0);

	SetEventHandler("startColonyAction", "FStartColonyAction", 0);
	SetEventHandler("endMoneyExchange", "FEndMoneyExchange", 0);
	SetEventHandler("eMoneyChange", "FMoneyChange", 0);
	SetEventHandler("eSlideChange", "FSlideChange", 0);
	SetEventHandler("endHovernorSet", "FEndHovernorSet", 0);
	SetEventHandler("HovernorAction", "FHovernorAction", 0);

	SetSelectable("CHARACTERS_BUTTON", true);
	SetSelectable("COLONIES_BUTTON", false);

	ProcessFrame();
	SetVariable();

	//SetFormatedText("INFO",xi_refCharacter.bio);
	SetSelectable("SHIPS_BUTTON", IsEnableShipButton());
	SetSelectable("SET_HOVERNOR_BUTTON",GetFreePassengersQuantity(pchar)>0);

}


void SetVariable()
{
	SendMessage(&GameInterface,"lslsssllllllfl", MSG_INTERFACE_MSG_TO_NODE,"MAIN_WINDOW_STRINGS",0,
		"CharacterFullName", XI_ConvertString("Colony"+colonies[iColony].id), FONT_CAPTION,262,249, COLOR_NORMAL,0, 
		SCRIPT_ALIGN_CENTER, true, 1.0, 420);

	string sText;

	sText = GetCharacterFullName(colonies[iColony].commander);
	sText = sText + " ";
	SendMessage(&GameInterface,"lslle",MSG_INTERFACE_MSG_TO_NODE,"CHARACTER_INFO", 10,0,&sText);

	sText = XI_ConvertString("ColonyLevel" + Colonies[iColony].colonylevel);
	sText = sText + " ";
	SendMessage(&GameInterface,"lslle",MSG_INTERFACE_MSG_TO_NODE,"CHARACTER_INFO", 10,1,&sText);

	sText = colonies[iColony].population;
	sText = sText + " ";
	SendMessage(&GameInterface,"lslle",MSG_INTERFACE_MSG_TO_NODE,"CHARACTER_INFO", 10,2,&sText);

	sText = colonies[iColony].money;
	sText = sText + " ";
	SendMessage(&GameInterface,"lslle",MSG_INTERFACE_MSG_TO_NODE,"CHARACTER_INFO", 10,3,&sText);

	sText = XI_ConvertString(GetMoraleName(sti(colonies[iColony].morale)));
	sText = sText + " ";
	SendMessage(&GameInterface,"lslle",MSG_INTERFACE_MSG_TO_NODE,"CHARACTER_INFO", 10,4,&sText);

	sText = XI_ConvertString(GetMoraleName(sti(colonies[iColony].loyality)));
	sText = sText + " ";
	SendMessage(&GameInterface,"lslle",MSG_INTERFACE_MSG_TO_NODE,"CHARACTER_INFO", 10,5,&sText);

	sText = colonies[iColony].crew;
	sText = sText + " ";
	SendMessage(&GameInterface,"lslle",MSG_INTERFACE_MSG_TO_NODE,"CHARACTER_INFO", 10,6,&sText);

	string sFortComm = colonies[iColony].commander;
	if (CheckAttribute(&characters[GetCharacterIndex(sFortComm)], "ship.crew.quantity"))
	{
		sText = characters[GetCharacterIndex(sFortComm)].ship.crew.quantity ;
	}
	else
	{
		sText = 0;
	}

	sText = sText + " ";
	SendMessage(&GameInterface,"lslle",MSG_INTERFACE_MSG_TO_NODE,"CHARACTER_INFO", 10,7,&sText);

	if(sti(colonies[iColony].agressor) != -1)
	{
		sText = XI_ConvertString(GetNationNameByType(sti(colonies[iColony].agressor)));
	}
	else
	{
		sText = XI_ConvertString("no");
	}
	sText = sText + " ";
	SendMessage(&GameInterface,"lslle",MSG_INTERFACE_MSG_TO_NODE,"CHARACTER_INFO", 10,8,&sText);

	if(sti(colonies[iColony].disease) != 0)
	{
		sText = XI_ConvertString("Yes");
	}
	else
	{
		sText = XI_ConvertString("no");
	}
	sText = sText + " ";
	SendMessage(&GameInterface,"lslle",MSG_INTERFACE_MSG_TO_NODE,"CHARACTER_INFO", 10,9,&sText);

	SetNewPicture("CHARACTER_BIG_PICTURE", "interfaces\portraits\256\face_" + characters[GetCharacterIndex(colonies[iColony].commander)].FaceId + ".tga");
	
	SendMessage(&GameInterface,"lsll",MSG_INTERFACE_MSG_TO_NODE,"CHARACTER_INFO_TEXT", 1, 0);
	SendMessage(&GameInterface,"lsll",MSG_INTERFACE_MSG_TO_NODE,"CHARACTER_INFO", 1, 0);
	
}

void ProcessCancelExit()
{
	interfaceResultCommand = RC_INTERFACE_CHARACTER_EXIT;
	if( CheckAttribute(&InterfaceStates,"InstantExit") && sti(InterfaceStates.InstantExit)==true )
	{
		CharInterfaceExit(true);
	}
	else
	{
		CharInterfaceExit(false);
	}
}

void DelEventHandlers()
{
	DelEventHandler("InterfaceBreak","ProcessCancelExit");
	DelEventHandler("exitCancel","ProcessCancelExit");
	DelEventHandler("ievnt_command","ProcessCommandExecute");
	DelEventHandler("frame","ProcessFrame");
	DelEventHandler("SetDescription","SetDescription");
	DelEventHandler("Upgrade","Upgrade");
	DelEventHandler("UpgradeBuy", "UpgradeBuy");
	DelEventHandler("UpgradeShow","UpgradeShow");
	DelEventHandler("Build","Build");
	DelEventHandler("HideBuildConfirm","HideBuildConfirm");
	DelEventHandler("ConfirmBuildYes","ConfirmBuildYes");
	DelEventHandler("MouseRClickUP", "HideAllWindows");
	DelEventHandler("ShowColonyInfo", "ShowColonyInfo");
	DelEventHandler("FormatedTextActivate", "FormatedTextActivate");
	DelEventHandler("startColonyAction", "FStartColonyAction");
	DelEventHandler("endMoneyExchange", "FEndMoneyExchange");
	DelEventHandler("eMoneyChange", "FMoneyChange");
	DelEventHandler("eSlideChange", "FSlideChange");
	DelEventHandler("endHovernorSet", "FEndHovernorSet");
	DelEventHandler("HovernorAction", "FHovernorAction");
}

void CharInterfaceExit(bool flag)
{
	DelEventHandlers();

	EndCancelInterface(true);
}

void ProcessCommandExecute()
{
	string comName = GetEventData();
	string nodName = GetEventData();

	
	switch(nodName)
	{
		case "CHARACTERS_BUTTON":
			if(comName=="click")
			{
				
				IDoExit(RC_INTERFACE_TO_CHAR, true);
				return;
			}
		break;
		
		case "SHIPS_BUTTON":
			if(comName=="click")
			{
				
				IDoExit(RC_INTERFACE_TO_SHIP, true);
				return;
			}
		break;
		
		case "JOURNAL_BUTTON":
			if(comName=="click")
			{
				
				IDoExit(RC_INTERFACE_TO_LOGBOOK, true);
				return;
			}
		break;
		
		case "COLONIES_BUTTON":
			if(comName=="click")
			{
				IDoExit(RC_INTERFACE_TO_COLONY, true);
				return;
			}
		break;
		
		case "ITEMS_BUTTON":
			if(comName=="click")
			{
				
				IDoExit(RC_INTERFACE_TO_ITEMS, true);
				return;
			}
		break;
	}

}

void IDoExit(int exitCode, bool bCode)
{
	interfaceResultCommand = exitCode;
	DelEventHandlers();
	EndCancelInterface(bCode);
}

void ProcessFrame()
{
	if(sti(GameInterface.COLONY_SCROLL.current)!=nCurScrollNum && GetCurrentNode() == "COLONY_SCROLL")
	{
			nCurScrollNum = sti(GameInterface.COLONY_SCROLL.current);
			SetDescription();
			FillUpgradesImages();
			return;
	}
	if(sti(GameInterface.SETHovernor_PASSENGER_LIST.current)!=nCurPassengerScrollNum && GetCurrentNode()=="SETHovernor_PASSENGER_LIST")
	{
		nCurPassengerScrollNum = sti(GameInterface.SETHovernor_PASSENGER_LIST.current);
		ShowHovernorInfo( nCurPassengerScrollNum );
		return;
	}
}


void SetDescription()
{	
	string attributeName = "pic" + (nCurScrollNum+1);
			
	if (CheckAttribute(&GameInterface, "COLONY_SCROLL." + attributeName))
	{
		if(CheckAttribute(GameInterface, "COLONY_SCROLL." + attributeName + ".colony"))
		{
			iColony = sti(GameInterface.COLONY_SCROLL.(attributeName).colony);
			SetVariable();
			FillUpgradesImages();
		}
	}
}

void FillColoniesScroll()
{
	int i;
	string attributeName;
	aref pRef2;

	DeleteAttribute(&GameInterface, "COLONY_SCROLL");
		
	nCurScrollNum = -1;
	GameInterface.COLONY_SCROLL.current = 0;
	GameInterface.COLONY_SCROLL.ImagesGroup.t1 = "ISLANDSICON";
	
	GameInterface.COLONY_SCROLL.BadPicture1 = "interfaces\empty_face.tga";
	
	int listsize = 0;
	
	//string sLocation = pchar.location;

	int iNotUsed = 0;
	for(int k = 0; k < MAX_COLONIES; k++)
	{
		if(colonies[k].nation != "none")
		{
			if(sti(colonies[k].nation) != PIRATE)
			{
				iNotUsed = iNotUsed + 1;
			}
		}
	}
	GameInterface.COLONY_SCROLL.NotUsed = iNotUsed;

	for(i=0; i<MAX_COLONIES; i++)
	{
		if (sti(Colonies[i].capture_flag) == 1)
		{
			attributeName = "pic" + (listsize+1);
			makearef(pRef2,GameInterface.COLONY_SCROLL.(attributeName));
			GameInterface.COLONY_SCROLL.(attributeName).img1 = Colonies[i].island;
			GameInterface.COLONY_SCROLL.(attributeName).tex1 = 0;
			sColony = Colonies[i].id;
			pRef2.str1 = "#" + XI_ConvertString("Colony" + sColony);
			pRef2.str2 = "#" + characters[GetCharacterIndex(Colonies[i].commander)].name;
			pRef2.str3 = "#" + characters[GetCharacterIndex(Colonies[i].commander)].lastname;
			GameInterface.COLONY_SCROLL.(attributeName).colony = i;
			GameInterface.COLONY_SCROLL.(attributeName).character = colonies[i].commander;
			listsize = listsize +  1;
		}
	}
	GameInterface.COLONY_SCROLL.current = 0;
	GameInterface.COLONY_SCROLL.ListSize = listsize + iNotUsed - 1;
}

void FillUpgradesImages()
{
	int iImageNumber;
	string sImageName;
	string sStep;
	string sText;
	
	iImageNumber = sti(colonies[iColony].fort);
	if(iImageNumber < 4)
	{
		sImageName = "Fort"+(iImageNumber+1);
		sStep = "l"+(iImageNumber+1);
	}
	else
	{
		sImageName = "Fort"+iImageNumber;
		sStep = "l" + iImageNumber;
	}
	
	if(CheckForCompletedUpgrade(UPGRADE_FORT, iColony) == true)
	{
		EI_CreateSubstrate("UPGRADES_BORDERS", "skillborder_silver",377,172,510,196);
		SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"FORT_BORDER", 1, argb(0,32,32,32), argb(150,100,100,100));
	}
	else
	{
		if(CheckForUpdate(UPGRADE_FORT, iColony) != true)
		{
			EI_CreateSubstrate("UPGRADES_BORDERS", "skillborder",377,172,510,196);
			SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"FORT_BORDER", 1, argb(160,32,32,32), argb(150,100,100,100));
		}
		else
		{
			EI_CreateSubstrate("UPGRADES_BORDERS", "skillborder_green",377,172,510,196);
			SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"FORT_BORDER", 1, argb(0,32,32,32), argb(150,100,100,100));
		}
	}

	sText = XI_ConvertString(ColonyUpgrades[UPGRADE_FORT].(sStep).name);
	SendMessage(&GameInterface,"lslsssllllllfl", MSG_INTERFACE_MSG_TO_NODE,"MAIN_WINDOW_STRINGS",0,
		"Fort",sText, FONT_NORMAL,444,176, COLOR_NORMAL,0, 
		SCRIPT_ALIGN_CENTER, true, 0.8, 420);
	
	SendMessage(&GameInterface,"lslss", MSG_INTERFACE_MSG_TO_NODE,"Fort",6, "UPGRADES", sImageName);

	iImageNumber = sti(colonies[iColony].shipyard);
	if(iImageNumber < 3)
	{
		sImageName = "Shipyard"+(iImageNumber+1);
		sStep = "l"+(iImageNumber+1);
	}
	else
	{
		sImageName = "Shipyard"+iImageNumber;
		sStep = "l" + iImageNumber;
	}

	if(CheckForCompletedUpgrade(UPGRADE_SHIPYARD, iColony) == true)
	{
		EI_CreateSubstrate("UPGRADES_BORDERS", "skillborder_silver",515,172,649,196);
		SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"SHIPYARD_BORDER", 1, argb(0,32,32,32), argb(150,100,100,100));
	}
	else
	{
		if(CheckForUpdate(UPGRADE_SHIPYARD, iColony) != true)
		{
			EI_CreateSubstrate("UPGRADES_BORDERS", "skillborder",515,172,649,196);
			SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"SHIPYARD_BORDER", 1, argb(160,32,32,32), argb(150,100,100,100));
		}
		else
		{
			EI_CreateSubstrate("UPGRADES_BORDERS", "skillborder_green",515,172,649,196);
			SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"SHIPYARD_BORDER", 1, argb(0,32,32,32), argb(150,100,100,100));
		}
	}

	sText = XI_ConvertString(ColonyUpgrades[UPGRADE_SHIPYARD].(sStep).name);
	SendMessage(&GameInterface,"lslsssllllllfl", MSG_INTERFACE_MSG_TO_NODE,"MAIN_WINDOW_STRINGS",0,
		"Shipyard",sText, FONT_NORMAL,582,176, COLOR_NORMAL,0, 
		SCRIPT_ALIGN_CENTER, true, 0.8, 420);
	SendMessage(&GameInterface,"lslss", MSG_INTERFACE_MSG_TO_NODE,"Shipyard",6, "UPGRADES", sImageName);

	sImageName = "Wheatfield";

	sStep = "l1";

	if(CheckForCompletedUpgrade(UPGRADE_WHEAT, iColony) == true)
	{
		EI_CreateSubstrate("UPGRADES_BORDERS", "skillborder_silver",653,172,787,196);
		SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"WHEATFIELD_BORDER", 1, argb(0,32,32,32), argb(150,100,100,100));
	}
	else
	{
		if(CheckForUpdate(UPGRADE_WHEAT, iColony) != true)
		{
			EI_CreateSubstrate("UPGRADES_BORDERS", "skillborder",653,172,787,196);
			SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"WHEATFIELD_BORDER", 1, argb(160,32,32,32), argb(150,100,100,100));
		}
		else
		{
			EI_CreateSubstrate("UPGRADES_BORDERS", "skillborder_green",653,172,787,196);
			SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"WHEATFIELD_BORDER", 1, argb(0,32,32,32), argb(150,100,100,100));
		}
	}
	sText = XI_ConvertString(ColonyUpgrades[UPGRADE_WHEAT].(sStep).name);
	SendMessage(&GameInterface,"lslsssllllllfl", MSG_INTERFACE_MSG_TO_NODE,"MAIN_WINDOW_STRINGS",0,
		"Wheatfield",sText, FONT_NORMAL,718,176, COLOR_NORMAL,0, 
		SCRIPT_ALIGN_CENTER, true, 0.8, 420);
	SendMessage(&GameInterface,"lslss", MSG_INTERFACE_MSG_TO_NODE,"Wheatfield",6, "UPGRADES", sImageName);

	sImageName = "Fishingpier";
	if(CheckForCompletedUpgrade(UPGRADE_PIER, iColony) == true)
	{
		EI_CreateSubstrate("UPGRADES_BORDERS", "skillborder_silver",377,288,510,312);
		SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"FISHINGPIER_BORDER", 1, argb(0,32,32,32), argb(150,100,100,100));
	}
	else
	{
		if(CheckForUpdate(UPGRADE_PIER, iColony) != true)
		{
			EI_CreateSubstrate("UPGRADES_BORDERS", "skillborder",377,288,510,312);
			SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"FISHINGPIER_BORDER", 1, argb(160,32,32,32), argb(150,100,100,100));
		}
		else
		{
			EI_CreateSubstrate("UPGRADES_BORDERS", "skillborder_green",377,288,510,312);
			SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"FISHINGPIER_BORDER", 1, argb(0,32,32,32), argb(150,100,100,100));
		}
	}

	sText = XI_ConvertString(ColonyUpgrades[UPGRADE_PIER].(sStep).name);
	SendMessage(&GameInterface,"lslsssllllllfl", MSG_INTERFACE_MSG_TO_NODE,"MAIN_WINDOW_STRINGS",0,
		"Pier",sText, FONT_NORMAL,444,291, COLOR_NORMAL,0, 
		SCRIPT_ALIGN_CENTER, true, 0.8, 420);

	SendMessage(&GameInterface,"lslss", MSG_INTERFACE_MSG_TO_NODE,"Fishingpier",6, "UPGRADES", sImageName);

	sImageName = "Mill";
	if(CheckForCompletedUpgrade(UPGRADE_MILL, iColony) == true)
	{
		EI_CreateSubstrate("UPGRADES_BORDERS", "skillborder_silver",515,288,649,312);
		SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"MILL_BORDER", 1, argb(0,32,32,32), argb(150,100,100,100));
	}
	else
	{
		if(CheckForUpdate(UPGRADE_MILL, iColony) != true)
		{
			EI_CreateSubstrate("UPGRADES_BORDERS", "skillborder",515,288,649,312);
			SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"MILL_BORDER", 1, argb(160,32,32,32), argb(150,100,100,100));
		}
		else
		{
			EI_CreateSubstrate("UPGRADES_BORDERS", "skillborder_green",515,288,649,312);
			SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"MILL_BORDER", 1, argb(0,32,32,32), argb(150,100,100,100));
		}
	}
	sText = XI_ConvertString(ColonyUpgrades[UPGRADE_MILL].(sStep).name);
	SendMessage(&GameInterface,"lslsssllllllfl", MSG_INTERFACE_MSG_TO_NODE,"MAIN_WINDOW_STRINGS",0,
		"MILL",sText, FONT_NORMAL,582,291, COLOR_NORMAL,0, 
		SCRIPT_ALIGN_CENTER, true, 0.8, 420);
	SendMessage(&GameInterface,"lslss", MSG_INTERFACE_MSG_TO_NODE,"Mill",6, "UPGRADES", sImageName);

	sImageName = "Huts";
	if(CheckForCompletedUpgrade(UPGRADE_HUTS, iColony) == true)
	{
		EI_CreateSubstrate("UPGRADES_BORDERS", "skillborder_silver",653,288,787,312);
		SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"HUTS_BORDER", 1, argb(0,32,32,32), argb(150,100,100,100));
	}
	else
	{
		if(CheckForUpdate(UPGRADE_HUTS, iColony) != true)
		{
			EI_CreateSubstrate("UPGRADES_BORDERS", "skillborder",653,288,787,312);
			SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"HUTS_BORDER", 1, argb(160,32,32,32), argb(150,100,100,100));
		}
		else
		{
			EI_CreateSubstrate("UPGRADES_BORDERS", "skillborder_green",653,288,787,312);
			SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"HUTS_BORDER", 1, argb(0,32,32,32), argb(150,100,100,100));
		}
	}
	sText = XI_ConvertString(ColonyUpgrades[UPGRADE_HUTS].(sStep).name);
	SendMessage(&GameInterface,"lslsssllllllfl", MSG_INTERFACE_MSG_TO_NODE,"MAIN_WINDOW_STRINGS",0,
		"Huts",sText, FONT_NORMAL,718,291, COLOR_NORMAL,0, 
		SCRIPT_ALIGN_CENTER, true, 0.8, 420);
	SendMessage(&GameInterface,"lslss", MSG_INTERFACE_MSG_TO_NODE,"Huts",6, "UPGRADES", sImageName);
	
	sImageName = "Storehouse";
	if(CheckForCompletedUpgrade(UPGRADE_STOREHOUSE, iColony) == true)
	{
		EI_CreateSubstrate("UPGRADES_BORDERS", "skillborder_silver",377,401,510,425);
		SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"STOREHOUSE_BORDER", 1, argb(0,32,32,32), argb(150,100,100,100));
	}
	else
	{
		if(CheckForUpdate(UPGRADE_STOREHOUSE, iColony) != true)
		{
			EI_CreateSubstrate("UPGRADES_BORDERS", "skillborder",377,401,510,425);
			SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"STOREHOUSE_BORDER", 1, argb(160,32,32,32), argb(150,100,100,100));
		}
		else
		{
			EI_CreateSubstrate("UPGRADES_BORDERS", "skillborder_green",377,401,510,425);
			SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"STOREHOUSE_BORDER", 1, argb(0,32,32,32), argb(150,100,100,100));
		}
	}
	sText = XI_ConvertString(ColonyUpgrades[UPGRADE_STOREHOUSE].(sStep).name);
	SendMessage(&GameInterface,"lslsssllllllfl", MSG_INTERFACE_MSG_TO_NODE,"MAIN_WINDOW_STRINGS",0,
		"StoreHouse",sText, FONT_NORMAL,444,406, COLOR_NORMAL,0, 
		SCRIPT_ALIGN_CENTER, true, 0.8, 420);
	SendMessage(&GameInterface,"lslss", MSG_INTERFACE_MSG_TO_NODE,"Storehouse",6, "UPGRADES", sImageName);

	sImageName = "Church";
	if(CheckForCompletedUpgrade(UPGRADE_CHURCH, iColony) == true)
	{
		EI_CreateSubstrate("UPGRADES_BORDERS", "skillborder_silver",515,401,649,425);
		SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"CHURCH_BORDER", 1, argb(0,32,32,32), argb(150,100,100,100));
	}
	else
	{
		if(CheckForUpdate(UPGRADE_CHURCH, iColony) != true)
		{
			EI_CreateSubstrate("UPGRADES_BORDERS", "skillborder",515,401,649,425);
			SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"CHURCH_BORDER", 1, argb(160,32,32,32), argb(150,100,100,100));
		}
		else
		{
			EI_CreateSubstrate("UPGRADES_BORDERS", "skillborder_green",515,401,649,425);
			SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"CHURCH_BORDER", 1, argb(0,32,32,32), argb(150,100,100,100));
		}
	}
	sText = XI_ConvertString(ColonyUpgrades[UPGRADE_CHURCH].(sStep).name);
	SendMessage(&GameInterface,"lslsssllllllfl", MSG_INTERFACE_MSG_TO_NODE,"MAIN_WINDOW_STRINGS",0,
		"Church",sText, FONT_NORMAL,582,406, COLOR_NORMAL,0, 
		SCRIPT_ALIGN_CENTER, true, 0.8, 420);
	SendMessage(&GameInterface,"lslss", MSG_INTERFACE_MSG_TO_NODE,"Church",6, "UPGRADES", sImageName);
	SendMessage(&GameInterface,"lslss", MSG_INTERFACE_MSG_TO_NODE,"Church",6, "UPGRADES", sImageName);

	sImageName = "Academy";
	if(CheckForCompletedUpgrade(UPGRADE_ACADEMY, iColony) == true)
	{
		EI_CreateSubstrate("UPGRADES_BORDERS", "skillborder_silver",653,401,787,425);
		SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"ACADEMY_BORDER", 1, argb(0,32,32,32), argb(150,100,100,100));
	}
	else
	{
		if(CheckForUpdate(UPGRADE_ACADEMY, iColony) != true)
		{
			EI_CreateSubstrate("UPGRADES_BORDERS", "skillborder",653,401,787,425);
			SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"ACADEMY_BORDER", 1, argb(160,32,32,32), argb(150,100,100,100));
		}
		else
		{
			EI_CreateSubstrate("UPGRADES_BORDERS", "skillborder_green",653,401,787,425);
			SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"ACADEMY_BORDER", 1, argb(0,32,32,32), argb(150,100,100,100));
		}
	}
	sText = XI_ConvertString(ColonyUpgrades[UPGRADE_ACADEMY].(sStep).name);
	SendMessage(&GameInterface,"lslsssllllllfl", MSG_INTERFACE_MSG_TO_NODE,"MAIN_WINDOW_STRINGS",0,
		"Academy",sText, FONT_NORMAL,718,406, COLOR_NORMAL,0, 
		SCRIPT_ALIGN_CENTER, true, 0.8, 420);
	SendMessage(&GameInterface,"lslss", MSG_INTERFACE_MSG_TO_NODE,"Academy",6, "UPGRADES", sImageName);

	sImageName = "Expedition";
	if(CheckForCompletedUpgrade(UPGRADE_EXPEDITION, iColony) == true)
	{
		EI_CreateSubstrate("UPGRADES_BORDERS", "skillborder_silver",377,517,510,541);
		SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"EXPEDITION_BORDER", 1, argb(0,32,32,32), argb(150,100,100,100));
	}
	else
	{
		if(CheckForUpdate(UPGRADE_EXPEDITION, iColony) != true)
		{
			EI_CreateSubstrate("UPGRADES_BORDERS", "skillborder",377,517,510,541);
			SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"EXPEDITION_BORDER", 1, argb(160,32,32,32), argb(150,100,100,100));
		}
		else
		{
			EI_CreateSubstrate("UPGRADES_BORDERS", "skillborder_green",377,517,510,541);
			SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"EXPEDITION_BORDER", 1, argb(0,32,32,32), argb(150,100,100,100));
		}
	}
	sText = XI_ConvertString(ColonyUpgrades[UPGRADE_EXPEDITION].(sStep).name);
	SendMessage(&GameInterface,"lslsssllllllfl", MSG_INTERFACE_MSG_TO_NODE,"MAIN_WINDOW_STRINGS",0,
		"Expedition",sText, FONT_NORMAL,444,522, COLOR_NORMAL,0, 
		SCRIPT_ALIGN_CENTER, true, 0.8, 420);
	SendMessage(&GameInterface,"lslss", MSG_INTERFACE_MSG_TO_NODE,"Expedition",6, "UPGRADES", sImageName);

	sImageName = "Goldmines";
	if(CheckForCompletedUpgrade(UPGRADE_GOLDMINES, iColony) == true)
	{
		EI_CreateSubstrate("UPGRADES_BORDERS", "skillborder_silver",515,517,649,541);
		SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"GOLDMINES_BORDER", 1, argb(0,32,32,32), argb(150,100,100,100));
	}
	else
	{
		if(CheckForUpdate(UPGRADE_GOLDMINES, iColony) != true)
		{
			EI_CreateSubstrate("UPGRADES_BORDERS", "skillborder",515,517,649,541);
			SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"GOLDMINES_BORDER", 1, argb(160,32,32,32), argb(150,100,100,100));
		}
		else
		{
			EI_CreateSubstrate("UPGRADES_BORDERS", "skillborder_green",515,517,649,541);
			SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"GOLDMINES_BORDER", 1, argb(0,32,32,32), argb(150,100,100,100));
		}
	}
	sText = XI_ConvertString(ColonyUpgrades[UPGRADE_GOLDMINES].(sStep).name);
	SendMessage(&GameInterface,"lslsssllllllfl", MSG_INTERFACE_MSG_TO_NODE,"MAIN_WINDOW_STRINGS",0,
		"Goldmines",sText, FONT_NORMAL,582,522, COLOR_NORMAL,0, 
		SCRIPT_ALIGN_CENTER, true, 0.8, 420);
	SendMessage(&GameInterface,"lslss", MSG_INTERFACE_MSG_TO_NODE,"Goldmines",6, "UPGRADES", sImageName);

	sImageName = "Silvermines";
	if(CheckForCompletedUpgrade(UPGRADE_SILVERMINES, iColony) == true)
	{
		EI_CreateSubstrate("UPGRADES_BORDERS", "skillborder_silver",653,517,787,541);
		SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"SILVERMINES_BORDER", 1, argb(0,32,32,32), argb(150,100,100,100));
	}
	else
	{
		if(CheckForUpdate(UPGRADE_SILVERMINES, iColony) != true)
		{
			EI_CreateSubstrate("UPGRADES_BORDERS", "skillborder",653,517,787,541);
			SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"SILVERMINES_BORDER", 1, argb(160,32,32,32), argb(150,100,100,100));
		}
		else
		{
			EI_CreateSubstrate("UPGRADES_BORDERS", "skillborder_green",653,517,787,541);
			SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"SILVERMINES_BORDER", 1, argb(0,32,32,32), argb(150,100,100,100));
		}
	}
	sText = XI_ConvertString(ColonyUpgrades[UPGRADE_SILVERMINES].(sStep).name);
	SendMessage(&GameInterface,"lslsssllllllfl", MSG_INTERFACE_MSG_TO_NODE,"MAIN_WINDOW_STRINGS",0,
		"Silvermines",sText, FONT_NORMAL,718,522, COLOR_NORMAL,0, 
		SCRIPT_ALIGN_CENTER, true, 0.8, 420);
	SendMessage(&GameInterface,"lslss", MSG_INTERFACE_MSG_TO_NODE,"Silvermines",6, "UPGRADES", sImageName);

	if(iFFMode == MAIN_WINDOW_FIRST)
	{
		EI_CreateFrame("UPGRADE_BORDERS", 377,83,511,197);
		EI_CreateFrame("UPGRADE_BORDERS", 515,83,649,197);
		EI_CreateFrame("UPGRADE_BORDERS", 653,83,787,197);

		EI_CreateFrame("UPGRADE_BORDERS", 377,199,511,313);
		EI_CreateFrame("UPGRADE_BORDERS", 515,199,649,313);
		EI_CreateFrame("UPGRADE_BORDERS", 653,199,787,313);

		EI_CreateFrame("UPGRADE_BORDERS", 377,315,511,429);
		EI_CreateFrame("UPGRADE_BORDERS", 515,315,649,429);
		EI_CreateFrame("UPGRADE_BORDERS", 653,315,787,429);

		EI_CreateFrame("UPGRADE_BORDERS", 377,431,511,545);
		EI_CreateFrame("UPGRADE_BORDERS", 515,431,649,545);
		EI_CreateFrame("UPGRADE_BORDERS", 653,431,787,545);

		iFFMode = MAIN_WINDOW_SECOND;
	}
}

void Upgrade()
{
	string sNode = GetCurrentNode();
	string sUpgrade, sUpgradeResult;

	sUpgrade = sNode;
	sUpgradeResult = sUpgrade;

	int iImageNumber;

	if(sUpgrade == "Fort")
	{
		iImageNumber = sti(colonies[iColony].fort);
		if(iImageNumber < 4)
		{
			sUpgradeResult = "Fort"+(iImageNumber+1);
		}
		else
		{
			sUpgradeResult = "Fort"+iImageNumber;
		}
	}

	if(sUpgrade == "Shipyard")
	{
		iImageNumber = sti(colonies[iColony].shipyard);
		if(iImageNumber < 4)
		{
			sUpgradeResult = "Shipyard"+(iImageNumber+1);
		}
		else
		{
			sUpgradeResult = "Shipyard"+iImageNumber;
		}
	}


	EI_CreateFrame("CONFIRM_BORDERS", 0,0,400,450);

	XI_WindowShow("BUILD_CONFIRM", true);
	XI_WindowDisable("BUILD_CONFIRM", false);
	XI_WindowDisable("MAIN_WINDOW", true);

	if(iShowMode == MODE_SHOW)
	{
		SetNodeUsing("CONFIRM_YES_BUTTON", false);
		SetNodeUsing("CONFIRM_NO_BUTTON", false);
	}
	else
	{
		SetNodeUsing("CONFIRM_YES_BUTTON", true);
		SetNodeUsing("CONFIRM_NO_BUTTON", true);
	}

	EI_CreateFrame("CONFIRM_BORDERS", 134,35,268,125);

	iUpgradeNum = FindUpgrade(sNode);

	SendMessage(&GameInterface,"lslss", MSG_INTERFACE_MSG_TO_NODE, "UPGRADE_PICTURE" ,6, "UPGRADES", sUpgradeResult);

	string sText = XI_ConvertString("Build") + ": " + XI_ConvertString(GetNextUpgradeNameFromColony(sNode, iColony));
	SendMessage(&GameInterface,"lslsssllllllfl", MSG_INTERFACE_MSG_TO_NODE,"CONFIRM_WINDOW_STRINGS",0,
		"Caption",sText, FONT_CAPTION,410,82, COLOR_NORMAL,0, 
		SCRIPT_ALIGN_CENTER, true, 1.0, 420);

	sText = GetUpgradeNextText(iUpgradeNum, iColony);
	SetFormatedText("CONFIRM_TEXTWINDOW_UP", sText);

	SetFormatedText("CONFIRM_TEXTWINDOW_MIDDLE", "");
	SetFormatedText("CONFIRM_TEXTWINDOW_DOWN", "");

	string sDescr = "";
	string sRequired;
	int iRequredUpgrade;
	bool isConinue = false;
	int iUpgradeLevel;
	string sString;	
	int iNeedMoney;
	
	iUpgradeLevel = sti(colonies[iColony].(sNode));

	iUpgradeLevel = iUpgradeLevel + 1;

	if(iUpgradeLevel < 1)
	{
		iUpgradeLevel = 1;
	}

	string sLevel = "l"+iUpgradeLevel;
	if(sti(colonies[iColony].(sNode)) == sti(ColonyUpgrades[iUpgradeNum].maxlevel))
	{
		sDescr = XI_ConvertString("This building is already built");
		SendMessage(&GameInterface,"lsle",MSG_INTERFACE_MSG_TO_NODE,"CONFIRM_TEXTWINDOW_DOWN", 0,&sDescr);
		SetSelectable("CONFIRM_YES_BUTTON",false);
	}
	else
	{
		if(sti(colonies[iColony].(sNode)) == -1 && sti(colonies[iColony].expedition) == 1)
		{
			sDescr = XI_ConvertString("On island are not discovered metalls");
			SendMessage(&GameInterface,"lsle",MSG_INTERFACE_MSG_TO_NODE,"CONFIRM_TEXTWINDOW_DOWN", 0,&sDescr);
			SetSelectable("CONFIRM_YES_BUTTON",false);
		}
		else
		{
			if(sti(ColonyUpgrades[iUpgradeNum].(sLevel).money) > sti(colonies[iColony].money))
			{
				iNeedMoney = sti(ColonyUpgrades[iUpgradeNum].(sLevel).money) - sti(colonies[iColony].money);

				sDescr = XI_ConvertString("Price of build") + " <color=255,255,128,128>" + ColonyUpgrades[iUpgradeNum].(sLevel).money + "</color>.";
				SendMessage(&GameInterface,"lsle",MSG_INTERFACE_MSG_TO_NODE,"CONFIRM_TEXTWINDOW_MIDDLE", 0,&sDescr);

				sDescr = XI_ConvertString("Colonies comes short") + FindRussianMoneyString(iNeedMoney);
				SendMessage(&GameInterface,"lsle",MSG_INTERFACE_MSG_TO_NODE,"CONFIRM_TEXTWINDOW_MIDDLE", 0,&sDescr);
			}
			else
			{
				sDescr = XI_ConvertString("Price of build") + " <color=255,128,255,128>" + ColonyUpgrades[iUpgradeNum].(sLevel).money + "</color>.";
				SendMessage(&GameInterface,"lsle",MSG_INTERFACE_MSG_TO_NODE,"CONFIRM_TEXTWINDOW_MIDDLE", 0,&sDescr);
			}

			if(CheckForUpdate(iUpgradeNum, iColony) == true)
			{
				SetSelectable("CONFIRM_YES_BUTTON",true);
				sDescr = XI_ConvertString("All condition for givenned buildings are executed");
				SendMessage(&GameInterface,"lsle",MSG_INTERFACE_MSG_TO_NODE,"CONFIRM_TEXTWINDOW_DOWN", 0,&sDescr);
			}
			else
			{
				SetSelectable("CONFIRM_YES_BUTTON",false);

				sDescr = XI_ConvertString("It Is Required for build");
				SendMessage(&GameInterface,"lsle",MSG_INTERFACE_MSG_TO_NODE,"CONFIRM_TEXTWINDOW_DOWN", 0,&sDescr);

				sString = sti(ColonyUpgrades[iUpgradeNum].colonylevel)*iUpgradeLevel;
				if((sti(ColonyUpgrades[iUpgradeNum].colonylevel)*iUpgradeLevel) > sti(colonies[iColony].colonylevel))
				{
					sDescr = XI_ConvertString("Colony Level") + "<color=255,255,128,128>" + XI_ConvertString("ColonyLevel" + sString) +  + "</color>.";
					SendMessage(&GameInterface,"lsle",MSG_INTERFACE_MSG_TO_NODE,"CONFIRM_TEXTWINDOW_DOWN", 0,&sDescr);
				}

				string sStep, sResult;
				aref aUpgrades, rUpgrade;
				
				int iLevel = sti(colonies[iColony].(sUpgrade)) + 1;
				if(iLevel > sti(ColonyUpgrades[iUpgradeNum].maxlevel))
				{
					iLevel = iLevel - 1;
				}
				sStep = "l"+iLevel;

				makearef(aUpgrades, ColonyUpgrades[iUpgradeNum].requred.(sStep));
				
				int iStep = GetAttributesNum(aUpgrades);

				string stempStep;
				int iTempStep;

				string sLoyality;

				for(int i=0; i <iStep; i++)
				{
					rUpgrade = GetAttributeN(aUpgrades, i);

					sResult = GetAttributeName(rUpgrade);
					
					//sDescr = ColonyUpgrades[iUpgradeNum].requred.(sStep).
					if(sResult != "population" && sResult != "loyality")
					{
						iTempStep = GetAttributeValue(rUpgrade);
						stempStep = "l"+iTempStep;
						//sResult = GetUpgradeName(FindUpgrade(sResult), iTempStep);
						if(sti(colonies[iColony].(sResult)) != sti(ColonyUpgrades[iUpgradeNum].requred.(sStep).(sResult)))
						{
							sResult = GetUpgradeName(FindUpgrade(sResult), iTempStep);
							sDescr = XI_ConvertString("BuildingsList") + "<color=255,255,128,128>" + XI_ConvertString(sResult) + "</color>.";
							SendMessage(&GameInterface,"lsle",MSG_INTERFACE_MSG_TO_NODE,"CONFIRM_TEXTWINDOW_DOWN", 0,&sDescr);
						}
					}

					if(sResult == "population" && sti(colonies[iColony].population) < sti(ColonyUpgrades[iUpgradeNum].requred.(sStep).population))
					{
						sDescr = XI_ConvertString("Colony Population") + "<color=255,255,128,128>" + ColonyUpgrades[iUpgradeNum].requred.(sStep).population + "</color>.";
						SendMessage(&GameInterface,"lsle",MSG_INTERFACE_MSG_TO_NODE,"CONFIRM_TEXTWINDOW_DOWN", 0,&sDescr);

						sDescr = XI_ConvertString("Current Colony Population") + colonies[iColony].population + ".";
						SendMessage(&GameInterface,"lsle",MSG_INTERFACE_MSG_TO_NODE,"CONFIRM_TEXTWINDOW_DOWN", 0,&sDescr);
					}

					if(sResult == "loyality" && sti(colonies[iColony].loyality) < sti(ColonyUpgrades[iUpgradeNum].requred.(sStep).loyality))
					{
						sLoyality = GetMoraleName(sti(ColonyUpgrades[iUpgradeNum].requred.(sStep).loyality));
						sDescr = XI_ConvertString("Loyality for Colony") + "<color=255,255,128,128>" + XI_ConvertString(sLoyality) + "</color>.";
						SendMessage(&GameInterface,"lsle",MSG_INTERFACE_MSG_TO_NODE,"CONFIRM_TEXTWINDOW_DOWN", 0,&sDescr);

						sDescr = XI_ConvertString("Current Loyality for Colony") + XI_ConvertString(GetMoraleName(sti(colonies[iColony].loyality))) + ".";
						SendMessage(&GameInterface,"lsle",MSG_INTERFACE_MSG_TO_NODE,"CONFIRM_TEXTWINDOW_DOWN", 0,&sDescr);
					}
				}
			}
		}
	}
	//SendMessage(&GameInterface,"lsl",MSG_INTERFACE_MSG_TO_NODE,"CONFIRM_TEXTWINDOW_DOWN", 5);
	SendMessage(&GameInterface,"lsl",MSG_INTERFACE_MSG_TO_NODE,"CONFIRM_TEXTWINDOW_UP", 5);
	SendMessage(&GameInterface,"lsl",MSG_INTERFACE_MSG_TO_NODE,"CONFIRM_TEXTWINDOW_MIDDLE", 5);
}

void Build()
{
	/*EI_CreateFrame("CONFIRM_BORDERS", 200,150,600,500);

	XI_WindowShow("BUILD_CONFIRM", true);
	XI_WindowDisable("BUILD_CONFIRM", false);

	string sText = "Вы уверены, что хотите построить " + ColonyUpgrades[iUpgradeNum].name + ", заплатив " + ColonyUpgrades[iUpgradeNum].(sLevel).money + "?";
	SetFormatedText("TEXTWINDOW",sText);
	*/
}

void HideBuildConfirm()
{
	XI_WindowShow("BUILD_CONFIRM", false);
	XI_WindowDisable("BUILD_CONFIRM", true);
	SetCurrentNode("COLONY_SCROLL");
	XI_WindowDisable("MAIN_WINDOW", false);
}

void ConfirmBuildYes()
{
	BuildUpgrade(iColony, iUpgradeNum, true);
	HideBuildConfirm();
	SetVariable();

	iFFMode = MAIN_WINDOW_FIRST;
	FillUpgradesImages();
}

void HideAllWindows()
{
	if(iShowMode == MODE_SHOW)
	{
		XI_WindowShow("BUILD_CONFIRM", false);
		XI_WindowDisable("BUILD_CONFIRM", true);
		XI_WindowShow("INFO_WINDOW", false);
		XI_WindowDisable("INFO_WINDOW", true);
		XI_WindowDisable("MAIN_WINDOW", false);
		iShowMode = MODE_NO;
	}
}

void UpgradeBuy()
{
	iShowMode = MODE_MAIN;
	Upgrade();
}

void UpgradeShow()
{
	iShowMode = MODE_SHOW;
	Upgrade();
}

void ShowColonyInfo()
{
	iShowMode = MODE_SHOW;
	string sText;
	switch(curCntrlIdx)
	{
		case 0:
			sText = "Губернатор колонии.";
		break;

		case 1:
			sText = "Уровень колонии.";
		break;

		case 2:
			sText = "Население колонии.";
		break;

		case 3:
			sText = "Золото колонии.";
		break;

		case 4:
			sText = "Мораль колонии.";
		break;

		case 5:
			sText = "Лояльность колонии.";
		break;

		case 6:
			sText = "Матросов в колонии.";
		break;

		case 7:
			sText = "Гарнизон форта.";
		break;

		case 8:
			sText = "Агрессия колонии.";
		break;

		case 9:
			sText = "Эпидемия на колонии.";
		break;
	}
	ShowInfoWindow(sText);
}

void ShowInfoWindow(string sText)
{
	XI_WindowShow("INFO_WINDOW", true);
	XI_WindowDisable("INFO_WINDOW", false);

	EI_CreateFrame("INFO_BORDERS", 200,220,600,380);

	SetFormatedText("INFO_WINDOW_STRINGS", sText);

	SendMessage(&GameInterface,"lsl",MSG_INTERFACE_MSG_TO_NODE,"INFO_WINDOW_STRINGS", 5);
}

void FStartColonyAction()
{
	int nCommand = GetEventData();
	string sNodName = GetEventData();

	if( sNodName == "SET_Hovernor_BUTTON" ) { ShowHovernorAssingWindow(true); }
	if( sNodName == "EXCHANGE_MONEY_BUTTON" ) { ShowColonyMoneyExchangeWindow(true); }
}

void FEndMoneyExchange()
{
	int nCommand = GetEventData();
	string sNodName = GetEventData();
	string sText;

	if( nCommand!=ACTION_ACTIVATE && nCommand!=ACTION_MOUSECLICK || sNodName!="EXCHANGE_MONEY_YES" ) 
	{
		StoreMoneyData( false );

	} else {
		// change money data
		sText = colonies[iColony].money;
		sText = sText + " ";
		SendMessage(&GameInterface,"lslle",MSG_INTERFACE_MSG_TO_NODE,"CHARACTER_INFO", 10,3,&sText);
		FillUpgradesImages();
	}

	ShowColonyMoneyExchangeWindow(false);
}

void FMoneyChange()
{
	int nCommand = GetEventData();
	string sNodName = GetEventData();
	switch( sNodName )
	{
	case "EXCHANGE_MONEY_ONETAKE": MoneyFromOurToColony(-1); break;
	case "EXCHANGE_MONEY_ONEGIVE": MoneyFromOurToColony(1); break;
	case "EXCHANGE_MONEY_ALLTAKE": MoneyFromOurToColony(-sti(colonies[iColony].money)); break;
	case "EXCHANGE_MONEY_ALLGIVE": MoneyFromOurToColony(sti(xi_refCharacter.money)); break;
	}
}

void FSlideChange()
{
	string sNodeName = GetEventData();
	int nVal = GetEventData();
	float fVal = GetEventData();

	MoneyFromOurToColony(nVal - sti(colonies[iColony].money));
}

void FEndHovernorSet()
{
	//int nCommand = GetEventData();
	//string sNodName = GetEventData();
	ShowHovernorAssingWindow(false);
}

void FHovernorAction()
{
	int nCommand = GetEventData();
	string sNodName = GetEventData();
	if( sNodName == "SETHovernor_INFO_CURRENT" ) {
		ShowHovernorInfo(-1);
		return;
	}
	if( sNodName=="SETHovernor_PASSENGER_LIST" || sNodName=="SETHovernor_PASSENGER_UP" || sNodName=="SETHovernor_PASSENGER_DOWN" ) {
		ShowHovernorInfo(nCurPassengerScrollNum);
		return;
	}
	if( sNodName == "SETHovernor_REPLACE" ) {
		ChangeHovernor();
	}
}

void MoneyFromOurToColony(int nMoney)
{
	if( nMoney>0 && sti(xi_refCharacter.money)<nMoney ) {
		nMoney = sti(xi_refCharacter.money);
	}
	if( nMoney<0 && sti(colonies[iColony].money)<-nMoney ) {
		nMoney = -sti(colonies[iColony].money);
	}
	if( nMoney!=0 ) {
		colonies[iColony].money = sti(colonies[iColony].money) + nMoney;
		xi_refCharacter.money = sti(xi_refCharacter.money) - nMoney;
		SetMoneyData();
	}
}

void ShowHovernorAssingWindow(bool bShow)
{
	if( bShow ) {
		XI_WindowDisable("MAIN_WINDOW", true);
		XI_WindowShow("SETHOVERNOR_WINDOW", true);
		XI_WindowDisable("SETHOVERNOR_WINDOW", false);
		SetCurrentNode("SETHOVERNOR_NO");
		ShowHovernorInfo(-1);
		SetCurrentNode("SETHOVERNOR_PASSENGER_LIST");
	} else {
		XI_WindowDisable("SETHOVERNOR_WINDOW", true);
		XI_WindowShow("SETHOVERNOR_WINDOW", false);
		XI_WindowDisable("MAIN_WINDOW", false);
		SetCurrentNode("SET_HOVERNOR_BUTTON");
	}
}

void ShowColonyMoneyExchangeWindow(bool bShow)
{
	if( bShow ) {
		XI_WindowDisable("MAIN_WINDOW", true);
		XI_WindowShow("EXCHANGE_MONEY_WINDOW", true);
		XI_WindowDisable("EXCHANGE_MONEY_WINDOW", false);
		SetCurrentNode("EXCHANGE_MONEY_NO");
		StoreMoneyData( true );
		int nAllMoney = sti(xi_refCharacter.money) + sti(colonies[iColony].money);
		SendMessage( &GameInterface,"lsll",MSG_INTERFACE_MSG_TO_NODE,"EXCHANGE_MONEY_SLIDER", 1, nAllMoney );
		SetMoneyData();
	} else {
		XI_WindowDisable("EXCHANGE_MONEY_WINDOW", true);
		XI_WindowShow("EXCHANGE_MONEY_WINDOW", false);
		XI_WindowDisable("MAIN_WINDOW", false);
		SetCurrentNode("EXCHANGE_MONEY_BUTTON");
	}
}

void SetMoneyData()
{
	string sTmp;
	sTmp = xi_refCharacter.money;
	SetFormatedText("EXCHANGE_MONEY_TEXT_MY",sTmp);
	sTmp = colonies[iColony].money;
	SetFormatedText("EXCHANGE_MONEY_TEXT_COLONY",sTmp);

	SendMessage( &GameInterface,"lsll",MSG_INTERFACE_MSG_TO_NODE,"EXCHANGE_MONEY_SLIDER", 2, sti(colonies[iColony].money) );
}

void StoreMoneyData(bool bStore)
{
	if( bStore ) {
		obj_CM_StoreData.mymoney = xi_refCharacter.money;
		obj_CM_StoreData.colonymoney = colonies[iColony].money;
	} else {
		xi_refCharacter.money = obj_CM_StoreData.mymoney;
		colonies[iColony].money = obj_CM_StoreData.colonymoney;
	}
}

void FormatedTextActivate()
{
	string nodName = GetEventData();
	curCntrlIdx = GetEventData(); // номер текста
}

void FillPassengerScroll()
{
	int i;
	string attributeName,PsgAttrName;
	int _curCharIdx;
	aref pRef,pRef2;

	DeleteAttribute(&GameInterface, "SETHovernor_PASSENGER_LIST");
	makearef(pRef,pchar.Fellows.Passengers);

	nCurPassengerScrollNum = -1;
	GameInterface.SETHOVERNOR_PASSENGER_LIST.current = 0;
	GameInterface.SETHOVERNOR_PASSENGER_LIST.BadPicture1 = "interfaces\empty_face.tga";
	GameInterface.SETHOVERNOR_PASSENGER_LIST.NotUsed = 6;

	int listsize = 0;

	int nListSize = GetPassengersQuantity(pchar);
	int nListSizeFree = GetFreePassengersQuantity(pchar);

	FillFaceList("SETHOVERNOR_PASSENGER_LIST.ImagesGroup", pchar, 2); // passengers

	int m = 0;
	for(i=0; i<nListSize; i++)
	{
		attributeName = "pic" + (m+1);
		PsgAttrName = "id"+(i+1);
		makearef(pRef2,GameInterface.SETHOVERNOR_PASSENGER_LIST.(attributeName));

		_curCharIdx = sti(pRef.(PsgAttrName));

		if(_curCharIdx!=-1 && !CheckAttribute(&characters[_curCharIdx], "isfree") && !CheckAttribute(&characters[_curCharIdx], "isquest") && sti(characters[_curCharIdx].prisoned) != 1)
		{
			GameInterface.SETHOVERNOR_PASSENGER_LIST.(attributeName).character = _curCharIdx;
			GameInterface.SETHOVERNOR_PASSENGER_LIST.(attributeName).img1 = GetFacePicName(GetCharacter(_curCharIdx));
			GameInterface.SETHOVERNOR_PASSENGER_LIST.(attributeName).tex1 = FindFaceGroupNum("SETHOVERNOR_PASSENGER_LIST.ImagesGroup","FACE128_"+Characters[_curCharIdx].FaceID);
			m++;
		}
	}

	GameInterface.SETHOVERNOR_PASSENGER_LIST.ListSize = m;
}

void ShowHovernorInfo( int nIdx )
{
	int charidx = -1;
	if( nIdx>=0 ) {
		// показать пассажира
		string attributeName = "pic" + (nIdx+1);
		if( CheckAttribute(&GameInterface,"SETHovernor_PASSENGER_LIST."+attributeName+".character") ) {
			charidx = sti(GameInterface.SETHOVERNOR_PASSENGER_LIST.(attributeName).character);
		}
		if( charidx >= 0 ) {
			SendMessage( &GameInterface,"lslls",MSG_INTERFACE_MSG_TO_NODE,"SETHOVERNOR_STRINGS", 1, 3, "#("+XI_ConvertString("passengership")+")" );
		} else {
			nIdx = -1;
		}
	}
	if( nIdx<0 ) {
		// показать текущего губернатора
		charidx = GetCharacterIndex( colonies[iColony].commander );
		SendMessage( &GameInterface,"lslls",MSG_INTERFACE_MSG_TO_NODE,"SETHOVERNOR_STRINGS", 1, 3, "#("+XI_ConvertString("Hovernor")+")" );
	}

	if( charidx >= 0 ) {
		SendMessage( &GameInterface,"lslls",MSG_INTERFACE_MSG_TO_NODE,"SETHOVERNOR_STRINGS", 1, 2, "#"+GetCharacterFullName(characters[charidx].id) );
		SendMessage( &GameInterface,"lslls",MSG_INTERFACE_MSG_TO_NODE,"SETHOVERNOR_STRINGS", 1, 4, "#"+characters[charidx].skill.leadership );
		SendMessage( &GameInterface,"lslls",MSG_INTERFACE_MSG_TO_NODE,"SETHOVERNOR_STRINGS", 1, 5, "#"+characters[charidx].skill.sailing );
		SendMessage( &GameInterface,"lslls",MSG_INTERFACE_MSG_TO_NODE,"SETHOVERNOR_STRINGS", 1, 6, "#"+characters[charidx].skill.tactic );
		SendMessage( &GameInterface,"lslls",MSG_INTERFACE_MSG_TO_NODE,"SETHOVERNOR_STRINGS", 1, 7, "#"+characters[charidx].skill.fencing );
		SendMessage( &GameInterface,"lslls",MSG_INTERFACE_MSG_TO_NODE,"SETHOVERNOR_STRINGS", 1, 8, "#"+characters[charidx].skill.gun );
		SendMessage( &GameInterface,"lslls",MSG_INTERFACE_MSG_TO_NODE,"SETHOVERNOR_STRINGS", 1, 9, "#"+characters[charidx].skill.cannons );
		SendMessage( &GameInterface,"lslls",MSG_INTERFACE_MSG_TO_NODE,"SETHOVERNOR_STRINGS", 1, 10, "#"+characters[charidx].skill.accuracy );
		SendMessage( &GameInterface,"lslls",MSG_INTERFACE_MSG_TO_NODE,"SETHOVERNOR_STRINGS", 1, 11, "#"+characters[charidx].skill.grappling );
		SendMessage( &GameInterface,"lslls",MSG_INTERFACE_MSG_TO_NODE,"SETHOVERNOR_STRINGS", 1, 12, "#"+characters[charidx].skill.defence );
		SendMessage( &GameInterface,"lslls",MSG_INTERFACE_MSG_TO_NODE,"SETHOVERNOR_STRINGS", 1, 13, "#"+characters[charidx].skill.repair );
		SendMessage( &GameInterface,"lslls",MSG_INTERFACE_MSG_TO_NODE,"SETHOVERNOR_STRINGS", 1, 14, "#"+characters[charidx].skill.commerce );
		SendMessage( &GameInterface,"lslls",MSG_INTERFACE_MSG_TO_NODE,"SETHOVERNOR_STRINGS", 1, 15, "#"+characters[charidx].skill.sneak );

		SetNewPicture("SETHovernor_CHARACTER_BIG_PICTURE", "interfaces\portraits\256\face_" + characters[charidx].FaceId + ".tga");
	} else {
		SendMessage( &GameInterface,"lslls",MSG_INTERFACE_MSG_TO_NODE,"SETHOVERNOR_STRINGS", 1, 2, "empty" );
		SendMessage( &GameInterface,"lslls",MSG_INTERFACE_MSG_TO_NODE,"SETHOVERNOR_STRINGS", 1, 4, "#0" );
		SendMessage( &GameInterface,"lslls",MSG_INTERFACE_MSG_TO_NODE,"SETHOVERNOR_STRINGS", 1, 5, "#0" );
		SendMessage( &GameInterface,"lslls",MSG_INTERFACE_MSG_TO_NODE,"SETHOVERNOR_STRINGS", 1, 6, "#0" );
		SendMessage( &GameInterface,"lslls",MSG_INTERFACE_MSG_TO_NODE,"SETHOVERNOR_STRINGS", 1, 7, "#0" );
		SendMessage( &GameInterface,"lslls",MSG_INTERFACE_MSG_TO_NODE,"SETHOVERNOR_STRINGS", 1, 8, "#0" );
		SendMessage( &GameInterface,"lslls",MSG_INTERFACE_MSG_TO_NODE,"SETHOVERNOR_STRINGS", 1, 9, "#0" );
		SendMessage( &GameInterface,"lslls",MSG_INTERFACE_MSG_TO_NODE,"SETHOVERNOR_STRINGS", 1, 10, "#0" );
		SendMessage( &GameInterface,"lslls",MSG_INTERFACE_MSG_TO_NODE,"SETHOVERNOR_STRINGS", 1, 11, "#0" );
		SendMessage( &GameInterface,"lslls",MSG_INTERFACE_MSG_TO_NODE,"SETHOVERNOR_STRINGS", 1, 12, "#0" );
		SendMessage( &GameInterface,"lslls",MSG_INTERFACE_MSG_TO_NODE,"SETHOVERNOR_STRINGS", 1, 13, "#0" );
		SendMessage( &GameInterface,"lslls",MSG_INTERFACE_MSG_TO_NODE,"SETHOVERNOR_STRINGS", 1, 14, "#0" );
		SendMessage( &GameInterface,"lslls",MSG_INTERFACE_MSG_TO_NODE,"SETHOVERNOR_STRINGS", 1, 15, "#0" );

		SetNewPicture("SETHOVERNOR_CHARACTER_BIG_PICTURE", "interfaces\empty_face.tga");
	}
}

void ChangeHovernor()
{
	if( nCurPassengerScrollNum < 0 ) return;
	string attributeName = "pic" + (nCurPassengerScrollNum+1);
	if( !CheckAttribute(&GameInterface,"SETHOVERNOR_PASSENGER_LIST."+attributeName+".character") ) {
		return;
	}
	int charidx = sti(GameInterface.SETHOVERNOR_PASSENGER_LIST.(attributeName).character);
	if( charidx<0 ) {
		return;
	}
	RemovePassenger(pchar, &characters[charidx]);

	int nCommanderIdx = GetCharacterIndex( colonies[iColony].commander );
	if( nCommanderIdx >= 0 ) {
		AddPassenger(pchar,&characters[nCommanderIdx],false);
	}

// to_do	CreateGovernor(&characters[charidx], sColony);

	FillPassengerScroll();
	SendMessage(&GameInterface,"lsl",MSG_INTERFACE_SCROLL_CHANGE,"SETHOVERNOR_PASSENGER_LIST",-1);
	ShowHovernorInfo(-1);
}
