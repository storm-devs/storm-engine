// Герцог Альбермаль и другие
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
            //Альбермаль, англ.линейка, квест №1
            if (npchar.id == "Albermal")
            {
                dialog.text = "Я вас внимательно слушаю.";
                link.l1 = "Меня зовут " + GetFullName(pchar) + ". Я получил необходимые инструкции от губернатора Мэдифорда.";
                link.l1.go = "Step_E1_1";
            }
            // тюремщик в Кюрасао, квест №4
            if (npchar.id == "Hoverd_Tantum")
            {
    			dialog.text = "Что, попался, голубь?";
    			link.l1 = "Издеваешься?";
    			link.l1.go = "Step_E4_1";
            }
            // ==> Комендант форта Оранж,  квест №5
            if (npchar.id == "Orange_Comendant")
            {
    			dialog.text = "Аргх... Проклятые английские псы! Хоть одному успею вспороть живот...";
    			link.l1 = "Попробуй...";
    			link.l1.go = "Step_E5_1";
            }
            // ==> Квест №6, Моррис Уильямс
            if (npchar.id == "Morris_Willams")
            {
                dialog.text = "Будь проклят тот день, когда ты вошел ко мне на палубу...";
                link.l1 = "Хм, зря ты так...";
                link.l1.go = "exit";
            }  
            if (pchar.questTemp.State == "MorrisWillams_GoToMorrisBegin" && npchar.id == "Morris_Willams")
            {
    			dialog.text = "Приветствую! Какими судьбами у меня на палубе?";
    			link.l1 = "Пришел посмотреть на удачливого капера. Говорят, ты взял три испанских галеона!";
    			link.l1.go = "Step_E6_20";
            }
            if (pchar.questTemp.State == "MorrisWillams_AfterFirstTalkingWithMorris" && npchar.id == "Morris_Willams")
            {
    			dialog.text = "Мы же уже все обсудили " + pchar.name + ". Я становлюсь под разгрузку в Порт Рояле.";
    			link.l1 = "Очень хорошо.";
    			link.l1.go = "exit";
            }
            if (pchar.questTemp.State == "MorrisWillams_ModifordGood5000" && npchar.id == "Morris_Willams")
            {
                LAi_ActorSetSitMode(pchar);
                dialog.text = "Черт возьми! Я послушал тебя, зашел в порт и у меня тут же конфисковали груз! Ты подставил меня!";
                link.l1 = "Что? Кто конфисковал?";
                link.l1.go = "Step_E6_2";
            }
            if (pchar.questTemp.State == "MorrisWillams_ModifordPolitic" && npchar.id == "Morris_Willams")
            {
                dialog.text = "Ну, что сказал тебе этот пройдоха в мундире губернатора?";
                link.l1 = "Мне тяжело это признавать, но, похоже, нас надули. Мэдифорд не отдаст твой товар.";
                link.l1.go = "Step_E6_6";
            }
            if (pchar.questTemp.State == "MorrisWillams_PinnaceNotFound" || pchar.questTemp.State == "MorrisWillams_2PinnaceNotFound")
            {
                dialog.text = "Ты захватил мой индиго?";
                link.l1 = "Я погнался за пинасом, но не смог увидеть даже его кормы, он ушел от меня. Извини...";
                link.l1.go = "Step_E6_11";
            }
            if (pchar.questTemp.State == "MorrisWillams_PinnaceBoard" && npchar.id == "Morris_Willams")
            {
                dialog.text = "Ты захватил мой индиго?";
                link.l1 = "У берегов Сан-Хуана я захватил испанский искомый пинас. Только в трюмах там не было ни индиго, ни серебра, ни красного дерева.";
                link.l1.go = "Step_E6_10";
            }
            if (pchar.questTemp.State == "MorrisWillams_PinnaceBoardNotFoundMoney" && npchar.id == "Morris_Willams")
            {
                dialog.text = "Ты захватил мой индиго?";
                link.l1 = "У берегов Сан-Хуна я захватил испанский искомый пинас. Только в трюмах там не было ни индиго, ни серебра, ни красного дерева. Я обыскал весь корабль, но ничего ценного не нашел, извини.";
                link.l1.go = "Step_E6_11";
            }
            if (pchar.questTemp.State == "MorrisWillams_DieHard" && npchar.id == "Morris_Willams")
            {
                dialog.text = "Ты захватил мой индиго?";
                link.l1 = "Извини, но этот пинас оказался мне не по зубам. Извини, но не я сумел взять его.";
                link.l1.go = "Step_E6_11";
            }
            if (pchar.questTemp.State == "MorrisWillams_PinnaceSink" && npchar.id == "Morris_Willams")
            {
                dialog.text = "Ты захватил мой индиго?";
                link.l1 = "Чертов пинас пошел ко дну, а вместе с ним и весь твой приз. Я сделал все, что мог, извини.";
                link.l1.go = "Step_E6_11";
            }
			// ==> Квест №12, диалоги с полковником Томасом Линчем
            if (pchar.questTemp.State == "ColonelLinch_GoToKingstown")
            {
                dialog.text = "С кем я имею честь разговаривать? Представьтесь!";
                link.l1 = "Я английский капер, сэр. Мое имя - " + GetFullName(pchar) + ".";
                link.l1.go = "Step_E12_1";
            }

            if (pchar.questTemp.State == "ColonelLinch_ArrestModiford")
            {
                dialog.text = "Вы должны арестовать Мэдифорда. Поторопитесь.";
                link.l1 = "Хорошо, сэр.";
                link.l1.go = "exit";
            }
        break;

//<<<<<<<<<<<<===== Разброс диалогов по персонажам =====================
        case "Step_E1_1":
            if (GetQuestPastDayParam("questTemp") < 2)
            {
                dialog.text = "Вы и ваш корабль готовы к выполнению задачи?";
                link.l1 = "Я готов, отправляемся немедленно.";
                link.l1.go = "Go_with_Albermal";
                link.l2 = "Позже, мне нужно пополнить трюмы. Путешествие будет опасным.";
                link.l2.go = "exit";
			}
			else
			{
				dialog.text = "Вы неоправданно задерживаетесь с выполнением данного вам поручения! Вы же знаете, что я ограничен во времени. Сколько можно ждать?";
				link.l1 = "Сэр, прошу прощения за задержку, но это было вызвано необходимостью. Теперь я готов, выступаем немедленно!";
				link.l1.go = "Go_with_Albermal";
				link.l2 = "Сэр, мне нужно провести еще ряд подготовительных мероприятий. Я не могу подвергать вашу жизнь неоправданному риску из-за недостаточной подготовки операции. Прошу вас подождать еще немного.";
				link.l2.go = "exit";
				ChangeCharacterReputation(pchar, -1);
            }
            NextDiag.TempNode = "Step_E1_1";
        break;

        case "Go_with_Albermal":
            AddQuestRecord("Eng_Line_1_Albermal", "2");
            AddPassenger(pchar, npchar, false);
            SetCharacterRemovable(npchar, false);
            LAi_SetActorType(npchar);
            LAi_ActorGoToLocation(NPChar, "reload", "reload1", "none", "", "", "", 10.0);
			pchar.quest.fight_withSpanish_Albermal.win_condition.l1          = "location";
			pchar.quest.fight_withSpanish_Albermal.win_condition.l1.location = "Antigua";
			pchar.quest.fight_withSpanish_Albermal.win_condition             = "fight_withSpanish_Albermal";
            NextDiag.CurrentNode = NextDiag.TempNode;
            DialogExit();
        break;

        case "in_the_SentJons":
            if (GetQuestPastDayParam("questTemp") < 14)
            {
                dialog.text = "Я благодарен вам от всей души. Вы проявили настоящий героизм в сражении с серьезным противником. К тому же, прибытие в Сент-Джонс состоялось в отведенные сроки. Я даю вам письмо для сэра Томаса Мэдифорда, передайте эту депешу ему лично в руки. Прощайте!";
                link.l1 = "Удачной дороги до Англии! Прощайте, сэр.";
                link.l1.go = "Albermal_go_out";
                pchar.questTemp.State = "Albermal_good_time";
            }
            else
            {
                dialog.text = "Я благодарен вам за героизм, проявленный в сражении с испанцами. Но из-за вашей медлительности у меня разрушены все планы! Я передаю с вами письмо для Мэдифорда. Ступайте, я вас больше не задерживаю.";
                link.l1 = "Всего наилучшего, сэр.";
                link.l1.go = "Albermal_go_out";
                ChangeCharacterReputation(pchar, -4);
                pchar.questTemp.State = "Albermal_late";
            }
            GiveItem2Character(pchar, "letter_Albermal");
            NextDiag.TempNode = "in_the_SentJons";
        break;

        case "Albermal_go_out":
            AddQuestRecord("Eng_Line_1_Albermal", "4");
            RemovePassenger(pchar, npchar);
            DialogExit();
            // уходит
            LAi_SetActorType(NPChar);
            LAi_ActorGoToLocation(NPChar, "reload", "reload2", "none", "", "", "", 10.0);
            LAi_SetPlayerType(pchar);
    	    NPChar.LifeDay = 1; // уберем нпс на след.день.
    	    SaveCurrentNpcQuestDateParam(NPChar, "LifeTimeCreate");
        break;

        case "Exit":
            NextDiag.CurrentNode = NextDiag.TempNode;
            DialogExit();
        break;

//********************************* Диалоги тюремщика по четвертому квесту английки **********************************
 		case "Step_E4_1":
			dialog.text = "Нет, спрашиваю. Вляпался ты серьезно. А что случилось то? Что за шум был в городе из-за тебя?";
			link.l1 = "Ваш ростовщик Йозеф Нунен де Фонкесао - порядочный мерзавец. У меня было к нему деловое предложение, но, когда мы не договорились, он вызвал охрану и меня арестовали, забрали оружие и деньги. Теперь я обвиняюсь в шпионаже.";
			link.l1.go = "Step_E4_2";
		break;

 		case "Step_E4_2":
			dialog.text = "Хм, похоже на нашего Йозефа... А что за дело ты ему предложил?";
			link.l1 = "Купить драгоценности и кое-какое испанское барахло. Я корсар, не хотел идти на Тортугу, решил сдать по-близости.";
			link.l1.go = "Step_E4_3";
		break;

 		case "Step_E4_3":
			dialog.text = "Хех, против парней-корсаров я ничего не имею, вы мне нравитесь, ребята. Однако, как пить дать, Йозеф этой же ночью постарается тебя здесь убить, чтобы ты не сболтнул лишнего, ведь связи с корсарами и слава барахольщика ему ни к чему. Он, вроде как, уважаемый человек в городе.";
			link.l1 = "Поможешь мне выбраться - я в долгу не останусь.";
			link.l1.go = "Step_E4_4";
		break;

 		case "Step_E4_4":
			dialog.text = "Я открою твой замок на решетке. Ты сможешь выйти, как только я уйду. Денег мне не нужно, у меня с Йозефом свои счеты...";
			link.l1 = "Понял. Что ж, спасибо тебе.";
			link.l1.go = "Step_E4_5";
		break;

 		case "Step_E4_5":
			dialog.text = "Отсюда пробивайся сам, здесь я тебе не помощник. Кстати, так у тебя нет оружия, даю тебе саблю из тюремного арсенала.\n"+
                          "Прощай. Надеюсь Йозеф долго будет в бешенстве.";
			link.l1 = "Это уж не сомневайся. Прощай, друг. Еще раз спасибо за помощь.";
			link.l1.go = "Step_E4_6";
		break;

 		case "Step_E4_6":
            DoQuestReloadToLocation("Villemstad_prison", "goto", "goto12", "");
            AddQuestRecord("Eng_Line_4_Intelligence_Curacao", "7");
            ChangeCharacterReputation(pchar, -1);  // за обман.
            GiveItem2Character(pchar,"blade1");   // себельку герою; и этого бы не давал, но без оружия солдеры не нападают.
            EquipCharacterbyItem(pchar, "blade1");
            pchar.questTemp.State = "Intelligence_Curacao_OutPrison";
			pchar.questTemp.jailCanMove = false; //на всякий случай
			//==> делаем солдат поменьше, а то хрен выберешься 
			for (int i=1; i<=4; i++)
			{
				sld = characterFromId("VillemstadJailSold_"+i);
				if (sld.location.locator != "soldier3")
				{
					ChangeCharacterAddress(sld, "None", "");
				}
			}
			ChangeCharacterAddress(npchar, "None", "");
			DialogExit();
		break;
//*************************************** Диалог коменданта форта Оранж *********************************
        case "Step_E5_1":
            LAi_SetPlayerType(pchar);
            LAi_group_MoveCharacter(npchar, "EnemyFight");
            LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
            LAi_group_FightGroups("EnemyFight", LAI_GROUP_PLAYER, true);
            LAi_group_SetCheck("EnemyFight", "AttackFortOrange_AfterFightWithCommendant");
            ChangeCharacterHunterScore(pchar, "holhunter", 70);
			DialogExit();
			AddDialogExitQuest("MainHeroFightModeOn");
		break;

//**************************************** Диалоги Морриса Уильямса **************************************
        case "Step_E6_2":
            dialog.text = "Мэдифорд, по его распоряжению.";
            link.l1 = "Ничего не понимаю. Зачем конфисковали?";
            link.l1.go = "Step_E6_3";
        break;

        case "Step_E6_3":
            dialog.text = "Груз передали испанскому собственнику.";
            link.l1 = "Я своим ушам не верю! А что, мы с испанцами друзья?";
            link.l1.go = "Step_E6_4";
        break;

        case "Step_E6_4":
            dialog.text = "Черт его знает!";
            link.l1 = "Я разберусь, жди меня здесь.";
            link.l1.go = "Step_E6_5";
        break;

        case "Step_E6_5":
            AddQuestRecord("Eng_Line_6_MorrisWillams", "4");
            pchar.questTemp.State = "MorrisWillams_ToModifordForCoolTalking";
            NextDiag.CurrentNode = "Willams_waiting";
            DialogExit();
        break;

        case "Willams_waiting": // если ГГ подойдет в таверне к Уильямсу до проведения разговора с Мэдифордом
            dialog.text = "Ну что, узнал что-нибудь?";
            link.l1 = "Пока нет, но я обязательно разберусь с этим делом. Жди и никуда не уходи из таверны.";
            link.l1.go = "exit";
            NextDiag.TempNode = "Willams_waiting";
        break;

        case "Step_E6_6":
            dialog.text = "Ты просто уничтожил меня. Этот товар шел у меня в погашение долгов.\n"+
                          "Как только я сошел на берег в порту, то к своему величайшему удивлению узнал, что все мои долговые расписки скупил местный ростовщик. Он заблокировал мне выход в море, а мой корабль находится под залогом выплаты долга. Вот так!";
            link.l1 = "Сколько ты должен?";
            link.l1.go = "Step_E6_7";
            LocatorReloadEnterDisable("PortRoyal_town", "gate_back", false); //открываем выход из города
            LocatorReloadEnterDisable("PortRoyal_town", "reload1_back", false);
        break;

        case "Step_E6_7":
            dialog.text = "Мой долг составляет сто сорок тысяч.";
            link.l1 = "Знаешь что, твой долг - твои проблемы. До свидания и будь здоров.";
            link.l1.go = "Willams_GoogBye";
            link.l2 = "У меня есть 140 000. Я виноват перед тобой, ведь из-за меня ты попал в ловушку. Так что бери и решай свои проблемы.";
            link.l2.go = "Willams_GiveMoney";
            link.l3 = "Я верну твой товар.";
            link.l3.go = "Willams_BackGoods";
        break;
        
        case "Willams_GoogBye":
            dialog.text = "И тебе здоровья, дорогой друг (с иронией).";
            link.l1 = "Да будем мы все здоровы, аминь... Прощай.";
            link.l1.go = "exit";
            SaveCurrentQuestDateParam("questTemp");
            pchar.questTemp.State = "empty";
            pchar.questTemp.Waiting_time = "10";
            pchar.questTemp.CurQuestNumber = "7";
            ChangeCharacterReputation(pchar, -100); 
            AddCharacterExpToSkill(pchar, "Leadership", -350);
            AddCharacterExpToSkill(pchar, "Accuracy", -50);
            AddCharacterExpToSkill(pchar, "Commerce", -150);
            AddCharacterExpToSkill(pchar, "Sneak ", -150);
            AddCharacterExpToSkill(pchar, "Fencing  ", -100);
            AddQuestRecord("Eng_Line_6_MorrisWillams", "6");
            CloseQuestHeader("Eng_Line_6_MorrisWillams");
            NextDiag.TempNode = "Willams_Fack";
			//слухи
			AddSimpleRumour("Капитан, ходят слухи, что вы поступили с беднягой Вильямсом не лучшим образом...", ENGLAND, 5, 1);
        break;

        case "Willams_Fack":
            dialog.text = "Вы крайне непорядочный человек, капитан " + GetFullName(pchar) + ". Я не желаю с вами разговаривать.";
            link.l1 = "Как знаете, капитан.";
            link.l1.go = "exit";
            NextDiag.TempNode = "Willams_Fack";
			//слухи
			AddSimpleRumour("Моррис Вильямс рассказывает о вас такие жуткие вещи! Послушать его, так вы форменный мерзавец...", ENGLAND, 5, 1);
        break;

        case "Willams_GiveMoney":
            dialog.text = "Вот это да, таких денег у тебя нет. Что ж, я буду ждать, когда ты их соберешь здесь, в таверне.";
            link.l1 = "Да, точно, упустил из виду... Но ничего, я скоро вернусь, долго ждать тебе не придется.";
            link.l1.go = "exit";
            NextDiag.TempNode = "Step_E6_7";
            if (sti(pchar.money) >= 140000)
            {
                dialog.text = "Ты честный и благородный человек, капитан. Законы берегового братства для тебя не пустой звук. Я возьму эти деньги, так как они мне крайне необходимы. Благодарю тебя.";
                link.l1 = "Не стоит благодарностей, это была и моя вина. Как закроешь свои долги, надеюсь встретить тебя в море. До свидания, друг.";
                link.l1.go = "exit";
                SaveCurrentQuestDateParam("questTemp");
                pchar.questTemp.State = "empty";
                pchar.questTemp.Waiting_time = "5";
                pchar.questTemp.CurQuestNumber = "7";
                ChangeCharacterReputation(pchar, 20);
                AddCharacterExpToSkill(pchar, "Leadership", 150);
                AddCharacterExpToSkill(pchar, "Commerce", 250);
                AddCharacterExpToSkill(pchar, "Sneak ", 50);
                AddCharacterExpToSkill(pchar, "Defence", 150);
                AddQuestRecord("Eng_Line_6_MorrisWillams", "7");
                CloseQuestHeader("Eng_Line_6_MorrisWillams");
                AddMoneyToCharacter(pchar, -140000);
                NextDiag.TempNode = "Willams_NiceTalking";
				//слухи
				AddSimpleRumour("Капитан, говорят вы здорово выручили Вильямса внесением денежной суммы в погашение его долга. Вы благородный человек, капитан, для меня честь разговаривать с вами!", ENGLAND, 5, 1);
            }
        break;

        case "Willams_NiceTalking":
            dialog.text = "Очень рад видет своего друга! Как поживаете, капитан " + GetFullName(pchar) + "?";
            link.l1 = "Все в порядке, Моррис. Надеюсь, у тебя тоже все в порядке.";
            link.l1.go = "exit";
            NextDiag.TempNode = "Willams_NiceTalking";
        break;

        case "Willams_BackGoods":
            dialog.text = "Хотел бы я взяться за это, но не могу, мой корабль в порту на приколе.\n"+
                          "А тебе пожелаю удачи. Конфискованный товар испанцы уже должны были забрать. Тебе нужно узнать каким кораблем они собираются его вывезти, и тогда - дело за малым!";
            link.l1 = "По-твоему, кто может это знать?";
            link.l1.go = "Step_E6_8";
        break;
        
        case "Step_E6_8":
            dialog.text = "Твой друг Мэдифорд в первую очередь, кто же еще!\n"+
                          "А так не знаю, поспрашивай в таверне, в борделе, в порту, а может и на улице встретишь нужного человека.";
            link.l1 = "Побегу искать, время не ждет.";
            link.l1.go = "Step_E6_9";
        break;

        case "Step_E6_9":
            dialog.text = "Да уж, поторопись, пожалуйста. Мой груз - красное дерево, серебро и индиго, остальное меня мало волнует.";
            link.l1 = "Хорошо. Жди меня здесь, с этим делом я разберусь.";
            link.l1.go = "exit";
            NextDiag.TempNode = "Willams_WaitSeek";
            pchar.questTemp.State = "MorrisWillams_SeekSpanishGoods";
            AddQuestRecord("Eng_Line_6_MorrisWillams", "8");
        break;

        case "Willams_WaitSeek":
            dialog.text = "Раскопал что-нибудь? Когда я получу свой груз?";
            link.l1 = "Я этим занимаюсь, жди.";
            link.l1.go = "exit";
            NextDiag.TempNode = "Willams_WaitSeek";
        break;

        case "Step_E6_10":
            dialog.text = "Черт возьми! А куда же девался мой товар?";
            link.l1 = "Понятия не имею, возможно разгрузились где-то по дороге. Извини.";
            link.l1.go = "MorrisWillams_NotGiveMoney";
            link.l2 = "Полагаю, испанцы продали его по дороге. В каюте капитана я нашел сто сорок тысяч монет! Это твои деньги, теперь ты можешь расчитаться с долгами.";
            link.l2.go = "MorrisWillams_GiveMoney";
            NextDiag.TempNode = "Willams_NiceTalking";
            SaveCurrentQuestDateParam("questTemp");
            pchar.questTemp.State = "empty";
            pchar.questTemp.CurQuestNumber = "7";
        break;

        case "MorrisWillams_NotGiveMoney":
            dialog.text = "Все, я пропал. Что ж, дружище, спасибо тебе, ты сделал все, что мог.";
            link.l1 = "Бывай, Моррис. Желаю тебе избегнуть тюрьмы.";
            link.l1.go = "MorrisWillams_GoAway";
            pchar.questTemp.Waiting_time = "10";
            ChangeCharacterReputation(pchar, -100);
            AddCharacterExpToSkill(pchar, "Leadership", -150);
            AddCharacterExpToSkill(pchar, "Accuracy", -200);
            AddCharacterExpToSkill(pchar, "Commerce", -250);
            AddCharacterExpToSkill(pchar, "Sneak ", -170);
            AddCharacterExpToSkill(pchar, "Fencing  ", -140);
            AddQuestRecord("Eng_Line_6_MorrisWillams", "13");
            CloseQuestHeader("Eng_Line_6_MorrisWillams");
			//слухи
			AddSimpleRumour("Говорят, капитан, что по вашей вине Моррис Вильямс оказался без гроша в кармане...", ENGLAND, 5, 1);
        break;

        case "MorrisWillams_GiveMoney":
        if (sti(pchar.money) >= 140000)
        {
            dialog.text = "Ты сдержал слово, спасибо! Отныне ты мой друг раз и навсегда. Мне нечем тебя сейчас отблагодарить. Правда... есть одна вещица, важность которой в нашем деле трудно переоценить. Ты человек бесстрашный и, возможно, сможешь сделать то, на что я моя команда решиться так и не смогли.";
            link.l1 = "Хм, заинтересовал. Говори дальше, что это за вещица и на что отчаянные головорезы решиться так и не смогли.";
            link.l1.go = "Step_E6_14";
        }
        else
        {
            dialog.text = "И где деньги? У тебя же их нет!";
            link.l1 = "Вот черт, забыл в каюте. Сейчас сбегаю и все 140000 будут у тебя, не волнуйся.";
            link.l1.go = "exit";
            NextDiag.TempNode = "MorrisWillams_GiveMoney";
        }
        break;

        case "Step_E6_14":
            dialog.text = "Вещица - это кираса, второй такой на Карибах, полагаю, нет. Об стальном слушай дальше... \n"+
                          "Два с половиной года назад мой фрегат бурей вынесло на рифы на севере Мейна, в залив Гибели. Из этой переделки живыми вышла только половина моей команды. Остальные так и остались лежать на берегу.\n"+
                          "Среди пропавших в ту жуткую ночь оказался и мой тогдаший боцман, голландец Ламберт Грофф - отчаянной храбрости человек.\n"+
                          "Вот у него то, кроме другой прекрасной аммуниции, и была та самая кираса, настоящее произведение искусства и в крайней степени надежный доспех. Хотел я было вернуться к кораблю потом, но мои оставшиеся в живых головорезы в слезах уговорили меня не делать этого.\n"+
                          "А все дело в том, что после крушения мы кое-как добрались до залива Корсумель, там нас подобрало голландское торговое судно, которое доставило нас до Тортуги. По прибытию пять человек из моей команды изъявили желание уйти на люггере к мысу Каточе, в бухту с нашим разбитым фрегатом.\n"+
                          "Оно и понятно - нужно было забрать сохранившиеся драгоценности, деньги. Когда не вернулись первые, отправились еще пять пиратов. Только вот больше их никто не видел, ни тех, ни других. Так что я до сих пор не знаю, где эти корсары и что с ними. Но на место крушения своего фрегата, пожалуй, действительно не сунусь.\n"+
                          "Ну а ты смотри сам, можешь и попытаться. Возможно мои пираты просто удрали с деньгами прямиком в Старый Свет или еще куда. Кто его знает...\n"+
                          "А кираса, черт ее дери, дорогого стоит. И не в деньгах дело, а в сохранении собственного здоровья и жизни в бою.";
            link.l1 = "Что ж, благодарю тебя, друг. Я подумаю, может и наведаюсь.";
            link.l1.go = "exit";
            pchar.questTemp.Waiting_time = "2";
            ChangeCharacterReputation(pchar, 20);
            AddCharacterExpToSkill(pchar, "Leadership", 250);
            AddCharacterExpToSkill(pchar, "Accuracy", 100);
            AddCharacterExpToSkill(pchar, "Commerce", 250);
            AddCharacterExpToSkill(pchar, "Sneak", 70);
            AddCharacterExpToSkill(pchar, "Fencing", 150);
            AddCharacterExpToSkill(pchar, "Defence", 150);
            AddQuestRecord("Eng_Line_6_MorrisWillams", "14");
            CloseQuestHeader("Eng_Line_6_MorrisWillams");
            AddMoneyToCharacter(pchar, -140000);
            NextDiag.TempNode = "Willams_NiceTalking";
            Pchar.quest.MorrisWillams_SeekCirassa.win_condition.l1 = "location";
            Pchar.quest.MorrisWillams_SeekCirassa.win_condition.l1.location = "Shore_ship2";
            Pchar.quest.MorrisWillams_SeekCirassa.win_condition = "MorrisWillams_SeekCirassa";
			//слухи
			AddSimpleRumour("Моррис Вильямс всюду только и говорит, какой вы отчаянный рубака, прекрасный кэп и честный человек. Характеристика, однако, просто великолепная! Не думал даже, что еще живут такие люди на земле.", ENGLAND, 5, 1);
        break;

        case "MorrisWillams_GoAway":
			chrDisableReloadToLocation = true;
            LAi_SetActorType(npchar);
            ChangeCharacterAddressGroup(npchar, "PortRoyal_tavern", "tables", "stay8");
            LAi_ActorGoToLocation(npchar, "reload", "reload1_back", "none", "", "", "OpenTheDoors", -1);
            NextDiag.CurrentNode = "First time";
            DialogExit();
        break;

        case "Step_E6_11":
            dialog.text = "Да пошел ты со своими извинениями. Мне грозит тюрьма, а ты извинения просишь...";
            link.l1 = "Несмотря на неудачу с пинасом, я все же попытаюсь тебе помочь.";
            link.l1.go = "Step_E6_12";
            link.l2 = "Знаешь, твое хамство мне уже порядком надоело. В конце-то концов, я тебе ничем не обязан. Прощай и будь здоров.";
            link.l2.go = "Willams_GoogBye";
        break;

        case "Step_E6_12":
            dialog.text = "Ну что ж, буду благодарен за любую помощь. А что ты собираешься предпринять?";
            link.l1 = "Пока не знаю. Давай посмотрим, что будет дальше, а потом уже будем принимать решение что предпринять.";
            link.l1.go = "Step_E6_13";
        break;

        case "Step_E6_13":
            dialog.text = "Будь по твоему. " + pchar.name + ", спасибо что не бросаешь меня в беде. Сейчас мне нужно идти по делам, увидимся позже.";
            link.l1 = "Не переживай, все будет хорошо.";
            link.l1.go = "MorrisWillams_GoAway";
            pchar.questTemp.State = "MorrisWillams_GoToHimselfBussines"
            Pchar.quest.MorrisWillams_Arrest.win_condition.l1 = "ExitFromLocation";
            Pchar.quest.MorrisWillams_Arrest.win_condition.l1.location = "PortRoyal_tavern";
            Pchar.quest.MorrisWillams_Arrest.win_condition = "MorrisWillams_Arrest";
        break;

        case "Step_E6_20":
			dialog.text = "Так и есть. Мне удалось перехватить небольшую эскадру, состоящую из трех испанцев.\n"+
                          "Мой фрегат атаковал этих набожных католиков и те бросились наутек, когда нужно было противостоять мне всем вместе. Я взял галеоны на абордаж по очереди!";
			link.l1 = "Пустил кровь испанцам?";
			link.l1.go = "Step_E6_21";
        break;
        case "Step_E6_21":
			dialog.text = "Напротив, я не позволил своим ребятам устраивать резню на первом галеоне. Остальные, видя, что их жизни ничего не угрожает, сдались практически без боя.";
			link.l1 = "Хм, в Древней Элладе спартанцы в сражении никогда не преследовали противника, который побежал, это было запрещено законом. Противники лакедемонян знали, что если бросят оружие, то останутся живы. Поэтому противник, как правило, сдавался спартанцам после первого же натиска. Ты применил ту же тактику, она дала хороший результат.";
			link.l1.go = "Step_E6_22";
        break;
        case "Step_E6_22":
			dialog.text = "Репутация милосердного пирата может быстро сделать меня богатым. Это я уже понял, хотя о спартанцах ничего такого не знал.\n"+
                          "Ладно, говори, зачем пришел?";
			link.l1 = "Томас Мэдифорд дает тебе разрешение пристать к Порт Роялю и реализовать призовой товар в магазинах острова.";
			link.l1.go = "Step_E6_23";
        break;
        case "Step_E6_23":
			dialog.text = "Хм, у меня французский каперский патент, я хочу идти на Тортугу, там у меня отлаженные связи.";
			link.l1 = "Приглашениями губернатора Ямайки не разбрасываются, это глупо. Советую принять.";
			link.l1.go = "Step_E6_24";
        break;
        case "Step_E6_24":
			dialog.text = "Что ж, мне меньше проблем: не нужно тащиться до Тортуги с полными трюмами.\n"+
                          "Хоть я немного и теряю в цене, все же я согласен, зайду в порт немедленно.";
			link.l1 = "Всего хорошего, Моррис. Мы встретимся еще.";
			link.l1.go = "exit";
			pchar.questTemp.State = "MorrisWillams_AfterFirstTalkingWithMorris";
			AddQuestRecord("Eng_Line_6_MorrisWillams", "2");
        break;

//**************************************** Диалоги Томаса Линча **************************************
        case "Step_E12_1":
            dialog.text = "Я слышал о вас. Ваши заслуги перед Англией весьма высоки.";
            link.l1 = "Спасибо, сэр.";
            link.l1.go = "Step_E12_2";
        break;

        case "Step_E12_2":
            dialog.text = "Меня зовут Томас Линч. Я был председателем Совета Ямайки до 1664 года, т.е. до момента назначения губернатором этого проходимца Мэдифорда.";
            link.l1 = "Сэр, Томас Мэдифорд является губернатором Ямайки и...";
            link.l1.go = "Step_E12_3";
        break;

        case "Step_E12_3":
            dialog.text = "Он больше не является губернатором Ямайки. Вы должны знать, что терпение правительства лопнуло.\n"+
                          "По назначению губернатором Ямайки Мэдифорд имел совершенно четкие распоряжения от Его Величества Карла II обуздать пиратство и поддерживать дружеские отношения с Испанией.\n"+
                          "Вместо того, чтобы следовать полученным указаниям Мэдифорд потворствовал пиратству, а последний его 'фокус' - захват и разграбление побережья Мейна - явился беспрецедентным скандалом в Европе! Королева Испании Анна Австрийская трое суток пребывала в форменной истерике.";
            link.l1 = "Сэр, испанцы во многом сами виноваты в этом. Это они начали боевые действиея с нападения на Порт Рояль.";
            link.l1.go = "Step_E12_4";
        break;

        case "Step_E12_4":
            dialog.text = "И в этом сложно их обвинять, ведь Порт Рояль сейчас является прибежищем бандитов и грабителей всех мастей.\n"+
                          "Вы должны понимать, что равновесие сил в Европе нельзя подвергать риску ради того, чтобы набить реалами разбойничьи кошельки!";
            link.l1 = "А как же Тортуга, сэр?";
            link.l1.go = "Step_E12_5";
        break;

        case "Step_E12_5":
            dialog.text = "Тортуга является французской колонией, а Испания и Франция перманентно находятся в состоянии войны. Ха, Людовик, если так можно выразится, остужает пыл папистов в Европе.\n"+
                          "Англия не может позволить себе серьезного усиления французского влияния в Старом Свете, поэтому мы и имеем мирный договор с Испанией.\n"+
                          "А наш 'уважаемый' сэр Томас Мэдифорд творит здесь черт знает что!!!";
            link.l1 = "Понятно, сэр.";
            link.l1.go = "Step_E12_6";
        break;

        case "Step_E12_6":
            dialog.text = "Более того, мы имеем совершенно четкие основания полагать, что Мэдифорд сохраняет существующее в Порт Рояле положение вещей исходя из собственных корыстных интересов.\n"+
                          "Проще говоря, он обирает каперов, которые обирают испанцев.";
            link.l1 = "Сэр, мы действительно платим 10 процентов от приза в английскую казну.";
            link.l1.go = "Step_E12_7";
        break;

        case "Step_E12_7":
            dialog.text = "Черта с два! Английское правительство не видит этих денег.\n"+
                          "Нам также известно, что Мэдифорд имеет особые отношения с авторитетными пиратами и буканьерами. И их совместные так называемые 'дела' имеют астрономические прибыли.";
            link.l1 = "Сэр, мне приходилось участвовать в некоторых таких делах, и я могу подтвердить, что так оно и есть.";
            link.l1.go = "Step_E12_8";
        break;

        case "Step_E12_8":
            dialog.text = "Даже и не сомневаюсь в этом... Послушайте, у меня к вам есть деловое предложение.";
            link.l1 = "Внимательно слушаю, сэр.";
            link.l1.go = "Step_E12_9";

        break;
        
        case "Step_E12_9":
            dialog.text = "Дело в том, что Мэдифорд подлежит аресту. Однако сделать это так просто не удастся.\n"+
                          "Для находящегося в Порт Рояле английского гарнизона он остается губернатором Ямайки. По сути мы можем спровоцировать кровопролитие между англичанами, чего допускать я не намерен. К тому же гарнизон Порт Рояля очень силен.\n"+
                          "Насколько мне известно, вы имеете прямой доступ к Мэдифорду?";
            link.l1 = "Точно так, сэр?";
            link.l1.go = "Step_E12_10";
        break;

         case "Step_E12_10":
            dialog.text = "Ну что же, очень хорошо. Мне нужно чтобы вы, не создавая суеты, в сопровождении одного-двух человек посетили Мэдифорда в его резиденции. Обычное дело.\n"+
                          "А добравшись до Мэдифорда, нужно тихо и без лишнего шума произвести арест. Всеми необходимыми полномочиями я вас наделю.\n"+
                          "Понятно, что это чрезвычайно опасно - Мэдифорду палец в рот не клади.";
            link.l1 = "Это верно. А награда, сэр?";
            link.l1.go = "Step_E12_11";
        break;

         case "Step_E12_11":
            dialog.text = "Чего бы вы хотели?";
            link.l1 = "В последнее время я был довольно близок Мэдифорду, и я хочу снисходительного отношения ко мне новой англйиской власти на Карибах.";
            link.l1.go = "Step_E12_12";
        break;
        
         case "Step_E12_12":
            dialog.text = "Мне не в чем вас обвинять, вы действовали на основании каперского свидетельства. В случае успешного задержания Мэдифорда я гарантирую вам не снисходительное, а лояльное отношение английских властей к вашей персоне.";
			link.l1 = "Великолепно! И второе: я хочу получить половину от доходов, которые Мэдифорд незаконно нажил.";
            link.l1.go = "Step_E12_13";
        break;
        
        case "Step_E12_13":
            dialog.text = "Гм... А вы весьма хваткий человек, " + GetFullName(pchar) + ".";
            link.l1 = "Поверьте, я по праву претендую на эти деньги - мой вклад в эту копилку весьма и весьма солиден...";
            link.l1.go = "Step_E12_14";
        break;

        case "Step_E12_14":
            dialog.text = "Э-э-эх, черт возьми... Договорились! Половина от конфиската - ваша. Итак, я назначаю вас своим помощником и даю вам поручение чрезвыйчаной важности.\n"+
                          "Мне нужно, чтобы вы немедленно отправились в Порт Рояль и арестовали бывшего губернатора и государственного преступника Томаса Мэдифорда для предания суду в Лондоне.";
            link.l1 = "Будет исполнено, сэр.";
            link.l1.go = "exit";
            pchar.questTemp.State = "ColonelLinch_ArrestModiford";
            AddQuestRecord("Eng_Line_12_ColonelLinch", "2");
            LAi_SetPlayerType(pchar);
            LAi_SetStayType(npchar);
            NextDiag.TempNode = "YouMustGo";
        break;

        case "YouMustGo":
            dialog.text = "Вам следует как можно быстрей арестовать Мэдифорда. Прошу вас выполнить поставленную задачу.";
            link.l1 = "Хорошо, сэр.";
            link.l1.go = "exit";
            NextDiag.TempNode = "YouMustGo";
        break;
    }
}
