// Секретать Моргана и актер для сложных диалогов с действиями.
void ProcessDialogEvent()
{
	ref NPChar, sld;
	aref Link, NextDiag;

	DeleteAttribute(&Dialog,"Links");

	makeref(NPChar,CharacterRef);
	makearef(Link, Dialog.Links);
	makearef(NextDiag, NPChar.Dialog);

    sld = characterFromID("Henry Morgan");
    
	switch(Dialog.CurrentNode)
	{
		// ----------------------------------- Диалог первый - первая встреча
		case "First time":
            if (sld.location == pchar.location && bWorldAlivePause)
            {
    			dialog.text = "Если ты к адмиралу, то пройди чуть дальше. Он сейчас в резиденции.";
    			link.l1 = "Хорошо, пойду поговорю с ним.";
            }
            else
            {
    			if (pchar.questTemp.piratesLine == "Panama_inEngland" || pchar.questTemp.piratesLine == "Panama_backToShip")
				{
					dialog.text = "Адмирал отправился в Лондон. Он будет здесь не раньше, чем через год.";
    				link.l1 = "Понятно...";
				}
				else
				{
					dialog.text = "Адмирала нет на месте - отсутствует по делам. А сейчас не доставай меня, я очень занят.";
    				link.l1 = "Если Моргана нет, то мне в этом доме делать нечего.";
				}
            }
			link.l1.go = "exit";
			NextDiag.TempNode = "First time";

			if (npchar.quest.meeting == "0")
			{
				dialog.text = "Меня зовут " + GetFullName(npchar) + ". Я секретать Генри Моргана.";
				link.l1 = "Мое имя " + GetFullName(pchar) + ".";
				link.l1.go = "First time";
				npchar.quest.meeting = "1";
			}
            //============>>>>>>>>> перехват квестовых диалогов
            if (pchar.questTemp.State == "WormEnglPlans_toMorgan")  // HolLine_3, переадресовка в Антигуа.
            {
    			link.l1 = "Послушай, мне нужен Морган, и очень срочно.";
    			link.l1.go = "Step_H3_1";
            }
            if (pchar.questTemp.State == "Fr8ThreeCorsairs_toSeek")  // FraLine_8, переадресовка в Антигуа.
            {
    			link.l1 = "Месье, я хотел бы поговорить с Генри Морганом. Где я могу его застать?";
    			link.l1.go = "Step_F8_1";
            }
            // квест Аскольда. Пасхалка на получение индейских предметов.
            if (pchar.questTemp.Ascold == "SeekThreeObject" && !CheckAttribute(pchar, "questTemp.Ascold.Oliver"))
            {
    			link.l2 = "Послушай, ты дела Моргана ведешь. Не встречались ли тебе в призовых бухгалтерских отчетах различные индейские предметы?";
    			link.l2.go = "Step_As_1";
            }
            if (pchar.questTemp.Ascold == "SeekThreeObject" && CheckAttribute(pchar, "questTemp.Ascold.Oliver"))
            {
    			link.l2 = "Оливер, может, ты передумаешь насчет цены тех индейских предметов, о которых я тебе говорил?";
    			link.l2.go = "Step_As_7";
            }
            if (pchar.questTemp.Azzy == "DestrContract" || pchar.questTemp.Azzy == "2DestrContract")
            {
                if (!CheckAttribute(pchar, "questTemp.Azzy.Oliver"))
                {
        			link.l2 = "Оливер, послушай, хочу у тебя попросить помощи. Мне позарез нужны индейские побрякушки. Речь идет о трех хрустальных черепах тольтеков и странной вещи, похожей на погремушку.";
        			link.l2.go = "Step_As_9";
        		}
            }
            //============>>>>>>>>> перехват квестовых диалогов
        break;

        case "Exit":
            NextDiag.CurrentNode = NextDiag.TempNode;
            DialogExit();
        break;
		//********************* Голландская линейка *********************
        case "Step_H3_1":
            dialog.text = "А кто ты такой и зачем тебе понадобился Генри?";
            link.l1 = "Меня зовут " + GetFullName(pchar) + ", к Моргану меня отправил Эдвард Мэнсфилд.";
            link.l1.go = "Step_H3_2";
        break;
        case "Step_H3_2":
            dialog.text = "Голландец?... Ты - голландец?";
            link.l1 = "Какое тебе дело, какой я нации? У меня дело к Моргану от Мэнсфилда, чего тебе еще не ясно?";
            link.l1.go = "Step_H3_3";
        break;
        case "Step_H3_3":
            dialog.text = "Не кипятись, минхер, все нормально. Голландцев среди пиратов много, а вот в Порт Рояле тебе сейчас находиться небезопасно.\n"+
                          "Поэтому вот что я тебе скажу: Морган на Антигуа, в своем доме. Там тоже будь аккуратнее - если я в тебе признал голландца, то это не будет тайной и для других.\n"+
                          "И еще - в дом к Моргану так просто не попасть, дверь работает только на выход, хе-хе... Ищи подземелье под Сент-Джонсом, оно приведет тебя к Генри...";
            link.l1 = "Спасибо, Оливер.";
            link.l1.go = "exit";
            pchar.questTemp.State = "WormEnglPlans_toMorganAntigua";
            AddQuestRecord("Hol_Line_3_WormEnglishPlans", "3");
        break;
		//********************* Французская линейка *********************
 	 	case "Step_F8_1":
			dialog.text = NPCStringReactionRepeat("Кто вы такой, сэр?", "Я уже ответил вам...", 
				          "Повторяю, я уже ответил вам на этот вопрос.", "Вам не стоит надоедать мне, я более ничего не скажу.", "block", 0, npchar, Dialog.CurrentNode);
			link.l1 = HeroStringReactionRepeat("Меня зовут " + GetFullName(pchar) + ", я действую по поручения французского правительства.", "Да, верно...", "Да, я помню...", "Хорошо.", npchar, Dialog.CurrentNode);
			link.l1.go = DialogGoNodeRepeat("Step_F8_2", "none", "none", "none", npchar, Dialog.CurrentNode);
        break;  
        case "Step_F8_2":
            dialog.text = "А по какому делу французскому правительству понадобился Генри Морган?";
            link.l1 = "Послушай, милейший, тебе не кажется, что для секретаря ты слишком разговорчив?";
            link.l1.go = "Step_F8_3";
        break;
        case "Step_F8_3":
            dialog.text = "Ладно, ладно, месье... Морган сейчас находится в своем доме на Антигуа.";
            link.l1 = "Ну вот и славно. Немедленно отправляюсь туда!";
            link.l1.go = "Step_F8_4";
        break;
        case "Step_F8_4":
            dialog.text = "Месье, имейте в виду, что в дом к Моргану в Сент-Джонсе так просто не попасть, дверь там всегда на замке. Вход - через подземелье...";
            link.l1 = "Разберемся...";
            link.l1.go = "exit";
        break;
		//********************* Квест Асокльда *********************
   		case "Step_As_1":
			dialog.text = "Что за вопросы такие странные?! Может, и доводилось, а почему я тебе об этом должен говорить?";
			link.l1 = "Ты не кипятись. Все дело в том, что я ищу три предмета индейского культа. Для покупки. Все одно - вы либо выбрасываете эту дрянь, либо продаете за бесценок. Я готов заплатить щедро за эти вещи, если они у вас есть.";
			link.l1.go = "Step_As_2";
		break;
   		case "Step_As_2":
			dialog.text = "А что за вещи?";
			link.l1 = "Индейская статуэтка бога ацтеков Тлалока, церемониальный сосуд и церемониальный нож.";
			link.l1.go = "Step_As_3";
		break;
   		case "Step_As_3":
			dialog.text = "Хех, все эти вещи - из драгоценных металлов. Мы переплаваляем их в слитки и отправляем в Европу ювелирам для работы. Большой спрос, знаешь ли, хорошие цены...";
			link.l1 = "Я понимаю. Я готов заплатить две цены за эти индейские побрякушки, только в непереплавленном виде, так сказать... А вообще, эти вещи у тебя есть? Или мы бестолковый разговор ведем?";
			link.l1.go = "Step_As_4";
		break;
   		case "Step_As_4":
			dialog.text = "Эти вещи у меня есть. И, в принципе, я готов буду тебе их продать. Но только две цены - не предмет для серьезного разговора. Слишком мало.";
			link.l1 = "И сколько ты хочешь?";
			link.l1.go = "Step_As_5";
		break;
   		case "Step_As_5":
			dialog.text = "Миллион.";
			link.l1 = "Что?!!! Ты рехнулся?!";
			link.l1.go = "Step_As_6";
		break;
   		case "Step_As_6":
			dialog.text = "Нисколько. Ты что думаешь, за какие-то две цены, размером в три тысячи, я буду из приза Моргана изымать что-то? Это ты рехнулся, друг мой, если не знаешь, чем это может для меня кончиться. Не нравится - иди в магазины, попробуй там купить.";
			link.l1 = "Черт возьми, Оливер!";
			link.l1.go = "Step_As_7";
			pchar.questTemp.Ascold.Oliver = 1;
		break;
   		case "Step_As_7":
			dialog.text = "Ладно, хватит болтать. Берешь или нет?";
			link.l1 = RandPhraseSimple("Да ни за что! За такие деньги я весь Порт Рояль куплю!", "За миллион? Да ну что ты! Нет, конечно.");
			link.l1.go = "exit";
            if (sti(pchar.money) >= 1000000)
            {
    			link.l2 = "Эх, беру! И заверни мне это в подарочную упаковку!";
    			link.l2.go = "Step_As_8";
            }
            else
            {
    			link.l2 = "Я бы взял, да таких денег нет. Пойду, может заработаю еще...";
    			link.l2.go = "exit";
            }
		break;
   		case "Step_As_8":
			dialog.text = GetFullName(pchar) + ", с тобой приятно иметь дело. Получай свои бесценные артефакты! Надеюсь, они принесут тебе удачу, раз ты готов платить на них такие деньги...";
			link.l1 = "Я тоже надеюсь, Оливер.";
			link.l1.go = "exit";
			AddMoneyToCharacter(pchar, -1000000);
			DeleteAttribute(pchar, "questTemp.Ascold.Oliver");
			pchar.questTemp.Ascold = "SoughtThreeObject";
    		GiveItem2Character(pchar, "indian19");
    		GiveItem2Character(pchar, "indian20");
    		GiveItem2Character(pchar, "indian21");
		break;
   		case "Step_As_9":
			dialog.text = "Есть у меня такие предметы. Пять миллионов - и они твои.";
			link.l1 = RandPhraseSimple("Пять миллионов?!!", "Пять миллионов... Хорошая сумма!");
			link.l1.go = "exit";
            if (sti(pchar.money) >= 5000000)
            {
    			link.l2 = "Похоже, что у меня нет другого выхода. Надо брать. Забирай деньги и давай поскорей предметы!";
    			link.l2.go = "Step_As_10";
            }
            else
            {
    			link.l2 = "Они мне очень нужны, но таких денег у меня нет...";
    			link.l2.go = "exit";
            }
		break;		
   		case "Step_As_10":
			dialog.text = GetFullName(pchar) + ", с тобой приятно иметь дело. Получай свои бесценные артефакты!";
			link.l1 = "Спасибо, Оливер! Ты мне буквально жизнь спас...";
			link.l1.go = "exit";
			AddMoneyToCharacter(pchar, -5000000);
    		GiveItem2Character(pchar, "sculMa1");
    		GiveItem2Character(pchar, "sculMa2");
    		GiveItem2Character(pchar, "sculMa3");
    		GiveItem2Character(pchar, "indian22");
    		pchar.questTemp.Azzy.Oliver = 1;
		break;

		case "PL_Q8_over":
			dialog.text = "Приветствую капитана " + GetFullName(pchar) + ".";
			link.l1 = "Слушай, Оливер, мне нужне Морган!";
			link.l1.go = "PL_Q8_over_1";
		break;
		case "PL_Q8_over_1":
			dialog.text = "А его нет, он ушел на Панаму без тебя. А тебе велено передать, что ты можешь загорать на пляжах.";
			link.l1 = "Чего?!";
			link.l1.go = "PL_Q8_over_2";
		break;
		case "PL_Q8_over_2":
			dialog.text = "Того! Перевожу специально для тебя. Ты не успел к сроку, и теперь ты не нужен. Прости...";
			link.l1 = "Дьявол!!! Да ладно, сам виноват...";
			link.l1.go = "exit";
			NextDiag.TempNode = "First time";
		break;
	}
}


