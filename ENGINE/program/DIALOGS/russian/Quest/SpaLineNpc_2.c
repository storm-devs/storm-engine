// диалоговый файл №2 на испанку
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
            if (npchar.id == "Francisco")// квест №2, базар с Франциско
            {
    			dialog.text = "Это вы тот человек, который получил задание от Антониу де Соуза?";
    			link.l1 = "Да, я только что от него.";
    			link.l1.go = "Step_S2_1";
            }
            if (CheckAttribute(pchar, "questTemp.State.Usurer"))// квест №2, малый де Фонкейсао
            {
    			dialog.text = "Я сдаюсь, ты победил!";
    			link.l1 = "Янсен Нунен де Фонкейсао собственной персоной, если не ошибаюсь...";
    			link.l1.go = "Step_S2_6";
            }
            if (npchar.id == "Bandit" && CheckAttribute(pchar, "questTemp.State.Store"))// квест №2, бандит на 1-м этаже
            {
    			dialog.text = "Что тебе здесь нужно? Проваливай!";
    			link.l1 = "Эй, полегче! Что за манеры?";
    			link.l1.go = "Step_S2_11";
            }
			if (npchar.id == "JacowDeFonseka" && CheckAttribute(pchar, "questTemp.State.Store"))
			{
                dialog.text = "Кто вы? Откуда вы меня знаете? Разве мы знакомы? Как вы узнали, где я нахожусь?";
    			link.l1 = "Стоп! Стоп, уважаемый. Я вам все расскажу, только дайте я сначала освобожу вас. Вы ведь не будете возражать против этого?";
    			link.l1.go = "Step_S2_14";
            }
			if (npchar.id == "PortMansBoy")  //квест №6, посыльный начальника порта.
			{
                dialog.text = "Месье, меня послал к вам начальник порта.";
    			link.l1 = "Да, очень хорошо. Что он велел мне передать?";
    			link.l1.go = "Step_S6_1";
            }	//квест №7, Моисей Воклейн.
 			if (pchar.questTemp.State == "Sp7SavePardal_toAntigua" || pchar.questTemp.State == "Sp7SavePardal_PardalIsSink")  
			{
                dialog.text = "Дьявол!! Все так хорошо начиналось - мы отловили Пардаля... И тут появился ты...";
    			link.l1 = "Хм, для того и появился, чтобы ваши хорошие начинания закончились не так славно...";
    			link.l1.go = "Step_S7_1";
            }  //квест №8, капитан Ансель.
 			if (pchar.questTemp.State == "Sp8SaveCumana_toCumana")  
			{
                dialog.text = "Аргх, проклятый испанский пес!";
    			link.l1 = "Продолжим, капитан, давайте не будем отвлекаться...";
    			link.l1.go = "exit";
				LAi_SetCurHPMax(npchar);
				QuestAboardCabinDialogExitWithBattle("");
            }
		break;

//<<<<<<<<<<<<===== Разброс диалогов по персонажам =====================
        case "Exit":
            NextDiag.CurrentNode = NextDiag.TempNode;
            DialogExit();
        break;

//*************************** Квест №2, диалоги с Франциско на выходе из Инквизиии ***************************
 		case "Step_S2_1":
			dialog.text = "Я знаю, в чем заключается ваше задание, и знаю, чем оно может закончиться. Ордену выгодно поправлять свои финансовые дела за счет негоциантов, тем более чужими руками.";
			link.l1 = "Вы думаете, вопрос только в этом?";
			link.l1.go = "Step_S2_2";
		break;
 		case "Step_S2_2":
			dialog.text = "Возможно нет, но вы должны помнить об одном - если перед вами встанет вопрос убийства негоциантов, то прошу хорошенько подумать, ради чего вы отнимаете жизнь у этих людей. Вы их не знаете, они могут быть исправными верующими и добропорядочными гражданами.";
			link.l1 = "Как они могут быть исправными верующими, если они являются янсенистами?!";
			link.l1.go = "Step_S2_3";
		break;
 		case "Step_S2_3":
			dialog.text = "Они веруют в того же Господа, что и мы с вами. Подумайте...";
			link.l1 = "Я принял ваши слова к сведению. Но с кем я имею честь общаться?";
			link.l1.go = "Step_S2_4";
		break;
 		case "Step_S2_4":
			dialog.text = "Франциско де Сан-Агостиньо.";
			link.l1 = "Спасибо вам, Франциско. Всего хорошего.";
			link.l1.go = "Step_S2_5";
		break;
 		case "Step_S2_5":
			chrDisableReloadToLocation = false;
			pchar.questTemp.State = "Inquisition_afterFrancisco";
            LAi_SetActorType(npchar);
            LAi_ActorGoToLocation(npchar, "reload", "gate_back", "none", "", "", "", 40.0);
            LAi_SetPlayerType(pchar);
            DialogExit();
		break;
 		case "Step_S2_6":
			dialog.text = "Именно так... А откуда вы меня знаете?";
			link.l1 = "Девушка одна рассказала, Таннекен Клеменс зовут. Знаешь такую?";
			link.l1.go = "Step_S2_7";
		break;
 		case "Step_S2_7":
			dialog.text = "Вот черт возьми! И на что только эти девки не идут, чтобы замуж выскочить! Это же надо - головорезов нанять!";
			link.l1 = "Она меня не нанимала, наши интересы просто совпали в данный момент...";
			link.l1.go = "Step_S2_8";
		break;
 		case "Step_S2_8":
			dialog.text = "Уф, уже лучше... Знаешь, как-то рано мне еще семью заводить... А тебе что нужно от меня?";
			link.l1 = "От тебя - ничего. Кое-что нужно от твоего отца. Посидишь пока у меня в трюме, подумаешь о семье, о детях, о непреходящих человеческих ценностях... А мы пока с твоим папашей кое-что обсудим.";
			link.l1.go = "Step_S2_9";
		break;
 		case "Step_S2_9":
			dialog.text = "Не худший вариант в моем нынешнем положении. Спасибо, что не убил меня.";
			link.l1 = "Таннекен благодари, просила не трогать тебя, мерзавца... Ну и все на этом.";
			link.l1.go = "Step_S2_10";
		break;
 		case "Step_S2_10":
            AddQuestRecord("Spa_Line_2_Inquisition", "8");
            pchar.quest.Inquisition_DieHard.over = "yes";
            Island_SetReloadEnableGlobal("Jamaica", true);
            QuestAboardCabinDialogQuestSurrender();
            DialogExit();
		break;
 		case "Step_S2_11":
			dialog.text = "Проваливай, тебе говорят. Жить надоело?!";
			link.l1 = "Хм, что-то тут не так... Я никуда не уйду, пока ты не ответишь мне на один вопрос. А что это вы здесь делаете? Уж не Яков ли Лопез де Фонсека находится у вас в плену?";
			link.l1.go = "Step_S2_12";
		break;
 		case "Step_S2_12":
			dialog.text = "Та-а-ак... Тебе пора на тот свет, говорун...";
			link.l1 = "Не тебе определять сроки моей жизни...";
			link.l1.go = "Step_S2_13";
		break;
 		case "Step_S2_13":
            LAi_SetPlayerType(pchar);
            LAi_SetWarriorType(npchar);
            LAi_group_MoveCharacter(npchar, "EnemyFight");
            LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
            LAi_group_FightGroups("EnemyFight", LAI_GROUP_PLAYER, true);
            LAi_group_SetCheck("EnemyFight", "OpenTheDoors");
			DialogExit();
			AddDialogExitQuest("MainHeroFightModeOn");
		break;
 		case "Step_S2_14":
			dialog.text = "Конечно не буду. Это я от волнения. Надежды на освобождение у меня уже не осталось, да и чудес я ждать не привык. Я слышал, как эти бандиты обсуждали цену моего выкупа и один из них, наверное, уже добрался до Кюрасао.\n"+
                          "Но каким образом вы так быстро смогли попасть сюда? И вы так и не сказали, как вас зовут.";
			link.l1 = "Зовут " + GetFullName(pchar) + ", к вашим услугам. Ну вот... мой корабль ждет в Порто Белло, мы можем отправляться.";
			link.l1.go = "Step_S2_15";
		break;
 		case "Step_S2_15":
			dialog.text = "Не хочу показаться наглым в ваших глазах, но, видя, как вы держались и разделались с этими людьми, я понял, что вы человек военный. Хочу попросить вас об одном одолжении.\n"+
                          "Это по пути отсюда до Кюрасао. Я не вправе настаивать, я могу только просить вас. Дело в том, что наша община владела древней христианской реликвией - Евангелием от Иуды Искариота.";
			link.l1 = "Владела?";
			link.l1.go = "Step_S2_16";
		break;
 		case "Step_S2_16":
			dialog.text = "Она пропала у нас где-то 20 лет назад с ее хранителем - Самуэлем Кохэно.";
			link.l1 = "Где-то я уже слышал это имя...";
			link.l1.go = "Step_S2_17";
		break;
 		case "Step_S2_17":
			dialog.text = "Это вряд ли. Так вот, недавно мы вышли на след похитителя, он объявился на Бермудах. Попытки выкупить у него реликвию не принесли результата, тогда мы наняли матросов из таверны. Вор оказался на удивление силен и даже ранил нескольких человек, после чего бросился бежать. Матросы следовали за ним по пятам и нагнали у верфи. Но в верфи он как сквозь землю провалился.";
			link.l1 = "Как это?";
			link.l1.go = "Step_S2_18";
		break;
 		case "Step_S2_18":
			dialog.text = "Он зашел в верфь и все. Куда он делся - совершенно непонятно. Хозяин верфи нам так ничего и не сказал. Я прошу вас помочь мне вернуть книгу. Вы поможете мне?";
			link.l1 = "Как по вашему, сколько может стоить эта книга?";
			link.l1.go = "Step_S2_19";
		break;
 		case "Step_S2_19":
			dialog.text = "Она бесценна! Но почему Вы об этом спрашиваете?";
			link.l1 = "Защита христианских реликвий - святой долг каждого истинного католика... Но я еще ничего не решил, посмотрим по обстоятельствам.";
			link.l1.go = "Step_S2_20";
		break;
 		case "Step_S2_20":
			dialog.text = "Спасибо вам!";
			link.l1 = "Пока не за что.";
			link.l1.go = "Step_S2_21";
		break;
 		case "Step_S2_21":
 		    LAi_SetPlayerType(pchar);
            AddQuestRecord("Spa_Line_2_Inquisition", "12");
            AddQuestRecord("Spa_Line_2_Inquisition", "13");
            AddPassenger(pchar, npchar, false);
            SetCharacterRemovable(npchar, false);
            LAi_SetActorType(npchar);
            LAi_ActorGoToLocation(NPChar, "reload", "reload1", "none", "", "", "", 5.0);
            pchar.questTemp.State.SeekBible = 1; //флаг на поиск Евангелие
            LocatorReloadEnterDisable("Shore3", "reload2_back", true); //закрываем для особо хитрых
            DialogExit();
		break;
//*************************** Квест №6, базар с посыльным начальника порта ***************************
 		case "Step_S6_1":
			dialog.text = "Судно, которое вы ожидаете, бросило якорь на рейде Тортуги. Вестовой, голландский офицер, сейчас находится у д'Ожерона, вы можете его встретить чуть погодя на выходе из резиденции.";
			link.l1 = "Великолепно! Спасибо тебе за службу.";
			link.l1.go = "Step_S6_2";
		break;
 		case "Step_S6_2":
			dialog.text = "Да не за что. Благодарите начальника порта.";
			link.l1 = "Хм, обязательно отблагодарю.";
			link.l1.go = "Step_S6_3";
		break;
 		case "Step_S6_3":
			chrDisableReloadToLocation = false;
			AddQuestRecord("Spa_Line_6_TakeMessangeer", "3");
			bDisableFastReload = true; //закрываем фаст релоад
			SaveCurrentQuestDateParam("questTemp");
			Pchar.quest.Sp6TakeMess_Interception.win_condition.l1 = "location";
            Pchar.quest.Sp6TakeMess_Interception.win_condition.l1.location = "Tortuga_town";
            Pchar.quest.Sp6TakeMess_Interception.win_condition = "Sp6TakeMess_Interception";
            LAi_SetPlayerType(pchar);
            LAi_SetActorType(npchar);
            LAi_ActorGoToLocation(NPChar, "reload", "reload1_back", "none", "", "", "", -1);
            DialogExit();
		break;
//*************************** Квест №7, базар с Моисеем Воклейном при абордаже ***************************
 		case "Step_S7_1":
			dialog.text = "Ну что же, тебе это удалось... Отдохнули - предлагаю продолжить.";
			link.l1 = "С превеликим удовольствием...";
			link.l1.go = "exit";
			LAi_SetCurHPMax(npchar);
            QuestAboardCabinDialogExitWithBattle("");
		break;


    }
}


