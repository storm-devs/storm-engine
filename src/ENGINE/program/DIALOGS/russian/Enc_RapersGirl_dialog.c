void ProcessDialogEvent()
{
	ref NPChar, sld;
	aref Link, Diag;
	int i;
	
	DeleteAttribute(&Dialog,"Links");

	makeref(NPChar,CharacterRef);
	makearef(Link, Dialog.Links);
	makearef(Diag, NPChar.Dialog);
	
	switch(Dialog.CurrentNode)
	{
		case "exit_1":
			AddDialogExitQuest("LandEnc_RapersTalk");
			Diag.CurrentNode = Diag.TempNode;
			DialogExit();			
		break;
		
		case "exit":
			LAi_SetCitizenTypeNoGroup(npchar);
			Diag.CurrentNode = Diag.TempNode;
			DialogExit();
		break;

		case "First time":
			LAi_RemoveCheckMinHP(npchar);
			for(i = 1; i <= 3; i++)
			{
				if (GetCharacterIndex("GangMan_" + i) == -1) continue;
				sld = CharacterFromID("GangMan_" + i);
				LAi_type_actor_Reset(sld);
			}
			dialog.text = LinkRandPhrase("Помогите! Помогите! Эти люди преследуют меня! Спасите!",
				"А-а! На помощь, незнакомец! Спасите меня от этих мерзавцев!",
				"Будьте милосердны, защитите меня от этих негодяев! Прошу вас!");
			Link.l1 = LinkRandPhrase("Что происходит?",
				"В чем дело?",
				"Что случилось?");
			Link.l1.go = "Node_2";			
		break;        

		case "Node_2":
			dialog.text = LinkRandPhrase("Они...они хотят надругаться надо мной! Умоляю вас, не позволяйте им этого! Спасите меня!",
				"Эти звери...они хотят сделать что-то ужасное... Защитите меня от них, пожалуйста!",
				"Во имя Господа нашего - защитите меня от этих похотливых чудовищ! Они хотят обесчестить меня!");
			Link.l1 = "Так, сейчас разберемся.";
			Link.l1.go = "exit_1";			
		break;

		case "ThanksForHelp":
			dialog.text = LinkRandPhrase("О! Благодарю вас за помощь!",
				"Вы спасли меня! Я так вам благодарна!",
				"Вы настоящий рыцарь! Спасибо вам!");
			Link.l1 = "Только так и надлежит поступать с такими мерзавцами!";
			Link.l1.go = "ThanksForHelp_1";			
		break;
		
		case "ThanksForHelp_1":
			Diag.TempNode = "ThanksAgain";
			if(makeint(Pchar.reputation) >= 80)
			{
				dialog.text = "У меня есть немного денег. Возьмите их в знак моей благодарности!";
				Link.l1 = "Спасибо. Будьте осторожнее в следующий раз.";
				Link.l1.go = "exit";
			}
			else 
			{
				if(Rand(1) == 0)
				{
					dialog.text = "Я всем расскажу, что вы меня спасли! Пусть все знают, какой вы храбрый и добрый человек!";
					Link.l1 = "Спасибо. Теперь вам лучше отправляться домой.";
					Link.l1.go = "exit";
				}
				else
				{
					addMoneyToCharacter(Pchar, makeint(PChar.rank)*100);
					dialog.text = "У меня есть немного денег. Возьмите их в знак моей благодарности!";
					Link.l1 = "Спасибо. Будьте осторожнее в следующий раз.";
					Link.l1.go = "exit";
				}
			}
		break;

		case "ThanksAgain":
            Diag.TempNode = "ThanksAgain";
            dialog.text = "Я всем расскажу, что вы меня спасли! Пусть все знают, какой вы храбрый и добрый человек!";
			Link.l1 = "Спасибо. Теперь вам лучше отправляться домой.";
			Link.l1.go = "exit";
		break;
	}
}
