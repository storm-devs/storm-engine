// boal 25/04/04 общий диалог Usurer
#include "DIALOGS\russian\Rumours\Common_rumours.c"  //homo 25/06/06
void ProcessDialogEvent()
{
	ref NPChar, chr, sld;
	aref Link, NextDiag;
	string NPC_Area, sTemp, sTitle;
	int LoanSum, LoanInterest, LoanPeriod, LoanResult, iPastMonths, DepositSum, DepositInterest, DepositResult, iNum;

	DeleteAttribute(&Dialog,"Links");

	makeref(NPChar,CharacterRef);
	makearef(Link, Dialog.Links);
	makearef(NextDiag, NPChar.Dialog);

	string iDay, iMonth;
	iDay = environment.date.day;
	iMonth = environment.date.month;
	string lastspeak_date = iday + " " + iMonth;

	NPC_Area = Npchar.City;
 
	// вызов диалога по городам -->
    NPChar.FileDialog2 = "DIALOGS\" + LanguageGetLanguage() + "\Usurer\" + NPChar.City + "_Usurer.c";
    if (LoadSegment(NPChar.FileDialog2))
	{
        ProcessCommonDialog(NPChar, Link, NextDiag);
		UnloadSegment(NPChar.FileDialog2);
	}
    // вызов диалога по городам <--
	ProcessCommonDialogRumors(NPChar, Link, NextDiag);//homo 16/06/06

	if (!CheckAttribute(npchar, "quest.trade_date"))
    {
        npchar.quest.trade_date = "";
    }
    if (!CheckAttribute(npchar, "quest.FindCitizenNoShip"))
    {
        npchar.quest.FindCitizenNoShip = 0;
    }

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
		break;
		
		case "First time":
			if (LAi_group_GetPlayerAlarm() > 0)
			{
       			dialog.text = NPCharRepPhrase(pchar, 
					LinkRandPhrase("В городе поднята тревога, тебя всюду ищут! На твоем месте я бы не стал здесь задерживаться.", "Вся городская стража рыщет по городу в поисках тебя. Я не идиот и разговаривать с тобой не буду!", "Беги приятель, пока солдаты не сделали из тебя решето..."), 
					LinkRandPhrase("Что тебе нужно, негодяй?! Городская стража уже взяла твой след, далеко тебе не уйти, грязный пират!", "Грязный убийца, вон из моего дома! Стража!!", "Я не боюсь тебя, мерзавец! Скоро тебя повесят в нашем форте, далеко тебе не уйти..."));
				link.l1 = NPCharRepPhrase(pchar,
					RandPhraseSimple("Хех, тревога для меня не проблема...", "Им меня ни за что не поймать."), 
					RandPhraseSimple("Заткни свою пасть, " + GetWorkTypeOfMan(npchar, "") + ", а не то вырву твой поганый язык!", "Хех, " + GetWorkTypeOfMan(npchar, "") + ", а все туда же - пиратов ловить! Вот что я тебе скажу, приятель: сиди тихо и будешь жить..."));
				link.l1.go = "fight";
				break;
			}
			//homo Линейка Блада
            if (Pchar.questTemp.CapBloodLine == true )
            {
                dialog.Text = LinkRandPhrase("Эй, доктор Блад! " + TimeGreeting() + ".",
                                    "Рад видеть Вас, Питер Блад.",
                                    "Хорошо, что Вы заглянули ко мне, " + GetFullName(pchar) + ". Как поживает полковник Бишоп?");
                Link.l1 = "Увы, я уже ухожу, " + NPChar.name + ". До встречи.";
				Link.l1.go = "exit";
				break;
            }
			//пиратка, квест №5, заглушка на возврат бабла -->
			if (CheckAttribute(npchar, "PLQ5Money"))
			{
				dialog.text = "Вы принесли мне мои 50 тысяч?";
				link.l1 = "Нет еще, я этим занимаюсь. Ждите.";
				link.l1.go = "exit";
				link.l2 = "Боюсь, что у меня ничего не выйдет с этим делом. Считайте, что я отказался.";
				link.l2.go = "StepPL5End_close";
				if (sti(pchar.money) >= 50000)
				{
					link.l3 = "Да, я забрал ваши 50 тысяч им готов их вам вернуть.";
					link.l3.go = "StepPL5End_1";
				}
				break;
			}
			//<--пиратка, квест №5, заглушка на возврат бабла
			//-->> дача квеста найти потерянный драгоценный камень
			if (rand(1) && pchar.questTemp.different == "free" && !CheckAttribute(npchar, "quest.usurersJewel") && GetNpcQuestPastDayWOInit(npchar, "usurersJewel") > 7)
			{
				dialog.text = "Здравствуйте, вы как нельзя вовремя! Хочу поручить вам одно дело.";
				link.l1 = "Внимательно слушаю. Что за дело?";
				link.l1.go = "usurersJewel_1";
				npchar.quest.usurersJewel = "inSeek"; //личный флаг ростовщика на взятый квест
				SaveCurrentNpcQuestDateParam(npchar, "usurersJewel");
				break;
			}
			//<<-- дача квеста найти потерянный драгоценный камень
			dialog.text = NPCStringReactionRepeat("Здравствуйте, " + GetAddress_Form(npchar) + ". Чем я могу вам помочь?", 
				"О, это опять вы? Добро пожаловать, " + GetAddress_Form(npchar) + " Что вы хотите на этот раз?", 
				"Хм, снова вы, " + GetAddress_Form(npchar) + "... Ну что же, рад вас видеть который уже раз за день. Что вы желаете?",
                "Эх, " + GetAddress_Form(npchar) + ", вы себе представить не можете, до чего порой бывают навязчивы клиенты... Что вам угодно?" , "block", 1, npchar, Dialog.CurrentNode);
			link.l1 = HeroStringReactionRepeat("Ничем, я просто зашел посмотреть и поговорить.", 
				"Опять я. И опять просто так поговорить решил.",
                "Ага, снова я. И снова без конкретного дела зашел поговорить.", 
				"Да-а-а, тяжело приходится ростовщикам порой... А мне от вас ничего не нужно, просто поболтать хотел...", npchar, Dialog.CurrentNode);
			link.l1.go = "exit";
			link.l2 = "Я хочу обсудить наши финансовые взаимоотношения.";
			link.l2.go = "LoanUsurer";//(на кредитный генератор)
			link.l3 = "Мне нужна кое-какая информация... В общем, есть вопрос.";
			link.l3.go = "quests";//(перессылка в файл города)
			//линейка ГПК, закладка миллиона на депозит
			if (CheckAttribute(pchar, "questTemp.LSC.usurerId") && pchar.questTemp.LSC.usurerId == npchar.id)
			{
				if (pchar.questTemp.LSC == "SignOnPoorMurder")
				{
					link.l4 = "Послушайте, вам должен был поступить депозит на мое имя.";
					link.l4.go = "ISS_deposit";
				}
				if (pchar.questTemp.LSC == "PoorMurderExecute")
				{
					link.l4 = "Мне необходимо снять депозит на миллион пиастров, что находится у вас.";
					link.l4.go = "ISS_takeDeposit";
					DeleteAttribute(pchar, "questTemp.LSC.usurerId");
				}
			}
			//квесты мэра, проникновение во враждебный город
  			if (CheckAttribute(pchar, "GenQuest.Intelligence") && pchar.GenQuest.Intelligence.SpyId == npchar.id && pchar.GenQuest.Intelligence == "") //квест мэра - на связь с нашим шпионом
			{
				link.l7 = RandPhraseSimple("Я здесь по поручению одного человека. Его зовут губернатор " + GetFullName(characterFromId(pchar.GenQuest.Intelligence.MayorId)) + ".", 
					GetFullName(characterFromId(pchar.GenQuest.Intelligence.MayorId)) + " прислал меня к вам. Я должен кое-что забрать...");
				link.l7.go = "IntelligenceForAll";
			}
			//<<<<---------------- пиратка, квест №5. --------------------
			if (CheckAttribute(pchar, "questTemp.piratesLine.Q5.city_1") && npchar.city == pchar.questTemp.piratesLine.Q5.city_1)
			{
				if (pchar.questTemp.piratesLine == "PL5Hunter_toFirstUsurer")
				{
					link.l5 = "Говорят, что вы Джона Эйвори нанимали.";
					link.l5.go = "StepPL5First_1";
				}
				if (pchar.questTemp.piratesLine == "PL5Hunter_toFirst")
				{
					link.l5 = "Послушайте, не нанимали ли вы Джона Эйвори?";
					link.l5.go = "StepPL5First_1";
				}
			}
			if (CheckAttribute(pchar, "questTemp.piratesLine.Q5.city_2") && npchar.city == pchar.questTemp.piratesLine.Q5.city_2)
			{
				if (pchar.questTemp.piratesLine == "PL5Hunter_toSecondUsurer")
				{
					link.l5 = "Говорят у вас были проблемы с Джоном Эйвори...";
					link.l5.go = "StepPL5Second_0";
				}
				if (pchar.questTemp.piratesLine == "PL5Hunter_toSecond")
				{
					link.l5 = "Послушайте, не нанимали ли вы Джона Эйвори для выполнения поручений?";
					link.l5.go = "StepPL5Second_1";
				}
			}
			if (CheckAttribute(pchar, "questTemp.piratesLine.Q5.city_3") && npchar.city == pchar.questTemp.piratesLine.Q5.city_3)
			{
				if (pchar.questTemp.piratesLine == "PPL5Hunter_toThirdUsurer" || pchar.questTemp.piratesLine == "PPL5Hunter_toThird")
				{
					link.l5 = "Я ищу Джона Эйвори, к вам он не обращался?";
					link.l5.go = "StepPL5Third_1";
				}
			}
			if (CheckAttribute(pchar, "questTemp.piratesLine.Q5.city_4") && npchar.city == pchar.questTemp.piratesLine.Q5.city_4)
			{
				if (pchar.questTemp.piratesLine == "PL5Hunter_toFourth")
				{
					link.l5 = "Я слышал у вас случилось несчастье, возможно вам понадобится помощь?";
					link.l5.go = "StepPL5Four_1";
				}
			}
			//-------------------- пиратка, квест №5. ---------------->>>>
			//-->> сдача квеста найти потерянный драгоценный камень
			if (CheckCharacterItem(pchar, "UsurersJew") && CheckAttribute(npchar, "quest.usurersJewel.type"))
			{
				link.l6 = "Я нашел ваш " + npchar.quest.usurersJewel.type + ". Вот он.";
				link.l6.go = "usurersJewel_R1";
			}
			//<<-- сдача квеста найти потерянный драгоценный камень
			NextDiag.TempNode = "First time";
		break;

		//<<<<---------------- пиратка, квест №5. --------------------
		case "StepPL5First_1":
			dialog.text = "Нанимал, и он прекрасно справился с моим поручением. Правда, хи-хи, говорят, в таверне он так напился, что перепутал местного пьянчужку с моим должником.";
			link.l1 = "А он не сказал куда направился? Может, еще какому ростовщику помощь нужна?";
			link.l1.go = "StepPL5First_2";
		break;
		case "StepPL5First_2":
			dialog.text = "Вот этого не знаю, чужие проблемы меня не касаются.";
			link.l1 = "Это понятно, что не касаются. Но может вскользь что-то было?..";
			link.l1.go = "StepPL5First_3";
		break;
		case "StepPL5First_3":
			pchar.questTemp.piratesLine.Q5.city_2 = GetQuestNationsCity(SPAIN); 
			dialog.text = "Да, вроде бы, собирался на север Мейна курс держать, к испанцам. А куда точно - не ведаю... Хотя подождите, что-то он говорил о " + XI_ConvertString("Colony" + pchar.questTemp.piratesLine.Q5.city_2 + "Dat") + ". Но сами понимаете, что за достоверность не ручаюсь.";
			link.l1 = "Понял. Спасибо.";
			link.l1.go = "exit";
			AddQuestRecord("Pir_Line_5_Hunter", "4");
			AddQuestUserData("Pir_Line_5_Hunter", "sCity1", XI_ConvertString("Colony" + pchar.questTemp.piratesLine.Q5.city_1 + "Gen"));
			AddQuestUserData("Pir_Line_5_Hunter", "sCity2", XI_ConvertString("Colony" + pchar.questTemp.piratesLine.Q5.city_2 + "Acc"));
			AddQuestUserData("Pir_Line_5_Hunter", "sIsland", XI_ConvertString(GetIslandByCityName(pchar.questTemp.piratesLine.Q5.city_2) + "Dat"));
			pchar.questTemp.piratesLine = "PL5Hunter_toSecond";
		break;

		case "StepPL5Second_0":
			dialog.text = "Э-э-э, не то чтоб проблемы...";
			link.l1 = "Хм, так вы нанимали его?";
			link.l1.go = "StepPL5Second_1";
		break;
		case "StepPL5Second_1":
			dialog.text = "Нанимал. Работу он выполнил, долг вернул сполна. Только в таверне, стал болтать языком, что, якобы, выбил у должника сумму, многократно превышающую сумму долга, и как провел меня, умолчав об этом.\n"+
				"Лично мне, совершенно не понятно, где здесь обман, и почему этот рассказ, всех ужасно веселит?! Немного обидно, конечно, знал бы я раньше, что должник обладает такой суммой...\n"+
				"В общем, я вздохнул с облегчением, когда Джон Эйвори куда-то уплыл из нашего города. И теперь, слышать о нем не желаю!";
			link.l1 = "Ясно. А куда он направился, вы, конечно, не в курсе...";
			link.l1.go = "StepPL5Second_2";
		break;
		case "StepPL5Second_2":
			pchar.questTemp.piratesLine.Q5.city_3 = GetQuestNationsCity(FRANCE);
			dialog.text = "Думаю, к французам подался этот паршивец. Довелось краем уха услышать его разговор в борделе. Туда я заходил по делам, не по прямому назначению этого заведения...";
			link.l1 = "Уважаемый, мне все равно, что вы там делали. Спасибо вам за информацию об Эйвори.";
			link.l1.go = "exit";
			AddQuestRecord("Pir_Line_5_Hunter", "6");
			AddQuestUserData("Pir_Line_5_Hunter", "sCity", XI_ConvertString("Colony" + pchar.questTemp.piratesLine.Q5.city_3));
			AddQuestUserData("Pir_Line_5_Hunter", "sIsland", XI_ConvertString(GetIslandByCityName(pchar.questTemp.piratesLine.Q5.city_3) + "Dat"));
			pchar.questTemp.piratesLine = "PL5Hunter_toThird";
		break;

		case "StepPL5Third_1":
			dialog.text = "Джон периодически выполняет мои заказы, вот совсем недавно выполнил еще один...";
			link.l1 = "А где он может сейчас находиться?";
			link.l1.go = "StepPL5Third_2";
		break;
		case "StepPL5Third_2":
			dialog.text = "Это одному Богу только известно. Понимаете, как исполнитель Джон хорош, но уж, очень он вспыльчивый. Давеча в таверне, прости господи душу грешную, человека зарезал. Поэтому я, лишних вопросов, Джону не задаю...";
			link.l1 = "Понятно... Но, может быть, есть хоть какие-то соображения, куда он мог отправится?";
			link.l1.go = "StepPL5Third_3";
		break;
		case "StepPL5Third_3":
			pchar.questTemp.piratesLine.Q5.city_4 = GetQuestNationsCity(Holland);
			dialog.text = "Если вас интересует мое мнение, то думаю, что он отправился в " + XI_ConvertString("Colony" + pchar.questTemp.piratesLine.Q5.city_4) + ".";
			link.l1 = "Хм... А причины данного умозаключения я могу услышать?";
			link.l1.go = "StepPL5Third_4";
		break;
		case "StepPL5Third_4":
			dialog.text = "Конечно. Видите ли, дело в том, что в " + XI_ConvertString("Colony" + pchar.questTemp.piratesLine.Q5.city_4 + "Dat") + " недавно был ограблен ростовщик.";
			link.l1 = "А-а-а, я вас понял! Спасибо огромное.";
			link.l1.go = "exit";
			AddQuestRecord("Pir_Line_5_Hunter", "8");
			AddQuestUserData("Pir_Line_5_Hunter", "sCity1", XI_ConvertString("Colony" + pchar.questTemp.piratesLine.Q5.city_3 + "Gen"));
			AddQuestUserData("Pir_Line_5_Hunter", "sCity2", XI_ConvertString("Colony" + pchar.questTemp.piratesLine.Q5.city_4 + "Gen"));
			AddQuestUserData("Pir_Line_5_Hunter", "sIsland", XI_ConvertString(GetIslandByCityName(pchar.questTemp.piratesLine.Q5.city_4) + "Dat"));
			pchar.questTemp.piratesLine = "PL5Hunter_toFourth";
		break;

		case "StepPL5Four_1":
			dialog.text = "Нет, нет, спасибо. Помощь уже не нужна, я только вчера нанял Джона Эйвори, он лучший в этом деле! О, не обижайтесь, вы тоже один из лучших, просто Эйвори зашел ко мне раньше вас... Э-э-э, простите, как вы сказали, вас зовут?";
			link.l1 = GetFullName(pchar) + ".";
			link.l1.go = "StepPL5Four_2";
		break;
		case "StepPL5Four_2":
			dialog.text = "Да, да, я о вас слышал... кажется... Так о чем я рассказывал? А да, представляете, врывается эдакая образина, приставляет нож к горлу, и начинает меня грабить! Меня!!! Уважаемого человека, посреди бела дня...";
			link.l1 = "Простите, что перебиваю, но вы это, сейчас, о ком?";
			link.l1.go = "StepPL5Four_3";
		break;
		case "StepPL5Four_3":
			dialog.text = "...сколько денег пропало. Как о ком?! О грабителе, конечно! Ах, мои деньги! Ну, ничего, Джон Эйвори ему покажет! Эйвори это сила! Мощь!.. Он имел наглость, бахвалятся, что мол, руки у меня коротки, его на Бермудах достать...";
			link.l1 = "Кто? Эйвори?!";
			link.l1.go = "StepPL5Four_4";
		break;
		case "StepPL5Four_4":
			dialog.text = "Да, причем здесь Эйвори, грабитель, конечно! Слушайте, что вы меня все время перебиваете?!";
			link.l1 = "Извините. А если я, принесу вам награбленное? Вас это не сильно огорчит?";
			link.l1.go = "StepPL5Four_5";
		break;
		case "StepPL5Four_5":
			dialog.text = "Э-э-э... Ну... Мне, в принципе, все равно, кто деньги вернет, но с Эйвори вы разбирайтесь сами. Меня, ваши с ним дела, не касаются.";
			link.l1 = "Никаких проблем. Наши с Джоном дела, вас не затронут. Так о какой сумме идет речь?";
			link.l1.go = "StepPL5Four_6";
		break;
		case "StepPL5Four_6":
			dialog.text = "Пятьдесят тысяч! Если вы мне все это принесете, ваша награда составит пять тысяч.";
			link.l1 = "Как звали грабителя?";
			link.l1.go = "StepPL5Four_7";
		break;
		case "StepPL5Four_7":
			dialog.text = "Вы думаете, он мне представился?! Где вы видели таких вежливых грабителей? Между прочим, Джон, подобных вопросов не задавал.";
			link.l1 = "Хорошо. По рукам! Ждите меня, я скоро!";
			link.l1.go = "exit";
			AddQuestRecord("Pir_Line_5_Hunter", "9");
			pchar.questTemp.piratesLine = "PL5Hunter_toBermudes";
			npchar.PLQ5Money = true; //метка на ростовщика, которому должен бабла
			//здесь ставим прерывание на вход в дом Орри на Бермудах
            Pchar.quest.PQ5_inOrryHouse.win_condition.l1 = "location";
            Pchar.quest.PQ5_inOrryHouse.win_condition.l1.location = "Pirates_houseS1";
            Pchar.quest.PQ5_inOrryHouse.function = "PQ5_inOrryHouse";
		break;

		case "StepPL5End_close":
			dialog.text = "Плохо, я вам очень разочарован. Мне кажется, что вы что-то недоговариваете... Ну да ладно.";
			link.l1 = "Вот именно! Все нормально, приятель.";
			link.l1.go = "exit";
			DeleteAttribute(npchar, "PLQ5Money");
    		ChangeCharacterReputation(pchar, -50);
			ChangeCharacterNationReputation(pchar, sti(NPChar.nation), -20);
		break;

		case "StepPL5End_1":
			dialog.text = "Черт побери, я рад! Скажите, а что случилось с тем мерзавцем, который ограбил меня?";
			link.l1 = "Он получил по заслугам, больше он вас не побеспокоит...";
			link.l1.go = "StepPL5End_2";
		break;
		case "StepPL5End_2":
			dialog.text = "Великолепно, ну что же, я готов посчитать мои деньги. Давайте начнем...";
			link.l1 = "Извольте получить.";
			link.l1.go = "StepPL5End_3";
		break;
		case "StepPL5End_3":
			dialog.text = "Отлично! Просто превосходно! Все в порядке. Вот ваши 5 тысяч, как и договаривались.";
			link.l1 = "Спасибо...";
			link.l1.go = "StepPL5End_4";
			DeleteAttribute(npchar, "PLQ5Money");
			AddMoneyToCharacter(pchar, -45000);
    		ChangeCharacterReputation(pchar, 10);
			ChangeCharacterNationReputation(pchar, sti(NPChar.nation), 5);
		break;
		case "StepPL5End_4":
			dialog.text = "Ну что же, я всегда рад вас видеть у себя. И хочу вам сказать, что всем расскажу о вашей исполнительности и честности. Наглец Эйвори вам не чета, вы однозначно лучший...";
			link.l1 = "Прямо в краску вгоняете... Ну ладно, прощайте.";
			link.l1.go = "exit";
		break;
		//-------------------- пиратка, квест №5. ---------------->>>>

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////		Кредитный генератор Loan
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		case "LoanUsurer": 
			dialog.text = "Давайте обсудим. Внимательно вас слушаю.";
			link.l1 = "Есть ли у вас дело, требующее сторонней помощи?";
			link.l1.go = "Loan_GiveWork";
			//найти должника
			if (CheckAttribute(pchar, "GenQuest.Loan.FindCitizen") && pchar.GenQuest.Loan.FindCitizenNpcharId == NPChar.id)
            {
                link.l2 = "Я по поводу задания о должнике.";
                link.l2.go = "FindCitizen_1";
            }
			//доставить сундук
            if (CheckAttribute(pchar, "GenQuest.LoanChest.TakeChest") && pchar.GenQuest.LoanChest.ChestNpcharId == NPChar.id)
            {
                link.l3 = "Я по поводу задания о доставке кредита.";
                link.l3.go = "TakeChest_1";
            }
            if (CheckAttribute(pchar, "GenQuest.LoanChest.TakeChest") && sti(pchar.GenQuest.LoanChest.TargetIdx) == sti(NPChar.index))
            {
                link.l4 = "Я привез ваш кредит.";
                link.l4.go = "TakeChest_2";
            }
			//кредиты
            if(CheckAttribute(Pchar, "Quest.Loans." + (NPC_Area)) && makeint(Pchar.Quest.Loans.(NPC_Area)) == true)
			{
				iPastMonths = GetPastTime("Month", makeint(Pchar.Quest.Loans.(NPC_Area).StartYear),makeint(Pchar.Quest.Loans.(NPC_Area).StartMonth),makeint(Pchar.Quest.Loans.(NPC_Area).StartDay), makefloat(Pchar.Quest.Loans.(NPC_Area).StartTime), getDataYear(),getDataMonth(),GetDataDay(), GetTime());
				Pchar.Quest.Loans.(NPC_Area).Result = makeint(Pchar.Quest.Loans.(NPC_Area).Sum) + ((makeint(Pchar.Quest.Loans.(NPC_Area).Sum)/100)*makeint(Pchar.Quest.Loans.(NPC_Area).Interest))*(iPastMonths+1);// boal 23.01.2004
				if(makeint(PChar.money) >= makeint(Pchar.Quest.Loans.(NPC_Area).Result))
				{
					Link.l5 = "Я должен вам "+ FindRussianMoneyString(sti(Pchar.Quest.Loans.(NPC_Area).Result)) + LinkRandPhrase(". Я готов вернуть долг.", ". В данный момент я в состоянии выплатить вам долг.", ". Настало время, наконец, рассчитаться.");
					Link.l5.go = "loan_return";	
					sTemp = "credit";
				}					
			}
			else
			{
				Link.l6 = LinkRandPhrase("Я хочу занять денег.", "Мне срочно понадобилась пара-другая пиастров.", "Как насчет небольшого кредита?");
				Link.l6.go = "loan";	
				if(makeint(Pchar.money) >= 100)
				{
					Link.l7 = LinkRandPhrase("Я бы хотел отдать деньги в рост.","Примете на хранение пиратскую добычу?", "Золотишко на черный день оставить можно?");
					Link.l7.go = "deposit";				
				}
			}
			if(CheckAttribute(Pchar, "Quest.Deposits." + (NPC_Area)) && makeint(Pchar.Quest.Deposits.(NPC_Area)) == true)
			{
				iPastMonths = GetPastTime("Month", makeint(Pchar.Quest.Deposits.(NPC_Area).StartYear),makeint(Pchar.Quest.Deposits.(NPC_Area).StartMonth),makeint(Pchar.Quest.Deposits.(NPC_Area).StartDay), makefloat(Pchar.Quest.Deposits.(NPC_Area).StartTime), getDataYear(),getDataMonth(),GetDataDay(), GetTime());
				Pchar.Quest.Deposits.(NPC_Area).Result = makeint(Pchar.Quest.Deposits.(NPC_Area).Sum) + ((makeint(Pchar.Quest.Deposits.(NPC_Area).Sum)/100)*makeint(Pchar.Quest.Deposits.(NPC_Area).Interest))*iPastMonths;
				Link.l8 = LinkRandPhrase("Я хочу забрать свой вклад.", "Пришло время забрать мои деньги, что у вас хранятся.", "Мне нужно мое золото и набежавшие проценты.");
				Link.l8.go = "Deposit_return";									
			}
			//выход
			if (sTemp == "credit")
			{
				link.l9 = LinkRandPhrase("Я помню, что брал у вас кредит, но рассчитатся пока не готов...", "Я знаю, что должен вам, но сейчас у меня нет денег. Вернуть позже...", "Я помню, что должен вам, но в данный момент не имею достаточных средств для погашения кредита. Я сделаю это позже.");
				link.l9.go = "Loan_Remind";
			}
			else
			{
				link.l9 = "Я ошибся, никаких финансовых дел у меня с вами нет. Прощайте.";
				link.l9.go = "exit";
			}
		break;
		//=============================  даем или не даем работу - сундуки и должники
		case "Loan_GiveWork": 
            // проверка на проф пригодность -->
            if (isBadReputation(pchar, 42))
            {
                dialog.text = RandSwear() + "Ты ведь " + GetFullName(pchar)+ ", не так ли? О тебе " +
                              RandPhraseSimple("ходят ужасные слухи","говорят не лучшим образом") + ", я не хочу иметь с тобой дело.";
				link.l1 = RandPhraseSimple("Эх... Стоит человеку оступиться, и все будут в него плевать.", RandSwear() + "Ну и не надо!");
				link.l1.go = "exit";
                break;
            }
            // проверка на проф пригодность <--
            dialog.text = "Нет, к счастью, у меня все хорошо.";

			if (npchar.quest.trade_date != lastspeak_date || bBettaTestMode)
			{
                npchar.quest.trade_date = lastspeak_date;
                iNum = rand(1);
                if (sti(Pchar.Ship.Type) == SHIP_NOTUSED)
                {
                    iNum = 0; // всегда первый
                }
                switch (iNum)
                {
                    case 0 : // найти должника
                        if (CheckAttribute(pchar, "GenQuest.Loan.FindCitizen")) break;
                        
                        if (sti(Pchar.Ship.Type) == SHIP_NOTUSED && sti(npchar.quest.FindCitizenNoShip) < 2)
                        {
                            iNum = findCitizenMan(Npchar, true);
                            npchar.quest.FindCitizenNoShip = sti(npchar.quest.FindCitizenNoShip) + 1; // знатичит на суше без корабля
                        }
                        else
                        {
                            iNum = findCitizenMan(Npchar, false);
                        }
                        if (iNum > 0)
                        {
                            chr = &Characters[iNum];
                            pchar.GenQuest.Loan.FindCitizenMoney   = (1 + rand(sti(PChar.rank)))*1500 + rand(10)*100;
                            pchar.GenQuest.Loan.FindCitizenPercent = makeint(sti(pchar.GenQuest.Loan.FindCitizenMoney) / (3+rand(2)));
                            pchar.GenQuest.Loan.FindCitizenIdx  =  iNum;
							sTemp = "";
							if (npchar.city != chr.city)
							{
								sTemp = ", что на " + XI_ConvertString(GetIslandByCityName(chr.city)+"Dat");
							}
                            dialog.text = "Да, вы вовремя. Есть у меня щекотливый вопрос, требующий решения. "+
                                     GetFullName(chr) + ", живущий в " + XI_ConvertString("Colony"+chr.city+"Dat") + sTemp + ", задолжал мне " + FindRussianMoneyString(sti(pchar.GenQuest.Loan.FindCitizenMoney)) +
                                     ". Если вы вернете мне всю сумму, то набежавшие проценты можете оставить себе. Они, кстати, составляют " + FindRussianMoneyString(sti(pchar.GenQuest.Loan.FindCitizenPercent)) + ".";
                            link.l1 = "Я берусь!";
                			link.l1.go = "LoanUsurer_GiveWork_1";
            			}
                    break;

                    case 1: // доставить сундуки
                        if (CheckAttribute(pchar, "GenQuest.LoanChest.TakeChest")) break;
                        
                        iNum = findChestMan(Npchar);
                        if (iNum > 0)
                        {
                            chr = &Characters[iNum];
                            pchar.GenQuest.LoanChest.Chest      = 2 + rand(2);
                            pchar.GenQuest.LoanChest.Money      = (1 + rand(sti(pchar.GenQuest.LoanChest.Chest)))*1500 + rand(10)*100;
                            pchar.GenQuest.LoanChest.TargetIdx  =  iNum;
                            pchar.GenQuest.LoanChest.Time       = 20 + rand(15);
							sTemp = "";
							if (npchar.city != chr.city)
							{
								sTemp = ", что на " + XI_ConvertString(GetIslandByCityName(chr.city)+"Dat");
							}
                            dialog.text = "Мне нужен проверенный человек для важной миссии. Нужно доставить кредит - " + pchar.GenQuest.LoanChest.Chest + 
									" сундука с золотом в "+ XI_ConvertString("Colony"+chr.city+"Acc") + sTemp + " для человека по имени " +
									GetFullName(chr) + ". Он местный " + GetWorkTypeOfMan(chr, "") +
									". За работу получите " + FindRussianMoneyString(sti(pchar.GenQuest.LoanChest.Money)) + ". Это нужно сделать за " + FindRussianDaysString(sti(pchar.GenQuest.LoanChest.Time)) + ".";
                            link.l1 = "Я готов!";
                			link.l1.go = "LoanUsurer_ChestWork_1";
            			}
                     break;
    			}
    		}
            else
            {
                dialog.text = "Я вам уже все сказал.";
            }
            link.l9 = "Извините, мне пора.";
		    link.l9.go = "exit";
		break;
		//============== boal нахождение должника ===============
		case "LoanUsurer_GiveWork_1":
            pchar.GenQuest.Loan.FindCitizen = true;
            pchar.GenQuest.Loan.FindCitizenNpcharId = Npchar.id;
            dialog.text = "Замечательно. Жду вас как можно скорее.";
			link.l9 = "Не подведу!";
			link.l9.go = "exit";			
            chr = &Characters[sti(pchar.GenQuest.Loan.FindCitizenIdx)];     
			sTemp = "";
			if (npchar.city != chr.city)
			{
				sTemp = ", что на " + XI_ConvertString(GetIslandByCityName(chr.city)+"Dat");
			}
			ReOpenQuestHeader("Gen_LoanFindCitizen");
            AddQuestRecord("Gen_LoanFindCitizen", "1");
            AddQuestUserData("Gen_LoanFindCitizen", "sName", GetFullName(chr));
            AddQuestUserData("Gen_LoanFindCitizen", "sCity", XI_ConvertString("Colony"+chr.city+"Dat") + sTemp);
			AddQuestUserData("Gen_LoanFindCitizen", "sFromCity", XI_ConvertString("Colony"+Npchar.city+"Gen"));
			AddQuestUserData("Gen_LoanFindCitizen", "sMoney", FindRussianMoneyString(sti(pchar.GenQuest.Loan.FindCitizenMoney)));
			AddQuestUserData("Gen_LoanFindCitizen", "sMyMoney", FindRussianMoneyString(sti(pchar.GenQuest.Loan.FindCitizenPercent)));
		break;

		case "FindCitizen_1": //проверка выполнения квеста
            dialog.text = "Я весь внимание.";
            if (CheckAttribute(pchar, "GenQuest.Loan.FindCitizenDone"))
            {
                link.l1 = "Ваше поручение выполнено!";
                link.l1.go = "finish_work";
                link.l2 = "Мне удалось найти должника, но не удалось выбить из него денег.";
                if (GetCharacterSkillToOld(PChar, SKILL_FORTUNE) >= rand(11))
                {
                    link.l2.go = "finish_work_bad";
                }
                else
                {
                    link.l2.go = "finish_work_lie";
                }
            }

            if (CheckAttribute(pchar, "GenQuest.Loan.FindCitizenFalied"))
            {
                link.l1 = "Мне удалось найти должника, но не удалось выбить из него денег.";
                link.l1.go = "finish_work_bad";
            }
            link.l3 = "Я решил аннулировать ваш заказ по поиску должника - это не для меня.";
            link.l3.go = "finish_work_cancel";
            link.l9 = "Нет. Ничего.";
			link.l9.go = "exit";
		break; 

		case "finish_work":
            if (sti(pchar.Money) >= sti(pchar.GenQuest.Loan.FindCitizenMoney))
            {
                dialog.text = "Вот и славно!";
    			link.l9 = "Пожалуйста!";
    			link.l9.go = "exit";
    			AddMoneyToCharacter(Pchar, -1*sti(pchar.GenQuest.Loan.FindCitizenMoney));
    			DeleteAttribute(pchar, "GenQuest.Loan");
    			DeleteAttribute(Pchar, "quest.LoanFindCitizenTimer");
    			AddCharacterExpToSkill(Pchar, "Commerce", makeint(Pchar.rank) * 3);
    			AddCharacterExpToSkill(Pchar, "Leadership", makeint(Pchar.rank) * 6);
    			ChangeCharacterReputation(pchar, 1);
    			OfficersReaction("good");    			
    			AddQuestRecord("Gen_LoanFindCitizen", "4");
                CloseQuestHeader("Gen_LoanFindCitizen");
			}
			else
			{
			    dialog.text = "Замечательно! Но где мои деньги? Время-то идет.";
    			link.l9 = "Скоро будут!";
    			link.l9.go = "exit";
    		}
		break;
		
		case "finish_work_bad":
            dialog.text = "Очень и очень плохо! Вы не в состоянии делать свою работу!";
			link.l9 = "Этот человек правда был не состоятелен, но вы правы - это моя работа.";
			link.l9.go = "exit";
			DeleteAttribute(pchar, "GenQuest.Loan");
			DeleteAttribute(Pchar, "quest.LoanFindCitizenTimer");
			ChangeCharacterReputation(pchar, -1);
			AddQuestRecord("Gen_LoanFindCitizen", "5");
            CloseQuestHeader("Gen_LoanFindCitizen");
		break;
		
		case "finish_work_cancel":
            if (CheckAttribute(pchar, "GenQuest.Loan.FindCitizenDone") || CheckAttribute(pchar, "GenQuest.Loan.FindCitizenFalied"))
            {
                dialog.text = "А мне сдается вы меня обманываете! Я точно знаю, что вы с ним говорили. Верните мои деньги!";
                link.l1 = "Да вы правы, вот деньги.";
    			link.l1.go = "finish_work";
    			link.l2 = "Но он отказался платить!";
    			if (GetCharacterSkillToOld(PChar, SKILL_FORTUNE) >= rand(11))
                {
    			   link.l2.go = "finish_work_bad";
    			}
    			else
    			{
    			   link.l2.go = "finish_work_bad_2";
    			}
    			link.l9 = "Мне счас некогда. Я зайду позже.";
    			link.l9.go = "exit";
    			ChangeCharacterReputation(pchar, -1);
			}
            else
            {
                ChangeCharacterReputation(pchar, -1);
                dialog.text = "Плохо, я уже рассчитывал на вас.";
                link.l1 = "Да вы правы, но такова жизнь.";
    			link.l1.go = "exit";
    			DeleteAttribute(pchar, "GenQuest.Loan");
    			DeleteAttribute(Pchar, "quest.LoanFindCitizenTimer");
    			AddQuestRecord("Gen_LoanFindCitizen", "6");
                CloseQuestHeader("Gen_LoanFindCitizen");
            }
		break;
		
		case "finish_work_bad_2":
            dialog.text = "Вы уже показали, что склонны ко лжи. Верните мои деньги!";
            link.l1 = "Да вы правы, вот деньги.";
    		link.l1.go = "finish_work";
		break;
		
		case "finish_work_lie":
            dialog.text = "Хм, точно?";
            link.l1 = "Конечно!";
    		link.l1.go = "finish_work_cancel";
		break;

		//============== boal доставка сундуков ===============
		case "LoanUsurer_ChestWork_1": 
            pchar.GenQuest.LoanChest.TakeChest = true;
            pchar.GenQuest.LoanChest.ChestNpcharId = Npchar.id;
            dialog.text = "Я на вас очень рассчитываю.";
			link.l9 = "Не подведу!";
			link.l9.go = "exit";
			TakeNItems(pchar, "Chest", sti(pchar.GenQuest.LoanChest.Chest));
			SetTimerCondition("LoanChestTimer", 0, 0, sti(pchar.GenQuest.LoanChest.Time), false);
			// немного веселой жизни
            TraderHunterOnMap();
            chr  = &Characters[sti(pchar.GenQuest.LoanChest.TargetIdx)];
			sTemp = "";
			if (npchar.city != chr.city)
			{
				sTemp = ", что на " + XI_ConvertString(GetIslandByCityName(chr.city)+"Dat");
			}
			ReOpenQuestHeader("Gen_LoanTakeChest");
            AddQuestRecord("Gen_LoanTakeChest", "1");
            AddQuestUserData("Gen_LoanTakeChest", "sFromCity", XI_ConvertString("Colony"+Npchar.city+"Gen"));
			AddQuestUserData("Gen_LoanTakeChest", "sQty", pchar.GenQuest.LoanChest.Chest);
			AddQuestUserData("Gen_LoanTakeChest", "sCity", XI_ConvertString("Colony"+chr.city+"Acc") + sTemp);
			AddQuestUserData("Gen_LoanTakeChest", "sName", GetFullName(chr));
			AddQuestUserData("Gen_LoanTakeChest", "sType", GetWorkTypeOfMan(chr, ""));
			AddQuestUserData("Gen_LoanTakeChest", "sMoney", FindRussianMoneyString(sti(pchar.GenQuest.LoanChest.Money)));
			AddQuestUserData("Gen_LoanTakeChest", "sDay", FindRussianDaysString(sti(pchar.GenQuest.LoanChest.Time)));
		break;

		case "TakeChest_1":
            dialog.text = "Я весь внимание.";
            link.l1 = "Я решил аннулировать ваш заказ по доставке кредита - это не для меня.";
            link.l1.go = "TakeChestFinish_work_cancel";
            link.l9 = "Нет. Ничего.";
			link.l9.go = "exit";
		break;

		case "TakeChestFinish_work_cancel":
            if (GetCharacterItem(pchar, "Chest") >= sti(pchar.GenQuest.LoanChest.Chest))
            {
                ChangeCharacterReputation(pchar, -1);
                dialog.text = "Плохо, я уже рассчитывал на вас.";
                link.l1 = "Да вы правы, но такова жизнь.";
    			link.l1.go = "exit";
    			TakeNItems(pchar, "Chest", -sti(pchar.GenQuest.LoanChest.Chest));
    			DeleteAttribute(pchar, "GenQuest.LoanChest");
    			DeleteAttribute(Pchar, "quest.LoanChestTimer");
                AddQuestRecord("Gen_LoanTakeChest", "5");
                CloseQuestHeader("Gen_LoanTakeChest");
			}
			else
			{
                dialog.text = "Возможно, я готов прервать выполнение вашей миссии, если вы ВЕРНЕТЕ мне все сундуки.";
                link.l1 = "Зайду позже.";
    			link.l1.go = "Exit";
			}
		break;
		//============== кредиты ===============
		case "loan":
            if (sti(NPChar.nation) == PIRATE)
            {
                dialog.text = "Нет, ничем не могу помочь. В нашем неспокойном месте я могу предложить только услуги хранения денег.";
                if(makeint(Pchar.money) >= 100)
				{
	                Link.l1 = "Хорошо, давайте определимся с суммой.";
	                Link.l1.go = "deposit";
				}
                Link.l2 = "Мне это не интересно.";
				link.l2.go = "exit";
                break;
            }
			// проверка на проф пригодность -->
            if (isBadReputation(pchar, 42))
            {
                dialog.text = RandSwear() + "Ты ведь " + GetFullName(pchar)+ ", не так ли? О тебе " +
                              RandPhraseSimple("ходят ужасные слухи","говорят не лучшим образом") + ", я не хочу иметь с тобой дело.";
				link.l1 = RandPhraseSimple("Эх.. стоит человеку оступиться и все будут в него плевать.", RandSwear() + "Ну и не надо!");
				link.l1.go = "exit";
                break;
            }
            // проверка на проф пригодность <--
			Dialog.snd = "voice\USDI\USDI016";
			dialog.text = "О, всегда пожалуйста. Какая сумма вам необходима?";
			Link.l1 = "Совсем небольшая.";
			Link.l1.go = "Small";
			Link.l2 = "Весьма значительная.";
			Link.l2.go = "Medium";
			Link.l3 = "Чем больше, тем лучше.";
			Link.l3.go = "Large";
		break;

		case "small":
			Pchar.Quest.Loans.(NPC_Area).Sum = 500*makeint(Pchar.rank);
			Dialog.snd = "voice\USDI\USDI017";
			dialog.text = "Отлично. С небольшими суммами всегда проще - меньше риска. Я могу предложить вам " + FindRussianMoneyString(sti(Pchar.Quest.loans.(NPC_Area).sum)) + "...";
			Link.l1 = "Договорились. Под какие проценты?";
			Link.l1.go = "Interest";
			Link.l2 = "Увы, эта сумма не годится. Давайте-ка ее изменим.";
			Link.l2.go = "Loan";
			Link.l3 = "Думаю, мне лучше не влезать в долги. До свидания.";
			Link.l3.go = "ExitDelLoan1";
			Pchar.Quest.Loans.(NPC_Area).Interest = 11 - makeint(GetCharacterSkillToOld(Pchar, "commerce"));
		break;

		case "Medium":
			Pchar.Quest.Loans.(NPC_Area).Sum = 1500*makeint(Pchar.rank);
			Dialog.snd = "voice\USDI\USDI018";
			dialog.text = "Не проблема. Надеюсь, что " + FindRussianMoneyString(sti(Pchar.Quest.Loans.(NPC_Area).sum)) + " помогут вам решить ваши проблемы. Это вполне значительная сумма.";
			Link.l1 = "Договорились. Под какие проценты?";
			Link.l1.go = "Interest";
			Link.l2 = "Увы, эта сумма не годится. Давайте-ка ее изменим.";
			Link.l2.go = "Loan";
			Link.l3 = "Думаю, мне лучше не влезать в долги. До свидания.";
			Link.l3.go = "ExitDelLoan1";
			Pchar.Quest.Loans.(NPC_Area).Interest = 16 - makeint(GetCharacterSkillToOld(Pchar, "commerce"));
		break;

		case "Large":
			Pchar.Quest.Loans.(NPC_Area).Sum = 4000*makeint(Pchar.rank);
			Dialog.snd = "voice\USDI\USDI019";
			dialog.text = "М-да... Рискованно... Но ладно, я готов предоставить вам заем на сумму в " + FindRussianMoneyString(sti(Pchar.Quest.Loans.(NPC_Area).sum)) + ". Как вы понимаете, капитан, это действительно крупные деньги. Очень прошу вас серьезно отнестись к этому делу.";
			Link.l1 = "Договорились. Под какие проценты?";
			Link.l1.go = "Interest";
			Link.l2 = "Увы, эта сумма не годится. Давайте-ка ее изменим.";
			Link.l2.go = "Loan";
			Link.l3 = "Думаю, мне лучше не влезать в долги. До свидания.";
			Link.l3.go = "ExitDelLoan1";
			Pchar.Quest.Loans.(NPC_Area).Interest = 26 - makeint(GetCharacterSkillToOld(Pchar, "commerce"));
		break;

		case "Interest":
			//Pchar.Quest.Loans.(NPC_Area).Interest = 16 - makeint(Pchar.skill.commerce);
			Dialog.snd = "voice\USDI\USDI020";
			dialog.text = Pchar.Quest.Loans.(NPC_Area).Interest + "% в месяц. Основываясь на том, что я о вас знаю, я не могу предложить вам сейчас лучших условий."; 
			Link.l1 = "Меня это вполне устраивает. Осталось обговорить срок.";
			Link.l1.go = "Period";
			Link.l3 = "Думаю, мне лучше не влезать в долги. До свидания.";
			Link.l3.go = "ExitDelLoan1";
		break;

		case "Period":
			Pchar.Quest.Loans.(NPC_Area).Period = makeint(makeint(Pchar.reputation)/20) + 1;			
			Dialog.snd = "voice\USDI\USDI021";
			dialog.text =  "Учитывая то, что я о вас слышал, " + Pchar.name + ", я буду ждать возврата этого кредита " + FindRussianMonthString(sti(Pchar.Quest.Loans.(NPC_Area).Period)) + ". После чего, как вы понимаете, я приму меры."; 
			Link.l1 = "Что же, я с удовольствием принимаю ваши условия... И ваши деньги.";
			Link.l1.go = "LoanGranted";
			Link.l3 = "Увы, мы с вами не договорились. До свидания.";
			Link.l3.go = "ExitDelLoan1";
		break;

		case "LoanGranted":
			Dialog.snd = "voice\USDI\USDI022";
			dialog.text = "Искренне этому рад. Но позвольте мне вас предупредить. Я давно занимаюсь своим делом и знаю, как вернуть вложенные средства. Так что если у вас и были какие мысли прикарманить мои деньги, лучше оставьте их\nНе хочу вас обидеть - просто предупреждаю.";
			Link.l1 = "Хм... Ну-ну. Всего хорошего.";
			Link.l1.go = "LoanGranted_exit";
		break;

		case "Loan_Remind":
			Dialog.snd = "voice\USDI\USDI023";
			dialog.text = "Ну смотрите... Проценты капают. Да и времени у вас осталось не так уж много.";
			Link.l1 = "Не волнуйтесь. До встречи.";
			Link.l1.go = "exit";
		break;

		case "loan_return":
			addMoneyToCharacter(Pchar, -(makeint(Pchar.Quest.Loans.(NPC_Area).Result)));
			// boal 27.01.2004 -->
            if (npchar.quest.trade_date != lastspeak_date) // дыра с прокачкой была
            {
    			AddCharacterExpToSkill(Pchar, "Commerce", makeint(Pchar.Quest.Loans.(NPC_Area).Result) / 350);
    			AddCharacterExpToSkill(Pchar, "Leadership", makeint(Pchar.rank) * 5);
			}
			// boal 27.01.2004 <--
			DeleteAttribute(PChar, "quest.Loans_" + NPC_Area); // bug fix
			DeleteAttribute(PChar, "quest.Loans." + NPC_Area);
			Dialog.snd1 = "voice\USDI\USDI024";
			Dialog.snd2 = "voice\USDI\USDI025";
			Dialog.snd3 = "voice\USDI\USDI026";
			dialog.text = LinkRandPhrase("О, это просто чудесно! В любое время, как вам понадобятся деньги - я к вашим услугам.", "Благодарю. Очень приятно иметь дело с деловым человеком, умеющим вовремя вернуть долг. А то знаете, разное бывает...",  "О, не зря я был в вас столь уверен, " + GetAddress_Form(NPChar) + "! Надеюсь, это не последний раз, когда вы прибегаете к моим услугам!");
			Link.l1 = LinkRandPhrase("Я хочу занять денег.", "Мне срочно понадобилась пара-другая пиастров.", "Как насчет небольшого кредита?");
			Link.l1.go = "loan";
			if(makeint(PChar.money) >= 100)
			{
				Link.l2 = LinkRandPhrase("Я бы хотел отдать деньги в рост.","Примете на хранение пиратскую добычу?", "Золотишко на черный день оставить можно?");
				Link.l2.go = "deposit";				
			}
			Link.l3 = "До свидания, " + NPchar.name + ".";
			Link.l3.go = "ExitDelLoan1";
		break;

		case "deposit":
			Dialog.snd1 = "voice\USDI\USDI027";
			Dialog.snd2 = "voice\USDI\USDI028";
			Dialog.snd3 = "voice\USDI\USDI029";
			dialog.text = LinkRandPhrase("Я слышу разумного человека! Так сколько денег вы готовы отдать в рост?", "Хорошо. Поверьте мне, та сумма, что вы мне отдаете, будет ждать вас в целости и сохранности, и даже с процентами...А, кстати, какова эта сумма?", "Я смотрю, вы понимаете, что к чему в этой жизни. Сколько вы хотите отдать мне на хранение?");
			Link.l1 = "Четверть - " + FindRussianMoneyString(makeint(makeint(PChar.money)/40)*10) + ".";
			Link.l1.go = "quarter";
			Link.l2 = "Половину - " + FindRussianMoneyString(makeint(makeint(PChar.money)/20)*10) + ".";
			Link.l2.go = "half";
			Link.l3 = "Все, что у меня есть - " + FindRussianMoneyString(makeint(makeint(PChar.money)/10)*10) + ".";
			Link.l3.go = "All";
		break;

		case "quarter":
			Pchar.QuestTemp.Deposits.(NPC_Area).Interest = makeint(GetCharacterSkillToOld(Pchar, "commerce")/2.0 + 0.33) + 1;
			Pchar.QuestTemp.Deposits.(NPC_Area).Sum = makeint(makeint(PChar.money)/40)*10;
			Dialog.snd = "voice\USDI\USDI030";
			dialog.text = "Хорошо. Процентов под эту сумму я готов вам предложить...мм...скажем, " + Pchar.QuestTemp.Deposits.(NPC_Area).Interest + ". Разумеется, в месяц.";
			Link.l1 = "Меня это устраивает.";
			Link.l1.go = "Deposit_placed";
			Link.l2 = "Лучше я изменю сумму.";
			Link.l2.go = "Deposit";
			Link.l3 = "Кажется, мне не стоит расставаться с деньгами. Всего хорошего.";
			Link.l3.go = "Exit";
		break;

		case "half":
			Pchar.QuestTemp.Deposits.(NPC_Area).Interest = makeint(GetCharacterSkillToOld(Pchar, "commerce")/2.0 + 0.33) + 1;
			Pchar.QuestTemp.Deposits.(NPC_Area).Sum = makeint(makeint(PChar.money)/20)*10;
			Dialog.snd = "voice\USDI\USDI031";
			dialog.text = "Хорошо. Процентов под эту сумму я готов вам предложить...мм...скажем, " + Pchar.QuestTemp.Deposits.(NPC_Area).Interest + ". Разумеется, в месяц.";
			Link.l1 = "Меня это устраивает.";
			Link.l1.go = "Deposit_placed";
			Link.l2 = "Лучше я изменю сумму.";
			Link.l2.go = "Deposit";
			Link.l3 = "Кажется, мне не стоит расставаться с деньгами. Всего хорошего.";
			Link.l3.go = "Exit";
		break;

		case "All":
			Pchar.QuestTemp.Deposits.(NPC_Area).Interest = makeint(GetCharacterSkillToOld(Pchar, "commerce")/2.0 + 0.33) + 1;
			Pchar.QuestTemp.Deposits.(NPC_Area).Sum = makeint(makeint(PChar.money)/10)*10;
			Dialog.snd = "voice\USDI\USDI032";
			dialog.text = "Хорошо. Процентов под эту сумму я готов вам предложить...мм...скажем, " + Pchar.QuestTemp.Deposits.(NPC_Area).Interest + ". Разумеется, в месяц.";
			Link.l1 = "Меня это устраивает.";
			Link.l1.go = "Deposit_placed";
			Link.l2 = "Лучше я изменю сумму.";
			Link.l2.go = "Deposit";
			Link.l3 = "Кажется, мне не стоит расставаться с деньгами. Всего хорошего.";
			Link.l3.go = "Exit";
		break;

		case "Deposit_placed":
			Dialog.snd = "voice\USDI\USDI033";
			if ( !CheckAttribute(Pchar, "Quest.Deposits." + (NPC_Area)+".Result"))
			{
			    Pchar.Quest.Deposits.(NPC_Area).Result = 0;//fix
			}
			// boal limit
			if ( (sti(Pchar.Quest.Deposits.(NPC_Area).Result) + sti(Pchar.QuestTemp.Deposits.(NPC_Area).Sum)) > sti(NPChar.UsurerDeposit)*1000)
			{
			    dialog.text = "К сожалению, я не могу обеспечить сохранность такой большой суммы. Вы уже храните у меня " +
				        FindRussianMoneyString(MakeMoneyShow(sti(Pchar.Quest.Deposits.(NPC_Area).Result), MONEY_SIGN,MONEY_DELIVER)) +
				        ", а я могу принять от вас в целом не более " +
						FindRussianMoneyString(MakeMoneyShow(sti(NPChar.UsurerDeposit)*1000, MONEY_SIGN,MONEY_DELIVER)) + ".";
				Link.l1 = "Очень жаль, придется обратиться к другому ростовщику.";
				Link.l1.go = "Exit";
			}
			else
			{
				dialog.text = "Теперь вы можете быть уверены, что даже самая жестокая буря не оставит вас без средств к существованию. Если же вы решите забрать вклад, то приходите в любое время.";
				Link.l1 = "Хорошо. Всего хорошего.";
				Link.l1.go = "Deposit_Exit";
			}
		break;
		
		case "Deposit_exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			Pchar.Quest.Deposits.(NPC_Area).Interest = sti(Pchar.QuestTemp.Deposits.(NPC_Area).Interest);
			Pchar.Quest.Deposits.(NPC_Area).Sum      = sti(Pchar.QuestTemp.Deposits.(NPC_Area).Sum);

			AddMoneyToCharacter(Pchar, -(makeint(Pchar.Quest.Deposits.(NPC_Area).Sum)));
			// общий долг
			Pchar.Quest.Deposits.(NPC_Area).Sum = sti(Pchar.Quest.Deposits.(NPC_Area).Result) + sti(Pchar.Quest.Deposits.(NPC_Area).Sum);
			Pchar.Quest.Deposits.(NPC_Area) = true;
			Pchar.Quest.Deposits.(NPC_Area).StartDay = getDataDay();
			Pchar.Quest.Deposits.(NPC_Area).StartMonth = getDataMonth();
			Pchar.Quest.Deposits.(NPC_Area).StartYear = getDataYear();
			Pchar.Quest.Deposits.(NPC_Area).StartTime = getTime();
			DialogExit();
		break;

		case "Deposit_return":
			Dialog.snd = "voice\USDI\USDI034";
			dialog.text = "Исходя из процентов, которые мы оговорили в прошлый раз, и учитывая прошедшее время, я подсчитал, что должен вам " + FindRussianMoneyString(sti(Pchar.Quest.Deposits.(NPC_Area).Result)) + "... Вы уверены, что хотите забрать эти деньги";
			Link.l1 = "Абсолютно. Давайте их сюда.";			
			Link.l1.go = "Deposit_return_1";		
			Link.l2 = "Знаете, вы правы. Пусть они еще немного полежат. Всего хорошего.";			
			Link.l2.go = "Exit";		
		break;

		case "Deposit_return_1":
			addMoneyToCharacter(Pchar, makeint(Pchar.Quest.Deposits.(NPC_Area).Result));
			Dialog.snd = "voice\USDI\USDI035";
			dialog.text = "Ох... Даже жаль с ними расставаться. Я к ним уже как-то привык. Что же - приходите ко мне еще.";			
			Link.l1 = "Если понадобится - приду. Счастливо оставаться.";			
			Link.l1.go = "Exit";
			DeleteAttribute(Pchar, "quest.Deposits." + (NPC_Area));
		break;

		case "DeadMotherfucker":
			Dialog.TempNode = "DeadMotherFucker_1";
			Dialog.snd = "voice\USDI\USDI035";
			dialog.text = "Вы еще живы? Наверное, хотите вернуть мне деньги? К черту! Из-за вас я потерял столько, сколько вы мне в жизни не возместите! Я не успокоюсь, пока не увижу ваш труп!";
            		Link.l1 = "Я как раз хочу уладить наши дела мирно.";
			Link.l1.go = "LoanRestore_1";
			Link.l2 = LinkRandPhrase("Ну давай, посмотрим! Только учти, что ты и сам легко можешь стать трупом!", "Угрожаешь мне?! Ну хорошо, посмотрим, что ты предпримешь...", "Деньги - не главное в жизни. Так что не стоит тебе ради них так убиваться самому и убивать других.");
			Link.l2.go = "ExitDelLoan2";
		break;

		case "DeadMotherFucker_1":			
			Dialog.snd = "voice\USDI\USDI035";
			dialog.text = "Убирайтесь из моего дома! Глаза мои вас бы не видели!";			
			Link.l1 = "...";			
			Link.l1.go = "ExitDelLoan2";
            Dialog.TempNode = "DeadMotherFucker_1";
		break;
		// boal обратимость факапства -->
		case "LoanRestore_1":
            iPastMonths = GetPastTime("Month", makeint(Pchar.Quest.Loans.(NPC_Area).StartYear),makeint(Pchar.Quest.Loans.(NPC_Area).StartMonth),makeint(Pchar.Quest.Loans.(NPC_Area).StartDay), makefloat(Pchar.Quest.Loans.(NPC_Area).StartTime), getDataYear(),getDataMonth(),GetDataDay(), GetTime());
			Pchar.Quest.Loans.(NPC_Area).Result = makeint(Pchar.Quest.Loans.(NPC_Area).Sum) + ((makeint(Pchar.Quest.Loans.(NPC_Area).Sum)/100)*makeint(Pchar.Quest.Loans.(NPC_Area).Interest))*(iPastMonths+1);
			dialog.text = "Да? И что вы предлагаете?";
			Link.l1 = "На сегодня я должен вам сумму в " + FindRussianMoneyString(sti(Pchar.Quest.Loans.(NPC_Area).Result)) + ". Предлагаю ее вам.";
			Link.l1.go = "DeadMotherFucker_1";
            if (sti(Pchar.Money) >= 2*sti(Pchar.Quest.Loans.(NPC_Area).Result))
            {
    			Link.l2 = "Чтобы наладить отношения, предлагаю вернуть вам сумму долга в размере " + FindRussianMoneyString(2*sti(Pchar.Quest.Loans.(NPC_Area).Result)) + ".";
    			Link.l2.go = "LoanRestore_2";
			}
		break;
		
		case "LoanRestore_2":
			dialog.text = "Это другое дело! С вами все же можно иметь дела.";
			Link.l1 = "Спасибо. Больше не подведу.";
			Link.l1.go = "ExitDelLoan2";
			NextDiag.TempNode = "First time";			
			addMoneyToCharacter(Pchar, -2*sti(Pchar.Quest.Loans.(NPC_Area).Result));
			DeleteAttribute(PChar, "quest.Loans_" + NPC_Area);
			DeleteAttribute(PChar, "quest.Loans." + NPC_Area);
			ChangeCharacterHunterScore(PChar, NationShortName(sti(NPChar.nation)) + "hunter", -30);
			ChangeCharacterReputation(pchar, 2);
		break;

		case "ExitDelLoan1":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
			DeleteAttribute(Pchar, "quest.Loans." + (NPC_Area));
		break;

		case "ExitDelLoan2":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
			DeleteAttribute(Pchar, "quest.Loans." + (NPC_Area));
			DeleteAttribute(Pchar, "quest.Deposits." + (NPC_Area));
		break;
		
        case "LoanGranted_exit":
			NextDiag.CurrentNode = NextDiag.TempNode;

			AddMoneyToCharacter(Pchar, makeint(Pchar.Quest.Loans.(NPC_Area).Sum));
			Pchar.Quest.Loans.(NPC_Area) = true;
			Pchar.Quest.Loans.(NPC_Area).StartDay = getDataDay();
			Pchar.Quest.Loans.(NPC_Area).StartMonth = getDataMonth();
			Pchar.Quest.Loans.(NPC_Area).StartYear = getDataYear();
			Pchar.Quest.Loans.(NPC_Area).StartTime = getTime();

			sTemp = "Loans_" + NPC_Area;			
			SetTimerCondition(sTemp, 0, makeint(Pchar.Quest.Loans.(NPC_Area).Period), 0, false);
			pchar.quest.(sTemp).CityId = NPC_Area;
			pchar.quest.(sTemp).win_condition = "LoansForAll";
			
			DialogExit();
			npchar.quest.trade_date = lastspeak_date; // дыра с прокачкой была
		break;
		//поиски драгоценного камня за гордом
		case "usurersJewel_1":
			sld = ItemsFromID("UsurersJew");
			switch (rand(3))
			{
				case 0: 
					sld.JevelType = "бриллиант";  
					sld.model = "Diamond";
					sld.picIndex = 2;
				break;
				case 1: 
					sld.JevelType = "рубин";  
					sld.model = "Ruby";
					sld.picIndex = 3;
				break;
				case 2: 
					sld.JevelType = "изумруд";  
					sld.model = "Emerald";
					sld.picIndex = 4;
				break;
				case 3: 
					sld.JevelType = "сапфир";  
					sld.model = "Sapphire";
					sld.picIndex = 1;
				break;
			}			
			dialog.text = "Вы знаете, со мной случилось огромное несчастье. Я потерял " + sld.JevelType + " прекрасной работы. Но самое неприятное даже не это. Самое плохое, что камень - не мой!";
			link.l1 = "Ну и что? Купите другой " + sld.JevelType + " и дело с концом.";
			link.l1.go = "usurersJewel_2";
			npchar.quest.usurersJewel.type = sld.JevelType; //тип камня
			npchar.quest.usurersJewel.money = (rand(6)+2) * 500; //вознаграждение
		break;
		case "usurersJewel_2":
			dialog.text = "Эх, если бы все было так просто! Дело в том, что этот " + npchar.quest.usurersJewel.type + " уникален в своем роде. Этот камень передал мне заказчик для изготовления перстня. Подобрать что-нибудь подобное по размеру и форме я не имею возможности...";
			link.l1 = "М-да, это действительно проблема. Ну, и чем же я могу вам помочь?";
			link.l1.go = "usurersJewel_3";
		break;
		case "usurersJewel_3":
			dialog.text = "Найдите " + npchar.quest.usurersJewel.type + ", только и всего!";
			link.l1 = "А где вы его потеряли?";
			link.l1.go = "usurersJewel_4";
		break;
		case "usurersJewel_4":
			dialog.text = "В джунглях, сразу за городом. Я иногда люблю прогуляться там в тишине. Это успокаивает...";
			link.l1 = "Понимаете, я человек несколько другой профессии. Я не сыщик и не лакей. Поиски потерянных вами вещей мне не интересны.";
			link.l1.go = "usurersJewel_out";
			link.l2 = "Гм, ну что же, я готов взяться за поиски потерянного камня. Каково вознаграждение за работу?";
			link.l2.go = "usurersJewel_5";
		break;

		case "usurersJewel_out":
			dialog.text = "М-да, очень жаль. Видимо, я в вас ошибся. Прошу меня извинить...";
			link.l1 = "Да не проблема.";
			link.l1.go = "exit";
			DeleteAttribute(npchar, "quest.usurersJewel");
			ChangeCharacterReputation(pchar, -1);
		break;
		case "usurersJewel_5":
			dialog.text = "Если вы найдете утерянный " + npchar.quest.usurersJewel.type + ", то я хорошо заплачу. Хочу заметить, чте время терпит.";
			link.l1 = "Хм, это хорошо. Ну что же, я отправляюсь на поиски. Ожидайте результата.";
			link.l1.go = "exit";
			pchar.questTemp.different = "TakeMayorsRing";
			//ложим камень
			sld = ItemsFromID("UsurersJew");
			sld.shown = true;
			sld.startLocation = npchar.city + "_ExitTown";
			sld.startLocator = "item" + (rand(14)+1);
			sld.CityName = XI_ConvertString("Colony" + npchar.city + "Gen");
			Log_QuestInfo("Камень находится в локаторе " + sld.startLocator);
			SetTimerFunction("SmallQuests_free", 0, 0, 1); //освобождаем разрешалку на миниквесты 
			sTitle = npchar.city + "UrurersJewel";
			AddQuestRecordEx(sTitle, "SeekUrurersJewel", "1");
			AddQuestUserDataForTitle(sTitle, "sCity", XI_ConvertString("Colony" + npchar.city + "Gen"));
			AddQuestUserDataForTitle(sTitle, "sJewType", npchar.quest.usurersJewel.type);
			AddQuestUserData(sTitle, "sCity", XI_ConvertString("Colony" + npchar.city + "Gen"));
			AddQuestUserData(sTitle, "sName", GetFullName(npchar));
			AddQuestUserData(sTitle, "sJewType", npchar.quest.usurersJewel.type);
		break;

		case "usurersJewel_R1":
			dialog.text = "Прекрасно! Да-да, это он, тот самый потерянный " + npchar.quest.usurersJewel.type + "... Ну что же, я доволен, очень доволен. Я готов выплатить вам награду за работу.";
			link.l1 = "Очень хорошо.";
			link.l1.go = "usurersJewel_R2";
			TakeItemFromCharacter(pchar, "UsurersJew");
		break;
		case "usurersJewel_R2":
			dialog.text = "Спасибо вам еще раз. Вы можете в любое время обращаться ко мне, я всегда выслушаю вас.";
			link.l1 = "Спасибо, чего уж...";
			link.l1.go = "exit";
			ChangeCharacterReputation(pchar, 2);
			AddCharacterExpToSkill(pchar, "Leadership", 15);
			AddCharacterExpToSkill(pchar, "Fortune", 25);
			AddMoneyToCharacter(pchar, sti(npchar.quest.usurersJewel.money));
			sTitle = npchar.city + "UrurersJewel";
			CloseQuestHeader(sTitle);
			DeleteAttribute(npchar, "quest.usurersJewel");
		break;
	}
}

int findCitizenMan(ref NPChar, bool bCity)
{
    ref ch;
	int n, nation;
    int storeArray[MAX_COLONIES];
    int howStore = 0;

	if (bCity && sti(Pchar.Ship.Type) == SHIP_NOTUSED)
	{
		ch = GetCharacter(NPC_GenerateCharacter("LoanFindingMan", "citiz_"+(rand(11)+1), "man", "man", 10, sti(npchar.nation), -1, false));
		ch.dialog.filename = "Common_citizen.c";
		ch.city = npchar.city;
		ch.RebirthPhantom = true; 
		LAi_NoRebirthDisable(ch);
		LAi_SetCitizenType(ch);
		LAi_group_MoveCharacter(ch, GetNationNameByType(sti(npchar.nation)) + "_citizens");
		PlaceCharacter(ch, "goto", npchar.city + "_town");
	}
	else
	{		
		for(n=0; n<MAX_COLONIES; n++)
		{			
			nation = GetNationRelation(sti(npchar.nation), sti(colonies[n].nation));	
			if (nation != RELATION_ENEMY && colonies[n].nation != "none")
			{           
				storeArray[howStore] = n;
				howStore++;
			}
		}
		if (howStore == 0) return -1; 
		nation = storeArray[rand(howStore-1)];
		ch = GetCharacter(NPC_GenerateCharacter("LoanFindingMan", "citiz_"+(rand(11)+1), "man", "man", 10, sti(colonies[nation].nation), -1, false));
		ch.dialog.filename = "Common_citizen.c";
		ch.city = colonies[nation].id;
		ch.RebirthPhantom = true; 
		LAi_NoRebirthDisable(ch);
		LAi_SetCitizenType(ch);
		LAi_group_MoveCharacter(ch, GetNationNameByType(sti(colonies[nation].nation)) + "_citizens");
		PlaceCharacter(ch, "goto", colonies[nation].id + "_town");
	}
	return sti(ch.index);
}

int findChestMan(ref NPChar)
{
    ref ch;
	int n;
    int storeArray[TOTAL_CHARACTERS];
    int howStore = 0;
	string sTemp, sCity;

	for(n=2; n<MAX_CHARACTERS; n++)
	{
		makeref(ch,Characters[n]);
		sTemp = ch.id;
		if (CheckAttribute(ch, "City") && ch.id != "Jackman") sCity = ch.City;
		else continue;
		sTemp = strcut(sTemp, strlen(sCity)+1, strlen(sTemp)-1);
		// магазины
		if (sTemp == "trader")
		{
            if (NPChar.city == ch.city) continue;
            if (ch.location == "none") continue;
            storeArray[howStore] = n;
            howStore++;
		}
        // мэры
		if (sTemp == "Mayor")
		{
            if (NPChar.city == ch.city) continue;
            if (sti(ch.nation) == PIRATE) continue; // пираты не имеют реплик
            if (ch.location == "none") continue;
			if (ch.location != ch.Default) continue; //захвачанных мэров не надо
            storeArray[howStore] = n;
            howStore++;
		}
        // верфисты
		if (sTemp == "shipyarder")
		{
            if (NPChar.city == ch.city) continue;
            if (ch.location == "none") continue;
            storeArray[howStore] = n;
            howStore++;
		}
		// тавернщики
		if (sTemp == "tavernkeeper")
		{
            if (NPChar.city == ch.city) continue;
            if (ch.location == "none") continue;
            storeArray[howStore] = n;
            howStore++;
		}
	    // церковники
		if (sTemp == "Priest")
		{
            if (NPChar.city == ch.city) continue;
            if (ch.location == "none") continue;
            storeArray[howStore] = n;
            howStore++;
		}		
		// ростовщики
		if (sTemp == "usurer")
		{
            if (NPChar.city == ch.city) continue;
            if (ch.location == "none") continue;
            storeArray[howStore] = n;
            howStore++;
		}
		// начальники портов
		if (sTemp == "PortMan")
		{
            if (NPChar.city == ch.city) continue;
            if (ch.location == "none") continue;
            storeArray[howStore] = n;
            howStore++;
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
