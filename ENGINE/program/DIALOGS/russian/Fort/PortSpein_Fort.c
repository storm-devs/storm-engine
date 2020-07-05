// диалог по городам
void ProcessCommonDialogEvent(ref NPChar, aref Link, aref NextDiag)
{
    switch (Dialog.CurrentNode)
	{
		case "quests":
			dialog.text = NPCStringReactionRepeat(RandPhraseSimple("Как вам наш форт? Неплохо, да? И гарнизон под стать, хе-хе...", "У солдата в форте жизнь скучна - караулы, разводы. То ли дело в городе... Что вам угодно, " + GetAddress_Form(NPChar) + "?"), "Не пристало приставать к солдату с глупыми вопросами, " + GetAddress_Form(NPChar) + "...", "Третий раз за сегодня вы пытаетесь задать вопрос...",
                          "Солдатская доля и без того непроста, а тут еще и ты, недоносок, выводишь из себя. Смотри, как бы не оказаться в нижнем помещении форта...", "block", 1, npchar, Dialog.CurrentNode);
			link.l1 = HeroStringReactionRepeat(RandPhraseSimple("Я передумал...", "Сейчас мне не о чем говорить"), "Не пристало...",
                      "Да уж, действительно в третий раз...", "Извиняюсь...", npchar, Dialog.CurrentNode);
			link.l1.go = "exit";
		break;
	}
	UnloadSegment(NPChar.FileDialog2);  // если где-то выход внутри switch  по return не забыть сделать анлод
}

