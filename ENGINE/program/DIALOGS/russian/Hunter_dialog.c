void ProcessDialogEvent()
{
	ref NPChar, TempChar;
	aref Link, Diag;
	
	DeleteAttribute(&Dialog,"Links");

	makeref(NPChar,CharacterRef);
	makearef(Link, Dialog.Links);
	makearef(Diag, NPChar.Dialog);

	switch(Dialog.CurrentNode)
	{
		// -----------------------------------Диалог первый - первая встреча
		case "exit":
			Diag.CurrentNode = Diag.TempNode;
			DialogExit();
		break;

		case "First time":
			chrDisableReloadToLocation = false;
			//Lai_SetPlayerType(pchar);
			
			dialog.text = GetFullName(PChar) + "! "+"Мы выслеживали тебя долгое время и вот, наконец, ты у нас в руках.";
			Link.l1 = "Кто вы такие и что вам от меня нужно?";
			Link.l1.go = "meeting"; 
			
			PChar.GenQuest.HunterStart = true;
		break;

		case "meeting":
			dialog.text = XI_ConvertString(Nations[sti(NPChar.nation)].Name) + " заплатила за твою голову хорошую цену, неважно живым мы тебя доставим или мертвым.";
			Link.l1 = "Я готов заплатить вам еще, лишь бы вы оставили меня в покое.";
			Link.l1.go = "Cost_Head"; 
            // boal 08.04.04 -->
            if (GetSummonSkillFromNameToOld(PChar, SKILL_SNEAK) > 2 || bBettaTestMode)
            {
    			TempChar = characterFromID("Bug Fixer");
    			SetRandomNameToCharacter(TempChar);
    			Link.l2 = "Позвольте, но похоже вы обознались. Я "+ GetFullName(TempChar) + " - известный в этих краях торговец.";
    			Link.l2.go = "lier";
			}
            Link.l3 = "Ну что же, испытайте свою удачу.";
			Link.l3.go = "battle";
			// to_do
            /*PChar.GenQuest.ToPrisonCmdIdx = GetCharIDXByParam("PrisonCommander", "nation", NPChar.nation);
            if (sti(PChar.GenQuest.ToPrisonCmdIdx) > 0)
            {
                Link.l4 = "Хорошо, я проследую с вами. Я сам уже собирался все уладить.";
    			Link.l4.go = "ToPrison";
            }*/
		break;
		
		/*case "ToPrison":
            AddDialogExitQuest("Hunters_GoToPrison");
            
            if (CheckAttribute(PChar, "GenQuest.HunterStart")) // только ОЗГи, а не кладоисатели
            {   // запись в СЖ
	            AddQuestRecord("HeadHunter", "HeadHunter_GiveUp");
				AddQuestUserData("HeadHunter", "sNation", NationNameGenitive(sti(NPChar.Nation)));
				AddQuestUserData("HeadHunter", "sLoc", GetLocHunterName());
                DeleteAttribute(PChar, "GenQuest.HunterStart"); 
            }
            DialogExit();
        break;  */
        
        case "lier":
            if (GetSummonSkillFromNameToOld(PChar, SKILL_SNEAK) > rand(15) || bBettaTestMode)
            {
                dialog.text = "О! Наверно мы ошиблись, простите нас, "+ GetAddress_Form(NPChar) + ".";
                Link.l1 = "Конечно, бывает..";
                Link.l1.go = "lier_2";
                AddCharacterExpToSkill(pchar, SKILL_SNEAK, 30);
            }
            else
            {
                dialog.text = "Похоже, ты врешь!";
                Link.l1 = "Я готов заплатить вам еще, лишь бы вы оставили меня в покое.";
			    Link.l1.go = "Cost_Head";
			    Link.l2 = "Ну что же, испытайте свою удачу.";
			    Link.l2.go = "battle";
            }
            
        break;
        
        case "lier_2":
            AddDialogExitQuest("GoAway_Hunters_Land");
            if (CheckAttribute(PChar, "GenQuest.HunterStart")) // только ОЗГи, а не кладоисатели
            {   // запись в СЖ
	            AddQuestRecord("HeadHunter", "HeadHunter_free");
				AddQuestUserData("HeadHunter", "sNation", NationNameGenitive(sti(NPChar.Nation)));
				AddQuestUserData("HeadHunter", "sLoc", GetLocHunterName());
                DeleteAttribute(PChar, "GenQuest.HunterStart"); 
            }
            DialogExit();
        break;
        // boal <--
        
        case "battle":
            AddDialogExitQuest("Battle_Hunters_Land");    // BOAL Весь код я перенес сюда по всем нациям, просто не переименовал
            if (CheckAttribute(PChar, "GenQuest.HunterStart")) // только ОЗГи, а не кладоисатели
            {   // запись в СЖ
	            AddQuestRecord("HeadHunter", "HeadHunter_battle");
				AddQuestUserData("HeadHunter", "sNation", NationNameGenitive(sti(NPChar.Nation)));
				AddQuestUserData("HeadHunter", "sLoc", GetLocHunterName());
                DeleteAttribute(PChar, "GenQuest.HunterStart"); 
            }
            DialogExit();
        break;

        case "Cost_Head2":
            AddMoneyToCharacter(pchar, -sti(PChar.HunterCost));

            AddDialogExitQuest("GoAway_Hunters_Land"); 
            
            if (CheckAttribute(PChar, "GenQuest.HunterStart")) // только ОЗГи, а не кладоисатели
            {   // запись в СЖ
	            AddQuestRecord("HeadHunter", "HeadHunter_Buy");
				AddQuestUserData("HeadHunter", "sNation", NationNameGenitive(sti(NPChar.Nation)));
				AddQuestUserData("HeadHunter", "sLoc", GetLocHunterName());
                DeleteAttribute(PChar, "GenQuest.HunterStart"); 
            }
            DialogExit();
        break;

        case "Cost_Head":
			dialog.text = "Я думаю, что " + PChar.HunterCost + " пиастров нас устроит.";
            if(makeint(Pchar.money) < sti(PChar.HunterCost))
            {
                Link.l1 = "У меня нет таких денег.";
                Link.l1.go = "NoMoney";
            }else{
                Link.l1 = "Вот ваши деньги, забирайте и проваливайте.";
                Link.l1.go = "Cost_Head2";
                Link.l2 = "Такую сумму вам, подонкам.... Уж лучше я вас всех здесь перережу!!!";
                Link.l2.go = "battle";
            }
		break;


        case "NoMoney":
			dialog.text = "В таком случае разговор окончен!";
			Link.l1 = "Живым вам меня не взять.";
			Link.l1.go = "battle"; 
		break;
		
		case "TreasureHunter":
			dialog.text = "Постой-ка, приятель... Сдается, у тебя есть кое-что интересное. Нужно делиться с близкими найдеными сокровищами.";
            Link.l1 = "Хорошо, я готов заплатить вам, лишь бы вы оставили меня в покое.";
			Link.l1.go = "Cost_Head";
            // boal 08.04.04 -->
            if (GetSummonSkillFromNameToOld(PChar, SKILL_SNEAK) > 3)
            {
    			TempChar = characterFromID("Bug Fixer");
    			SetRandomNameToCharacter(TempChar);
    			Link.l2 = "Позвольте, но похоже вы обознались. Я "+ GetFullName(TempChar) + " - известная в этих краях личность, а не искатель сокровищ!";
    			Link.l2.go = "TreasureHunterLier";
			}
            Link.l3 = "Ну что же, пришло время отделиться вашим головам от тела.";
			Link.l3.go = "battle";
		break;
		
		case "TreasureHunterLier":
            if (GetSummonSkillFromNameToOld(PChar, SKILL_SNEAK) > rand(15))
            {
                dialog.text = "О! Наверно мы ошиблись, простите нас, "+ GetAddress_Form(NPChar)+".";
                Link.l1 = "Конечно, бывает..";
                Link.l1.go = "lier_2";
                AddCharacterExpToSkill(pchar, SKILL_SNEAK, 20);
            }
            else
            {
                dialog.text = "Похоже ты врешь!";
			    Link.l1 = "Тогда послушайте как поет моя сабля.";
			    Link.l1.go = "battle";
            }
        break;
	}
}

string GetLocHunterName()
{
	int nLoc = FindLocation(Pchar.location);
	int nFile = LanguageOpenFile("LocLables.txt");
	string ret = "";
	
	if(nFile >= 0) 
	{
		if (CheckAttribute(&locations[nLoc],"islandId"))
		{
			if (locations[nLoc].islandId != "Mein")
			{
				ret = "острова " + LanguageConvertString(nFile, locations[nLoc].islandId);
			}
			else
			{
				ret = "материка";
			}
		}
		//
        if (CheckAttribute(&locations[nLoc],"fastreload"))
		{
			ret += " (" +LanguageConvertString(nFile, locations[nLoc].fastreload + " Town") + ")";
		}
		//ret += LanguageConvertString(nFile, locations[nLoc].id.label);
		LanguageCloseFile( nFile );
	}
	return ret;	
}