// диалог по городам
void ProcessCommonDialogEvent(ref NPChar, aref Link, aref NextDiag)
{
    switch (Dialog.CurrentNode)
	{
		case "quests":
			dialog.text = NPCStringReactionRepeat(RandPhraseSimple("Какие вопросы?", "Что вам угодно?"), "Совсем недавно вы пытались задать мне вопрос...", "В у себя на верфи, а и вообще в Тортуге, я таких однообразно любознательных не видал.",
                          "Мое дело - корабли строить, давайте этим и займемся.", "block", 1, npchar, Dialog.CurrentNode);
			link.l1 = HeroStringReactionRepeat(RandPhraseSimple("Я передумал...", "Сейчас мне не о чем говорить"), "Хм, что-то с памятью моей стало...",
                      "Хм, однако...", "Давайте...", npchar, Dialog.CurrentNode);
			link.l1.go = "exit";
			// ==> Проверяем поле состояния квестов.
			switch(pchar.questTemp.State)
            {
                case "WormEnglPlans_WormPicarder": //Голландская линейка, квест №3. узнаем о Пеьере Пикардийце.
                    link.l2 = "Слышал я, что у Пьера Пикардийца лучший корабль в здешних водах.";
                    link.l2.go = "Step_H3_1";
                break;

            }
             // <== Проверяем поле состояния квестов.
		break;
 	 	case "Step_H3_1":
			dialog.text = NPCStringReactionRepeat("Может быть и так. Его фрегат и без того был неплох, а на днях я закончил его модернизацию. Теперь кулеврины у него новейшие хоть куда, заряжаются с казенной части! Дорогое удовольствие, но оно того стоит... Так что, видимо, ты прав. А с чего ты спрашиваешь?", "Сеньор, давайте займемся корабельными делами, мне до Пикардийца сейчас дела нет...", "Спрашивали вы меня уже о Пьере...", "Послушайте, может вы поспрашиваете о Пьере в другом месте, а?", "block", 0, npchar, Dialog.CurrentNode);
			link.l1 = HeroStringReactionRepeat("Хочу провернуть небольшое мероприятие, но сам не потяну... Поэтому ищу, кто бы мог мне составить компанию.", "Давайте займемся...", "Да? Хм, точно спрашивал...", "Да, пожалуй, поспрашиваю в другом месте.", npchar, Dialog.CurrentNode);
			link.l1.go = DialogGoNodeRepeat("Step_H3_2", "none", "none", "none", npchar, Dialog.CurrentNode);
        break;
 	 	case "Step_H3_2":
			dialog.text = "Понятно. Ну что же, Пьер - хороший выбор в этом плане, ему такого рода мероприятия сам Морган предлагает время от времени.";
			link.l1 = "У-у-у, сам Морган?! Серьезный человек этот Пьер. Спасибо тебе за рекомендацию.";
			link.l1.go = "exit";
            pchar.questTemp.Talks.Shipyard = true;
            AddQuestRecord("Hol_Line_3_WormEnglishPlans", "9");
        break;


	}
	UnloadSegment(NPChar.FileDialog2);  // если где-то выход внутри switch  по return не забыть сделать анлод
}

