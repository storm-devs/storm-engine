
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
			dialog.text = "Да, капитан?";
			link.l1 = "Нет, ничего.";
			link.l1.go = "exit";

			
			NextDiag.TempNode = "First time";
		break;

		case "want_fire":
			dialog.text = "Я хотел бы поговорить с вами, капитан.";
			link.l1 = "Да, в чем дело?";
			link.l1.go = "want_fire_1";
			
			NextDiag.TempNode = "First time";
		break;

		case "want_money":
			dialog.text = "Я хотел бы поговорить с вами, капитан.";
			link.l1 = "Да, в чем дело?";
			link.l1.go = "want_money_1";
			
			NextDiag.TempNode = "First time";
		break;

		case "want_money_1":
			dialog.text = "Я хочу пересмотреть условия нашего с вами контракта. Вы платите мне хорошие деньги, не спорю, но с моим опытом и знаниями я легко найду работу получше.";
			link.l1 = "И сколько ты хочешь?";
			link.l1.go = "want_money_2";
		break;

		case "want_money_2":
			dialog.text = "Накиньте мне сотню пиастров, и я останусь с вами. В противном случае мне придется распрощаться с вами.";
			if(sti(pchar.money) >= 100)
			{
				link.l1 = "Хорошо. Договорились.";
				link.l1.go = "money_exit";
			}
			link.l2 = "Вот еще! Даже не подумаю!";
			link.l2.go = "money_decline_exit";
		break;

		case "want_fire_1":
			dialog.text = "Видите ли, я тут подумал, подумал и решил свалить к такой-то матери. Вы уж простите за грубость, но ваша манера вести дела, сидит у меня вот где!\nТак что я схожу на берег, а вы можете искать другого человека на мою должность.";
			link.l1 = "Чтоже, это твое право.";
			link.l1.go = "fire_exit";
		break;

		case "money_decline_exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
			OfficerNotGotMoney(npchar);
		break;

		case "money_exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
			OfficerGotMoney(npchar);
		break;

		case "fire_exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
			OfficerFired(npchar);
		break;

		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}
}
