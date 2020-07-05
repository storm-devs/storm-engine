// диалог по городам
void ProcessCommonDialogEvent(ref NPChar, aref Link, aref NextDiag)
{
    switch (Dialog.CurrentNode)
	{
		case "quests":
			dialog.text = NPCStringReactionRepeat(RandPhraseSimple("Какие вопросы, сын мой?", "Спрашивай, я слушаю тебя..."), "Я слушаю тебя, говори, сын мой...", "И в третий раз говорю тебе, сын мой: задавай свой вопрос.",
                          "Столько обязанностей у церковнослужителя, а тут еще и ты донимаешь, сын мой...", "block", 1, npchar, Dialog.CurrentNode);
			link.l1 = HeroStringReactionRepeat(RandPhraseSimple("Я передумал...", "Не сейчас, падре..."), "Да, пока особо и нечего сказать...",
                      "Задам, задам... Только позже...", "Простите, святой отец...", npchar, Dialog.CurrentNode);
			link.l1.go = "exit";
			if (pchar.questTemp.State == "Inquisition_toDeSouza")
            {
                link.l1 = "Отец, скажите, где я могу найти сеньора Антонио де Соуза?";
                link.l1.go = "Step_S2_1";
            }
			if (pchar.questTemp.State == "Fr7RockBras_toSeekPlace")
            {
                link.l1 = "Отец, скажите, где здесь у вас находится Инквизиция?";
                link.l1.go = "Step_F7_1";
            }
		break;
        case "Step_S2_1":
			dialog.text = NPCStringReactionRepeat("А ты знаешь, сын мой, кто это?", "Сын мой, я тебе уже все сазал.", "Ты опять с тем же вопросом? Я же уже ответил тебе...", "Опять? Ладно, вход в Инквизицию находится прямо лестницей, ведущий в церковь...",  "block", 0, npchar, Dialog.CurrentNode);
			link.l1 = HeroStringReactionRepeat("Конечно, он высокопоставленный инквизитор.", "Простите, падре, я не очено понял... Ну ладно...", "Да, но я не понял... Хм, простите, падре...", "Уф, спасибо, падре...", npchar, Dialog.CurrentNode);
			link.l1.go = "Step_S2_2";
		break;
        case "Step_S2_2":
			dialog.text = "Верно... Ну что же, он находится у себя в резиденции.";
			link.l1 = "Простите, падре, а где она, резиденция Инквизиции?";
			link.l1.go = "Step_S2_3";
		break;
        case "Step_S2_3":
			dialog.text = "Прямо под церковью.";
			link.l1 = "Как это?..";
			link.l1.go = "Step_S2_4";
		break;
        case "Step_S2_4":
			dialog.text = "Вот так... Иди, сын мой, иди. По доброй воле в Святую Инквизицию мало кто ходит...";
			link.l1 = "А-а-а, ну ладно...";
			link.l1.go = "exit";
		break;

        case "Step_F7_1":
			dialog.text = NPCStringReactionRepeat("Прямо под церковью, сын мой.", "Прямо под нашей церковью, сын мой. Вход находится под лестницей.", "Ты совсем тугой на слух, сын мой. Ох, лечиться тебе надобно...",
                          "Смирение...", "block", 1, npchar, Dialog.CurrentNode);
			link.l1 = HeroStringReactionRepeat("Спасибо, падре.", "Спасибо еще раз, падре.",
                      "Обязательно!", "Ась?", npchar, Dialog.CurrentNode);
			link.l1.go = "exit";
		break;
	}
	UnloadSegment(NPChar.FileDialog2);  // если где-то выход внутри switch  по return не забыть сделать анлод
}

