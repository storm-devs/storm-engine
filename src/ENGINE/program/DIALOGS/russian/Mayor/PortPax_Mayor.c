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
			if (pchar.questTemp.State == "Fr9GuardPP_GoodWork")
			{
				dialog.text = "Капитан, весь город должен вам по гроб жизни! Вы просто спасли нас...";
				link.l1 = "Барон, сударь. Я - барон, не просто капитан!.. Ну, а по этому поводу - да, спас, и весьма рад этому обстоятельству...";
				link.l1.go = "exit";			
			}
		break;

		case "Cupture_after":
            dialog.text = RandPhraseSimple("Вы уже все забрали. Что вам еще нужно?", "Неужели осталось еще что-то, что вы не прихватили?");
            link.l1 = RandPhraseSimple("Осматриваюсь напоследок...", "Проверяю, может забыл что забрать...");
            link.l1.go = "exit";
            NextDiag.TempNode = "Cupture_after";
		break;

        //==> Двенадцатый квест исп.линейки: захват Порт-о-Принса.
        case "SpaLine12Quest_PortPax":
			dialog.text = "Мы сдаемся, чертов кастилец...";
			link.l1 = "Это хорошо - вы сохранили свои жизни.";
			link.l1.go = "Step_S12_1";
			AfterTownBattle();
        break;
        case "Step_S12_1":
			dialog.text = "Это французская колония, какого дьявола...";
			link.l1 = "Теперь это испанская колония раз и навсегда. Ну а вас, месье бывший губернатор, я не задерживаю...";
			link.l1.go = "exit";
            AddQuestRecord("Spa_Line_12_OccupyPortPax", "2");
			pchar.questTemp.State = "OccupyPortPax_WeWin";
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

