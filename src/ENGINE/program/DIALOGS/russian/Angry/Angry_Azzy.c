// 11.12.05 Эдди, файл Аззи
void GeneralAngryDialog(aref _pchar, aref npchar, aref Link)
{
    if (CheckAttribute(npchar, "angry") && !CheckAttribute(npchar, "angry.ok"))
    {
        npchar.angry.ok = 1;
        switch (npchar.angry.kind)
        {
            case "quest":
                Dialog.CurrentNode = "AngryQuest";
            break;
        }
    }
	switch(Dialog.CurrentNode)
	{
		case "AngryQuest":
            dialog.text = GetFullName(_pchar)+ ", в этом мире есть гораздо более интересные и приятные вещи, нежели общение с таким занудой.";
			link.l1 = "Послушай...";
		    link.l1.go = "AngStep_1";
    	break;

		case "AngStep_1":
            dialog.text = "И слушать не буду! Прощай, смертный...";
			link.l1 = "...";
		    link.l1.go = "Back_Exit";
            DeleteAttribute(npchar, "angry.ok");
            Dialog.CurrentNode = "Back_Exit";
    	break;
	}
}

