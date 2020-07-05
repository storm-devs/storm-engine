
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
			if(CheckAttribute(pchar, "sneak.success"))
			{
				if(sti(pchar.sneak.success) == 0)
				{
					dialog.text = "Стоять! Смотрите кто это тут у нас! Стража, на помощь!";
					link.l1 = "О, дьявол!";
					link.l1.go = "exit";
					AddDialogExitQuest("sneak_to_town_failed");
				}
				else
				{
					dialog.text = "Проходите, проходите.";
					link.l1 = "...";
					link.l1.go = "exit";
				}
			}
			else
			{
				dialog.text = "Проходите, проходите.";
				link.l1 = "...";
				link.l1.go = "exit";							
			}

			NextDiag.TempNode = "First time";
		break;

		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}
}
