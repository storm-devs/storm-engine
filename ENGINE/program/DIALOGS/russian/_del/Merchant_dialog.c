
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
			dialog.text = "Хотите прикупить снаряжение для себя или своих офицеров, капитан? Тогда вы обратились по адресу!";
			link.l1 = "Покажи, что там у тебя есть.";
			link.l1.go = "Trade_exit";
			link.l2 = "Я по другому делу.";
			link.l2.go = "quests";
			
			NextDiag.TempNode = "First time";
		break;


		case "Trade_exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
			LaunchItemsTrade(npchar);
		break;

		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}
}
