// boal 27.03.2004 Муж Изабелы Армани

void ProcessDialogEvent()
{
	ref NPChar, d, sld;
	aref Link, NextDiag;
	string NPC_Meeting;
	int i;

	DeleteAttribute(&Dialog,"Links");

	makeref(NPChar,CharacterRef);
	makearef(Link, Dialog.Links);
	makeref(d, Dialog);
	makearef(NextDiag, NPChar.Dialog);

	string iDay, iMonth;
	iDay = environment.date.day;
	iMonth = environment.date.month;
	string lastspeak_date = iday + " " + iMonth;
	
	switch(Dialog.CurrentNode)
	{
		case "exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			NPChar.quest.meeting = NPC_Meeting;
			DialogExit();
		break;

		case "Good_exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			NPChar.quest.meeting = NPC_Meeting;
			DialogExit();
			LAi_SetActorType(npchar);
			LAi_ActorGoToLocator(npchar, "goto", "goto1", "RP_talkWithHusbandIsOver", 0);
		break;

		case "First time":
			NextDiag.TempNode = "first time";
			chrDisableReloadToLocation = false;
			bDisableFastReload = false;
			if (CheckAttribute(pchar, "quest.wasInBeedroom"))
			{
	            if (CheckAttribute(NPChar, "quest.NiceMeet"))
				{
					dialog.text = "Хм, однако, ты позволяешь себе слишком много - заходить в спальню без спроса хозяина дома...";
					link.l1 = "Извини, приятель, заплутал просто...";
					link.l1.go = "Good_exit";
				}
				else
				{
					dialog.text = "Какого черта ты шастаешь у меня дома? Защищайся, мерзавец!";
					link.l1 = "Ну хорошо...";
					link.l1.go = "Romantic_Battle_in_Bedroom_2";
				}
				DeleteAttribute(pchar, "quest.wasInBeedroom");
			}
			else
			{				
				dialog.text = "Приветствую вас, незнакомец! Могу я узнать, кто вы и чему обязан чести видеть вас у себя дома?";
				link.l1 = "Мое имя - " + PChar.Name + " " + PChar.LastName +
						", я капитан.";
				link.l1.go = "Next_1";
				link.l2 = "Прошу меня простить. Я ошибся адресом.";
				link.l2.go = "Good_exit";
			}
		break;
		
        case "Next_1":
            NextDiag.TempNode = "Usual_1";
			dialog.text = "Капитан или все-таки корсар? Судя по вашему виду, сеньор, скорее - второе. Ну что ж, в наше время не один достопочтенный дон сколотил таким образом приличное состояние. И я - не исключение.";
			link.l1 = "Вы были корсаром?!";
			link.l1.go = "Next_2";
			link.l2 = "Прошу меня простить... дела!";
			link.l2.go = "Good_exit";
		break;
		
		case "Next_2":
		    NextDiag.TempNode = "Usual_1";
			dialog.text = "А что в этом удивительного? Это очень быстрый и надежный способ разбогатеть. Но после удачной женитьбы я решил остепениться, чего и вам желаю.";
			link.l1 = "Понятно. Спасибо за совет. Непременно последую ему при случае...";
			link.l1.go = "Good_exit";
			NPChar.quest.NiceMeet = true;
		break;
		
		case "Usual_1":
			chrDisableReloadToLocation = false;
			bDisableFastReload = false;
		    NextDiag.TempNode = "Usual_1";
			dialog.text = "Приятно увидеться с вами вновь. Что привело вас ко мне на сей раз?";
			link.l1 = "Ничего особенного. Просто зашел навестить.";
			link.l1.go = "Good_exit";
			if (CheckAttribute(pchar, "quest.VisitStep"))
			{
				if (sti(pchar.quest.VisitStep)>2 && !CheckAttribute(pchar, "quest.wasInBeedroom"))
				{
					dialog.text = "О, "+ GetFullName(pchar)+" опять у меня дома. Ну что же, приятно видеть тебя вновь. Зачем пожаловал?";
					link.l1 = "Сальватор, я хочу пообщаться с твоей женой. Можно?";
					link.l1.go = "Step_2";
				}				
				if (sti(pchar.quest.VisitStep)>2 && CheckAttribute(pchar, "quest.wasInBeedroom"))
				{					
					if (!CheckAttribute(pchar, "quest.already"))
					{
						dialog.text = "Не слишком ли ты зачастил к моей жене, приятель?";
						link.l1 = "Да все нормально, друг Сальватор. Я знаком с ее родственниками, так что не волнуйся.";
						link.l1.go = "Step_1";
					}
					else
					{
						dialog.text = "Передал все, что нужно?";
						link.l1 = "Да, Сальватор, все нормально.";
						link.l1.go = "Good_exit";
						LocatorReloadEnterDisable("SanJuan_houseSp6", "reload2", true);
					}
					DeleteAttribute(pchar, "quest.wasInBeedroom");
				}		
			}
		break;
		case "Step_1":
			dialog.text = "Все это хорошо, но впредь изволь просить разрешения навестить ее, как это делается в приличных домах.";
			link.l1 = "Хорошо, Сальватор, как скажешь.";
			link.l1.go = "Good_exit";
			LocatorReloadEnterDisable("SanJuan_houseSp6", "reload2", true);
			pchar.quest.already = true;
		break;
		case "Step_2":
			dialog.text = "А зачем тебе это?";
			link.l1 = "Хочу передать ей весточку от родных.";
			link.l1.go = "Step_3";
		break;
		case "Step_3":
			dialog.text = "Ну ладно, разрешаю.";
			link.l1 = "Спасибо, Сальватор. Я мигом.";
			link.l1.go = "Good_exit";
			if (pchar.RomanticQuest == "DelivMigel")
			{
				dialog.text = "Ты вытащил ее братца Мигеля из Куманы, поэтому ты, вроде как, друг семьи теперь. Так что проходи, нет проблем.";
				link.l1 = "Хм, я понял, Сальватор. Спасибо тебе.";			
			}
			LocatorReloadEnterDisable("SanJuan_houseSp6", "reload2", false);
		break;

		case "Romantic_Battle_in_Bedroom":
		    if (CheckAttribute(NPChar, "quest.NiceMeet"))
		    {
			    dialog.text = PChar.Name + " " + PChar.LastName +", вы уже успели познакомиться с моей супругой? " + Characters[GetCharacterIndex("Isabella")].Name + ", если нет возражений, я вернусь к своим делам. Жду вас внизу...";
			    link.l1 = "О, да. Теперь я по-другому смотрю на ваш совет.";
			    link.l1.go = "Romantic_Battle_in_Bedroom_3";
			}
			else
			{
			    dialog.text = "Что! Вы! Делаете! В спальне моей жены?!";
			    link.l1 = "Я... Я попал сюда по ошибке... У меня и в мыслях не было...";
			    link.l1.go = "Romantic_Battle_in_Bedroom_1";
			}
		break;
		
		case "Romantic_Battle_in_Bedroom_1":
			dialog.text = "Защищайся, глупец, сейчас ты умрешь!";
			link.l1 = "Что??? Ну, это мы еще поглядим!";
			link.l1.go = "Romantic_Battle_in_Bedroom_2";
		break;
		
		case "Romantic_Battle_in_Bedroom_2":
			DialogExit();
			AddDialogExitQuest("Romantic_Battle_in_Bedroom_3");
		break;
		
		case "Romantic_Battle_in_Bedroom_3":
		    NextDiag.CurrentNode = "Usual_1";
            Pchar.RomanticQuest.TalkInShop = true;// обошлось все мирно - до встречи в магазине
			DialogExit();
			AddDialogExitQuest("Romantic_Battle_in_Bedroom_4");
		break;

		case "WeWaitYouTonight":
			NextDiag.CurrentNode = "WeWaitYouTonight";
			dialog.text = "Мы с супругой будем рады увидеть вас сегодня вечером, капитан.";
			link.l1 = "Буду обязательно, Сальватор.";
			link.l1.go = "exit";
			chrDisableReloadToLocation = false;
			bDisableFastReload = false;
		break;

		case "SeenMainHero":
			dialog.text = "Хех, вы как нельзя вовремя, " + GetFullName(pchar) + "... Убейте эту грязную свинью. Пусть все подумают, что они друг друга зарезали.";
			link.l1 = "...";
			link.l1.go = "SeenMainHero_1";
		break;
		case "SeenMainHero_1":
			AddQuestRecord("Romantic_Line", "9");
            LAi_SetActorType(npchar);
            LAi_ActorRunToLocation(npchar, "reload", "houseSp6", "SanJuan_houseSp6", "goto", "goto1", "Romantic_DeadBrother_3", 2.0);
            LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
            LAi_group_FightGroups("EnemyFight", LAI_GROUP_PLAYER, true);
            LAi_group_SetCheck("EnemyFight", "OpenTheDoors");
			NextDiag.CurrentNode = "ArrestInHome_1";
			DialogExit();
		break;

		case "ArrestInHome_1":
			dialog.text = "Ага, вот и ты.\n"+
				          "Офицер, этот человек убил несчастного дона Мигеля де Вальдеса! А сейчас, наверное, собирался убить меня и мою жену! Смотрите! У него руки в крови!";
			link.l1 = "Подлый обманщик! Это ты убил дона Мигеля де Вальдеса!";
			link.l1.go = "ArrestInHome_2";
		break;
		case "ArrestInHome_2":
			AddDialogExitQuest("Romantic_ArrestInHouse");
			NextDiag.CurrentNode = "Romantic_Battle_in_Bedroom_1";
			DialogExit();
		break;
		//базар в пещере
		case "TalkInCave":
			dialog.text = "О-о-о, прямо как голубки воркуют! Я вам не мешаю?";
			link.l1 = "Напротив, Сальватор, ты как раз вовремя! Если бы ты знал, как я рад тебя видеть!";
			link.l1.go = "TalkInCave_1";
		break;
		case "TalkInCave_1":
			dialog.text = "Неужели?";
			link.l1 = "Да-да, не сомневайся даже. По всем Карибам тебя ищу... Жаровню-то принес?";
			link.l1.go = "TalkInCave_2";
		break;
		case "TalkInCave_2":
			dialog.text = "Нет пока, позже...";
			link.l1 = "Боюсь, другого шанса для барбекю у тебя не будет, приятель. Сеньоре Изабелле пора стать вдовой.";
			link.l1.go = "TalkInCave_3";
		break;
		case "TalkInCave_3":
			dialog.text = "И уж не ты ли тот герой?";
			link.l1 = "Я, Сальватор, я...";
			link.l1.go = "TalkInCave_4";
		break;
		case "TalkInCave_4":
			dialog.text = "Тогда приступим! Добро пожаловать в ад, " + GetFullName(pchar) + "!";
			link.l1 = "Пожалуй, пропущу тебя вперед... Да, и еще - мне очень нравится твоя жена, Сальватор. Можно, после твоей смерти она не будет ходить в трауре? Мы торопимся обвенчаться...";
			link.l1.go = "TalkInCave_5";
		break;
		case "TalkInCave_5":
			dialog.text = "Агрх, эту мерзавку я отправлю следом за тобой!";
			link.l1 = "Нехорошо так говорить о собственной жене...";
			link.l1.go = "TalkInCave_6";
		break;
		case "TalkInCave_6":
			sld = CharacterFromID("Isabella");
			LAi_SetActorType(sld);
			LAi_ActorRunToLocator(sld, "goto", "goto5", "", -1);
			LAi_group_MoveCharacter(npchar, "EnemyFight");
            LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
            LAi_group_FightGroups("EnemyFight", LAI_GROUP_PLAYER, true);
			LAi_group_SetCheck("EnemyFight", "Romantic_SalvatorIsDead");
			DialogExit();
		break;
	}
}
