// диалог по городам
void ProcessCommonDialogEvent(ref NPChar, aref Link, aref NextDiag)
{
    switch (Dialog.CurrentNode)
	{
		case "quests":
			dialog.text = NPCStringReactionRepeat(RandPhraseSimple("Что вы хотели? Спрашивайте.", "Я слушаю вас, что за вопрос?"), "Второй раз за день вы пытаетесь задать ворпос...", "В третий раз за день вы опять пытаетесь задать вопрос...",
                          "Да когда же это кончится?! У меня дел полно по управлению делами города, а ты все вопросы пытаешься задать!", "block", 1, npchar, Dialog.CurrentNode);
			link.l1 = HeroStringReactionRepeat(RandPhraseSimple("Я передумал...", "Не сейчас, не место и не время..."), "Да, верно... Но не сейчас, позже...",
                      "Задам, задам... Только позже...", "Извините, " + GetAddress_FormToNPC(NPChar) + "...", npchar, Dialog.CurrentNode);
			link.l1.go = "exit";
			//==> Проверка на стейт квестов
			if (pchar.questTemp.State == "Sp10Maracaibo_toMaracaibo")
			{
				dialog.text = "Я слушаю вас...";
				link.l1 = "Сеньор, я прибыл к вам для выполнения ответственного поручения по защите Маракайбо от внешней угрозы. Я командирован в ваше распоряжение губернатором Гаваны Франсиско Орегон-и-Гасконом.";
				link.l1.go = "Step_S10_1";
			}
			if (pchar.questTemp.State == "Sp10Maracaibo_toGovernor")
			{
				dialog.text = "Ну что же, вы как нельзя вовремя. Еще не успели устроиться и разместить команду в городе - и пожалуйте в сражение!";
				link.l1 = "Верно подмечено!";
				link.l1.go = "Step_S10_4";
			}
			if (pchar.questTemp.State == "Sp10Maracaibo_toBattle")
			{
				dialog.text = "Вы еще здесь? Я полагаю, вам пора в сражение.";
				link.l1 = "Верно, сеньор. Я не заставлю ладронов ждать...";
				link.l1.go = "exit";
			}
			if (pchar.questTemp.State == "Sp10Maracaibo_GoodWork")
			{
				dialog.text = "Отличная работа, друг мой. Дон Франсиско прислал мне очень храброго и толкового капитана!";
				link.l1 = "Спасибо, сеньор...";
				link.l1.go = "Step_S10_5";
			}
			//<== Проверка на стейт квестов
		break;
		//************************** Квестовые штрумы ************************
		case "Cupture_after":
            dialog.text = RandPhraseSimple("Вы уже все забрали. Что вам еще нужно?", "Неужели осталось еще что-то, что вы не прихватили?");
            link.l1 = RandPhraseSimple("Осматриваюсь напоследок...", "Проверяю, может забыл что забрать...");
            link.l1.go = "exit";
            NextDiag.TempNode = "Cupture_after";
		break;

        //==> Двенадцатый квест голл. линейки. Захват Маракайбо.
        case "HolLine12Quest_Occupy":
			dialog.text = "Все, мы прекратили сопротивление. Город ваш. Что теперь будет?";
			link.l1 = "Этот город отныне принадлежит Республике Соединенных Провинций. Здесь устанавливается голландское управление и вводятся в действие голландские законы.";
			link.l1.go = "Step_H12_1";
			AfterTownBattle();
        break;
        case "Step_H12_1":
			dialog.text = "В данный момент вы оказались сильней. Но посмотрим, что будет через некоторое время. По всей видимости вам не очень сильно стала досаждать Португалия в Индийском океане, раз вы решились на такой дерзкий шаг.";
			link.l1 = "Этот шаг является логичным ответом на нападения испанских эскадр.";
			link.l1.go = "Step_H12_2";
        break;
        case "Step_H12_2":
			dialog.text = "Если бы вы не снабжали Францию деньгами, ничего бы этого не было!";
			link.l1 = "Полагаю, теперь поздно о чем-то спорить. Этот город стал голландским, а дальше видно будет.";
			link.l1.go = "exit";
            AddQuestRecord("Hol_Line_12_OccupyMaracaibo", "2");
            pchar.questTemp.State = "OccupyMaracaibo_toWeWon";
            NextDiag.TempNode = "Cupture_after";
			PChar.questTemp.DontSetNewDialogToMayor = true; // иначе может сменить диалог и сбойнуть
            PChar.questTemp.DontNullDeposit = true;    // чтоб не нулили ростовщика
            SetCaptureTownByNation(NPChar.City, sti(PChar.nation));
            DeleteAttribute(PChar, "questTemp.DontSetNewDialogToMayor");
            UpdateRelations();
            Statistic_AddValue(Pchar, NationShortName(sti(NPChar.nation)) + "_TakeTown", 1);
            SetReturn_Gover_Dialog_Exit(NPChar);
            chrDisableReloadToLocation = false;
            AddDialogExitQuestFunction("LaunchColonyInfoScreen"); // табличка  
        break;
        //==> Десятый квест испанки. Защита Маракайбо.
        case "Step_S10_1":
			dialog.text = "Отлично, мой запрос на усиление эскадры и гарнизона удовлетворен!";
			link.l1 = "Да, это так. Не могли бы вы рассказать мне, чего вы опасаетесь?";
			link.l1.go = "Step_S10_2";
        break;
        case "Step_S10_2":
			dialog.text = "Конечно, капитан. Итак, я располагаю некоторыми сведениями, что очередное сборище английских бандитов в Порт-Рояле собирается захватить Маракайбо. Сведения эти не заслуживают стопроцентного доверия, но тем не менее я запросил помощи у сеньора Франсиско Орегон-и-Гаскона.";
			link.l1 = "Понятно. Что я должен делать сейчас?";
			link.l1.go = "Step_S10_3";
        break;
        case "Step_S10_3":
			dialog.text = "Собственно, ничего. Просто находитесь в состоянии боевой готовности.";
			link.l1 = "Я и моя команда всегда находятся в таком состоянии... Ну что же, если я вам не нужен, то я пожалуй, пройдусь по городу по своим делам.";
			link.l1.go = "exit";
			pchar.questTemp.State = "Sp10Maracaibo_TalkWithOfficer";
            Pchar.quest.Sp10Maracaibo_TalkWithOfficer.win_condition.l1 = "location";
            Pchar.quest.Sp10Maracaibo_TalkWithOfficer.win_condition.l1.location = "Maracaibo_town";
            Pchar.quest.Sp10Maracaibo_TalkWithOfficer.win_condition = "Sp10Maracaibo_TalkWithOfficer";
		break;
        case "Step_S10_4":
			dialog.text = "Я прошу вас приступить к выполнению вашей задачи. Необходимо уничтожить английскую пиратскую эскадру!";
			link.l1 = "Хорошо, сеньор.";
			link.l1.go = "exit";
			LocatorReloadEnterDisable("Maracaibo_town", "gate", false);
			LocatorReloadEnterDisable("Maracaibo_town", "reload1_back", false);
			pchar.questTemp.State = "Sp10Maracaibo_toBattle";
            Pchar.quest.Sp10Maracaibo_Battle.win_condition.l1 = "location";
            Pchar.quest.Sp10Maracaibo_Battle.win_condition.l1.location = "Maracaibo";
            Pchar.quest.Sp10Maracaibo_Battle.win_condition = "Sp10Maracaibo_Battle";
        break;
        case "Step_S10_5":
			dialog.text = "За успешное отражение английской атаки город готов выплатить вам 70000 золотых. Извольте получить, капитан.";
			link.l1 = "Очень хорошо.";
			link.l1.go = "Step_S10_6";
        break;
        case "Step_S10_6":
			dialog.text = "Итак, ваша задача здесь выполнена, более не смею вас задерживать. Передавайте мою искреннюю благодарность генерал-губернатору Орегон-и-Гаскону.";
			link.l1 = "Всенепременно, сеньор. Прощайте.";
			link.l1.go = "exit";
			pchar.questTemp.State = "Sp10Maracaibo_2GoodWork";
			AddMoneyToCharacter(pchar, 70000);
        break;



	}
	UnloadSegment(NPChar.FileDialog2);  // если где-то выход внутри switch  по return не забыть сделать анлод
}

