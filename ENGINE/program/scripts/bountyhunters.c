// BOAL 27/06/06 переделка под нужны ВМЛ, все стерли и написали заново.
void SeaHunterCheck()
{
	int    j, i;
	string typeHunter, sGroup, sCapId;
	ref    sld;
	
	SetTimerCondition("SeaHunterCheck", 0, 0, 6+rand(20), true);
    for (j=0; j< MAX_NATIONS; j++)
    {
        if (j == PIRATE) continue;
        // открыть, если не нужны мирные охотники if(NationsRelations2MainCharacter[j] != RELATION_ENEMY) continue;

        typeHunter = NationShortName(j);

        if (ChangeCharacterNationReputation(pchar, j, 0) <= -10)// Минус это НЗГ
        {
            sCapId = typeHunter + "Hunter0";
            sGroup = "Sea_" + sCapId + "1";
            
			Group_DeleteGroup(sGroup);
			Group_FindOrCreateGroup(sGroup);
            for (i = 1; i <= 7; i++)
            {
                sld = GetCharacter(NPC_GenerateCharacter(sCapId + i, "off_hol_2", "man", "man", 5, j, 20, true));
                SetShipHunter(sld);
                sld.WatchFort = true; //видеть форты
                SetFantomParamHunter(sld); //крутые парни
                SetCaptanModelByEncType(sld, "war");
                sld.AlwaysEnemy = true;
                sld.DontRansackCaptain = true;
                sld.mapEnc.type = "war";
                sld.mapEnc.Name = "Охотники за головами";
				//sld.mapEnc.worldMapShip = "Manowar_gold";
                Group_AddCharacter(sGroup, sCapId + i);
                
                if (i == 1 || GetCharacterShipClass(sld) == 1) SetRandGeraldSail(sld, sti(sld.Nation));
                
                if (abs(ChangeCharacterNationReputation(pchar, j, 0)) < (i * 15)) break; // добавим капитана только когда положенно
            }

            Group_SetGroupCommander(sGroup, sCapId+ "1");
            Group_SetTaskAttackInMap(sGroup, PLAYER_GROUP);
            Group_LockTask(sGroup);
            Map_CreateWarrior("", sCapId + "1", 8);
        }
    }
}

void SetShipHunter(ref Hunter)
{
    int ShipsHunter, hcrew;

    if(makeint(pchar.rank) > 10)
    {
        ShipsHunter = SHIP_FLEUT + rand(makeint(SHIP_BATTLESHIP - SHIP_FLEUT));
    }

    if(makeint(pchar.rank) > 6 && makeint(pchar.rank) < 11)
    {
        ShipsHunter = SHIP_FLEUT + rand(makeint(SHIP_PINNACE - SHIP_FLEUT));
    }

    if(makeint(pchar.rank) < 7)
    {
        ShipsHunter = SHIP_LUGGER + rand(makeint(SHIP_SCHOONER - SHIP_LUGGER));
    }
    SetRandomNameToCharacter(Hunter);
    SetRandomNameToShip(Hunter);
    Hunter.Ship.Type = GenerateShip(ShipsHunter, 1);
    SetBaseShipData(Hunter);
    hcrew = GetMaxCrewQuantity(Hunter);
    SetCrewQuantity(Hunter, hcrew);
    SetCrewQuantityFull(Hunter); // to_do
    
    DeleteAttribute(Hunter,"ship.sails");
    DeleteAttribute(Hunter,"ship.masts");
    DeleteAttribute(Hunter,"ship.blots");

    if (Hunter.nation == PIRATE)
    {
        Fantom_SetCannons(Hunter, "pirate");
        Fantom_SetBalls(Hunter, "pirate");
    }
    else
    {
        Fantom_SetCannons(Hunter, "war");
        Fantom_SetBalls(Hunter, "war");
    }
}
// ОЗГИ на суше (порт и бухта)
void LandHunterReactionResult(ref loc)  // отработает после входа в локацию, но после квест_реакшна на вход
{
	int    j, i, k;
	string typeHunter, sTemp, sCapId;
	ref    sld;
	bool   ok;
	
	//if (actLoadFlag) return; // идет лоад
	if (LAi_IsCapturedLocation) return;
	if (chrDisableReloadToLocation) return; // идет некий другой квест с запретом выхода
	if (loc.type != "town" && loc.type != "seashore" && loc.type != "port") return;   
	if (CheckAttribute(Pchar, "GenQuest.Hunter2Pause")) 
	{
		DeleteAttribute(Pchar, "GenQuest.Hunter2Pause");
		return; 
	}
	if (LAi_grp_alarmactive == true)  return;  // уже ГГ кто-то бьет
	
	//if (GetQuestPastDayParam("Land_HunterTimerHasBeen") == 0) return; // сегодня уже были
	for (j=0; j< MAX_NATIONS; j++)
    {
        if (j == PIRATE) continue;
        // открыть, если не нужны мирные охотники if(NationsRelations2MainCharacter[j] != RELATION_ENEMY) continue;

        typeHunter = NationShortName(j);
        if (GetQuestPastDayParam("Land_HunterTimer" + typeHunter) > (7 + rand(7)))
        {
			SaveCurrentQuestDateParam("Land_HunterTimer" + typeHunter); // запомним, даже если нет озгов, выход их ФОР отложит озгов до след раза у др нации
			if (ChangeCharacterNationReputation(pchar, j, 0) <= -10)// Минус это НЗГ
	        {
	            Log_QuestInfo("LandHunterReactionResult " + typeHunter);
				sCapId = typeHunter + "LandHunter0";
	            sTemp = "LAND_HUNTER"; 
				ok = true;
				arrayNPCModelHow = 0;
	            for (i = 1; i <= 8; i++)
	            {
	                sld = GetCharacter(NPC_GenerateCharacter(sCapId + i, "off_hol_2", "man", "man", 5, j, 0, true));
	                SetFantomParamHunter(sld); //крутые парни
	                sld.name 	= "Охотник за головами";
	                sld.lastname 	= "";
	                sld.Dialog.CurrentNode = "First time";
	                sld.dialog.filename = "Hunter_dialog.c";
	                sld.greeting = "Gr_HUNTER";
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
	                if (abs(ChangeCharacterNationReputation(pchar, j, 0)) < (i * 9)) break; // число от НЗГ
	            }
	            
				LAi_group_SetRelation(sTemp, LAI_GROUP_PLAYER, LAI_GROUP_NEITRAL);
				LAi_group_SetRelation(sTemp, LAI_GROUP_PLAYER_OWN, LAI_GROUP_NEITRAL);
				
				LAi_group_ClearAllTargets();
				LAi_SetFightModeForOfficers(false); 
				if (ok)
	            {
	                pchar.HunterCost = abs(ChangeCharacterNationReputation(pchar, j, 0));
	                PChar.HunterCost = makeint(PChar.HunterCost)*2000 + rand(5000); //сразу генерим
	                PChar.HunterCost.TempHunterType = typeHunter;
	                PChar.HunterCost.Qty = i;
	                //LAi_SetActorType(Pchar);
	                sld = characterFromID(sCapId + "1");
	                LAi_type_actor_Reset(sld);
	                LAi_ActorDialog(sld, pchar, "", 4.0, 0);
					//SetActorDialogAny2Pchar(sld.id, "", 0.0, 0.0);
		    		//LAi_ActorFollow(sld, pchar, "ActorDialog_Any2Pchar", 4.1);
		    		chrDisableReloadToLocation = true;
		    		DoQuestCheckDelay("OpenTheDoors", 5.0);
	            }
	            // запомним и сегодня больше не парим ГГ
	            //SaveCurrentQuestDateParam("Land_HunterTimerHasBeen");
	            break;
	        }
        }
    }
}