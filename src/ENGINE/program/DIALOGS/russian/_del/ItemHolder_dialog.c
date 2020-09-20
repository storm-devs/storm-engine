
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
			dialog.text = "Не мешайте мне накачиваться ромом!";
			link.l1 = "Как скажешь.";
			link.l1.go = "exit";
			
			NextDiag.TempNode = "First time";
		break;

		case "Take_item"://взять предмет
			dialog.text = SelectTakeItemDialog(npchar);
			link.l1 = "Прощайте и спасибо, за ваше ожидание.";
			link.l1.go = "take_item_exit";
			NextDiag.TempNode = "First time";
		break;

		case "Give_item"://отдать предмет
			dialog.text = SelectGiveItemDialog(npchar);
			link.l1 = "Прощай.";
			link.l1.go = "give_item_exit";
			NextDiag.TempNode = "First time";
		break;

		case "take_item_exit":
			PrepareCompleteDeliveryItemQuest(npchar);
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;

		case "give_item_exit":
			PrepareCompleteDeliveryItemQuest(npchar);
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;

		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}
}
