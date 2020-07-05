void ProcessDialogEvent()
{	
	ref NPChar, sld;
	aref Link, NextDiag;
	int i, iRank, iMassive;

	DeleteAttribute(&Dialog,"Links");

	makeref(NPChar,CharacterRef);
	makearef(Link, Dialog.Links);
	makearef(NextDiag, NPChar.Dialog);
	
	switch(Dialog.CurrentNode)
	{		
		case "exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;

		case "First time":
			dialog.text = LinkRandPhrase("Господи, как хорошо, что я вас встретила!",
				"Наконец-то хоть кто-то повяился здесь!!",
				"Боже мой, как я рада вас видеть!");
			Link.l1 = LinkRandPhrase("Что случилось, крошка?",
				"Какие проблемы, дорогуша?",
				"Не надо так волноваться, это вредно для женского организма. Расскажи-ка мне быстренько, что там у тебя стряслось?");
			Link.l1.go = "Step_1";			
		break;
		case "Step_1":
			dialog.text = LinkRandPhrase("Моя подруга - такая бесстрашная, она полезла в заброшенный колодец!",
				"Моя подруга полезла в заброшенный колодец и уже несколько часов ее нет!",
				"Моя подруга исчезла в заброшенном колодце!");
			Link.l1 = "Какого черта она полезла туда?!";
			Link.l1.go = "Step_2";			
		break;
		case "Step_2":
			dialog.text = "Ах, она ничего не боиться! Она услышала где-то, что там внизу есть подземелье, в котором можно отыскать золото, вот и полезла. Но я не такая дура, я туда ни ногой...";
			Link.l1 = "И это правильно, не женское это дело.";
			Link.l1.go = "Step_3";			
		break;		
		case "Step_3":
			dialog.text = "Что же мне теперь делать? Прошу вас, ради всего святого, помогите ей!";
			Link.l1 = "Хорошо, я разберусь с этим. А ты иди домой, нечего тут прохлаждаться одной.";
			Link.l1.go = "Step_agree";
			Link.l2 = "Это ваши проблемы, мне до них дела нет. Прощай...";
			Link.l2.go = "Step_disAgree";
		break;

		case "Step_agree":
			pchar.quest.Enc_FriendGirl_after.over = "yes";
			if (rand(1))
			{ //бандюганы
				LAi_LocationDisableMonGenTimer(pchar.GenQuest.EncGirl.locationId, 1); //монстров не генерить 1 день
				LAi_LocationDisableOffGenTimer(pchar.GenQuest.EncGirl.locationId, 1); //офицеров не пускать 1 день
				LocatorReloadEnterDisable(pchar.GenQuest.EncGirl.locationId, "reload2", true);
				pchar.GenQuest.OpenTheRopeExit = pchar.GenQuest.EncGirl.locationId; //флаг для открытия релоада
				string model[10];
				model[0] = "pirate_1";
				model[1] = "pirate_2";
				model[2] = "pirate_3";
				model[3] = "pirate_4";
				model[4] = "pirate_5";
				model[5] = "pirate_6";
				model[6] = "pirate_7";
				model[7] = "pirate_8";
				model[8] = "pirate_9";
				model[9] = "pirate_10";
				i = 0;
				while(i < 3)
				{
					iMassive = rand(9);
					if (model[iMassive] != "")
					{
						iRank = sti(pchar.rank) - rand(5) + rand(5);
						if (iRank < 1) iRank = 1; 
						sld = GetCharacter(NPC_GenerateCharacter("CaveGandMan" + i, model[iMassive], "man", "man", iRank, PIRATE, 1, true));
						SetFantomParamFromRank(sld, iRank, true);
						sld.SaveItemsForDead = true;
						sld.DontClearDead = true; 
						sld.money = rand(5) * 1000;
						LAi_SetWarriorType(sld);
						LAi_warrior_SetStay(sld, true);
						LAi_group_MoveCharacter(sld, LAI_GROUP_MONSTERS);
						ChangeCharacterAddressGroup(sld, pchar.GenQuest.EncGirl.locationId, "quest", "quest" + i);
						i++;
						model[iMassive] = "";	
					}
				}
			}
			else
			{ //и правда девка 
				sld = GetCharacter(NPC_GenerateCharacter("UndergroundGirl", "girl_"+(rand(7)+1), "woman", "towngirl", 5, PIRATE, 1, false));
				sld.dialog.filename = "Enc_FriendGirl_dialog.c";
				int iTemp = rand(1);
				sld.dialog.currentnode = "Underground1";//+ iTemp; //здесь рендом поведения девки: 0-помогите, 1-сама крутая
				sld.greeting = "Enc_RapersGirl_" + (iTemp+1); //соотв. озвучка
				LAi_SetStayType(sld);
				LAi_group_MoveCharacter(sld, LAI_GROUP_PLAYER);
				ChangeCharacterAddressGroup(sld, pchar.GenQuest.EncGirl.locationId, "monsters", "monster"+(rand(9)+1));
			}
			LAi_SetActorTypeNoGroup(npchar);
            LAi_ActorRunToLocation(npchar, "reload", "reload1", "none", "", "", "", -1);
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;

		case "Step_disAgree":
			ChangeCharacterReputation(pchar, -3);
			LAi_SetCitizenTypeNoGroup(npchar);
			pchar.quest.Enc_FriendGirl_after.over = "yes";
			NextDiag.CurrentNode = "FackYou";
			DialogExit();
		break;			
		case "FackYou":
            NextDiag.TempNode = "FackYou";
            dialog.text = "Не хочу разговаривать с такими бездушным болваном. Проваливай отсюда!";
			Link.l1 = "Хех, стерва...";
			Link.l1.go = "exit";
		break;

		case "Underground0": //девка реально заблудилась
            dialog.text = "Боже мой, как я рада вас видеть!!";
			Link.l1 = "Что случилось, красавица?";
			Link.l1.go = "Underground0_1";
		break;
		case "Underground0_1": 
            dialog.text = "Я заблудилась здесь! Господи, как же мне было страшно!!";
			Link.l1 = "Ну, теперь-то все в порядке. Иди за мной, я выведу тебя. Как окажешься на поверхности - быстро домой, и чтобы больше я тебя не видел!";
			Link.l1.go = "Underground0_2";
		break;
		case "Underground0_2": 
            dialog.text = "Как скажете, " + GetAddress_Form(NPChar) + ".";
			Link.l1 = "Хорошо.";
			Link.l1.go = "exit";
			npchar.LifeDay = 0;
			LAi_SetActorType(npchar);
			LAi_ActorFollow(npchar, pchar, "", -1);
			ChangeCharacterReputation(pchar, 4);
		break;

		case "Underground1": //крутая мочалка
            dialog.text = "Надо же, еще кому-то дело есть до этого подземелья!";
			Link.l1 = "Красавица, ты что тут делаешь?";
			Link.l1.go = "Underground1_1";
		break;
		case "Underground1_1": 
            dialog.text = "Не твое дело!";
			Link.l1 = "Ого, ответ на миллион пиастров...";
			Link.l1.go = "Underground1_2";
		break;
		case "Underground1_2": 
            dialog.text = "На два миллиона... Шел бы ты отсюда своей дорогой, до тебя мне дела нет.";
			Link.l1 = "Хех, и мне до тебя тоже, собственно, да вот только подруга твоя панику подняла наверху...";
			Link.l1.go = "Underground1_3";
		break;
		case "Underground1_3": 
            dialog.text = "Вот ведь дуреха! Сама от страха обделаться готова, и думает, что я такая же.";
			Link.l1 = "Да уж, ты явно не такая же. Черт в юбке!";
			Link.l1.go = "Underground1_4";
		break;
		case "Underground1_4": 
            dialog.text = "Полегче с эпитетами! Да и вообще, проваливай и дело с концом.";
			Link.l1 = "Ну ладно, раз тебе моя помощь не нужна...";
			Link.l1.go = "Underground1_5";
		break;
		case "Underground1_5": 
            dialog.text = "Не нужна!";
			Link.l1 = "Тогда прощай и будь здорова.";
			Link.l1.go = "exit";
			NextDiag.TempNode = "Underground1_again";
		break;
		case "Underground1_again": 
            dialog.text = "Опять ты? Проваливай, надоел уже...";
			Link.l1 = "Хм...";
			Link.l1.go = "exit";
			NextDiag.TempNode = "Underground1_again";
		break;

	}
}
