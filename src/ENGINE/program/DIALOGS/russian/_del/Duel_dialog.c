

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
		case "Drink duel":
			dialog.text = "Я тебя уже заждался! Давай же обнажим оружие, и пусть удйет отсюда только один.";
			link.l1 = "Аминь. Надеюсь ты успел зайти в церковь исповедаться.";
			link.l1.go = "exit";
			
			NextDiag.TempNode = "First time";

			AddDialogExitQuest("Drink_Duel");
		break;

		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}
}
