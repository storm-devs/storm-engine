//  boal 14.02.06 меню дебугера
string totalInfo;
int idLngFile = -1;
int remInt = 0;

void InitInterface(string iniName)
{
 	StartAboveForm(true);
 	//SetTimeScale(0.0);
	//locCameraSleep(true);

	//EngineLayersOffOn(true);
	
	GameInterface.title = "titleBoal_Debug";
	
	SendMessage(&GameInterface,"ls",MSG_INTERFACE_INIT,iniName);
	
	CalculateInfoData();

	SetFormatedText("INFO_TEXT",totalInfo);//"Информация отладчика");
	SendMessage(&GameInterface,"lsl",MSG_INTERFACE_MSG_TO_NODE,"INFO_TEXT",5);


	SetEventHandler("InterfaceBreak","ProcessBreakExit",0); 
	SetEventHandler("exitCancel","ProcessCancelExit",0); 
	SetEventHandler("evntDoPostExit","DoPostExit",0); 
	SetEventHandler("ievnt_command","ProcCommand",0);
	SetEventHandler("SetScrollerPos","SetScrollerPos",0);
	SetEventHandler("ScrollPosChange","ProcScrollPosChange",0);
	SetEventHandler("ScrollTopChange","ProcScrollChange",0);

    GameInterface.reload_edit.str = "LostShipsCity_town, reload, reload1_back";
}

void ProcessBreakExit()
{
	IDoExit( RC_INTERFACE_QUICK_SAVE );
}

void ProcessCancelExit()
{
	IDoExit( RC_INTERFACE_QUICK_SAVE );
}

void IDoExit(int exitCode)
{
    EndAboveForm(true);
    //SetTimeScale(1.0);
	//locCameraSleep(false);
	
	DelEventHandler("InterfaceBreak","ProcessBreakExit");
	DelEventHandler("exitCancel","ProcessCancelExit");
	DelEventHandler("evntDoPostExit","DoPostExit");
	DelEventHandler("ievnt_command","ProcCommand");
 	DelEventHandler("SetScrollerPos","SetScrollerPos");
	DelEventHandler("ScrollPosChange","ProcScrollPosChange");
	DelEventHandler("ScrollTopChange","ProcScrollChange");

	interfaceResultCommand = exitCode;
	EndCancelInterface(true);
}


void DoPostExit()
{
	int exitCode = GetEventData();
	IDoExit(exitCode);
}

void CalculateInfoData()
{
	// тут высчитываем нужную информацию и выводим в totalInfo - Инициализация -->
	totalInfo = "Это отладчик. Позволяет выполнять заданные скриптологом функции. Кнопки: "+NewStr();
	totalInfo = totalInfo + "F1 - " + descF1 + NewStr() +
	                        "F2 - " + descF2 + NewStr() +
	                        "F3 - " + descF3 + NewStr() +
	                        "F4 - " + descF4 + NewStr() +
	                        "F5 - " + descF5 + NewStr() +
	                        "F6 - " + descF6 + NewStr() +
	                        "F7 - " + descF7 + NewStr() +
	                        "F8 - " + descF8 + NewStr() +
	                        "F9 - " + descF9 + NewStr() +
	                        "F10 - " + descF10 + NewStr() +
	                        "F11 - " + descF11 + NewStr() +
	                        "F12 - " + descF12 + NewStr() +
	                        "F13 - " + descF13 + NewStr() +
	                        "F14 - " + descF14 + NewStr() +
	                        "F15 - " + descF15 + NewStr() +
	                        "F16 - " + descF16 + NewStr() +
	                        "F17 - " + descF17 + NewStr() +
	                        "F18 - " + descF18 + NewStr() +
                            "F19 - " + descF19 + NewStr() +
                            "F20 - " + descF20 + NewStr() +
                            "F21 - " + descF21 + NewStr() +
                            "F22 - " + descF22 + NewStr() +
                            "F23 - " + descF23 + NewStr() +
	                        "F24 - " + descF24 + NewStr() +
	                        "F25 - " + descF25 + NewStr() +
	                        "F26 - " + descF26 + NewStr() +
	                        "F27 - " + descF27 + NewStr() +
	                        "F28 - " + descF28 + NewStr() +
	                        "F29 - " + descF29 + NewStr() +
	                        "F30 - " + descF30;
	// перевод строки (по другому у меня не вышло) +LanguageConvertString(idLngFile,"new_string");
    // тут высчитываем нужную информацию и выводим в totalInfo <--
}
void ProcCommand()
{
	string comName = GetEventData();
	string nodName = GetEventData();

	switch(nodName)
	{
	    case "B_F1":
		  if(comName=="activate" || comName=="click")
		  {
		      CalculateInfoDataF1();
		  }
	    break;
	    case "B_F2":
		  if(comName=="activate" || comName=="click")
		  {
		      CalculateInfoDataF2();
		  }
	    break;
	    case "B_F3":
		  if(comName=="activate" || comName=="click")
		  {
		      CalculateInfoDataF3();
		  }
	    break;
	    case "B_F4":
		  if(comName=="activate" || comName=="click")
		  {
		      CalculateInfoDataF4();
		  }
	    break;
	    case "B_F5":
		  if(comName=="activate" || comName=="click")
		  {
		      CalculateInfoDataF5();
		  }
	    break;

	    case "B_F6":
		  if(comName=="activate" || comName=="click")
		  {
		      CalculateInfoDataF6();
		  }
	    break;

	    case "B_F7":
		  if(comName=="activate" || comName=="click")
		  {
		      CalculateInfoDataF7();
		  }
	    break;

	    case "B_F8":
		  if(comName=="activate" || comName=="click")
		  {
		      CalculateInfoDataF8();
		  }
	    break;

	    case "B_F9":
		  if(comName=="activate" || comName=="click")
		  {
		      CalculateInfoDataF9();
		  }
	    break;

	    case "B_F10":
		  if(comName=="activate" || comName=="click")
		  {
		      CalculateInfoDataF10();
		  }
	    break;

	    case "B_F11":
		  if(comName=="activate" || comName=="click")
		  {
		      CalculateInfoDataF11();
		  }
	    break;

	    case "B_F12":
		  if(comName=="activate" || comName=="click")
		  {
		      CalculateInfoDataF12();
		  }
	    break;

	    case "B_F13":
		  if(comName=="activate" || comName=="click")
		  {
		      CalculateInfoDataF13();
		  }
	    break;

	    case "B_F14":
		  if(comName=="activate" || comName=="click")
		  {
		      CalculateInfoDataF14();
		  }
	    break;

	    case "B_F15":
		  if(comName=="activate" || comName=="click")
		  {
		      CalculateInfoDataF15();
		  }
	    break;

	    case "B_F16":
		  if(comName=="activate" || comName=="click")
		  {
		      CalculateInfoDataF16();
		  }
	    break;

	    case "B_F17":
		  if(comName=="activate" || comName=="click")
		  {
		      CalculateInfoDataF17();
		  }
	    break;

	    case "B_F18":
		  if(comName=="activate" || comName=="click")
		  {
		      CalculateInfoDataF18();
		  }
	    break;
	    
	    case "B_F19":
		  if(comName=="activate" || comName=="click")
		  {
		      CalculateInfoDataF19();
		  }
	    break;

        case "B_F20":
		  if(comName=="activate" || comName=="click")
		  {
		      CalculateInfoDataF20();
		  }
	    break;
	    
	    case "B_F21":
		  if(comName=="activate" || comName=="click")
		  {
		      CalculateInfoDataF21();
		  }
	    break;
	    case "B_F22":
		  if(comName=="activate" || comName=="click")
		  {
		      CalculateInfoDataF22();
		  }
	    break;
	    case "B_F23":
		  if(comName=="activate" || comName=="click")
		  {
		      CalculateInfoDataF23();
		  }
	    break;
	    case "B_F24":
		  if(comName=="activate" || comName=="click")
		  {
		      CalculateInfoDataF24();
		  }
	    break;
	    case "B_F25":
		  if(comName=="activate" || comName=="click")
		  {
		      CalculateInfoDataF25();
		  }
	    break;
	    case "B_F26":
		  if(comName=="activate" || comName=="click")
		  {
		      CalculateInfoDataF26();
		  }
	    break;
	    case "B_F27":
		  if(comName=="activate" || comName=="click")
		  {
		      CalculateInfoDataF27();
		  }
	    break;
	    case "B_F28":
		  if(comName=="activate" || comName=="click")
		  {
		      CalculateInfoDataF28();
		  }
	    break;
	    case "B_F29":
		  if(comName=="activate" || comName=="click")
		  {
		      CalculateInfoDataF29();
		  }
	    break;
	    case "B_F30":
		  if(comName=="activate" || comName=="click")
		  {
		      CalculateInfoDataF30();
		  }
	    break;
	    case "B_BETA":
		  if(comName=="activate" || comName=="click")
		  {
              IDoExit(RC_INTERFACE_DO_BOAL_BETA);
		  }
	    break;
	    case "B_RELOAD":
		  if(comName=="activate" || comName=="click")
		  {
              ReloadByStr();
		  }
	    break;

	}
}

////////////////////////////////////////////////////////////////////////
string descF1 = "Золото + 50 000";

void CalculateInfoDataF1()
{
	// тут высчитываем нужную информацию и выводим в totalInfo - Инициализация -->
	totalInfo = descF1;
	Pchar.money = sti(Pchar.money) + 50000;
	/*Pchar.quest.SetTreasureHunter.win_condition.l1          = "location";
                Pchar.quest.SetTreasureHunter.win_condition.l1.location = Pchar.location.from_sea;
                Pchar.quest.SetTreasureHunter.win_condition             = "SetTreasureHunter";
      */
	//Characters[GetCharacterIndex("Luisa Drake")].perks.list.Ciras = "1";
	totalInfo = totalInfo + GetAssembledString(" Денег у #sName#а теперь #dmoney#",	Pchar);
    // тут высчитываем нужную информацию и выводим в totalInfo <--
    totalInfo = totalInfo + NewStr() + NewStr() +
                "Команда отработала успешно!";
    SetFormatedText("INFO_TEXT",totalInfo);
}
////////////////////////////////////////////////////////////////////////
string descF2 = "CreateSiege";

void CalculateInfoDataF2()
{
	// -->
	totalInfo = descF2;
	//TraderHunterOnMap();
	CreateSiege("");
    // <
    totalInfo = totalInfo + NewStr() + NewStr() +
                "Команда отработала успешно!";
    SetFormatedText("INFO_TEXT",totalInfo);

	ProcessCancelExit();
}
////////////////////////////////////////////////////////////////////////
string descF3 = "ID текущей локации +коорд     ";

void CalculateInfoDataF3()
{
	// -->
	totalInfo = descF3;
	ref mc;
	mc = GetMainCharacter();

	float locx, locy, locz;
	if (bSeaActive && !bAbordageStarted)
	{
	    if (CheckAttribute(mc, "Ship.pos.x"))
		{
		    totalInfo = totalInfo + NewStr() + mc.location + "     ship(x,z)= "+mc.Ship.pos.x +", "+mc.Ship.pos.z;
		    if (false)//CheckAttribute(mc, "WMShip.Pos.x") && worldMap.island != "")
	        {
	            string sTemp = mc.curIslandId;
	            float r1 = stf(worldMap.islands.(sTemp).GoMapRadius);

				r1 *= WDM_MAP_TO_SEA_SCALE;
	            float d2 = GetDistance2D(stf(mc.Ship.Pos.x),
	                              stf(mc.Ship.Pos.z),
	                              stf(worldMap.islands.(sTemp).position.rx),
	                              stf(worldMap.islands.(sTemp).position.rz));

	            totalInfo = totalInfo + "         Координаты для карты radius= " + r1 + "   GetDistance2D= " + d2 + "      ";
	        }
		}
	}
	else
	{
  		if (IsEntity(loadedLocation))
  		{
			GetCharacterPos(GetMainCharacter(), &locx, &locy, &locz);
			totalInfo = totalInfo + "id = " + mc.location + " (x,y,z)= "+locx + ", " +locy + ", "+locz;
			totalInfo = totalInfo + NewStr() + "filespath.models = " + loadedLocation.filespath.models;
			totalInfo = totalInfo + NewStr() + "image = " + loadedLocation.image;
			totalInfo = totalInfo + NewStr() + "models.locators = " + loadedLocation.models.always.locators;
		}
	}
	
	//aref rootItems;
    //makearef(rootItems, worldMap);  //Islands[0]
	//DumpAttributes(rootItems);
	
	totalInfo = totalInfo + NewStr() + " MapShipX " +	worldMap.playerShipX + " MapShipZ " + worldMap.playerShipZ + " X " + worldMap.island.x + " Z " + worldMap.island.z;
	
    // <
    totalInfo = totalInfo + NewStr() + NewStr() +
                "Команда отработала успешно!";
    SetFormatedText("INFO_TEXT",totalInfo);
}
////////////////////////////////////////////////////////////////////////
string descF4 = "Опыт команды +10";

void CalculateInfoDataF4()
{
	// -->
	totalInfo = descF4;
	//locCameraLock(3);    Камеру зафиксировать на угол (мордой вперед)
	///mc.model.animation = "man";
	//mc.model = "napitan";
	//SetNewModelToChar(mc);
    ChangeCrewExp(pchar, "Sailors", 10);
	ChangeCrewExp(pchar, "Cannoners", 10);
	ChangeCrewExp(pchar, "Soldiers", 10);

	//float mhp = LAi_GetCharacterMaxHP(mc) + 5;
	//LAi_SetHP(mc,mhp,mhp);
	//totalInfo = totalInfo+ " iNumShips="+iNumShips+" iNumFantoms="+iNumFantoms;
	//ddCharacterExp(mc, 3000);
    // <
    totalInfo = totalInfo + NewStr() + NewStr() +
                "Команда отработала успешно!";
    SetFormatedText("INFO_TEXT",totalInfo);
}
////////////////////////////////////////////////////////////////////////
string descF5 = "Ранг +1 (35 скилов)";
int BOAL_debug_num = 1;
void CalculateInfoDataF5()
{
	idLngFile = LanguageOpenFile("ItemsDescribe.txt");
	// -->
	totalInfo = descF5;
    /*  pchar.PatentNation = "eng";
      GiveItem2Character(pchar, "patent_" + pchar.PatentNation);
      EquipCharacterbyItem(pchar, "patent_" + pchar.PatentNation);
      */
    pchar.Skill.FreeSkill = sti(pchar.Skill.FreeSkill) + 35;
    totalInfo = totalInfo + LanguageConvertString(idLngFile,"new_string") + LanguageConvertString(idLngFile,"new_string") +
                "Команда отработала успешно!";
    LanguageCloseFile(idLngFile);
    SetFormatedText("INFO_TEXT",totalInfo);
}
////////////////////////////////////////////////////////////////////////
string descF6 = "Супер подзорная труба, пистоль х2, сабля, мефисто, репутация герой, идол в магазине";

void CalculateInfoDataF6()
{
	idLngFile = LanguageOpenFile("ItemsDescribe.txt");
	// -->
	totalInfo = descF6;
	ref mc = GetMainCharacter();
    mc.Ship.Type = GenerateShip(SHIP_CORVETTE_QUEST, true);
    SetBaseShipData(mc);
    mc.Ship.Cannons.Type = CANNON_TYPE_CANNON_LBS32;
    SetCrewQuantityFull(mc);

    SetCharacterGoods(mc,GOOD_FOOD,2000);
	SetCharacterGoods(mc,GOOD_BALLS,2000);//2000);
    SetCharacterGoods(mc,GOOD_GRAPES,300);//700);
    SetCharacterGoods(mc,GOOD_KNIPPELS,400);//700);
    SetCharacterGoods(mc,GOOD_BOMBS,2000);//1500);
    SetCharacterGoods(mc,GOOD_POWDER,2000);
    SetCharacterGoods(mc,GOOD_PLANKS,10);
    SetCharacterGoods(mc,GOOD_RUM,40);//600);
    SetCharacterGoods(mc,GOOD_WEAPON,2000);//2000);

    if (mc.location.from_sea == "")
    {
        //setCharacterShipLocation(mc, mc.HeroParam.Location);
    }

	GiveItem2Character(mc,"spyglass5");
	GiveItem2Character(mc,"pistol7");
	aref arItm;
	Items_FindItem("pistol7",&arItm);
	arItm.chargespeed = 3;
	arItm.dmg_min = 1600.0;
	arItm.dmg_max = 2650.0;

	GiveItem2Character(mc,"blade32");
	//GiveItem2Character(mc,"indian11");
	//GiveItem2Character(mc,"cirass5");
	TakeNItems(mc,"bullet", 180);
	mc.perks.list.Gunman = "1";
	mc.perks.list.BasicDefense = "1";
	mc.perks.list.AdvancedDefense = "1";
	mc.perks.list.Ciras = "1";
	mc.perks.list.FlagPir = "1";
	mc.perks.list.FlagEng = "1";
	mc.perks.list.FlagHol = "1";
	mc.perks.list.FlagSpa = "1";
	mc.perks.list.FlagFra = "1";
	//mc.reputation = "89";

	EquipCharacterbyItem(mc, "spyglass5");
	EquipCharacterbyItem(mc, "pistol7");
 	EquipCharacterbyItem(mc, "blade32");
 	//EquipCharacterbyItem(mc, "map_good");
    // <
    totalInfo = totalInfo + LanguageConvertString(idLngFile,"new_string") + LanguageConvertString(idLngFile,"new_string") +
                "Команда отработала успешно!";
    LanguageCloseFile(idLngFile);
    SetFormatedText("INFO_TEXT",totalInfo);
}
////////////////////////////////////////////////////////////////////////
string descF7 = "Получить следующий квест.";

void CalculateInfoDataF7()
{
	idLngFile = LanguageOpenFile("ItemsDescribe.txt");
	// -->
	totalInfo = descF7;

    pchar.questTemp.Waiting_time = "0";
    pchar.questTemp.State = "empty";
    SaveCurrentQuestDateParam("questTemp");
    if (sti(pchar.questTemp.CurQuestNumber)<12)
    {
        pchar.questTemp.CurQuestNumber = sti(pchar.questTemp.CurQuestNumber)+1;
    }
    else
    {
        pchar.questTemp.CurQuestNumber = "2";
    }
    Log_SetStringToLog("Вы можете получить у Мэдифорда квест " + pchar.questTemp.CurQuestNumber + ".");
    // <
    totalInfo = totalInfo + LanguageConvertString(idLngFile,"new_string") + LanguageConvertString(idLngFile,"new_string") +
                "Команда отработала успешно!";
    LanguageCloseFile(idLngFile);
    SetFormatedText("INFO_TEXT",totalInfo);
}
////////////////////////////////////////////////////////////////////////
string descF8 = "телепорт по тавернам";

void CalculateInfoDataF8()
{
	idLngFile = LanguageOpenFile("ItemsDescribe.txt");
	// -->
	totalInfo = descF8;
	ref mc, ch;
	int n, idx;
	mc = GetMainCharacter();
    idx = GetCharIDXByParam("TavernMan", "location", mc.location);
    bool ok = true;
    while (ok)
    {
        for(n=0; n<MAX_CHARACTERS; n++)
    	{
    		makeref(ch,Characters[n]);
    		if (CheckAttribute(ch, "TavernMan"))
    		{
                if (ch.location == "none") continue; // фикс для новых, невидимых до поры островов
                if (n > idx)
                {
                    ok = false;
                    if (GetCityFrom_Sea(ch.City) != "")
                    {
                        setCharacterShipLocation(mc, GetCityFrom_Sea(ch.City));
                        setWDMPointXZ(GetCityFrom_Sea(ch.City));
                    }
                    DoQuestReloadToLocation(ch.location,"reload","reload1", "");
                    Log_Info(ch.location);
                    break;
                }
    		}
        }
        idx = -1;
    }
	// <
    totalInfo = totalInfo + LanguageConvertString(idLngFile,"new_string") + LanguageConvertString(idLngFile,"new_string") +
                "Команда отработала успешно!";
    LanguageCloseFile(idLngFile);
    SetFormatedText("INFO_TEXT",totalInfo);
}
////////////////////////////////////////////////////////////////////////
string descF9 = "Ветер == 9.5"//"DumpAttributes(&Weather)";//"Репутация в минус";

void CalculateInfoDataF9()
{
	idLngFile = LanguageOpenFile("ItemsDescribe.txt");
	// -->
	totalInfo = descF9;
    /*aref	aCurWeather = GetCurrentWeather();
    trace("----------------------GetCurrentWeather()------------------");
    DumpAttributes(aCurWeather);
    trace("--------------------DumpAttributes(&Weather)------------------");
    DumpAttributes(&Weather)
    trace("--------------------     end dump       ------------------");      */

    Weather.Wind.Speed = 9.5;
	pchar.wind.speed = Weather.Wind.Speed;
	fWeatherSpeed = stf(Weather.Wind.Speed);
	
    /*
    ref mc;
	mc = GetMainCharacter();
	if ( sti(mc.reputation) > REPUTATION_MIN)
	{
	   mc.reputation = sti(mc.reputation) -10;
	   if (sti(mc.reputation) < REPUTATION_MIN)
	   {
	       mc.reputation = REPUTATION_MIN;
	   }
	}
	*/
    // <
    totalInfo = totalInfo + LanguageConvertString(idLngFile,"new_string") + LanguageConvertString(idLngFile,"new_string") +
                "Команда отработала успешно!";
    LanguageCloseFile(idLngFile);
    SetFormatedText("INFO_TEXT",totalInfo);
}
////////////////////////////////////////////////////////////////////////
string descF10 = "Бессмертие вкл/выкл";

void CalculateInfoDataF10()
{
	idLngFile = LanguageOpenFile("ItemsDescribe.txt");
	// -->
	totalInfo = descF10;
	ref mc;
	mc = GetMainCharacter();
			if(LAi_IsImmortal(mc))
			{
				LAi_SetImmortal(mc, false);
				Log_SetStringToLog("God mode OFF");
			}else{
				LAi_SetImmortal(mc, true);
				Log_SetStringToLog("God mode ON");
			}
    // <--
    totalInfo = totalInfo + LanguageConvertString(idLngFile,"new_string") + LanguageConvertString(idLngFile,"new_string") +
                "Команда отработала успешно!";
    LanguageCloseFile(idLngFile);
    SetFormatedText("INFO_TEXT",totalInfo);
}
////////////////////////////////////////////////////////////////////////
string descF11 = "Репутация в плюс";

void CalculateInfoDataF11()
{
	idLngFile = LanguageOpenFile("ItemsDescribe.txt");

	totalInfo = descF11;
	// -->

    int i, cn;
    ref mc;
	mc = GetMainCharacter();

    if ( sti(mc.reputation) < REPUTATION_MAX)
	{
	   mc.reputation = sti(mc.reputation) + 10;
	   if (sti(mc.reputation) > REPUTATION_MAX)
	   {
	       mc.reputation = REPUTATION_MAX;
	   }
	}
    // <
    totalInfo = totalInfo + LanguageConvertString(idLngFile,"new_string") + LanguageConvertString(idLngFile,"new_string") +
                "Команда отработала успешно!";
    LanguageCloseFile(idLngFile);
    SetFormatedText("INFO_TEXT",totalInfo);
    //
}
////////////////////////////////////////////////////////////////////////
string descF12 = "НЗГ у всех наций +50";//Заменить текущий корабль на ЧА";  //"LaunchMunityCapture";;

void CalculateInfoDataF12()
{
	idLngFile = LanguageOpenFile("ItemsDescribe.txt");
	// -->
	totalInfo = descF12;
    for (int j=0; j< MAX_NATIONS; j++)
	{
        ChangeCharacterNationReputation(pchar, j, -50);
    }
/*	PChar.Ship.Type = SHIP_CYCLOPUS;
	SetBaseShipData(PChar);
	PChar.Ship.Crew.Quantity = 1400;
	PChar.Ship.Cannons.Type          = CANNON_TYPE_CULVERINE_LBS24;

	SetCharacterGoods(PChar,GOOD_BALLS,2000);
    SetCharacterGoods(PChar,GOOD_GRAPES,700);
    SetCharacterGoods(PChar,GOOD_KNIPPELS,700);
    SetCharacterGoods(PChar,GOOD_BOMBS,1500);
    SetCharacterGoods(PChar,GOOD_POWDER,1500);
    SetCharacterGoods(PChar,GOOD_SAILCLOTH,5);
    SetCharacterGoods(PChar,GOOD_PLANKS,10);
    SetCharacterGoods(PChar,GOOD_FOOD,2000);
    SetCharacterGoods(PChar,GOOD_WEAPON,2000);
	PChar.SystemInfo.CabinType = "My_Cabin";          */

	//ref rNames;
	//rNames = &sFrWomenNames;
	//makeref(rNames, sRndShpEnName);
    //totalInfo = GetArraySize(rNames) + "---- " +rNames[20];

    //LAi_SetActorType(PChar);
    //LAi_ActorAnimation(PChar, "attack_left_1", "", -1);
    //DoQuestCheckDelay("pchar_back_to_player", 15.0);

    // <
    totalInfo = totalInfo + LanguageConvertString(idLngFile,"new_string") + LanguageConvertString(idLngFile,"new_string") +
                "Команда отработала успешно! ";
    LanguageCloseFile(idLngFile);
    SetFormatedText("INFO_TEXT",totalInfo);
}

string descF13 = "Worldmap encounters ON/OFF";

void CalculateInfoDataF13()
{
	idLngFile = LanguageOpenFile("ItemsDescribe.txt");
	// -->
	totalInfo = descF13;

    ref mc;
	mc = GetMainCharacter();
    if(CheckAttribute(mc,"worldmapencountersoff") == 0)
	{
		mc.worldmapencountersoff = "1";
		Log_SetStringToLog("Worldmap encounters OFF");
	}
	else
	{
		if(mc.worldmapencountersoff == "1")
		{
			mc.worldmapencountersoff = "0";
			Log_SetStringToLog("Worldmap encounters ON");
		}
		else
		{
			mc.worldmapencountersoff = "1";
			Log_SetStringToLog("Worldmap encounters OFF");
		}
	}
    // <--
    totalInfo = totalInfo + LanguageConvertString(idLngFile,"new_string") + LanguageConvertString(idLngFile,"new_string") +
                "Команда отработала успешно!";
    LanguageCloseFile(idLngFile);
    SetFormatedText("INFO_TEXT",totalInfo);
}

string descF14 = "Shotgun mode ON\OFF";

void CalculateInfoDataF14()
{
	idLngFile = LanguageOpenFile("ItemsDescribe.txt");
	// -->
	totalInfo = descF14;
    if(globalSGMode != false)
	{
		globalSGMode = false;
		Log_SetStringToLog("Shotgun mode OFF");
	}else{
		globalSGMode = true;
		Log_SetStringToLog("Shotgun mode ON");
	}
    // <
    totalInfo = totalInfo + LanguageConvertString(idLngFile,"new_string") + LanguageConvertString(idLngFile,"new_string") +
                "Команда отработала успешно! Qfreze=";
    LanguageCloseFile(idLngFile);
    SetFormatedText("INFO_TEXT",totalInfo);
}

string descF15 = "Вызвать смену отношений наций";

void CalculateInfoDataF15()
{
	idLngFile = LanguageOpenFile("ItemsDescribe.txt");
	// -->
	totalInfo = descF15;

    DoQuestCheckDelay("Nation_Legend_Map", 2.0);
    // <
    totalInfo = totalInfo + LanguageConvertString(idLngFile,"new_string") + LanguageConvertString(idLngFile,"new_string") +
                "Команда отработала успешно!";
    LanguageCloseFile(idLngFile);
    SetFormatedText("INFO_TEXT",totalInfo);
    ProcessCancelExit();
}

string descF16 = "+5 дней";

void CalculateInfoDataF16()
{
	idLngFile = LanguageOpenFile("ItemsDescribe.txt");
	// -->
	totalInfo = descF16;

    LAi_Fade("", "");
    for (int i = 1; i<=5; i++)
    {
		AddDataToCurrent(0, 0, 1);
	}
    // <
    totalInfo = totalInfo + LanguageConvertString(idLngFile,"new_string") + LanguageConvertString(idLngFile,"new_string") +
                "Команда отработала успешно!";
    LanguageCloseFile(idLngFile);
    SetFormatedText("INFO_TEXT",totalInfo);
}

string descF17 = "Список квестов в pchar";

void CalculateInfoDataF17()
{
	idLngFile = LanguageOpenFile("ItemsDescribe.txt");
	// -->
	totalInfo = descF17;

    aref  curItem;
	aref  arItem;
	aref rootItems;
	int i, j, n;
	string attributeName;
	ref ch;

	//makeref(ch,Characters[GetCharacterIndex("Oxbay Commander")]);
	//makearef(rootItems,ch.Ship);
    //makearef(rootItems,Characters[GetMainCharacterIndex()].Items);
    //makearef(rootItems,ShipsTypes[Characters[GetMainCharacterIndex()].Ship.Type].Cannons.Borts.cannonl);

    makearef(rootItems, pchar.quest);

    //aref qst = makearef(qst, pchar.quest);

    trace("=================================================");
    DumpAttributes(rootItems);//&GameInterface);
    trace("=================================================");
    Log_Info("DumpAttributes");
    /*for(i=0; i<GetAttributesNum(rootItems); i++)
    {
		curItem = GetAttributeN(rootItems, i);
		j = sti(GetAttributeValue(curItem));
		totalInfo = totalInfo+"   "+i+"= "+//LanguageConvertString(idLngFile,"new_string") + LanguageConvertString(idLngFile,"new_string") +
                    GetAttributeName(curItem)+" value="+j;

        for(n=0; n<GetAttributesNum(curItem); n++)
        {
                 arItem = GetAttributeN(curItem,n);
		         j = sti(GetAttributeValue(arItem));
		         totalInfo = totalInfo+//LanguageConvertString(idLngFile,"new_string") + LanguageConvertString(idLngFile,"new_string") +
                    "   --> "+n+"= "+GetAttributeName(arItem)+" value="+j;
        }

    }*/
    remInt++;
    totalInfo = totalInfo + " page " + remInt;
    for(i=0; i<GetAttributesNum(rootItems); i++)
    {
        if (i > remInt*60 || i < (remInt-1)*60) continue;

        curItem = GetAttributeN(rootItems, i);
		j = sti(GetAttributeValue(curItem));
		totalInfo = totalInfo+"   "+i+"= "+//LanguageConvertString(idLngFile,"new_string") + LanguageConvertString(idLngFile,"new_string") +
                    GetAttributeName(curItem);// + " : " + GetAttributeValue(curItem);

        /*for(n=0; n<GetAttributesNum(curItem); n++)
        {
                 arItem = GetAttributeN(curItem,n);
		         j = sti(GetAttributeValue(arItem));
		         totalInfo = totalInfo+//LanguageConvertString(idLngFile,"new_string") + LanguageConvertString(idLngFile,"new_string") +
                    "   --> "+n+"= "+GetAttributeName(arItem)+" value="+j;
        } */

    }
    if (i <= remInt*60) remInt = 0;
    // <
    totalInfo = totalInfo + LanguageConvertString(idLngFile,"new_string") + LanguageConvertString(idLngFile,"new_string") +
                "Команда отработала успешно!";
    LanguageCloseFile(idLngFile);
    SetFormatedText("INFO_TEXT",totalInfo);
}

string descF18 = "ChangeShowIntarface - для скринов без надписей";

void CalculateInfoDataF18()
{
	idLngFile = LanguageOpenFile("ItemsDescribe.txt");
	// -->
	totalInfo = descF18;

    //LandEncTemplate[4].enc1.chance = 1.0;
    //LandEncTemplate[4].enc2.chance = 1.0;
    //
    /*if (bSeaActive)
    {
        Weathers[iCurWeatherNum].Wind.Speed.Min = 9.9;
    	Weathers[iCurWeatherNum].Wind.Speed.Max = 10.1;
	} */
	ChangeShowIntarface();
    // <
    totalInfo = totalInfo + LanguageConvertString(idLngFile,"new_string") + LanguageConvertString(idLngFile,"new_string") +
                "Команда отработала успешно!";
    LanguageCloseFile(idLngFile);
    SetFormatedText("INFO_TEXT",totalInfo);
}

string descF19 = "Офицеру-шкиперу +1 ранг (35 очков скилов)";
void CalculateInfoDataF19()
{
    totalInfo = descF19;
	// -->
    if (sti(pchar.Fellows.Passengers.navigator) != -1)
	{
	    ref chr = GetCharacter(sti(pchar.Fellows.Passengers.navigator));

    	chr.Skill.FreeSkill  = sti(chr.Skill.FreeSkill) + 35;
    }
    else
    {
        totalInfo = "Нет офицера";
    }
    // <--
    totalInfo = totalInfo + NewStr() + NewStr() +
                "Команда отработала успешно!";
    SetFormatedText("INFO_TEXT",totalInfo);
}

string descF20 = "Кач НПС Н307";
void CalculateInfoDataF20()
{
    totalInfo = descF20;
	// -->
    ref chr = GetCharacter(302);

	chr.Skill.FreeSkill  = sti(chr.Skill.FreeSkill) + 35;
    // <--
    totalInfo = totalInfo + NewStr() + NewStr() +
                "Команда отработала успешно!";
    SetFormatedText("INFO_TEXT",totalInfo);
}

string descF21 = "Пустить Золотой флот";
void CalculateInfoDataF21()
{
    totalInfo = descF21;
	// -->
    /*{
      pchar.PatentNation = "eng";
      GiveItem2Character(pchar, "patent_" + pchar.PatentNation);
      EquipCharacterbyItem(pchar, "patent_" + pchar.PatentNation);
    } */
    isGoldFleet = true;
    GoldMonth = XI_ConvertString("target_month_"+GetAddingDataMonth(0, 0, 0));
    AddTemplRumour("Init_GoldFleet", id_counter+1);
    StartGoldFleet("");
    // <--
    totalInfo = totalInfo + NewStr() + NewStr() +
                "Команда отработала успешно!";
    SetFormatedText("INFO_TEXT",totalInfo);
}

string descF22 = "Аскольд инфо";
void CalculateInfoDataF22()
{
    totalInfo = descF22;
	// -->
    {
		totalInfo = "Рендомный хозяин магаза: " + pchar.questTemp.Ascold.TraderId + NewStr() +
					"Рендомный мерчант(колония): " + pchar.questTemp.Ascold.MerchantColony + NewStr() +
					"Рендомный верфист: " + pchar.questTemp.Ascold.ShipyarderId + NewStr()+
					"Скрижали локация: " + Items[GetItemIndex("Rock_letter")].startLocation + NewStr()+
					"Скрижали локатор: " + Items[GetItemIndex("Rock_letter")].startLocator;
					if (pchar.questTemp.BlueBird.City != "")
					{
						totalInfo = totalInfo + NewStr()+ "Где будут слухи по флейту торговца: " + pchar.questTemp.BlueBird.City;
					}
					else
					{
						totalInfo = totalInfo + NewStr()+ "Слухи по флейтам торговцев еще не активированы.";
					}
    }
    // <--
    totalInfo = totalInfo + NewStr() + NewStr() +
                "Команда отработала успешно!";
    SetFormatedText("INFO_TEXT",totalInfo);
}

string descF23 = "Патент";
void CalculateInfoDataF23()
{
    totalInfo = descF23;
	// -->
    {
      if (!CheckAttribute(pchar, "PatentNation")) pchar.PatentNation = "eng";
	  else 
	  {
		  switch (pchar.PatentNation)
		  {
			  case "eng": 
				  TakeItemFromCharacter(pchar, "patent_" + pchar.PatentNation);
				  pchar.PatentNation = "fra"; 				  
				  break;
			  case "fra": 
				  TakeItemFromCharacter(pchar, "patent_" + pchar.PatentNation);
				  pchar.PatentNation = "spa"; 
				  break;
			  case "spa":
				  TakeItemFromCharacter(pchar, "patent_" + pchar.PatentNation);
				  pchar.PatentNation = "hol"; 
				  break;
			  case "hol": 
				  TakeItemFromCharacter(pchar, "patent_" + pchar.PatentNation);
				  pchar.PatentNation = "eng"; 
				  break;
		  }
	  }      
	  GiveItem2Character(pchar, "patent_" + pchar.PatentNation);
      EquipCharacterbyItem(pchar, "patent_" + pchar.PatentNation);
	  totalInfo = "Патент: " + pchar.PatentNation + NewStr();
	}
    // <--
    totalInfo = totalInfo + NewStr() + NewStr() +
                "Команда отработала успешно!";
    SetFormatedText("INFO_TEXT",totalInfo);
}

string descF24 = "Лицензия";
void CalculateInfoDataF24()
{
    totalInfo = descF24;
	// -->
    GiveNationLicence(sti(pchar.nation), 30);
    // <--
    totalInfo = totalInfo + NewStr() + NewStr() +
                "Команда отработала успешно!";
    SetFormatedText("INFO_TEXT",totalInfo);
}

string descF25 = "Перегруз локации - для КОЛОВ";
void CalculateInfoDataF25()
{
    totalInfo = descF25;
	// -->
    // <--
    totalInfo = totalInfo + NewStr() + NewStr() +
                "Команда отработала успешно!";
    SetFormatedText("INFO_TEXT",totalInfo);
    
    ProcessCancelExit();
    if( bSeaActive && !bAbordageStarted )
    {
    	Sea_ReloadStart();
	}
	else
	{
	    DoQuestReloadToLocation(pchar.location, pchar.location.group, pchar.location.locator, "");
	}
}

string descF26 = "пусто";
void CalculateInfoDataF26()
{
    totalInfo = descF26;
	// -->
    // <--
    totalInfo = totalInfo + NewStr() + NewStr() +
                "Команда отработала успешно!";
    SetFormatedText("INFO_TEXT",totalInfo);
}

string descF27 = "пусто";
void CalculateInfoDataF27()
{
    totalInfo = descF27;
	// -->
    // <--
    totalInfo = totalInfo + NewStr() + NewStr() +
                "Команда отработала успешно!";
    SetFormatedText("INFO_TEXT",totalInfo);
}

string descF28 = "Пустить ЛГ на карте";
void CalculateInfoDataF28()
{
    totalInfo = descF28;
	// -->
	GhostShipOnMap();
    // <--
    totalInfo = totalInfo + NewStr() + NewStr() +
                "Команда отработала успешно!";
    SetFormatedText("INFO_TEXT",totalInfo);
}

string descF29 = "НЗГ у всех наций -50";
void CalculateInfoDataF29()
{
    totalInfo = descF29;
	// -->
	for (int j=0; j< MAX_NATIONS; j++)
	{
        ChangeCharacterNationReputation(pchar, j, 50);
    }
    // <--
    totalInfo = totalInfo + NewStr() + NewStr() +
                "Команда отработала успешно!";
    SetFormatedText("INFO_TEXT",totalInfo);
}

string descF30 = "+20 дней";
void CalculateInfoDataF30()
{
    totalInfo = descF30;
	// -->
	LAi_Fade("", "");
	AddDataToCurrent(0, 0, 20);
    // <--
    totalInfo = totalInfo + NewStr() + NewStr() +
                "Команда отработала успешно!";
    SetFormatedText("INFO_TEXT",totalInfo);
}

void ReloadByStr()
{
	// разбор строки
	string  loc = stripblank(GetSubStringByNum(GameInterface.reload_edit.str, 0));
	string  grp = stripblank(GetSubStringByNum(GameInterface.reload_edit.str, 1));
	string  ltr = stripblank(GetSubStringByNum(GameInterface.reload_edit.str, 2));

    int i = FindLocation(loc);
    if (i != -1)
    {
        if(CheckAttribute(&locations[i],"fastreload"))
        {
			if (GetCityFrom_Sea(locations[i].fastreload) != "")
		    {
		        setCharacterShipLocation(pchar, GetCityFrom_Sea(locations[i].fastreload));
		        setWDMPointXZ(GetCityFrom_Sea(locations[i].fastreload));
		    }
	    }
	    else
	    {
	        if (locations[i].type == "seashore" || locations[i].type == "mayak")
	        {
	            setCharacterShipLocation(pchar, loc));
		        setWDMPointXZ(loc);
	        }
	    }
	    DoQuestReloadToLocation(loc, grp, ltr, "");
	    Log_Info(loc);
    }
    else
    {
        Log_Info("Нет локации");
    }
}

void XI_SetScroller(float pos)
{
	SendMessage(&GameInterface,"lsf",MSG_INTERFACE_SET_SCROLLER,"QUESTSCROLL",pos);
}

void SetScrollerPos()
{
	string nodName = GetEventData();
	float pos = GetEventData();
	XI_SetScroller(pos);
}

void ProcScrollPosChange()
{
	float newPos = GetEventData();
    SendMessage(&GameInterface,"lslf",MSG_INTERFACE_MSG_TO_NODE,"INFO_TEXT",2, newPos);
}

void ProcScrollChange()
{
	int changeNum = GetEventData();
	if(changeNum==0) return;
	string controlNode = "";
	if( GetSelectable("INFO_TEXT") ) controlNode = "INFO_TEXT";

	if(controlNode!="")
	{
		if(changeNum>0) {
			SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,controlNode,-1, 0,ACTION_DOWNSTEP);
		} else {
			SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,controlNode,-1, 0,ACTION_UPSTEP);
		}
	}
}