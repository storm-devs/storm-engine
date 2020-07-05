// диалоговый файл №2 на голландку
void ProcessDialogEvent()
{
	ref NPChar, sld;
	aref Link, NextDiag;

	DeleteAttribute(&Dialog,"Links");

	makeref(NPChar,CharacterRef);
	makearef(Link, Dialog.Links);
	makearef(NextDiag, NPChar.Dialog);
	float locx, locy, locz;
	
    switch (Dialog.CurrentNode)
    {
        case "First time":
            dialog.text = "Если вы это видите, то это бага. Надо срочно сообщить Эдди!";
            link.l1 = "...";
            link.l1.go = "exit";

//>>>>>>>>>>>>========= Разброс диалогов по персонажам =====================
            if (pchar.questTemp.State == "DelivLettTortuga_NotFoundLetter")// кэп пиратов, вывезший де Барраса из Тортуги
            {
    			dialog.text = "Ты победил! Я сдаюсь...";
    			link.l1 = "Я хочу знать, где этот чертов Антонио?! Отвечай!!!";
    			link.l1.go = "Step_H7_1";
            }
            if (npchar.id == "LoranDeGraf")// Лоран де Граф, квест №8
            {
    			dialog.text = "Сударь, сейчас не время для разговоров! Идет бой.";
    			link.l1 = "Да-да, я знаю...";
    			link.l1.go = "exit";
            }
			if (pchar.questTemp.State == "SeekBible_WeWon" && npchar.id == "LoranDeGraf")// Лоран де Граф, квест №8
            {
    			dialog.text = "Я в крайней степени благодарен вам за помощь, сударь. Вы подоспели как нельзя вовремя. Позвольте представиться, меня зовут Лоран де Граф.";
    			link.l1 = "Знаю, я как раз искал встречи с вами сначала на Бермудах, затем на Тортуге.";
    			link.l1.go = "Step_H8_10";
            }
            if (pchar.questTemp.State == "SeekBible_IHaveMap")
            {
    			dialog.text = "Мне кажется, что наша сделка завершена, сударь.";
    			link.l1 = "Верно, завершена.";
    			link.l1.go = "exit";
            }
            if (pchar.questTemp.State == "SeekBible_SeekMoney")
            {
    			dialog.text = "Вы уже нашли деньги, сударь?";
                if (sti(pchar.money) >= 235000)
                {
                    link.l1 = "Да, нашел. И готов их выложить тебе...";
        			link.l1.go = "Step_H8_13";
                }
                else
                {
                    link.l1 = "Нет еще, я в поиске...";
        			link.l1.go = "exit";
                }
            }
        break;

//<<<<<<<<<<<<===== Разброс диалогов по персонажам =====================
        case "Exit":
            NextDiag.CurrentNode = NextDiag.TempNode;
            DialogExit();
        break;

//********************************* Диалоги кэпа, который увез де Барраса из Тортуги **********************************
 		case "Step_H7_1":
			dialog.text = "Антонио де Баррас? Я высадил его по дороге сюда на испанский военный галеон.";
			link.l1 = "Дьявол! Как назывался этот галеон и куда он ушел?";
			link.l1.go = "Step_H7_2";
		break;
  		case "Step_H7_2":
			dialog.text = "Галеон  'Изабелла', ушел курсом на Мейн к Санта-Каталине. Антонио говорил мне, что ему обязательно и срочно нужно именно туда.";
			link.l1 = "А-а-а, черт! Слушай, ты же пират, какого дьявола ты яшкаешься с ипанцами?!";
			link.l1.go = "Step_H7_3";
		break;
  		case "Step_H7_3":
			dialog.text = "Не твое собачье дело, каррамба...\n"+
                          "Хей, вы как нельзя вовремя, ребята! Валим этого пижона, надоел он мне...";
			link.l1 = "Аргх...";
			link.l1.go = "Step_H7_4";
		break;
  		case "Step_H7_4":
            GetCharacterPos(pchar, &locx, &locy, &locz);
            for (int i=1; i<=2; i++)
            {
                sld = GetCharacter(NPC_GenerateCharacter("Pirate"+i, "officer_"+i, "man", "man", 25, PIRATE, 0, true));
                FantomMakeCoolFighter(sld, 25, 100, 100, BLADE_LONG, "pistol3", 50);
            	LAi_SetWarriorType(sld);
                LAi_group_MoveCharacter(sld, LAI_GROUP_BRDENEMY);
               	ChangeCharacterAddressGroup(sld, pchar.location, "rld", LAi_FindFarLocator("rld", locx, locy, locz));
            }
            LAi_group_SetRelation(LAI_GROUP_BRDENEMY, LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
            LAi_group_FightGroups(LAI_GROUP_BRDENEMY, LAI_GROUP_PLAYER, true);
            QuestAboardCabinDialogExitWithBattle("");
            AddQuestRecord("Hol_Line_7_DelivLettTortuga", "8");
            pchar.questTemp.State.KingResult = "BrigAborded"; //флаг бриг взят на абордаж
            pchar.quest.DelivLettTortuga_BrigDieHard.over = "yes";
            pchar.quest.DelivLettTortuga_AfterBrigSunk.over = "yes";
            Island_SetReloadEnableGlobal("PuertoRico", true);
            Pchar.quest.DelivLettTortuga_AttackWarship.win_condition.l1 = "location";
            Pchar.quest.DelivLettTortuga_AttackWarship.win_condition.l1.location = "SantaCatalina";
            Pchar.quest.DelivLettTortuga_AttackWarship.win_condition = "DelivLettTortuga_AttackWarship";
            SetTimerCondition("DelivLettTortuga_WarshipTimeOver", 0, 0, 12, false);
            DialogExit();
		break;

		case "Step_H8_10":
			dialog.text = "Кто вы и чем я могу помочь?";
			link.l1 = "Меня зовут " + GetFullName(pchar) + ", я хотел бы купить у вас книгу, которую вы предлагали к продаже в обществе Джекмена.";
			link.l1.go = "Step_H8_11";
    	break;
		case "Step_H8_11":
			dialog.text = "А-а-а, вот как? Хорошо, это можно устроить. Книгу я продать не смог и, собственно, спрятал ее в надежном месте. Предлагаю такие условия сделки: вы платите мне 235 тысяч и я даю вам карту того места, где лежит книга.";
			link.l1 = "А не слишком ли много - 235 тысяч?";
			link.l1.go = "Step_H8_12";
    	break;
		case "Step_H8_12":
			dialog.text = "Нет. Дело в том, что в том схроне еще кое-что лежит. Так что это цена за все, за весь клад.";
            if (sti(pchar.money) >= 235000)
            {
                link.l1 = "Договорились, сударь. Вот ваши деньги!";
    			link.l1.go = "Step_H8_13";
    			AddMoneyToCharacter(pchar, -235000);
            }
            else
            {
                link.l1 = "Нет таких денег у меня сейчас. Придется заглянуть к тебе позже.";
    			link.l1.go = "Step_H8_15";
            }
    	break;
		case "Step_H8_13":
			dialog.text = "Извольте плучить карту. Послушайте, я рад, что сумел наконец продать эту бесполезную для меня вещь. Но будте так добры, скажите мне, зачем она вам?";
			link.l1 = "Я действую по поручению голландских властей, так что мне особо рассуждать не приходится.";
			link.l1.go = "Step_H8_14";
            GiveItem2Character(pchar, "LegransMap");
    	break;
		case "Step_H8_14":
			dialog.text = "А-а-а, понял. Ну что же, выгодная сделка для нас обоих.";
			link.l1 = "Это точно. Я благодарен вам, Лоран де Граф. Надеюсь, еще увидимся.";
			link.l1.go = "exit";
            AddQuestRecord("Hol_Line_8_SeekBible", "9");
            NPChar.LifeDay = 2;
    	    SaveCurrentNpcQuestDateParam(NPChar, "LifeTimeCreate");
    	    pchar.questTemp.State = "SeekBible_IHaveMap";
         	pchar.GenQuestBox.Terks_Grot = true;
            pchar.GenQuestBox.Terks_Grot.box1.items.jewelry5 = 200;
            pchar.GenQuestBox.Terks_Grot.box1.items.blade33 = 1;
            pchar.GenQuestBox.Terks_Grot.box1.items.blade28 = 1;
            pchar.GenQuestBox.Terks_Grot.box1.items.blade34 = 1;
            pchar.GenQuestBox.Terks_Grot.box1.items.blade32 = 1;
            pchar.GenQuestBox.Terks_Grot.box1.items.arm1 = 1;
            pchar.GenQuestBox.Terks_Grot.box1.items.pistol5 = 1;
            pchar.GenQuestBox.Terks_Grot.box1.items.pistol4 = 1;
            pchar.GenQuestBox.Terks_Grot.box1.items.pistol6 = 1;
            pchar.GenQuestBox.Terks_Grot.box1.items.potion2 = 5;
            pchar.GenQuestBox.Terks_Grot.box1.items.indian18 = 1;
            pchar.GenQuestBox.Terks_Grot.box1.items.jewelry7 = 5;
            pchar.GenQuestBox.Terks_Grot.box1.items.lead1 = 1;
            pchar.GenQuestBox.Terks_Grot.box1.items.potion2 = 10;
            pchar.GenQuestBox.Terks_Grot.box1.items.potionwine = 3;
            pchar.GenQuestBox.Terks_Grot.box1.items.jewelry2 = 20;
            pchar.GenQuestBox.Terks_Grot.box1.items.jewelry14= 10;
            pchar.GenQuestBox.Terks_Grot.box1.items.indian6 = 1;
            pchar.GenQuestBox.Terks_Grot.box1.items.indian11 = 1;
            pchar.GenQuestBox.Terks_Grot.box1.items.indian15 = 1;
            pchar.GenQuestBox.Terks_Grot.box1.items.indian12 = 1;
            pchar.GenQuestBox.Terks_Grot.box1.items.indian18 = 1;
            pchar.GenQuestBox.Terks_Grot.box1.items.cirass5 = 1;
            pchar.GenQuestBox.Terks_Grot.box1.items.Bible = 1;
        break;
		case "Step_H8_15":
			dialog.text = "Прекрасно, сударь. Вы сможете найти меня на Тортуге таверне.";
			link.l1 = "Хорошо, скоро буду у тебя, жди.";
			link.l1.go = "exit";
			pchar.questTemp.State = "SeekBible_SeekMoney";
			pchar.quest.SeekBible_DeGrafToTavern.win_condition.l1 = "MapEnter";
            pchar.quest.SeekBible_DeGrafToTavern.win_condition = "SeekBible_DeGrafToTavern";
    	break;
    	
    	
    }
}
