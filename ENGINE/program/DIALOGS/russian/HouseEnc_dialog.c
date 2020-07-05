
void ProcessDialogEvent()
{
	ref NPChar;
	aref Link, NextDiag, arRld, arDis;
	int i, Qty;

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

		case "exit_setOwner":
			LAi_SetOwnerTypeNoGroup(npchar);
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;

		case "fight":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
			LAi_SetOwnerTypeNoGroup(npchar);
			LAi_group_Attack(NPChar, Pchar);
			if (rand(3) != 1) SetNationRelation2MainCharacter(sti(npchar.nation), RELATION_ENEMY);
			AddDialogExitQuest("MainHeroFightModeOn");
		break;

		case "exit_GoOut":
			makearef(arRld, Locations[reload_cur_location_index].reload);
    		Qty = GetAttributesNum(arRld);
			for (i=0; i<Qty; i++)
    		{
    			arDis = GetAttributeN(arRld, i);
    			if (arDis.go == npchar.location)
    			{
					arDis.disable = true; //закрываем вход в дом
					break;
    			}
    		}
			DoQuestFunctionDelay("HouseEnc_TimerGoUot", 10.0);
			pchar.questTemp.HouseEncTimer = pchar.location;
			pchar.questTemp.HouseEncTimer.Id = npchar.id;
			LAi_SetOwnerTypeNoGroup(npchar);
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;

		case "exit_close":
			makearef(arRld, Locations[reload_cur_location_index].reload);
    		Qty = GetAttributesNum(arRld);
			for (i=0; i<Qty; i++)
    		{
    			arDis = GetAttributeN(arRld, i);
    			if (arDis.go == npchar.location)
    			{
					arDis.disable = true; //закрываем вход в дом
					break;
    			}
    		}
			LAi_SetOwnerTypeNoGroup(npchar);
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;

		//--------------------------------- мужик ---------------------------------
		case "HouseMan":
			NextDiag.TempNode = "HouseMan";
			if (LAi_grp_playeralarm > 0)
			{
       			dialog.text = PCharRepPhrase(LinkRandPhrase("В городе поднята тревога. Похоже, и мне пора браться за оружие...", "Уж не за тобой ли носится вся городская стража?.. Ко мне, солдаты!!", "У меня приюта ты не найдешь. Зато найдешь несколько дюймов холодной стали под ребро!"), 
					LinkRandPhrase("Что тебе нужно, негодяй?! Городская стража уже взяла твой след, далеко тебе не уйти, грязный пират!", "Грязный убийца! Стража!!", "Я не боюсь тебя, мерзавец! Скоро тебя повесят в нашем форте, далеко тебе не уйти..."));
				link.l1 = PCharRepPhrase(RandPhraseSimple("Похоже, тебе жить надоело...", "Хех, и не живется же спокойно мирным гражданам " + XI_ConvertString("Colony" + npchar.city + "Gen") + "!"), 
					RandPhraseSimple("Отправляйся в ад!", "Хех, жить тебе осталось несколько секунд..."));
				link.l1.go = PCharRepPhrase("exit_setOwner", "fight");
				break;
			}
			if (isBadReputation(pchar, 40))
			{
				if (CheckAttribute(pchar, "questTemp.HouseEncTimer"))
				{
					dialog.text = "Я тебя предупредил. А дальше сам смотри, нарываться или нет...";
					link.l1 = "Хех!";
					link.l1.go = "exit";
				}
				else
				{
					dialog.text = "Что тебе надо в моем доме, негодяй?! Даю тебе 10 секунд, чтобы ты убрался отсюда!";
					link.l1 = LinkRandPhrase("Хех, и здесь меня знают!", "Слава моя идет впереди меня...", "Хм, понятно.");
					link.l1.go = "exit_GoOut";
				}
			}
			else
			{
				if (IsDay())
				{
					dialog.text = NPCStringReactionRepeat("Рад приветствовать вас в моем доме. Вы по делу ко мне?", 
						"Чем я могу вам помочь?", 
						"Хм, что я могу сделать для вас?",
						"Простите, если я вам не нужен, я попрошу вас меня не беспокоить...", "block", 1, npchar, Dialog.CurrentNode);
					link.l1 = HeroStringReactionRepeat("Нет, просто осматриваю город, знакомлюсь с жителями...", 
						"Да так, ничем особенным...",
						"Ничего...", 
						"Хорошо, как скажешь.", npchar, Dialog.CurrentNode);
					link.l1.go = DialogGoNodeRepeat("HouseMan_1", "exit_setOwner", "exit_setOwner", "exit_setOwner", npchar, Dialog.CurrentNode);
				}
				else
				{
					dialog.text = NPCStringReactionRepeat("Простите, вы не вовремя появились у меня дома - ночь на дворе. Я прошу вас покинуть мой дом!", 
						"Я уже говорил, что время позднее. Прошу вас уйти.", 
						"Не хочу показаться грубым, но я настаиваю, чтобы вы немедленно покинули мой дом!",
						"Черт возьми, что это такое?! Стража, воры!!", "block", 1, npchar, Dialog.CurrentNode);
					link.l1 = HeroStringReactionRepeat("Хорошо, я понимаю...", 
						"Да, сейчас...",
						"Ладно, не кипятись.", 
						"Заткнись, какие воры?!", npchar, Dialog.CurrentNode);
					link.l1.go = DialogGoNodeRepeat("exit_close", "exit_setOwner", "exit_setOwner", "fight", npchar, Dialog.CurrentNode);
				}
			}
		break;

		case "HouseMan_1":
			dialog.text = "О-о, разрешите представиться - " + GetFullName(npchar) + ". Мы рады вас видеть, законы гостеприимства для нашего города - не пустой звук.";
			link.l1 = GetFullName(pchar) + ", с вашего позволения...";
			link.l1.go = "exit_setOwner";
		break;
		case "Man_FackYou":
			dialog.text = LinkRandPhrase("Да ты вор, милейший! Стража, держи его!!", "Вот это да! Чуть я загляделся, а ты сразу в сундук с головой! Держи вора!!", "Стража! Грабят!! Держи вора!!!");
			link.l1 = "А-ать, дьявол!!";
			link.l1.go = "fight";
		break;
		// --------------------------------- баба ---------------------------------
		case "HouseWoman":
			NextDiag.TempNode = "HouseWoman";
			if (LAi_grp_playeralarm > 0)
			{
       			dialog.text = PCharRepPhrase(LinkRandPhrase("Предупреждаю, что скоро домой явится мой муж! Шли бы вы из моего дома...", "Не думаю, что вам удаться сбежать. И знайте, что скоро домой должен прийти мой муж!", "Мой муж скоро будет дома, вам немедленно нужно уйти отсюда!"), 
					LinkRandPhrase("Эх, жаль мужа нет дома... Убирайся!!", "Грязный убийца, вон из моего дома! Стража!!", "Каков мерзавец... Скоро прийдет мой муж и посмотрит, какого цвета у тебя кровь!"));
				link.l1 = PCharRepPhrase(RandPhraseSimple("Муж, объелся груш...", "Хех, мужем меня не напугаешь, киска..."), 
					RandPhraseSimple("Заткни свой ротик, а не то кишки выпущу...", "Я останусь здесь только, сколько захочу. А ты сиди тихо, не то хуже будет..."));
				link.l1.go = "exit_setOwner";
				break;
			}
			if (isBadReputation(pchar, 40))
			{
				if (CheckAttribute(pchar, "questTemp.HouseEncTimer"))
				{
					dialog.text = "Я тебя предупредила. Убирайся, или будет хуже!";
					link.l1 = "Одно слово - дура...";
					link.l1.go = "exit";
				}
				else
				{					
					dialog.text = "Что тебе надо в моем доме, негодяй?! Если ты не уберешься через 10 секунд, то я позову стражу!";
					link.l1 = "Вот дура...";
					link.l1.go = "exit_GoOut";
				}
			}
			else
			{
				if (IsDay())
				{
					dialog.text = NPCStringReactionRepeat("Я рада вас видеть в нашем доме. Меня зовут " + GetFullName(npchar) + ". Чем я могу вам помочь?", 
						"Вы еще здесь?", 
						"Хм, простите, но вам не кажется, что вы уже засиделись у нас?",
						"Я прошу вас не злоупотреблять нашим гостеприимством.", "block", 1, npchar, Dialog.CurrentNode);
					link.l1 = HeroStringReactionRepeat( GetFullName(pchar) + " к вашим услугам. К вам зашел без конкретного дела, познакомиться.", 
						"Еще здесь.",
						"Ну, как сказать...", 
						"Хорошо.", npchar, Dialog.CurrentNode);
					link.l1.go = DialogGoNodeRepeat("HouseWoman_1", "exit_setOwner", "exit_setOwner", "exit_setOwner", npchar, Dialog.CurrentNode);
				}
				else
				{
					dialog.text = NPCStringReactionRepeat("Ах, господи, что же это вы врываетесь в дом в такое время! Прямо испугали меня... Прошу вас уйти, приходите к нам завтра.", 
						"Уйдите же, прошу вас! Я замужняя женщина!!", 
						"Я последний раз добром прошу вас покинуть наш дом!",
						"Да что же это делается?!!", "block", 1, npchar, Dialog.CurrentNode);
					link.l1 = HeroStringReactionRepeat("Не бойтесь, я ничего плохого вам не сделаю.", 
						"Хорошо, не надо волноваться...",
						"Ухожу.", 
						"В чем дело?", npchar, Dialog.CurrentNode);
					link.l1.go = DialogGoNodeRepeat("exit_close", "exit_setOwner", "exit_setOwner", "HouseWoman_2", npchar, Dialog.CurrentNode);
				}
			}
		break;

		case "HouseWoman_1":
			dialog.text = "Хорошо, всегда рады гостям. Только прошу вас не засиживаться у меня, так как я замужем...";
			link.l1 = "О-о, да, конечно...";
			link.l1.go = "exit_setOwner";
		break;
		case "Woman_FackYou":
			dialog.text = "Ах, вот, значит, как?! Я вас пустила в дом как гостя, а вы по сундуками шарить вздумали?! Стража-а-а!!";
			link.l1 = "Заткнись, дура...";
			link.l1.go = "exit_setOwner";
			LAi_group_Attack(NPChar, Pchar);
			if (rand(3) != 1) SetNationRelation2MainCharacter(sti(npchar.nation), RELATION_ENEMY);
		break;
		case "HouseWoman_2":
			dialog.text = "Сколько я тебя просила по-хорошему убраться из дома! Ну, хватит... Помогите! Стража!!";
			link.l1 = "Заткнись дура, с ума сошла?!";
			link.l1.go = "exit_setOwner";
			LAi_group_Attack(NPChar, Pchar);
			if (rand(3) != 1) SetNationRelation2MainCharacter(sti(npchar.nation), RELATION_ENEMY);
		break; 
		//--------------------------------- завсклад ---------------------------------
		case "SkladMan":
			NextDiag.TempNode = "SkladMan";
			if (LAi_grp_playeralarm > 0)
			{
       			dialog.text = NPCharRepPhrase(pchar, 
					LinkRandPhrase("В городе поднята тревога. Похоже, и мне пора браться за оружие...", "Уж не за тобой ли носится вся городская стража?.. Ко мне, солдаты!!", "У меня приюта ты не найдешь. Зато найдешь несколько дюймов холодной стали под ребро!"), 
					LinkRandPhrase("Что тебе нужно, негодяй?! Городская стража уже взяла твой след, далеко тебе не уйти, грязный пират!", "Грязный убийца! Стража!!", "Я не боюсь тебя, мерзавец! Скоро тебя повесят в нашем форте, далеко тебе не уйти..."));
				link.l1 = NPCharRepPhrase(pchar,
					RandPhraseSimple("Похоже, тебе жить надоело...", "Хех, и не живется же спокойно мирным гражданам " + XI_ConvertString("Colony" + npchar.city + "Gen") + "!"), 
					RandPhraseSimple("Отправляйся в ад!", "Хех, жить тебе осталось несколько секунд..."));
				link.l1.go = NPCharRepPhrase("exit_setOwner", "fight");
				break;
			}
			if (isBadReputation(pchar, 40))
			{
				dialog.text = "Что тебе нужно здесь?.. Постой, я, кажется, знаю тебя... Стража, держи мерзавца!!";
				link.l1 = "Сейчас ты умолкнешь навеки...";
				link.l1.go = "fight";
			}
			else
			{
				dialog.text = NPCStringReactionRepeat("Приветсвую вас! Меня зовут " + GetFullName(npchar) + ". Я здесь заведую всем, так что не вздумай прихватить что-то с собой...", 
					"Веди себя прилично. И помни, что я смотрю за тобой.", 
					"Пока ты не лезешь в сундуки - все в порядке, можешь находиться здесь. Одному мне скучно...",
					RandPhraseSimple("Эх, тоска тут смертная!", "Эх, черт возьми, чем бы заняться?.."), "block", 3, npchar, Dialog.CurrentNode);
				link.l1 = HeroStringReactionRepeat("Хорошо, не переживай.", 
					"Конечно!",
					"Понимаю...", 
					"Да, проблемы у тебя серьезные...", npchar, Dialog.CurrentNode);
				link.l1.go = DialogGoNodeRepeat("exit", "exit", "exit", "exit", npchar, Dialog.CurrentNode);
				//открывание двери верфи по квесту промышленного шпионажа
				if (CheckAttribute(pchar, "questTemp.different.ShipyardsMap") && pchar.questTemp.different.ShipyardsMap == "toTarget" && npchar.city == pchar.questTemp.different.ShipyardsMap.city && locations[reload_cur_location_index].type == "shipyard")
				{
					link.l2 = "Послушай, приятель, хочу поговорить с тобой откровенно.";
					link.l2.go = "ShipyardsMap_1";
					pchar.questTemp.different.ShipyardsMap = "toResult";
				}
			}
		break;
		case "ShipyardsMap_1":
			dialog.text = "Хе! Ну, давай поговорим.";
			link.l1 = "Мне нужно попасть на верфь ночью, когда никого нет.";
			link.l1.go = "ShipyardsMap_2";
		break;
		case "ShipyardsMap_2":
			if (sti(pchar.questTemp.different.ShipyardsMap.skladFight))
			{
				dialog.text = "Вот это да!! Эй, стража, тут вор подбирается!!";
				link.l1 = "Какой вор?! Я поговорить хотел!..";
				link.l1.go = "fight";
				AddCharacterExpToSkill(pchar, "FencingLight", 5);
				AddCharacterExpToSkill(pchar, "Fencing", 5);
				AddCharacterExpToSkill(pchar, "FencingHeavy", 5);
				ChangeCharacterReputation(pchar, -1);
			}
			else
			{
				dialog.text = "Хо! Ну ты даешь приятель. Пришел на склад верфи с такой просьбой!";
				link.l1 = "Мне не на склад надо, а на верфь. Сюда я ни ногой...";
				link.l1.go = "ShipyardsMap_3";
				AddCharacterExpToSkill(pchar, "Fortune", 10);
			}
		break;
		case "ShipyardsMap_3":
			dialog.text = "Зачем тебе это?";
			link.l1 = "Надо приятель, надо. Я готов заплатить...";
			link.l1.go = "ShipyardsMap_4";
		break;
		case "ShipyardsMap_4":
			dialog.text = "Хм, уже интересней... В общем так! Гони " + FindRussianMoneyString(sti(pchar.questTemp.different.ShipyardsMap.sklad)*1000) + ", и следующие сутки входную дверь верфи я буду оставлять открытой.";
			link.l1 = "Это слишком дорого для меня. Обойдусь...";
			link.l1.go = "exit";
			if (sti(pchar.money) >= (sti(pchar.questTemp.different.ShipyardsMap.sklad)*1000))
			{
				link.l2 = "Хорошо, я согласен. Забирай деньги и делай, как договорились.";
				link.l2.go = "ShipyardsMap_5";
			}
		break;
		case "ShipyardsMap_5":
			dialog.text = "Не волнуйся, все сделаю.";
			link.l1 = "Хорошо, верю...";
			link.l1.go = "exit";
			AddMoneyToCharacter(pchar, -sti(pchar.questTemp.different.ShipyardsMap.sklad)*1000);
			AddQuestRecord("ShipyardsMap", "5");
			AddQuestUserData("ShipyardsMap", "sCity", XI_ConvertString("Colony" + npchar.city + "Gen"));
			//снимаем close_for_night
			string  sName;
			int location_index = FindLocation(npchar.city + "_town");
    		makearef(arRld, Locations[location_index].reload);
    		Qty = GetAttributesNum(arRld);
    		for (int a=0; a<Qty; a++)
    		{
    			arDis = GetAttributeN(arRld, a);
				sName = arDis.go;
    			if (findsubstr(sName, "_shipyard" , 0) != -1)
    			{
					DeleteAttribute(arDis, "close_for_night");
					break;
    			}
    		}
			LocatorReloadEnterDisable(npchar.city + "_shipyard", "reload2", true);
			//ставим таймер на возврат close_for_night
			SetTimerFunction("ShipyardsMap_returnCFN", 0, 0, 2);
		break;

	}
}
