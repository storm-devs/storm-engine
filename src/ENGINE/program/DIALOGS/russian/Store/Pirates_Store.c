// диалог по городам
void ProcessCommonDialogEvent(ref NPChar, aref Link, aref NextDiag)
{

	switch (Dialog.CurrentNode)
	{
		case "quests":
			dialog.text = NPCStringReactionRepeat("Спрашивай, чего ты хочешь?",
                          "Мы только что поднимали это тему. Вы, вероятно, запамятовали...", "Сегодня вы уже третий раз говорите о каком-то вопросе...",
                          "Послушай, это магазин, здесь люди покупают что-то. Не отвлекай меня!", "block", 1, npchar, Dialog.CurrentNode);
			link.l1 = HeroStringReactionRepeat("Знаешь, " + NPChar.name + ", как-нибудь в следующий раз.", "Точно, забыл что-то...",
                      "Да уж, действительно в третий раз...", "Гм, не буду...", npchar, Dialog.CurrentNode);
			link.l1.go = "exit";
			if (pchar.questTemp.BlueBird == "toBermudes")
			{
				link.l1 = "Уважаемый, я ищу один необычный корабль, щебеку 'Синяя Птица'. Ничего о ней не слышали? За любую информацию о ней я готов заплатить...";
				link.l1.go = "BlueBird_1";
			}
			if (pchar.questTemp.BlueBird == "weWon")
			{
				link.l1 = "Хотел вам сказать, что я добрался таки до 'Синей Птицы'. Больше она вам служить не будет.";
				link.l1.go = "BlueBird_3";
			}
		break;
		case "BlueBird_1":
			dialog.text = NPCStringReactionRepeat("Я ничего не знаю об этом корабле. Если это все, что вы хотели, то уходите.", 
				"Опять? Я все вам сказал.", 
				"Что, снова про то же?",
                "Да когда же это кончится?! Прошу вас, не отвлекайте меня от работы!", "block", 1, npchar, Dialog.CurrentNode);
			link.l1 = HeroStringReactionRepeat("Что-то не очень вы любезны, милейший!", 
				"Да-да, я помню...",
                "Ага.", 
				"Хорошо, хорошо...", npchar, Dialog.CurrentNode);
			link.l1.go = DialogGoNodeRepeat("BlueBird_2", "exit", "exit", "exit", npchar, Dialog.CurrentNode);
		break;
		case "BlueBird_2":
			dialog.text = "Я вам не милейший, ром хлестать и под забором с вами валяться не имел удовольствия.";
			link.l1 = "О как! Да я просто спросил. Ну ладно, спасибо и на этом...";
			link.l1.go = "exit";
			AddQuestRecord("Xebeca_BlueBird", "2");
		break;

		case "BlueBird_3":
			dialog.text = NPCStringReactionRepeat("Пошел вон отсюда, идиот...", 
				"Утомил...", 
				"Хм, кажется ты потерял рассудок...",
                "Опять о том же? Я-то здесь при чем?", "block", 1, npchar, Dialog.CurrentNode);
			link.l1 = HeroStringReactionRepeat("У-у-у, какие эпитеты!", 
				"Да-да.",
                "Все у меня в порядке!", 
				"Может и ни при чем, а может и при чем-то.", npchar, Dialog.CurrentNode);
			link.l1.go = DialogGoNodeRepeat("exit", "", "", "", npchar, Dialog.CurrentNode);
		break;
	}
	UnloadSegment(NPChar.FileDialog2);
}

