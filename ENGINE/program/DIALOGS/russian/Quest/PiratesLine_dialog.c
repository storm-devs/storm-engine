
void ProcessDialogEvent()
{
	ref NPChar, sld;
	aref Link, NextDiag;

	DeleteAttribute(&Dialog,"Links");

	makeref(NPChar,CharacterRef);
	makearef(Link, Dialog.Links);
	makearef(NextDiag, NPChar.Dialog);
	
	switch(Dialog.CurrentNode)
	{
		case "First time":
			dialog.text = "Мне не о чем с тобой говорить, извини...";
			link.l1 = "Понятно.";
			link.l1.go = "exit";
		break;

		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
		// -------------------------------------- квест №1 -------------------------------------------------
		//первая встреча с посыльным Моргана в Пуэрто Принсипе
		case "PiratesMan1":
			if (pchar.questTemp.State == "" || sti(pchar.questTemp.CurQuestNumber) > 8 || sti(pchar.questTemp.NationQuest) == SPAIN)
			{
				dialog.text = "Здорово, парень! Я смотрю, кораблик у тебя имеется... Подзаработать хочешь?";
				link.l1 = "Может и захочу, а что нужно?";
				link.l1.go = "PiratesMan1_1";
			}
			else
			{
				dialog.text = "Здорово, парень. Эх, дьявол!! Жаль, что не могу довериться, очень жаль...";
				link.l1 = "А в чем дело, дружище? Я что, чем-то запятнал себя в глазах Берегового Братства?";
				link.l1.go = "PiratesManNot_1";
				npchar.talker = 2; //начать диалог
				NextDiag.TempNode = "PiratesManNew1";
			}
		break;
		case "PiratesMan1_1":
			dialog.text = "Нужно оказать одну услугу Генри Моргану. Если интересно, приходи в таверну, там поговорим.";
			link.l1 = "Хм, ну пойдем...";
			link.l1.go = "PiratesMan1_2";
		break;
		case "PiratesMan1_2":
			DialogExit();
			NextDiag.CurrentNode = "Tavern_1"; 
			chrDisableReloadToLocation = true; 
			bDisableFastReload = true; 
			FreeSitLocator("PuertoPrincipe_tavern", "sit3");
			LAi_SetActorType(npchar);
			LAi_ActorRunToLocation(npchar, "reload", "reload5", "PuertoPrincipe_tavern", "sit", "sit3", "PiratesLine_toTavern", -1);
		break;

		case "PiratesManNot_1":
			dialog.text = "Да нет, дело не в этом. Ты занят работой на генерал-губернатора " + NationNameGenitive(sti(pchar.questTemp.NationQuest)) + ". Сейчас для меня это проблема.";
			link.l1 = "Ну что же, очень жаль...";
			link.l1.go = "PiratesManNot_2";
		break;
		case "PiratesManNot_2":
			dialog.text = "Послушай, ты приходи ко мне позже, когда твоя служба будет подходить к концу. Думаю, что тогда смогу быть с тобой откровенным.";
			link.l1 = "Ну что же, хорошо. Буду иметь ввиду.";
			link.l1.go = "exit";
            SetQuestHeader("Pir_Line_0_Waiting");
            AddQuestRecord("Pir_Line_0_Waiting", "1");
			AddQuestUserData("Pir_Line_0_Waiting", "sNation", NationNameAblative(sti(pchar.questTemp.NationQuest)));
		break;

		case "PiratesManNew1":
			if (pchar.questTemp.State == "" || sti(pchar.questTemp.CurQuestNumber) > 8 || sti(pchar.questTemp.NationQuest) == SPAIN)
			{
				dialog.text = "Здорово, приятель... Ну что же, время пришло! Подзаработать хочешь?";
				link.l1 = "А что нужно делать?";
				link.l1.go = "PiratesMan1_1";
			}
			else
			{
				dialog.text = "А-а, опять мы с тобой встретились. Но пока не время, приятель. Позже...";
				link.l1 = "Понятно...";
				link.l1.go = "exit";
				npchar.talker = 2; //начать диалог
				NextDiag.TempNode = "PiratesManNew1";
			}
		break;
		//разговор в таверне Пуэрто Принсипе
		case "Tavern_1":
			LAi_SetSitType(pchar);
			dialog.text = "Ну, что скажешь, поможешь Моргану?";
			link.l1 = "Моргану?!  Адмиралу пиратов?! Спрашиваешь! Конечно, я сделаю все, что нужно. Э-э-э... А что надо делать?";
			link.l1.go = "Tavern_2";
			npchar.lifeDay = 0;
		break;
		case "Tavern_2":
			dialog.text = "Да, все просто. Есть человек, которому нужно срочно попасть к Моргану, тот его уже заждался. Берешь его к себе на борт и - в Порт-Рояль. А там уж вас Морган встретит, там же и расплатится\nКак видишь, ничего сложного делать не придется. И не забывай, этим ты окажешь Моргану большую услугу. Ну как, согласен?";
			link.l1 = "Согласен! Еще бы я отказался! Оказать услугу Адмиралу! Стать вторым Морганом - моя мечта...";
			link.l1.go = "Tavern_3";
			link.l2 = "Морган, конечно, известный пират, но у меня есть другие дела, и я не намерен все бросать. И вообще, я не кэп курьерского баркаса!";
			link.l2.go = "Tavern_over";
		break;
		case "Tavern_3":
			dialog.text = "Вот и отлично! Тогда Джон Болтон будет ждать тебя в бухте. Ты только здесь не болтай лишнего!";
			link.l1 = "Я не из болтливых, можешь на меня положиться. А что, за ним охотятся? Мне надо чего-то опасаться?";
			link.l1.go = "Tavern_4";
		break;
		case "Tavern_4":
			dialog.text = "Опасаться? Да нет. Просто Морган, знаешь ли, этого не любит...";
			link.l1 = "Понимаю... Ну что же, я готов. Забираю твоего Джона - и вперед.";
			link.l1.go = "Tavern_5";
		break;
		case "Tavern_5":
			dialog.text = "Ну, счастливо тебе, приятель. Будь здоров!";
			link.l1 = "И тебе того же...";
			link.l1.go = "exit";
			npchar.lifeDay = 0;
			NextDiag.TempNode = "Tavern_again";
            SetQuestHeader("Pir_Line_1_JohnBolton");
            AddQuestRecord("Pir_Line_1_JohnBolton", "1");
			CloseQuestHeader("Pir_Line_0_Waiting");
			pchar.questTemp.piratesLine = "toJohnBolton";
			sld = GetCharacter(NPC_GenerateCharacter("QuestPirate2", "officer_4", "man", "man", 20, PIRATE, -1, true));
			sld.name = "Джон";
			sld.lastname = "Болтон";
			sld.rank = 20;
			sld.city = "PuertoPrincipe";
			sld.location	= "PuertoPrincipe_port";
			sld.location.group = "goto";
			sld.location.locator = "goto14";
			sld.dialog.filename   = "Quest\PiratesLine_dialog.c";
			sld.dialog.currentnode   = "JohnBolton";
			sld.greeting = "pirat_quest";
			sld.talker = 8; //начать диалог
			LAi_SetLoginTime(sld, 6.0, 21.99);
			LAi_SetCitizenType(sld);
			LAi_group_MoveCharacter(sld, "PIRATE_CITIZENS");
			AddDialogExitQuestFunction("PiratesLine_q1_tavernEnd");
		break;

		case "Tavern_again":
			dialog.text = "Джон ждет тебя в бухте Пуэрто-Принсипе. Не тяни с этим!";
			link.l1 = "Хорошо...";
			link.l1.go = "exit";
			NextDiag.TempNode = "Tavern_again";
		break;
		case "Tavern_over":
			dialog.text = "Хм... Ну, как знаешь, приятель. Наше дело предложить, ваше дело - отказаться. Бывай...";
			link.l1 = "Прощай.";
			link.l1.go = "exit";
			npchar.lifeDay = 0;
			pchar.questTemp.piratesLine = "over";
			LAi_CharacterDisableDialog(npchar);
			AddDialogExitQuestFunction("PiratesLine_q1_tavernEnd");
		break;
		//базар с Джоном Болтоном в порту Пуэрто Принсипе
		case "JohnBolton":
			dialog.text = "Добрый день. Скажите, вы капитан " + GetFullName(pchar) + "?";
			link.l1 = "Да, это я. А вы Джон Болтон?";
			link.l1.go = "JohnBolton_1";
		break;
		case "JohnBolton_1":
			dialog.text = "Да-да. И мне необходимо добраться до Порт-Рояля и желательно побыстрее.";
			link.l1 = "Не волнуйтесь, мигом доставлю! Проходите на борт, мы отплываем.";
			link.l1.go = "JohnBolton_2";
		break;
		case "JohnBolton_2":
			AddQuestRecord("Pir_Line_1_JohnBolton", "2");
			AddPassenger(pchar, npchar, false);
			SetCharacterRemovable(npchar, false);
			pchar.quest.PiratesLine_q1_arrest.win_condition.l1 = "locator";
			pchar.quest.PiratesLine_q1_arrest.win_condition.l1.location = "PortRoyal_town";
			pchar.quest.PiratesLine_q1_arrest.win_condition.l1.locator_group = "quest";
			pchar.quest.PiratesLine_q1_arrest.win_condition.l1.locator = "detector_PQ1";
			pchar.quest.PiratesLine_q1_arrest.function = "PiratesLine_q1_arrest";
			NextDiag.TempNode = "JohnBolton_inWay";
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
			LAi_SetActorType(npchar);
			LAi_ActorRunToLocation(npchar, "reload", "sea", "none", "", "", "", 5.0);	
		break;
		case "JohnBolton_inWay":
			dialog.text = "Мы еще не добрались до Порт Рояля, а я спешу, приятель...";
			link.l1 = "Мы идем туда, не волнуйтесь.";
			link.l1.go = "exit";
			NextDiag.TempNode = "JohnBolton_inWay";
		break;
		//базар с арестовывающими солдерами
		case "ArrestInPR":
			dialog.text = "Поступил донос, что у вас на борту находится преступник Джон Болтон... Ба! Да вот он! Вы арестованы!";
			link.l1 = "Это ошибка, офицер! Я привез этого человека по поручению Генри Моргана!";
			link.l1.go = "ArrestInPR_1";
		break;
		case "ArrestInPR_1":
			dialog.text = "Комендант разберется, где ошибка, а где нет... Извольте пройти со мной!";
			link.l1 = "Ну, хорошо...";
			link.l1.go = "ArrestInPR_2";
		break;
		case "ArrestInPR_2":
			AddQuestRecord("Pir_Line_1_JohnBolton", "3");
			chrDisableReloadToLocation = false; 
			bDisableFastReload = false; 
			LocatorReloadEnterDisable("Portroyal_town", "houseS1", false); //закроем дом Моргана
			DialogExit();
			AddDialogExitQuestFunction("PiratesLine_q1_toPrison");
		break;
		// -------------------------------------- квест №2 -------------------------------------------------
		case "EdwardLoy":
			dialog.text = "Ты ко мне, приятель? Чем обязан?";
			link.l1 = "Я принес тебе послание от Моргана! Протяни мне свою руку.";
			link.l1.go = "EdwardLoy_1";
			TakeItemFromCharacter(pchar, "BlackLabel");
		break;
		case "EdwardLoy_1":
			dialog.text = "Оба-на!.. Погоди, а как тебя зовут?";
			link.l1 = "Что-то ты даже не побледнел... Зовут меня " + GetFullName(pchar) + ".";
			link.l1.go = "EdwardLoy_2";
		break;
		case "EdwardLoy_2":
			dialog.text = "Поэтому и не побледнел, что о твоем появлении мне было извесно. Мы все уладили с Генри, он предупредил, что ты явишься. Так что забирай свою передачку и возвращайся к Моргану, он тебя ждет.";
			link.l1 = "Как это, забирай?";
			link.l1.go = "EdwardLoy_3";
		break;
		case "EdwardLoy_3":
			dialog.text = "Да вот так!.. В общем, дуй к Моргану и скажи, что все идет по плану. Понял?";
			link.l1 = "Понял... А что идет по плану?";
			link.l1.go = "EdwardLoy_4";
			GiveItem2Character(pchar, "BlackLabel");
		break;
		case "EdwardLoy_4":
			dialog.text = "Этого тебе знать не положено... Ну, давай, приятель, не задерживай меня. Будь здоров!";
			link.l1 = "И тебе того же...";
			link.l1.go = "exit";
			NextDiag.TempNode = "EdwardLoy_again";
			QuestSetCurrentNode("Henry Morgan", "PL_Q2_away_1");
			AddQuestRecord("Pir_Line_2_BlackLabel", "3");
		break;
		case "EdwardLoy_again":
			dialog.text = "Ты еще здесь?";
			link.l1 = "Уже ухожу...";
			link.l1.go = "exit";
			NextDiag.TempNode = "EdwardLoy_again";
		break;
		// -------------------------------------- квест №3 -------------------------------------------------
		case "CapGoodly":
			dialog.text = "Хо! Здорово, парень! Наслышан про твои приключения! Знатно тебя облапошили... Что так смотришь?";
			link.l1 = "Да так... Слушай, хозяин таверны говорит, что ты разговаривал с Эдвардом Лоу.";
			link.l1.go = "CapGoodly_1";
		break;
		case "CapGoodly_1":
			dialog.text = "Верно, раговаривал. Заливал он мне, что работает по секретному поручению Моргана...";
			link.l1 = "Ты не первый, кому Лоу лапши на уши навесил... А что он от тебя хотел?";
			link.l1.go = "CapGoodly_2";
		break;
		case "CapGoodly_2":
			dialog.text = "Так я же только из Куманы вернулся, караулил там галеоны. Вот Лоу и интересовали тамошние новости, видать, куда-то в те края собрался. А ты, получается, за ним гонишься?";
			link.l1 = "Ага, гонюсь.";
			link.l1.go = "CapGoodly_3";
		break;
		case "CapGoodly_3":
			dialog.text = "Эх, жаль, не знал раньше о его подленькой душонке, а то бы, глядишь, твоя погоня здесь и закончилась.";
			link.l1 = "Да, жаль. Ну что ж, пора мне. Бывай, может, еще свидимся.";
			link.l1.go = "exit";
			AddQuestRecord("Pir_Line_3_KillLoy", "13");
			pchar.questTemp.piratesLine = "KillLoy_toCumana";
			npchar.lifeDay = 0;
			pchar.quest.PiratesLine_q3_loginSeaWolf.win_condition.l1 = "location";
			pchar.quest.PiratesLine_q3_loginSeaWolf.win_condition.l1.location = "Cumana";
			pchar.quest.PiratesLine_q3_loginSeaWolf.function = "PiratesLine_q3_loginSeaWolf";	
		break;
		//Эдвард Лоу в каюте Морского Волка
		case "SeaWolf":
			dialog.text = "Дьявол! Ты все-таки добрался до меня...";
			link.l1 = "Добрался, приятель, иначе и быть не могло. Сейчас ты мне расскажешь всю правду, или познакомлю твои пятки с каленым железом.";
			link.l1.go = "SeaWolf_1";
		break;
		case "SeaWolf_1":
			dialog.text = "Да пошел ты!.. Ты не задумывался, кто ты есть? Ты - шестерка, шавка у Моргана на побегушках.";
			link.l1 = "Я выполняю поручения адмирала Берегового Братства!";
			link.l1.go = "SeaWolf_2";
		break;
		case "SeaWolf_2":
			dialog.text = "Ну конечно, куда же без Генри?! Куда не плюнь - обязательно в Моргана попадешь...";
			link.l1 = "И что в этом плохого? И адмиралом его избрали пираты!";
			link.l1.go = "SeaWolf_3";
		break;
		case "SeaWolf_3":
			dialog.text = "Лично я его не выбирал! Много есть недовольных Морганом, и наше время еще придет\nА вообще, не вижу смысла трепаться тут. К бою, шавка!..";
			link.l1 = "Прежде чем ты умрешь, я хочу, чтобы ты знал причину своей смерти - этот бриг.";
			link.l1.go = "SeaWolf_4";
		break;
		case "SeaWolf_4":
			dialog.text = "Чего?!";
			link.l1 = "Ты умрешь за то, что украл у Джекмена этот бриг.";
			link.l1.go = "SeaWolf_5";
		break;
		case "SeaWolf_5":
			dialog.text = "Ха! Представляю рожу Джекмена, когда он узнал, что бриг его - тю-тю...";
			link.l1 = "Представил? Ну и хватит на этом.";
			link.l1.go = "SeaWolf_6";
		break;
		case "SeaWolf_6":
			LAi_SetCurHPMax(npchar);
            QuestAboardCabinDialogExitWithBattle("");
            DialogExit();
			AddDialogExitQuest("MainHeroFightModeOn");			
		break;
		// -------------------------------------- квест №5 -------------------------------------------------
		case "PQ5CapGoodly":
			dialog.text = "Здорово, парень! Очень хорошо, что я тебя застал! Если не торопишься, давай поговорим, опять дело у меня к тебе, выручай.";
			link.l1 = "Здорово, коль не шутишь. Что за дело?";
			link.l1.go = "PQ5CapGoodly_1";
		break;
		case "PQ5CapGoodly_1":
			dialog.text = "Не буду ходить вокруг да около, дело у меня вот какое\nКак ты понимаешь, я работаю на Моргана. Но помимо этой работы у меня есть и другая, основная. Я - охотник за головами. До сей поры мне удавалось совмещать дела Моргана и основную работу, но сейчас, увы, не получилось. Ко мне с заказом обратился человек, которому я не могу отказать...";
			link.l1 = "Давай выкладывай дальше, смогу - помогу.";
			link.l1.go = "PQ5CapGoodly_2";
		break;
		case "PQ5CapGoodly_2":
			dialog.text = "Проблема в том, что Моргану я сейчас нужен в Порт-Рояле, и он не хочет меня отпускать. Выполнить заказ нет никакой возможности... Короче, возьмешь мой заказ?";
			link.l1 = "А отказаться ты не можешь?";
			link.l1.go = "PQ5CapGoodly_3";
		break;
		case "PQ5CapGoodly_3":
			dialog.text = "Если б все решалось так просто, я бы тебя не просил. Решайся. Всю награду от заказчика заберешь себе.";
			link.l1 = "Хм, неплохой стимул. Хорошо. Считай, что я согласен. Давай подробней, что за заказ?";
			link.l1.go = "PQ5CapGoodly_4";
		break;
		case "PQ5CapGoodly_4":
			dialog.text = "Нужно убрать человека по имени Джон Эйвори. Он подвизается, вытрясая долги для ростовщиков. Хороший боец, неплохо стреляет. Наглая скотина, что, впрочем, не мешает ему находить клиентов\n"+
				"Последний раз его видели в Виллемстаде. Не знаю, где он перебежал дорожку заказчику, но тот назначил награду за голову Джона 15 тысяч.";
			link.l1 = "Ого! А кто заказчик?";
			link.l1.go = "PQ5CapGoodly_5";
		break;
		case "PQ5CapGoodly_5":
			dialog.text = "Извини, дружище, вот этого я тебе не скажу. Заказчику это не понравится. Ну что, не передумал?";
			link.l1 = "Не передумал. Заказ принял!";
			link.l1.go = "PQ5CapGoodly_6";
		break;
		case "PQ5CapGoodly_6":
			dialog.text = "Отлично! Я буду ждать тебя в городе. Желаю удачи!";
			link.l1 = "Счастливо оставаться! И до скорой встречи!";
			link.l1.go = "PQ5CapGoodly_7";
		break;
		case "PQ5CapGoodly_7":
			chrDisableReloadToLocation = false;
			pchar.questTemp.piratesLine = "PL5Hunter_toVillemstad";
			SetQuestHeader("Pir_Line_5_Hunter");
			AddQuestRecord("Pir_Line_5_Hunter", "1");
			LAi_SetActorTypeNoGroup(NPChar);
            LAi_ActorGoToLocation(NPChar, "reload", "houseS1", "none", "", "", "", -1.0);
			DialogExit();
		break;
		//Эйвори вместо ГГ
		case "PQ5Hero":
			dialog.text = "...деньги! И давай по-хорошему, а не то мой тесак попробует твою кровь на вкус!";
			link.l1 = "Ха! Хочешь забрать?!! Шиш тебе! Это моя добыча!!!";
			link.l1.go = "PQ5Hero_1";
		break;
		case "PQ5Hero_1":
            DialogExit();
			AddDialogExitQuestFunction("PQ5_afterChangeMainHero");
		break;
		//Эйвори в доме на Бермудах
		case "PQ5Aivory":
			dialog.text = "Тебя не звали! Вали отсюда, пока отпускаю!";
			link.l1 = "Не могу. У меня заказ на вас обоих. Так что, вы сейчас умрете. Оба.";
			link.l1.go = "PQ5Aivory_1";
		break;
		case "PQ5Aivory_1":
			dialog.text = "Ха!! И еще говорят, что я наглец. Ну, проверим сейчас, кто ты есть...";
			link.l1 = "Проверим...";
			link.l1.go = "PQ5Aivory_2";
		break;
		case "PQ5Aivory_2":
            DialogExit();
			AddDialogExitQuestFunction("PQ5_afterChangeMainHero2");
		break;
		//Гудли в таверне Порт Рояля
		case "PQ5CapGoodlyEnd":
			dialog.text = "Кого я вижу, " + GetFullName(pchar) + "! Все таки вернулся! А я уже, признаться, заждался...";
			link.l1 = "Да, поиски продлились несколько дольше, чем я ожидал. Зато закончились успешно!";
			link.l1.go = "PQ5CapGoodlyEnd_1";
		break;
		case "PQ5CapGoodlyEnd_1":
			dialog.text = "Ха! Ты несказанно меня обрадовал! Гора с плеч свалилась... Награда уже у меня, так что забирай, 15 тысяч как и обещал, без обмана. Хорошо поработал, это дело прибавит тебе популярности среди пиратов.";
			link.l1 = "Да уж, надеюсь. Стать таким, как Генри, адмиралом пиратов - моя мечта... Ха! Надо будет и Генри рассказать об этом моем приключении.";
			link.l1.go = "PQ5CapGoodlyEnd_2";
			AddMoneyToCharacter(pchar, 15000);
		break;
		case "PQ5CapGoodlyEnd_2":
			dialog.text = "А Морган-то тут причем?";
			link.l1 = "Да ни при чем. Но знаешь, хочется, чтобы и он знал о том, на что я способен!";
			link.l1.go = "PQ5CapGoodlyEnd_3";
		break;
		case "PQ5CapGoodlyEnd_3":
			dialog.text = "Хе, ну-ну...";
			link.l1 = "Дружище, не стоит смеяться.";
			link.l1.go = "PQ5CapGoodlyEnd_4";
		break;
		case "PQ5CapGoodlyEnd_4":
			dialog.text = "Да я не смеюсь... Я только не понимаю, причем тут Морган?..";
			link.l1 = "Да ни причем. Ну, ладно, будь здоров. А мне пора!";
			link.l1.go = "exit";
			AddQuestRecord("Pir_Line_5_Hunter", "12");
			sld = characterFromId("Henry Morgan");
			LAi_SetHuberTypeNoGroup(sld);
			ChangeCharacterAddressGroup(sld, "PortRoyal_houseS1", "sit", "sit2");
			QuestSetCurrentNode("Henry Morgan", "PL_Q5_begin");
		break;
		// -------------------------------------- квест №6 -------------------------------------------------
		case "JohnLids":
			dialog.text = "Приветствую члена Берегового Братства! Чем обязан удвольствием видеть вас на моей палубе?";
			link.l1 = "Здравствуй, Джон. Меня послал к тебе Джекмен.";
			link.l1.go = "JohnLids_1";
		break;
		case "JohnLids_1":
			dialog.text = "Очень хорошо. А как твое полное имя?";
			link.l1 = GetFullName(pchar) + ".";
			link.l1.go = "JohnLids_2";
		break;
		case "JohnLids_2":
			dialog.text = "Ха!! Не может быть! А тогда кого мы заперли в этой бухте?";
			link.l1 = "Понятия не имею, именно я и есть " + GetFullName(pchar) +  ". Расскажи подробней в чем дело. Кого вы заперли?";
			link.l1.go = "JohnLids_3";
		break;
		case "JohnLids_3":
			dialog.text = "Началось все с того, что шхуну бедняги Сида Боннета захватил на корвете некий капер по имени " + GetFullName(pchar) + ". Может твой брат двоюродный, не знаю\n"+
				"Боннета он сдал испанцам, а те тут же повесили его на городской площади Гаваны. В общем, печальный конец для пирата, никому не пожелаешь...";
			link.l1 = "Ну а дальше, что там с моим однофамильцем?";
			link.l1.go = "JohnLids_4";
		break;
		case "JohnLids_4":
			dialog.text = "Мы нагнали его прямо здесь. Корвет пустили на дно, но команда сумела высадиться на берег с тонущего корабля\nЭти дьяволы хорошо вооружены и многочисленны, а высадка в бухту серьезно затруднена, числом абордажной команды тут вопрос не решить\nМои ребята не хотят рисковать, раз Сида уже нет в живых, а поживиться с них нечем.";
			link.l1 = "И что ты предлагаешь?";
			link.l1.go = "JohnLids_5";
		break;
		case "JohnLids_5":
			dialog.text = "Ну, мне кажется, что именно тебе надо разобраться с этим делом, что там за братан у тебя появился. Иначе, сам понимаешь, проблем у тебя будет чем дальше, тем больше\n"+
				"Дело в том, что в моей команде есть люди, которые видели тебя раньше. И они твердо уверены в том, что на корвете именно ты и есть. Вот так-то, приятель!";
			link.l1 = "Ха! Вот это да!.. Но может все-таки окажешь мне помощь?";
			link.l1.go = "JohnLids_6";
		break;
		case "JohnLids_6":
			dialog.text = "Только тем, что покараулю здесь эту братию, если тебе нужно куда-то отлучиться.";
			link.l1 = "Ну хорошо, придется мне пощекотать этих друзей. Разберусь, что к чему...";
			link.l1.go = "JohnLids_7";
		break;
		case "JohnLids_7":
			dialog.text = "Ну, вот и славно. Приступай, дружище. Мне очень интересно, чем все закончится...";
			link.l1 = "Хе, мне тоже...";
			link.l1.go = "exit";
			NextDiag.TempNode = "JohnLids_again";
			SetQuestHeader("Pir_Line_6_Jackman");
			AddQuestRecord("Pir_Line_6_Jackman", "3");
			Pchar.quest.PQ6_ShoreBattle.win_condition.l1 = "location";
			Pchar.quest.PQ6_ShoreBattle.win_condition.l1.location = "Shore53";
			Pchar.quest.PQ6_ShoreBattle.function = "PQ6_ShoreBattle";
		break;
		case "JohnLids_again":
			dialog.text = "Ты еще не покончил со своим 'братом', а я вечно ждать тебя не буду...";
			link.l1 = "Я понял, Джон...";
			link.l1.go = "exit";
			NextDiag.TempNode = "JohnLids_again";
		break;
		//двойник ГГ
		case "QuestPirateHead":
			dialog.text = "Ха, так ты и есть " + GetFullName(pchar) + "! Рад тебя видеть, братишка...";
			link.l1 = "Чего?! Какой я тебе братишка?!";
			link.l1.go = "QuestPirateHead_1";
		break;
		case "QuestPirateHead_1":
			dialog.text = "Ну, это я так, по-дружески... Так что, ты решил от меня избавиться?";
			link.l1 = "Ага. Давай, рассказывай все. Кто ты, откуда...";
			link.l1.go = "QuestPirateHead_2";
		break;
		case "QuestPirateHead_2":
			dialog.text = "А ты дашь нам уйти?";
			link.l1 = "Только в преисподнюю, братишка!";
			link.l1.go = "QuestPirateHead_3";
		break;
		case "QuestPirateHead_3":
			dialog.text = "Тогда я тебе ничего не скажу. Не вижу смысла...";
			link.l1 = "Аргх!! Ну, как знаешь...";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("PQ6_JungleBattle");
		break;
		// -------------------------------------- квест №7 -------------------------------------------------
		case "FriendLinney":
			dialog.text = "Рад видеть вас у себя на палубе.";
			link.l1 = "Я ищу Стива Линнея, говорят, вы с ним приятели, вас видели вместе в таверне.";
			link.l1.go = "FriendLinney_1";
		break;
		case "FriendLinney_1":
			dialog.text = "Хм... А зачем он вам нужен?";
			link.l1 = "Меня зовут " + GetFullName(pchar) + ", я выполняю поручение Моргана. Стив долго не возвращался, а поскольку он собирал сведения для Моргана, Генри послал меня выяснить его судьбу.";
			link.l1.go = "FriendLinney_2";
		break;
		case "FriendLinney_2":
			dialog.text = "А-а, да, я знаю Стива, о своей работе он мне тоже рассказал. В общем, в Ла Веге он ничего не разузнал, хотя здесь он должен был встретиться с одним человеком, который что-то знал, но человек не пришел. Стив собирался уходить в другой город, но к нему подошли два неизвестных мне человека. Они поговорили, и Стив ушел с ними\n"+
				"И все бы ничего, может, он новых информаторов нашел, но вчера я случайно узнал, что его шхуна 'Ласточка' продается на верфи Санто-Доминго. А Стив корабль свой очень любил и ни за что бы ни продал. Вот я и подумал, что пора с Ла Веги уходить, не к добру там долго высиживать. Можно дождаться, что и за мной придут...";
			link.l1 = "Понятно. Может, все-таки продал? Хотя, странно все это...";
			link.l1.go = "FriendLinney_3";
		break;
		case "FriendLinney_3":
			dialog.text = "Да, это верно. Ну я с этим делом закончил. Хотел Стиву помочь, но теперь не знаю, что и думать.";
			link.l1 = "Я понял. Спасибо за информацию, дружище. Желаю удачи.";
			link.l1.go = "exit";
			NextDiag.TempNode = "FriendLinney_again";
			pchar.questTemp.piratesLine = "Soukins_toSantoDomingo";
			AddQuestRecord("Pir_Line_7_Soukins", "5");
		break;
		case "FriendLinney_again":
			dialog.text = "Поговорили уже о Стиве, больше я ничего не знаю...";
			link.l1 = "Да-да, конечно.";
			link.l1.go = "exit";
			NextDiag.TempNode = "FriendLinney_again";
		break;
		//кэп, продавшие шлюп Ласточку
		case "LeonCapitain":
			dialog.text = "Что тебе нужно от меня?";
			link.l1 = "Говори, на кого работаешь?! Что сделал с капитаном 'Ласточки'?!";
			link.l1.go = "LeonCapitain_1";
		break;
		case "LeonCapitain_1":
			dialog.text = "Капитан 'Ласточки' уже на дне, больно любопытный был. Ха-ха-ха!";
			link.l1 = "Дьявол! Отвечай, на кого ты работаешь?!";
			link.l1.go = "LeonCapitain_2";
		break;
		case "LeonCapitain_2":
			dialog.text = "Я работаю на адмирала Берегового Братства. Не на этого урода Моргана, а на НАШЕГО адмирала. Это Ричард Соукинс! Присоединяйся к нам, дружище, такой человек нам очень нужен.";
			link.l1 = "У меня нет причин не считать Генри Моргана адмиралом. Его выбрало Береговое Братство.";
			link.l1.go = "LeonCapitain_3";
		break;
		case "LeonCapitain_3":
			dialog.text = "Ты его выбирал?";
			link.l1 = "Нет, но...";
			link.l1.go = "LeonCapitain_4";
		break;
		case "LeonCapitain_4":
			dialog.text = "Без 'но', приятель! И я его не выбирал. И причем мы оба - пираты. Морган - шустрый малый, но далеко не так честен с нами, как об этом принято думать...";
			link.l1 = "Мне плевать на твои рассуждения! Морган - адмирал, и точка!";
			link.l1.go = "LeonCapitain_5";
		break;
		case "LeonCapitain_5":
			dialog.text = "Идиот! Ты еще пожалеешь о своей глупости...";
			link.l1 = "Хе, ну-ну...";
			link.l1.go = "LeonCapitain_6";
		break;
		case "LeonCapitain_6":
			Map_ReleaseQuestEncounter("LeonCapitain");
			pchar.questTemp.piratesLine = "Soukins_toMorgan";
			QuestSetCurrentNode("Henry Morgan", "PL_Q7_afterLeon");
			AddQuestRecord("Pir_Line_7_Soukins", "7");
			LAi_SetCurHPMax(npchar);
            QuestAboardCabinDialogExitWithBattle("");
            DialogExit();
			AddDialogExitQuest("MainHeroFightModeOn");	
		break;
		//поход на Панаму, пираты в городе
		case "PQ8_PirInPan":
			dialog.text = "Знаешь, " + pchar.name + ", не зря я пошел к тебе в команду. В итоге я жив, да еще в захваченной Панаме! Кто бы мог подумать?..";
			link.l1 = "Да уж, на этом деле мы должны прилично заработать.";
			link.l1.go = "PQ8_PirInPan_1";
		break;
		case "PQ8_PirInPan_1":
			dialog.text = "Уже ищем, что бы прихватить!";
			link.l1 = "И это правильно.";
			link.l1.go = "exit";
		break;
		case "PQ8_RsPirInPan":
			dialog.text = "Эх, жаль наш капитан погиб! Славный был человек Ричард Соукинс...";
			link.l1 = "Ты думаешь?";
			link.l1.go = "PQ8_RsPirInPan_1";
		break;
		case "PQ8_RsPirInPan_1":
			dialog.text = "Конечно! Я прослужил под его началом больше года. Он был справедливым человеком!";
			link.l1 = "Хм, понятно...";
			link.l1.go = "exit";
		break;
		case "PQ8_MPirInPan":
			dialog.text = "Я слышал, вам тоже досталось на орехи! Мы с Морганом шли к Панаме из Порто Белло.";
			link.l1 = "Да уж, на каждом шагу нас ждали засады испанцев.";
			link.l1.go = "PQ8_MPirInPan_1";
		break;
		case "PQ8_MPirInPan_1":
			dialog.text = "У нас было то же самое, форменный ад...";
			link.l1 = "Но главное, что мы справились с этим и живы. Так что, не вешай нос!";
			link.l1.go = "exit";
		break;
		//фантом мэра Панамы
		case "PQ8_Mayor":
			dialog.text = "Что хотите от меня?";
			link.l1 = "Здравствуйте, уважаемый. Насколько я понимаю, вы являетесь губернатором Панамы...";
			link.l1.go = "PQ8_Mayor_1";
		break;
		case "PQ8_Mayor_1":
			dialog.text = "Именно так.";
			link.l1 = "Я хочу, чтобы вы отдали мне все золото Эскорила. Я знаю, что у вас оно есть.";
			link.l1.go = "PQ8_Mayor_2";
		break;
		case "PQ8_Mayor_2":
			dialog.text = "Хм, ну поищите...";
			link.l1 = "Вы играете с огнем! Панаму взял Генри Морган, здесь вообще собрался весь цвет карибского пиратства. Вы представляете себе, что с вами будет?";
			link.l1.go = "PQ8_Mayor_3";
		break;
		case "PQ8_Mayor_3":
			dialog.text = "Представляю. Но я ничем не могу помочь в любом случае. Все дело в том, что у меня нет ключа от сундука, в котором, возможно, и находится кое-что... Я говорю об одном из сундуков, находящихся в этом кабинете.";
			link.l1 = "И где этот ключ?";
			link.l1.go = "PQ8_Mayor_4";
		break;
		case "PQ8_Mayor_4":
			dialog.text = "Он всегда находится у коменданта. Этот сундук всегда был под контролем военных, а не гражданских властей города.";
			link.l1 = "Где комендант сейчас?";
			link.l1.go = "PQ8_Mayor_5";
		break;
		case "PQ8_Mayor_5":
			dialog.text = "Понятия не имею. Он руководил обороной города. Насколько мне известно, он должен был атаковать ладронов со стороны форта. Необходимо было не допустить захвата Панамы, поэтому форт никто не защищал.\n"+
				"Так что, может быть, он убит, а, может быть, находится у себя в форте.";
			link.l1 = "Имейте ввиду, что от успешного вскрытия этого сундука зависит ваша жизнь.";
			link.l1.go = "PQ8_Mayor_6";
		break;
		case "PQ8_Mayor_6":
			dialog.text = "Я понимаю. Но мне остается надеяться только на удачу.";
			link.l1 = "Я поищу ключ. А вам я рекомендую напрячь память - возможно вы еще что-нибудь вспомните...";
			link.l1.go = "PQ8_Mayor_7";
		break;
		case "PQ8_Mayor_7":
			dialog.text = "Если меня что-либо озарит, то вы узнаете об этом непременно. Это в моих интересах.";
			link.l1 = "Хорошо.";
			link.l1.go = "exit";
			AddQuestRecord("Pir_Line_8_Panama", "11");
			pchar.quest.PQ8_MorganInPanama_1.win_condition.l1 = "location";
			pchar.quest.PQ8_MorganInPanama_1.win_condition.l1.location = "Panama_town";
			pchar.quest.PQ8_MorganInPanama_1.function = "PQ8_MorganInPanama_1";
		break;
		//обманутые пиратусы в Панаме
		case "PQ8_FackMorgan":
			dialog.text = LinkRandPhrase("Дьявол, Генри обманул нас!", "Ты слышал, что сделал Морган?! Это просто уму непостижимо...", "Теперь нам здесь конец, надо выбираться отсюда как-нибудь. Теперь каждый сам за себя...");
			link.l1 = LinkRandPhrase("Да, я слышал...", "Черт!!!", "Разрази меня гром!");
			link.l1.go = "exit";
		break;
		первый подбегающий пират
		case "PL_Q8_QFackMorgan":
			dialog.text = "Ты как нельзя вовремя, капитан! Жаль, что тебя тут не было чуть раньше...";
			link.l1 = "Что случилось?";
			link.l1.go = "PL_Q8_QFackMorgan_1";
		break;
		case "PL_Q8_QFackMorgan_1":
			dialog.text = "Ха!! Я тебя сейчас так обрадую, что ты от радости с ума сойдешь!";
			link.l1 = "Говори по существу, не тяни.";
			link.l1.go = "PL_Q8_QFackMorgan_2";
		break;
		case "PL_Q8_QFackMorgan_2":
			dialog.text = "Наш адмирал, Генри Морган, собрал со всех нас добычу! Он построил пиратов и первым очистил свои карманы в общий котел!";
			link.l1 = "Ну и что, это мне было известно. Что в этом чрезвычайного?";
			link.l1.go = "PL_Q8_QFackMorgan_3";
		break;
		case "PL_Q8_QFackMorgan_3":
			dialog.text = "В этом - ничего, но только это еще не все новости... Мы все выложили содержимое своих кошельков и пошли заниматься своими делами в ожидании, когда Генри все поделит по закону Берегового Братства. А в это время наш доблестный и честнейший адмирал тихо погрузился в испанский галеон и отдал концы.";
			link.l1 = "Как?.. А деньги?!";
			link.l1.go = "PL_Q8_QFackMorgan_4";
		break;
		case "PL_Q8_QFackMorgan_4":
			dialog.text = "Наши деньги тоже отдали концы вместе с Морганом! Я же тебе о чем и толкую - Генри прихватил всю добычу и бросил нас здесь, в этой чертовой Панаме!";
			link.l1 = "Не может быть, этого просто не может быть!..";
			link.l1.go = "PL_Q8_QFackMorgan_5";
		break;
		case "PL_Q8_QFackMorgan_5":
			dialog.text = "Ха! Это факт, капитан! Что теперь делать?";
			link.l1 = "Хм... дьявол... Если это так, если мы остались тут без денег и продовольствия... то это конец.";
			link.l1.go = "PL_Q8_QFackMorgan_6";
		break;
		case "PL_Q8_QFackMorgan_6":
			dialog.text = "Эх, был бы Соукинс жив - ничего подобного не было бы точно! Какой был капитан, настоящий адмирал!\nДьявол, сколько раз Ричард говорил, что Морган нас предаст. А мы все не слушали, медлили...";
			link.l1 = "Что значит медлили?";
			link.l1.go = "PL_Q8_QFackMorgan_7";
		break;
		case "PL_Q8_QFackMorgan_7":
			dialog.text = "То и значит! Соукинс претендовал на должность адмирала Берегового Братства, но мы уговорили его не торопиться с выборами и не включать этот вопрос в повеску дня. Каррамба, как же мы ошибались!!";
			link.l1 = "Да... а как я ошибался!";
			link.l1.go = "PL_Q8_QFackMorgan_8";
		break;
		case "PL_Q8_QFackMorgan_8":
			dialog.text = "Да уж, вы с Морганом жили душа в душу. Не думал, что он и тебя бросит здесь...";
			link.l1 = "Чего уж теперь страдать?! Надо созвать оставшися в живых пиратов, собрать все продовльствие, порох, пули, и идти к своим кораблям на восток.";
			link.l1.go = "PL_Q8_QFackMorgan_9";
		break;
		case "PL_Q8_QFackMorgan_9":
			dialog.text = "Опять через джунгли? Нет уж, второго такого похода я не переживу. А с тобой никто и не пойдет, вы же с Морганом - кореши! А если ты заикнешься о том, чтобы что-то собрать... Уверен, что сразу получишь пулю в лоб!";
			link.l1 = "Каррамба!!";
			link.l1.go = "PL_Q8_QFackMorgan_10";
		break;
		case "PL_Q8_QFackMorgan_10":
			dialog.text = "Теперь каждый сам за себя, капитан... Ну ладно, некогда мне тут с тобой лясы точить. Пойду в таверну, надеюсь, там еще остался ром...";
			link.l1 = "Ну давай.";
			link.l1.go = "PL_Q8_QFackMorgan_11";
		break;
		case "PL_Q8_QFackMorgan_11":
			chrDisableReloadToLocation = false;	
			LAi_LocationFightDisable(&Locations[FindLocation("PortRoyal_houseS1")], true);
			SetTimerFunction("PQ8_openPanama_2", 0, 0, 30);
			pchar.questTemp.piratesLine = "Panama_backToShip";
			AddQuestRecord("Pir_Line_8_Panama", "13");
			Pchar.quest.PL_Q8_backFight_1.win_condition.l1 = "location";
			Pchar.quest.PL_Q8_backFight_1.win_condition.l1.location = "Panama_jungle_04";
			Pchar.quest.PL_Q8_backFight_1.function = "PL_Q8_backFight_1";
			LAi_SetActorType(NPChar);
			LAi_ActorRunToLocation(NPChar, "reload", "reload4", "none", "", "", "", -1);
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}
}
