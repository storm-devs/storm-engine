// диалог по городам
void ProcessCommonDialogEvent(ref NPChar, aref Link, aref NextDiag)
{
    switch (Dialog.CurrentNode)
	{
		case "quests":
			dialog.text = NPCStringReactionRepeat(RandPhraseSimple("Какие вопросы?", "Что вам угодно, " + GetAddress_Form(NPChar) + "?"), "Совсем недавно вы пытались задать мне вопрос, " + GetAddress_Form(NPChar) + "...", "В течение этого дня вы уже третий раз говорите о каком-то вопросе...",
                          "Опять вопросы будем задавать?", "block", 1, npchar, Dialog.CurrentNode);
			link.l1 = HeroStringReactionRepeat(RandPhraseSimple("Я передумал...", "Сейчас мне не о чем говорить"), "Хм, что-то с памятью моей стало...",
                      "Да уж, действительно в третий раз...", "Да нет, какие вопросы...", npchar, Dialog.CurrentNode);
			link.l1.go = "exit";
			// ==> Проверяем поле состояния квестов.
            if (pchar.questTemp.State == "SeekBible_toBermudes") 
			{				
                link.l1 = HeroStringReactionRepeat("Послушай, я ищу пирата, который продавал здесь Библию какую-то странную. Не слышал об этом деле?",
                "Я про Библию хотел спросить... уже выяснил, извини.", "Да уж, верно...", "А-а-а, разобрались уже, извини...", npchar, Dialog.CurrentNode);
                link.l1.go = DialogGoNodeRepeat("Step_H8_1", "", "", "", npchar, Dialog.CurrentNode);
            }
            if (pchar.RomanticQuest == "BrotherIsDead") 
			{				
                link.l1 = HeroStringReactionRepeat("Слушай, ты не в курсе, где Атилла сейчас обретается?",
                "Да, да, про Атиллу говорили...", "Да уж, верно...", "А-а-а, разобрались уже, извини...", npchar, Dialog.CurrentNode);
                link.l1.go = DialogGoNodeRepeat("Step_Romantic_1", "", "", "", npchar, Dialog.CurrentNode);
            }
			if (pchar.questTemp.BlueBird == "toBermudes")
			{
				link.l1 = "Послушай, приятель, ничего не слышно в таверне интересного про щебеку 'Синяя Птица'?";
				link.l1.go = "BlueBird_1";
			}
			if (pchar.questTemp.piratesLine == "KillLoy_toBermudes")
			{
				link.l1 = "Здесь, случайно, не появлялся Эдвард Лоу?";
				link.l1.go = "PL_Q3_1";
			}
			if (pchar.questTemp.piratesLine == "KillLoy_toTavernAgain" || pchar.questTemp.piratesLine == "KillLoy_2toTavernAgain")
			{
				link.l1 = "Я ищу Эдварда Лоу.";
				link.l1.go = "PL_Q3_2";
			}
			if (pchar.questTemp.piratesLine == "PL5Hunter_toBermudes")
			{
				link.l1 = "К тебе в таверну, Джон Эйвори не забегал?";
				link.l1.go = "StepPL5Berm_1";
			}
            // <== Проверяем поле состояния квестов.
		break;
		//щебека Синяя Птица
		case "BlueBird_1":
			dialog.text = NPCStringReactionRepeat("Слышно-то много всякого. В основном говорят, что корабль очень быстрый. Поймать пытались, да куда там...", 
				"Мы уже говорили об этом.", 
				"Опять? Уже все выяснили.",
                "Простите, если вам больше ничего не надо, то, пожалуй, займусь другими клиентами.", "block", 1, npchar, Dialog.CurrentNode);
			link.l1 = HeroStringReactionRepeat("А кто пытался поймать?", 
				"Да, точно...",
                "Хм, верно.", 
				"Хорошо...", npchar, Dialog.CurrentNode);
			link.l1.go = DialogGoNodeRepeat("BlueBird_2", "exit", "exit", "exit", npchar, Dialog.CurrentNode);
		break;

		case "BlueBird_2":
			dialog.text = "Многие пытались, да обломались, ха-ха!!";
			link.l1 = "А что-нибудь путное тебе известно? Где ее видят часто, к примеру.";
			link.l1.go = "BlueBird_3";
		break;
		case "BlueBird_3":
			dialog.text = "Дык здесь и видят, у Бермуд. По хождению в наших рифах 'Синей Птице' равных нет, не раз в погоне за ней галеоны да фрегаты бились - и прямиком на дно!";
			link.l1 = "Вот значит как... Ну что же, будем искать. Слушай, а хозяин магазина местный, он что за человек?";
			link.l1.go = "BlueBird_4";
		break;
		case "BlueBird_4":
			dialog.text = "Авторитетный весьма, уважаемый среди торговцев.";
			link.l1 = "Понятно... А есть человек, который с ним не в очень хороших отношениях?";
			link.l1.go = "BlueBird_5";
		break;
		case "BlueBird_5":
			dialog.text = "Откровенных врагов у него нет. Но вот бывший хозяин магазина, думаю, зуб на него имеет, хе-хе...";
			link.l1 = "Не будешь ли ты любезен, милейший, назвать мне его имя.";
			link.l1.go = "BlueBird_6";
		break;
		case "BlueBird_6":
			dialog.text = "Его зовут Джон Нортон. На Бермудах ты его уже не найдешь. Слышал я, что он перебрался в Пуэрто-Принсипе, торгует там себе потихоньку и никуда не высовывается.";
			link.l1 = "Большое тебе спасибо, приятель. Ты - хороший трактирщик.";
			link.l1.go = "BlueBird_7";
		break;
		case "BlueBird_7":
			dialog.text = "Спасибо за комплимент, " + GetAddress_Form(NPChar) + ". Удачи вам!";
			link.l1 = "И тебе всего хорошего, приятель.";
			link.l1.go = "exit";
			AddQuestRecord("Xebeca_BlueBird", "3");
			pchar.questTemp.BlueBird = "toPuertoPrincipe";
		break;

		//голл. линейка, квест №8
 		case "Step_H8_1":
    		dialog.text = "Нет, не слышал.";
    		link.l1 = "Вроде история известная, мне на Кюрасао ее рассказали.";
    		link.l1.go = "Step_H8_2";
 		break;
 		case "Step_H8_2":
    		dialog.text = "Ничего об этом не знаю, у меня в таверне такого не было.";
    		link.l1 = "Спасибо за помощь.";
    		link.l1.go = "exit";
 		break;
		//Изабелла, ищем Атиллу
 		case "Step_Romantic_1":
    		dialog.text = "Знаю, он дом купил здесь, в городе. Так что там и должен быть.";
    		link.l1 = "Спасибо, приятель...";
    		link.l1.go = "exit";
 		break;
		//Пиратка, квест №3, поиски Лоу
		case "PL_Q3_1":
			dialog.text = "Появлялся. Все ходил, чего-то вынюхивал. То к одному подсядет, то к другому... Кажется, он интересовался нашей верфью.";
			link.l1 = "Благодарю.";
			link.l1.go = "exit";
			AddQuestRecord("Pir_Line_3_KillLoy", "9");
			pchar.questTemp.piratesLine = "KillLoy_toAlexus";			
		break;

		case "PL_Q3_2":
			dialog.text = "Так вы уже про него спрашивали.";
			link.l1 = "Может, вы чего вспомнили?";
			link.l1.go = "PL_Q3_3";
		break;
		case "PL_Q3_3":
			dialog.text = "Да, чего тут вспоминать, он все про верфь выспрашивал, к людям разным подсаживался. Вон, кстати, Капитан Гудли сидит, он с ним последний разговаривал.";
			link.l1 = "Капитан Гудли?! Спасибо, уже кое-что.";
			link.l1.go = "exit";
			pchar.questTemp.piratesLine = "KillLoy_toGoogly";
		break;
		//Пиратка, квест №5, ОЗГ
		case "StepPL5Berm_1":
			dialog.text = "Да вот, только что и забегал. Спросил, где живет Орри Брюс, и ушел. ";
			link.l1 = "И где же этот Орри Брюс обитает?";
			link.l1.go = "StepPL5Berm_2";
		break;
		case "StepPL5Berm_2":
			dialog.text = "Так, в двухэтажном доме с каменной кладкой он живет. Как выйдешь из таверны, налево первый дом. Там и обитает...";
			link.l1 = "Спасибо, ты мне здорово помог!";
			link.l1.go = "exit";
			AddQuestRecord("Pir_Line_5_Hunter", "10");
			pchar.questTemp.piratesLine = "PL5Hunter_toOrryHouse";
		break;


	}
	UnloadSegment(NPChar.FileDialog2);  // если где-то выход внутри switch  по return не забыть сделать анлод
}

