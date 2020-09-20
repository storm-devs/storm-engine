// клады из ВМЛ

//  Карты сокровищ  ГЕНЕРАТОР -->
string GetIslandForTreasure()
{
	string ret = GetRandomIslandId();

	while (!CheckAttribute(&NullCharacter,"TravelMap.Islands." + ret + ".Treasure"))
	{
		ret = GetRandomIslandId();
	}
	
	int islIdx = GetCharacterCurrentIsland(Pchar);

	if (islIdx != -1)
	{
    	if (Islands[islIdx].id == ret)
    	{
    	    ret = GetIslandForTreasure();
    	}
	}
	return ret;
}

string GetLocationForTreasure(string island)
{
    int iNum;
	aref arDest, arImt;
	string sAttr;

	makearef(arDest, NullCharacter.TravelMap.Islands.(island).Treasure);
	iNum = GetAttributesNum(arDest);
    iNum = rand(iNum-1);
    
    arImt = GetAttributeN(arDest, iNum);
	return GetAttributeName(arImt);
}

string GetBoxForTreasure(string island, string location)
{
    int iNum;
	aref arDest, arImt;
	string sAttr;

	makearef(arDest, NullCharacter.TravelMap.Islands.(island).Treasure.(location));
	iNum = GetAttributesNum(arDest);
    iNum = rand(iNum-1);
    
    arImt = GetAttributeN(arDest, iNum);
	return GetAttributeValue(arImt);  // тут не атрибут, а значеие
}
// не при деле....
string GetFileMapForTreasure(string island)
{
    int n;

    for (n = 0; n < ITEMS_QUANTITY; n++)
	{
		if (CheckAttribute(&Items[n], "MapIsland") )
		{
            if (Items[n].MapIsland == island)
            {
                return Items[n].imageTga;
            }
		}
    }
    return "";
}

void FillMapForTreasure(ref item)
{
    item.MapIslId   = GetIslandForTreasure();
    item.MapLocId   = GetLocationForTreasure(item.MapIslId);
    item.MapBoxId   = GetBoxForTreasure(item.MapIslId, item.MapLocId);
    item.MapTypeIdx = rand(2);

    // генерим клад
    DeleteAttribute(item, "BoxTreasure");
    FillBoxForTreasure(item, rand(3));
    FillBoxForTreasureAddition(item);

    if (!CheckAttribute(Pchar, "GenQuest.TreasureBuild"))
    {
        if (rand(15) == 1) item.MapTypeIdx = -1;
    }
    else
    {// из кусочков даем супер вещи
       /*while (item.MapLocId == "Muelle_town_04" || item.MapLocId == "Havanna_town_04") // не для халявы
       {
            item.MapIslId   = GetIslandForTreasure();
            item.MapLocId   = GetLocationForTreasure(item.MapIslId);
            item.MapBoxId   = GetBoxForTreasure(item.MapIslId, item.MapLocId);
       } */
       FillBoxForTreasureSuper(item);
    }
    DeleteAttribute(Pchar, "GenQuest.TreasureBuild"); //сборный

    if (sti(item.MapTypeIdx) != -1)
    {
        Pchar.quest.SetTreasureFromMap.win_condition.l1          = "location";
        Pchar.quest.SetTreasureFromMap.win_condition.l1.location = item.MapLocId;
        Pchar.quest.SetTreasureFromMap.win_condition             = "SetTreasureFromMap";
    }
}
void FillBoxForTreasure(ref item, int i)
{
	// определяем тип
	switch (i)
	{
		// good
		case 0:
		    if (rand(1) == 1)
	        {
            	item.BoxTreasure.jewelry2 = 2 + rand(30);
	        }
	        else
	        {
	            item.BoxTreasure.jewelry3 = 10 + rand(3);
	        }
	        if (rand(1) == 1)
	        {
            	item.BoxTreasure.jewelry3 = 1 + rand(30);
	        }
	        if (rand(1) == 1)
	        {
            	item.BoxTreasure.jewelry5 = 5 + rand(60);
	        }
	        if (rand(1) == 1)
	        {
            	item.BoxTreasure.jewelry7 = 15 + rand(100);
	        }
	        if (rand(2) == 1)
	        {
            	item.BoxTreasure.jewelry10 = 15 + rand(100);
	        }
	        if (rand(2) == 1)
	        {
            	item.BoxTreasure.jewelry13 = 5 + rand(100);
	        }
	        if (rand(2) == 1)
	        {
            	item.BoxTreasure.jewelry14 = 5 + rand(100);
	        }
	        if (rand(2) == 1)
	        {
            	item.BoxTreasure.jewelry17 = 5 + rand(100);
	        }
	        if (rand(2) == 1)
	        {
            	item.BoxTreasure.jewelry18 = 5 + rand(100);
	        }
	        if (rand(2) == 1)
	        {
            	item.BoxTreasure.mineral2 = 5 + rand(100);
	        }
	        if (rand(2) == 1)
	        {
            	item.BoxTreasure.mineral5 = 5 + rand(100);
	        }
	        if (rand(4) == 1)
	        {
            	item.BoxTreasure.suit_2 = 1;
	        }
	    break;
	    // best
	    case 1:
            if (rand(2) == 1)
	        {
            	item.BoxTreasure.chest = 1 + rand(14);
	        }
	        else
	        {
	            item.BoxTreasure.jewelry12 = 10 + rand(3);
	        }
	        if (rand(2) == 1)
	        {
            	item.BoxTreasure.icollection = 1 + rand(4);
	        }
	        if (rand(1) == 1)
	        {
            	item.BoxTreasure.jewelry1 = 15 + rand(100);
	        }
	        if (rand(1) == 1)
	        {
            	item.BoxTreasure.jewelry12 = 15 + rand(100);
	        }
	        if (rand(1) == 1)
	        {
            	item.BoxTreasure.jewelry11 = 15 + rand(100);
	        }
	        if (rand(1) == 1)
	        {
            	item.BoxTreasure.jewelry6 = 5 + rand(100);
	        }
	        if (rand(4) == 1)
	        {
            	item.BoxTreasure.suit_3 = 1;
	        }
	    break;
	    // bad
	    case 2:
	        if (rand(2) == 1)
	        {
            	item.BoxTreasure.rabble = 5 + rand(100);
	        }
	        else
	        {
                item.BoxTreasure.blade1 = 5 + rand(100);//fix
	        }
	        if (rand(2) == 1)
	        {
            	item.BoxTreasure.blade1 = 5 + rand(100);
	        }
	        if (rand(2) == 1)
	        {
            	item.BoxTreasure.blade5 = 5 + rand(100);
	        }
	        if (rand(2) == 1)
	        {
            	item.BoxTreasure.jewelry16 = 5 + rand(100);
	        }
	        if (rand(2) == 1)
	        {
            	item.BoxTreasure.mineral4 = 5 + rand(100);
	        }
	        if (rand(2) == 1)
	        {
            	item.BoxTreasure.mineral6 = 5 + rand(200);
	        }
	        if (rand(2) == 1)
	        {
            	item.BoxTreasure.mineral7 = 5 + rand(100);
	        }
	        if (rand(2) == 1)
	        {
            	item.BoxTreasure.mineral9 = 5 + rand(100);
	        }
	        if (rand(2) == 1)
	        {
            	item.BoxTreasure.mineral10 = 25 + rand(300);
	        }
	        if (rand(2) == 1)
	        {
            	item.BoxTreasure.indian4 = 15 + rand(300);
	        }
	        if (rand(2) == 1)
	        {
            	item.BoxTreasure.indian8 = 5 + rand(100);
	        }
	        if (rand(2) == 1)
	        {
            	item.BoxTreasure.indian9 = 5 + rand(100);
	        }
	        if (rand(2) == 1)
	        {
            	item.BoxTreasure.indian13 = 5 + rand(100);
	        }
	        if (rand(2) == 1)
	        {
            	item.BoxTreasure.indian16 = 5 + rand(200);
	        }
	        if (rand(2) == 1)
	        {
            	item.BoxTreasure.Lead1 = 1;
	        }
	        if (rand(2) == 1)
	        {
            	item.BoxTreasure.suit_1 = 1;
	        }
	    break;

	    case 3:
	        FillBoxForTreasure(item, 0);
	    break;
	}
}

void FillBoxForTreasureAddition(ref item)
{
    float   nLuck   = GetCharacterSkillToOld(Pchar, SKILL_FORTUNE);

    if (5*nLuck > rand(55))
    {
	    if (GetCharacterItem(Pchar, "map_part1") == 0)
	    {
	        item.BoxTreasure.map_part1 = 1;
	    }
	    else
	    {
	        if (GetCharacterItem(Pchar, "map_part2") == 0)
		    {
		        item.BoxTreasure.map_part2 = 1;
		    }
	    }
	}
	// +1 вещи
	switch (rand(40))
	{
		case 1:
		    item.BoxTreasure.STATUE1 = 1;
		break;
		case 2:
		    item.BoxTreasure.jewelry9 = 1;
		break;
		case 3:
		    item.BoxTreasure.jewelry15 = 1;
		break;
		case 4:
		    item.BoxTreasure.jewelry8 = 1;
		break;
		case 5:
		    item.BoxTreasure.indian7 = 1;
		break;
		case 6:
		    item.BoxTreasure.indian2 = 1;
		break;
		case 7:
		    item.BoxTreasure.jewelry4 = 1;
		break;
		case 8:
		    item.BoxTreasure.indian3 = 1;
		break;
		case 9:
		    item.BoxTreasure.indian1 = 1;
		break;
		case 10:
		    item.BoxTreasure.indian5 = 1;
		break;
		case 11:
		    item.BoxTreasure.indian6 = 1;
		break;
		case 12:
		    item.BoxTreasure.indian10 = 1;
		break;
		case 13:
		    item.BoxTreasure.indian12 = 1;
		break;
		case 14:
		    item.BoxTreasure.indian14 = 1;
		break;
		case 15:
		    item.BoxTreasure.indian15 = 1;
		break;
		case 16:
		    item.BoxTreasure.indian17 = 1;
		break;
		case 17:
		    item.BoxTreasure.indian18 = 1;
		break;
		case 18:
		    item.BoxTreasure.indian19 = 1;
		break;
		case 19:
		    item.BoxTreasure.indian20 = 1;
		break;
		case 20:
		    item.BoxTreasure.indian21 = 1;
		break;
		case 21:
		    item.BoxTreasure.indian22 = 1;
		break;
		case 22:
		    item.BoxTreasure.Mineral3 = 1;
		break;
		case 23:
		    item.BoxTreasure.Mineral8 = 1;
		break;
		case 24:
		    item.BoxTreasure.Coins = 1;
		break;
		case 25:
		    item.BoxTreasure.sculMa1 = 1;
		break;
		case 26:
		    item.BoxTreasure.sculMa2 = 1;
		break;
		case 27:
		    item.BoxTreasure.sculMa3 = 1;
		break;
	}
}

void FillBoxForTreasureSuper(ref item)
{
    float     nLuck   = GetCharacterSkillToOld(Pchar, SKILL_FORTUNE);
	int     i;
	string  itmName;

    if (3*nLuck > rand(21))// еще поди найди 2 куска
    {
		i = 0;
		itmName = "";
		while (itmName == "" && i < 15)
		{
            switch (rand(11))
			{
                case 0:
                	itmName = "blade26";
				break;
				case 1:
        			itmName = "blade28";
				break;
				case 2:
        			itmName = "blade32";
				break;
				case 3:
        			itmName = "arm1";
				break;
				case 4:
        			itmName = "pistol4";
				break;
				case 5:
        			itmName = "indian11";//"idol";
				break;
				case 6:
                    itmName = "topor2"; // to_do
					//itmName = "map_good";
				break;
				case 7:
        			itmName = "spyglass4";
				break;
				case 8:
        			//itmName = "cirass5";
        			itmName = "cirass4";
				break;
				case 9:
        			itmName = "indian11";
				break;
				case 10:
        			itmName = "cirass3";
        			/*if (GetCharacterItem(Pchar, "cirass5") > 0)
        			{
                        itmName = "";
        			} */
				break;

				case 11:
        			itmName = "spyglass5";
				break;
			}
			/*if (itmName == "idol" && CheckAttribute(pchar, "QuestTemp.IdolUseYet"))
			{
			    itmName = "";
			} */
			if (GetCharacterItem(Pchar, itmName) > 0)
		    {
          		itmName = "";
		    }
		    i++;
	    }
	    if (itmName != "")
	    {
	        item.BoxTreasure.(itmName) = 1;
	    }
	}
}

void SetTreasureBoxFromMap()
{
    aref   item;
    ref    loc;
    string box;
    aref   arToBox;
    aref   arFromBox;

    if (GetCharacterItem(Pchar, "map_full")>0 )
    {
        Log_Info("Сокровища где-то рядом!");
        PlaySound("interface\notebook.wav");
        // немного веселой жизни
        TraderHunterOnMap();
        if( CheckAttribute(Pchar,"location.from_sea") )
        {
            if (rand(2) == 1) //33%
            {
                Pchar.quest.SetTreasureHunter.win_condition.l1          = "location";
                Pchar.quest.SetTreasureHunter.win_condition.l1.location = Pchar.location.from_sea;
                Pchar.quest.SetTreasureHunter.win_condition             = "";
                Pchar.quest.SetTreasureHunter.function    = "SetTreasureHunter";
            }
        }

        Items_FindItem("map_full", &item);

        box = item.MapBoxId;

        loc = &locations[FindLocation(item.MapLocId)];
        loc.(box).items = "";

        makearef(arToBox, loc.(box).items);
        makearef(arFromBox, item.BoxTreasure);
        CopyAttributes(arToBox, arFromBox);

        loc.(box) = Items_MakeTime(GetTime(), GetDataDay(), GetDataMonth(), GetDataYear());
        loc.(box).Treasure =  true; // признак сокровища в сундуке

        DeleteAttribute(item, "MapIslId");
        TakeNItems(Pchar, "map_full", -1);
    }
}
//  Карты сокровищ  ГЕНЕРАТОР <--

// погодня за ГГ на карте
void  TraderHunterOnMap()
{
    // немного веселой жизни
    ref  sld;
    int  i;

    string sCapId = "Follower0";
    string sGroup = "Sea_" + sCapId + "1";

	Group_DeleteGroup(sGroup);
	Group_FindOrCreateGroup(sGroup);
    for (i = 1; i <= GetCompanionQuantity(pchar); i++)
    {
        sld = GetCharacter(NPC_GenerateCharacter(sCapId + i, "off_hol_2", "man", "man", 5, PIRATE, 15, true));
        SetShipHunter(sld);
        SetFantomParamHunter(sld); //крутые парни
        SetCaptanModelByEncType(sld, "war");
        sld.AlwaysEnemy = true;
        sld.DontRansackCaptain = true;
        sld.mapEnc.type = "war";
        sld.mapEnc.Name = "Джентльмены удачи";
        Group_AddCharacter(sGroup, sCapId + i);
    }

    Group_SetGroupCommander(sGroup, sCapId+ "1");
    Group_SetTaskAttackInMap(sGroup, PLAYER_GROUP);
    Group_LockTask(sGroup);
    Map_CreateWarrior("", sCapId + "1", 8);
}

void SetTreasureHunter(string temp)
{
    int    j, i, k;
	string sTemp, sCapId;
	ref    sld;
	bool   ok;

	if (chrDisableReloadToLocation) return; // идет некий другой квест с запретом выхода
	
    Pchar.GenQuest.Hunter2Pause            = true;
    
    j = GetOfficersQuantity(Pchar) + 2;
    
	sCapId = "LandHunter0";
    sTemp = "LAND_HUNTER";
	ok = true;
	arrayNPCModelHow = 0;
    for (i = 1; i <= j; i++)
    {
        sld = GetCharacter(NPC_GenerateCharacter(sCapId + i, "off_hol_2", "man", "man", 5, PIRATE, 0, true));
        SetFantomParamHunter(sld); //крутые парни
        sld.Dialog.CurrentNode = "TreasureHunter";
        sld.dialog.filename = "Hunter_dialog.c";
        sld.greeting = "Enc_Raiders";
        sld.location = "none"; // вот где порылась собака!!!!!!!!!!!

        SetModelPirate(sld);
        k = 0;
		while (!CheckNPCModelUniq(sld) && k < 10)
		{
		    k++;
			SetModelPirate(sld);
		}
		arrayNPCModel[arrayNPCModelHow] = sld.model;
		arrayNPCModelHow++;
		
        LAi_SetActorTypeNoGroup(sld);
        LAi_SetCheckMinHP(sld, (LAi_GetCharacterHP(sld) - 1), false, "Battle_Hunters_Land");
        if (PlaceCharacter(sld, "goto", "random_must_be_near") == "" && i == 1) // fix если вдруг нет в локации
        {
            ok = false;
            break;
        }
        LAi_ActorFollow(sld, pchar, "", 8.0);
        //LAi_Actor2WaitDialog(sld, pchar); // ждать диалог, но бежать
        LAi_group_MoveCharacter(sld, sTemp);
    }

	LAi_group_SetRelation(sTemp, LAI_GROUP_PLAYER, LAI_GROUP_NEITRAL);
	LAi_group_SetRelation(sTemp, LAI_GROUP_PLAYER_OWN, LAI_GROUP_NEITRAL);

	LAi_group_ClearAllTargets();
	LAi_SetFightModeForOfficers(false);
	if (ok)
    {
        PChar.HunterCost = makeint(sti(Pchar.money) / 5) + rand(20)*1000; //сразу генерим
        PChar.HunterCost.Qty = i;
        PChar.HunterCost.TempHunterType = "";
        sld = characterFromID(sCapId + "1");
        LAi_type_actor_Reset(sld);
        LAi_ActorDialog(sld, pchar, "", 4.0, 0);
		chrDisableReloadToLocation = true;
		DoQuestCheckDelay("OpenTheDoors", 5.0);
    }
}

ref SetFantomSkeletForts(string group, string locator, string enemygroup, string _type)
// _type = "GhostShipCrew"   _type = "ParamHunter"  _type = "none"
{
    string emodel;
    ref    Cr;

    emodel = GetRandSkelModel();

    Cr = LAi_CreateFantomCharacterEx(emodel, "man", group, locator);

    if (Cr.location.locator != "")
    {
		if (_type == "GhostShipCrew" || _type == "ParamHunter")
		{
			SetFantomParamHunter(Cr);
		}
		else
		{
		    SetFantomParam(Cr);
		}
		LAi_SetWarriorType(Cr);
	    LAi_group_MoveCharacter(Cr, enemygroup);
	    LAi_NoRebirthEnable(Cr); //не показывать убитых при входе в локацию
	    LAi_LoginInCaptureTown(Cr, true); // для записи игры
	    if (_type == "GhostShipCrew")
	    {
		    // возможно дает вылеты от многих трупов Cr.DontClearDead = true;
		    Cr.GhostShipCrew = true;
	    }
    }
    return Cr;
}

///////////////////////////////////////////   Квест летучего голландца /////////////////////////////////
// летучий голландец
void  GhostShipOnMap()
{
    ref  sld;
    int  i;
    ref  rRealShip;
    
    sld = characterFromID("GhostCapt");
    // пушки чинятся в SetBaseShipData DeleteAttribute(sld, "ship.cannons");
    
    SetBaseShipData(sld);
    sld.ship.sailscolor = argb(255, 60, 60, 60);
    
    DeleteAttribute(sld,"ship.sails");
    DeleteAttribute(sld,"ship.masts");
    DeleteAttribute(sld,"ship.blots");

    Ship_SetTaskNone(SECONDARY_TASK, sti(sld.index));

    if (CheckAttribute(pchar , "GenQuest.GhostShip.LastBattle"))
    {
		rRealShip = GetRealShip(sti(sld.Ship.Type));
		rRealShip.EmblemedSails.normalTex =  "ships\parus_common_torn.tga";

		rRealShip.MaxCaliber = 32;
	    rRealShip.Cannon     = CANNON_TYPE_CANNON_LBS32;
	    rRealShip.HP         = 7000;
	    rRealShip.WindAgainstSpeed = 3.0;
		rRealShip.TurnRate   = 45.0;
	
        SetBaseShipData(sld);
        DeleteAttribute(sld, "ship.sailscolor");  // белый парус

        sld.ship.Cannons.Type = CANNON_TYPE_CULVERINE_LBS32;

        Character_SetAbordageEnable(sld, true);
    }
    else
    {
        sld.ship.hp = 52000; // 13%
    }

    SetCrewQuantityOverMax(sld, 666);
    sld.mapEnc.type = "war";
    sld.mapEnc.Name = "Летучий голландец";
	sld.mapEnc.worldMapShip = "pirates_manowar";
				
    sld.ship.Crew.Morale = 90;
    ChangeCrewExp(sld, "Sailors", 100);
	ChangeCrewExp(sld, "Cannoners", 100);
	ChangeCrewExp(sld, "Soldiers", 100);
				
    Fantom_SetBalls(sld, "pirate");
    Fantom_SetCharacterGoods(sld, GOOD_BALLS,  3900 + rand(300));
	Fantom_SetCharacterGoods(sld, GOOD_BOMBS,  2900 + rand(300));
	Fantom_SetCharacterGoods(sld, GOOD_POWDER, 6900 + rand(300));

    LAi_SetCurHPMax(sld); // если он умер

    string sGroup = "Sea_GhostCapt";  // приставка "Sea_" + ИД важна
    Group_FindOrCreateGroup(sGroup);
	Group_AddCharacter(sGroup, "GhostCapt");
    Group_SetGroupCommander(sGroup, "GhostCapt");

    SetCharacterRelationBoth(sti(sld.index), GetMainCharacterIndex(), RELATION_ENEMY);
    Group_SetTaskAttackInMap(sGroup, PLAYER_GROUP);
    Group_LockTask(sGroup);

    Map_CreateWarrior("", "GhostCapt", 3);
    
    Pchar.quest.GhostShip_Dead.win_condition.l1 = "NPC_Death";
	Pchar.quest.GhostShip_Dead.win_condition.l1.character = "GhostCapt";
	Pchar.quest.GhostShip_Dead.win_condition = "GhostShip_Dead";
}

void Survive_In_Sea_Go2Land()
{
    string  sTemp;
    int     iDay;
    ref 	ch;
	int 	n, i, idx;
    int 	storeArray[30];
    int 	howStore = 0;
    
    // трем всех офов и компаньонов, не квестовых
    iDay = GetPassengersQuantity(pchar);
    i = 0;
    n = 0;
 	while (i < iDay)
	{
		i++;
		idx = GetPassenger(pchar, n); // всегда первый в массиве со смещением, если не трем
		if (idx==-1) break;
		ch = GetCharacter(idx);
		if (GetRemovable(ch)) // квестовых не трогаем
		{
		    ch.LifeDay = 1; // чтоб не сразу потерся
		    RemovePassenger(pchar, ch);
			ChangeCharacterAddress(ch,"none","");
		}
		else
		{
		    n++;
		}

	}
    for (i=0; i<COMPANION_MAX; i++)
	{
		idx = GetCompanionIndex(pchar, i);
		if (idx != -1)
		{
            ch = GetCharacter(idx);
			if (GetShipRemovable(ch) && GetRemovable(ch))  // не трогаем квестовых и ПГГ
			{
			    ch.LifeDay = 1; // чтоб не сразу потерся
			    RemoveCharacterCompanion(pchar, ch);
			}
		}
	}
	
	DeleteAttribute(pchar, "ship");
	pchar.ship.name = "";
	pchar.ship.type = SHIP_NOTUSED;
	pchar.money = 0;

	SetCharacterShipLocation(pchar, "");

	// выбор города близ бухты
    for (n = 0; n < MAX_COLONIES; n++)
	{
  		if (colonies[n].nation == "none") continue; // необитайки
  		if (sti(colonies[n].nation) == PIRATE) continue;
  		if (colonies[n].from_sea == "") continue; // необитайки
  		if (!CheckAttribute(&Colonies[n], "islandLable") || Colonies[n].islandLable == "Mein") continue; // только острова
  		
        storeArray[howStore] = n;
   		howStore++;
    }

    makeref(ch,colonies[storeArray[rand(howStore-1)]]);
    setWDMPointXZ(ch.from_sea);
    pchar.location = ch.from_sea; // это порт, ниже ищем бухту

    // нулим предметы в каюте
    ref loc;
    if (Pchar.SystemInfo.CabinType != "")
    {
	    loc = &locations[FindLocation(Pchar.SystemInfo.CabinType)]; //"My_Cabin"

	    for (n = 1; n <= 4; n++)
	    {
	        sTemp = "box" + n;
			DeleteAttribute(loc, sTemp + ".items");
		    loc.(sTemp).items = "";
		    loc.(sTemp) = Items_MakeTime(0, 0, 1, 2003);
		    loc.(sTemp).money = 0;
	    }
    }
    loc = &locations[FindLocation("My_Deck")];
    for (n = 1; n <= 4; n++)
    {
        sTemp = "box" + n;
		DeleteAttribute(loc, sTemp + ".items");
	    loc.(sTemp).items = "";
	    loc.(sTemp) = Items_MakeTime(0, 0, 1, 2003);
	    loc.(sTemp).money = 0;
    }
	Pchar.GenQuest.Hunter2Pause = true;
    iDay = 5+rand(17);
    bQuestCheckProcessFreeze = true;
    WaitDate("",0,0,iDay, rand(10), 1); // прошло время
    bQuestCheckProcessFreeze = false;
    
	// бухта контры
    pchar.location = SelectSmugglingLocation();
    
    pchar.Health.Damg = stf(pchar.chr_ai.hp_max)*40;
	// дает лог в + и - AddCharacterHealth(pchar, -30);
	Log_Info("О чудо! Я жив!");
	if (sti(PChar.GenQuest.GhostShip.KillMe) <= 1)
	{
		AddQuestRecord("GhostShipQuest", "Survive_1");
	}
	else
	{
	    AddQuestRecord("GhostShipQuest", "Survive_2");
	    // добавить предмет Т102
	    if (sti(PChar.GenQuest.GhostShip.KillMe) == 2)
		{
			Log_Info("Вы нашли очень странный предмет.");
			TakeNItems(Pchar, "term_arm", 1);
	    	pchar.QuestTemp.TakeShotgun = true;
	    }
	}
	bDisableMapEnter           = false;   // мир, был бой с ЛГ
	
	SetLaunchFrameFormParam("Прошло " + iDay + " дней." + NewStr() +
	                        "Остров " + GetConvertStr(ch.islandLable, "LocLables.txt")+","+ NewStr() +
							"" + GetConvertStr(pchar.location, "LocLables.txt") + ".",
						                        "Reload_To_Location", 0.1, 7.0);
    SetLaunchFrameReloadLocationParam(pchar.location, "Smugglers", "Smuggler01", "");
	LaunchFrameForm();
}

void GhostShipInit()
{
	ref sld;
	//============> Капитан призрака
	sld = GetCharacter(NPC_GenerateCharacter("GhostCapt", "officer_7", "man", "man", 55, PIRATE, -1, true));
	sld.dialog.filename      = "Quest\GhostShip_dialog.c";
	sld.dialog.currentnode   = "GhostCapt";
	sld.name 	= "Деви";
	sld.lastname = "Джонс";
	sld.reputation = 15;
	sld.RebirthPhantom     = true;
	sld.AlwaysEnemy        = true;
	sld.DontHitInStorm     = true; // не ломается в шторм
	sld.DontRansackCaptain = true; //квестовые не сдаются
	sld.SinkTenPercent     = false;  // не тонуть после 10% корпуса
	sld.Abordage.Enable    = false; // запрет абордажа
	sld.SaveItemsForDead   = true; // сохранять на трупе вещи
	sld.AlwaysSandbankManeuver = true;  // тупым запрет тонуть об берег
	sld.GenQuest.CrewSkelMode  = true; // на абордаже скелеты
	LAi_SetHP(sld, 666, 666);
	SetCharacterPerk(sld, "Energaiser"); // скрытый перк дает 1.5 к приросту энергии, дается ГГ и боссам уровней
    sld.SuperShooter  = true;
    
	DeleteAttribute(sld, "items");
	GiveItem2Character(sld, "topor2");
	EquipCharacterByItem(sld, "topor2");
	GiveItem2Character(sld, "pistol6");
	EquipCharacterByItem(sld, "pistol6");
	TakeNItems(sld, "bullet", 10);
	TakeNItems(sld,"potion2", 10);
	sld.money = 70000;
	
	LAi_SetLoginTime(sld, 6.0, 21.99);
	LAi_SetCitizenType(sld);
	LAi_group_MoveCharacter(sld, "TmpEnemy");
	sld.Ship.Type = CreateBaseShip(SHIP_LINESHIP);
	sld.Ship.Name = "Летучий голландец";
	
	ref rRealShip = GetRealShip(sti(sld.Ship.Type));
	rRealShip.ship.upgrades.hull  = 4;
	rRealShip.ship.upgrades.sails = 3;
	rRealShip.EmblemedSails.normalTex =  "ships\parus_common_torn.tga";
	sld.ship.sailscolor = argb(255,60,60,60);
	rRealShip.MaxCaliber = 48;
    rRealShip.Cannon     = CANNON_TYPE_CANNON_LBS48;
    rRealShip.MaxCrew    = 525;
    rRealShip.OptCrew    = 400;
    rRealShip.Capacity   = 5000;
    rRealShip.HP         = 400000;
    rRealShip.WindAgainstSpeed = 10.0;
    rRealShip.SpeedRate	 = 15.0;
	rRealShip.TurnRate   = 60.0;
	rRealShip.Price      = 500000;
	
	pchar.GenQuest.GhostShip.ShipType   = sti(sld.Ship.Type); // запомним, потом пригодится, когда парус нужно сменить на целый
}

// квест шотгана - терминатора Т-102 появление в комнате таверны
void QuestShotgunT102()
{
	ref sld;
	sld = GetCharacter(NPC_GenerateCharacter("T102", "skelT", "man", "man2", 55, PIRATE, 0, false));
	sld.dialog.filename      = "Quest\T102_dialog.c";
    
    sld.name  = "Terminator";
    sld.lastname = "";
	TakeNItems(sld,"potion2", 30);
	LAi_SetHP(sld, 1600.0, 1600.0);
	
	ChangeCharacterAddressGroup(sld, PChar.location, "reload", "reload1");
	LAi_group_MoveCharacter(sld, "TmpEnemy");
	LAi_SetActorType(sld);
    LAi_SetActorType(pchar);

	SetActorDialogAny2Pchar(sld.id, "", 2.0, 0.0);
	LAi_ActorFollow(sld, pchar, "ActorDialog_Any2Pchar", 4.0);
    LAi_group_SetRelation("TmpEnemy", LAI_GROUP_PLAYER, LAI_GROUP_FRIEND);
}