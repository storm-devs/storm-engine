// диалог по городам
void ProcessCommonDialogEvent(ref NPChar, aref Link, aref NextDiag)
{
    switch (Dialog.CurrentNode)
	{
		case "quests":
			dialog.text = NPCStringReactionRepeat(RandPhraseSimple("Какие вопросы?", "Что вам угодно?"), "Совсем недавно вы пытались задать мне вопрос...", "В у себя на верфи, а и вообще в городе, я таких однообразно любознательных не видал.",
                          "Ну что за вопросы? Мое дело - корабли строить, давайте этим и займемся.", "block", 1, npchar, Dialog.CurrentNode);
			link.l1 = HeroStringReactionRepeat(RandPhraseSimple("Я передумал...", "Сейчас мне не о чем говорить"), "Хм, что-то с памятью моей стало...",
                      "Хм, однако...", "Давайте...", npchar, Dialog.CurrentNode);
			link.l1.go = "exit";
			if (pchar.questTemp.piratesLine == "KillLoy_toSeek")
			{
				link.l1 = "К вам обращался некто Эдвард Лоу, с целью приобретения корабля?";
				link.l1.go = "PL_Q3_1";
			}
		break;
		//пиратка, квест №3, поиски Лоу
		case "PL_Q3_1":
			dialog.text = NPCStringReactionRepeat("Приходил, правда, не приобрел ничего.", 
				"Я уже все сказал по этому поводу...", 
				"Обращался, обращался. Только к сказанному ранее мне добавить нечего.",
                "Послушай, утомил!", "block", 0, npchar, Dialog.CurrentNode);
			link.l1 = HeroStringReactionRepeat("Что так?", 
				"Да, точно...",
                "Понял.", 
				"Уже ухожу...", npchar, Dialog.CurrentNode);
			link.l1.go = DialogGoNodeRepeat("PL_Q3_2", "", "", "", npchar, Dialog.CurrentNode);
		break;
		case "PL_Q3_2":
			dialog.text = "Он же бриг хотел, а денег, я извиняюсь, у него только-только на  люггер было.";
			link.l1 = "Ясно... Не заметили, куда он пошел?";
			link.l1.go = "PL_Q3_3";
		break;
		case "PL_Q3_3":
			dialog.text = "Хе-хе, известно куда. К ростовщику пошел, денег занять, только тот ему денег не дал, хе-хе-хе...";
			link.l1 = "С чего это вы так решили?";
			link.l1.go = "PL_Q3_4";
		break;
		case "PL_Q3_4":
			dialog.text = "Так он обратно не вернулся, стало быть, денег не нашел, хе-хе...";
			link.l1 = "Логично!";
			link.l1.go = "exit";
			AddQuestRecord("Pir_Line_3_KillLoy", "4");
		break;
	}
	UnloadSegment(NPChar.FileDialog2);  // если где-то выход внутри switch  по return не забыть сделать анлод
}

