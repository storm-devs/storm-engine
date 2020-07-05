void ProcessDialogEvent()
{
	ref NPChar, sld;
	aref Link, Diag;
	int i;
	string sGroup;
	
	DeleteAttribute(&Dialog,"Links");

	makeref(NPChar,CharacterRef);
	makearef(Link, Dialog.Links);
	makearef(Diag, NPChar.Dialog);
	int iTemp = sti(npchar.EncQty);
	string sTemp = "Gang" + locations[FindLocation(npchar.location)].index + "_";

	switch(Dialog.CurrentNode)
	{
		case "exit":
			Diag.CurrentNode = Diag.TempNode;
			DialogExit();			
		break;

		case "exit_Robbed":
			ChangeCharacterReputation(pchar, -1);
			int iMoney = makeint(makeint(Pchar.money)/20)*10;
			AddMoneyToCharacter(pchar, -iMoney);
			AddMoneyToCharacter(npchar, iMoney);
			npchar.SaveItemsForDead   = true; // сохранять на трупе вещи
			npchar.DontClearDead = true;  // не убирать труп через 200с
			AddSimpleRumour(LinkRandPhrase("Вы слышали, у известного местного грабителя по имени " + GetFullName(npchar) + " появилась очередная жертва - некий капитан. И этот кэп струтил и откупился. По слухам, остегнул " + FindRussianMoneyString(iMoney) + ", хи-хи... ", 
				"Вы знаете, капитан " + GetFullName(pchar) + " отдал " + FindRussianMoneyString(iMoney) + ", чтобы откупиться от бандита по имени " + GetFullName(npchar) + " А-а, это же вы тот капитан! Ну что я могу сказть - не повезло! Ха-ха-ха.", 
				"Вы не слышали о том, местные грабители напали на капитана " + GetMainCharacterNameGen() + "? Так вот, его оргабили на целых " + FindRussianMoneyString(iMoney) + "!"), sti(npchar.nation), 5, 1);
			for(i = 0; i < iTemp; i++)
			{
				sld = CharacterFromID(sTemp + i);
				LAi_SetWarriorTypeNoGroup(sld);
				sld.Dialog.CurrentNode = "OnceAgain";
				LAi_SetCheckMinHP(sld, LAi_GetCharacterHP(sld)-1, true, "LandEnc_RaidersBeforeDialog");
			}
			DialogExit();			
		break;

		case "exit_fight":
			sGroup = "RaidersGroup_" + locations[FindLocation(npchar.location)].index;
			for(i = 0; i < iTemp; i++)
			{
				sld = CharacterFromID(sTemp + i);
				LAi_SetWarriorTypeNoGroup(sld);
				LAi_group_MoveCharacter(sld, sGroup);
				LAi_RemoveCheckMinHP(sld);
				LAi_SetImmortal(sld, false);
			}
			LAi_group_SetRelation(sGroup, LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
			LAi_group_FightGroups(sGroup, LAI_GROUP_PLAYER, true);
			LAi_group_SetCheck(sGroup, "LandEnc_RaidersAfrer");
			DialogExit();	
			AddDialogExitQuest("MainHeroFightModeOn");
		break;

		case "exit_RunFight":
			AddSimpleRumour(LinkRandPhrase("Вы слышали, какой-то капитан наконец-то образумил бандита по имени " + GetFullName(npchar) + ". Они хотели его ограбить, а в итоге сами наложили в штаны, ха-ха!", 
				"Вы знаете, капитан " + GetFullName(pchar) + " разогнал шайку бандита по имени " + GetFullName(npchar) + " А-а, это же вы тот капитан! Прекрасная работа...", 
				"Вы не слышали о том, что местный бандит по имени " + GetFullName(npchar) + " пытался ограбить капитана " + GetMainCharacterNameGen() + "? Так вот, они попытались его ограбить, но не тут то было, капитан - крепкий орешек. Бандиты разбежались кто куда!"), sti(npchar.nation), 5, 1);
			for(i = 0; i < iTemp; i++)
			{
				sld = CharacterFromID(sTemp + i);
				LAi_type_actor_Reset(sld);
				LAi_ActorAfraid(sld, Pchar, true);
				sld.Dialog.CurrentNode = "GetTheHellOut";
			}
			DialogExit();
		break;

		case "exit_noFight":
			for(i = 0; i < iTemp; i++)
			{
				sld = CharacterFromID(sTemp + i);
				LAi_SetCitizenTypeNoGroup(sld);
				sld.Dialog.CurrentNode = "GetTheHellOut";
				LAi_SetCheckMinHP(sld, LAi_GetCharacterHP(sld)-1, true, "LandEnc_RaidersBeforeDialog");
			}		
			DialogExit();			
		break;

		case "First Time":
			for(i = 0; i < iTemp; i++)
			{
				sld = CharacterFromID(sTemp + i);
				LAi_type_actor_Reset(sld);
				LAi_RemoveCheckMinHP(sld);
				LAi_SetImmortal(sld, false);
			}
			dialog.text = LinkRandPhrase("Стой! Брось оружие! Раскошеливайся, иначе расстанешься с головой!",
				"Эй, путник! Притормози! Хочу увидеть твой кошелек!",
				"Стой, мерзавец! Хочешь пройти этой дорогой - плати!");
			Link.l1 = LinkRandPhrase("Что вы несете?","В чем дело?", "Что ты имеешь в виду?");
			Link.l1.go = "Node_2";
			Link.l2 = LinkRandPhrase("А-а, разбойники! Преступность надо искоренять. К бою!",
				"Терпеть не могу нахалов, да еще немытых. Приготовься к смерти!",
				"За подобную дерзость я вырежу твое сердце из груди!");
			Link.l2.go = "CheckSkills";	
		break;
		
		case "Node_2":
			dialog.text = LinkRandPhrase("О, да он не понимает! Гони все золото, что у тебя есть! Тогда подумаю, чтобы отпустить тебя!",
				"Слышал о дорожных издержках? Так вот это они и есть! Не расстанешься со своим золотом - расстанешься со своей головой!",
				"Разъясняю для тупоголовых моряков - выкладывай все деньги, что у тебя есть! Я так люблю фунты и дублоны!");
			Link.l1 = "Проклятье! Мерзавец! У меня с собой только " + makeint(makeint(Pchar.money)/20)*10 + " пиастров.";
			Link.l1.go = "CheckMoney";
			Link.l2 = "Я не отдам свои деньги без боя!";
			Link.l2.go = "CheckSkills";			
		break;

		case "CheckSkills":
			if (GetCharacterSkillToOld(Pchar, "Fencing") >= 7 && makeint(Pchar.Rank) >= 8 && Makeint(PChar.reputation) <= 30)
			{
				Diag.TempNode = "GetLost";
				dialog.text = LinkRandPhrase("Эй, не нервничай так, приятель! Я пошутил! Иди своей дорогой!",//15
					"Ладно, я передумал. Ты можешь продолжать свой путь, никто не станет тебя грабить.",
					"Ты смелый человек! Поэтому я тебя отпущу...");
				Link.l1 = LinkRandPhrase("Ну что же, тогда до встречи!",
					"Бросили бы ты это дурное занятие! Прощай.",
					"Рад, что ты принял мудрое решение. Счастливо оставаться!");
				Link.l1.go = "Exit_NoFight";
				Link.l99 = LinkRandPhrase("Поздно отступать - тебя ждет встреча с создателем.",
					"Струсил? Но я уже не могу отпустить тебя живыми!",
					"Никуда я пойду, пока не закончу это дело.");
				Link.l99.go = "Exit_RunFight";
			}
			else 
			{
				dialog.text = LinkRandPhrase("А ведь мог бы уйти с миром! А теперь умрешь!",
					"Зря ты это затеял, каналья! Умри!",
					"Ах так! Я порву тебя на куски! Вырежу твою печень и поджарю на медленном огне!");
				Link.l1 = "Попробуй!";
				Link.l1.go = "Exit_Fight";
			}
		break;

		case "CheckMoney":
			if(makeint(makeint(Pchar.money)/20)*10 >= makeint(Pchar.rank)*100)
			{
				Diag.TempNode = "OnceAgain";
				dialog.text = LinkRandPhrase("Хорошо! Давай их сюда скорее и можешь убираться!",
					"Негусто, но лучше, чем ничего. Приятно иметь дело со сговорчивыми людьми! Ты можешь идти.",
					"В благодарность за твое золото дам тебе совет - не стоит шляться по джунглям, если ты такой слабак! Сиди в порту, в таверне, и пей свой ром - целее будешь.");
				Link.l1 = "Проклятье на ваши головы!";
				Link.l1.go = "Exit_Robbed";				
			}
			else
			{
				dialog.text = "Сдается мне, ты хочешь меня надуть! Но ничего, сейчас я тебя пощекочю. А с трупа и золотишко удобнее подобрать.";
				Link.l1 = "Проклятье!";
				Link.l1.go = "Exit_Fight";				
			}				
		break;

		case "OnceAgain":
			Diag.TempNode = "OnceAgain";
			dialog.text = LinkRandPhrase("Снова ты? Ты же должен сейчас бежать так, чтобы пятки сверкали! А то я могу и пожалеть, что отпустил тебя.",
				"Неужто в твоих карманах завалялось еще золото, и ты пришёл с нами поделиться? Как мило! Ха-ха!",
				"Приятель, ты мне надоел! Убирайся с глаз моих долой, или я перережу тебе глотку!");
			Link.l1 = "Да, я уже ухожу.";
			Link.l1.go = "Exit";
			Link.l2 = "Я подумал, и вернулся воздать вам по заслугам!";
			Link.l2.go = "Exit_Fight";
		break;
				
		case "GetLost":
			Diag.TempNode = "GetLost";
			dialog.text = LinkRandPhrase("Что тебе нужно? Мы же договорились, что ты уходишь!",
				"Отстань от меня, парень. У меня свои дела, у тебя свои, нам не стоит продолжать знакомство!",
				"Уходи, приятель! Черт возьми, неужели ты все-таки хочешь отдать свои деньги?");
			Link.l1 = "Да, я уже ухожу.";
			Link.l1.go = "Exit";
			Link.l2 = "Я решил вас покарать за ваши злодеяния!";
			Link.l2.go = "Exit_Fight";
		break;

		case "GetTheHellOut":
			Diag.TempNode = "GetTheHellOut";
			dialog.text = LinkRandPhrase("Отстань! Убирайся! Я хочу жить!",
				"Помогите! Он маньяк! Не убивай меня!",
				"Пощади! Я всего лишь скромный разбойник! Я слишком молод, чтобы умереть!");
			Link.l1 = "Сидел бы ты дома - жил бы ты дольше!";
			Link.l1.go = "Exit";			
		break;
	}
}
