// диалог по городам
void ProcessCommonDialogEvent(ref NPChar, aref Link, aref NextDiag)
{
    switch (Dialog.CurrentNode)
	{
		case "quests":
			dialog.text = NPCStringReactionRepeat(RandPhraseSimple("Какие имеете вопросы к солдату?", "У солдата жизнь расписана - караулы, разводы... Что вам угодно, " + GetAddress_Form(NPChar) + "?"), "Не пристало приставать к солдату с глупыми вопросами, " + GetAddress_Form(NPChar) + "...", "Третий раз за сегодня вы пытаетесь задать вопрос...",
                          "Солдатская доля и без того непроста, а тут еще и ты, недоносок, выводишь из себя...", "block", 1, npchar, Dialog.CurrentNode);
			link.l1 = HeroStringReactionRepeat(RandPhraseSimple("Я передумал...", "Сейчас мне не о чем говорить"), "Не пристало...",
                      "Да уж, действительно в третий раз...", "Извиняюсь...", npchar, Dialog.CurrentNode);
			link.l1.go = "exit";
		break;
	}
	UnloadSegment(NPChar.FileDialog2);  // если где-то выход внутри switch  по return не забыть сделать анлод
}

