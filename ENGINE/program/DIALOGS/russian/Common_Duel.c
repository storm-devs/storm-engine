//navy 26.07.06
//генератор дуэлей...
//в диалоге НПС делаем линк с проверкой флага оскорбления... по нему переход на "outraged"
//если НПС сделал что-то ГГ (продажа фальшивки) или ГГ наехал на НПС, вешаем на него флаг.
//для некоторых, например ПГГ, можно делать переход сразу на вызов: "let_s_duel"
void ProcessDuelDialog(ref NPChar, aref Link, aref NextDiag)
{
	int iHour;
	string sLocation;

	//флаг убираем
	DeleteAttribute(NPChar, "Outrage");
    switch (Dialog.CurrentNode)
	{
	case "outraged":
		Dialog.Text = NPCharRepPhrase(NPChar, RandSwear() + 
				RandPhraseSimple("Как ты смеешь меня оскорблять, мерзавец?!", "Ты дорого заплатишь за свои слова!"), 
				RandPhraseSimple("Вы задели мою честь, " + GetAddress_Form(NPChar) + "! За это придется заплатить.", "Как вы смеете? Немедленно возьмите свои слова назад!"));

		//это такая засада.. чтобы читали текст :)
		MakeRandomLinkOrderTwo(link, 
					NPCharRepPhrase(pchar, 
				RandPhraseSimple("Ха??!! Да я об тебя и шпагу марать не хочу!", "Ха??!! Да я об тебя и шпагу марать не хочу!"), 
				RandPhraseSimple("Я всегда к вашим услугам!", "Я не намерен выбирать выражения, разговаривая с отребьем!")), "outraged_1",
					NPCharRepPhrase(pchar, RandSwear() + 
				RandPhraseSimple("Я не это имел в виду...", "Я погорячился..."), 
				RandPhraseSimple("Это была ошибка. Прошу прощения.", "Я обознался, " + GetAddress_FormToNPC(NPChar) + ". Прошу меня простить.")), "change_mind");
		break;

	case "outraged_1":
		Dialog.Text = NPCharRepPhrase(NPChar,  
				RandPhraseSimple("Я тебе уши обрежу!", "Я вырежу тебе сердце!"), 
				RandPhraseSimple("Я надеюсь, вы немедленно принесете свои извинения, или я за себя не ручаюсь!", "Вы понимаете, что это означает?"));

		MakeRandomLinkOrderTwo(link,
					NPCharRepPhrase(pchar, RandSwear() + 
				RandPhraseSimple("Надеюсь, твоя шпага так же быстра, как твой язык!", "За меня будет говорить мой клинок."), 
				RandPhraseSimple("Я вызываю вас на дуэль!", "Это дело чести!")), "let_s_duel",
					NPCharRepPhrase(pchar, 
				RandPhraseSimple("Пожалуй, я еще не готов к встрече с дьяволом!", "Думаю, не стоит так горячиться! Это только слова!"), 
				RandPhraseSimple("Я тут вспомнил, меня ждут. Всех благ...", "Ах! Кажется, мой корабль уже отплывает.")), "change_mind");
		break;

	case "let_s_duel":
		//проверка на начатые дуэли.
		if (CheckAttribute(PChar, "questTemp.duel.Start") && sti(PChar.questTemp.duel.Start))
		{
			Dialog.Text = "Тебе стоит сначала разобраться с другими, а потом мы поговорим.";
			if (PChar.questTemp.duel.enemy == NPChar.id)
			{
				Dialog.Text = "Мы уже с тобой договорились обо всем.";
			}
			link.l1 = RandSwear() + "Как это я забыл...";
			link.l1.go = "exit";
			break
		}
		//может отказаться.
		if (rand(100) < 30)
		{
			Dialog.Text = RandPhraseSimple("Дуэль?!!! Проваливай, ты не стоишь потраченного времени.", "Дуэль? Слишком много чести! Сгинь!");
			link.l1 = RandPhraseSimple("Ну-ну...", "Я могу и подождать...");
			link.l1.go = "exit";
/**/
			//можно дать возможность драться полюбому :)			
			if (rand(100) < 30)
			{
				link.l1 = RandPhraseSimple("Ну, это мы сейчас посмотрим!!!", "Да что ты говоришь?! Сейчас я увижу цвет твоей крови!");
				link.l1.go = "fight_right_now";
			}
/**/
			break;
		}

		//согласен.
		Dialog.Text = RandPhraseSimple("Дуэль, говоришь? Пожалуй, я не прочь. Будем биться на шпагах и пистолетах.", 
			"Да ты, наверное, и шпагу-то держать не умеешь?");
		link.l1 = RandPhraseSimple("Ты будешь на коленях просить о пощаде.", "Я прикончу тебя вот этими руками, мерзавец!");
		link.l1.go = "land_duel";
		if (sti(pchar.Ship.Type) != SHIP_NOTUSED && sti(NPChar.Ship.Type) != SHIP_NOTUSED)
		{
			Dialog.Text = RandPhraseSimple(Dialog.Text + " Или мы можем выйти в море и посмотреть, кто на что способен!", 
				"Дуэль?!! Ха, да я раздавлю тебя как блоху! Выбирай, как ты умрешь!");
			link.l2 = RandPhraseSimple("В море мне нет равных.", "Мой корабль пустит ко дну твое старое корыто!");
			link.l2.go = "sea_duel";
		}
		link.l3 = "Я передумал...";
		link.l3.go = "change_mind";
		break;

	//дуэль на подводных лодках :))))
	case "sea_duel":
		Dialog.Text = RandPhraseSimple("Что-то не вижу твоего корабля в порту...", "Проваливай, пока не найдешь себе хоть какое-нибудь корыто!!!");
		link.l1 = RandPhraseSimple("Моя ошибка...", "...Тогда я вас покидаю.");
		link.l1.go = "exit";
		if (pchar.location.from_sea == GetCurrentTown() + "_town")
		{
			Dialog.Text = "Что ж, жду тебя на выходе из бухты...";
			link.l1 = "Долго не придется ждать.";

			pchar.questTemp.Duel.enemy = NPChar.id;
			pchar.questTemp.Duel.Sea_Location = Islands[GetCharacterCurrentIsland(PChar)].id;
			AddDialogExitQuestFunction("Duel_Sea_Prepare");
		}

		break;

	//на суше
	case "land_duel":
		iHour = 1 + rand(2);
		if (GetTime() > 4.0 && GetTime() < 17.0)
		{
//			iHour = MakeInt(18.0 - GetTime()) + rand(2); //раскоментировать для дуэлей только по ночам
		}
		pchar.questTemp.Duel.WaitTime = iHour;
		Dialog.Text = RandSwear() + RandPhraseSimple("Доставай свой клинок, и мы посмотрим, какого цвета твоя кровь!", "Ну, если ты так хочешь умереть...");
		link.l1 = RandPhraseSimple("Лучше нам пойти туда, где свидетелей поменьше. Я знаю одно местечко за городскими воротами. Встретимся там через " + iHour + " часа.",
			"Эй... не так быстро. Такие дела лучше делать без свидетелей. Приходи через " + iHour + " часа к входу в город.");
		link.l1.go = "fight_off_town";
		link.l2 = RandPhraseSimple("Ну, если ты так торопишься умереть...", "Проклятье! Я помогу тебе отправиться в ад!!!");
		link.l2.go = "fight_right_now";
		link.l3 = "Я передумал...";
		link.l3.go = "change_mind";

		if (rand(1))
		{
			Dialog.Text = RandSwear() + RandPhraseSimple("Я думаю, нам стоит выйти за городские ворота. Жду тебя там через " + pchar.questTemp.Duel.WaitTime + " часа. Не задерживайся!", 
				"Тут не лучшее место для разбирательств. Лучше выйдем из города. Через " + pchar.questTemp.Duel.WaitTime + " часа я буду там.");
			link.l1 = RandPhraseSimple("Слишком много чести! Защищайся!", "Мне некогда! Доставай свой клинок!");
			link.l1.go = "fight_right_now";
			link.l2 = RandPhraseSimple("Что ж, давай прогуляемся.", "Пожалуй, ты прав. Встретимся за воротами.");
			link.l2.go = "fight_off_town_prep";
		}
		break;

	//предложение "пойдем выйдем" рассматривается
	case "fight_off_town":
		Dialog.Text = RandPhraseSimple("Слишком много чести! Защищайся!", "Мне некогда! Здесь и сейчас!");
		link.l1 = RandPhraseSimple("Ну, если ты так торопишься умереть...", "Проклятье! Я помогу тебе отправиться в ад!!!");
		link.l1.go = "fight_right_now";
		if (rand(1))
		{
			Dialog.Text = RandPhraseSimple("Что ж, давай прогуляемся.", "Пожалуй, ты прав. Встретимся за воротами.");
			link.l1 = RandPhraseSimple("Жду тебя там.", "Не опаздывай.");
			link.l1.go = "fight_off_town_prep";
		}
		break;

	//что ж, пойдем выйдем
	case "fight_off_town_prep":
		SaveCurrentQuestDateParam("questTemp.Duel.StartTime");
		PChar.questTemp.duel.Start = true;
		PChar.questTemp.duel.enemy = NPChar.id;
		sLocation = GetCurrentTown();
		if (sLocation != "")
		{
			//где?
			sLocation += "_ExitTown";
			pchar.questTemp.duel.place = sLocation;

			Locations[FindLocation(sLocation)].DisableEncounters = true;
			//приходит ко времени.
			pchar.quest.duel_move_opponent2place.win_condition.l1 = "Location";
			pchar.quest.duel_move_opponent2place.win_condition.l1.location = pchar.questTemp.duel.place;
			pchar.quest.duel_move_opponent2place.function = "Duel_Move_Opponent2Place";
			//на случай, если не дождется, часа вполне достаточно
			SetTimerConditionParam("duel_move_opponentBack", "Duel_Move_OpponentBack", 0, 0, 0, sti(GetTime() + 0.5) + sti(pchar.questTemp.Duel.WaitTime) + 1, false);
			pchar.quest.duel_move_opponentBack.function = "Duel_Move_OpponentBack";
			if (CheckAttribute(NPChar, "CityType"))
			{
				DeleteAttribute(NPChar, "City"); // чтоб не было ругани с нацией
	    		DeleteAttribute(NPChar, "CityType");
				if (!CheckAttribute(NPChar, "PGGAi")) 
				{
					if (!CheckAttribute(NPChar, "LifeDay")) npchar.LifeDay = 0;
					npchar.LifeDay = sti(npchar.LifeDay) + 3; // чтоб до дуэли не помер
				}
    		}
		}
		NextDiag.CurrentNode = "let_s_duel";
		DialogExit();
		break;

	//последнее слово перед боем
	case "talk_off_town":
		Dialog.Text = "Ну что, ты готов отправиться в ад?";
		link.l1 = "Да. Сейчас посмотрим, кто кого!";
		link.l1.go = "fight_right_now";
		link.l2 = "Нет, я решил принести тебе извинения. Был неправ, вспылил.";
		link.l2.go = "change_mind";
		if (rand(100) < 20)
		{
			Dialog.Text = RandPhraseSimple("Слушай, я тут подумал и понял, что был неправ. Приношу свои извинения", 
				"Черт! Это все ром!!! Прости, брат!");
			link.l1 = NPCharRepPhrase(pchar, 
				RandPhraseSimple("Ну уж нет, теперь только твоя кровь может принести мне удовлетворение!", "Доставай свою шпагу, и нечего тут сопли распускать!"), 
				RandPhraseSimple("Нет! Я требую сатисфакции!", "Вы - позор своей семьи! Доставайте свою шпагу!"));
			link.l1.go = "fight_right_now";
			link.l2 = NPCharRepPhrase(pchar, 
				RandPhraseSimple("Что ж, согласен. Погорячились.", "Какие счеты между своими!!"), 
				RandPhraseSimple("Пожалуй, вы правы. Не стоит проливать кровь по пустякам.", "Мое великодушие не знает границ! Вы прощены!"));
			link.l2.go = "peace";
		}
		NextDiag.TempNode = npchar.BackUp.DialogNode;
		pchar.questTemp.Duel.End = true;
		break;

	//дуэли быть!
	case "fight_right_now":
		PChar.questTemp.duel.enemy = NPChar.id;
		AddDialogExitQuestFunction("Duel_Prepare_Fight");
		NextDiag.CurrentNode = NextDiag.TempNode;
		DialogExit();
		break;

	//передумал, э... не хорошо ;)
	case "change_mind":
		if (CheckAttribute(pchar, "questTemp.Duel.End")) LAi_SetWarriorType(NPChar);
		Dialog.Text = NPCharRepPhrase(NPChar, RandSwear() + 
				RandPhraseSimple("Тогда проваливай отсюда!", "Скройся тогда. А то я могу передумать."), 
				RandPhraseSimple("В таком случае, не смею вас больше задерживать!", "Тогда вам лучше уйти, пока я не передумал."));
		link.l1 = "Уже ухожу...";
		link.l1.go = "peace";
		break;

	case "after_peace":
		Dialog.Text = NPCharRepPhrase(NPChar, RandSwear() +
				RandPhraseSimple("Проваливай отсюда!", "Скройся. А то я могу передумать."),
				RandPhraseSimple("Чем могу быть полезен?", "Вы что-то хотели?"));
		link.l1 = "Я ухожу.";
		link.l1.go = "peace";
		break;

	//мир и все такое.
	case "peace":
		LAi_SetWarriorType(NPChar);
		NextDiag.CurrentNode = "after_peace";
		DialogExit();
		break;
	}
}
