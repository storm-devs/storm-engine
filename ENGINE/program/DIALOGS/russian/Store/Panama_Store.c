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
            // ==> проверка на квесты
			if (CheckAttribute(pchar, "questTemp.State.Store"))
            {
                link.l1 = "Послушай, я ищу торговца по имени Яков Лопез де Фонсека. Он отправился сюда по делам, но назад еще не вернулся.";
                link.l1.go = "Step_S2_1";
            }
            // <== проверка на квесты

		break;
//*************************** Квест №2 испанки, задание Инквизиции ***************************
 		case "Step_S2_1":
			dialog.text = NPCStringReactionRepeat("Видел я его с месяц назад. Должен сказать, странная встреча вышла. Утром на улице встретил Якова, поздоровались, и он сказал, что зайдет к обеду по делам. Но с тех пор так его и не было у меня.",
                                                  "Ты уже спрашивал об этом, я тебе ответил.", "Я же говорю тебе, ты уже спрашивал об этом Якове.", "Послушай, отойди и не мешай! Совсем что ли крыша поехала?", "block", 0, npchar, Dialog.CurrentNode);
			link.l1 = HeroStringReactionRepeat("А он ничего не сказал подробнее: где остановился, куда собирается?", "Хм, ну да...", "Да, точно, спрашивал...", "Извини, " + npchar.name + "...", npchar, Dialog.CurrentNode);
			link.l1.go = DialogGoNodeRepeat("Step_S2_2", "none", "none", "none", npchar, Dialog.CurrentNode);
		break;
 		case "Step_S2_2":
			dialog.text = "Нет, ничего.";
			link.l1 = "Понятно... Спасибо тебе.";
			link.l1.go = "exit";
			AddQuestRecord("Spa_Line_2_Inquisition", "11");
		break;
	}
	UnloadSegment(NPChar.FileDialog2);
}

