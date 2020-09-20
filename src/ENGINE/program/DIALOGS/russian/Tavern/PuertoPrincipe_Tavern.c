// диалог по городам
void ProcessCommonDialogEvent(ref NPChar, aref Link, aref NextDiag)
{
    switch (Dialog.CurrentNode)
	{
		case "quests":
			dialog.text = NPCStringReactionRepeat(RandPhraseSimple("Какие вопросы?", "Что вам угодно, " + GetAddress_Form(NPChar) + "?"), "Совсем недавно вы пытались задать мне вопрос, " + GetAddress_Form(NPChar) + "...", "В течение этого дня вы уже третий раз говорите о каком-то вопросе...",
                          "Опять вопросы будем задавать?", "block", 1, npchar, Dialog.CurrentNode);
			link.l1 = HeroStringReactionRepeat(RandPhraseSimple("Я передумал...", "Сейчас мне не о чем говорить"), "Хм, что-то с памятью моей стало...",
                      "Да уж, действительно в третий раз...", "Да нет, какие вопросы...", npchar, Dialog.CurrentNode);
			link.l1.go = "exit";
			if (pchar.questTemp.piratesLine == "Soukins_toPuertoPrincipe")
			{
				link.l1 = "Хотят слухи, что Соукинс собирает людей для какого-то дела. Ты не в курсе?";
				link.l1.go = "PL_Q7_1";
			}
		break;
		//пиратка, квест №7, попытка узнать о планах Соукинса
		case "PL_Q7_1":
			dialog.text = NPCStringReactionRepeat("Да, что-то назревает. Опять Ричард собрался испанцев пощипать, факт.", 
				"Мы уже говорили на эту тему.", 
				"Опять ты об планах Ричарда?",
                "Никакого нового совета я тебе не дам.", "block", 1, npchar, Dialog.CurrentNode);
			link.l1 = HeroStringReactionRepeat("Понятно. А ты не знаешь, кого конкретно и где?", 
				"Да, верно.",
                "Ага, опять...", 
				"Хм, спасибо и на этом...", npchar, Dialog.CurrentNode);
			link.l1.go = DialogGoNodeRepeat("PL_Q7_2", "", "", "", npchar, Dialog.CurrentNode);
		break;
		case "PL_Q7_2":
			dialog.text = "Ха, ну ты даешь! Это же военная тайна. Ричард таких вещей никогда не открывает до самого последнего момента даже для своих капитанов. А ты меня спрашиваешь...";
			link.l1 = "Ну мало ли... Так, значит, стоит мне идти с ним на это дело, как думаешь?";
			link.l1.go = "PL_Q7_3";
		break;
		case "PL_Q7_3":
			dialog.text = "Не знаю, сам решай. У Соукинса репутация удачливого кэпа. Но лично я бы пошел с ним, уж больно Ричард готовится к этому тщательно - целыми днями сидит в резиденции и строчит в своем журнале чего-то...";
			link.l1 = "Хе, понятно... Ну что же, спасибо тебе за совет.";
			link.l1.go = "exit";
		break;
	}
	UnloadSegment(NPChar.FileDialog2);  // если где-то выход внутри switch  по return не забыть сделать анлод
}

