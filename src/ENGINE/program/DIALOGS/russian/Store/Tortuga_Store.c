// диалог по городам
void ProcessCommonDialogEvent(ref NPChar, aref Link, aref NextDiag)
{

	switch (Dialog.CurrentNode)
	{
		case "quests":
			dialog.text = NPCStringReactionRepeat(RandPhraseSimple("Какие вопросы?", "Что вам угодно?"), "Совсем недавно вы пытались задать мне вопрос...", "В магазине Тортуги таких вопросов не было давно...",
                          "Вопросы, вопросы, опять вопросы...", "block", 1, npchar, Dialog.CurrentNode);
			link.l1 = HeroStringReactionRepeat(RandPhraseSimple("Я передумал...", "Сейчас мне не о чем говорить"), "Хм, что-то с памятью моей стало...",
                      "Ну, не было - так не было.", "Во-о-о-просы, да...", npchar, Dialog.CurrentNode);
			link.l1.go = "exit";
			// ==> Проверяем поле состояния квестов.
			switch(pchar.questTemp.State)
            {
                case "WormEnglPlans_WormPicarder": //Голландская линейка, квест №3. узнаем о Пеьере Пикардийце.
                    link.l2 = "Слушай, Пьер Пикардиец не был у тебя в магазине? Мне сказали, что он сюда отправился.";
                    link.l2.go = "Step_H3_1";
                break;

            }
             // <== Проверяем поле состояния квестов.
		break;
 	 	case "Step_H3_1":
			dialog.text = NPCStringReactionRepeat("Был часа два назад, прикупил очередную порцию дорогих безделушек и пошел дальше жизнь прожигать.", "Сеньор, давайте торговлей займемся, что за одни и те же вопросы?", "Спрашивали вы меня уже о Пьере...", "Послушайте, может вы поспрашиваете о Пьере в другом месте, а?", "block", 0, npchar, Dialog.CurrentNode);
			link.l1 = HeroStringReactionRepeat("Да уж, Пьер сейчас как сыр в масле катается... Часто он к тебе захаживает по такого рода делам?", "Говорили? Ну да, наверное...", "Можно и поторговать...", "Да, пожалуй, поспрашиваю в другом месте.", npchar, Dialog.CurrentNode);
			link.l1.go = DialogGoNodeRepeat("Step_H3_2", "none", "none", "none", npchar, Dialog.CurrentNode);
        break;
 	 	case "Step_H3_2":
			dialog.text = "Почитай, каждый день бывает, за редким исключением.";
			link.l1 = "Эх, и везет же людям! Ну ладно, пойду дальше его искать. Спасибо тебе.";
			link.l1.go = "exit";
            pchar.questTemp.Talks.Store = true;
            AddQuestRecord("Hol_Line_3_WormEnglishPlans", "8");
        break;



	}
	UnloadSegment(NPChar.FileDialog2);
}

