// диалог по городам
void ProcessCommonDialogEvent(ref NPChar, aref Link, aref NextDiag)
{

	switch (Dialog.CurrentNode)
	{
		case "quests":
			dialog.text = NPCStringReactionRepeat(RandPhraseSimple("Какие вопросы?", "Что вам угодно?"), "Некоторое время тому назад, находясь у меня в магазине, вы пытались задать какой-то вопрос...", "За сегодня третий вопрос. Мне торговать надо, а не пустые разговоры вести...",
                          "Опять вопросы? Может лучше торговлей займемся?", "block", 1, npchar, Dialog.CurrentNode);
			link.l1 = HeroStringReactionRepeat(RandPhraseSimple("Я передумал...", "Сейчас мне не о чем говорить"), "Да, точно, пытался... Находясь в магазине...",
                      "Да уж, действительно в третий раз...", "Хм, может и поторгуем...", npchar, Dialog.CurrentNode);
			link.l1.go = "exit";
		break;
	}
	UnloadSegment(NPChar.FileDialog2);
}

