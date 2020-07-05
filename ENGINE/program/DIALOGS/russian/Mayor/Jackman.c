// Джекмен на Бермудах
void ProcessDialogEvent()
{
	ref NPChar, sld;
	aref Link, NextDiag;

	DeleteAttribute(&Dialog,"Links");

	makeref(NPChar,CharacterRef);
	makearef(Link, Dialog.Links);
	makearef(NextDiag, NPChar.Dialog);

// ============================================================================
// ============================= блок angry ==========>>>>>>>>>>>>>>>>>>>>>>>>>
    if (CheckAttribute(npchar, "angry") && !CheckAttribute(npchar, "angry.ok"))
    {
        npchar.angry.ok = 1;
        if (!CheckAttribute(npchar, "angry.first")) //ловушка первого срабатывания
        {
            NextDiag.TempNode = NextDiag.CurrentNode;
            Dialog.CurrentNode = "AngryExitAgain";
            npchar.angry.first = 1;
        }
        else
        {
            switch (npchar.angry.kind) //сюда расписываем реакцию ангри. В npchar.angry.name пробелы удалены!!!
            {
                case "repeat":
                    if (npchar.angry.name == "Firsttime") Dialog.CurrentNode = "AngryRepeat_1";
                    if (npchar.angry.name == "I_know_you_good") Dialog.CurrentNode = "AngryRepeat_2";
					if (npchar.angry.name == "BlueBird_1") Dialog.CurrentNode = "AngryRepeat_1";
                break;
            }
        }
    }
// <<<<<<<<<<<<<<<<<<<<<<======= блок angry ===================================
// ============================================================================

	switch(Dialog.CurrentNode)
	{
		// ----------------------------------- Диалог первый - первая встреча
		case "First time":
            dialog.text = NPCStringReactionRepeat("У тебя дело ко мне? Нет? Тогда вали отсюда!",
                         "Я кажется ясно выразился.", "Хотя я выразился и ясно, но ты продолжаешь отвлекать меня!",
                         "Та-а-ак, это уже похоже на грубость, меня это утомило.", "repeat", 3, npchar, Dialog.CurrentNode);
			link.l1 = HeroStringReactionRepeat("Уже ухожу.",
                                               "Конечно, Джекмен...",
                                               "Извини, Джекмен...",
                                               "Ой...", npchar, Dialog.CurrentNode);
			link.l1.go = "exit";
			NextDiag.TempNode = "First time";
			//поиски щебеки Синяя Птица
			if (pchar.questTemp.BlueBird == "toBermudes")
			{
				link.l1 = "Слушай, Джекмен, тут такое дело... В общем, не чалилась ли у тебя в порту щебека 'Синяя Птица'?";
				link.l1.go = "BlueBird_1";
			}
			if (pchar.questTemp.BlueBird == "weWon")
			{
				link.l1 = "Представляешь, я таки отловил эту щебеку 'Синяя Птица'!";
				link.l1.go = "BlueBird_6";
			}
			if (pchar.questTemp.Sharp == "begin")
			{
				link.l1 = "Послушай, ходят слухи о промысле ловцов жемчуга в Карибском море. Ты ничего не слышал об этом?";
				link.l1.go = "SharpPearl_1";
			}
			if (pchar.questTemp.piratesLine == "KillLoy_toJackman")
			{
				link.l1 = "Я ищу своего друга Эдварда Лоу, Алексус сказал, что он у тебя.";
				link.l1.go = "PL_Q3_1";
			}
			if (pchar.questTemp.piratesLine == "KillLoy_toTavernAgain")
			{				
				dialog.text = "Что надо?! Я не в настроении отвечать на твои вопросы!";
				link.l1 = "Кажется, у нас есть с тобой общее дело. Эдвард Лоу.";
				link.l1.go = "PL_Q3_2";
			}
			if (pchar.questTemp.piratesLine == "KillLoy_GoodWork" && !CheckAttribute(npchar, "quest.PQ3"))
			{
				dialog.text = "Вернулся! Да не с пустыми руками!";
				link.l1 = "Рад сообщить, что Эдвард Лоу умер с ужасом в глазах и твоим именем в ушах.";
				link.l1.go = "PL_SEAWOLF";				
			}
			if (pchar.questTemp.piratesLine == "Panama_backToShip")
			{
				dialog.text = "А, раз тебя видеть, " + pchar.name + ". Ну, что скажешь?";
				link.l1 = "Ты знаешь о том, что сделал Морган в Панаме?";
				link.l1.go = "PL_Q8";				
			}
		break;

 		case "Exit":
			DialogExit();
			NextDiag.CurrentNode = NextDiag.TempNode;
		break;

        case "I_know_you_good":
            dialog.text = NPCStringReactionRepeat(GetFullName(pchar) + ", рад тебя видеть! Зачем пожаловал на этот раз?",
                         "Ну что тебе еще?", "Долго это будет продолжаться? Если тебе делать нечего, не отвлекай других!",
                         "Ты хороший капер, поэтому живи пока. Но общаться и разговаривать с тобой я больше не желаю.", "repeat", 10, npchar, Dialog.CurrentNode);
			link.l1 = HeroStringReactionRepeat("Да, собственно, просто так заглянул, проведать. Ничего по делу нет.",
                                               "Ничего, просто так...",
                                               "Хорошо, Джекмен, извини...",
                                               "Вот черт возьми, доигрался!!!", npchar, Dialog.CurrentNode);
			link.l1.go = "exit";
			NextDiag.TempNode = "I_know_you_good";
			//поиски щебеки Синяя Птица
			if (pchar.questTemp.BlueBird == "toBermudes")
			{
				link.l1 = "Слушай, Джекмен, тут такое дело... В общем, не чалилась ли у тебя в порту щебека 'Синяя Птица'?";
				link.l1.go = "BlueBird_1";
			}
			if (pchar.questTemp.BlueBird == "weWon")
			{
				link.l1 = "Представляешь, я таки отловил эту щебеку 'Синяя Птица'!";
				link.l1.go = "BlueBird_6";
			}
			if (pchar.questTemp.Sharp == "begin" && rand(1))
			{
				link.l1 = "Послушай, ходят слухи о промысле ловцов жемчуга в Карибском море. Ты ничего не слышал об этом?";
				link.l1.go = "SharpPearl_1";
			}
			if (pchar.questTemp.piratesLine == "KillLoy_toJackman")
			{
				link.l1 = "Я ищу своего друга Эдварда Лоу, Алексус сказал, что он у тебя.";
				link.l1.go = "PL_Q3_1";
			}
			if (pchar.questTemp.piratesLine == "KillLoy_toTavernAgain")
			{				
				dialog.text = "Что надо?! Я не в настроении отвечать на твои вопросы!";
				link.l1 = "Кажется, у нас есть с тобой общее дело. Эдвард Лоу.";
				link.l1.go = "PL_Q3_2";
			}
			if (pchar.questTemp.piratesLine == "KillLoy_GoodWork" && !CheckAttribute(npchar, "quest.PQ3"))
			{
				dialog.text = "Вернулся! Да не с пустыми руками!";
				link.l1 = "Рад сообщить, что Эдвард Лоу умер с ужасом в глазах и твоим именем в ушах.";
				link.l1.go = "PL_SEAWOLF";				
			}
			if (pchar.questTemp.piratesLine == "Panama_backToShip")
			{
				dialog.text = "А, раз тебя видеть, " + pchar.name + ". Ну, что скажешь?";
				link.l1 = "Ты знаешь о том, что сделал Морган в Панаме?";
				link.l1.go = "PL_Q8";				
			}
		break;
		//********************* пиратка, квест №3, поиски Лоу *********************
		case "PL_Q3_1":
			dialog.text = "Друга?! Хо! Ты удачно зашел, приятель. Эй, ребята! А ну вали его!!!";
			link.l1 = "Не понял!..";
			link.l1.go = "PL_Q3_fight";
		break;
		case "PL_Q3_fight":
			LAi_LocationFightDisable(&Locations[FindLocation("Pirates_townhall")], false);
			chrDisableReloadToLocation = true; // закрыть выход из локации.
            for (i=1; i<=3; i++)
            {
                sld = GetCharacter(NPC_GenerateCharacter("QuestPir_"+i, "pirate_"+i, "man", "man", 100, PIRATE, 0, true));
                FantomMakeCoolFighter(sld, 100, 100, 100, "topor2", "pistol3", 100);
            	LAi_SetWarriorType(sld);
				LAi_SetImmortal(sld, true);
            	sld.Dialog.Filename = "Quest\EngLineNpc_2.c";
                LAi_group_MoveCharacter(sld, "PIRATE_CITIZENS");
               	ChangeCharacterAddressGroup(sld, pchar.location, "reload",  "reload1");
            }
            LAi_group_SetLookRadius("PIRATE_CITIZENS", 100);
            LAi_group_SetRelation("PIRATE_CITIZENS", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
            LAi_group_FightGroups("PIRATE_CITIZENS", LAI_GROUP_PLAYER, false);
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;

		case "PL_Q3_2":
			dialog.text = "Лоу?! Какие у тебя дела с Лоу?";
			link.l1 = "Я его разыскиваю по поручению Моргана. Лоу досадил Моргану, и я ищу его, чтоб пустить ему кровь.";
			link.l1.go = "PL_Q3_3";	
		break;
		case "PL_Q3_3":
			dialog.text = "Хм. Он и Моргану успел подгадить. Нельзя сказать, что я сильно расстроен этим фактом. Но этот проходимец, украл мой бриг!";
			link.l1 = "Да, я знаю, мастер на вашей верфи мне рассказал.";
			link.l1.go = "PL_Q3_4";
		break;
		case "PL_Q3_4":
			dialog.text = "Да уж, тот еще гусь! Мозги у него все стружкой запорошило! Это ж надо додуматься, что я ему письма буду писать! Да от моей резиденции до его конуры черепаха за полчаса доползет. А он приковылял на следующий день. Денежки получить, за то, что бриг проморгал. Ага, сейчас!\nКогда мои молодцы выкидывали этого строгальщика за порог, я ему так и сказал: Не найдется 'Морской Волк' - шкуру живьем спущу и деньги в нее заверну! Чтоб нести легче было...";
			link.l1 = "Круто ты с ним.";
			link.l1.go = "PL_Q3_5";
		break;
		case "PL_Q3_5":
			dialog.text = "Да это я так, сгоряча, не буду я его трогать, никто, в здравом уме, такими мастерами не разбрасывается. Но я просто в ярости, оттого, что у меня украли корабль! А Алексус пусть маленько подрожит, умнее, в следующий раз, будет.\n"+
				"Значит, говоришь, убить Эдварда Лоу хочешь. Как ты понял, я тоже этого желаю, и хотя платить тебе за его смерть, было бы верхом глупости с моей стороны, есть одно но. Я хочу, чтоб вор знал, за что умирает.\n"+
				"Чтоб он знал - от Джекмена безнаказанно не уходят! И чтоб все это знали! Все!!! Поэтому если Лоу будет знать, что смерть к нему пришла от меня, не от Моргана, а от МЕНЯ! То я тебя отблагодарю.";
			link.l1 = "Хорошо, это не сложно. Джекмен ответь мне на один вопрос.";
			link.l1.go = "PL_Q3_6";
		break;
		case "PL_Q3_6":
			dialog.text = "Какой?";
			link.l1 = "Почему тебя не расстроило то, что Моргану досадили? Я всегда считал, что вы друзья.";
			link.l1.go = "PL_Q3_7";
		break;
		case "PL_Q3_7":
			dialog.text = "Хм...  Друзья... Что ж отвечу, если поймешь. Береговое Братство, Джентльмены Удачи, сокровища, женщины, слава!  Знакомые слова, правда? Когда мы юны, романтика этих слов манит нас, как мотыльков огонь свечи, и мы послушно летим ему на встречу.\n"+
				"Но мотыльки все разные, кто-то из нас кружит рядом, не решаясь приблизиться, кто-то обжегся и летит прочь, кто-то сгорает. А кто-то перерождается в огне, и несет неукротимый огонь на стальных крыльях\n"+
				"Хм... Я смотрю, ты не понимаешь о чем я. Слышал ли ты выражение: 'человек человеку волк'? Так вот, все мы - волчья стая. Стая возьмет добычу гораздо крупнее, чем волк одиночка.\n"+
				"Стаю ведет вожак, у которого кусок добычи больше и волчица слаще, все волки слушаются его и ждут, когда... когда он ошибется, потому, что каждый волк хочет кусок добычи побольше и волчицу послаще.";
			link.l1 = "Ну, спасибо за науку. Про волков я понял. И честно говоря, быть волком, мне нравится больше, чем каким-то мотыльком.";
			link.l1.go = "PL_Q3_8";
		break;
		case "PL_Q3_8":
			dialog.text = "Ха-ха-ха-ха, ну беги, волк! И возвращайся с добычей!";
			link.l1 = "Жди, скоро буду.";
			link.l1.go = "exit";
			NextDiag.TempNode = "I_know_you_good";
			AddQuestRecord("Pir_Line_3_KillLoy", "12");
			pchar.questTemp.piratesLine = "KillLoy_2toTavernAgain";
		break;
		//после поисков Лоу
		case "PL_SEAWOLF":
			NextDiag.TempNode = "I_know_you_good";
			npchar.quest.PQ3 = true;
			int chComp;
			bool bOk = false;
			for (int i=0; i<=COMPANION_MAX; i++)
        	{
				chComp = GetCompanionIndex(pchar, i);
                if(chComp != -1 && RealShips[sti(characters[chComp].ship.type)].Name == "BrigSW1")
        		{	
					bOk = true;
					break;
				}
			}
			if (bOk)
			{
				dialog.text = "Именно этих слов я и ждал. Вижу, что еще и мой бриг сумел прихватить! Молодец. Ты доказал, что ты – Волк! Поэтому с чистым сердцем отдаю тебе 'Морского Волка', владей по праву! Хотел бы я иметь такого парня как ты, в своей команде!";
				link.l1 = "Я - свободный капитан, Джекмен. Но твои слова - честь для меня!";
				link.l1.go = "exit";
				AddQuestRecord("Pir_Line_3_KillLoy", "19");	
			}
			else
			{
				dialog.text = "Именно этих слов я и ждал. Ну что же, спасибо тебе, приятель. В награду за труды даю тебе 20 тысяч пиастров!";
				link.l1 = "Спасибо, Джекмен! Эти деньги мне пригодятся.";
				link.l1.go = "exit";
				AddMoneyToCharacter(pchar, 20000);
				AddQuestRecord("Pir_Line_3_KillLoy", "20");	
			}
		break;
		//********************* пиратка, квест №6. двойник *********************
		case "PL_Q6":
			dialog.text = "О-о-о, кого я вижу?! Глазам не верю!";
			link.l1 = "Морган послал к тебе сказать, что капитан Гудли мертв.";
			link.l1.go = "PL_Q6_1";
		break;
		case "PL_Q6_1":
			dialog.text = "Вот так вот, значит... Новость не из приятных... И как это случилось?";
			link.l1 = "Я убил его в резиденции Моргана. Понимаешь, я по его поручению завалил Джона Эйвори, а оказалось, что это доверенное лицо Моргана...";
			link.l1.go = "PL_Q6_2";
		break;
		case "PL_Q6_2":
			dialog.text = "Эйвори что, тоже мертв?";
			link.l1 = "Ага... В общем, в итоге Гудли на очной ставке у Моргана отказался признать, что это он нанимал меня на Эйвори. Все решил поединок. Вот это я и должен был тебе рассказать.";
			link.l1.go = "PL_Q6_3";
		break;
		case "PL_Q6_3":
			dialog.text = "Ха, ну дела! А ты не в курсе, приятель, что ты еще захватил одного из моих кэпов, Сида Боннета, и сдал его испанцам?";
			link.l1 = "Я?!!";
			link.l1.go = "PL_Q6_4";
		break;
		case "PL_Q6_4":
			dialog.text = "Да, именно такие ходят слухи. Поэтому я слегка опешил от того, что увидел тебя здесь.";
			link.l1 = "Дьявол!! И что же мне делать?";
			link.l1.go = "PL_Q6_5";
		break;
		case "PL_Q6_5":
			dialog.text = "Отправляйся к берегу Москитов у Санта-Каталины. Там у бухты на фрегате 'Антверпен' стоит Джон Лидс, у него там проблемы с командой. Мне кажется, тебе будет очень интересно его навестить...";
			link.l1 = "Хорошо, Джекмен, так и сделаю.";
			link.l1.go = "exit";
			NextDiag.TempNode = "I_know_you_good";
			SetQuestHeader("Pir_Line_6_Jackman");
			AddQuestRecord("Pir_Line_6_Jackman", "2");
			pchar.questTemp.piratesLine = "PL6Brother_toSantaCatalina";
			//ставим Джона Лидса в берег москитов
			sld = GetCharacter(NPC_GenerateCharacter("JohnLids", "officer_10", "man", "man", 30, sti(pchar.nation), -1, true));	
			FantomMakeCoolSailor(sld, SHIP_FRIGATE, "Антверпен", CANNON_TYPE_CULVERINE_LBS24, 80, 60, 60);
			sld.name = "Джон";
			sld.lastname = "Лидс";
			sld.dialog.filename   = "Quest\PiratesLine_dialog.c";
			sld.dialog.currentnode   = "First time";	        
			sld.DeckDialogNode = "JohnLids";		
			sld.Abordage.Enable = false;
			sld.AnalizeShips = true; 
			sld.AlwaysFriend = true;
			sld.ShipEnemyDisable = true;
			sld.Ship.Mode = "Pirate";
			LAi_group_MoveCharacter(sld, LAI_GROUP_PLAYER_OWN);
			Group_AddCharacter("LidsGroup", "JohnLids");
			Group_SetGroupCommander("LidsGroup", "JohnLids");			
			Group_SetAddress("LidsGroup", "SantaCatalina", "quest_ships", "quest_ship_7");
			Group_SetTaskNone("LidsGroup");
		break;

		case "PL_Q6_after":
			dialog.text = "Джон уже рассказал мне о твоей операции с двойником. Ты сумел взять его живым? Узнал что-нибудь конкретное?";
			link.l1 = "Поговорить сумел, да и только. Ничего он мне не сказал...";
			link.l1.go = "PL_Q6_after_1";
		break;
		case "PL_Q6_after_1":
			dialog.text = "Это плохо. За тобой шлеф какой-то тянется, сплошные курьезы. Пока удача на твоей стороне, но однажды все может измениться. И тогда тебе конец!";
			link.l1 = "Я понимаю. Мой двойник что-то знал, но не сказал. Надо пытаться выяснить, кто стоит за всем этим...";
			link.l1.go = "PL_Q6_after_2";
		break;
		case "PL_Q6_after_2":
			dialog.text = "Верно. Ну что же, хорошо, что ты ко мне заскочил, прояснили некоторые важные детали, так сказать. Теперь отправляйся к Моргану, поведай ему о своем двойнике.";
			link.l1 = "Да, я так и сделаю. Спасибо тебе, Джекмен, что поверил мне.";
			link.l1.go = "PL_Q6_after_3";
		break;
		case "PL_Q6_after_3":
			dialog.text = "А я не верил, я просто знал, что Лидс запер тебя же у Санта-Каталины. Тебе просто повезло.";
			link.l1 = "Понятно... Да, действительно удача на моей стороне.";
			link.l1.go = "exit";
			SetQuestHeader("Pir_Line_6_Jackman");
			AddQuestRecord("Pir_Line_6_Jackman", "5");
			QuestSetCurrentNode("Henry Morgan", "PL_Q7_begin");
			sld = characterFromId("Henry Morgan");
			LAi_SetHuberTypeNoGroup(sld);
			ChangeCharacterAddressGroup(sld, "PortRoyal_houseS1", "sit", "sit2");
		break;


		//********************* капитан Шарп *********************
		case "SharpPearl_1":
			dialog.text = "Слышал, только ничего об этом толком не знаю. Специализируется на этих промыслах наш всеми любимый и уважаемый капитан Шарп! Это он у нас большой любитель жемчуга, ха-ха-ха!";
			link.l1 = "А где его можно разыскать, этого Шарпа?";
			link.l1.go = "SharpPearl_2";
			pchar.questTemp.Sharp = "seekSharp";
		break;
		case "SharpPearl_2":
			dialog.text = "Не знаю. Капитан Шарп - это неуловимый Джо. Хотя тебе же он понадобился\nА вообще искать его нужно в борделях.";
			link.l1 = "Спасибо, Джекмен.";
			link.l1.go = "exit";
		break;

        //********************** поиски Синей птицы ***********************
        case "BlueBird_1":
			dialog.text = NPCStringReactionRepeat("М-да, знаменитое стало корыто, что и говорить... А тебе-то зачем понадобилась эта щебека?", 
				"Мы уже говорили об этой щебеке.", 
				"Опять про то же?",
                "Что?! Опять?! Ну с меня хватит...", "repeat", 10, npchar, Dialog.CurrentNode);
			link.l1 = HeroStringReactionRepeat("Да видишь ли, очень ей интересуются одна группа товарищей...", 
				"Да, Джекмен, верно.",
                "Хм, ноты попутал, пардон...", 
				"Ай-ай!..", npchar, Dialog.CurrentNode);
			link.l1.go = DialogGoNodeRepeat("BlueBird_2", "exit", "exit", "exit", npchar, Dialog.CurrentNode);
  		break;
		case "BlueBird_2":
			dialog.text = "Хе-хе, уж не торговцы ли? А то слышал я, что здорово им достается от этой 'Синей Птицы'.";
			link.l1 = "Торговцы действительно интересуются этий щебекой. Но, знаешь, мне и самому интересно до жути, что за пернатый такой, от которого уйти нельзя. Это что же идельный каперский парусник по сути своей!";
			link.l1.go = "BlueBird_3";
		break;
		case "BlueBird_3":
			dialog.text = "Ха, это так! Не ты первый, кто на нее целится из Берегового Братства, да вот никто еще не изловил. Что до меня, то лично мне эта 'птица' не интересна - больно мала для серьезных дел.";
			link.l1 = "Джекмен, так ты знаешь, где щебека?";
			link.l1.go = "BlueBird_4";
		break;
		case "BlueBird_4":
			dialog.text = "Нет, конечно. Видеть - видел в окрестных водах, но в порт не заходила.";
			link.l1 = "Может, поделишься соображениями, где искать?";
			link.l1.go = "BlueBird_5";
		break;
		case "BlueBird_5":
			dialog.text = "Нет, думал уже об этом. Ни одной здравой мысли.";
			link.l1 = "Понятно. Ну что ж, спасибо и на этом, приятель.";
			link.l1.go = "exit";
		break;

		case "BlueBird_6":
			dialog.text = NPCStringReactionRepeat("Хм, поздравляю. Большая удача, черт побери! Молодец.", 
				"Поздравляю еще раз.", 
				"Это я уже слышал...",
                "Ха, да ты хвастун, приятель, каких карибы еще не видели!", "block", 1, npchar, Dialog.CurrentNode);
			link.l1 = HeroStringReactionRepeat("Спасибо, Джекмен. Доброе слово и кошке приятно...", 
				"Еще раз спасибо, Джекмен.",
                "А, ну да...", 
				"Горжусь этим!", npchar, Dialog.CurrentNode);
			link.l1.go = "exit";
		break;
        //********************** Английская линейка ***********************
 		case "EngLine_quests":
			dialog.text = "Рад видеть английского капера на моем острове. Что тебе нужно, говори.";
			link.l1 = "У меня есть для тебя работа. Заказчик - генерал-губернатор Ямайки.";
			link.l1.go = "Step_1";
  		break;

 		case "Step_1":
			dialog.text = "Очень интересно, продолжай.";
			link.l1 = "Мэдифорду нужна атака на Кюрасао, а нам не помешают богатства негоциантов этого города. В Кюрасао сейчас много протестантских купцов, которые отказались примерять инквизиторский 'испанский сапожок' и свалили из Испании и Португалии в Голландию. А где негоцианты, там много золота. Предлагаю совместить приятное с полезным.";
			link.l1.go = "Step_2";
  		break;
  		
 		case "Step_2":
			dialog.text = "Хех... Звучит заманчиво, но откажусь.";
			link.l1 = "И в чем причина отказа?";
			link.l1.go = "Step_3";
  		break;
  		
 		case "Step_3":
			dialog.text = "В том, что кроме негоциантов с их золотом, в Кюрасао много голландских солдат с их мушкетами и пушками в амбразурах форта.";
			link.l1 = "Испанские солдаты тебя не сильно пугают...";
			link.l1.go = "Step_4";
  		break;
  		
 		case "Step_4":
			dialog.text = "Меня пугает только состояние своей печени. Но, видишь ли в чем дело, вспороть брюхо испанцу - дело святое, а пустить кровь голландцу - на это я много людей не наберу. Так что без меня.";
			link.l1 = "Может, подумаешь?";
			link.l1.go = "Step_5";
  		break;
  		
 		case "Step_5":
			dialog.text = "Свое слово я сказал. А Мэдифорду передавай от меня привет.";
			link.l1 = "Как знаешь. Всего хорошего.";
			link.l1.go = "exit";
			
            AddQuestRecord("Eng_Line_2_Talking", "4");
            pchar.questTemp.Count = makeint(sti(pchar.questTemp.Count) + 1);
            if (pchar.questTemp.Count == "4")   {pchar.questTemp.State = "after_talks_with_pirates";}
            NextDiag.TempNode = "First time";
  		break;

 		case "Lets_go_bussines":
			dialog.text = "Рад тебя видеть. Какими судьбами на Бермудах?";
			link.l1 = "Я по делу. Мы с Морганом атакуем Белиз, ты примешь участие в деле?";
			link.l1.go = "Step_6";
  		break;

 		case "Step_6":
            if (GetCompanionQuantity(pchar) != 4)
            {
    			dialog.text = "Хех, мой корвет стоит на рейде, команда укомплектована, я готов.";
    			link.l1 = "Тогда выступаем немедленно!";
    			link.l1.go = "Step_7";
            }
            else
            {
    			dialog.text = "У вас и без меня комплект, так что без меня на этот раз.";
    			link.l1 = "Ну что ж, как знаешь, будь здоров.";
    			link.l1.go = "exit";
                NextDiag.TempNode = "First time";
            }
  		break;

 		case "Step_7":
			AddQuestRecord("Eng_Line_3_Morgan", "4");
            LAi_SetActorType(npchar);
            LAi_ActorSetStayMode(npchar);
            ChangeCharacterAddressGroup(npchar, "Pirates_townhall", "goto", "governor1");
            CharacterIntoCompanionAndGoOut(pchar, npchar, "reload", "reload1", 10, false);
            Fantom_SetBalls(npchar, "pirate");
            npchar.CompanionEnemyEnable = false;  // нет отпора при обстреле
            pchar.questTemp.CompanionQuantity = makeint(sti(pchar.questTemp.CompanionQuantity) + 1);
            pchar.questTemp.CompanionQuantity.Jackman = true;
            NextDiag.TempNode = "Have_hot_bussines";
			DialogExit();
			NextDiag.CurrentNode = NextDiag.TempNode;

  		break;

	 	case "Have_hot_bussines":   // Если геймеру вдруг захочется еще пообщаться, не знаю, сможет ли. Но на всякий случай.
			dialog.text = "Черт возьми, время работает против нас!";
            link.l1 = "Поспешим!";
			link.l1.go = "exit";
            NextDiag.TempNode = "Have_hot_bussines";
  		break;

	 	case "Gold_found_in_fort":    // Нода разговора в форте Куманы
            if (CheckAttribute(pchar, "questTemp.CompanionQuantity.Jackman") && !IsCompanion(characterFromID("Jackman")))
    		{
            	dialog.text = "Ты все видел - мой корвет был потоплен проклятыми испанцами! Но это не беда, команда почти в полном составе вышла сухой из воды и мы продолжили драться уже на суше."+
                              "А компенсацией за труды нам будет золото. Хех, чертовы испанцы, попытались спрятать золото. Но ты был прав, золото было здесь. Три с лишним тонны золота наше!!!"+
                              "Говорю это, а душа поет. Век бы такие слова говорил.";
            }
            else
            {
    			dialog.text = "Чертовы испанцы, попытались спрятать золото. Но ты был прав, золото было здесь. Три с лишним тонны золота наше!!!"+
                              "Говорю это, а душа поет. Век бы такие слова говорил.";
            }
            link.l1 = "А я век бы слушал!";
			link.l1.go = "Step_8";
        break;

	 	case "Step_8":
			dialog.text = "Ха-ха... Ты - хороший корсар, " + pchar.name + " " + pchar.lastname + ". Я желаю тебе удачи, друг.\n"+
                          "Мои головорезы рвутся домой после хорошей работы. Знай, что на Бермудах всегда рады тебя видеть.";
			link.l1 = "Еще увидимся!";
			link.l1.go = "Step_9";
        break;

	 	case "Step_9":
            LAi_SetActorType(npchar);
            LAi_ActorRunToLocation(npchar, "reload", "reload1_back", "none", "", "", "", 15.0);
            DialogExit();
			NextDiag.CurrentNode = NextDiag.TempNode;
        break;
        //********************** Голландская линейка ***********************
 		case "HolLine8_quest":
			dialog.text = "Слушаю тебя, зачем пожаловал ко мне?";
			link.l1 = "Я прибыл сюда по делу, ищу одного пирата, который пытался продать необычную Библию.";
			link.l1.go = "Step_H8_1";
  		break;
 		case "Step_H8_1":
			dialog.text = "Черт, было что-то... Вспомнить не могу... Но кто-то точно похвалялся редкой книгой, говорил, что христианская...";
			link.l1 = "И что?";
			link.l1.go = "Step_H8_2";
  		break;
 		case "Step_H8_2":
			dialog.text = "Да ничего. Не помню, разговор мельком был... Да и не до Библии нам было - в борделе развлекались как-никак!";
			link.l1 = "Хм, а в каком борделе?";
			link.l1.go = "Step_H8_3";
  		break;
 		case "Step_H8_3":
			dialog.text = "Что в Форт де Франсе.";
			link.l1 = "Понятно... Спасибо, Джекмен.";
			link.l1.go = "exit";
			AddQuestRecord("Hol_Line_8_SeekBible", "3");
			pchar.questTemp.State = "SeekBible_toFFBrothel";
			NextDiag.TempNode = "First time";
  		break;
		//********************* Французская линейка *********************
        case "FraLine8_talk":
            dialog.text = "Надо же, капитан " + GetFullName(pchar) + " пожаловал! Весьма рад! Выкладывай – я же вижу, что тебе не терпится сообщить что-то интересное.";
            link.l1 = "В общем, ничего особенно интересного я не предложу. Всего только отказаться от участия в грядущей войне между Англией и Голландией.";
            link.l1.go = "Step_F8_1";
        break;
        case "Step_F8_1":
            dialog.text = "Да уж, действительно ничего интересного. Тем более, что я и сам не собирался впутываться в это дело. Голландев трогать - глупость, на которую пойдет только полный идиот. Так что твои покровители заставили тебя сделать лишний крюк.";
            link.l1 = "Не лишний. По крайней мере, я узнал о твоем нейтралитете от тебя лично, а не из третьих рук.";
            link.l1.go = "exit";
			pchar.questTemp.Count = makeint(pchar.questTemp.Count)+1;
			NextDiag.TempNode = "I_know_you_good";
			AddQuestRecord("Fra_Line_8_ThreeCorsairs", "5");
        break;
		//********************* Пиратская линейка *********************
		case "PL_Q8_deck":
			dialog.text = "Ты зря теряешь время у меня на палубе. Нам нужно срочно идти на Порто Белло!";
			link.l1 = "Да, я помню...";
			link.l1.go = "exit";
			NextDiag.TempNode = "PL_Q8_deck_1";
		break;
		case "PL_Q8_deck_1":
			dialog.text = "Хм, опять ты время теряешь, не пойму я тебя...";
			link.l1 = "Все в порядке, не переживай, Джекмен.";
			link.l1.go = "exit";
			NextDiag.TempNode = "PL_Q8_deck_1";
		break;

		case "PL_Q8":
			dialog.text = "Знаю, конечно...";
			link.l1 = "Что скажешь по этому поводу?";
			link.l1.go = "PL_Q8_1";
		break;
		case "PL_Q8_1":
			dialog.text = "Ничего не скажу. Да и вообще, я завязываю с каперством. Так что с этими делами ко мне больше не лезь.";
			link.l1 = "Черт, как же так?!";
			link.l1.go = "PL_Q8_2";
		break;
		case "PL_Q8_2":
			dialog.text = "А вот так! В общем, я свое слово тебе сказал!";
			link.l1 = "Хм, ясно...";
			link.l1.go = "exit";
			NextDiag.TempNode = "I_know_you_good";
			pchar.questTemp.piratesLine = "Panama_inEngland";
		break;
// ======================== блок нод angry ===============>>>>>>>>>>>>>>>
		case "AngryRepeat_1":
            dialog.text = RandPhraseSimple("Пошел вон отсюда!", "Вон из моего дома!");
			link.l1 = "Ай...";
		    link.l1.go = "AngryExitAgainWithOut";
            if (CheckAttribute(npchar, "angry.terms")) //примиряемся с Джекменом через 10 дней.
            {
                if (GetNpcQuestPastDayParam(npchar, "angry.terms") > sti(npchar.angry.terms))
                {
         			dialog.text = "Вам не стоит отвлекать меня от дела пустыми обращениями. Впредь результат может быть более плачевным...";
        			link.l1 = "Я понял, Морган.";
        			link.l1.go = NextDiag.TempNode;
        			CharacterDelAngry(npchar);
                }
            }
    	break;
		case "AngryRepeat_2":
            dialog.text = RandPhraseSimple("Ты меня изрядно утомил, никакого общения.", "Я не хочу с тобой общаться, так что тебе лучше меня не беспокоить.");
			link.l1 = RandPhraseSimple("Ну как знаешь...", "Хм, ну что же...");
		    link.l1.go = "AngryExitAgain";
            if (CheckAttribute(npchar, "angry.terms")) //примиряемся с Джекменом через 10 дней.
            {
                if (GetNpcQuestPastDayParam(npchar, "angry.terms") > sti(npchar.angry.terms))
                {
         			dialog.text = "Надеюсь, впредь ты не будешь утомлять меня пустыми разговорами, иначе мне придется тебя убить. Признаюсь, мне это будет очень неприятно.";
        			link.l1 = "Джекмен, ты можешь быть уверен - не буду...";
        			link.l1.go = NextDiag.TempNode;
        			CharacterDelAngry(npchar);
                }
            }
    	break;
		case "AngryExitAgain":
            DialogExit();
            DeleteAttribute(npchar, "angry.ok");
		break;
		case "AngryExitAgainWithOut":
            DialogExit();
            DeleteAttribute(npchar, "angry.ok");
            DoReloadCharacterToLocation("Pirates_town","reload","reload3");
		break;
// <<<<<<<<<<<<============= блок нод angry =============================
	}
}
