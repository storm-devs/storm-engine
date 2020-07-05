// BOAL диалог пленников - в трюме
void ProcessDialogEvent()
{
	ref NPChar;
	aref Link, NextDiag;

	DeleteAttribute(&Dialog,"Links");

	makeref(NPChar,CharacterRef);
	makearef(Link, Dialog.Links);
	makearef(NextDiag, NPChar.Dialog);

	ref offref;
    int i, cn;
    int qty;
    string attrLoc;
    
	switch(Dialog.CurrentNode)
	{
		// -----------------------------------Диалог первый - первая встреча
		case "First time":
			if (sti(NPChar.nation) == PIRATE)
			{
                dialog.text = RandSwear() + "Я Ваш пленник, " + GetAddress_Form(NPChar) + ". Но должен сказать, что " + NationNameNominative(sti(NPChar.nation)) + " никогда не выкупает своих -  каждый сам за себя.";
				link.l2 = "Нда... На тебе не заработать, только если сдать тебя как преступника в каком-нибудь порту за выкуп. А может взять тебя к себе офицером?";
			    link.l2.go = "free_to_officer";
			}
			else
			{
			    dialog.text = RandSwear() + "Я Ваш пленник, " + GetAddress_Form(NPChar) + ". " + NationNameNominative(sti(NPChar.nation)) + " заплатит хорошую сумму за мою свободу.";
			}
			attrLoc = Sea_FindNearColony();
			if (attrLoc != "none") 
			{
                link.l1 = "Мы сейчас близ города " + GetConvertStr(attrLoc +" Town", "LocLables.txt") + ". Я мог бы тебя тут высадить.";
			    link.l1.go = "free_withoutFee";
			}
			else
			{
                link.l1 = "Я вот подумал и решил проявить милосердие. Я отпускаю тебя на все четыре стороны, " + GetFullName(NPChar) + ". Можешь убираться с моего корабля.";
			    link.l1.go = "free_in_sea";
			}
			link.l3 = "Мне не нужены пленники - мне нужны хорошие рабы!";
			link.l3.go = "Slave_1";
			link.l4 = "Молчать, пока тебя не спросили!!";
			link.l4.go = "PunishmentAction";
			link.l99 = "Я в курсе!";
			link.l99.go = "exit";
			NextDiag.TempNode = "first time";
		break;
		
		case "PunishmentAction":
            DialogExit();
            NextDiag.CurrentNode = "first time";
            pchar.GenQuest.PunishmentActionManType = "LAi_SetStayType";
            Pchar.GenQuest.FreeRandomOfficerIdx = NPChar.index;

            AddDialogExitQuest("PunishmentAction");
            AddCharacterExpToSkill(Pchar, "Leadership", -150);
            ChangeCharacterReputation(pchar, -0.2);
        break;
		
        case "Slave_1":
			dialog.text = "Но " + GetAddress_Form(NPChar) + ", я же сдался на Вашу милость в бою и достоен лучшей участи.";
            link.l1 = "Что!? Это мое дело решать твою участь. Быть тебе рабом!";
			link.l1.go = "Slave_2";
			link.l99 = "Э... Ну ладно, сиди пока в трюме.";
			link.l99.go = "exit";
		break;
		
		case "Slave_2":
            OfficersReaction("bad");
            if (rand(5) == 1)
            {
    			dialog.text = "Нет!! нет!! Я лучше покончу собой прямо здесь, чем стану рабом!";
                link.l1 = "Отставить! Я приказываю! Эй.. кто-нибудь уберите тут эту кучу дерьма.";
    			link.l1.go = "free_in_sea_4";
			}
            else
            {
                if (rand(1) == 1)
                {
                    if (GetPrisonerQty() > 1 && rand(1) == 1)
                    {
                        dialog.text = "А ну-ка, парни, покажем этому 'хозяину' чей это корабль на самом деле!!";
                        link.l1 = "Что!? Бунт!? Отставить!";
            			link.l1.go = "free_in_sea_battle_all";
            			break;
                    }
                    dialog.text = "Я предпочту умереть в бою!";
                    link.l1 = "Я с удовольствием предоставлю тебе такую возможность!";
        			link.l1.go = "free_in_sea_battle_1";
    			}
    			else
    			{
                    dialog.text = "Хорошо. Это ужасно, но мне ничего не остается - я Ваш раб!";
                    link.l1 = "Вот и славно. Добро пожаловать на борт в качестве живого груза.";
        			link.l1.go = "Slave_3";
    			}
			}
		break;
		
		case "Slave_3":
            ReleasePrisoner(NPChar); // освободили пленника
        	LAi_SetActorType(NPChar);
        	LAi_ActorRunToLocation(NPChar, "reload", "reload1", "none", "", "", "", 4.0);
        	ChangeCharacterReputation(pchar, -6);
        	DialogExit();
        	AddCharacterGoodsSimple(pchar, GOOD_SLAVES, 1);
        break;
        
        case "free_in_sea":
			dialog.text = "Но, " + GetAddress_Form(NPChar) + ", мы же в открытом море!!! Куда я пойду?";
            link.l1 = "А мне-то что! Я сказал. Ты свободен. Убирайся прочь!";
			link.l1.go = "free_in_sea_2";
			link.l99 = "Э.. ну ладно, сиди пока в трюме.";
			link.l99.go = "exit";
		break;
		
		case "free_in_sea_2":
			dialog.text = "Это же равносильно моей казни. Вы просто хотите бросить меня на корм акулам?";
            link.l1 = "Мне плевать, доплывешь ты до берега или нет! Вон с моего корабля!";
			link.l1.go = "free_in_sea_3";
			link.l99 = "Пожалуй, ты прав. Оставайся пока.";
			link.l99.go = "exit";
		break;

		case "free_in_sea_3":
            OfficersReaction("bad");
            if (rand(5) == 1)
            {
    			dialog.text = "Нет!! нет!! Я лучше покончу собой прямо здесь, чем буду кормить акул!";
                link.l1 = "Отставить! Я приказываю! Эй.. кто-нибудь уберите тут эту кучу дерьма.";
    			link.l1.go = "free_in_sea_4";
			}
            else
            {
                if (rand(1) == 1)
                {
                    if (GetPrisonerQty() > 1 && rand(1) == 1)
                    {
                        dialog.text = "А ну-ка, парни, покажем этому 'хозяину' чей это корабль на самом деле!!";
                        link.l1 = "Что!? Бунт!? Отставить!";
            			link.l1.go = "free_in_sea_battle_all";
            			break;
                    }
                    dialog.text = "Я предпочту умереть в бою!";
                    link.l1 = "Я с удовольствием предоставлю тебе такую возможность!";
        			link.l1.go = "free_in_sea_battle_1";
    			}
    			else
    			{
                    dialog.text = "Хорошо. Я ухожу. Если мы еще когда-нибудь встретимся я отомщу!";
                    link.l1 = "Ну-ну. Наверное ты прекрасный пловец?";
        			link.l1.go = "free_in_sea_5";
    			}
			}
		break;
		
		case "free_in_sea_battle_1":
            ReleasePrisoner(NPChar); // освободили пленника
        	
        	LAi_LocationFightDisable(&Locations[FindLocation("My_Deck")], false);
            LAi_SetFightMode(Pchar, true);
            LAi_SetWarriorType(NPChar);
            LAi_group_MoveCharacter(NPChar, LAI_GROUP_TmpEnemy);
            LAi_group_SetHearRadius(LAI_GROUP_TmpEnemy, 100.0);
            LAi_group_FightGroupsEx(LAI_GROUP_TmpEnemy, LAI_GROUP_PLAYER, true, Pchar, -1, false, false);
            LAi_group_SetRelation(LAI_GROUP_TmpEnemy, LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
            LAi_group_SetCheck(LAI_GROUP_TmpEnemy, "My_Deck_Battle_End");
            LAi_group_SetRelation(LAI_GROUP_TmpEnemy, "Prisoner", LAI_GROUP_NEITRAL);
        	DialogExit();
        break;
        
        case "free_in_sea_battle_all":
            for(i=0; i < GetPassengersQuantity(pchar); i++)
            {
                cn = GetPassenger(pchar,i);
                if(cn != -1)
                {
                    offref = GetCharacter(cn);
                    if (CheckAttribute(offref,"prisoned"))
                    {
        	            if(sti(offref.prisoned)==true && GetRemovable(offref)) // ставим только фантомов
        	            {
                        	ReleasePrisoner(offref); // освободили пленника
                            LAi_SetWarriorType(offref);
                            LAi_group_MoveCharacter(offref, LAI_GROUP_TmpEnemy);
                        }
                    }
                }
            }
            LAi_LocationFightDisable(&Locations[FindLocation("My_Deck")], false);
            LAi_SetFightMode(Pchar, true);

			LAi_group_SetHearRadius(LAI_GROUP_TmpEnemy, 100.0);
            LAi_group_FightGroupsEx(LAI_GROUP_TmpEnemy, LAI_GROUP_PLAYER, true, Pchar, -1, false, false);
            LAi_group_SetRelation(LAI_GROUP_TmpEnemy, LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
	            
            LAi_group_SetCheck(LAI_GROUP_TmpEnemy, "My_Deck_Battle_End");

        	DialogExit();
        break;
        
		case "free_in_sea_4":
        	ReleasePrisoner(NPChar); // освободили пленника
            ChangeCharacterReputation(pchar, -2);
            NPChar.location = "";
	        LAi_KillCharacter(NPChar);
        	DialogExit();
        break;
        
        case "free_in_sea_5":
            ReleasePrisoner(NPChar); // освободили пленника
        	LAi_SetActorType(NPChar);
        	LAi_ActorRunToLocation(NPChar, "reload", "reload1", "none", "", "", "", 4.0);
        	ChangeCharacterReputation(pchar, -2);
        	DialogExit();
        break;
        
		case "free_withoutFee":
			dialog.text = "О, " + GetAddress_Form(NPChar) + ", Вы столь добры, что отпускаете меня без выкупа?";
            link.l1 = "Да, отпускаю. Ты свободен, " + GetFullName(NPChar);
			link.l1.go = "free_withoutFee_2";
			link.l99 = "Хм.. да ты прав! Я не столь добр, что бы выкидывать живые деньги. Посиди-ка в трюме еще.";
			link.l99.go = "exit";
		break;
		
		case "free_withoutFee_2":
			dialog.text = "Спасибо, "+GetFullName(PChar) + "!!!";
            link.l1 = "Ладно, проваливай, пока я не передумал.";
			link.l1.go = "free_withoutFee_3";
			OfficersReaction("good");
		break;
		
		case "free_withoutFee_3":
            ReleasePrisoner(NPChar); // освободили пленника
        	LAi_SetActorType(NPChar);
        	LAi_ActorRunToLocation(NPChar, "reload", "reload1", "none", "", "", "", 4.0);
        	ChangeCharacterReputation(pchar, 2);
        	DialogExit();
        break;
        
        case "free_to_officer":
            if (rand(12) > GetSummonSkillFromNameToOld(PChar, SKILL_LEADERSHIP) || FindFreeRandomOfficer() < 0)
            {
    			dialog.text = "Преступника? Вы наверное шутите, " + GetAddress_Form(NPChar) + "! Служить у вас? Нет уж, лучше кормить акул!";
                link.l1 = "Но-но! Договоришься, могу ведь так и сделать!";
    			link.l1.go = "exit";
			}
			else
			{
    			dialog.text = "Офицером, " + GetAddress_Form(NPChar) + "? Пожалуй, я согласен, т.к. другого выхода у меня просто нет!";
                link.l1 = "Договорились! Приступай к своим обязанностям немедленно.";
    			link.l1.go = "free_to_officer_Hire";
    			link.l2 = "А зачем ты мне сдался? У меня и так полный комплект.";
    			link.l2.go = "exit";
			}

		break;

        case "free_to_officer_Hire":
            NPChar.Dialog.Filename = "Enc_Officer_dialog.c";
            NPChar.greeting = "Gr_Officer";
            NPChar.loyality = 5 + rand(10);
		    if (sti(NPChar.reputation) > 41)
		    {
		        NPChar.alignment = "good";
		    }
		    else
		    {
		        NPChar.alignment = "bad";
		    }
            ReleasePrisoner(NPChar); // освободили пленника
            DeleteAttribute(NPChar, "LifeDay") // постоянный
			Pchar.questTemp.HiringOfficerIDX = Npchar.index;
			// тут трем накопивщиеся сабли и корабли 290704 BOAL -->
			DeleteAttribute(Npchar, "Ship");
			Npchar.Ship.Type = SHIP_NOTUSED;
            DeleteAttribute(Npchar, "Fellows"); // его офицеры
            Npchar.Fellows.Passengers.id0 = Npchar.index; // свой пассажир

			Npchar.Fellows.Passengers.boatswain = "-1";
			Npchar.Fellows.Passengers.navigator = "-1";
			Npchar.Fellows.Passengers.doctor = "-1";
			Npchar.Fellows.Passengers.cannoner = "-1";
			Npchar.Fellows.Passengers.treasurer = "-1";
			Npchar.Fellows.Passengers.carpenter = "-1";
            
			AddDialogExitQuestFunction("LandEnc_OfficerHired");
			DialogExit();
        break;
        	
		case "Exit":
			DialogExit();
			NextDiag.CurrentNode = NextDiag.TempNode;
		break;
	}
}