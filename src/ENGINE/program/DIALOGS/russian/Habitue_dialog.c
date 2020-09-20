#include "DIALOGS\russian\Common_Duel.c" //navy
#include "DIALOGS\russian\Rumours\Common_rumours.c"  //homo 25/06/06
void ProcessDialogEvent()
{
    ref NPChar, d;
	aref Link, Diag;
	string NPC_Meeting, TempInfoCharacterID_1, TempInfoCharacterID_2;
	int DonationSize;
    	
	DeleteAttribute(&Dialog,"Links");

	makeref(NPChar,CharacterRef);
	makearef(Link, Dialog.Links);
	makeref(d, Dialog);
	makearef(Diag, NPChar.Dialog);
	
    ProcessCommonDialogRumors(NPChar, Link, Diag);//homo 25/06/06
    ProcessDuelDialog(NPChar, Link, Diag); //navy

	int iEnemyDice, iPCDice;
	// boal 13.03.2004 -->
	int d1,d2,d3,d4;
	bool ok;
	// boal 13.03.2004 <--
    ref sld;
    
	if (npchar.quest.Meeting != LastSpeakDate())
	{
		npchar.quest.last_theme = makeint(rand(1));
		npchar.quest.last_theme_game = makeint(rand(1));
		npchar.quest.Meeting = LastSpeakDate();
		npchar.money = 100 + rand(10) * makeint(50 * sti(PChar.rank) * (10.0 / MOD_SKILL_ENEMY_RATE));
        if (bBettaTestMode)
        {
            Dialog.CurrentNode = "Beta_test";
        }
	}
	
	switch (Dialog.CurrentNode)
	{
		// -----------------------------------Диалог первый - первая встреча
		
		case "exit":
			Diag.CurrentNode = Diag.TempNode;
			DialogExit();
		break;

        case "Beta_test":
            dialog.text = "Это меню выбора типа пьяни (бетатест)";
			link.l1 = "Пусть будет алкаш";
			link.l1.go = "Beta_test_alc";
            link.l2 = "Игрок в карты";
			link.l2.go = "Beta_test_card";
			link.l3 = "Игрок в кости";
			link.l3.go = "Beta_test_dice";
		break;
		
		case "Beta_test_alc":
            dialog.text = "Алкаш выбран";
			link.l1 = "Продолжить";
			link.l1.go = "First time";
			npchar.quest.last_theme = 0;
		    npchar.quest.last_theme_game = 0;
		break;
		
		case "Beta_test_card":
            dialog.text = "Игрок в карты (30%% что пошлет с первого раза)";
			link.l1 = "Продолжить";
			link.l1.go = "First time";
			npchar.quest.last_theme = 1;
		    npchar.quest.last_theme_game = 0;
		break;
		
		case "Beta_test_dice":
            dialog.text = "Игрок в кости (30%% что пошлет с первого раза)";
			link.l1 = "Продолжить";
			link.l1.go = "First time";
			npchar.quest.last_theme = 1;
		    npchar.quest.last_theme_game = 1;
		break;
		
		case "First time":
			if (LAi_grp_playeralarm > 0)
			{
       			dialog.text = LinkRandPhrase("Я, может, и пьян, но в своем уме. Сейчас вот выпью с тобой, а солдаты и меня загребут! Не-е-ет...", "Я не враг городу, в котором живу. Не буду с тобой пить!" , "Приятель, тебе лучше бы убраться отсюда подобру-поздорову...");
				link.l1 = RandPhraseSimple("Ну как знаешь...", "Хех, больно надо глаза заливать! Не то время."); 
				link.l1.go = "exit";
				break;
			}
            ok = CheckFreeSitFront(npchar); //checkAttribute(npchar, "Default.ToLocator"); // есть куда сесть
            
            dialog.text = "Не загораживай мне свет, мух в кружке не видно!";
            link.l1 = "Уже отошёл...";
		    link.l1.go = "exit";
		    if (!chrDisableReloadToLocation)
		    {
	            switch (npchar.quest.last_theme)
				{
					case "0": //пьяный.
						dialog.text = "И-ик! О, парень, ты выглядишь бывалым морским волком! Может, купишь мне стаканчик рома?";
						link.l1 = "Может я и морской волк, только это не значит, что я буду поить всякую рвань...";
						link.l1.go = "exit";

						if (makeint(pchar.money) >= 100 && ok) // только старые острова окучены на сесть
						{
							link.l2 = "Я и сам с радостью присоединюсь к тебе, за компанию.";
							link.l2.go = "talk_with_alchogol";
						}
						else
						{
	                        link.l2 = "Это еще почему?";
							link.l2.go = "talk_with_alchogol_song";
						}
					break;

					case "1":
	                    if (rand(3) != 1 && ok)
	                    {
							switch (sti(npchar.quest.last_theme_game))
							{
							    case 0 :
									// Игра в карты boal 20/05/05 -->
	                                if (CheckNPCQuestDate(npchar, "Card_date_begin"))
									{
										Dialog.text = RandPhraseSimple("Сыграем в карты, очень интересная игра?", "Предлагаю перекинуться в картишки. Ты как?");
										link.l1 = "Отчего же не сыграть? Давай сыграем.";
										link.l1.go = "Cards_begin";
										link.l2 = "А правила у игры какие?";
										link.l2.go = "Cards_Rule";
										link.l3 = "Не сейчас.";
										link.l3.go = "exit";
									}
									else
					    			{
										Dialog.text = "Нет, не хочу я больше играть с тобой в карты.";
						    			link.l1 = "Ну и не надо.";
						    			link.l1.go = "exit";
									}
			                        // Игра в карты boal 20/05/05 <--
		                        break;

								case 1:
								// кости
		    						// Игра в кости boal 13/07/05 -->
	                                if (CheckNPCQuestDate(npchar, "Dice_date_begin"))
									{
										Dialog.text = RandPhraseSimple("Сыграем в кости, очень интересная игра?", "Предлагаю постучать костяшками. Ты как?");
										link.l1 = "Отчего же не сыграть? Давай сыграем.";
										link.l1.go = "Dice_begin";
										link.l2 = "А правила у игры какие?";
										link.l2.go = "Dice_Rule";
										link.l3 = "Не сейчас.";
										link.l3.go = "exit";
									}
									else
					    			{
										Dialog.text = "Нет, не хочу я больше играть с тобой в кости.";
					    				link.l1 = "Ну и не надо.";
					    				link.l1.go = "exit";
									}
			                        // Игра в кости boal 13/07/05 <--
			                        //navy --> Дуэли
									if (CheckAttribute(NPchar, "Quest.DiceCheats") && sti(NPchar.Quest.DiceCheats) >= 1) // && sti(NPChar.Quest.HeroLose))
									{
					    				link.l9 = RandSwear() + " Ты жульничал!!!";
					    				link.l9.go = "outraged";
									}
									//navy <--
			    				break;
	    					} //switch
						}
	        			else
	        			{
	                        dialog.text = RandPhraseSimple("Убирайся от моего стола ко всем чертям!", " А.. чего? Кто это?.. и-ик.. Отвали!");
	                        link.l1 = RandPhraseSimple("Пьянство губит твою грешную душу - опомнись!", "Не нужно мне грубить.");
	        			    link.l1.go = "exit";
	        			    link.l2 = RandPhraseSimple("Как ты со мной разговариваешь, скотина! Сейчас я научу тебя хорошим манерам.", "Вот я тресну тебя сейчас по башке, вмиг протрезвеешь.");
	        	            link.l2.go = "tavern_keeper";
	        			}
					break;
				}
			}
			Diag.TempNode = "First time";
	    break;
	    
	    // карты -->
	    case "Cards_Rule":
   			dialog.text = CARDS_RULE;
			link.l1 = "Что ж, давай начнем!";
			link.l1.go = "Cards_begin";
			link.l3 = "Нет, это не для меня...";
			link.l3.go = "exit";
		break;
		
		case "Cards_begin":
			Diag.tempnode = "Cards_Node";
			Diag.CurrentNode = Diag.TempNode;
			DialogExit();
			pchar.questTemp.friend_in_tavern = npchar.id;
			AddDialogExitQuest("alc");
		break;

		case "Cards_Node":
			Diag.tempNode = "first time";

			Dialog.text = "Давай определимся со ставкой.";
			link.l1 = "Играем по 100 монет";
			link.l1.go = "Cards_Node_100";
			link.l2 = "Давай по 500 золотых";
			link.l2.go = "Cards_Node_500";
			link.l3 = "Пожалуй, мне пора.";
			link.l3.go = "exit_sit";
		break;
		
		case "Cards_Node_100":
            if (!CheckCardsGameSmallRate())
		    {
                dialog.text = "О тебе ходит слава непревзойденного шулера. Я не буду с тобой играть в карты вообще.";
                link.l1 = "Все врут! Ну и не нужно.";
			    link.l1.go = "exit_sit";
			    break;
		    }
		    
			if (sti(pchar.Money) < 300)
		    {
                dialog.text = "Да ты, дружок, на мели. Разбогатеешь, приходи.";
                link.l1 = "Ладно.";
			    link.l1.go = "exit_sit";
			    break;
		    }
		    if (sti(npchar.Money) < 300)
		    {
                dialog.text = "Я уже проигрался в пух и прах, не мой день.";
                link.l1 = "Ладно.";
			    link.l1.go = "exit_sit";
			    break;
		    }
   			dialog.text = "Хорошо, играем по 100 монет.";
			link.l1 = "Сдавай!";
			link.l1.go = "Cards_begin_go";
			pchar.GenQuest.Cards.npcharIdx = npchar.index;
            pchar.GenQuest.Cards.iRate     = 100;
            pchar.GenQuest.Cards.SitType   = true;
		break;
		
		case "Cards_Node_500":
            if (!CheckCardsGameSmallRate())
		    {
                dialog.text = "О тебе ходит слава непревзойденного шулера. Я не буду с тобой играть в карты вообще.";
                link.l1 = "Все врут! Ну и не нужно.";
			    link.l1.go = "exit_sit";
			    break;
		    }
			if (!CheckCardsGameBigRate())
		    {
                dialog.text = "Я слышал, что ты очень хорошо играешь. Я не буду играть с тобой по таким большим ставкам.";
                link.l1 = "Давай по более низким?";
			    link.l1.go = "Cards_Node_100";
				link.l2 = "Ну и не нужно.";
			    link.l2.go = "exit_sit";
			    break;
		    }
		    
			if (sti(pchar.Money) < 1500)
		    {
                dialog.text = "Да ты, дружок, на мели. Разбогатеешь, приходи.";
                link.l1 = "Ладно.";
			    link.l1.go = "exit_sit";
			    break;
		    }
		    if (sti(npchar.Money) < 1500)
		    {
                dialog.text = "Я уже проигрался для таких больших ставок, явно не мой день.";
                link.l1 = "Жаль.";
			    link.l1.go = "exit_sit";
			    break;
		    }
   			dialog.text = "Хорошо, играем по 500 монет.";
			link.l1 = "Сдавай!";
			link.l1.go = "Cards_begin_go";
			pchar.GenQuest.Cards.npcharIdx = npchar.index;
            pchar.GenQuest.Cards.iRate     = 500;
            pchar.GenQuest.Cards.SitType   = true;
		break;
		
		case "Cards_begin_go":
            // анлим игры в день SetNPCQuestDate(npchar, "Card_date_begin");
			Diag.CurrentNode = Diag.TempNode;
			DialogExit();
            LaunchCardsGame();
		break;
	    // карты <--
	    // КОСТИ -->
	    case "Dice_Rule":
   			dialog.text = DICE_RULE;
			link.l1 = "Что ж, давай начнем!";
			link.l1.go = "Dice_begin";
			link.l3 = "Нет, это не для меня...";
			link.l3.go = "exit";
		break;

		case "Dice_begin":
			Diag.tempnode = "Dice_Node";
			Diag.CurrentNode = Diag.TempNode;
			DialogExit();
			pchar.questTemp.friend_in_tavern = npchar.id;
			AddDialogExitQuest("alc");
		break;

		case "Dice_Node":
		    Diag.tempNode = "first time";
		    
			Dialog.text = "Давай определимся со ставкой.";
			link.l1 = "Играем по 50 монет за кубик";
			link.l1.go = "Dice_Node_100";
			link.l2 = "Давай по 200 золотых за кубик";
			link.l2.go = "Dice_Node_500";
			link.l3 = "Пожалуй, мне пора.";
			link.l3.go = "exit_sit";
		break;

		case "Dice_Node_100":
            if (!CheckDiceGameSmallRate())
		    {
                dialog.text = "О тебе ходит слава непревзойденного шулера. Я не буду с тобой играть в кубики вообще.";
                link.l1 = "Все врут! Ну и не нужно.";
			    link.l1.go = "exit_sit";
			    break;
		    }

			if (sti(pchar.Money) < 300)
		    {
                dialog.text = "Да ты, дружок, на мели. Разбогатеешь, приходи.";
                link.l1 = "Ладно.";
			    link.l1.go = "exit_sit";
			    break;
		    }
		    if (sti(npchar.Money) < 300)
		    {
                dialog.text = "Я уже проигрался в пух и прах, не мой день.";
                link.l1 = "Ладно.";
			    link.l1.go = "exit_sit";
			    break;
		    }
   			dialog.text = "Хорошо, играем по 50 монет.";
			link.l1 = "Поехали!";
			link.l1.go = "Dice_begin_go";
			pchar.GenQuest.Dice.npcharIdx = npchar.index;
            pchar.GenQuest.Dice.iRate     = 50;
            pchar.GenQuest.Dice.SitType   = true;
		break;

		case "Dice_Node_500":
            if (!CheckDiceGameSmallRate())
		    {
                dialog.text = "О тебе ходит слава непревзойденного шулера. Я не буду с тобой играть в кости вообще.";
                link.l1 = "Все врут! Ну и не нужно.";
			    link.l1.go = "exit_sit";
			    break;
		    }
			if (!CheckDiceGameBigRate())
		    {
                dialog.text = "Я слышал, что ты очень хорошо играешь. Я не буду играть с тобой по таким большим ставкам.";
                link.l1 = "Давай по более низким?";
			    link.l1.go = "Dice_Node_100";
				link.l2 = "Ну и не нужно.";
			    link.l2.go = "exit_sit";
			    break;
		    }

			if (sti(pchar.Money) < 1500)
		    {
                dialog.text = "Да ты, дружок, на мели. Разбогатеешь, приходи.";
                link.l1 = "Ладно.";
			    link.l1.go = "exit_sit";
			    break;
		    }
		    if (sti(npchar.Money) < 1500)
		    {
                dialog.text = "Я уже проигрался для таких больших ставок, явно не мой день.";
                link.l1 = "Жаль.";
			    link.l1.go = "exit_sit";
			    break;
		    }
   			dialog.text = "Хорошо, играем по 200 монет.";
			link.l1 = "Поехали!";
			link.l1.go = "Dice_begin_go";
			pchar.GenQuest.Dice.npcharIdx = npchar.index;
            pchar.GenQuest.Dice.iRate     = 200;
            pchar.GenQuest.Dice.SitType   = true;
		break;

		case "Dice_begin_go":
            // анлим игры в день SetNPCQuestDate(npchar, "Dice_date_begin");
			Diag.CurrentNode = Diag.TempNode;
			DialogExit();
            LaunchDiceGame();
		break;
	    // КОСТИ <--
	    // homo 01/08/06 to_del
        // наводка на товар - перенес в слухи
	    //case "Find_Merchant_1":
            //if (pchar.GenQuest.Find_Merchant.lastspeak_date == LastSpeakDate())
            //{
            //    dialog.text = "А че это ты интересуешся? И-ик! Уж не пират ли ты?";
    		//	link.l1 = "Забудь! Тебе послышалось.";
    	//		link.l1.go = "exit_sit";
            /*}    // to_do
            else
            {
                pchar.GenQuest.Find_Merchant.lastspeak_date = LastSpeakDate();
                MerchantOnMap();
                sld = characterFromID("Quest_Merchant");
       			dialog.text = "Слыхал я один " + RandPhraseSimple("торгаш", "купец") + " на корабле '" + sld.Ship.Name +
                              "' под флагом " + NationNameGenitive(sti(sld.nation)) + " везет полный трюм " + GetGoodsNameAlt(sti(sld.QuestGoodsIdx)) + ".";
    			link.l1 = "Спасибо, браток! Пора мне в море, засиделся я тут.";
    			link.l1.go = "exit_sit";
			} */
		//break;
		//////  1
	    case "GhostShip_Speak_1":
            npchar.GhostShip_Speak_Yet = true;
            pchar.GenQuest.GhostShip.lastspeak_date = LastSpeakDate();
   			dialog.text = "Ну... И-ик! Видел я страшное - корабль-призрак, а на нем сплошные мертвецы... Говорят, он тут со времен Колумба плавает...";
			link.l1 = "Да врешь небось!";
			link.l1.go = "GhostShip_Speak_1_2";
		break;
		
		case "GhostShip_Speak_1_2":
			dialog.text = "Я вру? И-ик! Да я вторую неделю пью, чтоб забыть, что видел... как он на черных рваных парусах входил в бухту...";
			link.l1 = "И как же ты уцелел?";
			link.l1.go = "GhostShip_Speak_1_3";
		break;
		
		case "GhostShip_Speak_1_3":
			dialog.text = "Да я как увидал, что они к берегу пристают, аж штаны испачкал И-ик!... А ты не смейся - сам бы испачкал! И бегом, значит, оттудова...";
			link.l1 = "Нда.. врун ты изрядный, совсем голову пропил";
			link.l1.go = "exit_sit";
			pchar.GenQuest.GhostShip.AskAbout = "1";
			AddQuestRecord("GhostShipQuest", pchar.GenQuest.GhostShip.AskAbout);
		break;
		//////  2
		case "GhostShip_Speak_2":
            npchar.GhostShip_Speak_Yet = true;
            pchar.GenQuest.GhostShip.lastspeak_date = LastSpeakDate();
            dialog.text = "Деется? Да че тут деется?.. А вот говорят Летучий голландец опять золотой караван пустил на дно.";
			link.l1 = "Какой еще голландец?";
			link.l1.go = "GhostShip_Speak_2_2";
		break;

		case "GhostShip_Speak_2_2":
			dialog.text = "'Летучий'! Так корабль-призрак опять появился в архипелаге. Вроде пропадал на много лет... в преисподней был не иначе. А то как прокляты все на нем и пуля их не берет.";
			link.l1    = "И что же, их убить нельзя?";
			link.l1.go = "GhostShip_Speak_2_3";
		break;

		case "GhostShip_Speak_2_3":
			dialog.text = "Бывало, города брали штурмом, весь гарнизон форта вырезали, а солдат-то в форте тыщи! Хотя, я слышал, топили их как-то, но чё им, покойникам сделается - по дну до берега дотащат корабль, починятся кое-как и опять зверствуют.";
			link.l1 = "Эх, до чего люди горазды выдумывать... 'По дну тащат' - надо же. Хотя, спасибо, интересная история.";
			link.l1.go = "exit_sit";
			pchar.GenQuest.GhostShip.AskAbout = "2";
			AddQuestRecord("GhostShipQuest", "2");
		break;
		//////  3
		case "GhostShip_Speak_3":
            npchar.GhostShip_Speak_Yet = true;
            pchar.GenQuest.GhostShip.lastspeak_date = LastSpeakDate();
            
			if (!CheckAttribute(pchar, "GenQuest.GhostShip.LastBattle") &&
                             CheckAttribute(pchar, "GenQuest.GhostShip.NeedCoins")  &&
                            !CheckAttribute(npchar, "quest.GhostShipHelp")          &&
                            !CheckAttribute(pchar, "GenQuest.GhostShipWorkId")      &&
                            rand(2) == 1)
            {
                dialog.text = "Как же, говорят они опять умыслили форт ограбить. Какой не знаю, поспрошай градоначальников, может кому помощь нужна.";
                link.l1 = "О как! Спасибо, мне пора.";
    			link.l1.go = "exit_sit";
                break;
            }

            dialog.text = "Летучий голландец? Да есть такой корабль, хотя многие не верят.";
            if (sti(PChar.GenQuest.GhostShip.KillMe) > 0 || sti(PChar.GenQuest.GhostShip.DeadByMe) > 0)
            {
                link.l1 = "Да я сам его видел! Еле жив остался. Где его найти?";
            }
            else
            {
				link.l1 = "Ладно, я верю. Где его найти?";
			}
			link.l1.go = "GhostShip_Speak_3_2";
		break;

		case "GhostShip_Speak_3_2":
			dialog.text = "Мне местные контрабандисты говорили, что вчера он в нашу бухту заходил.";
			link.l1    = "Прямо здесь, рядом? Он еще там?";
			link.l1.go = "GhostShip_Speak_3_3";
		break;

		case "GhostShip_Speak_3_3":
			dialog.text = "Откуда мне знать, в море ушел наверное. Контрабандисты сами убежали, весь товар потеряли...";
    		if (sti(PChar.GenQuest.GhostShip.KillMe) > 0 || sti(PChar.GenQuest.GhostShip.DeadByMe) > 0)
            {
                link.l1 = "Что же, попробую его нагнать, наш разговор с ним не закончен!";
            }
            else
            {
				link.l1 = "Что же, попробую его нагнать, посмотреть что это за призрак.";
			}
			link.l1.go = "exit_sit";
			pchar.GenQuest.GhostShip.AskAbout = "2";// вечный генератор
			AddQuestRecord("GhostShipQuest", "3");
			GhostShipOnMap();
		break;
		/////////////////////////////////////////////
        case "talk_with_alchogol_song":
			dialog.text = "Ну... И-ик! Бывает все на свете хорошо.. И-ик! В чем дело сразу не поймешь.. И-ик! А просто славно ром пошел.. И-ик! ямайский черный ром.. И-ик!\n Подставляй, наливай!";
			link.l1 = "А дальше?";
			link.l1.go = "talk_with_alchogol_song_2";
			link.l2 = "Да ну тебя!";
			link.l2.go = "exit";
		break;
		
		case "talk_with_alchogol_song_2":
			dialog.text = "Мелькают кружки, лица, каблуки.. И-ик! И полом по башке. Я пью всегда со всеми и везде.. И-ик! Где выпивка? Нужна!";
			link.l1 = "А дальше?";
			link.l1.go = "talk_with_alchogol_song_3";
			link.l2 = "Да ну тебя!";
			link.l2.go = "exit";
		break;
		
		case "talk_with_alchogol_song_3":
			dialog.text = "Когда на сердце тяжесть и холодно в груди, ты пару кружек рома на грудь себе прими! И без забот и фальши все станет вдруг вокруг. Поймешь ты, что я лучший тебе я самый друг!";
			link.l1 = "Да ну тебя!";
			link.l1.go = "exit";
		break;
		
		case "talk_with_alchogol":
			Diag.TempNode = "begin_sit";
			Diag.CurrentNode = Diag.TempNode;
			DialogExit();
			pchar.questTemp.friend_in_tavern = npchar.id;
			AddDialogExitQuest("alc");
		break;

		case "begin_sit":
			Diag.TempNode = "first time";
			dialog.snd = "Voice\HADI\HADI028";
			dialog.text = "Ик! Вот, это я понимаю! Вот это по-нашему! Ну, так чего? Поставишь мне кружечку рома?";
			link.l1 = "Да запросто! Трактирщик, рома!";
			link.l1.go = "sit_2";
		break;

		case "sit_2":
			AddMoneyToCharacter(pchar, -2);
			WaitDate("",0,0,0, 0, 30);
			dialog.text = LinkRandPhrase("Ты настоящий друг! Давай за знакомство!",
			                             "Ик! Какой ром! Ну... вздрогнем!",
										 "За твое здоровье, и за твою щедрость!");
			link.l1 = "Эх, забористый ром. Ладно, мне уже пора, удачно посидеть.";
			link.l1.go = "exit_sit";
			if (makeint(pchar.money) >=2)
			{
				link.l2 = "Давай еще по одной кружечке.";
				link.l2.go = "sit_3";
			}
			// homo 01/08/06 to_del
            // наводка на товар - перенес в слухи
			//if (sti(pchar.reputation) < 41)
            //{
            //    link.l3 = "Что интересного можешь сказать про 'честных' купцов в этих водах?";
			//	link.l3.go = "Find_Merchant_1";
            //}
            //homo 15/06/06 слухи
            link.l4 = LinkRandPhrase("Кажется, что провел в море целую вечность. Что новенького в ваших краях?",
                                    "Расскажи-ка мне, о чем теперь болтают? Не зря же я пою тебя ромом...",
                                    "Скажи мне, братец, какие байки ходят по тавернам?");
		    link.l4.go = "rumours_habitue";
		    //homo
		break;

		case "sit_3":
			int iSituation = makeint(rand(5));
			switch (iSituation)
			{
				case 0:
					dialog.text = "О! Здравая идея!";
					link.l1 = "Трактирщик! Рома!";
					link.l1.go = "sit_2";
				break;

				case 1:
					dialog.text = "Ик! Под... подонок! Ты х-хочешь меня споить, ик! И украсть мои деньги!";
					link.l1 = "Спокойней! Ты чего забыл? Я твой единственный настоящий друг!";
					link.l1.go = "sit_case_2_friend";
					link.l2 = "А! Заткнись, крыса, пока я тебе горло не перерезал!";
					link.l2.go = "sit_case_2_enemy";
				break;

				case 2:
					dialog.text = "Слушай! Ты х-хороший человек! М-может быть, т-ты поможешь мне в... в... в одном дельце?";
					link.l1 = "Нет пожалуй, мне уже пора идти.";
					link.l1.go = "exit_sit";
					link.l2 = "К-конечно! Ведь мы же ик... друзья!";
					link.l2.go = "sit_case_3";
				break;

				case 3:
					dialog.text = "...И тогда я схв-схватил его шпагу за л-лезвие и...";
					link.l1 = "... а она мне и говорит...";
					link.l1.go = "sit_case_4_exit";
				break;

				case 4:
					dialog.text = "За тебя! А, дьявол! Кто это?!";
					link.l1 = "А? Что? Где? Показалось видать.";
					link.l1.go = "sit_2";// to_do "sit_case_5_exit";
				break;
				
                case 5:
                    if (!CheckAttribute(pchar , "GenQuest.GhostShip.LastBattleEnd") && !checkAttribute(npchar, "GhostShip_Speak_Yet") && pchar.GenQuest.GhostShip.lastspeak_date != LastSpeakDate())
					{
                        dialog.text = "Давай еще по одной кружечке.";
                        link.l1 = "Эх, забористый ром. Ладно, мне уже пора, удачно посидеть.";
						link.l1.go = "exit_sit";
						switch (sti(pchar.GenQuest.GhostShip.AskAbout))
						{
							case 0 :
								link.l2 = "Какие байки ходят по тавернам?";
								link.l2.go = "GhostShip_Speak_1";
							break;

							case 1 :
								link.l2 = "Что интересного в этих водах делается?";
								link.l2.go = "GhostShip_Speak_2";
							break;

							case 2 :
								link.l2 = "Скажи мне, братец, что ты знаешь про корабль-призрак - 'Летучий голландец'?";
								link.l2.go = "GhostShip_Speak_3";
							break;
						}
					}
					else
					{
	         			dialog.text = "Пить так пить! Еще давай?";
						link.l1 = "Наливай!";
						link.l1.go = "sit_2";
					}
                break;
			}
		break;

		case "sit_case_5_exit": // дуэль
			Diag.CurrentNode = Diag.TempNode;
			DialogExit();
			AddDialogExitQuest("fighting");
		break;

		case "sit_case_4_exit":
			Diag.CurrentNode = Diag.TempNode;
			DialogExit();
			pchar.questTemp.friend_in_tavern = npchar.id;
			AddDialogExitQuest("without_memory");
		break;

		case "sit_case_3":
			dialog.snd = "Voice\HADI\HADI037";
			dialog.text = "Делов т-том, что м-меня оскорбил один из с-солдат. В-вот!";
			link.l1 = "С-солдат? Н-нет, я пожалуй пас.";
			link.l1.go = "exit_sit";
			link.l2 = "Спокойней! Ты чего забыл? Я твой единственный настоящий друг!";
			link.l2.go = "sit_2";
		break;

		case "sit_case_2_friend":
			dialog.snd = "Voice\HADI\HADI038";
			dialog.text = "Ты? К-какой ты мне друг?! Н-нет! Ты хочешь... ик...  ты хочешь меня убить! В-вот!";
			link.l1 = "Ладно, вижу, тебе уже хватит на сегодня. Прощай.";
			link.l1.go = "exit_sit";
			link.l2 = "Что? Да я тебе сейчас докажу, что я не хочу тебя убивать! Я тебе сейчас руку... ик... отрежу!";
			link.l2.go = "sit_case_2_friend_2";
		break;

		case "sit_case_2_friend_2":
			dialog.snd = "Voice\HADI\HADI039";
			dialog.text = "А-а-а! Уберите его от меня!!";
			link.l1 = "Да ты успокойся... ик... больно не будет!";
			link.l1.go = "tavern_keeper";
		break;

		case "sit_case_2_enemy":
			dialog.snd = "Voice\HADI\HADI040";
			dialog.text = "Агрх! Я тебя разделаю, как свинью!";
			link.l1 = "Это мы еще посмотрим!";
//			link.l1.go = "tavern_keeper";
			link.l1.go = "fight_right_now"; //navy -- битва в таверне без базаров! :)
		break;

		case "exit_sit":
			Diag.CurrentNode = Diag.TempNode;
			DialogExit();
			AddDialogExitQuest("exit_sit");
		break;

		case "tavern_keeper":
			Diag.CurrentNode = Diag.TempNode;
			DialogExit();
			AddDialogExitQuest("tavern_keeper");
		break;
	}
}

bool CheckFreeSitFront(ref _npchar)
{
 	ref rCharacter; //ищем
	int n;

	if (!CheckAttribute(_npchar, "Default.ToLocator")) return false;
	
	for (n=0; n<MAX_CHARACTERS; n++)
	{
		makeref(rCharacter, Characters[n]);
		if (CheckAttribute(rCharacter, "location") && rCharacter.location == _npchar.location)
		{
            if (rCharacter.location.locator == _npchar.Default.ToLocator)
                return  false;
		}
    }
    return  true;
}