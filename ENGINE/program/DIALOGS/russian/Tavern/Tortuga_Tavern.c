// диалог по городам
void ProcessCommonDialogEvent(ref NPChar, aref Link, aref NextDiag)
{
    switch (Dialog.CurrentNode)
	{
		case "quests":
			dialog.text = NPCStringReactionRepeat("Все сплетни города "+ GetCityName(npchar.city) +" к вашим услугам. Что бы вы хотели узнать?",
                          "Мы только что поднимали это тему. Вы, вероятно, запамятовали...", "Сегодня вы уже третий раз говорите о каком-то вопросе...",
                          "Что ты заладил как попугай одно и то же...", "block", 1, npchar, Dialog.CurrentNode);
			link.l1 = HeroStringReactionRepeat("Знаешь, " + NPChar.name + ", как-нибудь в следующий раз.", "Точно, забыл что-то...",
                      "Да уж, действительно в третий раз...", "Да уж...", npchar, Dialog.CurrentNode);
			link.l1.go = "exit";
            // ==> Проверяем поле состояния квестов.
            switch(pchar.questTemp.State)
            {
                case "WormEnglPlans_WormPicarder": //Голландская линейка, квест №3. узнаем о Пеьере Пикардийце.
                    link.l1 = "Послушай, ты не знаешь флибустьера Пьера Пикардийца?";
                    link.l1.go = "Step_H3_1";
                break;
                case "DelivLettTortuga_toStranger": //Голландская линейка, квест №7. узнаем о ожидающем ГГ человеке.
                    link.l1 = "Слушай, ты не в курсе дела? Меня тут ждут, я прибыл из Кюрасао...";
                    link.l1.go = "Step_H7_1";
                break;
                case "DelivLettTortuga_AfterRape": //Голландская линейка, квест №7. пытаемся узнать, кто был грабитель.
                    link.l1 = "Где этот человек, что ждал меня в комнате наверху?";
                    link.l1.go = "Step_H7_2";
                break;
                case "SeekBible_toTavern": //Голландская линейка, квест №8. узнаем, где Лоран де Граф.
                    link.l1 = "Ты не знаешь, где мне найти флибустьера Лорана де Графа?";
                    link.l1.go = "Step_H8_1";
                break;
                case "TakeRockBras_toTortuga": //Испанская линейка, квест №3. узнаем, где Рок Бразилец.
                    link.l1 = "Приятель, не подскажешь, как мне найти Рока Бразильца?";
                    link.l1.go = "Step_S3_1";
                break;
                case "Sp4Detection_toTortuga": //Испанская линейка, квест №4. узнаем о донне Анне.
                    link.l1 = "Что нового на Тортуге?";
                    link.l1.go = "Step_S4_1";
                break;
                case "Sp6TakeMess_toTotuga": //Испанская линейка, квест №6. узнаем о вестовом
                    link.l1 = "Послушай, голландцы с Кюрасао не часто у вас бывают?";
                    link.l1.go = "Step_S6_1";
                break;
                case "Sp6TakeMess_waitMessanger": //Испанская линейка, квест №6. узнаем о вестовом
                    link.l1 = "Послушай, голландцы с Кюрасао не часто у вас бывают?";
                    link.l1.go = "Step_S6_1";
                break;
            }
            // <== Проверяем поле состояния квестов.
        break;
//===================================== Голл. линейка, квест №3 =========================================
 	 	case "Step_H3_1":
			dialog.text = NPCStringReactionRepeat("Знаю, как не знать. Он снял у меня комнату и заплатил за год вперед!", "Хей, не ты ли уже спрашивал меня об этом?", 
				          "Хм, мы говорили уже о Пьере...", "Может ты не будешь меня отвлекать? Я могу послать за Пьером, если ты так желаешь его видеть.", "block", 0, npchar, Dialog.CurrentNode);
			link.l1 = HeroStringReactionRepeat("Вот это да! Видимо, с деньгами у него все в порядке...", "Спрашивал? Ну да, наверное...", "Да, точно, говорили, извини...", "Да нет, не стоит. Сам его найду...", npchar, Dialog.CurrentNode);
			link.l1.go = DialogGoNodeRepeat("Step_H3_2", "none", "none", "none", npchar, Dialog.CurrentNode);
        break;
 	 	case "Step_H3_2":
			dialog.text = "Еще бы! Недавно Морган приглашал его участвовать в нападении на испанские серебряные рудники. По всему видать, вовремя они там появились.";
			link.l1 = "Повезло, что тут скажешь...";
			link.l1.go = "Step_H3_3";
        break;
 	 	case "Step_H3_3":
			dialog.text = "Но сейчас его нет в своей комнате, приходи попозже, тогда сможешь его застать.";
			link.l1 = "Спасибо, так и сделаю.";
			link.l1.go = "exit";
            pchar.questTemp.Talks.Tavern = true;
            AddQuestRecord("Hol_Line_3_WormEnglishPlans", "7");
        break;
//============================ Голл. линейка, квест №7. узнаем о ожидающем ГГ челе =======================
 	 	case "Step_H7_1":
			dialog.text = NPCStringReactionRepeat("В курсе. Пройди в комнату наверху, там тебя ждет какой-то тип.", "Ты уже задавал мне этот вопрос сегодня.", "Опять?", "Может ты не будешь меня отвлекать от работы?", "block", 1, npchar, Dialog.CurrentNode);
			link.l1 = HeroStringReactionRepeat("Спасибо, старина.", "Верно, задавал...", "Хм, извини...", "Хорошо, не буду. Извини...", npchar, Dialog.CurrentNode);
			link.l1.go = DialogGoNodeRepeat("exit", "none", "none", "none", npchar, Dialog.CurrentNode);
        break;
 	 	case "Step_H7_2":
			dialog.text = NPCStringReactionRepeat("С час назад ушел, сказал, что ты еще задержишься...", "Ты уже задавал мне этот вопрос сегодня.", "Опять?", "Может ты не будешь меня отвлекать от работы?", "block", 1, npchar, Dialog.CurrentNode);
			link.l1 = HeroStringReactionRepeat("Черт возьми! Кто он и куда он ушел, ты знаешь?", "Верно, задавал...", "Хм, извини...", "Хорошо, не буду. Извини...", npchar, Dialog.CurrentNode);
			link.l1.go = DialogGoNodeRepeat("Step_H7_3", "none", "none", "none", npchar, Dialog.CurrentNode);
        break;
  	 	case "Step_H7_3":
			dialog.text = "Появился он здесь позавчера, сразу сказал, что ждет одного человека. Интересовался кораблями из Кюрасао. Выходит, тебя и ждал. А куда он ушел - мне неведомо.";
			link.l1 = "И ты вообще ничего не знаешь о нем?";
			link.l1.go = "Step_H7_4";
        break;
  	 	case "Step_H7_4":
			dialog.text = "Раньше я его не видел, а постоялец он хороший, заплатил две цены за комнату. Более ничего тебе сказать не могу.";
			link.l1 = "Ну что же, спасибо и на этом...";
			link.l1.go = "exit";
        break;
//============================ Голл. линейка, квест №8. узнаем о Лоране де Графе =======================
  	 	case "Step_H8_1":
			dialog.text = "На Тортуге ты сейчас его не найдешь. Пару недель назад он ушел из города. Опять, небось, испанцев потрошить...";
			link.l1 = "А куда он ушел, точно ты не знаешь?";
			link.l1.go = "Step_H8_2";
        break;
  	 	case "Step_H8_2":
			dialog.text = "О своих планах как-то не принято распространяться флибустьеру.";
			link.l1 = "Понятно... А у кого можно узнать?";
			link.l1.go = "Step_H8_3";
        break;
  	 	case "Step_H8_3":
			dialog.text = "А зачем тебе?";
			link.l1 = "Он мне нужен срочно по очень важному делу. Искал его на Бермудах, теперь ищу здесь. Я сам голландец, как и Лоран...";
			link.l1.go = "Step_H8_4";
        break;
   	 	case "Step_H8_4":
			dialog.text = "Ладно, скажу тебе, раз так надо. Слышал я, что он курсирует рядом с Картахеной. Де Граф имеет в своем распоряжении фрегат 'Коминте'.";
			link.l1 = "Понятно. Спасибо тебе, дружище.";
			link.l1.go = "exit";
            pchar.questTemp.State = "SeekBible_toDeGraf";
            AddQuestRecord("Hol_Line_8_SeekBible", "5");
            Pchar.quest.SeekBible_SaveDeGraf.win_condition.l1 = "location";
            Pchar.quest.SeekBible_SaveDeGraf.win_condition.l1.location = "Cartahena";
            Pchar.quest.SeekBible_SaveDeGraf.win_condition = "SeekBible_SaveDeGraf";
        break;
//============================ Исп. линейка, квест №3. узнаем о Роке Бразильце =======================
  	 	case "Step_S3_1":
			dialog.text = "Смотря зачем он тебе нужен. Как срочно?";
			link.l1 = "Он мне нужен срочно, у меня к нему серьезное дело.";
			link.l1.go = "Step_S3_2";
        break;
  	 	case "Step_S3_2":
			dialog.text = "Тогда это проблема. Рок ушел в поиск к Маракайбо на своем корвете, будет курсировать рядом, пока не подловит галеон с приличной добычей.";
			link.l1 = "Вот как! Да это же просто здорово, так как мне тоже нужно попасть в этот регион. Надеюсь, что встречу его там. Спасибо тебе.";
			link.l1.go = "Step_S3_3";
        break;
  	 	case "Step_S3_3":
			dialog.text = "Да не за что...";
			link.l1 = "Будь здоров, старина.";
			link.l1.go = "exit";
            pchar.questTemp.State = "TakeRockBras_toNearestMaracaibo";
            AddQuestRecord("Spa_Line_3_RockBrasilian", "2");
			pchar.quest.TakeRockBras_toRock.win_condition.l1 = "location";
			pchar.quest.TakeRockBras_toRock.win_condition.l1.location = "Maracaibo";
			pchar.quest.TakeRockBras_toRock.win_condition = "TakeRockBras_toRock";
        break;
//============================ Испанская линейка, квест №4. узнаем о донне Анне. =======================
  	 	case "Step_S4_1":
			dialog.text = "Да все по старому, воюем с проклятыми папистами.";
			link.l1 = "А-а-а, это дело хорошее. А как папистки, есть у вас в городе? Горячие, говорят, женщины...";
			link.l1.go = "Step_S4_2";
        break;
  	 	case "Step_S4_2":
			dialog.text = "До недавнего времени не было ни одной. Но теперь у нашего уважаемого генерал-губернатора д'Ожерона есть испанская козочка, о-о-очень красивая. Генерал-губернатор просто счастлив и рот у него до ушей с утра до позднего вечера. Не знаю, как ночью, хи-хи...";
			link.l1 = "Вот так! И кто она, эта отважная женщина?";
			link.l1.go = "Step_S4_3";
        break;
  	 	case "Step_S4_3":
			dialog.text = "Донна Анна - так ее зовут все. Слышал я, что она из Гаваны.";
			link.l1 = "До Гаваны флибустьеры еще не добирались. Как она попала к д'Ожерону, захватили на испанском галеоне?";
			link.l1.go = "Step_S4_4";
        break;
  	 	case "Step_S4_4":
			dialog.text = "Не-е-ет, здесь обоюдное чувство. Донна Анна так же любит нашего генерал-губернатора, как и он ее. Слышал я, что они познакомились еще в Старом Свете. Привез ее на Тортугу один из доверенных офицеров генерал-губернатора - Анри д'Эстре, а где он ее нашел - никто не знает.";
			link.l1 = "Понятно... А где сейчас этот Анри д'Эстре?";
			link.l1.go = "Step_S4_5";
        break;
  	 	case "Step_S4_5":
			dialog.text = "У себя, наверное. У него дом здесь на острове.";
			link.l1 = "Вижу, что Тортуга процветает и я этому весьма и весьма рад. Будь здоров, приятель.";
			link.l1.go = "exit";
            pchar.questTemp.State = "Sp4Detection_toDEstre";
            AddQuestRecord("Spa_Line_4_MirderDetection", "5");
        break;
//============================ Испанская линейка, квест №6. перехватить вестового. =======================
  	 	case "Step_S6_1":
			dialog.text = NPCStringReactionRepeat("Бывают, а что тебе до них?",
                          "Голландцы то? Бывают, бывают...", "Говорили же мы о голландцах сегодня...",
                          "Я больше не хочу говорить о голландцах...", "block", 0, npchar, Dialog.CurrentNode);
			link.l1 = HeroStringReactionRepeat("Я тут жду одного человека из Виллемстада. Так что придется у тебя снять комнату на недельку.",
                      "Здорово!", "Говорили, верно...", "Хорошо, как скажешь...", npchar, Dialog.CurrentNode);
			link.l1.go =  DialogGoNodeRepeat("Step_S6_2", "none", "none", "none", npchar, Dialog.CurrentNode);
        break;
  	 	case "Step_S6_2":
			dialog.text = "Нет проблем, комната свободна. Плати 35 монет за неделю - и живи.";
			link.l1 = "Я тебе сотню монет заплачу за неделю, что такое 35 монет? Крохи...";
			link.l1.go = "Step_S6_3";
        break;
  	 	case "Step_S6_3":
			dialog.text = "Хех, ну спасибо тебе, добрый человек. От денег не откажусь. А комнату можешь занимать хоть прямо сейчас, там открыто.";
			link.l1 = "Спасибо.";
			link.l1.go = "exit";
			pchar.questTemp.State.Open = 1;
			LocatorReloadEnterDisable("Tortuga_tavern", "reload2_back", false);
            AddMoneyToCharacter(pchar, -100);
            if (CheckAttribute(pchar, "questTemp.State.Sp6Ok"))
            {
                SetTimerCondition("Sp6TakeMess_waitingBoy", 0, 0, rand(7)+1, false);
                DeleteAttribute(pchar, "questTemp.State.Sp6Ok");
            }
            else
            {
                pchar.questTemp.State.Sp6Ok = 1;
            }
        break;
        
        
        
	}
	UnloadSegment(NPChar.FileDialog2);  // если где-то выход внутри switch  по return не забыть сделать анлод
}

