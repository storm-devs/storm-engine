
void ProcessDialogEvent()
{
	ref NPChar, sld;
	aref Link, NextDiag;
	string sTemp;
	float locx, locy, locz;

	DeleteAttribute(&Dialog,"Links");

	makeref(NPChar,CharacterRef);
	makearef(Link, Dialog.Links);
	makearef(NextDiag, NPChar.Dialog);

    // вызов диалога по городам -->
    NPChar.FileDialog2 = "DIALOGS\" + LanguageGetLanguage() + "\Prison\" + NPChar.City + "_Prison.c";
    if (LoadSegment(NPChar.FileDialog2))
	{
        ProcessCommonDialog(NPChar, Link, NextDiag);
		UnloadSegment(NPChar.FileDialog2);
	}
    // вызов диалога по городам <--

	switch(Dialog.CurrentNode)
	{
		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	
		case "fight":
			DialogExit();
			NextDiag.CurrentNode = NextDiag.TempNode;
			LAi_group_Attack(NPChar, Pchar);
			if (rand(3) != 1) SetNationRelation2MainCharacter(sti(npchar.nation), RELATION_ENEMY);
			AddDialogExitQuest("MainHeroFightModeOn");
		break;

		case "NoMoreTalkExit":
			LAi_CharacterDisableDialog(npchar);
			DialogExit();
		break;
		//---------------- Начальник тюрьмы ------------------
		case "First_officer":
			dialog.text = RandPhraseSimple("Я начальник тюрьмы. Что вам нужно здесь?", "Что вам нужно? Зачем вы явились в тюрьму?");
			link.l1 = "Да ничего особенного, осматриваю город, знаете ли. Вот и забрел сюда по случаю.";
			link.l1.go = "exit";
			link.l2 = "Хочу поговорить по делу.";
			link.l2.go = "quests";
			if (!sti(pchar.questTemp.jailCanMove))
			{
				link.l3 = "Я хочу пройти внутрь тюрьмы.";
				link.l3.go = "ForGoodMove";		
			}
			if (CheckAttribute(pchar, "questTemp.jailCanMove.City") && npchar.city == pchar.questTemp.jailCanMove.City)
			{
				link.l4 = "Послушайте, не подскажете мне, за какое преступление сидит здесь заключенный по имени " + GetFullName(characterFromId(pchar.questTemp.jailCanMove.prisonerId)) + "?";
				link.l4.go = "KnowAboutPrisoner";	
			}
			NextDiag.TempNode = "First_officer";
		break;
        case "ForGoodMove":
			dialog.text = NPCStringReactionRepeat("Но зачем вам это? Поверьте мне, там нет ничего достойного внимания, одни воры и бандиты.", "Мы уже обсуждали это ваше желание.", 
				"Опять о том же? Уже дважды мы с вами говорили об этом!", "Гм, опять...", "block", 1, npchar, Dialog.CurrentNode);
			link.l1 = HeroStringReactionRepeat("Тем не менее, я хотел бы провести экскурс по вашим казематам. Мне очень интересно!", "Да, именно так. Но я хотел еще поговорить на эту тему.", 
				"Ну, может в третий раз...", "Надежда посмотреть на ваших узников не покидает меня...", npchar, Dialog.CurrentNode);
			link.l1.go = "ForGoodMove_1";
		break;
        case "ForGoodMove_1":
			pchar.questTemp.jailCanMove.money = (rand(15)+10)*100;
			if (sti(colonies[FindColony(npchar.city)].jail) && sti(pchar.money) >= sti(pchar.questTemp.jailCanMove.money))
			{
				dialog.text = "Ну что же, не вижу причины вам отказывать. " + FindRussianMoneyString(sti(pchar.questTemp.jailCanMove.money)) + " - и пока вы не покините тюрьму, вы можете беспрепятственно ходить по коридорам и даже разговаривать с заключенными.";
				link.l1 = "Я согласен, вот ваши монеты!";
				link.l1.go = "ForGoodMove_agree";
				link.l2 = "Не пойдет, слишком дорого для бесцельного хождения по коридорам.";
				link.l2.go = "exit";
			}
			else
			{
				dialog.text = RandPhraseSimple("Нет, это категорически запрещено уставом. Вам здесь что, цирк? Уходите, и не отвлекайте меня от несения службы.", "Я не могу позволить расхаживать по тюрьме посторонним людям. Прошу вас, не мешайте мне!");
				link.l1 = "Хорошо, как скажете...";
				link.l1.go = "exit";
			}
		break;
        case "ForGoodMove_agree":
            dialog.text = "Ну что же, вы можете приступать с своему экскурсу, так сказать...";
			link.l1 = "Спасибо, офицер.";
			link.l1.go = "exit";
			pchar.questTemp.jailCanMove = true;
			AddMoneyToCharacter(pchar, -sti(pchar.questTemp.jailCanMove.money));
		break;
        case "KnowAboutPrisoner":
			switch (pchar.questTemp.jailCanMove.ownerPrison)
			{
				case "0": sTemp = "За убийство."; break;
				case "1": sTemp = "За бандитизм и вооруженный грабеж."; break;
				case "2": sTemp = "Его поймали на воровстве."; break;
				case "3": sTemp = "За мелкую кражу."; break;
				case "4": sTemp = "За шулерство."; break;
			}
			dialog.text = sTemp;
			link.l1 = "Понятно... А есть шанс внести за него выкуп, залог, или еще как-нибудь содействовать его вызволению?";
			link.l1.go = "KnowAboutPrisoner_" + pchar.questTemp.jailCanMove.ownerPrison;
			DeleteAttribute(pchar, "questTemp.jailCanMove.City");
		break;
        case "KnowAboutPrisoner_0":
			dialog.text = "Нет, конечно. Этому висельнику прямая дорога в ад. Он на особом контроле у губернатора!";
			link.l1 = "Хех, понятно...";
			link.l1.go = "notFree_exit";
		break;	
        case "KnowAboutPrisoner_1":
			dialog.text = "Не думаю. Много он крови попортил жителям нашего города. Так что даже и не проси.";
			link.l1 = "Хм, ясно.";
			link.l1.go = "notFree_exit";
		break;
        case "KnowAboutPrisoner_2":
			dialog.text = "Нет, пожалуй. Он вор, а вор должен сидеть в тюрьме!";
			link.l1 = "О как! Хорошо сказано, лейтенант...";
			link.l1.go = "notFree_exit";
		break;
        case "KnowAboutPrisoner_3":
			dialog.text = "Не знаю даже. Этот паршивец стащил у одной хозяйки сковороду. И отпустить его не могу, но и гноить здесь за такую мелочь - тоже не дело...";
			link.l1 = "Так отдайте его мне, лейтенант. Я заплачу, в разумных пределах, естественно.";
			link.l1.go = "KnowAboutPrisoner_free";
		break;
        case "KnowAboutPrisoner_4":
			dialog.text = "Вызволению? Вполне можно. С моей точки зрения, содержать этого мелкого жулика здесь не имеет никакого смысла.";
			link.l1 = "Так отдайте его мне, лейтенант. Я заплачу, в разумных пределах, естественно.";
			link.l1.go = "KnowAboutPrisoner_free";
		break;
        case "KnowAboutPrisoner_free":
			pchar.questTemp.jailCanMove.ownerPrison.money = (rand(3)+2)*1000;
			if (sti(pchar.money) > sti(pchar.questTemp.jailCanMove.ownerPrison.money))
			{
				dialog.text = "Хм, ну хорошо. " + FindRussianMoneyString(sti(pchar.questTemp.jailCanMove.ownerPrison.money)) + " на бочку, и забирай его хоть прямо сейчас.";
				link.l1 = "Здорово, я согласен.";
				link.l1.go = "KnowAboutPrisoner_agree";
				link.l2 = "Нет, дороговато будет. Откажусь.";
				link.l2.go = "KnowAboutPrisoner_exit";
			}
			else
			{
				dialog.text = "Нет, не выйдет. У меня могут провести инспекцию в любой момент, рисковать не хочу.";
				link.l1 = "О как! Хорошо сказано, лейтенант...";
				link.l1.go = "exit";
			}
		break;
        case "KnowAboutPrisoner_agree":
			dialog.text = "Все, вы можете пройти к его камере и забирать этого оборванца.";
			link.l1 = "Спасибо.";
			link.l1.go = "exit";
			pchar.questTemp.jailCanMove = true;
			AddMoneyToCharacter(pchar, -sti(pchar.questTemp.jailCanMove.ownerPrison.money));
			DeleteAttribute(pchar, "questTemp.jailCanMove.ownerPrison");
			sld = characterFromId(pchar.questTemp.jailCanMove.prisonerId)
			LAi_CharacterEnableDialog(sld);
			sld.dialog.currentnode = "ToPrisonHead_canMove";
		break;
        case "KnowAboutPrisoner_exit":
			dialog.text = "Ну смотри, другого шанса я не дам...";
			link.l1 = "И не надо. Я принял решение.";
			link.l1.go = "exit";
			DeleteAttribute(pchar, "questTemp.jailCanMove.ownerPrison");
			sld = characterFromId(pchar.questTemp.jailCanMove.prisonerId);
			LAi_CharacterEnableDialog(sld);
			sld.dialog.currentnode = "ToPrisonHead_expansive";
		break;
        case "notFree_exit":
			sld = characterFromId(pchar.questTemp.jailCanMove.prisonerId);
			LAi_CharacterEnableDialog(sld);
			sld.dialog.currentnode = "ToPrisonHead_notFree";
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
		//---------------- Протектор ------------------
        case "First_protector":
			if (sti(pchar.questTemp.jailCanMove))
			{
				dialog.text = RandPhraseSimple("Вы можете пройти, начальник разрешил...", "Получен приказ начальника тюрьмы. Вы можете свободно проходить.");
				link.l1 = "Ну вот и чудно.";
				link.l1.go = "exit";
				DeleteAttribute(pchar, "questTemp.jailCanMove.ownerPrison.money");
			}
			else
			{
				dialog.text = RandPhraseSimple("Проходить далее без разрешения начальника тюрьмы категорически запрещено!", "Я подчиняюсь только начальнику тюрьмы. Если ты попытаешься пройти дальше без его разрешения - тебе конец!");
				link.l1 = RandPhraseSimple("Понятно", "Ясно") + ", солдат.";
				link.l1.go = "exit";
			}
			NextDiag.TempNode = "First_protector";
		break;
		//---------------- Солдаты ------------------
        case "First_soldier":
            dialog.text = RandPhraseSimple("Я на посту, не отвлекайте меня.", "Проходите мимо, я не могу с вами разговаривать.");
			link.l1 = "Хорошо, солдат.";
			link.l1.go = "exit";
			NextDiag.TempNode = "First_soldier";
		break;
		//---------------- Заключенные ------------------
        case "First_prisoner": 
			if (GetNationRelation2MainCharacter(sti(npchar.nation)) == RELATION_ENEMY)
			{
				dialog.text = LinkRandPhrase("Хо-хо, до чего же весело!!", "Режь их, приятель, режь!!!", "Эх, дъявол! Уж не думал, что увижу труп своего надсмотрщика!");
				link.l1 = RandPhraseSimple("Хех!", "Аргх...");
				link.l1.go = "NoMoreTalkExit";
				if (rand(10) > 6 && !CheckAttribute(pchar, "quest.GivePrisonFree") && !CheckAttribute(pchar, "quest.GivePrisonFree_Over") && !CheckAttribute(pchar, "quest.DeliverToBander"))
				{
					dialog.text = RandPhraseSimple("Постой, дружище!! Выпусти меня!", "Послушай, приятель, будь так добр, открой камеру.");
					link.l1 = "С какой это стати?";
					link.l1.go = "Prisoner_1"; // на квест освобождения заключенного
				}
			}
			else
			{
				dialog.text = LinkRandPhrase("Меня взяли за грабеж, дело шьют...", "Агрх, холеный ты... Тебя бы ко мне в камеру на недельку! Кхе-хе-хе...", "Я ни в чем не виноват!");
				link.l1 = RandPhraseSimple("Заткнись!", "Мне нет до тебя дела...");
				link.l1.go = "NoMoreTalkExit";
				if (rand(10) > 6 && !CheckAttribute(pchar, "quest.GivePrisonFree_Over") && !CheckAttribute(pchar, "quest.GivePrisonFree") && !CheckAttribute(pchar, "quest.DeliverToBander") && !sti(colonies[FindColony(npchar.city)].HeroOwn))
				{
					dialog.text = RandPhraseSimple("Постой, приятель, не проходи мимо моей камеры так быстро!", "Не торопись, дружище, давай поговорим.");
					link.l1 = "С какой это стати?";
					if (rand(1))
					{
						link.l1.go = "Prisoner_1"; // на квест освобождения заключенного
					}
					else
					{
						link.l1.go = "Deliver_1"; // на квест передачу весточки
					}
				}
			}
		break;
		// -------------------- освобождение заключенного --------------------
        case "Prisoner_1":
            dialog.text = "Чтобы послушать меня. Ты не смотри, что сейчас я в таком виде. Мне бы выбраться отсюда...";
			link.l1 = "Чего?!";
			link.l1.go = "Prisoner_2";
			GetChestPlaceName();
			pchar.questTemp.jailCanMove.prisonerId = npchar.id; //запомним Id
			pchar.questTemp.jailCanMove.IsTrue = rand(1); //врет или правду глаголит о кладе
			pchar.questTemp.jailCanMove.Sum = (rand(10)+10)*1000+(sti(pchar.rank)*1000); //сумма клада
			pchar.questTemp.jailCanMove.Name = GetFullName(npchar); //имя перца
		break;
        case "Prisoner_2":
            dialog.text = "Что слышал. Помоги мне выбраться, и я сумею отблагодарить.";
			link.l1 = "Уже интересней. Кто ты такой и что можешь мне предложить?";
			link.l1.go = "Prisoner_3";
		break;
        case "Prisoner_3":
            dialog.text = "Меня зовут " + GetFullName(npchar) + ". Кое-что у меня припрятано в надежном месте. Вытащи меня отсюда и отвези на " + XI_ConvertString(pchar.questTemp.jailCanMove.islandId + "Acc") + ". Там в гроте у меня спрятан клад. Достанем его и поделим поровну!";
			link.l1 = "И что там у тебя в кладе? И откуда мне знать, что твои слова - правда?";
			link.l1.go = "Prisoner_4";
		break;
        case "Prisoner_4":
            dialog.text = "Клянусь тебе, что я говорю чистейшую правду! А в кладе я спрятал деньги и драгоценности...";
			link.l1 = "Нет, приятель. Я не собираюсь рисковать шкурой из-за сомнительного клада. Извини...";
			link.l1.go = "Prisoner_5";
			if (GetNationRelation2MainCharacter(sti(npchar.nation)) != RELATION_ENEMY)
			{			
				link.l2 = "Пожалуй, стоит рискнуть... Предлагаю следующее: я могу перебить стражу в тюрьме и забрать тебя на свой корабль. Если все получится, я хочу, чтобы ты не отходил от меня до тех пор, пока мы не доберемся до грота на " + XI_ConvertString(pchar.questTemp.jailCanMove.islandId + "Dat") + ". Идет?";
				link.l2.go = "Prisoner_agree"; //силовой способ вызволения
				link.l3 = "Ну хорошо, я постараюсь помочь тебе - поговорю с начальником тюрьмы. Возможно, удастся внести залог за твое освобождение.";
				link.l3.go = "ToPrisonHead_agree"; //мирный способ вызволения
			}
			else
			{
				link.l2 = "Ну хорошо, поверю тебе и открою камеру. Я хочу, чтобы ты не отходил от меня до тех пор, пока мы не доберемя до грота на " + XI_ConvertString(pchar.questTemp.jailCanMove.islandId + "Dat") + ". Идет?";
				link.l2.go = "Prisoner_agree_3"; //силовой способ вызволения, когда тюремщики убиты
			}
		break;
        case "Prisoner_5":
            dialog.text = RandPhraseSimple("Будь ты проклят, каналья!!", "Чтоб тебя разорвало, иуда...");
			link.l1 = RandPhraseSimple("И тебе всего хорошего. Прощай...", "Собака лаяла на дядю фраера...");
			link.l1.go = "NoMoreTalkExit";
		break;
        case "Prisoner_agree":
            dialog.text = "Идет, мне выбирать не приходится.";
			link.l1 = "Хорошо. Я сейчас сломаю твой замок, выходи оттуда, и будем прорываться. Ты готов?";
			link.l1.go = "Prisoner_agree_1";
		break;
        case "Prisoner_agree_1":
            dialog.text = "Да, приятель, я готов!";
			link.l1 = "За дело!";
			link.l1.go = "Prisoner_agree_2";
		break;
        case "Prisoner_agree_2":
			string slai_group = GetNationNameByType(sti(npchar.nation))  + "_citizens";
			LAi_group_AttackGroup(slai_group, LAI_GROUP_PLAYER);
			if (rand(3) != 1) SetNationRelation2MainCharacter(sti(npchar.nation), RELATION_ENEMY);
			ReOpenQuestHeader("GivePrisonFree");
			AddQuestRecord("GivePrisonFree", "1");
            AddQuestUserData("GivePrisonFree", "sCity", XI_ConvertString("Colony"+npchar.city+"Gen"));
            AddQuestUserData("GivePrisonFree", "sName", pchar.questTemp.jailCanMove.Name);
            AddQuestUserData("GivePrisonFree", "sIsland", XI_ConvertString(pchar.questTemp.jailCanMove.islandId+"Dat"));
			pchar.quest.GivePrisonFree_Death.win_condition.l1 = "NPC_Death";
			pchar.quest.GivePrisonFree_Death.win_condition.l1.character = npchar.id;
			pchar.quest.GivePrisonFree_Death.win_condition = "GivePrisonFree_Death";
			Pchar.quest.GivePrisonFree.win_condition.l1 = "location";
			Pchar.quest.GivePrisonFree.win_condition.l1.location = pchar.questTemp.jailCanMove.placeId;
			Pchar.quest.GivePrisonFree.win_condition = "GivePrisonFree";			
			DeleteAttribute(npchar, "LifeDay");
			GiveItem2Character(npchar, "unarmed");
			EquipCharacterbyItem(npchar, "unarmed");
			GetCharacterPos(npchar, &locx, &locy, &locz);	
			ChangeCharacterAddressGroup(npchar, npchar.location, "reload", LAi_FindNearestFreeLocator("reload", locx, locy, locz));
			LAi_SetActorType(npchar);
			LAi_group_Register("DeliveQuest_Prison");
			LAi_group_SetRelation("DeliveQuest_Prison", LAI_GROUP_PLAYER, LAI_GROUP_FRIEND);
			LAi_group_SetAlarmReaction(LAI_GROUP_PLAYER, "DeliveQuest_Prison", LAI_GROUP_ENEMY, LAI_GROUP_FRIEND);
			LAi_group_MoveCharacter(npchar, "DeliveQuest_Prison");
			LAi_ActorFollowEverywhere(npchar, "", -1);
			DialogExit();
			AddDialogExitQuest("MainHeroFightModeOn");
		break;

        case "Prisoner_agree_3":
            dialog.text = "Идет, мне выбирать не приходится.";
			link.l1 = "Хорошо. Следуй за мной как можно быстрей, нам нужно еще добраться до моего корабля. Не отставай!";
			link.l1.go = "Prisoner_agree_4";
		break;
        case "Prisoner_agree_4":
			ReOpenQuestHeader("GivePrisonFree");
			AddQuestRecord("GivePrisonFree", "1");
            AddQuestUserData("GivePrisonFree", "sCity", XI_ConvertString("Colony"+npchar.city+"Gen"));
            AddQuestUserData("GivePrisonFree", "sName", pchar.questTemp.jailCanMove.Name);
            AddQuestUserData("GivePrisonFree", "sIsland", XI_ConvertString(pchar.questTemp.jailCanMove.islandId+"Dat"));
			pchar.quest.GivePrisonFree_Death.win_condition.l1 = "NPC_Death";
			pchar.quest.GivePrisonFree_Death.win_condition.l1.character = npchar.id;
			pchar.quest.GivePrisonFree_Death.win_condition = "GivePrisonFree_Death";
			Pchar.quest.GivePrisonFree.win_condition.l1 = "location";
			Pchar.quest.GivePrisonFree.win_condition.l1.location = pchar.questTemp.jailCanMove.placeId;
			Pchar.quest.GivePrisonFree.win_condition = "GivePrisonFree";
			DeleteAttribute(npchar, "LifeDay");
			GiveItem2Character(npchar, "unarmed");
			EquipCharacterbyItem(npchar, "unarmed");
			GetCharacterPos(npchar, &locx, &locy, &locz);	
			ChangeCharacterAddressGroup(npchar, npchar.location, "reload", LAi_FindNearestFreeLocator("reload", locx, locy, locz));
			LAi_SetActorType(npchar);
			LAi_group_Register("DeliveQuest_Prison");
			LAi_group_SetRelation("DeliveQuest_Prison", LAI_GROUP_PLAYER, LAI_GROUP_FRIEND);
			LAi_group_SetAlarmReaction(LAI_GROUP_PLAYER, "DeliveQuest_Prison", LAI_GROUP_ENEMY, LAI_GROUP_FRIEND);
			LAi_group_MoveCharacter(npchar, "DeliveQuest_Prison");
			LAi_ActorFollowEverywhere(npchar, "", -1);
			DialogExit();
		break;

        case "ToPrisonHead_agree":
            dialog.text = "Ну, попробуй. Эх-х-х, получилось бы! Только не теряй времени, через пару дней меня здесь уже не будет...";
			link.l1 = "Жди, я все улажу.";
			link.l1.go = "NoMoreTalkExit";
			pchar.questTemp.jailCanMove.MayorId = characters[GetCharacterIndex(npchar.city + "_Mayor")].id; //Id мэра
			pchar.questTemp.jailCanMove.City = npchar.City; //город
			pchar.questTemp.jailCanMove.ownerPrison = rand(4); //характер преступления
			SetTimerCondition("GivePrisonFree_Over", 0, 0, 2, false);
			AddQuestRecord("GivePrisonFree", "5");
            AddQuestUserData("GivePrisonFree", "sCity", XI_ConvertString("Colony"+npchar.city+"Gen"));
            AddQuestUserData("GivePrisonFree", "sName", pchar.questTemp.jailCanMove.Name);
		break;

        case "ToPrisonHead_canMove":
            dialog.text = "Ну, что скажешь, приятель?";
			link.l1 = "Все улажено, я могу забирать тебя.";
			link.l1.go = "ToPrisonHead_canMove_1";
		break;
        case "ToPrisonHead_canMove_1":
            dialog.text = "Ну так пойдем отсюда быстрее! Черт возьми, как я рад!";
			link.l1 = "Подведем итоги. Я хочу, чтобы все время, пока мы не доберемся до грота на " + XI_ConvertString(pchar.questTemp.jailCanMove.islandId + "Dat") + ", ты не отходил от меня ни на шаг. На всякий случай, так сказать. Ты согласен?";
			link.l1.go = "ToPrisonHead_canMove_2";
		break;
        case "ToPrisonHead_canMove_2":
            dialog.text = "Конечно согласен! Да и выбирать мне не приходится.";
			link.l1 = "Тогда ты свободен.";
			link.l1.go = "ToPrisonHead_canMove_exit";
		break;
        case "ToPrisonHead_canMove_exit":
			pchar.quest.GivePrisonFree_Over.over = "yes";
			ReOpenQuestHeader("GivePrisonFree");
			AddQuestRecord("GivePrisonFree", "1");
            AddQuestUserData("GivePrisonFree", "sCity", XI_ConvertString("Colony"+npchar.city+"Gen"));
            AddQuestUserData("GivePrisonFree", "sName", pchar.questTemp.jailCanMove.Name);
            AddQuestUserData("GivePrisonFree", "sIsland", XI_ConvertString(pchar.questTemp.jailCanMove.islandId+"Dat"));
			pchar.quest.GivePrisonFree_Death.win_condition.l1 = "NPC_Death";
			pchar.quest.GivePrisonFree_Death.win_condition.l1.character = npchar.id;
			pchar.quest.GivePrisonFree_Death.win_condition = "GivePrisonFree_Death";
			Pchar.quest.GivePrisonFree.win_condition.l1 = "location";
			Pchar.quest.GivePrisonFree.win_condition.l1.location = pchar.questTemp.jailCanMove.placeId;
			Pchar.quest.GivePrisonFree.win_condition = "GivePrisonFree";			
			DeleteAttribute(npchar, "LifeDay");
			GiveItem2Character(npchar, "unarmed");
			EquipCharacterbyItem(npchar, "unarmed");
			GetCharacterPos(npchar, &locx, &locy, &locz);	
			ChangeCharacterAddressGroup(npchar, npchar.location, "reload", LAi_FindNearestFreeLocator("reload", locx, locy, locz));
			LAi_SetActorType(npchar);
			LAi_group_Register("DeliveQuest_Prison");
			LAi_group_SetRelation("DeliveQuest_Prison", LAI_GROUP_PLAYER, LAI_GROUP_FRIEND);
			LAi_group_SetAlarmReaction(LAI_GROUP_PLAYER, "DeliveQuest_Prison", LAI_GROUP_ENEMY, LAI_GROUP_FRIEND);
			LAi_group_MoveCharacter(npchar, "DeliveQuest_Prison");
			LAi_ActorFollowEverywhere(npchar, "", -1);
			DialogExit();
		break;

        case "ToPrisonHead_notFree":
			switch (pchar.questTemp.jailCanMove.ownerPrison)
			{
				case "0": sTemp = "убийстве"; break;
				case "1": sTemp = "вооруженном грабеже"; break;
				case "2": sTemp = "воровстве"; break;
			}
            dialog.text = "Ну, что скажешь, приятель?";
			link.l1 = "Мне не удалось договориться о твоем освобождении. Тебя обвиняют в " + sTemp + ", так что ни о каких залогах речи быть не может.";
			link.l1.go = "ToPrisonHead_notFree_1";
			DeleteAttribute(pchar, "questTemp.jailCanMove.ownerPrison");
		break;
        case "ToPrisonHead_notFree_1":
            dialog.text = "Меня оболгали!.. Вот дьявол!! И что теперь мне делать? Я же сгнию здесь заживо!";
			link.l1 = "Очень жаль, приятель, но я ничем не могу тебе помочь.";
			link.l1.go = "ToPrisonHead_notFree_2";
			link.l2 = "Остается шанс освободить тебя силой. Я сейчас сломаю твой замок, выходи оттуда, и будем прорываться. Ты готов?";
			link.l2.go = "Prisoner_agree_1";
			pchar.quest.GivePrisonFree_Over.over = "yes";
		break;
        case "ToPrisonHead_notFree_2":
            dialog.text = RandPhraseSimple("Будь ты проклят, каналья!!", "Чтоб тебя разорвало, иуда...");
			link.l1 = "И тебе всего хорошего. Прощай...";
			link.l1.go = "NoMoreTalkExit";
			AddQuestRecord("GivePrisonFree", "7");
            AddQuestUserData("GivePrisonFree", "sName", pchar.questTemp.jailCanMove.Name);
			DeleteAttribute(pchar, "questTemp.jailCanMove.islandId");	
			DeleteAttribute(pchar, "questTemp.jailCanMove.placeId");
			DeleteAttribute(pchar, "questTemp.jailCanMove.prisonerId");
			DeleteAttribute(pchar, "questTemp.jailCanMove.IsTrue");
			DeleteAttribute(pchar, "questTemp.jailCanMove.Sum");
			DeleteAttribute(pchar, "questTemp.jailCanMove.Name");
		break;

        case "ToPrisonHead_expansive":
			dialog.text = "Ну, чем обрадуешь, приятель?";
			link.l1 = "Залог за твое осводождение слишком велик, у меня нет таких денег. Я вынужден отказаться от помощи тебе.";
			link.l1.go = "ToPrisonHead_notFree_2";
			pchar.quest.GivePrisonFree_Over.over = "yes";
		break;
		//==> в месте, где клад припрятан
        case "PrisonerInPlace":
			if (sti(pchar.questTemp.jailCanMove.IsTrue))
			{
				dialog.text = "Я нашел его, приятель. Денюжки были там же, где я их спрятал. Ну, теперь пойдет другая жизнь...";
				link.l1 = "Хех, рад, что не ошибся в тебе! Давай делить.";
				link.l1.go = "PrisonerInPlace_3";			
			}
			else
			{
				dialog.text = "Слушай, я тут проверил все, но моего клада не нашел. Очень жаль, что так вышло, наверное, кто-то раскопал его до нас.";
				link.l1 = "Как такое может быть?!";
				link.l1.go = "PrisonerInPlace_1";
			}
		break;
        case "PrisonerInPlace_1":
			dialog.text = "Да очень просто, приятель. Еще и не такое в жизни случается.\nНу ладно, ты тут, если хочешь, оставайся, поищи еще. Ну а мне пора!\nПрощай друг, спасибо, что спас мне жизнь. Я буду помнить тебя вечно!";
			link.l1 = "Ушам не верю...";
			link.l1.go = "PrisonerInPlace_2";
			AddQuestRecord("GivePrisonFree", "4");
            AddQuestUserData("GivePrisonFree", "sName", pchar.questTemp.jailCanMove.Name);
			CloseQuestHeader("GivePrisonFree");
		break;
        case "PrisonerInPlace_2":
			LAi_group_Delete("EnemyFight");
			DeleteAttribute(pchar, "questTemp.jailCanMove.islandId");	
			DeleteAttribute(pchar, "questTemp.jailCanMove.placeId");
			DeleteAttribute(pchar, "questTemp.jailCanMove.prisonerId");
			DeleteAttribute(pchar, "questTemp.jailCanMove.IsTrue");
			DeleteAttribute(pchar, "questTemp.jailCanMove.Sum");
			DeleteAttribute(pchar, "questTemp.jailCanMove.Name");
			chrDisableReloadToLocation = false;
			LAi_LocationDisableMonstersGen(pchar.location, false);
			npchar.lifeDay = 0;
			LAi_SetActorType(npchar);
			LAi_group_MoveCharacter(npchar, "EnemyFight");
            LAi_ActorRunToLocation(NPChar, "reload", "reload1_back", "none", "", "", "", -1);
			DialogExit();
            Pchar.quest.DeletePrisonGroup.win_condition.l1 = "ExitFromLocation";
            Pchar.quest.DeletePrisonGroup.win_condition.l1.location = pchar.location;
            Pchar.quest.DeletePrisonGroup.win_condition = "DeletePrisonGroup";
		break;
        case "PrisonerInPlace_3":
			dialog.text = "Конечно, приятель! Итак, я закопал здесь всякой всячины на сумму " + FindRussianMoneyString(sti(pchar.questTemp.jailCanMove.Sum)*2) + ". Твоя доля состаляет половину, как и было уговорено. Изволите получить наличными, " + GetAddress_Form(NPChar) + "?";
			link.l1 = "Естественно!";
			link.l1.go = "PrisonerInPlace_4";
		break;
        case "PrisonerInPlace_4":
			dialog.text = "Прошу вас! Ну ладно, не буду тебе больше надоедать, мне пора. Прощай, приятель. И спасибо тебе!";
			link.l1 = "Прощай... А ты уверен, что хочешь уйти?";
			link.l1.go = "PrisonerInPlace_5";
			AddMoneyToCharacter(pchar, sti(pchar.questTemp.jailCanMove.Sum));
		break;
        case "PrisonerInPlace_5":
			dialog.text = "Уверен, приятель. За меня не переживай, это не тюрьма " + XI_ConvertString("Colony"+npchar.city+"Gen") + ", здесь у меня все схвачено.\nНу, будь здоров, дружище!";
			link.l1 = "Будь и ты в норме.";
			link.l1.go = "PrisonerInPlace_2";
			AddQuestRecord("GivePrisonFree", "3");
            AddQuestUserData("GivePrisonFree", "iSum", FindRussianMoneyString(sti(pchar.questTemp.jailCanMove.Sum)));
            AddQuestUserData("GivePrisonFree", "sName", pchar.questTemp.jailCanMove.Name);
			CloseQuestHeader("GivePrisonFree");
 		break;
		// -------------------- освобождение заключенного --------------------
		// ------------------------ передать маляву --------------------------
        case "Deliver_1":
			dialog.text = "Меня кличут " + GetFullName(npchar) + ". Есть у меня к тебе просьба одна, хе-хе...";
			link.l1 = "Что такое?";
			link.l1.go = "Deliver_2";
		break;
        case "Deliver_2":
			dialog.text = "Нужно передать маляву на волю. Сделаешь?";
			link.l1 = "А какой мне с этого резон?";
			link.l1.go = "Deliver_3";
		break;
        case "Deliver_3":
			dialog.text = "Не обидят тебя кореши, чего уж... Ну так что, возьмешься?";
			link.l1 = "Нет, меня это не интересует.";
			link.l1.go = "Prisoner_5";
			link.l2 = "Хм, если далеко ходить не надо, то возьмусь.";
			link.l2.go = "Deliver_agree";
		break;
        case "Deliver_agree":
			dialog.text = "Хорошо. Слушай теперь меня внимательно. В городе ждут мою маляву два кента, заныкались они в доме, чтобы под шмон не попасть. В каком доме - не ведаю, надо будет поискать.";
			link.l1 = "Как поискать?";
			link.l1.go = "Deliver_4";
		break;
        case "Deliver_4":
			dialog.text = "Обычно поискать. Туда-сюда по домам прошмальнешься, и найдешь в итоге. Как в нужной хазе окажешься - фраерочки сами к тебе подкатят. Только имей в виду, что на этой киче я еще дня два пробуду, не более. Так что не тяни.";
			link.l1 = "Ясно. Ну, давай свою маляву.";
			link.l1.go = "Deliver_5";
		break;
        case "Deliver_5":
			pchar.questTemp.jailCanMove.Deliver.locationId = GetBanderLocation(npchar); //Id коммона
			if (pchar.questTemp.jailCanMove.Deliver.locationId == "none")
			{
				dialog.text = "Хм, что-то ты мне не нравишься. Я передумал. Давай, проходи, не стой здесь!";
				link.l1 = "Вот это да! Ну ладно, как хочешь...";
				link.l1.go = "NoMoreTalkExit";
				DeleteAttribute(pchar, "questTemp.jailCanMove.Deliver");
			}
			else
			{
				dialog.text = "Держи. Спасибо тебе, приятель!";
				link.l1 = "Да не за что.";
				link.l1.go = "NoMoreTalkExit";
				GiveItem2Character(Pchar, "Malyava");
				pchar.questTemp.jailCanMove.Deliver.Id = npchar.id; //Id зэка
				pchar.questTemp.jailCanMove.Deliver.name = GetFullName(npchar); //имя зэка
				pchar.questTemp.jailCanMove.Deliver.City = npchar.city; //город (Id колонии)
				pchar.questTemp.jailCanMove.Deliver.good = rand(1); //опрокинут или нет
				pchar.questTemp.jailCanMove.Deliver.price = rand(6)*500+500; //цена услуги
				pchar.quest.DeliverToBander.win_condition.l1 = "location";
				pchar.quest.DeliverToBander.win_condition.l1.location = pchar.questTemp.jailCanMove.Deliver.locationId;
				pchar.quest.DeliverToBander.win_condition = "DeliverToBander";
				pchar.quest.DeliverToBander.again = true; 
				SetTimerCondition("DeliverToBander_over", 0, 0, 2, false);
				ReOpenQuestHeader("GivePrisonFree");
				AddQuestRecord("GivePrisonFree", "8");
				AddQuestUserData("GivePrisonFree", "sCity", XI_ConvertString("Colony"+npchar.city+"Gen"));
				AddQuestUserData("GivePrisonFree", "sName", pchar.questTemp.jailCanMove.Deliver.name);
			}
		break;
	}
}

void GetChestPlaceName()
{
	string islandId;
	int iTemp = rand(3);
	switch (iTemp)
	{
		case 0: 
			pchar.questTemp.jailCanMove.islandId = "Bermudes";
			pchar.questTemp.jailCanMove.placeId = "Bermudes_Cavern";
		break;
		case 1: 
			pchar.questTemp.jailCanMove.islandId = "Caiman";
			pchar.questTemp.jailCanMove.placeId = "Caiman_Grot";
		break;
		case 2: 
			pchar.questTemp.jailCanMove.islandId = "Dominica";
			pchar.questTemp.jailCanMove.placeId = "Dominica_Grot";
		break;
		case 3: 
			pchar.questTemp.jailCanMove.islandId = "Terks";
			pchar.questTemp.jailCanMove.placeId = "Terks_Grot";
		break;
	}
}

string GetBanderLocation(ref npchar)
{
    aref	arCommon, arRld, arRld2;
    int	i, n, Qty, Qty2;
	string LocId; 
	string	storeArray [50];
	int howStore = 0;
    makearef(arRld, Locations[FindLocation(npchar.city + "_town")].reload);
	Qty = GetAttributesNum(arRld);
    if (CheckAttribute(pchar, "GenQuest.SeekSpy.Location")) //если взят квест мэра по поиску шпиона
	{
		for (i=0; i<Qty; i++)
		{
    		arCommon = GetAttributeN(arRld, i);
			LocId = arCommon.go;
    		if (findsubstr(LocId, "Common" , 0) != -1 && LocId != pchar.GenQuest.SeekSpy.Location)
    		{			
				storeArray[howStore] = LocId;
				howStore++; 
			}
			if (arCommon.label != "Sea")
			{	
				makearef(arRld2, Locations[FindLocation(LocId)].reload);
				Qty2 = GetAttributesNum(arRld2);
				for (n=0; n<Qty2; n++)
				{
    				arCommon = GetAttributeN(arRld2, n);
					LocId = arCommon.go;
					if (findsubstr(LocId, "Common" , 0) != -1 && LocId != pchar.GenQuest.SeekSpy.Location)
    				{
						storeArray[howStore] = LocId;
						howStore++; 					
					}
				}
			}	
		}
	}
	else
	{
		for (i=0; i<Qty; i++)
		{
    		arCommon = GetAttributeN(arRld, i);
			LocId = arCommon.go;
    		if (findsubstr(LocId, "Common" , 0) != -1)
    		{			
				storeArray[howStore] = LocId;
				howStore++; 
			}
			if (arCommon.label != "Sea")
			{	
				makearef(arRld2, Locations[FindLocation(LocId)].reload);
				Qty2 = GetAttributesNum(arRld2);
				for (n=0; n<Qty2; n++)
				{
    				arCommon = GetAttributeN(arRld2, n);
					LocId = arCommon.go;
					if (findsubstr(LocId, "Common" , 0) != -1)
    				{
						storeArray[howStore] = LocId;
						howStore++; 
					}
				}
			}	
		}
	}
	if (howStore == 0) return "none";
	LocId = storeArray[rand(howStore-1)];
	SetOpenDoorCommonLoc(npchar.city, LocId); //открываем дверь
	for(n=0; n<MAX_CHARACTERS; n++)
	{
		if (CheckAttribute(&characters[n], "locations") && characters[n].locations == LocId) 
		{			
			characters[n].lifeDay = 0;
		}
	}
	return LocId;
}
