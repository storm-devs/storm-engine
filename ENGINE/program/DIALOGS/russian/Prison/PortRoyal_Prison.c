// диалог по городам
void ProcessCommonDialogEvent(ref NPChar, aref Link, aref NextDiag)
{
 	switch(Dialog.CurrentNode)
	{
        case "quests":
            dialog.text = "Говорите, я слушаю.";
			link.l1 = "Я ошибся. Прощайте.";
			link.l1.go = "Exit";
            if (pchar.questTemp.State == "MorrisWillams_ArrestedToPrison")   // разговор по шестому квесту англ.линейки
            {
    			link.l2 = "Лейтенант, не буду ходить вокруг да около. Я хочу, чтобы заключенный Моррис Уильямс был свободен.";
    			link.l2.go = "Step_E6_1";
            }
            if (pchar.questTemp.State == "SaveFromMorgan_toPrison")   // испанка, квест №1
            {
    			link.l2 = "Офицер, я знаю, что у вас содержаться три испанца, вроде был Морган их захватил.";
    			link.l2.go = "Step_S1_1";
            }
		break;

        case "Step_E6_1":
            dialog.text = "Хм... Что ж, ценю вашу откровенность. Признаюсь, что мне тоже не по нраву арест и содержание джентльмена под стражей.\n"+
                          "Но я человек военный и приказ нарушать не стану ни при каких обстоятельствах. Посоветую вам обратиться к генерал-губернатору и желаю удачи.";
			link.l1 = "Почему именно к генерал-губернатору?";
			link.l1.go = "Step_E6_2";
		break;
        case "Step_E6_2":
            dialog.text = "Потому, что Моррис Уильямс арестован по его письменному распоряжению.";
			link.l1 = "Понятно... Скажите, лейтенант, а я могу поговорить с Уильямсом?";
			link.l1.go = "Step_E6_3";
		break;
        case "Step_E6_3":
            dialog.text = "Думаю, да. Только именно сейчас, не позже. И будте немногословны.";
			link.l1 = "Спасибо, лейтенант.";
			link.l1.go = "exit";
			pchar.questTemp.jailCanMove = true;
			pchar.questTemp.State = "MorrisWillams_ArrestedToModiford";
			AddQuestRecord("Eng_Line_6_MorrisWillams", "16");
		break;

        case "Step_S1_1":
            dialog.text = "И что?";
			link.l1 = "Да ничего особенного. Просто я хочу дать им свободу.";
			link.l1.go = "Step_S1_2";
		break;
        case "Step_S1_2":
            dialog.text = "Этот вопрос находится вне зоны моей компетенции.";
			link.l1 = "А я полагаю, что вы должны быть крайне заинтересованы в этом. Иначе я убью вас.";
			link.l1.go = "Step_S1_3";
		break;
        case "Step_S1_3":
            dialog.text = "Такого наглеца свет еще не видел. Взять его!!";
			link.l1 = "Ну что же, вы сами напросились...";
			link.l1.go = "fight";
		break;
	}
	UnloadSegment(NPChar.FileDialog2);  // если где-то выход внутри switch  по return не забыть сделать анлод
}

