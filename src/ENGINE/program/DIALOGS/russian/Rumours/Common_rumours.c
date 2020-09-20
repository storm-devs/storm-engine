// Слухи - диалог по типажам
void ProcessCommonDialogRumors(ref NPChar, aref Link, aref NextDiag);
{

    string strum;
    string srum;
	switch(Dialog.CurrentNode)
	{
	case "rumours_citizen":	
		NextDiag.CurrentNode = "rumours";
		//------------- квестовые слухи горожан --------->>>>>>>>
		//жемчужный генератор Шарпа
		if (pchar.questTemp.Sharp.City == npchar.city && sti(pchar.questTemp.Sharp.City.rumour))
		{
			pchar.questTemp.Sharp.City.rumour = false; //слух дан
			Dialog.Text = Sharp_choiceAction();
			link.l1 = "Гм, понятно... Еще что нового?";
			link.l1.go = "new question";
			link.l2 = RandPhraseSimple("Спасибо, должен откланяться.",
									"Всего хорошего.");
			link.l2.go = "exit";
			break;
		}
		//квест щебеки Синяя Птица
		if (pchar.questTemp.BlueBird.City == npchar.city && pchar.questTemp.BlueBird == "returnMoney")
		{
			Dialog.Text = BlueBurd_setTradeShip();
			link.l1 = "Ага, понятно... Что еще нового в городе?";
			link.l1.go = "new question";
			link.l2 = RandPhraseSimple("Спасибо, должен откланяться.",
									"Всего хорошего.");
			link.l2.go = "exit";
			//новый слух в другом городе
			pchar.questTemp.BlueBird.City = GetSharpCity();
			break;
		}
		//<<<<<-------- квестовые слухи горожан -----------------
		if (!CheckAttribute(NPChar, "quest.repeat.rumours_citizen") || NPChar.quest.repeat.rumours_citizen != 3 )
        srum = SelectRumourEx("citizen", NPChar);
        else srum = NO_RUMOUR_TEXT[rand(4)]; // fix
        string posrep1, posrep2, answ1, answ2, answ3, answ4;
        if (RumourHasInformation(srum))
        {
            posrep1 = RandPhraseSimple(" Такие вот дела...", " Возможно, вас это позабавит.");
            posrep2 = " А у вас, капитан, есть какие-нибудь известия для нашей колонии?";
            answ1 = RandPhraseSimple(RandSwear() + "Интересные вещи ты рассказываешь, "+GetFullName(NPChar)+".",
                                 "Тогда расскажи мне еще...");
            answ2 = RandPhraseSimple(RandSwear() + "Это очень интересно! Вот что еще хотел спросить...",
                                 "Еще вопрос.");
            answ3 = RandPhraseSimple("Чего только не случается на свете. Мне жаль, но никаких новостей у меня нет. ",
                                 "Я немного спешу, так что как-нибудь в другой раз.");
            answ4 = "";
        
        }
        else
        {
            posrep1 = " У нас вообще редко что-либо происходит. Но если же произойдет что-то необычное, то все об этом только и говорят.";
            posrep2 = " Так что новостей никаких. Но может " + GetAddress_Form(NPChar) + " капитан что-то знает?";
            answ1 = RandPhraseSimple("Тогда расскажи-ка мне лучше...",
                                 RandSwear() + "Ты ничего не знаешь! Ладно, вот что еще хотел спросить...");
            answ2 = RandPhraseSimple("Говоришь, не знаешь, ладно, скажи мне тогда...",
                                 "Да, не сильно ты мне помог"+NPCharSexPhrase(NPChar, " ", "ла ")+", но, может, ты знаешь что-то еще?");
            answ3 = RandPhraseSimple("Не думаю, что знаю что-то что, могло бы вас заинтересовать.",
                                 "Я с удовольствием что-нибудь расскажу, но как-нибудь в другой раз.");
            answ4 = "";
        
        }
		Dialog.Text = NPCStringReactionRepeat(srum,
                                                srum+posrep1,
                                                srum+posrep2,
                                                RandPhraseSimple("К сожалению, больше я ничего не знаю, позвольте мне идти.",
                                                                    "Вы утомили меня своими расспросами, прошу меня простить, но меня ждут дела."),
                                                "block", 1, npchar, Dialog.CurrentNode);
		
		//link.l1 = RandPhraseSimple(RandSwear() + "Это очень интересно! Вот что еще хотел спросить...",
        //                         "Еще вопрос.");
        link.l1 = HeroStringReactionRepeat(answ1,
                                            answ2,
                                            answ3,
                                            answ4,
                                            npchar, Dialog.CurrentNode);
		link.l1.go = "new question";
		link.l2 = RandPhraseSimple("Спасибо, должен откланяться.",
                                 "Всего хорошего.");
		link.l2.go = "exit";
	break;
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	case "rumours_habitue":
		Dialog.Text = LinkRandPhrase("Ну... И-ик! ","Эх, какой ром! ","Нда... ")+SelectRumourEx("habitue", NPChar);
		link.l1 = RandPhraseSimple(RandSwear() + "За это надо выпить еще кружечку...",
                                 "Хорошо. Давай еще по одной.");
		link.l1.go = "sit_3";
		link.l2 = RandPhraseSimple("Спасибо, должен откланятся.",
                                 "Эх, забористый ром. Ладно, мне уже пора, удачно посидеть.");
		link.l2.go = "exit_sit";
		if (pchar.questTemp.piratesLine == "PL5Hunter_toFirst" && npchar.location == pchar.questTemp.piratesLine.Q5.city_1 + "_tavern")
		{
			dialog.text = "Представляешь, сижу никого не трогаю, вдруг, подлетает этот Эйвори и не спросясь – хрясь мне в морду! Ты, говорит, барыге денег должен?! А потом говорит - обознался. Он обознался, а я без зуба остался! А! Каково?! И, не поверишь, неделя уж прошла, а до сих пор болит. Такая боль! Может, по кружке эля?..";
			link.l1 = "Ну, это можно...";
			link.l1.go = "sit_3";
			AddQuestRecord("Pir_Line_5_Hunter", "3");
			AddQuestUserData("Pir_Line_5_Hunter", "sCity", XI_ConvertString("Colony" + pchar.questTemp.piratesLine.Q5.city_1 + "Gen"));
			pchar.questTemp.piratesLine = "PL5Hunter_toFirstUsurer";
		}
		if (pchar.questTemp.piratesLine == "PL5Hunter_toSecond" && npchar.location == pchar.questTemp.piratesLine.Q5.city_2 + "_tavern")
		{
			dialog.text = "Подсел ко мне, дней пять назад, мужик, вот прям как ты сейчас. Звали его, дай бог памяти... Джон Эйвори. Рассказал, как он надул местного ростовщика, хе-хе-хе. Так ему и надо, кровососу проклятому, сидит на деньгах и пухнет, а работному человеку и закусить-то нечего...";
			link.l1 = "Да уж, эти ростовщики - народ поганый.";
			link.l1.go = "sit_3";
			AddQuestRecord("Pir_Line_5_Hunter", "5");
			AddQuestUserData("Pir_Line_5_Hunter", "sCity", XI_ConvertString("Colony" + pchar.questTemp.piratesLine.Q5.city_2 + "Gen"));
			pchar.questTemp.piratesLine = "PL5Hunter_toSecondUsurer";
		}
		if (pchar.questTemp.piratesLine == "PL5Hunter_toThird" && npchar.location == pchar.questTemp.piratesLine.Q5.city_3 + "_tavern")
		{
			dialog.text = "Три дня назад, тут такая драка была! Джон Эйвори шулера подколол. Знатно, надо сказать, дядька железякой машет. Вжик, вжик! И нету шулера. Крик поднялся, гам. Но Джон, мужик правильный, всем в таверне выпивку поставил. Уважаю!";
			link.l1 = "О как! Круто...";
			link.l1.go = "sit_3";
			AddQuestRecord("Pir_Line_5_Hunter", "7");
			AddQuestUserData("Pir_Line_5_Hunter", "sCity", XI_ConvertString("Colony" + pchar.questTemp.piratesLine.Q5.city_3 + "Gen"));
			pchar.questTemp.piratesLine = "PPL5Hunter_toThirdUsurer";
		}
	break;
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		case "rumours_trader":
            if (!CheckAttribute(NPChar, "quest.repeat.rumours_trader") || NPChar.quest.repeat.rumours_trader!= 3 )
            strum = SelectRumourEx("trader", NPChar);
            else strum = NO_RUMOUR_TEXT[rand(4)]; // fix
			Dialog.Text =  NPCStringReactionRepeat(strum, strum+" Да. Больше ничего не припомню",
                                                   strum+" А сейчас мне нужно вернуться к делам.",
                                                    RandPhraseSimple("Я же уже сказал, что у меня нет времени на пустую болтовню!",
                                                                        "Я занятой человек и мне некогда обсуждать всякие сплетни!"),
                                                                         "block", 1, npchar, Dialog.CurrentNode);
			link.l1 = HeroStringReactionRepeat(pcharrepphrase(RandSwear() + "Это очень интересно! Но давай к делу","Благодарю, тогда вот еще какое дело"),
                                                pcharrepphrase("Чтоб я сдох! Складно врешь. Но у меня есть еще вопрос.",
                                                                "Так не будем больше терять драгоценное время и перейдем к делам"),
                                                pcharrepphrase(RandSwear() +" Еще одно дельце и я уйду.","Вы правы, время - деньги."),
                                                pcharrepphrase("Только и думаешь, что о своей выгоде. Ладно не кипятись.",
                                                                "Прошу простить меня. Перейдем к делу."),
                                                npchar, Dialog.CurrentNode);

			link.l1.go = "business";
			link.l2 = HeroStringReactionRepeat(NPCharRepPhrase(npchar,
			                                        pcharrepphrase("Пока, старый пропойца.",
													               "Всего доброго, "+GetAddress_FormToNPC(NPChar)+" " +GetFullName(npchar)+"."),
													pcharrepphrase("Каррамба! Никуда не уходи. Я еще зайду.",
													               "Приятно было вас видеть, "+GetAddress_FormToNPC(NPChar)+" " +GetFullName(npchar)+"! До свидания!")),
											   NPCharRepPhrase(npchar,
											        pcharrepphrase("Отдать швартовы!",
													               "До свидания, милейший."),
													pcharrepphrase("Эй! Ты что там прячешь? Ром? Нет!?! Ну ладно, пойду горло промочу.",
													               "Прошу меня извинить, у меня срочные дела в городе.")),
											   NPCharRepPhrase(npchar,
											        pcharrepphrase("Хех! Еще увидимся!",
													               "Мне надо идти. Прощайте, "+GetAddress_FormToNPC(NPChar)+" " +GetFullName(npchar)+"!"),
													pcharrepphrase("И не смей  мне перечить! Недосуг мне тут с тобой спорить!",
													               "Пожалуй, стоит проверить как дела на корабле. Прощайте, "+GetAddress_FormToNPC(NPChar)+" " +GetFullName(npchar)+"!")),
											   NPCharRepPhrase(npchar,
											        pcharrepphrase("Что с тобой, старина? Это же я - " +GetFullName(pchar)+ "! Проспись, я потом зайду.",
													               "Боюсь, ваша печень прикончит вас раньше, чем я еще раз зайду сюда."),
													pcharrepphrase("Считай свои пиастры, пройдоха! Я еще зайду.",
													               "Рад был вас видеть, " + GetAddress_FormToNPC(NPChar)+" " +GetFullName(npchar)+"!")),
											  npchar, Dialog.CurrentNode);
			link.l2.go = "Exit";
		break;
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        case "rumours_capitan":
			Dialog.Text = SelectRumourEx("capitan", NPChar);
			Link.l1 = RandPhraseSimple(RandSwear() + "Это очень интересно! Вот что еще хотел спросить...",
                                     "Еще вопрос.");
			Link.l1.go = "quests";
			Link.l2 = RandPhraseSimple("Спасибо, должен откланятся.",
                                     "Всего хорошего");
			Link.l2.go = "exit";
			NextDiag.TempNode = "quests";
            NPChar.talk_date =  lastspeakdate();
		break;
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		case "rumours_tavern":  // homo 03/08/06

                 Dialog.Text = NPCStringReactionRepeat(LinkRandPhrase("Сведения стоят денег. За скромные 1000 золотых я расскажу все что знаю.",
                "Что-то я запамятовал. Но возможно 1000 монет освежат мою память.",
                "Я готов рассказать все, что знаю. Но не бесплатно - 1000 золотых, на меньшее я не согласен."),
                "Больше мне нечего добавить. А теперь прошу меня извинить, но дела не ждут.",
                "", "", "block", 1, npchar, Dialog.CurrentNode);
             
    			link.l1 = HeroStringReactionRepeat(RandPhraseSimple(RandSwear() + "Платить за пустую болтовню? Нет уж, поищи кого-нибудь другого.",
                                         "За такие деньги я сам все узнаю."), "Давай поговорим о чем-нибудь другом.", "Давай поговорим о чем-нибудь другом.", "Давай поговорим о чем-нибудь другом.", npchar, Dialog.CurrentNode);
                link.l1.go = HeroStringReactionRepeat("rumours_tavern_no", "first time", "first time", "first time", npchar, Dialog.CurrentNode);//"rumours_tavern_no";
                NextDiag.CurrentNode = NextDiag.TempNode;
    			if (makeint(pchar.money) >= 1000 )
    			{
    				link.l2 = HeroStringReactionRepeat(RandPhraseSimple("Ты их получишь, если я получу действительно ценные сведения.", "Я заплачу, но только при условии, что ты мне расскажешь действительно что-то важное."),
                     "", "", "" , npchar, Dialog.CurrentNode);
    				link.l2.go = HeroStringReactionRepeat("rumours_tavern_yes", "exit", "exit", "exit", npchar, Dialog.CurrentNode);
    			}
    			else
    			{
                    link.l2 =  HeroStringReactionRepeat("У меня нет такой суммы.", "", "", "", npchar, Dialog.CurrentNode);
    				link.l2.go = HeroStringReactionRepeat("rumours_tavern_no", "exit", "exit", "exit", npchar, Dialog.CurrentNode);
    			}

		break;
		
		case "rumours_tavern_yes":

			//------------- квестовые слухи тавернщика --------->>>>>>>>
			//пиратка, квест №3
			if (pchar.questTemp.piratesLine == "KillLoy_toCumana")
			{
				Dialog.Text =  "Последнее время, стали часто пропадать корабли, причем, что странно, пропадают только курьерские люггеры и шлюпы. Поговаривают о появлении в водах Куманы какого-то брига... Так что будьте острожны!";
				link.l1 = "Пропадают только курьерские люггеры, говоришь? Очень интересно... А больше ничего примечательного не знаешь?";
				link.l1.go = "rumours_tavern";
				AddQuestRecord("Pir_Line_3_KillLoy", "14");		
				pchar.questTemp.piratesLine = "KillLoy_toCumana_2";
				break;
			}
			//<<<<<-------- квестовые слухи тавернщика -----------------
		
            string RumText = SelectRumourEx("tavern", NPChar); //fix
            //SelectRumour();


		    if ( RumText == NO_RUMOUR_TEXT[0] || RumText == NO_RUMOUR_TEXT[1]
                || RumText == NO_RUMOUR_TEXT[2] || RumText == NO_RUMOUR_TEXT[3] || RumText == NO_RUMOUR_TEXT[4] )//fix
            {

                Dialog.Text = NO_RUMOUR_TEXT[rand(3)]; //fix
                link.l1 = RandPhraseSimple("Я не стану платить за пустые сплетни, прощай.", "Твои сведения не стоят и ломаного гроша - ты ничего не получишь.");

			    link.l1.go = "Exit";
			    NextDiag.CurrentNode = NextDiag.TempNode;
			  //  link.l2 = LinkRandPhrase("Спасибо, должен откланятся.",
              //                       "Всего хорошего","Спасибо, мне пора. До свидания.");
			  //  link.l2.go = "Exit";
            }
            else
            {

                AddMoneyToCharacter(pchar, -1000);

                Dialog.Text = RumText;

    			link.l1 = RandPhraseSimple("Благодарю, вот твое золото. А больше ничего не припомнишь?", "Хорошо, ты заслужил свои деньги - держи. Есть что-нибудь еще?");
                //RandPhraseSimple(RandSwear() + "Это очень интересно! Вот что еще хотел спросить...",
                //                         "Еще вопрос.");

    			link.l1.go = "rumours_tavern";
    			NextDiag.CurrentNode = NextDiag.TempNode;
    			//link.l2 = LinkRandPhrase("Спасибо, должен откланятся.",
              //                           "Всего хорошего","Спасибо, ты мне очень помог. До свидания.");
    		//	link.l2.go = "Exit";
            }

		break;

		case "rumours_tavern_no":
			Dialog.Text = "Поймите меня правильно, мне нужно кормить семью.";
            link.l1 = "Давай поговорим о чем-нибудь другом.";
			link.l1.go = "first time";
			NextDiag.CurrentNode = NextDiag.TempNode;
			link.l2 = LinkRandPhrase("Спасибо, должен откланятся.",
                                     "Всего хорошего","До свидания.");
			link.l2.go = "Exit";
		break;	
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        case "rumours_poor":
			if (!CheckAttribute(NPChar, "quest.repeat.rumours_poor") || NPChar.quest.repeat.rumours_poor <= 3)
			{
				Dialog.Text = SelectRumourEx("poor", NPChar);
				Link.l1 = RandPhraseSimple(RandSwear() + "Да, это интересно. Имеется еще вопрос...",
										"Хм, вот что еще хотел спросить...");
				Link.l1.go = "rumours_poor_2";
				Link.l2 = RandPhraseSimple("Ну, прощай, барыга.",
										"Всего хорошего, приятель.");
				Link.l2.go = "exit";
				NPChar.quest.repeat.rumours_poor = sti(NPChar.quest.repeat.rumours_poor) + 1;
			}
			else 
			{
				Dialog.Text = RandPhraseSimple("Не о чем говорить, все уже выболтал, что знал.", 
					"Больше я ничего не слышал, извините...");
				Link.l1 = RandPhraseSimple("Понятно. Ну что же, всего хорошего.",
										"Ясно. Ну, бывай, приятель...");
				Link.l1.go = "exit";			
			}
            NPChar.talk_date = lastspeakdate();
		break;
        case "rumours_poor_2":
			if (NPChar.quest.repeat.rumours_poor <= 3)
			{
				Dialog.Text = SelectRumourEx("poor", NPChar);
				Link.l1 = RandPhraseSimple(RandSwear() + "Да, это интересно. Имеется еще вопрос...",
										"Хм, вот что еще хотел спросить...");
				Link.l1.go = "rumours_poor";
				Link.l2 = RandPhraseSimple("Ну, прощай, барыга.",
										"Всего хорошего, приятель.");
				Link.l2.go = "exit";
				NPChar.quest.repeat.rumours_poor = sti(NPChar.quest.repeat.rumours_poor) + 1;
			}
			else 
			{
				Dialog.Text = RandPhraseSimple("Не о чем говорить, все уже выболтал, что знал.", 
					"Больше я ничего не слышал, извините...");
				Link.l1 = RandPhraseSimple("Понятно. Ну что же, всего хорошего.",
										"Ясно. Ну, бывай, приятель...");
				Link.l1.go = "exit";			
			}
			NPChar.talk_date = lastspeakdate();
		break;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////		Кредитный генератор Loan
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		case "LoanForAll": //пересылка в финансы для всех
			dialog.text = LinkRandPhrase("Финансы? Хм, говорите, вас слушаю.", "Финансовые вопросы - это всегда интересно. Говорите, я слушю вас.", "Что же, давайте поговорим о финансах.");
            if (CheckAttribute(pchar, "GenQuest.Loan.FindCitizen") && !CheckAttribute(pchar, "GenQuest.Loan.FindCitizenDone") && !CheckAttribute(pchar, "GenQuest.Loan.FindCitizenFalied"))
            {
                if (Characters[sti(pchar.GenQuest.Loan.FindCitizenIdx)].id == Npchar.id)
                {
                    dialog.text = LinkRandPhrase("А вы уверены, что обратились по адресу, " + GetAddress_Form(NPChar) + "? Мое имя " + GetFullName(npchar) + ". Чем могу служить?",
                             "По финансам ко мне, " + GetAddress_Form(NPChar) + "?! Я " + GetFullName(npchar) + ". А что за дело у вас?",
                             "Я вас слушаю, " + GetAddress_Form(NPChar) + ". Что вы хотели? Кстати, меня зовут " + GetFullName(npchar) + ".");
                    link.l1 = NPCharRepPhrase(pchar, "Я от ростовщика, должник! Деньги на бочку!", "У меня к вам щекотливый вопрос по вашему долгу, " + GetAddress_FormToNPC(NPChar) + ".");
                    link.l1.go = "LoanFindCitizen_1";
				    NPChar.quest.LoanFindCitizenType = rand(2); // три типа
                    break;
                }
            }
			if (CheckAttribute(pchar, "GenQuest.LoanChest.TakeChest") && sti(pchar.GenQuest.LoanChest.TargetIdx) == sti(NPChar.index))
            {
                link.l2 = "Я привез ваш кредит.";
                link.l2.go = "TakeChest_2";
            }
			link.l9 = RandPhraseSimple("М-м-м, я ,видимо, ошибся. Никаких финасовых дел я с вами не веду.", "Простите, никаких финансовых вопросов у меня к вам нет.");
			link.l9.go = "exit";
		break;
		// вернуть долг ростовщику
		case "LoanFindCitizen_1":
            switch (sti(NPChar.quest.LoanFindCitizenType))
            {
                case 0 :
                    dialog.text = "Да, точно. Я как раз собирался ему все отдать.";
        			Link.l1 = "Ну что же, очень хорошо! Итого с тебя " + FindRussianMoneyString(makeint(sti(pchar.GenQuest.Loan.FindCitizenMoney) + sti(pchar.GenQuest.Loan.FindCitizenPercent))) + ".";
        			Link.l1.go = "LoanFindCitizenType_0";
        			Link.l2 = "Замечательно, вот и отнеси все сам!";
        			Link.l2.go = "exit";
                break;
                
                case 1 :
                    dialog.text = "Нет! Ничего я не отдам! А будете приставать, стражу позову.";
                    Link.l1 = "Ничего не знаю, с тебя " + FindRussianMoneyString(makeint(sti(pchar.GenQuest.Loan.FindCitizenMoney) + sti(pchar.GenQuest.Loan.FindCitizenPercent))) + ". А то можно поговорить и по другому.";
        			Link.l1.go = "LoanFindCitizenType_1";
        			Link.l2 = "Ну и не нужно!";
        			Link.l2.go = "LoanFindCitizenType_1_1";
                break;
                
                case 2 :
                    dialog.text = "К сожалению у меня нет всей суммы.";
                    Link.l1 = "Ничего не знаю, с тебя " + FindRussianMoneyString(makeint(sti(pchar.GenQuest.Loan.FindCitizenMoney) + sti(pchar.GenQuest.Loan.FindCitizenPercent))) + ". А то можно поговорить и по другому.";
        			Link.l1.go = "LoanFindCitizen_1";
        			Link.l2 = "А сколько есть?";
        			Link.l2.go = "LoanFindCitizenType_2";
        			NPChar.quest.LoanFindCitizenType = rand(1);// первые 2 типа
                break;
			}
		break;

		case "LoanFindCitizenType_0":
            dialog.text = "Вот вся сумма.";
			Link.l1 = "Спасибо за сотрудничество!";
			Link.l1.go = "exit";
			pchar.GenQuest.Loan.FindCitizenDone = true;
        	addMoneyToCharacter(Pchar, (sti(pchar.GenQuest.Loan.FindCitizenMoney) + sti(pchar.GenQuest.Loan.FindCitizenPercent)));
        	SetTimerCondition("LoanFindCitizenTimer", 0, 0, 30, false);
        	Log_SetStringToLog("У вас есть 30 дней, чтобы доложить работодателю и вернуть ему деньги.");
        	AddCharacterExpToSkill(pchar, "COMMERCE", 20);        	
        	AddQuestRecord("Gen_LoanFindCitizen", "3");
			npchar.LifeDay = 0;
		break;
		
		case "LoanFindCitizenType_1_1":
            dialog.text = "Рад, что мы с вами договорились.";
			Link.l1 = "Да ну тебя!";
			Link.l1.go = "exit";
			pchar.GenQuest.Loan.FindCitizenFalied = true;
        	SetTimerCondition("LoanFindCitizenTimer", 0, 0, 30, false);
        	Log_SetStringToLog("У вас есть 30 дней, чтобы доложить работодателю о провале миссии.");
        	AddQuestRecord("Gen_LoanFindCitizen", "2");
			npchar.LifeDay = 0;
		break;
		
		case "LoanFindCitizenType_1":
            dialog.text = "Стража!! Хулиганы зрения лишают!";
			Link.l1 = "Замолчи!";
			Link.l1.go = "fight";
			pchar.GenQuest.Loan.FindCitizenFalied = true;
        	SetTimerCondition("LoanFindCitizenTimer", 0, 0, 30, false);
        	Log_SetStringToLog("У вас есть 30 дней, чтобы доложить работодателю о провале миссии.");
        	AddQuestRecord("Gen_LoanFindCitizen", "2");
			npchar.LifeDay = 0;
		break;
		
		case "LoanFindCitizenType_2":
            pchar.GenQuest.Loan.FindCitizenTempMoney = 10 + rand(makeint(sti(pchar.GenQuest.Loan.FindCitizenMoney) + sti(pchar.GenQuest.Loan.FindCitizenPercent) - 1000));
            dialog.text = "Только " + pchar.GenQuest.Loan.FindCitizenTempMoney + " золотых.";
			Link.l1 = "Нет. Нужно всю сумму!";
			Link.l1.go = "LoanFindCitizen_1";
			Link.l2 = "Давай, что есть. Сам доплачу. Такой я сегодня добрый.";
			Link.l2.go = "LoanFindCitizenType_2_1";
			NPChar.quest.LoanFindCitizenType = rand(1);// первые 2 типа
		break;
		
		case "LoanFindCitizenType_2_1":
            dialog.text = "Вы очень добры! Большое спасибо!";
			Link.l1 = "Эх! Вечно сам во все лезу.";
			Link.l1.go = "exit";
			pchar.GenQuest.Loan.FindCitizenDone = true;
        	addMoneyToCharacter(Pchar, sti(pchar.GenQuest.Loan.FindCitizenTempMoney));
        	SetTimerCondition("LoanFindCitizenTimer", 0, 0, 30, false);
        	Log_SetStringToLog("У вас есть 30 дней, чтобы доложить работодателю и вернуть ему деньги.");
        	OfficersReaction("good");
        	ChangeCharacterReputation(pchar, 3);
        	AddCharacterExpToSkill(pchar, "Leadership", 20);
        	AddQuestRecord("Gen_LoanFindCitizen", "3");
			npchar.LifeDay = 0;
		break;
		//доставка сундуков
        case "TakeChest_2":
            if (GetCharacterItem(pchar, "Chest") >= sti(pchar.GenQuest.LoanChest.Chest))
            {
                if (CheckAttribute(pchar, "GenQuest.LoanChest.Money.Discount"))
				{
					Dialog.Text = "Все это хорошо и я принимаю у вас золото. Однако, вы не уложились в отведенные для вас сроки доставки. Поэтому я вынужден сократить ваш размер вашего гонорара на " + pchar.GenQuest.LoanChest.Money.Discount + "%. Прошу прощения, но это обсуждению не подлежит. Извольте получить...";
					link.l1 = "Спасибо!";
    				link.l1.go = "exit";
					AddMoneyToCharacter(pchar, sti(pchar.GenQuest.LoanChest.Money)/100*sti(pchar.GenQuest.LoanChest.Money.Discount));
				}
				else
				{
					Dialog.Text = "Замечательно! Я давно его жду. Вот ваша награда.";
					link.l1 = "Спасибо!";
    				link.l1.go = "exit";
					AddMoneyToCharacter(pchar, sti(pchar.GenQuest.LoanChest.Money));
				}
				ChangeCharacterReputation(pchar, 1);
                AddCharacterExpToSkill(pchar, "Sailing", 50);
				AddCharacterExpToSkill(pchar, "Leadership", 20);
				AddCharacterExpToSkill(pchar, "COMMERCE", 5);
                OfficersReaction("good");
    			TakeNItems(pchar, "Chest", -sti(pchar.GenQuest.LoanChest.Chest));    			
    			DeleteAttribute(pchar, "GenQuest.LoanChest");
    			DeleteAttribute(Pchar, "quest.LoanChestTimer");
    			AddQuestRecord("Gen_LoanTakeChest", "3");
                CloseQuestHeader("Gen_LoanTakeChest");
			}
			else
			{
                Dialog.Text = "Замечательно! Но где же все сундуки с золотом?";
                link.l1 = "А вы можете принять золото без сундуков?";
    			link.l1.go = "TakeChest_3";
                link.l2 = "Зайду позже.";
    			link.l2.go = "exit";
			}
		break;

		case "TakeChest_3":
            Dialog.Text = "Нет! Только в сундуках!";
            link.l1 = "Плохо. Крысы у меня их попортили.";
			link.l1.go = "exit";
			link.l2 = "Ну а, скажем, сумма в " + FindRussianMoneyString(sti(pchar.GenQuest.LoanChest.Chest)*30000) + " покроет стоимость сундуков?";
			link.l2.go = "TakeChest_4";
		break;

		case "TakeChest_4":
            Dialog.Text = "Да, это приемлимо.";
            if (sti(pchar.Money) >= (sti(pchar.GenQuest.LoanChest.Chest)*30000))
            {
                link.l1 = "Вот и договорились!";
    			link.l1.go = "TakeChest_5";
			}
			link.l2 = "Только это много для меня.";
			link.l2.go = "exit";
		break;

		case "TakeChest_5":
            ChangeCharacterReputation(pchar, 1);
            AddCharacterExpToSkill(pchar, "Sailing", 30);
			AddCharacterExpToSkill(pchar, "Leadership", 10);
			AddCharacterExpToSkill(pchar, "COMMERCE", 5);
            OfficersReaction("good");
            Dialog.Text = "С вами можно иметь дело!";
            link.l1 = "Спасибо!";
			link.l1.go = "exit";
			AddMoneyToCharacter(pchar, -1*(sti(pchar.GenQuest.LoanChest.Chest)*30000));
			DeleteAttribute(pchar, "GenQuest.LoanChest");
			DeleteAttribute(Pchar, "quest.LoanChestTimer");
			AddQuestRecord("Gen_LoanTakeChest", "4");
            CloseQuestHeader("Gen_LoanTakeChest");
		break; 

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////		Квесты мэра, проникновение во враждебный город
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		case "IntelligenceForAll":
            Dialog.Text = RandPhraseSimple("Ну что же, очень хорошо. У меня готов для вас пакет, который вам надлежит доставить известному лицу лично в руки. Извольте получить.\nНе подходите ко мне больше и не заговаривайте со мной, это может быть подозрительным. Соблюдайте острожность...", 
				"Хм, прекрасно. Я вручаю вам пакет, и помните, что он должнен быть доставлен названному вами лицу лично в руки.\nТеперь прощайте и не подходите ко мне более - это опасно.");
            link.l1 = RandPhraseSimple("Я понял. Прощайте...", "Прощайте и удачи.");
			link.l1.go = "exit";
			pchar.GenQuest.Intelligence = "Execute";
			GiveItem2Character(pchar, "sicretLetter");
			ref rItem = ItemsFromID("sicretLetter");
			rItem.City = XI_ConvertString("Colony"+characters[GetCharacterIndex(pchar.GenQuest.Intelligence.MayorId)].city+"Gen")); 
			rItem.Mayor = GetFullName(characterFromId(pchar.GenQuest.Intelligence.MayorId)); 
			AddQuestRecord("MayorsQuestsList", "7");
			AddQuestUserData("MayorsQuestsList", "ColonyName", XI_ConvertString("Colony"+characters[GetCharacterIndex(pchar.GenQuest.Intelligence.MayorId)].city+"Gen"));
			AddQuestUserData("MayorsQuestsList", "MayorName", GetFullName(characterFromId(pchar.GenQuest.Intelligence.MayorId)));
			AddQuestUserData("MayorsQuestsList", "sCity", XI_ConvertString("Colony"+pchar.GenQuest.Intelligence.City+"Dat"));
			AddQuestUserData("MayorsQuestsList", "sWho", GetWorkTypeOfMan(&characters[GetCharacterIndex(pchar.GenQuest.Intelligence.SpyId)], "Gen"));
			AddQuestUserData("MayorsQuestsList", "SpyName", GetFullName(&characters[GetCharacterIndex(pchar.GenQuest.Intelligence.SpyId)]));			
		break;
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////		Квест Асокольда, рендомные базары
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		case "AscoldTrader": //рендомный хозяин магаза
            dialog.Text = "Послушай, не встречал еще порошка мумии, который я просил тебя разыскать?";
            if (GetQuestPastDayParam("questTemp.Ascold") > 3)
			{
				Link.l1 = "Нет, пока не встречал... Послушай, у торговцев на улицах он, кажется, есть, но что-то они темнят...";
            	Link.l1.go = "AscoldTrader_6";
			}
			else
			{
				Link.l1 = "Нет, пока не встречал... Но я продолжу поиски.";
            	Link.l1.go = "quests";
			}
		break;
        case "AscoldTrader_WasSeek":
            dialog.Text = "Послушай, не встречал еще порошка мумии, который я просил тебя разыскать?";
            Link.l2 = "Да, нашел. И сколько ты мне за него заплатишь?";
            Link.l2.go = "AscoldTrader_1";
        break;		
		case "AscoldTrader_1":
        	dialog.text = "Я - нисколько. Но я знаю, кто купит у тебя это.";
    		link.l1 = "Хм, я расcчитывал на получение денег прямо сейчас.";
    		link.l1.go = "AscoldTrader_2";
		break;
 		case "AscoldTrader_2":
        	dialog.text = "Полагаю, ты не останешься в накладе. Слушай внимательно.\n"+
                          "На Гваделупе в городе Бас Тер живет один странный тип - Аскольд.";
            if (Characters[GetCharacterIndex("Ascold")].quest.meeting == "0")
            {
                link.l1 = "Странное имя...";
                link.l1.go = "AscoldTrader_4";
            }
            else
            {
                link.l1 = "Знаю я его, довелось поговорить. Наглец, каких поискать...";
                link.l1.go = "AscoldTrader_3";
            }
		break;
 		case "AscoldTrader_3":
        	dialog.text = "А что случилось? Испытал на себе его крутой нрав?";
    		link.l1 = "Да уж, он просто выставил меня за дверь как мальчишку!";
    		link.l1.go = "AscoldTrader_4";
		break;
 		case "AscoldTrader_4":
        	dialog.text = "Аскольд немного резковат, но человек он хороший, честный. По слухам - московит, есть такой народ, который живет на севере Старого Света, где круглый год снег, а вокруг одни леса да медведи. Оттого и характер суровый, а силы в руках хватит на четверых здоровенных молодцов.\n"+
                          "Как и когда он оказался здесь, я точно не знаю, как и не знаю, чем он занимается.\n"+
                          "Но мне он доверяет, так как я имею с ним торговые дела.\n"+
                          "Так что поезжай к нему, представишься ему от меня и все будет в порядке. Купит он у тебя эту дрянь, так как его заказ.";
    		link.l1 = "А тебе с этого какой доход?";
    		link.l1.go = "AscoldTrader_5";
		break;
 		case "AscoldTrader_5":
        	dialog.text = "За меня не переживай, я удержу нужную сумму комиссионных из нашей с ним текущей оборотки.";
    		link.l1 = "Понятно. Что ж, пожалуй, загляну к твоему Аскольду при случае.";
    		link.l1.go = "exit";
    	    pchar.questTemp.Ascold = "GoToAscold";
    	    QuestSetCurrentNode("Ascold", "HaveRecomendation");
            AddQuestRecord("Ascold", "2");
		break;
 		case "AscoldTrader_6":
        	dialog.text = "Небойсь, и Инквизицией грозятся?";
    		link.l1 = "Точно, грозятся...";
    		link.l1.go = "AscoldTrader_7";
		break;
 		case "AscoldTrader_7":
        	dialog.text = "Ну а как ты хотел?.. Вот что я тебе скажу, приятель - порошок есть у них. Не у всех, конечно, но у кого-то точно он имеется. Поэтому ты будь настойчивей с ними - и в итоге добьешься своего.";
    		link.l1 = "Хех, ну что же, спасибо за помощь.";
    		link.l1.go = "quests";
			SaveCurrentQuestDateParam("questTemp.Ascold");
		break;
        //==> Общий диалог для хозяев верфи, квест Аззи.
		case "AzzyTryTalk":
			dialog.Text = NPCStringReactionRepeat("Давай посмотрим на твою жестянку. Э-э-э, нет. Ты посмотри на нее, она сплошь покрыта странными письменами, не иначе это колдовство. Советую тебе отнести эту вещь в церковь, там разберуться, что с ней делать.", "none", "none", "none", "block", 0, npchar, Dialog.CurrentNode);
  			Link.l1 = HeroStringReactionRepeat("Спасибо за добрый совет.", "none", "none", "none", npchar, Dialog.CurrentNode);
			Link.l1.go = "Exit";
		break;
		case "AggryHelp":
			dialog.Text = NPCStringReactionRepeat("Вот там на столе лежит инструмент: паяльная лампа, ножовка по металлу, ножницы для жести и прочее. Бери и открывай сам, а денег мне не надо.", "none", "none", "none", "block", 0, npchar, Dialog.CurrentNode);
			Link.l10 = HeroStringReactionRepeat("Спасибо, " + npchar.name + ".", "none", "none", "none", npchar, Dialog.CurrentNode);
			Link.l10.go = "exit";
		break;
		//==> Общий диалог для церкви, квест Аззи.
        case "GiveLamp":
			dialog.text = "Ты правильно поступил, сын мой. Неизвестно, что там внутри, но по всему видно, что завтра церковь нужно будет освящать заново.\n"+
                          "Я не хочу спрашивать, где ты это взял. Именем Спасителя прощаю тебе грехи твои! Иди с миром.";
			link.l1 = "Спасибо, Святой Отец.";
			link.l1.go = "exit";
            ChangeCharacterReputation(pchar, 100);
            TakeItemFromCharacter(pchar, "Azzy_bottle");
		break;
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////		Грабеж среди бела дня, попытка залезть в сундуки
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        case "Man_FackYou":
			dialog.text = LinkRandPhrase("Грабеж среди бела дня!!! Это что же такое делается?! Ну погоди, приятель...", "Эй, ты чего это там копаешься?! Никак, вздумал ограбить меня? Ну, тогда тебе конец...", "Постой, ты куда это полез? Да ты вор, оказывается! Ну, считай, что ты приплыл, родной...");
			link.l1 = LinkRandPhrase("Дьявол!!", "Каррамба!!", "А-ать, черт!");
			link.l1.go = "fight_owner";
		break;
		case "fight_owner":
			LAi_SetOwnerTypeNoGroup(NPChar);
			LAi_group_Attack(NPChar, Pchar);
			if (rand(3) != 1) SetNationRelation2MainCharacter(sti(npchar.nation), RELATION_ENEMY);
			NextDiag.CurrentNode = "First time";
			DialogExit();
		break;
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////		Квест получения Синей птицы
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		case "RBlueBird":
			dialog.text = "Эх, капитан, не случилось, а случается уже давно. Мы, торговцы Карибского моря, на грани разорения. А во всем виновата проклятая 'Синяя Птица'...";
			link.l1 = "'Синяя Птица'?";
			link.l1.go = "RBlueBird_1";
		break;
		case "RBlueBird_1":
			dialog.text = "Да, это чертова щебека, единственный корабль такого типа здесь, уйти от него в море невозможно. Грабит только корабли торговцев. Большие караваны, усиленные военными кораблями, не трогает.";
			link.l1 = "А почему вы сказали, что торговцы разоряются? Ведь товары все равно поступают сюда с этими караванами.";
			link.l1.go = "RBlueBird_2";
		break;
		case "RBlueBird_2":
			dialog.text = "Поступают, верно. Но все дело в том, что оборот при таком раскладе становится невыгодным. Лучше всего иметь свой шлюп или флейт, самим закупать товары в Старом Свете и везти сюда. Вот это - торговля!\nА когда мы закупаем товары у перевозчиков - тут уже не до прибылей, речь идет о простом выживании.";
			link.l1 = "Понятно. И как раз ваши корабли берет на абордаж щебека...";
			link.l1.go = "RBlueBird_3";
		break;
		case "RBlueBird_3":
			dialog.text = "Именно так. Уже почти все торговцы потеряли собственные корабли, черт ее дери, эту 'Синюю Птицу'!";
			link.l1 = "М-да, печально для вас... Ну, так что же вы хотите от меня?";
			link.l1.go = "RBlueBird_4";
		break;
		case "RBlueBird_4":
			dialog.text = "Мы хотим, чтобы вы уничтожили эту сизокрылую выхухоль, мать ее перемать!!!";
			link.l1 = "И что я буду с этого иметь?";
			link.l1.go = "RBlueBird_5";
		break;
		case "RBlueBird_5":
			dialog.text = "Я соберу со всех коллег достойную оплату для вас за эту работу. Пятьдесят тысяч пиастров я могу вам твердо обещать. Чертова щебека должна пойти на дно!";
			link.l1 = "За такие деньги, приятель, я и пальцем не шевельну. Ищи дурака!";
			link.l1.go = "RBlueBird_close";
			link.l2 = "Хм, эти деньги меня интересуют. Я готов взяться за это дело. Каковы сроки выполнения?";
			link.l2.go = "RBlueBird_6";
		break;
		case "RBlueBird_6":
			dialog.text = "Вы не ограниченны во времени. Я прекрасно понимаю, что сделать это будет нелегко, черт возьми! Сказать по правде, мы уже привлекали наемных убийц к этому делу, хотели убрать капитана. Безрезультатно...";
			link.l1 = "Вот как? Вы можете подробней об этом рассказать?";
			link.l1.go = "RBlueBird_7";
		break;
		case "RBlueBird_7":
			dialog.text = "Ну, скинулись мы и наняли кое-кого, чтобы он разыскал кэпа этой щебеки среди пиратов. Ничего не вышло, наш наймит с полгода рыскал по Карибам, но ничего не нашел. Черт его знает, где они базируются и куда сбывают награбленное...";
			link.l1 = "Та-а-а-ак, приятель, это уже что-то. А теперь послушай мои условия сделки.";
			link.l1.go = "RBlueBird_8";
		break;
		case "RBlueBird_8":
			dialog.text = "Внимательно слушаю, " + GetAddress_Form(NPChar) + "." ;
			link.l1 = "Эта сделка между тобой и мной, деньги собирай как хочешь, но никому не болтай, что я взялся за это дело. И второе. Ты сказал, что потеряли свои корабли ПОЧТИ все торговцы. Что значит 'почти'?";
			link.l1.go = "RBlueBird_9";
		break;
		case "RBlueBird_9":
			dialog.text = "У одного торговца остался флейт. Но он счастливчик, ни разу не попадал на эту щебеку. Он - наша единственная надежда закрепится здесь, уже все наши ему должны. Если бы он не поддерживал нас в трудную минуту - всем давно пришел бы конец. Он еще никому не отказывал в просьбах дать денег в долг!";
			link.l1 = "Понятно. Почти святой! И как же его зовут?";
			link.l1.go = "RBlueBird_10";
		break;
		case "RBlueBird_10":
			dialog.text = "Паскаль Вуазье. Он содержит лавку на Бермудах в пиратском поселении. Так вы беретесь за дело?";
			link.l1 = "Вы принимаете мое условие о молчании?";
			link.l1.go = "RBlueBird_11";
		break;
		case "RBlueBird_11":
			dialog.text = "Безусловно!";
			link.l1 = "Тогда берусь. Я начинаю поиски с этого дня, надеюсь вскоре закончить.";
			link.l1.go = "RBlueBird_12";
		break;
		case "RBlueBird_12":
			dialog.text = "Эх, вашими бы устами, да мед пить! Ну что же, приступайте. Очень надеюсь, что все будет проходить так, как вы сказали.";
			link.l1 = "Будет, можете не сомневаться!";
			link.l1.go = "exit";
			pchar.questTemp.BlueBird = "toBermudes";
			pchar.questTemp.BlueBird.traiderId = npchar.id; //запомним Id торговца
			SetQuestHeader("Xebeca_BlueBird");
			AddQuestRecord("Xebeca_BlueBird", "1");
			AddQuestUserData("Xebeca_BlueBird", "sCity", XI_ConvertString("Colony" + npchar.city + "Gen"));
			AddQuestUserData("Xebeca_BlueBird", "sName", GetFullName(npchar));
		break;


		case "RBlueBird_close":
			dialog.text = "Ну что же, очень жаль. Пожалуй, мы поищем 'дурака' в другом месте...";
			link.l1 = "Гм...";
			link.l1.go = "exit";
			pchar.questTemp.BlueBird = "over";
		break;
		case "RBlueBirdWon":
			dialog.text = "Прекрасная новость, скажу я вам! Наконец-то мы сможем заняться торговлей по-настоящему... Вы хотите получить деньги прямо сейчас?";
			link.l1 = "Да, желательно...";
			link.l1.go = "RBlueBirdWon_1";
			int chComp;
			for (int i=0; i<=COMPANION_MAX; i++)
        	{
                chComp = GetCompanionIndex(pchar, i);
                if(chComp != -1 && RealShips[sti(characters[chComp].ship.type)].Name == "XebekVML1")
        		{	
					dialog.text = "Хм, подождите... Насколько мне известно, вы сумели настигнуть щебеку, но не потопили ее! Теперь она находится в вашем владении.";
					link.l1 = "Да, это так. Ну и в чем проблема?";
					link.l1.go = "RBlueBirdWon_10";
					break;
                }
            }
		break;

		case "RBlueBirdWon_1":
			dialog.text = "Без проблем, извольте получить.";
			link.l1 = "Спасибо, мы в рассчете. Желаю всего наилучшего... Кстати, а вы не хотите узнать, кто стоял за всем этим?";
			link.l1.go = "RBlueBirdWon_2";
			AddMoneyToCharacter(pchar, 50000);
		break;
		case "RBlueBirdWon_2":
			dialog.text = "Хм, очень будет интересно узнать...";
			link.l1 = "Это самый 'святой' торговец в Карибском море - Паскаль Вуазье.";
			link.l1.go = "RBlueBirdWon_3";
		break;
		case "RBlueBirdWon_3":
			dialog.text = "Не может быть!..";
			link.l1 = "Может. Доказательств, к сожалению, у меня нет, но вы имейте ввиду, кто ссужает вам ваши же деньги.";
			link.l1.go = "RBlueBirdWon_4";
		break;
		case "RBlueBirdWon_4":
			dialog.text = "Хм, все это нужно проверить... Ну что же, спасибо вам за работу и за информацию.";
			link.l1 = "Да не за что, это вами оплачено. Прощайте.";
			link.l1.go = "exit";
			CloseQuestHeader("Xebeca_BlueBird");
			pchar.questTemp.BlueBird = "over";
		break;

		case "RBlueBirdWon_10":
			dialog.text = "Проблема в том, что этот проклятый корабль теперь у вас в руках. А вы, кстати, тоже пират! И кто вам помешает грабить торговцев точно также, как это делал предыдущий хозяин?";
			link.l1 = "Хм, вопрос, конечно, интересный... Однако, я выполнил работу!";
			link.l1.go = "RBlueBirdWon_11";
		break;
		case "RBlueBirdWon_11":
			dialog.text = "Нет, вы ее не выполнили. Мы договаривались о потоплении 'Синей Птицы', а не об абордаже.";
			link.l1 = "Хм, вот значит как! Ну что же, я вас понял... Всего наилучшего, передавайте мой пламенный привет вашим коллегам.";
			link.l1.go = "RBlueBirdWon_12";
		break;
		case "RBlueBirdWon_12":
			dialog.text = "Передам всенепременно! Прощайте, и впредь держите свое слово 'от' и 'до'.";
			link.l1 = "Обязательно. Но я не прощаюсь...";
			link.l1.go = "exit";
			AddQuestRecord("Xebeca_BlueBird", "9");
			pchar.questTemp.BlueBird = "returnMoney";
			pchar.questTemp.BlueBird.City = GetSharpCity();
		break;

		case "RBlueBirdDieHard":
			dialog.text = "Очень и очень жаль. Ну что же, вы не первый, кто берется за это дело, и у кого ничего не выходит. Плохо, придется нам и далее прозябать...";
			link.l1 = "Надеюсь, все придет в норму со временем...";
			link.l1.go = "RBlueBirdDieHard_1";
		break;
		case "RBlueBirdDieHard_1":
			dialog.text = "Это уж точно. Вот живем тут в суете, и наивно полагаем, что именно вокруг нас земля вертится. Ан нет, из земли взято, в землю и уйдем\nХех, что-то меня на лирику потянуло.";
			link.l1 = "Бывает... Ну что же, прощайте.";
			link.l1.go = "exit";
			CloseQuestHeader("Xebeca_BlueBird");
			pchar.questTemp.BlueBird = "over";
		break;

		case "RBlueBird_retMoney_1":
			dialog.text = "Ах ты мерзавец! Значит, наши опасения оказались верными!!";
			link.l1 = "Я бы вас не трогал, если бы вы не начали жульничать!";
			link.l1.go = "RBlueBird_retMoney_2";
		break;
		case "RBlueBird_retMoney_2":
			dialog.text = "А-а-ать, черт! Стража, хватай его!! Далеко тебе не уйти, мерзавец...";
			link.l1 = "Лучше бы вернули мои деньги, проныры. Будем лечить вас дальше...";
			link.l1.go = "fight";
		break;

		case "RBlueBird_retMoney_3":
			dialog.text = "Будь ты проклят, мерзавец! Как я жалею, что связался с тобой!! До тебя 'Синяя Птица' никогда не доставляла нам СТОЛЬКО проблем!..";
			link.l1 = "Друзья мои, вы сами виноваты, нужно было держаться договора.";
			link.l1.go = "RBlueBird_retMoney_4";
		break;
		case "RBlueBird_retMoney_4":
			dialog.text = "Какого договора, нашего с вам? Мы его неукоснительно придерживались! Это вы его нарушили, не потопив щебеку!";
			link.l1 = "Мон шер, не надо так кричать, это пагубно отражается на голосовых связках. А вдруг, не сможете потом разговаривать? В общем, я готов говорить с вами только в умеренных тонах...";
			link.l1.go = "RBlueBird_retMoney_5";
		break;
		case "RBlueBird_retMoney_5":
			dialog.text = "Ну хорошо, хотя тон здесь ни при чем. Это вы нарушили условия сделки, а не мы - вот суть вопроса!";
			link.l1 = "Хех! Вы, торговцы, большие мастера эдаких заковык в договорах. Знаю я ваши штучки. Где-то запятую не поставил, и смысл текста сразу меняется.";
			link.l1.go = "RBlueBird_retMoney_6";
		break;
		case "RBlueBird_retMoney_6":
			dialog.text = "При чем здесь запятые? У нас с вами была устная договоренность.";
			link.l1 = "Ну, это я образно выражаюсь. Смысл в том, что вы зацепились за возможность кинуть меня на бабки, и сделали это. А я пират, мне недосуг разбираться в двойных смыслах ваших условий.";
			link.l1.go = "RBlueBird_retMoney_7";
		break;
		case "RBlueBird_retMoney_7":
			dialog.text = "Каких условий?";
			link.l1 = "Условий наших договоренностей. Вам мешала 'Синяя Птица' - я решил эту проблему. Если бы вы отдали мне мои деньги, то я вас и пальцем бы не тронул. Так что все ваши проблемы от вашей мудрости, так сказать...";
			link.l1.go = "RBlueBird_retMoney_8";
		break;
		case "RBlueBird_retMoney_8":
			dialog.text = "Ну ладно, забирайте ваши 50 тысяч и убирайтесь отсюда!";
			link.l1 = "Не так быстро, мон шер. Все дело в том, что я понес дополнительные финансовые издержки, пытаясь вразумить вас. Не говоря уже о моральном ущербе. Поэтому сумма выросла.";
			link.l1.go = "RBlueBird_retMoney_9";
		break;
		case "RBlueBird_retMoney_9":
			dialog.text = "И что это за новая сумма? Назовите ее.";
			link.l1 = "Увеличьте ее в четыре раза. 200 тысяч, и я оставлю вас в покое. Иначе - разорю. А потом убью. Сначала вашу семью, а потом вас...";
			link.l1.go = "RBlueBird_retMoney_10";
		break;
		case "RBlueBird_retMoney_10":
			dialog.text = "Да ты - дьявол!!";
			link.l1 = "И очень, очень злой дьявол...";
			link.l1.go = "RBlueBird_retMoney_11";
		break;
		case "RBlueBird_retMoney_11":
			dialog.text = "Хорошо. Вот твои деньги. А теперь убирайся! И оставь нас в покое!";
			link.l1 = "Всенепременно. Теперь, когда вы рассчитались со мной, 'Синяя Птица' вам более не угрожает. Прощайте, удачи в делах коммерческих...";
			link.l1.go = "exit";
			DeleteAttribute(pchar, "questTemp.BlueBird");
			pchar.questTemp.BlueBird = "over"; //закрываем квест
			AddMoneyToCharacter(pchar, 200000);
			AddQuestRecord("Xebeca_BlueBird", "14");
			CloseQuestHeader("Xebeca_BlueBird");
		break;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////		Линейка острова погибших кораблей ISS
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		case "ISS_deposit":
			dialog.text = NPCStringReactionRepeat("Совершенно верно. Один миллион пиастров мы можете получить через минуту после получения мной подтверждения.", 
				"Я уже говорил вам, " + GetAddress_Form(NPChar) + ", что депозит у меня. Я жду подтверждения на его выдачу.", 
				"Вам не стоит беспокоится. Депозит находится у меня, я жду только подтверждения на его выдачу.",
				"Хм, мне кажется, вы черезмерно обеспокоены этим депозитом. Моя репутация безупречна, поэтому я могу только повторить ранее сказанное. Депозит на ваше имя находится у меня, получить вы его сможете после подтверждения на выдачу.", "block", 0, npchar, Dialog.CurrentNode);
			link.l1 = HeroStringReactionRepeat("Понятно. Ну что же, прекрасно. Я узнал то, что хотел.", 
				"Ага, понятно...",
				"Я и не беспокоюсь особо.", 
				"Понятно... Спасибо вам, " + GetAddress_Form(NPChar) + ".", npchar, Dialog.CurrentNode);
			link.l1.go = "exit";
		break;
		case "ISS_takeDeposit":
			dialog.text = "Да-да, я получил разрешение провести эту финансовую операцию. Извольте получить, " + GetAddress_Form(NPChar) + ".";
			link.l1 = "Хе-хе, очень хорошо...";
			link.l1.go = "ISS_takeDeposit_1";
			AddMoneyToCharacter(pchar, 1000000);
			CloseQuestHeader("ISS_PoorsMurder");
		break;
		case "ISS_takeDeposit_1":
			dialog.text = "Быть может, вы решите пустить эти деньги в рост? Я всегда к вашим услугам, моя репутация...";
			link.l1 = "Я понял вас, спасибо. Я подумаю.";
			link.l1.go = "exit";
			pchar.questTemp.LSC = "over"; //конец линейки ГПК
			CloseQuestHeader("ISS_PoorsMurder");
		break;

	}
}
