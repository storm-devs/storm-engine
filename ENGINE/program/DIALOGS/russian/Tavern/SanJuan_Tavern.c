// диалог по городам
void ProcessCommonDialogEvent(ref NPChar, aref Link, aref NextDiag)
{
    switch (Dialog.CurrentNode)
	{
		case "quests":
			dialog.text = NPCStringReactionRepeat("Все сплетни города "+ GetCityName(npchar.city) +" к вашим услугам. Что бы вы хотели узнать?",
                          "Мы только что поднимали это тему. Вы, вероятно, запамятовали...", "Сегодня вы уже третий раз говорите о каком-то вопросе...",
                          "Что ты заладил как попугай одно и то же...", "block", 1, npchar, Dialog.CurrentNode);
			link.l1 = HeroStringReactionRepeat("Знаешь, " + NPChar.name + ", как-нибудь в следующий раз.", "Точно, забыл что-то...",
                      "Да уж, действительно в третий раз...", "Да уж...", npchar, Dialog.CurrentNode);
			link.l1.go = "exit";
            // ==> Проверяем поле состояния квестов. Эдди.
			switch(pchar.questTemp.State)
            {
                case "MorrisWillams_PinnaceNotFound":
                    link.l2 = "Давненько я у вас не был... Что нового в Сан-Хуане?";
                    link.l2.go = "Step_E6_1";
                break;
            }
		break;
        //---------------- квесты --------------------
        case "Step_E6_1":
			dialog.text = "В общем, все по старому. Город процветает.";
			link.l1 = "Понятно... Слушай, в город не заходил пинас 'Санта Агуэда'?";
			link.l1.go = "Step_E6_2";
		break;
        case "Step_E6_2":
			dialog.text = "Да, прибыл прямиком не откуда-нибудь, а из Порт-Рояля!";
			link.l1 = "Вот это да! С англичанами мир?";
			link.l1.go = "Step_E6_3";
		break;
        case "Step_E6_3":
			dialog.text = "Да не сказать, чтобы мир, но уже и не война. У англичан сейчас с голландцами столкновения, так что в Старом Свете отношения налаживаются.";
			link.l1 = "Ну что же, это к лучшему... А пинас этот где сейчас?";
			link.l1.go = "Step_E6_4";
		break;
        case "Step_E6_4":
			dialog.text = "Разгрузил трюмы и ушел. Теперь Сан-Хуан завален индиго да серебром. Цены упали...";
			link.l1 = "Так что, груз с пинаса распродан?";
			link.l1.go = "Step_E6_5";
		break;
        case "Step_E6_5":
			dialog.text = "Еще бы! А тебе то зачем этот пинас понадобился?";
			link.l1 = "Да я договаривался, что груз этот мне уступят, а они продали все здесь...";
			link.l1.go = "Step_E6_6";
		break;
        case "Step_E6_6":
			dialog.text = "Ну что же, в следующий раз договаривайся с кем-нибудь другим.";
			link.l1 = "Это верно, приятель. Спасибо тебе, будь здоров.";
			link.l1.go = "exit";
			pchar.questTemp.State = "MorrisWillams_2PinnaceNotFound";
		break;

	}
	UnloadSegment(NPChar.FileDialog2);  // если где-то выход внутри switch  по return не забыть сделать анлод
}

