// диалог по городам
void ProcessCommonDialogEvent(ref NPChar, aref Link, aref NextDiag)
{
    switch (Dialog.CurrentNode)
	{
		case "quests":
			dialog.text = NPCStringReactionRepeat(RandPhraseSimple("Вопросы, " + GetAddress_Form(NPChar) + "?", "Что вам угодно, " + GetAddress_Form(NPChar) + "?"), "Хм, уж не пристаете ли вы к мне, " + GetAddress_Form(NPChar) + "? Опять со странным вопросами...", "В течение этого дня вы уже третий раз говорите о каком-то вопросе... Это знаки внимания?",
                          "Опять вопросы будете задавать, " + GetAddress_Form(NPChar) + "?", "block", 1, npchar, Dialog.CurrentNode);
			link.l1 = HeroStringReactionRepeat(RandPhraseSimple("Я передумал...", "Сейчас мне не о чем говорить"), "Да нет, что ты, красавица...",
                      "Никак нет, дорогуша, склероз...", "Да нет, какие вопросы...", npchar, Dialog.CurrentNode);
			link.l1.go = "exit";
		break;

        case "DefenceOrange": //Голландская линейка, квест №4, защита форта Оранж.
            dialog.text = "Ох, минхер, я просто счастлива, что осталась жива! И еще я счаслива...";
            link.l2 = "А еще от чего?";
            link.l2.go = "Step_H4_1";
        break;

        case "Step_H4_1":
			dialog.text = "И еще я счаслива, что вижу такого великого капитана и офицера! Весь форт вам благодарен, вы просто спасли нас!";
			link.l1 = "Да ну что ты, красавица, я исполнял свой долг...";
			link.l1.go = "exit";
			NextDiag.TempNode = "First time";
		break;
		
	}
	UnloadSegment(NPChar.FileDialog2);  // если где-то выход внутри switch  по return не забыть сделать анлод
}

