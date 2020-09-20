
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
			dialog.text = "Ќам больше не о чем с вами говорить.";
			link.l1 = " ак скажешь.";
			link.l1.go = "exit";
			
			NextDiag.TempNode = "First time";
		break;

		case "Smuggler_offer":
			dialog.text = " апитан, насколько мне известно, у вас на борту наход€тс€ товары, которые представл€ют интерес дл€ братства контрабандистов.\n≈сли вы хотите избавитьс€ от них и получить за это хорошую сумму, приплывайте к одному из необитаемых островов и сообщите о своем намерении любому контрабандисткому кораблю, сто€щему там на рейде.";
			link.l1 = "¬озможно, в будущем € последую вашему совету.";
			link.l1.go = "exit";
			NextDiag.TempNode = "Smuggler_offer";
		break;


		case "Smuggler_tutorial":
			dialog.text = "Ёй, капитан, не хотите ли узнать о новом дл€ себ€ зан€тии? Ёто может быть весьма опасно дл€ вашей жизни и репутации, но зато какие прибыли...";
			link.l1 = "ѕочему бы и нет.";
			link.l1.go = "Smuggler_tutorial1";
			link.l2 = "Ќе сейчас.";
			link.l2.go = "exit";
			NextDiag.TempNode = "Smuggler_tutorial";
		break;

		case "Smuggler_tutorial1":
			dialog.text = "“огда слушайте, и € расскажу вам всю правду о контрабанде.\n онтрабандный товар иногда можно найти на захваченных корабл€х - некоторые капитаны промышл€ют этим славным ремеслом. ≈сли у вас в трюме есть такой товар, загл€ните на любой из необитаемых островов, и вы наверн€ка встретите один из наших кораблей.\nѕросто окликните его, перегрузите товар, и взамен вы получите золото.";
			link.l1 = "—пасибо за совет.";
			link.l1.go = "Smuggler_tutorial_exit";			
			NextDiag.TempNode = "First time";
		break;

		case "Smuggler_nothing":
			dialog.text = "Ќе думаю, капитан, что вы захотите воспользоватьс€ моими услугами... ƒа и мен€ ничего из ваших товаров не интересует.";
			link.l1 = "Ќу, на нет и суда нет.";
			link.l1.go = "exit";
			NextDiag.TempNode = "First time";
		break;

		case "Smuggler_tutorial_exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
			CompleteSmugglerTutorial(npchar);
		break;

		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}
}
