// Генри Морган
void ProcessDialogEvent()
{
	ref NPChar, sld;
	aref Link, NextDiag;

	DeleteAttribute(&Dialog,"Links");

	makeref(NPChar,CharacterRef);
	makearef(Link, Dialog.Links);
	makearef(NextDiag, NPChar.Dialog);

	float locx, locy, locz;
	string sTemp;
	int iTemp;
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
            dialog.text = NPCStringReactionRepeat("У вас ко мне дело? Нет? Тогда покиньте мою резиденцию немедленно!",
                         "Я, кажется, ясно выразился.", "Хотя я выразился ясно, но вы продолжаете отвлекать меня!",
                         "Та-а-ак, это уже похоже на грубость, меня это утомило.", "repeat", 20, npchar, Dialog.CurrentNode);
			link.l1 = HeroStringReactionRepeat("Уже ухожу.",
                                               "Да, Морган, совершенно ясно.",
                                               "Извини, Морган...",
                                               "Ой...", npchar, Dialog.CurrentNode);
			link.l1.go = "exit";
			NextDiag.TempNode = "First time";
			if (pchar.questTemp.Sharp == "begin")
			{
				link.l1 = "Морган, послушай, ты ничего не слышал про жемчужные промыслы в море? Говорят, они у нас промышляют.";
				link.l1.go = "SharpPearl_1";
			}
		break;

 		case "Exit":
			DialogExit();
			NextDiag.CurrentNode = NextDiag.TempNode;
		break;

		case "fight":
			DialogExit();
			NextDiag.CurrentNode = NextDiag.TempNode;
			LAi_group_Attack(NPChar, Pchar);
			AddDialogExitQuest("MainHeroFightModeOn");
		break;

		case "I_know_you_good":
            dialog.text = NPCStringReactionRepeat(GetFullName(pchar) + ", рад тебя видеть! Зачем пожаловал на этот раз?",
                         "Ну что тебе еще?", "Долго это будет продолжаться? Если тебе делать нечего, не отвлекай других!",
                         "Ты хороший капер, поэтому живи пока. Но общаться и разговаривать с тобой я больше не желаю.", "repeat", 3, npchar, Dialog.CurrentNode);
			link.l1 = HeroStringReactionRepeat("Да, собственно, просто так заглянул, проведать. Ничего по делу нет.",
                                               "Ничего, просто так...",
                                               "Хорошо, Морган, извини...",
                                               "Вот черт возьми, доигрался!!!", npchar, Dialog.CurrentNode);
			link.l1.go = "exit";
			NextDiag.TempNode = "I_know_you_good";
			if (pchar.questTemp.Sharp == "begin")
			{
				link.l1 = "Морган, послушай, ты ничего не слышал про жемчужные промыслы в море? Говорят, они у нас промышляют.";
				link.l1.go = "SharpPearl_1";
			}
			if (pchar.questTemp.piratesLine == "waiting_Q4" && GetQuestPastDayParam("questTemp.piratesLine") > 5 && pchar.questTemp.Sharp != "toPearl")
			{
				link.l1 = "Слушай, Генри, есть для меня подходящее дельце?";
				link.l1.go = "PL_Q4_1";
			}
			if (pchar.questTemp.piratesLine == "waiting_Q8" && GetQuestPastDayParam("questTemp.piratesLine") > 30)
			{
				link.l1 = "Генри, я пришел, как мы и договаривались.";
				link.l1.go = "PL_Q8_1";
			}
		break;

		//********************* капитан Шарп *********************
		case "SharpPearl_1":
			dialog.text = "Хех, промышляют, это верно. Только прибыли с них - гроши... Поищи Шарпа, он у нас специалист по жемчугу, хе-хе...";
			link.l1 = "А ты не знаешь, где его можно найти?";
			link.l1.go = "SharpPearl_2";
			pchar.questTemp.Sharp = "seekSharp";
		break;
		case "SharpPearl_2":
			dialog.text = "Понятия не имею. А вообще этот лучший корсар всех времен часто ошивается в борделях. Там и ищи...";
			link.l1 = "Понятно... Спасибо, Морган.";
			link.l1.go = "exit";
		break;
		//********************* Английская линейка *********************
 		case "EngLine_quests":
			dialog.text = "У вас ко мне дело?";
			link.l1 = "Сэр, генерал-губернатор Ямайки, сэр Томас Мэдифорд нуждается в вашей помощи.";
			link.l1.go = "Step_1_1";
  		break;

 		case "Step_1_1":
			dialog.text = "А кто вы?";
			link.l1 = "Я его посланник.";
			link.l1.go = "Step_1_2";
  		break;
  		
 		case "Step_1_2":
			dialog.text = "Хм, хорошо. И чем же я могу помочь сэру Томасу Мэдифорду?";
			link.l1 = "Вы должны напасть на голландский город Кюрасао.";
			link.l1.go = "Step_1_3";
  		break;
  		
 		case "Step_1_3":
			dialog.text = "Не думаю, что это возможно в данный момент. Я не имею сейчас достаточно сил для такой операции. Передайте это Мэдифорду.";
			link.l1 = "Сэр Томас Мэдифорд рассчитывает на вас в этом деле.";
			link.l1.go = "Step_1_4";
  		break;
  		
 		case "Step_1_4":
			dialog.text = "Мне кажется, я ясно выразился, что не могу провести сейчас данную операцию. Однако если вы найдете меня несколько позже в моем доме на Ямайке, то, возможно, я смогу предложить лично вам горячее дело. Дело для настоящего корсара. А пока всего хорошего.";
			link.l1 = "Я запомню ваши слова. Думаю, нам будет о чем поговорить, как только я освобожусь. Всего наилучшего.";
			link.l1.go = "exit";
            AddQuestRecord("Eng_Line_2_Talking", "2");
			AddQuestRecord("Eng_Line_3_Morgan", "1");

            pchar.questTemp.Count = makeint(sti(pchar.questTemp.Count) + 1);
            if (pchar.questTemp.Count == "4")   {pchar.questTemp.State = "after_talks_with_pirates";}
            NextDiag.TempNode = "First time";
  		break;

 		case "hot_business":            // начало диалога по третьему квесту англ.линейки
			dialog.text = "Ага, нашел-таки время заглянуть?";
			link.l1 = "Добрый день, Генри. Я пришел узнать о 'горячем деле для настоящего корсара'. Время пришло?";
			link.l1.go = "Step_3_1";
  		break;

	 	case "Step_3_1":
            if (GetQuestPastDayParam("questTemp") < 27 && pchar.questTemp.CurQuestNumber == "3")
            {
    			dialog.text = "Хе-хе, время пришло еще раньше! Когда ты был у меня и уговаривал напасть на Кюрасао, я уже планировал экспедицию и начал подготовку к ней. Но не на Кюрасао.";
    			link.l1 = "Почему же не сказал сразу об этом?";
    			link.l1.go = "Step_3_2";
            }
            else
            {
    			dialog.text = "Время уже ушло. Ты что думал, я буду ждать тебя вечно? Хех... Я хотел предложить тебе поход на Куману, но ты мотался где-то все это время. Мы справились без тебя.";
    			link.l1 = "Эх, как жаль...";
    			link.l1.go = "exit";
                AddQuestRecord("Eng_Line_3_Morgan", "8");
                CloseQuestHeader("Eng_Line_3_Morgan");
                NextDiag.TempNode = "First time";
                if (pchar.questTemp.CurQuestNumber == "3")
                {
                    SaveCurrentQuestDateParam("questTemp");
                    pchar.questTemp.Waiting_time = "2";
                    pchar.questTemp.CurQuestNumber = "4";
                }
            }
  		break;

	 	case "Step_3_2":
			dialog.text = "Потому, что ты мотался по всему архипелагу как угорелый, ревностно пытаясь выполнить поручение Мэдифорда. А я приглашаю к участию в деле только тех людей, кто может сразу же влиться в мою эскадру. Мне лишний треп ни к чему. К тому же от самого поручения Мэдифорда отдавало душком.";
			link.l1 = "Поясни, что за душок?";
			link.l1.go = "Step_3_3";
  		break;
  		
	 	case "Step_3_3":
			dialog.text = "Дело в том, что Мэдифорд прекрасно знает, что мы, как правило, не нападаем на голландцев даже в море, не говоря уже о захвате поселений. Голландцы - самые активные торговцы, на них держится изобилие товаров в архипелаге. Если мы отвадим их от торговли, то лишимся многих вещей, которые сейчас продаются везде.\n"+
                          "К тому же, среди корсаров довольно много голландцев, это тоже фактор. Так что Мэдифорд дал тебе практически невыполнимое задание.";
			link.l1 = "Теперь ясно. Если не возражаешь, давай вернемся к нашим делам. Что за горячее дело?";
			link.l1.go = "Step_3_4";
  		break;

	 	case "Step_3_4":
			dialog.text = "Я хочу напасть на испанский город. Ты готов участвовать в кровопускании испанцам?";
			link.l1 = "Естественно! Я с тобой, за равную долю в добыче по законам берегового братства.";
			link.l1.go = "I_want_attack";
			link.l2 = "Черт! Как заманчиво это ни звучит, но я вынужден отказаться. Состояние моего корабля оставляет желать лучшего, да и команда на пределе после длительной болтанки по Карибам. Не в этот раз.";
			link.l2.go = "Step_3_6";
  		break;

	 	case "I_want_attack":    // Идем на испанцев, а может и не идем
            if (GetCompanionQuantity(pchar) == 1)   // Заставляем геймера избавиться от взятых кубиков.
            {
           		dialog.text = "Ну что ж, атакуемый город - Белиз. Мне донесли, что сейчас там собрано много золота с испанских золотых шахт на американском континенте. Золото готовится к отправке в Европу, нам нельзя медлить.\n"+
                              "Осталось завершить последние приготовления к экспедиции. Сейчас нам нужно отправиться к Джекмену на Бермуды и к Джону Моррису в Порт-оф-Спейн, что на острове Тринидад. Они готовы и ждут нас. На то, чтобы взять Белиз, у нас есть пятьдесят дней.\n"+
                              "И еще один важный момент - если ты будешь штурмовать форт и тебе улыбнется удача, то знай, что команды наших кораблей поддержат тебя при штурме.\n"+
                              "А сейчас мы должны поторопиться.";
    			link.l1 = "Вперед!";
                link.l1.go = "Step_3_7";
            }
            else
            {
                NextDiag.TempNode = "Step_3_4";
               	dialog.text = "За тобой целый хвост тянется! Мы идем на серьезное дело, нам обуза из лишних кораблей ни к чему."+
                              "Так что позаботься о своей готовности к операции, избавься от лишних компаньонов. И не тяни с этим, иначе не сможешь участвовать в деле.";
    			link.l1 = "Сделаю все, что смогу.";
    			link.l1.go = "exit";
    			link.l2 = "У меня нет возможности так резко изменить свои планы. Вынужден отказаться от предлагаемого тобой дела.";
    			link.l2.go = "Step_3_6";
             }
  		break;

	 	case "Have_hot_bussines":    // Если геймеру вдруг захочется еще пообщаться, не знаю, сможет ли. Но на всякий случай.
			dialog.text = "Чего мы ждем? Время дорого, нужно выступать.";
			link.l1 = "Конечно, выступаем немедленно.";
			link.l1.go = "exit";
            NextDiag.TempNode = "Have_hot_bussines";
  		break;

	 	case "Step_3_6":                   // Отказ от предложения Моргана атаковать испанский город.
			dialog.text = "Что ж, знать тебе деньги не нужны... Ха-ха!\n"+
                          "Такого предложения от меня больше не последует. Всего хорошего.";
			link.l1 = "Бывай, Морган. Извини...";
			link.l1.go = "exit";
			ChangeCharacterReputation(pchar, -10); 
            AddCharacterSkill(pchar, SKILL_LEADERSHIP, 0);
            
            AddQuestRecord("Eng_Line_3_Morgan", "2");
            CloseQuestHeader("Eng_Line_3_Morgan");
            pchar.questTemp.Waiting_time = "2";       // 2 так, для проформы, чтоб поддержать стандарт
            pchar.questTemp.CurQuestNumber = "4";     // идем на четвертый квест
            NextDiag.TempNode = "First time";
  		break;

	 	case "Step_3_7":                  // Согласие на участие в нападении на Белиз
			AddQuestRecord("Eng_Line_3_Morgan", "3");
            pchar.nation = PIRATE;
            pchar.questTemp.State = "AttackBeliz_GoOn";
            LAi_SetActorType(NPChar);
            LAi_ActorSetStayMode(NPChar);
            ChangeCharacterAddressGroup(NPChar, "PortRoyal_houseS1", "goto", "goto7");
            CharacterIntoCompanionAndGoOut(pchar, npchar, "reload", "reload1", 8.5, false);
            Fantom_SetBalls(npchar, "pirate");
            npchar.CompanionEnemyEnable = false;  // нет отпора при обстреле
            // ==>  Кол-во компаньонов. Нужно для раздела золота независимо от того, кто остался жив, т.е. в обойме компаньонов(по закону берегового братства).
            pchar.questTemp.CompanionQuantity = "2";
            // ==>  Даем соотв. ноду Джекмену
            characters[GetCharacterIndex("Jackman")].dialog.currentnode = "Lets_go_bussines";
            // ==>  Помещаем Джона Морриса в Бриджтаун и даем соотв. ноду
            characters[GetCharacterIndex("John Morris")].dialog.currentnode = "Lets_go_bussines";
            // ==>  Заводим таймер на прерывание, даем 31 дней.
            SetTimerCondition("time_over_Beliz_attack", 0, 0, 51, false);
            SaveCurrentQuestDateParam("questTemp");
            NextDiag.TempNode = "Have_hot_bussines";
            pchar.TempPerks.QuestTroopers = true; //перк квестового десанта
            // ==> капитулянтская нода губернатору Белиза
			characters[GetCharacterIndex("Beliz_Mayor")].dialog.captureNode = "EngLine3Quest_BelizAttack"; //капитулянтская нода мэра
			DialogExit();
			NextDiag.CurrentNode = NextDiag.TempNode;
  		break;

	 	case "time_over_Beliz":    // Геймер не успел взять Венесуэлу
			dialog.text = "Пока мы тут болтались неизвестно где, золото из Белиза ушло в Испанию, а без этого золота Белиз мне не интересен. Ты сорвал наши планы, мы уходим.";
			link.l1 = "Что ж, это ваше право.";
			link.l1.go = "Step_3_8";
			// выход с палубы
			pchar.quest.Munity = "Deads";
            LAi_LockFightMode(Pchar, false);
        break;

        case "Step_3_8":
			LAi_SetPlayerType(pchar);
            if(IsCompanion(npchar))  {RemoveCharacterCompanion(pchar, npchar);}
            if(IsCompanion(characterFromID("Jackman")))  {RemoveCharacterCompanion(pchar, characterFromID("Jackman"));}
            if(IsCompanion(characterFromID("John Morris")))  {RemoveCharacterCompanion(pchar, characterFromID("John Morris"));}

            QuestToSeaLogin_Prepare(worldMap.playerShipX, worldMap.playerShipZ, worldMap.island);
            if(worldMap.island !=	WDM_NONE_ISLAND)      // если возле острова
            {
                questToSeaLoginer.FromCoast = true;
            }
            else
            {
                questToSeaLoginer.FromCoast = false;
            }
            QuestToSeaLogin_Launch();
            
            Log_SetStringToLog("Корабли известных корсаров вышли из состава эскадры.");
            // ==> Возвращаем на место Моргана
            ChangeCharacterAddressGroup(npchar, "PortRoyal_houseS1", "sit", "sit2");
            LAi_SetHuberType(npchar);
            // ==> Возвращаем на место Джекмена
            if (CheckAttribute(pchar, "questTemp.CompanionQuantity.Jackman"))
            {
                LAi_SetHuberType(characterfromID("Jackman"));
				LAi_group_MoveCharacter(characterfromID("Jackman"), "PIRATE_CITIZENS");
                ChangeCharacterAddressGroup(&characters[GetCharacterIndex("Jackman")], "Pirates_townhall", "sit", "sit1");
                characters[GetCharacterIndex("Jackman")].dialog.currentnode = "First time";
            }
            // ==> Возвращаем на место Джона Морриса
            if (CheckAttribute(pchar, "questTemp.CompanionQuantity.Morris"))
            {
                LAi_SetHuberType(characterfromID("John Morris"));
				LAi_group_MoveCharacter(characterfromID("John Morris"), "ENGLAND_CITIZENS");
                ChangeCharacterAddressGroup(&characters[GetCharacterIndex("John Morris")], "PortSpein_houseF2", "sit", "sit1");
                characters[GetCharacterIndex("John Morris")].dialog.currentnode = "First time";
            }
            AddMoneyToCharacter(pchar, -50000);
            ChangeCharacterReputation(pchar, -20); 
            // нулим опыт в авторитете и удаче
            AddCharacterSkill(pchar, SKILL_LEADERSHIP, 0);
            AddCharacterSkill(pchar, SKILL_SNEAK, 0);
            
            AddQuestRecord("Eng_Line_3_Morgan", "6");
            CloseQuestHeader("Eng_Line_3_Morgan");
            pchar.questTemp.Waiting_time = "2";
            pchar.questTemp.CurQuestNumber = "4";     // идем на четвертый квест
            DeleteAttribute(pchar, "questTemp.CompanionQuantity");
            DeleteAttribute(pchar, "TempPerks.QuestTroopers");  //снятие перка квестового десанта
			// потом подумаем как дать солдат в помошь...
            NextDiag.TempNode = "First time";
            NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
  		break;

	 	case "Gold_found_in_fort":    // Нода разговора в форте Куманы
			dialog.text = "Золото найдено, как ты и говорил, в форте. Мы заканчиваем его погрузку в трюмы. Ты храбрый и умелый воин.";
			link.l1 = "Спасибо.";
			link.l1.go = "Step_3_9";
        break;
        
	 	case "Step_3_9":
            if (!IsCompanion(characterFromID("Henry Morgan")))
            {
                dialog.text = "Что ж, пришло время оставить этот город. И хотя я потерял здесь свой великолепный фрегат, считаю этот рейд удачным. Из Белиза каждый из нас уходит своим курсом. До свидания и будь здоров.";
            }
            else
            {
            	dialog.text = "Что ж, пришло время оставить этот город. Мы исполнили, что хотели и уходим с хорошим призом. Из Белиза каждый из нас уходит своим курсом. До свидания и будь здоров.";
            }
			link.l1 = "Всего хорошего, Морган.";
			link.l1.go = "Step_3_10";
        break;

	 	case "Step_3_10":
            if (GetCargoMaxSpace(pchar) >= RecalculateCargoLoad(pchar))
            {
                dialog.text = "Подожди еще минуту. Я тут подумал: может быть, весьма кстати, что ты не хочешь обременять себя куплей-продажей? Если так, то я готов прикупить у тебя твою долю золота.\n"+
                              "Твоя доля золота - " + pchar.Ship.Cargo.Goods.gold + " центнера. Может, продашь часть или даже все? Возьму по 200 монет за единицу веса.";
            }
            else
            {
                dialog.text = "Подожди минутку. Смотри, твой корабль просел ниже ватерлинии. У тебя перегруз, ты не сможешь далеко уйти из порта.\n"+
                              "Максимальная грузоподъемность твоего корабля " + GetCargoMaxSpace(pchar) + " центнера, а в трюмах у тебя сейчас " + RecalculateCargoLoad(pchar) + " центнера\n"+
                              "Золота у тебя " + pchar.Ship.Cargo.Goods.gold + " центнера. Может, продашь часть? Возьму по 200 монет за еденицу веса.";
            }
			link.l1 = "Нет, Морган. Со своим золотом я разберусь сам. Прощай.";
			link.l1.go = "out_fort";
			link.l2 = "Мне действительно нужно продать излишки.";
			link.l2.go = "Agree_sale_gold";
			link.l3 = "Дай подумать, я пока не готов ответить.";
			link.l3.go = "exit";
			NextDiag.TempNode = "Think_sale_gold";
        break;

	 	case "Think_sale_gold":    // ГГ думает о продаже золота
			dialog.text = "Ну что, надумал продать мне золото?";
			link.l1 = "Эх, продаю...";
			link.l1.go = "Agree_sale_gold";
			link.l2 = "Нет, Морган. Продам в другом месте. Прощай.";
			link.l2.go = "out_fort";
			link.l3 = "Подумаю еще немного...";
			link.l3.go = "exit";
			NextDiag.TempNode = "Think_sale_gold";
        break;

	 	case "Agree_sale_gold":    // ГГ согласен продать золото
			dialog.text = "Хорошо. Сколько ты хочешь продать золота?";
			link.l1 = "Готов продать 25 процентов принадлежащей мне доли золота.";
			link.l1.go = "sale_gold_25_persent";
			link.l2 = "Готов продать 50 процентов принадлежащей мне доли золота.";
			link.l2.go = "sale_gold_50_persent";
			link.l3 = "Готов продать 75 процентов принадлежащей мне доли золота.";
			link.l3.go = "sale_gold_75_persent";
			link.l4 = "Готов продать все.";
			link.l4.go = "sale_gold_all";
			link.l5 = "Извини Морган, я передумал... Прощай.";
			link.l5.go = "out_fort";
        break;
        
	 	case "sale_gold_25_persent":
			dialog.text = "Хорошо, ты продал четверть своей доли. Что ж, выгодная сделка для нас обоих. Прощай.";
			link.l1 = "Бывай, Морган. Еще увидимся.";
			link.l1.go = "out_fort";
			RemoveCharacterGoods(Pchar, GOOD_GOLD, makeint(sti(pchar.questTemp.BelizGoldQty)*0.25));
			AddMoneyToCharacter(pchar, makeint(sti(pchar.questTemp.BelizGoldQty)*0.25*200));
        break;

	 	case "sale_gold_50_persent":
			dialog.text = "Очень хорошо, ты продал половину своей доли. Что ж, выгодная сделка для нас обоих. Прощай.";
			link.l1 = "Бывай, Морган. Еще увидимся.";
			link.l1.go = "out_fort";
   			RemoveCharacterGoods(Pchar, GOOD_GOLD, makeint(sti(pchar.questTemp.BelizGoldQty)*0.5));
			AddMoneyToCharacter(pchar, makeint(sti(pchar.questTemp.BelizGoldQty)*0.50*200));
        break;

	 	case "sale_gold_75_persent":
			dialog.text = "Очень хорошо, ты продал три четверти своей доли. Что ж, выгодная сделка для нас обоих. Прощай.";
			link.l1 = "Бывай, Морган. Еще увидимся.";
			link.l1.go = "out_fort";
   			RemoveCharacterGoods(Pchar, GOOD_GOLD, makeint(sti(pchar.questTemp.BelizGoldQty)*0.75));
			AddMoneyToCharacter(pchar, makeint(sti(pchar.questTemp.BelizGoldQty)*0.75*200));
        break;

	 	case "sale_gold_all":
			dialog.text = "Ты продаешь мне все? Великолепно! Выгодная сделка для нас обоих. Прощай.";
			link.l1 = "Бывай, Морган. Еще увидимся.";
			link.l1.go = "out_fort";
   			RemoveCharacterGoods(Pchar, GOOD_GOLD, sti(pchar.questTemp.BelizGoldQty));
			AddMoneyToCharacter(pchar, makeint(sti(pchar.questTemp.BelizGoldQty)*200));
        break;

	 	case "out_fort":
			pchar.quest.time_over_Beliz_attack_Map_01.over = "yes"; // сброс, если уже откладывали
	        pchar.quest.time_over_Beliz_attack_Map_02.over = "yes";
	        pchar.quest.time_over_Beliz_attack.over = "yes";
	        		
			if(IsCompanion(characterFromID("Jackman")))  {RemoveCharacterCompanion(pchar, characterFromID("Jackman"));}
            if(IsCompanion(characterFromID("John Morris")))  {RemoveCharacterCompanion(pchar, characterFromID("John Morris"));}
            AddQuestRecord("Eng_Line_3_Morgan", "7");
            RemoveCharacterCompanion(pchar, npchar);
            // ==> Возвращаем на место Моргана
            ChangeCharacterAddressGroup(npchar, "PortRoyal_houseS1", "sit", "sit2");
            LAi_SetHuberTypeNoGroup(npchar);
            // ==> Возвращаем на место Джекмена
            if (CheckAttribute(pchar, "questTemp.CompanionQuantity.Jackman"))
            {
                ChangeCharacterAddressGroup(&characters[GetCharacterIndex("Jackman")], "Pirates_townhall", "sit", "sit1");
                LAi_SetHuberType(characterfromID("Jackman"));
                characters[GetCharacterIndex("Jackman")].dialog.currentnode = "I_know_you_good";
            }
            else
            {
                characters[GetCharacterIndex("Jackman")].dialog.currentnode = "First time";
            }
            // ==> Возвращаем на место Джона Морриса
            if (CheckAttribute(pchar, "questTemp.CompanionQuantity.Morris"))
            {
                ChangeCharacterAddressGroup(&characters[GetCharacterIndex("John Morris")], "PortSpein_houseF2", "sit", "sit1");
                LAi_SetHuberType(characterfromID("John Morris"));
                characters[GetCharacterIndex("John Morris")].dialog.currentnode = "I_know_you_good";
            }
            else
            {
               characters[GetCharacterIndex("John Morris")].dialog.currentnode = "First time";
            }
            ChangeCharacterReputation(pchar, 1); 
            AddCharacterExpToSkill(pchar, "Leadership", 20);
            AddCharacterExpToSkill(pchar, "Fencing", 500);
            AddCharacterExpToSkill(pchar, "Pistol", 550);
            AddCharacterExpToSkill(pchar, "Sailing", 550);
            AddCharacterExpToSkill(pchar, "Accuracy", 330);
            AddCharacterExpToSkill(pchar, "Cannons", 150);

            CloseQuestHeader("Eng_Line_3_Morgan");
            pchar.questTemp.Waiting_time = "2";
            pchar.questTemp.CurQuestNumber = "4";     // идем на четвертый квест
            pchar.questTemp.State = "empty";
            DeleteAttribute(pchar, "questTemp.CompanionQuantity");
            DeleteAttribute(pchar, "questTemp.BelizGoldQty");
            DeleteAttribute(pchar, "TempPerks.QuestTroopers"); //дублирование снятия перка квестового десанта
            LAi_LocationFightDisable(&Locations[FindLocation("Beliz_fort")], false); //вернем боевки
            chrDisableReloadToLocation = false;
            pchar.nation = ENGLAND; // вернем нацию
            InterfaceStates.Buttons.Save.enable = true;   // вернем запись
			DoReloadCharacterToLocation("Beliz_town",  "reload", "gate"); //ГГ в город
            NextDiag.CurrentNode = "I_know_you_good";
			DialogExit();
        break;

	 	case "Give_blade":
			dialog.text = GetFullName(pchar) + ", я благодарен тебе до глубины души. Старик Мэнсфилд был очень хорошим человеком и заслуживал большего, нежели быть зарезанным испанцем.";
			link.l1 = "Я видел Мэнсфилда только несколько раз и близко с ним не общался. Но мне он показался честным и благородным человеком.";
			link.l1.go = "Step_9_1";
        break;
        
	 	case "Step_9_1":
			dialog.text = "И это действительно так.\n"+
                          "В награду за такое дело прошу принять от меня подарок - мою рапиру. Этому клинку нет равных на Карибах.";
			link.l1 = "Спасибо, Морган. Этот подарок делает мне честь. Прощай.";
			link.l1.go = "exit";
			NextDiag.TempNode = "I_know_you_good";
			
            GiveItem2Character(pchar, "blade27");
            RemoveCharacterEquip(NPChar, BLADE_ITEM_TYPE);
            TakeItemFromCharacter(NPChar, "blade27");
            GiveItem2Character(NPChar, "blade22");
            EquipCharacterByItem(NPChar, "blade22");
        break;
		//********************* Голландская линейка *********************
        case "HolLine3_fromMansfield":
            dialog.text = "Чему обязан за удовольствие видеть вас здесь?";
            link.l1 = "Я прибыл к вам по поручению Эдварда Мэнсфилда.";
            link.l1.go = "Step_H3_1";
        break;
 	 	case "Step_H3_1":
			dialog.text = "Слушаю вас внимательнейшим образом.";
			link.l1 = "Эдвард просит вас оказать мне помощь. Мне нужна информация о планах генерал-губернатора Порт Рояля Томаса Мэдифорда. Меня интересует, есть ли у него приказ на открытие боевых действий против голландских колоний и что конкретно он собирается предпринять.";
			link.l1.go = "Step_H3_2";
        break;
 	 	case "Step_H3_2":
			dialog.text = "Хм, ни много, ни мало - а именно вот это и интересует?!";
			link.l1 = "Да, именно так, сэр.";
			link.l1.go = "Step_H3_3";
        break;
 	 	case "Step_H3_3":
			dialog.text = "Ну что же, тогда давайте сразу к делу. Мэнсфилд мне - как отец родной, ради него я готов на многое. Но вот ради Голландии я на многое не готов. Поэтому если хотите, чтобы я вам помог, придется вам поработать лично на меня.";
			link.l1 = "Я выполняю поручения только генерал-губернатора Кюрасао.";
			link.l1.go = "Step_H3_NoWork";
			link.l2 = "Я готов выполнить любое ваше поручение.";
			link.l2.go = "Step_H3_4";
        break;
 	 	case "Step_H3_NoWork":
			dialog.text = "В таком случае прощайте. Нам не о чем более говорить.";
			link.l1 = "Очень жаль...";
			link.l1.go = "exit";
			NextDiag.TempNode = "First time";
    		pchar.questTemp.State = "WormEnglPlans_SurrenderWorkMorgan";
    		AddQuestRecord("Hol_Line_3_WormEnglishPlans", "4");
        break;
 	 	case "Step_H3_4":
			dialog.text = "Хорошо. Поручение следующее: на Тортуге в данный момент ошивается флибустьер по имени Пьер Пикардиец. Мы вместе участвовали в одном деле, и я подозреваю, что Пикардиец присвоил часть добычи.\n"+
                          "Мне нужно это проверить. Отправляйтесь на Тортугу - вас там никто толком не знает, - и попытайтесь добыть мне максимум информации о Пикардийце.";
			link.l1 = "Ждите, я скоро буду у вас с результатами.";
			link.l1.go = "exit";
			NextDiag.TempNode = "HolLine3_aboutWorm";
    		pchar.questTemp.State = "WormEnglPlans_WormPicarder";
    		AddQuestRecord("Hol_Line_3_WormEnglishPlans", "6");
        break;
 	 	case "HolLine3_aboutWorm":
			dialog.text = "Что вы узнали? Как там поживает мой большой друг и соратник Пьер Пикардиец?";
            if (CheckAttribute(pchar, "questTemp.Talks"))
            {
    			link.l1 = "Да неплохо, сорит деньгами направо и налево.";
    			link.l1.go = "Step_H3_5";
            }
            else
            {
    			link.l1 = "Пока ничего существенного мне узнать не удалось...";
    			link.l1.go = "exit";
            }
        break;
 	 	case "Step_H3_5":
            sTemp = "Мне удалось разузнать следующие факты.";
            if (CheckAttribute(pchar, "questTemp.Talks.Tavern")) sTemp = sTemp + " Пьер Пикардиец снял комнату в таверне Тортуги на год вперед.";
            if (CheckAttribute(pchar, "questTemp.Talks.Store")) sTemp = sTemp + " Скупает драгоценности в магазине Тортуги чуть ли не оптом.";
            if (CheckAttribute(pchar, "questTemp.Talks.Shipyard")) sTemp = sTemp + " Поставил на свой фрегат новейшие дорогущие кулеврины.";
            if (CheckAttribute(pchar, "questTemp.Talks.Usurer")) sTemp = sTemp + " Ростовщику дал в рост такую сумму, что тот теперь ни у кого не принимает деньги.";
            if (CheckAttribute(pchar, "questTemp.Talks.Brothel")) sTemp = sTemp + " В борделе он завсегдатай и желанный гость.";
            dialog.text = "С этого момента я хочу, чтобы вы говорили максимально подробно. На что он конкретно тратит деньги?";
			link.l1 = sTemp;
			link.l1.go = "Step_H3_6";
        break;
 	 	case "Step_H3_6":
            if (CheckAttribute(pchar, "questTemp.Talks.Tavern") && CheckAttribute(pchar, "questTemp.Talks.Store") && CheckAttribute(pchar, "questTemp.Talks.Shipyard") && CheckAttribute(pchar, "questTemp.Talks.Usurer") && CheckAttribute(pchar, "questTemp.Talks.Brothel"))
 	 	    {
    			dialog.text = "Ну что же, я полагаю, все ясно. Такой суммой Пикардиец не располагал ранее и не получил в результате проведенной совместно операции. Учитывая мои подозрения, ему придется серьезно попотеть, дабы обосновать такие траты... Я благодарю вас.";
    			link.l1 = "Гм, рад, что смог помочь. Ну а теперь, если вы не возражаете, давайте приступим к тому делу, по которому я сюда и прибыл, собственно.";
    			link.l1.go = "Step_H3_10";
 	 	    }
            else
            {
                dialog.text = "Понятно. Но это еще ничего не доказывает точно. Я знаю, что такие суммы у него есть в наличии. Так что вам придется вернуться на Тортугу и продолжить сбор информации.";
    			link.l1 = "Я узнал достаточно много, чтобы сделать соответствующие выводы.";
    			link.l1.go = "Step_H3_7";
    			link.l2 = "Хорошо, еще раз побываю на Тортуге и продолжу расспросы.";
    			link.l2.go = "exit";
    		}
        break;
 	 	case "Step_H3_7":
			dialog.text = "Я так не думаю.";
			link.l1 = "Деньги у Пьера Пикардийца есть.";
			link.l1.go = "Step_H3_8";
        break;
 	 	case "Step_H3_8":
			dialog.text = "Конечно, есть, он же получил свою долю. Вопрос в том, сколько.";
			link.l1 = "Этого я узнать не смогу.";
			link.l1.go = "Step_H3_9";
        break;
 	 	case "Step_H3_9":
			dialog.text = "В таком случае считайте, что мы не договорились. Ради Мэнсфилда я не буду никого информировать о вашем странном желании так много знать. Прощайте, вы можете спокойно убираться восвояси.";
			link.l1 = "Хм, ну что же, прощайте.";
			link.l1.go = "exit";
			NextDiag.TempNode = "First time";
			AddQuestRecord("Hol_Line_3_WormEnglishPlans", "12");
            DeleteAttribute(pchar, "questTemp.Talks");
        break;
 	 	case "Step_H3_10":
			dialog.text = "Дело в том, что я не знаю, какие планы у Мэдифорда и какие приказания он получил по поводу участия в войне. Однако мне известно, что Мэдифорд готовит десантную операцию силами гарнизона Порт Рояля.";
			link.l1 = "Этого мне мало, мне нужно знать точно, что он затевает.";
			link.l1.go = "Step_H3_11";
        break;
 	 	case "Step_H3_11":
			dialog.text = "Есть способ это узнать. Сейчас в тюрьме сидит английский офицер, лейтенант по имени Алан Маклейн, которому завтра должны повязать пеньковый галстук. Вчера он отправил к праотцам своего коллегу из гарнизона в пьяной драке. Вытащи его из тюрьмы, и он расскажет тебе, что затевается. И помни, что завтра его повесят.";
			link.l1 = "Я понял. Спасибо за совет, Генри.";
			link.l1.go = "exit";
			NextDiag.TempNode = "I_know_you_good";
            AddQuestRecord("Hol_Line_3_WormEnglishPlans", "13");
            DeleteAttribute(pchar, "questTemp.Talks");
            pchar.questTemp.State = "WormEnglPlans_SaveOfficer";
            sld = GetCharacter(NPC_GenerateCharacter("Alan_MacLine", "off_eng_2", "man", "man", 10, ENGLAND, 1, false));
        	sld.name 	= "Алан";
        	sld.lastname = "Маклейн";
            sld.Dialog.Filename = "Quest\HolLineNpc_1.c";
        	LAi_SetStayTypeNoGroup(sld);
            ChangeCharacterAddressGroup(&characters[GetCharacterIndex("Alan_MacLine")], "SentJons_prison", "goto", "goto9");
        break;
        // ==> Квест №6 голландки.
 	 	case "HolLine6_fromStuvesant":
			dialog.text = "Что привело вас ко мне на этот раз?";
			link.l1 = "Я уполномочен заявить, что Нидерланды хотят возмездия за смерть Эдварда Мэнcфилда.";
			link.l1.go = "Step_H6_1";
        break;
 	 	case "Step_H6_1":
			dialog.text = "Что же, Эдвард был голландцем, я понимаю... Что вы планируете предпринять?";
			link.l1 = "Мне необходимо согласовать с вами порядок моих действий.";
			link.l1.go = "Step_H6_2";
        break;
 	 	case "Step_H6_2":
			dialog.text = "Хм, разумно. Ну что же, давайте я изложу вам то, что знаю сам. Испанцы совершили нападение на поселение буканьеров. Заказчиком и непосредственным организатором этого является генерал-губернатор Сантьяго Хосе Санчо Хименес, именно он задумал и финансировал эту операцию.";
			link.l1 = "Мы нападем на Сантьяго?";
			link.l1.go = "Step_H6_3";
        break;
 	 	case "Step_H6_3":
			dialog.text = "Возможно. Однако нам не стоит торопиться - уже выстроилась очередь за головой генерал-губернатора Сантьяго. Англичане тоже хотят его крови, а вы с Англией в войне и совместные действия невозможны. Посему эта партия должна быть четко расписана по ролям.";
			link.l1 = "Да-а-а, у Хименеса большие проблемы.";
			link.l1.go = "Step_H6_4";
        break;
 	 	case "Step_H6_4":
			dialog.text = "Хех, это верно. Итак, если вы хотите участвовать в этом деле, вам надлежит провести разведывательную миссию.";
			link.l1 = "Что конкретно я должен сделать?";
			link.l1.go = "Step_H6_5";
        break;
 	 	case "Step_H6_5":
			dialog.text = "Вы должны пробраться в Сантьяго и узнать, на месте ли генерал-губернатор этого города, небезызвестный Хосе Санчо Хименес. Нападение должно быть совершено именно тогда, когда генерал-губернатор будет в городе. Поэтому желательно знать его распорядок на ближайшее время.";
			link.l1 = "Все ясно. Скоро я буду у вас с необходимой информацией.";
			link.l1.go = "exit";
            AddQuestRecord("Hol_Line_6_RevengeMansfield", "2");
            pchar.questTemp.State = "RevengeMansfield_toTavern";
            NextDiag.TempNode = "I_know_you_good";
            LAi_group_MoveCharacter(characterFromID("Santiago_tavernkeeper"), "QuestGroup");
            LAi_group_MoveCharacter(characterFromID("Santiago_waitress"), "QuestGroup");
        break;
 	 	case "HolLine6_fromSantiago":
			dialog.text = "Ну, говори быстрей, что удалось узнать.";
			link.l1 = "Хименес в городе и никуда не собирается в ближайшее время. У них в Сантьяго намечается праздник города и без генерал-губернатора никак.";
			link.l1.go = "Step_H6_6";
        break;
 	 	case "Step_H6_6":
			dialog.text = "Отлично! Ну что же, ты сделал свое дело, спасибо. Ваш весомый вклад в это святое дело будет по достоинству оценен Береговым братством.";
			link.l1 = "Я очень рад, что мы смогли помочь вам. Ну, а как же штурм города?";
			link.l1.go = "Step_H6_7";
        break;
 	 	case "Step_H6_7":
			dialog.text = "Это уже не ваша задача, вы свое дело сделали. Мэнсфилд будет отомщен.";
			link.l1 = "Я понял. Прощай, Морган.";
			link.l1.go = "exit";
            pchar.questTemp.State = "RevengeMansfield_FromMorganWithResult";
            AddQuestRecord("Hol_Line_6_RevengeMansfield", "4");
            NextDiag.TempNode = "I_know_you_good";
        break;
		//********************* Французская линейка *********************
        case "FraLine8_talk":
            dialog.text = "Ага, да это сам капитан " + GetFullName(pchar) + "! Я давно за тобой присматриваю. До сих пор ты не лез в мои дела, и поэтому еще жив и здоров. Говори, зачем пожаловал. Полагаю, не для праздных разговоров.";
            link.l1 = "Послушай, Морган, те же знаешь, что просто так я не стал бы тебя беспокоить. Я по делу.";
            link.l1.go = "Step_F8_1";
        break;
        case "Step_F8_1":
            dialog.text = "Выкладывай...";
            link.l1 = "Что ты думаешь о предстоящей англо-голландской заварушке?";
            link.l1.go = "Step_F8_2";
        break;
        case "Step_F8_2":
            dialog.text = "Ты тоже в курсе... Ну что же, могу сказать, что голландцы в этих водах нам не мешают, более того, они нам выгодны - на них держится вся торговля в этих краях. Так что сама затея с войной мне не нравится, да и английские военные не смогут толком досадить голландцам, только бесполезную кровь прольют на радость испанцам.\n"+
				          "Однако полагаю, что старый лис Мэдифорд попытается вытащить каштаны из огня чужими руками.";
            link.l1 = "Что верно, то верно. Есть мнение, что он попытается использовать в войне Береговое братство.";
            link.l1.go = "Step_F8_3";
        break;
        case "Step_F8_3":
            dialog.text = "Такое вполне может быть...";
            link.l1 = "Генри, не хочу ходить вокруг да около. Мне нужно, чтобы ты не принимал участия в нападении на голландцев. Могу сказать, меня радует тот факт, что война эта тебе не интересна...";
            link.l1.go = "Step_F8_4";
        break;
        case "Step_F8_4":
            dialog.text = "Хм, здесь есть проблема - эта война мне неприятна, но я бы не сказал, что она мне не интересна. Дело в том, что моя пьяная гвардия уже спустила в тавернах последние пиастры, а крупных дел у нас давно не было. Впрочем, тот, кто прислал тебя с поручением отговорить меня, наверняка предусмотрел такой вариант?";
            link.l1 = "Ага, ни дав при этом ни гроша. Но это мои заботы. Сколько понадобится твоим головорезам, чтобы веселиться на берегу и не вмешиваться в большую политику?";
            link.l1.go = "Step_F8_5";
        break;
        case "Step_F8_5":
            dialog.text = "До политики им дела вообще нет, а 250 тысяч, я думаю, на пару-тройку недель хватит.";
            link.l1 = "Я намерен возместить тебе потери от ненападения на голландцев.";
            link.l1.go = "Step_F8_6";
        break;
        case "Step_F8_6":
            dialog.text = "250 тысяч на бочку - и я пас в голландских делах Мэдифорда.";
			if (sti(pchar.money) >= 250000)
			{
				link.l1 = "Ну, здоровы же твои ребята ром хлестать! Бери деньги и помни: это уговор чести.";
				link.l1.go = "Step_F8_7";
				AddMoneyToCharacter(pchar, -250000);
			}
			else
			{
				link.l1 = "Сейчас у меня нет таких денег, но я обязательно их найду и отдам тебе. Так что подожди чуть-чуть.";
				link.l1.go = "exit";
				NextDiag.TempNode = "Step_F8_6";
			}
        break;
        case "Step_F8_7":
            dialog.text = "С каких это пор мне напоминают о чести? Ты говори, да не заговаривайся...";
            link.l1 = "Извини, Генри, не хотел тебя обидеть. Ну, прощай и будь здоров.";
            link.l1.go = "exit";
			pchar.questTemp.Count = makeint(pchar.questTemp.Count)+1;
			NextDiag.TempNode = "I_know_you_good";
			AddQuestRecord("Fra_Line_8_ThreeCorsairs", "4");
        break;
		//********************* Пиратская линейка *********************
		//квест №1, диалог в тюрьме
		case "PL_Q1_1":
			dialog.text = "Да-а-а, парень, попал ты в переплет. Хорошо, что не стал сопротивляться аресту. Здесь, в Порт Рояле, мне многое подвластно.\nКомендант мне cразу доложил о тебе, так что вопрос о твоем освобождении уже решен.";
			link.l1 = "А как Джон Болтон? Я же вез его к тебе из Пуэрто-Принсипе.";
			link.l1.go = "PL_Q1_2";
		break;
		case "PL_Q1_2":
			dialog.text = "Его я тоже освобожу. Позже. Кто-то пытался меня подставить, и я узнаю кто.\nТеперь о тебе. У меня будет для тебя задание, сразу предупреждаю - отказа не приму, за тобой должок. Приводи свои дела в порядок и подходи ко мне.";
			link.l1 = "Хорошо, я приду. И, Морган...";
			link.l1.go = "PL_Q1_3";
		break;
		case "PL_Q1_3":
			dialog.text = "Да?";
			link.l1 = "Спасибо!";
			link.l1.go = "exit";
			NextDiag.TempNode = "PL_Q2_1";
			AddDialogExitQuestFunction("PiratesLine_q1_MorganEnd");
		break;
		//квест №2, диалог в резиденции Порт Рояля.
		case "PL_Q2_1":
			CloseQuestHeader("Pir_Line_1_JohnBolton");
			dialog.text = "А-а-а, пришел. Это хорошо.\nКое-что я уже выяснил по Болтону. Сейчас тебе нужно отправиться на Мартинику. Там живет один проходимец, зовут его Эдвард Лоу. Как его разыщешь, вручи ему от меня Черную Метку.";
			link.l1 = "На словах ему что-нибудь передать?";
			link.l1.go = "PL_Q2_2";
		break;
		case "PL_Q2_2":
			dialog.text = "Хех, ну какие могут быть слова, когда ты даешь ЧЕРНУЮ МЕТКУ?!\nОдного этого достаточно. Но убивать его тебе не стоит, пусть все обдумает, понервничает. А с Черной Меткой ему не прожить и пары дней.\nВсе, иди, по возвращении тебя будет ждать награда.";
			link.l1 = "Я понял, Генри, приступаю.";
			link.l1.go = "exit";
			sld = GetCharacter(NPC_GenerateCharacter("EdwardLoy", "officer_10", "man", "man", 30, PIRATE, -1, true));
			FantomMakeCoolFighter(sld, 30, 90, 50, "blade25", "pistol3", 100);
			FantomMakeCoolSailor(sld, SHIP_BRIGSW, "Морской волк", CANNON_TYPE_CANNON_LBS24, 90, 90, 90);
			sld.name = "Эдвард";
			sld.lastname = "Лоу";
			sld.rank = 30;
			sld.city = "LeFransua";
			sld.location	= "LeFransua_House4";
			sld.location.group = "goto";
			sld.location.locator = "goto1";
			sld.dialog.filename   = "Quest\PiratesLine_dialog.c";
			sld.dialog.currentnode   = "EdwardLoy";
			sld.greeting = "pirat_quest";
			sld.AlwaysSandbankManeuver = true;
			sld.AnalizeShips = true;  //анализировать вражеские корабли при выборе таска
			LAi_SetWarriorType(sld);
			GiveItem2Character(pchar, "BlackLabel");
			LAi_group_MoveCharacter(sld, "PIRATE_CITIZENS");
			pchar.questTemp.piratesLine = "BlackLabel_toLaVega";
            SetQuestHeader("Pir_Line_2_BlackLabel");
            AddQuestRecord("Pir_Line_2_BlackLabel", "1");
			NextDiag.TempNode = "PL_Q2_wait";
		break;
		case "PL_Q2_wait":
			dialog.text = "Вручил Метку?";
			link.l1 = "Нет еще, Морган, занимаюсь этим...";
			link.l1.go = "exit";
			NextDiag.TempNode = "PL_Q2_wait";
		break;

		case "PL_Q2_away_1":
			dialog.text = "Ну, рассказывай, как все прошло?";
			link.l1 = "Генри, возвращаю тебе метку.";
			link.l1.go = "PL_Q2_away_2";
			TakeItemFromCharacter(pchar, "BlackLabel");
			CloseQuestHeader("Pir_Line_2_BlackLabel");
		break;
		case "PL_Q2_away_2":
			dialog.text = "Ты в своем уме?!!";
			link.l1 = "Ну да, я в полном порядке. Эдвард мне сообщил, что вы все уладили...";
			link.l1.go = "PL_Q2_away_3";
		break;
		case "PL_Q2_away_3":
			dialog.text = "Уладили?! Чтобы я, Генри Морган, что-то улаживал с этой шпаной?! Ну, ты меня здорово удивил, приятель...";
			link.l1 = "Выходит, этот Лоу меня обманул?";
			link.l1.go = "PL_Q2_away_4";
		break;
		case "PL_Q2_away_4":
			dialog.text = "Обвел вокруг пальца, как слепого щенка! М-да, я даже не знаю, что и думать теперь о тебе...";
			link.l1 = "Генри, дай мне шанс все исправить! Да я его на клочки порву! Без всяких черных меток на ремни порежу!";
			link.l1.go = "PL_Q2_away_5";
		break;
		case "PL_Q2_away_5":
			dialog.text = "У-у-у, какой грозный капер, прямо страшно!.. Ну ладно, сам напакостил - сам исправляй, это справедливо. Разыщи своего 'друга' Лоу и пусти ему кровь.";
			link.l1 = "Я так и сделаю, адмирал!";
			link.l1.go = "PL_Q2_away_6";
		break;
		case "PL_Q2_away_6":
			dialog.text = "Хорошо. Жду тебя с хорошими новостями. И не будь таким легковерным...";
			link.l1 = "Конечно, Генри. Я приступаю!";
			link.l1.go = "exit";
            SetQuestHeader("Pir_Line_3_KillLoy");
            AddQuestRecord("Pir_Line_3_KillLoy", "1");
			NextDiag.TempNode = "PL_Q2_awaywait";
			pchar.questTemp.piratesLine = "KillLoy_toHouse";
			ChangeCharacterAddress(characterFromId("EdwardLoy"), "none", "");
			pchar.quest.PiratesLine_q3_LoyHouse.win_condition.l1 = "location";
			pchar.quest.PiratesLine_q3_LoyHouse.win_condition.l1.location = "LeFransua_House4";
			pchar.quest.PiratesLine_q3_LoyHouse.function = "PiratesLine_q3_LoyHouse";
		break;
		case "PL_Q2_awaywait":
			dialog.text = "Эдвард Лоу мертв?";
			link.l1 = "Нет еще, Морган, но скоро будет.";
			link.l1.go = "exit";
			link.l2 = "Знаешь, Морган, надоело мне все это. Не нашел я Лоу, и искать уже нет никаких сил...";
			link.l2.go = "PL_Q3_notFound";
			NextDiag.TempNode = "PL_Q2_awaywait";
		break;

		case "PL_Q3_notFound":
			dialog.text = "Что-то у тебя вид невеселый, никак упустил этого Лоу?";
			link.l1 = "Упустил...";
			link.l1.go = "PL_Q3_notFound_1";
		break;
		case "PL_Q3_notFound_1":
			dialog.text = "Что ж, почему-то я так и подумал. Может, и не стоило тебе такие поручения давать, может, твоя стихия - налеты, абордажи, схватки?\nВремя покажет. Ступай, возможно, когда-нибудь, я дам тебе еще один шанс.";
			link.l1 = "Спасибо и на этом, Морган. Извини...";
			link.l1.go = "exit";
			NextDiag.TempNode = "I_know_you_good";
			pchar.questTemp.piratesLine = "waiting_Q4";
			SaveCurrentQuestDateParam("questTemp.piratesLine");
			CloseQuestHeader("Pir_Line_3_KillLoy");
		break;

		case "PL_Q3_GoodWork":
			dialog.text = "По твоей сияющей физиономии можно понять, что все прошло успешно.";
			link.l1 = "Да! Эдвард Лоу мертв!";
			link.l1.go = "PL_Q3_GoodWork_1";
		break;
		case "PL_Q3_GoodWork_1":
			dialog.text = "Прекрасная новость. Удалось что-нибудь у него узнать?";
			link.l1 = "Только то, что много недовольных тобой среди пиратов. Так сказал Лоу.";
			link.l1.go = "PL_Q3_GoodWork_2";
		break;
		case "PL_Q3_GoodWork_2":
			dialog.text = "Ну, всем не угодишь, и стремиться к этому не стоит! В общем, ты заслужил награду. Получи 20 тысяч. Заглядывай периодически ко мне, возможно скоро для тебя появится задание.";
			link.l1 = "Хорошо, Морган, спасибо!";
			link.l1.go = "exit";
			NextDiag.TempNode = "I_know_you_good";
			pchar.questTemp.piratesLine = "waiting_Q4";
			SaveCurrentQuestDateParam("questTemp.piratesLine");
			CloseQuestHeader("Pir_Line_3_KillLoy");
			AddMoneyToCharacter(pchar, 20000);
		break;
		//квест №4, жемчужные тартаны
		case "PL_Q4_1":
			dialog.text = "Хорошо, что ты зашел. Мне как раз нужен кто-то вроде тебя. Хочешь поучаствовать в моей задумке? Сразу предупреждаю, славы тут немного, зато доход неплохой.";
			link.l1 = "Деньги на дороге не валяются. Почему бы и нет, что за задумка?";
			link.l1.go = "PL_Q4_2";
		break;
		case "PL_Q4_2":
			dialog.text = "Хм, в данном случае все достаточно просто. Составим конкуренцию лучшему пирату всех времен - мистеру Бартоломью Шарпу? Короче, у меня появилась информация, где будут находиться тартаны ловцов жемчуга ближайший месяц.";
			link.l1 = "Неплохо. И где это?";
			link.l1.go = "PL_Q4_3";
		break;
		case "PL_Q4_3":
			dialog.text = "У Теркса, в заливе Северный. Так что отправляйся туда и наведи шорох на промысле. Жду тебя с кучей жемчуга.";
			link.l1 = "Это сделаем...";
			link.l1.go = "PL_Q4_4";
		break;
		case "PL_Q4_4":
			dialog.text = "Тебе нужно привезти не менее тысячи малых и пятисот больших жемчужин. Так что постарайся. Добыча - пополам.";
			link.l1 = "Понял, Генри. Можешь на меня рассчитывать, я не поведу!";
			link.l1.go = "exit";
			NextDiag.TempNode = "PL_Q4_wait";
			pchar.questTemp.piratesLine = "PearlQ4_toTerks"; //флаг квеста
			pchar.questTemp.Sharp.PearlAreal = "Terks";
			pchar.questTemp.Sharp.PearlAreal.Island = "Terks";
			pchar.questTemp.Sharp.PearlAreal.Shore = "Shore56";
			pchar.questTemp.Sharp.PearlAreal.locator = "Quest_ship_1";
			pchar.questTemp.Sharp.PearlAreal.terms = 30; //сроки
			pchar.questTemp.Sharp.PearlAreal.qtyShips = 9; //кол-во тартан ловцов
			pchar.questTemp.Sharp.PearlAreal.SmallPearlQty = 0; //для итогового подсчета
			pchar.questTemp.Sharp.PearlAreal.BigPearlQty = 0;
			SetQuestHeader("Pir_Line_4_Pearl");
			AddQuestRecord("Pir_Line_4_Pearl", "1");
			pchar.quest.PQ4_SeaPearl_login.win_condition.l1 = "location";
			pchar.quest.PQ4_SeaPearl_login.win_condition.l1.location = "Terks";
			pchar.quest.PQ4_SeaPearl_login.function = "PQ4_SeaPearl_login";
			SetTimerFunction("PQ4_SeaPearl_Late", 0, 0, sti(pchar.questTemp.Sharp.PearlAreal.terms));
		break;

		case "PL_Q4_wait":
			dialog.text = "Ограбил ловцов жемчуга?";
			link.l1 = "Нет еще, Морган, я занимаюсь этим...";
			link.l1.go = "exit";
			NextDiag.TempNode = "PL_Q4_wait";
		break;
		//опоздал
		case "PL_Q4_late":
			dialog.text = "Ну, что скажешь?";
			link.l1 = "Генри, извини, но я опоздал к Терксу.";
			link.l1.go = "PL_Q4_late_1";
		break;
		case "PL_Q4_late_1":
			dialog.text = "Приятель, ты форменный недотепа! Хотя я сам виноват. У меня куча пиратов в распоряжении, а я тебе отдал это дело. Какой просчет!..";
			link.l1 = "Генри, я исправлюсь!";
			link.l1.go = "PearlQ4_late_2";
		break;
		case "PearlQ4_late_2":
			dialog.text = "Обязательно. Только как-нибудь в другой раз. А пока иди куда-нибудь подальше, мне не до тебя...";
			link.l1 = "Хорошо, адмирал...";
			link.l1.go = "exit";
			NextDiag.TempNode = "I_know_you_good";
			pchar.questTemp.piratesLine = "waiting_Q5";
			SaveCurrentQuestDateParam("questTemp.piratesLine");
			CloseQuestHeader("Pir_Line_4_Pearl");
			//дача пятого квеста
            Pchar.quest.PQ5_meetInPortRoyal.win_condition.l1 = "location";
            Pchar.quest.PQ5_meetInPortRoyal.win_condition.l1.location = "PortRoyal_town";
            Pchar.quest.PQ5_meetInPortRoyal.function = "PQ5_meetInPortRoyal";
		break;
		//неудачный наезд
		case "PL_Q4_badWork":
			dialog.text = "Ну, как дела с жемчугом?";
			link.l1 = "Плохо. Эти чертовы тартаны рассыпались от меня кто куда. Ни одно не догнал...";
			link.l1.go = "PL_Q4_badWork_1";
		break;
		case "PL_Q4_badWork_1":
			dialog.text = "Ха! Ну и гусь же ты лапчатый! Надо же было подготовиться! Хотя я сам виноват. У меня куча пиратов в распоряжении, а я тебе отдал это дело. Какой просчет!..";
			link.l1 = "Генри, это моя ошибка. Но я тебя прошу, не делай поспешный выводов! Я исправлю положение...";
			link.l1.go = "PearlQ4_late_2";
		break;
		//удачный наезд
		case "PL_Q4_goodWork":
			dialog.text = "Ну, где наш жемчуг, приятель?";
			if (!CheckAttribute(pchar, "items.jewelry12") || !CheckAttribute(pchar, "items.jewelry11"))
			{
				link.l1 = "Знаешь, Генри, как-то все неудачно вышло. С жемчугом, в общем, проблемы... Нет его.";
				link.l1.go = "PL_Q4_goodWork_lair";
			}
			else
			{
				link.l1 = "У меня. Мне удалось добыть " + pchar.items.jewelry12 + " малых и " + pchar.items.jewelry11 + " больших жемчужин!";
				link.l1.go = "PL_Q4_goodWork_1";
			}
		break;

		case "PL_Q4_goodWork_lair":
			dialog.text = "Да ты, оказывается, крыса! Ты что, идиот, думаешь я не в курсе, что ты операцию провел?!";
			link.l1 = "Генри, не горячись...";
			link.l1.go = "PL_Q4_goodWork_lair_1";
		break;
		case "PL_Q4_goodWork_lair_1":
			dialog.text = "Не горячись?!! Да уж, наглости тебе не занимать. Вот что, крыса, тебе конец. Собственными руками прикончу... Закрыть двери!";
			link.l1 = "Генри, Генри!..";
			link.l1.go = "fight";
			chrDisableReloadToLocation = true; 
			pchar.questTemp.piratesLine = "over";
		break;

		case "PL_Q4_goodWork_1":
			if (sti(pchar.items.jewelry12) < sti(pchar.questTemp.piratesLine.Qty.SmallPearl) || sti(pchar.items.jewelry11) < sti(pchar.questTemp.piratesLine.Qty.BigPearl))
			{
				dialog.text = "Хм, погоди, у меня другие сведения. Ты получил от грабежа " + pchar.questTemp.piratesLine.Qty.SmallPearl + " малых и " + pchar.questTemp.piratesLine.Qty.BigPearl + " больших жемчужин. Что скажешь?";
				link.l1 = "Да ну что ты, Генри! Я тебе правду говорю.";
				link.l1.go = "PL_Q4_goodWork_2";
			}
			else
			{
				if (sti(pchar.items.jewelry12) >= 1000 && sti(pchar.items.jewelry11) >= 500)
				{
					dialog.text = "Отлично, ты хорошо поработал. К тому же, выполнил условия сделки - привез жемчуга не меньше означенного количества. Молодец, что еще я могу сказать!";
					link.l1 = "Адмирал, твои слова греют мое сердце... Ну, так как насчет моей доли?";
					link.l1.go = "PL_Q4_goodWork_3";
				}
				else
				{
					dialog.text = "Гм, не могу сказать, что я доволен. Ты не уложился в означенный план - 1000 мелких и 500 крупных жемчужин. Так что извини, но я забираю все себе...";
					link.l1 = "Как это, адмирал! Я же выполнил работу!";
					link.l1.go = "PL_Q4_goodWork_5";
				}
			}
		break;
		case "PL_Q4_goodWork_2":
			dialog.text = "А вот я так не думаю. Сведения мои исключительно верны. Ты прикарманил жемчуг. Наказание за это одно - смерть. А ну, закройте двери, сейчас крысу давить буду...";
			link.l1 = "Генри, ты чего это?!..";
			link.l1.go = "fight";
			chrDisableReloadToLocation = true; 
			pchar.questTemp.piratesLine = "over";
		break;

		case "PL_Q4_goodWork_3":
			dialog.text = "Половина добычи - твоя, как и договаривались. Остальное - мое.";
			link.l1 = "Отлично! Ну что, адмирал, я, пожалуй, пойду...";
			link.l1.go = "PL_Q4_goodWork_4";
			TakeNItems(pchar, "jewelry12", -makeint(sti(pchar.items.jewelry12)/2));
			TakeNItems(pchar, "jewelry11", -makeint(sti(pchar.items.jewelry11)/2));
		break;
		case "PL_Q4_goodWork_4":
			dialog.text = "Не задерживаю. Заходи позже, может еще что-нибудь подвернется...";
			link.l1 = "Хорошо, адмирал.";
			link.l1.go = "exit";
			NextDiag.TempNode = "I_know_you_good";
			pchar.questTemp.piratesLine = "waiting_Q5";
			SaveCurrentQuestDateParam("questTemp.piratesLine");
			CloseQuestHeader("Pir_Line_4_Pearl");
			//дача пятого квеста
            Pchar.quest.PQ5_meetInPortRoyal.win_condition.l1 = "location";
            Pchar.quest.PQ5_meetInPortRoyal.win_condition.l1.location = "PortRoyal_town";
            Pchar.quest.PQ5_meetInPortRoyal.function = "PQ5_meetInPortRoyal";
		break;
		case "PL_Q4_goodWork_5":
			dialog.text = "Ты выполнил дело плохо, приятель! Я рассчитывал на совсем другой доход. И если ты проморгал тартаны - это за счет твоей доли. В общем, спорить бесполезно, жемчуг я забираю...";
			link.l1 = "Хм, что же, мне деваться некуда, уступаю... Теперь я могу идти?";
			link.l1.go = "PL_Q4_goodWork_4";
			TakeNItems(pchar, "jewelry12", -sti(pchar.items.jewelry12));
			TakeNItems(pchar, "jewelry11", -sti(pchar.items.jewelry11));
		break;
		//квест №5, ГГ-ОЗГ. базар после дела
		case "PL_Q5_begin":
			dialog.text = "Чего пришел? У меня для тебя, особых заданий нет.";
			link.l1 = "Ты знаешь, я тут прикончил одного типа... Хорошего бойца!";
			link.l1.go = "PL_Q5_begin_1";
		break;
		case "PL_Q5_begin_1":
			dialog.text = "И ты думаешь, что мне, это будет интересно? Ну и пираты нынче пошли. Мда-а... И что за боец?";
			link.l1 = "Да! Хороший боец! Джон Эйвори его звали, он...";
			link.l1.go = "PL_Q5_begin_2";
		break;
		case "PL_Q5_begin_2":
			dialog.text = "Джон Эйвори? И как это произошло? Из-за чего сцепились?";
			link.l1 = "Да мы не сцепились, мне его заказали. Поработал, так сказать, охотником за головами...";
			link.l1.go = "PL_Q5_begin_3";
		break;
		case "PL_Q5_begin_3":
			dialog.text = "Хм... И кто тебе умудрился его заказать?";
			link.l1 = "Капитан Гудли.";
			link.l1.go = "PL_Q5_begin_4";
		break;
		case "PL_Q5_begin_4":
			dialog.text = "Вот как?.. Может, ты не в курсе, конечно, но Джон Эйвери был пиратом, одним из моих доверенных лиц. Ладно, если бы вы схватились на дуэли, такое бывает. Но убить члена Берегового братства за деньги!.. За его смерть я жестоко отомщу.";
			link.l1 = "Дьявол!!! Морган, я не знал!..";
			link.l1.go = "PL_Q5_begin_5";
		break;
		case "PL_Q5_begin_5":
			dialog.text = "Заткнись, говорить будешь, когда я скажу... Так что ты там про Гудли лепетал?";
			link.l1 = "Это Гудли подбил меня на это! Я не знал...";
			link.l1.go = "PL_Q5_begin_6";
		break;
		case "PL_Q5_begin_6":
			dialog.text = "Заткнись... Оливер, прикажи немедленно доставить Гудли сюда! Разберемся на месте, так сказать... А ты пока в сторонке постой, лихой корсар...";
			link.l1 = "Адмирал, я вообще был не в курсе...";
			link.l1.go = "exit";
			NextDiag.TempNode = "PL_Q5_talking";
			AddDialogExitQuestFunction("PQ5_Morgan_1");
		break;

		case "PL_Q5_talking":
			dialog.text = "О-о-о, какие люди! Спасибо, что зашел... Скажи-ка, друг наш Гудли, какой это заказ, ты поручил " + GetMainCharacterNameDat() + "?";
			link.l1 = "Я?!! Ничего такого не было, адмирал. Я ничего ему не поручал...";
			link.l1.go = "PL_Q5_talking_1";
		break;
		case "PL_Q5_talking_1":
			dialog.text = "А вот он утверждает, что ты дал ему заказ за деньги убрать Джона Эйвори. Ты такого не припоминаешь?";
			link.l1 = "Ну я что, совсем больной? Нет, конечно! Что-то он крутит, а меня пытается подставить...";
			link.l1.go = "exit";
			NextDiag.TempNode = "PL_Q5_end";
			AddDialogExitQuestFunction("PQ5_Morgan_4");
		break;

		case "PL_Q5_end":
			dialog.text = "Ну, что скажешь?";
			link.l1 = "Я говорю правду, Генри.";
			link.l1.go = "PL_Q5_end_1";
		break;
		case "PL_Q5_end_1":
			dialog.text = "В общем, все ясно. Пусть вас рассудит Господь Всемогущий. Сейчас между вами будет схватка до смерти. Кто останется жив, тот и прав.";
			link.l1 = "Ну что же, другого пути я не вижу...";
			link.l1.go = "PL_Q5_end_2";
		break;
		case "PL_Q5_end_2":
			dialog.text = "Ну вот и славно. Тогда приступайте...";
			link.l1 = "...";
			link.l1.go = "exit";
			NextDiag.TempNode = "PL_Q5_end_3";
			AddDialogExitQuestFunction("PQ5_Morgan_5");
		break;

		case "PL_Q5_end_3":
			dialog.text = "Сначала нужно закончить дело...";
			link.l1 = "...";
			link.l1.go = "exit";
			NextDiag.TempNode = "PL_Q5_end_3";
		break;

		case "PL_Q5_afterFight":
			dialog.text = "Ну что же, будем считать, что правду говорил ты. Сам Господь сегодня был на твоей стороне.";
			link.l1 = "Да, это так... Но я действительно говорил правду, Генри. Что-то намутил Гудли непонятное...";
			link.l1.go = "PL_Q5_afterFight_1";
		break;
		case "PL_Q5_afterFight_1":
			dialog.text = "Вот что, отправляйся к Джекмену, расскажи ему, что здесь произошло.";
			link.l1 = "Хорошо, адмирал. Отправлюсь к нему прямо сейчас.";
			link.l1.go = "PL_Q5_afterFight_2";
		break;
		case "PL_Q5_afterFight_2":
			dialog.text = "Ну, а этот инцидент считаю исчерпанным...";
			link.l1 = "Я понял, адмирал. Спасибо, что поверил мне...";			
			link.l1.go = "exit";
			NextDiag.TempNode = "I_know_you_good";
			pchar.questTemp.piratesLine = "waiting_Q6";
			SaveCurrentQuestDateParam("questTemp.piratesLine");
			CloseQuestHeader("Pir_Line_5_Hunter");
			//сразу даем квест №6
			SetQuestHeader("Pir_Line_6_Jackman");
			AddQuestRecord("Pir_Line_6_Jackman", "1");
			QuestSetCurrentNode("Jackman", "PL_Q6"); //даем ноду Джекмену
		break;
		//квест №7, охота на Соукинса
		case "PL_Q7_begin":
			dialog.text = "А-а-а, вот и " + pchar.lastname + " пожаловал! Значит, скоро начнутся проблемы...";
			link.l1 = "Да уж... Генри, понимаешь, тут такая история со мной произошла...";
			link.l1.go = "PL_Q7_begin_1";
			CloseQuestHeader("Pir_Line_6_Jackman");
		break;
		case "PL_Q7_begin_1":
			dialog.text = "Если ты хочешь рассказать историю про своего двойника, то она мне уже известна.";
			link.l1 = "Да уж, такие вот вещи творятся... И что ты думаешь по этому поводу? Джекмен меня к тебе отправил.";
			link.l1.go = "PL_Q7_begin_2";
		break;
		case "PL_Q7_begin_2":
			dialog.text = "Дело ясное, что дело темное. В общем, нечто назревает, и весьма кровопролитное нечто.";
			link.l1 = "А мне что делать?";
			link.l1.go = "PL_Q7_begin_3";
		break;
		case "PL_Q7_begin_3":
			dialog.text = "Попробуй узнать кто стоит за твоим раздвоением и прочими несчастьями. Полагаю, это звенья одной цепи...\nЯ тут предпринял кое-какие меры к тому, чтобы прояснить ситуацию. Отправил человека на разведку, так сказать. Ты можешь найти его и узнать, что он нарыл, все одно не с нуля копать.";
			link.l1 = "Ну да, верно. И как зовут этот человека?";
			link.l1.go = "PL_Q7_begin_4";
		break;
		case "PL_Q7_begin_4":
			dialog.text = "Стив Линней, отправился он в Ла Вегу, потом должен был посетить каждое пиратское поселение, а вот посетил или нет, это выяснишь ты.";
			link.l1 = "Понял. Отправляюсь немедленно.";
			link.l1.go = "exit";
            SetQuestHeader("Pir_Line_7_Soukins");
            AddQuestRecord("Pir_Line_7_Soukins", "1");
			NextDiag.TempNode = "I_know_you_good";
			pchar.questTemp.piratesLine = "Soukins_toLaVega";
		break;

		case "PL_Q7_LaVegaLate":
			dialog.text = "О-о, опять наш герой явился. Ну, что узнал?";
			link.l1 = "Ты знаешь, я не нашел Стива Линнея в Ла Веге.";
			link.l1.go = "PL_Q7_LaVegaLate_1";
		break;
		case "PL_Q7_LaVegaLate_1":
			dialog.text = "Я даже не знаю, что тебе и сказать. Что-то ты нерасторопен, как морская черепаха. Стив Линней пропал, и ходят слухи, что его шхуна 'Ласточка' продавалась на верфи Санто-Доминго.";
			link.l1 = "Дьявол!!";
			link.l1.go = "PL_Q7_LaVegaLate_2";
		break;
		case "PL_Q7_LaVegaLate_2":
			dialog.text = "Вот именно. Отправляйся в Санто-Доминго, разузнай там на верфи все относительно 'Ласточки'. И будь пошустрей, что ли...";
			link.l1 = "Хорошо, Генри, все сделаю в лучшем виде.";
			link.l1.go = "exit";
            AddQuestRecord("Pir_Line_7_Soukins", "4");
			NextDiag.TempNode = "I_know_you_good";
			pchar.questTemp.piratesLine = "Soukins_toSantoDomingo";
		break;

		case "PL_Q7_LeonNotFound":
			dialog.text = "О-о, опять наш герой явился. Ну, что узнал?";
			link.l1 = "Знаешь, Генри, Стив Линней пропал без вести.";
			link.l1.go = "PL_Q7_LeonNotFound_1";
		break;
		case "PL_Q7_LeonNotFound_1":
			dialog.text = "Как это?";
			link.l1 = "Его куда-то вызвали двое неизвестных в Ла Веге, с тех пор его никто не видел, а корабль его был продан на верфи Санто-Доминго.";
			link.l1.go = "PL_Q7_LeonNotFound_2";
		break;
		case "PL_Q7_LeonNotFound_2":
			dialog.text = "М-да, видимо, Стива мы уже не увидим... Ну что же, заходи ко мне как-нибудь. Может, подвернется для тебя какая работа.";
			link.l1 = "Хорошо, Генри, зайду...";
			link.l1.go = "exit";
			NextDiag.TempNode = "I_know_you_good";
			CloseQuestHeader("Pir_Line_7_Soukins");
			ChangeCharacterReputation(pchar, -3);
			SaveCurrentQuestDateParam("questTemp.piratesLine");
			pchar.questTemp.piratesLine = "waiting_Q8";
		break;

		case "PL_Q7_afterLeon":
			dialog.text = "А-а, рад тебя видеть. Что нового расскажешь?";
			link.l1 = "К сожалению, Стив Линней мертв. Его убили пираты с фрегата 'Леон', который я, в свою очередь, отправил на дно.";
			link.l1.go = "PL_Q7_afterLeon_1";
		break;
		case "PL_Q7_afterLeon_1":
			dialog.text = "Карамба! Жаль Стива...";
			link.l1 = "Это еще не все, Генри. Кэп этого самого фрегата уговаривал меня работать на адмирала Берегового Братства. Только этот адмирал - не ты...";
			link.l1.go = "PL_Q7_afterLeon_2";
		break;
		case "PL_Q7_afterLeon_2":
			dialog.text = "Ну и кто же это, интересно? Уж не Эдвард ли Мэнсфилд? Мне казалось, что мы со стариком достигли консенсуса...";
			link.l1 = "Нет, Генри. Это Ричард Соукинс, глава Пуэрто-Принсипе.";
			link.l1.go = "PL_Q7_afterLeon_3";
		break;
		case "PL_Q7_afterLeon_3":
			dialog.text = "Ха!.. Хотя, чему удивляться? Капитан пиратской шхуны - и тот каждую минуту находится под угрозой получения черной метки от команды. А уж адмирал Берегового Братства...";
			link.l1 = "Неужели все так печально? Джекмен пытался мне об этом рассказать, что-то о волках втирал... Я так толком ничего и не понял.";
			link.l1.go = "PL_Q7_afterLeon_4";
		break;
		case "PL_Q7_afterLeon_4":
			dialog.text = "Джекмен - известный философ, еще не такую метафору может ввернуть.\nНу да ладно. Мы победим! Всех врагов отправим на дно, кормить акул! Ха! Чую, скоро рыбки нажрутся до отвала! Лучше давай прикинем, что нужно делать, раз Соукинс уже вовсю раскрутил свои дела.";
			link.l1 = "Давай прикинем, Генри.";
			link.l1.go = "PL_Q7_afterLeon_5";
		break;
		case "PL_Q7_afterLeon_5":
			dialog.text = "Значит так. Ходят слухи, что Соукинс планирует акцию. Нельзя допустить, чтобы его затея удалась, иначе он приобретет дополнительный вес. Он удачлив, надо ему подпортить это дело.";
			link.l1 = "Понятно. А что за акция?";
			link.l1.go = "PL_Q7_afterLeon_6";
		break;
		case "PL_Q7_afterLeon_6":
			dialog.text = "Хе, ну как всегда - испанцев пощипать. В общем, нужно сорвать это дело.";
			link.l1 = "А каким образом?";
			link.l1.go = "PL_Q7_afterLeon_7";
		break;
		case "PL_Q7_afterLeon_7":
			dialog.text = "Нужно опередить его. Выясни, что он затеял и сделай это раньше него. Ну, или еще что-нибудь придумай. Действуй по ситуации, в общем. Для меня главное - положительный результат.";
			link.l1 = "Понятно. Ну что же, приступаю немедленно.";
			link.l1.go = "PL_Q7_afterLeon_8";
		break;
		case "PL_Q7_afterLeon_8":
			dialog.text = "Отлично! Жду тебя с хорошими новостями.";
			link.l1 = "Я все сделаю, Генри.";
			link.l1.go = "exit";
            AddQuestRecord("Pir_Line_7_Soukins", "8");
			NextDiag.TempNode = "I_know_you_good";
			pchar.questTemp.piratesLine = "Soukins_toPuertoPrincipe";
			sld = ItemsFromID("OpenBook");
			sld.shown = true;
			sld = ItemsFromID("Ladder");
			sld.shown = true;
			sld.startLocation = "PuertoPrincipe_town";
			sld.startLocator = "item1";
			sld.useLocation = "PuertoPrincipe_town";
			sld.useLocator = "button01";
			//прерывание на вход в резиденцию, не дать осмотреться геймеру
            Pchar.quest.PQ7_inSoukinsResidence.win_condition.l1 = "location";
            Pchar.quest.PQ7_inSoukinsResidence.win_condition.l1.location = "PuertoPrincipe_townhall";
            Pchar.quest.PQ7_inSoukinsResidence.function = "PQ7_inSoukinsResidence";
		break;

		case "PL_Q7_BadWork":
			dialog.text = "Ну, как дела? Сумел помешать Соукинсу?";
			link.l1 = "К сожалению, нет, Генри...";
			link.l1.go = "PL_Q7_BadWork_1";
		break;
		case "PL_Q7_BadWork_1":
			dialog.text = "Плохо. Теперь Соукинс стал сильней...";
			link.l1 = "Я сделал все, что мог.";
			link.l1.go = "exit";
			NextDiag.TempNode = "I_know_you_good";
			CloseQuestHeader("Pir_Line_7_Soukins");
			ChangeCharacterReputation(pchar, -10);
			SaveCurrentQuestDateParam("questTemp.piratesLine");
			pchar.questTemp.piratesLine = "waiting_Q8";
		break;

		case "PL_Q7_GoodWork":
			dialog.text = "Ну, как дела? Сумел помешать Соукинсу?";
			link.l1 = "Да, все сделал. Как и обещал...";
			link.l1.go = "PL_Q7_GoodWork_1";
		break;
		case "PL_Q7_GoodWork_1":
			dialog.text = "Отлично! И что он затевал?";
			link.l1 = "Он откуда-то узнал про испанский линейный корабль, который... Гм, на котором было много чего интересного. По слухам.";
			link.l1.go = "PL_Q7_GoodWork_2";
		break;
		case "PL_Q7_GoodWork_2":
			dialog.text = "И что?";
			link.l1 = "Я потопил испанца!";
			link.l1.go = "PL_Q7_GoodWork_3";
		break;
		case "PL_Q7_GoodWork_3":
			dialog.text = "А абордаж был, трюмы не проверил?";
			link.l1 = "Хм, нет Генри, не удалось...";
			link.l1.go = "PL_Q7_GoodWork_4";
		break;
		case "PL_Q7_GoodWork_4":
			dialog.text = "Черт! Жаль... Я надеюсь, ты не пытаешься меня обмануть?";
			link.l1 = "Ни в коем разе, Генри. Чтоб я сдох!";
			link.l1.go = "PL_Q7_GoodWork_5";
		break;
		case "PL_Q7_GoodWork_5":
			dialog.text = "Ну, это успеется, а пока рановато. Ты мне еще понадобишься. И не вздумай мне лгать, приятель...";
			link.l1 = "Да ни за что на свете, адмирал!";
			link.l1.go = "PL_Q7_GoodWork_6";
		break;
		case "PL_Q7_GoodWork_6":
			dialog.text = "Хорошо, я верю тебе... Ну что же, теперь тебе лучше заняться своими делами, если ты хочешь поработать на какую-нибудь державу - я не буду против. Как закончишь - приходи ко мне, продолжим наше сотрудничество.";
			link.l1 = "А если я не хочу связываться с властями?";
			link.l1.go = "PL_Q7_GoodWork_7";
		break;
		case "PL_Q7_GoodWork_7":
			dialog.text = "Тогда приходи где-то через месяц.";
			link.l1 = "Понятно, адмирал...";
			link.l1.go = "exit";
			NextDiag.TempNode = "I_know_you_good";
			pchar.questTemp.piratesLine = "Soukins_toSantoDomingo";
			CloseQuestHeader("Pir_Line_7_Soukins");
			ChangeCharacterReputation(pchar, 10);
			SaveCurrentQuestDateParam("questTemp.piratesLine");
			pchar.questTemp.piratesLine = "waiting_Q8";
		break;
		//квест №8, штурм Панамы
		case "PL_Q8_1":
			if (pchar.questTemp.State == "" || pchar.questTemp.State == "EndOfQuestLine")
			{
				dialog.text = "Ты как нельзя вовремя! Я уже хотел отправлять на твои поиски человека...";
				link.l1 = "Ну, наконец-то! Что за дело, Генри?";
				link.l1.go = "PL_Q8_2";
				pchar.questTemp.State = "EndOfQuestLine"; //если геймер еще не брал нац.линейку, то и не даем ее после штурма Панамы
				bWorldAlivePause = false; // Конец линейки
			}
			else
			{
				dialog.text = "Хм, дошли до меня слухи, что ты, приятель, связался с " + NationNamePeopleAcc(sti(pchar.questTemp.NationQuest)) + ". Я тебе, кажется, ясно говорил, чтобы ты закончил работу на власти, а потом уже меня дергал. Что тебе непонятно?!";
				link.l1 = "Все ясно, Генри, извини...";
				link.l1.go = "exit";
			}
		break;
		case "PL_Q8_2":
			dialog.text = "Дело? Хм, дело серьезное. Я собираю всех пиратов для предприятия, которое прославит мое имя в веках!";
			link.l1 = "У-у-у, даже страшновато стало!";
			link.l1.go = "PL_Q8_3";
		break;
		case "PL_Q8_3":
			dialog.text = "И правильно! Но в этом деле, друг мой, у тебя будет особая миссия. Пришла пора свести счеты с нашим другом Соукинсом...";
			link.l1 = "Ха!! Давно пора.";
			link.l1.go = "PL_Q8_4";
		break;
		case "PL_Q8_4":
			dialog.text = "Когда приходит 'пора' - это мне решать, уважаемый. Ты знай свое дело - безукоснительно выполняй то, о чем я тебя прошу.";
			link.l1 = "Генри, хоть у нас и было раньше эпизодическое недопонимание, но я всегда действовал из принципа верности адмиралу Берегового Братства!";
			link.l1.go = "PL_Q8_5";
		break;
		case "PL_Q8_5":
			dialog.text = "Ты правильно поступал, приятель, я это ценю... Ну, а сейчас давай я введу тебя в курс дела!";
			link.l1 = "Внимательно тебя слушаю, Генри...";
			link.l1.go = "PL_Q8_6";
		break;
		case "PL_Q8_6":
			dialog.text = "О главном. Я задумал взять Панаму!!";
			link.l1 = "Генри, это сумасшествие! Это невозможно сделать!";
			link.l1.go = "PL_Q8_7";
		break;
		case "PL_Q8_7":
			dialog.text = "Почему?";
			link.l1 = "Там всегда находился самый сильный гарнизон солдат. Не думаю, что с тех пор что-то поменялось...";
			link.l1.go = "PL_Q8_8";
		break;
		case "PL_Q8_8":
			dialog.text = "А тебе не приходило в голову, что самый сильный гарнизон охраняет самое ценное, что есть у Эскориала в Новом свете?";
			link.l1 = "Хм, возможно, это так... Погоди, я слышал, что наш Карл заключил мир с испанской королевой! В Лондоне тебя за это по головке не погладят...";
			link.l1.go = "PL_Q8_9";
		break;
		case "PL_Q8_9":
			dialog.text = "Ты правильно сказал - 'слышал'. И я слышал, ну а где официальное уведомление об этом? Я его не видел. А слухам, знаешь ли, доверять нельзя.";
			link.l1 = "Хм... Да мне-то, собственно, все равно. Отвечать перед британскими властями тебе придется. Но ты думаешь, что мы сможем дойти до Панамы через мыс Горн, сохранив при этом эскадру? Не думаю...";
			link.l1.go = "PL_Q8_10";
		break;
		case "PL_Q8_10":
			dialog.text = "И я не думаю. Поэтому мы пойдем не через мыс Горн, а пешком, через перешеек!";
			link.l1 = "Дьявол! Это же еще большее безумие!!";
			link.l1.go = "PL_Q8_11";
		break;
		case "PL_Q8_11":
			dialog.text = "Точно!! Поэтому испанцы этого от нас точно не ждут. Но запомни мои слова - мы сделаем это!";
			link.l1 = "А, черт! Я с тобой, адмирал!! Когда приступаем?";
			link.l1.go = "PL_Q8_12";
		break;
		case "PL_Q8_12":
			dialog.text = "Я даю тебе 20 дней, чтобы ты смог подготовиться как следует. Подбери корабль покрупней, собери команду побольше. Ты сможешь участвовать в этом деле только на одном корабле, имей это в виду.";
			link.l1 = "Понятно. Что мне еще нужно знать?";
			link.l1.go = "PL_Q8_13";
		break;
		case "PL_Q8_13":
			dialog.text = "Сначала мы возьмем Порто Белло, чтобы не оставлять гарнизон испанцев в тылу. Затем двинемся на Панаму через джунгли. Кстати, Порто Белло атаковать раньше времени бесполезно, поэтому можешь даже и не пытаться.";
			link.l1 = "Да я и не собирался!";
			link.l1.go = "PL_Q8_14";
		break;
		case "PL_Q8_14":
			dialog.text = "Хе-хе, знаю я таких шустрых малых, сам такой... Главное: от тебя, кроме успешных действий в бою, потребуется еще решить наши проблемы... Я скажу, что и как, на месте. Но будь готов в любую минуту.";
			link.l1 = "Генри, ты меня знаешь. Я всегда готов!";
			link.l1.go = "PL_Q8_15";
		break;
		case "PL_Q8_15":
			dialog.text = "Вот это радует, молодец! Настоящий корсар.";
			link.l1 = "Хм, спасибо, адмирал...";
			link.l1.go = "PL_Q8_16";
		break;
		case "PL_Q8_16":
			dialog.text = "В общем, через 20 дней... А сейчас готовься.";
			link.l1 = "Я понял, Генри.";
			link.l1.go = "exit";
            AddQuestRecord("Pir_Line_8_Panama", "1");
			NextDiag.TempNode = "PL_Q8_ready";
			pchar.questTemp.piratesLine = "Panama_toReady";
			SaveCurrentQuestDateParam("questTemp.piratesLine");
			SetTimerFunction("PQ8_timeOver", 0, 0, 35);
			LocatorReloadEnterDisable("Shore48", "reload1_back", true); //закрывем подходы к Панаме
			LocatorReloadEnterDisable("PortoBello_Jungle_01", "reload1_back", true);
		break;

		case "PL_Q8_ready":
			if (GetQuestPastDayParam("questTemp.piratesLine") > 20)
			{
				dialog.text = "Ага, вот и ты пришел! Это хорошо, так как время пришло. Я так понимаю, что ты готов...";
				link.l1 = "Да, адмирал. В полной боевой готовности.";
				link.l1.go = "PL_Q8_ready_1";
			}
			else
			{
				dialog.text = "Зачем явился? Через 20 дней, я же сказал тебе!";
				link.l1 = "Да, я помню. Извини, Генри...";
				link.l1.go = "exit";
			}
		break;
		case "PL_Q8_ready_1":
            if (GetCompanionQuantity(pchar) == 1)
            {
				dialog.text = "Отлично! Итак, мы выступаем. В качестве капитанов в деле еще участвуют Джекмен, Джон Моррис и наш большой друг Ричард Соукинс... На тебя я возлагаю командование эскадрой и штурм Порто Белло.";
				link.l1 = "Спасибо за доверие, адмирал. Я не подведу, будь уверен!";
				link.l1.go = "PL_Q8_ready_2";
			}
			else
			{
				dialog.text = "Послушай, " + pchar.name + ", я же тебе английским языком сказал, чтобы ты приходил с одним кораблем. Тебе нужно делать лишь то, что я говорю. Что тут непонятного?!";
				link.l1 = "Все ясно, адмирал, все сделаю.";
				link.l1.go = "exit";
				NextDiag.TempNode = "PL_Q8_ready";
			}
		break;
		case "PL_Q8_ready_2":
			dialog.text = "Очень на это рассчитываю, приятель. Иначе мне придется тебя убить...";
			link.l1 = "Надеюсь, до этого не дойдет, Генри. Итак, я готов приступить к командованию.";
			link.l1.go = "PL_Q8_ready_3";
		break;
		case "PL_Q8_ready_3":
			dialog.text = "Хорошо. Тогда начинаем немедленно.";
			link.l1 = "Я готов.";
			link.l1.go = "PL_Q8_ready_4";
		break;
		case "PL_Q8_ready_4":
			pchar.quest.PQ8_timeOver.over = "yes"; //снимаем таймер 
			SetTimerFunction("PQ8_PanamaTimerOver", 0, 0, 20); //ставим новый таймер
			pchar.Quest.PQ8_EnterPortoBello.win_condition.l1 = "location";
			pchar.Quest.PQ8_EnterPortoBello.win_condition.l1.location = "PortoBello";
			pchar.Quest.PQ8_EnterPortoBello.function = "PQ8_EnterPortoBello";
            pchar.questTemp.piratesLine = "Panama_toPortoBello";
			AddQuestRecord("Pir_Line_8_Panama", "4");
			pchar.TempPerks.QuestTroopers = true; //перк квестового десанта
			characters[GetCharacterIndex("PortoBello_Mayor")].dialog.captureNode = "PQ8_MayorPortoBello"; //капитулянтская нода мэра
			//Морган
			FantomMakeCoolSailor(npchar, SHIP_BATTLESHIP, "Ужасающий", CANNON_TYPE_CANNON_LBS32, 90, 90, 90);
            pchar.nation = PIRATE;
            LAi_SetActorType(NPChar);
            LAi_ActorSetStayMode(NPChar);
			LAi_SetImmortal(NPChar, false);
			NPChar.Ship.Cargo.Goods.Food = 1000;
            LAi_NoRebirthDisable(NPChar);
			LAi_RebirthOldName(NPChar); 
			npchar.DeckDialogNode = "PL_Q8_deck"; //ноду на палубу
            ChangeCharacterAddressGroup(NPChar, "PortRoyal_houseS1", "goto", "goto7");
            CharacterIntoCompanionAndGoOut(pchar, npchar, "reload", "reload1", 8.5, false);
            npchar.CompanionEnemyEnable = false;  // нет отпора при обстреле
			SetCharacterRemovable(npchar, false);
			//Джекмен
			sld = characterFromId("Jackman");
			FantomMakeCoolSailor(sld, SHIP_WARSHIP, "Фортунато", CANNON_TYPE_CULVERINE_LBS32, 90, 90, 90);
			sld.CompanionEnemyEnable = false;  // нет отпора при обстреле
			SetCharacterRemovable(sld, false);
			sld.Ship.Cargo.Goods.Food = 1000;
			LAi_SetImmortal(sld, false);
            LAi_NoRebirthDisable(sld);
			LAi_RebirthOldName(sld);
			SetCompanionIndex(pchar, -1, sti(sld.index));
			//Джон Моррис
			if (GetCharacterIndex("John Morris") == -1)
			{
				sld = GetCharacter(NPC_GenerateCharacter("John Morris", "bocman", "man", "man", 38, PIRATE, -1, false));
        		sld.name 	= "Джон";
        		sld.lastname = "Моррис";
				sld.Dialog.Filename = "Quest\JohnMorris.c";
				FantomMakeCoolFighter(sld, 35, 90, 50, BLADE_LONG, "pistol4", 150);
           		LAi_SetStayType(sld);
        		LAi_SetLoginTime(sld, 0.0, 24.0);
        		LAi_SetHP(sld, 180.0, 180.0);
			}
			else
			{
				sld = characterFromId("John Morris");
			}
			FantomMakeCoolSailor(sld, SHIP_FRIGATE, "Лагуна", CANNON_TYPE_CANNON_LBS32, 90, 90, 90);	
			sld.CompanionEnemyEnable = false;  // нет отпора при обстреле
			LAi_SetImmortal(sld, false);
			SetCharacterRemovable(sld, false);
			sld.Ship.Cargo.Goods.Food = 1000;
            LAi_NoRebirthDisable(sld);
			LAi_RebirthOldName(sld);
			sld.RebirthPhantom = true; //не тереть фантома после смерти.  
			sld.DeckDialogNode = "PL_Q8_deck"; //ноду на палубу
			SetCompanionIndex(pchar, -1, sti(sld.index));
			//Ричард Соукинс
			sld = characterFromId("Richard_Soukins");
			FantomMakeCoolSailor(sld, SHIP_MANOWAR, "Счастливчик", CANNON_TYPE_CULVERINE_LBS32, 90, 90, 90);
			sld.CompanionEnemyEnable = false;  // нет отпора при обстреле
			SetCharacterRemovable(sld, false);
			sld.Ship.Cargo.Goods.Food = 2500;
			LAi_SetImmortal(sld, true);
			LAi_NoRebirthEnable(sld);
			LAi_RemoveLoginTime(sld);
			sld.DeckDialogNode = "PL_Q8_deck"; //ноду на палубу
			SetCompanionIndex(pchar, -1, sti(sld.index));
            NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;

		case "PL_Q8_timeOver":
			dialog.text = "Я доверил тебе управление эскадрой, а ты непонятно чем занимаешься. Мы уже дней двадцать ходим по Карибам, когда от Порт Рояля до Порто Белло от силы десять дней хода!";
			link.l1 = "Так получилось, Генри...";
			link.l1.go = "PL_Q8_timeOver_1";
			// выход с палубы
			pchar.quest.Munity = "Deads";
            LAi_LockFightMode(Pchar, false);
		break;
		case "PL_Q8_timeOver_1":
			dialog.text = "Пусть у тебя 'так получается' где-нибудь в другом месте. Надеюсь, тебе быстро снесут голову испанцы с таким подходом к работе. В общем, мы уходим, как-нибудь обойдемся без тебя в нашем деле... Прощай.";
			link.l1 = "Адмирал, а может?..";
			link.l1.go = "PL_Q8_timeOver_2";
		break;
		case "PL_Q8_timeOver_2":
			dialog.text = "До свидания, приятель. Не выводи меня из себя.";
			link.l1 = "Ну ладно, Генри. И прошу прощения за задержку...";
			link.l1.go = "PL_Q8_timeOver_3";
		break;
		case "PL_Q8_timeOver_3":
			SetTimerFunction("PQ8_openPanama", 0, 0, 40);
			LocatorReloadEnterDisable("Shore48", "reload1_back", true); //закрывем подходы к Панаме
			LocatorReloadEnterDisable("PortoBello_Jungle_01", "reload1_back", true);
			pchar.questTemp.piratesLine = "over";
			AddQuestRecord("Pir_Line_8_Panama", "3");
			CloseQuestHeader("Pir_Line_8_Panama");
			//возвращаем Моргана
            if(IsCompanion(npchar))  RemoveCharacterCompanion(pchar, npchar);
            //Возвращаем на место Джекмена
			iTemp = GetCharacterIndex("Jackman");
			if (iTemp != -1)
			{
				if(IsCompanion(&characters[iTemp]))  
				{
					RemoveCharacterCompanion(pchar, &characters[iTemp]);
				}               
				LAi_SetHuberType(&characters[iTemp]);
				LAi_group_MoveCharacter(&characters[iTemp], "PIRATE_CITIZENS");
                ChangeCharacterAddressGroup(&characters[iTemp], "Pirates_townhall", "sit", "sit1");
                characters[iTemp].dialog.currentnode = "First time";
            }
			//Джон Моррис
			iTemp = GetCharacterIndex("John Morris");
			if (iTemp != -1)
			{
				if(IsCompanion(&characters[iTemp]))  
				{
					RemoveCharacterCompanion(pchar, &characters[iTemp]);
				}   				
				sld = &characters[iTemp];
				sld.lifeDay = 0;
			}
            //Возвращаем на место Соукинса
			iTemp = GetCharacterIndex("Richard_Soukins");
			if (iTemp != -1)
			{
				if(IsCompanion(&characters[iTemp]))  
				{
					RemoveCharacterCompanion(pchar, &characters[iTemp]);
				}                   
				LAi_SetHuberType(&characters[iTemp]);
				LAi_group_MoveCharacter(&characters[iTemp], "PIRATE_CITIZENS");
                ChangeCharacterAddressGroup(&characters[iTemp], "PuertoPrincipe_townhall", "sit", "sit1");
                characters[iTemp].dialog.currentnode = "First time";
            }
            NextDiag.TempNode = "PL_Q8_timeOver_exit";
            NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
		case "PL_Q8_timeOver_exit":
			dialog.text = "Не о чем больше говорить.";
			link.l1 = "...";
			link.l1.go = "PL_Q8_timeOver_exit";
		break;
		//в резиденции взятого Порто Белло
		case "PL_Q8_PBResidence":
			dialog.text = "Ты хорошо провел штурм, хвалю.";
			link.l1 = "Порто Белло наш, адмирал. Но у нас есть серьезная проблема.";
			link.l1.go = "PL_Q8_PBResidence_1";
		break;
		case "PL_Q8_PBResidence_1":
			dialog.text = "В чем дело?";
			link.l1 = "Все дело в том, что испанцам, похоже, известно о наших намерениях. Это рассказал мне пленный губернатор.";
			link.l1.go = "PL_Q8_PBResidence_2";
		break;
		case "PL_Q8_PBResidence_2":
			dialog.text = "Дьявол!! А кто ему сказал об этом?";
			link.l1 = "По его словам, испанский генерал-губернатор.";
			link.l1.go = "PL_Q8_PBResidence_3";
		break;
		case "PL_Q8_PBResidence_3":
			dialog.text = "М-да, неважные дела... Сдается мне, что это опять происки Ричарда! Скорее всего, он и сдал наши планы испанцам. Теперь они будут готовы к нашему приходу под стены Панамы.";
			link.l1 = "Да уж, поход на Панаму и раньше казался предприятием рискованным, ну а теперь это вообще самоубийство.";
			link.l1.go = "PL_Q8_PBResidence_4";
		break;
		case "PL_Q8_PBResidence_4":
			dialog.text = "Да-да, может быть... Вот что, " + pchar.name + ", нам пора избавляться от Соукинса.";
			link.l1 = "Давно пора!";
			link.l1.go = "PL_Q8_PBResidence_5";
		break;
		case "PL_Q8_PBResidence_5":
			dialog.text = "Но тут есть одна проблема - его люди. Они мне нужны. Если с ним что-то случиться, то они вполне могут нас покинуть. А они мне крайне необходимы для штурма Панамы!";
			link.l1 = "Генри, так ты не собираешься отказываться от этого дела?!";
			link.l1.go = "PL_Q8_PBResidence_6";
		break;
		case "PL_Q8_PBResidence_6":
			dialog.text = "Конечно, нет!! Когда это я пасовал перед кастильскими донами? Ты не волнуйся, главное - делай то, что я тебе говорю. И все будет хорошо.";
			link.l1 = "Ты сумасшедший, Генри... И мне это по нраву! Что я должен делать?";
			link.l1.go = "PL_Q8_PBResidence_7";
		break;
		case "PL_Q8_PBResidence_7":
			if (pchar.location.from_sea == "Shore48")
			{
				dialog.text = "Ты высадился в Дарьенском заливе, и это правильно. Следуй туда, собери своих людей и двигай к Панаме. Я буду идти к цели со своими бойцами отсюда, из Порто Белло. Мы встретимся как раз у ворот Панамы и разом начнем штурм с двух направлений.";
				link.l1 = "Я понял, адмирал!";
				link.l1.go = "PL_Q8_PBResidence_8";
			}
			else
			{
				dialog.text = "Отправляйся в Дарьенский залив. Собери своих людей и двигай к Панаме. Я буду идти к цели со своими бойцами отсюда, из Порто Белло. Мы встретимся как раз у ворот Панамы и разом начнем штурм с двух направлений.";
				link.l1 = "Я понял, адмирал!";
				link.l1.go = "PL_Q8_PBResidence_8";
			}
		break;
		case "PL_Q8_PBResidence_8":
			dialog.text = "Это еще не все. Джекмен и Моррис пойдут со мной. Ты пойдешь с Соукинсом. Испанцы знают на нашем походе, и наверняка эти тупые доны попытаются устроить нам засаду в джунглях. А скорее всего, и не одну.";
			link.l1 = "Думаю, что это так, адмирал. Мы будем готовы...";
			link.l1.go = "PL_Q8_PBResidence_9";
		break;
		case "PL_Q8_PBResidence_9":
			dialog.text = "Тебе нужно не просто отбиться от кастильцев. Во время схватки реши и проблему Соукинса, я не хочу видеть его у стен Панамы. Этот Ричард чертовски храбр и сумеет в очередной раз отличиться. А героя так просто не удавишь, популярность - великая вещь и в Береговом Братстве.";
			link.l1 = "Я что-то не очень понял, Генри. Так как мне решить эту проблему?";
			link.l1.go = "PL_Q8_PBResidence_10";
		break;
		case "PL_Q8_PBResidence_10":
			dialog.text = "Очень просто. По-тихому прикончи Соукинса в бою. Но так, чтобы никто не видел, конечно. Выстрел в спину - самое верное средство.";
			link.l1 = "Хм, это не совсем честно...";
			link.l1.go = "PL_Q8_PBResidence_11";
		break;
		case "PL_Q8_PBResidence_11":
			dialog.text = "А то, что он вытворял с тобой, честно?";
			link.l1 = "Нет, конечно...";
			link.l1.go = "PL_Q8_PBResidence_12";
		break;
		case "PL_Q8_PBResidence_12":
			dialog.text = "Вот и делай, как я тебе говорю. И помни о том, что Соукинс не должен появиться на штурме Панамы.";
			link.l1 = "Я понял, адмирал, все сделаю.";
			link.l1.go = "PL_Q8_PBResidence_13";
		break;
		case "PL_Q8_PBResidence_13":
			dialog.text = "Ну, вот и отлично! В общем, бери своих людей и двигай к указанному месту. Не более, чем через два дня ты должен быть там.";
			link.l1 = "Уже бегу, Генри.";
			link.l1.go = "PL_Q8_PBResidence_14";		
		break;
		case "PL_Q8_PBResidence_14":
			LAi_SetWarriorTypeNoGroup(npchar);
			chrDisableReloadToLocation = false; 
			//bQuestDisableMapEnter = false;
			LocatorReloadEnterDisable("Shore48", "reload1_back", false); //открываем проход в джанглы через бухту
            Pchar.quest.PQ8_PBExitResidence.win_condition.l1 = "ExitFromLocation";
            Pchar.quest.PQ8_PBExitResidence.win_condition.l1.location = pchar.location;
            Pchar.quest.PQ8_PBExitResidence.function = "PQ8_PBExitResidence";
			NextDiag.TempNode = "PL_Q8_PBResidence_over";
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
		case "PL_Q8_PBResidence_over":
			dialog.text = "Не задерживайся, вперед к Панаме!";
			link.l1 = "Да, я понял, Генри.";
			link.l1.go = "exit";
			NextDiag.TempNode = "PL_Q8_PBResidence_over";
		break;
		//у ворот Панамы
		case "PL_Q8_ExTPanama":
			if (pchar.questTemp.piratesLine == "Panama_RichardBadWork")
			{
				iTemp = GetCharacterIndex("Richard_Soukins");				
				if (!LAi_IsDead(&characters[iTemp]))
				{
					dialog.text = "Я видел Соукинса среди сражающихся в последнем бою. Похоже, что ты оплошал в джунглях...";
					link.l1 = "Да, но ведь сейчас он мертв! А значит, дело сделано - Ричарду можно готовить надгробный камень.";
					link.l1.go = "PL_Q8_ExTPanama_1";
				}
				else
				{
					dialog.text = "Итак, ты не смог выполнить задание! Наш друг разгуливает здесь как ни в чем ни бывало.";
					link.l1 = "Генри, я не смог ничего сделать, это было очень опасно!";
					link.l1.go = "PL_Q8_ExTPanama_7";
				}
			}
			else
			{
				dialog.text = "Хе-хе, рад тебя видеть, " + pchar.name + ", живым и здоровым! Я что-то не видел Соукинса в этом бою...";
				link.l1 = "Генри, с прискорбием должен тебе сообщить, что Ричарда Соукинса уже нет среди живых. Он пал как герой!..";
				link.l1.go = "PL_Q8_ExTPanama_1";
			}
		break;
		case "PL_Q8_ExTPanama_1":
			dialog.text = "Хо-хо-хо, мы будем помнить его вечно... Ну, рассказывай, что было в переходе?";
			link.l1 = "До этого боя у городских стен Панамы нам пришлось выдержать еще три атаки. Даже индейцы нас атаковали!";
			link.l1.go = "PL_Q8_ExTPanama_2";
		break;
		case "PL_Q8_ExTPanama_2":
			dialog.text = "У нас было то же самое, и это прекрасно, дружище!";
			link.l1 = "Что ты имеешь в виду? Мы понесли серьезные потери...";
			link.l1.go = "PL_Q8_ExTPanama_3";
		break;
		case "PL_Q8_ExTPanama_3":
			dialog.text = "Хм, ну а как без потерь? Это война... Но все дело в том, что испанцы серьезно оплошали тактически. Засадами они рассеяли свои силы, таким образом, нам удалось уничтожить их по частям! Если бы они собрали все силы в кулак, то, несомненно, разбили бы нас. Ну, а сейчас сам город уже некому защищать. Я уверен, что Панама наша!";
			link.l1 = "Ты думаешь, что в городе нет солдат?";
			link.l1.go = "PL_Q8_ExTPanama_4";
		break;
		case "PL_Q8_ExTPanama_4":
			dialog.text = "Уверен в этом!.. Теперь слушай, что ты должен сделать. Сейчас же двигайся в резиденцию губернатора и начинай его допрос. У меня плохое предчувствие...";
			link.l1 = "Что ты имеешь в виду, Генри?";
			link.l1.go = "PL_Q8_ExTPanama_5";
		break;
		case "PL_Q8_ExTPanama_5":
			dialog.text = "Уже очевидно, что мэр Порто Белло сказал правду - нас ждали. А значит, население города ушло из него вместе со всем ценным скарбом. В этом случае выручка от данного предприятия будет ничтожно мала. Но у нас есть шанс взять золото Эскориала, а для этого нам нужен губернатор. Если он здесь, то и золото здесь же, в городе!";
			link.l1 = "Понятно. Да, адмирал, ты - голова...";
			link.l1.go = "PL_Q8_ExTPanama_6";
		break;
		case "PL_Q8_ExTPanama_6":
			dialog.text = "Выдвигайся в резиденцию Панамы, и не задерживайся! А я займусь другими делами в городе...";
			link.l1 = "Я понял, Генри, выполняю.";
			link.l1.go = "PL_Q8_ExTPanama_7";
		break;
		case "PL_Q8_ExTPanama_7":
			AddQuestRecord("Pir_Line_8_Panama", "9");
			//characters[GetCharacterIndex("Panama_Mayor")].dialog.captureNode = "PQ8_afterAttack"; //капитулянтская нода мэра
			pchar.quest.PQ8_piratesInPanama.win_condition.l1 = "location";
			pchar.quest.PQ8_piratesInPanama.win_condition.l1.location = "Panama_town";
			pchar.quest.PQ8_piratesInPanama.function = "PQ8_piratesInPanama";
			NextDiag.TempNode = "PL_Q8_ExTPanama_over";
			LAi_SetActorType(NPChar);
			LAi_ActorRunToLocation(NPChar, "reload", "reload4", "none", "", "", "OpenTheDoors", -1);
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
		case "PL_Q8_ExTPanama_over":
			dialog.text = "Двигай в резиденцию, не теряй времени!";
			link.l1 = "Да, Генри, я понял...";
			link.l1.go = "exit";
		break;
		case "PL_Q8_ExTPanama_7":
			dialog.text = "Ну еще бы!";
			link.l1 = "Генри, перед ним всегда находились его бойцы, я не смог улучить момента.";
			link.l1.go = "PL_Q8_ExTPanama_8";
		break;
		case "PL_Q8_ExTPanama_8":
			dialog.text = "Это я понял!.. Итак, на этом и остановимся. Ты можешь возвращаться к своему кораблю, в Панаму тебе хода нет. За то, что ты подвел меня, исключаю тебя из Берегового Братства. Пошел вон...";
			link.l1 = "Но, адмирал!..";
			link.l1.go = "PL_Q8_ExTPanama_9";
		break;
		case "PL_Q8_ExTPanama_9":
			QuestSetCurrentNode("Richard_Soukins", "PQ8_exitTown");
			AddQuestRecord("Pir_Line_8_Panama", "10");
			CloseQuestHeader("Pir_Line_8_Panama");
			LocatorReloadEnterDisable("Panama_ExitTown", "reload4", true); //закроем вход в город
			SetTimerFunction("PQ8_openPanama_2", 0, 0, 30); //таймер на открыть все потом.
			LAi_SetActorType(NPChar);
			LAi_ActorRunToLocation(NPChar, "reload", "reload4", "none", "", "", "OpenTheDoors", -1);
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
		//в Панаме после выхода ГГ из резиденции
		case "PL_Q8_Panama1":
			dialog.text = "Ты нашел губернатора?";
			link.l1 = "Да, адмирал, нашел и допросил.";
			link.l1.go = "PL_Q8_Panama1_1";
		break;
		case "PL_Q8_Panama1_1":
			dialog.text = "И что он сказал?";
			link.l1 = "Золото Эскориала, возможно, находится в кабинете губернатора в резиденции. Только вот ключа к сундуку нет, он закрыт.";
			link.l1.go = "PL_Q8_Panama1_2";
		break;
		case "PL_Q8_Panama1_2":
			dialog.text = "И где этот ключ?";
			link.l1 = "У коменданта. Мне стоит как следует осмотреть поле последнего боя, может, найду там коменданта. Если нет, то придется искать в форте.";
			link.l1.go = "PL_Q8_Panama1_3";
		break;
		case "PL_Q8_Panama1_3":
			dialog.text = "Хм, ну что же, отправляйся. А я пока займусь губернатором, что-то его рассказ подозрителен. Сундук находится у губернатора, а ключ - у коменданта! Хех, странно...";
			link.l1 = "Может быть, Генри. Ну, я побежал!";
			link.l1.go = "PL_Q8_Panama1_4";
		break;
		case "PL_Q8_Panama1_4":
			sld = characterFromId("FantomMayor");
			sld.lifeDay = 0;
			pchar.quest.PL_Q8_ResEnterBox.win_condition.l1 = "locator";
			pchar.quest.PL_Q8_ResEnterBox.win_condition.l1.location = "Panama_TownhallRoom";
			pchar.quest.PL_Q8_ResEnterBox.win_condition.l1.locator_group = "box";
			pchar.quest.PL_Q8_ResEnterBox.win_condition.l1.locator = "private1";
			pchar.quest.PL_Q8_ResEnterBox.function = "PL_Q8_ResEnterBox";
			LAi_SetActorType(NPChar);
			LAi_ActorRunToLocation(NPChar, "reload", "reload3", "none", "", "", "OpenTheDoors", -1);
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
		//в резиденции Панамы с ключом
		case "PL_Q8_Panama2":
			npchar.quest.locInd = FindLocation(pchar.location);
			sld = &locations[sti(npchar.quest.locInd)];
			if (CheckAttribute(sld, "private1.money")) sld.private1.money = 0;
			if (CheckAttribute(sld, "private2.money")) sld.private2.money = 0;	
			dialog.text = "Я вижу, ты справился с задачей. Нашел все-таки ключ!";
			link.l1 = "Да, Генри, я нашел его! А где губернатор?";
			link.l1.go = "PL_Q8_Panama2_1";
		break;
		case "PL_Q8_Panama2_1":
			dialog.text = "Я отправил его на тот свет. Мне показалось, что он не все тебе сказал, и я допросил его с пристрастием. Здоровье у него оказалось неважное...";
			link.l1 = "Хм... и что нового удалось узнать?";
			link.l1.go = "PL_Q8_Panama2_2";
		break;
		case "PL_Q8_Panama2_2":
			dialog.text = "Он знал, сколько золота в закрытом сундуке. Там 25 миллионов пиастров, приятель! Да ты и без меня уже это знаешь, давай, выкладывай добычу!";
			link.l1 = "Вот эти деньги, Морган... Слушай, а когда делить будем?";
			link.l1.go = "PL_Q8_Panama2_3";
			sld = &locations[sti(npchar.quest.locInd)];
			if (CheckAttribute(sld, "private1.items.gold")) DeleteAttribute(sld, "private1.items.gold");
			else AddMoneyToCharacter(pchar, -50000000);
		break;
		case "PL_Q8_Panama2_3":
			dialog.text = "Когда соберем все и со всех. Наши бравые пираты много чего припрятали у себя в карманах. Сегодня я заставлю сдать все в общий котел, а потом поделим все по законам Берегового Братства.";
			link.l1 = "Это справедливо, адмирал!";
			link.l1.go = "PL_Q8_Panama2_4";
		break;
		case "PL_Q8_Panama2_4":
			dialog.text = "Ты свою добычу уже сдал, так что я претензий к тебе не имею - можешь быть свободен от планируемого мной мероприятия. Но для тебя на это время есть работа\n"+
				"Отправляйся опять в форт Панамы. Там за фортом, с внешней стороны стены есть сундук. В нем должно быть еще золото Эскориала! Это рассказал покойный губернатор Панамы...";
			link.l1 = "Черт, не может быть! С внешней стороны?!";
			link.l1.go = "PL_Q8_Panama2_5";
		break;
		case "PL_Q8_Panama2_5":
			dialog.text = "Именно так, сундук находится прямо на отвесной скале. Я жду тебя с этим золотом, так что не мешкай...";
			link.l1 = "Хорошо, адмирал, как скажешь...";
			link.l1.go = "PL_Q8_Panama2_6";
		break;
		case "PL_Q8_Panama2_6":
			AddQuestRecord("Pir_Line_8_Panama", "12");
			Pchar.quest.PL_Q8_inPanamaFort2.win_condition.l1 = "location";
			Pchar.quest.PL_Q8_inPanamaFort2.win_condition.l1.location = "Panama_Fort";
			Pchar.quest.PL_Q8_inPanamaFort2.function = "PL_Q8_inPanamaFort2";
			LAi_SetActorType(NPChar);
			LAi_ActorRunToLocation(NPChar, "reload", "reload2", "none", "", "", "OpenTheDoors", -1);
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
		//Морган в Порт Рояле
		case "PL_Q8_afterPanama":
			dialog.text = "Ого, вот так встреча! " + pchar.name + ", ты ли это?!";
			link.l1 = "Я, собственной персоной, адмирал. Вот, хочу взглянуть тебе в глаза...";
			link.l1.go = "PL_Q8_afterPanama_1";
		break;
		case "PL_Q8_afterPanama_1":
			dialog.text = "Ты, наверное, обижен на меня. Ну, извини, что пришлось оставить тебя в Панаме - очень торопился, знаешь ли.";
			link.l1 = "Послушай, Морган, ты же затащил в эту чертову Панаму все Береговое Братство. Ты его уничтожил, по сути!";
			link.l1.go = "PL_Q8_afterPanama_2";
		break;
		case "PL_Q8_afterPanama_2":
			dialog.text = "Можно и так сказать. Знаешь, времена меняются...";
			link.l1 = "Какие, к черту, времена?!!";
			link.l1.go = "PL_Q8_afterPanama_3";
		break;
		case "PL_Q8_afterPanama_3":
			dialog.text = "Обыкновенные, приятель. Ты что, надеешься до конца жизни грабить корабли в море или штурмовать города? Пиратство потому и сумело так развиться, что шла война между великими державами. Как только установится прочный мир, мы сразу станем лишними\n"+
				"И это время уже наступило! Вот ты хочешь быть лишним?";
			link.l1 = "Как это понять?";
			link.l1.go = "PL_Q8_afterPanama_4";
		break;
		case "PL_Q8_afterPanama_4":
			dialog.text = "Да очень просто. Ты и шагу ступить не сможешь без риска лишиться головы. Мне удалось в суде оправдаться за взятие Панамы, но знаешь, почему так случилось?";
			link.l1 = "Хороший адвокат?";
			link.l1.go = "PL_Q8_afterPanama_5";
		break;
		case "PL_Q8_afterPanama_5":
			dialog.text = "Куда там! Все дело в том, что я оказал огромную услугу английской короне - здорово сократил количество пиратов в Карибском море, да еще и принес в английскую казну такие деньги!";
			link.l1 = "Так ты что, отвез все это в Лондон?!!";
			link.l1.go = "PL_Q8_afterPanama_6";
		break;
		case "PL_Q8_afterPanama_6":
			dialog.text = "Да, приятель. И это был единственный шанс существовать далее. Так что я заплатил и за себя, и за тебя, и за всех пиратов, кто хочет жить обычной жизнью. И вообще, я теперь плантатор, а не адмирал Берегового Братства.";
			link.l1 = "Так что, адмиральское место вакантно?";
			link.l1.go = "PL_Q8_afterPanama_7";
		break;
		case "PL_Q8_afterPanama_7":
			dialog.text = "Нет, ты не понял. Берегового Братства больше нет, значит, нет и его адмирала. Я привез прощение английского престола всем пиратам! А тех, кто не захочет подчиниться - буду карать. Так что сам видишь, как изменились времена...";
			link.l1 = "Это точно! Изменения просто глобальные, все поставлено с ног на голову...";
			link.l1.go = "PL_Q8_afterPanama_8";
		break;
		case "PL_Q8_afterPanama_8":
			dialog.text = "С головы на ноги, приятель... В общем, спасибо, что зашел, рад был тебя встретить живым и здоровым.";
			link.l1 = "Генри, я не собираюсь бросать каперство!";
			link.l1.go = "PL_Q8_afterPanama_9";
		break;
		case "PL_Q8_afterPanama_9":
			dialog.text = "Ну как знаешь. Но учти, что ты играешь с огнем!";
			link.l1 = "Учту. Прощай, Генри.";
			link.l1.go = "exit";
			NextDiag.TempNode = "I_know_you_good";
			pchar.questTemp.piratesLine = "over";
            AddQuestRecord("Pir_Line_8_Panama", "14");
            CloseQuestHeader("Pir_Line_8_Panama");
		break;

// ======================== блок нод angry ===============>>>>>>>>>>>>>>>
		case "AngryRepeat_1":
            dialog.text = RandPhraseSimple("Пошел вон отсюда!", "Вон из моего кабинета!");
			link.l1 = "Ай...";
		    link.l1.go = "AngryExitAgainWithOut";
            if (CheckAttribute(npchar, "angry.terms")) //примиряемся с Морганом через 10 дней.
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
            if (CheckAttribute(npchar, "angry.terms")) //примиряемся с Морганом через 20 дней.
            {
                if (GetNpcQuestPastDayParam(npchar, "angry.terms") > sti(npchar.angry.terms))
                {
         			dialog.text = "Надеюсь, впредь ты не будешь утомлять меня пустыми разговорами, иначе мне придется тебя убить. Признаюсь, мне это будет очень неприятно.";
        			link.l1 = "Не буду, Морган.";
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
			if (Pchar.location == "SentJons_HouseF3")
			{
				DoReloadCharacterToLocation("SentJons_town","reload","houseF3");	
			}
			else
			{
				DoReloadCharacterToLocation("PortRoyal_town","reload","houseS1");	
			}
		break;
// <<<<<<<<<<<<============= блок нод angry =============================
	}
}

