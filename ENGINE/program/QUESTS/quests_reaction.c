
void QuestComplete(string sQuestName, string qname)
{
	// boal -->
	ref sld, npchar;
	aref arOldMapPos, arAll, arPass;
	int     iTemp, i, ShipType, Rank; // нужно для вычислений любых целых (нации)
    float locx, locy, locz, fTemp;
    string  attrName, Model, Blade, Gun, sTemp; // любые строки для вычислений
	bool   bOk;
	if (bQuestLogShow)
    {
	    Log_Info("Quest completed : " + sQuestName + "  param = " + qname);
		trace("Quest completed : " + sQuestName + "  param = " + qname + " " + GetQuestBookDataDigit());
	}
	// boal <--
	if (CheckAttribute(pchar, "quest." + qname + ".function"))
	{
		string sFunction = pchar.quest.(qname).function;
		call sFunction(qname);
		return;
	}

	switch(sQuestName)
	{	
		// boal -->
		// смена отношений от времени
        // boal все теперь проверяется 15-25 день при расчете состояния мира = изменение в 5% - ранд(100) > 95
        case "Nation_Legend":
            SetTimerCondition("Nation_Legend", 0, 0, 15+rand(10), true);
			// тут чиним стророжевики -->
			if (!bSeaActive && !bAbordageStarted)
            {
              //  RepairAllDefenderFortShips();
            }
            // тут чиним стророжевики <--
            if (rand(100) > 93 && !bWorldAlivePause)
            {
                if(IsEntity(worldMap))
                {
                    LaunchNationLegend();
                }else{
                    if (!bQuestCheckProcessFreeze) // можно показать
					{
					    DoQuestCheckDelay("Nation_Legend_Map", 1.5);
					}
					else
					{
	      				Pchar.quest.Nation_Legend_Map.win_condition.l1 = "MapEnter";
	                    Pchar.quest.Nation_Legend_Map.win_condition = "Nation_Legend_Map";
			        }
                }
        	}
        break;

        case "Nation_Legend_Map":
            if (!bQuestCheckProcessFreeze) // можно показать
			{
                LaunchNationLegend();
            }else{
                Pchar.quest.Nation_Legend_Map_TEST.win_condition.l1 = "MapEnter";
                Pchar.quest.Nation_Legend_Map_TEST.win_condition = "Nation_Legend_Map";
            }
        break;
		// начало игры, туториал
		case "Tut_StartGame_CheckMinHP_1":
			if (!CheckAttribute(pchar, "HeroParam.Teach_beat"))
			{
				pchar.HeroParam.Teach_beat	= 0;
			}
			pchar.HeroParam.Teach_beat = sti(pchar.HeroParam.Teach_beat) + 1;
			sld = characterFromID("Sailor_1");
			LAi_SetActorTypeNoGroup(sld);
			LAi_ActorAfraid(sld, pchar, true);
			bOk = false;
			if (sti(pchar.HeroParam.Teach_beat) == 1 && sti(pchar.HeroParam.Teach_battle) == 1)
			{
				bOk = true;
			}
			else
			{
				if (sti(pchar.HeroParam.Teach_beat) == 2 && sti(pchar.HeroParam.Teach_battle) == 2)
				{
					bOk = true;
					pchar.HeroParam.Teach_beat = 3; // признак, что выиграл
				}
			}
			if (bOk)
			{
				DoQuestCheckDelay("Tut_StartGame_CheckMinHP_Hero", 0.3);
			}
        break;
        
        case "Tut_StartGame_CheckMinHP_2":
        	if (!CheckAttribute(pchar, "HeroParam.Teach_beat"))
			{
				pchar.HeroParam.Teach_beat	= 0;
			}
			pchar.HeroParam.Teach_beat = sti(pchar.HeroParam.Teach_beat) + 1;
			sld = characterFromID("Sailor_2");
   			LAi_SetActorTypeNoGroup(sld);
			LAi_ActorAfraid(sld, pchar, true);
			if (sti(pchar.HeroParam.Teach_beat) == 2 && sti(pchar.HeroParam.Teach_battle) == 2)
			{
				sld = characterFromID("Sailor_1");
				LAi_SetActorTypeNoGroup(sld);
				pchar.HeroParam.Teach_beat = 3; // признак, что выиграл
				DoQuestCheckDelay("Tut_StartGame_CheckMinHP_Hero", 0.3);
			}
        break;
        
        case "Tut_StartGame_CheckMinHP_Hero":
        	// запуск диалога
        	LAi_SetActorTypeNoGroup(pchar);
        	sld = characterFromID("Sailor_2");
			LAi_SetActorTypeNoGroup(sld);
			sld = characterFromID("Sailor_1");
			LAi_SetActorTypeNoGroup(sld);
            PChar.GenQuest.CallFunctionParam = "Tut_StartDialog";
			DoQuestCheckDelay("CallFunctionParam", 2.0);
        break;
		//  Карты сокровищ  ГЕНЕРАТОР -->
        case "SetTreasureFromMap":
            SetTreasureBoxFromMap();
        break;
        //  Карты сокровищ  ГЕНЕРАТОР <--
        
		case "ArestInResidenceEnd":
            //LAi_LocationFightDisable(&Locations[FindLocation(Pchar.quest.ArestInResidenceEnd.ResidenceLocation)], true);
		    LAi_LockFightMode(Pchar, false); // ножками убежал
        break;
        // ОЗГи
        case "Battle_Hunters_Land":
            Lai_SetPlayerType(pchar);
            LAi_SetFightMode(Pchar, true);
			for (i=1; i<= sti(PChar.HunterCost.Qty); i++)
			{
	            sld = characterFromID(PChar.HunterCost.TempHunterType + "LandHunter0" + i);
				LAi_RemoveCheckMinHP(sld);
	            LAi_SetWarriorType(sld);
	            //LAi_group_MoveCharacter(sld, LAI_GROUP_TmpEnemy);
	            LAi_group_MoveCharacter(sld, "LAND_HUNTER");
			}
            //LAi_group_SetRelation(LAI_GROUP_TmpEnemy, LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
            //LAi_group_FightGroups(LAI_GROUP_TmpEnemy, LAI_GROUP_PLAYER, true);
            LAi_group_SetRelation("LAND_HUNTER", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
            LAi_group_FightGroups("LAND_HUNTER", LAI_GROUP_PLAYER, true);
        break;
        
        case "GoAway_Hunters_Land":
            //LAi_SetActorType(Pchar);
            //DoQuestCheckDelay("pchar_back_to_player", 4.0);
            DoQuestCheckDelay("OpenTheDoors", 4.0);
            sTemp = LAi_FindNearestFreeLocator2Pchar("reload");
            for (i=1; i<= sti(PChar.HunterCost.Qty); i++)
			{
	            sld = characterFromID(PChar.HunterCost.TempHunterType + "LandHunter0" + i);
				//LAi_RemoveCheckMinHP(sld);  можно пальнуть в спину, тогда по идее будет бой
				LAi_type_actor_Reset(sld);
				LAi_ActorGoToLocation(sld, "reload", sTemp, "none", "", "", "", 4.0);
			}
        break;
        
          // офицеры выживают!  homo
        case "Survive_In_SeaOfficer":
			sld = GetCharacter(sti(Pchar.GenQuest.Survive_In_SeaOfficerIdx));
			ref rOff;
            iTemp = NPC_GeneratePhantomCharacter("citizen", sti(sld.Nation), MAN, -1);  // создать клон
			if (iTemp != -1)
			{
                rOff = &Characters[iTemp];
                ChangeAttributesFromCharacter(rOff, sld, true);
                if (findsubstr(sld.id, "PsHero_" , 0) != -1) rOff.id = sld.id; //homo for navy (ПГГ fix)
                PlaySound("interface\_EvShip1.wav");
                LAi_SetCurHPMax(rOff);
    			AddPassenger(Pchar, rOff, false);
    			sld.location = "none";
    			sld.location.from_sea = "";
    			Log_Info("Офицер " + GetFullName(rOff) + " поднялся на борт.");
            }


        break;
        // враги спасшиеся на шлюпках homo
        case "Survive_In_SeaPrisoner":  //homo 22/06/07
            sld = GetCharacter(sti(Pchar.GenQuest.Survive_In_SeaPrisonerIdx));
			ref rPris;
            iTemp = SetCharToPrisoner(sld);
			if (iTemp != -1)
			{
                rPris = &Characters[iTemp];
                PlaySound("interface\_EvShip1.wav");
                if (findsubstr(sld.id, "PsHero_" , 0) != -1) rPris = sld.id; //homo for navy (ПГГ fix)
                LAi_SetCurHPMax(rPris);
    			sld.location = "none";
    			sld.location.from_sea = "";
                Log_Info(GetFullName(rPris) + " сдался в плен.");
            }

        break;
        //  ЛГ
        case "Survive_In_Sea":
            DeleteSeaEnvironment();
            PChar.GenQuest.GhostShip.KillMe      = sti(PChar.GenQuest.GhostShip.KillMe) + 1;
            PChar.GenQuest.GhostShip.AskAbout    = 2; // опять будет
            Survive_In_Sea_Go2Land();
        break;
        
        case "GhostShip_Dead":
            if (CheckAttribute(pchar , "GenQuest.GhostShip.LastBattle"))
            {
                Log_Info("Призрак уничтожен, теперь он не вернется.");
                AddQuestRecord("GhostShipQuest", "Dead_2");
                CloseQuestHeader("GhostShipQuest");

                AddCharacterExpToSkill(PChar, "Leadership", 300);
    		    AddCharacterExpToSkill(PChar, "Sailing", 500);
    		    AddCharacterExpToSkill(PChar, "Accuracy", 500);
    		    AddCharacterExpToSkill(PChar, "Cannons", 500);
    		    AddCharacterExpToSkill(PChar, "Repair", 500);
    		    AddCharacterExpToSkill(PChar, "Defence", 500);
    		    AddCharacterExpToSkill(PChar, "Sneak", 300);
    		    PChar.GenQuest.GhostShip.LastBattleEnd = true;
    		    // меняем парус у корабля, если корабль взял ГГ
				sld = GetRealShip(sti(pchar.GenQuest.GhostShip.ShipType));
				DeleteAttribute(sld, "EmblemedSails.normalTex");
            }
            else
            {
                Log_Info("Призрак уничтожен, но может ли он вернуться?");
                AddQuestRecord("GhostShipQuest", "Dead_1");
                AddCharacterExpToSkill(PChar, "Leadership", 600);
    		    AddCharacterExpToSkill(PChar, "Sailing", 1000);
    		    AddCharacterExpToSkill(PChar, "Accuracy", 1000);
    		    AddCharacterExpToSkill(PChar, "Cannons", 1000);
    		    AddCharacterExpToSkill(PChar, "Repair", 1000);
    		    AddCharacterExpToSkill(PChar, "Defence", 1000);
    		    AddCharacterExpToSkill(PChar, "Sneak", 500);
    		    PChar.GenQuest.GhostShip.DeadByMe = sti(PChar.GenQuest.GhostShip.DeadByMe) + 1;
    		    PChar.GenQuest.GhostShip.AskAbout    = 2; // опять будет

    		    /*PChar.GenQuest.VideoAVI        = "Ending";
                PChar.GenQuest.VideoAfterQuest = "";

                DoQuestCheckDelay("PostVideo_Start", 0.1); */
            }
        break;
        
        case "GhostShipHelp": // битва в порту
            chrDisableReloadToLocation = true;
            Pchar.GenQuestFort.FarLocator = true;
			sTemp = "officers"; // не везде есть! :(
            for (i = 0; i < MAX_TOWN_CREW; i++)
            {
				sld = SetFantomSkeletForts(sTemp, "", "TOWN_BATTLE_SOLDIERS", "GhostShipCrew");
				if (sld.location.locator == "")
				{
                    sTemp = "rld";
					sld = SetFantomSkeletForts(sTemp, "", "TOWN_BATTLE_SOLDIERS", "GhostShipCrew");
					if (sld.location.locator == "") break;
				}
            }
            // наши
            Pchar.GenQuestFort.FarLocator = false;
            // помощь эскадре
            iTemp = sti(Characters[GetCharacterIndex(pchar.GenQuest.GhostShipWorkId)].nation);

            for (i = 0; i < MAX_ESCADRA_CREW; i++) //всегда 5 даем
            {
            	SetFantomDefenceForts("goto", "", iTemp, LAI_GROUP_PLAYER);
            }
            // натравим
            LAi_group_SetHearRadius("TOWN_BATTLE_SOLDIERS", 100.0);
            LAi_group_FightGroupsEx("TOWN_BATTLE_SOLDIERS", LAI_GROUP_PLAYER, true, Pchar, -1, false, false);
            LAi_group_SetRelation("TOWN_BATTLE_SOLDIERS", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);

            LAi_group_SetCheck("TOWN_BATTLE_SOLDIERS", "GhostShipHelp_Finish");
            LAi_SetFightMode(Pchar, true);
        break;

		case "GhostShipHelp_Finish":
            LAi_SetFightMode(Pchar, false);
            LAi_grp_alarmactive = false; // конец тревоги

            if (!CheckAttribute(pchar , "GenQuest.GhostShip.ChurchHelpStart"))
            {  // город
                sTemp = "GhostShipDone_" + pchar.GenQuest.GhostShipWorkId;
                pchar.GenQuest.(sTemp) = true;
                Log_Info("Мы отбились!");
                Statistic_AddValue(Pchar, "QuestGhostShipHelp_Done", 1);
                AddQuestRecord("GhostShipQuest", "help_t2");
            }
            else
            { // бухта
                pchar.GenQuest.GhostShip.ChurchHelp = true;
                Log_Info("Задание церкви выполнено!");
                DeleteAttribute(pchar , "GenQuest.GhostShip.ChurchHelpStart")
                //AddQuestTemplate("GhostShipQuest", "church_t2");
                Statistic_AddValue(Pchar, "QuestChurchHelpGhostShip_Done", 1);
            }
            PlaySound("interface\door_locked.wav");
            chrDisableReloadToLocation = false;
            bDisableFastReload         = false;
            SetLocationCapturedState(pchar.location, false);
        break;
        // помощь церкви
        case "GhostShipChurchShorePlace":
            Pchar.GenQuest.Hunter2Pause            = true;

            Pchar.GenQuest.GhostShip.ChurchHelpStart = true;

            chrDisableReloadToLocation = true;
            Pchar.GenQuestFort.FarLocator = true;
			sTemp = "officers"; // не везде есть! :(
            iTemp = 3 + rand(4);
            for (i = 0; i < iTemp; i++)
            {
				sld = SetFantomSkeletForts(sTemp, "", "SKEL_BATTLE_SOLDIERS", "GhostShipCrew");
				if (sld.location.locator == "")
				{
                    sTemp = "reload"; // опасно - может быть лодка!
					sld = SetFantomSkeletForts(sTemp, "", "SKEL_BATTLE_SOLDIERS", "GhostShipCrew");
					if (sld.location.locator == "") break;
				}
            }
            Pchar.GenQuestFort.FarLocator = false;

            // натравим
            LAi_group_SetHearRadius("SKEL_BATTLE_SOLDIERS", 100.0);
            LAi_group_FightGroupsEx("SKEL_BATTLE_SOLDIERS", LAI_GROUP_PLAYER, true, Pchar, -1, false, false);
            LAi_group_SetRelation("SKEL_BATTLE_SOLDIERS", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);

            LAi_group_SetCheck("SKEL_BATTLE_SOLDIERS", "GhostShipHelp_Finish");
            LAi_SetFightMode(Pchar, true);
        break;
        // GhostShipHelp <--
		////////////////////////////////////Квест награда за голову///////////////////////////////////////////////////////
        case "SeaHunterCheck":
            SeaHunterCheck();
        break;

		case "CheckMaxHealthQuest":
            SetTimerCondition("CheckMaxHealthQuest", 0, 0, 5, true);
            SetNewDayHealthMax();
        break;
        
		case "My_Deck_Battle_End":  // запрет битв в трюме -после казни пленных
            LAi_LocationFightDisable(&Locations[FindLocation("My_Deck")], true);
            LAi_SetFightMode(Pchar, false);
        break;
		//  boal 290804 наказание битьем морды -->
        case "PunishmentAction":
            LAi_SetActorTypeNoGroup(PChar);
            sld = &Characters[makeint(Pchar.GenQuest.FreeRandomOfficerIdx)];
			LAi_SetActorTypeNoGroup(sld);
            LAi_ActorTurnToCharacter(pchar, sld);
            LAi_ActorAnimation(pchar, "attack_force_3", "", 0.9);
            DoQuestCheckDelay("PunishmentAction_1", 0.5);
         break;

         case "PunishmentAction_1":
            sld = &Characters[makeint(Pchar.GenQuest.FreeRandomOfficerIdx)];
            LAi_ActorAnimation(sld, "hit_attack_2", "PunishmentAction_2", 0.6);
         break;

         case "PunishmentAction_2":
            Lai_SetPlayerType(pchar);
            sld = &Characters[makeint(Pchar.GenQuest.FreeRandomOfficerIdx)];
            switch (pchar.GenQuest.PunishmentActionManType)
            {
                case "LAi_SetHuberStayType":
                    LAi_SetHuberStayTypeNoGroup(sld);
                    break;

                case "LAi_SetStayType":
                    LAi_SetCitizenTypeNoGroup(sld);
                    break;
            }
		break;
		//  boal 290804 наказание битьем морды <--
		// индульгенция -->
		case "donation":
			int iDonation = makeint(pchar.questTemp.donate);
			int iRep      = makeint(pchar.reputation);
			int iRepIncr;
			if (iRep <10)
			{
				if (iDonation >= 1000)
				{
					iRepIncr = iDonation / 1000;
					ChangeCharacterReputation(pchar, iRepIncr);
					pchar.questTemp.donate = 0;
				}
			}
			if (iRep < 20 && iRep >= 10)
			{
				if (iDonation >= 2000)
				{
					iRepIncr = iDonation/2000;
					ChangeCharacterReputation(pchar, iRepIncr);
					pchar.questTemp.donate = 0;
				}
			}
			if (iRep <30 && iRep >=20)
			{
				if (iDonation >= 3000)
				{
					iRepIncr = iDonation/3000;
					ChangeCharacterReputation(pchar, iRepIncr);
					pchar.questTemp.donate = 0;
				}
			}
			if (iRep <40 && iRep >=30)
			{
				if (iDonation >= 4000)
				{
					iRepIncr = iDonation/4000;
					ChangeCharacterReputation(pchar, iRepIncr);
					pchar.questTemp.donate = 0;
				}
			}
			if (iRep <50 && iRep >=40)
			{
				if (iDonation >= 5000)
				{
					iRepIncr = iDonation/5000;
					ChangeCharacterReputation(pchar, iRepIncr);
					pchar.questTemp.donate = 0;
				}
			}
			if (iRep <60 && iRep >=50)
			{
				if (iDonation >= 6000)
				{
					iRepIncr = iDonation/6000;
					ChangeCharacterReputation(pchar, iRepIncr);
					pchar.questTemp.donate = 0;
				}
			}
			if (iRep <70 && iRep >=60)
			{
				if (iDonation >= 7000)
				{
					iRepIncr = iDonation/7000;
					ChangeCharacterReputation(pchar, iRepIncr);
					pchar.questTemp.donate = 0;
				}
			}
			if (iRep <80 && iRep >=70)
			{
				if (iDonation >= 8000)
				{
					iRepIncr = iDonation/8000;
					ChangeCharacterReputation(pchar, iRepIncr);
					pchar.questTemp.donate = 0;
				}
			}
			if (iRep < REPUTATION_MAX && iRep >=80)
			{
				if (iDonation >= 10000)
				{
					iRepIncr = iDonation/10000;
					ChangeCharacterReputation(pchar, iRepIncr);
					pchar.questTemp.donate = 0;
				}
			}
		break;
	    // индульгенция <--
		case "SetNPCInShipDeck":  // народ внутри нашего корабля
            if (Pchar.location == Get_My_Cabin())//"My_Cabin" || Pchar.location == "My_Cabin_Small")
	        {
                SetOfficersInCabin();
            }
            else
            {
                if (Pchar.location == "My_Deck")// трюм
                {
                    SetPrisonerInHold();
                }
                else
                {
	                if (Pchar.location == "My_Campus")// кают-компания
	                {
	                    SetOfficersInCampus();
	                }
                }
            }
        break;
        
        case "Munity_on_Ship_Map":
            if (IsEntity(worldMap) && GetCrewQuantity(pchar) > 0)
        	{
                MunityOnShip("ShipMunity");
            }
        break;

        case "Munity_on_Ship":
            pchar.quest.Munity = "";
            pchar.GenQuest.MunityStart = true;
            PlaySound("INTERFACE\_GTBoard2.wav");
            InterfaceStates.Buttons.Save.enable = 0;
            LAi_SetFightMode(Pchar, true);

            if (!CheckAttribute(pchar, "GenQuest.SlavesMunity"))
            {
                for (i=1; i<=9; i++)
                {
                    sld = SetFantomDefenceForts("goto", "", PIRATE, LAI_GROUP_TmpEnemy);
                    if (i >= (sti(PChar.Ship.Crew.Quantity)/((8-GetCharacterShipClass(PChar))*8))) break;
                    RemoveCharacterCrew(PChar, (8-GetCharacterShipClass(PChar))*8);
                }
                PChar.Ship.Crew.Morale = 25;
                ChangeCrewExp(PChar, "Soldiers", 2); // бонус на подавление
            }
            else
            {
                for (i=1; i<=9; i++)
                {
                    sld = SetFantomSlaves("goto", "", LAI_GROUP_TmpEnemy);
                    if (i >= (GetCargoGoods(PChar, GOOD_SLAVES)/((8-GetCharacterShipClass(PChar))*8))) break;
                    RemoveCharacterGoodsSelf(PChar, GOOD_SLAVES, (8-GetCharacterShipClass(PChar))*8);
                }
                DeleteAttribute(pchar, "GenQuest.SlavesMunity");
                ChangeCrewExp(PChar, "Soldiers", 3); // бонус на подавление
            }
            LAi_group_SetHearRadius(LAI_GROUP_TmpEnemy, 100.0);
            LAi_group_FightGroups(LAI_GROUP_TmpEnemy, LAI_GROUP_PLAYER, true);
            LAi_group_SetRelation(LAI_GROUP_TmpEnemy, LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);

            LAi_group_SetCheck(LAI_GROUP_TmpEnemy, "Dead_Munity");
        break;

        case "Dead_Munity":
            Log_SetStringToLog("Восстание было подавлено");
            InterfaceStates.Buttons.Save.enable = 1;
            pchar.quest.Munity = "Deads";
            LAi_SetFightMode(Pchar, false);
            DeleteAttribute(pchar, "GenQuest.MunityStart");
        break;
        
		///  захват форта - город
        case "FreeCaptureForts": // уберем солдат, откроем входы (на всяк случай) при выходе на карту
            TWN_FreeCaptureForts();
        break;
        // захват с суши по диалогу мэра (телепорт)
        case "Town_Capture_Land":
            sld = GetCharacter(sti(Pchar.GenQuestFort.fortCharacterIdx));
            //Pchar.GenQuestFort.SoundOff = true;
            DeleteQuestAttribute("Union_with_Escadra");

            SetLocationCapturedState(sld.Default, true);

            DoQuestReloadToLocation(sld.Default.BoardLocation, sld.Default.BoardGroup, sld.Default.BoardLocator, "Capture_Forts");
        break;
        
        case "Capture_Town_Siege":
            PrepareTownBattle(); // тут только джунгли для этого случая

			if (!CheckAttribute(Pchar, "GenQuestFort.SoundOff"))
			{
            	PlaySound("INTERFACE\_GTTown2.wav");
            	AddCharacterExpToSkill(Pchar, "Leadership", 100);
			    AddCharacterExpToSkill(Pchar, "Sneak", 100);
            }
            else
            {
                DeleteAttribute(Pchar, "GenQuestFort.SoundOff");
            }
        break;
        // Возвращение губернаторам родные диалоги // boal переделал 251204
        case "Return_Gover_Dialog":
            Characters[sti(PChar.GenQuest.GoverIdx)].dialog.CurrentNode = "First time";
        break;

        case "Return_Gover_Dialog_Exit":
            DoQuestCheckDelay("Return_Gover_Dialog", 0.5);
        break;
        // после битвы в форте сюда
        case "Capture_Forts":
            TWN_Capture_Forts();
        break;

        case "FightInTown":
            TWN_FightInTown();
        break;

        case "FightInTown_OpenNext":
            TWN_FightInTown_OpenNext();
        break;

        case "FightInTown_Finish":
			// в диалоге разрешим - а то можно слинять заранее
            //chrDisableReloadToLocation = false;
            //bDisableFastReload = false; // только после всего захвата
            //LAi_SetFightMode(Pchar, false);
            LAi_grp_alarmactive = false; // конец тревоги

            //LAi_LocationFightDisable(&Locations[FindLocation(Pchar.location)], true); eddy. не лочить боевки в резиденции
			LAi_LockFightMode(pchar, false);
            LAi_SetFightMode(Pchar, false);
        break;

        //Два пути захвата форта
        // 1 уходим с добычей - в диалоге мэра

        // 2 Если оставляем форт себе
        case "Residence_Captured_Any":
            TWN_Residence_Captured_Any();
        break;

        case "Residence_Captured_Any_02":
   			TWN_Residence_Captured_Any_02();
        break;

        case "Back_Mayor_Type":
            TWN_Back_Mayor_Type();
        break;

        case "CapturedToOurCity":
			PlayStereoOGG("music_ship_victory");
			sld = GetCharacter(sti(Pchar.GenQuestFort.fortCharacterIdx));
            //DeleteQuestHeader("tales_" + sld.City);  // to_do
            //AddQuestRecord("tales_" + sld.City, "1");

            Log_Info("Город " + GetCityName(sld.City) + " теперь наш!");

			// убрать контрабандиста
			sTemp = sld.City + "_Smuggler";
			if (GetCharacterIndex(sTemp) >= 0) // не везде есть
			{
				ChangeCharacterAddress(characterFromID(sTemp), "None", ""); // вся логика теперь в рассидки таверны
			}
            SetCaptureTownByHero(sld.City);
            LaunchColonyInfoScreen(); // табличка   to_do дает вылет :(
			// отсчет для отбития
			//if (!isCityHasFort(sld.City)) // сухопутный город
			sTemp = "Attack_Fort_" + sld.City;
            SetTimerCondition(sTemp, 0, 0, 25 + rand(70), false);
            PChar.Quest.(sTemp).CityId   = sld.City;
			PChar.Quest.(sTemp).function = "TWN_Siege_Any";
        break;
		///////////////////////////////// захват города <--
		//  системные фичи
		case "PostVideo_Start":
            bMainMenuLaunchAfterVideo = true;
			PostVideoAndQuest(PChar.GenQuest.VideoAVI, 100, "PostVideo_After");
        break;

        case "PostVideo_After":
            PostEvent("DoInfoShower",100,"s","");
			DoQuestCheckDelay(PChar.GenQuest.VideoAfterQuest, 0.1);
        break;

        case "CallFunctionParam":
            attrName = PChar.GenQuest.CallFunctionParam;
            call attrName();
        break;
        
        // вызов табличкм А в это время..
        case "LaunchFrameForm":
            PChar.GenQuest.CallFunctionParam = "LaunchFrameForm";
			DoQuestCheckDelay("CallFunctionParam", stf(pchar.AutoQuest.FrameDelay));
        break;
        
        case "Move_Govenour":  // переместим губернатора в другой город
            SetTimerCondition("Move_Govenour", 0, 0, 25+rand(10), true);
            Move_Govenour();
        break;
        
		case "pchar_back_to_player":
			Lai_SetPlayerType(pchar);
		break;

        case "God_hit_us": // это такой прикол - задействовать в ловушки для сундуков(boal)
			iTemp = 10+rand(15);
			if ((MakeInt(pchar.chr_ai.hp)-iTemp) > 0)
            {
    			LAi_SetActorType(PChar);
                LAi_ActorAnimation(PChar, "hit_attack_2", "pchar_back_to_player", 1.0);
            }
			LAi_ApplyCharacterDamage(pchar, iTemp);
			SendMessage(pchar, "lfff", MSG_CHARACTER_VIEWDAMAGE, iTemp, MakeFloat(MakeInt(pchar.chr_ai.hp)), MakeFloat(MakeInt(pchar.chr_ai.hp_max)));
			LAi_CheckKillCharacter(pchar);
		break;
		// диалог с ГГ генератор
        case "TalkSelf_Start":
            StartActorSelfDialog("TalkSelf_Main");
        break;
        // диалог с ГГ квестовые реплики
        case "TalkSelf_Quest":
            StartActorSelfDialog("First time");
        break;
        /////  сидим за столом в таверне ->
        case "alc":
			LAi_Fade("alc_2_prepare", "alc2");
		break;

		case "alc_2_prepare":
			LAi_SetSitType(pchar);
			LAi_SetActorType(characterFromID(pchar.questTemp.friend_in_tavern));
			LAi_ActorSetSitMode(characterFromID(pchar.questTemp.friend_in_tavern));

            sld = characterFromID(pchar.questTemp.friend_in_tavern);
            FreeSitLocator(sld.Default, sld.Default.ToLocator);  // очистим стул
			ChangeCharacterAddressGroup(pchar, sld.Default, sld.Default.group, sld.Default.ToLocator);
		break;

		case "alc2":
			LAi_ActorDialogNow(characterFromID(pchar.questTemp.friend_in_tavern), pchar, "", -1);
		break;

		case "exit_sit":
			if (CheckQuestAttribute("fight_in_tavern", "fight"))
			{
				pchar.quest.fight_in_tavern = "0";
				LAi_SetSitType(characterFromID(pchar.questTemp.friend_in_tavern));
			}
			else
			{
				if (pchar.location.group == "sit")
				{
					LAi_Fade("exit_sit_2", "");
				}
			}
		break;

		case "exit_sit_2":
   			sld = characterFromID(pchar.questTemp.friend_in_tavern);
			LAi_SetSitType(sld);
			LAi_SetPlayerType(pchar);
			//ChangeCharacterAddress(pchar, sld.Default, sld.Default.ToGo);
			PlaceCharacter(pchar, "tables", pchar.location); // ближний к столу
		break;
		// реплика таверншика о запрете драк -->
		case "tavern_keeper":
			attrName = GetCharIDByParam("TavernMan", "location", pchar.location);
            sld = characterFromID(attrName);
			sld.dialog.currentnode = "tavern_keeper";
			LAi_ActorWaitDialog(pchar, sld);
			LAi_SetActorType(sld);
			LAi_ActorDialogNow(sld, pchar, "tavern_keeper_2", -1);
		break;

		case "tavern_keeper_2":
            attrName = GetCharIDByParam("TavernMan", "location", pchar.location);
            sld = characterFromID(attrName);
			sld.dialog.currentnode = "First time";
			LAi_SetBarmanType(sld);
		break;
		// реплика таверншика о запрете драк <--
 		case "without_memory":
			LAi_Fade("exit_sit_2", "saved_by_off");
			bQuestCheckProcessFreeze = true;
			WaitDate("",0,0,0, 5, 30);
			bQuestCheckProcessFreeze = false;
			RefreshLandTime();
		break;

		case "saved_by_off":
			LAi_SetStayType(pchar);
			Log_Info("Кто-то порылся в карманах.");
   			AddMoneyToCharacter(pchar, -100 - rand(5000));
   			sld = characterFromID(loadedLocation.fastreload + "_waitress");
   			pchar.questTemp.officiantLocator = sld.location.locator;
   			pchar.questTemp.officiantGroup   = sld.location.group;   // запомнить, где была
			PlaceCharacter(sld, "goto", pchar.location);
			sld.dialog.currentnode = "without_money";
			LAi_SetActorType(sld);
			LAi_ActorDialog(sld, pchar, "officiant_back_to_citizen", 5.0, 1.0);
			DoQuestCheckDelay("pchar_back_to_player", 6.0);
		break;

		case "officiant_back_to_citizen":
			LAi_SetPlayerType(pchar);
            sld = characterFromID(loadedLocation.fastreload + "_waitress");
			LAi_SetWaitressType(sld);
			sld.location.locator = pchar.questTemp.officiantLocator;
			sld.location.group   = pchar.questTemp.officiantGroup;
		break;
		//  агенты стран в таверне -->
        case "NationUpdate":
            UpdateRelations();
            if(bSeaActive)
            {
                RefreshBattleInterface();
            }
		break;

        case "pir_flag_rise":
			Flag_PIRATE();
		break;
		// агенты в тавернах -->
        case "any_patent_take":
            AddMoneyToCharacter(pchar, -sti(pchar.PatentPrice));

            RemovePatent();

			GiveItem2Character(pchar, "patent_" + pchar.PatentNation);
            EquipCharacterbyItem(pchar, "patent_" + pchar.PatentNation);

            Items[sti(pchar.EquipedPatentId)].TitulCur = 1; // текущ звание сбросим
        	Items[sti(pchar.EquipedPatentId)].TitulCurNext = 0; // счетчик звание сбросим
		break;

        case "fra_flag_rise":
            Flag_FRANCE();
        break;

        case "eng_flag_rise":
            Flag_ENGLAND();
        break;

        case "spa_flag_rise":
            Flag_SPAIN();
        break;

        case "hol_flag_rise": 
            Flag_HOLLAND();
        break;

        case "QuestAboardCabinDialog":  // диалог в абордаже, в каюте при достижении минНР
			sld = &Characters[sti(pchar.GenQuest.QuestAboardCabinDialogIdx)]; // фантом, тень отца капитана
			LAi_SetActorType(pchar);
            LAi_SetActorType(sld);
            SetActorDialogAny2Pchar(sld.id, "", 0.0, 0.0);
		    LAi_ActorFollow(pchar, sld, "ActorDialog_Any2Pchar", 0.5);
		break;
		
		case "LAi_ReloadBoarding": // задержка нужна
			LAi_EnableReload();
		    DelEventHandler("Control Activation", "LAi_ActivateReload");
			Log_SetActiveAction("Nothing");
			LAi_ReloadBoarding();
		break;
		// boal <---
		
        ////////////////////////////////////////////////////////////////////////
		//  Служебный квест контрабандистов - ограничение по времени
		////////////////////////////////////////////////////////////////////////
		case "Rand_Smuggling":
			//слишком падает ChangeContrabandRelation(pchar, -10); // boal
			pchar.quest.KillSmugglers_after.over = "yes";  
			RemoveSmugglersFromShore();
		break;

		//Постановка стражников в локацию передачи контрабандистов
		case "Rand_ContrabandInterruption":
			chrDisableReloadToLocation = true;
			LAi_SetFightMode(pchar, false);
			LAi_LockFightMode(pchar, true);
			// солдаты в начале
            iTemp = sti(pchar.GenQuest.Contraband.GuardNation);// Нация патруля
			Pchar.quest.contraband.SoldierQty = makeint(2*GetOfficersQuantity(Pchar) + 3);
			for (i = 2; i <= sti(Pchar.quest.contraband.SoldierQty); i++)
			{
    			sld = SetFantomDefenceForts("", "", iTemp, "CoastalGuards");
    			attrName = "SoldierIDX"+i;
    			Pchar.quest.contraband.(attrName) = sld.index;
    			LAi_SetActorType(sld);
    			LAi_ActorFollow(sld, Pchar, "", 38);
    			sld.Dialog.Filename = "Smuggler_Guards_dialog.c";
				sld.Dialog.CurrentNode = "First time";
				sld.greeting = "Gr_Costal_Guards";
            }
			// вызовем независимо!!!! не тут :) SetCoastalGuardPursuit();

			// офицер в конце
            sld = SetFantomOfficer("", "", iTemp, "CoastalGuards");
			Pchar.quest.contraband.SoldierIDX1 = sld.index;
			LAi_SetActorType(sld);
			sld.Dialog.Filename = "Smuggler_Guards_dialog.c";
			sld.Dialog.CurrentNode = "First time";
			sld.greeting = "Gr_Costal_Guards";

			LAi_ActorDialog(&Characters[makeint(Pchar.quest.contraband.SoldierIDX1)], Pchar, "", 35, 1); // boal 120c - озвереть ждать!!!
			LAi_group_SetCheck("CoastalGuards", "CoastalGuardsAllDead");
			//LAi_SetActorType(Pchar); //fix
			//LAi_ActorFollow(PChar, &Characters[makeint(Pchar.quest.contraband.SoldierIDX1)], "", 35);
			//Lai_QuestDelay("Rand_CoastalPatrolAppear", 3.0);
		break;

		/*case "Rand_CoastalPatrolAppear":
			//Trace("QUEST Rand_CoastalPatrolAppear reports: Soldier Idx = " + Pchar.quest.contraband.SoldierIDX1);
			//StartQuestMovie(true, true, true);
			//LAi_SetActorTypeNoGroup(Pchar);
			LAi_SetActorType(Pchar);
			LAi_ActorTurnToCharacter(Pchar, &Characters[makeint(Pchar.quest.contraband.SoldierIDX1)]);
			LAi_ActorWaitDialog(Pchar, &Characters[makeint(Pchar.quest.contraband.SoldierIDX1)]);
			//LAi_SetPlayerType(Pchar);
		break;*/

		case "Rand_ContrabandInterruptionAtSeaEnded":
			StopCoastalGuardPursuit();
		break;

		case "Rand_ContrabandAtSeaEnded":
			StopCoastalGuardPursuit();
		break;

		case "CoastalGuardsAllDead":
			for (i=1; i<4; i++)
			{
				sTemp = "Rand_Smug0" + i;
				LAi_SetGuardianTypeNoGroup(CharacterFromID(sTemp));
			}
			LAi_grp_alarmactive = false; // конец тревоги
		break;
		//  контрабанды <--
		
		/////////////////////////////////////////////////////////////////////////////////
		// НАЧАЛО КВЕСТА - УДАЛЕНИЕ ТЕКУЩЕГО НАПРАВЛЕНИЯ И СИЛЫ ВЕТРА
		/////////////////////////////////////////////////////////////////////////////////
		case "EraseWind":
			DeleteAttribute(pchar, "wind");
		break;
		/////////////////////////////////////////////////////////////////////////////////
		// КОНЕЦ КВЕСТА - УДАЛЕНИЕ ТЕКУЩЕГО НАПРАВЛЕНИЯ И СИЛЫ ВЕТРА
		/////////////////////////////////////////////////////////////////////////////////

		/////////////////////////////////////////////////////////////////////////////////
		//	СЛУЖЕБНЫЕ ФИЧИ
		/////////////////////////////////////////////////////////////////////////////////
		case "hide_weapon":
			LAi_SetFightMode(pchar, false);
			break;

		case "AnyReloadToLocation":
			int iMonth, iDay, iHour, iMin;

			iMonth	= sti(PChar.AutoQuest.delayMonth);
			iDay	= sti(PChar.AutoQuest.delayDay);
			iHour	= sti(PChar.AutoQuest.delayHour);
			iMin	= sti(PChar.AutoQuest.delayMin);

			if(iMonth > 0 || iDay > 0 || iHour > 0 || iMin > 0)
			{
                bQuestCheckProcessFreeze = true;
				WaitDate("", 0, iMonth, iDay, iHour, iMin);
				bQuestCheckProcessFreeze = false;
			}
			DoQuestReloadToLocation(PChar.AutoQuest.idLocation, PChar.AutoQuest.idGroup, PChar.AutoQuest.idLocator, PChar.AutoQuest.questName);
			break;
		/////////////////////////////////////////////////////////////////////////////////
		//	СЛУЖЕБНЫЕ ФИЧИ
		/////////////////////////////////////////////////////////////////////////////////

		/////////////////////////////////////////////////////////////////////////////////
		//	START OF ПГГ
		/////////////////////////////////////////////////////////////////////////////////
		case "PGG_Companion_Leave":
			chrDisableReloadToLocation = true;
			sld = &Characters[sti(pchar.questTemp.PGG_Companion_Leave.index)];
			PlaceCharacter(sld, "goto", "random_must_be_near");

			sld.Dialog.CurrentNode = "companion_leave";
			LAi_SetActorType(sld);
			LAi_ActorDialog(sld, pchar, "", -1, 0);
			break;

		case "RemoveCharacterFromLocation":
			sld = CharacterFromID(PChar.questTemp.Chr2Remove);
			ChangeCharacterAddressGroup(sld, "None", "", "");
			LAi_SetImmortal(sld, false);
			break;

		case "PGG_Q1AfterShoreFight":
			PGG_Q1AfterShoreFight();
			break;

		case "PGG_Q1AfterDeckFight":
			PGG_Q1AfterDeckFight();
			break;
		/////////////////////////////////////////////////////////////////////////////////
		//	END OF ПГГ
		/////////////////////////////////////////////////////////////////////////////////

		/////////////////////////////////////////////////////////////////////////////////
		//	Корсарское метро
		/////////////////////////////////////////////////////////////////////////////////
		case "RemoveTravelSmugglers":
			if (!CheckAttribute(PChar, "GenQuest.contraTravel.PatrolFight")) ChangeContrabandRelation(PChar, -10);
			RemoveSmugglersFromShore();

			if (!CheckAttribute(PChar, "GenQuest.contraTravel.ship")) 
			{
				AddQuestRecord("Gen_ContrabandTravel", "3");
				DeleteAttribute(PChar, "GenQuest.contraTravel");
				CloseQuestHeader("Gen_ContrabandTravel");
			}

			sld = CharacterFromID("Abracham_Gray");
			sld.location.from_sea = "";
			break;

		case "Travel_talkOnDeck":
			//убираем контрабандистов
			RemoveSmugglersFromShore();
			//кинозвезда Абрахам Грей :)
			sld = CharacterFromID("Abracham_Gray");
			//параметры..
			sld.nation = PIRATE;
			Ship_FlagRefresh(PChar); //флаг на лету
			sld.location.from_sea = "";

			LAi_group_MoveCharacter(sld, "TmpEnemy");
			LAi_SetActorType(sld);
			LAi_SetActorType(pchar);

			SetActorDialogAny2Pchar("Abracham_Gray", "", 2.0, 0.0);
			LAi_ActorFollow(sld, pchar, "ActorDialog_Any2Pchar", 2.0);
			LAi_ActorFollow(pchar, sld, "", 1.0);
			//компания....
			iTemp = 3 + rand(3);
			for(i = 0; i < iTemp; i++)
			{
				sTemp = "pirat" + (2 + rand(5));
				attrName = "goto" + (3 + i);
				sld = SetFantomDefenceForts("goto", attrName, PIRATE, "TmpEnemy");
			}
			LAi_group_SetRelation("TmpEnemy", LAI_GROUP_PLAYER, LAI_GROUP_FRIEND);
			LAi_group_ClearAllTargets();
			LAi_SetFightModeForOfficers(false);
			break;

		case "Travel_AfterDeckFight":
			AddQuestRecord("Gen_ContrabandTravel", "5");
			StartActorSelfDialog("GenTravel_Main");
			break;
		/////////////////////////////////////////////////////////////////////////////////
		//	END OF Корсарское метро
		/////////////////////////////////////////////////////////////////////////////////

		/////////////////////////////////////////////////////////////////////////////////
		// НАЧАЛО КВЕСТА - ДОСТАВКА ГРУЗА
		/////////////////////////////////////////////////////////////////////////////////
		case "trade_quest_open":
			int iTradeGoods    = sti(pchar.CargoQuest.iTradeGoods);
			int iQuantityGoods = sti(pchar.CargoQuest.iQuantityGoods);
			int iTradeNation   = sti(pchar.CargoQuest.iTradeNation);
			int iMoney         = sti(pchar.CargoQuest.iMoney);
            AddCharacterGoods(pchar, iTradeGoods, iQuantityGoods);
			//задаем квест
			SetTimerCondition("generate_trade_quest", 0, 0, sti(pchar.CargoQuest.iDaysExpired), false);

			pchar.quest.generate_trade_quest_progress = "begin";
			ReOpenQuestHeader("DELIVERY_TRADE_QUEST");
			AddQuestRecord("DELIVERY_TRADE_QUEST", "1");

			AddQuestUserDataForTitle("DELIVERY_TRADE_QUEST", "sGoodGen", GetGoodsNameAlt(iTradeGoods));
			AddQuestUserDataForTitle("DELIVERY_TRADE_QUEST", "sTargetColony", XI_ConvertString("Colony"+pchar.CargoQuest.iTradeColony+"Gen"));

			AddQuestUserData("DELIVERY_TRADE_QUEST", "sGoodGen", GetGoodsNameAlt(iTradeGoods));
			AddQuestUserData("DELIVERY_TRADE_QUEST", "sGoodQty", FindRussianQtyString(sti(pchar.CargoQuest.iQuantityGoods)));
			AddQuestUserData("DELIVERY_TRADE_QUEST", "sBaseColony", XI_ConvertString("Colony"+characters[GetCharacterIndex(pchar.CargoQuest.GiveTraderID)].city+"Gen"));
			AddQuestUserData("DELIVERY_TRADE_QUEST", "sTargetColony",XI_ConvertString("Colony"+pchar.CargoQuest.iTradeColony+"Gen"));
            attrName = "";
            if (pchar.CargoQuest.iTradeIsland != pchar.CargoQuest.iTradeColony)
            {
                attrName = ", что находится на " + XI_ConvertString(pchar.CargoQuest.iTradeIsland+"Dat");
            }
            AddQuestUserData("DELIVERY_TRADE_QUEST", "island", attrName);
    		AddQuestUserData("DELIVERY_TRADE_QUEST", "sTermsDelivery", FindRussianDaysString(makeint(pchar.CargoQuest.iDaysExpired)));
    		AddQuestUserData("DELIVERY_TRADE_QUEST", "sMoney", FindRussianMoneyString(iMoney));
            // немного веселой жизни
            TraderHunterOnMap();
		break;
		
		case "generate_trade_quest":
			if (pchar.quest.generate_trade_quest_progress == "begin")
			{
				pchar.quest.generate_trade_quest_progress = "failed";
				Log_SetStringToLog("Время для поставки груза истекло.");
				ChangeCharacterReputation(pchar, -10);
				OfficersReaction("bad");
				ChangeCharacterHunterScore(GetMainCharacter(), NationShortName(sti(characters[GetCharacterIndex(pchar.CargoQuest.TraderID)].nation)) + "hunter", 15);
                AddQuestRecord("DELIVERY_TRADE_QUEST", "3");
                AddQuestUserData("DELIVERY_TRADE_QUEST", "sGoodGen", GetGoodsNameAlt(sti(pchar.CargoQuest.iTradeGoods)));
			    AddQuestUserData("DELIVERY_TRADE_QUEST", "sTargetColony",XI_ConvertString("Colony"+pchar.CargoQuest.iTradeColony+"Gen"));
			}
		break;
		/////////////////////////////////////////////////////////////////////////////////
		// КОНЕЦ КВЕСТА - ДОСТАВКА ГРУЗА
		/////////////////////////////////////////////////////////////////////////////////
		
		/////////////////////////////////////////////////////////////////////////////////
		// НАЧАЛО КВЕСТА - СПИМ В ТАВЕРНЕ
		/////////////////////////////////////////////////////////////////////////////////
		case "sleep_in_tavern":
			if (CheckAttribute(pchar, "QuestTemp.TakeShotgun") && rand(3) == 1 && !CheckAttribute(pchar , "GenQuest.GhostShip.LastBattleEnd")) // пасхалка
			{
			    DeleteAttribute(pchar, "QuestTemp.TakeShotgun");
			    PChar.GenQuest.CallFunctionParam = "QuestShotgunT102";
			    DoQuestReloadToLocation(pchar.location + "_upstairs", "goto", "goto1", "CallFunctionParam");
			}
			else
			{
				DoQuestReloadToLocation(pchar.location + "_upstairs", "goto", "goto1", "restore_hp");
			}
		break;
		
		case "restore_hp":
			int iOfficer;
			LAi_SetCurHPMax(pchar);
			for (i=1; i<4; i++)
			{
				if (GetOfficersIndex(Pchar, i) != -1)
				{
					iOfficer = GetOfficersIndex(Pchar, i);
					LAi_SetCurHPMax(&characters[iOfficer]);
				}
			}			
		break;
		/////////////////////////////////////////////////////////////////////////////////
		// КОНЕЦ КВЕСТА - СПИМ В ТАВЕРНЕ
		/////////////////////////////////////////////////////////////////////////////////

 		////////////////////////////////////////////////////////////////////////
		//  Начало    LOAN - ОГРАНИЧЕНИЕ ПО ВРЕМЕНИ
		////////////////////////////////////////////////////////////////////////
		case "LoanFindCitizenTimer": // должник
            Log_SetStringToLog("Вы не вернули деньги должника работодателю.");
            LoansMoneyAvengerAmount(&Characters[GetCharacterIndex(pchar.GenQuest.Loan.FindCitizenNpcharId)], MOD_SKILL_ENEMY_RATE+rand(20));
            //DeleteAttribute(pchar, "GenQuest.Loan");
		break;

		case "LoanChestTimer": // сундук
            Log_SetStringToLog("Вы не доставили кредит по назначению.");
            LoansMoneyAvengerAmount(&Characters[GetCharacterIndex(pchar.GenQuest.LoanChest.ChestNpcharId)], MOD_SKILL_ENEMY_RATE+rand(5)); // кто дал
            LoansMoneyAvengerAmount(&Characters[sti(pchar.GenQuest.LoanChest.TargetIdx)], MOD_SKILL_ENEMY_RATE+rand(5));// кому везли
            pchar.GenQuest.LoanChest.Money.Discount = (rand(3)+1)*10; //процент удержания гонорара за просрочку
			//DeleteAttribute(pchar, "GenQuest.LoanChest");
            AddQuestRecord("Gen_LoanTakeChest", "2");
            //CloseQuestHeader("Gen_LoanTakeChest");
		break;

		case "LoansForAll":			
			sTemp = pchar.quest.(qname).CityId;
			LoansMoneyAvenger(&Characters[GetCharacterIndex(sTemp + "_usurer")]);
			Characters[GetCharacterIndex(sTemp + "_usurer")].Dialog.CurrentNode = "DeadMotherfucker";
		break;
 		////////////////////////////////////////////////////////////////////////
		//  Конец    LOAN - ОГРАНИЧЕНИЕ ПО ВРЕМЕНИ
		////////////////////////////////////////////////////////////////////////				
		
 		////////////////////////////////////////////////////////////////////////
		//  Начало    Сопровождение торговца
		////////////////////////////////////////////////////////////////////////
		case "prepare_for_convoy_quest":
			iTemp = sti(colonies[FindColony(pchar.quest.destination)].nation);
            sld = GetCharacter(NPC_GenerateCharacter("QuestTrader", "", "man", "man", 10, iTemp, -1, true));
			SetCaptanModelByEncType(sld, "trade");
			sld.Dialog.Filename = "convoy_traider.c";
			sld.dialog.currentnode = "prepare_convoy_quest";
			sld.greeting = "Gr_ConvoyTrader";
			sld.BakNation = sld.nation;		
		    sld.Abordage.Enable = true;
			sld.CompanionEnemyEnable = true;
			LAi_LoginInCaptureTown(sld, true);
			ChangeCharacterAddressGroup(sld, pchar.location, "reload", "reload1");			
			pchar.quest.generate_convoy_quest_progress = "begin";
			Pchar.GenQuest.Hunter2Pause = true; // ОЗГи на паузу.
			chrDisableReloadToLocation = true;
			LAi_SetActorType(sld);
			LAi_ActorDialog(sld, pchar, "", -1, 0);
		break;
		
		case "convoy_refused":
			pchar.quest.generate_convoy_quest_failed.over = "yes";
			pchar.quest.generate_convoy_quest_completed.over = "yes";
			pchar.quest.generate_convoy_quest_timer.over = "yes";
			if (checkquestattribute("generate_convoy_quest_progress", "completed"))
			{
				OfficersReaction("good");
				AddCharacterExpToSkill(pchar, "Sailing", 100);
				AddCharacterExpToSkill(pchar, "Leadership", 20);
				AddCharacterExpToSkill(pchar, "COMMERCE", 5);
				AddQuestRecord("convoy_quest", "4");
				CloseQuestHeader("convoy_quest");
				AddMoneyToCharacter(pchar, sti(pchar.ConvoyQuest.convoymoney));
				ChangeCharacterReputation(pchar, 1);
				RemoveCharacterCompanion(Pchar, characterFromID("QuestTrader"));
				CloseQuestHeader("convoy_quest");
			}
			pchar.quest.generate_convoy_quest_progress = "";
			GetCharacterPos(pchar, &locx, &locy, &locz);
			LAi_SetActorType(characterFromID("QuestTrader"));
			LAi_ActorGoToLocation(characterFromID("QuestTrader"), "reload", LAi_FindFarLocator("reload", locx, locy, locz), "none", "", "", "", 20.0);
		break;
		
		case "generate_convoy_quest_timer":
            AddQuestRecord("convoy_quest", "2");
			CloseQuestHeader("convoy_quest");            
            sld = characterFromID("QuestTrader");
            ChangeCharacterHunterScore(pchar, NationShortName(sti(sld.BakNation)) + "hunter", 5+rand(10));// награда
			RemoveCharacterCompanion(Pchar, sld);
			OfficersReaction("bad");
			ChangeCharacterReputation(pchar, -10);
			pchar.quest.generate_convoy_quest_progress = "";
			pchar.quest.generate_convoy_quest_failed.over = "yes";
			pchar.quest.generate_convoy_quest_completed.over = "yes";
		break;
		
		case "generate_convoy_quest_failed": // торговец умер
			ChangeCharacterReputation(pchar, -5);
			OfficersReaction("bad");
            RemoveCharacterCompanion(Pchar, characterFromID("QuestTrader"));
			pchar.quest.generate_convoy_quest_progress = "";
			pchar.quest.generate_convoy_quest_failed.over = "yes";
			pchar.quest.generate_convoy_quest_completed.over = "yes";
            pchar.quest.generate_convoy_quest_timer.over  = "yes";            
            AddQuestRecord("convoy_quest", "3");
			CloseQuestHeader("convoy_quest");
		break;
		
		case "generate_convoy_quest_completed":
			sld = characterFromID("QuestTrader");
			if (IsCompanion(sld))
			{
				sld.dialog.currentnode = "complete_convoy_quest";
				PlaceCharacter(sld, "goto", pchar.location);			
				Pchar.GenQuest.Hunter2Pause = true; // ОЗГи на паузу.			
				chrDisableReloadToLocation = true;
				LAi_SetActorType(sld);
				LAi_ActorDialog(sld, pchar, "", -1, 0);
			}
			else
			{
				ChangeCharacterReputation(pchar, -5);
				ChangeCharacterHunterScore(pchar, NationShortName(sti(sld.BakNation)) + "hunter", 10+rand(10));// награда
				OfficersReaction("bad");
				pchar.quest.generate_convoy_quest_progress = "";				
				pchar.quest.generate_convoy_quest_failed.over = "yes";
				pchar.quest.generate_convoy_quest_completed.over = "yes";
				pchar.quest.generate_convoy_quest_timer.over  = "yes";  
				AddQuestRecord("convoy_quest", "5");
				CloseQuestHeader("convoy_quest");
			}
		break;		

		case "convoy_agreeded":
			sld = characterFromID("QuestTrader");
			CharacterIntoCompanionAndGoOut(pchar, sld, "reload", "reload1_back", 3.0, false);
            ReOpenQuestHeader("convoy_quest");
			AddQuestRecord("convoy_quest", "1");
			sTemp = XI_ConvertString("Colony" + pchar.quest.destination + "Gen") + ", что на " + XI_ConvertString(GetIslandByCityName(pchar.quest.destination) + "Dat") + ",";
			AddQuestUserData("convoy_quest", "sCity", sTemp);
			AddQuestUserData("convoy_quest", "sDay", FindRussianDaysString(sti(pchar.ConvoyQuest.iDay)));
            AddQuestUserData("convoy_quest", "sMoney", FindRussianMoneyString(sti(pchar.ConvoyQuest.convoymoney)));
			// по городу вернем его порт
            attrName = GetPortByCityName(pchar.quest.destination);            
			pchar.quest.generate_convoy_quest_completed.win_condition.l1 = "Location";
			pchar.quest.generate_convoy_quest_completed.win_condition.l1.location = attrName;
			pchar.quest.generate_convoy_quest_completed.win_condition = "generate_convoy_quest_completed";			
			if (rand(2) == 1) TraderHunterOnMap();
		break;
 		////////////////////////////////////////////////////////////////////////
		//  Конец    Сопровождение торговца
		////////////////////////////////////////////////////////////////////////

 		////////////////////////////////////////////////////////////////////////
		//  Начало    Доставка пассажира
		////////////////////////////////////////////////////////////////////////
		case "prepare_for_passenger_quest":
			iTemp = sti(colonies[FindColony(sGlobalTemp)].nation);
            Rank = NPC_GenerateCharacter("QuestPassanger", "", "man", "man", 20, iTemp, -1, true);
			sld = &characters[Rank];
			sld.id = sld.id + "_" + sld.index; //меняем ID на оригинальный
			SetCaptanModelByEncType(sld, "trade");
			SetRandomNameToCharacter(sld);
			sld.Dialog.Filename = "convoy_passenger.c";
			sld.dialog.currentnode = "prepare_convoy_quest";
			LAi_LoginInCaptureTown(sld, true);
			ChangeCharacterAddressGroup(sld, pchar.location, "reload", "reload1");			
			Pchar.GenQuest.Hunter2Pause = true; // ОЗГи на паузу.
			chrDisableReloadToLocation = true;
			LAi_SetActorType(sld);
			LAi_ActorDialog(sld, pchar, "", -1, 0);            
		break;

		case "AllPassangersDead": // торговец умер
			sld = &characters[sti(pchar.quest.(qname).Idx)];
			ChangeCharacterReputation(pchar, -5);
            RemovePassenger(PChar, sld);
			sTemp = sld.id + "_complited";
            pchar.quest.(sTemp).over = "yes";
			sTemp = sld.id + "_TimeOver";
			pchar.quest.(sTemp).over = "yes";
			DeleteAttribute(pchar, "GenQuest.ConvoyPassenger." + sld.id); //извлекаем из структуры недовольных
			//--> смотрим Deck
    		makearef(arAll, pchar.GenQuest.ConvoyPassenger);
			if (GetAttributesNum(arAll) == 0) pchar.quest.ConvoyMapPassenger.over = "yes";
			//<-- смотрим Deck
			attrName = sld.index + "convoy_passenger";
			AddQuestRecordEx(attrName, "Gen_convoy_passenger", "3");
            CloseQuestHeader(attrName);
		break;
		
		case "AllPassangersTimeOver":
			sld = &characters[sti(pchar.quest.(qname).Idx)];
			sTemp = sld.id;
			Pchar.GenQuest.ConvoyPassenger.(sTemp) = sld.index; //в структуру недовольных
			if (!CheckAttribute(Pchar, "quest.ConvoyMapPassenger"))
			{
				Pchar.quest.ConvoyMapPassenger.win_condition.l1 = "MapEnter";
        		Pchar.quest.ConvoyMapPassenger.win_condition = "AllPassengerDeck";
			}
		break;
		
		case "AllPassengerDeck": // разговор на палубе			
            MakeCloneShipDeck(pchar, true); // подмена палубы
			makearef(arAll, pchar.GenQuest.ConvoyPassenger);
    		iTemp = GetAttributesNum(arAll);
    		for (i=0; i<iTemp; i++)
    		{
    			arPass = GetAttributeN(arAll, i);
				sld = &characters[sti(arPass)];				
				ChangeCharacterAddress(sld, "Ship_deck", "goto4");
				LAi_SetActorType(sld);
				LAi_ActorDialog(sld, pchar, "", -1, 0); 
    		}
			makearef(arOldMapPos, worldMap.old);
			WdmPrepareMapForAbordage(arOldMapPos);
			DoReloadFromWorldMapToLocation("Ship_deck", "goto", "goto5");
			ReloadFromWMtoL_complete();
            pchar.quest.Munity = "";  // признак выхода с палубы
            LAi_LockFightMode(Pchar, true);
        break;
        
		case "AllPassangersComplited":
			sld = &characters[sti(pchar.quest.(qname).Idx)];
			sTemp = sld.id + "_TimeOver";
            pchar.quest.(sTemp).over = "yes";
			sTemp = sld.id + "_dead";
			pchar.quest.(sTemp).over = "yes";
			Pchar.GenQuest.Hunter2Pause = true; // ОЗГи на паузу.
			chrDisableReloadToLocation = true;
			sld.dialog.currentnode = "complete_convoy_quest";
			PlaceCharacter(sld, "goto", pchar.location);
			LAi_SetActorType(sld);
			LAi_ActorDialog(sld, pchar, "", -1, 0);     
		break;
 		////////////////////////////////////////////////////////////////////////
		//  Конец    Доставка пассажира
		////////////////////////////////////////////////////////////////////////

 		////////////////////////////////////////////////////////////////////////
		//  Начало   Ланд энкаунтеры
		//////////////////////////////////////////////////////////////////////// 
		//------------------- банда грабителей -------------------------
		case "LandEnc_RaidersBegin":
			LAi_SetFightMode(pchar, false);
			LAi_LockFightMode(pchar, true);
			iTemp = sti(pchar.quest.(qname).EncQty);
			sTemp = "Gang"+ pchar.quest.(qname).LocIdx + "_";
			sld = characterFromId(sTemp + "0");
			fTemp = (stf(locations[reload_location_index].locators_radius.encdetector)-3)/5;
			LAi_SetActorTypeNoGroup(sld);
			LAi_ActorDialog(sld, pchar, "", fTemp, 0); 
			for(i = 1; i < iTemp; i++)
			{
				sld = CharacterFromID(sTemp + i);				
				if (sld.id != "none")
				{
					LAi_SetActorTypeNoGroup(sld);
					LAi_ActorFollow(sld, pchar, "", -1);
				}
			}
		break;

		case "LandEnc_RaidersBeforeDialog": //если выстрел до диалога
			attrName = locations[reload_location_index].index;
			sTemp = "EncRaiders_" + attrName;
			pchar.quest.(sTemp).over = "yes";
			Model = "RaidersGroup_" + attrName; //имя группы
			sTemp = "Gang"+ attrName + "_";
			for(i = 0; i < 4; i++)
			{				
				if (GetCharacterIndex(sTemp + i) == -1) continue;
				sld = CharacterFromID(sTemp + i);
				LAi_RemoveCheckMinHP(sld);
				LAi_SetWarriorTypeNoGroup(sld);
				//LAi_group_MoveCharacter(sld, Model);
				LAi_SetImmortal(sld, false);
			}
			LAi_group_SetRelation(Model, LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
			LAi_group_FightGroups(Model, LAI_GROUP_PLAYER, true);
			LAi_group_SetCheck(Model, "LandEnc_RaidersAfrer");
		break;

		case "LandEnc_RaidersAfrer": //чистим за собой
			attrName = locations[reload_location_index].index;
			sTemp = "TimeRaiders_" + attrName;
			pchar.quest.(sTemp).over = "yes";
			sTemp = "RaidersGroup_" + attrName;
			Delay_DeleteGroup(sTemp);
			Gun = "Raiders_" + attrName; 
			AddSimpleRumour(LinkRandPhrase("Вы слышали, капитан " + GetFullName(pchar) + " схватился с бандитом по имени " + pchar.GenQuest.(Gun).name + ". Его хотели ограбить, а он не захотел платить. Смелый кэп - он просто его убил.", 
				"Вы знаете, недавно за городом была схватка - на капитана " + GetMainCharacterNameGen() + " напали бандиты, ограбить хотели... А-а, так вы же и есть тот капитан, кто отправил к праотцам этого мародера по имени " + pchar.GenQuest.(Gun).name + "! Прекрасная работа, скажу я вам...", 
				"Вы не слышали о том, что местная шайка бандитов под предводительством человека по имени " + pchar.GenQuest.(Gun).name + " напала на капитана " + GetMainCharacterNameGen() + "? Тот отказался платить и перебил бандитов."), sti(pchar.GenQuest.(Gun).nation), 5, 1);
			DeleteAttribute(pchar, "GenQuest." + Gun); //трем нацию и имя
		break;

		case "LandEnc_RaidersOver": //чистим за собой
			sTemp = "EncRaiders_" + pchar.quest.(qname).LocIdx;
			iTemp = sti(pchar.quest.(sTemp).EncQty);
			sTemp = "Gang" + pchar.quest.(qname).LocIdx + "_"
			for(i = 0; i < iTemp; i++)
			{
				if (GetCharacterIndex(sTemp + i) == -1) continue;
				LAi_RemoveCheckMinHP(CharacterFromID(sTemp + i));
			}
			sTemp = "RaidersGroup_" + pchar.quest.(qname).LocIdx;
			LAi_group_RemoveCheck(sTemp);
			LAi_group_Delete(sTemp);
			pchar.quest.(qname).over = "yes";
			sTemp = "EncRaiders_" + pchar.quest.(qname).LocIdx;
			pchar.quest.(sTemp).over = "yes";
			sTemp = pchar.quest.(qname).LocIdx;
			DeleteAttribute(pchar, "GenQuest." + sTemp); //трем нацию и имя
		break;
		//------------------- девка в джунлях -------------------------
		case "LandEnc_RapersTalk":
			for(i = 1; i <= 3; i++)
			{
				if (GetCharacterIndex("GangMan_" + i) == -1) continue;
				sld = CharacterFromID("GangMan_" + i);
				LAi_SetActorTypeNoGroup(sld);
				LAi_ActorDialog(sld, pchar, "", -1, 0); 
			}
			LAi_SetFightMode(pchar, false);
			LAi_LockFightMode(pchar, true);
		break;

		case "LandEnc_RapersBeforeDialog": //если выстрел до диалога
			if (GetCharacterIndex("CangGirl") != -1) LAi_RemoveCheckMinHP(CharacterFromID("CangGirl"));			
			for(i = 1; i <= 3; i++)
			{				
				if (GetCharacterIndex("GangMan_" + i) == -1) continue;
				sld = CharacterFromID("GangMan_" + i);
				LAi_RemoveCheckMinHP(sld);
				LAi_SetWarriorType(sld);
				LAi_group_MoveCharacter(sld, "EnemyFight");
			}
			LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
			LAi_group_FightGroups("EnemyFight", LAI_GROUP_PLAYER, true);
			LAi_group_SetCheck("EnemyFight", "LandEnc_RapersAfrer");
			sld = CharacterFromID("CangGirl");			 
			LAi_SetActorTypeNoGroup(sld);
			LAi_ActorAfraid(sld, pchar, true);
		break;

		case "LandEnc_RapersAfrer":
			sGlobalTemp = "Saved_CangGirl";
			sld = CharacterFromID("CangGirl");
			sld.greeting = "Enc_RapersGirl_2";
			sld.dialog.currentnode = "ThanksForHelp";
			LAi_SetActorTypeNoGroup(sld);
			LAi_ActorDialog(sld, pchar, "", -1, 0);
		break;

		case "LandEnc_RapersBadExit":
			for(i = 1; i <= 3; i++)
			{
				if (GetCharacterIndex("GangMan_" + i) == -1) continue;
				sld = CharacterFromID("GangMan_" + i);
				LAi_RemoveCheckMinHP(sld);
				sld.lifeDay = 0;
			}
			if (GetCharacterIndex("CangGirl") == -1)
			{
				ChangeCharacterReputation(pchar, -3);
				AddSimpleRumour(LinkRandPhrase("Вы слышали, какой-то капитан убил в джунглях несчастную девушки по имени " + pchar.GenQuest.EncGirl.name + ". Отловить бы мерзавца и вздернуть!", 
					"Вы знаете, капитан " + GetFullName(pchar) + ", кто-то убил несчастную девушку по имени " + pchar.GenQuest.EncGirl.name  + ", когда та по делам отправилась за город. Как жаль, она была такой хорошей...", 
					"Ее звали " + pchar.GenQuest.EncGirl.name  + ", и она погибла в джунглях. И ничего удивительного, скажу я вам, за городом так неспокойно..."), sti(pchar.GenQuest.EncGirl.nation), 5, 1);			
			}
			else
			{
				characters[GetCharacterIndex("CangGirl")].lifeDay = 0;
				if (sGlobalTemp == "Saved_CangGirl")
				{
					ChangeCharacterReputation(pchar, 5);
					AddSimpleRumour(LinkRandPhrase("Вы слышали, какой-то капитан спас девушку по имени " + pchar.GenQuest.EncGirl.name + " от бандитов, буквально вытащил ее из их лап! Какой прекрасный человек и настоящий мужчина.", 
						"Вы знаете, капитан " + GetFullName(pchar) + " спас девушку по имени " + pchar.GenQuest.EncGirl.name + " от насильников в джунглях... А-а, так это же вы!! Я вами восхищаюсь...", 
						"Девушке по имени " + pchar.GenQuest.EncGirl.name + " здорово повезло - недавно на нее напали бандиты в джунглях, хотели изнасиловать, видать. Но ничего у них не вышло, хе-хе, и благодарить она должна капитана " + GetMainCharacterNameGen() + "!"), sti(pchar.GenQuest.EncGirl.nation), 5, 1);
				}
				else
				{			
					ChangeCharacterReputation(pchar, -8);
					AddSimpleRumour(RandPhraseSimple("Да уж, капитан " + GetFullName(pchar) + ", никто не думал, что вы способны оставить беззащитную женщину на поругание бандитам. Бедная " + pchar.GenQuest.EncGirl.name + ", у меня нет слов...", 
						"Знаете, капитан, вы форменный мерзавец. Как вы могли бросить беззащитную девушку в джунглях? Вы хоть представляете себе, что они с ней сделали? Бедная " + pchar.GenQuest.EncGirl.name + ", ее теперь никто не возьмет замуж..."), sti(pchar.GenQuest.EncGirl.nation), 5, 1);
				}
			}			
			DeleteAttribute(pchar, "GenQuest.EncGirl"); //трем нацию и имя
		break;
		//------------------- девка просит помочь подруге -------------------------
		case "Enc_FriendGirl_after": //если выстрел до диалога
			ChangeCharacterReputation(pchar, -(rand(4)+1));
			if (rand(1))
			{
				AddSimpleRumour(LinkRandPhrase("Вы слышали, какой-то капитан убил в джунглях несчастную девушки по имени " + pchar.GenQuest.EncGirl.name + ". Отловить бы мерзавца и вздернуть!", 
					"Вы знаете, капитан " + GetFullName(pchar) + ", кто-то убил несчастную девушку по имени " + pchar.GenQuest.EncGirl.name  + ", когда та по делам отправилась за город. Как жаль, она была такой хорошей...", 
					"Ее звали " + pchar.GenQuest.EncGirl.name  + ", и она погибла в джунглях. И ничего удивительного, скажу я вам, за городом так неспокойно..."), sti(pchar.GenQuest.EncGirl.nation), 5, 1);
			}
		break;
		//------------------- патруль в джунглях -------------------------
		case "LandEnc_PatrolBegin":
			LAi_SetFightMode(pchar, false);
			LAi_LockFightMode(pchar, true);
			iTemp = sti(pchar.quest.(qname).EncQty);
			sTemp = "Patrol"+ pchar.quest.(qname).LocIdx + "_";
			npchar = characterFromId(sTemp + "0");
			fTemp = (stf(locations[reload_location_index].locators_radius.encdetector)-3)/5;
			LAi_SetActorTypeNoGroup(npchar);
			LAi_ActorDialog(npchar, pchar, "", fTemp, 0); 
			for(i = 1; i < iTemp; i++)
			{
				sld = characterFromId(sTemp + i);
				LAi_SetActorType(sld);
				LAi_ActorFollow(sld, npchar, "", -1); 
			}
		break;

		case "LandEnc_PatrolBeforeDialog": //если выстрел до диалога
			attrName = locations[reload_location_index].index;
			sTemp = "EncPatrol_" + attrName;
			pchar.quest.(sTemp).over = "yes";
			Model = "PatrolGroup_" + attrName; //имя группы
			sTemp = "Patrol"+ attrName + "_";
			for(i = 0; i < 4; i++)
			{				
				if (GetCharacterIndex(sTemp + i) == -1) continue;
				sld = CharacterFromID(sTemp + i);
				LAi_RemoveCheckMinHP(sld);
				LAi_SetWarriorTypeNoGroup(sld);
			}
			LAi_group_SetRelation(Model, LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
			LAi_group_FightGroups(Model, LAI_GROUP_PLAYER, true);
			LAi_group_SetCheck(Model, "LandEnc_PatrolAfrer");
		break;

		case "LandEnc_PatrolAfrer": //чистим за собой
			attrName = locations[reload_location_index].index;
			sTemp = "TimePatrol_" + attrName;
			pchar.quest.(sTemp).over = "yes";
			sTemp = "PatrolGroup_" + attrName;
			Delay_DeleteGroup(sTemp);
			//слухи
			AddSimpleRumour(LinkRandPhrase("Недавно за городом нашли мертвым нашего офицера и приданных ему солдат. Кто мог их убить? Зачем?..", 
				"Наш комендат постоянно отправляет за город патрули, но теперь даже не знаю, как будет дальше. Недавно солдат нашли метрвыми в джунглях...", 
				"Черт возьми, кто-то за городом перебил наши солдат, что патрулировали территорию..."), sti(pchar.GenQuest.(attrName).nation), 5, 1);
			AddSimpleRumour("Говорят, что капитан " + GetFullName(pchar) + " нарвался в джунглях на солдат " + NationNameGenitive(sti(pchar.GenQuest.(attrName).nation)) + " и всех их перебил. Хех, " + NationNamePeople(sti(pchar.GenQuest.(attrName).nation)) + " - никуда негодные вояки...", sti(pchar.GenQuest.(attrName).nation)+10, 5, 1);
			DeleteAttribute(pchar, "GenQuest." + attrName); //трем нацию патруля, если есть
		break;

		case "LandEnc_PatrolOver": //чистим за собой
			sTemp = pchar.quest.(qname).LocIdx;
			//слухи
			AddSimpleRumour(LinkRandPhrase("Вы слышали, капитана " + GetMainCharacterNameGen() + " проверял наш патруль за городом...", 
				"Хм, насколько я знаю, вас недавно проверил наш патруль за городом. Не знаю, насколько эта мера эффективна...", 
				"Наш комендант усилит патрулирование прилегающей к городу территории. Ну вы и сами об этом знаете, вас же недавно проверяли в джунглях..."), sti(pchar.GenQuest.(sTemp).nation), 5, 1);
			DeleteAttribute(pchar, "GenQuest." + sTemp); //трем нацию патруля, если есть
			sTemp = "PatrolGroup_" + pchar.quest.(qname).LocIdx;
			LAi_group_RemoveCheck(sTemp);
			LAi_group_Delete(sTemp);
			pchar.quest.(qname).over = "yes";
			sTemp = "EncPatrol_" + pchar.quest.(qname).LocIdx;
			pchar.quest.(sTemp).over = "yes";
		break;

		case "DeleteGroupOnExitLocation": //группу нулим
			sTemp = pchar.quest.DeleteGroupOnExitLocation.GroupName;
			LAi_group_Delete(sTemp);
		break;

 		////////////////////////////////////////////////////////////////////////
		//  Конец   Ланд энкаунтеры
		//////////////////////////////////////////////////////////////////////// 

 		////////////////////////////////////////////////////////////////////////
		//  Начало    Уничтожение банды
		////////////////////////////////////////////////////////////////////////  
		case "AllMayorsQuests_Late": //провальное прерывание на все квесты мэра
			sTemp = pchar.GenQuest.questName;
			pchar.quest.(sTemp).over = "yes"; //снимаем прерывание на квест
			pchar.GenQuest.(sTemp) = "Late"; //флаг опоздал
			npchar = characterFromId(pchar.GenQuest.(sTemp).MayorId);
			AddQuestRecord("MayorsQuestsList", "20");
			AddQuestUserData("MayorsQuestsList", "ColonyName", XI_ConvertString("Colony"+npchar.city+"Gen"));
			AddQuestUserData("MayorsQuestsList", "MayorName", GetFullName(npchar));			
			//снимаем кораблики пирата и торговцев
			if (sTemp == "DestroyPirate") 
			{
				Group_DeleteGroup("MQGroupPirate");
				LAi_group_Delete("MayorPirateGroup");
				for (i=0; i<5; i++)
				{
					Group_DeleteGroup("MQGroupAll_" + i);
				}
			}
		break;

		case "DestroyGang_fight":
			if (rand(1))
			{
				chrDisableReloadToLocation = true;
				Pchar.GenQuest.Hunter2Pause = true; // ОЗГи на паузу.			
				LAi_group_Delete("EnemyFight");
				sTemp = pchar.GenQuest.questName;
				pchar.quest.(sTemp).over = "yes"; //снимаем текущее многоразовое прерывание					
				ref location = &locations[reload_location_index];
				LAi_LocationFightDisable(location, true);      //лочим возможность начать драку до диалог, это лишнее
				string encGroup = LAi_FindRandomLocator("encdetector");
				sTemp = "locators." + encGroup;
				makearef(arAll, location.(sTemp));
				iTemp = GetAttributesNum(arAll); //кол-во человек в банде
				if (iTemp <= 0 ) iTemp = 1; //если локаторов меньше четырех
				//--> генерим ранг 
				if (sti(pchar.rank) > 6) 
				{
					if (sti(pchar.rank) > 20) Rank = sti(pchar.rank) + sti(MOD_SKILL_ENEMY_RATE*4/iTemp);
					else Rank = sti(pchar.rank) + sti(MOD_SKILL_ENEMY_RATE*2.5/iTemp);
				}
				else  
				{	//казуалам зеленый свет на начало игры
					if (sti(pchar.rank) > 3) Rank = sti(pchar.rank);
					else 
					{
						Rank = 1;
						iTemp = 1;
					}
				}	
				//<-- генерим ранг 
				string iModel[10];
				iModel[0] = "pirate_1";
				iModel[1] = "pirate_2";
				iModel[2] = "pirate_3";
				iModel[3] = "pirate_4";
				iModel[4] = "pirate_5";
				iModel[5] = "pirate_6";
				iModel[6] = "pirate_7";
				iModel[7] = "pirate_8";
				iModel[8] = "pirate_9";
				iModel[9] = "pirate_10";
				i = 0;
				while(i < iTemp)
				{
					ShipType = rand(9);
					if (iModel[ShipType] != "")
					{
						if (i == 0) 
						{   //главарь банды
							int iMainGang = NPC_GenerateCharacter("MayorQuestGang_" + i, iModel[ShipType], "man", "man", Rank, PIRATE, 0, true);
							sld = &characters[iMainGang];
							sld.name = pchar.GenQuest.DestroyGang.name;  //имя главаря
							sld.lastname = pchar.GenQuest.DestroyGang.lastname;
							sld.dialog.filename = "MayorQuests_dialog.c";
							sld.dialog.currentnode = "DestroyGang_begin";
							sld.greeting = "Enc_Raiders"; 
							sld.SaveItemsForDead   = true; // сохранять на трупе вещи
							sld.DontClearDead = true;  // не убирать труп через 200с
						}
						else sld = GetCharacter(NPC_GenerateCharacter("MayorQuestGang_" + i, iModel[ShipType], "man", "man", Rank, PIRATE, 0, true));
						LAi_SetActorType(sld);
						SetFantomParamFromRank(sld, Rank, true);
						//Получим локатор для логина
						attrName = GetAttributeName(GetAttributeN(arAll, i));
						ChangeCharacterAddressGroup(sld, location.id, encGroup, attrName);					
						LAi_SetActorType(sld);
						if (i == 0) LAi_ActorDialog(sld, pchar, "", -1, 0); 
						else LAi_ActorFollow(sld, &characters[iMainGang], "", -1);
						i++;
						iModel[ShipType] = "";
					}
				} 				
			}
		break;

		case "DestroyGang_SuddenAttack":
			for(i = 0; i < 4; i++)
			{
				if (GetCharacterIndex("MayorQuestGang_" + i) == -1) continue;
				sld = CharacterFromID("MayorQuestGang_" + i);
				LAi_SetWarriorType(sld);
				LAi_group_MoveCharacter(sld, "EnemyFight");
				LAi_SetImmortal(sld, false);
			}
			LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
			LAi_group_FightGroups("EnemyFight", LAI_GROUP_PLAYER, true);
			LAi_group_SetCheck("EnemyFight", "DestroyGang_Afrer");
		break;

		case "DestroyGang_Afrer":
			pchar.quest.AllMayorsQuests_Late.over = "yes"; //снимаем общий таймер
			Delay_DeleteGroup("EnemyFight");
			makearef(arAll, pchar.GenQuest.DestroyGang);
			pchar.GenQuest.DestroyGang = "Execute"; //флаг выполнил успешно
			sTemp = GetFullName(&characters[GetCharacterIndex(arAll.MayorId)]);
			// судовой журнал
			AddQuestRecord("MayorsQuestsList", "2");
			AddQuestUserData("MayorsQuestsList", "ColonyName", XI_ConvertString("Colony"+characters[GetCharacterIndex(pchar.GenQuest.DestroyGang.MayorId)].city+"Gen"));
			AddQuestUserData("MayorsQuestsList", "MayorName", sTemp);
			AddQuestUserData("MayorsQuestsList", "GangName", GetFullName(arAll));
			// слухи
			AddSimpleRumour(LinkRandPhrase("Вы знаете, губернатор " + sTemp + " поручил некоему капитану " + GetMainCharacterNameDat() + " уничтожить известного бандита по имени " + GetFullName(arAll) + ". И, кажется, что капитан выполнил задание губернатора...", 
				"Какой-то смелый капитан вызвался найти и уничтожить местного бандита по имени " + GetFullName(arAll) + ". И, похоже, он сделал это!", 
				"Ха, наконец-то наш губернатор взялся за наведение порядка в джунглях - он поручил капитану " + GetMainCharacterNameDat() + " разыскать в джунглях и убить наиболее известного бандита. И вы знате, этот бандит, " + GetFullName(arAll) + " - мертв!"), sti(characters[GetCharacterIndex(arAll.MayorId)].nation), 5, 1);
		break;
 		////////////////////////////////////////////////////////////////////////
		//  Конец    Уничтожение банды
		////////////////////////////////////////////////////////////////////////

 		////////////////////////////////////////////////////////////////////////
		//  Начало    Поиск лазутчика в городе
		//////////////////////////////////////////////////////////////////////// 
        case "SeekSpy_login":
			if (CheckAttribute(pchar, "GenQuest.SeekSpy.City"))
			{	//шпион на улице
				sTemp = pchar.GenQuest.questName;
				pchar.quest.(sTemp).over = "yes"; //снимаем текущее многоразовое прерывание	
				sld = GetCharacter(NPC_GenerateCharacter("MayorQuestSpy", "citiz_"+(rand(11)+1), "man", "man", Rank, PIRATE, sti(pchar.GenQuest.SeekSpy.Terms), false));
				sld.dialog.filename = "MayorQuests_dialog.c";
				sld.dialog.currentnode = "SeekSpy_street";
				sld.city = characters[GetCharacterIndex(pchar.GenQuest.SeekSpy.MayorId)].city;
				sld.greeting = "cit_common";
				LAi_SetLoginTime(sld, 6.0, 21.99);
				//SetFantomParamFromRank(sld, Rank, true);
				LAi_SetCitizenType(sld);
				iTemp = sti(characters[GetCharacterIndex(pchar.GenQuest.SeekSpy.MayorId)].nation);
				sTemp = GetNationNameByType(iTemp) + "_citizens";
				LAi_group_MoveCharacter(sld, sTemp);
				PlaceCharacter(sld, "goto", "random");
			}
			else
			{	//шпион в коммонах
				iTemp = reload_location_index;
				if (locations[iTemp].fastreload == characters[GetCharacterIndex(pchar.GenQuest.SeekSpy.MayorId)].city)
				{
					chrDisableReloadToLocation = true;  
					LAi_group_Delete("EnemyFight");
					sTemp = pchar.GenQuest.questName;
					pchar.quest.(sTemp).over = "yes"; //снимаем текущее прерывание	
					pchar.quest.AllMayorsQuests_Late.over = "yes"; //снимаем общий таймер
					//--> генерим ранг 
					if (sti(pchar.rank) > 6) 
					{
						if (sti(pchar.rank) > 20) Rank = sti(pchar.rank) + MOD_SKILL_ENEMY_RATE + 5);
						else Rank = sti(pchar.rank) + MOD_SKILL_ENEMY_RATE);
					}
					else  
					{	//казуалам зеленый свет на начало игры
						if (sti(pchar.rank) > 3) Rank = sti(pchar.rank);
						else 
						{
							Rank = 1;
							if (iTemp > 2) iTemp = 2;
						}
					}	
					//<-- генерим ранг 
					sld = GetCharacter(NPC_GenerateCharacter("MayorQuestSpy", "citiz_"+(rand(11)+1), "man", "man", Rank, PIRATE, 0, true));
					sld.dialog.filename = "MayorQuests_dialog.c";
					sld.dialog.currentnode = "SeekSpy_house";
					sld.greeting = "cit_common"; 
					SetFantomParamFromRank(sld, Rank, true);
					PlaceCharacter(sld, "goto", "random");
					LAi_SetCitizenType(sld);
					//LAi_ActorDialog(sld, pchar, "", -1, 0); 
				}
			}
        break;

		case "SeekSpy_Afrer":
			LAi_group_Delete("EnemyFight");
			pchar.GenQuest.SeekSpy = "Execute"; //флаг выполнил успешно
			sTemp = GetFullName(&characters[GetCharacterIndex(pchar.GenQuest.SeekSpy.MayorId)]);
			// судовой журнал
			AddQuestRecord("MayorsQuestsList", "4");
			AddQuestUserData("MayorsQuestsList", "ColonyName", XI_ConvertString("Colony"+characters[GetCharacterIndex(pchar.GenQuest.SeekSpy.MayorId)].city+"Gen"));
			AddQuestUserData("MayorsQuestsList", "MayorName", sTemp);
			// слухи
			AddSimpleRumour(RandPhraseSimple("Вы знаете, губернатор " + sTemp + " поручил некоему капитану " + GetMainCharacterNameDat() + " найти в городе вражеского лазутчика. И вы знаете, он его нашел и убил!", 
				"Наш губернатор зал задание капитану " + GetMainCharacterNameDat() + " разыскать в городе вражеского агента. И этот капитан нашел и прикончил лазутчика! Прекрасная работа, скажу я вам..."), sti(characters[sti(pchar.GenQuest.SeekSpy.MayorId)].nation), 5, 1);
		break;
 		////////////////////////////////////////////////////////////////////////
		//  Конец    Поиск лазутчика в городе
		//////////////////////////////////////////////////////////////////////// 

 		////////////////////////////////////////////////////////////////////////
		//  Начало    Уничтожение контры
		//////////////////////////////////////////////////////////////////////// 
        case "KillSmugglers_after":
            if (CheckAttribute(pchar, "GenQuest.KillSmugglers") && pchar.GenQuest.KillSmugglers.Areal == GiveArealByLocation(&locations[reload_location_index]))
            {
				pchar.GenQuest.KillSmugglers = "Execute"; //флаг выполнил успешно
				pchar.quest.AllMayorsQuests_Late.over = "yes"; //снимаем общий таймер
				ChangeContrabandRelation(pchar, -30); //репу контры вниз
				Pchar.quest.Contraband.over = "yes";
				CloseQuestHeader("Gen_Contraband");
				pchar.GenQuest.KillSmugglers.shoreId = pchar.location; //бухту запомним, чисто понты :)
				sTemp = GetFullName(&characters[GetCharacterIndex(pchar.GenQuest.KillSmugglers.MayorId)]);
				ReOpenQuestHeader("MayorsQuestsList");
				AddQuestRecord("MayorsQuestsList", "9");
				AddQuestUserData("MayorsQuestsList", "ColonyName", XI_ConvertString("Colony"+characters[GetCharacterIndex(pchar.GenQuest.KillSmugglers.MayorId)].city+"Gen"));
				AddQuestUserData("MayorsQuestsList", "MayorName", sTemp));
				// слухи
				AddSimpleRumour(RandPhraseSimple("Вы знаете, губернатор " + sTemp + " поручил некоему капитану " + GetMainCharacterNameDat() + " найти и уничтожить контрабандистов. И скажу я вам, он это сделал.",
					"Наш губернатор " + sTemp + " дал задание капитану " + GetMainCharacterNameDat() + " разыскать контрабандистов в окрестностях города. Он нашел их и убил, значит цены у торговцев скоро подскочат..."), sti(characters[sti(pchar.GenQuest.KillSmugglers.MayorId)].nation), 5, 1);
			}
		break;

        case "KillSmugglers_outShore":
			pchar.quest.KillSmugglers.over = "yes"; //снимаем текущее прерывание
		break;
 		////////////////////////////////////////////////////////////////////////
		//  Конец    Уничтожение контры
		//////////////////////////////////////////////////////////////////////// 

 		////////////////////////////////////////////////////////////////////////
		//  Начало    Поиск и уничтожение пирата
		//////////////////////////////////////////////////////////////////////// 
 		case "DestroyPirate_Begin":
			npchar = characterFromId(pchar.GenQuest.DestroyPirate.MayorId);
			int islandIdx = FindIsland(pchar.GenQuest.DestroyPirate.Areal);	
			aref arQuestLoc, arPlTarget, arLocator;	
			makearef(arPlTarget, Islands[islandIdx].reload);
			string locatorName, captainId, GroupName;
			//--> генерим ранг 
			if (sti(pchar.rank) > 6) 
			{
				if (sti(pchar.rank) > 20) Rank = 15;
				else Rank = 10;
			}
			else
			{
				Rank = sti(pchar.rank);
			}	
			//<-- генерим ранг 
			//определяем локаторы для размещения торговцев
			makearef(arQuestLoc, Islands[islandIdx].Quest_ships);
			iTemp = GetAttributesNum(arQuestLoc)-1; 
			int n = 0;
			int iRand = rand(4);			
			for (i=iTemp; n<5; i--)
			{
				arLocator = GetAttributeN(arQuestLoc, i);
				locatorName = GetAttributeName(arLocator);
				sTemp = "l" + n;
				if (findsubstr(locatorName, "Quest" , 0) != -1)
				{
					if (iRand == n)
					{
						//==> ставим пирата
						Group_AddCharacter("MQGroupPirate", "MQPirate");			
						Group_SetType("MQGroupPirate", "pirate");
						Group_SetGroupCommander("MQGroupPirate", "MQPirate");
						Group_LockTask("MQGroupPirate");
						Group_SetAddress("MQGroupPirate", pchar.GenQuest.DestroyPirate.Areal, "Quest_ships", locatorName);						
						int iShore = GetAttributesNum(arPlTarget)-1;
						arLocator = GetAttributeN(arPlTarget, iShore);
						locatorName = GetAttributeName(arLocator);						
						Group_SetTaskMove("MQGroupPirate", stf(arPlTarget.(locatorName).ships.(sTemp).x), stf(arPlTarget.(locatorName).ships.(sTemp).z))
					}
					else
					{
						//ставим торговцев
						captainId = "MQCaptain_" + n;
						GroupName = "MQGroupAll_" + n;
						sld = GetCharacter(NPC_GenerateCharacter(captainId, "", "man", "man", Rank, sti(npchar.nation), -1, true));					
						SetCaptanModelByEncType(sld, "trade");
						DeleteAttribute(sld, "Ship");
						SetShipToFantom(sld, "trade", true);
						sld.Ship.Mode = "Trade";
						sld.AlwaysSandbankManeuver = true;
						Group_AddCharacter(GroupName, captainId);
						Group_SetGroupCommander(GroupName, captainId);
						Group_SetAddress(GroupName, pchar.GenQuest.DestroyPirate.Areal, "Quest_ships", locatorName);					
						Group_SetTaskMove(GroupName, stf(arPlTarget.l1.ships.(sTemp).x), stf(arPlTarget.l1.ships.(sTemp).z));
					}
					n++;
				}
			}
			//сформируем строку с типом корабля, пригодится
			sld = characterFromId("MQPirate"); 
			iTemp = RealShips[sti(sld.ship.type)].basetype; 
			pchar.GenQuest.DestroyPirate.ShipType = GetShipSexWord(ShipsTypes[iTemp].name, "пиратский ", "пиратскую ") + XI_ConvertString(ShipsTypes[iTemp].name+"Gen");
			//прерывание на смерть пирата
			pchar.quest.DestroyPirate.win_condition.l1 = "NPC_Death";
			pchar.quest.DestroyPirate.win_condition.l1.character = "MQPirate";
			pchar.quest.DestroyPirate.win_condition = "DestroyPirate_after";
			//таймер
			SetTimerCondition("DestroyPirate_Late", 0, 0, sti(pchar.GenQuest.DestroyPirate.Terms), false);
			//судовой журнал
			ReOpenQuestHeader("MayorsQuestsList");
			AddQuestRecord("MayorsQuestsList", "10");
			AddQuestUserData("MayorsQuestsList", "ColonyName", XI_ConvertString("Colony"+npchar.city+"Gen"));
			AddQuestUserData("MayorsQuestsList", "MayorName", GetFullName(npchar));
			AddQuestUserData("MayorsQuestsList", "sDay", FindRussianDaysString(sti(pchar.GenQuest.DestroyPirate.Terms)));
			AddQuestUserData("MayorsQuestsList", "sMoney", FindRussianMoneyString(sti(pchar.GenQuest.DestroyPirate.Money)));			
		break;
		
		case "DestroyPirate_WithoutFight":	
			pchar.quest.Munity = "Deads";  // откроем выход
			sld = characterFromId("MQPirate");
			sld.money = pchar.money;
			pchar.money = 0;
			Ship_SetTaskRunaway(SECONDARY_TASK, sti(sld.index), 1); //сваливаем
		break;

		case "DestroyPirate_FightInDeck":
			GetCharacterPos(pchar, &locx, &locy, &locz);
			LAi_LocationFightDisable(loadedLocation, false);
			LAi_SetFightMode(pchar, true);
			Rank = 5 + sti(MOD_SKILL_ENEMY_RATE);
			LAi_group_Delete("EnemyFight");
			for (i=1; i<Rank; i++)
			{				
				iTemp = GetCharacterIndex("saylor_0"+i);
				if ( iTemp == -1) 
				{
					iTemp = NPC_GenerateCharacter("saylor_0" + i, "pirate_"+(rand(9)+1) , "man", "man", 10, PIRATE, 0, true);
					ChangeCharacterAddressGroup(&characters[iTemp], pchar.location, "goto", LAi_FindFarLocator("goto", locx, locy, locz));
				}
				LAi_group_MoveCharacter(&characters[iTemp], "EnemyFight");
			}
            LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
            LAi_group_FightGroups("EnemyFight", LAI_GROUP_PLAYER, true);
            LAi_group_SetCheck("EnemyFight", "DestroyPirate_TeamIsDead");
		break;

		case "DestroyPirate_TeamIsDead":
      		Pchar.quest.DestroyPirate_PirateIsOut.win_condition.l1 = "ExitFromSea";
			Pchar.quest.DestroyPirate_PirateIsOut.win_condition = "DestroyPirate_PirateIsOut";			
			LAi_SetFightMode(pchar, false);
			LAi_LocationFightDisable(location, true);
			DoQuestCheckDelay("TalkSelf_Quest", 0.4); //диалог сам-на-сам
		break;

		case "DestroyPirate_PirateIsOut":
			pchar.quest.DestroyPirate_Late.over = "yes"; //снимаем таймер
			pchar.GenQuest.DestroyPirate = "PirateIsOut"; //флаг пират свалил
			npchar = characterFromId(pchar.GenQuest.DestroyPirate.MayorId);
			LAi_group_Delete("MayorPirateGroup");
			Group_DeleteGroup("MQGroupPirate");
			for (i=0; i<5; i++)
			{
				Group_DeleteGroup("MQGroupAll_" + i);
			}
			// слухи
			AddSimpleRumour(LinkRandPhrase("Вы слышали о недавнем бое капитана " + GetMainCharacterNameGen() + " с пиратами? Говорят, что пираты сумели ускользнуть от него...", 
				"Капитан, как вы могли упустить этих пиратов?! М-да, очень и очень жаль...", 
				"Эх, капитан " + GetFullName(pchar) + ", как же это вы упустили этих пиратов, а? Ищи теперь ветра в поле..."), sti(npchar.nation), 5, 1);
		break;

		case "DestroyPirate_after":
			pchar.quest.DestroyPirate_PirateIsOut.over = "yes"; //снимаем прерывание на ушел с поля боя
			pchar.quest.DestroyPirate_Late.over = "yes"; //снимаем таймер
			LAi_group_Delete("MayorPirateGroup");
			Group_DeleteGroup("MQGroupPirate");
			pchar.GenQuest.DestroyPirate = "Execute"; //флаг выполнил успешно
			npchar = characterFromId(pchar.GenQuest.DestroyPirate.MayorId);
			// судовой журнал
			AddQuestRecord("MayorsQuestsList", "11");
			AddQuestUserData("MayorsQuestsList", "ColonyName", XI_ConvertString("Colony"+npchar.city+"Gen"));
			AddQuestUserData("MayorsQuestsList", "MayorName", GetFullName(npchar));
			AddQuestUserData("MayorsQuestsList", "ShipType", GetStrSmallRegister(pchar.GenQuest.DestroyPirate.ShipType));
			AddQuestUserData("MayorsQuestsList", "sDay", FindRussianDaysString(sti(pchar.GenQuest.DestroyPirate.Terms)));
			AddQuestUserData("MayorsQuestsList", "sMoney", FindRussianMoneyString(sti(pchar.GenQuest.DestroyPirate.Money)));			
			// слухи
			AddSimpleRumour(LinkRandPhrase("Вы знаете, губернатор " + GetFullName(npchar) + " поручил некоему капитану " + GetMainCharacterNameDat() + " найти и уничтожить пирата, который занимался грабежом в окрестных водах. Так вот, кэп нашел и потопил " + GetStrSmallRegister(pchar.GenQuest.DestroyPirate.ShipType) + "!", 
				"Какой-то смелый капитан вызвался найти и уничтожить пирата, грабящего наших торговцев. И, похоже, он сделал это - потопил " + GetStrSmallRegister(pchar.GenQuest.DestroyPirate.ShipType) + "!", 
				"Ха, наконец-то наш губернатор взялся за обеспечение безопасного плавания в водах " + XI_ConvertString("Colony"+npchar.city+"Gen") + " - он поручил капитану " + GetMainCharacterNameDat() + " разыскать пирата, не дававшего покоя нашим торговцам. И вы знаете, этот капитан потопил " + GetStrSmallRegister(pchar.GenQuest.DestroyPirate.ShipType) + "!"), sti(npchar.nation), 5, 1);			
		break;

		case "DestroyPirate_Late":
			if (bSeaActive)
			{
      			Pchar.quest.AllMayorsQuests_Late.win_condition.l1 = "ExitFromSea";
				Pchar.quest.AllMayorsQuests_Late.win_condition = "AllMayorsQuests_Late";
			}
			else
			{
				DoQuestCheckDelay("AllMayorsQuests_Late", 0.1);
			}
		break;

		case "DestroyPirate_toShore":
			sTemp = pchar.GenQuest.questName;
			pchar.quest.DestroyPirate_Late.over = "yes"; //снимаем таймер
			pchar.GenQuest.(sTemp) = "Found";
			npchar = characterFromId(pchar.GenQuest.(sTemp).MayorId);
			int lngFileID = LanguageOpenFile("LocLables.txt");
			attrName = LanguageConvertString(lngFileID, pchar.GenQuest.DestroyPirate.outShoreId);
			//судовой журнал
			AddQuestRecord("MayorsQuestsList", "12");
			AddQuestUserData("MayorsQuestsList", "Shore", attrName);			
			// слухи
			AddSimpleRumour(LinkRandPhrase("Вы знаете, губернатор " + GetFullName(npchar) + " поручил некоему капитану " + GetMainCharacterNameDat() + " найти и уничтожить пирата, что грабит торговцев у нас под носом. И что вы думаете, он ограбил и капитана " + GetMainCharacterNameGen() + " тоже!", 
				"Капитан, как вам путешествие на пиратском корабле? Нормально? Вас там не обижали? Ха-ха-ха!!", 
				"О-о-о, капитан, я слышал вам здорово досталось недавно, хи-хи... Как вам местные пираты? Не хотите еще их поискать? Ха-ха-ха!!"), sti(npchar.nation), 5, 1);
			//снимаем кораблики пирата и торговцев
			LAi_group_Delete("MayorPirateGroup");
			Group_DeleteGroup("MQGroupPirate");
			for (i=0; i<5; i++)
			{
				Group_DeleteGroup("MQGroupAll_" + i);
			}
			pchar.money = 0;
			pchar.quest.Munity = "Deads";
			pchar.location.from_sea = npchar.city + "_town";
			//--> ВОТ ЗДЕСЬ МЕТОД НА РЕЛОАД ИЗ ПАЛУБЫ В БУХТУ
			DoReloadFromDeckToLocation(pchar.GenQuest.DestroyPirate.outShoreId, "reload", "sea");
			//<-- ВОТ ЗДЕСЬ МЕТОД НА РЕЛОАД ИЗ ПАЛУБЫ В БУХТУ
		break;
 		////////////////////////////////////////////////////////////////////////
		//  Конец    Поиск и уничтожение пирата
		//////////////////////////////////////////////////////////////////////// 

  		////////////////////////////////////////////////////////////////////////
		//  Начало    Грабители в городах
		//////////////////////////////////////////////////////////////////////// 		
		case "Berglars_Ostin": 
			chrDisableReloadToLocation = true;
			sld = GetCharacter(NPC_GenerateCharacter("Ostin", "Ostin", "man", "man", 30, SPAIN, -1, false));
			FantomMakeCoolFighter(sld, 30, 100, 70, "blade32", "pistol5", 50);
			sld.name 	= "Альберто";
			sld.lastname = "Гаодио";
			sld.rank = 30;
			sld.dialog.filename   = "Quest\Berglars.c";
			sld.dialog.currentnode = "Final_fight";
			sld.greeting = "pirat_quest";
			sld.money = 110670;
			sld.SaveItemsForDead  = true; 
			sld.DontClearDead = true;
			SetCharacterPerk(sld, "Energaiser");
			SetCharacterPerk(sld, "SwordplayProfessional");
			SetCharacterPerk(sld, "AdvancedDefense");
			SetCharacterPerk(sld, "CriticalHit");
			SetCharacterPerk(sld, "Sliding");
			ChangeCharacterAddressGroup(sld, pchar.location, "goto", "goto1");
			LAi_SetActorType(sld);
			LAi_ActorDialog(sld, pchar, "", -1, 0);
        break;
  		////////////////////////////////////////////////////////////////////////
		//  Конец    Грабители в городах
		//////////////////////////////////////////////////////////////////////// 

   		////////////////////////////////////////////////////////////////////////
		//  Начало   Бордель
		//////////////////////////////////////////////////////////////////////// 
        case "Brothel_checkVisitTime":
			LocatorReloadEnterDisable(qname + "_Brothel", "reload2_back", true); //закрываем комнату
			sld = characterFromId(pchar.quest.(qname).HorseId);
			sld.location = qname+"_Brothel";
			sld.location.group = "goto";
			sld.location.locator = pchar.quest.(qname).locator;
			//ChangeCharacterAddressGroup(sld, qname+"_Brothel", "goto", pchar.quest.(qname).locator);
			DeleteAttribute(&characters[GetCharacterIndex(qname + "_Hostess")], "quest.selected"); //снимаем флаг у хозяйки
			//==> ноду девкам
			for(i=0; i<MAX_CHARACTERS; i++)
			{
				makeref(sld, Characters[i]);
				if (sld.location == qname+"_Brothel" && CheckAttribute(sld, "CityType") && sld.CityType == "horse")
				{
					sld.dialog.currentnode = "Horse_talk";
				}
			}
        break;	
		////////////////////////////////////////////////////////////////////////
		//  Конец    Бордель
		//////////////////////////////////////////////////////////////////////// 

		////////////////////////////////////////////////////////////////////////
		//  Начало    Грабеж жемчужного промысла
		//////////////////////////////////////////////////////////////////////// 
        //--------------------- промысел через залив Сан-Хуан дель Норте и подземелье -----------------------
		case "PearlGen_checkSantaCatalina":
			if (locations[reload_cur_location_index].id == "Pearl_CaveEntrance")
			{
				if (!CheckAttribute(&locations[reload_location_index], "PearlSantaCatalina") || GetNpcQuestPastDayParam(&locations[reload_location_index], "PearlSantaCatalina") > 30)
				{				
					LAi_group_Delete("PearlGroup_2");
					SaveCurrentNpcQuestDateParam(&locations[reload_location_index], "PearlSantaCatalina");
					sld = GetCharacter(NPC_GenerateCharacter("PearlGuardMan_2", "pirate_"+(rand(9)+1), "man", "man", 10, SPAIN, 31, true));
					ChangeCharacterAddressGroup(sld, pchar.location, "goto", "goto"+RandFromThreeDight(3, 4, 8));
					sld.city = "SantaCatalina"; //испанские НЗГ
					LAi_SetActorType(sld);
					LAi_group_MoveCharacter(sld, "PearlGroup_2");
					locations[FindLocation("Pearl_town_2")].pearlVillage = true; //флаг генерить жемчужников
					bOk = true;	//мир
					if (GetRelation2BaseNation(SPAIN) != RELATION_FRIEND)
					{	
						if (GetDaysContinueNationLicence(SPAIN) == -1)
						{
							Log_SetStringToLog("Часового необходимо уничтожить!");
							LAi_ActorRunToLocation(sld, "reload", "reload1_back", "none", "", "", "", -1);					
							bOk = false; //война
							
						}
					}
					if (bOk)
					{
						chrDisableReloadToLocation = true;
						sld.dialog.Filename = "Pearl_dialog.c";
						sld.dialog.currentnode = "GuardMan";
						LAi_ActorDialog(sld, pchar, "", -1, 0);
					}
				}
			}
			else //обратный путь
			{
				if (locations[reload_cur_location_index].id == "Pearl_town_2" && GetCharacterIndex("HeadMan_2") == -1)
				{
					iTemp = 0;
					for(i=0; i<MAX_CHARACTERS; i++)
					{
						if (characters[i].location == "Pearl_town_2")
						{
							characters[i].location = "none";
							iTemp++;
						}
					}
					if (iTemp > 0) //остатки жемчуга скоммуниздили оставшиеся в живых жемчужники
					{
						DeleteAttribute(&locations[FindLocation("PearlTown2_Townhall")], "box1");
						DeleteAttribute(&locations[FindLocation("PearlTown2_Townhall")], "box2");
					}
				}
			}
			pchar.quest.PearlGen_SetNewInterrapt_2.win_condition.l1 = "ExitFromLocation";
			pchar.quest.PearlGen_SetNewInterrapt_2.win_condition.l1.location = "Pearl_Jungle_06";
			pchar.quest.PearlGen_SetNewInterrapt_2.function = "PearlGen_SetSantaCatalina";
		break;

		//----------------------------------- промысел через Теночтитлан --------------------------------------
        case "PearlGen_checkTeno":
			if (locations[reload_cur_location_index].id == "Pearl_Jungle_03")
			{
				if (!CheckAttribute(&locations[reload_location_index], "PearlTeno") || GetNpcQuestPastDayParam(&locations[reload_location_index], "PearlTeno") > 30)
				{				
					LAi_group_Delete("PearlGroup_1");
					SaveCurrentNpcQuestDateParam(&locations[reload_location_index], "PearlTeno");
					sld = GetCharacter(NPC_GenerateCharacter("PearlGuardMan_1", "pirate_"+(rand(9)+1), "man", "man", 10, SPAIN, 31, true));
					ChangeCharacterAddressGroup(sld, pchar.location, "goto", "goto"+RandFromThreeDight(1, 9, 8));
					sld.city = "SantaCatalina"; //испанские НЗГ
					LAi_SetActorType(sld);
					LAi_group_MoveCharacter(sld, "PearlGroup_1");
					locations[FindLocation("Pearl_town_1")].pearlVillage = true; //флаг генерить жемчужников
					bOk = true;	//мир
					if (GetRelation2BaseNation(SPAIN) != RELATION_FRIEND)
					{	
						if (GetDaysContinueNationLicence(SPAIN) == -1)
						{
							Log_SetStringToLog("Часового необходимо уничтожить!");
							LAi_ActorRunToLocation(sld, "reload", "reload2_back", "none", "", "", "", -1);					
							bOk = false; //война					
						}
					}
					if (bOk)
					{
						chrDisableReloadToLocation = true;
						sld.dialog.Filename = "Pearl_dialog.c";
						sld.dialog.currentnode = "GuardMan";
						LAi_ActorDialog(sld, pchar, "", -1, 0);
					}
				}
			}
			else //обратный путь
			{
				if (locations[reload_cur_location_index].id == "Pearl_town_1" && GetCharacterIndex("HeadMan_1") == -1)
				{
					iTemp = 0;
					for(i=0; i<MAX_CHARACTERS; i++)
					{
						if (characters[i].location == "Pearl_town_1")
						{
							characters[i].location = "none";
							iTemp++;
						}
					}
					if (iTemp > 0) //остатки жемчуга скоммуниздили оставшиеся в живых жемчужники
					{
						DeleteAttribute(&locations[FindLocation("PearlTown1_Townhall")], "box1");
						DeleteAttribute(&locations[FindLocation("PearlTown1_Townhall")], "box2");
					}
				}
			}
			pchar.quest.PearlGen_SetNewInterrapt_1.win_condition.l1 = "ExitFromLocation";
			pchar.quest.PearlGen_SetNewInterrapt_1.win_condition.l1.location = "Pearl_Jungle_02";
			pchar.quest.PearlGen_SetNewInterrapt_1.function = "PearlGen_SetTeno";
        break;

        case "PearlToShore_ForSale":
			chrDisableReloadToLocation = false;
			LAi_SetCitizenTypeNoGroup(&characters[sti(pchar.quest.PearlToShore_ForSale.idx)]);
        break;

        case "Pearl_GuardManIsLife":
			sld = &characters[sti(pchar.quest.Pearl_GuardManIsLife.idx)];
			sld.dialog.Filename = "Pearl_dialog.c";
 			sld.dialog.currentnode = "GuardManAllarm";
			ChangeCharacterAddressGroup(sld, pchar.location, "goto", "goto10");
			LAi_SetActorTypeNoGroup(sld);
			LAi_ActorDialog(sld, pchar, "", 1.5, 0);			
        break;

		////////////////////////////////////////////////////////////////////////
		//  Конец    Грабеж жемчужного промысла
		//////////////////////////////////////////////////////////////////////// 

		////////////////////////////////////////////////////////////////////////
		//  Начало Тюремные квесты
		//////////////////////////////////////////////////////////////////////// 
        case "GivePrisonFree":
			chrDisableReloadToLocation = true;
			LAi_LocationDisableMonstersGen(pchar.location, true);
			pchar.quest.GivePrisonFree_Death.over = "yes";
			sld = characterFromId(pchar.questTemp.jailCanMove.prisonerId);
			sld.dialog.currentnode = "PrisonerInPlace";
			LAi_SetActorTypeNoGroup(sld);
			GetCharacterPos(sld, &locx, &locy, &locz);	
			ChangeCharacterAddressGroup(sld, pchar.location, "monsters", LAi_FindFarLocator("monsters", locx, locy, locz));
			LAi_ActorDialog(sld, pchar, "", -1, 0);		
		break;

        case "GivePrisonFree_Death":
			AddQuestRecord("GivePrisonFree", "2");
            AddQuestUserData("GivePrisonFree", "sName", pchar.questTemp.jailCanMove.Name);
			CloseQuestHeader("GivePrisonFree");
			pchar.quest.GivePrisonFree.over = "yes";
			DeleteAttribute(pchar, "questTemp.jailCanMove.islandId");	
			DeleteAttribute(pchar, "questTemp.jailCanMove.placeId");
			DeleteAttribute(pchar, "questTemp.jailCanMove.prisonerId");
			DeleteAttribute(pchar, "questTemp.jailCanMove.IsTrue");
			DeleteAttribute(pchar, "questTemp.jailCanMove.Sum");
			DeleteAttribute(pchar, "questTemp.jailCanMove.Name");
            Pchar.quest.DeletePrisonGroup.win_condition.l1 = "ExitFromLocation";
            Pchar.quest.DeletePrisonGroup.win_condition.l1.location = pchar.location;
            Pchar.quest.DeletePrisonGroup.win_condition = "DeletePrisonGroup"
		break;

        case "GivePrisonFree_Over":
			AddQuestRecord("GivePrisonFree", "6");
            AddQuestUserData("GivePrisonFree", "sName", pchar.questTemp.jailCanMove.Name);
			CloseQuestHeader("GivePrisonFree");
			iTemp = GetCharacterIndex(pchar.questTemp.jailCanMove.prisonerId);
			if (iTemp > 0) 
			{
				characters[iTemp].lifeDay = 0;
				ChangeCharacterAddress(&characters[iTemp], "none", "");	
			}
			DeleteAttribute(pchar, "questTemp.jailCanMove.islandId");	
			DeleteAttribute(pchar, "questTemp.jailCanMove.placeId");
			DeleteAttribute(pchar, "questTemp.jailCanMove.prisonerId");
			DeleteAttribute(pchar, "questTemp.jailCanMove.IsTrue");
			DeleteAttribute(pchar, "questTemp.jailCanMove.Sum");
			DeleteAttribute(pchar, "questTemp.jailCanMove.Name");
			DeleteAttribute(pchar, "questTemp.jailCanMove.ownerPrison");
			DeleteAttribute(pchar, "questTemp.jailCanMove.City");
            Pchar.quest.DeletePrisonGroup.win_condition.l1 = "ExitFromLocation";
            Pchar.quest.DeletePrisonGroup.win_condition.l1.location = pchar.location;
            Pchar.quest.DeletePrisonGroup.win_condition = "DeletePrisonGroup"
		break;

        case "DeliverToBander":
			if (CheckAttribute(pchar, "questTemp.jailCanMove.Deliver.City") && pchar.questTemp.jailCanMove.Deliver.City == locations[reload_location_index].fastreload)
			{
				LAi_group_Delete("EnemyFight");
				chrDisableReloadToLocation = true;
				pchar.quest.DeliverToBander.over = "yes";
				pchar.quest.DeliverToBander_over.over = "yes";
				//--> генерим ранг 
				if (sti(pchar.rank) > 6) 
				{
					Rank = sti(pchar.rank) + MOD_SKILL_ENEMY_RATE;
				}
				else  
				{	//казуалам зеленый свет на начало игры
					if (sti(pchar.rank) > 3) Rank = sti(pchar.rank);
					else 
					{
						Rank = 1;
					}
				}	
				//<-- генерим ранг
				GetCharacterPos(pchar, &locx, &locy, &locz);
				for (i=1; i<=2; i++)
				{
					sld = GetCharacter(NPC_GenerateCharacter("Bander_"+i, "pirate_"+(rand(9)+1), "man", "man", Rank, PIRATE, 0, true));
					sld.dialog.filename = "quest\Banders_dialog.c";
					sld.dialog.currentnode = "First time";
					sld.greeting = "cit_common"; 
					SetFantomParamFromRank(sld, Rank, true);
					if (i == 1)
					{
						LAi_SetOwnerType(sld);
						sld.dialog.currentnode = "Second time";
         				ChangeCharacterAddressGroup(sld, pchar.location, "goto", LAi_FindFarLocator("goto", locx, locy, locz));
					}
					else
					{
						LAi_SetActorType(sld);
						sld.dialog.currentnode = "First time";
         				ChangeCharacterAddressGroup(sld, pchar.location, "goto", LAi_FindNearestLocator("goto", locx, locy, locz));
					}
					LAi_group_MoveCharacter(sld, "EnemyFight");
				}
				LAi_ActorDialog(sld, pchar, "", 1.0, 0); 
			}
		break;

        case "DeliverToBander_over":
			AddQuestRecord("GivePrisonFree", "9");
			CloseQuestHeader("GivePrisonFree");
			pchar.quest.DeliverToBander.over = "yes";
			iTemp = GetCharacterIndex(pchar.questTemp.jailCanMove.Deliver.Id);
			if (iTemp > 0) ChangeCharacterAddress(&characters[iTemp], "none", "");
			DeleteAttribute(pchar, "questTemp.jailCanMove.Deliver");
			TakeItemFromCharacter(pchar, "Malyava");
		break;

        case "DeliverToBander_Dead":
			AddQuestRecord("GivePrisonFree", "10");
			AddQuestUserData("GivePrisonFree", "sName", pchar.questTemp.jailCanMove.Deliver.name);
			CloseQuestHeader("GivePrisonFree");
            chrDisableReloadToLocation = false;
			iTemp = GetCharacterIndex(pchar.questTemp.jailCanMove.Deliver.Id);
			if (iTemp > 0) ChangeCharacterAddress(&characters[iTemp], "none", "");
			DeleteAttribute(pchar, "questTemp.jailCanMove.Deliver");
			TakeItemFromCharacter(pchar, "Malyava");
		break;

        case "DeletePrisonGroup":
			LAi_group_Delete("DeliveQuest_Prison");
		break;
		////////////////////////////////////////////////////////////////////////
		//  Конец  Тюремные квесты
		//////////////////////////////////////////////////////////////////////// 

		// универсальный диалогостартер (см метод  SetActorDialogAny2Pchar)
        case "ActorDialog_Any2Pchar":
            LAi_SetActorType(PChar);
            LAi_ActorWaitDialog(Pchar, characterFromID(Pchar.AutoQuest.NPCharId));
            LAi_ActorDialog(characterFromID(Pchar.AutoQuest.NPCharId), Pchar, Pchar.AutoQuest.Quest, stf(Pchar.AutoQuest.Time1), stf(Pchar.AutoQuest.Time2));
        break;

        case "OpenTheDoors":
            chrDisableReloadToLocation = false;
			bDisableFastReload = false; 
        break;

        case "CanEnterToMap":
            bQuestDisableMapEnter = false;
        break;

        case "CloseBermudesDungeonDoor":
            LocatorReloadEnterDisable("Pirates_Shipyard", "reload2", true);
			sld = characterFromId("Pirates_shipyarder");
			DeleteAttribute(sld, "Step_Door");
        break;

        case "CanFightCurLocation":
			LAi_LocationFightDisable(loadedLocation, false);
			chrDisableReloadToLocation = false;
        break;

        case "CannotFightCurLocation":
			LAi_LocationFightDisable(loadedLocation, true);
			chrDisableReloadToLocation = false;
        break;

        case "MainHeroFightModeOn":
			LAi_SetFightMode(pchar, true);
        break;

        case "MainHeroFightModeOff":
			LAi_SetFightMode(pchar, false);
			LAi_LockFightMode(pchar, true);
        break;

		case "ChurchNightGuard_timerInChurch_2":
			TavernWaitDate("wait_day");
		break;

/////////////////////////////////////////////////////////////////////////////////////////////////////////
////  START Английская линейка
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//==========================   Квест №1, Сопровождение герцога Альбермаля ========================

        case "fight_withSpanish_Albermal":                      //  Атака испанца на наш корабль с Альбермалем
            bQuestDisableMapEnter = true;
            Island_SetReloadEnableGlobal("Antigua", false);

            sld = GetCharacter(NPC_GenerateCharacter("CaptainAttack_1", "off_spa_2", "man", "man", 15, SPAIN, 0, true));
            FantomMakeCoolSailor(sld, SHIP_FLEUT, "", CANNON_TYPE_CULVERINE_LBS24, 90, 90, 90);
            FantomMakeCoolFighter(sld, 20, 90, 50, "blade25", "pistol3", 80);
            sld.DontRansackCaptain = true; //квестовые не сдаются
            sld.SinkTenPercent = true; //уходим с повреждениями
            
			Group_FindOrCreateGroup("Spa_Attack");
			Group_SetType("Spa_Attack", "war");
			Group_AddCharacter("Spa_Attack", "CaptainAttack_1");
			Group_SetGroupCommander("Spa_Attack", "CaptainAttack_1");
			Group_SetPursuitGroup("Spa_Attack", PLAYER_GROUP);
			Group_SetTaskAttack("Spa_Attack", PLAYER_GROUP);
			Group_SetAddress("Spa_Attack", "Antigua", "", "");
			Group_LockTask("Spa_Attack");

			pchar.quest.after_fight_withSpanish_Albermal.win_condition.l1 = "NPC_Death";
			pchar.quest.after_fight_withSpanish_Albermal.win_condition.l1.character = "CaptainAttack_1";
			pchar.quest.after_fight_withSpanish_Albermal.win_condition = "after_fight_withSpanish_Albermal";
        break;

        case "after_fight_withSpanish_Albermal":                  // Испанский флейт на дне или захвачен
            bQuestDisableMapEnter = false;
            Island_SetReloadEnableGlobal("Antigua", true);
			pchar.quest.goto_Oxbay_Albermal.win_condition.l1 = "location";
			pchar.quest.goto_Oxbay_Albermal.win_condition.l1.location = "SentJons_townhall";
			pchar.quest.goto_Oxbay_Albermal.win_condition = "goto_SentJons_Albermal";
            AddQuestRecord("Eng_Line_1_Albermal", "3");
            group_DeleteGroup("Spa_Attack");
        break;

        case "goto_SentJons_Albermal":
            sld = characterFromID("Albermal");                    //  Альбермаль в резиденции губернатора
            ChangeCharacterAddressGroup(sld, "SentJons_townhall", "goto", "goto5");
			sld.dialog.currentnode = "in_the_SentJons";
            LAi_SetActorType(pchar);
            LAi_SetActorType(sld);
            SetActorDialogAny2Pchar("Albermal", "", 0.0, 1.0);
		    LAi_ActorFollow(pchar, sld, "ActorDialog_Any2Pchar", -1);
        break;

//==========================   Квест №3, Нападение на Белиз ========================
        case "time_over_Beliz_attack":
            // 50 дней на взятие Белиза, включая и походы за двумя пиратами, если геймер решит, что это ему нужно
            if (IsEntity(worldMap))
            {
                if (!bQuestCheckProcessFreeze) // можно показать
				{
	        		pchar.quest.time_over_Beliz_attack_Map_01.over = "yes"; // сброс, если уже откладывали
	        		pchar.quest.time_over_Beliz_attack_Map_02.over = "yes";
                   
                    makearef(arOldMapPos, worldMap.old);
                    WdmPrepareMapForAbordage(arOldMapPos);
                    MakeCloneShipDeck(pchar, true); // подмена палубы
                    DoReloadFromWorldMapToLocation("Ship_deck", "goto", "goto5");
	                ReloadFromWMtoL_complete();
	                
                    sld = characterFromID("Henry Morgan");
                    ChangeCharacterAddressGroup(sld, "Ship_deck", "goto", "goto6");
                    LAi_SetWarriorType(sld);
	                LAi_warrior_DialogEnable(sld, true);
	                sld.dialog.currentnode = "time_over_Beliz";
   	                // ==> антураж
                    for (i=1; i<5; i++)
        			{
                        attrName = "pirate_" + i;
                        sld = GetCharacter(NPC_GenerateCharacter(attrName, attrName, "man", "man", 15, PIRATE, 1, true));
                        sld.dialog.Filename = "Quest\sailors_dialog.c";
	                    sld.dialog.currentnode = "Morgan_wait_you";
                    	LAi_SetWarriorType(sld);
                    	LAi_warrior_DialogEnable(sld, true);
                    	LAi_SetLoginTime(sld, 0.0, 24.0);
                    	LAi_SetHP(sld, 100.0, 100.0);
                    	ChangeCharacterAddressGroup(sld, "Ship_deck", "goto", "goto"+i);
                    }
                	pchar.quest.Munity = "";  // признак выхода с палубы (закрыто, а то можно не говорить)
            		LAi_LockFightMode(pchar, true);
				}
				else
				{   // отложим до разморозки (пролет табличек)
      				Pchar.quest.time_over_Beliz_attack_Map_01.win_condition.l1 = "MapEnter";
                    Pchar.quest.time_over_Beliz_attack_Map_01.win_condition = "time_over_Beliz_attack";
		        }
			}
			else
			{   // подождем выхода на карту, раз время прошло, но мы не на ней
			    Pchar.quest.time_over_Beliz_attack_Map_02.win_condition.l1 = "MapEnter";
                Pchar.quest.time_over_Beliz_attack_Map_02.win_condition = "time_over_Beliz_attack";
			}
        break;

//==========================   Квест №4, Разведмиссия в Кюрасао ========================
        case "Intelligence_Curacao_GoToShore":    //  Встреча с бригом близ Кюрасао.
            if (pchar.questTemp.State == "Intelligence_Curacao_FightGard" || pchar.questTemp.State == "Intelligence_Curacao_OutPrison")
            {
                LAi_LocationFightDisable(&Locations[FindLocation("Villemstad_Bank")], true); // вертаем запрет на боевки
                sld = characterFromID("Villemstad_usurer");
                ChangeCharacterAddressGroup(sld, "Villemstad_Bank", "goto", "goto1");
	            LAi_SetBarmanType(sld);
                LAi_SetHP(sld, 80, 80); //вертаем ростовщика если убит
                pchar.quest.Intelligence_Curacao_OutTown.over = "yes";
            }
            else    //==> боевка с голландским бригом
            {
                bQuestDisableMapEnter = true;
                Island_SetReloadEnableGlobal("Curacao", false);

				pchar.questTemp.abordage = 0; // запрещаем абордаж повторно - это сброс запрета

                sld = GetCharacter(NPC_GenerateCharacter("CaptainAttack_1", "off_hol_2", "man", "man", 23, HOLLAND, 0, true));
                FantomMakeCoolSailor(sld, SHIP_BRIG, "", CANNON_TYPE_CANNON_LBS32, 95, 95, 95);
                FantomMakeCoolFighter(sld, 25, 90, 50, BLADE_LONG, "pistol3", 100);
                sld.DontRansackCaptain = true; //квестовые не сдаются
                sld.SinkTenPercent = true; // уходим с повреждениями
                
    			Group_FindOrCreateGroup("Hol_Attack");
    			Group_SetType("Hol_Attack", "war");
    			Group_AddCharacter("Hol_Attack", "CaptainAttack_1");
    			Group_SetGroupCommander("Hol_Attack", "CaptainAttack_1");
                Group_SetTaskAttack("Hol_Attack", PLAYER_GROUP);
			    Group_SetPursuitGroup("Hol_Attack", PLAYER_GROUP);
			    Group_SetAddress("Hol_Attack", "Curacao", "", "");
    			Group_LockTask("Hol_Attack");
    			//==> признак нахождения письма в каюте кэпа
    			pchar.questTemp.EngLineQ4_PutLetter = 1;

    			pchar.quest.Intelligence_Curacao_Boarding.win_condition.l1 = "Character_Capture";
    			pchar.quest.Intelligence_Curacao_Boarding.win_condition.l1.character = "CaptainAttack_1";
    			pchar.quest.Intelligence_Curacao_Boarding.win_condition = "Intelligence_Curacao_Boarding";

    			pchar.quest.Intelligence_Curacao_Sink.win_condition.l1 = "Character_sink";
    			pchar.quest.Intelligence_Curacao_Sink.win_condition.l1.character = "CaptainAttack_1";
    			pchar.quest.Intelligence_Curacao_Sink.win_condition = "Intelligence_Curacao_Sink";
            }
        break;

        case "Intelligence_Curacao_Sink":   // Голландский бриг потоплен
            bQuestDisableMapEnter = false;
            Island_SetReloadEnableGlobal("Curacao", true);
            pchar.questTemp.State = "Intelligence_Curacao_Sink";
            AddQuestRecord("Eng_Line_4_Intelligence_Curacao", "8");
            group_DeleteGroup("Hol_Attack");
            DeleteAttribute(PChar, "questTemp.EngLineQ4_PutLetter");
            BackItemDescribe("letter_1");
            pchar.quest.Intelligence_Curacao_Boarding.over = "yes";
        break;

        case "Intelligence_Curacao_Boarding":   // Голландский брин захвачен
            pchar.quest.Intelligence_Curacao_Sink.over = "yes";
			if (CheckCharacterItem(pchar, "letter_1"))
            {
                AddQuestRecord("Eng_Line_4_Intelligence_Curacao", "9");
                pchar.questTemp.State = "Intelligence_Curacao_Boarding";
                TakeItemFromCharacter(pchar, "letter_1");
                ChangeItemDescribe("letter_open", "itmdescr_letter_open_EngLineQ4");
                GiveItem2Character(pchar, "letter_open");
            }
            else
            {
                AddQuestRecord("Eng_Line_4_Intelligence_Curacao", "11");
                pchar.questTemp.State = "Intelligence_Curacao_BoardingNotFoundLetter";
            }

            bQuestDisableMapEnter = false;
            Island_SetReloadEnableGlobal("Curacao", true);
            pchar.quest.Intelligence_Curacao_Sink.over = "yes";
            group_DeleteGroup("Hol_Attack");
            DeleteAttribute(PChar, "questTemp.EngLineQ4_PutLetter");
            BackItemDescribe("letter_1");
        break;

		case "Intelligence_Curacao_KickYozef":  // ГГ отпинали в доме у ростовщика.
            LAi_SetFightMode(pchar, false);
            DoReloadCharacterToLocation("Villemstad_prison",  "goto", "goto9");
			LAi_SetImmortal(pchar, false);
            // ==> Забираем клинки, пистоли и деньги.
			RemoveCharacterEquip(pchar, BLADE_ITEM_TYPE);
			RemoveCharacterEquip(pchar, GUN_ITEM_TYPE);
            while (FindCharacterItemByGroup(pchar, BLADE_ITEM_TYPE) != "")
            {
                TakeItemFromCharacter(pchar, FindCharacterItemByGroup(pchar, BLADE_ITEM_TYPE));
            }
            while (FindCharacterItemByGroup(pchar, GUN_ITEM_TYPE) != "")
            {             
                TakeItemFromCharacter(pchar, FindCharacterItemByGroup(pchar, GUN_ITEM_TYPE));
            }
            pchar.money = 0;
            // <== Забираем клинки, пистоли и деньги.
			TakeNationLicence(HOLLAND);
            DoQuestCheckDelay("Intelligence_Curacao_GoPrisonMan", 10);
            sld = GetCharacter(NPC_GenerateCharacter("Hoverd_Tantum", "citiz_11", "man", "man", 15, ENGLAND, 3, false));
            sld.Dialog.Filename = "Quest\EngLineNpc_1.c";
        	sld.name 	= "Говерд";
        	sld.lastname = "Тантум";
            LAi_SetStayTypeNoGroup(sld);
            ChangeCharacterAddressGroup(sld, "Villemstad_prison", "goto", "goto13");
        break;

        case "Intelligence_Curacao_GoPrisonMan":
            sld = characterFromID("Hoverd_Tantum");
            LAi_SetActorType(sld);
            LAi_ActorGoToLocator(sld, "goto", "goto23", "Intelligence_Curacao_TalkingPrisonMan", -1);
        break;

        case "Intelligence_Curacao_TalkingPrisonMan":
            sld = characterFromID("Hoverd_Tantum");
            LAi_SetActorType(sld);
			LAi_ActorDialog(sld, pchar, "", 0, 0);
        break;

//==========================   Квест №5, Нападение на Форт Оранж ========================
        // boal нужно переделать на генераторо захвата с сущи -->
		case "AttackFortOrange_GoToFort":
    	    Pchar.GenQuestFort.fortCharacterIdx = GetCharIDXForTownAttack(pchar.location);
    	    DeleteQuestAttribute("Union_with_Escadra");
            sld = GetCharacter(sti(Pchar.GenQuestFort.fortCharacterIdx));
            SetLocationCapturedState(sld.city + "_town", true);
            DoQuestCheckDelay("Capture_Forts", 0.5);
            Ship_NationAgressive(sld, sld);
            Log_SetStringToLog("На штурм, орлы!");
            // установка отмены боевки в резиденции при захвате города
            //string _city, string _method, bool _majorOff
            SetCaptureResidenceQuest("FortOrange", "", true); // ФО, нет метода, губернатора в сад
            
            Pchar.quest.AttackFortOrange_FightWithCommendant.win_condition.l1 = "location";
            Pchar.quest.AttackFortOrange_FightWithCommendant.win_condition.l1.location = "FortOrange_townhall";
            Pchar.quest.AttackFortOrange_FightWithCommendant.win_condition = "AttackFortOrange_FightWithCommendant";
        break;

        case "AttackFortOrange_FightWithCommendant":
            chrDisableReloadToLocation = true; // закрыть выход из локации
            bDisableFastReload = true; // закрыть переходы.
			//--> огонь и пламень
			locations[FindLocation("FortOrange_town")].hidden_effects = true;
			locations[FindLocation("FortOrange_ExitTown")].hidden_effects = true;		
			//<-- огонь и пламень

            sld = GetCharacter(NPC_GenerateCharacter("Orange_Comendant", "off_hol_2", "man", "man", 25, HOLLAND, 0, true));
            sld.Dialog.Filename = "Quest\EngLineNpc_1.c";
        	LAi_SetWarriorType(sld);
            LAi_warrior_DialogEnable(sld, true);
            sld.SaveItemsForDead = true; // сохранять на трупе вещи
            sld.DontClearDead = true; // не убирать труп через 200с
            LAi_LoginInCaptureTown(sld, true);
            FantomMakeCoolFighter(sld, 38, 90, 70, BLADE_LONG, "pistol3", 30);
            Statistic_AddValue(Pchar, "hol_GrabbingTown", 1); // статистика
           	ChangeCharacterAddressGroup(sld, "FortOrange_townhall", "goto", "goto1");
            LAi_SetActorType(pchar);
            LAi_SetActorType(sld);
            SetActorDialogAny2Pchar(sld.id, "", 0.0, 0.0);
		    LAi_ActorFollow(pchar, sld, "ActorDialog_Any2Pchar", 1.2);
        break;

        case "AttackFortOrange_AfterFightWithCommendant":
            AfterTownBattle();
            chrDisableReloadToLocation = false; 
            bDisableFastReload = false; 
            pchar.questTemp.State = "AttackFortOrange_WeWin";
            AddQuestRecord("Eng_Line_5_AttackFortOrange", "2");
        break;
        
//==========================   Квест №6, Моррис Уильямс ========================
        case "MorrisWillams_ShipToPort":
            Log_QuestInfo("На рейде Порт Рояля установлен фрегат Морриса Уильямса.");
            sld = GetCharacter(NPC_GenerateCharacter("Morris_Willams", "MorrisWillams", "man", "man", 25, ENGLAND, -1, true));
            sld.Dialog.Filename = "Quest\EngLineNpc_1.c";
        	sld.name 	= "Моррис";
        	sld.lastname = "Уильямс";
        	sld.Abordage.Enable = false; // НЕЛЬЗЯ!
            LAi_SetStayTypeNoGroup(sld);
            FantomMakeCoolSailor(sld, SHIP_FRIGATE, "Фаворит", CANNON_TYPE_CULVERINE_LBS24, 90, 90, 90);
            FantomMakeCoolFighter(sld, 28, 70, 40, BLADE_LONG, "pistol4", 100);
            sld.Ship.Stopped = true;
            sld.DeckDialogNode = "First time";
			sld.Ship.Mode = "Pirate";
            
			Group_FindOrCreateGroup("Willams_Group");
			Group_AddCharacter("Willams_Group", "Morris_Willams");
			Group_SetGroupCommander("Willams_Group", "Morris_Willams");
            Group_SetAddress("Willams_Group", "Jamaica", "quest_ships", "quest_ship_1");

			pchar.quest.MorrisWillams_WillamsAttack.win_condition.l1 = "NPC_Death";
			pchar.quest.MorrisWillams_WillamsAttack.win_condition.l1.character = "Morris_Willams";
			pchar.quest.MorrisWillams_WillamsAttack.win_condition = "MorrisWillams_WillamsAttack";
        break;

        case "MorrisWillams_WillamsAttack":
            sld = characterFromID("Morris_Willams");
    	    sld.LifeDay = 1; // уберем нпс на след.день.
    	    SaveCurrentNpcQuestDateParam(sld, "LifeTimeCreate");
            pchar.questTemp.State = "QuestLineBreake";
            ChangeCharacterReputation(pchar, -20);
            CloseQuestHeader("Eng_Line_6_MorrisWillams");
        break;

        case "MorrisWillams_MeetWithManFromWillams":
			chrDisableReloadToLocation = true;
            Log_QuestInfo("Первая встреча с боцманом Уильямса.");
            pchar.quest.MorrisWillams_WillamsAttack.over = "yes"; // снимаем прерывание на завершение линейки

            sld = GetCharacter(NPC_GenerateCharacter("Bocman_Willams", "officer_15", "man", "man", 25, ENGLAND, -1, true));
            sld.Dialog.Filename = "Quest\EngLineNpc_2.c";
        	sld.name 	= "Ален";
        	sld.lastname = "Смит";
            GetCharacterPos(pchar, &locx, &locy, &locz);
         	ChangeCharacterAddressGroup(sld, "PortRoyal_town", "goto", LAi_FindNearestFreeLocator("goto", locx, locy, locz));
            LAi_SetActorType(sld);
			LAi_ActorDialog(sld, pchar, "", -1, 0);
        break;

        case "MorrisWillams_InTavernWithWillams":
            sld = characterFromID("Morris_Willams");            
            FreeSitLocator("PortRoyal_tavern", "sit11");
			LAi_SetSitTypeNoGroup(sld);
         	ChangeCharacterAddressGroup(sld, "PortRoyal_tavern", "sit", "sit11");
            LocatorReloadEnterDisable("PortRoyal_town", "gate_back", true); //на всякий случай, чтобы геймер не слинял.
            LocatorReloadEnterDisable("PortRoyal_town", "reload1_back", true);
        break;

        case "MorrisWillams_CheckTimeForForward":
            if (GetQuestPastTimeParam("questTemp") < 2)
            {
                Log_QuestInfo("Успел выйти в море за два часа игрового времени. Пинас должен быть на месте.");
                Pchar.quest.MW_AttackPinnace.win_condition.l1 = "location";
                Pchar.quest.MW_AttackPinnace.win_condition.l1.location = "PuertoRico";
                Pchar.quest.MW_AttackPinnace.win_condition = "MW_AttackPinnace";
                SetTimerCondition("MorrisWillams_PinaceTimeOver", 0, 0, 16, false);
            }
            else
            {
                Log_QuestInfo("Не успел выйти в море за два часа игрового времени. Пинаса не будет.");
                pchar.questTemp.State = "MorrisWillams_PinnaceNotFound";
                characters[GetCharacterIndex("Morris_Willams")].dialog.currentnode = "First time";
            }
        break;

        case "MorrisWillams_PinaceTimeOver":
            pchar.quest.MW_AttackPinnace.over = "yes";
            Log_QuestInfo("Пинас не появится, не успел за 16 дней.");
            pchar.questTemp.State = "MorrisWillams_PinnaceNotFound";
            characters[GetCharacterIndex("Morris_Willams")].dialog.currentnode = "First time";
        break;

        case "MW_AttackPinnace":
            Log_QuestInfo("Пинас установлен между фортом и кораблями охранения.");
            Island_SetReloadEnableGlobal("PuertoRico", false);
            characters[GetCharacterIndex("Morris_Willams")].dialog.currentnode = "First time";

            sld = GetCharacter(NPC_GenerateCharacter("CaptainAttack_1", "off_spa_1", "man", "man", 28, SPAIN, 0, true));
            sld.SinkTenPercent = true; // уходим с повреждениями
            sld.DontRansackCaptain = true; //квестовые не сдаются

            FantomMakeCoolSailor(sld, SHIP_PINNACE, "Санта Агуэда", CANNON_TYPE_CANNON_LBS32, 95, 90, 90);
            FantomMakeCoolFighter(sld, 28, 90, 70, BLADE_LONG, "pistol3", 100);
			Group_FindOrCreateGroup("Spa_Attack");
			Group_SetType("Spa_Attack", "trade");
			Group_AddCharacter("Spa_Attack", "CaptainAttack_1");
			Group_SetPursuitGroup("Spa_Attack", PLAYER_GROUP);
			Group_SetGroupCommander("Spa_Attack", "CaptainAttack_1");
            Group_SetAddress("Spa_Attack", "PuertoRico", "", "");
            Group_SetTaskAttack("Spa_Attack", PLAYER_GROUP);
//            Group_SetTaskMove("Spa_Attack", 1930.0, -315.0);
            Group_LockTask("Spa_Attack");

            //==> признак нахождения письма в комоде
            pchar.questTemp.EngLineQ6_PutMoney = 1;
            pchar.questTemp.QtyMoneyTillBoarding = pchar.money;  // запомнить кол-во денег до абордажа, нужно для определения взял ли ГГ бабки из сундука в каюте

			pchar.quest.MorrisWillams_PinnaceBoarding.win_condition.l1 = "Character_Capture";
			pchar.quest.MorrisWillams_PinnaceBoarding.win_condition.l1.character = "CaptainAttack_1";
			pchar.quest.MorrisWillams_PinnaceBoarding.win_condition = "MorrisWillams_PinnaceBoarding";

			pchar.quest.MorrisWillams_PinnaceSink.win_condition.l1 = "Character_sink";
			pchar.quest.MorrisWillams_PinnaceSink.win_condition.l1.character = "CaptainAttack_1";
			pchar.quest.MorrisWillams_PinnaceSink.win_condition = "MorrisWillams_PinnaceSink";

			pchar.quest.MorrisWillams_DieHard.win_condition.l1 = "MapEnter";
            pchar.quest.MorrisWillams_DieHard.win_condition = "MorrisWillams_DieHard";
            pchar.quest.MorrisWillams_PinaceTimeOver.over = "yes";  // снимаем таймер на прибытие пинаса к острову
        break;


        case "MorrisWillams_PinnaceBoarding":
			iTemp = sti(pchar.money) - sti(pchar.questTemp.QtyMoneyTillBoarding);
            if (iTemp >= 140000)
            {
                Log_QuestInfo("Пинас взят на абордаж, деньги найдены.");
                AddQuestRecord("Eng_Line_6_MorrisWillams", "10");
                pchar.questTemp.State = "MorrisWillams_PinnaceBoard";
            }
            else
            {
                Log_QuestInfo("Пинас взят на абордаж, деньги не найдены. Миссия провалена.");
                AddQuestRecord("Eng_Line_6_MorrisWillams", "23");
                pchar.questTemp.State = "MorrisWillams_PinnaceBoardNotFoundMoney";
            }
            Island_SetReloadEnableGlobal("PuertoRico", true);
            group_DeleteGroup("Spa_Attack");
            Pchar.quest.MorrisWillams_DieHard.over = "yes";
            pchar.quest.MorrisWillams_PinnaceSink.over = "yes";
            DeleteAttribute(PChar, "questTemp.EngLineQ6_PutMoney");
            DeleteAttribute(PChar, "questTemp.QtyMoneyTillBoarding");
        break;

        case "MorrisWillams_PinnaceSink":
            Log_QuestInfo("Пинас потоплен.");
            Island_SetReloadEnableGlobal("PuertoRico", true);
            Pchar.quest.MorrisWillams_DieHard.over = "yes";
            pchar.quest.MorrisWillams_PinnaceBoarding.over = "yes";
            pchar.questTemp.State = "MorrisWillams_PinnaceSink";
            AddQuestRecord("Eng_Line_6_MorrisWillams", "11");
            group_DeleteGroup("Spa_Attack");
            DeleteAttribute(PChar, "questTemp.EngLineQ6_PutMoney");
            DeleteAttribute(PChar, "questTemp.QtyMoneyTillBoarding");
        break;

        case "MorrisWillams_DieHard":
            Log_QuestInfo("Не сумел взять пинас. Пинаса больше нет.");
            Island_SetReloadEnableGlobal("PuertoRico", true);
            pchar.quest.MorrisWillams_PinnaceBoarding.over = "yes";
            pchar.quest.MorrisWillams_PinnaceSink.over = "yes";
            pchar.questTemp.State = "MorrisWillams_DieHard";
            AddQuestRecord("Eng_Line_6_MorrisWillams", "12");
            group_DeleteGroup("Spa_Attack");
            DeleteAttribute(PChar, "questTemp.EngLineQ6_PutMoney");
            DeleteAttribute(PChar, "questTemp.QtyMoneyTillBoarding");
        break;

        case "MorrisWillams_Arrest":
            Log_QuestInfo("Вторая встреча с боцманом Уильямса.");
			chrDisableReloadToLocation = true;
            sld = characterFromID("Bocman_Willams");
            GetCharacterPos(pchar, &locx, &locy, &locz);
         	ChangeCharacterAddressGroup(sld, "PortRoyal_town", "goto", LAi_FindNearestFreeLocator("goto", locx, locy, locz));
            LAi_SetActorType(sld);
			LAi_ActorDialog(sld, pchar, "", -1, 0);
        break;

        case "MorrisWillams_HusbandInCave":
            Log_QuestInfo("Диалог и сразу боевка. Снова диалог с Эттербери и на выход.");
            chrDisableReloadToLocation = true; // закрыть выход из локации.
            for (i=1; i<=3; i++)
            {
                sld = GetCharacter(NPC_GenerateCharacter("Bandit"+i, "pirate_"+i, "man", "man", 20, PIRATE, 0, true));
                FantomMakeCoolFighter(sld, 20, 70, 60, BLADE_LONG, "pistol2", 50);
            	LAi_SetWarriorType(sld);
            	sld.Dialog.Filename = "Quest\EngLineNpc_2.c";
                LAi_group_MoveCharacter(sld, "EnemyFight");
               	ChangeCharacterAddressGroup(sld, "Jamaica_Grot", "monsters",  "monster"+i);
            }
            LAi_SetActorType(pchar);
            LAi_SetActorType(sld);
            LAi_ActorTurnToCharacter(sld, pchar);
            SetActorDialogAny2Pchar(sld.id, "", 0.0, 0.0);
		    LAi_ActorFollow(pchar, sld, "ActorDialog_Any2Pchar", 1.5);

            npchar = GetCharacter(NPC_GenerateCharacter("Jillians_Husband", "citiz_3", "man", "man", 10, ENGLAND, 0, false));
            npchar.Dialog.Filename = "Quest\EngLineNpc_2.c";
        	npchar.name 	= "Том";
        	npchar.lastname = "Эттербери";
            ChangeCharacterAddressGroup(npchar, "Jamaica_Grot", "monsters", "monster4");
            LAi_SetActorType(npchar);
            LAi_SetImmortal(npchar, true);
            LAi_ActorWaitDialog(npchar, pchar);
        break;

        case "MorrisWillams_AftreFightingInCave":
            pchar.questTemp.State = "MorrisWillams_AfterFight";
            LAi_SetActorType(pchar);
            sld = characterFromID("Jillians_Husband");
            LAi_ActorTurnToCharacter(sld, pchar);
            SetActorDialogAny2Pchar(sld.id, "", 0.0, 0.0);
		    LAi_ActorFollow(pchar, sld, "ActorDialog_Any2Pchar", -1);
		    chrDisableReloadToLocation = false; // открыть выход из локации.
        break;

        case "MorrisWillams_SeekCirassa":
			Pchar.GenQuest.Hunter2Pause = true; // ОЗГи на паузу.
			LAi_LocationDisableOffGenTimer("Shore_ship2", 1); //офицеров не пускать 1 день
            for (i=1; i<=7; i++)
            {
                sld = GetCharacter(NPC_GenerateCharacter("Skelet"+i, "Skel"+(rand(3)+1), "man", "man", 25, PIRATE, 0, true));
                if (i == 2)
                {
                	sld.name 	= "Ламберт";
                	sld.lastname = "Грофф";
                    sld.SaveItemsForDead = true; // сохранять на трупе вещи
                    sld.DontClearDead = true;
                    FantomMakeCoolFighter(sld, 40, 100, 70, BLADE_LONG, "pistol3", 110);
                    GiveItem2Character(sld, "cirass5");
                    GiveItem2Character(sld, "map_nevis");
                    GiveItem2Character(sld, "map_normal");
                    GiveItem2Character(sld, "spyglass4");
                    TakeNItems(sld, "jewelry5", 150);
                }
                else FantomMakeCoolFighter(sld, 25, 80, 60, BLADE_LONG, "pistol3", 70);
            	LAi_SetWarriorType(sld);
                LAi_group_MoveCharacter(sld, "EnemyFight");
                GetCharacterPos(pchar, &locx, &locy, &locz);
				sTemp = LAi_FindNearestFreeLocator("goto", locx, locy, locz);
				if (sTemp == "") sTemp = LAi_FindNearestLocator("goto", locx, locy, locz);
               	ChangeCharacterAddressGroup(sld, "Shore_ship2", "goto", sTemp);
            }
            LAi_group_SetLookRadius("EnemyFight", 100);
            LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
            LAi_group_FightGroups("EnemyFight", LAI_GROUP_PLAYER, true);
            LAi_group_SetCheck("EnemyFight", "MorrisWillams_AfterFightInShore");
            Pchar.quest.MorrisWillams_SeekCirassaAgain.win_condition.l1 = "ExitFromLocation";
            Pchar.quest.MorrisWillams_SeekCirassaAgain.win_condition.l1.location = "Shore_ship2";
            Pchar.quest.MorrisWillams_SeekCirassaAgain.win_condition = "MorrisWillams_SeekCirassaAgain";
        break;

        case "MorrisWillams_AfterFightInShore":
            Pchar.quest.MorrisWillams_SeekCirassaAgain.over = "yes";
        break;

        case "MorrisWillams_SeekCirassaAgain":
            Pchar.quest.MorrisWillams_SeekCirassa.win_condition.l1 = "location";
            Pchar.quest.MorrisWillams_SeekCirassa.win_condition.l1.location = "Shore_ship2";
            Pchar.quest.MorrisWillams_SeekCirassa.win_condition = "MorrisWillams_SeekCirassa";
        break;
        
//==========================   Квест №8, К Эдварду Мэнсфилду за золотом ========================
        case "ToMansfield_IntoTheTown":
            sld = GetCharacter(NPC_GenerateCharacter("Dick_Manson", "pirate_8", "man", "man", 10, ENGLAND, 5, true));
            sld.Dialog.Filename = "Quest\EngLineNpc_2.c";
        	sld.name 	= "Дик";
        	sld.lastname = "Мэнсон";
        	LAi_LoginInCaptureTown(sld, true);
            ChangeCharacterAddressGroup(sld, "LaVega_tavern", "goto", "goto2");
            LAi_SetStayTypeNoGroup(sld);
			DoQuestCheckDelay("TalkSelf_Quest", 2.0); //диалог сам-на-сам
        break;

        case "ToMansfield_OutFromFort":
            Log_QuestInfo("Диалоговый буканьер убран из форта.");
            characters[GetCharacterIndex("Dick_Manson")].lifeDay = 0;
            if (pchar.questTemp.State == "ToMansfield_CatchToPardal")
            {
                Log_QuestInfo("Установлено прерывание на появление галеона возле Кубы.");
                Pchar.quest.ToMansfield_FightNearSantiago.win_condition.l1 = "location";
                Pchar.quest.ToMansfield_FightNearSantiago.win_condition.l1.location = "Cuba1";
                Pchar.quest.ToMansfield_FightNearSantiago.win_condition = "ToMansfield_FightNearSantiago";
                SetTimerCondition("ToMansfield_NotFoundGaleon", 0, 0, 12, false);
            }
            else
            {
                AddQuestRecord("Eng_Line_8_ToMansfield", "3");
                pchar.questTemp.State = "ToMansfield_DidNotFindBecaners";
            }
        break;

        case "ToMansfield_NotFoundGaleon":
            // ==> снимаем все прерывания, которые могут быть установлены
            Pchar.quest.ToMansfield_FightNearSantiago.over = "yes";
            pchar.questTemp.State = "ToMansfield_NotFoundGaleon";
            AddQuestRecord("Eng_Line_8_ToMansfield", "4");
            Log_QuestInfo("Не успел доплыть до Кубы. Галеон снят с бухты Сантьяго.");
        break;

        case "ToMansfield_FightNearSantiago":
            Log_QuestInfo("Галеон находится в бухте возле Сантьяго. Галеон идет вглубь бухты в город.");
            Pchar.quest.ToMansfield_NotFoundGaleon.over = "yes"; // снимаем таймер на появление галеона

            sld = GetCharacter(NPC_GenerateCharacter("CaptainAttack_1", "off_spa_2", "man", "man", 31, SPAIN, 0, true));
            sld.SinkTenPercent = true; // уходим с повреждениями
            sld.DontRansackCaptain = true; //квестовые не сдаются

            FantomMakeCoolSailor(sld, SHIP_GALEON_H, "", CANNON_TYPE_CANNON_LBS32, 95, 90, 90);
            FantomMakeCoolFighter(sld, 38, 90, 70, BLADE_LONG, "pistol3", 120);
			Group_FindOrCreateGroup("Spa_Attack");
			Group_SetType("Spa_Attack", "war");
			Group_AddCharacter("Spa_Attack", "CaptainAttack_1");
			Group_SetGroupCommander("Spa_Attack", "CaptainAttack_1");
            Group_SetPursuitGroup("Spa_Attack", PLAYER_GROUP);
            Group_SetAddress("Spa_Attack", "Cuba1", "", "");
            Group_SetTaskAttack("Spa_Attack", PLAYER_GROUP);
            Group_LockTask("Spa_Attack");
            //==> признак нахождения письма в комоде
            pchar.questTemp.ELQ8_PutLetter = 1;

			pchar.quest.ToMansfield_GaleonBoarding.win_condition.l1 = "Character_Capture";
			pchar.quest.ToMansfield_GaleonBoarding.win_condition.l1.character = "CaptainAttack_1";
			pchar.quest.ToMansfield_GaleonBoarding.win_condition = "ToMansfield_GaleonBoarding";

			pchar.quest.ToMansfield_GaleonSink.win_condition.l1 = "Character_sink";
			pchar.quest.ToMansfield_GaleonSink.win_condition.l1.character = "CaptainAttack_1";
			pchar.quest.ToMansfield_GaleonSink.win_condition = "ToMansfield_GaleonSink";
			pchar.quest.ToMansfield_DieHard.win_condition.l1 = "MapEnter";
            pchar.quest.ToMansfield_DieHard.win_condition = "ToMansfield_DieHard";
        break;

        case "ToMansfield_GaleonBoarding":
            if (CheckCharacterItem(pchar, "letter_2"))
            {
                Log_QuestInfo("Письмо найдено и помещено в предметы.");
                AddQuestRecord("Eng_Line_8_ToMansfield", "5");
                pchar.questTemp.State = "ToMansfield_Boarding";
                TakeItemFromCharacter(pchar, "letter_2");
                ChangeItemDescribe("letter_open", "itmdescr_letter_open_EngLineQ8");
                GiveItem2Character(pchar, "letter_open");
            }
            else
            {
                Log_QuestInfo("Письмо не найдено.");
                AddQuestRecord("Eng_Line_8_ToMansfield", "6");
                pchar.questTemp.State = "ToMansfield_BoardingNotFoundLetter";
            }
            pchar.quest.ToMansfield_GaleonSink.over = "yes";
            Pchar.quest.ToMansfield_DieHard.over = "yes"; // снимаем таймер на снятие галеона
            group_DeleteGroup("Spa_Attack");
            DeleteAttribute(PChar, "questTemp.ELQ8_PutLetter");
            BackItemDescribe("letter_2");
        break;

        case "ToMansfield_GaleonSink":
            Log_QuestInfo("Галеон потоплен.");
            pchar.quest.ToMansfield_GaleonBoarding.over = "yes";
            Pchar.quest.ToMansfield_DieHard.over = "yes"; // снимаем таймер на снятие галеона
            pchar.questTemp.State = "ToMansfield_GaleonSink";
            AddQuestRecord("Eng_Line_8_ToMansfield", "7");
            group_DeleteGroup("Spa_Attack");
            DeleteAttribute(PChar, "questTemp.ELQ8_PutLetter");
            BackItemDescribe("letter_2");
        break;

        case "ToMansfield_DieHard":
            Log_QuestInfo("Галеон снят.");
            pchar.quest.ToMansfield_GaleonBoarding.over = "yes";
            pchar.quest.ToMansfield_GaleonSink.over = "yes";
            pchar.questTemp.State = "ToMansfield_NotFoundGaleon";
            AddQuestRecord("Eng_Line_8_ToMansfield", "4");
            group_DeleteGroup("Spa_Attack");
            DeleteAttribute(PChar, "questTemp.ELQ8_PutLetter");
            BackItemDescribe("letter_2");
        break;
        
//==========================   Квест №9, Нападение на Саньяго ========================
        case "AttackSantiago_FightWithPardal":
            Log_QuestInfo("Эскадра Пардаля установлена, будет рубилово.");
            Island_SetReloadEnableGlobal("Cuba1", false);
            PChar.questTemp.ELQ9_Diffrent = 1;
            Group_FindOrCreateGroup("Spa_Attack");
			Group_SetType("Spa_Attack", "war");
            // ==> генерим эскадру Пардаля
            for (i=1; i<=4; i++)
            {
                attrName = "off_spa_1";
                Rank = 27;
                ShipType = SHIP_GALEON_H;
                if (i==1)
                {
                    attrName = "SpaOfficer1";
                    Rank = 40;
                    ShipType = SHIP_BATTLESHIP;
                }
                if (i==2)
                {
                    attrName = "off_spa_2";
                    Rank = 30;
                    ShipType = SHIP_FRIGATE;
                }
                sld = GetCharacter(NPC_GenerateCharacter("CaptainAttack_"+i, attrName, "man", "man", Rank, SPAIN, 0, true));
                FantomMakeCoolSailor(sld, ShipType, "", CANNON_TYPE_CANNON_LBS32, 95, 90, 90);
                FantomMakeCoolFighter(sld, Rank, 95, 70, BLADE_LONG, "pistol3", 120);
    			Group_AddCharacter("Spa_Attack", "CaptainAttack_"+i);
                if (i==1)
                {
                    sld.Ship.Name = "Эскадо";
                    sld.name 	= "Маноэль Риверо";
                	sld.lastname = "Пардаль";
                }
            }
			Group_SetGroupCommander("Spa_Attack", "CaptainAttack_1");
            Group_SetPursuitGroup("Spa_Attack", PLAYER_GROUP);
            Group_SetAddress("Spa_Attack", "Cuba1", "", "");
            Group_SetTaskAttack("Spa_Attack", PLAYER_GROUP);
            Group_LockTask("Spa_Attack");

            pchar.quest.AttackSantiago_AfterFight.win_condition.l1 = "Group_Death";
			pchar.quest.AttackSantiago_AfterFight.win_condition.l1.group = "Spa_Attack";
			pchar.quest.AttackSantiago_AfterFight.win_condition = "AttackSantiago_AfterFight";
			pchar.quest.AttackSantiago_DieHard.win_condition.l1 = "MapEnter";
            pchar.quest.AttackSantiago_DieHard.win_condition = "AttackSantiago_DieHard";
			// ==> запрет выхода на карту на 6мин если порт, а то есть возмоность свалить
            // === на карту сразу по выходу и это меняет схему боя.
            if (pchar.location.from_sea == "Santiago_town")
            {
                bQuestDisableMapEnter = true;
                DoQuestCheckDelay("CanEnterToMap", 400);
            }
        break;

        case "AttackSantiago_AfterFight":
			Log_QuestInfo("Эскадра Пардаля разбита.");
            pchar.questTemp.State = "AttackSantiago_GoodWork";
            AddQuestRecord("Eng_Line_9_AttackSantiago", "3");
            group_DeleteGroup("Spa_Attack");
            DeleteAttribute(PChar, "questTemp.ELQ9_Diffrent");
            ChangeCharacterReputation(pchar, 3);
            pchar.quest.AttackSantiago_DieHard.over = "yes"; //fix
            Island_SetReloadEnableGlobal("Cuba1", true);
        break;

        case "AttackSantiago_DieHard":
			pchar.quest.AttackSantiago_AfterFight.over = "yes";
			pchar.questTemp.State = "AttackSantiago_GoodWork";
            Log_QuestInfo("Эскадра Пардаля снята.");
            AddQuestRecord("Eng_Line_9_AttackSantiago", "4");
            group_DeleteGroup("Spa_Attack");
            DeleteAttribute(PChar, "questTemp.ELQ9_Diffrent");
            AddCharacterExpToSkill(pchar, "Leadership", -1650);
			AddCharacterExpToSkill(pchar, "Defence", -1300);
			AddCharacterExpToSkill(pchar, "Cannons", -1600);
			AddCharacterExpToSkill(pchar, "Sailing", -1400);
			AddCharacterExpToSkill(pchar, "Sneak", -1350);
			AddCharacterExpToSkill(pchar, "Grappling", 1200);
			AddCharacterExpToSkill(pchar, "Repair", -950);
			AddCharacterExpToSkill(pchar, "Pistol", -1200);
			AddCharacterExpToSkill(pchar, "Accuracy", -1510);
			AddCharacterExpToSkill(pchar, "Fencing", -1900);
			ChangeCharacterReputation(pchar, -5);
			Island_SetReloadEnableGlobal("Cuba1", true);
        break;
        
//==========================   Квест №10, Отбить атаку испанцев на Порт-Рояль ========================
        case "SpanishAttack_Battle":
            Log_QuestInfo("Испанская эскадра установлена, будет крутое рубилово.");
            Island_SetReloadEnableGlobal("Jamaica", false);
    		Group_FindOrCreateGroup("Spa_Attack");
			Group_SetType("Spa_Attack", "war");
            // ==> Испанские кэпы
            for (i=1; i<=7; i++)
            {
                switch (i)
                {
                    case 1:
                        ShipType = SHIP_MANOWAR;
                        Model = "off_spa_1";
                        Rank = 40;
                        Blade = "blade33";
                    break;
                    case 2:
                        ShipType = SHIP_WARSHIP;
                        Model = "off_spa_2";
                        Rank = 30;
                        Blade = "blade28";
                    break;
                    case 3:
                        ShipType = SHIP_BATTLESHIP;
                        Model = "off_spa_1";
                        Rank = 33;
                        Blade = "blade24";
                    break;
                    case 4:
                        ShipType = SHIP_MANOWAR;
                        Model = "off_spa_1";
                        Rank = 35;
                        Blade = "blade34";
                    break;
                    case 5:
                        ShipType = SHIP_LINESHIP;
                        Model = "off_spa_2";
                        Rank = 27;
                        Blade = "blade26";
                    break;
                    case 6:
                        ShipType = SHIP_BATTLESHIP;
                        Model = "off_spa_2";
                        Rank = 33;
                        Blade = "blade28";
                    break;
                    case 7:
                        ShipType = SHIP_MANOWAR;
                        Model = "off_spa_1";
                        Rank = 39;
                        Blade = "blade32";
                    break;
                }
                sld = GetCharacter(NPC_GenerateCharacter("CaptainAttack_"+i, Model, "man", "man", Rank, SPAIN, 0, true));
                FantomMakeCoolSailor(sld, ShipType, "", CANNON_TYPE_CANNON_LBS32, 100, 95, 95);
                FantomMakeCoolFighter(sld, Rank, 100, 90, Blade, "pistol3", 150);
    			Group_AddCharacter("Spa_Attack", "CaptainAttack_"+i);
            }
            // ==> стравливание
			Group_SetGroupCommander("Spa_Attack", "CaptainAttack_1");
			Group_SetTaskAttack("Spa_Attack", PLAYER_GROUP);
			Group_SetAddress("Spa_Attack", "Jamaica", "quest_ships", "quest_ship_3");
			Group_LockTask("Spa_Attack");
			// натравим сторожевиков
			//Group_SetTaskAttack("JamaykaSquadron", "Spa_Attack"); // обязательно потом сбросить, иначе вылет
			//Group_LockTask("JamaykaSquadron");
            // ==> прерывание на убиение эскадры
            pchar.quest.SpanishAttack_AfterBattle.win_condition.l1 = "Group_Death";
			pchar.quest.SpanishAttack_AfterBattle.win_condition.l1.group = "Spa_Attack";
			pchar.quest.SpanishAttack_AfterBattle.win_condition = "SpanishAttack_AfterBattle";
			pchar.quest.SpanishAttack_DieHard.win_condition.l1 = "MapEnter";
            pchar.quest.SpanishAttack_DieHard.win_condition = "SpanishAttack_DieHard";

            if (pchar.location.from_sea == "PortRoyal_town")
            {
                bQuestDisableMapEnter = true;
                DoQuestCheckDelay("CanEnterToMap", 400);
            }
        break;

        case "SpanishAttack_AfterBattle":
            /*if (!Group_isDead("JamaykaSquadron"))
			{
            	Group_SetTaskNone("JamaykaSquadron");
            }*/
			Log_QuestInfo("Испанская эскадра разбита.");
            pchar.questTemp.State = "SpanishAttack_GoodWork";
            AddQuestRecord("Eng_Line_10_SpanishAttack", "2");
            group_DeleteGroup("Spa_Attack");
            Island_SetReloadEnableGlobal("Jamaica", true);
            pchar.quest.SpanishAttack_DieHard.over = "yes";
        break;
        
        case "SpanishAttack_DieHard":
            /*if (!Group_isDead("JamaykaSquadron"))
            {
            	Group_SetTaskNone("JamaykaSquadron");
            } */
            Log_QuestInfo("Свалил с поля боя, линейке конец.");
            pchar.questTemp.State = "QuestLineBreake";
            AddQuestRecord("Eng_Line_10_SpanishAttack", "3");
			CloseQuestHeader("Eng_Line_10_SpanishAttack");
            group_DeleteGroup("Spa_Attack");
            Island_SetReloadEnableGlobal("Jamaica", true);
            pchar.quest.SpanishAttack_AfterBattle.over = "yes";
            SetCaptureTownByNation("PortRoyal", SPAIN);
            UpdateRelations();
            RefreshBattleInterface();
            SetGovenourToCity(ENGLAND, "PortSpein");
        break;

//==========================   Квест №12, Встреча полковника Линча и арест Мэдифорда ========================
        case "ColonelLinch_IntoTheResidence":
            sld = GetCharacter(NPC_GenerateCharacter("Thomas_Linch", "ThomasLinch", "man", "man", 30, ENGLAND, 10, true));
            sld.name 	= "Полковник Томас";
        	sld.lastname = "Линч";
            sld.Dialog.Filename = "Quest\EngLineNpc_1.c";
            ChangeCharacterAddressGroup(sld, "SentJons_townhall", "goto", "goto5");
            LAi_SetActorType(pchar);
            LAi_SetActorType(sld);
            SetActorDialogAny2Pchar("Thomas_Linch", "", 0.0, 1.0);
		    LAi_ActorFollow(pchar, sld, "ActorDialog_Any2Pchar", -1);
        break;

        case "ColonelLinch_AfterFight":
            pchar.nation = ENGLAND;
            Ship_FlagRefresh(PChar); //флаг на лету
            bDisableFastReload = false; // открыть переходы.
            chrDisableReloadToLocation = false; // открыть выход из локации.
            pchar.questTemp.State = "ColonelLinch_ModifordArrested";
            //LAi_LocationFightDisable(&Locations[FindLocation(pchar.location)], true);
            sld = GetCharacter(NPC_GenerateCharacter("Thomas_Modiford", "Modyford", "man", "man", 10, ENGLAND, 40, false));
            sld.name 	= "Томас";
        	sld.lastname = "Мэдифорд";
            sld.Dialog.Filename = "Quest\EngLineNpc_2.c";
            sld.dialog.currentnode = "You_baster";
			LAi_SetImmortal(sld, true);
        	FaceMaker(sld);
            SendMessage(sld, "lss", MSG_CHARACTER_SETMODEL, sld.model, sld.model.animation);
            DoReloadCharacterToLocation("PortRoyal_prison",  "goto", "goto23");
            ChangeCharacterAddressGroup(sld, "PortRoyal_prison", "goto", "goto9");
			LAi_SetActorType(sld);
            LAi_ActorDialog(sld, Pchar, "", 1.5, 0);
			pchar.quest.ColonelLinch_Exit.win_condition.l1 = "ExitFromLocation";
			pchar.quest.ColonelLinch_Exit.win_condition.l1.location = pchar.location;
			pchar.quest.ColonelLinch_Exit.win_condition = "ColonelLinch_Exit";
        break;

        case "ColonelLinch_Exit":
            sld = characterFromID("eng_guber");   //  Англ.губер будет Томасом Линчем.
            sld.model = "ThomasLinch";
            sld.name 	= "Полковник Томас";
        	sld.lastname = "Линч";
        	sld.model.animation = "man"
            SendMessage(sld, "lss", MSG_CHARACTER_SETMODEL, sld.model, sld.model.animation);
            FaceMaker(sld);
			SetGovenourToCity(ENGLAND, "PortRoyal");
            AddQuestRecord("Eng_Line_12_ColonelLinch", "3");
            LAi_SetHuberStayType(sld);
            LAi_SetImmortal(sld, false);
            LAi_SetImmortal(characterFromID("PortRoyal_Mayor"), false);
        break;
/////////////////////////////////////////////////////////////////////////////////////////////////////////
////  END Английская линейка
/////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////
////  START Голландская линейка
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//==========================   Квест №1, доставка Аарона Мендеса Чумакейро ========================
        case "SeekChumakeiro_intoMarigoHouse":
            Log_QuestInfo("Выручаем Аарона из беды.");
            chrDisableReloadToLocation = true; // закрыть выход из локации.
			LAi_group_Delete("EnemyFight");            
            for (i=1; i<=2; i++)
            {
                sld = GetCharacter(NPC_GenerateCharacter("Bandit"+i, "pirate_"+i, "man", "man", 20, PIRATE, 0, true));
                FantomMakeCoolFighter(sld, 18, 70, 60, BLADE_LONG, "pistol2", 10);
            	LAi_SetWarriorType(sld);
				LAi_warrior_DialogEnable(sld, false);
            	sld.Dialog.Filename = "Quest\HolLineNpc_1.c";
                LAi_group_MoveCharacter(sld, "EnemyFight");
               	ChangeCharacterAddressGroup(sld, "Marigo_houseF1", "goto",  "goto"+i);
            }
            LAi_SetActorType(sld);
			LAi_ActorDialog(sld, pchar, "", -1, 0);
        break;

        case "SeekChumakeiro_intoResidence":
            sld = characterFromID("Chumakeiro");                    //  Чумакейро в резиденции губернатора
            ChangeCharacterAddressGroup(sld, "Villemstad_townhall", "goto", "goto2");
            LAi_SetActorType(pchar);
            LAi_SetActorType(sld);
            SetActorDialogAny2Pchar("Chumakeiro", "", 0.0, 1.0);
		    LAi_ActorFollow(pchar, sld, "ActorDialog_Any2Pchar", -1);
        break;

//==========================   Квест №4, защита форта Оранж   ========================
        case "DefenceOrange_FightNearJamaica":
            if (GetQuestPastDayParam("questTemp") < 12)
            {
                Log_QuestInfo("Установлены английские корабли.");
                Island_SetReloadEnableGlobal("Jamaica", false);
                //bQuestDisableMapEnter = true;
        		Group_FindOrCreateGroup("Eng_Attack");
    			Group_SetType("Eng_Attack", "war");
                // ==> Английские кэпы
                for (i=1; i<=3; i++)
                {
                    switch (i)
                    {
                        case 1:
                            ShipType = SHIP_FRIGATE;
                            Model = "off_eng_1";
                            Rank = 23;
                            Blade = "blade33";
                        break;
                        case 2:
                            ShipType = SHIP_BRIG;
                            Model = "off_eng_2";
                            Rank = 19;
                            Blade = "blade28";
                        break;
                        case 3:
                            ShipType = SHIP_CORVETTE;
                            Model = "off_eng_1";
                            Rank = 21;
                            Blade = "blade24";
                        break;
                    }
                    sld = GetCharacter(NPC_GenerateCharacter("CaptainAttack_"+i, Model, "man", "man", Rank, ENGLAND, 0, true));
                    FantomMakeCoolSailor(sld, ShipType, "", CANNON_TYPE_CANNON_LBS32, 80, 70, 95);
                    FantomMakeCoolFighter(sld, Rank, 70, 50, Blade, "pistol3", 75);
        			Group_AddCharacter("Eng_Attack", "CaptainAttack_"+i);
                }
                // ==> стравливание
    			Group_SetGroupCommander("Eng_Attack", "CaptainAttack_1");
    			Group_SetTaskAttack("Eng_Attack", PLAYER_GROUP);
    			Group_SetAddress("Eng_Attack", "Jamaica", "quest_ships", "quest_ship_8");
    			Group_LockTask("Eng_Attack");
                // ==> прерывание на убиение эскадры
                pchar.quest.DefenceOrange_AfterBattle.win_condition.l1 = "Group_Death";
    			pchar.quest.DefenceOrange_AfterBattle.win_condition.l1.group = "Eng_Attack";
    			pchar.quest.DefenceOrange_AfterBattle.win_condition = "DefenceOrange_AfterBattle";
    			pchar.quest.DefenceOrange_DieHard.win_condition.l1 = "MapEnter";
                pchar.quest.DefenceOrange_DieHard.win_condition = "DefenceOrange_DieHard";
            }
            else
            {
                SetLocationCapturedState("FortOrange_town", true);
                pchar.questTemp.State = "DefenceOrange_LateRiseOrange";
				//--> огонь и пламень
				locations[FindLocation("FortOrange_town")].hidden_effects = true;
				locations[FindLocation("FortOrange_ExitTown")].hidden_effects = true;	
				//<-- огонь и пламень
            Log_QuestInfo("Не успел прибыть вовремя, форт Оранж разорен.");
            AddQuestRecord("Hol_Line_4_DefenceOrange", "3");
            }
        break;

        case "DefenceOrange_AfterBattle":
			Log_QuestInfo("Английская эскадра разбита, теперь очередь за десантом.");
            AddQuestRecord("Hol_Line_4_DefenceOrange", "2");
            pchar.questTemp.State = "DefenceOrange_SquadronDestroyed";
            group_DeleteGroup("Eng_Attack");
            Island_SetReloadEnableGlobal("Jamaica", true);
            bQuestDisableMapEnter = false;
            Pchar.quest.DefenceOrange_FightInShore.win_condition.l1 = "location";
            Pchar.quest.DefenceOrange_FightInShore.win_condition.l1.location = "Shore35";
            Pchar.quest.DefenceOrange_FightInShore.win_condition = "DefenceOrange_FightInShore";
        break;

        case "DefenceOrange_FightInShore":
            Log_QuestInfo("Схватка с английским десантом на берегу.");
            chrDisableReloadToLocation = true; // закрыть выход из локации.
            GetCharacterPos(pchar, &locx, &locy, &locz);
            for (i=1; i<=15; i++)
            {
                if (i == 3 || i == 7 || i == 11)
                {
                    Model = "off_eng_" + (rand(1)+1);
                    Blade = "blade24";
                    Rank = 25;
                    Gun = "pistol4";
                }
                else
                {
                    Model = "sold_eng_" + (rand(7)+1);
                    Rank = 20;
                    Blade = BLADE_LONG;
                    Gun = "pistol3";
                }
                sld = GetCharacter(NPC_GenerateCharacter("Solder"+i, Model, "man", "man", 25, ENGLAND, 0, true));
                FantomMakeCoolFighter(sld, Rank, 70, 60, Blade, Gun, 70);

            	LAi_SetWarriorType(sld);
                LAi_group_MoveCharacter(sld, "EnemyFight");
               	ChangeCharacterAddressGroup(sld, "Shore35", "goto", LAi_FindFarLocator("goto", locx, locy, locz));
            }
            for (i=1; i<=8; i++)
            {
                sld = GetCharacter(NPC_GenerateCharacter("pirate_"+i, "sold_hol_"+(rand(7)+1), "man", "man", 25, HOLLAND, 0, true));
                FantomMakeCoolFighter(sld, Rank, 60, 50, Blade, Gun, 40);
            	LAi_SetWarriorType(sld);
                LAi_group_MoveCharacter(sld, LAI_GROUP_PLAYER);
				sTemp = LAi_FindNearestFreeLocator("goto", locx, locy, locz);
				if (sTemp == "") sTemp = LAi_FindNearestLocator("goto", locx, locy, locz);
                ChangeCharacterAddressGroup(sld, "Shore35", "goto", sTemp);
            }
            LAi_group_SetLookRadius("EnemyFight", 100);
            LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
            LAi_group_FightGroups("EnemyFight", LAI_GROUP_PLAYER, true);
            LAi_group_SetCheck("EnemyFight", "OpenTheDoors");
            Pchar.quest.DefenceOrange_FightInExitTown.win_condition.l1 = "location";
            Pchar.quest.DefenceOrange_FightInExitTown.win_condition.l1.location = "FortOrange_ExitTown";
            Pchar.quest.DefenceOrange_FightInExitTown.win_condition = "DefenceOrange_FightInExitTown";
        break;

        case "DefenceOrange_FightInExitTown":
            Log_QuestInfo("Схватка с английским десантом у форта Оранж.");
            chrDisableReloadToLocation = true; // закрыть выход из локации.
            GetCharacterPos(pchar, &locx, &locy, &locz);
            for (i=1; i<=18; i++)
            {
                if (i == 3 || i == 7 || i == 11 || i == 17)
                {
                    Model = "off_eng_" + (rand(1)+1);
                    Blade = "blade24";
                    Rank = 25;
                    Gun = "pistol4";
                }
                else
                {
                    Model = "sold_eng_" + (rand(7)+1);
                    Rank = 20;
                    Blade = BLADE_LONG;
                    Gun = "pistol3";
                }
                sld = GetCharacter(NPC_GenerateCharacter("Solder"+i, Model, "man", "man", 25, ENGLAND, 0, true));
                FantomMakeCoolFighter(sld, Rank, 100, 70, Blade, Gun, 70);

            	LAi_SetWarriorType(sld);
                LAi_group_MoveCharacter(sld, "EnemyFight");
               	ChangeCharacterAddressGroup(sld, "FortOrange_ExitTown", "rld", LAi_FindFarLocator("rld", locx, locy, locz));
            }
            for (i=1; i<=15; i++)
            {
                sld = GetCharacter(NPC_GenerateCharacter("pirate_"+i, "sold_hol_"+(rand(7)+1), "man", "man", 25, HOLLAND, 0, true));
                FantomMakeCoolFighter(sld, Rank, 60, 50, Blade, Gun, 40);
            	LAi_SetWarriorType(sld);
                LAi_group_MoveCharacter(sld, LAI_GROUP_PLAYER);
                ChangeCharacterAddressGroup(sld, "FortOrange_ExitTown", "rld",  LAi_FindNearestLocator("rld", locx, locy, locz));
            }
            LAi_group_SetLookRadius("EnemyFight", 100);
            LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
            LAi_group_FightGroups("EnemyFight", LAI_GROUP_PLAYER, true);
            LAi_group_SetCheck("EnemyFight", "DefenceOrange_WeWon");
        break;

        case "DefenceOrange_WeWon":
            chrDisableReloadToLocation = false; // открыть выход из локации.
			Log_QuestInfo("Английский десант уничтожен.");
            AddQuestRecord("Hol_Line_4_DefenceOrange", "4");
            pchar.questTemp.State = "DefenceOrange_LandingDestroyed";
            pchar.quest.DefenceOrange_DieHard.over = "yes";
            //==> чтобы все были просто счастливы и сразу.
            QuestSetCurrentNode("FortOrange_Mayor", "DefenceOrange");
            QuestSetCurrentNode("FortOrange_waitress", "DefenceOrange");
            QuestSetCurrentNode("FortOrange_tavernkeeper", "DefenceOrange");
            QuestSetCurrentNode("FortOrange_trader", "DefenceOrange");
        break;
        
        case "DefenceOrange_DieHard":
            pchar.quest.DefenceOrange_AfterBattle.over = "yes";
            pchar.quest.DefenceOrange_FightInShore.over = "yes";
            AddQuestRecord("Hol_Line_4_DefenceOrange", "5");
            SetLocationCapturedState("FortOrange_town", true);
            group_DeleteGroup("Eng_Attack");
            Island_SetReloadEnableGlobal("Jamaica", true);
            bQuestDisableMapEnter = false;
            pchar.questTemp.State = "DefenceOrange_DieHard";
			//--> огонь и пламень
			locations[FindLocation("FortOrange_town")].hidden_effects = true;
			locations[FindLocation("FortOrange_ExitTown")].hidden_effects = true;	
			//<-- огонь и пламень
        break;
        
//==========================   Квест №5, сопровождение трех флейтов до Ла Веги и обратно  ========================
        case "ThreeFleutes_TalkIntoLaVega":
			chrDisableReloadToLocation = true;
            //===> Эдвард убит :( теперь Ла Вегой рулит капитан Купер.
            sld = characterFromID("Edward Mansfield");
            sld.name 	= "Капитан";
        	sld.lastname = "Купер";
        	sld.model.animation = "man"
            sld.model = "officer_8";
            SendMessage(sld, "lss", MSG_CHARACTER_SETMODEL, sld.model, sld.model.animation);
            FaceMaker(sld);
			//===> к ГГ подходит перец и сообщает о смерти Мэнсфилда
			sld = GetCharacter(NPC_GenerateCharacter("Talker", "officer_6", "man", "man", 25, PIRATE, 0, true));
            sld.Dialog.Filename = "Quest\HolLineNpc_1.c";
        	LAi_SetStayTypeNoGroup(sld);
            GetCharacterPos(pchar, &locx, &locy, &locz);
            ChangeCharacterAddressGroup(sld, "LaVega_town", "goto", LAi_FindNearestFreeLocator("goto", locx, locy, locz));
            LAi_SetActorType(sld);
			LAi_ActorDialog(sld, pchar, "", 1.5, 0);
        break;
        
        case "ThreeFleutes_BackToCuracao":
            Log_QuestInfo("Засада испанцев, к бою.");
            SetNationRelation2MainCharacter(SPAIN, RELATION_ENEMY); //ссорим ГГ и испанцев, а то они в нейтрале.
			SetNationRelationBoth(SPAIN, HOLLAND, RELATION_ENEMY); 
            Island_SetReloadEnableGlobal("Hispaniola1", false);
    		Group_FindOrCreateGroup("Spa_Attack");
			Group_SetType("Spa_Attack", "war");
            // ==> Испанские кэпы
            for (i=1; i<=4; i++)
            {
                switch (i)
                {
                    case 1:
                        ShipType = SHIP_LINESHIP;
                        Model = "off_spa_1";
                        Rank = 28;
                        Blade = "blade24";
                    break;
                    case 2:
                        ShipType = SHIP_GALEON_H;
                        Model = "off_spa_2";
                        Rank = 26;
                        Blade = "blade28";
                    break;
                    case 3:
                        ShipType = SHIP_CARAVEL;
                        Model = "off_spa_1";
                        Rank = 31;
                        Blade = "blade33";
                    break;
                    case 4:
                        ShipType = SHIP_GALEON_H;
                        Model = "off_spa_2";
                        Rank = 30;
                        Blade = "blade33";
                    break;
                }
                sld = GetCharacter(NPC_GenerateCharacter("CaptainAttack_"+i, Model, "man", "man", Rank, SPAIN, 0, true));
                FantomMakeCoolSailor(sld, ShipType, "", CANNON_TYPE_CANNON_LBS32, 90, 80, 80);
                FantomMakeCoolFighter(sld, Rank, 90, 70, Blade, "pistol3", 75);
    			Group_AddCharacter("Spa_Attack", "CaptainAttack_"+i);
            }
            // ==> стравливание
			Group_SetGroupCommander("Spa_Attack", "CaptainAttack_1");
			Group_SetTaskAttack("Spa_Attack", PLAYER_GROUP);
            Group_SetPursuitGroup("Spa_Attack", PLAYER_GROUP);
			Group_SetAddress("Spa_Attack", "Hispaniola1", "", "");
			Group_LockTask("Spa_Attack");
            // ==> прерывание на убиение эскадры
            pchar.quest.ThreeFleutes_AfterBattle.win_condition.l1 = "Group_Death";
			pchar.quest.ThreeFleutes_AfterBattle.win_condition.l1.group = "Spa_Attack";
			pchar.quest.ThreeFleutes_AfterBattle.win_condition = "ThreeFleutes_AfterBattle";
			pchar.quest.ThreeFleutes_DieHard.win_condition.l1 = "MapEnter";
            pchar.quest.ThreeFleutes_DieHard.win_condition = "ThreeFleutes_DieHard";
        break;

        case "ThreeFleutes_AfterBattle":
            Island_SetReloadEnableGlobal("Hispaniola1", true);
            pchar.quest.ThreeFleutes_DieHard.over = "yes";
			group_DeleteGroup("Spa_Attack");
			//--> сколько у нас флейтов из компаньонов
			iTemp = 0;
            for (i=1; i<=COMPANION_MAX; i++)
            {
                ShipType = GetCompanionIndex(pchar,i);
                if(ShipType != -1)
                {
                	sld = GetCharacter(ShipType);
                    ShipType = sti(sld.ship.type);
                    Rank = sti(RealShips[ShipType].basetype);
                    if (Rank == 7) iTemp++;
                }
            }
			//--> результат в СЖ
			if (iTemp < 3)
            {
                pchar.questTemp.State = "ThreeFleutes_LostFleuts";
                if (iTemp == 1)
                {
                    AddQuestRecord("Hol_Line_5_ThreeFleutes", "6");
                }
                else
                {
                    AddQuestRecord("Hol_Line_5_ThreeFleutes", "5");
                    AddQuestUserData("Hol_Line_5_ThreeFleutes", "QtyFleutes", iTemp);
                }
            }
            else
            {
                AddQuestRecord("Hol_Line_5_ThreeFleutes", "4");
                pchar.questTemp.State = "ThreeFleutes_AllFleuts";
            }
        break;
        
        case "ThreeFleutes_DieHard":
            pchar.quest.ThreeFleutes_AfterBattle.over = "yes";
            AddQuestRecord("Hol_Line_5_ThreeFleutes", "3");
            pchar.questTemp.State = "ThreeFleutes_DieHard";
            Island_SetReloadEnableGlobal("Hispaniola1", true);
            group_DeleteGroup("Spa_Attack");
            for (i=1; i<=3; i++)
            {
                sld = characterFromID("Captain_"+i);
				if (sld.id != "none")
				{
					RemoveCharacterCompanion(pchar, sld);
					sld.LifeDay = 0;
				}
            }
        break;

//==========================   Квест №7, доставка депеши на Тортугу  ========================
        case "DelivLettTortuga_SnapMeeting":
			chrDisableReloadToLocation = true;
            Log_QuestInfo("Встреча с незнакомцем.");
            sld = GetCharacter(NPC_GenerateCharacter("Stranger_HL7", "citiz_2", "man", "man", 15, FRANCE, 0, false));
            sld.Dialog.Filename = "Quest\HolLineNpc_1.c";
        	sld.name 	= "Гийом";
        	sld.lastname = "Дотей";
            GetCharacterPos(pchar, &locx, &locy, &locz);
         	ChangeCharacterAddressGroup(sld, "Tortuga_town", "goto", LAi_FindNearestFreeLocator("goto", locx, locy, locz));
            LAi_SetActorType(sld);
			LAi_ActorDialog(sld, pchar, "", -1, 0);
        break;

        case "DelivLettTortuga_2KickPhar":
            LAi_SetCitizenType(characterFromId("AntonioDeBarras"));
            LAi_ActorAnimation(pchar, "death_2", "DelivLettTortuga_3KickPhar", 3.0);
        break;

        case "DelivLettTortuga_3KickPhar":
            LAi_ActorSetLayMode(pchar);
            DoQuestCheckDelay("DelivLettTortuga_KickPhar", 1.0);
            sld = characterFromID("AntonioDeBarras");
            ChangeCharacterAddress(sld, "None", "");
        break;

        case "DelivLettTortuga_KickPhar":
            SetLaunchFrameFormParam("Прошло что-то около часа...", "", 0, 3);
            LaunchFrameForm();
            WaitDate("", 0, 0, 0, 1, 10); //крутим час
        	RecalculateJumpTable();
			pchar.questTemp.State = "DelivLettTortuga_AfterRape";
            //LAi_SetStayType(sld);
            TakeItemFromCharacter(pchar, "letter_1");
            AddQuestRecord("Hol_Line_7_DelivLettTortuga", "4");
            LAi_SetPlayerType(pchar);
            LAi_LocationFightDisable(&Locations[FindLocation("Tortuga_tavern_upstairs")], false);
        break;
        
        case "DelivLettTortuga_BrigTimeOver":
            pchar.quest.DelivLettTortuga_AttackBrig.over = "yes";
            AddQuestRecord("Hol_Line_7_DelivLettTortuga", "7");
            pchar.questTemp.State.KingResult = "BrigNotFound"; //флаг на вообще не нашел
        break;

        case "DelivLettTortuga_AttackBrig":
            Log_QuestInfo("Бриг найден, на абордаж!");
            pchar.quest.DelivLettTortuga_BrigTimeOver.over = "yes";
            //SetNationRelation2MainCharacter(PIRATE, RELATION_ENEMY); //ссорим ГГ и пиратов.
            Island_SetReloadEnableGlobal("PuertoRico", false);
    		Group_FindOrCreateGroup("Pirate_Attack");
			Group_SetType("Pirate_Attack", "trade");
            sld = GetCharacter(NPC_GenerateCharacter("CaptainAttack_1", "officer_15", "man", "man", 30, PIRATE, 0, true));
            sld.Dialog.Filename    = "Quest\HolLineNpc_2.c";
			sld.DontRansackCaptain = true;
            FantomMakeCoolSailor(sld, SHIP_BRIG, "Ла Рошель", CANNON_TYPE_CULVERINE_LBS24, 90, 80, 80);
            FantomMakeCoolFighter(sld, 30, 100, 40, "blade24", "pistol3", 80);
			Group_AddCharacter("Pirate_Attack", "CaptainAttack_1");
            // ==> стравливание
			Group_SetGroupCommander("Pirate_Attack", "CaptainAttack_1");
			Group_SetTaskAttack("Pirate_Attack", PLAYER_GROUP);
            Group_SetPursuitGroup("Pirate_Attack", PLAYER_GROUP);
			Group_SetAddress("Pirate_Attack", "PuertoRico", "", "");
			Group_LockTask("Pirate_Attack");
            // ==> прерывания
			pchar.quest.DelivLettTortuga_AfterBrigSunk.win_condition.l1 = "Character_sink";
			pchar.quest.DelivLettTortuga_AfterBrigSunk.win_condition.l1.character = "CaptainAttack_1";
			pchar.quest.DelivLettTortuga_AfterBrigSunk.win_condition = "DelivLettTortuga_AfterBrigSunk";
			pchar.quest.DelivLettTortuga_BrigDieHard.win_condition.l1 = "MapEnter";
            pchar.quest.DelivLettTortuga_BrigDieHard.win_condition = "DelivLettTortuga_BrigDieHard";
        break;
        
        case "DelivLettTortuga_BrigDieHard":
            pchar.quest.DelivLettTortuga_AfterBrigSunk.over = "yes";
            group_DeleteGroup("Pirate_Attack");
            Island_SetReloadEnableGlobal("PuertoRico", true);
            AddQuestRecord("Hol_Line_7_DelivLettTortuga", "7");
            pchar.questTemp.State.KingResult = "BrigNotFound"; //флаг на вообще не нашел
        break;

        case "DelivLettTortuga_AfterBrigSunk":
            pchar.quest.DelivLettTortuga_BrigDieHard.over = "yes";
            Island_SetReloadEnableGlobal("PuertoRico", true);
            group_DeleteGroup("Pirate_Attack");
            AddQuestRecord("Hol_Line_7_DelivLettTortuga", "6");
            pchar.questTemp.State.KingResult = "BrigSunk"; //флаг на утонул
        break;
        
        case "DelivLettTortuga_WarshipTimeOver":
            pchar.quest.DelivLettTortuga_AttackWarship.over = "yes";
            AddQuestRecord("Hol_Line_7_DelivLettTortuga", "9");
        break;

        case "DelivLettTortuga_AttackWarship":
            Log_QuestInfo("Военный галеон найден, на абордаж!");
            pchar.quest.DelivLettTortuga_WarshipTimeOver.over = "yes";
            SetNationRelation2MainCharacter(SPAIN, RELATION_ENEMY); //ссорим ГГ и испанцев.
			SetNationRelationBoth(SPAIN, HOLLAND, RELATION_ENEMY); 
            Island_SetReloadEnableGlobal("SantaCatalina", false);
    		Group_FindOrCreateGroup("Spa_Attack");
			Group_SetType("Spa_Attack", "war");
			Group_AddCharacter("Spa_Attack", "AntonioDeBarras");
            // ==> стравливание
			Group_SetGroupCommander("Spa_Attack", "AntonioDeBarras");
			Group_SetTaskAttack("Spa_Attack", PLAYER_GROUP);
            Group_SetPursuitGroup("Spa_Attack", PLAYER_GROUP);
			Group_SetAddress("Spa_Attack", "SantaCatalina", "", "");
			Group_LockTask("Spa_Attack");
            // ==> прерывания
			pchar.quest.DelivLettTortuga_AfterWarshipSunk.win_condition.l1 = "Character_sink";
			pchar.quest.DelivLettTortuga_AfterWarshipSunk.win_condition.l1.character = "AntonioDeBarras";
			pchar.quest.DelivLettTortuga_AfterWarshipSunk.win_condition = "DelivLettTortuga_AfterWarshipSunk";
			pchar.quest.DelivLettTortuga_WarshipDieHard.win_condition.l1 = "MapEnter";
            pchar.quest.DelivLettTortuga_WarshipDieHard.win_condition = "DelivLettTortuga_WarshipDieHard";
        break;

        case "DelivLettTortuga_WarshipDieHard":
            pchar.quest.DelivLettTortuga_AfterWarshipSunk.over = "yes";
            group_DeleteGroup("Spa_Attack");
            Island_SetReloadEnableGlobal("SantaCatalina", true);
            AddQuestRecord("Hol_Line_7_DelivLettTortuga", "9");
        break;

        case "DelivLettTortuga_AfterWarshipSunk":
            pchar.quest.DelivLettTortuga_WarshipDieHard.over = "yes";
            Island_SetReloadEnableGlobal("SantaCatalina", true);
            group_DeleteGroup("Spa_Attack");
            AddQuestRecord("Hol_Line_7_DelivLettTortuga", "10");
            pchar.questTemp.State.KingResult = "WarshipSunk"; //флаг на утонул
        break;

//==========================   Квест №8, поиски Библии  ========================
        case "SeekBible_SaveDeGraf":
            Log_QuestInfo("Нужно спасти фрегат де Графа!");
            Island_SetReloadEnableGlobal("Cartahena", false);
            SetNationRelation2MainCharacter(SPAIN, RELATION_ENEMY); //ссорим ГГ и испанцев. 
            SetNationRelationBoth(SPAIN, HOLLAND, RELATION_ENEMY);
    		Group_FindOrCreateGroup("Spa_Attack");
			Group_SetType("Spa_Attack", "war");
            // ==> Испанские кэпы
            for (i=1; i<=5; i++)
            {
                switch (i)
                {
                    case 1:
                        ShipType = SHIP_BATTLESHIP;
                        Model = "SpaOfficer2";
                        Rank = 28;
                        Blade = "blade24";
                    break;
                    case 2:
                        ShipType = SHIP_GALEON_H;
                        Model = "off_spa_2";
                        Rank = 26;
                        Blade = "blade28";
                    break;
                    case 3:
                        ShipType = SHIP_LINESHIP;
                        Model = "off_spa_1";
                        Rank = 34;
                        Blade = "blade28";
                    break;
                    case 4:
                        ShipType = SHIP_GALEON_H;
                        Model = "off_spa_2";
                        Rank = 30;
                        Blade = "blade33";
                    break;
                    case 5:
                        ShipType = SHIP_CARAVEL;
                        Model = "off_spa_1";
                        Rank = 36;
                        Blade = "blade28";
                    break;
                }
                sld = GetCharacter(NPC_GenerateCharacter("CaptainAttack_"+i, Model, "man", "man", Rank, SPAIN, 0, true));
                FantomMakeCoolSailor(sld, ShipType, "", CANNON_TYPE_CANNON_LBS32, 95, 90, 85);
                FantomMakeCoolFighter(sld, Rank, 96, 80, Blade, "pistol3", 85);
    			Group_AddCharacter("Spa_Attack", "CaptainAttack_"+i);
            }
			Group_SetGroupCommander("Spa_Attack", "CaptainAttack_1");
            Group_SetPursuitGroup("Spa_Attack", PLAYER_GROUP);
			Group_SetAddress("Spa_Attack", "Cartahena", "", "");
            // ==> де Граф
            sld = GetCharacter(NPC_GenerateCharacter("LoranDeGraf", "officer_14", "man", "man", 45, HOLLAND, -1, true));
            sld.DeckDialogNode = "First time";
            sld.Dialog.Filename = "Quest\HolLineNpc_2.c";
        	sld.name 	= "Лоран";
        	sld.lastname = "де Граф";
			sld.ShipEnemyDisable = true; //при попадании не враждебен
        	sld.Abordage.Enable = false; // НЕЛЬЗЯ!
            LAi_SetStayTypeNoGroup(sld);
            FantomMakeCoolSailor(sld, SHIP_FRIGATE, "Коминте", CANNON_TYPE_CANNON_LBS32, 90, 80, 80);
            FantomMakeCoolFighter(sld, 45, 90, 70, "topor2", "pistol3", 100);
            Group_AddCharacter("Quest_Group", "LoranDeGraf");
            Group_SetType("Quest_Group", "war");
            Group_SetGroupCommander("Quest_Group", "LoranDeGraf");
            Group_SetPursuitGroup("Quest_Group", PLAYER_GROUP);
            Group_SetAddress("Quest_Group", "Cartahena", "", "");
			// ==> стравливание
			Group_SetTaskAttack("Spa_Attack", "Quest_Group");
            Group_SetTaskAttack("Quest_Group", "Spa_Attack");
            Group_LockTask("Quest_Group");
			Group_LockTask("Spa_Attack");
            pchar.quest.SeekBible_WeWon.win_condition.l1 = "Group_Death";
			pchar.quest.SeekBible_WeWon.win_condition.l1.group = "Spa_Attack";
			pchar.quest.SeekBible_WeWon.win_condition = "SeekBible_WeWon";
    		pchar.quest.SeekBible_NotSavedDeGraf.win_condition.l1 = "NPC_Death";
			pchar.quest.SeekBible_NotSavedDeGraf.win_condition.l1.character = "LoranDeGraf";
			pchar.quest.SeekBible_NotSavedDeGraf.win_condition = "SeekBible_NotSavedDeGraf";
			pchar.quest.SeekBible_DieHard.win_condition.l1 = "MapEnter";
            pchar.quest.SeekBible_DieHard.win_condition = "SeekBible_DieHard";
        break;
        
        case "SeekBible_WeWon":
            pchar.quest.SeekBible_NotSavedDeGraf.over = "yes";
            pchar.quest.SeekBible_DieHard.over = "yes";
            group_DeleteGroup("Spa_Attack");
            Island_SetReloadEnableGlobal("Cartahena", true);
            AddQuestRecord("Hol_Line_8_SeekBible", "8");
            pchar.questTemp.State = "SeekBible_WeWon";  
        break;

        case "SeekBible_DieHard":
            if (pchar.questTemp.State == "SeekBible_DeGrafIsDead")
            {
                group_DeleteGroup("Spa_Attack");
            }
            else
            {
                pchar.quest.SeekBible_WeWon.over = "yes";
                pchar.quest.SeekBible_NotSavedDeGraf.over = "yes";
                group_DeleteGroup("Spa_Attack");
                group_DeleteGroup("Quest_Group");
                Island_SetReloadEnableGlobal("Cartahena", true);
                AddQuestRecord("Hol_Line_8_SeekBible", "6");
                pchar.questTemp.State = "SeekBible_DeGrafDieHard";
            }
        break;
        
        case "SeekBible_NotSavedDeGraf":
            pchar.quest.SeekBible_WeWon.over = "yes";
            Island_SetReloadEnableGlobal("Cartahena", true);
            AddQuestRecord("Hol_Line_8_SeekBible", "7");
            pchar.questTemp.State = "SeekBible_DeGrafIsDead";
        break;
        
        case "SeekBible_DeGrafToTavern":
            sld = characterFromId("LoranDeGraf");
            FreeSitLocator("Tortuga_tavern", "sit3");
			ChangeCharacterAddressGroup(sld, "Tortuga_tavern", "sit", "sit3");			
            LAi_SetSitTypeNoGroup(sld);
            group_DeleteGroup("Quest_Group");
        break;
        
//==========================   Квест №10, Оборона Кюрасао.  ========================
        case "SpaAttackCuracao_Battle":
            SetNationRelation2MainCharacter(SPAIN, RELATION_ENEMY); //ссорим ГГ и испанцев.
            SetNationRelationBoth(SPAIN, HOLLAND, RELATION_ENEMY);
            Log_QuestInfo("Испанская эскадра установлена, будет крутое рубилово.");
            Island_SetReloadEnableGlobal("Curacao", false);
    		Group_FindOrCreateGroup("Spa_Attack");
			Group_SetType("Spa_Attack", "war");
            // ==> Испанские кэпы
            for (i=1; i<=7; i++)
            {
                switch (i)
                {
                    case 1:
                        ShipType = SHIP_MANOWAR;
                        Model = "off_spa_1";
                        Rank = 45;
                        Blade = "blade33";
                    break;
                    case 2:
                        ShipType = SHIP_LINESHIP;
                        Model = "off_spa_2";
                        Rank = 35;
                        Blade = "blade28";
                    break;
                    case 3:
                        ShipType = SHIP_BATTLESHIP;
                        Model = "off_spa_1";
                        Rank = 33;
                        Blade = "blade24";
                    break;
                    case 4:
                        ShipType = SHIP_MANOWAR;
                        Model = "off_spa_1";
                        Rank = 35;
                        Blade = "blade34";
                    break;
                    case 5:
                        ShipType = SHIP_GALEON_H;
                        Model = "off_spa_2";
                        Rank = 32;
                        Blade = "topor2";
                    break;
                    case 6:
                        ShipType = SHIP_MANOWAR;
                        Model = "off_spa_2";
                        Rank = 32;
                        Blade = "topor2";
                    break;
                    case 7:
                        ShipType = SHIP_WARSHIP;
                        Model = "off_spa_2";
                        Rank = 32;
                        Blade = "topor2";
                    break;
                }
                sld = GetCharacter(NPC_GenerateCharacter("CaptainAttack_"+i, Model, "man", "man", Rank, SPAIN, 0, true));
                FantomMakeCoolSailor(sld, ShipType, "", CANNON_TYPE_CANNON_LBS32, 100, 95, 95);
                FantomMakeCoolFighter(sld, Rank, 100, 90, Blade, "pistol3", 150);
    			Group_AddCharacter("Spa_Attack", "CaptainAttack_"+i);
            }
            // ==> стравливание
			Group_SetGroupCommander("Spa_Attack", "CaptainAttack_1");
			Group_SetTaskAttack("Spa_Attack", PLAYER_GROUP);
            Group_SetPursuitGroup("Spa_Attack", PLAYER_GROUP);
			Group_SetAddress("Spa_Attack", "Curacao", "", "");
			Group_LockTask("Spa_Attack");
			// натравим сторожевиков
			//Group_SetTaskAttack("CuracaoSquadron", "Spa_Attack"); // обязательно потом сбросить, иначе вылет
			//Group_LockTask("CuracaoSquadron");
            // ==> прерывание на убиение эскадры
            pchar.quest.SpaAttackCuracao_AfterBattle.win_condition.l1 = "Group_Death";
			pchar.quest.SpaAttackCuracao_AfterBattle.win_condition.l1.group = "Spa_Attack";
			pchar.quest.SpaAttackCuracao_AfterBattle.win_condition = "SpaAttackCuracao_AfterBattle";
			pchar.quest.SpaAttackCuracao_DieHard.win_condition.l1 = "MapEnter";
            pchar.quest.SpaAttackCuracao_DieHard.win_condition = "SpaAttackCuracao_DieHard";

            if (pchar.location.from_sea == "Villemstad_town")
            {
                bQuestDisableMapEnter = true;
                DoQuestCheckDelay("CanEnterToMap", 400);
            }
        break;

        case "SpaAttackCuracao_AfterBattle":
            /*if (!Group_isDead("CuracaoSquadron"))
			{
            	Group_SetTaskNone("CuracaoSquadron");
            }*/
			Log_QuestInfo("Испанская эскадра разбита.");
            pchar.questTemp.State = "SpaAttackCuracao_GoodWork";
            AddQuestRecord("Hol_Line_10_SpaAttackCuracao", "2");
            group_DeleteGroup("Spa_Attack");
            Island_SetReloadEnableGlobal("Curacao", true);
            pchar.quest.SpaAttackCuracao_DieHard.over = "yes";
        break;

        case "SpaAttackCuracao_DieHard":
            /*if (!Group_isDead("CuracaoSquadron"))
            {
            	Group_SetTaskNone("CuracaoSquadron");
            } */
            Log_QuestInfo("Свалил с поля боя, линейке конец.");
            pchar.questTemp.State = "QuestLineBreake";
            AddQuestRecord("Hol_Line_10_SpaAttackCuracao", "3");
			CloseQuestHeader("Hol_Line_10_SpaAttackCuracao");
            group_DeleteGroup("Spa_Attack");
            Island_SetReloadEnableGlobal("Curacao", true);
            pchar.quest.SpaAttackCuracao_AfterBattle.over = "yes";
            SetCaptureTownByNation("Villemstad", SPAIN);
            UpdateRelations();
            RefreshBattleInterface();
            SetGovenourToCity(HOLLAND, "Marigo");
        break;

//==========================   Квест №11, Спасти Мариго от нападения испанцев.  ========================
        case "SpaAttackSentMartin_YouLate":
            Log_QuestInfo("Опаздал. Мариго захвачен испанцами.");
            pchar.questTemp.State = "SpaAttackSentMartin_Late";
            AddQuestRecord("Hol_Line_11_SpaAttackSentMartin", "4");
            pchar.quest.SpaAttackSentMartin_Battle.over = "yes";
            SetCaptureTownByNation("Marigo", SPAIN);
            UpdateRelations();
            RefreshBattleInterface();
        break;

        case "SpaAttackSentMartin_Battle":
            Log_QuestInfo("Испанская эскадра установлена, будет крутое рубилово.");
            pchar.quest.SpaAttackSentMartin_YouLate.over = "yes";
            Island_SetReloadEnableGlobal("SentMartin", false);
    		Group_FindOrCreateGroup("Spa_Attack");
			Group_SetType("Spa_Attack", "war");
            // ==> Испанские кэпы
            for (i=1; i<=8; i++)
            {
                switch (i)
                {
                    case 1:
                        ShipType = SHIP_MANOWAR;
                        Model = "off_spa_1";
                        Rank = 42;
                        Blade = "blade33";
                    break;
                    case 2:
                        ShipType = SHIP_GALEON_H;
                        Model = "off_spa_1";
                        Rank = 42;
                        Blade = "blade33";
                    break;
                    case 3:
                        ShipType = SHIP_WARSHIP;
                        Model = "off_spa_2";
                        Rank = 32;
                        Blade = "blade28";
                    break;
                    case 4:
                        ShipType = SHIP_BATTLESHIP;
                        Model = "off_spa_1";
                        Rank = 35;
                        Blade = "blade24";
                    break;
                    case 5:
                        ShipType = SHIP_MANOWAR;
                        Model = "off_spa_1";
                        Rank = 35;
                        Blade = "blade34";
                    break;
                    case 6:
                        ShipType = SHIP_LINESHIP;
                        Model = "off_spa_2";
                        Rank = 29;
                        Blade = "blade26";
                    break;
                    case 7:
                        ShipType = SHIP_GALEON_H;
                        Model = "off_spa_2";
                        Rank = 33;
                        Blade = "blade28";
                    break;
                    case 8:
                        ShipType = SHIP_MANOWAR;
                        Model = "off_spa_1";
                        Rank = 39;
                        Blade = "blade32";
                    break;
                }
                sld = GetCharacter(NPC_GenerateCharacter("CaptainAttack_"+i, Model, "man", "man", Rank, SPAIN, 0, true));
                FantomMakeCoolSailor(sld, ShipType, "", CANNON_TYPE_CANNON_LBS32, 100, 95, 95);
                FantomMakeCoolFighter(sld, Rank, 100, 90, Blade, "pistol3", 150);
    			Group_AddCharacter("Spa_Attack", "CaptainAttack_"+i);
            }
            // ==> стравливание
			Group_SetGroupCommander("Spa_Attack", "CaptainAttack_1");
			Group_SetTaskAttack("Spa_Attack", PLAYER_GROUP);
			Group_SetAddress("Spa_Attack", "SentMartin", "quest_ships", "quest_ship_4");
			Group_LockTask("Spa_Attack");
			// натравим сторожевиков
			//Group_SetTaskAttack("SentMartinSquadron", "Spa_Attack"); // обязательно потом сбросить, иначе вылет
			//Group_LockTask("SentMartinSquadron");
            // ==> прерывание на убиение эскадры
            pchar.quest.SpaAttackSentMartin_AfterBattle.win_condition.l1 = "Group_Death";
			pchar.quest.SpaAttackSentMartin_AfterBattle.win_condition.l1.group = "Spa_Attack";
			pchar.quest.SpaAttackSentMartin_AfterBattle.win_condition = "SpaAttackSentMartin_AfterBattle";
			pchar.quest.SpaAttackSentMartin_DieHard.win_condition.l1 = "MapEnter";
            pchar.quest.SpaAttackSentMartin_DieHard.win_condition = "SpaAttackSentMartin_DieHard";
        break;

        case "SpaAttackSentMartin_AfterBattle":
            /*if (!Group_isDead("SentMartinSquadron"))
			{
            	Group_SetTaskNone("SentMartinSquadron");
            }*/
			Log_QuestInfo("Испанская эскадра разбита.");
            pchar.questTemp.State = "SpaAttackSentMartin_GoodWork";
            AddQuestRecord("Hol_Line_11_SpaAttackSentMartin", "2");
            group_DeleteGroup("Spa_Attack");
            Island_SetReloadEnableGlobal("SentMartin", true);
            pchar.quest.SpaAttackSentMartin_DieHard.over = "yes";
        break;

        case "SpaAttackSentMartin_DieHard":
            /*if (!Group_isDead("SentMartinSquadron"))
            {
            	Group_SetTaskNone("SentMartinSquadron");
            } */
            Log_QuestInfo("Свалил с поля боя, линейке конец.");
            pchar.questTemp.State = "QuestLineBreake";
            AddQuestRecord("Hol_Line_11_SpaAttackSentMartin", "3");
			CloseQuestHeader("Hol_Line_11_SpaAttackSentMartin");
            group_DeleteGroup("Spa_Attack");
            Island_SetReloadEnableGlobal("SentMartin", true);
            pchar.quest.SpaAttackSentMartin_AfterBattle.over = "yes";
            SetCaptureTownByNation("Marigo", SPAIN);
            UpdateRelations();
            RefreshBattleInterface();
        break;
/////////////////////////////////////////////////////////////////////////////////////////////////////////
////  END Голландская линейка
/////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////
////  START Испанская линейка
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//========================  Квест №2, задание Инквизиции.  =======================
        case "Inquisition_MeetFrancisco":
			chrDisableReloadToLocation = true;
            Log_QuestInfo("Встреча с Франциско.");
            sld = GetCharacter(NPC_GenerateCharacter("Francisco", "priest_4", "man", "man", 15, SPAIN, 0, false));
            sld.Dialog.Filename = "Quest\SpaLineNpc_2.c";
        	sld.name 	= "Франциско";
        	sld.lastname = "де Сан-Агостиньо";
            GetCharacterPos(pchar, &locx, &locy, &locz);
         	ChangeCharacterAddressGroup(sld, "Santiago_town", "goto", LAi_FindNearestFreeLocator("goto", locx, locy, locz));
            LAi_SetActorType(sld);
			LAi_ActorDialog(sld, pchar, "", -1, 0);
        break;

        case "Inquisition_afterFightAndrew":
            LAi_LocationFightDisable(&Locations[FindLocation("Villemstad_bank")], true);
            sld = characterFromId("Villemstad_usurer");
            LAi_IsDead(sld) LAi_SetHP(sld, 100, 100);
        	sld.location	= "Villemstad_Bank";
        	sld.location.group = "barmen";
        	sld.location.locator = "stay";
        	LAi_SetBarmanType(sld);
        	LAi_group_MoveCharacter(sld, "HOLLAND_CITIZENS");
        break;
        
        case "Inquisition_LuggerTimeOver":
            pchar.quest.Inquisition_AttackLugger.over = "yes";
            AddQuestRecord("Spa_Line_2_Inquisition", "7");
        break;

        case "Inquisition_AttackLugger":
            Log_QuestInfo("Люггер найден, на абордаж!");
            pchar.quest.Inquisition_LuggerTimeOver.over = "yes";
            //SetNationRelation2MainCharacter(PIRATE, RELATION_ENEMY); //ссорим ГГ и пиратов.
            Island_SetReloadEnableGlobal("Jamaica", false);
    		Group_FindOrCreateGroup("Pirate_Attack");
			Group_SetType("Pirate_Attack", "trade");
            sld = GetCharacter(NPC_GenerateCharacter("JansenDeFonceicao", "shipowner_5", "man", "man", 25, PIRATE, -1, true));
            sld.Dialog.Filename    = "Quest\SpaLineNpc_2.c";
        	sld.name 	= "Янсен Нунен";
        	sld.lastname 	= "де Фонкесао";
			sld.DontRansackCaptain = true;
            FantomMakeCoolSailor(sld, SHIP_LUGGER, "Отчаянный", CANNON_TYPE_CULVERINE_LBS16, 100, 100, 100);
            FantomMakeCoolFighter(sld, 25, 60, 40, "blade24", "pistol3", 70);
            Group_AddCharacter("Pirate_Attack", "JansenDeFonceicao");
            // ==> стравливание
			Group_SetGroupCommander("Pirate_Attack", "JansenDeFonceicao");
			Group_SetTaskAttack("Pirate_Attack", PLAYER_GROUP);
			Group_SetAddress("Pirate_Attack", "Jamaica", "quest_ships", "quest_ship_7");
			Group_LockTask("Pirate_Attack");
            // ==> прерывания
			pchar.quest.Inquisition_AfterLuggerSink.win_condition.l1 = "NPC_Death";
			pchar.quest.Inquisition_AfterLuggerSink.win_condition.l1.character = "JansenDeFonceicao";
			pchar.quest.Inquisition_AfterLuggerSink.win_condition = "Inquisition_AfterLuggerSink";
			pchar.quest.Inquisition_DieHard.win_condition.l1 = "MapEnter";
            pchar.quest.Inquisition_DieHard.win_condition = "Inquisition_DieHard";
        break;
        
        case "Inquisition_DieHard":
            pchar.quest.Inquisition_LuggerSink.over = "yes";
            group_DeleteGroup("Pirate_Attack");
            Island_SetReloadEnableGlobal("Jamaica", true);
            AddQuestRecord("Spa_Line_2_Inquisition", "7");
        break;

        case "Inquisition_AfterLuggerSink":
			if (!CheckAttribute(pchar, "GenQuest.LastQuestPrisonerIdx"))
			{
				pchar.quest.Inquisition_DieHard.over = "yes";
				Island_SetReloadEnableGlobal("Jamaica", true);
				group_DeleteGroup("Pirate_Attack");
				AddQuestRecord("Spa_Line_2_Inquisition", "9");
			}
			else
			{
				characters[sti(pchar.GenQuest.LastQuestPrisonerIdx)].id = "JansenDeFonceicao";
			}
        break;
        
        case "Inquisition_fightForJacow":
            Log_QuestInfo("Яков здесь.");
            chrDisableReloadToLocation = true; // закрыть выход из локации.
            sld = GetCharacter(NPC_GenerateCharacter("Bandit", "pirate_10", "man", "man", 25, PIRATE, 0, true));
            FantomMakeCoolFighter(sld, 25, 70, 60, "blade21", "pistol2", 30);
        	LAi_SetStayType(sld);
        	sld.Dialog.Filename = "Quest\SpaLineNpc_2.c";
            LAi_group_MoveCharacter(sld, "EnemyFight");
           	ChangeCharacterAddressGroup(sld, "Panama_houseSp1", "goto",  "goto1");
            LAi_SetActorType(pchar);
            LAi_SetActorType(sld);
            LAi_ActorTurnToCharacter(sld, pchar);
            SetActorDialogAny2Pchar(sld.id, "", -1, 0.0);
		    LAi_ActorFollow(pchar, sld, "ActorDialog_Any2Pchar", 0.3);
            Pchar.quest.Inquisition_fightForJacow2.win_condition.l1 = "location";
            Pchar.quest.Inquisition_fightForJacow2.win_condition.l1.location = "Panama_houseSp1_Room";
            Pchar.quest.Inquisition_fightForJacow2.win_condition = "Inquisition_fightForJacow2";
       break;

       case "Inquisition_fightForJacow2":
            chrDisableReloadToLocation = true; // закрыть выход из локации.
            sld = GetCharacter(NPC_GenerateCharacter("JacowDeFonseka", "trader_7", "man", "man", 10, HOLLAND, -1, false));
            ChangeCharacterAddressGroup(sld, "Panama_houseSp1_Room", "goto",  "goto4");
           	sld.Dialog.Filename = "Quest\SpaLineNpc_2.c";
			sld.name 	= "Яков Лопез";
			sld.lastname 	= "де Фонсека";            
			LAi_SetImmortal(sld, true);
            LAi_SetStayTypeNoGroup(sld);
            for (i=1; i<=3; i++)
            {
                sld = GetCharacter(NPC_GenerateCharacter("Bandit"+i, "pirate_"+i, "man", "man", 25, PIRATE, 0, true));
                FantomMakeCoolFighter(sld, 25, 70, 60, BLADE_LONG, "pistol3", 30);
            	LAi_SetStayType(sld);
                LAi_group_MoveCharacter(sld, "EnemyFight");
               	ChangeCharacterAddressGroup(sld, "Panama_houseSp1_Room", "goto",  "goto"+i);
            }
            LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
            LAi_group_FightGroups("EnemyFight", LAI_GROUP_PLAYER, true);
            LAi_group_SetCheck("EnemyFight", "Inquisition_afterFightJacow");
       break;

       case "Inquisition_afterFightJacow":
            chrDisableReloadToLocation = false;
            LAi_SetActorType(pchar);
            sld = characterFromID("JacowDeFonseka");
            LAi_SetActorType(sld);
            LAi_ActorTurnToCharacter(sld, pchar);
            SetActorDialogAny2Pchar(sld.id, "", 0.0, 0.0);
		    LAi_ActorFollow(pchar, sld, "ActorDialog_Any2Pchar", -1);
       break;

       case "Inquisition_fightInDangeon":
            Log_QuestInfo("Со скелетами надо подраться.");
            chrDisableReloadToLocation = true;
            for (i=1; i<=40; i++)
            {
                sld = GetCharacter(NPC_GenerateCharacter("Skelet"+i, "Skel"+(rand(3)+1), "man", "man", 1, PIRATE, 0, true));
				sld.rank = 1;
				sld.skill.Fencing = 1;                
				sld.skill.Pistol = 1;
				LAi_SetHP(sld, 1, 1);
				DeleteAttribute(sld, "Items");
				GiveItem2Character(sld, "blade1");
				EquipCharacterbyItem(sld, "blade1");
				GiveItem2Character(sld, "pistol1");
				EquipCharacterbyItem(sld, "pistol1");
				TakeNItems(sld,"bullet", 1);
            	LAi_SetWarriorType(sld);
                LAi_group_MoveCharacter(sld, "EnemyFight");
                ChangeCharacterAddressGroup(sld, "Bermudes_Dungeon", "monsters",  "monster"+i);
            }
            LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
            LAi_group_FightGroups("EnemyFight", LAI_GROUP_PLAYER, true);
            LAi_group_SetCheck("EnemyFight", "Inquisition_afterFightSkel");
       break;

       case "Inquisition_afterFightSkel":
            LocatorReloadEnterDisable("Pirates_Shipyard", "reload2", true);
            chrDisableReloadToLocation = false;
       break;
//========================  Квест №3, захват Рока Бразильца.  =======================
       case "TakeRockBras_toRock":
            Log_QuestInfo("Корвет Рока найден, на абордаж!");
            pchar.quest.Inquisition_LuggerTimeOver.over = "yes";
            //SetNationRelation2MainCharacter(PIRATE, RELATION_ENEMY); //ссорим ГГ и пиратов.
            Island_SetReloadEnableGlobal("Maracaibo", false);
    		Group_FindOrCreateGroup("Pirate_Attack");
			Group_SetType("Pirate_Attack", "war");
            sld = GetCharacter(NPC_GenerateCharacter("RockBrasilian", "officer_15", "man", "man", 30, PIRATE, -1, true));
            sld.Dialog.Filename    = "Quest\SpaLineNpc_1.c";
        	sld.name 	= "Рок";
        	sld.lastname 	= "Бразилец";
			sld.DontRansackCaptain = true;
            FantomMakeCoolSailor(sld, SHIP_CORVETTE, "Стремительный", CANNON_TYPE_CULVERINE_LBS24, 100, 100, 100);
            FantomMakeCoolFighter(sld, 30, 90, 90, "blade28", "pistol3", 80);
            Group_AddCharacter("Pirate_Attack", "RockBrasilian");
            // ==> стравливание
			Group_SetGroupCommander("Pirate_Attack", "RockBrasilian");
			Group_SetTaskAttack("Pirate_Attack", PLAYER_GROUP);
            Group_SetPursuitGroup("Pirate_Attack", PLAYER_GROUP);
			Group_SetAddress("Pirate_Attack", "Maracaibo", "", "");
			Group_LockTask("Pirate_Attack");
            // ==> прерывания
			pchar.quest.TakeRockBras_AfterCorvetteSink.win_condition.l1 = "NPC_Death";
			pchar.quest.TakeRockBras_AfterCorvetteSink.win_condition.l1.character = "RockBrasilian";
			pchar.quest.TakeRockBras_AfterCorvetteSink.win_condition = "TakeRockBras_AfterCorvetteSink";
			pchar.quest.TakeRockBras_DieHard.win_condition.l1 = "MapEnter";
            pchar.quest.TakeRockBras_DieHard.win_condition = "TakeRockBras_DieHard";
       break;
        
        case "TakeRockBras_DieHard":
            pchar.questTemp.State = "TakeRockBras_DieHardRock";
            pchar.quest.TakeRockBras_AfterCorvetteSink.over = "yes";
            group_DeleteGroup("Pirate_Attack");
            Island_SetReloadEnableGlobal("Maracaibo", true);
            AddQuestRecord("Spa_Line_3_RockBrasilian", "3");
        break;

        case "TakeRockBras_AfterCorvetteSink":
            if (!CheckAttribute(pchar, "GenQuest.LastQuestPrisonerIdx"))
			{
				pchar.questTemp.State = "TakeRockBras_RockSink";
				pchar.quest.TakeRockBras_DieHard.over = "yes";
				Island_SetReloadEnableGlobal("Maracaibo", true);
				group_DeleteGroup("Pirate_Attack");
				AddQuestRecord("Spa_Line_3_RockBrasilian", "4");
			}
			else
			{
				characters[sti(pchar.GenQuest.LastQuestPrisonerIdx)].id = "RockBrasilian";
			}
        break;
//========================  Квест №4, Расследование убийства коменданта.  =======================
        case "Sp4Detection_Race_1":
			LAi_LocationDisableMonstersGen("Tortuga_Cave", true); //монстров не генерить
			LAi_LocationDisableOfficersGen("Shore58", true); //офицеров не пускать
            sld = GetCharacter(NPC_GenerateCharacter("AnryDEstre", "BigPirate", "man", "man", 40, FRANCE, 10, false));
        	sld.name 	= "Андре";
        	sld.lastname = "д'Эстре";
            sld.Dialog.Filename = "Quest\SpaLineNpc_1.c";
            sld.money = 21340;
            FantomMakeCoolFighter(sld, 40, 100, 100, "blade32", "pistol4", 100);
            TakeNItems(sld,"jewelry7", 1);
            ChangeCharacterAddressGroup(sld, "Tortuga_town", "goto", "goto7");
            LAi_SetHP(sld, 1, 1);
			LAi_group_MoveCharacter(sld, "FRANCE_CITIZENS");
            LAi_SetActorTypeNoGroup(sld);            
            LAi_ActorRunToLocation(sld, "reload", "basement1", "Tortuga_Cave", "goto", "goto1", "", -1);
			pchar.quest.Sp4Detection_Race_2.win_condition.l1 = "location";
			pchar.quest.Sp4Detection_Race_2.win_condition.l1.location = "Tortuga_Cave";
			pchar.quest.Sp4Detection_Race_2.win_condition = "Sp4Detection_Race_2";
        break;

        case "Sp4Detection_Race_2":
            sld = characterFromId("AnryDEstre");
            LAi_ActorRunToLocation(sld, "reload", "reload2", "Shore58", "goto", "goto15", "", -1);
			pchar.quest.Sp4Detection_Race_3.win_condition.l1 = "location";
			pchar.quest.Sp4Detection_Race_3.win_condition.l1.location = "Shore58";
			pchar.quest.Sp4Detection_Race_3.win_condition = "Sp4Detection_Race_3";
        break;
        
        case "Sp4Detection_Race_3":
            sld = characterFromId("AnryDEstre");
        	sld.DontClearDead      = true;
        	sld.SaveItemsForDead   =   true;
            LAi_SetActorType(pchar);
            LAi_ActorTurnToCharacter(sld, pchar);
            SetActorDialogAny2Pchar(sld.id, "", -1, 0.0);
		    LAi_ActorFollow(pchar, sld, "ActorDialog_Any2Pchar", 2.0);
        break;

        case "Sp4Detection_YouWin":
			LAi_LocationDisableOfficersGen("Shore58", false); //офицеров пускать
			LAi_LocationDisableMonstersGen("Tortuga_Cave", false); //монстров генерить
            pchar.questTemp.State = "Sp4Detection_DEstreIsDead";
            AddQuestRecord("Spa_Line_4_MirderDetection", "6");
        break;

//==========================   Квест №5, Разорение Ла Веги ========================
		case "Sp5LaVegaAttack_AttackGoOn":
    	    Pchar.GenQuestFort.fortCharacterIdx = GetCharacterIndex("Edward Mansfield");
    	    DeleteQuestAttribute("Union_with_Escadra");
            sld = GetCharacter(sti(Pchar.GenQuestFort.fortCharacterIdx));
            SetLocationCapturedState(sld.city + "_town", true);
            DoQuestCheckDelay("Capture_Forts", 0.5);
            Ship_NationAgressive(sld, sld);
            Log_SetStringToLog("На штурм, орлы!");
            //==> снимаем губернатора, он нестандарт.
            ChangeCharacterAddress(characterFromID("Edward Mansfield"), "None", "");
            Pchar.quest.Sp5LaVegaAttack_FightWithMayor.win_condition.l1 = "location";
            Pchar.quest.Sp5LaVegaAttack_FightWithMayor.win_condition.l1.location = "LaVega_townhall";
            Pchar.quest.Sp5LaVegaAttack_FightWithMayor.win_condition = "Sp5LaVegaAttack_FightWithMayor";
        break;

        case "Sp5LaVegaAttack_FightWithMayor":
            chrDisableReloadToLocation = true; // закрыть выход из локации
            bDisableFastReload = true; // закрыть переходы.
			//--> огонь и пламень
			locations[FindLocation("LaVega_town")].hidden_effects = true;
			locations[FindLocation("LaVega_ExitTown")].hidden_effects = true;	
			//<-- огонь и пламень
            sld = GetCharacter(NPC_GenerateCharacter("LaVega_Comendant", "Mansfield", "man", "man", 35, PIRATE, -1, true));
            sld.Dialog.Filename = "Quest\SpaLineNpc_1.c";
        	sld.name 	= "Эдвард";
        	sld.lastname = "Мэнсфилд";
	        sld.City = "LaVega";
			LAi_LoginInCaptureTown(sld, true);
            sld.DontClearDead = true; // не убирать труп через 200с
            sld.SaveItemsForDead = true; // сохранять на трупе вещи
            FantomMakeCoolFighter(sld, 35, 90, 70, BLADE_LONG, "pistol3", 50);
			ChangeCharacterAddressGroup(sld, "LaVega_townhall", "goto", "goto1");
			Statistic_AddValue(Pchar, "pir_GrabbingTown", 1); // статистика
            LAi_SetActorType(pchar);
            LAi_SetActorType(sld);
            SetActorDialogAny2Pchar(sld.id, "", 0.0, 0.0);
		    LAi_ActorFollow(pchar, sld, "ActorDialog_Any2Pchar", 0.8);
        break;

        case "Sp5LaVegaAttack_AfterFight":
            AfterTownBattle();
            chrDisableReloadToLocation = false; 
            bDisableFastReload = false;
            pchar.questTemp.State = "Sp5LaVegaAttack_WeWon";
            AddQuestRecord("Spa_Line_5_LaVegaAttack", "3");
        break;
//=================== Квест №6, перехват голландского вестового на Тортуге ===================
        case "Sp6TakeMess_waitingBoy":
            Pchar.quest.Sp6TakeMess_TalkWithBoy.win_condition.l1 = "location";
            Pchar.quest.Sp6TakeMess_TalkWithBoy.win_condition.l1.location = "Tortuga_tavern";
            Pchar.quest.Sp6TakeMess_TalkWithBoy.win_condition = "Sp6TakeMess_TalkWithBoy";
        break;

        case "Sp6TakeMess_TalkWithBoy":
            //==> Алкаш
            sld = GetCharacter(NPC_GenerateCharacter("Boozer", "citiz_3", "man", "man", 10, FRANCE, 0, false));
            sld.Dialog.Filename = "Quest\SpaLineNpc_1.c";
            LAi_SetSitType(sld);
			FreeSitLocator("Tortuga_tavern", "sit2");
			ChangeCharacterAddressGroup(sld, "Tortuga_tavern", "sit", "sit2");
            //==> Посыльный
			chrDisableReloadToLocation = true;
            sld = GetCharacter(NPC_GenerateCharacter("PortMansBoy", "shipowner_10", "man", "man", 10, FRANCE, 0, false));
            sld.Dialog.Filename = "Quest\SpaLineNpc_2.c";
         	ChangeCharacterAddressGroup(sld, "Tortuga_tavern", "goto", "goto2");
            LAi_SetActorType(sld);
			LAi_ActorDialog(sld, pchar, "", -1, 0);
        break;

		case "Sp6TakeMess_Interception":
			if (GetQuestPastTimeParam("questTemp") < 3)
			{
				Log_QuestInfo("Вперед на перехват!");
				//==> вестовой.
				sld = GetCharacter(NPC_GenerateCharacter("Messanger", "off_hol_2", "man", "man", 30, FRANCE, 30, false));
				sld.Dialog.Filename = "Quest\SpaLineNpc_1.c";
				FantomMakeCoolFighter(sld, 30, 70, 50, "blade34", "pistol5", 60);
         		ChangeCharacterAddressGroup(sld, "Tortuga_town", "reload", "reload3");
				ChangeItemDescribe("letter_1", "itmdescr_letter_1_HolLineQ7"); //даем ту же депешу, что была по голландке
				GiveItem2Character(sld,"letter_1");
				sld.money = 18430;
				sld.DontClearDead = true; // не убирать труп через 200с
				sld.SaveItemsForDead = true; // сохранять на трупе вещи
				LAi_SetCheckMinHP(sld, (LAi_GetCharacterHP(sld) - 1), false, "Sp6TakeMess_AttackMessanger");
				LAi_SetActorType(sld);
				LAi_ActorGoToLocation(sld, "reload", "reload2_back", "none", "", "", "Sp6TakeMess_MessengerOut", -1);
				LAi_Actor2WaitDialog(sld, pchar);
				LAi_group_MoveCharacter(sld, "FRANCE_CITIZENS");
				LocatorReloadEnterDisable("Tortuga_town", "gate_back", true);
				//==> добавим солдатиков в город
				for (i=1; i<=10; i++)
				{
					sld = GetCharacter(NPC_GenerateCharacter("AddSolder"+i, "Sold_fra_"+(rand(7)+1), "man", "man", 25, FRANCE, 0, true));
					sld.Dialog.Filename = "Common_Soldier.c";
					sld.city = "Tortuga";
					sld.citytype = "soldier";
					FantomMakeCoolFighter(sld, sti(pchar.rank)+6, 70, 50, BLADE_LONG, "pistol3", 60);
            		LAi_SetPatrolType(sld);
					ChangeCharacterAddressGroup(sld, "Tortuga_town", "goto",  "goto"+(rand(20)+1));         
					LAi_group_MoveCharacter(sld, "FRANCE_CITIZENS");
				}
				LAi_group_SetLookRadius("FRANCE_CITIZENS", 50);
				Pchar.quest.Sp6TakeMess_FightInPort.win_condition.l1 = "location";
				Pchar.quest.Sp6TakeMess_FightInPort.win_condition.l1.location = "Tortuga";
				Pchar.quest.Sp6TakeMess_FightInPort.win_condition = "Sp6TakeMess_FightInPort";
			}
        break;

        case "Sp6TakeMess_AttackMessanger":
			iTemp = GetCharacterIndex("Messanger");
			if (iTemp != -1)
			{			
				sld = &characters[iTemp];
				LAi_RemoveCheckMinHP(sld);
				LAi_SetWarriorType(sld);
				LAi_group_MoveCharacter(sld, "FRANCE_CITIZENS");			
				LAi_group_Attack(sld, Pchar);			
				SetNationRelation2MainCharacter(FRANCE, RELATION_ENEMY);
			}
        break;

        case "Sp6TakeMess_MessengerOut":
			ChangeCharacterAddress(characterFromID("Messanger"), "None", "");
        break;
		
		case "Sp6TakeMess_FightInPort":
			if (GetCharacterIndex("Messanger") == -1)
			{
				bDisableFastReload = false; //открываем фаст релоад
				LocatorReloadEnterDisable("Tortuga_town", "gate_back", false);
				SetNationRelation2MainCharacter(FRANCE, RELATION_ENEMY);
				Island_SetReloadEnableGlobal("Tortuga", false);
    			Group_FindOrCreateGroup("Fra_Attack");
				Group_SetType("Fra_Attack", "war");
				// ==> Французские кэпы
				for (i=1; i<=8; i++)
				{
					iTemp = FRANCE;
					switch (i)
					{
						case 1:
							ShipType = SHIP_FRIGATE;
							Model = "off_fra_1";
							Rank = 42;
							Blade = "blade33";
						break;
						case 2:
							ShipType = SHIP_BRIG;
							Model = "off_fra_1";
							Rank = 42;
							Blade = "blade33";
						break;
						case 3:
							ShipType = SHIP_FRIGATE;
							Model = "off_fra_2";
							Rank = 32;
							Blade = "blade28";
						break;
						case 4:
							ShipType = SHIP_FRIGATE;
							Model = "off_fra_1";
							Rank = 35;
							Blade = "blade24";
						break;
						case 5:
							ShipType = SHIP_CORVETTE;
							Model = "off_fra_1";
							Rank = 35;
							Blade = "blade34";
						break;
						case 6:
							ShipType = SHIP_BRIG;
							Model = "off_hol_2";
							Rank = 29;
							Blade = "blade26";
							//iTemp = HOLLAND;
						break;
						case 7:
							ShipType = SHIP_PINNACE;
							Model = "off_fra_2";
							Rank = 33;
							Blade = "blade28";
						break;
						case 8:
							ShipType = SHIP_CORVETTE;
							Model = "off_fra_1";
							Rank = 39;
							Blade = "blade32";
						break;
					}
					sld = GetCharacter(NPC_GenerateCharacter("CaptainAttack_"+i, Model, "man", "man", Rank, iTemp, -1, true));
					FantomMakeCoolSailor(sld, ShipType, "", CANNON_TYPE_CANNON_LBS24, 80, 70, 90);
					FantomMakeCoolFighter(sld, Rank, 80, 60, Blade, "pistol3", 75);
    				Group_AddCharacter("Fra_Attack", "CaptainAttack_"+i);
				}
				// ==> стравливание
				Group_SetGroupCommander("Fra_Attack", "CaptainAttack_1");
				Group_SetTaskAttack("Fra_Attack", PLAYER_GROUP);
				Group_SetPursuitGroup("Fra_Attack", PLAYER_GROUP);
				Group_SetAddress("Fra_Attack", "Tortuga", "", "");
				Group_LockTask("Fra_Attack");
				pchar.quest.Sp6TakeMess_OpenTortuga.win_condition.l1 = "MapEnter";
				pchar.quest.Sp6TakeMess_OpenTortuga.win_condition = "Sp6TakeMess_OpenTortuga";
			}
			else characters[GetCharacterIndex("Messanger")].lifeDay = 0;
        break;

		case "Sp6TakeMess_OpenTortuga":
			Island_SetReloadEnableGlobal("Tortuga", true);		
        break;
  
		case "Sp6TakeMess_WaitMessanger":
            chrDisableReloadToLocation = true; // закрыть выход из локации
            bDisableFastReload = true; // закрыть переходы.
			DoQuestCheckDelay("Sp6TakeMess_2WaitMessanger", 10.0);			
        break;

		case "Sp6TakeMess_2WaitMessanger":
            sld = GetCharacter(NPC_GenerateCharacter("Messanger", "off_hol_2", "man", "man", 40, FRANCE, 0, false));
            sld.Dialog.Filename = "Quest\SpaLineNpc_1.c";
            FantomMakeCoolFighter(sld, 40, 95, 80, "blade34", "pistol5", 100);
         	ChangeCharacterAddressGroup(sld, "Tortuga_tavern_upstairs", "reload", "reload1_back");
            ChangeItemDescribe("letter_1", "itmdescr_letter_1_HolLineQ7"); //даем ту же депешу, что была по голландке
            GiveItem2Character(sld,"letter_1");
            sld.money = 18430;
            sld.DontClearDead = true; // не убирать труп через 200с
            sld.SaveItemsForDead = true; // сохранять на трупе вещи
	        LAi_SetActorType(pchar);
            LAi_SetActorType(sld);
            SetActorDialogAny2Pchar(sld.id, "", -1, 0.0);
		    LAi_ActorFollow(pchar, sld, "ActorDialog_Any2Pchar", 0.0);
        break;
//=================== Квест №7, спасти Пардаля ===================
        case "Sp7SavePardal_FightNearAntigua":
            Log_QuestInfo("Спасаем Пардаля, он не должен быть потоплен.");
			PChar.questTemp.SLQ7_Diffrent = 1; //Моисею Воклейну в каюту
            Island_SetReloadEnableGlobal("Antigua", false);
			// ==> Пардаль 
            Group_SetType("Quest_Group", "war");
    		Group_FindOrCreateGroup("Quest_Group");
            for (i=1; i<=3; i++)
            {			
                switch (i)
                {			
                    case 1:
                        ShipType = SHIP_BATTLESHIP;
                        Model = "SpaOfficer1";
                        Rank = 30;
                        Blade = "blade26";
						iTemp = CANNON_TYPE_CANNON_LBS32;
                    break;
                    case 2:
                        ShipType = SHIP_GALEON_H;
                        Model = "off_spa_1";
                        Rank = 27;
                        Blade = "blade26";
						iTemp = CANNON_TYPE_CANNON_LBS24;
                    break;
                    case 3:
                        ShipType = SHIP_GALEON_L;
                        Model = "off_spa_2";
                        Rank = 24;
                        Blade = "blade26";
						iTemp = CANNON_TYPE_CANNON_LBS16;
                    break;
				}
                sld = GetCharacter(NPC_GenerateCharacter("OurCaptain_"+i, Model, "man", "man", Rank, SPAIN, 0, true));
				sld.ShipEnemyDisable = true; //при попадании не враждебен
				FantomMakeCoolSailor(sld, ShipType, "", iTemp, 60, 50, 50);
                FantomMakeCoolFighter(sld, Rank, 70, 40, Blade, "pistol3", 50);
				if (sld.id == "OurCaptain_1")
				{
					sld.DeckDialogNode = "First time";
					sld.Dialog.Filename = "Quest\SpaLineNpc_1.c";
        			sld.name 	= "Маноэль";
        			sld.lastname = "Риверо Пардаль";
        			sld.Abordage.Enable = false; // НЕЛЬЗЯ!
					sld.Ship.Name = "Эскадо";
					LAi_SetStayTypeNoGroup(sld);
				}
    			Group_AddCharacter("Quest_Group", "OurCaptain_"+i);
			}
            Group_SetGroupCommander("Quest_Group", "OurCaptain_1"); //это Пардаль
			Group_SetPursuitGroup("Quest_Group", PLAYER_GROUP);
			Group_SetAddress("Quest_Group", "Antigua", "", "");
			// ==> Флибустьеры
    		Group_FindOrCreateGroup("Pir_Attack");
			Group_SetType("Pir_Attack", "war");
            for (i=1; i<=7; i++)
            {
                switch (i)
                {
                    case 1:
                        ShipType = SHIP_BATTLESHIP;
                        Rank = 35;
                        Blade = "blade33";
						iTemp = CANNON_TYPE_CULVERINE_LBS32;
                    break;
                    case 2:
                        ShipType = SHIP_FRIGATE;
                        Rank = 31;
                        Blade = "blade33";
						iTemp = CANNON_TYPE_CANNON_LBS24;
                    break;
                    case 3:
                        ShipType = SHIP_WARSHIP;
                        Rank = 33;
                        Blade = "blade28";
						iTemp = CANNON_TYPE_CANNON_LBS32;
                    break;
                    case 4:
                        ShipType = SHIP_CORVETTE;
                        Rank = 29;
                        Blade = "blade24";
						iTemp = CANNON_TYPE_CANNON_LBS24;
                    break;
                    case 5:
                        ShipType = SHIP_BATTLESHIP;
                        Rank = 34;
                        Blade = "blade34";
						iTemp = CANNON_TYPE_CANNON_LBS32;
                    break;
                    case 6:
                        ShipType = SHIP_LINESHIP;
                        Rank = 32;
                        Blade = "blade26";
						iTemp = CANNON_TYPE_CULVERINE_LBS32;
                    break;
                    case 7:
                        ShipType = SHIP_CORVETTE;
                        Rank = 28;
                        Blade = "blade34";
						iTemp = CANNON_TYPE_CULVERINE_LBS24;
                    break;
				}
                sld = GetCharacter(NPC_GenerateCharacter("CaptainAttack_"+i, "officer_"+(rand(19)+1), "man", "man", Rank, PIRATE, 0, true));
                FantomMakeCoolSailor(sld, ShipType, "", iTemp, 100, 95, 95);
                FantomMakeCoolFighter(sld, Rank, 100, 90, Blade, "pistol3", 150);
				if (sld.id == "CaptainAttack_1")
				{
        			sld.name 	= "Моисей";
        			sld.lastname = "Воклейн";
					sld.Dialog.Filename = "Quest\SpaLineNpc_2.c";
					sld.Ship.Name = "Орлеан";
					LAi_SetStayTypeNoGroup(sld);
				}
    			Group_AddCharacter("Pir_Attack", "CaptainAttack_"+i);
            }            
			Group_SetGroupCommander("Pir_Attack", "CaptainAttack_1");
            Group_SetPursuitGroup("Pir_Attack", PLAYER_GROUP);
			Group_SetAddress("Pir_Attack", "Antigua", "", "");
			// ==> стравливание
			Group_SetTaskAttack("Pir_Attack", "Quest_Group");
            Group_SetTaskAttack("Quest_Group", "Pir_Attack");
            Group_LockTask("Quest_Group");
			//Group_LockTask("Pir_Attack");			
			// ==> прерывание на убиение эскадры
            pchar.quest.Sp7SavePardal_AfterBattle.win_condition.l1 = "Group_Death";
			pchar.quest.Sp7SavePardal_AfterBattle.win_condition.l1.group = "Pir_Attack";
			pchar.quest.Sp7SavePardal_AfterBattle.win_condition = "Sp7SavePardal_AfterBattle";
    		pchar.quest.Sp7SavePardal_NotSavedPardal.win_condition.l1 = "NPC_Death";
			pchar.quest.Sp7SavePardal_NotSavedPardal.win_condition.l1.character = "OurCaptain_1";
			pchar.quest.Sp7SavePardal_NotSavedPardal.win_condition = "Sp7SavePardal_NotSavedPardal";
			pchar.quest.Sp7SavePardal_DieHard.win_condition.l1 = "MapEnter";
            pchar.quest.Sp7SavePardal_DieHard.win_condition = "Sp7SavePardal_DieHard";
        break;

        case "Sp7SavePardal_AfterBattle":
			Log_QuestInfo("Флибустьерскся эскадра эскадра разбита.");
            pchar.questTemp.State = "Sp7SavePardal_GoodWork";
            AddQuestRecord("Spa_Line_7_SavePardal", "2");
            Island_SetReloadEnableGlobal("Antigua", true);
			pchar.quest.Sp7SavePardal_NotSavedPardal.over = "yes";
        break;

        case "Sp7SavePardal_DieHard":
			group_DeleteGroup("Pir_Attack");
			group_DeleteGroup("Quest_Group");
            if (pchar.questTemp.State != "Sp7SavePardal_PardalIsSink" && pchar.questTemp.State != "Sp7SavePardal_GoodWork" && pchar.questTemp.State != "Sp7SavePardal_2GoodWork")
			{
				Log_QuestInfo("Свалил с поля боя, очень плохо.");
				pchar.questTemp.State = "Sp7SavePardal_DieHard";
				AddQuestRecord("Spa_Line_7_SavePardal", "3");
				Island_SetReloadEnableGlobal("Antigua", true);
				pchar.quest.Sp7SavePardal_AfterBattle.over = "yes";
				pchar.quest.Sp7SavePardal_NotSavedPardal.over = "yes";
			}
        break;

        case "Sp7SavePardal_NotSavedPardal":
            pchar.quest.Sp7SavePardal_AfterBattle.over = "yes";
            Island_SetReloadEnableGlobal("Antigua", true);
            AddQuestRecord("Spa_Line_7_SavePardal", "4");
            pchar.questTemp.State = "Sp7SavePardal_PardalIsSink";
        break;
//=================== Квест №8, спасти Куману ===================
        case "Sp8SaveCumana_RescueTimeOver":
            Log_QuestInfo("Опаздал. Кумана разорена.");
            pchar.questTemp.State = "Sp8SaveCumana_YouLate";
            AddQuestRecord("Spa_Line_8_SaveCumana", "2");
            pchar.quest.Sp8SaveCumana_Battle.over = "yes";
			SetLocationCapturedState("Cumana_town", true);
			//TO_DO партиклы в куману
        break;

        case "Sp8SaveCumana_Battle":
            Log_QuestInfo("Пиратская эскадра установлена.");
            pchar.quest.Sp8SaveCumana_RescueTimeOver.over = "yes";
			PChar.questTemp.SLQ8_Diffrent = 1; //капитану Анселю в каюту
            Island_SetReloadEnableGlobal("Cumana", false);
    		Group_FindOrCreateGroup("Pir_Attack");
			Group_SetType("Pir_Attack", "war");
            // ==> Пиратские кэпы
            for (i=1; i<=8; i++)
            {
                switch (i)
                {
					case 1:
                        ShipType = SHIP_BATTLESHIP;
                        Rank = 42;
                        Blade = "blade33";
						iTemp = CANNON_TYPE_CULVERINE_LBS32;
                    break;
                    case 2:
                        ShipType = SHIP_FRIGATE;
                        Rank = 34;
                        Blade = "blade33";
						iTemp = CANNON_TYPE_CANNON_LBS24;
                    break;
                    case 3:
                        ShipType = SHIP_WARSHIP;
                        Rank = 36;
                        Blade = "blade28";
						iTemp = CANNON_TYPE_CULVERINE_LBS32;
                    break;
                    case 4:
                        ShipType = SHIP_CORVETTE;
                        Rank = 32;
                        Blade = "blade24";
						iTemp = CANNON_TYPE_CULVERINE_LBS24;
                    break;
                    case 5:
                        ShipType = SHIP_BATTLESHIP;
                        Rank = 37;
                        Blade = "blade34";
						iTemp = CANNON_TYPE_CANNON_LBS32;
                    break;
                    case 6:
                        ShipType = SHIP_LINESHIP;
                        Rank = 33;
                        Blade = "blade26";
						iTemp = CANNON_TYPE_CANNON_LBS32;
                    break;
                    case 7:
                        ShipType = SHIP_CORVETTE;
                        Rank = 33;
                        Blade = "blade28";
						iTemp = CANNON_TYPE_CULVERINE_LBS24;
                    break;
                    case 8:
                        ShipType = SHIP_FRIGATE;
                        Model = "off_spa_1";
                        Rank = 35;
                        Blade = "blade32";
						iTemp = CANNON_TYPE_CULVERINE_LBS24;
                    break;
                }
                sld = GetCharacter(NPC_GenerateCharacter("CaptainAttack_"+i, "officer_"+(rand(19)+1), "man", "man", Rank, PIRATE, 0, true));
                FantomMakeCoolSailor(sld, ShipType, "", iTemp, 100, 95, 95);
                FantomMakeCoolFighter(sld, Rank, 100, 90, Blade, "pistol3", 150);
				if (sld.id == "CaptainAttack_1")
				{
        			sld.name 	= "капитан";
        			sld.lastname = "Ансель";
					sld.Dialog.Filename = "Quest\SpaLineNpc_2.c";
					sld.Ship.Name = "Зевс";
					LAi_SetStayTypeNoGroup(sld);
				}
    			Group_AddCharacter("Pir_Attack", "CaptainAttack_"+i);
            }
            // ==> стравливание
			Group_SetGroupCommander("Pir_Attack", "CaptainAttack_1");
			Group_SetTaskAttack("Pir_Attack", PLAYER_GROUP);
            Group_SetPursuitGroup("Pir_Attack", PLAYER_GROUP);
			Group_SetAddress("Pir_Attack", "Cumana", "", "");
			Group_LockTask("Pir_Attack");
            pchar.quest.Sp8SaveCumana_AfterBattle.win_condition.l1 = "Group_Death";
			pchar.quest.Sp8SaveCumana_AfterBattle.win_condition.l1.group = "Pir_Attack";
			pchar.quest.Sp8SaveCumana_AfterBattle.win_condition = "Sp8SaveCumana_AfterBattle";
			pchar.quest.Sp8SaveCumana_DieHard.win_condition.l1 = "MapEnter";
            pchar.quest.Sp8SaveCumana_DieHard.win_condition = "Sp8SaveCumana_DieHard";
        break;

        case "Sp8SaveCumana_AfterBattle":
			Log_QuestInfo("Пиратская эскадра разбита.");
            pchar.questTemp.State = "Sp8SaveCumana_GoodWork";
            AddQuestRecord("Spa_Line_8_SaveCumana", "3");
            group_DeleteGroup("Pir_Attack");
            Island_SetReloadEnableGlobal("Cumana", true);
            pchar.quest.Sp8SaveCumana_DieHard.over = "yes";
        break;

        case "Sp8SaveCumana_DieHard":
            Log_QuestInfo("Свалил с поля боя, очень плохо.");
            pchar.questTemp.State = "Sp8SaveCumana_DieHard";
            AddQuestRecord("Spa_Line_8_SaveCumana", "4");
            group_DeleteGroup("Pir_Attack");
            Island_SetReloadEnableGlobal("Cumana", true);
            pchar.quest.Sp8SaveCumana_AfterBattle.over = "yes";
			SetLocationCapturedState("Cumana_town", true);
			//TO_DO партиклы в куману
        break;
//=================== Квест №9, сопровождение четырех галеонов ===================
        case "Sp9SaveCumana_toCaimanBattle":
            Log_QuestInfo("Засада пиратов, к бою.");
			pchar.questTemp.State = "Sp9SaveCumana_BackHavana";
            Island_SetReloadEnableGlobal("Caiman", false);
    		Group_FindOrCreateGroup("Pir_Attack");
			Group_SetType("Pir_Attack", "war");
            // ==> Пиратские кэпы
            for (i=1; i<=5; i++)
            {
                switch (i)
                {
                    case 1:
                        ShipType = SHIP_LINESHIP;
                        Rank = 32;
                        Blade = "blade33";
						iTemp = CANNON_TYPE_CANNON_LBS32;
                    break;
                    case 2:
                        ShipType = SHIP_GALEON_H;
                        Rank = 26;
                        Blade = "blade28";
						iTemp = CANNON_TYPE_CULVERINE_LBS24;
                    break;
                    case 3:
                        ShipType = SHIP_CARAVEL;
                        Rank = 25;
                        Blade = "blade25";
						iTemp = CANNON_TYPE_CANNON_LBS24;
                    break;
                    case 4:
                        ShipType = SHIP_GALEON_H;
                        Rank = 27;
                        Blade = "blade28";
						iTemp = CANNON_TYPE_CANNON_LBS32;
                    break;
                    case 5:
                        ShipType = SHIP_WARSHIP;
                        Rank = 30;
                        Blade = "blade33";
						iTemp = CANNON_TYPE_CULVERINE_LBS32;
                    break;
                }
                sld = GetCharacter(NPC_GenerateCharacter("CaptainAttack_"+i, "officer_"+(rand(19)+1), "man", "man", Rank, PIRATE, 0, true));
                FantomMakeCoolSailor(sld, ShipType, "", iTemp, 90, 80, 80);
                FantomMakeCoolFighter(sld, Rank, 90, 70, Blade, "pistol3", 75);
    			Group_AddCharacter("Pir_Attack", "CaptainAttack_"+i);
            }
            // ==> стравливание
			Group_SetGroupCommander("Pir_Attack", "CaptainAttack_1");
			Group_SetTaskAttack("Pir_Attack", PLAYER_GROUP);
            Group_SetPursuitGroup("Pir_Attack", PLAYER_GROUP);
			Group_SetAddress("Pir_Attack", "Caiman", "", "");
			Group_LockTask("Pir_Attack");
            // ==> прерывание на убиение эскадры
            pchar.quest.Sp9SaveCumana_AfterBattle.win_condition.l1 = "Group_Death";
			pchar.quest.Sp9SaveCumana_AfterBattle.win_condition.l1.group = "Pir_Attack";
			pchar.quest.Sp9SaveCumana_AfterBattle.win_condition = "Sp9SaveCumana_AfterBattle";
			pchar.quest.Sp9SaveCumana_DieHard.win_condition.l1 = "MapEnter";
            pchar.quest.Sp9SaveCumana_DieHard.win_condition = "Sp9SaveCumana_DieHard";
        break;

        case "Sp9SaveCumana_AfterBattle":
            Island_SetReloadEnableGlobal("Caiman", true);
            pchar.quest.Sp9SaveCumana_DieHard.over = "yes";
			group_DeleteGroup("Pir_Attack");
			//--> сколько у нас флейтов из компаньонов
			iTemp = 0;
            for (i=1; i<=COMPANION_MAX; i++)
            {
                ShipType = GetCompanionIndex(pchar,i);
                if(ShipType != -1)
                {
                	sld = GetCharacter(ShipType);
                    ShipType = sti(sld.ship.type);
                    Rank = sti(RealShips[ShipType].basetype);
                    if (Rank == 9) iTemp++;
                }
            }
			//--> результат в СЖ
			if (iTemp < 4)
            {
                if (iTemp == 1)
                {
                    AddQuestRecord("Spa_Line_9_FourGolgGaleons", "3");
                }
                else
                {
                    AddQuestRecord("Spa_Line_9_FourGolgGaleons", "4");
                    AddQuestUserData("Spa_Line_9_FourGolgGaleons", "QtyGaleons", iTemp);
                }
            }
            else AddQuestRecord("Spa_Line_9_FourGolgGaleons", "5");
        break;
        
        case "Sp9SaveCumana_DieHard":
            pchar.quest.Sp9SaveCumana_AfterBattle.over = "yes";
            AddQuestRecord("Spa_Line_9_FourGolgGaleons", "6");
            Island_SetReloadEnableGlobal("Caiman", true);
            group_DeleteGroup("Pir_Attack");
            for (i=1; i<=4; i++)
            {
                sld = characterFromID("Captain_"+i);
                if (sld.id != "none")
				{
					RemoveCharacterCompanion(pchar, sld);
					sld.LifeDay = 0;
				}
            }
        break;
//=================== Квест №10, Защита Маракайбо  ===================
        case "Sp10Maracaibo_TalkWithOfficer":
            Log_QuestInfo("Столкновение с офицером на выходе из резиденции.");
            sld = GetCharacter(NPC_GenerateCharacter("SpaTalkOfficer", "off_spa_2", "man", "man", 20, SPAIN, 0, true));
            sld.Dialog.Filename = "Quest\SpaLineNpc_1.c";
         	ChangeCharacterAddressGroup(sld, "Maracaibo_town", "goto", "goto8");
            LAi_SetActorType(pchar);
            LAi_SetActorType(sld);
            LAi_ActorTurnToCharacter(sld, pchar);
            SetActorDialogAny2Pchar(sld.id, "", -1, 0.0);
		    LAi_ActorFollow(pchar, sld, "ActorDialog_Any2Pchar", 0.0);
        break;

        case "Sp10Maracaibo_Battle":
            Log_QuestInfo("Пиратская эскадра установлена.");
            Island_SetReloadEnableGlobal("Maracaibo", false);
    		Group_FindOrCreateGroup("Pir_Attack");
			Group_SetType("Pir_Attack", "war");
            // ==> Пиратские кэпы
            for (i=1; i<=8; i++)
            {
                switch (i)
                {
					case 1:
                        ShipType = SHIP_MANOWAR;
                        Rank = 45;
                        Blade = "blade28";
						iTemp = CANNON_TYPE_CULVERINE_LBS32;
                    break;
                    case 2:
                        ShipType = SHIP_FRIGATE;
                        Rank = 34;
                        Blade = "blade33";
						iTemp = CANNON_TYPE_CANNON_LBS24;
                    break;
                    case 3:
                        ShipType = SHIP_WARSHIP;
                        Rank = 37;
                        Blade = "blade34";
						iTemp = CANNON_TYPE_CULVERINE_LBS32;
                    break;
                    case 4:
                        ShipType = SHIP_CORVETTE;
                        Rank = 32;
                        Blade = "blade33";
						iTemp = CANNON_TYPE_CULVERINE_LBS24;
                    break;
                    case 5:
                        ShipType = SHIP_BATTLESHIP;
                        Rank = 39;
                        Blade = "blade28";
						iTemp = CANNON_TYPE_CULVERINE_LBS32;
                    break;
                    case 6:
                        ShipType = SHIP_LINESHIP;
                        Rank = 36;
                        Blade = "blade32";
						iTemp = CANNON_TYPE_CANNON_LBS32;
                    break;
                    case 7:
                        ShipType = SHIP_MANOWAR;
                        Rank = 42;
                        Blade = "topor2";
						iTemp = CANNON_TYPE_CANNON_LBS42;
                    break;
                    case 8:
                        ShipType = SHIP_FRIGATE;
                        Model = "off_spa_1";
                        Rank = 35;
                        Blade = "blade33";
						iTemp = CANNON_TYPE_CULVERINE_LBS24;
                    break;
                }
                sld = GetCharacter(NPC_GenerateCharacter("CaptainAttack_"+i, "officer_"+(rand(19)+1), "man", "man", Rank, PIRATE, 0, true));
                FantomMakeCoolSailor(sld, ShipType, "", iTemp, 100, 95, 95);
                FantomMakeCoolFighter(sld, Rank, 100, 90, Blade, "pistol3", 150);
    			Group_AddCharacter("Pir_Attack", "CaptainAttack_"+i);
            }
            // ==> стравливание
			Group_SetGroupCommander("Pir_Attack", "CaptainAttack_1");
			Group_SetTaskAttack("Pir_Attack", PLAYER_GROUP);
            Group_SetPursuitGroup("Pir_Attack", PLAYER_GROUP);
			Group_SetAddress("Pir_Attack", "Maracaibo", "", "");
			Group_LockTask("Pir_Attack");
            pchar.quest.Sp10Maracaibo_AfterBattle.win_condition.l1 = "Group_Death";
			pchar.quest.Sp10Maracaibo_AfterBattle.win_condition.l1.group = "Pir_Attack";
			pchar.quest.Sp10Maracaibo_AfterBattle.win_condition = "Sp10Maracaibo_AfterBattle";
			pchar.quest.Sp10Maracaibo_DieHard.win_condition.l1 = "MapEnter";
            pchar.quest.Sp10Maracaibo_DieHard.win_condition = "Sp10Maracaibo_DieHard";
        break;

        case "Sp10Maracaibo_AfterBattle":
			Log_QuestInfo("Пиратская эскадра разбита.");
            pchar.questTemp.State = "Sp10Maracaibo_GoodWork";
            AddQuestRecord("Spa_Line_10_Maracaibo", "3");
            group_DeleteGroup("Pir_Attack");
            Island_SetReloadEnableGlobal("Maracaibo", true);
            pchar.quest.Sp10Maracaibo_DieHard.over = "yes";
        break;

        case "Sp10Maracaibo_DieHard":
            Log_QuestInfo("Свалил с поля боя, линейке конец.");
            pchar.questTemp.State = "Sp10Maracaibo_DieHard";
            AddQuestRecord("Spa_Line_10_Maracaibo", "4");
            group_DeleteGroup("Pir_Attack");
            Island_SetReloadEnableGlobal("Maracaibo", true);
            pchar.quest.Sp10Maracaibo_AfterBattle.over = "yes";
            SetCaptureTownByNation("Maracaibo", ENGLAND);
            UpdateRelations();
            RefreshBattleInterface();
        break;
/////////////////////////////////////////////////////////////////////////////////////////////////////////
////  END Испанская линейка
/////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////
////  START Французская линейка
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//=================== Квест №1, доставка Пьера Леграна до Мартиники  ===================
        case "Fr1Legran_intoLeMaren":			
            GetCharacterPos(pchar, &locx, &locy, &locz);
			sld = characterFromId("Legran");
			LAi_SetActorType(sld);
			ChangeCharacterAddressGroup(sld, "Shore39", "goto",  LAi_FindNearestFreeLocator("goto", locx, locy, locz));
            for (i=1; i<=6; i++)
            {
                sld = GetCharacter(NPC_GenerateCharacter("FraBandit"+i, "Pirate_"+(rand(9)+1), "man", "man", 20, PIRATE, 0, true));
                sld.Dialog.Filename = "Quest\FraLineNpc_2.c";
				FantomMakeCoolFighter(sld, 20, 65, 30, BLADE_LONG, "pistol2", 40);
            	LAi_SetWarriorType(sld);
                LAi_group_MoveCharacter(sld, "EnemyFight");
               	ChangeCharacterAddressGroup(sld, "Shore39", "goto", LAi_FindFarLocator("goto", locx, locy, locz));
            }
			chrDisableReloadToLocation = true;
			LAi_LocationFightDisable(&Locations[FindLocation("Shore39")], true);
            LAi_SetActorType(sld);
			LAi_ActorDialog(sld, pchar, "", -1, 0);
        break;

        case "Fr1Legran_afterFightBandits":
			DeleteAttribute(&locations[FindLocation("Shore39")], "DisableEncounters"); //энкаутеры вернем
			chrDisableReloadToLocation = false;
			sld = characterFromId("Legran");
			if (!LAi_IsDead(sld))
			{
				pchar.questTemp.State = "Fr1Legran_afterFightBand";
				LAi_SetActorType(pchar);
				LAi_SetActorType(sld);
				SetActorDialogAny2Pchar(sld.id, "", -1, 0.0);
				LAi_ActorFollow(pchar, sld, "ActorDialog_Any2Pchar", 1.0);
			}
			else
			{
				pchar.questTemp.State = "Fr1Legran_LegranIsDead";
				AddQuestRecord("Fra_Line_1_LegranDis", "5");
			}
        break;
//=================== Квест №2, доставка письма в Кюрасао  ===================
        case "Fr2Letter_intoPrison_1":		
			sld = GetCharacter(NPC_GenerateCharacter("HolPrisoner", "citiz_5", "man", "man", 15, HOLLAND, 0, false));
            sld.Dialog.Filename = "Quest\FraLineNpc_2.c";
            LAi_SetStayTypeNoGroup(sld);
            LAi_LoginInCaptureTown(sld, true);
            ChangeCharacterAddressGroup(sld, "Villemstad_prison", "goto", "goto13");
            LAi_SetActorType(sld);
            LAi_ActorGoToLocator(sld, "goto", "goto23", "Fr2Letter_intoPrison_2", -1);
        break;

        case "Fr2Letter_intoPrison_2":
			pchar.questTemp.State = "Fr2Letter_PrisonTalk";
            sld = characterFromID("HolPrisoner");
			LAi_SetActorType(pchar);
			LAi_ActorTurnToCharacter(sld, pchar);
            SetActorDialogAny2Pchar(sld.id, "", 0.0, 0.0);
		    LAi_ActorFollow(pchar, sld, "ActorDialog_Any2Pchar", 0.0);
        break;

		case "Fr2Letter_intoPrison_3":
            WaitDate("", 0, 0, 1, 0, 0); //крутим день
			SetCurrentTime(9, 20);
        	RecalculateJumpTable();
            SetLaunchFrameFormParam("Прошел один день...", "", 0, 6);
            LaunchFrameForm();
			npchar = characterFromId("hol_guber");
			sld = GetCharacter(NPC_GenerateCharacter("GoverFantom", npchar.model, "man", npchar.model.animation, 15, HOLLAND, 0, false));
			sld.name  = "Петер";
   			sld.lastname = "Стэвезант";
			sld.Dialog.Filename = "Quest\FraLineNpc_2.c";
            LAi_SetStayTypeNoGroup(sld);
            LAi_LoginInCaptureTown(sld, true);
            ChangeCharacterAddressGroup(sld, "Villemstad_prison", "goto", "goto13");
            LAi_SetActorType(sld);
            LAi_ActorGoToLocator(sld, "goto", "goto23", "Fr2Letter_intoPrison_4", -1);
        break;

        case "Fr2Letter_intoPrison_4":
			pchar.questTemp.State = "Fr2Letter_VisitGovernor";
            sld = characterFromID("GoverFantom");
			LAi_SetActorType(pchar);
            LAi_ActorTurnToCharacter(sld, pchar);
            SetActorDialogAny2Pchar(sld.id, "", 0, 0.0);
		    LAi_ActorFollow(pchar, sld, "ActorDialog_Any2Pchar", 0.0);
        break;

        case "Fr2Letter_OutTavern_1":
			if (!CheckAttribute(pchar, "questTemp.Out"))
			{
				chrDisableReloadToLocation = false;
				bDisableFastReload = true; 
				DoQuestCheckDelay("TalkSelf_Quest", 0.2); //диалог сам-на-сам
				pchar.quest.Fr2Letter_shore22.win_condition.l1 = "locator";
				pchar.quest.Fr2Letter_shore22.win_condition.l1.location = "Shore22";
				pchar.quest.Fr2Letter_shore22.win_condition.l1.locator_group = "quest";
				pchar.quest.Fr2Letter_shore22.win_condition.l1.locator = "detector1";
				pchar.quest.Fr2Letter_shore22.function = "Fr2Letter_shore22";
			}
			for (i=1; i<=2; i++)
			{
				sld = characterFromId("Bandit_"+i);
				LAi_SetActorTypeNoGroup(sld);
				sTemp = "goto14";
				if (i==1) sTemp = "goto13";
				ChangeCharacterAddressGroup(sld, "Shore22", "goto", sTemp);
			}
        break;

        case "Fr2Letter_OutTavern_2":
			bDisableFastReload = false; 
			if (GetQuestPastMinutesParam("questTemp") < 5) 
			{
				Log_QuestInfo("Успел выбежать из таверны.");
				for (i=1; i<=2; i++)
				{
					sld = characterFromId("Bandit_"+i);
					ChangeCharacterAddressGroup(sld, "Villemstad_town", "officers", "houseS1_2");
					LAi_SetActorTypeNoGroup(sld);
					LAi_ActorRunToLocation(sld, "reload", "gate_back", "none", "", "", "Fr2Letter_OutTavern_1", -1);
				}
				Pchar.quest.Fr2Letter_OutTavern_3.win_condition.l1 = "location";
				Pchar.quest.Fr2Letter_OutTavern_3.win_condition.l1.location = "Villemstad_ExitTown";
				Pchar.quest.Fr2Letter_OutTavern_3.win_condition = "Fr2Letter_OutTavern_3";
			}
			else Log_QuestInfo("Не успел выбежать из таверны, рулим в бухту Пальмовый берег.");	
        break;

        case "Fr2Letter_OutTavern_3":
			if (GetQuestPastMinutesParam("questTemp") < 13) 
			{
				Log_QuestInfo("Успел выбежать.");
				for (i=1; i<=2; i++)
				{
					sld = characterFromId("Bandit_"+i);
					ChangeCharacterAddressGroup(sld, "Villemstad_ExitTown", "goto", "goto11");
					LAi_SetActorTypeNoGroup(sld);
					LAi_ActorRunToLocation(sld, "reload", "reload1_back", "none", "", "", "Fr2Letter_OutTavern_1", -1);
				}
				Pchar.quest.Fr2Letter_OutTavern_4.win_condition.l1 = "location";
				Pchar.quest.Fr2Letter_OutTavern_4.win_condition.l1.location = "Curacao_jungle_01";
				Pchar.quest.Fr2Letter_OutTavern_4.win_condition = "Fr2Letter_OutTavern_4";
				//SaveCurrentQuestDateParam("questTemp");
			}
			else Log_QuestInfo("Не успел, рулим в бухту Пальмовый берег.");	
        break;

        case "Fr2Letter_OutTavern_4":
			if (GetQuestPastMinutesParam("questTemp") < 19) 
			{
				Log_QuestInfo("Успел выбежать.");
				for (i=1; i<=2; i++)
				{
					sld = characterFromId("Bandit_"+i);
					ChangeCharacterAddressGroup(sld, "Curacao_jungle_01", "goto", "goto5");
					LAi_SetActorTypeNoGroup(sld);
					LAi_ActorRunToLocation(sld, "reload", "reload1_back", "none", "", "", "Fr2Letter_OutTavern_1", -1);

				}
				Pchar.quest.Fr2Letter_OutTavern_5.win_condition.l1 = "location";
				Pchar.quest.Fr2Letter_OutTavern_5.win_condition.l1.location = "Curacao_jungle_02";
				Pchar.quest.Fr2Letter_OutTavern_5.win_condition = "Fr2Letter_OutTavern_5";
				//SaveCurrentQuestDateParam("questTemp");
			}
			else Log_QuestInfo("Не успел, рулим в бухту Пальмовый берег.");	
        break;

        case "Fr2Letter_OutTavern_5":
			if (GetQuestPastMinutesParam("questTemp") < 24) 
			{
				Log_QuestInfo("Успел выбежать.");
				for (i=1; i<=2; i++)
				{
					sld = characterFromId("Bandit_"+i);
					ChangeCharacterAddressGroup(sld, "Curacao_jungle_02", "goto", "goto4");
					LAi_SetActorTypeNoGroup(sld);
					LAi_ActorRunToLocation(sld, "reload", "reload1_back", "none", "", "", "Fr2Letter_OutTavern_1", -1);
				}
				Pchar.quest.Fr2Letter_OutTavern_6.win_condition.l1 = "location";
				Pchar.quest.Fr2Letter_OutTavern_6.win_condition.l1.location = "Curacao_jungle_03";
				Pchar.quest.Fr2Letter_OutTavern_6.win_condition = "Fr2Letter_OutTavern_6";
				//SaveCurrentQuestDateParam("questTemp");
			}
			else Log_QuestInfo("Не успел, рулим в бухту Пальмовый берег.");	
        break;

        case "Fr2Letter_OutTavern_6":
			if (GetQuestPastMinutesParam("questTemp") < 32) 
			{
				Log_QuestInfo("Успел выбежать.");
				for (i=1; i<=2; i++)
				{
					sld = characterFromId("Bandit_"+i);
					ChangeCharacterAddressGroup(sld, "Curacao_jungle_03", "goto", "goto7");
					LAi_SetActorTypeNoGroup(sld);
					LAi_ActorRunToLocation(sld, "reload", "reload2_back", "none", "", "", "Fr2Letter_OutTavern_1", -1);
				}
			}
			else Log_QuestInfo("Не успел, рулим в бухту Пальмовый берег.");	
        break;

        case "Fr2Letter_FightWithGaleon":
			Log_QuestInfo("Галеон найден, на абордаж!");
			Group_FindOrCreateGroup("Spa_Attack");
			Group_SetType("Spa_Attack", "war");
			sld = GetCharacter(NPC_GenerateCharacter("CaptainAttack_1", "off_spa_1", "man", "man", 30, SPAIN, 0, true));
			sld.Dialog.Filename    = "Quest\FraLineNpc_2.c";
			sld.DontRansackCaptain = true;
			FantomMakeCoolSailor(sld, SHIP_GALEON_H, "Дездечадо", CANNON_TYPE_CULVERINE_LBS24, 80, 60, 60);
			FantomMakeCoolFighter(sld, 30, 70, 50, "blade28", "pistol3", 50);
			Group_AddCharacter("Spa_Attack", "CaptainAttack_1");
			// ==> стравливание
			Group_SetGroupCommander("Spa_Attack", "CaptainAttack_1");
			Group_SetTaskAttack("Spa_Attack", PLAYER_GROUP);
			Group_SetPursuitGroup("Spa_Attack", PLAYER_GROUP);
			Group_SetAddress("Spa_Attack", "Curacao", "", "");
			Group_LockTask("Spa_Attack");
		break;

        case "Fr2Letter_FightAfterShot":
			LAi_group_Delete("EnemyFight");
			chrDisableReloadToLocation = true;
            for (i=1; i<=2; i++)
            {
				sld = characterFromId("Bandit_"+i);
				LAi_SetWarriorType(sld);
				LAi_group_MoveCharacter(sld, "EnemyFight");
				LAi_RemoveCheckMinHP(sld);
				LAi_SetHP(sld, 200, 200);
			}
			LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
			LAi_group_FightGroups("EnemyFight", LAI_GROUP_PLAYER, true);
			LAi_group_SetCheck("EnemyFight", "OpenTheDoors");
			AddQuestRecord("Fra_Line_2_DelivLetter", "7");
        break;
//=================== Квест №3, похищение донны Анны  ===================
		case "Fr3TakeAnna_FightInHouse":
			chrDisableReloadToLocation = true;
			LAi_LocationFightDisable(loadedLocation, true); 
			LAi_group_Delete("PeaceGroup");
			sld = GetCharacter(NPC_GenerateCharacter("AnnaDeLeiva", "AnnaDeLeiva", "woman", "towngirl2", 10, SPAIN, -1, false));
			sld.Dialog.Filename = "Quest\FraLineNpc_2.c";
			sld.name = "Анна Рамирес";
			sld.lastname = "де Лейва";
			sld.RebirthPhantom = true; 
			LAi_NoRebirthEnable(sld);
			LAi_SetStayType(sld);
			LAi_group_MoveCharacter(sld, "PeaceGroup");
			ChangeCharacterAddressGroup(sld, "Havana_houseS1Bedroom", "goto","goto7");
			GetCharacterPos(pchar, &locx, &locy, &locz);
			LAi_group_Delete("EnemyFight");
			for (i=1; i<=5; i++)
			{
				Model = "Sold_spa_"+(rand(7)+1);
				sTemp = 18;
				attrName = "SpaFighter"+i;
				if (i==1) 
				{	
					Model = "SpaOfficer1";
					sTemp = 23;
					attrName = "DeLeiva";
				}
				sld = GetCharacter(NPC_GenerateCharacter(attrName, Model, "man", "man", sTemp, SPAIN, 0, true));
				FantomMakeCoolFighter(sld, sTemp, 70, 50, BLADE_LONG, "pistol2", 40);
				LAi_SetWarriorType(sld);
				if (i==1) 
				{
					sld.Dialog.Filename = "Quest\FraLineNpc_1.c";
					sld.name = "Хосе";
					sld.lastname = "Рамирес де Лейва";
					ChangeCharacterAddressGroup(sld, "Havana_houseS1", "goto", "goto2");					
				}
				else 
				{
					LAi_warrior_DialogEnable(sld, false);
					ChangeCharacterAddressGroup(sld, "Havana_houseS1", "goto", "goto5");
				}
				LAi_group_MoveCharacter(sld, "EnemyFight");
			}
			sld = characterFromId("DeLeiva");
			LAi_SetActorTypeNoGroup(sld);
			LAi_ActorDialog(sld, pchar, "", -1, 0);
			DoQuestCheckDelay("MainHeroFightModeOff", 1.0);
		break;

		case "Fr3TakeAnna_DelivShip":
			sld = characterFromId("AnnaDeLeiva");
			if (LAi_IsDead(sld))
			{
				AddQuestRecord("Fra_Line_3_TakeAnna", "5");
				pchar.questTemp.State = "Fr3TakeAnna_BadResult";
				DeleteAttribute(sld, "RebirthPhantom");
				sld.LifeDay = 0;
			}
			else
			{
				AddQuestRecord("Fra_Line_3_TakeAnna", "4");
				AddPassenger(pchar, sld, false);
				SetCharacterRemovable(sld, false);
				pchar.questTemp.State = "Fr3TakeAnna_NiceResult";
				LAi_SetStayType(sld);
				LAi_CharacterDisableDialog(sld);
				ChangeCharacterAddressGroup(sld, "Tortuga_townhall", "goto", "goto2");
			}
		break;

		case "Fr3_AfterAnna_IKnowYou": //Узнавание ГГ в таверне Гаваны.				
			sld = characterFromId("Havana_waitress");
			if (sti(sld.nation) == SPAIN)
			{
				chrDisableReloadToLocation = true;
				LAi_LocationFightDisable(&Locations[FindLocation("Havana_tavern")], false);
				sld.dialog.currentnode = "Allarm";			
				LAi_SetActorTypeNoGroup(sld);
				LAi_ActorDialog(sld, pchar, "", 0, 0);
			}
		break;

//=================== Квест №4, Солей Руаяль, сопровождение до Гваделупы  ===================
		case "Fr4SoleiRoyal_Fight":
            Log_QuestInfo("Испанская эскадра установлена.");
            Island_SetReloadEnableGlobal("Dominica", false);
			group_DeleteGroup("Spa_Attack");
    		Group_FindOrCreateGroup("Spa_Attack");
			Group_SetType("Spa_Attack", "war");
            // ==> Испанские кэпы
            for (i=1; i<=4; i++)
            {
                switch (i)
                {
					case 1:
                        ShipType = SHIP_GALEON_H;
                        Rank = 23;
                        Blade = "topor2";
						iTemp = CANNON_TYPE_CANNON_LBS24;
                    break;
                    case 2:
                        ShipType = SHIP_GALEON_H;
                        Rank = 21;
                        Blade = "blade33";
						iTemp = CANNON_TYPE_CANNON_LBS24;
                    break;
                    case 3:
                        ShipType = SHIP_GALEON_H;
                        Rank = 23;
                        Blade = "blade34";
						iTemp = CANNON_TYPE_CANNON_LBS24;
                    break;
                    /*case 4:
                        ShipType = SHIP_WARSHIP;
                        Rank = 32;
                        Blade = "blade33";
						iTemp = CANNON_TYPE_CULVERINE_LBS32;
                    break;
                    case 5:
                        ShipType = SHIP_GALEON_H;
                        Rank = 24;
                        Blade = "blade28";
						iTemp = CANNON_TYPE_CULVERINE_LBS24;
                    break;
                    case 6:
                        ShipType = SHIP_GALEON_H;
                        Rank = 32;
                        Blade = "blade32";
						iTemp = CANNON_TYPE_CULVERINE_LBS24;
                    break;*/
                }
                sld = GetCharacter(NPC_GenerateCharacter("CaptainAttack_"+i, "off_spa_"+(rand(1)+1), "man", "man", Rank, SPAIN, 0, true));
				if (i==3) 
				{
					sld.name = "Хуан";
					sld.lastname = "Галеоно";
					sld.DontRansackCaptain = true;
					FantomMakeCoolSailor(sld, ShipType, "", iTemp, 80, 80, 60);
					FantomMakeCoolFighter(sld, Rank, 77, 80, Blade, "pistol5", 80);
				}
				else
				{
					FantomMakeCoolSailor(sld, ShipType, "", iTemp, 60, 50, 50);
					FantomMakeCoolFighter(sld, Rank, 80, 90, Blade, "pistol3", 50);
				}
    			Group_AddCharacter("Spa_Attack", "CaptainAttack_"+i);
            }
            // ==> стравливание
			Group_SetGroupCommander("Spa_Attack", "CaptainAttack_1");
			Group_SetTaskAttack("Spa_Attack", PLAYER_GROUP);
            Group_SetPursuitGroup("Spa_Attack", PLAYER_GROUP);
			Group_SetAddress("Spa_Attack", "Dominica", "", "");
			Group_LockTask("Spa_Attack");
            pchar.quest.Fr4SoleiRoyal_WiWon.win_condition.l1 = "Group_Death";
			pchar.quest.Fr4SoleiRoyal_WiWon.win_condition.l1.group = "Spa_Attack";
			pchar.quest.Fr4SoleiRoyal_WiWon.win_condition = "Fr4SoleiRoyal_WiWon";
			pchar.quest.Fr4SoleiRoyal_DieHard.win_condition.l1 = "MapEnter";
            pchar.quest.Fr4SoleiRoyal_DieHard.win_condition = "Fr4SoleiRoyal_DieHard";
        break;

        case "Fr4SoleiRoyal_WiWon":
			Log_QuestInfo("Испанская эскадра разбита.");
			group_DeleteGroup("Spa_Attack");
			Island_SetReloadEnableGlobal("Dominica", true);
			pchar.quest.Fr4SoleiRoyal_DieHard.over = "yes";
			if (LAi_IsDead(characterFromId("SoleiRoyalCaptain")))
			{
				AddQuestRecord("Fra_Line_4_SoleiRoyal", "4");
				pchar.questTemp.State = "Fr4SoleiRoyal_SoleiRoyalSunk";

			}
			else
			{
				pchar.questTemp.State = "Fr4SoleiRoyal_GoodWork";
				AddQuestRecord("Fra_Line_4_SoleiRoyal", "3");
			}
        break;

        case "Fr4SoleiRoyal_DieHard":
            Log_QuestInfo("Свалил с поля боя, очень плохо.");
            pchar.questTemp.State = "Fr4SoleiRoyal_DieHard";
            AddQuestRecord("Fra_Line_4_SoleiRoyal", "5");
            group_DeleteGroup("Spa_Attack");
            Island_SetReloadEnableGlobal("Dominica", true);
            pchar.quest.Fr4SoleiRoyal_WiWon.over = "yes";
			sld = characterFromId("SoleiRoyalCaptain");
			RemoveCharacterCompanion(pchar, sld);
			sld.LifeDay = 0;
        break;
		//=================== Квест №5, решение проблем донны Анны  ===================
        case "Fr5Anna_InShore":	
			chrDisableReloadToLocation = true;
			//==> братьев к маяку.          
			for (i=1; i<=2; i++)
            {
				if (i==1) sTemp = "officer_12";
				else sTemp = "officer_2";
				sld = GetCharacter(NPC_GenerateCharacter("DeLeivaBrother_"+i, sTemp, "man", "man", 35, SPAIN, -1, true));
				if (i==1)
				{
					sld.name = "Марио";
					sld.lastname = "Эстебан де Лейва";
					sld.money = 34780;
					FantomMakeCoolFighter(sld, 37, 95, 70, "blade25", "pistol5", 100);
				}
				else
				{
					sld.name = "Роберто";
					sld.lastname = "Винченцо де Лейва";
					sld.money = 41340;
					FantomMakeCoolFighter(sld, 34, 95, 70, "blade33", "pistol4", 100);
				}				
				sld.Dialog.Filename = "Quest\FraLineNpc_1.c";
				sld.SaveItemsForDead = true; // сохранять на трупе вещи
				sld.DontClearDead = true; // не убирать труп через 200с	
				LAi_SetWarriorTypeNoGroup(sld);
				LAi_warrior_DialogEnable(sld, true);
				ChangeCharacterAddressGroup(sld, "Mayak10", "goto", "goto12");
				LAi_group_MoveCharacter(sld, "PeaceGroup");
			}	
			LAi_LocationFightDisable(&Locations[FindLocation("Mayak10")], true); 
        break;		
//=================== Квест №6, доставка письма Олоне  ===================
        case "Fr6Olone_GuadeloupeBattle":			
            Log_QuestInfo("Испанец установлен.");
			AddQuestRecord("Fra_Line_6_Olone", "7");
            bQuestDisableMapEnter = true;
			group_DeleteGroup("Spa_Attack");
    		Group_FindOrCreateGroup("Spa_Attack");
			Group_SetType("Spa_Attack", "war");
            // ==> Испанский кэп
            sld = GetCharacter(NPC_GenerateCharacter("CaptainAttack_1", "off_spa_1", "man", "man", 35, SPAIN, -1, true));
			sld.DontRansackCaptain = true;
			FantomMakeCoolSailor(sld, SHIP_LINESHIP, "", CANNON_TYPE_CULVERINE_LBS32, 100, 95, 95);
            FantomMakeCoolFighter(sld, 35, 100, 90, BLADE_LONG, "pistol3", 150);
    		Group_AddCharacter("Spa_Attack", "CaptainAttack_1");
            // ==> стравливание
			Group_SetGroupCommander("Spa_Attack", "CaptainAttack_1");
			Group_SetTaskAttack("Spa_Attack", PLAYER_GROUP);
            Group_SetPursuitGroup("Spa_Attack", PLAYER_GROUP);
			Group_SetAddress("Spa_Attack", "Guadeloupe", "", "");
			Group_LockTask("Spa_Attack");
			pchar.quest.CanEnterToMap.win_condition.l1 = "NPC_Death";
			pchar.quest.CanEnterToMap.win_condition.l1.character = "CaptainAttack_1";
			pchar.quest.CanEnterToMap.win_condition = "CanEnterToMap";
        break;

        case "Fr6Olone_TalkWithOlone":
            //==> Олоне
			GetCharacterPos(pchar, &locx, &locy, &locz);
			chrDisableReloadToLocation = true;
            sld = characterFromID("Olone");
			LAi_SetCurHPMax(sld);
         	ChangeCharacterAddress(sld, "Cumana_town", LAi_FindNearestFreeLocator("goto", locx, locy, locz));
            LAi_SetActorType(sld);
			LAi_ActorDialog(sld, pchar, "", -1, 0);
        break;
//=================== Квест №7, спасение рядового Рока Бразильца  ===================
		case "Fr7RockBras_DelivShip":
			sld = characterFromId("RockBrasilian");
			if (LAi_IsDead(sld))
			{
				AddQuestRecord("Fra_Line_7_RockBras", "4");
				pchar.questTemp.State = "Fr7RockBras_RockIsDead";
				DeleteAttribute(sld, "RebirthPhantom");
				sld.LifeDay = 0;
			}
			else
			{
				AddQuestRecord("Fra_Line_7_RockBras", "3");
				AddPassenger(pchar, sld, false);
				SetCharacterRemovable(sld, false);
				pchar.questTemp.State = "Fr7RockBras_RockIsSaved";
				LAi_SetStayTypeNoGroup(sld);
				LAi_CharacterDisableDialog(sld);
				ChangeCharacterAddressGroup(sld, "Tortuga_townhall", "goto", "goto2");
			}
		break;
//=================== Квест №9, отбить нападение на Порт-о-Принс  ===================
		case "Fr9GuardPP_Late":
			pchar.quest.Fr9GuardPP_Fight.over = "yes";
			pchar.questTemp.State = "Fr9GuardPP_Late";
			AddQuestRecord("Fra_Line_9_GuardPortPax", "5");
            SetCaptureTownByNation("PortPax", SPAIN);
            UpdateRelations();
            RefreshBattleInterface();
		break;

		case "Fr9GuardPP_Fight":
            Log_QuestInfo("Испанская эскадра установлена. Это будет великая битва...");
			pchar.quest.Fr9GuardPP_Late.over = "yes";
            Island_SetReloadEnableGlobal("Hispaniola2", false);
    		group_DeleteGroup("Spa_Attack");
    		Group_FindOrCreateGroup("Spa_Attack");
			Group_SetType("Spa_Attack", "war");
            // ==> Испанские кэпы
            for (i=1; i<=6; i++)
            {
                switch (i)
                {
					case 1:
                        ShipType = SHIP_MANOWAR;
                        Rank = 40;
                        Blade = "topor2";
						iTemp = CANNON_TYPE_CANNON_LBS42;
                    break;
                    case 2:
                        ShipType = SHIP_GALEON_H;
                        Rank = 30;
                        Blade = "blade31";
						iTemp = CANNON_TYPE_CULVERINE_LBS24;
                    break;
                    case 3:
                        ShipType = SHIP_BATTLESHIP;
                        Rank = 38;
                        Blade = "blade34";
						iTemp = CANNON_TYPE_CANNON_LBS32;
                    break;
                    case 4:
                        ShipType = SHIP_WARSHIP;
                        Rank = 35;
                        Blade = "blade33";
						iTemp = CANNON_TYPE_CULVERINE_LBS32;
                    break;
                    case 5:
                        ShipType = SHIP_GALEON_H;
                        Rank = 31;
                        Blade = "blade28";
						iTemp = CANNON_TYPE_CULVERINE_LBS24;
                    break;
                    case 6:
                        ShipType = SHIP_LINESHIP;
                        Rank = 36;
                        Blade = "blade32";
						iTemp = CANNON_TYPE_CANNON_LBS32;
                    break;
                    /*case 7:
                        ShipType = SHIP_GALEON_H;
                        Rank = 30;
                        Blade = "topor2";
						iTemp = CANNON_TYPE_CULVERINE_LBS24;
                    break;
                    case 8:
                        ShipType = SHIP_BATTLESHIP;
                        Rank = 37;
                        Blade = "blade35";
						iTemp = CANNON_TYPE_CULVERINE_LBS32;
                    break;*/
                }
                sld = GetCharacter(NPC_GenerateCharacter("CaptainAttack_"+i, "off_spa_"+(rand(1)+1), "man", "man", Rank, SPAIN, 0, true));
				if (i==2 || i==5 || i==7) 
				{
					FantomMakeCoolSailor(sld, ShipType, "", iTemp, 80, 90, 70);
					FantomMakeCoolFighter(sld, Rank, 90, 90, Blade, "pistol3", 100);
				}
				else
				{
					FantomMakeCoolSailor(sld, ShipType, "", iTemp, 100, 100, 90);
					FantomMakeCoolFighter(sld, Rank, 100, 90, Blade, "pistol5", 200);
				}
    			Group_AddCharacter("Spa_Attack", "CaptainAttack_"+i);
            }
            // ==> стравливание
			Group_SetGroupCommander("Spa_Attack", "CaptainAttack_1");
			Group_SetTaskAttack("Spa_Attack", PLAYER_GROUP);
            Group_SetPursuitGroup("Spa_Attack", PLAYER_GROUP);
			Group_SetAddress("Spa_Attack", "Hispaniola2", "", "");
			Group_LockTask("Spa_Attack");
            pchar.quest.Fr9GuardPP_WiWon.win_condition.l1 = "Group_Death";
			pchar.quest.Fr9GuardPP_WiWon.win_condition.l1.group = "Spa_Attack";
			pchar.quest.Fr9GuardPP_WiWon.win_condition = "Fr9GuardPP_WiWon";
			pchar.quest.Fr9GuardPP_DieHard.win_condition.l1 = "MapEnter";
            pchar.quest.Fr9GuardPP_DieHard.win_condition = "Fr9GuardPP_DieHard";
        break;

        case "Fr9GuardPP_WiWon":
			Log_QuestInfo("Испанская эскадра разбита.");
			group_DeleteGroup("Spa_Attack");
			Island_SetReloadEnableGlobal("Hispaniola2", true);
			pchar.quest.Fr9GuardPP_DieHard.over = "yes";
			if (LAi_IsDead(characterFromId("SoleiRoyalCaptain")))
			{
				AddQuestRecord("Fra_Line_9_GuardPortPax", "3");
				pchar.questTemp.State = "Fr9GuardPP_SoleiRoyalSunk";

			}
			else
			{
				pchar.questTemp.State = "Fr9GuardPP_GoodWork";
				AddQuestRecord("Fra_Line_9_GuardPortPax", "2");
			}
        break;

        case "Fr9GuardPP_DieHard":
            Log_QuestInfo("Свалил с поля боя, линейке конец.");
            pchar.questTemp.State = "Fr9GuardPP_DieHard";
            AddQuestRecord("Fra_Line_9_GuardPortPax", "4");
            group_DeleteGroup("Spa_Attack");
            Island_SetReloadEnableGlobal("Hispaniola2", true);
            pchar.quest.Fr9GuardPP_WiWon.over = "yes";
			sld = characterFromId("SoleiRoyalCaptain");
			RemoveCharacterCompanion(pchar, sld);
			sld.LifeDay = 0;
            SetCaptureTownByNation("PortPax", SPAIN);
            UpdateRelations();
            RefreshBattleInterface();
        break;
//=================== Квест №12, конец войны  ===================
        case "Fr12EndOfWar_TakeOutShip":
			group_DeleteGroup("SoleiRoyal");
        break;

/////////////////////////////////////////////////////////////////////////////////////////////////////////
////  END Французская линейка
/////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////   Изабелла начало   //////////////////////////////////////////
        case "Romantic_Pirat_toTavern":
			chrDisableReloadToLocation = true;
			bDisableFastReload = true; 
			FreeSitLocator("Pirates_tavern", "sit2");
			DoQuestReloadToLocation("Pirates_tavern", "sit", "sit2", "");			
			sld = characterFromID("Atilla");        
			LAi_SetActorType(sld);
			LAi_ActorSetSitMode(sld);
			LAi_ActorDialogDelay(sld, pchar, "", 1.5);
        break;

        case "Romantic_Pirat_toTavern_end":
			chrDisableReloadToLocation = false; 
			bDisableFastReload = false; 
			LocatorReloadEnterDisable("SanJuan_town", "houseSp6", false); //теперь открываем дом Изабеллы
			pchar.RomanticQuest = "toSalvator";
			AddQuestRecord("Romantic_Line", "3");
			LAI_SetPlayerType(pchar);           
            DoQuestReloadToLocation("Pirates_tavern", "tables", "stay3", "");
            sld = CharacterFromID("Atilla");
            LAI_SetSitType(sld);
            Pchar.quest.Romantic_TalkInShop.win_condition.l1 = "location";
            Pchar.quest.Romantic_TalkInShop.win_condition.l1.location = "SanJuan_Store";
            Pchar.quest.Romantic_TalkInShop.win_condition = "Romantic_TalkInShop";
            PChar.quest.Romantic_TalkInShop.again = true; // вешаем на долго, тк не факт, что Pchar.RomanticQuest.TalkInShop = true; было
            PChar.quest.Romantic_TalkInShop.DontCheck = true;            
            Pchar.RomanticQuest.PriestAsk = true;
        break;

        case "RP_afterVisitBedroom":
			chrDisableReloadToLocation = true;
			bDisableFastReload = true;
			sld = characterFromID("Husband");        
			LAi_SetActorType(sld);
			LAi_ActorDialog(sld, Pchar, "", -1, 0);
        break;

        case "RP_talkWithHusbandIsOver":     
			LAi_SetStayType(characterFromID("Husband"));
        break;

        case "Romantic_TalkInShop":
            if(CheckAttribute(Pchar, "RomanticQuest.TalkInShop"))
            {
                PChar.quest.Romantic_TalkInShop.over = "yes";
                characters[GetCharacterIndex("SanJuan_trader")].dialog.currentnode = "Romantic_1";
                sld = CharacterFromID("Isabella");
        		LAi_SetStayType(sld);
				ChangeCharacterAddressGroup(sld, PChar.location, "goto", "goto3");
                sld.dialog.currentnode = "TalkInShop";
    			LAi_SetActorType(sld);
				LAi_ActorDialog(sld, pchar, "Romantic_TalkInShop_3", -1, 0);
            }
        break;

        case "Romantic_TalkInShop_2":
			sld = characterFromID("Isabella");
            LAi_ActorDialog(sld, Pchar, "", 1.0, 0);
        break;

        case "Romantic_TalkInShop_3":
            sld = CharacterFromID("SanJuan_trader");
			LAi_SetActorType(sld);
            LAi_ActorDialog(sld, Pchar, "", 0.0, 0);
        break;

        case "Romantic_TalkInShop_end": 
			chrDisableReloadToLocation = false;
            LAi_SetStayType(characterFromID("SanJuan_trader"));
            LAI_SetPlayerType(Pchar);
            AddQuestRecord("Romantic_Line", "4");
			sld = CharacterFromID("Isabella");
			LAi_SetCitizenType(sld);
            sld.dialog.currentnode = "TalkInChurch";
			DoQuestFunctionDelay("SetIsabellaTalker", 4.0);
        break;
        
        case "Romantic_Battle_in_Bedroom_1":
            sld = CharacterFromID("Husband");
            ChangeCharacterAddressGroup(sld, "SanJuan_houseS1Bedroom", "reload", "reload1");
            LAi_SetActorType(Pchar);
			LAi_SetActorType(sld);
            LAi_ActorFollow(sld, Pchar, "Romantic_Battle_in_Bedroom_2", 3.0);
        break;

        case "Romantic_Battle_in_Bedroom_2":
            sld = CharacterFromID("Husband");
            sld.Dialog.CurrentNode = "Romantic_Battle_in_Bedroom";
            LAi_ActorWaitDialog(Pchar, sld);
		    LAi_ActorDialog(sld, Pchar, "", 2.0, 0);
        break;
        
        case "Romantic_Battle_in_Bedroom_3":
			pchar.quest.RP_afterVisitBedroom.over = "yes";
			chrDisableReloadToLocation = true;
			bDisableFastReload = true;             
            LAi_SetPlayerType(pchar);
            ChangeCharacterReputation(pchar, -15);  
			sld = CharacterFromID("Husband");
            Lai_SetImmortal(sld, false);
            LAi_SetWarriorType(sld);
            LAi_group_MoveCharacter(sld, "TmpEnemy");
            LAi_group_SetHearRadius("TmpEnemy", 100.0);
	        LAi_group_FightGroups("TmpEnemy", LAI_GROUP_PLAYER, true);
	        LAi_group_SetCheck("TmpEnemy", "Romantic_Battle_in_Bedroom_End");
        break;
        
        case "Romantic_Battle_in_Bedroom_End":
			chrDisableReloadToLocation = false;
			bDisableFastReload = false; 
            sld = CharacterFromID("Isabella");
            sld.Dialog.CurrentNode = "Murder";
            AddQuestRecord("Romantic_Line", "1");
            CloseQuestHeader("Romantic_Line");
        break;
        
        case "Romantic_Battle_in_Bedroom_4":
            LAi_SetPlayerType(pchar);
            sld = CharacterFromID("Husband");
            LAi_ActorRunToLocation(sld, "reload", "reload1", "SanJuan_houseSp6", "goto", "goto5", "Romantic_Battle_in_Bedroom_5", 2.0);
	        AddQuestRecord("Romantic_Line", "2");
        break;
        
        case "Romantic_Battle_in_Bedroom_5":
            sld = CharacterFromID("Husband");
            LAi_SetHuberStayType(sld);
	        LAi_SetStayHuberPointWindow(sld, "goto", "goto1");
	        LAi_SetStayHuberPointMap(sld, "goto", "goto2");
        break;
        // церковь      
        case "Romantic_TalkInChurch_end":
            sld = CharacterFromID("Isabella");
            LAi_SetStayType(sld);
			chrDisableReloadToLocation = false;
			bDisableFastReload = false; 
        break;
         // разговор с братом в порту куманы -->
        case "Romantic_Brother":
			chrDisableReloadToLocation = true;
			bDisableFastReload = true; 
            Pchar.GenQuest.Hunter2Pause = true; // ОЗГи на паузу.
			sld = GetCharacter(NPC_GenerateCharacter("MigelDeValdes", "citiz_9", "man", "man", 5, SPAIN, -1, true));
            sld.name 	= "Мигель";
	        sld.lastname = "де Вальдес";
			sld.SaveItemsForDead = true; // сохранять на трупе вещи
			sld.DontClearDead = true; // не убирать труп через 200с
			sld.Dialog.Filename = "Quest\Isabella\BrigCaptain.c";
	        sld.dialog.currentnode = "Romantic_Brother_1";	        
            PlaceCharacter(sld, "goto", PChar.location);
			LAi_SetActorType(sld);			
			LAi_ActorDialog(sld, pchar, "", 1.5, 0);
        break;
        
        case "Romantic_Brother_port":
			chrDisableReloadToLocation = true; 
            Pchar.GenQuest.Hunter2Pause = true; // ОЗГи на паузу.
            sld = CharacterFromID("MigelDeValdes");
	        sld.dialog.currentnode = "Romantic_Brother_port_1";
            GetCharacterPos(pchar, &locx, &locy, &locz);
         	ChangeCharacterAddressGroup(sld, pchar.location, "goto", LAi_FindNearestFreeLocator("goto", locx, locy, locz));
            //PlaceCharacter(sld, "goto", PChar.location);
			LAi_SetActorType(sld);
			LAi_ActorDialog(sld, pchar, "", 0.5, 0);
        break;		
		// --> разговор в доме с братов, сцена 8
        case "Romantic_Brother_House":
            sld = CharacterFromID("Isabella");
            sld.dialog.currentnode = "Brother_1";
            ChangeCharacterAddressGroup(sld, "SanJuan_houseSp6", "goto", "goto3");
            LAi_SetActorType(sld);            
            sld = CharacterFromID("MigelDeValdes");
            sld.dialog.currentnode = "Romantic_Brother_port_1";
            ChangeCharacterAddressGroup(sld, "SanJuan_houseSp6", "goto", "goto4");
            LAi_SetActorType(sld);           
			pchar.RomanticQuest = "exitFromDetector";
        break;
        
        case "Romantic_Brother_House_3":
            // ГГ обратно
            SetMainCharacterIndex(1);
            PChar = GetMainCharacter();
            LAi_SetPlayerType(PChar);
			locCameraTarget(PChar)
            locCameraFollow();
            EndQuestMovie();            
            LAi_SetCitizenType(characterFromID("Isabella"));
			sld = characterFromID("MigelDeValdes");
			LAi_SetCitizenType(sld);
			sld.dialog.currentnode = "Romantic_Brother_Thanks";
			sld = characterFromID("Husband");
			sld.dialog.currentnode = "WeWaitYouTonight";
			pchar.RomanticQuest = "DelivMigel"; //флаг "братан доставлен"
			LocatorReloadEnterDisable("SanJuan_town", "houseSp6", true); //закрываем двери, чтобы не доставал до поры своим присутствием
        break;
		// --> смерть брата
        case "Romantic_DeadBrother_1":
			LocatorReloadEnterDisable("SanJuan_town", "houseSp6", false);
			chrDisableReloadToLocation = true; 
            SetMainCharacterIndex(1);
            PChar = GetMainCharacter();
            LAi_SetPlayerType(PChar);
			locCameraTarget(PChar)
            locCameraFollow();	
			npchar = characterFromID("MigelDeValdes");
			LAi_group_MoveCharacter(npchar, "BrotherGroup");
			sld = characterFromID("Husband");			
			LAi_SetWarriorType(sld);
			LAi_group_MoveCharacter(sld, "EnemyFight");
			LAi_group_SetRelation("EnemyFight", "BrotherGroup", LAI_GROUP_ENEMY);
			LAi_group_FightGroups("EnemyFight", "BrotherGroup", true);
			LAi_group_SetCheck("BrotherGroup", "Romantic_DeadBrother_2");	
			LAi_LockFightMode(pchar, true); //саблю не достать
			EndQuestMovie();
        break;

		case "Romantic_DeadBrother_2":         
			LAi_group_Delete("BrotherGroup");
			sld = characterFromID("Husband");
			sld.dialog.currentnode = "SeenMainHero";
			LAi_SetActorTypeNoGroup(sld);
			LAi_ActorDialog(sld, pchar, "", 1.0, 0);
        break;

		case "Romantic_DeadBrother_3":         
			sld = characterFromID("Husband");
			LAi_SetStayType(sld);
			LocatorReloadEnterDisable("SanJuan_houseSp6", "reload2", true); //спальню тоже, а то может быть открыта
			LAi_LocationFightDisable(&Locations[FindLocation("SanJuan_houseSp6")], true); 
		break;

		case "Romantic_DeadBrother_Cancel": //если ГГ не явлся к дому Изабеллы в течение суток        
			pchar.RomanticQuest = "QuestOver";
			//LocatorReloadEnterDisable("SanJuan_town", "houseSp6", true); //закрываем дверь в дом Изабеллы
			ChangeCharacterAddress(CharacterFromID("Husband"), "none", "");
			ChangeCharacterAddress(CharacterFromID("MigelDeValdes"), "none", "");
			CloseQuestHeader("Romantic_Line");
		break;
		// --> попытка ареста ГГ
		case "Romantic_ArrestInHouse":
			// Атиллу поставим в дом на Бермудах для диалога в дальнейшем после ареста
			sld = characterFromID("Atilla");
			LAi_SetSitType(sld);
			ChangeCharacterAddressGroup(sld, "Pirates_houseF1", "sit", "sit2");
			sld.dialog.currentnode = "TalkingNews";
			//Сальватор убегает на выход   
			npchar = characterFromID("Husband");
			LAi_SetActorType(npchar);
			LAi_ActorRunToLocation(npchar, "reload", "reload1", "SanJuan_houseS1Bedroom", "goto", "goto3", "", 5.0);
			LocatorReloadEnterDisable("SanJuan_town", "houseSp6", true); //опять закрываем, чтобы ГГ до поры не ломился в дом
			//Команда на арест ГГ
			iTemp = FindColony("SanJuan");
			sTemp = NationShortName(sti(colonies[iTemp].nation));
            for (i=1; i<=3; i++)
            {
				if (i==1) 
				{
					if (sti(colonies[iTemp].HeroOwn))
					{
						Model = "officer_18";
					}
					else
					{
						Model = "off_" + sTemp + "_1";
					}
					Rank = sti(pchar.rank)+ MOD_SKILL_ENEMY_RATE + 6;
					Blade = "blade28";
				}
				else 
				{
					if (sti(colonies[iTemp].HeroOwn))
					{
						Model = "pirate_" + i;
					}
					else
					{
						Model = "sold_" + sTemp + "_" +(rand(7)+1);
					}
					Rank = sti(pchar.rank)+ MOD_SKILL_ENEMY_RATE + 2;
					Blade = "blade24";
				}
				sld = GetCharacter(NPC_GenerateCharacter("Soldier_"+i, Model, "man", "man", Rank, sti(colonies[iTemp].nation), 0, true));
				FantomMakeCoolFighter(sld, Rank, 100, 90, Blade, "pistol5", 200);
				sld.City = colonies[iTemp].id;
				sld.CityType = "soldier";
				sld.Dialog.Filename = "Quest\Isabella\BrigCaptain.c";
				sld.dialog.currentnode = "ArrestInHome_2";
				LAi_SetWarriorType(sld);
				if (i != 1) LAi_warrior_DialogEnable(sld, false);
				LAi_group_MoveCharacter(sld, GetNationNameByType(sti(colonies[iTemp].nation)) + "_citizens");
				ChangeCharacterAddressGroup(sld, "SanJuan_houseSp6", "reload", "reload1");
			}
			sld = characterFromID("Soldier_1");
			LAi_SetActorTypeNoGroup(sld);
			LAi_ActorDialog(sld, pchar, "", 1.5, 0);
        break;
		//через два месяца Алиллу - в дом
		case "Romantic_AtillaToHouse":
			sld = characterFromID("Atilla");
			LAi_SetSitType(sld);
			ChangeCharacterAddressGroup(sld, "Pirates_houseF1", "sit", "sit2");
			sld.dialog.currentnode =  "AtillaInHouse";
        break;
		//засада в доме Сальватора в Сан-Хуане
		case "Romantic_AmbushInHouse":
            Log_QuestInfo("Засада Сальватора.");
            chrDisableReloadToLocation = true; // закрыть выход из локации.
			LAi_LocationFightDisable(&locations[FindLocation("SanJuan_houseSp6")], true);
            LAi_group_Delete("EnemyFight");
			for (i=1; i<=3; i++)
            {
                sld = GetCharacter(NPC_GenerateCharacter("Bandit"+i, "officer_"+(i+10), "man", "man", 25, PIRATE, 0, true));
                FantomMakeCoolFighter(sld, 25, 70, 60, BLADE_LONG, "pistol2", 20);
            	LAi_SetStayType(sld);  
				LAi_CharacterDisableDialog(sld);
                LAi_group_MoveCharacter(sld, "EnemyFight");
               	ChangeCharacterAddressGroup(sld, "SanJuan_houseSp6", "goto",  "goto"+i);
            }
			sld.Dialog.Filename = "Quest\Isabella\BrigCaptain.c";
			sld.dialog.currentnode = "AmbushBandit";
			LAi_CharacterEnableDialog(sld);
            LAi_SetActorType(sld);
			LAi_ActorDialog(sld, pchar, "", -1, 0);
        break;
		//атака брига с Изабеллой
        case "Romantic_BrigTimeOver":
			Log_QuestInfo("Не успел к бригу у Порто Белло. Изабелла погибла.");
            pchar.quest.Romantic_AttackBrig.over = "yes";
            AddQuestRecord("Romantic_Line", "14");
            QuestSetCurrentNode("Atilla", "Brig_Late"); //нода Атилле на опаздал
        break;

        case "Romantic_AttackBrig":
            Log_QuestInfo("Бриг найден, на абордаж!");
            pchar.quest.Romantic_BrigTimeOver.over = "yes";
            Island_SetReloadEnableGlobal("PortoBello", false);
    		Group_FindOrCreateGroup("Pirate_Attack");
			Group_SetType("Pirate_Attack", "trade");
            sld = GetCharacter(NPC_GenerateCharacter("CaptainAttack_1", "officer_14", "man", "man", 20, PIRATE, 0, true));
            sld.Dialog.Filename = "Quest\Isabella\BrigCaptain.c";
			sld.dialog.currentnode = "BrigAbordage";
            FantomMakeCoolSailor(sld, SHIP_BRIG, "Восторженный", CANNON_TYPE_CULVERINE_LBS24, 70, 70, 70);
            FantomMakeCoolFighter(sld, 20, 70, 50, "blade24", "pistol3", 20);
			sld.DontRansackCaptain = true;
			Group_AddCharacter("Pirate_Attack", "CaptainAttack_1");
            // ==> стравливание
			Group_SetGroupCommander("Pirate_Attack", "CaptainAttack_1");
			Group_SetTaskAttack("Pirate_Attack", PLAYER_GROUP);
            Group_SetPursuitGroup("Pirate_Attack", PLAYER_GROUP);
			Group_SetAddress("Pirate_Attack", "PortoBello", "", "");
			Group_LockTask("Pirate_Attack");
            // ==> прерывания
			pchar.quest.Romantic_AfterBrigSunk.win_condition.l1 = "Character_sink";
			pchar.quest.Romantic_AfterBrigSunk.win_condition.l1.character = "CaptainAttack_1";
			pchar.quest.Romantic_AfterBrigSunk.win_condition = "Romantic_AfterBrigSunk";
			pchar.quest.Romantic_BrigDieHard.win_condition.l1 = "MapEnter";
            pchar.quest.Romantic_BrigDieHard.win_condition = "Romantic_BrigDieHard";
        break;
        
        case "Romantic_BrigDieHard":
            pchar.quest.Romantic_AfterBrigSunk.over = "yes";
            group_DeleteGroup("Pirate_Attack");
            Island_SetReloadEnableGlobal("PortoBello", true);
            AddQuestRecord("Romantic_Line", "16");
            QuestSetCurrentNode("Atilla", "Brig_DieHard"); //нода Атилле на бриг ушел от погони
        break;

        case "Romantic_AfterBrigSunk":
            pchar.quest.Romantic_BrigDieHard.over = "yes";
            Island_SetReloadEnableGlobal("PortoBello", true);
            group_DeleteGroup("Pirate_Attack");
            AddQuestRecord("Romantic_Line", "15");
            QuestSetCurrentNode("Atilla", "Brig_Sunk"); //нода Атилле бриг утонул
        break;

        case "Romantic_TaklInCabinBrig": 
			sld = characterFromID("Isabella");
			sld.dialog.currentnode = "BrigAbordage";
			GetCharacterPos(pchar, &locx, &locy, &locz);
			ChangeCharacterAddressGroup(sld, pchar.location, "rld", LAi_FindFarLocator("rld", locx, locy, locz));
			LAi_SetActorType(pchar);
            LAi_SetActorType(sld);
            SetActorDialogAny2Pchar(sld.id, "", 0.0, 0.0);
		    LAi_ActorFollow(pchar, sld, "ActorDialog_Any2Pchar", -1);
        break;
		//засада в Порто Белло
		case "Romantic_AmbushInPortoBello":
            Log_QuestInfo("Засада Сальватора.");
            chrDisableReloadToLocation = true; // закрыть выход из локации.
			LAi_LocationFightDisable(&locations[FindLocation("PortoBello_houseF2")], true);
			LAi_group_Delete("EnemyFight");
            for (i=4; i<=5; i++)
            {
                sld = GetCharacter(NPC_GenerateCharacter("Bandit"+i, "officer_"+i, "man", "man", 30, PIRATE, 0, true));
                FantomMakeCoolFighter(sld, 30, 80, 70, "topor2", "pistol3", 40);
            	LAi_SetStayType(sld);
				LAi_CharacterDisableDialog(sld);
                LAi_group_MoveCharacter(sld, "EnemyFight");
               	ChangeCharacterAddressGroup(sld, "PortoBello_houseF2", "goto",  "goto"+i);
            }
			sld.Dialog.Filename = "Quest\Isabella\BrigCaptain.c";
			sld.dialog.currentnode = "AmbushPotroBello";
			LAi_CharacterEnableDialog(sld);
            LAi_SetActorType(sld);
			LAi_ActorDialog(sld, pchar, "", -1, 0);
        break;

		case "Romantic_TalkPortoBello":
			chrDisableReloadToLocation = false; 
            sld = CharacterFromID("Isabella");
	        sld.dialog.currentnode = "AmbushPortoBello";
            ChangeCharacterAddressGroup(sld, pchar.location, "reload",  "reload1");
			LAi_SetActorType(sld);
			LAi_ActorDialog(sld, pchar, "", -1, 1.5);
        break;
		//базар с Изабеллой у дома Роситы
        case "Romantic_BelizCityTalk":
			chrDisableReloadToLocation = true; 
            Pchar.GenQuest.Hunter2Pause = true; // ОЗГи на паузу.
            sld = CharacterFromID("Isabella");
	        sld.dialog.currentnode = "BelizTalk";
            ChangeCharacterAddressGroup(sld, pchar.location, "goto",  "goto17");
			LAi_SetActorType(sld);
			LAi_ActorDialog(sld, pchar, "", -1, 0.0);
        break;
		//смерть охранника Изабеллы в пещере
        case "Romantic_BanditIsDead":
            sld = CharacterFromID("Isabella");
			LAi_SetActorType(sld);
			LAi_ActorDialog(sld, pchar, "", -1, 0.0);
        break;
		//Сальватор в пещере
        case "Romantic_SalvatorInCave":
			LAi_LockFightMode(pchar, true); //саблю не достать
            sld = CharacterFromID("Husband");
	        sld.dialog.currentnode = "TalkInCave";
			sld.items.jewelry5 = 100;
			sld.items.jewelry13 = 7;
			sld.items.jewelry14 = 6; 
			sld.items.jewelry15 = 5; 
			sld.items.jewelry17 = 30; 
			sld.items.jewelry18 = 12; 
			sld.items.indian1 = 1; 
			sld.items.indian5 = 1; 
			sld.items.indian6 = 1; 
			sld.items.indian10 = 2; 
			sld.items.indian12 = 1; 
			sld.items.indian14 = 1; 
			sld.items.jewelry12 = 12; 
			sld.items.jewelry11 = 6; 
			sld.items.jewelry10 = 20; 
			sld.items.jewelry7 = 21; 
			sld.items.jewelry6 = 11; 
			sld.items.jewelry4 = 7; 
			sld.items.jewelry3 = 15;
			sld.items.jewelry2 = 18;
			sld.items.jewelry1 = 31;
			sld.money = 350535;
			Lai_SetImmortal(sld, false);
			ChangeCharacterAddressGroup(sld, pchar.location, "goto",  "goto7");
			LAi_SetActorType(sld);
			LAi_ActorDialog(sld, pchar, "", -1, 0);
        break;

        case "Romantic_SalvatorIsDead":
			LAi_LocationDisableOfficersGen("Beliz_Cave_2", false); //офицеров пускать
			LAi_LocationDisableMonstersGen("Beliz_Cave_2", false); //монстров генерить
			QuestSetCurrentNode("Isabella", "SalvatorIsDead");
            sld = CharacterFromID("Isabella");
			LAi_SetActorType(sld);
			LAi_ActorDialog(sld, pchar, "", -1, 0.0);
        break;

		// --> разговор Изабеллы с Роситой в доме после освобождения Изабеллы
        case "Romantic_DelivToRosita": 
			if (GetCharacterIndex("Isabella") == -1)
			{
				QuestSetCurrentNode("Rosita", "IsabellaIsDead_Beliz");
			}
			else
			{
				pchar.RomanticQuest = "Beliz_exitFromDetector";
			QuestSetCurrentNode("Rosita", "SavedIsabella");
			}
        break;
        
        case "Romantic_DelivToRosita_2":
            // ГГ обратно
            SetMainCharacterIndex(1);
            PChar = GetMainCharacter();
            LAi_SetPlayerType(PChar);
			locCameraTarget(PChar)
            locCameraFollow();
            EndQuestMovie();            
            LAi_SetCitizenType(characterFromID("Rosita"));
			sld = characterFromID("Isabella");
			LAi_SetCitizenType(sld);
			sld.dialog.currentnode = "RositaHouse";
			//через месяц Изабеллу - в Сан-Хуан
			SetTimerCondition("Romantic_IsabellaToSanJuan", 0, 1, 0, false);
        break;

        case "Romantic_IsabellaToSanJuan":
			sld = CharacterFromID("Isabella");
			ChangeCharacterAddressGroup(sld , "SanJuan_houseSp6", "goto",  "goto1");
			sld.dialog.currentnode = "InSanJuanAgain";
        break;

        case "Romantic_OutFromHouse":
			DoReloadCharacterToLocation("SanJuan_town", "reload", "houseSp6");
        break;

		case "Romantic_Widding_Cancel": //если ГГ не явлся к в церковь для венчания в течение суток        
			pchar.quest.Isabella_widding.over = "yes";
			pchar.RomanticQuest = "QuestOver";
			LocatorReloadEnterDisable("SanJuan_town", "houseSp6", false); //открываем дом
			sld = characterFromID("Isabella");
			LAi_SetStayType(sld);
			sld.dialog.currentnode = "Cancel_Widding";
			ChangeCharacterAddressGroup(sld , "SanJuan_houseSp6", "goto",  "goto1");
			sld = CharacterFromID("SanJuan_Priest");
			LAi_SetPriestType(sld);
		break;
		//свадьба Изабеллы и ГГ. Шампанского!!!!!
        case "Romantic_Padre":
			LAi_SetActorType(pchar);
			LAi_ActorTurnToLocator(pchar, "barmen", "stay");
			DoQuestCheckDelay("Romantic_Padre_1", 1.0);
        break;

        case "Romantic_Padre_1":
            SetLaunchFrameFormParam("Во время службы прошло сорок минут.", "Romantic_Padre_2", 0, 3);
            LaunchFrameForm();
            WaitDate("", 0, 0, 0, 0, 40);
        	RecalculateJumpTable();
        break;

        case "Romantic_Padre_2":
			StartQuestMovie(true, true, true);
			locCameraToPos(-1.52, 3.25, -3.36, false);
			sld = CharacterFromID("SanJuan_Priest");
			sld.dialog.currentnode = "Romantic_2";
			LAi_SetActorType(sld);
			SetActorDialogAny2Pchar("SanJuan_Priest", "", 0.0, 0.0);
		    LAi_ActorFollow(PChar, sld, "ActorDialog_Any2Pchar", 0.0);
        break;

        case "Romantic_Padre_3":
            LAi_SetPlayerType(PChar);
			locCameraTarget(PChar)
            locCameraFollow();
            EndQuestMovie(); 
			sld = CharacterFromID("Isabella");
			LAi_SetActorType(sld);
			LAi_ActorTurnToCharacter(sld, pchar);
			DoQuestCheckDelay("Romantic_fightInChurch", 2.0);
        break;
		//Нападение бандитов в церкви
        case "Romantic_fightInChurch":
            Log_QuestInfo("Последняя подляна Сальватора.");
            chrDisableReloadToLocation = true; // закрыть выход из локации.
			LAi_group_Delete("EnemyFight");
			iTemp = sti(2+(MOD_SKILL_ENEMY_RATE/1.5));
			Rank = 10+MOD_SKILL_ENEMY_RATE*2;
            for (i=1; i<=iTemp; i++)
            {
                sld = GetCharacter(NPC_GenerateCharacter("Bandit"+i, "officer_"+(i+3), "man", "man", Rank, PIRATE, 0, true));
                FantomMakeCoolFighter(sld, Rank, 80, 70, "topor2", "pistol1", 40);
				sld.equip.gun = "";
            	LAi_SetWarriorType(sld);
				LAi_warrior_SetStay(sld, true);
            	sld.Dialog.Filename = "Quest\Isabella\BrigCaptain.c";				
                LAi_group_MoveCharacter(sld, "EnemyFight");				
               	ChangeCharacterAddressGroup(sld, "SanJuan_church", "reload",  "reload1");
            }
			sld.dialog.currentnode = "AmbushInChurch";
            LAi_SetActorTypeNoGroup(sld);
			LAi_ActorDialog(sld, pchar, "", 1.0, 0);
        break;

		case "Romantic_fightInChurch_1":
			sld = characterFromID("Isabella");
 			sld.dialog.currentnode = "AmbushInChurch";
            LAi_SetActorTypeNoGroup(sld);
			LAi_ActorDialog(sld, pchar, "", 0, 0);       
		break;

		case "Romantic_fightInChurch_2":		
			LAi_LocationFightDisable(loadedLocation, false); 
			//падре в сторону
			sld = characterFromID("SanJuan_Priest");
			LAi_SetActorType(sld);
			LAi_ActorRunToLocator(sld, "barmen", "bar2", "", -1);
			//деремся
            LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
			LAi_group_SetRelation("EnemyFight", "SPAIN_CITIZENS", LAI_GROUP_ENEMY);
            LAi_group_FightGroups("EnemyFight", LAI_GROUP_PLAYER, true);
            LAi_group_SetCheck("EnemyFight", "Romantic_afterFightInChurch");
			//Изабелла в сторону
			sld = characterFromID("Isabella");
			//LAi_SetImmortal(sld, false); // для тестов
			LAi_group_MoveCharacter(sld, LAI_GROUP_PLAYER);
			GiveItem2Character(sld, "unarmed");
			EquipCharacterbyItem(sld, "unarmed");
			LAi_SetFightMode(sld, false);
			LAi_SetActorTypeNoGroup(sld);
			LAi_ActorRunToLocator(sld, "barmen", "bar1", "", -1);	
        break;

		case "Romantic_afterFightInChurch":
            chrDisableReloadToLocation = false;
			LAi_LocationFightDisable(loadedLocation, true);
			sld = CharacterFromID("SanJuan_Priest");
			LAi_SetPriestType(sld);
			LAi_group_MoveCharacter(sld, "SPAIN_CITIZENS");
			sld = CharacterFromID("Isabella");
			if (LAi_IsDead(sld))
			{
				AddQuestRecord("Romantic_Line", "25");
				pchar.RomanticQuest = "IsabellaIsDead";
				QuestSetCurrentNode("Atilla", "IsabellaIsDead");
				QuestSetCurrentNode("Rosita", "IsabellaIsDead");
			}
			else
			{
				AddQuestRecord("Romantic_Line", "26");
				pchar.RomanticQuest = "IsabellaIsMyWife";
				QuestSetCurrentNode("Atilla", "IsabellaIsMyWife");
				QuestSetCurrentNode("Rosita", "IsabellaIsMyWife");
				LAi_SetCitizenType(CharacterFromID("Isabella"));
				QuestSetCurrentNode("Isabella", "IsabellaIsMyWife");
			}
        break;
		//новая сеймейная жизнь.
		case "Romantic_afterFirstTime":
			SaveCurrentQuestDateParam("RomanticQuest");
			IsabellaSetCurrentState("all");  //запомнинаем текущую статистику
			IsabellaCheckBreachState(); //тут же проверяем для обращений сразу же
			pchar.RomanticQuest.HorseCheck = pchar.questTemp.HorseQty; //запоминаем текущее кол-во посещения борделей
			IsabellaNullBudget(); //нулим семейный бюджет
			sld = CharacterFromID("Isabella");
			SaveCurrentNpcQuestDateParam(sld, "sex"); //запомниаем последний секс 
			sld.sex.control_year = sti(sld.sex.control_year) - 1; //-1 год (для первого раза)
			QuestSetCurrentNode("Rosita", "IsabellaIsWife"); //Росите ноду, по которой она помогать будет с проблемами
			pchar.RomanticQuest = "NewLifeForHero";
			AddQuestRecord("Romantic_Line", "28");
			AddQuestUserData("Romantic_Line", "IsabellaBuget", FindRussianMoneyString(MOD_SKILL_ENEMY_RATE*10000));
			//прерывание на вход в дом Изабеллы
			Pchar.quest.Romantic_OutHome.win_condition.l1 = "location";
			Pchar.quest.Romantic_OutHome.win_condition.l1.location = "PuertoRico";
			Pchar.quest.Romantic_OutHome.win_condition = "Romantic_OutHome";
		break;
		//запускаем проверку на пиратские делишки на входе в дом
		case "Romantic_EnterHome":
			if (CheckAttribute(pchar, "RomanticQuest.State"))
			{
				IsabellaCheckBreachState();
				pchar.RomanticQuest.Cheking = "";
				QuestSetCurrentNode("Isabella", "NewLife_hello");
				Pchar.quest.Romantic_OutHome.win_condition.l1 = "location";
				Pchar.quest.Romantic_OutHome.win_condition.l1.location = "PuertoRico";
				Pchar.quest.Romantic_OutHome.win_condition = "Romantic_OutHome";
			}     
		break;

		case "Romantic_OutHome":
			if (CheckAttribute(pchar, "RomanticQuest.State"))
			{
				pchar.RomanticQuest = "NewLifeForHero";     //для проверки сроков отсутсвия на детекторе итем
				SaveCurrentQuestDateParam("RomanticQuest"); //эти самые сроки и запомним
				Pchar.quest.Romantic_EnterHome.win_condition.l1 = "location";
				Pchar.quest.Romantic_EnterHome.win_condition.l1.location = "SanJuan_houseSp6";
				Pchar.quest.Romantic_EnterHome.win_condition = "Romantic_EnterHome";
			}     
		break;
		//сам секс собсно.
		case "Romantic_Sex":			
			sld = CharacterFromID("Isabella");
			LAi_SetActorType(sld);
			LAi_ActorTurnToCharacter(sld, pchar);
			if (GetNpcQuestPastMinutesParam(sld, "sex") > 10)
			{
				QuestSetCurrentNode("Isabella", "NewLife_Sex_Late");
				LAi_ActorDialog(sld, pchar, "", 1.0, 0);    
			}
			else
			{
				DoQuestCheckDelay("PlaySex_1", 3.0);
			}
		break;

		case "PlaySex_1":
			ResetSound();
			iTemp = rand(13)+1;
			switch (iTemp)
			{
				case 1: fTemp = 18.2; break;
				case 2: fTemp = 5.1;  break;
				case 3: fTemp = 19.3; break;
				case 4: fTemp = 8.2;  break;
				case 5: fTemp = 11.3; break;
				case 6: fTemp = 18.2; break;
				case 7: fTemp = 18.2; break;
				case 8: fTemp = 9.3;  break;
				case 9: fTemp = 19.4; break;
				case 10:fTemp = 12.2; break;
				case 11:fTemp = 19.4; break;
				case 12:fTemp = 12.3; break;
				case 13:fTemp = 10.2; break;
				case 14:fTemp = 11.2; break;
			}
			sGlobalTemp = iTemp;
			//LAi_FadeDelay(fTemp, "loading\inside\censored1.tga");
			//StartPictureAsVideo( "loading\inside\censored1.tga", fTemp );
			SetLaunchFrameFormParam("", "", 0, fTemp);
			SetLaunchFrameFormPic("loading\inside\censored1.tga");
            LaunchFrameForm();
			DoQuestCheckDelay("PlaySex_2", 1.0);			
		break;

		case "PlaySex_2":
			PlayStereoSound("sex\sex" + sGlobalTemp + ".wav");
            AddTimeToCurrent(2, rand(30));
			if (pchar.location == "SanJuan_houseS1Bedroom")
			{
			    QuestSetCurrentNode("Isabella", "NewLife_afterSex");
			    LAi_SetStayType(CharacterFromID("Isabella"));
			}
			//квест официантки
			if (pchar.questTemp.different == "FackWaitress_facking")
			{
				sld = characterFromId("WairessQuest");
				ChangeCharacterAddress(sld, "none", "");
				AddCharacterExpToSkill(pchar, "Loyality", 20);
				if (sti(pchar.questTemp.different.FackWaitress.Kick) != 1) 
				{
					pchar.questTemp.different = "FackWaitress_fackNoMoney";
					AddCharacterExpToSkill(pchar, "Fortune", 100);
				}
				pchar.money = sti(pchar.money) / sti(pchar.questTemp.different.FackWaitress.Kick);
				chrDisableReloadToLocation = false;
			}
			//квест развода хозяйки борделя
			if (pchar.questTemp.different == "HostessSex")
			{
				sld = characterFromId(pchar.questTemp.different.HostessSex.city + "_Hostess");
				ChangeCharacterAddressGroup(sld, pchar.questTemp.different.HostessSex.city + "_SecBrRoom", "goto", "goto8");
				LAi_SetOwnerTypeNoGroup(sld);
				DeleteAttribute(pchar, "questTemp.different.HostessSex");
				pchar.questTemp.different = "free";
			}
		break;

		case "Romantic_IsabellaBackToHall":
			Pchar.quest.Romantic_Sex.over = "yes";
            sld = CharacterFromID("Isabella");
            LAi_SetStayType(sld);
			ChangeCharacterAddressGroup(sld, "SanJuan_houseSp6", "goto",  "goto1"); 
			QuestSetCurrentNode("Isabella", "NewLife");
		break;

		case "sleep_in_home":
			DoQuestReloadToLocation("SanJuan_houseS1Bedroom", "goto", "goto4", "restore_hp");
		break;

/////////////////////////////////////   Изабелла конец   //////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////
////  Квест Аскольда начало
/////////////////////////////////////////////////////////////////////////////////////////////////////////
        case "Ascold_AttackTenManowars":
			LocatorReloadEnterDisable("Tenotchitlan_Jungle_02", "reload3_back", true); //закроем до поры храм
            pchar.questTemp.Ascold = "ILookedThisAss";
            // ==> корабль, куда помещаем предметы
            i = rand(9)+1;
            pchar.questTemp.Ascold.Ship = "AscoldCaptainAttack_" + i;
			Log_QuestInfo("Журнал в мановаре с ID кэпа: " + pchar.questTemp.Ascold.Ship);
            // ==> меняем дейскрайб судового журнала
            ChangeItemDescribe("ShipsJournal", "itmdescr_ShipsJournal_Ascold");
    		Group_FindOrCreateGroup("Ascold_Spa_Attack");
			Group_SetType("Ascold_Spa_Attack", "war");
            // ==> Испанские кэпы
			SetNationRelation2MainCharacter(SPAIN, RELATION_ENEMY);
			for (i = 1; i <=10; i++)
			{                
				Rank = 20 + rand(10);
				sTemp = "AscoldCaptainAttack_"+i;
				sld = GetCharacter(NPC_GenerateCharacter(sTemp, "off_spa_"+(rand(1)+1), "man", "man", Rank, SPAIN, -1, true));	
				FantomMakeCoolFighter(sld, Rank, 80, 80, "topor2", "pistol3", 50);
				FantomMakeCoolSailor(sld, SHIP_MANOWAR, "", CANNON_TYPE_CULVERINE_LBS32, 80, 80, 80);
				sld.DontRansackCaptain = true;
				Group_AddCharacter("Ascold_Spa_Attack", sTemp);
			}
            // ==> стравливание
			Group_SetGroupCommander("Ascold_Spa_Attack", "AscoldCaptainAttack_1");			
			Group_SetPursuitGroup("Ascold_Spa_Attack", PLAYER_GROUP); //TO_DO
			Group_SetAddress("Ascold_Spa_Attack", "Beliz", "quest_ships", "quest_ship_6");
			Group_SetTaskAttack("Ascold_Spa_Attack", PLAYER_GROUP);
			Group_LockTask("Ascold_Spa_Attack");
            // ==> прерывание на убиение эскадры
            pchar.quest.Ascold_VictoryManowar.win_condition.l1 = "Group_Death";
			pchar.quest.Ascold_VictoryManowar.win_condition.l1.group = "Ascold_Spa_Attack";
			pchar.quest.Ascold_VictoryManowar.win_condition = "Ascold_VictoryManowar";
			SetTimerCondition("Ascold_ManowarRepair", 0, 0, 10, true);
        break;

        case "Ascold_VictoryManowar":
			PChar.quest.Ascold_ManowarRepair.over = "yes";
			LocatorReloadEnterDisable("Tenotchitlan_Jungle_02", "reload3_back", false); //откроем храм
            group_DeleteGroup("Ascold_Spa_Attack");
            pchar.questTemp.Ascold = "Ascold_ManowarsDead";
            DeleteAttribute(PChar, "questTemp.Ascold.Ship");
            AddQuestRecord("Ascold", "9");
            Pchar.quest.Ascold_FightNearTemple.win_condition.l1 = "location";
            Pchar.quest.Ascold_FightNearTemple.win_condition.l1.location = "Temple";
            Pchar.quest.Ascold_FightNearTemple.win_condition = "Ascold_FightNearTemple";
        break;

        case "Ascold_ManowarRepair": //ремонт мановаров
			if (pchar.location != "Beliz")
			{
				for (i=1; i<=10; i++)
				{
					iTemp = GetCharacterIndex("AscoldCaptainAttack_" + i);
					if (iTemp != -1)
					{
						sld = &characters[iTemp];
						SetCrewQuantityFull(sld);
						DeleteAttribute(sld, "ship.sails");// убрать дыры на парусах
						DeleteAttribute(sld, "ship.blots");
						DeleteAttribute(sld, "ship.masts");// вернуть сбитые мачты
						SetBaseShipData(sld);
					}
				}
			}
        break;

        case "Ascold_FightNearTemple":
            chrDisableReloadToLocation = true; // закрыть выход из локации
            bDisableFastReload = true; // закрыть переходы.
			LAi_group_Delete("EnemyFight");
            for (i=1; i<=20; i++)
            {
                if (i==1 || i==8 || i==11 || i==15 ) 
                {					
					sld = GetCharacter(NPC_GenerateCharacter("Enemy_"+i, "off_spa_"+(rand(1)+1), "man", "man", 25, SPAIN, 0, true));	
					FantomMakeCoolFighter(sld, 25, 80, 80, BLADE_LONG, "pistol5", 100);
                }
				else
				{
					sld = GetCharacter(NPC_GenerateCharacter("Enemy_"+i, "sold_spa_"+(rand(7)+1), "man", "man", 20, SPAIN, 0, true));	
					FantomMakeCoolFighter(sld, 20, 50, 50, BLADE_LONG, "pistol3", 40);
				}
            	LAi_SetWarriorType(sld);				
				if (i < 16) LAi_warrior_SetStay(sld, true);
				LAi_group_MoveCharacter(sld, "EnemyFight");				
               	ChangeCharacterAddressGroup(sld, "Temple", "goto",  "goto"+i);		
            }
            LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
            //LAi_group_FightGroups("EnemyFight", LAI_GROUP_PLAYER, true);
            LAi_group_SetCheck("EnemyFight", "Ascold_WinTemple");
        break;

        case "Ascold_WinTemple":
            chrDisableReloadToLocation = false;
            bDisableFastReload = false;
            characters[GetCharacterIndex("Ascold")].dialog.currentnode = "Ascold_KilledTemple";
            AddQuestRecord("Ascold", "11");
        break;

        case "Ascold_OpenTheGrave":
			DoReloadCharacterToLocation("Guadeloupe_Cave", "reload", "reload5");	     
		break;
        
        case "Ascold_InGraveAfterFight":
			pchar.questTemp.Ascold = "Ascold_MummyIsLive";
		    LAi_LocationFightDisable(&Locations[FindLocation("Guadeloupe_Cave")], true); 
			sld = GetCharacter(NPC_GenerateCharacter("LeifEricson", "Mummy", "man", "man", 100, PIRATE, -1, true));	
			FantomMakeCoolFighter(sld, 100, 100, 100, "blade28", "", 3000);
			sld.name = "Лейф";
			sld.lastname = "Эриксон";
			sld.Dialog.Filename = "Quest\LeifEricson.c";
            sld.SaveItemsForDead = true; // сохранять на трупе вещи
			GiveItem2Character(sld, "Azzy_bottle");
            sld.DontClearDead = true; // не убирать труп через 200с
			ChangeCharacterAddressGroup(sld, pchar.location, "goto",  "locator1");
			LAi_SetImmortal(sld, true);
			LAi_SetActorType(pchar);
			LAi_SetActorType(sld);
			SetActorDialogAny2Pchar("LeifEricson", "", -1, 0.0);
		    LAi_ActorFollow(PChar, sld, "ActorDialog_Any2Pchar", 0.0);
        break;

        case "Ascold_MummyFightTown":
            for (i=1; i<=6; i++)
            {
				sld = characterFromId("MySkel"+i);	
				sld.LifeDay = 0;
				ChangeCharacterAddress(sld, "none", "");
            }
    	    Pchar.GenQuestFort.fortCharacterIdx = GetCharIDXForTownAttack(pchar.location);
    	    DeleteQuestAttribute("Union_with_Escadra");
            sld = GetCharacter(sti(Pchar.GenQuestFort.fortCharacterIdx));
            SetLocationCapturedState("BasTer_town", true);
            DoQuestCheckDelay("Capture_Forts", 0.5);
            Ship_NationAgressive(sld, sld);
            Log_SetStringToLog("За свое тело я иcкромсаю всех лягушатников на этом острове!");
			characters[GetCharacterIndex("BasTer_Mayor")].dialog.captureNode = "Ascold_MummyAttack"; //капитулянтская нода мэра
        break;

        case "Ascold_AzzyIsFree":
  		    LAi_LocationFightDisable(&Locations[FindLocation("Shore28")], true);
            ChangeCharacterAddressGroup(&characters[GetCharacterIndex("Azzy")], "Shore28", "goto", "goto25");
            DoReloadCharacterToLocation("Shore28","goto","goto24");
            DoQuestCheckDelay("Azzy_IsFree", 4.0);
        break;

        case "Azzy_IsFree":
            LAi_SetActorType(PChar);
            sld = characterFromID("Azzy");
            LAi_ActorTurnToCharacter(sld, pchar);
            SetActorDialogAny2Pchar(sld.id, "", -1, 0);
		    LAi_ActorFollow(pchar, sld, "ActorDialog_Any2Pchar", 0.0);
		break;

		case "Azzy_PlusSkill":
            DeleteAttribute(pchar, "questTemp.Azzy.AddSpecial");
            // boal оптимизация скилов -->
            DelBakSkillAttr(pchar);
            ClearCharacterExpRate(pchar);
            RefreshCharacterSkillExpRate(pchar);
            // boal оптимизация скилов <--
        	WaitDate("",0,0,2,0,1);
        	RecalculateJumpTable();
        	StoreDayUpdate();
			SetLaunchFrameFormParam("В отрубе...", "", 0, 4);
			LaunchFrameForm();
            //DoReloadCharacterToLocation("Tortuga_town", "reload", "reload91");
		break;
/////////////////////////////////////////////////////////////////////////////////////////////////////////
////  Квест Аскольда-Аззи конец
/////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////
////  Пиратская линейка     начало
/////////////////////////////////////////////////////////////////////////////////////////////////////////
		case "PiratesLine_toTavern":
			chrDisableReloadToLocation = true;
			bDisableFastReload = true; 
			FreeSitLocator("PuertoPrincipe_tavern", "sit2");
			DoQuestReloadToLocation("PuertoPrincipe_tavern", "sit", "sit2", "");			
			sld = characterFromID("QuestPirate1");        
			LAi_SetActorType(sld);
			LAi_ActorSetSitMode(sld);
			LAi_ActorDialogDelay(sld, pchar, "", 1.5);		
		break;

		case "PiratesLine_q1_MorganGoTo":
            sld = characterFromID("Henry Morgan");
			sld.Dialog.CurrentNode = "PL_Q1_1";
            LAi_SetActorTypeNoGroup(sld);
			LAi_ActorDialog(sld, pchar, "", 0, 0);
		break;

		case "PQ5_Morgan_2":
			LAi_SetActorType(pchar);
			LAi_ActorTurnToCharacter(pchar, characterFromID("Henry Morgan"));
			sld = characterFromID("CapGoodly"); 
			ChangeCharacterAddressGroup(sld, "PortRoyal_houseS1", "reload", "reload1");
			LAi_SetActorType(sld);
			LAi_ActorGoToLocator(sld, "goto", "goto1", "PQ5_Morgan_3", -1.0);
		break;

		case "PQ5_Morgan_3":
			SetMainCharacterIndex(GetCharacterIndex("CapGoodly"));
			PChar   = GetMainCharacter();			
			locCameraToPos(2.49, 2.65, -11.076, false);
			LAi_SetActorType(PChar);
			LAi_ActorDialog(PChar, characterFromID("Henry Morgan"), "", 0, 0);
		break;

		case "PQ6_afterBattle":
			//убираем Джона Лидса
			group_DeleteGroup("LidsGroup");
			LocatorReloadEnterDisable("Shore53", "boat", false); //откроем выход в море
			SetQuestHeader("Pir_Line_6_Jackman");
			AddQuestRecord("Pir_Line_6_Jackman", "4");
			QuestSetCurrentNode("Jackman", "PL_Q6_after");
		break;
		

/////////////////////////////////////////////////////////////////////////////////////////////////////////
////  Пиратская линейка     конец
/////////////////////////////////////////////////////////////////////////////////////////////////////////

		case "CitizSeekCap_afterCabinFight":
			sld = GetCharacter(NPC_GenerateCharacter(pchar.GenQuest.CitizSeekCap.label + "_" + pchar.GenQuest.CitizSeekCap.WifeCity, pchar.GenQuest.CitizSeekCap.model, pchar.GenQuest.CitizSeekCap.sex, pchar.GenQuest.CitizSeekCap.ani, 5, pchar.GenQuest.CitizSeekCap.nation, -1, false));
			sld.name = pchar.GenQuest.CitizSeekCap.WifeName;
			sld.lastname = pchar.GenQuest.CitizSeekCap.WifeLastname;
			sld.dialog.filename   = "Quest\ForAll_dialog.c";
			sld.dialog.currentnode = pchar.GenQuest.CitizSeekCap.label + "_Board";
			sld.quest.SeekCap = pchar.GenQuest.CitizSeekCap.label;
			sld.quest.cribCity = pchar.GenQuest.CitizSeekCap.WifeCity;
			DeleteAttribute(pchar, "GenQuest.CitizSeekCap");
			LAi_SetStayType(sld);
			GetCharacterPos(pchar, &locx, &locy, &locz);
			ChangeCharacterAddressGroup(sld, pchar.location, "rld", LAi_FindFarLocator("rld", locx, locy, locz));
			LAi_SetActorType(pchar);
			LAi_SetActorType(sld);
			SetActorDialogAny2Pchar(sld.id, "", 0.0, 0.0);
			LAi_ActorFollow(pchar, sld, "ActorDialog_Any2Pchar", -1);
		break;

		case "PQ8_jungle_afterFight":
			chrDisableReloadToLocation = false;
			pchar.questTemp.piratesLine = "Panama_inJungleIsOver";
			iTemp = GetCharacterIndex("Richard_Soukins");
			if(iTemp != -1 && !LAi_IsDead(&characters[iTemp]))
			{
				sld = &characters[iTemp];
				LAi_group_MoveCharacter(sld, LAI_GROUP_PLAYER_OWN);
				for (i=1; i<=sti(pchar.questTemp.piratesLine.crewRichard); i++)
				{
					iTemp = GetCharacterIndex("RSpirate_"+i);
					if(iTemp != -1)
					{
						LAi_group_MoveCharacter(&characters[iTemp], sld.chr_ai.group);
					}
				}
				for (i=1; i<=sti(pchar.questTemp.piratesLine.crewRichard); i++)
				{
					iTemp = GetCharacterIndex("RSmush_"+i);
					if(iTemp != -1)
					{
						LAi_group_MoveCharacter(&characters[iTemp], sld.chr_ai.group);
					}
				}
				for (i=1; i<=3; i++)
				{
					iTemp = GetCharacterIndex("RSofficer_"+i);
					if(iTemp != -1)
					{
						LAi_group_MoveCharacter(&characters[iTemp], sld.chr_ai.group);
					}
				}
			}
		break;

		case "PQ8_ExitTown_afterFight":
			LocatorReloadEnterDisable("Shore48", "boat", false); //откроем выход из бухты
			LocatorReloadEnterDisable("PortoBello_Jungle_01", "reload1_back", false); //открываем подходы к Панаме
			SetLocationCapturedState("Panama_town", true); //убираем всех из города
			sld = characterFromId("Henry Morgan");
			sld.dialog.currentNode = "PL_Q8_ExTPanama";
			ChangeCharacterAddressGroup(sld, pchar.location, "reload", "reload1");
			LAi_SetActorType(sld);
			LAi_group_MoveCharacter(sld, LAI_GROUP_PLAYER);
			LAi_ActorDialog(sld, pchar, "", -1, 0);
		break;

		case "NarvalDestroyed":
			pchar.questTemp.LSC = "NarvalDestroyed";
			AddQuestRecord("ISS_MainLine", "13");
		break;

		//homo  Линейка Блада
		//case "CureLord":
        //    CureLord();
		//break;
		
		case "CureLordMovie_Con1":
            CureLordMovie_Con1();
		break;
		
		case "CureLordMovie_Con2":
            CureLordMovie_Con2();
		break;
		
		case "CureLordMovie_Con3":
            CureLordMovie_Con3();
		break;
		
		case "CapBloodLineInit":
            CapBloodLineInit("");
		break;
		
		case "CapBlood_CheckMinHP":
            LAi_group_SetRelation("TmpEnemy", LAI_GROUP_PLAYER, LAI_GROUP_NEITRAL);
            LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_NEITRAL);
            InterfaceStates.Buttons.Save.enable = false;
            InterfaceStates.Launched = true;
            CharacterPlayAction(Pchar, "death_0");
            DoQuestFunctionDelay("Blood_StartGame_End", 2.0);
            //CapBloodLineInit();
		break;

		case "Dragun_0_CheckMinHP":
            for (i=0; i<=2; i++)
            {
                sld = characterFromID("Dragun_"+i);
                if (i == 0)
                {
                    LAi_RemoveCheckMinHP(sld);
                    LAi_KillCharacter(sld);
                }
                else
                {
                    string sQuest = "CapGobartAttack_"+i;
                    pchar.quest.(sQuest).win_condition.l1 = "NPC_Death";
                    pchar.quest.(sQuest).win_condition.l1.character = sld.id;
                    pchar.quest.(sQuest).function= "CapGobartAttack";
                    LAi_SetImmortal(sld, false);
                    LAi_SetWarriorType(sld);
                    LAi_group_MoveCharacter(sld, "TmpEnemy");
                }
            }
            //LAi_group_FightGroups("TmpEnemy", LAI_GROUP_PLAYER, true);
		break;

		//homo



	}	
}

// boal -->
////////////////////////   общие методы //////////////////////////
void SetActorDialogAny2Pchar(string _charId, string _doQuest, float time1, float time2)
{
    Pchar.AutoQuest.NPCharId = _charId;
    Pchar.AutoQuest.Quest    = _doQuest;
    Pchar.AutoQuest.Time1    = time1;
    Pchar.AutoQuest.Time2    = time2;
}

//для универсализации (navy 17.10.05)
//уcтановки для телепорта ГГ в любую локацию с задержкой или без. 
//Для отложенного вызова напр. AddDialogExitQuest()
void SetAnyReloadToLocation(string idLocation, string idGroup, string idLocator, string questName, int delayMonth, int delayDay, int delayHour, int delayMin)
{
    ref PChar = GetMainCharacter();

	PChar.AutoQuest.delayMonth	= delayMonth;
	PChar.AutoQuest.delayDay	= delayDay;
	PChar.AutoQuest.delayHour	= delayHour;
	PChar.AutoQuest.delayMin	= delayMin;

	PChar.AutoQuest.idLocation	= idLocation;
	PChar.AutoQuest.idGroup		= idGroup;
	PChar.AutoQuest.idLocator	= idLocator;
	PChar.AutoQuest.questName	= questName;
}

// Инициация таблички В это время на Беде  -->
// _FrameText - текст  Перенос строки делать "текст"+ NewStr() +"текст"
// _FrameQuest - квест после завершения таблички, если == "Reload_To_Location" см. SetLaunchFrameReloadLocationParam
// если же будет == "Run_Function" см. SetLaunchFrameRunFunctionParam
// _delay - время в с. отложения вызова таблички (нужно для неконфликта с заставкой загрузки локаций, где-то 0.1-1.5с)
// _autoEnd - время в с. автозавершения показа таблички, 4-6с на фразу
void SetLaunchFrameFormParam(string _FrameText, string _FrameQuest, float _delay, float _autoEnd)
{
    pchar.AutoQuest.FrameText    = _FrameText;
    pchar.AutoQuest.FrameQuest   = _FrameQuest;
    pchar.AutoQuest.FrameDelay   = _delay;
    pchar.AutoQuest.FrameAutoEnd = _autoEnd;
}

void SetLaunchFrameFormPic(string _FramePic)
{
    pchar.AutoQuest.FramePic    = _FramePic;
}
// метод для инициации перехода после таблички
void SetLaunchFrameReloadLocationParam(string idLocation, string idGroup, string idLocator, string questName)
{
    pchar.AutoQuest.FrameLocation      = idLocation;
    pchar.AutoQuest.FrameGroup         = idGroup;
    pchar.AutoQuest.FrameLocator       = idLocator;
    pchar.AutoQuest.FrameLocationQuest = questName;
}
// метод для вызова метода из кода после таблички _func - метод, _delay - задержка вызова метода после таблички в с.
void SetLaunchFrameRunFunctionParam(string _func, float _delay)
{
    pchar.AutoQuest.FrameFunction      = _func;
    pchar.AutoQuest.FrameFunctionDelay = _delay;
}
// Инициация таблички В это время на Беде  <--

void RemovePatent()
{
	RemoveCharacterEquip(pchar, "patent");
    if (CheckCharacterItem(Pchar, "patent_eng"))
    {
        TakeItemFromCharacter(pchar, "patent_eng");
        ChangeCharacterReputation(pchar, -10);
    }
    if (CheckCharacterItem(Pchar, "patent_fra"))
    {
        TakeItemFromCharacter(pchar, "patent_fra");
        ChangeCharacterReputation(pchar, -10);
    }
    if (CheckCharacterItem(Pchar, "patent_spa"))
    {
        TakeItemFromCharacter(pchar, "patent_spa");
        ChangeCharacterReputation(pchar, -10);
    }
    if (CheckCharacterItem(Pchar, "patent_hol"))
    {
        TakeItemFromCharacter(pchar, "patent_hol");
        ChangeCharacterReputation(pchar, -10);
    }
}

void StartActorSelfDialog(string _CurrentNode)
{
    LAi_SetActorType(pchar);
    LAi_CharacterSaveAy(pchar);
    locCameraSleep(true);
    if (stf(pchar.chr_ai.type.ay) > 0)
    {
    	CharacterTurnAy(pchar,  -PI + abs(stf(pchar.chr_ai.type.ay)));  // 180 == 1
    }
    else
    {
    	CharacterTurnAy(pchar,  PI - abs(stf(pchar.chr_ai.type.ay)));  // 180 == 1
    }
    pchar.Dialog.CurrentNode = _CurrentNode;
    LAi_ActorSelfDialog(pchar, "pchar_back_to_player");
}

// подъем флагов
void Flag_PIRATE()
{
	PChar.nation	= PIRATE;
	Ship_FlagRefresh(PChar); //флаг на лету
	SetNationToOfficers(PIRATE);
	
	SetNationRelation2MainCharacter(ENGLAND, RELATION_ENEMY);
	SetNationRelation2MainCharacter(FRANCE, RELATION_ENEMY);
	SetNationRelation2MainCharacter(SPAIN, RELATION_ENEMY);
	
	SetNationRelation2MainCharacter(PIRATE, RELATION_FRIEND);
	SetNationRelation2MainCharacter(HOLLAND, RELATION_ENEMY);
	
	LAi_group_SetRelation("PIRATE_CITIZENS", LAI_GROUP_PLAYER, LAI_GROUP_NEITRAL);
	//Relation_PIRATE_SOLDIERS();
	
	LAi_group_ClearAllTargets();
	DoQuestCheckDelay("NationUpdate", 3.0);
	UpdateRelations();
	if(bSeaActive)
	{
	    RefreshBattleInterface();
	}
}            

void Flag_FRANCE()
{
	PChar.nation	= FRANCE;
    Ship_FlagRefresh(PChar); //флаг на лету
    SetNationToOfficers(FRANCE);

    SetNationRelation2MainCharacter(ENGLAND, GetNationRelation(ENGLAND, FRANCE));
    SetNationRelation2MainCharacter(FRANCE, RELATION_FRIEND);
    SetNationRelation2MainCharacter(SPAIN, GetNationRelation(FRANCE, SPAIN));
    SetNationRelation2MainCharacter(PIRATE, RELATION_ENEMY);

    SetNationRelation2MainCharacter(HOLLAND, GetNationRelation(FRANCE, HOLLAND));

    LAi_group_SetRelation("FRANCE_CITIZENS", LAI_GROUP_PLAYER, LAI_GROUP_NEITRAL); //LAI_GROUP_FRIEND);
    //Relation_FRANCE_SOLDIERS();
    LAi_group_ClearAllTargets();
    DoQuestCheckDelay("NationUpdate", 3.0);
    UpdateRelations();
    if(bSeaActive)
    {
        RefreshBattleInterface();
    }
}

void Flag_ENGLAND()
{
	PChar.nation	= ENGLAND;
    Ship_FlagRefresh(PChar); //флаг на лету
    SetNationToOfficers(ENGLAND);

	SetNationRelation2MainCharacter(ENGLAND, RELATION_FRIEND);
	SetNationRelation2MainCharacter(FRANCE, GetNationRelation(ENGLAND, FRANCE));
	SetNationRelation2MainCharacter(SPAIN, GetNationRelation(ENGLAND, SPAIN));
	SetNationRelation2MainCharacter(PIRATE, RELATION_ENEMY);
    SetNationRelation2MainCharacter(HOLLAND, GetNationRelation(ENGLAND, HOLLAND));

    LAi_group_SetRelation("ENGLAND_CITIZENS", LAI_GROUP_PLAYER, LAI_GROUP_NEITRAL);
    //Relation_ENGLAND_SOLDIERS();
    LAi_group_ClearAllTargets();
    DoQuestCheckDelay("NationUpdate", 3.0);
    UpdateRelations();
    if(bSeaActive)
    {
        RefreshBattleInterface();
    }
}

void Flag_SPAIN()
{
	PChar.nation = SPAIN;
    Ship_FlagRefresh(PChar); //флаг на лету
    SetNationToOfficers(SPAIN);

	SetNationRelation2MainCharacter(ENGLAND, GetNationRelation(ENGLAND, SPAIN));
	SetNationRelation2MainCharacter(FRANCE, GetNationRelation(FRANCE, SPAIN));
	SetNationRelation2MainCharacter(SPAIN, RELATION_FRIEND);
	SetNationRelation2MainCharacter(PIRATE, RELATION_ENEMY);
    SetNationRelation2MainCharacter(HOLLAND, GetNationRelation(SPAIN, HOLLAND));

    LAi_group_SetRelation("SPAIN_CITIZENS", LAI_GROUP_PLAYER, LAI_GROUP_NEITRAL);
    //Relation_SPAIN_SOLDIERS();
    LAi_group_ClearAllTargets();
    DoQuestCheckDelay("NationUpdate", 3.0);
    UpdateRelations();
    if(bSeaActive)
    {
        RefreshBattleInterface();
    }
}

void Flag_HOLLAND()
{
	PChar.nation	= HOLLAND;
    Ship_FlagRefresh(PChar); //флаг на лету
    SetNationToOfficers(HOLLAND);

    SetNationRelation2MainCharacter(ENGLAND, GetNationRelation(ENGLAND, HOLLAND));
    SetNationRelation2MainCharacter(FRANCE, GetNationRelation(FRANCE, HOLLAND));
    SetNationRelation2MainCharacter(SPAIN, GetNationRelation(SPAIN, HOLLAND));
    SetNationRelation2MainCharacter(PIRATE, RELATION_ENEMY);
    SetNationRelation2MainCharacter(HOLLAND, RELATION_FRIEND);

    LAi_group_SetRelation("HOLLAND_CITIZENS", LAI_GROUP_PLAYER, LAI_GROUP_NEITRAL);
    //Relation_HOLLAND_SOLDIERS();
    LAi_group_ClearAllTargets();
    DoQuestCheckDelay("NationUpdate", 3.0);
    UpdateRelations();
    if(bSeaActive)
    {
        RefreshBattleInterface();
    }
}

void Flag_Rerise()
{
	switch (sti(PChar.nation))
	{
    	case ENGLAND:	Flag_ENGLAND();	break;
    	case FRANCE:	Flag_FRANCE();	break;
    	case SPAIN:		Flag_SPAIN();	break;
    	case PIRATE:	Flag_PIRATE();	break;
    	case HOLLAND:	Flag_HOLLAND();	break;
	}
}
