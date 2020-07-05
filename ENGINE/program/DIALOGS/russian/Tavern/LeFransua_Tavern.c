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
			if (pchar.questTemp.piratesLine == "BlackLabel_toLaVega")
			{
				link.l1 = "Приятель, подскажи, не проживает ли у вас в городишке Эдвард Лоу?";
				link.l1.go = "PL_Q2_1";
			}
			if (pchar.questTemp.piratesLine == "KillLoy_toSeek")
			{
				link.l1 = "Дружище, я опять к тебе... Скажи, а куда подевался Эдвард Лоу? Мне он срочно нужен...";
				link.l1.go = "PL_Q3_1";
			}
		break;
		//пиратка, квест №2, черная метка для Лоу
		case "PL_Q2_1":
			dialog.text = NPCStringReactionRepeat("Проживает.", 
				"Я уже говорил, что проживает.", 
				"Проживает, проживает...",
                "Утомил...", "block", 1, npchar, Dialog.CurrentNode);
			link.l1 = HeroStringReactionRepeat("Не подскажешь, как мне его найти?", 
				"Да-да, я понял...",
                "Понял...", 
				"...", npchar, Dialog.CurrentNode);
			link.l1.go = DialogGoNodeRepeat("PL_Q2_2", "exit", "exit", "exit", npchar, Dialog.CurrentNode);
		break;
		case "PL_Q2_2":
			dialog.text = "Отчего ж не подсказать, подскажу. Хорошему человеку и подсказать приятно.\nА найти его дом совсем просто. Тебе нужно выйти из таверны,повернуть направо и пройти до упора. Обшитый досками дом и есть дом Эдварда Лоу.";
			link.l1 = "Спасибо тебе, приятель!";
			link.l1.go = "exit";
			AddQuestRecord("Pir_Line_2_BlackLabel", "2");
		break;
		//пиратка, квест №3, поиски Лоу
		case "PL_Q3_1":
			dialog.text = NPCStringReactionRepeat("Э-э-э, опоздал ты, приятель. Он ушел из города, совсем ушел.", 
				"Уже говорил тебе, что Лоу ушел из города.", 
				"Ушел он, ушел...",
                "Ох, и до чего же ты надоедливый!", "block", 1, npchar, Dialog.CurrentNode);
			link.l1 = HeroStringReactionRepeat("Куда ушел?", 
				"Да, я помню.",
                "Да, запамятовал...", 
				"Понял, ухожу...", npchar, Dialog.CurrentNode);
			link.l1.go = DialogGoNodeRepeat("PL_Q3_2", "", "", "", npchar, Dialog.CurrentNode);
		break;
		case "PL_Q3_2":
			dialog.text = "Да, кто ж его знает, все срочно распродал и ушел. Главное, ведь бочонок, бочонок-то не забрал, а так просил, отложи, говорит, для меня бочонок Ямайского рому\nВидать, торопился сильно. Ну, ничего я его, бочонок-то, уже пристроил.";
			link.l1 = "Погоди, ты со своим бочонком. Ты лучше скажи, где мне его теперь искать?";
			link.l1.go = "PL_Q3_3";
		break;
		case "PL_Q3_3":
			dialog.text = "Ой, не знаю, говорят, он корабль пошел покупать. Ты народ поспрошай, может, кто чего видел, может, кто чего знает\nА я то не знаю, он ведь, ко мне за бочонком не зашел, а хороший бочонок-то был...";
			link.l1 = "Понятно... Спасибо тебе, дружище.";
			link.l1.go = "exit";
		break;
	}
	UnloadSegment(NPChar.FileDialog2);  // если где-то выход внутри switch  по return не забыть сделать анлод
}

