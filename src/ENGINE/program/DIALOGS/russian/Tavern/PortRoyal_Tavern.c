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
            // ==> Шестой квест английки, Моррис Уильямс
            if (pchar.questTemp.State == "MorrisWillams_ArrestedSeekInfo")
			{
                link.l1 = "Послушай, а что за человек местный ростовщик?";
                link.l1.go = "Step_E6_1";
			}
            // ==> Восьмой квест фр.линейки, отговорить трех корсаров
            if (pchar.questTemp.State == "Fr8ThreeCorsairs_toSeek")
			{
                link.l1 = "Где я могу найти капитана Гея? У меня к нему важное дело.";
                link.l1.go = "Step_F8_1";
			}
		break;
        //********************** Английская линейка **********************
        case "Step_E6_1":
			dialog.text = "Ростовщик он и есть ростовщик, хороших людей среди них нет.";
			link.l1 = "Что-нибудь интересное слышал о нем?";
			link.l1.go = "Step_E6_2";
		break;
        case "Step_E6_2":
			dialog.text = "Да нет, ничего особенного.";
			link.l1 = "А кто в городе с ним хорошо знаком?";
			link.l1.go = "Step_E6_3";
		break;
        case "Step_E6_3":
			dialog.text = "Да ни с кем он толком не общается, ничего не могу сказать.";
			link.l1 = "Ну что ж, спасибо и на этом.";
			link.l1.go = "exit";
			pchar.questTemp.State = "MorrisWillams_ArrestedSeekInfoFather";
		break;
		//********************** Французская линейка **********************
        case "Step_F8_1":
			dialog.text = NPCStringReactionRepeat("Капитана Гея? Нет ничего проще. Он снимает у меня комнату, и сегодня еще не выходил, так что вы его там обязательно увидите.", 
				          "Я уже все сказал...", "Не о чем говорить...", "Сэр, дайте мне возможность заниматься своими делами!", "block", 0, npchar, Dialog.CurrentNode);
			link.l1 = HeroStringReactionRepeat("Спасибо, дружище...", "Понял, извини...", "Хм, ну да...", "Да, да, извини, приятель...", npchar, Dialog.CurrentNode);
			link.l1.go = "exit";
        break;



	}
	UnloadSegment(NPChar.FileDialog2);  // если где-то выход внутри switch  по return не забыть сделать анлод
}

