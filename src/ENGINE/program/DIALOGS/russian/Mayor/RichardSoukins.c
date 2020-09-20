// Соукинс в Пуэрто-Принсипе
void ProcessDialogEvent()
{
	ref NPChar, sld;
	aref Link, NextDiag;
	int i;

	DeleteAttribute(&Dialog,"Links");

	makeref(NPChar,CharacterRef);
	makearef(Link, Dialog.Links);
	makearef(NextDiag, NPChar.Dialog);

	switch(Dialog.CurrentNode)
	{
		// ----------------------------------- Диалог первый - первая встреча
		case "First time":
            dialog.text = "У тебя дело ко мне? Нет? Тогда вали отсюда!";
			link.l1 = "Уже ухожу.";
			link.l1.go = "exit";
			NextDiag.TempNode = "First time";
			if (pchar.questTemp.piratesLine == "Soukins_toPuertoPrincipe")
			{
				link.l1 = "Ричард, слышал я, что ты собираешь людей для акции против испанцев. Я хотел бы принять в этом участие.";
				link.l1.go = "PL_Q7_1";
			}
		break;

 		case "Exit":
			DialogExit();
			NextDiag.CurrentNode = NextDiag.TempNode;
		break;

		case "fight":
			DialogExit();
			NextDiag.CurrentNode = NextDiag.TempNode;
			LAi_group_Attack(NPChar, Pchar);
			AddDialogExitQuest("MainHeroFightModeOn");
		break;
		//пиратка, квест №7
		case "PL_Q7_1":
			dialog.text = "Назови свое имя.";
			link.l1 = GetFullName(pchar) + ".";
			link.l1.go = "PL_Q7_2";
		break;
		case "PL_Q7_2":
			dialog.text = "Хе-хе, а не прихвостень ли ты Моргана, приятель?";
			link.l1 = "Я работаю на Генри, при чем тут прихвостень?";
			link.l1.go = "PL_Q7_3";
		break;
		case "PL_Q7_3":
			dialog.text = "А при том, что ты мне не подходишь. Эй, на вахте! Ну ка, все сюда! Валите эту шестерку...";
			link.l1 = "Ну зачем ты так?..";
			link.l1.go = "fight";
			LAi_SetPlayerType(pchar);
			pchar.questTemp.piratesLine = "Soukins_FightinResidence";
			for (i=1; i<=5; i++)
			{
				iTemp = 10 + rand(10);
				sld = GetCharacter(NPC_GenerateCharacter("questPirate_"+i, "pirate_"+i, "man", "man", 25, PIRATE, 10, true));
				FantomMakeCoolFighter(sld, 25, 70, 70, "blade26", "pistol6", 20);
				LAi_SetWarriorType(sld);
				LAi_warrior_SetStay(sld, true);
				LAi_group_MoveCharacter(sld, npchar.chr_ai.group);
				ChangeCharacterAddressGroup(sld, pchar.location, "reload", "reload1");
			}
		break;
		case "PL_Q7_5":
			dialog.text = "Какого черта ты вваливаешься через окно?!! А-а-а, я, кажется, знаю тебя... Эй, на вахте, Ну ка, все сюда! Валите этого прихвостня!";
			link.l1 = "Эй-эй, что за дела?!";
			link.l1.go = "fight";
			LAi_SetPlayerType(pchar);
			pchar.questTemp.piratesLine = "Soukins_FightinResidence";
			for (i=1; i<=5; i++)
			{
				iTemp = 10 + rand(10);
				sld = GetCharacter(NPC_GenerateCharacter("questPirate_"+i, "pirate_"+i, "man", "man", 25, PIRATE, 10, true));
				FantomMakeCoolFighter(sld, 25, 70, 70, "blade26", "pistol6", 20);
				LAi_SetWarriorType(sld);
				LAi_warrior_SetStay(sld, true);
				LAi_group_MoveCharacter(sld, npchar.chr_ai.group);
				ChangeCharacterAddressGroup(sld, pchar.location, "reload", "reload1");
			}
		break;
		//пиратка, квест №8
		case "PL_Q8_deck":
			dialog.text = "И как это Морган умудрился поставить такого оболтуса в командование эскадрой? Ума не приложу...";
			link.l1 = "Хм, зря ты так, Ричард.";
			link.l1.go = "exit";
			NextDiag.TempNode = "PL_Q8_deck_1";
		break;
		case "PL_Q8_deck_1":
			dialog.text = "Ну вот, опять ты. О чем я и говорю!";
			link.l1 = "Ты за меня не переживай.";
			link.l1.go = "exit";
			NextDiag.TempNode = "PL_Q8_deck_1";
		break;

		case "PL_Q8_deck_2":
			dialog.text = "Ну и что ты опять делаешь у меня на палубе? Думаешь, если ты удачно провел штурм Порто Белло, то теперь можно павлином тут разгуливать?";
			link.l1 = "Павлины-то тут при чем?..";
			link.l1.go = "PL_Q8_deck_3";
		break;
		case "PL_Q8_deck_3":
			dialog.text = "Слушай, шел бы ты отсюда... Мне с тобой не о чем говорить. Все решиться в Панаме, пусть каждый там и покажет, на что он способен.";
			link.l1 = "Ты это о чем?";
			link.l1.go = "PL_Q8_deck_4";
		break;
		case "PL_Q8_deck_4":
			dialog.text = "Я тебе отчета давать не намерен, ты мне другом не являешься. Так что убирайся отсюда к чертям собачьим.";
			link.l1 = "Ну ладно, я пошел. Не хочешь разговаривать - как хочешь...";
			link.l1.go = "exit";
			NextDiag.TempNode = "PL_Q8_deck_5";
			npchar.DeckDialogNode = "PL_Q8_deck_5";
		break;
		case "PL_Q8_deck_5":
			dialog.text = "Я не желаю с тобой говорить!";
			link.l1 = "Хм, ну это понятно...";
			link.l1.go = "exit";
			NextDiag.TempNode = "PL_Q8_deck_5";
			npchar.DeckDialogNode = "PL_Q8_deck_5";
		break;
		//в бухте Дарьенский залив
		case "Shore48":
			dialog.text = "Ну что же, до залива мы добрались. Теперь нужно двигаться через джунгли. Я и мои люди готовы, можем выступать.";
			link.l1 = "Хорошо, я понял тебя.";
			link.l1.go = "Shore48_1";
		break;
		case "Shore48_1":
			LAi_LocationFightDisable(&Locations[FindLocation("Shore48")], false); 
			chrDisableReloadToLocation = false; 
			LAi_SetWarriorTypeNoGroup(npchar);
			NextDiag.TempNode = "Shore48_over";
            NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
		case "Shore48_over":
			dialog.text = "Чего мы ждем? Давай, двигай свою задницу к Панаме!!";
			link.l1 = "Хм, это конечно...";
			link.l1.go = "exit";
		break;
		//после штурма у стен Панамы
		case "PQ8_exitTown":
			dialog.text = "Что тебе нужно?";
			link.l1 = "Послушай, я знаю, что тебе неприятно будет это услышать, но Морган приказал мне убить тебя, когда мы шли через джунгли.";
			link.l1.go = "PQ8_exitTown_1";
		break;
		case "PQ8_exitTown_1":
			dialog.text = "И ты думаешь я тебе поверю?";
			link.l1 = "Можешь не верить, но это правда.";
			link.l1.go = "PQ8_exitTown_2";
		break;
		case "PQ8_exitTown_2":
			dialog.text = "Ты не раз лез в мои дела, портил мои замыслы. Ты - шестерка Моргана. И теперь хочешь пошестерить на меня, что ли?";
			link.l1 = "Да пошел ты, Ричард... Идиот!";
			link.l1.go = "PQ8_exitTown_3";
		break;
		case "PQ8_exitTown_3":
			dialog.text = "Я-то, конечно, пойду... в Панаму. А вот ты в город не войдешь. Так что вали отсюда, пока цел...";
			link.l1 = "Дьявол!..";
			link.l1.go = "exit";
			LAi_warrior_DialogEnable(npchar, false);
		break;
	}
}
