// диалоговый файл №1 на фр. линейку
void ProcessDialogEvent()
{
	ref NPChar, sld;
	aref Link, NextDiag;
	int i, iTemp;
    float locx, locy, locz;
	string sTemp, attrName;

	DeleteAttribute(&Dialog,"Links");

	makeref(NPChar,CharacterRef);
	makearef(Link, Dialog.Links);
	makearef(NextDiag, NPChar.Dialog);
	
    switch (Dialog.CurrentNode)
    {
        case "First time":
            dialog.text = "Если вы это видите, то это бага. Надо срочно сообщить Эдди!";
            link.l1 = "...";
            link.l1.go = "exit";

//>>>>>>>>>>>>========= Разброс диалогов по персонажам =====================
            //Квест №1, сопровождение Леграна.
            if (pchar.questTemp.State == "Fr1Legran_toTavern")// квест №1, Легран в таверне
            {
    			dialog.text = "Слушаю вас, что вы хотите?";
    			link.l1 = "Месье, насколько я знаю, вы - Пьер Легран.";
    			link.l1.go = "Step_F1_1";
            }
            if (pchar.questTemp.State == "Fr1Legran_afterFightBand")// квест №1, Легаран после боя.
            {
    			dialog.text = "Мне надо поторапливаться, нет времени...";
    			link.l1 = "Месье, я доставил вас до бухты, моя задача выполнена...";
    			link.l1.go = "Step_F1_3";
    			link.l2 = "А мы уже пришли! Я решил, что твои денежки мне самому пригодятся.";
    			link.l2.go = "Step_F1_5";
            } 
            //Квест №2, доставка письма на Тортугу
			if (pchar.questTemp.State == "Fr2Letter_SeekProblems")// квест №2, странные типы в таверне
            {
    			dialog.text = "...на Пальмовый берег.";
				link.l1 = "(про себя) так-так, Пальмовый берег...";
    			link.l1.go = "Step_F2_1";
            }
			if (pchar.questTemp.State == "Fr2Letter_SeekProblems" && pchar.location == "Shore22")// квест №2, странные типы в бухте
            {
    			dialog.text = "...наш галеон уже должен быть здесь.";
				link.l1 = "(про себя) так-так, галеон...";
    			link.l1.go = "Step_F2_5";
            }
			if (npchar.id == "DeLeiva")// квест №3, база с мужем донны Анны
            {
    			dialog.text = "Ну что же, вот мы и дождались нашего гостя. Добро пожаловать на ужин, месье. Главным блюдом для вас сегодня - добрая толледская сталь!";
				link.l1 = "Вот так встреча! А в чем дело?!";
    			link.l1.go = "Step_F3_1";
            }
			if (npchar.id == "AnnaDeLeiva" && pchar.questTemp.State == "Fr5AnnaHant_toAnna")// квест №5, донна Анна в резиденции Тортуги
            {
    			dialog.text = "Вас прислал сеньор д'Ожерон?";
    			link.l1 = "Да, мадам. Чем могу быть полезен?";
    			link.l1.go = "Step_F5_1";
            }
			if (npchar.id == "AnnaDeLeiva" && pchar.questTemp.State == "Fr5AnnaHant_toHavana")
            {
    			dialog.text = "Рассказывайте, сеньор, что вы узнали?";
    			link.l1 = "Пока ничего. Но я занимаюсь этим делом, не беспокойтесь, мадам...";
    			link.l1.go = "exit";
            }
			if (npchar.id == "AnnaDeLeiva" && pchar.questTemp.State == "Fr5AnnaHant_SeekHoseBrothers")
            {
    			dialog.text = "Рассказывайте, сеньор, что вы узнали?";
    			link.l1 = "Пока ничего. Но я занимаюсь этим делом, не беспокойтесь, мадам...";
    			link.l1.go = "exit";
            }
			if (npchar.id == "DeLeivaBrother_1" || npchar.id == "DeLeivaBrother_2")// квест №5, братья де Лейва
            {
    			dialog.text = "Что тебе здесь нужно?";
    			link.l1 = "Сеньоры, не вы ли родственники дона Хосе Рамиреса де Лейва?";
    			link.l1.go = "Step_F5_2";
            }
			if (npchar.id == "AnnaDeLeiva" && pchar.questTemp.State == "Fr5AnnaHant_GoodWork")// квест №5, донна Анна после выполнения задачи
            {
				dialog.text = NPCStringReactionRepeat("Сеньор, вы уже второй раз спасаете мою жизнь. Знайте, что в моем лице вы обрели верного друга.",
					         "Здравствуйте снова, капитан. Рада вас видеть.", "И еще раз здравствуйте капитан...", "Который раз на дню: здравствуйте, капитан...", "block", 0, npchar, Dialog.CurrentNode);
				link.l1 = HeroStringReactionRepeat("Сеньора, совершенно случайно близ Гаваны мне встретились два кастильских дворянина, и знали бы вы, как они о вас выражались... По всему видать, донна Анна, что память о вас бережно хранится в Кастилии.", 
					      "И я вас рад видеть так же.", "И еще раз я рад вас видеть...", "Здраствуйте, сеньора...", npchar, Dialog.CurrentNode);
				link.l1.go = DialogGoNodeRepeat("Step_F5_8", "none", "none", "none", npchar, Dialog.CurrentNode);

            }
			if (npchar.id == "AnnaDeLeiva" && pchar.questTemp.State == "empty")// квест №5, донна Анна после выполнения задачи
            {
				dialog.text = NPCStringReactionRepeat("Сеньор, вы уже второй раз спасаете мою жизнь. Знайте, что в моем лице вы обрели верного друга.",
					         "Здравствуйте снова, капитан. Рада вас видеть.", "И еще раз здравствуйте капитан...", "Который раз на дню: здравствуйте, капитан...", "block", 0, npchar, Dialog.CurrentNode);
				link.l1 = HeroStringReactionRepeat("Сеньора, совершенно случайно близ Гаваны мне встретились два кастильских дворянина, и знали бы вы, как они о вас выражались... По всему видать, донна Анна, что память о вас бережно хранится в Кастилии.", 
					      "И я вас рад видеть так же.", "И еще раз я рад вас видеть...", "Здраствуйте, сеньора...", npchar, Dialog.CurrentNode);
				link.l1.go = DialogGoNodeRepeat("Step_F5_8", "none", "none", "none", npchar, Dialog.CurrentNode);
            }
			if (npchar.id == "Olone" && pchar.questTemp.State == "Fr5Olone_toGuadeloupe")// квест №6, базар в доме Олоне
            {
				dialog.text = NPCStringReactionRepeat("Сударь, вас что, поучить хорошим манерам?! Ко мне в дом без приглашения не смеет входить никто!",
					         "Мы вроде бы все обсудили...", "Еще что-то?", "Опять то же самое, сколько можно?!", "block", 0, npchar, Dialog.CurrentNode);
				link.l1 = HeroStringReactionRepeat("Мое приглашение при мне. Вот оно - это срочный пакет от генерал-губернатора д'Ожерона.", 
					      "Да, точно...", "Да нет, все нормально...", "Да ладно тебе, не кипятись...", npchar, Dialog.CurrentNode);
				link.l1.go = DialogGoNodeRepeat("Step_F6_1", "none", "none", "none", npchar, Dialog.CurrentNode);
            }
			if (npchar.id == "Olone" && pchar.questTemp.State == "Fr5Olone_FoundMoney") //успели к деньгам
            {
    			dialog.text = "Ты нашел наши реалы?";
    			link.l1 = "Да, все в порядке. К счастью, мы успели вовремя - деньги у меня.";
    			link.l1.go = "Step_F6_14";
				LAi_LocationDisableOfficersGen("Cumana_town", false); //офицеров пускать
            }
			if (npchar.id == "Olone" && pchar.questTemp.State == "Fr5Olone_WeLate") //деньги ушли
            {
    			dialog.text = "Ты нашел наши реалы?";
    			link.l1 = "Нет, Франусуа...";
    			link.l1.go = "Step_F6_10";
				LAi_LocationDisableOfficersGen("Cumana_town", false); //офицеров пускать
            }
			if (npchar.id == "Bonrepo" && pchar.questTemp.State == "Fr8ThreeCorsairs_toBonrepo") //Квест №8. Маркиз Бонрепо в Бас-Тере
            {
    			dialog.text = "С кем имею честь?..";
    			link.l1 = "Капитан " + GetFullName(pchar) + ". Я прибыл к вам по распоряжению генерал-губернатора д`Ожерона.";
    			link.l1.go = "Step_F8_1";
            }
			if (npchar.id == "Bonrepo" && pchar.questTemp.State == "Fr8ThreeCorsairs_toSeek") 
            {
    			dialog.text = "Итак, как успешно продвигается ваша миссия?";
    			if (sti(pchar.questTemp.Count) == 3)
				{
					link.l1 = "Она выполнена. Все трое не будут участвовать в войне на стороне Англии.";
    				link.l1.go = "Step_F8_9";
				}
				else
				{
					link.l1 = "Пока мне не удалось встретиться со всеми, но я работаю над этим.";
    				link.l1.go = "exit";
				}
            }
			if (npchar.id == "Bonrepo" && pchar.questTemp.State == "Fr12EndWar_toBorepo")
            {
    			dialog.text = "А-а-а, барон! Рад вас видеть. Скажу больше - я ждал вас...";
    			link.l1 = "Рад вас видеть так же, маркиз.";
    			link.l1.go = "Step_F12_1";
            }
			if (npchar.id == "Bonrepo" && pchar.questTemp.State == "EndOfQuestLine")
            {
    			dialog.text = "Все, барон, я вас больше не задерживаю. Прощайте...";
    			link.l1 = "Прощайте, маркиз...";
    			link.l1.go = "exit";
            }
		break;
//<<<<<<<<<<<<===== Разброс диалогов по персонажам =====================
        case "Exit":
            NextDiag.CurrentNode = NextDiag.TempNode;
            DialogExit();
        break;

//********************************* Доставка Леграна до Мартиники. Квест №1 **********************************
 		case "Step_F1_1":
			dialog.text = "Да, это так.";
			link.l1 = "Меня зовут "+ GetFullName(pchar) +". Я прислан к вам генерал-губернатором д'Ожероном. Имею поручение тихо и незаметно доставить вас до Мартиники в бухту Ле Марен.";
			link.l1.go = "Step_F1_2";
		break;
 		case "Step_F1_2":
			dialog.text = "Если вам доверяет генерал-губернатор, то доверяю и я. Когда мы отправляемся?";
			link.l1 = "Как можно скорее. Ждите меня на моем корабле.";
			link.l1.go = "Step_F1_exit";
		break;
 		case "Step_F1_exit":
			locations[FindLocation("Shore39")].DisableEncounters = true; //энкаунтеры закроем
    		pchar.questTemp.State = "Fr1Legran_toLeMaren";
            AddQuestRecord("Fra_Line_1_LegranDis", "2");
			LAi_SetStayType(npchar);
            GetCharacterPos(pchar, &locx, &locy, &locz);
         	ChangeCharacterAddressGroup(npchar, "Tortuga_tavern", "tables", LAi_FindNearestFreeLocator("tables", locx, locy, locz));
			LAi_SetActorType(npchar);
			LAi_ActorGoToLocation(npchar, "reload", "reload1_back", "none", "", "", "", 8);
			Pchar.quest.Fr1Legran_intoLeMaren.win_condition.l1 = "location";
            Pchar.quest.Fr1Legran_intoLeMaren.win_condition.l1.location = "Shore39";
            Pchar.quest.Fr1Legran_intoLeMaren.win_condition = "Fr1Legran_intoLeMaren";
            AddPassenger(pchar, npchar, false);
            SetCharacterRemovable(npchar, false);
            DialogExit();
		break;
 		case "Step_F1_3":
			dialog.text = "Да, да, бесспорно, вы полностью выполнили взятые на себя обязательства. Позвольте отблагодарить вас должным образом: сумма в 20000 золотых смотрится, полагаю, весьма достойно?";
			link.l1 = "Вполне...";
			link.l1.go = "Step_F1_4";
		break;
 		case "Step_F1_4":
			dialog.text = "Эти деньги ваши, извольте получить.\n"+
				          "Прощайте и будте здоровы, месье.";
			link.l1 = "Прощайте, сударь. Я желаю вам удачной дороги домой, во Францию... Прощайте!";
			link.l1.go = "Step_F1_4_exit";
		break;
		case "Step_F1_4_exit":
			pchar.questTemp.State = "Fr1Legran_backGovernor";
			AddQuestRecord("Fra_Line_1_LegranDis", "3");
			AddMoneyToCharacter(pchar, 20000);
			LAi_SetPlayerType(pchar);
            LAi_SetActorType(npchar);
            LAi_ActorRunToLocation(NPChar, "reload", "reload1", "none", "", "", "CanFightCurLocation", 10.0);
			LAi_LocationFightDisable(&Locations[FindLocation("Shore39")], true);
			NPChar.LifeDay = 0;
			RemovePassenger(pchar, npchar);
            DialogExit();
		break;
 		case "Step_F1_5":
			dialog.text = "Ах ты, мерзавец! Я сообщу генерал-губернатору, какой негодяй втерся к нему в доверие!";
			link.l1 = "Сначала останься в живых, тюфяк!";
			link.l1.go = "Step_F1_6";
		break;
 		case "Step_F1_6":
			LAi_SetPlayerType(pchar);
            LAi_SetActorType(npchar);
			AddQuestRecord("Fra_Line_1_LegranDis", "4");
			pchar.questTemp.State = "Fr1Legran_LegranKilled";
			LAi_group_MoveCharacter(npchar, "EnemyFight");
            LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
            LAi_group_FightGroups("EnemyFight", LAI_GROUP_PLAYER, true);
            LAi_group_SetCheck("EnemyFight", "OpenTheDoors");
            DialogExit();
			AddDialogExitQuest("MainHeroFightModeOn");
		break;
//********************************* Арест ГГ у Стэвезанта. Квест №2 **********************************
 		case "Step_F2_1":
			dialog.text = "Эй, тебе чего здесь надо, чего встал? Мы тебя выпить не приглашали.";
			link.l1 = "Меня зовут капитан " + GetFullName(pchar) + ", я из только что с Тортуги... Не желаете ко мне в команду?";
    		link.l1.go = "Step_F2_2";
		break;
 		case "Step_F2_2":
			dialog.text = "Нет, не желаем!";
			link.l1 = "У меня хорошие условия, подумайте... Может выпьем? Давайте-ка я к вам подсяду...";
			link.l1.go = "Step_F2_3";
		break;
 		case "Step_F2_3":
			dialog.text = "Садись и выпивай, капитан. Ну а нам пора.";
			link.l1 = "А-а-а, ну ладно...";
			link.l1.go = "Step_F2_4";
		break;
 		case "Step_F2_4":
			chrDisableReloadToLocation = true; 
            GetCharacterPos(pchar, &locx, &locy, &locz);
            for (i=1; i<=2; i++)
            {
				sld = characterFromId("Bandit_"+i);
				LAi_SetStayType(sld);
				if (i==2) ChangeCharacterAddressGroup(sld, "Villemstad_tavern", "tables", "stay2");
				else ChangeCharacterAddressGroup(sld, "Villemstad_tavern", "goto", "goto3");
				LAi_SetActorType(sld);
				LAi_ActorRunToLocation(sld, "reload", "reload1_back", "none", "", "", "Fr2Letter_OutTavern_1", 10);
			}
			Pchar.quest.Fr2Letter_OutTavern_2.win_condition.l1 = "location";
            Pchar.quest.Fr2Letter_OutTavern_2.win_condition.l1.location = "Villemstad_town";
            Pchar.quest.Fr2Letter_OutTavern_2.win_condition = "Fr2Letter_OutTavern_2";
			locations[FindLocation("Shore22")].DisableEncounters = true; //энкаутеры уберем временно
			LAi_SetPlayerType(pchar);
            DialogExit();
		break;

 		case "Step_F2_5":
			dialog.text = "Опять ты? И что это ты делаешь здесь?";
			link.l1 = "Хочу узнать кое-что. Вы тут говорили про какой-то ваш галеон...";
    		link.l1.go = "Step_F2_6";
		break;
 		case "Step_F2_6":
			dialog.text = "У тебя острый слух. Хорошие уши...";
			link.l1 = "Я жду ответа!";
    		link.l1.go = "Step_F2_7";
		break;
 		case "Step_F2_7":
			dialog.text = "А кто ты такой, чтобы требовать ответа?";
			link.l1 = "Я уж представлялся, если мне не изменяет память. Или вам неизвестно, что такое Тортуга и кто такой флибустьер?!";
    		link.l1.go = "Step_F2_8";
		break;
 		case "Step_F2_8":
			dialog.text = "Кто такой флибустьер - нам известно. Мы называем вас ладронами.";
			link.l1 = "Так нас называют только испанцы...";
    		link.l1.go = "Step_F2_9";
		break;
 		case "Step_F2_9":
			dialog.text = "Верно... Амиго, святой Антоний предоставил нам возможность причастить капитана ладронов - благо он один. Не будем терять времени...";
			link.l1 = "Вот те на, испанцы! А что за ваш галеон, вы не ответили!";
    		link.l1.go = "Step_F2_10";
		break;
 		case "Step_F2_10":
			dialog.text = "Мы ждем наш галеон здесь, он вот-вот должен бросить якорь. Теперь ты знаешь все, но знание это тебе не пригодится...";
			link.l1 = "Ну, это как сказать. Пожалуй, пора посмотреть на цвет кастильской крови...";
    		link.l1.go = "Step_F2_11";
		break;
 		case "Step_F2_11":
			DeleteAttribute(&locations[FindLocation("Shore22")], "DisableEncounters"); //энкаутеры вернем
			chrDisableReloadToLocation = true;
			Pchar.quest.Fr2Letter_FightWithGaleon.win_condition.l1 = "location";
			Pchar.quest.Fr2Letter_FightWithGaleon.win_condition.l1.location = "Curacao";
			Pchar.quest.Fr2Letter_FightWithGaleon.win_condition = "Fr2Letter_FightWithGaleon";
			AddQuestRecord("Fra_Line_2_DelivLetter", "4");
            for (i=1; i<=2; i++)
            {
				sld = characterFromId("Bandit_"+i);
				LAi_SetWarriorType(sld);
				LAi_group_MoveCharacter(sld, "EnemyFight");
				LAi_RemoveCheckMinHP(sld);
				LAi_SetHP(sld, 200, 200);
			}
			LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
			LAi_group_FightGroups("EnemyFight", LAI_GROUP_PLAYER, true);
			LAi_group_SetCheck("EnemyFight", "OpenTheDoors");
			LAi_SetPlayerType(pchar);
			DialogExit();
			AddDialogExitQuest("MainHeroFightModeOn");
		break;
//********************************* Драка с мужем де Лейва. Квест №3 **********************************
 		case "Step_F3_1":
			dialog.text = "Нет, ну как вам нравится... В чужой дом посреди ночи вламывается мужчина, а потом спрашивает: 'А в чем дело?!'\n"+
				          "Не стройте из себя идиота! Моя жена заперта, я нашел у нее это чертово кольцо д’Ожерона.";
			link.l1 = "Все ясно... Ну что же, приложу все силы для того, чтобы угостить вас блюдом, которое вы уготовили мне сегодня.";
    		link.l1.go = "Step_F3_2";
		break;
 		case "Step_F3_2":
			dialog.text = "Начнем трапезу!";
			link.l1 = "Начнем, пожалуй...";
    		link.l1.go = "Step_F3_3";
		break;
 		case "Step_F3_3":
			LAi_LocationFightDisable(loadedLocation, false); 
			SetNationRelation2MainCharacter(SPAIN, RELATION_ENEMY);
			LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
			LAi_group_FightGroups("EnemyFight", LAI_GROUP_PLAYER, true);
			LAi_group_SetCheck("EnemyFight", "OpenTheDoors");
			LAi_SetPlayerType(pchar);
			DialogExit();
			AddDialogExitQuest("MainHeroFightModeOn");
		break;
//********************************* Месть Анне де Лейва. Квест №5 **********************************
 		case "Step_F5_1":
			dialog.text = "Я получила письмо из Гаваны от моей подруги Инес де лас Сьеррас. генерал-губернатор Гаваны предпринял расследование смерти моего мужа и убежден, что в этом повинна я. Ах, так оно и есть! Но мои соотечественники замыслили месть, мне грозит гибель или похищение, и я не знаю, что страшнее.\n"+
				          "Сударь, я прошу вас отправиться в Гавану и выяснить, насколько это серьезно.";
			link.l1 = "Мадам, вы можете рассчитывать на меня.";
    		link.l1.go = "exit";
    		pchar.questTemp.State = "Fr5AnnaHant_toHavana";
			AddQuestRecord("Fra_Line_5_KillAnnaHanters", "2");			
			sld = GetCharacter(NPC_GenerateCharacter("InesDeLasCierras", "girl_8", "woman", "towngirl", 10, SPAIN, -1, false));
			sld.Dialog.Filename = "Quest\FraLineNpc_2.c";
			sld.name = "Инес";
			sld.lastname = "де Лас Сьеррас";
			LAi_SetStayType(sld);
			ChangeCharacterAddressGroup(sld, "Havana_houseSp2", "goto","goto2");
		break;
 		case "Step_F5_2":
			dialog.text = "Какое тебе до этого дело?";
			link.l1 = "Не было бы дела, не спрашивал, поверьте уж...";
    		link.l1.go = "Step_F5_3";
		break;
 		case "Step_F5_3":
			dialog.text = "Возможно. И что тебе нужно от нас?";
			link.l1 = "Я так понял, вы замыслили отомстить за дона Хосе?";
    		link.l1.go = "Step_F5_4";
		break;
 		case "Step_F5_4":
			dialog.text = "Немедленно говори, кто ты. Иначе я отрежу тебе твой длинный язык.";
			link.l1 = "Я тот, кто отправил уважаемого дона Хосе на тот свет. Еще и слуг его отправил туда же вслед за хозяином.";
    		link.l1.go = "Step_F5_5";
		break;
 		case "Step_F5_5":
			dialog.text = "Аргх, удача сегодня с нами! Пол дела сделано, брат, осталось найти эту бестию Анну и мы можем возвращаться домой.";
			link.l1 = "А как же я?!";
    		link.l1.go = "Step_F5_6";
		break;
 		case "Step_F5_6":
			dialog.text = "А тебя уже нет... Вперед брат, покончим с этим мерзавцем!";
			link.l1 = "Я с такой трактовкой моего бытия не согласен. Я есть! Ну, давайте начнем...";
    		link.l1.go = "Step_F5_7";
		break;
 		case "Step_F5_7":
			LAi_LocationDisableOfficersGen("Mayak10", false); //офицеров пускать
			pchar.questTemp.State = "Fr5AnnaHant_GoodWork";
			LAi_LocationFightDisable(&Locations[FindLocation("Mayak10")], false); 			
			LAi_group_Delete("EnemyFight");
			for (i=1; i<=2; i++)
            {				
				LAi_group_MoveCharacter(characterFromId("DeLeivaBrother_"+i), "EnemyFight");
			}
			LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
			LAi_group_FightGroups("EnemyFight", LAI_GROUP_PLAYER, true);
			LAi_group_SetCheck("EnemyFight", "OpenTheDoors");
			DialogExit();
		break;
 		case "Step_F5_8":
			dialog.text = "От такого рода почитания я бы с удовольствием отказалась.";
			link.l1 = "Вам ничего более не грозит, поверьте мне. Я сумел переубедить этих храбрых кастильцев...";
    		link.l1.go = "Step_F5_9";
		break;
 		case "Step_F5_9":
			dialog.text = "Ах, сеньор " + pchar.name + ", как я могу отблагодарить вас?";
			link.l1 = "Донна Анна, я прошу вас заботиться о месье д’Ожероне, он того заслуживает.";
    		link.l1.go = "Step_F5_10";
		break;
 		case "Step_F5_10":
			dialog.text = "Непременно исполню вашу просьбу, о душевном здоровье Бертрана вы можете более не переживать.";
			link.l1 = "Мадам, вы оказали всем флибустьерам огромную услугу. Полагаю, мы в расчете.";
    		link.l1.go = "exit";
			npchar.LifeDay = 0;
		break;
//********************************* Франсуа Олоне. Квест №6 **********************************
 		case "Step_F6_1":
			dialog.text = "А-a-a! Ну, это меняет дело. Располагайтесь, пока я прочту письмо.";
			link.l1 = "Между прочим, месье, хочу сообщить вам, что этим письмом крайне интересовались испанцы. Они даже отрядили военный корабль, чтобы перехватить мой корабль.";
    		link.l1.go = "Step_F6_2";
            TakeItemFromCharacter(pchar, "letter_1");
            BackItemDescribe("letter_1");
		break;
 		case "Step_F6_2":
			dialog.text = "Полагаю, раз вы здесь, значит, испанцы не скоро дождутся своего военного корабля, ха-ха! Да, они спят и видят, как бы доказать связи французского генерал-губернатора с флибустьерами. Так что, доставив письмо в целости, вы оказали и мне, и месье д`Ожерону немалую услугу. Награда будет соответствующей. Кстати, как вы предпочитаете ее получить?";
			link.l1 = "В каком смысле?";
    		link.l1.go = "Step_F6_3";
		break;
 		case "Step_F6_3":
			dialog.text = "Я могу вручить вам мешочек с золотом. А могу предложить участие в интересном дельце. В этом письме генерал-губернатор сообщает, что в форте Куманы сейчас находится золото на сумму не меньше двухсот тысяч реалов. В моей флотилии три корабля, их капитаны - мои компаньоны. Присоединяйтесь – и добычу, согласно законам братства, поделим на четверых. Ну, как?";
			link.l1 = "Я предпочту первый вариант.";
    		link.l1.go = "Step_F6_WantMoney";
			link.l2 = "Какой же корсар откажется от жаркого, но выгодного дела! Разумеется, я с вами!";
    		link.l2.go = "Step_F6_4";
			SaveCurrentQuestDateParam("questTemp");
		break;
 		case "Step_F6_WantMoney":
			dialog.text = "Как скажете. Вот ваша награда.";
			link.l1 = "Спасибо, Франсуа. Ну а теперь, я, пожалуй, отбуду на Тортугу...";
    		link.l1.go = "exit";
			NextDiag.TempNode = "First time";
			AddMoneyToCharacter(pchar, 10000);
			pchar.questTemp.State = "Fr6Olone_TakeMoney";
			AddQuestRecord("Fra_Line_6_Olone", "2");
		break;
 		case "Step_F6_4":
            if (GetCompanionQuantity(pchar) == 1)   // Заставляем геймера избавиться от взятых кубиков.
            {
				if (GetQuestPastDayParam("questTemp") < 6)
				{
					dialog.text = "Ну что же, ты готов к тому, чтобы занять место в нашей эскадре. Добро пожаловать и вперед к Кумане!";
					link.l1 = "Очень хорошо! Франсуа, каков план действий на эту компанию?";
    				link.l1.go = "Step_F6_5";
				}
				else
				{
					dialog.text = "Ну, и где ты пропадал столько времени?";
					link.l1 = "Да вот, избавлялся от кораблей, как ты и сказал.";
    				link.l1.go = "Step_F6_7";
				}
            }
            else
            {
				dialog.text = "Коллега, я тебя одного могу взять в это на условиях, что твоя доля - четвертая часть. Больше мне компаньоны не требуются. Поэтому избавься от лишних кораблей, если хочешь участвовать. И побыстрей!";
				link.l1 = "Хорошо, Франсуа, я скоро вернусь.";
    			link.l1.go = "exit";
				link.l2 = "Не, на такой расклад я не согласен. Поэтому я, пожалуй, лучше возьму таки свои деньги... ";
    			link.l2.go = "Step_F6_WantMoney";
				NextDiag.TempNode = "Step_F6_4";
             }
		break;
 		case "Step_F6_5":
			dialog.text = "План достаточно прост. Мы приходим туда и берем Куману штурмом. Затем находим золото, делим и разбегаемся каждый по своим делам.";
			link.l1 = "Превосходно. Ну что же, я готов.";
    		link.l1.go = "Step_F6_6";
		break;
 		case "Step_F6_6":
			dialog.text = "Тогда вперед на Куману, и как можно быстрей!";
			link.l1 = "Очень хорошо, начинаем...";
    		link.l1.go = "Step_F6_GoOn";
		break;
		case "Step_F6_GoOn": 
			//==> формируем эскадру
            LAi_SetActorType(NPChar);
            LAi_ActorSetStayMode(NPChar);
			CharacterIntoCompanionAndGoOut(pchar, npchar, "reload", "reload1", 5, false);
			for (i=1; i<=2; i++)
			{
                sld = GetCharacter(NPC_GenerateCharacter("Captain_"+i, "officer_"+(rand(9)+1), "man", "man", 20, FRANCE, -1, true));
                FantomMakeCoolSailor(sld, SHIP_CORVETTE, "", CANNON_TYPE_CANNON_LBS24, 70, 80, 40);
                FantomMakeCoolFighter(sld, 20, 90, 70, BLADE_LONG, "pistol4", 120);
				sld.Dialog.Filename = "Quest\FraLineNpc_2.c";
				sld.CompanionEnemyEnable = false; 
				sld.Abordage.Enable = false; // НЕЛЬЗЯ!
				sld.RebirthPhantom = true; 
                SetCompanionIndex(pchar, -1, sti(sld.index));
				sld.DeckDialogNode = "HaveHotBussines";
				SetCharacterRemovable(sld, false);
				LAi_LoginInCaptureTown(sld, true);
			}
            // ==> текущая дата, у генерал-губернатора Куманы потом сверим
            SaveCurrentQuestDateParam("questTemp");            
            pchar.TempPerks.QuestTroopers = true; //перк квестового десанта
            // ==> капитулянтская нода губернатору Куманы
			characters[GetCharacterIndex("Cumana_Mayor")].dialog.captureNode = "FraLine6Quest_Attack"; 
			npchar.DeckDialogNode = "HaveHotBussines";
			AddQuestRecord("Fra_Line_6_Olone", "3");
			DialogExit();
		break;
		case "Step_F6_7":
			dialog.text = "Избавился?";
			link.l1 = "Да, конечно.";
    		link.l1.go = "Step_F6_8";
		break;
 		case "Step_F6_8":
			dialog.text = "Я тебя поздравляю с успешным избавлением, но похода на Куману не будет.";
			link.l1 = "Как не будет?!";
    		link.l1.go = "Step_F6_9";
		break;
 		case "Step_F6_9":
			dialog.text = "Очень просто, мон шер. Золота в Кумане уже нет, его вывезли. Так что в нашем случае потерянное время стоило нам 200 тысяч реалов. Вот так-то...";
			link.l1 = "Черт возьми, досадно... Ну что же, тогда я заберу те деньги, которые мне причитаются за доставку письма.";
    		link.l1.go = "Step_F6_WantMoney";
		break;
 		case "HaveHotBussines":
			dialog.text = "Капитан, нам нельзя терять времени, нужно срочно брать Куману пока золото не испарилось.";
			link.l1 = "Да, Франсуа, мы обязательно успеем...";
    		link.l1.go = "exit";
		break;
 		case "Step_F6_10":
			dialog.text = "Что-то мне не нравится твой ответ, " + pchar.name + ".";
			link.l1 = "Мы опоздали, Франсуа. Дело в том, что деньги уже ушли в Старый Свет с неделю тому назад. Все было бесполезно...";
    		link.l1.go = "Step_F6_11";
		break;
 		case "Step_F6_11":
			dialog.text = "Я так не думаю, мон шер. А думаю я то, что это из-за тебя не успели вовремя!";
			link.l1 = "Спорное утверждение, друг Олоне. Очень спорное...";
    		link.l1.go = "Step_F6_12";
		break;
 		case "Step_F6_12":
			dialog.text = "Полагаю, пришло время разрешить все споры. К оружию, сударь! Попробуем твою кровь на вкус...";
			link.l1 = "Видит Бог, я не хочу с тобой драться!";
    		link.l1.go = "Step_F6_13";
		break;
 		case "Step_F6_13":
			dialog.text = "Тогда я убью тебя как трусливого пса! Последний шанс - берись за оружие.";
			link.l1 = "Ну хорошо, Франсуа или как там тебя... Ты сам напросился.";
    		link.l1.go = "Step_F6_fight";
			LAi_group_Delete("EnemyFight");
			AddQuestRecord("Fra_Line_6_Olone", "4");
		break;
 		case "Step_F6_fight":			
			NPChar.LifeDay = 0;
			DeleteAttribute(NPChar, "RebirthPhantom");
			for (i=1; i<=2; i++)
			{
				sld = characterFromId("Captain_"+i);
				sld.LifeDay = 0;
				DeleteAttribute(sld, "RebirthPhantom");
			}
			chrDisableReloadToLocation = true;
			pchar.questTemp.State = "Fr6Olone_OloneIsDead";
			LAi_group_MoveCharacter(npchar, "EnemyFight");
			LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
			LAi_group_FightGroups("EnemyFight", LAI_GROUP_PLAYER, true);
			LAi_group_SetCheck("EnemyFight", "OpenTheDoors");
			DialogExit();
			AddDialogExitQuest("MainHeroFightModeOn");
		break;
 		case "Step_F6_14":
			if (sti(pchar.money) >= 200000)
			{
				dialog.text = "Отлично, компаньон! Делим деньги - и по домам.";
				link.l1 = "Прощай, Олоне. Я похозяйничаю еще немного в городе...";
    			link.l1.go = "Step_F6_15";
				link.l2 = "А я вот думаю, что не обязан с тобой делиться. Все работу сделал я!!";
    			link.l2.go = "Step_F6_20";
			}
			else
			{
				dialog.text = "Деньги ты нашел, а у тебя их нет в наличии. Хитрить вздумал?!";
				link.l1 = "Да ты что, Франусуа?!";
    			link.l1.go = "Step_F6_17";			
			}
		break;
 		case "Step_F6_15":
			dialog.text = "Похозяйничай, но только надолго не задерживайся - рядом могут быть испанцы.";
			link.l1 = "Хорошо, Франусуа. Будь здоров...";
    		link.l1.go = "Step_F6_16";
			AddMoneyToCharacter(pchar, -150000);
		break;
 		case "Step_F6_16":
			chrDisableReloadToLocation = true;
			NPChar.LifeDay = 0;
			DeleteAttribute(NPChar, "RebirthPhantom");
			for (i=1; i<=2; i++)
			{
				iTemp = GetCharacterIndex("Captain_"+i)
				if (iTemp > 0)
				{				
					sld = &characters[iTemp];
					sld.LifeDay = 0;
					DeleteAttribute(sld, "RebirthPhantom");
				}
			}
			AddQuestRecord("Fra_Line_6_Olone", "5");
			pchar.questTemp.State = "Fr6Olone_OloneIsLive";
            LAi_SetActorType(npchar);
            LAi_ActorRunToLocation(NPChar, "reload", "reload1_back", "none", "", "", "CanFightCurLocation", -1);
			LAi_LocationFightDisable(&Locations[FindLocation("Cumana_town")], true);
            DialogExit();
		break;
 		case "Step_F6_17":
			dialog.text = "А я так думаю, хитришь, приятель... Но есть способ это выяснить - доставай свой клинок.";
			link.l1 = "Схватка?";
    		link.l1.go = "Step_F6_18";
		break;
 		case "Step_F6_18":			
			dialog.text = "Именно так. Крысу надо наказать...";
			link.l1 = "Ну ладно, ты сам напросился.";
    		link.l1.go = "Step_F6_fight";
			LAi_group_Delete("EnemyFight");
			AddQuestRecord("Fra_Line_6_Olone", "6");
			LAi_LocationFightDisable(&Locations[FindLocation("Cumana_town")], false);
			//==> кэпов в подмогу Олоне, пенальти за попытку сшустрить.
			for (i=1; i<=2; i++)
			{
				iTemp = GetCharacterIndex("Captain_"+i)
				if (iTemp > 0)
				{				
					sld = &characters[iTemp];
					LAi_SetWarriorType(sld);
					ChangeCharacterAddress(sld, "Cumana_town", LAi_FindRandomLocator("goto"));	
					LAi_group_MoveCharacter(sld, "EnemyFight");
				}
			}
		break;
 		case "Step_F6_20":
			dialog.text = "Вот значит как?! Ну тогда готовься к худжему...";
			link.l1 = "Всегда готов, Франсуа...";
    		link.l1.go = "Step_F6_21";
		break;
 		case "Step_F6_21":
			NPChar.LifeDay = 0;
			LAi_group_Delete("EnemyFight");
			AddQuestRecord("Fra_Line_6_Olone", "8");
			LAi_LocationFightDisable(&Locations[FindLocation("Cumana_town")], false);
			//==> кэпов в подмогу Олоне, пенальти за попытку сшустрить.
			for (i=1; i<=2; i++)
			{
				iTemp = GetCharacterIndex("Captain_"+i)
				if (iTemp > 0)
				{				
					sld = &characters[iTemp];
					sld.LifeDay = 0;
					DeleteAttribute(sld, "RebirthPhantom");
					LAi_SetWarriorType(sld);
					ChangeCharacterAddress(sld, "Cumana_town", LAi_FindRandomLocator("goto"));	
					LAi_group_MoveCharacter(sld, "EnemyFight");
				}
			}
			chrDisableReloadToLocation = true;
			pchar.questTemp.State = "Fr6Olone_OloneIsDead";
			LAi_group_MoveCharacter(npchar, "EnemyFight");
			LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
			LAi_group_FightGroups("EnemyFight", LAI_GROUP_PLAYER, true);
			LAi_group_SetCheck("EnemyFight", "OpenTheDoors");
			DialogExit();
			AddDialogExitQuest("MainHeroFightModeOn");
		break;
//********************************* Маркиз Бонрепо. Квест №8 **********************************
 		case "Step_F8_1":
			dialog.text = "Ага, наконец-то! А я уже начал волноваться. генерал-губернатор д'Ожерон сообщил мне о вас много лестного. Я надеюсь, что вы полностью оправдаете его и мое доверие. И учтите: все, что вы здесь услышите, никогда не должно стать известным кому-либо, кроме вас.";
			link.l1 = "Я предупрежден об этом...";
    		link.l1.go = "Step_F8_2";
		break;
 		case "Step_F8_2":
			dialog.text = "Великолепно! Итак, к делу.\n"+
				          "Сейчас в европейской политике очень сложное положение. Назревает очередная торговая война между Англией и Голландией, а наш великий Король-Солнце Людовик XIV начинает деволюционную войну с Испанией.";
			link.l1 = "Простите, маркиз... Какую войну? Революционную?!";
    		link.l1.go = "Step_F8_3";
		break;
 		case "Step_F8_3":
			dialog.text = "Гм... Ну да, откуда же знать простому каперу.\n"+
				          "Деволюционная война - это война за наследственное право на некоторые территории. Такой для Франции, несомненно, является часть Фландрии и Бургундия, находящиеся сейчас под испанским владычеством. Это благое дело, поскольку голландцы вот уже сотню лет стремятся сбросить иго испанских дьяволов, рядящихся в церковные рясы.\n"+
						  "Для успешного ведения войны мы в крайней степени нуждаемся в деньгах голландцев, так что необходимо помешать нападению англичан на их колонии. Задание контролирует лично Жан-Батист Кольбер, надеюсь, вы знаете, кто это...";
			link.l1 = "Каким образом я должен это сделать?";
    		link.l1.go = "Step_F8_4";
		break;
 		case "Step_F8_4":
			dialog.text = "Голландцы в состоянии постоять за себя, если генерал-губернатор Ямайки сэр Томас Мэдифорд не получит подкреплений. А в этих водах они могут рассчитывать только на помощь известных корсаров, имеющих вес и авторитет в Береговом братстве.\n"+
				          "Таких, кто мог бы серьезно повлиять на ход войны, всего трое: Генри Морган на Ямайке, Джекмен - в пиратском поселении на Бермудах, Джон Моррис на Тринидаде. Ваша задача – встретиться с каждым из них и убедить ни в коем случае не нападать на голландские колонии и суда. Для этой братии главное - выгода, так что средств не жалейте!";
			link.l1 = "И где же я должен взять эти средства?";
    		link.l1.go = "Step_F8_5";
		break;
 		case "Step_F8_5":
			dialog.text = "Признаюсь, вы меня удивили. Вас рекомендовали мне, как на редкость умного и находчивого человека! Если бы эти средства у меня были - зачем, спрашивается, понадобилась бы ваша помощь? Впрочем, вы вправе отказаться, но...";
			link.l1 = "Но что тогда?";
    		link.l1.go = "Step_F8_6";
		break;
 		case "Step_F8_6":
			dialog.text = "Тогда мне придется отправить вас в тюрьму. Вы ведь, милейший, стали обладателем государственной тайны! С такими знаниями нелояльные люди на свободе не разгуливают.";
			link.l1 = "Сначала надо меня задержать, а это не так-то просто!";
    		link.l1.go = "Step_F8_Fight";
			link.l2 = "Ну что ж, если король не может обойтись без моих пиастров, так и быть, я помогу ему!";
    		link.l2.go = "Step_F8_7";
		break;
 		case "Step_F8_Fight":
			CloseQuestHeader("Fra_Line_8_ThreeCorsairs");
			SetNationRelation2MainCharacter(FRANCE, RELATION_ENEMY);
            for (i=1; i<=4; i++)
            {
                sld = GetCharacter(NPC_GenerateCharacter("Guard_"+i, "Sold_fra_"+(rand(7)+1), "man", "man", 30, FRANCE, 0, true));
				FantomMakeCoolFighter(sld, 30, 65, 50, BLADE_LONG, "pistol3", 40);
            	LAi_SetWarriorType(sld);
                LAi_group_MoveCharacter(sld, "FRANCE_CITIZENS");
               	ChangeCharacterAddressGroup(sld, pchar.location, "reload", "reload1");
            }
			LAi_group_AttackGroup("FRANCE_CITIZENS", LAI_GROUP_PLAYER);
            LAi_SetActorType(npchar);
            LAi_ActorRunToLocation(NPChar, "reload", "reload1", "none", "", "", "CanFightCurLocation", 10.0);
			NPChar.LifeDay = 0;
			pchar.questTemp.State = "QuestLineBreake";
			AddQuestRecord("Fra_Line_8_ThreeCorsairs", "2");
			DialogExit();
			AddDialogExitQuest("MainHeroFightModeOn");
		break;
 		case "Step_F8_7":
			dialog.text = "Вот прекрасно! Д'Ожерон в вас не ошибался. Итак, я жду вас с хорошими новостями.";
			link.l1 = "Я все сделаю, маркиз.";
    		link.l1.go = "exit";
			AddQuestRecord("Fra_Line_8_ThreeCorsairs", "3");
			pchar.questTemp.State = "Fr8ThreeCorsairs_toSeek";
			SaveCurrentQuestDateParam("questTemp");
            // ==>  Помещаем Моргана на Антигуа и даем ноду доступа к квестам
            ChangeCharacterAddressGroup(&characters[GetCharacterIndex("Henry Morgan")], "SentJons_HouseF3", "goto", "goto1");
            QuestSetCurrentNode("Henry Morgan", "FraLine8_talk");
            sld = characterFromID("Henry Morgan");
            LAi_SetHuberStayType(sld);
			LAi_group_MoveCharacter(sld, "PeaceGroup");
            LAi_SetLoginTime(sld, 0.0, 24.0);
            // ==>  Генерим и помещаем Джона Морриса в Бриджтаун и даем ноду доступа к квестам
            sld = GetCharacter(NPC_GenerateCharacter("John Morris", "bocman", "man", "man", 38, PIRATE, -1, false));
        	sld.name 	= "Джон";
        	sld.lastname = "Моррис";
            sld.Dialog.Filename = "Quest\JohnMorris.c"; 
           	LAi_SetHuberType(sld);
			LAi_group_MoveCharacter(sld, "PeaceGroup");
        	LAi_SetLoginTime(sld, 0.0, 24.0);
            ChangeCharacterAddressGroup(sld, "PortSpein_houseF2", "sit", "sit1");
            LAi_LocationFightDisable(&Locations[FindLocation("PortSpein_houseF2")], true);
            // ==>  Даем ноду доступа к квестам Джону Моррису
            QuestSetCurrentNode("John Morris", "FraLine8_talk");
            // ==>  Даем ноду доступа к квестам Джекмену
            QuestSetCurrentNode("Jackman", "FraLine8_talk");
            // ==>  Оформляем счетчик посещения корсаров, в итоге должен == 3
            pchar.questTemp.Count = "0";
		break;
 		case "Step_F8_9":
			dialog.text = "Должен заметить, что это было одно из самых непростых дипломатических поручений, которые я давал кому-либо. Человеку, который так блестяще справляется с подобными делами, просто не пристало ходить в простолюдинах. По счастью, мне даны полномочия пожаловать вам баронский титул! Такие люди, как вы, нужны Франции!";
			link.l1 = "Э-э... а... Баронский титул? А как быть с тратами, которые я понес?..";
    		link.l1.go = "Step_F8_10";
		break;
 		case "Step_F8_10":
			dialog.text = "Ох, пират всегда останется пиратом! Ха-ха!\n"+
				          "Поверьте, милейший, я даю вам гораздо больше, чем презренный металл! Я даю вам честь и уважение самого короля! А сейчас не смею вас задерживать, вас ждет генерал-губернатор д'Ожерон. Кажется, у него для вас опять какое-то важное дело... Прощайте.";
			link.l1 = "Прощайте, интендант Бонрепо...";
    		link.l1.go = "Step_F8_11";
		break;
 		case "Step_F8_11":
			pchar.questTemp.State = "Fr8ThreeCorsairs_backGovernor";
			AddQuestRecord("Fra_Line_8_ThreeCorsairs", "8");
            LAi_SetActorType(npchar);
            LAi_ActorGoToLocation(NPChar, "reload", "reload1", "none", "", "", "", 6.0);
			pchar.name = "барон " + pchar.name;
            DialogExit();
		break;
//********************************* Конец войне. Квест №12 **********************************
 		case "Step_F12_1":
			dialog.text = "Итак, как вы уже знаете, деволюционная война с Испанией закончена. Испания повержена!";
			link.l1 = "Да, маркиз, превосходная новость.";
    		link.l1.go = "Step_F12_2";
		break;
 		case "Step_F12_2":
			dialog.text = "Ваши заслуги оценены по достоинству - о вас теперь знает сам Людовик...";
			link.l1 = "Маркиз, честно говоря, мне бы хотелось несколько иной оценке моих заслуг перед Францией.";
    		link.l1.go = "Step_F12_3";
		break;
 		case "Step_F12_3":
			dialog.text = "Что вы имеете в виду?";
			link.l1 = "Я говорю о денежной компенсации своих затрат в этой войне. Я на своих плечах вынес бремя боевых действий с испанцами и хотел бы получить компенсацию.";
    		link.l1.go = "Step_F12_4";
		break;
 		case "Step_F12_4":
			dialog.text = "Послушайте, вы стали дворянином, о вас знает Лувр! Вам этого мало?!";
			link.l1 = "Для моего самолюбия - это больше, чем много. Но я о другом говорю - о деньгах...";
    		link.l1.go = "Step_F12_5";
		break;
 		case "Step_F12_5":
			dialog.text = "Барон, не делайте из меня идиота, я же не требую с вас отчета о захваченных в городах трофеях. Это и есть ваш гонорар.";
			link.l1 = "Хм, теперь ясно...";
    		link.l1.go = "Step_F12_6";
		break;
 		case "Step_F12_6":
			dialog.text = "Очень хорошо, что мы прояснили все нюансы, барон. А теперь к делу - 'Солей Руаяль' выводится из состава вашей эскадры.";
			link.l1 = "Понятно...";
    		link.l1.go = "Step_F12_7";
		break;
 		case "Step_F12_7":
			dialog.text = "Через месяц я отправляю этот великолепный корабль в Старый Свет. Так что я благодарен вам за то, что вы сохранили его в надлежащем виде.";
			link.l1 = "Спасибо за благодарность, маркиз...";
    		link.l1.go = "Step_F12_8";
		break;
 		case "Step_F12_8":
			dialog.text = "Не стоит, барон... Ну что же, не смею вас задерживать более, до свидания.";
			link.l1 = "Всего хорошего, маркиз.";
    		link.l1.go = "exit";
			npchar.LifeDay = 0;
			pchar.questTemp.State = "EndOfQuestLine";
			bWorldAlivePause   = false; // Конец линейки
			AddQuestRecord("Fr12EndWar_toBorepo", "2");
			CloseQuestHeader("Fr12EndWar_toBorepo");
            DeleteAttribute(pchar, "questTemp.Waiting_time");
			//==> в течение месяца Сулей Руаяль можно взять себе.			
			sld = characterFromId("SoleiRoyalCaptain");
			RemoveCharacterCompanion(pchar, sld);
			SetCrewQuantityFull(sld);
			Fantom_SetBalls(sld, "pirate");
			Group_SetType("SoleiRoyal", "war");
			Group_AddCharacter("SoleiRoyal", "SoleiRoyalCaptain");
			Group_SetGroupCommander("SoleiRoyal", "SoleiRoyalCaptain");
			Group_SetAddress("SoleiRoyal", "Guadeloupe", "Quest_ships", "quest_ship_1");
			SetTimerCondition("Fr12EndOfWar_TakeOutShip", 0, 0, 30, false);
			AddTitleNextRate(sti(NPChar.nation), 5);
			ChangeCharacterReputation(pchar, 5);
			ChangeCharacterNationReputation(pchar, sti(NPChar.nation), 20);
			AddCharacterExpToSkill(pchar, "Repair", 630);
			AddCharacterExpToSkill(pchar, "Sneak", 490);
			AddCharacterExpToSkill(pchar, "FencingLight", 530);
			AddCharacterExpToSkill(pchar, "FencingHeavy", 530);
			AddCharacterExpToSkill(pchar, "Fencing", 530);
			AddCharacterExpToSkill(pchar, "Pistol", 530);
			AddCharacterExpToSkill(pchar, "Fortune", 660);
			AddCharacterExpToSkill(pchar, "Accuracy", 520);
			AddCharacterExpToSkill(pchar, "Grappling", 750);
			//слухи
			AddSimpleRumour("Говорят, что вы теперь маркиз! Вы баловень судьбы, капитан...", sti(npchar.nation), 5, 1);
		break;
    }
}
