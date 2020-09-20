void ProcessDialogEvent()
{
	ref NPChar;
	aref Link, NextDiag;

	DeleteAttribute(&Dialog,"Links");

	makeref(NPChar,CharacterRef);
	makearef(Link, Dialog.Links);
	makearef(NextDiag, NPChar.Dialog);

	ref PChar;
	PChar = GetMainCharacter();

    string ret;
    
	switch(Dialog.CurrentNode)
	{
		case "exit":
			NextDiag.CurrentNode = NextDiag.Tempnode;
			DialogExit();
		break;

		case "start":
			dialog.text = "Просыпайся, " + Pchar.name + "! Мы наконец добрались до "+ Pchar.HeroParam.Land +" после всех этих штормов!"+
                          " Что собираешься делать? Корабля-то у тебя больше нет.";
			link.l1 = "Не в первый раз! Удача - переменчивая штука.";
			link.l1.go = "start1";
			link.l2 = "Хочешь мне предложить работу, Абрахам?";
			link.l2.go = "start2";
		break;

		case "start1":
			dialog.text = "Надеюсь, тебе повезет больше чем с 'Меркурием'. Прощай.";
			link.l1 = "Не сомневаюсь. Прощай, капитан!";
			link.l1.go = "game_begin";
		break;
		
		case "start2":
            ret = Pchar.name + ", я контрабандист, а не мэр города. Какая у меня для тебя работа?";
            if (sti(Pchar.nation) == PIRATE)
            {
                dialog.text = ret + " Загляни в таверну к Инес. Она знает всех на этом острове."+
                              " Зайди к Юрксену, он здесь у пиратов вместо губернатора. Но сначала поговори с простыми пиратами.";
            }
            else
            {
                dialog.text = ret + " Займи где-нибудь денег, продай что-нибудь. Не мне тебя учить.";
            }
            link.l1 = "Ладно, спасибо за совет. Прощай.";
			link.l1.go = "game_begin";
			link.l2 = "Знаешь, Абрахам, я, пожалуй, реквизирую твой корабль. Мне он очень понравился!";
			link.l2.go = "start3";
		break;
		
		case "game_begin":
			DialogExit();
            addDialogExitQuest("Tut_SkipTutorialOnShip");
		break;
		
		case "start3":
			dialog.text = "Ты плохо шутишь, приятель.";
			link.l1 = "Не бери в голову, старик. Прощай!";
			link.l1.go = "game_begin";
			link.l2 = "А кто сказал, что я шучу? Ты скажешь команде, что продал мне свою посудину. А я оставлю тебе жизнь. Договорились?";
			link.l2.go = "start4";
		break;
		
		case "start4":
			dialog.text = "Говорили, что ты убил своего учителя Малькольма Хатчера из-за паршивой сабли. Не верил, а зря. Сегодня ты будешь кормить рыб. У тебя нет ни шанса, приятель.";
			link.l1 = "Неужели?!";
			link.l1.go = "exit";
			addDialogExitQuest("Tut_KillOnShip");
		break;

		case "First time":
			dialog.text = "";
			Link.l1 = "Упс...";
			Link.l1.go = "exit";
		break;

////////////////////////////////////////////////////////////////////////////////
//	Корсарское метро
////////////////////////////////////////////////////////////////////////////////
		case "Travel_talkStart":
            NPChar.location = "none"; // чтоб на палубе не болтался
			//Шанс на то что продадут на рудники.
			int iRnd = (rand(100) == 30);
			if (iRnd)
			{
				dialog.text = "Мы тут подумали, что может стоит продать тебя на рудники. Ха... деньги лишними не бывают.";
				link.l1 = "Что вы говорите?";
				//заглушка, пока нет рудников.
				link.l1.go = "Travel_fight";
				//"Travel_mine"; //Собственно тоже можно боевку организовать, ГГ сопротивляется.
			}
			else
			{
				dialog.text = "Мы приблыли на место, прощай.";
				link.l1 = "Прощай.";
				link.l1.go = "Travel_end";
				link.l2 = "Я тут подумал, нравится мне твой корабль. Хочу оставить его себе.";
				link.l2.go = "Travel_fight";
			}
		break;

		case "Travel_fight":
			ChangeContrabandRelation(pchar, -60);

			LAi_group_MoveCharacter(NPChar, "TmpEnemy");
			LAi_group_SetCheck("TmpEnemy", "Travel_AfterDeckFight");
			LAi_group_FightGroups(LAI_GROUP_PLAYER, "TmpEnemy", true);
			LAi_SetPlayerType(PChar);
			//Вытащим саблю
   			LAi_SetFightMode(Pchar, true);

			NextDiag.CurrentNode = NextDiag.Tempnode;
			DialogExit();
			LAi_SetWarriorTypeNoGroup(NPChar);
		break;

		case "Travel_end":
			NextDiag.CurrentNode = NextDiag.Tempnode;
			DialogExit();
			//Квест бук
			AddQuestRecord("Gen_ContrabandTravel", "4");
			AddQuestUserData("Gen_ContrabandTravel", "sLocTo", GetConvertStr(pchar.GenQuest.contraTravel.destination.loc, "LocLables.txt")));


			LAi_SetPlayerType(PChar);
			//грузим ГГ куда нужно.... 
			setWDMPointXZ(pchar.GenQuest.contraTravel.destination.loc);
			SetAnyReloadToLocation(pchar.GenQuest.contraTravel.destination.loc,
									pchar.GenQuest.contraTravel.destination.group,
									pchar.GenQuest.contraTravel.destination.locator, "", 0, 0, 0, 0);
			AddDialogExitQuest("AnyReloadToLocation");
            chrDisableReloadToLocation = false;
			CloseQuestHeader("Gen_ContrabandTravel");
			//трем аттрибуты
			DeleteAttribute(PChar, "GenQuest.contraTravel");
		break;
	}
}