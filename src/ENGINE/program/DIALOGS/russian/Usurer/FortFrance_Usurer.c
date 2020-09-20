// диалог по городам
void ProcessCommonDialogEvent(ref NPChar, aref Link, aref NextDiag)
{
    switch (Dialog.CurrentNode)
	{
		case "quests":
			dialog.text = NPCStringReactionRepeat(RandPhraseSimple("Какие вопросы?", "Что вам угодно?"), "Совсем недавно вы пытались задать мне вопрос...", "М-да, позвольте угадаю... Опять ничего существенного?",
                          "Послушайте, я финансами оперирую, а не на вопросы отвечаю...", "block", 1, npchar, Dialog.CurrentNode);
			link.l1 = HeroStringReactionRepeat(RandPhraseSimple("Я передумал...", "Сейчас мне не о чем говорить"), "Хм, что-то с памятью моей стало...",
                      "Вы угадали, простите...", "Я понимаю...", npchar, Dialog.CurrentNode);
			link.l1.go = "exit";
			if (pchar.questTemp.piratesLine == "KillLoy_toSeek")
			{
				link.l1 = "Говорят, к вам должен был обратиться Эдвард Лоу, я ищу его.";
				link.l1.go = "PL_Q3_1";
			}
		break;
		case "PL_Q3_1":
			dialog.text = NPCStringReactionRepeat("Он приходил ко мне, просил денег. Не дал. Очень скользкий тип, глазки так и бегают, явно намеревался дать деру. Я, смею заметить, в людях хорошо разбираюсь.", 
				"Я уже все сказал на эту тему, ищите сами.", 
				"Опять?",
                "Проваливай!", "block", 1, npchar, Dialog.CurrentNode);
			link.l1 = HeroStringReactionRepeat("А куда...", 
				"Понятно.",
                "Ага, извините...", 
				"Понял, ухожу.", npchar, Dialog.CurrentNode);
			link.l1.go = DialogGoNodeRepeat("PL_Q3_2", "", "", "", npchar, Dialog.CurrentNode);
		break;
		case "PL_Q3_2":
			dialog.text = "И не имею ни малейшего представления, куда он мог пойти.";
			link.l1 = "Ясно...";
			link.l1.go = "exit";
			AddQuestRecord("Pir_Line_3_KillLoy", "5");
		break;


	}
	UnloadSegment(NPChar.FileDialog2);  // если где-то выход внутри switch  по return не забыть сделать анлод
}

