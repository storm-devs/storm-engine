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
            // ==> проверка на квесты
			if (CheckAttribute(pchar, "questTemp.State.Store"))
            {
                link.l1 = "Послушай, тебе ничего не известно о голландском торговце по имени Яков Лопез де Фонсека?";
                link.l1.go = "Step_S2_1";
            }
            // <== проверка на квесты
		break;
//*************************** Квест №2 испанки, задание Инквизиции ***************************
 		case "Step_S2_1":
			dialog.text = NPCStringReactionRepeat("Нет, не видел здесь такого и не слышал о нем.", "Ты уже спрашивал об этом, я тебе ответил.", "Я же говорю тебе, ты уже спрашивал об этом Якове.", "Послушай, отойди и не мешай! Совсем что ли крыша поехала?", "block", 0, npchar, Dialog.CurrentNode);
			link.l1 = HeroStringReactionRepeat("Ну что же, спасибо тебе.", "Хм, ну да...", "Да, точно, спрашивал...", "Извини, " + npchar.name + "...", npchar, Dialog.CurrentNode);
			link.l1.go = "exit";
		break;

	}
	UnloadSegment(NPChar.FileDialog2);  // если где-то выход внутри switch  по return не забыть сделать анлод
}

