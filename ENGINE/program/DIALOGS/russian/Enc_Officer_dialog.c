// BOAL новый диалог офицера и компаньона 21/06/06
void ProcessDialogEvent()
{
	ref NPChar, d;
	aref Link, Diag;

	DeleteAttribute(&Dialog,"Links");

	makeref(NPChar,CharacterRef);
	makearef(Link, Dialog.Links);
	makeref(d, Dialog);
	makearef(Diag, NPChar.Dialog);
	
	ref    sld;
	string attr;
	int    iTemp, iTax, iFortValue;
	
	switch(Dialog.CurrentNode)
	{
        case "First time":
			if (LAi_grp_playeralarm > 0)
			{
       			dialog.text = RandPhraseSimple("Хех, капитан, за тобой тут половина гарнизона гоняется. Сейчас явно не время идти к тебе в команду!", "Предлагаешь с боем прорываться на корабль? Нет уж, в другой раз, кэп...");
				link.l1 = RandPhraseSimple("Ну как знаешь...", "Да я и не собирался тебя нанимать."); 
				link.l1.go = "exit";
				break;
			}
			Diag.TempNode = "OnceAgain";
			dialog.text = "Офицер не нужен, капитан?";
			if (FindFreeRandomOfficer() > 0)
			{
    			Link.l1 = "Офицер? А на что ты годен?";
    			Link.l1.go = "Node_2";
			}
			Link.l2 = "Нет, у меня полный комплект.";
			Link.l2.go = "Exit";			
		break;
		
		case "hired":
			Diag.TempNode = "Hired";
			// только наняли -->
			if (IsCompanion(NPChar))
			{
				Dialog.text = "Сегодня мы снова выйдем в море!";
				link.l2 = "Да... Я тоже рад.";
				link.l2.go = "Exit";
				break;
			}
			if (Npchar.location.group == "sit")
			{
				dialog.text = "Сейчас я допью, капитан, и отправлюсь на борт. Не извольте беспокоиться - я успею до отплытия.";
				Link.l1 = "Хорошо. Опоздаешь - заставлю драить палубу!";
				Link.l1.go = "Exit";
				break;
			}
			// только наняли <--
			dialog.text = "Что вы хотите, капитан?";
			if (isOfficer(Npchar)) // проверка для кампуса, где слотовые офы
			{
	            Link.l2 = "Слушай мой приказ!";
	            Link.l2.go = "stay_follow";
            }
			Link.l4 = "Ты уволен. Я обойдусь без твоих услуг!";
			Link.l4.go = "AsYouWish";

            // boal отчет о корабле
            /*if (CheckNPCharQuestAttribute(Npchar, "officertype", XI_ConvertString("treasurer")))
            {
                Link.l8 = "Дай мне полный отчет, " + XI_ConvertString("treasurer") + ".";
                Link.l8.go = "QMASTER_1";
            } */
            // по тек локации определим можно ли тут приказать  -->
            if (IsEntity(loadedLocation))
            {
                if (CheckAttribute(loadedLocation, "fastreload"))
                {
                    iTemp = FindColony(loadedLocation.fastreload);
                    if (iTemp != -1)
                    {
                        sld = GetColonyByIndex(iTemp);
                        if (sti(sld.HeroOwn) == true && !CheckAttribute(sld, "OfficerIdx"))
                        {
                            NPChar.ColonyIdx = iTemp;
							Link.l7 = "Я назначаю тебя наместником этого города!";
                            Link.l7.go = "Gover_Hire";
                        }
                    }
                }
            }
            Link.l9 = "Ничего. Вольно.";
            Link.l9.go = "Exit";
        break;
        
		case "ShowParam_exit":
			Diag.CurrentNode = "OnceAgain";
			NPChar.quest.meeting = true;
			DialogExit();
			
			PChar.SystemInfo.OnlyShowCharacter = true;
            LaunchCharacter(NPChar);
		break;
		
		case "exit":
			Diag.CurrentNode = Diag.TempNode;
			NPChar.quest.meeting = true;
			DialogExit();			
		break;

		case "exit_hire":
			Pchar.questTemp.HiringOfficerIDX = GetCharacterIndex(Npchar.id);
			AddDialogExitQuestFunction("LandEnc_OfficerHired");
			Diag.CurrentNode = Diag.TempNode;
			NPChar.quest.meeting = true;
			DialogExit();
		break;

		case "exit_fire":
			//navy -->
			if (CheckAttribute(NPChar, "PGGAi"))
			{
				pchar.questTemp.FiringOfficerIDX = NPChar.index;
				AddDialogExitQuestFunction("PGG_FireOfficer");
				DialogExit();			
				break;
			}
			//navy <--
			Diag.TempNode = "Fired";
			Pchar.questTemp.FiringOfficerIDX = GetCharacterIndex(Npchar.id);
			AddDialogExitQuestFunction("LandEnc_OfficerFired");

			Diag.CurrentNode = Diag.TempNode;
			NPChar.quest.meeting = true;

			DialogExit();			
		break;
		
		// boal 29.05.04 офицер хочет свалить -->
        case "WantToGo":
			chrDisableReloadToLocation = false;
			Diag.TempNode = "Hired";
			dialog.text = LinkRandPhrase("Капитан, все люди как люди: влюбляются, женятся, дома сидят, одни мы, словно заведенные, по морю шныряем. Хочу другой жизни вкусить немного. Отпусти, будь человеком!",
                           "Капитан, по морям шататься с вами, конечно, не плохо, но надо и о себе когда-то подумать. Деток завести, домишко купить. А на это нужно время. Вы уж отпустите меня. Хочу пожить, как нормальный человек.",
                           "Капитан, извини, но что-то устал я все время по морям шастать. Земля тянет неимоверно. Хочу загулять на некоторое время. Ты уж без меня обойдись как-нибудь. Без обид?");
			Link.l1 = LinkRandPhrase("Этак я всю свою команду порастеряю. Все разбегутся. Ну, держать тебя в неволе не стану - гуляй.",
			                         "Корабль не темница, а я не тюремщик. Иди, верши свою историю.",
			                         "Не любите вы меня. Вижу, вижу. Ладно, гуляй, но помни: я в обиде.");
			Link.l1.go = "WantToGo_free";
			Link.l2 = LinkRandPhrase("Нет, друг, извини. Не отпущу я тебя. Никак невозможно.",
			                         "Хочешь не хочешь, но я тебя купил и нанял. Ты помнишь об этом? Так что о личной жизни забудь.",
			                         "Никаких гулянок на стороне. Впереди жаркие бои и у меня каждый офицер на счету.");
			Link.l2.go = "WantToGo_Stay";
		break;
		
		case "WantToGo_Stay":
			dialog.text = "Я все же не раб. И работаю исключительно за деньги. Хотя вы и были мне симпатичны. Я вправе решать, когда мне свалить!";
			Link.l1 = "На счет денег ты прав. Как на счет прибавки?";
			Link.l1.go = "WantToGo_Stay_2";
			Link.l2 = LinkRandPhrase("Этак я всю свою команду порастеряю. Все разбегутся. Ну, держать тебя в неволе не стану - гуляй.",
			                         "Корабль не темница, а я не тюремщик. Иди, верши свою историю.",
			                         "Не любите вы меня. Вижу, вижу. Ладно, гуляй, но помни: я в обиде.");
			Link.l2.go = "WantToGo_free";
		break;
		
		case "WantToGo_Stay_2":
			dialog.text = "Хорошо, "+sti(NPChar.rank)*500+" золотых прямо сейчас на руки меня вполне устроят.";
            if (sti(Pchar.money) >= sti(NPChar.rank)*500)
            {
        		Link.l1 = "Договорились.";
        		Link.l1.go = "WantToGo_Stay_ForMoney";
			}
			Link.l2 = "Вот еще! Платить? Да я за такие деньги двух офицеров найму.";
        	Link.l2.go = "WantToGo_free";

		break;
		case "WantToGo_Stay_ForMoney":
            Diag.TempNode = "Hired";
            NPChar.greeting           = "Gr_Officer";
            AddMoneyToCharacter(Pchar, -(makeint(sti(NPChar.rank)*500)));
            Diag.CurrentNode = Diag.TempNode;
			NPChar.quest.meeting = true;
			DialogExit();
			NPChar.location = "none";
			LAi_SetOfficerType(NPChar);
		break;
		
		case "WantToGo_free":
            Diag.TempNode = "WantToGo_free_Yet";
			//navy -->
			if (CheckAttribute(NPChar, "PGGAi"))
			{
				pchar.questTemp.FiringOfficerIDX = NPChar.index;
				AddDialogExitQuestFunction("PGG_FireOfficer");
				Diag.CurrentNode = Diag.TempNode;
				DialogExit();			
				break;
			}
			//navy <--
		    Pchar.questTemp.FiringOfficerIDX = NPChar.index;
			AddDialogExitQuestFunction("LandEnc_OfficerFired");
			
			Diag.CurrentNode = Diag.TempNode;
			NPChar.quest.meeting = true;
			DialogExit();
		break;
		
		case "WantToGo_free_Yet":
            Diag.TempNode = "WantToGo_free_Yet";
			dialog.text = "Эх.. вот я теперь заживу свободной жизнью...";
			Link.l1 = "Ну-ну, всяческих тебе успехов.";
			Link.l1.go = "exit";
		break;
		// boal 29.05.04 офицер хочет свалить <--
		
		// пассажир возмущен игроком и сваливает -->
		case "WantToRemove":
            chrDisableReloadToLocation = false;
			if (Npchar.alignment == "good")
            {
			    dialog.text = "Капитан, мое терпение лопнуло! У нас с вами совершенно разные понятия о 'хорошем' в этой жизни. Я ухожу от вас.";
			}
			else
			{
                dialog.text = "Капитан, мне думалось, что я служу настоящему корсару. А мы? Чем мы занимаемся? Я не хочу терять время и ухожу.";
			}
			Link.l1 = "Что?!! Ну и убирайся ко всем чертям!";
			Link.l1.go = "WantToGo_free";
		break;
		// пассажир возмущен игроком и сваливает <--
		
		case "Node_2":
			dialog.text = NPCharRepPhrase(Npchar, RandSwear() + "Да я лучший "+ XI_ConvertString(Npchar.quest.officertype) + " в этих водах. " + Npchar.quest.officertype_2 + "Готов оказать вам услугу, пойдя на службу.",
                                    "Ну, говорят, что я неплохой " + XI_ConvertString(Npchar.quest.officertype) + ". " + Npchar.quest.officertype_2 + "Возьмите меня на свое судно, капитан - убедитесь в этом сами.");
			Link.l1 = "Сколько ты хочешь?";
			Link.l1.go = "price";
			Link.l2 = "А что ты из себя представляешь как специалист?";
			Link.l2.go = "ShowParam_exit";
			Link.l3 = "Увы, у меня уже есть " + XI_ConvertString(Npchar.quest.officertype) + ".";
			Link.l3.go = "Exit";
		break;

		case "OnceAgain":
			if (LAi_grp_playeralarm > 0)
			{
       			dialog.text = RandPhraseSimple("Хех, капитан, за тобой тут половина гарнизона гоняется. Сейчас явно не время идти к тебе в команду!", "Предлагаешь с боем прорываться на корабль? Нет уж, в другой раз, кэп...");
				link.l1 = RandPhraseSimple("Ну как знаешь...", "Да я и не собирался тебя нанимать."); 
				link.l1.go = "exit";
				break;
			}
			Diag.TempNode = "OnceAgain";
			dialog.text = "Неужели передумали, капитан? Решили, что новый " + XI_ConvertString(Npchar.quest.officertype) + " вам не помешает?";
			// boal -->
			if (FindFreeRandomOfficer() > 0)
			{
                Link.l1 = "Пожалуй, так. Сколько ты хочешь?";
                Link.l1.go = "price";
			}
			Link.l2 = "Каков ты будешь в деле?";
			Link.l2.go = "ShowParam_exit";
			// boal <--
			Link.l3 = "У меня полный комплект. Счастливо оставаться.";
			Link.l3.go = "exit";
		break;

		case "price":
			dialog.text = "Думаю, мы с вами сойдемся на " + Npchar.quest.OfficerPrice + " пиастров.";
			Link.l1 = "Не слишком ли ты много хочешь?";
			Link.l1.go = "trade";
			Link.l2 = "Я согласен. Считай себя зачисленным в команду.";
			Link.l2.go = "hire";
			Link.l3 = "Ты не стоишь этих денег. Прощай.";
			Link.l3.go = "exit";
		break;

		case "trade":
			if (GetSummonSkillFromNameToOld(pchar, SKILL_COMMERCE) >= Rand(12) && (sti(Npchar.quest.OfficerPrice) - GetCharacterSkillToOld(Pchar, "commerce")*100) >= sti(Npchar.quest.OfficerLowPrice))// bug fix
			{
				Npchar.quest.OfficerPrice = makeint(makeint(Npchar.quest.OfficerPrice) - GetCharacterSkillToOld(Pchar, "commerce")*100); // пусть будет коммерция перса голой
				dialog.text = "Хорошо...Пожалуй, я готов согласиться даже на " + Npchar.quest.OfficerPrice + " пиастров. Ну как?";
				Link.l1 = "Вот это уже лучше. Ты принят.";
				Link.l1.go = "hire";				
				Link.l2 = "Я по-прежнему считаю, что это чересчур. Прощай.";
				Link.l2.go = "exit";
			}
			else
			{
				dialog.text = "Увы, капитан, я стою именно столько, сколько запросил. Если это слишком дорого для вас - поищите кого-нибудь другого.";
				Link.l1 = "Ну ладно, я согласен. Я беру тебя.";
				Link.l1.go = "hire";				
				Link.l2 = "Так я и поступлю. Всего хорошего.";
				Link.l2.go = "exit";
			}
		break;

		case "hire":
			if(makeint(Pchar.money) >= makeint(Npchar.quest.OfficerPrice))
			{
				AddMoneyToCharacter(Pchar, -(makeint(Npchar.quest.OfficerPrice)));
				Diag.TempNode = "OnboardSoon";
				dialog.text = "Благодарю вас, капитан. Вы не пожалеете, что отдали мне это золото.";
				Link.l1 = "Хочется в это верить.";
				Link.l1.go = "Exit_hire";								
			}
			else
			{
				dialog.text = "Эй, похоже, у вас проблемы с наличностью! Извините, капитан, но я не работаю в кредит.";
				Link.l1 = "Ах ты, черт!";
				Link.l1.go = "Exit";								
			}
		break;

		case "OnboardSoon":			
			Diag.TempNode = "OnboardSoon";
			dialog.text = "Сейчас я допью, капитан, и отправлюсь на борт. Не извольте беспокоиться - я успею до отплытия.";
			Link.l1 = "Хорошо. Опоздаешь - заставлю драить палубу!";
			Link.l1.go = "Exit";											
		break;
        
        case "QMASTER_1":
            dialog.text = "Максимальная активность крыс в рейсе " +
                     FloatToString(50.0 / (2.0+GetSummonSkillFromNameToOld(PChar, SKILL_REPAIR) + GetSummonSkillFromNameToOld(PChar,SKILL_SNEAK)), 1) +
                     "% от количества груза. На "+GetCrewQuantity(PChar) + " матросов нужно " + makeint((GetCrewQuantity(PChar)+6) / 10) + " провианта в день. Это без учета перевозимых рабов.";
            Link.l1 = "Спасибо.";
            Link.l1.go = "Exit";
            Diag.TempNode = "Hired";
        break;
        
        // boal 05.09.03 offecer need to go to abordage -->
        case "stay_follow":
            dialog.text = "Какие будут приказания?";
            Link.l1 = "Стой здесь!";
            Link.l1.go = "Boal_Stay";
            Link.l2 = "Следуй за мной и не отставай!";
            Link.l2.go = "Boal_Follow";
            // boal 05.09.03 offecer need to go to abordage <--
		break;
		
		case "AsYouWish":
			// проверка на море -->
			if (bSeaActive)
			{
				attr = Sea_FindNearColony();
				if (attr == "none")
				{
					dialog.text = "Это ваше право, но сперва довезите меня до какого-нибудь города.";
					Link.l1 = "Хорошо, так и сделаем. Пока оставайся на службе.";
					Link.l1.go = "Exit";	
					break;	
				}
			}
			// проверка на море <--
			dialog.text = "Как скажете, капитан. Но смотрите: если вы выгоните меня - я больше никогда не вернусь на ваше судно. Так что решать вам.";
			Link.l1 = "Пожалуй, я передумал. Ты мне еще нужен.";
			Link.l1.go = "Exit";								
			Link.l2 = "Отлично. Можешь проваливать, куда подальше.";
			Link.l2.go = "Exit_Fire";		
		break;
		
		case "Fired":
		      Diag.TempNode = "Fired";
		      dialog.text = "Я же предупреждал вас, капитан, что не вернусь на ваш корабль. У меня есть своя гордость.";
		      Link.l1 = "Да ты мне и не нужен!";
		      Link.l1.go = "Exit";
	      break;
        // boal 05.09.03 offecer need to go to abordage -->
	      case "Boal_Stay":
             //SetCharacterTask_Stay(Characters[Npchar.index]); // it's a mistic but here doesn't work :(
             //Log_SetStringToLog(Npchar.id +" "+Npchar.index);
              Pchar.questTemp.HiringOfficerIDX = GetCharacterIndex(Npchar.id);
              AddDialogExitQuestFunction("LandEnc_OfficerStay");
		      Diag.TempNode = "Hired";
		      dialog.text = "Есть изменить дислокацию!";
		      Link.l1 = "Вольно.";
		      Link.l1.go = "Exit";
		      Npchar.chr_ai.tmpl = LAI_TMPL_STAY;
	      break;
	      case "Boal_Follow":
		      SetCharacterTask_FollowCharacter(Npchar, PChar); // it works here!!!
		      Diag.TempNode = "Hired";
		      dialog.text = "Есть изменить дислокацию!";
		      Link.l1 = "Вольно.";
		      Link.l1.go = "Exit";
	      break;
        // boal 05.09.03 offecer need to go to abordage <--

		//////////////////////////////    офицер-наместник -->
		case "Gover_Hire":
            dialog.Text = "Почту за честь!";
            Link.l1 = "Следи за порядком и процветанием города, я буду иногда приплывать за собранной пошлиной.";
			Link.l1.go = "Exit_Gover_Hire";
            Link.l2 = "Я передумал.";
            Link.l2.go = "Exit";
            Diag.TempNode = "Hired";
        break;

        case "Exit_Gover_Hire":
            sld = GetColonyByIndex(sti(NPChar.ColonyIdx));
            attr = sld.id + "_townhall";

            // снимем пассажира -->
			CheckForReleaseOfficer(sti(NPChar.index));
			RemovePassenger(pchar, NPChar);
			// снимем пассажира <--
		    sld.OfficerIdx = sti(NPChar.index);
            NPChar.location = attr;
            NPChar.location.group = "goto";
			NPChar.location.locator = "governor1";
			LAi_SetHuberStayTypeNoGroup(NPChar);
            SaveCurrentNpcQuestDateParam(NPChar, "StartTaxDate");
            SaveCurrentNpcQuestDateParam(NPChar, "GoverTalkDate");
            LAi_LoginInCaptureTown(NPChar, true);
			//  СЖ -->
			ReOpenQuestHeader("Gen_CityCapture");
            AddQuestRecord("Gen_CityCapture", "t3_1");
			AddQuestUserData("Gen_CityCapture", "sCity", XI_ConvertString("colony" + sld.id));
			AddQuestUserData("Gen_CityCapture", "sName", GetFullName(NPChar));
			//  СЖ <--
			Diag.CurrentNode = "Gover_Main";
            DialogExit();
        break;

        case "Gover_Main":
            iTemp = GetNpcQuestPastDayParam(NPChar, "GoverTalkDate");
            if (iTemp > 0)
            {
				dialog.Text = "Какие будут приказания? Последний ваш визит был " + FindRussianDaysString(iTemp) + " назад.";
			}
			else
			{
			    dialog.Text = "Какие будут приказания?";
			}

            Link.l1 = "Какую сумму налогов ты собрал на данный момент?";
        	Link.l1.go = "Gover_Tax";
            Link.l8 = "Ты нужен мне на корабле, я снимаю тебя с городской службы.";
            Link.l8.go = "Gover_Fire";
			Link.l9 = "Всего хорошего.";
            Link.l9.go = "Exit";
            Diag.TempNode = "Gover_Main";
            SaveCurrentNpcQuestDateParam(NPChar, "GoverTalkDate");
        break;

        case "Gover_Tax":
            iTemp = GetNpcQuestPastDayParam(NPChar, "StartTaxDate");
            iTax  = makeint((GetCharacterSkillSimple(NPChar, SKILL_COMMERCE) + GetCharacterSkillSimple(NPChar, SKILL_LEADERSHIP)) / 2);
            sld = GetColonyByIndex(sti(NPChar.ColonyIdx));
			if (CheckAttribute(sld, "FortValue"))
			{
			    iFortValue = sti(sld.FortValue);
			}
			else
			{
			    iFortValue = 20;
			}
			NPChar.Gover_Tax_Sum = iFortValue*iTax*iTemp;
			dialog.Text = "Мои навыки позволяют мне собирать " + FindRussianMoneyString(iFortValue*iTax) + " в день. За " + FindRussianDaysString(iTemp) + " я собрал " + FindRussianMoneyString(sti(NPChar.Gover_Tax_Sum)) + ".";
			if (sti(NPChar.Gover_Tax_Sum) > 0)
			{
			    Link.l1 = "Я хочу забрать всю сумму налогов.";
            	Link.l1.go = "Gover_Tax_Get";
			}
			Link.l2 = "Благодарю за службу!";
            Link.l2.go = "Exit";
        break;

        case "Gover_Tax_Get":
            SaveCurrentNpcQuestDateParam(NPChar, "StartTaxDate");
            AddMoneyToCharacter(Pchar, sti(NPChar.Gover_Tax_Sum));

			Diag.CurrentNode = "Gover_Main";
            DialogExit();
        break;

        case "Gover_Fire":
            dialog.Text = "Замечательно! Быть сухопутной крысой не мой удел.";
            Link.l8 = "Вот и славно.";
            Link.l8.go = "exit_hire";
            Diag.TempNode = "Hired";
            sld = GetColonyByIndex(sti(NPChar.ColonyIdx));
            DeleteAttribute(sld, "OfficerIdx");
            //  СЖ -->
			ReOpenQuestHeader("Gen_CityCapture");
            AddQuestRecord("Gen_CityCapture", "t3_2");
			AddQuestUserData("Gen_CityCapture", "sCity", XI_ConvertString("colony" + sld.id));
			AddQuestUserData("Gen_CityCapture", "sName", GetFullName(NPChar));
			//  СЖ <--
        break;
        //////////////////////////////    офицер-наместник <--
	}
}
