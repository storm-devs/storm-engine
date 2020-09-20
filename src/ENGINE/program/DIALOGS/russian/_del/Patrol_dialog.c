
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
			dialog.text = "Проходите, проходите.";
			link.l1 = "...";
			link.l1.go = "exit";			

			NextDiag.TempNode = "First time";
		break;

		case "Fight_with_enemy":
			dialog.text = "Враг в городе! К оружию!";
			link.l1 = "О, дьявол!";
			link.l1.go = "exit";
			AddDialogExitQuest("sneak_to_town_failed");
			NextDiag.TempNode = "First time";
		break;

		case "Fight_with_contaminator":
			dialog.text = "Эй! Да ты та самая сволочь, которая заразила наш город! К оружию!";
			link.l1 = "О, дьявол!";
			link.l1.go = "exit";
			AddDialogExitQuest("sneak_to_town_failed");
			NextDiag.TempNode = "First time";
		break;

		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}
}
