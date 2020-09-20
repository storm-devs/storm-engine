bool IsQuestUsedItem(string itmID)
{
	aref arItm;
	if( Items_FindItem(itmID,&arItm)<0 ) return false;
	if( CheckAttribute(arItm,"price") && sti(arItm.price)>0 ) return false;
	return true;
}

void DoCharacterUsedItem(ref chref, string itmID)
{
	aref arItm;
	if( Items_FindItem(itmID,&arItm)<0 ) return;
	TakeItemFromCharacter(chref,itmID);

	if( CheckAttribute(arItm,"potion.health") )
	{
		LAi_UseHealthBottle(chref,stf(arItm.potion.health));
		if(sti(chref.index)==GetMainCharacterIndex()) {
			Log_SetStringToLog( XI_ConvertString("Health Up"));
		}
		// boal
		if( CheckAttribute(arItm,"potion.health.speed") )
		{
			LAi_UseHealthBottleSpeed(chref, stf(arItm.potion.health.speed));
		}
	}
	if( CheckAttribute(arItm,"potion.antidote") )
	{
		LAi_UseAtidoteBottle(chref);
		if(sti(chref.index)==GetMainCharacterIndex()) {
			Log_SetStringToLog( XI_ConvertString("You are cured from poison") );
		}
	}
	if( CheckAttribute(arItm,"potion.sound") )
	{
		PlaySound(arItm.potion.sound);
	}
}

float MinHealthPotionForCharacter(ref chref)
{
	float ftmp;
	bool isFinded = false;

	for(int n=0; n<ITEMS_QUANTITY; n++)
	{
		if( CheckAttribute(&Items[n],"potion") )
		{
			if(CheckAttribute(&Items[n],"potion.health"))
			{
				if( GetCharacterItem(chref,Items[n].id)>0 )
				{
					if(isFinded)
					{
						if( stf(Items[n].potion.health)<ftmp )
						{
							ftmp = stf(Items[n].potion.health);
						}
					}else{
						ftmp = stf(Items[n].potion.health);
						isFinded = true;
					}
				}
			}
		}
	}
	if(!isFinded) return 0.0;
	return ftmp;
}

string FindHealthForCharacter(ref chref,float fHealth)
{
	string sret = "";
	float fdelta = fHealth + 100.0;
	float ftmp;

	for(int n=0; n<ITEMS_QUANTITY; n++)
	{
		if( CheckAttribute(&Items[n],"potion") )
		{
			if( CheckAttribute(&Items[n],"potion.health") )
			{
				if( GetCharacterItem(chref,Items[n].id)>0 )
				{
					ftmp = stf(Items[n].potion.health);
					if( ftmp<fHealth )	{ftmp = fHealth - ftmp;}
					else	{ftmp = ftmp - fHealth;}
					if(ftmp<fdelta)
					{
						fdelta = ftmp;
						sret = Items[n].id;
					}
				}
			}
		}
	}

	return sret;
}

int FindPotionFromChr(ref chref, ref arFind, int startIdx)
{
	int i;
	aref arItm;
	for(i=startIdx; i<ITEMS_QUANTITY; i++)
	{
		makearef(arItm,Items[i]);
		if( CheckAttribute(arItm,"potion") && GetCharacterItem(chref,Items[i].id)>0 )
		{
			arFind = arItm;
			return i;
		}
	}
	return -1;
}

int FindQuestUsableItem(ref arFind, int startIdx)
{
	int i;
	aref arItm;
	bool bSeaInterface = bSeaActive && !bAbordageStarted;

	if(startIdx<0) startIdx=0;
	for(i=startIdx; i<ITEMS_QUANTITY; i++)
	{
		makearef(arItm,Items[i]);
		if( CheckAttribute(arItm,"quest") && CheckAttribute(arItm,"quest.tex"))// boal 16.03.2004
		{
			if( bSeaInterface && arItm.quest.tex=="QuestCommands" )
			{
				arFind = arItm;
				return i;
			}
		}
	}

	return -1;
}

bool EnablePotionUsing(ref mc, aref arItm)
{
	bool bEnableUse = false;
	if( CheckAttribute(arItm,"potion.health") ) {
		if( LAi_GetCharacterHP(mc)<LAi_GetCharacterMaxHP(mc) ) {
			return true;
		}
	}
	if( CheckAttribute(arItm,"potion.antidote") ) {
		if( LAi_IsPoison(mc) ) {
			return true;
		}
	}
	return false;
}

int FindItem(string sItemID)
{
	for(int i = 0; i < ITEMS_QUANTITY; i++)
	{
		if(Items[i].id == sItemID)
		{
			return i;
		}
	}
	return -1;
}
// eddy -->
/////////////////////// ==> Items-методы
int GetItemIndex(string _ItemID)
{
    aref curItem;
	for(int i=0; i<ITEMS_QUANTITY; i++)
	{
		makearef(curItem,Items[i]);
		if( curItem.id == _ItemID )
		{
			return i;
		}
	}
	return -1;
}

ref ItemsFromID(string _Items)
{
	return &items[GetItemIndex(_Items)];
}

void ChangeItemDescribe(string _Items, string _Describe)
{
    Items[GetItemIndex(_Items)].describe = _Describe;
}

void BackItemDescribe(string _Items)
{
    ref ItemAR = ItemsFromID(_Items);
    ItemAR.describe = "itmdescr_" + ItemAR.id;
}

void ChangeItemName(string _Items, string _Name)
{
    Items[GetItemIndex(_Items)].name = _Name;
}

void BackItemName(string _Items)
{
    ref ItemAR = ItemsFromID(_Items);
    ItemAR.name = "itmname_" + ItemAR.id;
}
///////////////////////  Items-методы <--


void QuestCheckEnterLocItem(aref _location, string _locator) /// <<<проверка вхождения ГГ в локаторы группы Item.<<<
{
	ref sld;
	int i;
	//======> Генератор инквизиции.
	if (_location.id == "Santiago_Incquisitio" && CheckNPCQuestDate(_location, "AttackGuard_date") && sti(Colonies[FindColony(_location.fastreload)].nation) == SPAIN) 
	{
		SetNPCQuestDate(_location, "AttackGuard_date"); //одна засада в день.
		LAi_group_AttackGroup("SPAIN_CITIZENS", LAI_GROUP_PLAYER);
		StartIncquisitioAttack();
		//==>фр.линейка, квест №7. Рок Бразилец, даем диалог.
		if (pchar.questTemp.State == "Fr7RockBras_toSeekPlace") LAi_ActorWaitDialog(characterFromId("RockBrasilian"), pchar);
	}
	//=======> Квест Изабеллы, детектор на скрипт базара Сальватора с братом
	if (_location.id == "SanJuan_town" && pchar.RomanticQuest == "SeeTalkNearHouse") 
	{
        pchar.quest.Romantic_DeadBrother_Cancel.over = "yes"; //убираем таймер на вовремя не явлился
		StartQuestMovie(true, true, true);
		pchar.RomanticQuest = "BrotherIsDead";
		LAi_SetActorType(pchar);
		SetMainCharacterIndex(GetCharacterIndex("Husband"));
        locCameraToPos(40.585, 6.0, -47.549, false);
		LAi_SetActorType(PChar);
		PChar.BaseNation = SPAIN; //скрипт в городе, иначе глючит опрос патрулями
		PChar.RomanticQuest = "";
		LAi_SetActorType(CharacterFromID("MigelDeValdes"));
		SetActorDialogAny2Pchar("MigelDeValdes", "", 1.0, 0.0);
		LAi_ActorFollow(pchar, CharacterFromID("MigelDeValdes"), "ActorDialog_Any2Pchar", 0.0);
	}
	//=======> Квест Изабеллы, закрываем дверь дома, если прошлялся не заходя домой более 3 месяцев
	if (_location.id == "SanJuan_town" && pchar.RomanticQuest == "NewLifeForHero") 
	{
		if (GetQuestPastMonthParam("RomanticQuest") > 3) 
		{
			LocatorReloadEnterDisable("SanJuan_town", "houseSp6", true);
			pchar.RomanticQuest = "TheDoosIsClosed";
			AddQuestRecord("Romantic_Line", "29");
		}
	}
	//=======> Квест Изабеллы, закрываем дверь дома, если был у шлюх
	if (_location.id == "SanJuan_town" && CheckAttribute(pchar, "RomanticQuest.HorseCheck") && pchar.RomanticQuest.HorseCheck != "-1" && sti(pchar.questTemp.HorseQty) > sti(pchar.RomanticQuest.HorseCheck))
	{
		LocatorReloadEnterDisable("SanJuan_town", "houseSp6", true);
		pchar.RomanticQuest = "over";
		pchar.RomanticQuest.HorseCheck = -1;
		AddQuestRecord("Romantic_Line", "29");
	}
	//======> Генератор маяка Порт Рояля.
	if (_location.id == "Mayak3") 	
	{
		int iNation = GetCityNation(GetCityNameByIsland(GiveArealByLocation(_location)));
		if(iNation == -1) return;
		string sGroup = GetNationNameByType(iNation)  + "_mayak";
		LAi_group_AttackGroup(sGroup, LAI_GROUP_PLAYER);
	}
	//======> Генератор монстров при входе в локатор духов
	if (_locator == "duhi1" && CheckAttribute(_location, "locators.monsters") && !bMonstersGen)
	{
		//проверяем флаг запрещения генерации
		if(LAi_LocationIsMonstersGen(_location) && LAi_grp_playeralarm == 0) 
		{
			SetSkeletonsToLocation(_location);
		}
	}
	//======> детектор в тюрьме, вторжение без разрешения
	if (_location.type == "jail" && !sti(pchar.questTemp.jailCanMove) && _locator == "detector1")
	{	
		pchar.questTemp.jailCanMove = true; //чтобы не срабатывало 2 раза
		if (!LAi_grp_alarmactive && !IsLocationCaptured(_location.id))
		{
			string slai_group = GetNationNameByType(GetCityNation(_location.parent_colony)) + "_citizens";
			LAi_group_AttackGroup(slai_group, LAI_GROUP_PLAYER);
			LAi_SetFightMode(pchar, true);
		}
	}
}

void QuestCheckExitLocItem(aref _location, string _locator) /// <<<проверка выхода ГГ из локаторов группы Item.<<<
{
	//=======> Испанская линейка, квест №4. В спальне нашел возле комода нашел недописанное письмо
	if (_location.id == "Havana_houseS1Bedroom" && pchar.questTemp.State == "Sp4Detection_toMirderPlace" && CheckCharacterItem(pchar, "letter_notes"))
    {
        DoQuestCheckDelay("TalkSelf_Quest", 0.1); //диалог сам-на-сам
    }
	//=======> Квест Аскольда, нашли гробницу.
    if (_location.id == "Guadeloupe_Cave" && _locator == "detector1")
    {
        if (pchar.questTemp.Ascold == "Ascold_SeekGrave" || pchar.questTemp.Ascold == "Ascold_CantSeekGrave")
        {
            Log_QuestInfo("Прерывание на нахождение входа в пещеру сработало.");
            pchar.questTemp.Ascold = "Ascold_NotEnterFoundGrave";
    		DoQuestCheckDelay("TalkSelf_Quest", 0.1); //диалог сам-на-сам
        }
    }
	//=======> Квест Аскольда, ГГ добрался до мумии, но бутылки еще не имеет. Вместо прерывания на локатор.
    if (_location.id == "Guadeloupe_Cave" && _locator == "button02")
    {
		if (pchar.questTemp.Ascold == "Ascold_SeekRockLetter" || pchar.questTemp.Ascold == "Ascold_EnterGrave")
		{
			pchar.questTemp.Ascold = "Ascold_FoundMummy";
			characters[GetCharacterIndex("Ascold")].dialog.currentnode = "Found_Grave";
		}
	}
	//=======> Энкаунтеры заманухи в пещеру, открываем закрытый релоад на колодце.
    if (_locator == "CheckReload1" && CheckAttribute(pchar, "GenQuest.OpenTheRopeExit") && pchar.GenQuest.OpenTheRopeExit == pchar.location)
    {
		DeleteAttribute(pchar, "GenQuest.OpenTheRopeExit");
		LocatorReloadEnterDisable(pchar.location, "reload2", false);
	}
	//=======> Изабелла, в доме начало скрипта с братом
    if (_location.id == "SanJuan_houseSp6" && pchar.RomanticQuest == "exitFromDetector")
    {
		pchar.RomanticQuest = "executeFromDetector";
		StartQuestMovie(true, true, true);
		// ГГ теперь Сальватор
		//sGlobalTemp = GetMainCharacterIndex();
		SetMainCharacterIndex(GetCharacterIndex("MigelDeValdes"));
        PChar   = GetMainCharacter();			
		locCameraToPos(-2.41, 2.35, -2.41, false);
		SetActorDialogAny2Pchar("Isabella", "", 3.0, 0.0);
		LAi_ActorFollow(PChar, CharacterFromID("Isabella"), "ActorDialog_Any2Pchar", 4.0);
	}
	//=======> Изабелла, в доме Роситы после смерти мужа
    if (_location.id == "Beliz_houseS4" && pchar.RomanticQuest == "Beliz_exitFromDetector")
    {
		pchar.RomanticQuest = "Beliz_executeFromDetector";
		StartQuestMovie(true, true, true);
        // ГГ теперь Изабелла
		ChangeCharacterAddressGroup(CharacterFromID("Isabella"), pchar.location, "goto",  "goto3");
		ChangeCharacterAddressGroup(CharacterFromID("Rosita"), pchar.location, "goto",  "goto1");
        SetMainCharacterIndex(GetCharacterIndex("Isabella"));
        PChar   = GetMainCharacter();			
        locCameraToPos(-3.84, 2.35, 0.85, false);
		LAi_SetActorType(CharacterFromID("Rosita"));
		LAi_SetActorType(pchar);
		PChar.BaseNation = SPAIN; //скрипт в городе, иначе глючит опрос патрулями
		SetActorDialogAny2Pchar("Rosita", "", 1.0, 0.0);
		LAi_ActorFollow(PChar, CharacterFromID("Rosita"), "ActorDialog_Any2Pchar", 0.0);
	}
	//=======> Пиратка, квест №5
    if (_location.id == "Pirates_houseS1" && pchar.questTemp.piratesLine == "PL5Hunter_exitReload")
    {
		pchar.questTemp.piratesLine = "PL5Hunter_executeReload";
		StartQuestMovie(true, true, true);
		SetMainCharacterIndex(GetCharacterIndex("Orry"));
        PChar   = GetMainCharacter();			
		locCameraToPos(1.34, 2.1, 1.68, false);
		LAi_SetActorType(CharacterFromID("Aivory"));
		LAi_SetActorType(pchar);
		SetActorDialogAny2Pchar("Aivory", "", 3.0, 0.0);
		LAi_ActorFollow(PChar, CharacterFromID("Aivory"), "ActorDialog_Any2Pchar", 4.0);
	}
}

void QuestCheckUseButton(aref _location, string _locator) /// <<< квестовые действия при установке предметов в button <<<
{
    //==> Квест Аскольда, установка скрижалей для открытия входа в пещеру к гробнице.
    if (_location.id == "Guadeloupe_Cave" && _locator == "button01")
    {
		//==> перебиваем иниты пещеры Гваделупы
		_location.reload.l1.name = "reload3_back";
		locations[FindLocation("Guadeloupe_CaveEntrance")].reload.l1.emerge = "reload3"; 
		//==> перемещаемся на reload5
		DoQuestCheckDelay("Ascold_OpenTheGrave", 1.5);
    }
    //==> Квест Аскольда, вскрытие гробницы кочергой.
    if (_location.id == "Guadeloupe_Cave" && _locator == "button02" && pchar.questTemp.Ascold == "Ascold_WateringMummy")
    {
        ref sld;
		for (int i=1; i<=3; i++)
        {
			sld = GetCharacter(NPC_GenerateCharacter("Enemy_"+i, "Skel"+i, "man", "man", 30, PIRATE, 0, true));	
			FantomMakeCoolFighter(sld, 30, 85, 85, "topor2", "pistol3", 100);  
            LAi_SetWarriorType(sld);				
			LAi_group_MoveCharacter(sld, "EnemyFight");				
            ChangeCharacterAddressGroup(sld, pchar.location, "goto",  "goto"+i);	
        }
        LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
        LAi_group_FightGroups("EnemyFight", LAI_GROUP_PLAYER, true);
        LAi_group_SetCheck("EnemyFight", "Ascold_InGraveAfterFight");
        chrDisableReloadToLocation = true;
    }
    //==> Квест Аскольда, вскрытие лампы и появление Аззи на верфи 
    if (_location.id == (pchar.questTemp.Ascold.ShipyarderColony + "_shipyard") && _locator == "button01")
    {
		LAi_SetActorType(PChar);
		DoQuestCheckDelay("TalkSelf_Quest", 0.2);
    }
	//==> Пиратка, квест №7
    if (_location.id == "PuertoPrincipe_town" && _locator == "button01")
    {
		ref loc = &locations[FindLocation("PuertoPrincipe_townhall")];
		loc.reload.l2.name = "reload2";
		loc.reload.l2.go = "PuertoPrincipe_town";
		loc.reload.l2.emerge = "reload9";
		loc.reload.l2.autoreload = "0";
		loc.reload.l2.label = "Street";
		DoQuestFunctionDelay("PQ7_loadToRecidence", 2.0);
	}
}
//проверка взятия предметов из локатора item
void QuestCheckTakeItem(aref _location, string _itemId)
{
	//квест промышленного шпионажа не верфях
	if (_itemId == "ShipyardsMap")
	{		
		AddQuestRecord("ShipyardsMap", "2");
		if (IsLoginTime())
		{
			ref sld = characterFromId(_location.fastreload + "_shipyarder");
			sld.dialog.currentnode = "Allarm";	
			LAi_SetActorType(pchar);
			LAi_ActorDialog(pchar, sld, "", 0, 0);
		}
	}
	//квест поиски кольца мэра в борделях
	if (_itemId == "MayorsRing")
	{
		AddQuestRecord("SeekMayorsRing", "6");
		AddQuestUserData("SeekMayorsRing", "sCity", XI_ConvertString("Colony" + pchar.questTemp.different.TakeMayorsRing.city + "Gen"));
	}
	//квест поиска драгоценного камня ростовщика
	if (_itemId == "UsurersJew")
	{
		string sTitle = _location.townsack + "UrurersJewel";
		AddQuestRecordEx(sTitle, "SeekUrurersJewel", "2");
	}
	//пиратка, квест №7
	if (_itemId == "OpenBook")
	{
		AddQuestRecord("Pir_Line_7_Soukins", "9");
		SaveCurrentQuestDateParam("questTemp.piratesLine");
		pchar.questTemp.piratesLine = "Soukins_catchBattleship";
		Pchar.quest.PQ7_setBattleShip.win_condition.l1 = "MapEnter";
	    Pchar.quest.PQ7_setBattleShip.function = "PQ7_setBattleShip";
	}
	//линейка ГПК, разборки с представителем в Мариго
	if (_itemId == "letter_LSC")
	{
		AddQuestRecord("ISS_PoorsMurder", "11");
		pchar.questTemp.LSC = "readyGoLSC";
		int n = FindIsland("LostShipsCity");
		Islands[n].visible = true;
		Islands[n].reload_enable = true;
		Islands[n].alwaysStorm = true; //живем в штормах
		Islands[n].MaxSeaHeight = 2.0;
		Islands[n].storm = true;
		Islands[n].tornado = true;
		n = GetCharacterIndex("PoorKillSponsor");
		if (n != -1 && !LAi_IsDead(&characters[n])) 
		{
			chrDisableReloadToLocation = true; 
			LAi_LocationFightDisable(loadedLocation, false);
			LAi_group_Attack(&characters[n], pchar);
			pchar.quest.PQ8_afterFight.win_condition.l1 = "NPC_Death";
			pchar.quest.PQ8_afterFight.win_condition.l1.character = "PoorKillSponsor";
			pchar.quest.PQ8_afterFight.win_condition = "OpenTheDoors";
		}
	}
}

void StartIncquisitioAttack()
{
	ref sld;
    for (int i=1; i<=5; i++)
    {
		sld = GetCharacter(NPC_GenerateCharacter("IncqAddGuard_"+i, "sold_spa_"+(rand(7)+1), "man", "man", 35, SPAIN, 1, true));
		FantomMakeCoolFighter(sld, sti(pchar.rank)+MOD_SKILL_ENEMY_RATE+8, 80, 70, BLADE_LONG, "pistol4", 50);
        LAi_SetWarriorType(sld);
        LAi_group_MoveCharacter(sld, "SPAIN_CITIZENS");            
        ChangeCharacterAddressGroup(sld, "Santiago_Incquisitio", "goto", LAi_FindRandomLocator("goto"));
    }
}
