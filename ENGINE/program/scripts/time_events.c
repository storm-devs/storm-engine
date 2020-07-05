
void ActivateTimeEvents()
{

}

//////////////////////// boal SLiB ////////////////////////////////
void SalaryNextDayUpdate()
{
	if (sti(NullCharacter.SalayPayMonth) != GetDataMonth() && !bDisableMapEnter) // boal
	{
        // проверка на наличие кому платить -->
        int nPaymentQ = 0;
        int i, cn;
        ref chref;
        
		for (i=0; i<COMPANION_MAX; i++)
		{
			cn = GetCompanionIndex(pchar, i);
			if (cn >= 0)
			{
				chref = GetCharacter(cn);
				if (GetRemovable(chref)) // считаем только своих, а то вских сопровождаемых кормить!!!
				{
				    nPaymentQ += GetSalaryForShip(chref);
				}
			}
		}
		// проверка на наличие кому платить <--
		NullCharacter.SalayPayMonth = GetDataMonth(); // boal
		if (nPaymentQ > 0)
        {
			LaunchMoneyGraphCollect();
	
			if (!dialogRun && !bQuestCheckProcessFreeze && !bAbordageStarted) // можно показать
			{
	      		LaunchSalaryScreen("");
			}
			else
			{
		        Pchar.quest.LaunchSalaryScreen.win_condition.l1 = "MapEnter";
		        PChar.quest.LaunchSalaryScreen.win_condition    = "LaunchSalaryScreen";
		        PChar.quest.LaunchSalaryScreen.function         = "LaunchSalaryScreen";
	        }
        }
	}
}

#event_handler("EvSituationsUpdate","WorldSituationsUpdate");
void WorldSituationsUpdate()
{
	int iStep = GetEventData();

	// boal -->
	/*if (bQuestCheckProcessFreeze)   // если в квесте, то откладываем
	{
	    if(iStep < 10)
		{
			PostEvent("EvSituationsUpdate", 1000, "l", iStep);
		}
	}*/
	// boal <--

	switch(iStep)
	{
		case 0:
            DeleteAttribute(pchar, "SkipEshipIndex");// boal
			DailyEatCrewUpdate(); // boal
			Log_QuestInfo("WorldSituationsUpdate DailyEatCrewUpdate");
		break;
		case 1:
			UpdateFame();   // это теперь известность репутации
			GenerateRumour() //homo 05/07/06
			
		break;
		case 2:
			// ушло в переходы локаций PGG_DailyUpdate(); // navy 
			ProcessDayRepair();
		break;
		case 3:
//			UpdateDisease();
			Group_FreeAllDead();
		break;
		case 4:
			QuestActions(); //eddy
		break;
		case 5:
		//	UpdateColonyProfit();  
			wdmEmptyAllOldEncounter();// homo чистка энкоутеров
		break;
		case 6:
			UpdateCrewExp();  // изменение опыта команды
		break;
		case 7:
			UpdateCrewInColonies(); // пересчет наемников в городах
		break;
		case 8:
		    if (IsEntity(worldMap))
		    {
			    EmptyAllFantomCharacter(); // трем НПС
				wdmEmptyAllDeadQuestEncounter();
			}
		break;
		case 9:
			SalaryNextDayUpdate();  // запрлата
			Log_QuestInfo("WorldSituationsUpdate SalaryNextDayUpdate");
		break;
		case 10:
			//
		break;
	}

	iStep++;
	InterfaceStates.WorldSituationUpdateStep = iStep;

	if(iStep <= 10)
	{
		PostEvent("EvSituationsUpdate", 1000, "l", iStep);
	}
}


//////////////////////////////// начало игры - туториал ////////////////////////////////
void Tut_StartGame(string sQuest)
{
    InterfaceStates.Buttons.Save.enable = false;
	StartQuestMovie(true, true, true);
    SetCharacterTask_None(GetMainCharacter());
    LAi_LockFightMode(Pchar, true);
	PChar.GenQuest.CallFunctionParam = "Tut_Continue";
	DoQuestCheckDelay("CallFunctionParam", 5.0);
	LAi_LocationFightDisable(loadedLocation, true);
	InterfaceStates.Buttons.Save.enable = false;
	DoQuestFunctionDelay("Tut_locCamera_1", 0.1);
}

void Tut_locCamera_1(string _tmp)
{
    locCameraToPos(-5, 2.5, 5.6, false);
    DoQuestFunctionDelay("Tut_locCamera_2", 10.0);
}

void Tut_locCamera_2(string _tmp)
{
    locCameraFollow();
}
void Tut_Continue()
{
    ref sld;
    LAi_LocationFightDisable(loadedLocation, false);
    LAi_SetFightMode(Pchar, false);
    LAi_LockFightMode(pchar, true);
    
	sld = GetCharacter(NPC_GenerateCharacter("Sailor_1", "Pirate_9", "man", "man", 1, PIRATE, 0, false));
    sld.name 	= "Сандро";
    sld.lastname 	= "Торн";
    sld.Dialog.CurrentNode = "First time";
    sld.dialog.filename = "Quest\StartGame_dialog.c";
    sld.greeting = "Teacher_pirat";

    SetSPECIAL(sld, 9,8,10,3,6,10,4);
    InitStartParam(sld);
    SetEnergyToCharacter(sld);

    LAi_SetCheckMinHP(sld, 1, true, "Tut_StartGame_CheckMinHP_1");
    GiveItem2Character(sld, "blade1");
	EquipCharacterByItem(sld, "blade1");
	// пустоля нет, лечилок нет
    ChangeCharacterAddressGroup(sld, "Ship_deck_Low", "reload", "reload1");
    LAi_SetActorType(sld);
	LAi_ActorDialog(sld, pchar, "", 5.0, 0);

	// генерим второго матроса, но пока не ставим
	sld = GetCharacter(NPC_GenerateCharacter("Sailor_2", "shipowner_2", "man", "man", 1, PIRATE, 0, false));
    sld.name 	= "Джим";
    sld.lastname 	= "Хопкинс";

    SetSPECIAL(sld, 8,10,9,3,6,10,4);
    InitStartParam(sld);
    SetEnergyToCharacter(sld);

    LAi_SetCheckMinHP(sld, 1, true, "Tut_StartGame_CheckMinHP_2");
    GiveItem2Character(sld, "blade4");
	EquipCharacterByItem(sld, "blade4");
	GiveItem2Character(sld, "pistol2");
	EquipCharacterByItem(sld, "pistol2");
	TakeNItems(sld, "bullet", 30);
	// лечилок нет
    sld.location = "Ship_deck_Low";  // чтоб не терся, пока его нет
	//раскидаем квестовых нпс по локациям
	SetQuestsCharacters();	
}

void Tut_RestoreState()
{
	ref sld;
	
	pchar.Health.Damg = 0.0; // здоровье бережем
	pchar.Health.weekDamg = 0.0;

	LAi_SetCurHPMax(pchar);
	LAi_SetCheckMinHP(pchar, 1, true, "Tut_StartGame_CheckMinHP_Hero");
	LAi_SetImmortal(pchar, false);
	
	sld = characterFromID("Sailor_1");
	LAi_SetCurHPMax(sld);
	LAi_SetCheckMinHP(sld, 1, true, "Tut_StartGame_CheckMinHP_1");
	LAi_SetImmortal(sld, false);
	
	sld = characterFromID("Sailor_2");
	LAi_SetCurHPMax(sld);
	LAi_SetCheckMinHP(sld, 1, true, "Tut_StartGame_CheckMinHP_2");
	LAi_SetImmortal(sld, false);
	
	DeleteAttribute(pchar, "HeroParam.Teach_beat");
}

void Tut_TeachBattle()
{
	ref sld;
	
	LAi_SetPlayerType(pchar);
	LAi_SetFightMode(Pchar, true);
	sld = characterFromID("Sailor_1");
	LAi_SetWarriorType(sld);
    LAi_group_MoveCharacter(sld, "TmpEnemy");
	
	if (sti(pchar.HeroParam.Teach_battle) == 2)
	{
        sld = characterFromID("Sailor_2");
		if (!IsEntity(sld))
        {
        	ChangeCharacterAddressGroup(sld, "Ship_deck_Low", "reload", "reload1");
        }
		LAi_SetWarriorType(sld);
	    LAi_group_MoveCharacter(sld, "TmpEnemy");
	}
	LAi_group_SetHearRadius("TmpEnemy", 100.0);
    LAi_group_FightGroupsEx("TmpEnemy", LAI_GROUP_PLAYER, true, Pchar, -1, false, false);
    LAi_group_SetRelation("TmpEnemy", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
}

void Tut_StartDialog()
{
	ref sld = characterFromID("Sailor_1");
	
	if (CheckAttribute(pchar, "HeroParam.Teach_beat"))
	{ // признак, что выиграл
		if (sti(pchar.HeroParam.Teach_beat) == 3)
		{
		    sld.Dialog.CurrentNode = "Teach_battle_win_2";
		}
		else
		{
			if (sti(pchar.HeroParam.Teach_battle) == 1)
			{
				sld.Dialog.CurrentNode = "Teach_battle_win";
			}
			else
			{
			    sld.Dialog.CurrentNode = "Teach_battle_win_3";
			}
		}
	}
	else
	{
		sld.Dialog.CurrentNode = "Teach_battle_lose";
	}
	LAi_ActorWaitDialog(pchar, sld);
	LAi_ActorDialog(sld, pchar, "", 4.0, 0);
}
//поставить НПС-раздатчиков квеста в нужные города по нации ГГ
void SetQuestsCharacters()
{
	ref sld;
	string sTown;
	int iNation = sti(pchar.nation);
	switch (iNation)
	{
		case 0: sTown = "PortRoyal"; break;
		case 1: sTown = "Tortuga";	break;
		case 2:	sTown = "Havana";	break;
		case 3:	sTown = "Villemstad"; break;
	}
	//заказчик убийства нищих
	sld = characterFromId("PoorKillSponsor");
	sld.nation = iNation;
	sld.city = sTown;
	sld.location = sTown + "_town";
	sld.location.group = "goto";
	sld.location.locator = "goto2";
	LAi_group_MoveCharacter(sld, GetNationNameByType(iNation) + "_citizens");
}