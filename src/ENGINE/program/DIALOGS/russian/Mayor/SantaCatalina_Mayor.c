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
		break;
		case "Cupture_after":
            dialog.text = RandPhraseSimple("Вы уже все забрали. Что вам еще нужно?", "Неужели осталось еще что-то, что вы не прихватили?");
            link.l1 = RandPhraseSimple("Осматриваюсь напоследок...", "Проверяю, может забыл что забрать...");
            link.l1.go = "exit";
            NextDiag.TempNode = "Cupture_after";
		break;

        //********************** Фр.линейка. Квест №11. Захват Санта Каталины ************************
		case "FraLine11Quest_OccupySC":
            dialog.text = "Это нападение - второе за последнее время! Французы сильно рискуют обратить на себя гнев Эскорила в полной мере.";
			link.l1 = "Хех, ну надо же! Да пусть гневается королева-мать, у нас война, друг мой.";
            link.l1.go = "Step_F11_1";
			AfterTownBattle();
        break;
		case "Step_F11_1":
            dialog.text = "И что вы намерены делать дальше?";
			link.l1 = "Дальше я намерен сместить вас, привести жителей к присяге Людовику XIV и передать управление городом в руки французской администрации. Вы не будете против?";
			link.l1.go = "Step_F11_2";
        break;
		case "Step_F11_2":
            dialog.text = "Ну, поиронизируйте, сенор, по праву победителя вы можете себе это позволить. Но все меняется здесь, в Карибском море...";
			link.l1 = "Приложу все усилия, чтобы эти перемены были на пользу Франции, сеньор. Ну все, дело сделано...";
			link.l1.go = "exit";
			pchar.questTemp.State = "Fr11OccupySC_toWeWon";
            AddQuestRecord("Fra_Line_11_OccupySantaCatalina", "2");
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

	}
	UnloadSegment(NPChar.FileDialog2);  // если где-то выход внутри switch  по return не забыть сделать анлод
}

