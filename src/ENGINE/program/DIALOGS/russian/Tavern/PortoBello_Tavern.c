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
			if (pchar.RomanticQuest == "SeekInPortoBello")
			{
				link.l1 = "Послушай, ты не в курсе, никто не ждет здесь прибытия брига 'Восторженный'?";
				link.l1.go = "Romantic_1";
			}
		break;
		case "Romantic_1":
            dialog.text = "Нет, меня никто не спрашивал.";
            link.l1 = "Понятно... Скажи, а комнату кто-нибудь снимает у тебя?";
			link.l1.go = "Romantic_2";
		break;	
		case "Romantic_2":
            dialog.text = "Сейчас нет, " + GetAddress_Form(pchar) + ", все пусто.";
            link.l1 = "Ясно... Ну что же, спасибо тебе.";
			link.l1.go = "exit";
			pchar.RomanticQuest = "ToRositaInBeliz";
		break;	
	}
	UnloadSegment(NPChar.FileDialog2);  // если где-то выход внутри switch  по return не забыть сделать анлод
}

