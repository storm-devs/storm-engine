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
			if (pchar.questTemp.piratesLine == "Soukins_toPuertoPrincipe")
			{
				link.l1 = "Слушай, красавица, мне нужно знать, что затевает Соукинс. Очень надо...";
				link.l1.go = "PL_Q7_1";
			}
			if (pchar.questTemp.piratesLine == "Soukins_seekRings" && CheckCharacterItem(pchar, "jewelry18") && CheckCharacterItem(pchar, "jewelry7"))
			{
				link.l1 = "Я принес то, что ты хотела. Вот твои кольца.";
				link.l1.go = "PL_Q7_3";
			}
		break;
		case "PL_Q7_1":
			dialog.text = "Принеси мне колечко с измурудом и колечко с рубином. И тогда, возможно, я смогу тебе помочь...";
			link.l1 = "У меня их нет...";
			link.l1.go = "PL_Q7_2";
			pchar.questTemp.piratesLine = "Soukins_seekRings";
			if (CheckCharacterItem(pchar, "jewelry18") && CheckCharacterItem(pchar, "jewelry7"))
            {
				link.l2 = "Вот твои кольца, дорогуша...";
				link.l2.go = "PL_Q7_3";
			}
		break;
		case "PL_Q7_2":
			dialog.text = "Очень жаль, но это мое категорическое условие.";
			link.l1 = "Понятно...";
			link.l1.go = "exit";
		break;
		case "PL_Q7_3":
			dialog.text = "М-м-м, как красиво!.. Спасибо. Так что ты хотел знать?";
			link.l1 = "Мне нужно знать, что затевает Ричард Соукинс. Ты в курсе?";
			link.l1.go = "PL_Q7_4";
			TakeItemFromCharacter(pchar, "jewelry7");
			TakeItemFromCharacter(pchar, "jewelry18");
		break;
		case "PL_Q7_4":
			dialog.text = "Нет.";
			link.l1 = "Так какого черта ты морочишь мне голову?!!";
			link.l1.go = "PL_Q7_5";
		break;
		case "PL_Q7_5":
			dialog.text = "Я же говорю, что знаю, как узнать. Значит, слушай внимательно. Ричард - человек педантичный, он все записывает в свой судовой журнал. Тебе нужно получить его, вот и все.";
			link.l1 = "Ха! Может ты еще подскажешь, как это сделать?";
			link.l1.go = "PL_Q7_6";
		break;
		case "PL_Q7_6":
			dialog.text = "Подскажу. Забери журнал из его резиденции. Днем он все время там торчит, так что живым ты оттуда не уйдешь, если посмеешь забрать журнал. Но вот ночью...";
			link.l1 = "Так ночью все закрыто!";
			link.l1.go = "PL_Q7_7";
		break;
		case "PL_Q7_7":
			dialog.text = "Влезь через окно с задней стороны дома. Только тебе лестница нужна будет, чтобы забраться наверх.";
			link.l1 = "И где ее взять?";
			link.l1.go = "PL_Q7_8";
		break;
		case "PL_Q7_8":
			dialog.text = "Не знаю...";
			link.l1 = "Хм, ну что же, спасибо и на этом.";
			link.l1.go = "exit";
			pchar.questTemp.piratesLine = "Soukins_toWindow";
		break;
	}
	UnloadSegment(NPChar.FileDialog2);  // если где-то выход внутри switch  по return не забыть сделать анлод
}

