// диалог по городам
void ProcessCommonDialogEvent(ref NPChar, aref Link, aref NextDiag)
{

	switch (Dialog.CurrentNode)
	{
		case "quests":
			dialog.text = NPCStringReactionRepeat(RandPhraseSimple("Какие вопросы?", "Что вам угодно?"), "Некоторое время тому назад, находясь у меня в магазине, вы пытались задать какой-то вопрос...", "За сегодня третий вопрос. Мне торговать надо, а не пустые разговоры вести...",
                          "Опять вопросы? Может лучше торговлей займемся?", "block", 1, npchar, Dialog.CurrentNode);
			link.l1 = HeroStringReactionRepeat(RandPhraseSimple("Я передумал...", "Сейчас мне не о чем говорить"), "Да, точно, пытался... Находясь в магазине...",
                      "Да уж, действительно в третий раз...", "Хм, может и поторгуем...", npchar, Dialog.CurrentNode);
			link.l1.go = "exit";
			if (pchar.questTemp.piratesLine == "KillLoy_toSeek")
			{
				link.l1 = "Простите, у вас Эдвард Лоу, приобретал корабль?";
				link.l1.go = "PL_Q3_1";
			}
		break;
		//пиратка, квест №3, поиски Лоу
		case "PL_Q3_1":
			dialog.text = NPCStringReactionRepeat("У меня?! Вы меня с кем-то путаете, я торговец, но кораблями... Кораблями не торгую! Вам, молодой человек, на верфь нужно обратиться, к корабелам. Да, да к ним, а ко мне за всем остальным!", 
				"Вы уже спрашивали. Нет, не приобретал.", 
				"Нет, молодой человек, нет...",
                "Ох, ну и зануда же вы!!", "block", 1, npchar, Dialog.CurrentNode);
			link.l1 = HeroStringReactionRepeat("Да, но в вашем городишке нет верфи!", 
				"Понятно...",
                "Ага, ясно...", 
				"Хм, уже ухожу...", npchar, Dialog.CurrentNode);
			link.l1.go = DialogGoNodeRepeat("PL_Q3_2", "", "", "", npchar, Dialog.CurrentNode);
		break;
		case "PL_Q3_2":
			dialog.text = "Ничем помочь не могу, по моему желанию верфь у нас не появится. Спросите у кого-нибудь еще...";
			link.l1 = "Ясно... Ну что же, спасибо и на этом.";
			link.l1.go = "exit";
			AddQuestRecord("Pir_Line_3_KillLoy", "3");
		break;
	}
	UnloadSegment(NPChar.FileDialog2);
}

