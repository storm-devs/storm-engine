// диалоговый файл №1 на голландку
void ProcessDialogEvent()
{
	ref NPChar, sld;
	aref Link, NextDiag;

	DeleteAttribute(&Dialog,"Links");

	makeref(NPChar,CharacterRef);
	makearef(Link, Dialog.Links);
	makearef(NextDiag, NPChar.Dialog);
	float locx, locy, locz;
	
    switch (Dialog.CurrentNode)
    {
        case "First time":
            dialog.text = "Если вы это видите, то это бага. Надо срочно сообщить Эдди!";
            link.l1 = "...";
            link.l1.go = "exit";

//>>>>>>>>>>>>========= Разброс диалогов по персонажам =====================
            if (npchar.id == "Bandit2" && pchar.questTemp.State == "SeekChumakeiro_ToAaronHouse")// перехват на бандитов, грябящих Аарона
            {
    			dialog.text = "Проваливай, пока цел...";
    			link.l1 = "Меня зовут "+ GetFullName(pchar) +". Я ищу Аарона Мендеса Чумакейро.";
    			link.l1.go = "Step_H1_1";
            }// Аарон собственной персоной, после драки с грабителями
            if (npchar.id == "Chumakeiro" && pchar.questTemp.State == "SeekChumakeiro_ToAaronHouse")
            {
    			dialog.text = "Кажется, вы спасли мою шкуру! Кто вы и что здесь делаете?";
    			link.l1 = "Меня зовут "+ GetFullName(pchar) +". Я ищу Аарона Мендеса Чумакейро.";
    			link.l1.go = "Step_H1_3";
            }
            if (npchar.id == "Chumakeiro" && pchar.questTemp.State == "SeekChumakeiro_GoToVillemstad")
            {
    			dialog.text = "Я благодарен вам за удачно завершившееся путешествие. И благодарен вдвойне за то, что вы так вовремя появились в Сен-Мартине. Я надеюсь, мы встретимся еще.";
    			link.l1 = "С удовольствием.";
    			link.l1.go = "Step_H1_6";
            }// Квест №3, лейтенант в тюрьме.
            if (npchar.id == "Alan_MacLine" && pchar.questTemp.State == "WormEnglPlans_SaveOfficer")
            {
    			dialog.text = "Хм, я вижу вас впервые... Кто вы?";
    			link.l1 = "Я пришел освободить вас!";
    			link.l1.go = "Step_H3_1";
            } // Квест №5, базар в Ла Веге о гибели Мэнсфилда
            if (npchar.id == "Talker" && pchar.questTemp.State == "ThreeFleutes_toLaVega")
            {
    			dialog.text = "Ох, в невеселое время ты появился у нас... Мэнсфилд убит.";
    			link.l1 = "Как убит?!";
    			link.l1.go = "Step_H5_1";
            } // Квест №7, незнакомец на Тортуге
            if (npchar.id == "Stranger_HL7" && pchar.questTemp.State == "DelivLettTortuga_SnapMeeting")
            {
    			dialog.text = "Скажите, вы не кто иной, как вестовой генерал-губернатора Кюрасао?";
    			link.l1 = "А что вы хотели?";
    			link.l1.go = "Step_H7_1";
            } // Квест №7, Антонио в комнате таверны
            if (npchar.id == "AntonioDeBarras" && pchar.questTemp.State == "DelivLettTortuga_toStranger")
            {
    			dialog.text = "А, вот и ты! Я уже заждался...";
    			link.l1 = "Вы хотели меня видеть? Кто вы?";
    			link.l1.go = "Step_H7_4";
            }
            if (pchar.questTemp.State == "DelivLettTortuga_NotFoundLetter")
            {
    			dialog.text = "Проклятье! Не думал снова увидеть тебя, да еще так быстро...";
    			link.l1 = "Здраствуй, Антонио. Давай продолжим нашу занимательную беседу. Только теперь все будет проходить по моим правилам.";
    			link.l1.go = "Step_H7_6";
            } // Квест №8, Аарон дает задание
            if (npchar.id == "Chumakeiro" && pchar.questTemp.State == "SeekBible_toAaron")
            {
    			dialog.text = "Рад видеть вас, друг мой. Зачем пожаловали?";
    			link.l1 = "Минхер, я откомандирован в ваше распоряжение Петером Стэвезантом.";
    			link.l1.go = "Step_H8_1";
            }
            if (npchar.id == "Chumakeiro" && pchar.questTemp.State == "SeekBible_toBermudes")
            {
    			dialog.text = "С нетерпением ожидаю вашего доклада о поисках.";
    			link.l1 = "Я занимаюсь этим, результаты будут...";
    			link.l1.go = "exit";
            }
            if (npchar.id == "Chumakeiro" && pchar.questTemp.State == "SeekBible_DeGrafIsDead")
            {
    			dialog.text = "С нетерпением ожидаю вашего доклада о поисках.";
    			link.l1 = "Минхер, мне удалось выяснить, кто продавал искомую христианскую книгу. Этого флибустьера зовут Лоран де Граф. Вернее, звали, так как он погиб. Когда я добрался до него у берегов Картахены, он вел бой с очень мощной испанской эскадрой. Мне не удалось спасти его фрегат, сам Лоран утонул вместе с кораблем. Боюсь, что это все, книгу найти теперь просто невозможно.";
    			link.l1.go = "Step_H8_12";
            }
            if (npchar.id == "Chumakeiro" && pchar.questTemp.State == "SeekBible_DeGrafDieHard")
            {
    			dialog.text = "С нетерпением ожидаю вашего доклада о поисках.";
    			link.l1 = "Минхер, мне удалось выяснить, кто продавал искомую христианскую книгу. Этого флибустьера зовут Лоран де Граф. Когда я его близ Картахены, он вел бой с очень мощной испанской эскадрой. Я не смог ничем помочь де Графу, силы были слишком неравны. Мне пришлось покинуть поле боя.";
    			link.l1.go = "Step_H8_12";
            }
            if (npchar.id == "Chumakeiro" && pchar.questTemp.State == "SeekBible_BadResult")
            {
    			dialog.text = "Мне более нечего вам сказать. Отправляйтесь к Стэвезанту.";
    			link.l1 = "Хорошо, минхер.";
    			link.l1.go = "exit";
            }
            if (npchar.id == "Chumakeiro" && pchar.questTemp.State == "SeekBible_WeWon")
            {
    			dialog.text = "С нетерпением ожидаю вашего доклада о поисках.";
    			link.l1 = "Мне ничего не удалось отыскать. Я вынужден закончить поиски.";
    			link.l1.go = "Step_H8_12";
    			link.l2 = "Все ищу, дерусь, топлю испанцев... Но книгу не нашел пока... Однако, продолжу поиски...";
    			link.l2.go = "exit";
			}			
			if (npchar.id == "Chumakeiro" && pchar.questTemp.State == "SeekBible_IHaveMap")
            {
                dialog.text = "С нетерпением ожидаю вашего доклада о поисках.";
    			link.l1 = "Мне ничего не удалось отыскать. Я вынужден закончить поиски.";
    			link.l1.go = "Step_H8_12";
				if (CheckCharacterItem(pchar, "Bible"))
				{
           			link.l2 = "Мне удалось найти искомую книгу. Не знаю, то ли это, что вы ищете, но крест на обложке точно есть.";
        			link.l2.go = "Step_H8_13";
                }
                else
                {
        			link.l2 = "Я пока не сумел разыскать книгу, но я нахожусь в поиске. Вам нужно подождать...";
        			link.l2.go = "exit";
                }
            }
            if (npchar.id == "Chumakeiro" && pchar.questTemp.State == "SeekBible_GoodResult")
            {
    			dialog.text = "Я и вся наша община молимся за вас...";
    			link.l1 = "Хех, спасибо...";
    			link.l1.go = "exit";
            }

        break;

//<<<<<<<<<<<<===== Разброс диалогов по персонажам =====================
        case "Exit":
            NextDiag.CurrentNode = NextDiag.TempNode;
            DialogExit();
        break;

//********************************* Дилоги бандитов, грабящих Аарона. Квест №1 **********************************
 		case "Step_H1_1":
			dialog.text = "Я же сказал тебе - проваливай, не то выпущу кишки.";
			link.l1 = "Вообще-то, я против такого подхода, своими кишками очень дорожу. А вот на ваши кишки, похоже, пора взглянуть...";
			link.l1.go = "Step_H1_2";
		break;
 		case "Step_H1_2":
            LAi_SetWarriorType(npchar);
            LAi_group_MoveCharacter(npchar, "EnemyFight");
            LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
            LAi_group_FightGroups("EnemyFight", LAI_GROUP_PLAYER, true);
            LAi_group_SetCheck("EnemyFight", "OpenTheDoors");
			DialogExit();
			AddDialogExitQuest("MainHeroFightModeOn");
		break;
 		case "Step_H1_3":
			dialog.text = "Это я.";
			link.l1 = "Меня прислал за вами Петер Стэвезант, я должен доставить вас в Кюрасао. А что делали в вашем доме эти неучтивые люди?";
			link.l1.go = "Step_H1_4";
		break;
 		case "Step_H1_4":
			dialog.text = "Похоже, что они грабили меня. Дело в том, что я жду человека от Питера уже третий день, и когда постучали в дверь, я даже не спросил кто это, прежде чем открывать. В одну минуту я оказался скручен и брошен в эту комнату, сами же грабители активно начали потрошить мои личные вещи.";
			link.l1 = "Ну что же, все хорошо, что хорошо кончается. Собирайтесь и добро пожаловать на борт моего парусника. Стэвезант ждет вас, выходим немедленно.";
			link.l1.go = "Step_H1_5";
		break;
 		case "Step_H1_5":
            AddQuestRecord("Hol_Line_1_Chumakeiro", "2");
		    pchar.questTemp.State = "SeekChumakeiro_GoToVillemstad";
            LAi_SetActorType(npchar);
            LAi_ActorRunToLocation(npchar, "reload", "reload1", "none", "", "", "", 2.0);
            //ChangeCharacterReputation(pchar, 5); TO_DO eddy
            AddPassenger(pchar, npchar, false);
            SetCharacterRemovable(npchar, false);
            Pchar.quest.SeekChumakeiro_intoResidence.win_condition.l1 = "location";
            Pchar.quest.SeekChumakeiro_intoResidence.win_condition.l1.location = "Villemstad_townhall";
            Pchar.quest.SeekChumakeiro_intoResidence.win_condition = "SeekChumakeiro_intoResidence";
			DialogExit();
		break;
 		case "Step_H1_6":
			dialog.text = "А сейчас прошу вас принять от меня небольшое вознаграждение - 30000 монет.";
			link.l1 = "Отказываться не буду, спасибо.";
			link.l1.go = "Step_H1_7";
		break;
 		case "Step_H1_7":
            LAi_SetPlayerType(pchar);
			AddMoneyToCharacter(pchar, 30000);
			RemovePassenger(pchar, npchar);
			AddQuestRecord("Hol_Line_1_Chumakeiro", "3");
            LAi_SetActorType(npchar);
            LAi_ActorGoToLocation(npchar, "reload", "reload2", "none", "", "", "", 10.0);
			DialogExit();
		break;

//********************************* Дилоги лейтенанта в тюрьме. Квест №3 **********************************
 		case "Step_H3_1":
			dialog.text = "Ух ты, да это же здорово! Чем обязан таким вниманием к моей персоне и таким участим в моей судьбе?";
			link.l1 = "Мне нужно кое-что узнать от вас.";
			link.l1.go = "Step_H3_2";
		break;
 		case "Step_H3_2":
			dialog.text = "Весь к вашим услугам. Только сначала давайте выберем более умиротворенное место для столь занимательной беседы.";
			link.l1 = "Я обещаю, что выпущу тебя на свободу. Но сначала я должен получить необходимую мне информацию. Я хочу знать, куда готовит высадку десанта Мэдифорд?";
			link.l1.go = "Step_H3_3";
		break;
 		case "Step_H3_3":
			dialog.text = "Будет взят и уничтожен голландский форт Оранж.";
			link.l1 = "Черт возьми, все указывало именно на это! Я сдержу свое слово, ты свободен и можешь идти куда угодно. Прощай.";
			link.l1.go = "Step_H3_4";
		break;
 		case "Step_H3_4":
            ChangeCharacterAddressGroup(&characters[GetCharacterIndex("Henry Morgan")], "PortRoyal_houseS1", "sit", "sit2");
            LAi_SetHuberType(characterFromId("Henry Morgan"));
            AddQuestRecord("Hol_Line_3_WormEnglishPlans", "14");
 		    pchar.questTemp.State = "WormEnglPlans_GoodRusultWorm";
            ChangeCharacterAddressGroup(npchar, "SentJons_prison", "goto", "goto23");
            LAi_SetActorType(npchar);
            LAi_ActorRunToLocation(npchar, "reload", "reload1", "none", "", "", "", 15.0);
            DialogExit();
		break;

//********************************* Базар в Ла Веге о гибели Мэнсфилда. Квест №5 **********************************
  		case "Step_H5_1":
			dialog.text = "Испанцы напали на нас. Добрая половина народа полегла.";
			link.l1 = "Вот это да! Жаль Эдварда, великий был старик.";
			link.l1.go = "Step_H5_2";
		break;
 		case "Step_H5_2":
			chrDisableReloadToLocation = false;
 		    AddQuestRecord("Hol_Line_5_ThreeFleutes", "2");
            LAi_SetActorType(npchar);
            LAi_ActorGoToLocation(npchar, "reload", "reload1", "none", "", "", "", 5.0);
            Pchar.quest.ThreeFleutes_BackToCuracao.win_condition.l1 = "location";
            Pchar.quest.ThreeFleutes_BackToCuracao.win_condition.l1.location = "Hispaniola1";
            Pchar.quest.ThreeFleutes_BackToCuracao.win_condition = "ThreeFleutes_BackToCuracao";
            DialogExit();
		break;

//********************************* Базар с незнакомцем на Тортуге. Квест №7 **********************************
  		case "Step_H7_1":
			dialog.text = "В таверне вас ждет доверенное лицо Стэвезанта.";
			link.l1 = "И что лицу нужно от меня?";
			link.l1.go = "Step_H7_2";
		break;
  		case "Step_H7_2":
			dialog.text = "Срочное распоряжение генерал-губернатора, вам необходимо немедленно с ним ознакомиться.";
			link.l1 = "Хм... ну что же, хорошо.";
			link.l1.go = "Step_H7_3";
		break;
  		case "Step_H7_3":
			chrDisableReloadToLocation = false;
			pchar.questTemp.State = "DelivLettTortuga_toStranger";
            LAi_SetActorType(npchar);
            LAi_ActorGoToLocation(npchar, "reload", "gate_back", "none", "", "", "", 40.0);
            LAi_SetPlayerType(pchar);
            sld = GetCharacter(NPC_GenerateCharacter("AntonioDeBarras", "officer_9", "man", "man", 40, SPAIN, -1, true));
            sld.Dialog.Filename = "Quest\HolLineNpc_1.c";
        	sld.name 	= "Антонио";
        	sld.lastname = "де Баррас";
        	LAi_SetStayTypeNoGroup(sld);
            FantomMakeCoolSailor(sld, SHIP_WARSHIP, "Изабелла", CANNON_TYPE_CULVERINE_LBS32, 100, 100, 100);
            FantomMakeCoolFighter(sld, 40, 100, 100, "blade32", "pistol5", 150);
          	ChangeCharacterAddressGroup(sld, "Tortuga_tavern_upstairs", "goto", "goto1");
            DialogExit();
		break;

        case "Step_H7_4":
			dialog.text = "Меня зовут Антонио, но это так, к слову. А пока.\n"+
                          "Ну и ну! Ты смотри, кто к нам пожаловал!";
			link.l1 = "А?...";
			link.l1.go = "Step_H7_5";
		break;
  		case "Step_H7_5":
            LocatorReloadEnterDisable("Tortuga_town", "gate_back", false);
            LocatorReloadEnterDisable("Tortuga_town", "reload1_back", false);
            LocatorReloadEnterDisable("Tortuga_town", "reload2_back", false);
            LocatorReloadEnterDisable("Tortuga_tavern", "reload2_back", true);
            LAi_SetActorType(npchar);
            LAi_SetActorType(pchar);
            LAi_ActorTurnToLocator(pchar, "reload", "reload1_back");
            DoQuestFunctionDelay("DelivLettTortuga_Digress", 1.0);
            DialogExit();
		break;

        case "Step_H7_6":
			dialog.text = "И что это будут за правила?";
			link.l1 = "Ты будешь отвечать на вопросы, за каждый неверный ответ я буду отрезать у тебя по пальцу. Десять неверных ответов - не будет пальцев на руках. Правила ясны?";
			link.l1.go = "Step_H7_7";
		break;
        case "Step_H7_7":
			dialog.text = "Хм... Знаешь, в данный конкретный момент мои конечности очень мне дороги. Так что задавай свои вопросы, я все скажу.";
			link.l1 = "Вопрос номер один: кто ты и что делал на Тортуге?";
			link.l1.go = "Step_H7_8";
		break;
        case "Step_H7_8":
			dialog.text = "Меня зовут Антонио де Баррас, я испанский агент. На Тортуге я не ждал именно тебя, просто караулил посыльного от Стэвезанта. Определить голландский корабль, входящий в порт, не сложно. Ну а когда ты прямиком направился к д'Ожерону, все стало ясно.";
			link.l1 = "Хорошо... Вопрос номер два: где депеша?";
			link.l1.go = "Step_H7_9";
		break;
        case "Step_H7_9":
			dialog.text = "Она у меня. Пожалуйста, забирай...";
			link.l1 = "Очень хорошо, я рад за твои пальцы... Ну и хватит на этом, остальное - в Виллемстаде. Я надеюсь, вы окажите мне любезность, сеньор Антонио, посетить сей славный голландский город?";
			link.l1.go = "Step_H7_10";
            GiveItem2Character(pchar,"letter_1");
		break;
        case "Step_H7_10":
			dialog.text = "Боюсь, что в данном конкретном случае мое желание в учет принято не будет...";
			link.l1 = "Не будет, сеньор Антонио, это верно.";
			link.l1.go = "Step_H7_11";
		break;
        case "Step_H7_11":
            AddQuestRecord("Hol_Line_7_DelivLettTortuga", "11");
            pchar.questTemp.State = "DelivLettTortuga_LetterFound";
            pchar.quest.DelivLettTortuga_WarshipDieHard.over = "yes";
            pchar.quest.DelivLettTortuga_AfterWarshipSunk.over = "yes";
            Island_SetReloadEnableGlobal("SantaCatalina", true);
            QuestAboardCabinDialogSurrender();
            DialogExit();
		break;
		
//********************************* Поиски Библии. Квест №8 **********************************
        case "Step_H8_1":
			dialog.text = "О-о-о, это очень здорово, что именно вы будете помогать мне в поисках. Мне уже довелось видеть вас в деле.";
			link.l1 = "О каких поисках идет речь? Стэвезант ничего конкретного не сказал...";
			link.l1.go = "Step_H8_2";
		break;
        case "Step_H8_2":
			dialog.text = "Все верно, друг мой. Но давайте по порядку.\n"+
                          "Первым приверженцем учения Янсения, бежавшим из Европы в Кюрасао около двадцати лет назад, был некто Самуэль Кохэно. Он был очень богатым человеком, и однажды ему удалось через посредников выкупить за баснословные деньги в самой канцелярии Инквизиции в Севилье один очень и очень таинственный документ. Речь идет об еще одном Евангелие.\n"+
                          "Будучи христианином, вы, наверное, читали Ветхий и Новый заветы?";
			link.l1 = "Что я могу сказать, не без этого...";
			link.l1.go = "Step_H8_3";
		break;
        case "Step_H8_3":
			dialog.text = "А знаете ли вы, что библейские тексты, которыми мы сейчас располагаем, не являются первоисточниками, так сказать...";
			link.l1 = "Как это понимать?";
			link.l1.go = "Step_H8_4";
		break;
        case "Step_H8_4":
			dialog.text = "Дело в том, что Библию не написали сразу же после Воскресения Христа. Прошло достаточно много времени, прежде чем окрепшая церковь озаботилась необходимостью сбора и классификации христианского материала, дабы привести веру к единым догмам.\n"+
                          "Вот тогда-то и произошла детальная ревизия всего, что находилось в то время в руках церкви. Когда сбор и анализ информации был проведен, оказалось, что некоторые вещи прямо противоречат друг другу. Тогда был проведен церковный собор на самом высоком уровне, где и принимались решения относительно того, можно ли считать тот или иной документ истинным.";
			link.l1 = "Очень интересно. Но какое это отношение имеет к моему заданию?";
			link.l1.go = "Step_H8_5";
		break;
        case "Step_H8_5":
			dialog.text = "Терпение, друг мой, и вам все станет ясно.\n"+
                          "Итак, многое было признано ересью и, либо уничтожалось, либо пряталось так глубоко и надежно, что вытащить это вновь на свет Божий не представлялось возможным. Но Самуэлю это удалось! За астрономическую сумму он сумел подкупить иезуита-архивариуса и тот выкрал из тайных хранилищ Инквизиции неизвестное доселе Евангелие!\n"+
                          "Самуэль немедленно бежал из Старого Света, но несчастия преследовали его буквально на каждом шагу. В результате этот богатейший и достойнейший человек лишился всего, был продан в рабство и сгинул на плантациях сахарного тростника где-то здесь, в английских поселениях.";
			link.l1 = "Я должен найти место его смерти?";
			link.l1.go = "Step_H8_6";
		break;
        case "Step_H8_6":
			dialog.text = "Нет, в этом нет никакого смысла. Вы должны найти пропавший трактат.";
			link.l1 = "Даже не представляю себе, за что можно зацепиться, с чего начать поиски...";
			link.l1.go = "Step_H8_7";
		break;
        case "Step_H8_7":
			dialog.text = "Зацепка есть, именно она и явилась катализатором того, что я выпросил вас у Стэвезанта.";
			link.l1 = "Слушаю вас внимательно, Аарон.";
			link.l1.go = "Step_H8_8";
		break;
        case "Step_H8_8":
			dialog.text = "До меня дошли слухи, что один из английских каперов в пиратском поселении на Бермудах намеревался продать старую христианскую книгу, написанную на неизвестном ему языке.";
			link.l1 = "Хм, но если язык неизвестен, то как он определил, что книга христианская?";
			link.l1.go = "Step_H8_9";
		break;
        case "Step_H8_9":
			dialog.text = "По изображениям распятия на книге, друг мой.";
			link.l1 = "Понятно... Хорошо, как имя этого пирата?";
			link.l1.go = "Step_H8_10";
		break;
        case "Step_H8_10":
			dialog.text = "Я не знаю.";
			link.l1 = "Вот как... Ну что же, полагаю, я смогу узнать и его имя, и где эта книга сейчас.";
			link.l1.go = "Step_H8_11";
		break;
        case "Step_H8_11":
			dialog.text = "Я уверен в этом практически на сто процентов. Приступайте, друг мой. В случае получения успешного результата в поисках, то есть, если вы найдете эту книгу - награда будет очень высокой.";
			link.l1 = "Эта обнадеживающая новость несомненно придаст мне рвения. До встречи, Аарон, я немедленно приступаю к поискам.";
			link.l1.go = "exit";
            pchar.questTemp.State = "SeekBible_toBermudes";
            AddQuestRecord("Hol_Line_8_SeekBible", "2");
            QuestSetCurrentNode("Jackman", "HolLine8_quest");
		break;
        case "Step_H8_12":
			dialog.text = "Это очень и очень плохо... Ну что же, вы мне больше не нужны. Отправляйтесь обратно к Стэвезанту.";
			link.l1 = "Хорошо, минхер.";
			link.l1.go = "exit";
            pchar.questTemp.State = "SeekBible_BadResult";
            NPChar.LifeDay = 0;
    	    SaveCurrentNpcQuestDateParam(NPChar, "LifeTimeCreate");
		break;
        case "Step_H8_13":
			dialog.text = "Так, давайте посмотрим... Да, это именно то, что мы потеряли! Вы нашли утерянное сокровище Самуэля Кохэно! Господи, Слава Тебе!!!";
			link.l1 = "Ныне, и присно, и вовеки веков. Аминь.";
			link.l1.go = "Step_H8_14";
			TakeItemFromCharacter(pchar, "Bible");
		break;
        case "Step_H8_14":
			dialog.text = GetFullName(pchar) + ", я обещал вам, что награда ваша будет очень высокой. Во сколько вы оцените свою работу?";
			link.l1 = "Ну, у меня были накладные расходы, я потратил 235 тысяч на покупку. Все, что вы предложите мне сверх данной суммы, и будет являться наградой.";
			link.l1.go = "Step_H8_15";
		break;
        case "Step_H8_15":
			dialog.text = "Сверх этого я предложу вам один миллион. И безграничное уважение нашей общины до скончания века, которое, кстати, скоро придет...";
			link.l1 = "Хех, миллион - это неплохо, даже очень не плохо! А что это вы про конец света заговорили?";
			link.l1.go = "Step_H8_16";
		break;
        case "Step_H8_16":
			dialog.text = "Бренное существование человечества на этой земле - трагедия людей. Но скоро все придет в норму, к тому, с чего началось...";
			link.l1 = "Вы это серьезно? Проклятье, испортили мне все настроение от получения такой значительной суммы. Ну да ладно... Вы, пожалуй, ожидайте развязки своей трагедии, а я займусь более прозаичными вещами - суетой сует. С такой суммой в кармане эта моя личная трагедия существования доставит мне очень много приятных минут.";
			link.l1.go = "Step_H8_17";
		break;
        case "Step_H8_17":
			dialog.text = "Прощайте, и да хранит вас Господь!";
			link.l1 = "И вам всего самого наилучшего. Прощайте.";
			link.l1.go = "exit";
			AddMoneyToCharacter(pchar, 1235000);
            pchar.questTemp.State = "SeekBible_GoodResult";
            AddQuestRecord("Hol_Line_8_SeekBible", "10");
            NPChar.LifeDay = 0;
    	    SaveCurrentNpcQuestDateParam(NPChar, "LifeTimeCreate");
		break;
    }
}
