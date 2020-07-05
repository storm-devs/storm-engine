void ProcessDialogEvent()
{
	ref NPChar;
	aref Link, Diag;
	string NPC_Meeting;	
	int Shit, i;
	ref refStore;
	
	DeleteAttribute(&Dialog,"Links");

	makeref(NPChar,CharacterRef);
	makearef(Link, Dialog.Links);
	makearef(Diag, NPChar.Dialog);
	
	switch(Dialog.CurrentNode)
	{
        case "exit":
			Diag.CurrentNode = Diag.TempNode;
			NPChar.quest.meeting = NPC_Meeting;
			DialogExit();
		break;

        case "First time":
			Diag.TempNode = "first time";
			if(CheckAttribute(PChar, "quest.Contraband.active"))
			{
				Dialog.snd = "voice\SMSH\SMSH001";
				dialog.Text = RandPhraseSimple("Эй, что ты тут забыл, приятель?",
                                          RandSwear() + "Что тебе тут нужно?!");
				Link.l1 = RandPhraseSimple("Расслабься, парень, я привез товар!", "Ты чем смотришь? Я доставил товар!");
				Pchar.quest.Contraband.Counter = 0; // не торговали
				if(Pchar.Location == Pchar.location.from_sea)
				{
					Link.l1.go = "Exchange";
				}
				else
				{
					Link.l1.go = "No_Ship";
				}
				//по заданию губернатора, истребление контры, только эта ветка.
				if (CheckAttribute(pchar, "GenQuest.KillSmugglers") && pchar.GenQuest.KillSmugglers == "" && pchar.GenQuest.KillSmugglers.Areal == GiveArealByLocation(&locations[FindLocation(pchar.location)]))				
				{
					Link.l2 = "Я здесь по распоряжению губернатора " + XI_ConvertString("Colony"+characters[GetCharacterIndex(pchar.GenQuest.KillSmugglers.MayorId)].city+"Gen") + "! Приказываю вам сложить оружие и следовать за мной - вы арестованы!";
					Link.l2.go = "GenQuestKillContraband_1";
				}
			}
			else
			{
//================ METRO ====================
				//если заплатил, то разговаривают, иначе посылают
				if (CheckAttribute(PChar, "GenQuest.contraTravel.payed") && sti(PChar.GenQuest.contraTravel.payed) == true)
				{
                    dialog.Text = RandPhraseSimple("Ага, наконец то!!! Вот и ты, а то мы уж заждались совсем. Идем, мы отчаливаем.", "Пошевеливайся, тут повсюду патрули. Нужно быстрее убраться отсюда!");
					//по заданию губернатора, истребление контры, только эта ветка.
					if (CheckAttribute(pchar, "GenQuest.KillSmugglers") && pchar.GenQuest.KillSmugglers == "" && pchar.GenQuest.KillSmugglers.Areal == GiveArealByLocation(&locations[FindLocation(pchar.location)]))
					{
						Link.l1 = "Я здесь по распоряжению губернатора " + XI_ConvertString("Colony"+characters[GetCharacterIndex(pchar.GenQuest.KillSmugglers.MayorId)].city+"Gen") + "! Приказываю вам сложить оружие и следовать за мной - вы арестованы!";
						Link.l1.go = "GenQuestKillContraband_1";
						break;
					}
					//если набрал пассажиров, в сад..
					if (GetPassengersQuantity(PChar) > 0)
					{
						dialog.Text = "А ты кто такой, мы договаривались доставить одного пассажира, а не толпу.";
						Link.l1 = "Упс...";
						Link.l1.go = "Exit";
						break;
					}
					//если набрал компаньонов или купил корабль себе уже :), в сад..
					if (GetCompanionQuantity(PChar) > 1 || sti(PChar.ship.type) != SHIP_NOTUSED)
					{
						dialog.Text = RandPhraseSimple("Ты кто? Мы договаривались доставить пассажира, а не капитана.", "Проваливай!! Пока не избавишься от своего корабля, не приходи.");
						Link.l1 = "Ах черт...";
						Link.l1.go = "Exit";
						break;
					}
					//типа помог отбиться, свой парень... 
					if (CheckAttribute(PChar, "GenQuest.contraTravel.PatrolFight") && sti(PChar.GenQuest.contraTravel.PatrolFight) == true)
					{
						if (chrDisableReloadToLocation) // еще бой идет
						{
						    dialog.Text = "Патруль! Мы тебя не знаем, ты нас не знаешь.";
							Link.l1 = "Да уж...";
							Link.l1.go = "Exit";
							break;
						}
						dialog.Text = RandPhraseSimple("Спасибо за помощь. Мы не забудем этого. Отправляемся.", "Молодец! Отлично сражался. Идем.");
						ChangeContrabandRelation(PChar, 5);
						PChar.GenQuest.contraTravel.PatrolFight = false;
						AddCharacterExpToSkill(Pchar, "Sneak", 10 + 4 * makeint(sti(PChar.rank)/2));
					}
					//тут все ок, отправляемся.
					Link.l2 = "Уже иду.";
					Link.l2.go = "Exit";
					//а это патруль... 
					if (GetSummonSkillFromNameToOld(Pchar, SKILL_SNEAK) < frandSmall(13.0*(1.0 - pow(0.9, sti(PChar.rank)))) && !CheckAttribute(PChar, "GenQuest.contraTravel.PatrolFight"))
					{
						AddDialogExitQuest("Rand_ContrabandInterruption");
						PChar.GenQuest.contraTravel.PatrolFight = true;
					}
					else 
					{
						//поместим Грея в локацию.
						refStore = CharacterFromID("Abracham_Gray");
						ChangeCharacterAddressGroup(refStore, "Ship_deck", "goto", "goto2");
						refStore.dialog.Filename = "Smuggler_Ship_dialog.c";
                        refStore.dialog.CurrentNode = "Travel_talkStart";
						//поплыл, иначе RemoveTravelSmugglers грохнет всю ветку
						PChar.GenQuest.contraTravel.ship = true;
						PChar.quest.Munity = "";  // признак выхода с палубы
						
						SetLaunchFrameFormParam("Прошло " + sti(Pchar.GenQuest.contraTravel.destination.days) + " дней." + NewStr() + "Палуба корабля контрабандистов.",
						                        "Reload_To_Location", 0.1, 5.0);
                        bQuestCheckProcessFreeze = true;
						WaitDate("", 0, 0, sti(Pchar.GenQuest.contraTravel.destination.days), rand(20), 0);
						bQuestCheckProcessFreeze = false;
						MakeCloneShipDeck(refStore, true); // подмена палубы
						SetLaunchFrameReloadLocationParam("Ship_deck", "reload", "reload1", "Travel_talkOnDeck");
						AddDialogExitQuest("LaunchFrameForm");
						AddCharacterExpToSkill(Pchar, "Sneak", 10 + 4 * makeint(sti(PChar.rank)/2));
					}
//================ METRO ====================
				}
				else
				{
                    // если таможня уже бежит
					if (CheckAttribute(NPChar, "ContrabandInterruption"))
					{
					    dialog.Text = RandSwear()+ "Патруль! Мы тебя не знаем, ты нас не знаешь!";
						Link.l1 = "Точно!";
						Link.l1.go = "Exit";
					}
					else
					{
						dialog.Text = "Слушай, проваливай отсюда! Сорвешь сделку - мы выпустим тебе кишки!";
						Link.l1 = RandPhraseSimple("Ты смеешь мне угрожать, собака?", "Ты подавишься своими словами, каналья!");
						Link.l1.go = "Exit_fight";
						Link.l2 = "Да ладно, я знаю, что такое бизнес. До встречи.";
						Link.l2.go = "Exit";
					}
				}
			}
		break;

		case "No_Ship":
			Dialog.snd = "voice\SMSH\SMSH003";
			dialog.Text = "На своем горбу, что ли? Где твой корабль?";
			Link.l1 = RandPhraseSimple("Сначала я хотел убедиться, что здесь нет никакого подвоха.", "Не хотел рисковать кораблем. Предпочитаю сначала поглядеть на все своими глазами.");
			Link.l1.go = "No_ship_1";
			Link.l2 = "Сделка отменяется!";
			Link.l2.go = "Cancellation";
		break;

		case "No_ship_1":
			Dialog.snd = "voice\SMSH\SMSH004";
			dialog.Text = "Мы люди честные, и обманом не живем. Приводите свой корабль, только аккуратнее - чтобы за вами не увязался береговой патруль.";
			Link.l1 = "Хорошо. Ждите меня здесь.";
			Link.l1.go = "Exit";
		break;
		
		case "Exit_fight":
            //DeleteAttribute(Pchar, "quest.Contraband");
            DeleteAttribute(Pchar, "quest.Contraband.active");
			DeleteAttribute(Pchar, "quest.Contraband.counter");
			DeleteAttribute(Pchar, "quest.Contraband.price1");
			DeleteAttribute(Pchar, "quest.Contraband.price2");
			DeleteAttribute(Pchar, "quest.Contraband.price3");
			DeleteAttribute(Pchar, "quest.Contraband.price4");
			DeleteAttribute(Pchar, "quest.Contraband.price5");
			DeleteAttribute(Pchar, "quest.Contraband.price6");
			DeleteAttribute(Pchar, "quest.Contraband.price7");
			DeleteAttribute(Pchar, "quest.Contraband.goodsIDX1");
			DeleteAttribute(Pchar, "quest.Contraband.goodsIDX2");
			DeleteAttribute(Pchar, "quest.Contraband.goodsIDX3");
			DeleteAttribute(Pchar, "quest.Contraband.goodsIDX4");
			DeleteAttribute(Pchar, "quest.Contraband.goodsIDX5");
			DeleteAttribute(Pchar, "quest.Contraband.goodsIDX6");
			DeleteAttribute(Pchar, "quest.Contraband.goodsIDX7");
			DeleteAttribute(Pchar, "quest.Contraband.sum");
			DeleteAttribute(Pchar, "quest.Contraband.ChooseGoodsQty");
			DeleteAttribute(Pchar, "quest.Contraband.ChooseGoodsID");


			CloseQuestHeader("Gen_Contraband");
			Pchar.quest.Rand_Smuggling.over = "yes";
			LAi_group_FightGroups(LAI_GROUP_PLAYER, pchar.GenQuest.Smugglers_Group, true);
			RemoveSmugglersFromShore();
			LAi_SetFightMode(Pchar, true);

			Diag.CurrentNode = Diag.TempNode;
			NPChar.quest.meeting = NPC_Meeting;
			DialogExit();
		break;

		case "Exit_Cancel":
            DeleteAttribute(Pchar, "quest.Contraband");
            CloseQuestHeader("Gen_Contraband");
			Pchar.quest.Rand_Smuggling.over = "yes";
			RemoveSmugglersFromShore();
			Diag.CurrentNode = Diag.TempNode;
			NPChar.quest.meeting = NPC_Meeting;
			DialogExit();
		break;
		
		case "Cancellation":
            if (sti(Pchar.quest.Contraband.Counter) == 0)
            {
    			dialog.Text = "Отменяется? Ты что, смеешься?";
    			Link.l1 = "Совершенно серьезно.";
    			Link.l1.go = "Cancellation_1";
			}
			else
            {
    			dialog.Text = "Ну и ладно. Мы и так неплохо поторговали.";
    			Link.l1 = "Точно!";
    			Link.l1.go = "Finish_exit";
			}
		break;

		case "Cancellation_1":
			if( makeint(Pchar.rank) <= 3 || GetSummonSkillFromNameToOld(Pchar, SKILL_FENCING) <= 5 || GetSummonSkillFromNameToOld(Pchar, SKILL_LEADERSHIP) <= 5 )
			{
				if(Rand(1) == 1)
				{
					ChangeContrabandRelation(pchar, -30);
					Dialog.snd = "voice\SMSH\SMSH006";
					dialog.Text = "Паршивец!! Ты не можешь нас так дурачить! Поплатишься за это своей головой!";
					Link.l1 = "А вот это вряд ли!";
					Link.l1.go = "Exit_fight";			
				}
				else
				{
					ChangeContrabandRelation(pchar, -10);
					Dialog.snd = "voice\SMSH\SMSH007";
					dialog.Text = "Это не сойдет тебе с рук!";
					Link.l1 = "Отправляйтесь гавкать куда-нибудь еще, псы!";
					Link.l1.go = "Exit_cancel";
				}
			}	
			else
			{
				ChangeContrabandRelation(pchar, -10);
				Dialog.snd = "voice\SMSH\SMSH008";
				dialog.Text = "Ты об этом еще пожалеешь!";
				Link.l1 = "Ну-ну!";
				Link.l1.go = "Exit_cancel";			
			}	
		break;
		
        case "GenQuestKillContraband_1":
			//счетчик подстав по "метро"...
			if(CheckAttribute(PChar, "GenQuest.contraTravel.active") && sti(PChar.GenQuest.contraTravel.active) == true)
			{
				Statistic_AddValue(PChar, "contr_TravelKill", 1);
				ChangeContrabandRelation(pchar, -20); //репу контры вниз
			}
			dialog.Text = "Ты нам сейчас ответишь за такую подставу!";
			Link.l1 = "Тогда вы все умрете. У меня четкий приказ - живых не брать!";
			Link.l1.go = "Exit_fight";
		break;
		
		case "Exchange":
			// сама торговля -->
            if(FindFirstContrabandGoods(Pchar) == -1 && sti(Pchar.quest.Contraband.Counter) == 0)
            {
				dialog.Text = "Ну и чего ты здесь делаешь? У тебя же ничего нет, что можно продать! Все, что лежит у тебя в трюме, можно купить в городе!";
				Link.l1 = "Не повезло.";
				Link.l1.go = "NoGoods";
			}
			else
			{
                // установим окружение -->
                if (sti(Pchar.quest.Contraband.Counter) == 0) // не торговали еще
                {
                    if(rand(11) == 3)
                    {
        				dialog.Text = "Так-так.. пожалуй, мы заберем твой товар бесплатно. Врядли ты побежишь жаловаться властям.";
        				Link.l1 = "Что ж, попробуйте!";
        				Link.l1.go = "Exit_fight";
                        break;
        			}
        			// море и солдаты на суше НЕЗАВИСИМЫ!!!
        			if(GetSummonSkillFromNameToOld(Pchar, SKILL_SNEAK) < Rand(12))
        			{
                        SetCoastalGuardPursuit();
        			}
        			// при убегании от патруля на карту - корабли трем
        			SetTimerCondition("Rand_ContrabandInterruptionAtSeaEnded", 0, 0, 2, false);// если в порту сидим, спим, то 2 день
        			
                    Pchar.quest.Rand_ContrabandAtSeaEnded.win_condition.l1 = "MapEnter";
        			Pchar.quest.Rand_ContrabandAtSeaEnded.win_condition = "Rand_ContrabandAtSeaEnded";
    			}
    			// установим окружение <--
                dialog.Text = RandPhraseSimple("Итак, какой груз ты хотел продать?",
                                          "Что продаешь?");
                shit = FindFirstContrabandGoods(Pchar);
    			if(shit != -1)
    			{
    				Pchar.quest.Contraband.goodsIDX1 = shit;
    				Pchar.quest.Contraband.price1 = GetCurrentIslandGoodsPrice(shit);
    				Link.l1 = XI_ConvertString(Goods[shit].name);
				    Link.l1.go = "goodsIDX1";
    			}
    			shit = FindNextContrabandGoods(Pchar);
    			if(shit != -1)
    			{
    				Pchar.quest.Contraband.goodsIDX2 = shit;
    				Pchar.quest.Contraband.price2 = GetCurrentIslandGoodsPrice(shit);
    				Link.l2 = XI_ConvertString(Goods[shit].name);
				    Link.l2.go = "goodsIDX2";
    			}
    			shit = FindNextContrabandGoods(Pchar);
    			if(shit != -1)
    			{
    				Pchar.quest.Contraband.goodsIDX3 = shit;
    				Pchar.quest.Contraband.price3 = GetCurrentIslandGoodsPrice(shit);
    				Link.l3 = XI_ConvertString(Goods[shit].name);
				    Link.l3.go = "goodsIDX3";
    			}
    			shit = FindNextContrabandGoods(Pchar);
    			if(shit != -1)
    			{
    				Pchar.quest.Contraband.goodsIDX4 = shit;
    				Pchar.quest.Contraband.price4 = GetCurrentIslandGoodsPrice(shit);
    				Link.l4 = XI_ConvertString(Goods[shit].name);
				    Link.l4.go = "goodsIDX4";
    			}
    			shit = FindNextContrabandGoods(Pchar);
    			if(shit != -1)
    			{
    				Pchar.quest.Contraband.goodsIDX5 = shit;
    				Pchar.quest.Contraband.price5 = GetCurrentIslandGoodsPrice(shit);
    				Link.l5 = XI_ConvertString(Goods[shit].name);
				    Link.l5.go = "goodsIDX5";
    			}
    			if(shit != -1)
    			{
    				Pchar.quest.Contraband.goodsIDX6 = shit;
    				Pchar.quest.Contraband.price6 = GetCurrentIslandGoodsPrice(shit);
    				Link.l6 = XI_ConvertString(Goods[shit].name);
				    Link.l6.go = "goodsIDX6";
    			}
    			if(shit != -1)
    			{
    				Pchar.quest.Contraband.goodsIDX7 = shit;
    				Pchar.quest.Contraband.price7 = GetCurrentIslandGoodsPrice(shit);
    				Link.l7 = XI_ConvertString(Goods[shit].name);
				    Link.l7.go = "goodsIDX7";
    			}
    			if (sti(Pchar.quest.Contraband.Counter) == 0)
    			{
        			Link.l99 = "Пожалуй, я передумал.";
    				Link.l99.go = "Cancellation";
				}
				else
				{
        			Link.l99 = "Думаю, с меня хватит.";
    				Link.l99.go = "Finish_exit";
				}
            }
		break;

        case "goodsIDX1":
            Pchar.quest.Contraband.ChooseGoodsID    = Pchar.quest.Contraband.goodsIDX1;
            Pchar.quest.Contraband.ChooseGoodsPrice = sti(Pchar.quest.Contraband.price1);
            Pchar.quest.Contraband.ChooseGoodsUnit  = sti(Goods[sti(Pchar.quest.Contraband.ChooseGoodsID)].Units);

			dialog.Text = XI_ConvertString(Goods[sti(Pchar.quest.Contraband.ChooseGoodsID)].name) + "? Цена " + Pchar.quest.Contraband.ChooseGoodsPrice +" золотых за "+ Pchar.quest.Contraband.ChooseGoodsUnit+" шт. Давай посмотрим сколько у тебя этого груза.";
			Link.l1 = "Давай.";
			Link.l1.go = "Make_trade_1";
			Link.l2 = "Нет. Пожалуй, поищу другого покупателя. Сделка отменяется!";
			Link.l2.go = "Cancellation";
		break;
		
		case "goodsIDX2":
            Pchar.quest.Contraband.ChooseGoodsID    = Pchar.quest.Contraband.goodsIDX2;
            Pchar.quest.Contraband.ChooseGoodsPrice = sti(Pchar.quest.Contraband.price2);
            Pchar.quest.Contraband.ChooseGoodsUnit  = sti(Goods[sti(Pchar.quest.Contraband.ChooseGoodsID)].Units);

			dialog.Text = XI_ConvertString(Goods[sti(Pchar.quest.Contraband.ChooseGoodsID)].name) + "? Цена " + Pchar.quest.Contraband.ChooseGoodsPrice +" золотых за "+ Pchar.quest.Contraband.ChooseGoodsUnit+" шт. Давай посмотрим сколько у тебя этого груза.";
			Link.l1 = "Давай.";
			Link.l1.go = "Make_trade_1";
			Link.l2 = "Нет. Пожалуй, поищу другого покупателя. Сделка отменяется!";
			Link.l2.go = "Cancellation";
		break;
		
		case "goodsIDX3":
            Pchar.quest.Contraband.ChooseGoodsID    = Pchar.quest.Contraband.goodsIDX3;
            Pchar.quest.Contraband.ChooseGoodsPrice = sti(Pchar.quest.Contraband.price3);
            Pchar.quest.Contraband.ChooseGoodsUnit  = sti(Goods[sti(Pchar.quest.Contraband.ChooseGoodsID)].Units);

			dialog.Text = XI_ConvertString(Goods[sti(Pchar.quest.Contraband.ChooseGoodsID)].name) + "? Цена " + Pchar.quest.Contraband.ChooseGoodsPrice +" золотых за "+ Pchar.quest.Contraband.ChooseGoodsUnit+" шт. Давай посмотрим сколько у тебя этого груза.";
			Link.l1 = "Давай.";
			Link.l1.go = "Make_trade_1";
			Link.l2 = "Нет. Пожалуй, поищу другого покупателя. Сделка отменяется!";
			Link.l2.go = "Cancellation";
		break;
		
		case "goodsIDX4":
            Pchar.quest.Contraband.ChooseGoodsID    = Pchar.quest.Contraband.goodsIDX4;
            Pchar.quest.Contraband.ChooseGoodsPrice = sti(Pchar.quest.Contraband.price4);
            Pchar.quest.Contraband.ChooseGoodsUnit  = sti(Goods[sti(Pchar.quest.Contraband.ChooseGoodsID)].Units);

			dialog.Text = XI_ConvertString(Goods[sti(Pchar.quest.Contraband.ChooseGoodsID)].name) + "? Цена " + Pchar.quest.Contraband.ChooseGoodsPrice +" золотых за "+ Pchar.quest.Contraband.ChooseGoodsUnit+" шт. Давай посмотрим сколько у тебя этого груза.";
			Link.l1 = "Давай.";
			Link.l1.go = "Make_trade_1";
			Link.l2 = "Нет. Пожалуй, поищу другого покупателя. Сделка отменяется!";
			Link.l2.go = "Cancellation";
		break;
		
		case "goodsIDX5":
            Pchar.quest.Contraband.ChooseGoodsID    = Pchar.quest.Contraband.goodsIDX5;
            Pchar.quest.Contraband.ChooseGoodsPrice = sti(Pchar.quest.Contraband.price5);
            Pchar.quest.Contraband.ChooseGoodsUnit  = sti(Goods[sti(Pchar.quest.Contraband.ChooseGoodsID)].Units);

			dialog.Text = XI_ConvertString(Goods[sti(Pchar.quest.Contraband.ChooseGoodsID)].name) + "? Цена " + Pchar.quest.Contraband.ChooseGoodsPrice +" золотых за "+ Pchar.quest.Contraband.ChooseGoodsUnit+" шт. Давай посмотрим сколько у тебя этого груза.";
			Link.l1 = "Давай.";
			Link.l1.go = "Make_trade_1";
			Link.l2 = "Нет. Пожалуй, поищу другого покупателя. Сделка отменяется!";
			Link.l2.go = "Cancellation";
		break;
		
		case "goodsIDX6":
            Pchar.quest.Contraband.ChooseGoodsID    = Pchar.quest.Contraband.goodsIDX6;
            Pchar.quest.Contraband.ChooseGoodsPrice = sti(Pchar.quest.Contraband.price6);
            Pchar.quest.Contraband.ChooseGoodsUnit  = sti(Goods[sti(Pchar.quest.Contraband.ChooseGoodsID)].Units);

			dialog.Text = XI_ConvertString(Goods[sti(Pchar.quest.Contraband.ChooseGoodsID)].name) + "? Цена " + Pchar.quest.Contraband.ChooseGoodsPrice +" золотых за "+ Pchar.quest.Contraband.ChooseGoodsUnit+" шт. Давай посмотрим сколько у тебя этого груза.";
			Link.l1 = "Давай.";
			Link.l1.go = "Make_trade_1";
			Link.l2 = "Нет. Пожалуй, поищу другого покупателя. Сделка отменяется!";
			Link.l2.go = "Cancellation";
		break;
		
		case "goodsIDX7":
            Pchar.quest.Contraband.ChooseGoodsID    = Pchar.quest.Contraband.goodsIDX7;
            Pchar.quest.Contraband.ChooseGoodsPrice = sti(Pchar.quest.Contraband.price7);
            Pchar.quest.Contraband.ChooseGoodsUnit  = sti(Goods[sti(Pchar.quest.Contraband.ChooseGoodsID)].Units);

			dialog.Text = XI_ConvertString(Goods[sti(Pchar.quest.Contraband.ChooseGoodsID)].name) + "? Цена " + Pchar.quest.Contraband.ChooseGoodsPrice +" золотых за "+ Pchar.quest.Contraband.ChooseGoodsUnit+" шт. Давай посмотрим сколько у тебя этого груза.";
			Link.l1 = "Давай.";
			Link.l1.go = "Make_trade_1";
			Link.l2 = "Нет. Пожалуй, поищу другого покупателя. Сделка отменяется!";
			Link.l2.go = "Cancellation";
		break;
		
		case "Make_trade_1":
			dialog.Text = "Груз '"+XI_ConvertString(Goods[sti(Pchar.quest.Contraband.ChooseGoodsID)].name) + "', количество "+
                     GetSquadronGoods(Pchar, sti(Pchar.quest.Contraband.ChooseGoodsID)) + " шт. Цена " + Pchar.quest.Contraband.ChooseGoodsPrice +" за "+Pchar.quest.Contraband.ChooseGoodsUnit+" шт. Сколько продаешь?";
			Link.l1 = "Все, что есть.";
			Link.l1.go = "Make_trade_Sell_All";
			Link.l2 = "Давай половину.";
			Link.l2.go = "Make_trade_Sell_Half";
			Link.l3 = "Десятую часть";
			Link.l3.go = "Make_trade_Sell_10th";
			Link.l9 = "Нет. Пожалуй, поищу другого покупателя. Сделка отменяется!";
			Link.l9.go = "Cancellation";
		break;
		
		case "Make_trade_Sell_All":
            Pchar.quest.Contraband.ChooseGoodsQty =  GetSquadronGoods(Pchar, sti(Pchar.quest.Contraband.ChooseGoodsID));
            pchar.quest.contraband.sum = makeint(stf(Pchar.quest.Contraband.ChooseGoodsQty) * stf(Pchar.quest.Contraband.ChooseGoodsPrice) / stf(Pchar.quest.Contraband.ChooseGoodsUnit));
            
			dialog.Text = "Итого: " + Pchar.quest.Contraband.ChooseGoodsQty + " шт. за "+ pchar.quest.contraband.sum + " золотых. Идет?";
			Link.l1 = "Да, вполне.";
			Link.l1.go = "Make_trade_End";
			Link.l9 = "Нет. Сделка отменяется!";
			Link.l9.go = "Cancellation";
		break;
		
		case "Make_trade_Sell_Half":
            Pchar.quest.Contraband.ChooseGoodsQty =  makeint(GetSquadronGoods(Pchar, sti(Pchar.quest.Contraband.ChooseGoodsID)) / 2);
            pchar.quest.contraband.sum = makeint(stf(Pchar.quest.Contraband.ChooseGoodsQty) * stf(Pchar.quest.Contraband.ChooseGoodsPrice) / stf(Pchar.quest.Contraband.ChooseGoodsUnit));

			dialog.Text = "Итого: " + Pchar.quest.Contraband.ChooseGoodsQty + " шт. за "+ pchar.quest.contraband.sum + " золотых. Идет?";
			Link.l1 = "Да, вполне.";
			Link.l1.go = "Make_trade_End";
			Link.l9 = "Нет. Сделка отменяется!";
			Link.l9.go = "Cancellation";
		break;
		
		case "Make_trade_Sell_10th":
            Pchar.quest.Contraband.ChooseGoodsQty =  makeint(GetSquadronGoods(Pchar, sti(Pchar.quest.Contraband.ChooseGoodsID)) / 10);
            pchar.quest.contraband.sum = makeint(stf(Pchar.quest.Contraband.ChooseGoodsQty) * stf(Pchar.quest.Contraband.ChooseGoodsPrice) / stf(Pchar.quest.Contraband.ChooseGoodsUnit));

			dialog.Text = "Итого: " + Pchar.quest.Contraband.ChooseGoodsQty + " шт. за "+ pchar.quest.contraband.sum + " золотых. Идет?";
			Link.l1 = "По рукам!";
			Link.l1.go = "Make_trade_End";
			Link.l9 = "Сумма меня не устраивает! Сделка отменяется!";
			Link.l9.go = "Cancellation";
		break;

		case "NoGoods":
			Dialog.snd = "voice\SMSH\SMSH015";
			ChangeContrabandRelation(pchar, -10);
			dialog.Text = "Не повезло? Ты договариваешься о встрече, а потом просто говоришь 'Не повезло'? Проваливай отсюда. Не знаю даже, захотим ли мы иметь с тобой дело в будущем!";
			Link.l1 = "Посмотрим, захочу ли я иметь дело с вами!";
			Link.l1.go = "Exit_Cancel";				
		break; 
			
		case "Make_trade_End":
            RemoveCharacterGoods(Pchar, makeint(Pchar.quest.contraband.ChooseGoodsID), makeint(Pchar.quest.contraband.ChooseGoodsQty));
            // засунуть товар в магазин, иначе не работает затаривание 031104 boal -->
            Shit = GetCharacterCurrentStore(Pchar);
            if (Shit >= 0)
            {
                refStore = &Stores[Shit];
                Shit     = sti(Pchar.quest.contraband.ChooseGoodsID);
                SetStoreGoods(refStore, Shit, GetStoreGoodsQuantity(refStore, Shit)+ sti(Pchar.quest.contraband.ChooseGoodsQty));
            }
            // засунуть товар в магазин, иначе не работает затаривание 031104 boal <--
            Pchar.quest.Contraband.Counter = 1; // поторговали!
            AddMoneyToCharacter(Pchar, makeint(Pchar.quest.Contraband.sum));
			dialog.Text = "Деньги ваши, товар наш! Еще что-нибудь продаешь?";
			Link.l1 = "Да, давай посмотрим.";
			Link.l1.go = "Exchange";
			Link.l2 = "Нет. Спасибо, хватит.";
			Link.l2.go = "Finish_exit";
		break;
		
		case "Finish_exit":
            // таможня на суше
            if(GetSummonSkillFromNameToOld(Pchar, SKILL_SNEAK) < Rand(12))
			{
				AddDialogExitQuest("Rand_ContrabandInterruption");
				for (i=1; i<=3; i++)
				{
					characters[GetCharacterIndex("Rand_Smug0"+i)].ContrabandInterruption = true;
				}
			}
			AddCharacterExpToSkill(Pchar, "Sneak", 10 + 4 * makeint(sti(PChar.rank)/2));
			Diag.CurrentNode = Diag.TempNode;
			NPChar.quest.meeting = NPC_Meeting;
			DialogExit();
			ChangeContrabandRelation(pchar, 15);
            OfficersReaction("bad");
            ChangeCharacterReputation(pchar, -1);
            
            CloseQuestHeader("Gen_Contraband");
            
			DeleteAttribute(Pchar, "quest.Contraband.active");
			DeleteAttribute(Pchar, "quest.Contraband.counter");
			DeleteAttribute(Pchar, "quest.Contraband.price1");
			DeleteAttribute(Pchar, "quest.Contraband.price2");
			DeleteAttribute(Pchar, "quest.Contraband.price3");
			DeleteAttribute(Pchar, "quest.Contraband.price4");
			DeleteAttribute(Pchar, "quest.Contraband.price5");
			DeleteAttribute(Pchar, "quest.Contraband.goodsIDX1");
			DeleteAttribute(Pchar, "quest.Contraband.goodsIDX2");
			DeleteAttribute(Pchar, "quest.Contraband.goodsIDX3");
			DeleteAttribute(Pchar, "quest.Contraband.goodsIDX4");
			DeleteAttribute(Pchar, "quest.Contraband.goodsIDX5");
			DeleteAttribute(Pchar, "quest.Contraband.sum");
			DeleteAttribute(Pchar, "quest.Contraband.ChooseGoodsQty");
			DeleteAttribute(Pchar, "quest.Contraband.ChooseGoodsID");

			Pchar.quest.Rand_Smuggling.over = "yes";
			RemoveSmugglersFromShore();
		break;
	}
}