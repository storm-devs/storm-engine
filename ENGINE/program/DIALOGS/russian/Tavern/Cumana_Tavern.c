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
            if (CheckAttribute(pchar, "RomanticQuest.ShipToCumana"))
			{
    			link.l1 = "Ты не знаешь, где я могу найти капитана брига 'Пять ран Христовых'?";
    			Link.l1.go = "RomanticQuest_1";
			}
			if (CheckAttribute(pchar, "RomanticQuest.ShipToCumanaTavenrn"))
			{
    			link.l1 = "Где я могу найти капитана брига 'Пять ран Христовых'? Мне сказали он тут.";
    			Link.l1.go = "RomanticQuest_2";
			}
			if (pchar.questTemp.piratesLine == "KillLoy_toCumana")
			{
				link.l1 = "Не заходил ли к вам некий капитан брига, по имени Эдвард Лоу?";
				link.l1.go = "PL_Q3_1";
			}
		break;

		case "RomanticQuest_1":
			dialog.text = "Да, появляется тут этот капитан, уже какой день чинит свой корабль. Но сейчас его нет. В городе поищите.";
			link.l1 = "Спасибо!";
			link.l1.go = "exit";
		break;		
		case "RomanticQuest_2":
			dialog.text = "Вон за круглым столиком сидит, вином горе заливает.";
			link.l1 = "Спасибо!";
			link.l1.go = "exit";
			DeleteAttribute(pchar, "RomanticQuest.ShipToCumanaTavenrn");
		break;

		case "PL_Q3_1":
			dialog.text = NPCStringReactionRepeat("Нет. Первый раз это имя слышу.", 
				"Я уже отвечал вам на этот вопрос.", 
				"Говорили мы уже об этом Лоу...",
                "Последний раз говорю, что ничего об Эдварде Лоу не слышал!", "block", 1, npchar, Dialog.CurrentNode);
			link.l1 = HeroStringReactionRepeat("Спасибо, приятель.", 
				"Да-да, я помню...",
                "Да, конечно...", 
				"Понял.", npchar, Dialog.CurrentNode);
			link.l1.go = DialogGoNodeRepeat("exit", "", "", "", npchar, Dialog.CurrentNode);
		break;

	}
	UnloadSegment(NPChar.FileDialog2);  // если где-то выход внутри switch  по return не забыть сделать анлод
}

