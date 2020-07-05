// диалог по городам
void ProcessCommonDialogEvent(ref NPChar, aref Link, aref NextDiag)
{
    switch (Dialog.CurrentNode)
	{
		case "quests":
			dialog.text = NPCStringReactionRepeat(RandPhraseSimple("Какие вопросы?", "Что вам угодно?"), "Совсем недавно вы пытались задать мне вопрос...", "М-да, позвольте угадаю... Опять ничего существенного?",
                          "Послушайте, я финансами оперирую, а не на вопросы отвечаю...", "block", 1, npchar, Dialog.CurrentNode);
			link.l1 = HeroStringReactionRepeat(RandPhraseSimple("Я передумал...", "Сейчас мне не о чем говорить"), "Хм, что-то с памятью моей стало...",
                      "Вы угадали, простите...", "Я понимаю...", npchar, Dialog.CurrentNode);
			link.l1.go = "exit";
            // ==> Квест Аззи
			if (CheckAttribute(pchar, "questTemp.Azzy.GiveMoney"))
            {
                link.l5 = "Я пришёл по поручению Аззи. У вас для меня должно быть кое-что приготовлено.";
                link.l5.go = "Step_Az_1";
            }
			if (!CheckAttribute(pchar, "questTemp.Azzy.GiveMoney") && pchar.questTemp.Azzy == "HowToKnowAzzy")
            {
                link.l5 = "Послушайте, мне нужно срочно найти Аззи.";
                link.l5.go = "Step_Az_4";
            }
		break;
        // ==> Квест Аззи. Дача трех лимонов ГГ по поручению Аззи.
		case "Step_Az_1":
			dialog.text = "Все верно. Три миллиона наличными оджидают вас, " + GetAddress_Form(NPChar) + ". Изволите получить прямо сейчас?";
			Link.l1 = "Да, и всю сумму целиком.";
			Link.l1.go = "Step_Az_2";
		break;
		case "Step_Az_2":
			dialog.text = "Прошу вас, " + GetAddress_Form(NPChar) + ". Что-нибудь еще?";
			Link.l1 = "Нет, ничего не нужно... Послушай, а что у тебя за дела с этим Аззи? Так, для интереса спрашиваю...";
			Link.l1.go = "Step_Az_3";
            DeleteAttribute(pchar, "questTemp.Azzy.GiveMoney");
            AddMoneyToCharacter(pchar, 3000000);
		break;
		case "Step_Az_3":
			dialog.text = "Сэр, советую вам не проявлять излишнего любопытсва. С Аззи это - опасно для жизни. Если у вас все, то позвольте мне заняться своими делами...";
			Link.l1 = "Да, конечно! Спасибо за деньги.";
			Link.l1.go = "exit";
		break;
		case "Step_Az_4":
            dialog.text = NPCStringReactionRepeat("Вам нужен Аззи? Не советую вам искать его. Я уже говорил вам, что это опасно для жизни.", "Я не хочу более говорить об этом.", "Нет, хватит с меня...", "Не буду говорить.", "block", 0, npchar, Dialog.CurrentNode);
			Link.l1 = HeroStringReactionRepeat("Речь как раз и идет о моей жизни. Мне он нужен, если Вы знаете, как его найти, то скажите, прошу вас.", "Понимаю...", "Очень жаль...", "Ну что ж, значит вам действительно нечего сказать.", npchar, Dialog.CurrentNode);
			Link.l1.go = DialogGoNodeRepeat("Step_Az_5", "none", "none", "none", npchar, Dialog.CurrentNode);
		break;
		case "Step_Az_5":
			dialog.text = "К сожалению, я не знаю. Даже понятия не имею.\n"+
                     "Знаете, я взял эти пять миллионов особо не задумаываюсь над последствиями. Да-да, именно пять, так как два из них предназначались мне в качестве оплаты за работу.\n"+
                     "Так вот эти два миллиона, огромная сумма, буквально разорили меня. Я не хочу вдаваться в тонкости финансовых операциий, но поверьте мне - здесь дело не чисто.\n"+
                     "Я всегда считал себя очень проницательным. Но как я мог не догадаться здесь - ума не приложу.";
			Link.l1 = "Все ясно... Спасибо.";
			Link.l1.go = "exit";
		break;
	}
	UnloadSegment(NPChar.FileDialog2);  // если где-то выход внутри switch  по return не забыть сделать анлод
}

