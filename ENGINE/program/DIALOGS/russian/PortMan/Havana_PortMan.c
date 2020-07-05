// диалог по городам
void ProcessCommonDialogEvent(ref NPChar, aref Link, aref NextDiag)
{
    switch (Dialog.CurrentNode)
	{
		case "quests":
			dialog.text = NPCStringReactionRepeat(RandPhraseSimple(" акие вопросы?", "„то вам угодно, " + GetAddress_Form(NPChar) + "?"), "—овсем недавно вы пытались задать мне вопрос " + GetAddress_Form(NPChar) + "...", "¬ течение этого дн€ вы уже третий раз говорите о каком-то вопросе...",
                          "ѕослушайте, если вы не по делам порта, то не стоит мен€ вопросами донимать.", "block", 1, npchar, Dialog.CurrentNode);
			link.l1 = HeroStringReactionRepeat(RandPhraseSimple("я передумал...", "—ейчас мне не о чем говорить"), "’м, что-то с пам€тью моей стало...",
                      "ƒа уж, действительно в третий раз...", "»звините, но портовые дела мен€ сейчас не интересуют.", npchar, Dialog.CurrentNode);
			link.l1.go = "exit";
		break;

	}
	UnloadSegment(NPChar.FileDialog2);  // если где-то выход внутри switch  по return не забыть сделать анлод
}

