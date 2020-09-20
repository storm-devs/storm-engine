
void ProcessDialogEvent()
{
	ref NPChar;
	aref Link, NextDiag;

	DeleteAttribute(&Dialog,"Links");

	makeref(NPChar,CharacterRef);
	makearef(Link, Dialog.Links);
	makearef(NextDiag, NPChar.Dialog);

	switch(Dialog.CurrentNode)
	{
		case "First time":
			dialog.text = "Чего вам от меня нужно?!";
			link.l1 = "Только удостовериться в том, что тебя зовут " + GetCharacterFullName(npchar.id) + ".";
			link.l1.go = "node_1";

			NextDiag.TempNode = "First time";
		break;

		case "node_1":
			dialog.text = "Это я. Какое у тебя ко мне дело?";
			link.l1 = "Мне приказано убить тебя. Я, пожалуй, я это сделаю.";
			link.l1.go = "fight";
			link.l2 = "Вообще-то, мне надо убить тебя, но ты можешь откупиться...";
			link.l2.go = "trade";
		break;

		case "fight":
			dialog.text = "Отлично! Давай покончим с этим! Защищайся!";
			link.l1 = "Защищайся?! Я предпочту напасть. Готовься к смерти.";
			link.l1.go = "exit";
			AddDialogExitQuest("quest_fight_kill_character");
		break;

		case "trade":
			if (sti(pchar.skill.sneak) < rand(13) && sti(pchar.skill.leadership) < rand(13) &&sti(pchar.skill.commerce) <rand(13))
			{
				dialog.text = "Вот еще! Мне проще убить тебя!";
				link.l1 = "Как скажешь. Надеюсь, ты был у священника этим утром.";
				link.l1.go = "exit";
				AddDialogExitQuest("quest_fight_kill_character");
			}
			else
			{
				dialog.text = "Заманчивое предложение, а теперь поподробнее.";
				link.l1 = "Все просто: ты отдаешь мне свое кольцо, платишь " + sti(pchar.skill.commerce)*500 + " монет, и я отпускаю тебя.";
				link.l1.go = "trade_2";
			}
		break;

		case "trade_2":
			if (sti(npchar.money) < sti(pchar.skill.commerce)*500)
			{
				dialog.text = "Я и рад бы заплатить, но у меня есть всего " + npchar.money + " пиастров. Если тебе это подойдет...";
				link.l1 = "Подойдет. Давай их сюда.";
				link.l1.go = "trade_min";
				link.l2 = "Что?! Нет уж, мне будет проще убить тебя.";
				link.l2.go = "fight";
			}
			else
			{
				dialog.text = "Договорились. Вот твои деньги и кольцо.";
				link.l1 = "Адьос. Было приятно иметь с тобой дело.";
				link.l1.go = "exit";
				AddDialogExitQuest("give_ring");
				AddMoneyToCharacter(pchar, sti(pchar.skill.commerce)*500);
			}
		break;

		case "trade_min":
			dialog.text = "Держи. Вот деньги, а вот кольцо.";
			link.l1 = "Окей. Было приятно иметь с тобой дело.";
			link.l1.go = "exit";
			AddDialogExitQuest("give_ring");
			AddMoneyToCharacter(pchar, sti(npchar.money));
		break;

		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}
}
