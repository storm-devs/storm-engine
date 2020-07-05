void ProcessDialogEvent()
{
	ref NPChar, sld;
	aref Link, Diag;
	int i;
	
	DeleteAttribute(&Dialog,"Links");

	makeref(NPChar,CharacterRef);
	makearef(Link, Dialog.Links);
	makearef(Diag, NPChar.Dialog);
	int iTemp = sti(npchar.EncQty);
	string sTemp = "Patrol" + locations[FindLocation(npchar.location)].index + "_";
	string sGroup = "PatrolGroup_" + locations[FindLocation(npchar.location)].index;

	switch(Dialog.CurrentNode)
	{
		case "exit":
			Diag.CurrentNode = Diag.TempNode;
			DialogExit();			
		break;

		case "exit_fight":
			for(i = 0; i < iTemp; i++)
			{
				sld = CharacterFromID(sTemp + i);
				LAi_SetWarriorType(sld);
				LAi_group_MoveCharacter(sld, sGroup);
			}
			LAi_group_SetRelation(sGroup, LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
			LAi_group_FightGroups(sGroup, LAI_GROUP_PLAYER, true);
			LAi_group_SetCheck(sGroup, "LandEnc_PatrolAfrer");
			DialogExit();	
			AddDialogExitQuest("MainHeroFightModeOn");
		break;

		case "exit_noFight":
			for(i = 0; i < iTemp; i++)
			{
				sld = CharacterFromID(sTemp + i);
				sld.Dialog.CurrentNode = "GoodBye";
				sld.greeting = "soldier_common";
				LAi_SetWarriorType(sld);
				LAi_warrior_DialogEnable(sld, true);
				LAi_group_MoveCharacter(sld, sGroup);				
			}
			LAi_group_SetCheck(sGroup, "LandEnc_PatrolAfrer");
			DialogExit();			
		break;

		case "First Time":
			for(i = 0; i < iTemp; i++)
			{
				sld = CharacterFromID(sTemp + i);
				LAi_type_actor_Reset(sld);
				LAi_RemoveCheckMinHP(sld);
			}
			if (GetNationRelation2MainCharacter(sti(NPChar.nation)) == RELATION_ENEMY && sti(NPChar.nation) != PIRATE)
			{
    			if (pchar.nation == npchar.nation)
				{
					dialog.text = RandPhraseSimple("А-а-а, кажется, этот мерзавец поднял тревогу в " + XI_ConvertString("Colony" + npchar.city + "Dat") + "! Схватить его!!", "Хех, вы только полюбуйтесь! Некоторые " + NationNamePeople(sti(pchar.nation))+ " умудряются быть во вражде с " + NationNameAblative(sti(npchar.nation)) + "! Схватить негодяя!!");
				}
				else
				{
					dialog.text = RandPhraseSimple("Вражеский агент в близ " + XI_ConvertString("Colony" + npchar.city + "Gen") + "! Взять его!!", "Смотрите-ка, " + NationNamePeople(sti(pchar.nation))+ " разгуливают чуть ли не в " + XI_ConvertString("Colony" + npchar.city + "Dat") + "! Немедленно схватить!!");
				}
				link.l1 = RandPhraseSimple("Попробуйте, здесь мы в одиночестве...", "Хех, здесь вам помощи ждать неоткуда.");
				link.l1.go = "exit_fight"; 				
			}
			else
			{				
				dialog.text = LinkRandPhrase("Имею честь представиться! Я начальник патруля из " + XI_ConvertString("Colony" + npchar.city + "Gen") + ", мы разыскиваем беглого каторжника.",
					"Здравствуйте, я начальник этого патруля. Мы разыскиваем сбежавшего из " + XI_ConvertString("Colony" + npchar.city + "Gen") + " раба.",
					"Приветствую вас, " + GetAddress_Form(NPChar) + ". Мое подразделение осуществляет патрулирование территории близ " + XI_ConvertString("Colony" + npchar.city + "Gen") + ".");
				Link.l1 = LinkRandPhrase("Прекрасно. Чем я могу вам помочь?",
				"Очень хорошо. Могу ли я быть вам чем-нибудь полезен, " + GetAddress_Form(NPChar) + "?",
				"Великолепно. Что лично я могу для вас сделать?");
				Link.l1.go = "Node_2";
			}
		break;
		
		case "Node_2":
			dialog.text = RandPhraseSimple("Вы не видели ничего подозрительного в округе?",
				"Встречался ли вам кто-нибудь, внушающий подозрения, " + GetAddress_Form(NPChar) + "?");
			Link.l1 = RandPhraseSimple("Нет, ничего такого.", "Нет, офицер, все спокойно.");
			Link.l1.go = "Node_3";		
		break;

		case "Node_3":
			Diag.TempNode = "GoodBye";
			dialog.text = RandPhraseSimple("Ну что же, не смею вас задерживать. Прощайте, " + GetAddress_Form(NPChar) + ".",
				"Понятно. В таком случае, прощайте.");
			Link.l1 = "Удачи вам.";
			Link.l1.go = "exit_noFight";
		break;

		case "GoodBye":
			Diag.TempNode = "GoodBye";
			dialog.text = LinkRandPhrase("Не отвлекайте нас от несения службы.",
				"Я прошу вас не мешать нам!",
				"А, это опять вы... Идите своей дорогой, не мешайте нам.");
			Link.l1 = "Хорошо.";
			Link.l1.go = "Exit";			
		break;
	}
}
