
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
			dialog.text = "Добро пожаловать в церковь.";
			link.l1 = "Мне хотелось бы исповедаться и сделать подношение, отец.";
			link.l1.go = "donate";
			link.l2 = "Извините, я уже ухожу.";
			link.l2.go = "exit";


			NextDiag.TempNode = "First time";
		break;

		case "donate":
			dialog.text = "Сей поступок Богу угоден. Сколько же ты хочешь пожертвовать?";
			if(sti(pchar.money) >= 1000)
			{
				link.l1 = "1000 золотых.";
				link.l1.go = "donate1000";
			}
			if(sti(pchar.money) >= 5000)
			{
				link.l2 = "5000 золотых.";
				link.l2.go = "donate5000";
			}
			if(sti(pchar.money) >= 10000)
			{
				link.l3 = "10000 золотых.";
				link.l3.go = "donate10000";
			}
			link.l4 = "Эм... Думаю, я отложу это до следующего посещения церкви.";
			link.l4.go = "exit";
		break;

		case "donate1000":
			dialog.text = "Господь прощает тебе твои грехи.";
			link.l1 = "О да, я чувствую, что после исповеди мне стало легче.";
			link.l1.go = "donateExit";
			npchar.donate = 1000;
		break;

		case "donate5000":
			dialog.text = "Господь прощает тебе твои грехи.";
			link.l1 = "О да, я чувствую, что после исповеди мне стало легче.";
			link.l1.go = "donateExit";
			npchar.donate = 5000;
		break;

		case "donate10000":
			dialog.text = "Господь прощает тебе твои грехи.";
			link.l1 = "О да, я чувствую, что после исповеди мне стало легче.";
			link.l1.go = "donateExit";
			npchar.donate = 10000;
		break;

		case "donateExit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
			MakeDonate(npchar);
		break;

		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}
}
