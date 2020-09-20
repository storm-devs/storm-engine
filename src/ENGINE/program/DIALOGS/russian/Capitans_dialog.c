// разговор с капитаном на палубе  Boal
//homo 25/06/06
#include "DIALOGS\russian\Rumours\Common_rumours.c"
void ProcessDialogEvent()
{
	ref NPChar;
	aref Link, Diag;

	DeleteAttribute(&Dialog,"Links");

	makeref(NPChar,CharacterRef);
	makearef(Link, Dialog.Links);
	makearef(Diag, NPChar.Dialog);
    //homo Осады
    aref aData;
    makearef(aData, NullCharacter.Siege);
    string sCap, sGroup;
    string fort;

    if (CheckAttribute(aData, "nation"))
    {
	    sCap = NationShortName(sti(aData.nation))+"SiegeCap_";
	    sGroup = "Sea_"+sCap+"1";
	    
	   // string myships  = GetCompanionQuantity(PChar);
       // string escships = Group_GetCharactersNum(sGroup);
        
        switch(sti(aData.conation))
        {
            case 0:  fort = "Английским фортом"; break;
            case 1:  fort = "Французским фортом"; break;
            case 2:  fort = "Испанским фортом"; break;
            case 3:  fort = "Голландским фортом"; break;
            case 4:  fort = "Пиратским фортом"; break;
        }
        int ifortPower = sti(colonies[FindColony(aData.colony)].FortValue);
        int fortDamage = CheckFortInjuri();
        int SquadronDamage = CheckSquadronInjuri();
        
    }

    int iMoney;
	ref sld;
    
    if (CheckNPCQuestDate(npchar, "Card_date"))
	{
		SetNPCQuestDate(npchar, "Card_date");
		npchar.money = 3000 + rand(10) * makeint(100 * sti(PChar.rank) * (10.0 / MOD_SKILL_ENEMY_RATE));
	}
    //homo 26/06/06
	ProcessCommonDialogRumors(NPChar, Link, Diag);
	switch(Dialog.CurrentNode)
	{
        case "Exit":
			DialogExit();
			Diag.CurrentNode = Diag.TempNode;
		break;
		
		case "First time":
			if (!bDisableMapEnter)
			{
                if (CheckAttribute(NPChar, "EncType")  && NPChar.EncType == "war")
                {
    			    Dialog.text = TimeGreeting() + ", "+ GetAddress_Form(NPChar)+"! Я "+ GetFullName(NPChar) +" - капитан корабля флота "+NationKingsName(NPChar)+". Что привело вас ко мне на борт?";
    			}
    			else
                {
    			    Dialog.text = TimeGreeting() + ", "+ GetAddress_Form(NPChar)+"! Я капитан "+ GetFullName(NPChar) +". Что привело вас на борт моего корабля?";
    			}
    			link.l1 = "Меня зовут " + GetFullName(Pchar) + ". У меня есть  к вам дело!";
    			link.l1.go = "quests";
                link.l2 = "Ничего особенного, просто зашел поприветствовать вас!";
    			link.l2.go = "exit";
            }
            else
            {
                Dialog.text = "Вы заметили, " + GetAddress_Form(NPChar) + ", идет битва? Нам сейчас не до разговоров!";
    			link.l1 = "Вы правы! Вернусь на свой корабль.";
    			link.l1.go = "exit";
            }
			Diag.TempNode = "first time";
 		break;

        case "quests":
            if (!CheckAttribute(NPChar, "MainCaptanId")  || NPChar.MainCaptanId == pchar.GenQuest.CaptainId)
            {
                Dialog.text = "Я вас внимательно слушаю, " + GetAddress_Form(NPChar)+".";
                link.l1 = "Я хотел бы узнать последние новости архипелага.";
                //homo 25/06/06
                link.l1.go = "rumours_capitan";
                //
                if (CheckAttribute(NPChar, "EncGroupName")) // только для фантомов грабеж
                {
                    link.l2 = RandPhraseSimple("Вы в курсе кто я? Думаю ясно, что мои корабельные орудия могут сделать решето из вашего корыта. Давайте решим все мирно. Выкуп вполне меня устроит.",
                                               "Море... и мы одни.. предлагаю проспонсировать мою команду небольшим выкупом вашей безопасности.");
                    link.l2.go = "Talk_board";
                }
                if (CheckAttribute(NPChar, "Ship.Mode") && NPChar.Ship.Mode == "Trade")
                {
	           		link.l3 = "Откуда вы плывете?";
	    			link.l3.go = "price_1";
	    			link.l4 = "Поторгуем?";
	    			link.l4.go = "Trade_1";
    			}
				//--> квест мэра, поиски пирата
				if (CheckAttribute(pchar, "GenQuest.DestroyPirate"))
				{
	    			link.l6 = "Я разыскиваю пиратский корабль, орудующий в здешних водах. Вы ничего не видели странного?";
	    			link.l6.go = "MQ_step_1";
				}
				//<-- квест мэра, поиски пирата
    			link.l5 = "Желаете развеяться?";
			    link.l5.go = "Play_Game";
                link.l9 = "Думаю мне пора!";
    			link.l9.go = "exit";
			}
			else
			{
                Dialog.text = "Я не уполномочен решать дела с вами, обратитесь к командующему нащей эскадры. Его имя " + GetFullName(characterFromID(NPChar.MainCaptanId))+
                              ", он находится на корабле " + XI_ConvertString(RealShips[sti(characters[GetCharacterIndex(NPChar.MainCaptanId)].Ship.Type)].BaseName) + " '" + characters[GetCharacterIndex(NPChar.MainCaptanId)].Ship.Name + "'.";
                link.l1 = "Спасибо. Так и поступлю.";
                link.l1.go = "exit";
                Diag.TempNode = "Go_away_Good";
			}
        break;
        case "Trade_1":
			dialog.text = "Почему бы и нет?";
			link.l1 = "Пойдемте смотреть товары.";
			link.l1.go = "Trade_2";
			link.l2 = "Я передумал.";
			link.l2.go = "exit";
		break;
		
		case "Trade_2":
			FillShipStore(NPChar);
	    	pchar.shiptrade.character = NPChar.id;
   			Diag.CurrentNode = Diag.TempNode;
		    DialogExit();
		    DeleteAttribute(pchar, "PriceList.StoreManIdx"); // не вести лог по кораблям
		    LaunchStore(SHIP_STORE);
		break;
		
        case "Play_Game":
			dialog.text = "Что вы предлагаете?";
			link.l1 = "Перекинемся в карты на солидный интерес?";
			link.l1.go = "Card_Game";
   			link.l2 = "Погремим костями в покер-джокер?";
			link.l2.go = "Dice_Game";
			link.l10 = "Прошу простить меня, но меня ждут дела.";
			link.l10.go = "exit";
		break;
        // карты -->
        case "Card_Game":
            if (!CheckNPCQuestDate(npchar, "Card_date_Yet") || isBadReputation(pchar, 30) || GetCharacterSkillToOld(pchar, SKILL_LEADERSHIP) < rand(3))
            {
                SetNPCQuestDate(npchar, "Card_date_Yet");
                dialog.text = "Я не буду играть в азартные игры с разными проходимцами!";
    			link.l1 = "Как вам будет угодно.";
    			link.l1.go = "exit";
			}
			else
			{
                dialog.text = "Давайте! Немного передохну от корабельных дел...";
    			link.l1 = "Замечательно.";
    			link.l1.go = "Cards_begin";
    			link.l2 = "По каким правилам играем?";
    			link.l2.go = "Cards_Rule";
			}
		break;

		case "Cards_Rule":
   			dialog.text = CARDS_RULE;
			link.l1 = "Что ж, давайте начнем!";
			link.l1.go = "Cards_begin";
			link.l3 = "Нет, это не для меня...";
			link.l3.go = "exit";
		break;

		case "Cards_begin":
			Dialog.text = "Давайте определимся со ставкой.";
			link.l1 = "Играем по 100 монет.";
			link.l1.go = "Cards_Node_100";
			link.l2 = "Давайте по 500 золотых.";
			link.l2.go = "Cards_Node_500";
			link.l3 = "Пожалуй, мне пора.";
			link.l3.go = "exit";
		break;

		case "Cards_Node_100":
		    if (sti(pchar.Money) < 300)
		    {
                dialog.text = "Шутить изволите? У вас нет денег!";
                link.l1 = "Бывает.";
			    link.l1.go = "exit";
			    break;
		    }
		    if (sti(npchar.Money) < 300)
		    {
                dialog.text = "Хватит с меня, а то на содержание корабля не останется...";
                link.l1 = "Жаль.";
			    link.l1.go = "exit";
			    break;
		    }
   			dialog.text = "Хорошо, играем по 100 монет.";
			link.l1 = "Начали!";
			link.l1.go = "Cards_begin_go";
			pchar.GenQuest.Cards.npcharIdx = npchar.index;
            pchar.GenQuest.Cards.iRate     = 100;
            pchar.GenQuest.Cards.SitType   = false;
		break;

		case "Cards_Node_500":
		    if (sti(pchar.Money) < 1500)
		    {
                dialog.text = "Шутить изволите? У вас нет 1500 золотых!";
                link.l1 = "Будут!";
			    link.l1.go = "exit";
			    break;
		    }
		    if (sti(npchar.Money) < 1500)
		    {
                dialog.text = "Нет, такие ставки не доведут до добра.";
                link.l1 = "Как угодно.";
			    link.l1.go = "exit";
			    break;
		    }
   			dialog.text = "Хорошо, играем по 500 монет.";
			link.l1 = "Начали!";
			link.l1.go = "Cards_begin_go";
			pchar.GenQuest.Cards.npcharIdx = npchar.index;
            pchar.GenQuest.Cards.iRate     = 500;
            pchar.GenQuest.Cards.SitType   = false;
		break;

		case "Cards_begin_go":
            Diag.CurrentNode = Diag.TempNode;
			DialogExit();
            LaunchCardsGame();
		break;
	    // карты <--
	    //  Dice -->
        case "Dice_Game":
            if (!CheckNPCQuestDate(npchar, "Dice_date_Yet") || isBadReputation(pchar, 30) || GetCharacterSkillToOld(pchar, SKILL_LEADERSHIP) < rand(3))
            {
                SetNPCQuestDate(npchar, "Dice_date_Yet");
                dialog.text = "Я не буду играть в азартные игры с разными проходимцами!";
    			link.l1 = "Как вам будет угодно.";
    			link.l1.go = "exit";
			}
			else
			{
				if (CheckNPCQuestDate(npchar, "Dice_date_begin"))
				{
					dialog.text = "Давайте! Отдых никогда не вредил здоровью... только кошельку...";
	    			link.l1 = "Замечательно.";
	    			link.l1.go = "Dice_begin";
	    			link.l2 = "По каким правилам игра?";
	    			link.l2.go = "Dice_Rule";
    			}
    			else
    			{
					dialog.text = "Нет, с меня довольно на сегодня. Дела ждут.";
	    			link.l1 = "Как вам будет угодно.";
	    			link.l1.go = "exit";
				}
			}
		break;

		case "Dice_Rule":
   			dialog.text = DICE_RULE;
			link.l1 = "Что ж, давайте начнем!";
			link.l1.go = "Dice_begin";
			link.l3 = "Нет, это не для меня...";
			link.l3.go = "exit";
		break;

		case "Dice_begin":
			Dialog.text = "Давайте определимся со ставкой.";
			link.l1 = "Играем по 50 монет за кубик.";
			link.l1.go = "Dice_Node_100";
			link.l2 = "Давайте по 200 золотых за кубик.";
			link.l2.go = "Dice_Node_500";
			link.l3 = "Пожалуй, мне пора.";
			link.l3.go = "exit";
		break;

		case "Dice_Node_100":
            if (!CheckDiceGameSmallRate())
		    {
                dialog.text = "О тебе ходит слава непревзойденного шулера. Я не буду с тобой играть в кости вообще.";
                link.l1 = "Все врут! Ну и не нужно.";
			    link.l1.go = "exit";
			    break;
		    }

			if (sti(pchar.Money) < 300)
		    {
                dialog.text = "Шутить изволите? У вас нет денег!";
                link.l1 = "Бывает.";
			    link.l1.go = "exit";
			    break;
		    }
		    if (sti(npchar.Money) < 300)
		    {
                dialog.text = "Все! Нужно завязывать с играми, а то запишут в растратчики и спишут на берег...";
                link.l1 = "Жаль.";
			    link.l1.go = "exit";
			    break;
		    }
   			dialog.text = "Хорошо, играем по 50 монет.";
			link.l1 = "Начали!";
			link.l1.go = "Dice_begin_go";
			pchar.GenQuest.Dice.npcharIdx = npchar.index;
            pchar.GenQuest.Dice.iRate     = 50;
            pchar.GenQuest.Dice.SitType   = false;
		break;

		case "Dice_Node_500":
            if (!CheckDiceGameSmallRate())
		    {
                dialog.text = "О тебе ходит слава непревзойденного шулера. Я не буду с тобой играть в кости вообще.";
                link.l1 = "Все врут! Ну и не нужно.";
			    link.l1.go = "exit";
			    break;
		    }
			if (!CheckDiceGameBigRate())
		    {
                dialog.text = "Я слышал, что ты очень хорошо играешь. Я не буду играть с тобой по таким большим ставкам.";
                link.l1 = "Давайте по более низким ставкам?";
			    link.l1.go = "Dice_Node_100";
				link.l2 = "Извините, мне пора.";
			    link.l2.go = "exit";
			    break;
		    }

			if (sti(pchar.Money) < 1500)
		    {
                dialog.text = "Шутить изволите? У вас нет 1500 золотых!";
                link.l1 = "Будут!";
			    link.l1.go = "exit";
			    break;
		    }
		    if (sti(npchar.Money) < 1500)
		    {
                dialog.text = "Нет, такие ставки не доведут до добра.";
                link.l1 = "Как угодно.";
			    link.l1.go = "exit";
			    break;
		    }
   			dialog.text = "Хорошо, играем по 200 монет за кубик.";
			link.l1 = "Начали!";
			link.l1.go = "Dice_begin_go";
			pchar.GenQuest.Dice.npcharIdx = npchar.index;
            pchar.GenQuest.Dice.iRate     = 200;
            pchar.GenQuest.Dice.SitType   = false;
		break;

		case "Dice_begin_go":
            SetNPCQuestDate(npchar, "Dice_date_begin");
			Diag.CurrentNode = Diag.TempNode;
			DialogExit();
            LaunchDiceGame();
		break;
	    // Dice <--
        case "price_1":
			if (CheckNPCQuestDate(npchar, "trade_date"))
			{
                SetNPCQuestDate(npchar, "trade_date");
				iMoney = findPriceStoreMan(NPChar);
	            if (iMoney == -1)
	            {
	                Dialog.Text = "Зачем вам это знать? Это мое личное дело.";
					Link.l1 = "Как вам будет угодно.";
					Link.l1.go = "exit";
	            }
	            else
	            {
                    sld = &Characters[iMoney];
                    pchar.PriceList.ShipStoreIdx = iMoney;
                    
					Dialog.Text = "Я плыву из города " + GetCityName(sld.City) + ".";
					Link.l1 = "Какие цены на товары в местном магазине?";
					Link.l1.go = "price_2";
					Link.l9 = "Мне уже пора.";
					Link.l9.go = "exit";
				}
			}
			else
			{
                Dialog.Text = "Я же вам все уже сказал. Не беспокойте меня по пустякам.";
				Link.l1 = "Да, вы правы. Прощайте.";
				Link.l1.go = "exit";
            }
		break;
		
		case "price_2":
            sld = &Characters[sti(pchar.PriceList.ShipStoreIdx)];
			SetPriceListByStoreMan(&Colonies[FindColony(sld.City)]);
			Dialog.Text = "Вот такие... (вы получили сводку цен на товары).";
			Link.l1 = "Большое спасибо!";
			Link.l1.go = "exit";
			PlaySound("interface\important_item.wav");
		break;
		
        case "Talk_board":
            if ((Group_GetCharactersNum(NPChar.EncGroupName) - Group_GetDeadCharactersNum(NPChar.EncGroupName)) > GetCompanionQuantity(PChar) && rand(11) > GetCharacterSkillToOld(PChar, SKILL_FORTUNE))
            {
                Dialog.text = "Ха-ха! Хорошая шутка. Тем более, что у меня больше кораблей. Убирайтесь на свой корабль и тоните вместе с ним.";
                link.l1 = "Больше - не значит мощнее...";
                link.l1.go = "Boarding";
            }
            else
            {
                if(rand(21) > (GetSummonSkillFromNameToOld(PChar, SKILL_GRAPPLING) + GetSummonSkillFromNameToOld(PChar, SKILL_LEADERSHIP)) )
                {
                    Dialog.text = "Напрасно вы затеяли свое грязное дело, капитан, на борту моего корабля. Я проявлю милосердие и позволю вам вернуться на свой корабль и утонуть вместе с ним.";
                    link.l1 = "Мы еще посмотрим кто сегодня пойдет на корм акулам!";
                    link.l1.go = "Boarding";
                }
                else
                {
                    Pchar.GenQuest.MoneyForCaptureShip = makeint(100 + (Group_GetCharactersNum(NPChar.EncGroupName) - Group_GetDeadCharactersNum(NPChar.EncGroupName))*(7 - sti(RealShips[sti(NPChar.Ship.Type)].Class))*(1+rand(10))*500);
                    Dialog.text = RandSwear() + "Да, я наслышан о ваших бесчинствах. Будь по-вашему, но помните " +
                                  XI_ConvertString(NationShortName(sti(NPChar.nation))+"hunter") + " не оставит это безнаказанным!";
                    link.l1 = "Замечательно, Сумма в "+Pchar.GenQuest.MoneyForCaptureShip+" золотых меня вполне устроит, "+GetAddress_FormToNPC(NPChar)+".";
                    link.l1.go = "Capture";
                    link.l2 = "Ой. Да ладно. Я пошутил!";
                    link.l2.go = "exit";
                }
            }
            // тут нужен учет запрета повторного наезда
            Diag.TempNode = "Go_away";
			sld = characterFromId(pchar.GenQuest.CaptainId);
            sld.talk_date_Go_away =  lastspeakdate(); // boal злопамятность :)
        break;

        case "rumours":
			Dialog.Text = SelectRumour(); // to_do
			Link.l1 = RandPhraseSimple(RandSwear() + "Это очень интересно, другой вопрос?",
                                     "Еще одно дело.");
			Link.l1.go = "quests";
			Link.l2 = RandPhraseSimple("Спасибо, должен откланятся.",
                                     "Всего хорошего.");
			Link.l2.go = "exit";
			Diag.TempNode = "quests";
		break;
		
		case "Go_away":
			Dialog.Text = "Убирайтесь с моего корабля!";
			Link.l1 = "Уже ухожу.";
			Link.l1.go = "exit";
			Diag.TempNode = "Go_away";
		break;
		
		case "Go_away_Good":
			Dialog.Text = "Наш разговор уже окончен. Ничего нового я вам не скажу, "+GetAddress_Form(NPChar)+".";
			Link.l1 = "Ясно. До встречи в море!";
			Link.l1.go = "exit";
			Diag.TempNode = "Go_away_Good";
		break;

        case "Capture":
            Diag.TempNode = "Go_away";
            Dialog.Text = "Получите! А теперь убирайтесь вон!";
			Link.l1 = "Замечательно!";
			Link.l1.go = "exit";
			AddMoneyToCharacter(pchar, sti(Pchar.GenQuest.MoneyForCaptureShip));
			ChangeCharacterReputation(pchar, -5);
			AddCharacterExpToSkill(pchar, "Leadership", 20);
			ChangeCharacterHunterScore(pchar, NationShortName(sti(NPChar.nation)) + "hunter", 7 + rand(10));
        break;

        case "Boarding":
            if (CheckAttribute(NPChar, "MainCaptanId"))
            {
                PChar.StartBattleAfterDeck = true;
                PChar.StartBattleMainCaptanId = NPChar.MainCaptanId;
                PChar.StartBattleEncGroupName = NPChar.EncGroupName;
            }
            DialogExit();
            Diag.CurrentNode = Diag.TempNode;
        break;
        
        case "QuestAboardCabinDialog":  // тестовый диалог, в игре не работает, для метода SetQuestAboardCabinDialog
			Diag.TempNode = "QuestAboardCabinDialog";
            Dialog.Text = "Стой, так и убить можно. Что ты хочешь от меня?";
			Link.l1 = "Убить!";
			Link.l1.go = "QuestAboardCabinDialog_1";
			Link.l2 = "Взять в плен.";
			Link.l2.go = "QuestAboardCabinDialog_2";
			Link.l3 = "Отдай нам карту, Билли (С)";  // предмет, далее не ясно или  QuestAboardCabinDialog_1 или QuestAboardCabinDialog_2
			Link.l3.go = "QuestAboardCabinDialog_3";  // можно закодить покидание корабля, но экипаж уже вырезан и тп...
        break;
        
        case "QuestAboardCabinDialog_1":
			Dialog.Text = "Ну, это мы еще посмотрим!";
			Link.l1 = "А что тут смотреть?";
			Link.l1.go = "exit";
			// тут можно восстановить НР противника (NPChar) или добавить парочку гардов против ГГ
			LAi_SetCurHPMax(NPChar);
			AddDialogExitQuestFunction("QuestAboardCabinDialogExitWithBattle"); // тут ругань обратно
		break;
		
		case "QuestAboardCabinDialog_2":
			Dialog.Text = "Хорошо, твоя взяла!";
			Link.l1 = "Так бы сразу?";
			Link.l1.go = "exit";
			AddDialogExitQuestFunction("QuestAboardCabinDialogSurrender");
		break;
		
		case "QuestAboardCabinDialog_3":
			Dialog.Text = "Да забирай!";
			Link.l1 = "Вот и хорошо, теперь сдавайся в плен.";
			if (rand(1) == 1)  // пример!!!
			{
				Link.l1.go = "QuestAboardCabinDialog_2";
			}
			else
			{
			    Link.l1.go = "QuestAboardCabinDialog_3_1";
			}
			TakeNItems(NPChar, "Chest", -1);
			TakeNItems(pchar, "Chest", 1);
		break;
		
		case "QuestAboardCabinDialog_3_1":
			Dialog.Text = "Пришел, наследил, все отнял и еще меня в рабы? Фиг!";
			Link.l1 = "Тогда тапки белые готовь.";
			Link.l1.go = "QuestAboardCabinDialog_1";
		break;
		//eddy. квест мэра, поиски пирата
		case "MQ_step_1":
			Dialog.Text = "Нет, никакого подозрительного корабля, похожего на пиратский, я не видел.";
			Link.l1 = "Понятно.";
			Link.l1.go = "exit";
		break;
		
		//homo Наводка на купца
		case "MerchantTrap_Abordage":

			Dialog.Text = "Именем "+NationKingsName(NPChar)+" сдавайся, подлый пират! И тебе сохранят жизнь, до справедливого суда, где ты ответишь, за все "+NationNameSK(sti(NPChar.nation))+"ие корабли, которые ты ограбил.";
			Link.l1 = "Как бы не так - сам сдавайся.";
			Link.l1.go = "exit";
			LAi_SetCurHPMax(NPChar);
			AddDialogExitQuestFunction("QuestAboardCabinDialogExitWithBattleNoParam"); // тут ругань обратно
		break;
		
		case "Siegehelp":
		
            ref rchar = Group_GetGroupCommander(sGroup);
            if (NPChar.id == rchar.id)
            {
    			
    			dialog.text = "Я вас слушаю, но давайте поскорее выкладывайте цель вашего визита, у нас идет бой с " + fort +
                          " и мне нужно руководить ходом сражения.";
                link.l1 = "Именно поэтому я здесь, " + GetAddress_FormToNPC(NPChar) + ".";
                link.l1.go = "attack_fort";

                link.l2 = "В таком случае не буду вас больше задерживать. Прощайте, "+ GetAddress_FormToNPC(NPChar) + ".";
                link.l2.go = "exit";
    			
            }
            else
            {
                Dialog.text = "Я не уполномочен решать дела с вами, обратитесь к командующему нащей эскадры. Его имя " + GetFullName(rchar)+
                              ", он находится на корабле " + XI_ConvertString(RealShips[sti(rchar.Ship.Type)].BaseName) + " ''" + rchar.Ship.Name + "''.";
                link.l1 = "Спасибо. Так и поступлю.";
                link.l1.go = "exit";
            
            }
            Diag.TempNode = "Siegehelp";
            
		break;
		
		case "attack_fort":
                dialog.text = "И о чем же идет речь?";
                link.l1 = "Я могу помочь вам разгромить форт колонии " +GetConvertStr(aData.Colony+" Town", "LocLables.txt")+ " и захватить город, а добычу, полученную в случае нашего успеха, мы поделим между собой.";
                link.l1.go = "Siegehelp_1";
                link.l2 = "Собственно мое дело вряд ли заслуживает вашего внимания. Прощайте, "+ GetAddress_FormToNPC(NPChar) + ".";
                link.l2.go = "exit";
                Diag.TempNode = "Siegehelp_0";
		break;
		
		case "Siegehelp_0":
            Dialog.text = "Это опять вы? Мне казалось мы все уже обсудили?";
            link.l1 = "Вы правы - забыл.";
			link.l1.go = "exit";
			NPChar.DeckDialogNode = "Siegehelp_0";
            Diag.TempNode = "Siegehelp_0";

		break;
		
		case "Siegehelp_1":
            SiegeResult("");
            if (sti(aData.win)==0)
            {
                dialog.text = "И какую же часть добычи вы рассчитываете получить?";
                link.l1 = "Половину.";
                link.l1.go = "Big_part";
                link.l2 = "Одну треть.";
                link.l2.go = "Middle_part";
                link.l3 = "Четвертую часть.";
                link.l3.go = "Small_part";
            }
            else
            {
                dialog.text = "Мне не требуются ваши услуги, капитан! Я не нуждаюсь ни в чьей помощи, так как в любом случае захвачу эту колонию.";
                link.l1 = "Откуда такая уверенность, " + GetAddress_FormToNPC(NPChar) + ", неудачи могут постигнуть любого.";
                link.l1.go = "attack_fort_03";
                link.l2 = "В таком случае не буду вас больше задерживать, " + GetAddress_FormToNPC(NPChar) + ". Прощайте!";
                link.l2.go = "exit";
            }
			Diag.TempNode = "Siegehelp_0";

		break;
		
		case "attack_fort_03":


            if((sti(aData.iSquadronPower)*SquadronDamage - (ifortPower)*fortDamage) < 4000)
            {
                dialog.text = "С другой стороны мои корабли уже изрядно потрепаны в этом сражении и подкрепление нам бы не помешало. Если вы согласны на " + GetPart(4) + ", то я думаю мы сможем договориться.";
                link.l1 = "Меня это устраивает, " + GetAddress_FormToNPC(NPChar) + ". Я попытаюсь взять форт штурмом, а вы со своей стороны поддержите нас огнем корабельных орудий.";
                link.l1.go = "attack_fort_04";
                aData.PartAttaksFort = 4;
                link.l2 = "Я стою не так дешево, " + GetAddress_FormToNPC(NPChar) + ". Прошу прощения, что оторвал вас от насущных дел. Прощайте.";
                link.l2.go = "exit";
            }else{
                dialog.text = "В моей эскадре достаточно кораблей, чтобы разнести этот форт в пух и прах, это только вопрос времени. А теперь соизвольте покинуть мой корабль, впереди меня ожидает победа!";
                link.l1 = "Ну что ж, в таком случае, " + GetAddress_FormToNPC(NPChar) + ", позвольте откланяться. Прощайте!";
                link.l1.go = "exit";
            }
		    Diag.TempNode = "Siegehelp_0";
		break;
		case "Big_part":
            if( (sti(aData.iSquadronPower)*SquadronDamage - (ifortPower)*fortDamage) <= -1500)
            {
                dialog.text = "Орудия форта сильно повредили корабли моей эскадры и исход этого сражения может обернуться не в нашу пользу. Я согласен на эти условия и принимаю вашу помощь.";
                link.l1 = "Отлично, " + GetAddress_FormToNPC(NPChar) + ", в таком случае мы возьмем форт штурмом, а вы уж поддержите нас огнем корабельных пушек.";
                link.l1.go = "attack_fort_04";
                aData.PartAttaksFort = 2;
            }
            else
            {
                dialog.text = "Не многовато ли, капитан? У вас " + GetNumShips(GetCompanionQuantity(PChar)) +
                              ", в моей эскадре " + GetNumShips(Group_GetCharactersNum(sGroup)) + ". Я могу вам предложить " + GetPart(4) +
                              ". И я думаю это правильно.";
                aData.PartAttaksFort = 4;
                link.l1 = "Я согласен, " + GetAddress_FormToNPC(NPChar) + ", в таком случае я беру на себя захват форта, а вы уж поддержите нас огнем корабельных орудий.";
                link.l1.go = "attack_fort_04";
                link.l2 = "К сожалению я так не думаю, " + GetAddress_FormToNPC(NPChar) + ". Нам больше не о чем разговаривать. Прощайте.";
                link.l2.go = "exit";
            }
            Diag.TempNode = "Siegehelp_0";
        break;
        
        case "attack_fort_04":
            dialog.text = "Как только форт будет захвачен, мы немедленно начинаем высадку десантных групп на берег, а дальше уже продолжим бой в самом городе, где я думаю мы и встретимся с вами.";
            link.l1 = "На этом и порешим. До встречи, " + GetAddress_FormToNPC(NPChar) + "!";
            link.l1.go = "exit";
            Diag.TempNode = "Siegehelp_0";
            PChar.quest.Union_with_Escadra = "Yes";
        break;

        case "Middle_part":
            if((sti(aData.iSquadronPower)*SquadronDamage - (ifortPower)*fortDamage) <= -600)
            {
                dialog.text = "Пожалуй, я соглашусь на ваши условия. Совместная операция увеличивает мои шансы на благополучный исход этого сражения. В случае успешного завершения дела, вы получите свою часть добычи.";
                link.l1 = "Ну вот и славно, что мы договорились, " + GetAddress_FormToNPC(NPChar) + ". Я немедленно отдам приказ своим людям атаковать форт.";
                link.l1.go = "attack_fort_04";
                aData.PartAttaksFort = 3;
            }else{
                dialog.text = "Не многовато ли, капитан? У вас " + GetNumShips(GetCompanionQuantity(PChar)) + ", в моей эскадре " + GetNumShips(Group_GetCharactersNum(sGroup)) + ". Я могу вам предложить " + GetPart(4) + ". И я думаю это правильно.";
                link.l1 = "Я согласен, " + GetAddress_FormToNPC(NPChar) + ", в таком случае я беру на себя штурм форта, а вы поддержите нас огнем корабельных орудий.";
                link.l1.go = "attack_fort_04";
                aData.PartAttaksFort = 4;
                link.l2 = "К сожалению я так не думаю, " + GetAddress_FormToNPC(NPChar) + ". Нам больше не о чем разговаривать. Прощайте.";
                link.l2.go = "exit";
            }
            Diag.TempNode = "Siegehelp_0";
        break;

        case "Small_part":
            dialog.text = "Корабли моей эскадры измотаны сражением, так что свежие силы нам не помешают. Я согласен на эти условия и принимаю ваше предложение.";
            link.l1 = "Я рад, что мы смогли договориться, " + GetAddress_FormToNPC(NPChar) + ". Я немедленно отдам приказ своим людям вступить в бой и начать атаку форта.";
            link.l1.go = "attack_fort_04";
            aData.PartAttaksFort = 4;
            Diag.TempNode = "Siegehelp_0";
        break;
		
		case "Talk_Capture_City":
            AfterTownBattle();  // все, все свободны
            LAi_LoginInCaptureTown(NPChar, false);
            aData.win = 1;
            EndOfTheSiege("End");
            NPChar.location = "";
            int ilt = makeint(sti(aData.loot)/sti(aData.PartAttaksFort));
            dialog.text = "Здравствуйте, капитан! Хоть битва и была жаркой, но все таки нам удалось сломить яростное сопротивление защитников города. Доходы, полученные в результате захвата колонии составляют " + sti(aData.loot)+
                          " золотых. И по нашему договору вы получаете "+ilt+
                          ". Вот ваши деньги, вы их несомненно заслужили. А теперь прошу меня извинить, но меня ждут дела.";
            link.l1 = "В таком случае позвольте откланяться, " + GetAddress_FormToNPC(NPChar) + ". Прощайте!";
            link.l1.go = "exit";
            //AddDialogExitQuest("End_Siege_and_Attaks_City");

            Diag.TempNode = "no_return_02";
            AddMoneyToCharacter(PChar, ilt);
            ChangeCharacterNationReputation(pchar, sti(NPChar.nation), 20);
            ChangeCharacterReputation(PChar, 10);
            AddCharacterExpToSkill(GetMainCharacter(), "Leadership", 80);
            AddCharacterExpToSkill(GetMainCharacter(), "Sailing", 100);
            AddCharacterExpToSkill(GetMainCharacter(), "Defence", 100);
            AddCharacterExpToSkill(GetMainCharacter(), "Accuracy", 100);
            AddCharacterExpToSkill(GetMainCharacter(), "Cannons", 100);
            AddCharacterExpToSkill(GetMainCharacter(), "Fortune", 80);
            aData.loot = sti(aData.loot) - ilt;
            if (CheckAttribute(PChar, "quest.LeaveTown")) Pchar.quest.LeaveTown.over = "yes";
            //--> слухи
            SiegeRumour("Прошел слух, что вы помогли нашей эскадре, при штурме "+NationNameSK(sti(aData.conation))+"ой колонии - "+GetConvertStr(aData.Colony+" Town", "LocLables.txt")+"! Мы так благодарны вам, "+ GetAddress_Form(NPChar)+".", "", sti(aData.nation), -1, 30, 3);
            //<-- слухи
		break;

        case "no_return_02":
            dialog.text = "Приветствую вас, капитан! У вас ко мне дело?";
            link.l1 = "Никаких, " + GetAddress_FormToNPC(NPChar) + ". Прощайте!";
            link.l1.go = "exit";
            Diag.TempNode = "no_return_02";
		break;
		
		case "GoldSquadron":
            ref rch = Group_GetGroupCommander("Sea_Head_of_Gold_Squadron");
            if (NPChar.id == rch.id)
            {
    			dialog.text = "Я вас слушаю, но давайте поскорее выкладывайте цель вашего визита, я очень спешу. На меня возложена важная миссия секретного характера, и к тому же мне предписано, топить все подозрительные корабли в открытом море, которые могут представлять опасность нашему конвою.";
                          
                link.l1 = "В таком случае не буду вас больше задерживать. Прощайте, "+ GetAddress_FormToNPC(NPChar) + ".";
                link.l1.go = "exit";

            }
            else
            {
                Dialog.text = "Я не уполномочен решать дела с вами, обратитесь к командующему нащей эскадры. Его имя " + GetFullName(rch)+
                              ", он находится на корабле " + XI_ConvertString(RealShips[sti(rch.Ship.Type)].BaseName) + " ''" + rch.Ship.Name + "''.";
                link.l1 = "Спасибо. Так и поступлю.";
                link.l1.go = "exit";

            }
            Diag.TempNode = "GoldSquadron";
		break;
	}
}

string GetNumShips(int ship)
{
    string numships;
    switch(ship)
    {
       case 0: numships = "ни одного корабля"; break;
       case 1: numships = "один корабль";      break;
       case 2: numships = "два корабля";       break;
       case 3: numships = "три корабля";       break;
       case 4: numships = "четыре корабля";    break;
       case 5: numships = "пять кораблей";     break;
       case 6: numships = "шесть кораблей";    break;
    }
    return numships;
}

string GetPart(int part)
{
    string numpart;
    switch(part)
    {
       case 0: numpart = ""; break;
       case 1: numpart = "всю добычу"; break;
       case 2: numpart = "половину добычи"; break;
       case 3: numpart = "треть добычи"; break;
       case 4: numpart = "четверть добычи";      break;

    }
    return numpart;
}

int findPriceStoreMan(ref NPChar)
{
    ref ch;
	int n;
    int storeArray[40];
    int howStore = 0;

	for(n=0; n<MAX_CHARACTERS; n++)
	{
		makeref(ch,Characters[n]);
		if (CheckAttribute(ch, "Dialog.Filename") && ch.Dialog.Filename == "Common_Store.c") // магазин
		{
            //if (sti(ch.nation) !=  sti(NPChar.nation)) continue;
            if (ch.location == "none") continue; // фикс для новых, невидимых до поры островов
            storeArray[howStore] = n;
            howStore++;
            
            // homo 05/09/06
            if ( CheckAttribute(NPChar, "FromColony") && NPChar.FromColony == ch.City ) return n;
            //
		}
    }
    if (howStore == 0)
    {
        return -1;
    }
    else
    {
        return storeArray[rand(howStore-1)];
    }
}