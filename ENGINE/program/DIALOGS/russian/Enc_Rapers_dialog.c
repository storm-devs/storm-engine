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
		case "exit":
			Diag.CurrentNode = Diag.TempNode;
			DialogExit();			
		break;

		case "exit_fight":
			for(i = 1; i <= 3; i++)
			{
				if (GetCharacterIndex("GangMan_" + i) == -1) continue;
				sld = CharacterFromID("GangMan_" + i);
				LAi_SetWarriorType(sld);
				LAi_group_MoveCharacter(sld, "EnemyFight");
			}
			LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
			LAi_group_FightGroups("EnemyFight", LAI_GROUP_PLAYER, true);
			LAi_group_SetCheck("EnemyFight", "LandEnc_RapersAfrer");
			sld = CharacterFromID("CangGirl");
			LAi_SetActorType(sld);
			LAi_ActorAfraid(sld, npchar, true);
			DialogExit();
			AddDialogExitQuest("MainHeroFightModeOn");
		break;

		case "exit_noFight":
			for(i = 1; i <= 3; i++)
			{
				if (GetCharacterIndex("GangMan_" + i) == -1) continue;
				sld = CharacterFromID("GangMan_" + i);
				LAi_SetActorType(sld);
				LAi_ActorFollow(sld, characterFromId("CangGirl"), "", -1);
				LAi_SetCheckMinHP(sld, LAi_GetCharacterHP(sld)-1, false, "LandEnc_RapersBeforeDialog");
			}
			sld = CharacterFromID("CangGirl");
			LAi_SetActorType(sld);
			LAi_ActorAfraid(sld, npchar, true);
			DialogExit();			
		break;

		case "First time":
			for(i = 1; i <= 3; i++)
			{
				if (GetCharacterIndex("GangMan_" + i) == -1) continue;
				sld = CharacterFromID("GangMan_" + i);
				LAi_type_actor_Reset(sld);
				LAi_RemoveCheckMinHP(sld);
			}
			Diag.TempNode = "OnceAgain";
			dialog.text = LinkRandPhrase("Убирайся отсюда! Не мешай нам развлекаться!",
				"Проваливай, приятель! Не ввязывайся не в свое дело! Подумаешь, мы чуть позабавимся!",
				"Иди своей дорогой, незнакомец! Она потом нам сама будет благодарна. А ты целее будешь!");
			Link.l1 = LinkRandPhrase("Нет уж, мне придется вмешаться.",
				"Я не позволю свершиться этому преступлению!",
				"Бросьте свое дурное дело!");
			Link.l1.go = "Node_2";
			Link.l2 = LinkRandPhrase("Я пойду. Простите, что помешал.",
				"Извините, что побеспокоил. Счастливо оставаться.",
				"Я все понял. До свидания!");
			Link.l2.go = "Exit_NoFight";
		break;
		
		case "Node_2":
			dialog.text = "Ну, тогда пеняй на себя! Пощекочем его ребра сталью, ребята!";
			Link.l1 = "Помолитесь перед смертью!";
			Link.l1.go = "Exit_Fight";
		break;

		case "OnceAgain":
			Diag.TempNode = "OnceAgain";
			dialog.text = "Что, захотелось присоединиться? Иди к черту - нам самим мало!";
			Link.l1 = "Раз так, не буду вам мешать.";
			Link.l1.go = "Exit_NoFight";
			Link.l2 = "Я не позволю насилию случиться на моих глазах!";
			Link.l2.go = "Node_2";
		break;
				
	}
}
