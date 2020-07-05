// Блад
void ProcessDialogEvent()
{
	ref NPChar, sld;
	aref Link, NextDiag;
    string sLocator;
    int iTime;

	DeleteAttribute(&Dialog,"Links");

	makeref(NPChar,CharacterRef);
	makearef(Link, Dialog.Links);
	makearef(NextDiag, NPChar.Dialog);




	switch(Dialog.CurrentNode)
	{

		// ----------------------------------- Диалог первый - первая встреча
		case "Exit":
            NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
		
        case "Exit_Away":
            
            LAi_SetActorType(npchar);
            LAi_ActorGoToLocation(npchar, "reload", Pchar.questTemp.sLocator, "none", "", "", "", sti(Pchar.questTemp.iTime));
            NextDiag.CurrentNode = NextDiag.TempNode;
            DialogExit();
        break;
        
        case "Exit_RunAway":

            LAi_SetActorTypeNoGroup(npchar);
            LAi_ActorRunToLocation(npchar, "reload", Pchar.questTemp.CapBloodLine.sLocator, "none", "", "", "", sti(Pchar.questTemp.CapBloodLine.iTime));
            NextDiag.CurrentNode = NextDiag.TempNode;
            DialogExit();
        break;
        
        case "fight":
			DialogExit();
			NextDiag.CurrentNode = NextDiag.TempNode;
   	        LAi_SetWarriorType(NPChar);
            LAi_group_MoveCharacter(NPChar, "TmpEnemy");
            LAi_SetWarriorType(NPChar);
            /*
            LAi_group_SetHearRadius("TmpEnemy", 100.0);
            LAi_group_FightGroupsEx("TmpEnemy", LAI_GROUP_PLAYER, true, Pchar, -1, false, false);
            LAi_group_SetRelation("TmpEnemy", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
			LAi_group_Attack(NPChar, Pchar);
			*/
			LAi_group_FightGroups("TmpEnemy", LAI_GROUP_PLAYER, true);
			LAi_group_FightGroups(GetNationNameByType(ENGLAND) + "_citizens", LAI_GROUP_PLAYER, true);
			AddDialogExitQuest("MainHeroFightModeOn");
			chrDisableReloadToLocation = false;

		break;

		case "First time":
        	dialog.text = "Ошибка";
            link.l1 = "...";
            link.l1.go = "Exit";
            npchar.quest.meeting = "1";
            

            if (CheckAttribute(npchar, "CityType") && npchar.CityType == "soldier")
            {

                if(Pchar.questTemp.CapBloodLine.stat == "PrepareToEscape2_1")
                {
                    dialog.text = "Проходи своей дорогой, и не отвлекай меня.";
					link.l1 = "Боюсь, дело серьёзное. В город попал испанский лазутчик, возможно даже, наёмник. Он отправился в дом оружейника, и сейчас дверь к Гриффину наглухо закрыта.";
					link.l1.go = "SQStep_0";
					Pchar.questTemp.CapBloodLine.stat = "PrepareToEscape2_2";
				    break;
                }

        		switch (rand(5))
					{
						case 0: ////////////////////////////////////////
							dialog.text = "С чего это раб шатается тут без дела?";
							link.l1 = "Я выполняю поручение полковника Бишопа.";
							link.l1.go = "exit";
						break;

						case 1:
							dialog.text = "А, это Вы, доктор Блад. Вы лечили меня, когда меня списали с ''Прайд оф Дивон''";
							link.l1 = "Я рад, что ты поправился.";
							link.l1.go = "exit";
						break;

						case 2: ///////////////////////////////////////////
							dialog.text = "Пшёл прочь, раб!";
							link.l1 = "Я тоже рад тебя видеть, кто б ты ни был.";
							link.l1.go = "exit";
						break;

						case 3:
							dialog.text = "Какая погода, а я вынужден стоять здесь. Мое единственное развлечение заключается в отгонянии мух от своего лица.";
							link.l1 = "Сочувствую. Но ничем не могу помочь, работа у тебя такая.";
							link.l1.go = "exit";
						break;

						case 4: ///////////////////////////////////////////
							dialog.text = "Если тебе хочется поболтать, то найдите себе кого-нибудь другого, а лучше возвращайся на плантации.";
							link.l1 = "Непременно.";
							link.l1.go = "exit";
						break;



						case 5: ////////////////////////////////////////////
							dialog.text = "Проходи своей дорогой, и не отвлекай меня.";
							link.l1 = "Как скажешь, солдат.";
							link.l1.go = "exit";
						break;


					}
    		}
    		
    		if (CheckAttribute(npchar, "CityType") && npchar.CityType == "citizen")
    		{
            	dialog.text = TimeGreeting() + ", доктор.";
        		link.l1 = "Здраствуйте.";
        		link.l1.go = "Exit";
        		if( Pchar.questTemp.CapBloodLine.stat == "WakerOffer")
        		{
                    link.l2 = RandPhraseSimple("Не подскажете, где мне найти доктора Вакера?", "Мне нужен доктор Вакер. Не видели его?");
                    link.l2.go = "Citizen_0";
                }
    		}
    		
    		if (CheckAttribute(npchar, "CityType") && npchar.CityType == "citizen" && npchar.location.group == "merchant")
    		{
                if(findsubstr(Pchar.questTemp.CapBloodLine.stat, "PrepareToEscape" , 0) != -1)
                {
                        if (CheckAttribute(npchar, "quest.bGoodMerch") && sti(Pchar.reputation) >= 55)
                        {
                            dialog.text = "О, доктор Блад, здравствуйте! Я так и не поблагодарил"+NPCharSexPhrase(NPChar, " ", "а ")+" Вас тогда за спасение моего маленького сына, о чём до сих пор часто вспоминаю и сожалею. Как славно, что я встретил Вас сегодня!";
                            link.l1 = "Действительно, как славно, что люди стремятся отблагодарить в самый подходящий момент. Не подскажешь ли, "+NPCharSexPhrase(NPChar, "дорогой друг ", "моя дорогая ")+", где я могу в кратчайшие сроки разжиться несколькими абордажными саблями и парой-тройкой пистолетов без лишнего шума и возни?";
                            link.l1.go = "Merchant_0";
                            break;
                        }
                }
                if (CheckAttribute(npchar, "quest.bGoodMerch"))
                {
                    dialog.text = TimeGreeting() + ", доктор.";
                    link.l1 = "Здраствуйте.";
                }
                else
                {
                    dialog.text = "Ты мне товар загораживаешь. А ну пшёл, падаль рабская!";
                    link.l1 = "Уже ухожу.";
                }
        		link.l1.go = "Exit";
    		}
    		
    		if (npchar.id == "Bridgetown_Poorman")
    		{
            	dialog.text = "Эй, парень подкинь монетку... А это Вы, доктор Блад.";
        		link.l1 = "Удачи.";
        		link.l1.go = "Exit";
    		}
    		
    		if (npchar.id == "QuestCitiz_Bridgetown")
    		{
            	dialog.text = TimeGreeting() + ", доктор.";
        		link.l1 = "Доброе.";
        		link.l1.go = "Exit";
    		}
    		
    		if (npchar.id == "SolderTakeBlades")
    		{
            	dialog.text = RandPhraseSimple("С каких это пор рабы разгуливают, где попало с оружием?", "Ты же раб полковника Бишопа, почему ты вооружен?");
        		link.l1 = RandPhraseSimple("Эээ...", "Сам не пойму...");
        		link.l1.go = "STBStep_0";
    		}
    		
            
            
		break;
		
		
		case "Citizen_0":
		
            if (!CheckAttribute(npchar, "quest.btmp"))
            {
                npchar.quest.btmp = true;
                if(rand(3)==2)
                {
                    dialog.text = LinkRandPhrase("Рысью в таверну пробежал - там ищи. Не забудь смотреть под столами.", "Недавно зашёл в таверну, вроде. Если правильно помню, не выходил.", "Заскочил в кабак только что... с похмелья, что ли?");
                    link.l1 = "Спасибо.";
                    link.l1.go = "Exit";
                }
                else
                {
                    dialog.text = RandPhraseSimple(RandPhraseSimple("Не знаю, сгинь!", "Ничем не могу помочь."), RandPhraseSimple("А ну пшёл, падаль рабская!", "А кто это? Я знаю только доктора Блада... а, это Вы, здравствуйте."));
                    link.l1 = "Спасибо.";
                    link.l1.go = "Exit";
                }
                
            }
            else
            {
                dialog.text = "Доктор, вы уже спрашивали у меня об этом.";
                link.l1 = "Извините.";
                link.l1.go = "Exit";
            }

		break;
		
		case "Merchant_0":
		
        	dialog.text = "Я не буду спрашивать, для чего это Вам понадобилось, и если меня спросят, мне не придётся лгать и выкручиваться. Тем не менее, отчёт с меня попросят не ранее чем послезавтра, так что...";
    		link.l1 = "Послезавтра Вы уже сможете смело заявить, что предоставили в безвозмездное пользование несколько сабель и пистолетов доктору Питеру Бладу, что был в Бриджтауне доктором. Вряд ли полковник Бишоп будет против...";
    		link.l1.go = "Merchant_1";
    		
		break;
		
		case "Merchant_1":

        	dialog.text = "Мой сын обязан Вам жизнью, впрочем, как и я. Вот, возьмите, доктор Блад. И удачи Вам!";
    		link.l1 = "Премного благодарен.";
    		link.l1.go = "Exit";
            NextDiag.TempNode = "Merchant_2";
            Pchar.questTemp.CapBloodLine.stat = "ReadyToEscape";
            GiveItem2Character(Pchar, "Weapon_for_escape");
            AddQuestRecord("WeaponsForEscape", "10");
    		CloseQuestHeader("WeaponsForEscape");

		break;
		
		case "Merchant_2":

        	dialog.text = "И удачи Вам, доктор Блад!";
    		link.l1 = "Благодарю.";
    		link.l1.go = "Exit";
    		NextDiag.TempNode = "Merchant_2";

		break;
		
		
		// ==> Забираем клинки, пистоли.
		case "STBStep_0":
        	dialog.text = "А ну, немедленно сдай  все оружие! Следует сообщить полковнику Бишопу, чтобы он всыпал тебе как следует...";
    		link.l1 = "Вот возьмите, и забудем об этом.";
    		link.l1.go = "STBStep_1";
            link.l2 = "Сейчас мы посмотрим, кто кому всыпет!";
            link.l2.go = "fight";
		break;

		case "STBStep_1":
		
                dialog.text = "А ладно, проваливай.";
                link.l1.go = "Exit_Away";
                Pchar.questTemp.sLocator = "reload1_back";
                Pchar.questTemp.iTime = 40;
                RemoveCharacterEquip(pchar, BLADE_ITEM_TYPE);
                RemoveCharacterEquip(pchar, GUN_ITEM_TYPE);
                while (FindCharacterItemByGroup(pchar, BLADE_ITEM_TYPE) != "" && FindCharacterItemByGroup(pchar, BLADE_ITEM_TYPE) != "blade5")
                {
                    TakeItemFromCharacter(pchar, FindCharacterItemByGroup(pchar, BLADE_ITEM_TYPE));

                }
                while (FindCharacterItemByGroup(pchar, GUN_ITEM_TYPE) != "")
                {
                    TakeItemFromCharacter(pchar, FindCharacterItemByGroup(pchar, GUN_ITEM_TYPE));
                }
                GiveItem2Character(Pchar, "unarmed");
                EquipCharacterByItem(Pchar, "unarmed");
                
                //свободен
                pchar.quest.CapBloodDetectBlades1.over = "yes";
                pchar.quest.CapBloodDetectBlades2.over = "yes";
                chrDisableReloadToLocation = false;

		break;
		

        // --> Квестовый солдат


 		case "SQStep_0":

            dialog.text = "Испанец? Да ты издеваешься!.. Испанец не мог... или...?";
        	link.l1 = "Уверяю Вас, это был испанец.";
            link.l1.go = "SQStep_1";
		break;
		
 		case "SQStep_1":

            dialog.text = "Я немедленно отправляюсь туда. Где находится этот дом?";
        	link.l1 = "Это дом мистера Гриффина, он неподалёку от усадьбы губернатора.";
            link.l1.go = "Exit_RunAway";
            Pchar.questTemp.CapBloodLine.sLocator = "houseSp2";
            Pchar.questTemp.CapBloodLine.iTime = 20;
            
            string smodel = NPChar.model;
            if (findsubstr(smodel, "eng_mush" , 0) != -1) smodel = "sold_eng_"+(rand(7)+1);
            sld = GetCharacter(NPC_GenerateCharacter("CPBQuest_Solder", smodel, "man", "man", 10, ENGLAND, 1, false));
            sld.dialog.filename = "Quest\CapBloodLine\questNPC.c";
            SetFantomParamHunter(sld); //крутые парни
            sld.SaveItemsForDead = true; // сохранять на трупе вещи
            sld.DontClearDead = true;
            LAi_SetActorTypeNoGroup(sld);
            LAi_ActorSetLayMode(sld);
            LAi_SetImmortal(sld, true);
			ChangeCharacterAddressGroup(sld, "CommonFlamHouse", "reload","reload3");

            sld = &characters[GetCharacterIndex("Griffin")];
            ChangeCharacterAddressGroup(sld, "CommonRoom_MH4", "barmen","bar1");
            LAi_SetActorTypeNoGroup(sld);
            LAi_ActorTurnToLocator(sld, "goto","goto2");
            LAi_SetStayTypeNoGroup(sld);
            sld = &characters[GetCharacterIndex("Spain_spy")];
            ChangeCharacterAddressGroup(sld, "CommonRoom_MH4", "goto","goto2");
            sld.dialog.currentnode = "SSStep_3";
            LAi_SetActorType(sld);
            LAi_ActorDialog(sld, pchar, "", 0, 0);

            
            pchar.quest.PrepareToEscape2_3.win_condition.l1          = "location";
            pchar.quest.PrepareToEscape2_3.win_condition.l1.location = "CommonFlamHouse";
            pchar.quest.PrepareToEscape2_3.function                  = "_DeadSolder";

            Pchar.questTemp.CapBloodLine.stat = "PrepareToEscape2_3";
            DoQuestFunctionDelay("SpainSpyAttack", 20.0);
            
		break;




	}
}