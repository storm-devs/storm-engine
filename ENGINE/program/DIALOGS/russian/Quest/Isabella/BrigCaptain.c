// это всякая мелочь по изабелле: кэп брига, брат, офицер при аресте и пр.
void ProcessDialogEvent()
{
	ref NPChar, sld;
	aref Link, NextDiag;
	float locx, locy, locz;

	DeleteAttribute(&Dialog,"Links");

	makeref(NPChar,CharacterRef);
	makearef(Link, Dialog.Links);
	makearef(NextDiag, NPChar.Dialog);
	
	switch(Dialog.CurrentNode)
	{
		case "First time":
            NextDiag.TempNode = "First time";
			dialog.text = "Буэнос диас, сеньор! Не вы ли капитан того корабля, что стоит на рейде?";
			link.l1 = "А вы - капитан 'Пять ран Христовых'?";
			link.l1.go = "ShipToDomingoTavenr_2";
			link.l2 = "Нет, это не я.";
			link.l2.go = "exit";
		break;
		
		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;

		case "ShipToDomingoTavenr_2":
			dialog.text = "Он самый, сеньор.  Проклятый шторм!  Мой бриг еще долго не выйдет в море.  Каррамба!  А я, как на грех, взял пассажира.  Уж как он ругается, Матерь Божья!\n"+
				          "Обещает шкуру с меня спустить, да ведь я своим задом все одно всех щелей в днище не заткну.  Позарез ему надо в Сан-Хуан, а ни одно судно туда не идет. Сеньор благородный, знатный, рад бы ему услужить, да не могу!";
			link.l1 = "Пассажир ваш, случайно, не дон Мигель де Вальдес? Я мог бы доставить его на Сан-Хуан.";
			link.l1.go = "ShipToDomingoTavenr_3";
		break;
		
		case "ShipToDomingoTavenr_3":
            NextDiag.TempNode = "ShipToDomingoTavenr_Temp";
			dialog.text = "Да, сеньор! Именно! Дон Мигель де Вальдес!  О, сеньор, да благословит вас Господь!";
			link.l1 = "Ну, так скорей обрадуй его и направь его на мой корабль. Буду ждать его в порту.";
			link.l1.go = "exit";
			Pchar.quest.Romantic_Brother.win_condition.l1           = "location";
        	Pchar.quest.Romantic_Brother.win_condition.l1.location  = "Cumana_town";
        	Pchar.quest.Romantic_Brother.win_condition              = "Romantic_Brother";
        	Pchar.GenQuest.Hunter2Pause = true; // ОЗГи на паузу.
        	AddQuestRecord("Romantic_Line", "6");
			npchar.LifeDay = 2;
            SaveCurrentNpcQuestDateParam(npchar, "LifeTimeCreate");
		break;
		
		case "ShipToDomingoTavenr_Temp":
            NextDiag.TempNode = "ShipToDomingoTavenr_Temp";
            dialog.text = "О, сеньор, да благословит вас Господь!";
			link.l1 = "Всяко бывает. Сочтемся.";
			link.l1.go = "exit";
		break;
		// братан Изабеллы
		case "Romantic_Brother_1":
            NextDiag.TempNode = "Romantic_Brother_2";
            dialog.text = "Добрый день! Вы капитан "+GetFullName(Pchar)+"? Я дон Мигель де Вальдес. Я направляюсь в Сан-Хуан";
			link.l1 = "Добро пожаловать на борт, дон Мигель. Да, я капитан "+GetFullName(Pchar)+". Я надеюсь, путешествие будет для вас приятным.";
			link.l1.go = "Step_1";
		break;
		case "Step_1":
			AddQuestRecord("Romantic_Line", "7");
			OfficersReaction("good");
			AddCharacterExpToSkill(pchar, "Sailing", 40);
			AddCharacterExpToSkill(pchar, "Leadership", 20);
			AddPassenger(pchar, NPChar, false);
			SetCharacterRemovable(NPChar, false);
            Pchar.quest.Romantic_Brother_port.win_condition.l1           = "location";
        	Pchar.quest.Romantic_Brother_port.win_condition.l1.location  = "SanJuan_town";
        	Pchar.quest.Romantic_Brother_port.win_condition              = "Romantic_Brother_port";
            // немного веселой жизни
            TraderHunterOnMap();
            LAi_SetActorType(npchar);
            LAi_ActorRunToLocation(npchar, "reload", "reload1_back", "none", "", "", "", -1);
			chrDisableReloadToLocation = false;
			bDisableFastReload = false; 
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;

		case "Romantic_Brother_2":
            NextDiag.TempNode = "Romantic_Brother_2";
            dialog.text = "Спасибо за помощь!";
			link.l1 = "Поверьте, мне это совсем не в тягость.";
			link.l1.go = "exit";
		break;
		
		case "Romantic_Brother_port_1":
            NextDiag.TempNode = "Romantic_Brother_2";
            dialog.text = "Спасибо. А теперь пойдем навестим мою сестру.";
			link.l1 = "Пожалуйста. Конечно.";
			link.l1.go = "Step_2";
		break;
		case "Step_2":
			RemovePassenger(pchar, NPChar);
            Pchar.quest.Romantic_Brother_House.win_condition.l1           = "location";
        	Pchar.quest.Romantic_Brother_House.win_condition.l1.location  = "SanJuan_houseSp6";
        	Pchar.quest.Romantic_Brother_House.win_condition              = "Romantic_Brother_House";
            chrDisableReloadToLocation = false;
			LAi_SetActorType(npchar);
            LAi_ActorRunToLocation(npchar, "reload", "houseSp6", "", "goto", "goto4", "", -1);
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;

		case "Romantic_Brother_Thanks":
            NextDiag.TempNode = "Romantic_Brother_Thanks";
            dialog.text = "Капитан, огромное вам спасибо за оказанную услугу. Как здорово, что вы оказались в Кумане. От Сальватора помощи ждать не приходится...";
			link.l1 = "Не берите в голову, Мигель. Все в порядке. Главное, чтобы ваша сестра была счастлива...";
			link.l1.go = "Step_3";
		break;
		case "Step_3":
            dialog.text = "Мы будем рады увидеть вас сегодня вечером, капитан.";
			link.l1 = "Обязательно буду, Мигель.";
			link.l1.go = "exit";
		break;
		//разборки Сальватора с братом
		case "TalkNearHouse_1":
            dialog.text = "Мошенник! Это ты украл деньги моей сестры по фальшивым векселям! Вор и убийца!";
			link.l1 = "Да, это я! Мне чертовски были нужны деньги! А ты придержи язык, щенок, а не то я тебе быстро его подрежу! И твоей сестрице тоже...";
			link.l1.go = "TalkNearHouse_2";
		break;
		case "TalkNearHouse_2":
            dialog.text = "Если тебя до этого не вздернут, негодяй! С такими, как ты, здесь церемониться не будут! Ты завтра же будешь за решеткой!";
			link.l1 = "Ты сам выбрал свою судьбу, щенок!";
			link.l1.go = "TalkNearHouse_3";
		break;
		case "TalkNearHouse_3":		
			DialogExit();
			AddDialogExitQuest("Romantic_DeadBrother_1");
		break;
		//офицер в доме Сальватора при аресте
		case "ArrestInHome_2":
			ref rColony = GetColonyByIndex(FindColony("SanJuan"));
			if (sti(rColony.HeroOwn))
			{
				dialog.text = "Капитан ?!.. Извините, нам донесли, что здесь находится убийца!";
				link.l1 = "Расслабьтесь. Убийца как раз только что ускользнул. Если бы вы не пытались меня задержать...";
				link.l1.go = "ArrestInHome_5";
			}
			else
			{
				dialog.text = "Капитан " + GetFullName(pchar) + ", именем закона вы арестованы!";
				link.l1 = "Офицер, этот негодяй Сальватор Олеварес и есть настоящий убийца. По подложным векселям он разорил свою жену!";
				link.l1.go = "ArrestInHome_3";
			}
		break;
		case "ArrestInHome_3":
			dialog.text = "У вас есть доказательства? Где эти векселя?";
			if (CheckCharacterItem(pchar, "Order"))
			{
				link.l1 = "Они у меня, офицер!";
				link.l1.go = "ArrestInHome_Have";				
			}
			else
			{
				link.l1 = "О них говорил Мигель, я сам слышал...";
				link.l1.go = "ArrestInHome_NotHave";
			}
		break;
		case "ArrestInHome_Have":
			dialog.text = "Хорошо, разберемся. Полагаю, они будут приняты к рассмотрению. А пока сдайте свое оружие и следуйте за мной!";
			link.l1 = "Сдаваться на милость?! Нет уж, есть у меня серьезные сомнения в том, что все будет рассмотрено верно.";
			link.l1.go = "ArrestInHome_4";
		break;
		case "ArrestInHome_4":
			dialog.text = "В таком случае, капитан, я буду вынужден применить силу.";
			link.l1 = "А я, в таком случае, буду вынужден оказать сопротивление незаконному аресту.";
			link.l1.go = "FightInHouse";
		break;

		case "ArrestInHome_NotHave":
			dialog.text = "Капитан, ваши показания будут заслушаны комендантом. А сейчас сдайте оружие и следуйте за мной!";
			link.l1 = "Каррамба, кажется, дела мои плохи - кроме моего слова у меня ничего нет!";
			link.l1.go = "ArrestInHome_4";
		break;
		case "ArrestInHome_4":
			dialog.text = "Это решит суд... Сдайте оружие немедленно!";
			link.l1 = "Хм... Да нет, офицер, я предпочту удалиться. И не советую пытаться меня остановить...";
			link.l1.go = "FightInHouse";
		break;

		case "FightInHouse":
			DialogExit();
			LAi_group_Attack(NPChar, Pchar);
			LAi_SetWarriorTypeNoGroup(npchar);
			LAi_LocationFightDisable(&Locations[FindLocation("SanJuan_houseSp6")], false);
			AddQuestRecord("Romantic_Line", "10");
		break;

		case "ArrestInHome_5":
			dialog.text = "Простите, но я не мог знать, что это вы. Еще раз сожалею...";
			link.l1 = "Да ладно, попытаюсь нагнать этого негодяя. Ну, а вы свободны!";
			link.l1.go = "exit";
			AddQuestRecord("Romantic_Line", "31");
		break;
		//засада в доме Сальватора, диалог с наемным убийцей
		case "AmbushBandit":
			dialog.text = "Так-так... Кажется, мы дождались...";
			link.l1 = "Меня вы дождались? А с кем имею честь разговаривать?";
			link.l1.go = "AmbushBandit_1";
		break;
		case "AmbushBandit_1":
			dialog.text = "Тебя дождались, родимый, тебя...";
			link.l1 = "Как приятно, когда тебя кто-то ждет...";
			link.l1.go = "AmbushBandit_fight";
		break;
		case "AmbushBandit_fight":
			LAi_LocationFightDisable(&locations[FindLocation("SanJuan_houseSp6")], false);
            LAi_SetWarriorType(npchar);
            LAi_group_MoveCharacter(npchar, "EnemyFight");
            LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
            LAi_group_FightGroups("EnemyFight", LAI_GROUP_PLAYER, true);
            LAi_group_SetCheck("EnemyFight", "OpenTheDoors");
			DialogExit();
			AddDialogExitQuest("MainHeroFightModeOn");
			AddQuestRecord("Romantic_Line", "12");
		break;
		//абордаж брига, где Изабеллу типа держат
		case "BrigAbordage":
			dialog.text = "Проклятье! Какого черта ты напал на мой корабль?!!";
			link.l1 = "У тебя, капитан, две проблемы: не с тем связался и не ту взял на борт.";
			link.l1.go = "BrigAbordage_1";
		break;
		case "BrigAbordage_1":
			dialog.text = "Выражайся ясней...";
			link.l1 = "Я забираю Изабеллу Олеварес и ты рассказываешь мне, где Сальватор. В этом случае ты остаешься жив.";
			link.l1.go = "BrigAbordage_2";
		break;
		case "BrigAbordage_2":
			dialog.text = "А иначе?";
			link.l1 = "Иначе я сейчас же прикончу тебя, каналья!";
			link.l1.go = "BrigAbordage_3";
		break;
		case "BrigAbordage_3":
			dialog.text = "Ну, это мы еще посмотрим, кто кого прикончит...";
			link.l1 = "Хм, предыдущей схватки тебе оказалось мало? Ну, давай посмотрим...";
			link.l1.go = "BrigAbordage_4";
		break;
  		case "BrigAbordage_4":
			LAi_SetCurHPMax(npchar);
			QuestAboardCabinDialogExitWithBattle("Romantic_TaklInCabinBrig");            
            pchar.quest.Romantic_BrigDieHard.over = "yes";
			pchar.quest.Romantic_AfterBrigSunk.over = "yes";
            Island_SetReloadEnableGlobal("PortoBello", true);
            DialogExit();
			AddDialogExitQuest("MainHeroFightModeOn");
		break;
		//засада в Порто Белло
		case "AmbushPotroBello":
			dialog.text = "Наконец-то, мы уже заждались! Приветствую кэпа брига 'Восторженный'. А где дамочка?";
			link.l1 = "Изабелла?";
			link.l1.go = "AmbushPotroBello_1";
		break;
		case "AmbushPotroBello_1":
			dialog.text = "Ага, Изабелла.";
			link.l1 = "У меня на корабле.";
			link.l1.go = "AmbushPotroBello_2";
		break;
		case "AmbushPotroBello_2":
			dialog.text = "Так веди ее сюда... Или подожди, не надо - лучше сами пойдем к тебе на корабль и по-тихому ее придушим. Нечего ей по городу разгуливать.";
			link.l1 = "Зачем было ее сюда тащить из Сан-Хуана?";
			link.l1.go = "AmbushPotroBello_3";
		break;
		case "AmbushPotroBello_3":
			dialog.text = "Не тебе решать это, приятель...";
			link.l1 = "Хм, верно, это Сальватора дела. А сам Сальватор где? Попрощаться с женой не желает?";
			link.l1.go = "AmbushPotroBello_4";
		break;
		case "AmbushPotroBello_4":
			dialog.text = "Нет его здесь, по делам уехал. Так что мы свое дело делаем - и свободны.";
			link.l1 = "Понятно... А где дела у Сальватора? Как найти мне его?";
			link.l1.go = "AmbushPotroBello_5";
		break;
		case "AmbushPotroBello_5":
			dialog.text = "Сальватор сам находит тех, кто ему нужен, а где сейчас Сальватор - знает только сам Сальватор... Эй, что-то ты не по делу любопытен!";
			link.l1 = "Верно, я ведь не капитан брига 'Восторженный', а капитан, который пустил бриг 'Восторженный' на дно...";
			link.l1.go = "AmbushPotroBello_6";
		break;
		case "AmbushPotroBello_6":
			dialog.text = "Это ж надо было так лопухнуться! Ну все, приятель, тебе пора на тот свет...";
			link.l1 = "Спасибо, но я, пожалуй, задержусь до поры...";
			link.l1.go = "AmbushPotroBello_7";
		break;
		case "AmbushPotroBello_7":
			LAi_LocationFightDisable(&locations[FindLocation("PortoBello_houseF2")], false);
            LAi_SetWarriorType(npchar);
            LAi_group_MoveCharacter(npchar, "EnemyFight");
            LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
            LAi_group_FightGroups("EnemyFight", LAI_GROUP_PLAYER, true);
            LAi_group_SetCheck("EnemyFight", "Romantic_TalkPortoBello");
			DialogExit();
			AddDialogExitQuest("MainHeroFightModeOn");
		break;
		//доставка Изабеллы Росите в Белиз
		case "Rosita":
			dialog.text = "Приветствую вас в моем доме, сеньор. Кто вы?";
			link.l1 = "Здравствуйте, сеньора. Меня зовут капитан " + GetFullName(pchar) + ", полагаю, мое имя вам ничего не скажет.";
			link.l1.go = "Rosita_1";
		break;
		case "Rosita_1":
			dialog.text = "Да, оно мне не известно. Так чему я обязана чести видеть вас в моем доме?";
			link.l1 = "Сеньора, я действую по поручению вашей кузины - Изабеллы Олеварес.";
			link.l1.go = "Rosita_2";
		break;
		case "Rosita_2":
			dialog.text = "Продолжайте, сеньор.";
			link.l1 = "Сеньора Изабелла изъявила желание прибыть к вам, и я с удовольствием исполнил эту ее просьбу.";
			link.l1.go = "Rosita_3";
		break;
		case "Rosita_3":
			dialog.text = "Господи, капитан, неужели вы привезли сюда Изабеллу живой и невредимой?!";
			link.l1 = "Именно так, сеньора. Она на моем корабле и сейчас готовится сойти на берег.";
			link.l1.go = "Rosita_4";
		break;
		case "Rosita_4":
			dialog.text = "Ах, сеньор, я даже не знаю, как вас благодарить! Цепь трагических несчастий обрушилась на Изабеллу, она потеряла отца и брата, а муж исчез! Вы себе представить не можете...";
			link.l1 = "Отнюдь, сеньора, мне все это известно. Скажите, а Сальватор Олеварес не появлялся у вас?";
			link.l1.go = "Rosita_5";
		break;
		case "Rosita_5":
			dialog.text = "Нет, его здесь не было, он бесследно исчез. А почему вы спрашиваете о нем, капитан?";
			link.l1 = "Полагаю, лучше будет, если сама Изабелла все вам расскажет. Я убедился, что ваш дом для Изабеллы безопасен и сейчас отправлю ее к вам. Берегите ее, сеньора, ей здорово досталось за последнее время...";
			link.l1.go = "Rosita_6";
		break;
		case "Rosita_6":
			dialog.text = "Спасибо вам за все, что вы сделали для Изабеллы, сеньор " + pchar.name + ". Я буду молиться за вас...";
			link.l1 = "Хорошо, сеньора Росита, я не буду против - молитесь.";
			link.l1.go = "exit";
			NextDiag.TempNode = "Rosita_after";
            Pchar.quest.Romantic_BelizCityTalk.win_condition.l1 = "location";
            Pchar.quest.Romantic_BelizCityTalk.win_condition.l1.location = "Beliz_town";
            Pchar.quest.Romantic_BelizCityTalk.win_condition = "Romantic_BelizCityTalk";
			//снимем прерывания на засады на всякий случай
			pchar.quest.Romantic_AmbushInHouse.over = "yes";
			pchar.quest.Romantic_AmbushInPortoBello.over = "yes";
		break;
		case "Rosita_after":
			dialog.text = "Капитан " + GetFullName(pchar) + ", я рада вас видеть.";
			link.l1 = "Взаимно, сеньора.";
			link.l1.go = "exit";
			NextDiag.TempNode = "Rosita_after";
		break;
		//Росита переживает, что Изабелла пропала
		case "BackRosita":
			dialog.text = "Ах, это вы, капитан " + pchar.name + ". Как вовремя! Слава Господу, что он послал вас!";
			link.l1 = "Что случилось, Росита? Сальватор был здесь?";
			link.l1.go = "BackRosita_1";
		break;
		case "BackRosita_1":
			dialog.text = "Нет, Сальватора не было. Но с Изабеллой что-то случилось! Она пропала!";
			link.l1 = "Как пропала? Когда?";
			link.l1.go = "BackRosita_2";
		break;
		case "BackRosita_2":
			dialog.text = "Вчера утром она отправилась на рынок со служанкой, и с тех пор ее никто не видел!";
			link.l1 = "Сеньора Росита, я сейчас же отправляюсь на поиски!";
			link.l1.go = "BackRosita_3";
		break;
		case "BackRosita_3":
			dialog.text = "Да поможет вам святой Франциск!";
			link.l1 = "Хорошо...";
			link.l1.go = "exit";
			NextDiag.TempNode = "BackRosita_after";
			pchar.RomanticQuest = "SeekIsabella";
			pchar.quest.SeekIsabella_exitTown.win_condition.l1 = "locator";
			pchar.quest.SeekIsabella_exitTown.win_condition.l1.location = "Beliz_ExitTown";
			pchar.quest.SeekIsabella_exitTown.win_condition.l1.locator_group = "quest";
			pchar.quest.SeekIsabella_exitTown.win_condition.l1.locator = "detector1";
			pchar.quest.SeekIsabella_exitTown.function = "SeekIsabella_exitTown";		
			AddQuestRecord("Romantic_Line", "21");
			//миортвая служанка
			sld = GetCharacter(NPC_GenerateCharacter("ServantGirl", "girl_6", "woman", "towngirl", 5, SPAIN, 30, false));
			LAi_SetActorType(sld);
			LAi_ActorSetLayMode(sld);
			ChangeCharacterAddressGroup(sld, "Beliz_ExitTown", "goto", "goto17");
			locations[FindLocation("Beliz_ExitTown")].DisableEncounters = true; //энкаутеры запретим
			//охранник Изабеллы в шахте
            sld = GetCharacter(NPC_GenerateCharacter("Bandit", "officer_6", "man", "man", 35, PIRATE, -1, true));
            FantomMakeCoolFighter(sld, 35, 85, 75, "topor2", "pistol3", 60);
			LAi_SetWarriorType(sld);
			LAi_warrior_SetStay(sld, true);
			LAi_group_MoveCharacter(sld, LAI_GROUP_MONSTERS);
			ChangeCharacterAddressGroup(sld, "Beliz_Cave_2", "goto", "goto4");
			LAi_LocationDisableOfficersGen("Beliz_Cave_2", true); //офицеров не пускать
			LAi_LocationDisableMonstersGen("Beliz_Cave_2", true); //монстров не генерить
			pchar.quest.Romantic_BanditIsDead.win_condition.l1 = "NPC_Death";
			pchar.quest.Romantic_BanditIsDead.win_condition.l1.character = "Bandit";
			pchar.quest.Romantic_BanditIsDead.win_condition = "Romantic_BanditIsDead";
		break;
		case "BackRosita_after":
			dialog.text = "Вы нашли Изабеллу?!";
			link.l1 = "Пока нет, сеньора, я ищу...";
			link.l1.go = "exit";
		break;
		//если Изабелла погибла где-нить при похищении
		case "IsabellaIsDead_Beliz":
			dialog.text = "Капитан, вы нашли Изабеллу?";
			link.l1 = "Да, Росита, нашел. Но по дороге в город на нас напали и Изабелла погибла...";
			link.l1.go = "IsabellaIsDead_Beliz_1";
			NextDiag.TempNode = "IsabellaIsDead_Beliz_after";
		break;
		case "IsabellaIsDead_Beliz_1":
			dialog.text = "Поверить не могу... Она так на вас надеялась! Упокой Господь ее душу...";
			link.l1 = "Росита, я умираю от горя!!";
			link.l1.go = "exit";
		break;
		case "IsabellaIsDead_Beliz_after":
			dialog.text = "Капитан, я не хочу вас видеть, прощайте.";
			link.l1 = "Прощайте, Росита...";
			link.l1.go = "exit";
		break;
		//после спасения Изабеллы в пещере
		case "SavedIsabella":
			dialog.text = "Изабелла!!";
			link.l1 = "Росита!";
			link.l1.go = "SavedIsabella_1";
		break;
		case "SavedIsabella_1":
			dialog.text = "Ты в порядке! Слава Господу!";
			link.l1 = "Ныне, и присно, и вовеки веков...";
			link.l1.go = "SavedIsabella_2";
		break;
		case "SavedIsabella_2":
			dialog.text = "Что же произошло? Где ты пропадала? И где Канчита?";
			link.l1 = "Росита, меня похитил Сальватор. А Канчита мертва - ее убил мой муж...";
			link.l1.go = "SavedIsabella_3";
		break;
		case "SavedIsabella_3":
			dialog.text = "Мерзавец! Нужно обо всем доложить губернатору, чтобы он немедленно арестовал Сальватора!";
			link.l1 = "Сальватор тоже мертв. Меня нашел и вызволил из плена мой ангел-хранитель... Сальватор напал и на него, но был убит у меня на глазах.";
			link.l1.go = "SavedIsabella_4";
		break;
		case "SavedIsabella_4":
			dialog.text = "Изабелла, неужели этот кошмар наконец-то закончился?";
			link.l1 = "Да, Росита, все кончено...";
			link.l1.go = "SavedIsabella_5";
		break;
		case "SavedIsabella_5":
			dialog.text = "И вновь все проблемы решил твой верный рыцарь... Изабелла, тебе крупно повезло, что у тебя есть такой почитатель!";
			link.l1 = "Ах, кузина, это действительно так! Росита, чуть позже я тебе расскажу и о других новостях, а сейчас давай не будем задерживать моего спасителя.";
			link.l1.go = "exit";
			NextDiag.TempNode = "SavedIsabella_after";			
			AddDialogExitQuest("Romantic_DelivToRosita_2");
		break;
		case "SavedIsabella_after":
			dialog.text = "Капитан, вы действительно настоящий мужчина!";
			link.l1 = "Хм, Росита, спасибо за комплимент...";
			link.l1.go = "exit";
			NextDiag.TempNode = "SavedIsabella_after";
		break;
		//Нападения на ГГ и Изабеллу в церкви
		case "AmbushInChurch":
			dialog.text = "Капитан, тебе привет от Сальватора!";
			link.l1 = "Сальватор мертв, угомонитесь...";
			link.l1.go = "AmbushInChurch_1";
		break;
		case "AmbushInChurch_1":
			dialog.text = "К сожалению, веселье оплачено, кэп, и отменить все может только заказчик...";
			link.l1 = "Еще раз повторяю: Сальватор мертв! Для вас что, нет ничего святого? Это же венчание в храме Божьем!";
			link.l1.go = "AmbushInChurch_2";
		break;
		case "AmbushInChurch_2":
			dialog.text = "Все, время на разговоры вышло...";
			link.l1 = "Аргх!..";
			link.l1.go = "exit";
			AddDialogExitQuest("Romantic_fightInChurch_1");
		break;
		//базар с Роситой после убийства Изабеллы
		case "IsabellaIsDead":
			dialog.text = "Капитан " + GetFullName(pchar) + ", рада видеть вас. Какие новости? Вы уже обвенчались с Изабеллой?";
			link.l1 = "Новости хуже некуда, Росита. Изабелла мертва, ее убили в церкви прямо на алтаре. Нападавших было слишком много...";
			link.l1.go = "IsabellaIsDead_1";
		break;
		case "IsabellaIsDead_1":
			dialog.text = "О, Господи, бедная Изабелла!! Она так страдала, ей так не повезло с мужем.\n"+
				          "Потом появились вы и я подумала, что ее судьба наконец-то изменилась к лучшему. Но от судьбы, видимо, не уйдешь...";
			link.l1 = "Ох, Росита, как мне тяжело!..";
			link.l1.go = "IsabellaIsDead_2";
		break;
		case "IsabellaIsDead_2":
			dialog.text = "Я понимаю вас, капитан... Простите, я должна известить всех родных - нам нужно одеть траур...";
			link.l1 = "Да, Росита, понимаю...";
			link.l1.go = "exit";
			npchar.LifeDay = 0;
			NextDiag.TempNode = "IsabellaIsDead_after";
		break;
		case "IsabellaIsDead_after":
			dialog.text = "Капитан, что вы хотели?";
			link.l1 = "Ничего, Росита, просто хотел навестить кузину моей Изабеллы...";
			link.l1.go = "exit";
			NextDiag.TempNode = "IsabellaIsDead_after";
		break;
		//базар с Роситой, если ГГ отбился в церкви
		case "IsabellaIsMyWife":
			dialog.text = "Сеньор " + pchar.name + ", рада вас видеть! Мы теперь родственники!";
			link.l1 = "Да, сеньора Росита. Мы обвенчались с Изабеллой...";
			link.l1.go = "IsabellaIsMyWife_1";
		break;
		case "IsabellaIsMyWife_1":
			dialog.text = "Это прекрасно, капитан. Изабелла достойна большего, нежели этот негодяй Сальватор!";
			link.l1 = "Совершенно с вами согласен. А о Сальваторе даже и вспоминать не желаю, я отправил его к праотцам и очень этим доволен...";
			link.l1.go = "IsabellaIsMyWife_2";
		break;
		case "IsabellaIsMyWife_2":
			dialog.text = "Сеньор " + pchar.name + ", вы настоящий рыцарь! Берегите Изабеллу... Кстати, если у вас будут с ней проблемы, поссоритесь, к примеру, то всегда пожалуйста ко мне - буду рада помочь.";
			link.l1 = "Хорошо, кузина Росита...";
			link.l1.go = "exit";
			NextDiag.TempNode = "IsabellaIsMyWife_after";
		break;
		case "IsabellaIsMyWife_after":
			dialog.text = "Рада вас видеть, кузен " + pchar.name + "!";
			link.l1 = "Взаимно, сеньора Росита.";
			link.l1.go = "exit";
			NextDiag.TempNode = "IsabellaIsMyWife_after";
		break;
		//семейная жизнь ГГ
		case "IsabellaIsWife":
			dialog.text = NPCStringReactionRepeat("Здравствуйте, кузен. Я так рада вас видеть! Как поживает Изабелла?", 
				"Вы опять ко мне, кузен? Мы же только что поговорили.", 
				"Кузен, что опять?! Ничего нового вы от меня не услышите!", 
				"Послушайте, капитан, если вы и дома такой зануда, то мне жаль бедную мою кузину Изабеллу!", "block", 1, npchar, Dialog.CurrentNode);
			link.l1 = HeroStringReactionRepeat("Здравствуйте, кузина Росита. Изабелла в порядке.", 
				"Да, верно, только что... Извините меня, Росита, не буду вас отвлекать...", 
				"Да? Ну тогда ладно...", 
				"Нет, Росита, дома я - ангел во плоти...", npchar, Dialog.CurrentNode);
			link.l1.go = DialogGoNodeRepeat("IsabellaIsWife_1", "none", "none", "none", npchar, Dialog.CurrentNode);
			NextDiag.TempNode = "IsabellaIsWife";
		break;
		case "IsabellaIsWife_1":
			dialog.text = "Вы что-то хотели у меня узнать, капитан?";
			link.l1 = "Да нет, Росита, просто зашел проведать.";
			link.l1.go = "exit";				
			if (pchar.RomanticQuest == "over")
			{
				link.l2 = "Росита, даже не знаю как сказать... Изабелла закрыла дверь в доме и не пускает меня!";
				link.l2.go = "HorseCheck";	
				break;
			}			
			if (pchar.RomanticQuest == "TheDoosIsClosed")
			{
				link.l2 = "Росита, даже не знаю как сказать... Изабелла закрыла дверь в доме и не пускает меня!";
				link.l2.go = "TheDoosIsClosed";			
			}
			sld = characterFromID("Isabella");
			if (CheckAttribute(sld, "angry")) //если Изабелла сидит на ангри
			{
				if (sld.angry.name == "NewLife_Budget_Pay")
				{
					link.l3 = "Ты знаешь, Росита, у меня тут с деньгами проблемы были и Изабелла здорово на меня обиделать...";
					link.l3.go = "Budget";			
				}
				if (sld.angry.name == "NewLife_KnowPirates")
				{
					link.l4 = "Ты представляешь, Росита, Изабелла каким-то образом узнала о том, что я не совсем бросил каперство, как обещал перед свадьбой...";
					link.l4.go = "KnowPirates";			
				}
				if (sld.angry.name == "NewLife_Present")
				{
					link.l5 = "Росита, тут такое дело... В общем, Изабелла здорово надулась на меня, не хочет общаться с мужем. Вот так!";
					link.l5.go = "Present";			
				}
			}
		break;
		case "TheDoosIsClosed":
			dialog.text = "Понятно... Вы, видимо, серьезно провинились, капитан. Вы что, долго не были дома?";
			link.l1 = "Хм, ну как сказать... В общем-то, да. Но я же моряк, Росита, и моя работа - это постоянные выходы в море.";
			link.l1.go = "TheDoosIsClosed_1";	
		break;
		case "TheDoosIsClosed_1":
			dialog.text = "Я понимаю, капитан... Ну что же, я смогу уладить это дело. Отправляйтесь домой, все будет нормально...";
			link.l1 = "Росита, вы просто чудо!";
			link.l1.go = "TheDoosIsClosed_2";	
		break;
		case "TheDoosIsClosed_2":
			dialog.text = "Не перехвалите меня, " + pchar.name + ". Я бы вам посоветовала взяться за ум и чаще бывать дома.";
			link.l1 = "Кузина, я обязательно поступлю именно так. До свидания...";
			link.l1.go = "exit";
			AddQuestRecord("Romantic_Line", "30");
			LocatorReloadEnterDisable("SanJuan_town", "houseSp6", false);
			pchar.RomanticQuest = "OpenTheDoosOfHouse";
		break;

		case "Budget":
			dialog.text = "Что, такие долги?";
			link.l1 = "Да, Росита. Изабелла просила меня несколько раз погасить задолженность, но у меня, как назло, не было тогда денег.";
			link.l1.go = "Budget_1";	
		break;
		case "Budget_1":
			dialog.text = "Хм, плохо... Ну ладно, подскажу тебе, что нужно сделать. Принеси ей в подарок изумруды: кольцо, ожерелье и просто камень. Она очень любит эти камни и, полагаю, растает...";
			link.l1 = "Пожалуй, так и сделаю! Спасибо тебе, Росита. До свидания.";
			link.l1.go = "exit";	
		break;

		case "KnowPirates":
			dialog.text = "Хм, кузен, вы меня простите за вопрос, но 'не совсем' - это как?";
			link.l1 = "Ну, понимаешь, нападают на меня сами, а мне что остается делать? Атаковать, конечно!";
			link.l1.go = "KnowPirates_1";	
		break;
		case "KnowPirates_1":
			dialog.text = "Хм, понятно... Значит вы, капитан, совершенно ни при чем?";
			link.l1 = "Именно так, кузина, совершенно точное определение моей причастности к пиратству!";
			link.l1.go = "KnowPirates_2";	
		break;
		case "KnowPirates_2":
			dialog.text = "Ясно... Ну что же, единственно могу посоветовать задобрить кузину Изабеллу.";
			link.l1 = "Да это пожалуйста, но чем?";
			link.l1.go = "KnowPirates_3";	
		break;
		case "KnowPirates_3":
			dialog.text = "Разных камней и побольше. Я бы, например, подарила сапфир, алмаз, рубин, большую жемчужину, ну и золотое кольцо с рубином в придачу...";
			link.l1 = "Хм, а это мысль!.. Спасибо тебе, Росита, я так и сделаю, кузина! Прощай...";
			link.l1.go = "exit";	
		break;

		case "Present":
			dialog.text = "А что произошло?";
			link.l1 = "Да так, ничего особенного... Я тут решил подарок ей сделать, но подходящего ничего не нашлось.";
			link.l1.go = "Present_1";	
		break;
		case "Present_1":
			dialog.text = "Хм, ничего страшного... Кузен, вы что-то не договариваете.";
			link.l1 = "Да я что-то забегался и забыл, что уже предлагал ей подарок... В общем, снова предложил. И так четыре раза...";
			link.l1.go = "Present_2";	
		break;
		case "Present_2":
			dialog.text = "Это вы хватили лишку, капитан! Кто же это стерпит такое?!";
			link.l1 = "Понятно, что переборщил, но что делать то теперь?";
			link.l1.go = "Present_3";	
		break;
		case "Present_3":
			dialog.text = "Делать вот что: сделайте ей, наконец, подарок. Возьмите три золотых самородка и пять серебряных слитков. Я думаю, Изабелла растает...";
			link.l1 = "Ох, кузина Росита, что бы я без вас делал! Побегу искать злато-серебро. Прощайте...";
			link.l1.go = "exit";	
		break;

		case "HorseCheck":
			dialog.text = "Да, мне это известно. А вам я хочу сказать следующее, капитан - ваш брак расторгнут в одностороннем порядке.";
			link.l1 = "Это что за дела такие? Уж не сам ли папа римский расторгнул?";
			link.l1.go = "HorseCheck_1";	
		break;
		case "HorseCheck_1":
			dialog.text = "Брак расторгнут де-факто, вы больше никогда не увидите Изабеллу.";
			link.l1 = "Ничего не понимаю, да что же это делается?";
			link.l1.go = "HorseCheck_2";	
		break;
		case "HorseCheck_2":
			dialog.text = "А вы не знаете? Ну тогда я вам скажу, в чем дело.";
			link.l1 = "Да, будьте так добры.";
			link.l1.go = "HorseCheck_3";	
		break;
		case "HorseCheck_3":
			dialog.text = "Вам нужно было меньше бегать по борделям, дорогой мой! Изабелла может много простить, но измену - никогда!";
			link.l1 = "Черт!! А кто ей сказал? Меня оговорили!";
			link.l1.go = "HorseCheck_4";	
		break;
		case "HorseCheck_4":
			dialog.text = "Не нужно лицемерить, полученные Изабеллой сведения более чем достоверны. Прощайте, капитан...";
			link.l1 = "Эх... Прощайте, Росита...";
			link.l1.go = "exit";
			npchar.LifeDay = 0;
			NextDiag.TempNode = "HorseCheck_Bue";
			CloseQuestHeader("Romantic_Line");
		break;
		case "HorseCheck_Bue":
			dialog.text = "Я не желаю более с вами говорить.";
			link.l1 = "...";
			link.l1.go = "exit";
			NextDiag.TempNode = "HorseCheck_Bue";
		break;
	}
}
