
object ColonyList[MAX_COLONIES];
int playership_x, playership_z;

void InitInterface(string iniName)
{
  GameInterface.title = "titlePaperMap";

	string sColony, sPic;
	int x, y;

	for(int i = 0; i < MAX_COLONIES; i++)
	{
		sColony = colonies[i].id;
		x = sti(colonies[i].map.x);
		y = sti(colonies[i].map.y);
		if(colonies[i].nation != "none")
		{
			sPic = GetNationNameByType(sti(colonies[i].nation));
		}
		else
		{
			sPic = "select_nation";
		}
		if(sColony == "IslaMona")
		{
			sPic = "Pirate";
		}
	
		if(sPic != "select_nation")
		{
			GameInterface.MAP.imagelist.(sColony).group = "MAIN_ICONS";
			GameInterface.MAP.imagelist.(sColony).x = x;
			GameInterface.MAP.imagelist.(sColony).y = y;
			GameInterface.MAP.imagelist.(sColony).pic = sPic;
			GameInterface.MAP.imagelist.(sColony).width = 12;
			GameInterface.MAP.imagelist.(sColony).height = 12;
		}
	}
	
	x = sti(worldMap.playerShipX)/1.66;
	x = 1024 + x - 5;
	y = sti(worldMap.playerShipZ)/1.66;
	y = 1024 - y - 5;

	GameInterface.MAP.imagelist.PShip.group = "FRENDLY";
	GameInterface.MAP.imagelist.PShip.x = x;
	GameInterface.MAP.imagelist.PShip.y = y;
	GameInterface.MAP.imagelist.PShip.pic = "frendly";
	GameInterface.MAP.imagelist.PShip.width = 32;
	GameInterface.MAP.imagelist.PShip.height = 32;

	GameInterface.MAP.centerx = x;
	GameInterface.MAP.centery = y;

	SendMessage(&GameInterface,"ls",MSG_INTERFACE_INIT,iniName);

	//SendMessage(&GameInterface,"lsllff",MSG_INTERFACE_MSG_TO_NODE, "MAP", -1, 10, x, y);

  
	CreateString(true,"MapsListCaption",XI_ConvertString("MapsList"),FONT_CAPTION,COLOR_NORMAL,714,55,SCRIPT_ALIGN_CENTER,1.0);

	SetEventHandler("InterfaceBreak","ProcessBreakExit",0);
	SetEventHandler("exitCancel","ProcessCancelExit",0);
	SetEventHandler("ievnt_command","ProcCommand",0);
	SetEventHandler("evntDoPostExit","DoPostExit",0);

	SetEventHandler("SelectRColony","SelectRColony",0);
	SetEventHandler("MouseRClickUP", "HideRColony",0);
}

void ProcessBreakExit()
{
	IDoExit(RC_INTERFACE_PAPER_MAP);
}

void ProcessCancelExit()
{
	IDoExit(RC_INTERFACE_PAPER_MAP);
}

void IDoExit(int exitCode)
{
	DelEventHandler("InterfaceBreak","ProcessBreakExit");
	DelEventHandler("exitCancel","ProcessCancelExit");
	DelEventHandler("ievnt_command","ProcCommand");
	DelEventHandler("evntDoPostExit","DoPostExit");

	DelEventHandler("SelectRColony","SelectRColony");
	DelEventHandler("MouseRClickUP", "HideRColony");

	interfaceResultCommand = exitCode;
	EndCancelInterface(true);
}

void ProcCommand()
{
}

void DoPostExit()
{
	int exitCode = GetEventData();
	IDoExit(exitCode);
}

void SelectRColony()
{
	float fMouseX = stf(GameInterface.mousepos.x) - 6.0 + 5;
	float fMouseY = stf(GameInterface.mousepos.y) - 50.0 + 5;

	//iMouseX = iMouseX * 1.6;

	//SendMessage(&GameInterface,"lslff",MSG_INTERFACE_MSG_TO_NODE,"MAP", 10, iMouseX, iMouseY);

	//определяем верхний левый угол картинки
	float fOffsetX = stf(GameInterface.MAP.offset.x);
	float fOffsetY = stf(GameInterface.MAP.offset.y);

	fMouseX = fOffsetX + fMouseX * stf(GameInterface.MAP.scale.x);
	fMouseY = fOffsetY + fMouseY * stf(GameInterface.MAP.scale.y);

	string sColony;
	for(int i = 0; i < MAX_COLONIES; i++)
	{
		sColony = colonies[i].id;
		if(CheckAttribute(&GameInterface, "MAP.imagelist." + sColony))
		{
			if(fMouseX >= stf(GameInterface.MAP.imagelist.(sColony).x))
			{
				if(fMouseX <= stf(GameInterface.MAP.imagelist.(sColony).x) + 12.0)
				{
					if(fMouseY >= stf(GameInterface.MAP.imagelist.(sColony).y))
					{
						if(fMouseY <= stf(GameInterface.MAP.imagelist.(sColony).y) + 12.0)
						{
							XI_WindowDisable("MAIN_WINDOW", true);
							XI_WindowDisable("INFO_WINDOW", false);
							XI_WindowShow("INFO_WINDOW", true);

							FillColonyInfo(i);
						}
					}
				}
			}
		}
	}
}

void HideRColony()
{
	XI_WindowDisable("MAIN_WINDOW", false);
	XI_WindowDisable("INFO_WINDOW", true);
	XI_WindowShow("INFO_WINDOW", false);
}

void FillColonyInfo(int iColony)
{
	string sColony = colonies[iColony].id;

	SetFormatedText("INFO_CAPTION", XI_ConvertString("Colony"+sColony));

	//string sNation = GetNationNameByType(sti(colonies[iColony].nation));
	//SetNewGroupPicture("INFO_NATION_PICTURE", "NATIONS", sNation);

	string sNation = GameInterface.MAP.imagelist.(sColony).pic;
	SetNewGroupPicture("INFO_NATION_PICTURE", "NATIONS", sNation);

	EI_CreateFrame("INFO_NATIONS_BORDERS",210,152,274,215);

	SelectColony(iColony);
}

void SelectColony(int iColony)
{
	SetFormatedText("COLONY_INFO_TEXT", "");
	SetFormatedText("COLONY_INFO", "");

	string sColony = colonies[iColony].id;

	int iColor;
	int iDistance = CalculateDistance(iColony);

	string sText;

	sText = " " + XI_ConvertString("Population");
	SendMessage(&GameInterface,"lsle",MSG_INTERFACE_MSG_TO_NODE,"COLONY_INFO_TEXT", 0,&sText);
	
	sText = " " + XI_ConvertString("Gold");
	SendMessage(&GameInterface,"lsle",MSG_INTERFACE_MSG_TO_NODE,"COLONY_INFO_TEXT", 0,&sText);

	sText = " " + XI_ConvertString("Is Level");
	SendMessage(&GameInterface,"lsle",MSG_INTERFACE_MSG_TO_NODE,"COLONY_INFO_TEXT", 0,&sText);

	sText = " " + XI_ConvertString("Is Fort");
	SendMessage(&GameInterface,"lsle",MSG_INTERFACE_MSG_TO_NODE,"COLONY_INFO_TEXT", 0,&sText);

	sText = " " + XI_ConvertString("Is Wheat field");
	SendMessage(&GameInterface,"lsle",MSG_INTERFACE_MSG_TO_NODE,"COLONY_INFO_TEXT", 0,&sText);

	sText = " " + XI_ConvertString("Is Fishing pier");
	SendMessage(&GameInterface,"lsle",MSG_INTERFACE_MSG_TO_NODE,"COLONY_INFO_TEXT", 0,&sText);

	sText = " " + XI_ConvertString("Is Mill");
	SendMessage(&GameInterface,"lsle",MSG_INTERFACE_MSG_TO_NODE,"COLONY_INFO_TEXT", 0,&sText);

	sText = " " + XI_ConvertString("Is Shipyard");
	SendMessage(&GameInterface,"lsle",MSG_INTERFACE_MSG_TO_NODE,"COLONY_INFO_TEXT", 0,&sText);

	sText = " " + XI_ConvertString("Is Academy");
	SendMessage(&GameInterface,"lsle",MSG_INTERFACE_MSG_TO_NODE,"COLONY_INFO_TEXT", 0,&sText);

	sText = " " + XI_ConvertString("is Huts");
	SendMessage(&GameInterface,"lsle",MSG_INTERFACE_MSG_TO_NODE,"COLONY_INFO_TEXT", 0,&sText);

	sText = " " + XI_ConvertString("is church");
	SendMessage(&GameInterface,"lsle",MSG_INTERFACE_MSG_TO_NODE,"COLONY_INFO_TEXT", 0,&sText);

	sText = " " + XI_ConvertString("is storehouse");
	SendMessage(&GameInterface,"lsle",MSG_INTERFACE_MSG_TO_NODE,"COLONY_INFO_TEXT", 0,&sText);

	sText = Colonies[iColony].population;
	sText = sText + " ";
	SendMessage(&GameInterface,"lsle",MSG_INTERFACE_MSG_TO_NODE,"COLONY_INFO", 0,&sText);

	sText = Colonies[iColony].money;
	sText = sText + " ";
	SendMessage(&GameInterface,"lsle",MSG_INTERFACE_MSG_TO_NODE,"COLONY_INFO", 0,&sText);

	sText = XI_ConvertString("ColonyLevel" + Colonies[iColony].colonylevel);
	sText = sText + " ";
	SendMessage(&GameInterface,"lsle",MSG_INTERFACE_MSG_TO_NODE,"COLONY_INFO", 0,&sText);
	
	sText = Colonies[iColony].fort;
	sText = sText + " ";
	SendMessage(&GameInterface,"lsle",MSG_INTERFACE_MSG_TO_NODE,"COLONY_INFO", 0,&sText);

	sText = Colonies[iColony].wheatfield;
	sText = sText + " ";
	SendMessage(&GameInterface,"lsle",MSG_INTERFACE_MSG_TO_NODE,"COLONY_INFO", 0,&sText);

	sText = Colonies[iColony].fishingpier;
	sText = sText + " ";
	SendMessage(&GameInterface,"lsle",MSG_INTERFACE_MSG_TO_NODE,"COLONY_INFO", 0,&sText);

	sText = Colonies[iColony].mill;
	sText = sText + " ";
	SendMessage(&GameInterface,"lsle",MSG_INTERFACE_MSG_TO_NODE,"COLONY_INFO", 0,&sText);

	sText = Colonies[iColony].shipyard;
	sText = sText + " ";
	SendMessage(&GameInterface,"lsle",MSG_INTERFACE_MSG_TO_NODE,"COLONY_INFO", 0,&sText);

	sText = Colonies[iColony].academy;
	sText = sText + " ";
	SendMessage(&GameInterface,"lsle",MSG_INTERFACE_MSG_TO_NODE,"COLONY_INFO", 0,&sText);

	sText = Colonies[iColony].huts;
	sText = sText + " ";
	SendMessage(&GameInterface,"lsle",MSG_INTERFACE_MSG_TO_NODE,"COLONY_INFO", 0,&sText);

	sText = Colonies[iColony].church;
	sText = sText + " ";
	SendMessage(&GameInterface,"lsle",MSG_INTERFACE_MSG_TO_NODE,"COLONY_INFO", 0,&sText);

	sText = Colonies[iColony].storehouse;
	sText = sText + " ";
	SendMessage(&GameInterface,"lsle",MSG_INTERFACE_MSG_TO_NODE,"COLONY_INFO", 0,&sText);

	if(CheckAttribute(&colonies[iColony], "info.day"))
	{
		sText = XI_ConvertString("LastInfoFromColony");
		sText = sText + colonies[iColony].info.day + " " + XI_ConvertString("target_month_" + colonies[iColony].info.month) + " " + colonies[iColony].info.year + ".";
	}
	SetFormatedText("COLONY_TIME_INFO", sText);

	sText = XI_ConvertString("ColonyDistance") + " "  + FindRussianDaysString(iDistance);
	SetFormatedText("COLONY_TRAVEL_INFO", sText);

	SetFormatedText("IMPORT_INFO", "");
	SetFormatedText("EXPORT_INFO", "");
	int iGood = -1;
	string sGood = "";

	iColor = argb(255,196,196,255);

	iGood = Colonies[iColony].Trade.Import.id1;
	sGood = goods[iGood].name;
	SetNewGroupPicture("IMPORT1_PICTURE", "GOODS", sGood);

	sGood = XI_ConvertString(sGood);
	SendMessage(&GameInterface,"lsle",MSG_INTERFACE_MSG_TO_NODE,"IMPORT_INFO", 0,&sGood);
	SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"IMPORT_INFO", 8,-1,iColor);

	iGood = Colonies[iColony].Trade.Import.id2;
	sGood = goods[iGood].name;
	SetNewGroupPicture("IMPORT2_PICTURE", "GOODS", sGood);
	
	sGood = XI_ConvertString(sGood);
	SendMessage(&GameInterface,"lsle",MSG_INTERFACE_MSG_TO_NODE,"IMPORT_INFO", 0,&sGood);
	SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"IMPORT_INFO", 8,-1,iColor);
	
	iGood = Colonies[iColony].Trade.Import.id3;
	sGood = goods[iGood].name;
	SetNewGroupPicture("IMPORT3_PICTURE", "GOODS", sGood);

	sGood = XI_ConvertString(sGood);
	SendMessage(&GameInterface,"lsle",MSG_INTERFACE_MSG_TO_NODE,"IMPORT_INFO", 0,&sGood);
	SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"IMPORT_INFO", 8,-1,iColor);

	iColor = argb(255,196,255,196);

	iGood = Colonies[iColony].Trade.Export.id1;
	sGood = goods[iGood].name;
	SetNewGroupPicture("EXPORT1_PICTURE", "GOODS", sGood);

	sGood = XI_ConvertString(sGood);
	SendMessage(&GameInterface,"lsle",MSG_INTERFACE_MSG_TO_NODE,"EXPORT_INFO", 0,&sGood);
	SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"EXPORT_INFO", 8,-1,iColor);

	iGood = Colonies[iColony].Trade.Export.id2;
	sGood = goods[iGood].name;
	SetNewGroupPicture("EXPORT2_PICTURE", "GOODS", sGood);

	sGood = XI_ConvertString(sGood);
	SendMessage(&GameInterface,"lsle",MSG_INTERFACE_MSG_TO_NODE,"EXPORT_INFO", 0,&sGood);
	SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"EXPORT_INFO", 8,-1,iColor);
	
	iGood = Colonies[iColony].Trade.Export.id3;
	sGood = goods[iGood].name;
	SetNewGroupPicture("EXPORT3_PICTURE", "GOODS", sGood);

	sGood = XI_ConvertString(sGood);
	SendMessage(&GameInterface,"lsle",MSG_INTERFACE_MSG_TO_NODE,"EXPORT_INFO", 0,&sGood);
	SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"EXPORT_INFO", 8,-1,iColor);
}

// to_Do del -->
int CalculateDistance(int iColonyNumber)
{
	int playership_x, playership_z;

	int iX = sti(worldMap.playerShipX)/1.66;
	int iZ = sti(worldMap.playerShipZ)/1.66;

	iX = 1024 + iX - 10;
	iZ = 1024 - iZ - 10;

	playership_x = iX;
	playership_z = iZ;

	string sColony = colonies[iColonyNumber].id;

	iX = colonies[iColonyNumber].map.x;
	iZ = colonies[iColonyNumber].map.y;


	int ipX = playership_x - iX;
	int ipZ = playership_z - iZ;

	if(ipX < 0)
	{
		ipX = -ipX;
	}

	if(ipZ < 0)
	{
		ipZ = -ipZ;
	}

	iX = 0;
	iZ = 0;

	int iDist = (ipX*ipX + ipZ*ipZ);

	iDist = sti(sqrt(iDist)+0.5);

	iDist = iDist/100;

	int iDistance = iDist;

	float fDist = 1.0;//  + (0.1 * makefloat(MOD_SKILL_ENEMY_RATE));

	iDistance = makefloat(iDistance) * fDist;

	iDistance = iDistance * 2.8;

	if(iDistance < 1)
	{
		iDistance = 1;
	}

	return iDistance;
}