// диалог по городам
// Это мастер тюнинга
void ProcessCommonDialogEvent(ref NPChar, aref Link, aref NextDiag)
{
    if ( sti(Pchar.Ship.Type) != SHIP_NOTUSED)
    {
	    int   cannonQ       = sti(RealShips[sti(Pchar.Ship.Type)].CannonsQuantity);
		int   cannonMax     = sti(RealShips[sti(Pchar.Ship.Type)].MaxCaliber);
		int   shipClass     = sti(RealShips[sti(Pchar.Ship.Type)].Class);
		//int   shipPrice     = sti(RealShips[sti(Pchar.Ship.Type)].Price);
		int   shipCapacity  = sti(RealShips[sti(Pchar.Ship.Type)].Capacity);
		float shipSpeedRate = stf(RealShips[sti(Pchar.Ship.Type)].SpeedRate);
		int   shipMaxCrew   = sti(RealShips[sti(Pchar.Ship.Type)].MaxCrew);
		int   shipHP        = sti(RealShips[sti(Pchar.Ship.Type)].HP);
		float shipTurnRate  = stf(RealShips[sti(Pchar.Ship.Type)].TurnRate);
		
	    int  cannonMatherial1 = makeint(cannonQ * (cannonMax/shipClass));
	    int	 cannonMatherial2 = makeint(cannonQ * (30/shipClass));
	    int  cannonWorkPrice  = makeint(30*(10-shipClass)/(0.5*shipClass) * cannonQ * cannonMax);
	    //int  cannonWorkPrice  = makeint(shipPrice * cannonQ * cannonMax / 2000);
	    
	    int  сapacityMatherial1 = makeint(shipCapacity /(shipClass*2));
	    int	 сapacityMatherial2 = makeint(cannonQ *10 /shipClass);
	    int  сapacityWorkPrice  = makeint(shipCapacity/5 * 200);
	    
	    int  SpeedRateMatherial1 = makeint(shipSpeedRate*18 / shipClass);
	    int	 SpeedRateMatherial2 = makeint(shipSpeedRate*24 / shipClass);
	    int  SpeedRateWorkPrice  = makeint(shipSpeedRate/5 * 10000*6/shipClass);
	    
	    int  MaxCrewMatherial1 = makeint(shipMaxCrew*10 / shipClass);
	    int	 MaxCrewMatherial2 = makeint(shipMaxCrew*12 / shipClass);
	    int  MaxCrewWorkPrice  = makeint(shipMaxCrew/5 * 500*6/shipClass);
	    
	    int  TurnRateMatherial1 = makeint(shipTurnRate*8 / shipClass);
	    int	 TurnRateMatherial2 = makeint(shipTurnRate*10 / shipClass);
	    int  TurnRateWorkPrice  = makeint(shipTurnRate/5 * 3350*6/shipClass);
	    
	    int  HPMatherial1 = makeint(shipHP / (5*shipClass));
	    int	 HPMatherial2 = makeint(shipHP / (7*shipClass));
	    int  HPWorkPrice  = makeint(shipHP/5 * 70*6/shipClass);
	
	}
	int amount;
	ref shTo;
    string sTemp;
    string s1;
    
	switch (Dialog.CurrentNode)
	{
		
		// -----------------------------------Диалог первый - первая встреча
		case "First time":  // перехват основного диалога, только один раз
			if(NPChar.quest.meeting == "0")
			{
				Dialog.Text = "Кха... проклятая погода. Меня зовут " + GetFullName(npchar) + ", и, дьявол меня побери, я лучший корабел на этих чертовых островах.";
				Link.l1 = pcharrepphrase("Тысяча акул, только твоего нытья мне не хватало. Мое имя " + GetFullName(pchar) + ", думаю, ты обо мне слышал.",
	            "Позвольте с вами не согласиться - погода прекрасна. Меня зовут " + GetFullName(pchar) + ", думаю ты обо мне слышал.");
				Link.l1.go = "node_1";
				npchar.quest.meeting = "1";
			}
			else // нужно, если по квестам будет сброс на  "First time"
			{
				dialog.text = "А, это опять вы, опять со своими проблемами... кха... кха.";
				link.l1 = pcharrepphrase("Ты прав, это опять я, а ты, я смотрю, все так же недоволен жизнью, но давай поговорим о делах.",
	            "У вас хорошая память, а теперь, если позволите, мне хотелось бы перейти к более конкретным вопросам.");
				link.l1.go = "node_3";
				link.l2 = pcharrepphrase("Ты утомил меня своими жалобами на жизнь. Прощай.",
	            "Мне пора идти, извините, что побеспокоил.");
				link.l2.go = "exit";
			}
			NextDiag.TempNode = "Master_Ship";
			Dialog.CurrentNode = "Master_Ship";
		break;
		
		case "Master_Ship":
			dialog.text = "А, это опять вы, опять со своими проблемами... кха... кха.";
			link.l1 = pcharrepphrase("Ты прав, это опять я, а ты, я смотрю, все так же недоволен жизнью, но давай поговорим о делах.",
            "У вас хорошая память, а теперь, если позволите, мне хотелось бы перейти к более конкретным вопросам.");
			link.l1.go = "node_3";
			link.l2 = pcharrepphrase("Ты утомил меня своими жалобами на жизнь. Прощай.",
            "Мне пора идти, извините, что побеспокоил.");
			link.l2.go = "exit";
			NextDiag.TempNode = "Master_Ship";
		break;

		case "node_1":
			dialog.text = "Молодежь, молодежь, не дождешься от вас никакого почтения. Горячие головы, крепкие руки, что вам еще надо?";
			link.l1 = pcharrepphrase("Да, старик, похоже на то, что твоя молодость закончилась так давно, что ты этого уже и не помнишь.",
            "Извините, но вы же тоже были молодым, как вы можете говорить так?");
			link.l1.go = "node_2";
		break;

		case "node_2":
			dialog.text = "Не перечь старшим, щенок!";
			link.l1 = pcharrepphrase("Как ты меня назвал?! Еще хоть одно слово в таком духе услышу, и тебе не сдобровать. Займись-ка лучше делами, вместо пустого разговора.",
            "Прошу впредь не повышать на меня голос. А теперь, если вы закончили свой монолог, мне хотелось бы перейти к делам.");
			link.l1.go = "node_3";
			link.l2 = pcharrepphrase("Все, с меня хватит твоих разглагольствований! Прощай!",
            "Я вижу, что мы с вами не нашли общий язык. Прощайте.");
			link.l2.go = "exit";
		break;

		case "node_3":
			dialog.text = "Кха, кха... так что вам от меня нужно, " + GetAddress_Form(NPChar) + "?";
			link.l1 = pcharrepphrase("Мне надо воспользоваться услугами твоей верфи.","Я хочу воспользоваться услугами вашей верфи.");
			link.l1.go = "shipyard_dlg";
            Link.l2 = "У меня вопрос.";
			Link.l2.go = "quests"; //(перессылка в файл города)
			// -->
			if (CheckAttribute(pchar, "GenQuest.LoanChest.TakeChest") && sti(pchar.GenQuest.LoanChest.TargetIdx) == sti(NPChar.index))
			{
				link.l3 = "Я хотел бы поговорить с вами о делах финансовых.";
				link.l3.go = "LoanForAll";//(перессылка в файл города)
			}
			link.l9 = pcharrepphrase("Черт, у меня есть пара неотложных дел, требующих немедленного решения.", "Мне уже пора идти.");
			link.l9.go = "exit";
			NextDiag.TempNode = "Master_Ship";
		break;
		
		case "shipyard_dlg":
			dialog.Text = "Могу предложить работу со штучными заказами - доводку до ума лоханки, которые нонче плавают по морю!";
			Link.l1 = "Замечательно! Это то, что мне нужно. У меня как раз есть такой кораблик. Может взглянете?";
			Link.l1.go = "ship_tunning_start";
			link.l2 = "Нет, мне не нужно улучшений, я хочу просто воспользоваться услугами верфи.";
			link.l2.go = "shipyard";
			link.l3 = "Мне нужны орудия на корабль.";
			link.l3.go = "Cannons";
			Link.l9 = "О.. извините. Я уже ухожу.";
			Link.l9.go = "ship_tunning_not_now";
		break;
		
		case "ship_tunning_again":
			dialog.Text = RandPhraseSimple("А.. это снова ты... Решил кораблик свой достроить?",
                                           RandPhraseSimple("Приветствую известного мореплавателя. Что кораблик не устраивает?",
                                           "Эх.. ни минуты покоя нет, сосредоточится не могу.. Опять ты со своими улучшениями?"));
			Link.l1 = "Да. Есть такое дело. Может посмотрим, что можно сделать еще?";
			Link.l1.go = "ship_tunning_start";
			link.l2 = "Нет, мне не нужно улучшений, я хочу просто воспользоваться услугами верфи.";
			link.l2.go = "shipyard";
			link.l3 = "Мне нужны орудия на корабль.";
			link.l3.go = "Cannons";
            Link.l4 = "У меня вопрос.";
			Link.l4.go = "quests"; //(перессылка в файл города)
			// -->
			if (CheckAttribute(pchar, "GenQuest.LoanChest.TakeChest") && sti(pchar.GenQuest.LoanChest.TargetIdx) == sti(NPChar.index))
			{
				link.l5 = "Я хотел бы поговорить с вами о делах финансовых.";
				link.l5.go = "LoanForAll";//(перессылка в файл города)
			}
			Link.l9 = "Нет. Думаю меня устроит то, что есть";
			Link.l9.go = "ship_tunning_not_now_2";
		break;
		
        case "ship_tunning_not_now_2":
			dialog.Text = "Да ты, я погляжу, еще и думаешь! Далеко пойдешь! Приходи, когда решишься достроить кораблик.";
			Link.l1 = RandPhraseSimple("Спасибо! Всего хорошего.", "Пока, мастер. Можно и без сарказма.");
			Link.l1.go = "ship_tunning_not_now";
		break;
        		
        case "ship_tunning_start":
            if ( sti(Pchar.Ship.Type) == SHIP_NOTUSED || Pchar.location.from_sea != "Pirates_town")
            {
                dialog.Text = "Корабль-то где? Что вы мне голову тут все морочите!";
			    Link.l1 = "И то правда.. что это я... Извиняюсь";
			    Link.l1.go = "ship_tunning_not_now";
            }
            else
            {
			    if (GetHour() == 13)
			    {
                    dialog.Text = "Я, конечно, трудоголик, но и у меня есть обеденое время. Приходи позже!";
			        Link.l1 = "Понятно...";
			        Link.l1.go = "ship_tunning_not_now";
			    }
			    else
			    {
                    if (shipClass == 7)
                    {
                        dialog.Text = "Э.. Лодками я не занимаюсь. Не мой это размах!";
    			        Link.l1 = "Понятно...";
    			        Link.l1.go = "ship_tunning_not_now";
    			        break;
                    }

                    s1 = "Так так.. посмотрим, что тут у нас есть... Ага " + XI_ConvertString(RealShips[sti(Pchar.Ship.Type)].BaseName) + ".";
    			    if (sti(RealShips[sti(Pchar.Ship.Type)].Stolen) == true && !CheckAttribute(&RealShips[sti(Pchar.Ship.Type)], "Tuning"))
    			    {
    			        s1 = s1 + " Ба! Да корабль-то побывал не в одних руках. То-то он такой потертый. Патент корсарский у тебя есть? Ладно шучу... к делу.";
    			    }
    			    s1 = s1 + " Что бы ты хотел с ним сделать?";
                    dialog.Text = s1;
                    if (cannonMax < 32 && !CheckAttribute(&RealShips[sti(Pchar.Ship.Type)], "Tuning.Cannon"))
                    {
                        Link.l1 = "Увеличить калибр орудий";
    			        Link.l1.go = "ship_tunning_cannon";
    			    }
    			    if (!CheckAttribute(&RealShips[sti(Pchar.Ship.Type)], "Tuning.Capacity"))
                    {
    			        Link.l2 = "Увеличить трюм";
    			        Link.l2.go = "ship_tunning_Capacity";
    			    }
    			    if (!CheckAttribute(&RealShips[sti(Pchar.Ship.Type)], "Tuning.SpeedRate"))
                    {
    			        Link.l3 = "Увеличить скорость";
    			        Link.l3.go = "ship_tunning_SpeedRate";
    			    }
    			    if (!CheckAttribute(&RealShips[sti(Pchar.Ship.Type)], "Tuning.TurnRate"))
                    {
    			        Link.l4 = "Увеличить маневренность";
    			        Link.l4.go = "ship_tunning_TurnRate";
    			    }
    			    if (!CheckAttribute(&RealShips[sti(Pchar.Ship.Type)], "Tuning.MaxCrew"))
                    {
    			        Link.l5 = "Прибавить место для экипажа";
    			        Link.l5.go = "ship_tunning_MaxCrew";
    			    }
    			    if (!CheckAttribute(&RealShips[sti(Pchar.Ship.Type)], "Tuning.HP"))
                    {
    			        Link.l6 = "Увеличить прочность корпуса";
    			        Link.l6.go = "ship_tunning_HP";
    			    }
    			    Link.l10 = "Постойте! Я передумал.";
    			    Link.l10.go = "ship_tunning_not_now_2";
			    }
			}
		break;
		
		case "ship_tunning_cannon":
			s1 = "Давай прикинем, что нужно сделать. Пушек у тебя " + cannonQ + ", калибр максимальный " + cannonMax + "ф. ";
			
			s1 = s1 + " Мне понадобится "+ cannonMatherial1 + " черного дерева для усиления портиков и "+ cannonMatherial2+" кожи для системы отката больших орудий.";
			s1 = s1 + " За работу я попрошу " + cannonWorkPrice + " золотых. Половину вперед.";
            dialog.Text = s1;
			Link.l1 = "Годится. Я принимаю условия. Материалы я привезу";
			Link.l1.go = "ship_tunning_cannon_start";
			Link.l2 = "Нет. Меня это не устраивает.";
			Link.l2.go = "ship_tunning_not_now";
		break;
		
		case "ship_tunning_cannon_start":
		    amount = cannonWorkPrice;
		    if(makeint(Pchar.money) >= makeint(amount / 2 +0.5))
			{
				AddMoneyToCharacter(Pchar, -makeint(amount / 2 +0.5));
				NPChar.Tuning.Cannon = true;
			    NPChar.Tuning.Money  = makeint(amount - makeint(amount / 2 +0.5));
			    NPChar.Tuning.Matherial1 = cannonMatherial1; // черное дерево
			    NPChar.Tuning.Matherial2 = cannonMatherial2; // кожа
			    NPChar.Tuning.ShipType       = Pchar.Ship.Type;
			    NPChar.Tuning.ShipName       = RealShips[sti(Pchar.Ship.Type)].BaseName;
			    
				NextDiag.TempNode = "ship_tunning_cannon_again";
                dialog.text = "Вот и славно. Жду материал для работы";
			    link.l1 = "Побежал за ним...";
			    link.l1.go = "Exit";
			    
			    ReOpenQuestHeader("ShipTuning");
			    AddQuestRecord("ShipTuning", "t1");
				AddQuestUserData("ShipTuning", "sText", "Для увеличения калибра орудий на корабле " + XI_ConvertString(RealShips[sti(Pchar.Ship.Type)].BaseName) +
                " необходимо: "+NPChar.Tuning.Matherial1+ " шт. черного дерева, "+ NPChar.Tuning.Matherial2+" шт. кожи и " + NPChar.Tuning.Money + " золотых.");
			}
			else
			{
				NextDiag.TempNode = "ship_tunning_not_now";
                dialog.text = "Не вижу задатка...";
				link.l1 = "Я позже зайду";
				link.l1.go = "Exit";								
			}
		break;
		
		case "ship_tunning_cannon_again":
		    if (sti(NPChar.Tuning.ShipType) == sti(Pchar.Ship.Type) && NPChar.Tuning.ShipName      == RealShips[sti(Pchar.Ship.Type)].BaseName)
		    {
                NextDiag.TempNode = "ship_tunning_cannon_again";
			    dialog.Text = "Пушки твои ждут. Принес " + sti(NPChar.Tuning.Matherial1) + " черного дерева, " + sti(NPChar.Tuning.Matherial2) + " кожи и " + sti(NPChar.Tuning.Money) + " пиастров?";
			    Link.l1 = "Да. Кое-что удалось доставить";
			    Link.l1.go = "ship_tunning_cannon_again_2";
			    Link.l2 = "Нет. Еще добываю";
			    Link.l2.go = "Exit";
			}
			else
			{
			    DeleteAttribute(NPChar, "Tuning");
                NextDiag.TempNode = "ship_tunning_again";
			    dialog.Text = "Сдается мне, сударь, что Вы поменяли корабль со времени нашего уговора. Придеться все заново расчитывать..";
			    Link.l1 = "Было дело. Обидно, что задаток пропал...";
			    Link.l1.go = "Exit";
			    AddQuestRecord("ShipTuning", "Lose");
			    CloseQuestHeader("ShipTuning");
			}
		break;
		
		case "ship_tunning_cannon_again_2":
		    amount = makeint(Pchar.money) - sti(NPChar.Tuning.Money);
		    if (amount < 0)
		    {
		       amount = amount + sti(NPChar.Tuning.Money);
		    }
		    else
		    {
		        amount = sti(NPChar.Tuning.Money);
		    }
		    AddMoneyToCharacter(Pchar, -amount);
		    NPChar.Tuning.Money = makeint(sti(NPChar.Tuning.Money) - amount);
		    
		    amount = GetSquadronGoods(Pchar, GOOD_EBONY) - sti(NPChar.Tuning.Matherial1);
		    if (amount < 0)
		    {
		       amount = amount + sti(NPChar.Tuning.Matherial1);
		    }
		    else
		    {
		        amount = sti(NPChar.Tuning.Matherial1);
		    }
		    RemoveCharacterGoods(Pchar, GOOD_EBONY, amount);
		    NPChar.Tuning.Matherial1 = sti(NPChar.Tuning.Matherial1) - amount;
		    
		    amount = GetSquadronGoods(Pchar, GOOD_LEATHER) - sti(NPChar.Tuning.Matherial2);
		    if (amount < 0)
		    {
		       amount = amount + sti(NPChar.Tuning.Matherial2);
		    }
		    else
		    {
		        amount = sti(NPChar.Tuning.Matherial2);
		    }
		    RemoveCharacterGoods(Pchar, GOOD_LEATHER, amount);
		    NPChar.Tuning.Matherial2 = sti(NPChar.Tuning.Matherial2) - amount;
		    
		    if(sti(NPChar.Tuning.Matherial2) < 1 && sti(NPChar.Tuning.Matherial1) < 1 && sti(NPChar.Tuning.Money) < 1)
			{
				DeleteAttribute(NPChar, "Tuning");
                NextDiag.TempNode = "ship_tunning_again";
                dialog.text = "Все привез. Молодец! Начинаю работу...";
			    link.l1 = "Жду";
			    link.l1.go = "ship_tunning_cannon_complite";
			}
			else
			{
				NextDiag.TempNode = "ship_tunning_cannon_again";
                dialog.Text = "Тебе осталось " + sti(NPChar.Tuning.Matherial1) + " черного дерева, " + sti(NPChar.Tuning.Matherial2) + " кожи и " + sti(NPChar.Tuning.Money) + " пиастров";
				link.l1 = "Хорошо.";
				link.l1.go = "Exit";

                AddQuestRecord("ShipTuning", "t1");
				AddQuestUserData("ShipTuning", "sText", "Мне осталось довезти еще " +  sti(NPChar.Tuning.Matherial1)+ " шт. черного дерева, "+ sti(NPChar.Tuning.Matherial2)+" шт. кожи и "+ sti(NPChar.Tuning.Money) + " золотых.");
			}
		break;
		
		case "ship_tunning_cannon_complite":
		    AddTimeToCurrent(6, 30);
		    shTo = &RealShips[sti(Pchar.Ship.Type)];
		    DeleteAttribute(NPChar, "Tuning");
		    // изменим
	    int iCaliber = sti(shTo.MaxCaliber);
        switch(iCaliber)
	    {
		case 12:
			iCaliber = 0;
		break;
		case 16:
			iCaliber = 1;
		break;
		case 24:
			iCaliber = 2;
		break;
		case 32:
			iCaliber = 3;
		break;
	    }
	    iCaliber = iCaliber + 1;
	    if (iCaliber > 3) iCaliber = 3;
	    switch(iCaliber)
	    {
		case 0:
			iCaliber = 12;
		break;
		case 1:
			iCaliber = 16;
		break;
		case 2:
			iCaliber = 24;
		break;
		case 3:
			iCaliber = 32;
		break;
	    }
	    shTo.MaxCaliber = iCaliber;
	    shTo.Tuning.Cannon = true;
	    // finish <--
            NextDiag.TempNode = "ship_tunning_again";
			dialog.Text = "... Вроде бы все... Можешь идти на верфь и менять калибр на побольше";
			Link.l1 = "Спасибо! Проверю обязательно";
			Link.l1.go = "Exit";
			
			AddQuestRecord("ShipTuning", "End");
			CloseQuestHeader("ShipTuning");
		break;
		////////////////////////////////////////// трюм ////////////////////////////////////////////////////
		case "ship_tunning_Capacity":
			s1 = "Давай прикинем, что нужно сделать. Трюм у тебя " + shipCapacity;
			
			s1 = s1 + " Мне понадобится "+ сapacityMatherial1 + " красного дерева для уменьшения объема переборок и "+ сapacityMatherial2+" масла для пропитки швов.";
			s1 = s1 + " За работу я попрошу " + сapacityWorkPrice + " золотых. Половину вперед.";
            dialog.Text = s1;
			Link.l1 = "Годится. Я принимаю условия. Материалы я привезу";
			Link.l1.go = "ship_tunning_Capacity_start";
			Link.l2 = "Нет. Меня это не устраивает.";
			Link.l2.go = "ship_tunning_not_now";
		break;
		
		case "ship_tunning_Capacity_start":
		    amount = сapacityWorkPrice;
		    if(makeint(Pchar.money) >= makeint(amount / 2 +0.5))
			{
				AddMoneyToCharacter(Pchar, -makeint(amount / 2 +0.5));
			    NPChar.Tuning.Money  = makeint(amount - makeint(amount / 2 +0.5));
			    NPChar.Tuning.Matherial1 = сapacityMatherial1; // красное дерево GOOD_MAHOGANY
			    NPChar.Tuning.Matherial2 = сapacityMatherial2; //  GOOD_OIL
			    NPChar.Tuning.ShipType       = Pchar.Ship.Type;
			    NPChar.Tuning.ShipName       = RealShips[sti(Pchar.Ship.Type)].BaseName;
			
				NextDiag.TempNode = "ship_tunning_Capacity_again";
                dialog.text = "Вот и славно. Жду материал для работы";
			    link.l1 = "Побежал за ним...";
			    link.l1.go = "Exit";
			    
			    ReOpenQuestHeader("ShipTuning");
			    AddQuestRecord("ShipTuning", "t1");
				AddQuestUserData("ShipTuning", "sText", "Для увеличения трюма на корабле " + XI_ConvertString(RealShips[sti(Pchar.Ship.Type)].BaseName) +
                " необходимо: "+NPChar.Tuning.Matherial1+ " шт. красного дерева, "+ NPChar.Tuning.Matherial2+" шт. масла и " + NPChar.Tuning.Money + " золотых.");
			}
			else
			{
				NextDiag.TempNode = "ship_tunning_not_now";
                dialog.text = "Не вижу задатка...";
				link.l1 = "Я позже зайду";
				link.l1.go = "Exit";								
			}
		break;
		
		case "ship_tunning_Capacity_again":
		    if (sti(NPChar.Tuning.ShipType) == sti(Pchar.Ship.Type) && NPChar.Tuning.ShipName      == RealShips[sti(Pchar.Ship.Type)].BaseName)
		    {
                NextDiag.TempNode = "ship_tunning_Capacity_again";
			    dialog.Text = "Трюм ждет. Принес " + sti(NPChar.Tuning.Matherial1) + " красного дерева, " + sti(NPChar.Tuning.Matherial2) + " масла и " + sti(NPChar.Tuning.Money) + " пиастров?";
			    Link.l1 = "Да. Кое-что удалось доставить";
			    Link.l1.go = "ship_tunning_Capacity_again_2";
			    Link.l2 = "Нет. Еще добываю";
			    Link.l2.go = "Exit";
			}
			else
			{
			    DeleteAttribute(NPChar, "Tuning");
                NextDiag.TempNode = "ship_tunning_again";
			    dialog.Text = "Сдается мне, сударь, что Вы поменяли корабль со времени нашего уговора. Придеться все заново расчитывать..";
			    Link.l1 = "Было дело. Обидно, что задаток пропал...";
			    Link.l1.go = "Exit";
			    
			    AddQuestRecord("ShipTuning", "Lose");
			    CloseQuestHeader("ShipTuning");
			}
		break;
		
		case "ship_tunning_Capacity_again_2":
		    checkMatherial(Pchar, NPChar, GOOD_MAHOGANY, GOOD_OIL);
		
		    if(sti(NPChar.Tuning.Matherial2) < 1 && sti(NPChar.Tuning.Matherial1) < 1 && sti(NPChar.Tuning.Money) < 1)
			{
				DeleteAttribute(NPChar, "Tuning");
                NextDiag.TempNode = "ship_tunning_again";
                dialog.text = "Все привез. Молодец! Начинаю работу...";
			    link.l1 = "Жду";
			    link.l1.go = "ship_tunning_Capacity_complite";
			}
			else
			{
				NextDiag.TempNode = "ship_tunning_Capacity_again";
                dialog.Text = "Тебе осталось " + sti(NPChar.Tuning.Matherial1) + " красного дерева, " + sti(NPChar.Tuning.Matherial2) + " масла и " + sti(NPChar.Tuning.Money) + " пиастров";
				link.l1 = "Хорошо.";
				link.l1.go = "Exit";

                AddQuestRecord("ShipTuning", "t1");
				AddQuestUserData("ShipTuning", "sText", "Мне осталось довезти еще " +  sti(NPChar.Tuning.Matherial1)+ " шт. красного дерева, "+ sti(NPChar.Tuning.Matherial2)+" шт. масла и "+ sti(NPChar.Tuning.Money) + " золотых.");
			}
		break;
		
		case "ship_tunning_Capacity_complite":
		    AddTimeToCurrent(6, 30);
		    shTo = &RealShips[sti(Pchar.Ship.Type)];
		    DeleteAttribute(NPChar, "Tuning");
		    // изменим
	        shTo.Capacity        = sti(shTo.Capacity) + makeint(sti(shTo.Capacity)/5);
	        shTo.Tuning.Capacity = true;
	        // finish <--
            NextDiag.TempNode = "ship_tunning_again";
			dialog.Text = "... Вроде бы все... Можешь идти к купцам и грузиться по полной";
			Link.l1 = "Спасибо! Проверю обязательно";
			Link.l1.go = "Exit";
			
			AddQuestRecord("ShipTuning", "End");
			CloseQuestHeader("ShipTuning");
		break;
		
		////////////////////////////////////////// SpeedRate ////////////////////////////////////////////////////
		case "ship_tunning_SpeedRate":
			s1 = "Давай прикинем, что нужно сделать. Скорость у тебя " + shipSpeedRate;
			
			s1 = s1 + " Мне понадобится "+ SpeedRateMatherial1 + " парусины и "+ SpeedRateMatherial2+" шелка";
			s1 = s1 + " За работу я попрошу " + SpeedRateWorkPrice + " золотых. Половину вперед.";
            dialog.Text = s1;
			Link.l1 = "Годится. Я принимаю условия. Материалы я привезу";
			Link.l1.go = "ship_tunning_SpeedRate_start";
			Link.l2 = "Нет. Меня это не устраивает.";
			Link.l2.go = "ship_tunning_not_now";
		break;
		
		case "ship_tunning_SpeedRate_start":
		    amount = SpeedRateWorkPrice;
		    if(makeint(Pchar.money) >= makeint(amount / 2 +0.5))
			{
				AddMoneyToCharacter(Pchar, -makeint(amount / 2 +0.5));
			    NPChar.Tuning.Money  = makeint(amount - makeint(amount / 2 +0.5));
			    NPChar.Tuning.Matherial1 = SpeedRateMatherial1; // GOOD_SAILCLOTH
			    NPChar.Tuning.Matherial2 = SpeedRateMatherial2; //  GOOD_SILK
			    NPChar.Tuning.ShipType       = Pchar.Ship.Type;
			    NPChar.Tuning.ShipName       = RealShips[sti(Pchar.Ship.Type)].BaseName;
			
				NextDiag.TempNode = "ship_tunning_SpeedRate_again";
                dialog.text = "Вот и славно. Жду материал для работы";
			    link.l1 = "Побежал за ним...";
			    link.l1.go = "Exit";
			    
			    ReOpenQuestHeader("ShipTuning");
			    AddQuestRecord("ShipTuning", "t1");
				AddQuestUserData("ShipTuning", "sText", "Для увеличения скорости на корабле " + XI_ConvertString(RealShips[sti(Pchar.Ship.Type)].BaseName) +
                " необходимо: "+NPChar.Tuning.Matherial1+ " шт. парусины, "+ NPChar.Tuning.Matherial2+" шт. шелка и " + NPChar.Tuning.Money + " золотых.");
			}
			else
			{
				NextDiag.TempNode = "ship_tunning_not_now";
                dialog.text = "Не вижу задатка...";
				link.l1 = "Я позже зайду";
				link.l1.go = "Exit";								
			}
		break;
		
		case "ship_tunning_SpeedRate_again":
		    if (sti(NPChar.Tuning.ShipType) == sti(Pchar.Ship.Type) && NPChar.Tuning.ShipName      == RealShips[sti(Pchar.Ship.Type)].BaseName)
		    {
                NextDiag.TempNode = "ship_tunning_SpeedRate_again";
			    dialog.Text = "Работа ждет. Принес " + sti(NPChar.Tuning.Matherial1) + " парусины, " + sti(NPChar.Tuning.Matherial2) + " шелка и " + sti(NPChar.Tuning.Money) + " пиастров?";
			    Link.l1 = "Да. Кое-что удалось доставить";
			    Link.l1.go = "ship_tunning_SpeedRate_again_2";
			    Link.l2 = "Нет. Еще добываю";
			    Link.l2.go = "Exit";
			}
			else
			{
			    DeleteAttribute(NPChar, "Tuning");
                NextDiag.TempNode = "ship_tunning_again";
			    dialog.Text = "Сдается мне, сударь, что Вы поменяли корабль со времени нашего уговора. Придеться все заново расчитывать..";
			    Link.l1 = "Было дело. Обидно, что задаток пропал...";
			    Link.l1.go = "Exit";
			    
			    AddQuestRecord("ShipTuning", "Lose");
			    CloseQuestHeader("ShipTuning");
			}
		break;
		
		case "ship_tunning_SpeedRate_again_2":
		    checkMatherial(Pchar, NPChar, GOOD_SAILCLOTH, GOOD_SILK);
		    
		    if(sti(NPChar.Tuning.Matherial2) < 1 && sti(NPChar.Tuning.Matherial1) < 1 && sti(NPChar.Tuning.Money) < 1)
			{
				DeleteAttribute(NPChar, "Tuning");
                NextDiag.TempNode = "ship_tunning_again";
                dialog.text = "Все привез. Молодец! Начинаю работу...";
			    link.l1 = "Жду";
			    link.l1.go = "ship_tunning_SpeedRate_complite";
			}
			else
			{
				NextDiag.TempNode = "ship_tunning_SpeedRate_again";
                dialog.Text = "Тебе осталось " + sti(NPChar.Tuning.Matherial1) + " парусины, " + sti(NPChar.Tuning.Matherial2) + " шелка и " + sti(NPChar.Tuning.Money) + " пиастров";
				link.l1 = "Хорошо.";
				link.l1.go = "Exit";

                AddQuestRecord("ShipTuning", "t1");
				AddQuestUserData("ShipTuning", "sText",  "Мне осталось довезти еще " +  sti(NPChar.Tuning.Matherial1)+ " шт. парусины, "+ sti(NPChar.Tuning.Matherial2)+" шт. шелка и "+ sti(NPChar.Tuning.Money) + " золотых.");
			}
		break;
		
		case "ship_tunning_SpeedRate_complite":
		    AddTimeToCurrent(6, 30);
		    shTo = &RealShips[sti(Pchar.Ship.Type)];
		    DeleteAttribute(NPChar, "Tuning");
		    // изменим
	        shTo.SpeedRate        = (stf(shTo.SpeedRate) + stf(shTo.SpeedRate)/5.0);
	        shTo.Tuning.SpeedRate = true;
	        // finish <--
            NextDiag.TempNode = "ship_tunning_again";
			dialog.Text = "... Вроде бы все... Можешь ловить ветер полными парусами";
			Link.l1 = "Спасибо! Проверю обязательно";
			Link.l1.go = "Exit";
			
			AddQuestRecord("ShipTuning", "End");
			CloseQuestHeader("ShipTuning");
		break;
		////////////////////////////////////////// MaxCrew ////////////////////////////////////////////////////
		case "ship_tunning_MaxCrew":
			s1 = "Давай прикинем, что нужно сделать. Максимальный экипаж твой с учетом перегруза " + shipMaxCrew + " матросов";
			
			s1 = s1 + " Мне понадобится "+ MaxCrewMatherial1 + " фруктов и "+ MaxCrewMatherial2+" одежды";
			s1 = s1 + " За работу я попрошу " + MaxCrewWorkPrice + " золотых. Половину вперед.";
            dialog.Text = s1;
			Link.l1 = "Годится. Я принимаю условия. Материалы я привезу";
			Link.l1.go = "ship_tunning_MaxCrew_start";
			Link.l2 = "Нет. Меня это не устраивает.";
			Link.l2.go = "ship_tunning_not_now";
		break;
		
		case "ship_tunning_MaxCrew_start":
		    amount = MaxCrewWorkPrice;
		    if(makeint(Pchar.money) >= makeint(amount / 2 +0.5))
			{
				AddMoneyToCharacter(Pchar, -makeint(amount / 2 +0.5));
			    NPChar.Tuning.Money  = makeint(amount - makeint(amount / 2 +0.5));
			    NPChar.Tuning.Matherial1 = MaxCrewMatherial1; // GOOD_FRUITS
			    NPChar.Tuning.Matherial2 = MaxCrewMatherial2; //  GOOD_CLOTHES
			    NPChar.Tuning.ShipType       = Pchar.Ship.Type;
			    NPChar.Tuning.ShipName       = RealShips[sti(Pchar.Ship.Type)].BaseName;
			
				NextDiag.TempNode = "ship_tunning_MaxCrew_again";
                dialog.text = "Вот и славно. Жду материал для работы";
			    link.l1 = "Побежал за ним...";
			    link.l1.go = "Exit";
			    
			    ReOpenQuestHeader("ShipTuning");
			    AddQuestRecord("ShipTuning", "t1");
				AddQuestUserData("ShipTuning", "sText",  "Для увеличения экипажа на корабле " + XI_ConvertString(RealShips[sti(Pchar.Ship.Type)].BaseName) +
                " необходимо: "+NPChar.Tuning.Matherial1+ " шт. фруктов, "+ NPChar.Tuning.Matherial2+" шт. одежды и " + NPChar.Tuning.Money + " золотых.");
			}
			else
			{
				NextDiag.TempNode = "ship_tunning_not_now";
                dialog.text = "Не вижу задатка...";
				link.l1 = "Я позже зайду";
				link.l1.go = "Exit";								
			}
		break;
		
		case "ship_tunning_MaxCrew_again":
		    if (sti(NPChar.Tuning.ShipType) == sti(Pchar.Ship.Type) && NPChar.Tuning.ShipName      == RealShips[sti(Pchar.Ship.Type)].BaseName)
		    {
                NextDiag.TempNode = "ship_tunning_MaxCrew_again";
			    dialog.Text = "Работа ждет. Принес " + sti(NPChar.Tuning.Matherial1) + " фруктов, " + sti(NPChar.Tuning.Matherial2) + " одежды и " + sti(NPChar.Tuning.Money) + " пиастров?";
			    Link.l1 = "Да. Кое-что удалось доставить";
			    Link.l1.go = "ship_tunning_MaxCrew_again_2";
			    Link.l2 = "Нет. Еще добываю";
			    Link.l2.go = "Exit";
			}
			else
			{
			    DeleteAttribute(NPChar, "Tuning");
                NextDiag.TempNode = "ship_tunning_again";
			    dialog.Text = "Сдается мне, сударь, что Вы поменяли корабль со времени нашего уговора. Придеться все заново расчитывать..";
			    Link.l1 = "Было дело. Обидно, что задаток пропал...";
			    Link.l1.go = "Exit";
			    
			    AddQuestRecord("ShipTuning", "Lose");
			    CloseQuestHeader("ShipTuning");
			}
		break;
		
		case "ship_tunning_MaxCrew_again_2":
		    checkMatherial(Pchar, NPChar, GOOD_FRUITS, GOOD_CLOTHES);
		    
		    if(sti(NPChar.Tuning.Matherial2) < 1 && sti(NPChar.Tuning.Matherial1) < 1 && sti(NPChar.Tuning.Money) < 1)
			{
				DeleteAttribute(NPChar, "Tuning");
                NextDiag.TempNode = "ship_tunning_again";
                dialog.text = "Все привез. Молодец! Начинаю работу...";
			    link.l1 = "Жду";
			    link.l1.go = "ship_tunning_MaxCrew_complite";
			}
			else
			{
				NextDiag.TempNode = "ship_tunning_MaxCrew_again";
                dialog.Text = "Тебе осталось " + sti(NPChar.Tuning.Matherial1) + " фруктов, " + sti(NPChar.Tuning.Matherial2) + " одежды и " + sti(NPChar.Tuning.Money) + " пиастров";
				link.l1 = "Хорошо.";
				link.l1.go = "Exit";

                AddQuestRecord("ShipTuning", "t1");
				AddQuestUserData("ShipTuning", "sText",  "Мне осталось довезти еще " +  sti(NPChar.Tuning.Matherial1)+ " шт. фруктов, "+ sti(NPChar.Tuning.Matherial2)+" шт. одежды и "+ sti(NPChar.Tuning.Money) + " золотых.");
			}
		break;
		
		case "ship_tunning_MaxCrew_complite":
		    AddTimeToCurrent(6, 30);
		    shTo = &RealShips[sti(Pchar.Ship.Type)];
		    DeleteAttribute(NPChar, "Tuning");
		    // изменим
	        shTo.MaxCrew        = sti(shTo.MaxCrew) + makeint(sti(shTo.MaxCrew)/5);
	        shTo.Tuning.MaxCrew = true;
	        // finish <--
            NextDiag.TempNode = "ship_tunning_again";
			dialog.Text = "... Вроде бы все... Набирай новый экипаж";
			Link.l1 = "Спасибо! Проверю обязательно";
			Link.l1.go = "Exit";
			
			AddQuestRecord("ShipTuning", "End");
			CloseQuestHeader("ShipTuning");
		break;
		
		////////////////////////////////////////// TurnRate ////////////////////////////////////////////////////
		case "ship_tunning_TurnRate":
			s1 = "Давай прикинем, что нужно сделать. Маневренность " + shipTurnRate;
			
			s1 = s1 + " Мне понадобится "+ TurnRateMatherial1 + " досок и "+ TurnRateMatherial2+" полотна";
			s1 = s1 + " За работу я попрошу " + TurnRateWorkPrice + " золотых. Половину вперед.";
            dialog.Text = s1;
			Link.l1 = "Годится. Я принимаю условия. Материалы я привезу";
			Link.l1.go = "ship_tunning_TurnRate_start";
			Link.l2 = "Нет. Меня это не устраивает.";
			Link.l2.go = "ship_tunning_not_now";
		break;
		
		case "ship_tunning_TurnRate_start":
		    amount = TurnRateWorkPrice;
		    if(makeint(Pchar.money) >= makeint(amount / 2 +0.5))
			{
				AddMoneyToCharacter(Pchar, -makeint(amount / 2 +0.5));
			    NPChar.Tuning.Money  = makeint(amount - makeint(amount / 2 +0.5));
			    NPChar.Tuning.Matherial1 = TurnRateMatherial1; // GOOD_PLANKS
			    NPChar.Tuning.Matherial2 = TurnRateMatherial2; //  GOOD_LINEN
			    NPChar.Tuning.ShipType       = Pchar.Ship.Type;
			    NPChar.Tuning.ShipName       = RealShips[sti(Pchar.Ship.Type)].BaseName;
			
				NextDiag.TempNode = "ship_tunning_TurnRate_again";
                dialog.text = "Вот и славно. Жду материал для работы";
			    link.l1 = "Побежал за ним...";
			    link.l1.go = "Exit";
			    
			    ReOpenQuestHeader("ShipTuning");
			    AddQuestRecord("ShipTuning", "t1");
				AddQuestUserData("ShipTuning", "sText",  "Для увеличения маневренности на корабле " + XI_ConvertString(RealShips[sti(Pchar.Ship.Type)].BaseName) +
                " необходимо: "+NPChar.Tuning.Matherial1+ " шт. досок, "+ NPChar.Tuning.Matherial2+" шт. полотна и " + NPChar.Tuning.Money + " золотых.");
			}
			else
			{
				NextDiag.TempNode = "ship_tunning_not_now";
                dialog.text = "Не вижу задатка...";
				link.l1 = "Я позже зайду";
				link.l1.go = "Exit";								
			}
		break;
		
		case "ship_tunning_TurnRate_again":
		    if (sti(NPChar.Tuning.ShipType) == sti(Pchar.Ship.Type) && NPChar.Tuning.ShipName      == RealShips[sti(Pchar.Ship.Type)].BaseName)
		    {
                NextDiag.TempNode = "ship_tunning_TurnRate_again";
			    dialog.Text = "Работа ждет. Принес " + sti(NPChar.Tuning.Matherial1) + " досок, " + sti(NPChar.Tuning.Matherial2) + " полотна и " + sti(NPChar.Tuning.Money) + " пиастров?";
			    Link.l1 = "Да. Кое-что удалось доставить";
			    Link.l1.go = "ship_tunning_TurnRate_again_2";
			    Link.l2 = "Нет. Еще добываю";
			    Link.l2.go = "Exit";
			}
			else
			{
			    DeleteAttribute(NPChar, "Tuning");
                NextDiag.TempNode = "ship_tunning_again";
			    dialog.Text = "Сдается мне, сударь, что Вы поменяли корабль со времени нашего уговора. Придеться все заново расчитывать..";
			    Link.l1 = "Было дело. Обидно, что задаток пропал...";
			    Link.l1.go = "Exit";
			    
			    AddQuestRecord("ShipTuning", "Lose");
			    CloseQuestHeader("ShipTuning");
			}
		break;
		
		case "ship_tunning_TurnRate_again_2":
		    checkMatherial(Pchar, NPChar, GOOD_PLANKS, GOOD_LINEN);
		
		    if(sti(NPChar.Tuning.Matherial2) < 1 && sti(NPChar.Tuning.Matherial1) < 1 && sti(NPChar.Tuning.Money) < 1)
			{
				DeleteAttribute(NPChar, "Tuning");
                NextDiag.TempNode = "ship_tunning_again";
                dialog.text = "Все привез. Молодец! Начинаю работу...";
			    link.l1 = "Жду";
			    link.l1.go = "ship_tunning_TurnRate_complite";
			}
			else
			{
				NextDiag.TempNode = "ship_tunning_TurnRate_again";
                dialog.Text = "Тебе осталось " + sti(NPChar.Tuning.Matherial1) + " досок, " + sti(NPChar.Tuning.Matherial2) + " полотна и " + sti(NPChar.Tuning.Money) + " пиастров";
				link.l1 = "Хорошо.";
				link.l1.go = "Exit";

                AddQuestRecord("ShipTuning", "t1");
				AddQuestUserData("ShipTuning", "sText",  "Мне осталось довезти еще " +  sti(NPChar.Tuning.Matherial1)+ " шт. досок, "+ sti(NPChar.Tuning.Matherial2)+" шт. полотна и "+ sti(NPChar.Tuning.Money) + " золотых.");
			}
		break;
		
		case "ship_tunning_TurnRate_complite":
		    AddTimeToCurrent(6, 30);
		    shTo = &RealShips[sti(Pchar.Ship.Type)];
		    DeleteAttribute(NPChar, "Tuning");
		    // изменим
	        shTo.TurnRate        = (stf(shTo.TurnRate) + stf(shTo.TurnRate)/5.0);
	        shTo.Tuning.TurnRate = true;
	        // finish <--
            NextDiag.TempNode = "ship_tunning_again";
			dialog.Text = "... Вроде бы все... Можешь крутить штурвал";
			Link.l1 = "Спасибо! Проверю обязательно";
			Link.l1.go = "Exit";
			
			AddQuestRecord("ShipTuning", "End");
			CloseQuestHeader("ShipTuning");
		break;
		////////////////////////////////////////// HP ////////////////////////////////////////////////////
		case "ship_tunning_HP":
			s1 = "Давай прикинем, что нужно сделать. Корпус сейчас " + shipHP;
			
			s1 = s1 + " Мне понадобится "+ HPMatherial1 + " черного дерева и "+ HPMatherial2+" сандала";
			s1 = s1 + " За работу я попрошу " + HPWorkPrice + " золотых. Половину вперед.";
            dialog.Text = s1;
			Link.l1 = "Годится. Я принимаю условия. Материалы я привезу";
			Link.l1.go = "ship_tunning_HP_start";
			Link.l2 = "Нет. Меня это не устраивает.";
			Link.l2.go = "ship_tunning_not_now";
		break;
		
		case "ship_tunning_HP_start":
		    amount = HPWorkPrice;
		    if(makeint(Pchar.money) >= makeint(amount / 2 +0.5))
			{
				AddMoneyToCharacter(Pchar, -makeint(amount / 2 +0.5));
			    NPChar.Tuning.Money  = makeint(amount - makeint(amount / 2 +0.5));
			    NPChar.Tuning.Matherial1 = HPMatherial1; // GOOD_EBONY
			    NPChar.Tuning.Matherial2 = HPMatherial2; //  GOOD_SANDAL
			    NPChar.Tuning.ShipType       = Pchar.Ship.Type;
			    NPChar.Tuning.ShipName       = RealShips[sti(Pchar.Ship.Type)].BaseName;
			
				NextDiag.TempNode = "ship_tunning_HP_again";
                dialog.text = "Вот и славно. Жду материал для работы";
			    link.l1 = "Побежал за ним...";
			    link.l1.go = "Exit";
			    
			    ReOpenQuestHeader("ShipTuning");
			    AddQuestRecord("ShipTuning", "t1");
				AddQuestUserData("ShipTuning", "sText",  "Для увеличения прочности корпуса на корабле " + XI_ConvertString(RealShips[sti(Pchar.Ship.Type)].BaseName) +
                " необходимо: "+NPChar.Tuning.Matherial1+ " шт. черного дерева, "+ NPChar.Tuning.Matherial2+" шт. сандала и " + NPChar.Tuning.Money + " золотых.");
			}
			else
			{
				NextDiag.TempNode = "ship_tunning_not_now";
                dialog.text = "Не вижу задатка...";
				link.l1 = "Я позже зайду";
				link.l1.go = "Exit";								
			}
		break;
		
		case "ship_tunning_HP_again":
		    if (sti(NPChar.Tuning.ShipType) == sti(Pchar.Ship.Type) && NPChar.Tuning.ShipName      == RealShips[sti(Pchar.Ship.Type)].BaseName)
		    {
                NextDiag.TempNode = "ship_tunning_HP_again";
			    dialog.Text = "Работа ждет. Принес " + sti(NPChar.Tuning.Matherial1) + " черного дерева, " + sti(NPChar.Tuning.Matherial2) + " сандала и " + sti(NPChar.Tuning.Money) + " пиастров?";
			    Link.l1 = "Да. Кое-что удалось доставить";
			    Link.l1.go = "ship_tunning_HP_again_2";
			    Link.l2 = "Нет. Еще добываю";
			    Link.l2.go = "Exit";
			}
			else
			{
			    DeleteAttribute(NPChar, "Tuning");
                NextDiag.TempNode = "ship_tunning_again";
			    dialog.Text = "Сдается мне, сударь, что Вы поменяли корабль со времени нашего уговора. Придеться все заново расчитывать..";
			    Link.l1 = "Было дело. Обидно, что задаток пропал...";
			    Link.l1.go = "Exit";
			    
			    AddQuestRecord("ShipTuning", "Lose");
			    CloseQuestHeader("ShipTuning");
			}
		break;
		
		case "ship_tunning_HP_again_2":
		    checkMatherial(Pchar, NPChar, GOOD_EBONY, GOOD_SANDAL);
		
		    if(sti(NPChar.Tuning.Matherial2) < 1 && sti(NPChar.Tuning.Matherial1) < 1 && sti(NPChar.Tuning.Money) < 1)
			{
				DeleteAttribute(NPChar, "Tuning");
                NextDiag.TempNode = "ship_tunning_again";
                dialog.text = "Все привез. Молодец! Начинаю работу...";
			    link.l1 = "Жду";
			    link.l1.go = "ship_tunning_HP_complite";
			}
			else
			{
				NextDiag.TempNode = "ship_tunning_HP_again";
                dialog.Text = "Тебе осталось " + sti(NPChar.Tuning.Matherial1) + " черного дерева, " + sti(NPChar.Tuning.Matherial2) + " сандала и " + sti(NPChar.Tuning.Money) + " пиастров";
				link.l1 = "Хорошо.";
				link.l1.go = "Exit";

                AddQuestRecord("ShipTuning", "t1");
				AddQuestUserData("ShipTuning", "sText",  "Мне осталось довезти еще " +  sti(NPChar.Tuning.Matherial1)+ " шт. черного дерева, "+ sti(NPChar.Tuning.Matherial2)+" шт. сандала и "+ sti(NPChar.Tuning.Money) + " золотых.");
			}
		break;
		
		case "ship_tunning_HP_complite":
		    AddTimeToCurrent(6, 30);
		    shTo = &RealShips[sti(Pchar.Ship.Type)];
		    DeleteAttribute(NPChar, "Tuning");
		    // изменим
	        shTo.HP        = sti(shTo.HP) + makeint(sti(shTo.HP)/5);
	        shTo.Tuning.HP = true;
	        // finish <--
            NextDiag.TempNode = "ship_tunning_again";
			dialog.Text = "... Вроде бы все... Пару лишних взрывов бомб выдержит точно";
			Link.l1 = "Спасибо! Проверю обязательно";
			Link.l1.go = "Exit";
			
			AddQuestRecord("ShipTuning", "End");
			CloseQuestHeader("ShipTuning");
		break;
		///////////////////////////////  квестовые ветки
		case "quests":
			dialog.text = NPCStringReactionRepeat(RandPhraseSimple("Какие вопросы?", "Что вам угодно?"), "Совсем недавно вы пытались задать мне вопрос...", "У себя на верфи, да и вообще в городе, я таких однообразно любознательных не видел.",
                          "Ну что за вопросы? Мое дело - корабли строить, давайте этим и займемся.", "block", 1, npchar, Dialog.CurrentNode);
			link.l1 = HeroStringReactionRepeat(RandPhraseSimple("Я передумал...", "Сейчас мне не о чем говорить"), "Хм, что-то с памятью моей стало...",
                      "Хм, однако...", "Давайте...", npchar, Dialog.CurrentNode);
			link.l1.go = "exit";
			if (!CheckAttribute(npchar, "Step_Door"))
			{
				link.l2 = "Послушай, я хочу пройти, а дверь закрыта...";
				link.l2.go = "Step_Door_1";
			}
        // ==> Перехват на квесты
			if (pchar.questTemp.BlueBird == "toSeekBermudes") //щебека Синяя Птица
			{
				link.l1 = "Послушайте, мне нужно знать одну вещь касательно вашего подземелья...";
				link.l1.go = "BlueBird_1";
			}
            if (CheckAttribute(pchar, "questTemp.State.SeekBible"))// квест №2, поиск Евангелие
            {
                if (!CheckCharacterItem(pchar, "Bible"))
                {
                    dialog.text = "Говорите...";
        			link.l2 = "Послушайте, я хочу узнать, куда на вашей верфи провалился некий человек, спасающийся от преследования?";
        			link.l2.go = "Step_S2_1";
                }
                else
                {
                    dialog.text = "О-о-о, рад видеть тебя. Значит, не зря я тебя туда впустил - сам выжил и меня от это дряни избавил... Ну скажи, кто там был?";
        			link.l2 = "Лучше тебе этого не знать, спать будешь спокойней. Ну, прощай, еще раз спасибо, что открыл дверь.";
        			link.l2.go = "exit";
                }
            }
			if (pchar.questTemp.piratesLine == "KillLoy_toAlexus")
			{
				link.l1 = "К вам Эдвард Лоу заходил?";
				link.l1.go = "PL_Q3_1";
			}
			if (pchar.questTemp.piratesLine == "KillLoy_GoodWork")
			{
				link.l1 = "Джекмен подарил мне 'Морского Волка'!";
				link.l1.go = "PL_Q3_SW1";
			}
			if (pchar.questTemp.piratesLine == "KillLoy_LoyIsDied")
			{
				link.l1 = "У меня не очень хорошие новости относительно твоего брига, который Лоу увел...";
				link.l1.go = "PL_Q3_SW2";
			}
        // <== Перехват на квесты
		break;
//*************************** Пиратка, квест №3, поиски Лоу ***************************
		case "PL_Q3_1":
			dialog.text = "Заходил... А вам он зачем нужен?";
			link.l1 = "Этот... Этот нехороший человек обманул меня и предал Моргана. Я ищу его, чтоб убить негодяя!";
			link.l1.go = "PL_Q3_3";
			link.l2 = "Я его друг, давно не виделись, а тут в таверне сказали, что он заходил сюда...";
			link.l2.go = "PL_Q3_2";
		break;
		case "PL_Q3_2":
			dialog.text = "Ага. Друг, значит. Это хорошо... Это просто замечательно! Ты, вот что, иди к Джекмену, скажи, что ищешь своего друга Эдварда Лоу. Джекмен тебе поможет\nМожет даже, твой друг еще у него. Иди, иди, там и встретитесь, соскучился, поди.";
			link.l1 = "Спасибо! Уже бегу!";
			link.l1.go = "exit";
			AddQuestRecord("Pir_Line_3_KillLoy", "10");
			pchar.questTemp.piratesLine = "KillLoy_toJackman";
		break;
		case "PL_Q3_3":
			dialog.text = "Да он, оказывается, трижды подлец! Он, ведь, и меня обманул! Принес письмо от Джекмена, с просьбой отдать 'Морского Волка' подателю сего письма...";
			link.l1 = "'Морского Волка'?";
			link.l1.go = "PL_Q3_4";
		break;
		case "PL_Q3_4":
			dialog.text = "'Морской Волк' это бриг. Мой бриг! Самое лучшее мое творение! Всю душу в него вложил. Он прекрасен, великолепен, пленителен! На закате от него невозможно оторвать взгляд, на восходе, когда красавец поднимает паруса, он подобен распускающемуся бутону. А как он бежит по волнам...";
			link.l1 = "Гх-м-м...  Вы говорили что-то про Лоу.";
			link.l1.go = "PL_Q3_5";
		break;
		case "PL_Q3_5":
			dialog.text = "Простите? А, да. Эдвард Лоу. Бриг-то я Джекмену делал, достроил почти, а тут он, Лоу, с письмом. В общем, отдал я бриг, а когда пошел за деньгами к Джекмену, оказалось, что письмо поддельное. Джекмен расстроился... Сильно... Очень сильно\nКогда меня оттуда вы... Э-э-э...  Когда я оттуда уходил, Джекмен сказал: 'Я вас уважаю Мастер, но бриг надо вернуть'. Вот я тут и подумал, раз уж вы все равно этого Лоу ищете, может и мой бриг, заодно найдете?";
			link.l1 = "Я не против вам помочь, только где мне теперь этого Лоу искать?!";
			link.l1.go = "PL_Q3_6";
		break;
		case "PL_Q3_6":
			dialog.text = "Так спросите хозяина таверны. Тавернщики, они народ такой, все про всех знают. Наверняка знает, куда негодяй уплыл.";
			link.l1 = "Что ж, попробую.";
			link.l1.go = "exit";
			AddQuestRecord("Pir_Line_3_KillLoy", "11");
			pchar.questTemp.piratesLine = "KillLoy_toTavernAgain";
			//садим капитана Гудли в таверну
			sld = GetCharacter(NPC_GenerateCharacter("CapGoodly", "officer_9", "man", "man", 20, PIRATE, -1, true));
			sld.name = "Капитан";
			sld.lastname = "Гудли";
			sld.rank = 20;
			sld.city = "Pirates";
			sld.dialog.filename   = "Quest\PiratesLine_dialog.c";
			sld.dialog.currentnode   = "CapGoodly";
			sld.greeting = "pirat_quest";
			LAi_SetSitType(sld);
			LAi_group_MoveCharacter(sld, "PIRATE_CITIZENS");
            FreeSitLocator("Pirates_tavern", "sit2");
         	ChangeCharacterAddressGroup(sld, "Pirates_tavern", "sit", "sit2");
		break;

		case "PL_Q3_SW1":
			dialog.text = "Видел, видел, твое прибытие. Не удержался, бегал на пристань поглядеть.\nДостойный подарок. Поздравляю и спасибо тебе! Спас старика.";
			link.l1 = "Так уж и спас...";
			link.l1.go = "exit";
		break;
		case "PL_Q3_SW2":
			dialog.text = "Черт! Я так и знал... И что?";
			link.l1 = "Он утонул, сожалею....";
			link.l1.go = "PL_Q3_SW3";
		break;
		case "PL_Q3_SW3":
			dialog.text = "Да уж, все плохо.";
			link.l1 = "Да не переживай ты, все утрясется...";
			link.l1.go = "exit";
		break;
//*************************** щебека Синяя Птица ***************************
		case "BlueBird_1":
			dialog.text = "Оно не мое. Наоборот, я пытаюсь туда никого не пускать ради их же блага.";
			link.l1 = "А-а, ну понятно. Вопрос собственно заключается вот в чем. Не проносят ли через это подземелье товары в город?";
			link.l1.go = "BlueBird_2";
		break;
		case "BlueBird_2":
			dialog.text = "Ха-ха, нет конечно, хотя подземелье и сквозное. Посудите сами, это каким идиотом надо быть, чтобы на себе таскать грузы по этой норе, когда можно нанять любую тартану и спокойно привезти все по морю.";
			link.l1 = "Ну а если кто-то не хочет, чтобы об этом узнали в городе?";
			link.l1.go = "BlueBird_3";
		break;
		case "BlueBird_3":
			dialog.text = "А вы думаете, что если кто-то начнет таскать грузы из моей верфи в город, этого никто не заметит?";
			link.l1 = "Хм, да, точно... Так значит, никто ничего не проносит через вашу верфь?";
			link.l1.go = "BlueBird_4";
		break;
		case "BlueBird_4":
			dialog.text = "Нет, никто ничего не проносит, можете быть уверенным.";
			link.l1 = "Ну что же, огромное вам спасибо!";
			link.l1.go = "exit";
			AddQuestRecord("Xebeca_BlueBird", "5");
			pchar.questTemp.BlueBird = "toCavern";
			pchar.quest.BlueBird_inCavern.win_condition.l1 = "locator";
			pchar.quest.BlueBird_inCavern.win_condition.l1.location = "Bermudes_Cavern";
			pchar.quest.BlueBird_inCavern.win_condition.l1.locator_group = "quest";
			pchar.quest.BlueBird_inCavern.win_condition.l1.locator = "BlueBird";
			pchar.quest.BlueBird_inCavern.function = "BlueBird_inCavern";
			LAi_LocationDisableMonstersGen("Bermudes_Cavern", true); //монстров не генерить
			LAi_LocationDisableOfficersGen("Bermudes_Cavern", true); //офицеров не пускать
		break;	
//*************************** Открывание двери ***************************
 		case "Step_Door_1":
			dialog.text = "Все верно. Чтобы здесь пройти, тебе надо заплатить мне 20000 монет. Не заплатишь - не открою, у меня здесь не проходной двор. Да и тебе там шляться не советую - не к добру...";
			if(sti(pchar.money) >= 20000)
			{
    			link.l1 = "Вот твои деньги, открывай.";
    			link.l1.go = "Step_Door_2";
            }
            else
            {
    			link.l1 = "Нет у меня таких денег.";
    			link.l1.go = "Step_Door_3";
            }
            link.l2 = "Вот еще! Платить за какую-то дверь.";
    		link.l2.go = "exit";
		break;
 		case "Step_Door_2":
			npchar.Step_Door = true; // fix
			dialog.text = "Проходи, дверь открыта. И не благодари!";
			link.l1 = "Да чего уж там, спасибо тебе, " + npchar.name + "...";
			link.l1.go = "exit";
			LocatorReloadEnterDisable("Pirates_Shipyard", "reload2", false);
            AddMoneyToCharacter(pchar, -20000);
			pchar.quest.CloseBermudesDungeonDoor.win_condition.l1 = "MapEnter";
            pchar.quest.CloseBermudesDungeonDoor.win_condition = "CloseBermudesDungeonDoor";
		break;
 		case "Step_Door_3":
			dialog.text = "Ну вот когда будут, тогда и приходи. А так и не уговаривай - бесполезно...";
			link.l1 = "Аргх, ну слов просто никаких нет!";
			link.l1.go = "exit";
		break;
//*************************** Квест №2, диалоги с Франциско на выходе из Инквизиии ***************************
 		case "Step_S2_1":
			dialog.text = NPCStringReactionRepeat("У-у-у, хороший вопрос... А зачем он вам нужен?", "Я уже все сказал - дверь открыта.", "Еще раз повоторяю - дверь открыта.", "Послушай, ну сколько можно, а?", "block", 0, npchar, Dialog.CurrentNode);
			link.l1 = HeroStringReactionRepeat("У него есть книга, которая ему не принадлежит. Он вор.", "Я понял...", "Понятно...", "Вот такой я непонятно настойчивый...", npchar, Dialog.CurrentNode);
			link.l1.go = DialogGoNodeRepeat("Step_S2_2", "none", "none", "none", npchar, Dialog.CurrentNode);
		break;
 		case "Step_S2_2":
			dialog.text = "Понятно. Вы знаете, его преследователи, матросы, уже спрашивали меня об этом. Потом приходил какой-то торговец и тоже пытался разузнать. Я им ничего не сказал.";
			link.l1 = "Почему?";
			link.l1.go = "Step_S2_3";
		break;
 		case "Step_S2_3":
			dialog.text = "Не захотел греха на душу брать - не жить им, если бы они туда полезли... Тебе могу сказать, ты, вроде, не из боязливых, да и крепок...";
			link.l1 = "Куда бы они полезли?";
			link.l1.go = "Step_S2_4";
		break;
 		case "Step_S2_4":
			dialog.text = "Туда, куда полез тот воришка. В эту дверь, что справа от меня.";
			link.l1 = "И там, за дверью? И почему ты никого туда не пускаешь?";
			link.l1.go = "Step_S2_5";
		break;
 		case "Step_S2_5":
			dialog.text = "За дверью подземелье, кто его построил - мне неведомо, когда я стал здесь хозяином, оно уже было. А не пускаю потому, что опасно там, очень опасно. Воришка тот, похоже, там и сгинул...\n"+
                          "Живет там не знаю кто, или что. Но порой такие оттуда вопли раздаются, что кровь в жилах стынет...";
			link.l1 = "Вот так даже... Хм, ну что же, запускай.";
			link.l1.go = "Step_S2_6";
		break;
 		case "Step_S2_6":
			dialog.text = "Проходи, открыто... Надесь, что увижу тебя еще на этом свете.";
			link.l1 = "Увидишь, не сомневайся.";
			link.l1.go = "exit";
            LocatorReloadEnterDisable("Pirates_Shipyard", "reload2", false);
            LocatorReloadEnterDisable("Shore3", "reload2_back", false);
            Pchar.quest.Inquisition_fightInDangeon.win_condition.l1 = "location";
            Pchar.quest.Inquisition_fightInDangeon.win_condition.l1.location = "Bermudes_Dungeon";
            Pchar.quest.Inquisition_fightInDangeon.win_condition = "Inquisition_fightInDangeon";
         	pchar.GenQuestBox.Terks_Grot = true;
            pchar.GenQuestBox.Bermudes_Dungeon.box1.items.jewelry5 = 200;
            pchar.GenQuestBox.Bermudes_Dungeon.box1.items.blade33 = 1;
            pchar.GenQuestBox.Bermudes_Dungeon.box1.items.blade28 = 1;
            pchar.GenQuestBox.Bermudes_Dungeon.box1.items.blade34 = 1;
            pchar.GenQuestBox.Bermudes_Dungeon.box1.items.blade32 = 1;
            pchar.GenQuestBox.Bermudes_Dungeon.box1.items.arm1 = 1;
            pchar.GenQuestBox.Bermudes_Dungeon.box1.items.pistol5 = 1;
            pchar.GenQuestBox.Bermudes_Dungeon.box1.items.pistol4 = 1;
            pchar.GenQuestBox.Bermudes_Dungeon.box1.items.pistol6 = 1;
            pchar.GenQuestBox.Bermudes_Dungeon.box1.items.potion2 = 5;
            pchar.GenQuestBox.Bermudes_Dungeon.box1.items.indian18 = 1;
            pchar.GenQuestBox.Bermudes_Dungeon.box1.items.jewelry7 = 5;
            pchar.GenQuestBox.Bermudes_Dungeon.box1.items.lead1 = 1;
            pchar.GenQuestBox.Bermudes_Dungeon.box1.items.potion2 = 10;
            pchar.GenQuestBox.Bermudes_Dungeon.box1.items.potionwine = 3;
            pchar.GenQuestBox.Bermudes_Dungeon.box1.items.jewelry2 = 20;
            pchar.GenQuestBox.Bermudes_Dungeon.box1.items.jewelry14= 10;
            pchar.GenQuestBox.Bermudes_Dungeon.box1.items.indian6 = 1;
            pchar.GenQuestBox.Bermudes_Dungeon.box1.items.indian11 = 1;
            pchar.GenQuestBox.Bermudes_Dungeon.box1.items.indian15 = 1;
            pchar.GenQuestBox.Bermudes_Dungeon.box1.items.indian12 = 1;
            pchar.GenQuestBox.Bermudes_Dungeon.box1.items.indian18 = 1;
            pchar.GenQuestBox.Bermudes_Dungeon.box1.items.Bible = 1;
		break;

	}
	UnloadSegment(NPChar.FileDialog2);  // если где-то выход внутри switch  по return не забыть сделать анлод
}

void checkMatherial(ref Pchar, ref NPChar, int good1, int good2)
{
    int amount;
    amount = makeint(Pchar.money) - sti(NPChar.Tuning.Money);
    if (amount < 0)
    {
       amount = amount + sti(NPChar.Tuning.Money);
    }
    else
    {
        amount = sti(NPChar.Tuning.Money);
    }
    AddMoneyToCharacter(Pchar, -amount);
    NPChar.Tuning.Money = makeint(sti(NPChar.Tuning.Money) - amount);

    amount = GetSquadronGoods(Pchar, good1) - sti(NPChar.Tuning.Matherial1);
    if (amount < 0)
    {
       amount = amount + sti(NPChar.Tuning.Matherial1);
    }
    else
    {
        amount = sti(NPChar.Tuning.Matherial1);
    }
    RemoveCharacterGoods(Pchar, good1, amount);
    NPChar.Tuning.Matherial1 = sti(NPChar.Tuning.Matherial1) - amount;

    amount = GetSquadronGoods(Pchar, good2) - sti(NPChar.Tuning.Matherial2);
    if (amount < 0)
    {
       amount = amount + sti(NPChar.Tuning.Matherial2);
    }
    else
    {
        amount = sti(NPChar.Tuning.Matherial2);
    }
    RemoveCharacterGoods(Pchar, good2, amount);
    NPChar.Tuning.Matherial2 = sti(NPChar.Tuning.Matherial2) - amount;
}