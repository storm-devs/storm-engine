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
		break;

		case "Romantic_1":
            dialog.Text = "(Изабелле):  Так вы будете платить или мне позвать солдат?";
			Link.l1 = "Да как ты смеешь, мерзавец!  Немедленно извинись перед сеньорой, или я проткну твое толстое брюхо!";
		    Link.l1.go = "exit";
		    NextDiag.TempNode = "Second time";
		    AddDialogExitQuest("Romantic_TalkInShop_2");
		break;
		
		case "Romantic_2":
            dialog.Text = "Конечно, конечно, сеньор капитан.";
			Link.l1 = "Вот, другой разговор.";
		    Link.l1.go = "exit";
		    NextDiag.TempNode = "Second time";
		    AddDialogExitQuest("Romantic_TalkInShop_2");
		break;

	}
	UnloadSegment(NPChar.FileDialog2);
}

