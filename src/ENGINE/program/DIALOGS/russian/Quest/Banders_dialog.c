
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
		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
		case "fight":
            LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
            LAi_group_FightGroups("EnemyFight", LAI_GROUP_PLAYER, true);
			DialogExit();
			AddDialogExitQuest("MainHeroFightModeOn");
		break;
		case "quest_fight":
            LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
            LAi_group_FightGroups("EnemyFight", LAI_GROUP_PLAYER, true);
            LAi_group_SetCheck("EnemyFight", "DeliverToBander_Dead");
			DialogExit();
			AddDialogExitQuest("MainHeroFightModeOn");
		break;

		case "First time":
			dialog.text = "Ты кто такой?!";
			link.l1 = "Тихо, фраер, кипиша не поднимай. Меня послал к тебе твой кореш, что на киче томится. " + pchar.questTemp.jailCanMove.Deliver.name + ", знаешь такого?";
			link.l1.go = "Step_1";			
			NextDiag.TempNode = "Second time";
		break;
		case "Step_1":
			dialog.text = "Чем за базар ответишь?";
			link.l1 = "Маляву велено передать.";
			link.l1.go = "Step_2";
			TakeItemFromCharacter(pchar, "Malyava");
		break;
		case "Step_2":
			if (sti(pchar.questTemp.jailCanMove.Deliver.good))
			{
				dialog.text = "За маляву спасибо. Что там у тебя в карманах? Давай, выкладывай, нам деньги нужны.";
				link.l1 = "Ты в своем уме?! Это вы мне денег должны за помощь!";
				link.l1.go = "Step_fight";
			}
			else
			{
				dialog.text = "Очень хорошо! За такое дело положена награда - " + FindRussianMoneyString(sti(pchar.questTemp.jailCanMove.Deliver.price)) + ".";
				link.l1 = "Спасибо, неплохо.";
				link.l1.go = "Step_good";
			}
		break;
		case "Step_fight":
			dialog.text = "Мы тебе ничего не должны. Пиастры на бочку, иначе живым отсюда не выйдешь.";
			link.l1 = "Ну, это мы еще поглядим...";
			link.l1.go = "quest_fight";
		break;

		case "Step_good":
			dialog.text = "Забирай и вали отсюда.";
			link.l1 = "Всего хорошего.";
			link.l1.go = "Step_good_exit";
		break;
        case "Step_good_exit":
			AddQuestRecord("GivePrisonFree", "11");
			AddQuestUserData("GivePrisonFree", "iMoney", FindRussianMoneyString(sti(pchar.questTemp.jailCanMove.Deliver.price)));
			CloseQuestHeader("GivePrisonFree");
			LAi_SetWarriorTypeNoGroup(npchar);
			chrDisableReloadToLocation = false;
			AddMoneyToCharacter(pchar, sti(pchar.questTemp.jailCanMove.Deliver.price));
			DeleteAttribute(pchar, "questTemp.jailCanMove.Deliver");
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;

        case "Man_FackYou":
			dialog.text = "Ты чё, крыса, вообще нюх потерял?!";
			link.l1 = LinkRandPhrase("Дъявол!!", "Каррамба!!", "А-ать, черт!");
			link.l1.go = "fight";
		break;

		case "Second time":
			dialog.text = "Не о чем базарить, вали отсюда по-тихому.";
			link.l1 = "Ладно, ладно...";
			link.l1.go = "exit";			
			NextDiag.TempNode = "Second time";
		break;
	}
}
