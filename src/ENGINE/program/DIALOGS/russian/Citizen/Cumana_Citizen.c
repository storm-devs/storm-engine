// диалог по городам
void ProcessCommonDialogEvent(ref NPChar, aref Link, aref NextDiag)
{
    switch (Dialog.CurrentNode)
	{
		case "quests":
            dialog.text = RandPhraseSimple("Какие вопросы?", "Что вам угодно?");
			link.l1 = RandPhraseSimple("Я передумал...", "Сейчас мне не о чем говорить");
		    link.l1.go = "exit";
			// Квест Изабеллы, розыск кэпа брига "раны"
			if (CheckAttribute(pchar, "RomanticQuest.ShipToCumana"))
			{
                dialog.Text = "Я вас слушаю.";
    			link.l1 = "Не заходило ли в порт судно 'Пять ран Христовых'?";
    			Link.l1.go = "RomanticQuest_1";
			}			
		break;
		// ==> Квест Изабеллы, розыск кэпа брига "раны"
		case "RomanticQuest_1":
			dialog.Text = "Как же, сеньор, заходило. Они попали в шторм, потеряли грот-мачту, пришлось им вернуться сюда.";
			link.l1 = "А где мне найти капитана?.";
			Link.l1.go = "RomanticQuest_2";
		break;		
		case "RomanticQuest_2":
			dialog.Text = "Мне показалось, он ушел в таверну совсем недавно";
			link.l1 = "Спасибо.";
			Link.l1.go = "exit";
			DeleteAttribute(pchar, "RomanticQuest.ShipToCumana");
			Pchar.RomanticQuest.ShipToCumanaTavenrn = true;			
			ref rChar = GetCharacter(NPC_GenerateCharacter("BrigCaptain", "shipowner_13", "man", "man", 20, SPAIN, -1, false));
            rChar.Dialog.Filename = "Quest\Isabella\BrigCaptain.c";
            FreeSitLocator("Cumana_tavern", "sit3");  // очистим стул
			ChangeCharacterAddressGroup(rChar, "Cumana_tavern", "sit", "sit3");
            LAi_SetSitType(rChar);
		break;
		// <== Квест Изабеллы, розыск кэпа брига "раны"
		case "info":
        // заменить на описание неких НПС, по квестам
			dialog.text = "Я что, работаю на тайную агентурную сеть для "+NationNameGenitive(sti(NPChar.nation))+"?";
			link.l1 = "Ну ладно... тогда пока.";
			link.l1.go = "exit";
			link.l2 = "Тогда другой вопрос";
			link.l2.go = "new question";
		break;
		
		case "town":
        // заменить на описание как пройти, по квестам
			dialog.text = "Я вам что справочная? Не знаю. Ничего не ведаю.";

            link.l1 = "Вот бестолочь! Пока.";
			link.l1.go = "exit";
			link.l2 = "Тогда другой вопрос";
			link.l2.go = "new question";
		break;
	}
	UnloadSegment(NPChar.FileDialog2);  // если где-то выход внутри switch  по return не забыть сделать анлод
}
