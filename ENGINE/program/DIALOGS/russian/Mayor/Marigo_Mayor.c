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

        //==> Одиннадцатый квест исп.линейки: разорение Виллемстада и Мариго.
        case "SpaLine11Quest_DestrHol":
			dialog.text = "Почему Испания напала на голландские поселения? Меж нами нет войны!!";
			link.l1 = "Де-факто вы ведете войну на стороне французов. Вы помогаете им деньгами.";
			link.l1.go = "Step_S11_1";
			AfterTownBattle();
        break;
        case "Step_S11_1":
			dialog.text = "Доказательства у вас есть?";
			link.l1 = "Они нам не нужны. Ваше торговое могущество на Карибах надолго подорвано, а теперь попробуйте ссужать деньги Людовику. Если найдете свободные средства...";
			link.l1.go = "Step_S11_2";
        break;
        case "Step_S11_2":
			dialog.text = "Будь проклята Испания...";
			link.l1 = "Осторожней в выражениях, иначе глотку перережу! Минхер голландский...";
			link.l1.go = "exit";
            AddQuestRecord("Spa_Line_11_DestrHolland", "3");
            pchar.questTemp.Q11_Marigo = 1;
            NextDiag.TempNode = "Cupture_after";
            Statistic_AddValue(Pchar, NationShortName(sti(NPChar.nation)) + "_TakeTown", 1);
            SetReturn_Gover_Dialog_Exit(NPChar);
            chrDisableReloadToLocation = false;
        break;
	}
	UnloadSegment(NPChar.FileDialog2);  // если где-то выход внутри switch  по return не забыть сделать анлод
}

