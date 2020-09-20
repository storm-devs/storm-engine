// boal 25/04/04 общий диалог солдат
void ProcessDialogEvent()
{
	ref NPChar;
	aref Link, NextDiag;	

	DeleteAttribute(&Dialog,"Links");

	makeref(NPChar,CharacterRef);
	makearef(Link, Dialog.Links);
	makearef(NextDiag, NPChar.Dialog);

    // вызов диалога по городам -->
    NPChar.FileDialog2 = "DIALOGS\" + LanguageGetLanguage() + "\Soldier\" + NPChar.City + "_Soldier.c";
    if (LoadSegment(NPChar.FileDialog2))
	{
        ProcessCommonDialog(NPChar, Link, NextDiag);
		UnloadSegment(NPChar.FileDialog2);
	}
    // вызов диалога по городам <--
    int iTest, iTemp;
	string sTemp;
    iTest = FindColony(NPChar.City); // город
    ref rColony;
	if (iTest != -1)
	{
		rColony = GetColonyByIndex(iTest);
	}
	switch(Dialog.CurrentNode)
	{
		case "Exit":
			sTemp = npchar.location;
			if (findsubstr(sTemp, "_townhall" , 0) != -1) AddDialogExitQuest("MainHeroFightModeOff");
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
		
		case "fight":
			DialogExit();
			NextDiag.CurrentNode = NextDiag.TempNode;
			LAi_group_Attack(NPChar, Pchar);
			if (rand(3) != 1) SetNationRelation2MainCharacter(sti(npchar.nation), RELATION_ENEMY);
			AddDialogExitQuest("MainHeroFightModeOn");
		break;
		
		case "First time":			
            NextDiag.TempNode = "First time";
			if (GetNationRelation2MainCharacter(sti(NPChar.nation)) == RELATION_ENEMY && sti(NPChar.nation) != PIRATE)
			{
				// заглушка на пирата
				if (sti(pchar.nation) == PIRATE)
				{
    				dialog.text = RandPhraseSimple("Пираты в городе?! Ну дела... Хватай его!!", "Это пират!! Держи его!!!");
					link.l1 = RandPhraseSimple("Пират, ну и что?..", "Хех, попробуйте схватить.");
					link.l1.go = "fight"; 
					break;
				} 
				dialog.text = RandPhraseSimple("Шпион? Сдать оружие!! Следовать за мной!", "Вражеский агент!! Немедленно схватить его!");
				link.l1 = RandPhraseSimple("Заткнись, малахольный!", "Как бы не так!");
				link.l1.go = "fight"; 
				// ==> eddy. Засада, если опознали в инквизиции.
				if (Pchar.location == "Santiago_Incquisitio") StartIncquisitioAttack();
			}
			else
			{
				// eddy. проверяем, не казачок ли. -->
				if (GetNationRelation(sti(NPChar.nation), GetBaseHeroNation()) == RELATION_ENEMY && sti(NPChar.nation) != PIRATE)
				{
					dialog.text = RandPhraseSimple("Кто ты и что тебе здесь нужно?", "Стой! Кто ты? На каком основании пытаешься войти в город?");
					//==> по лицензии
					if (CheckNationLicence(sti(npchar.nation)))
					{
						link.l1 = "Офицер, у меня имеется " + GetRusNameNationLicence(sti(npchar.nation)) + ", так что, я нахожусь здесь на законных основаниях. Прошу ознакомиться...";
						link.l1.go = "LicenceOk";
						if (findsubstr(pchar.location.from_sea, "_town" , 0) != -1) //если причалил в городе
						{
							link.l2 = "Ты что, не видишь, как на моем корабле развивается флаг " + NationNameGenitive(sti(pchar.nation)) + "?!";
						}
						else //если причалил не в городе
						{
							link.l2 = "Я бросил якорь у " + XI_ConvertString(GetIslandByCityName(npchar.city)+"Gen") + " под флагом " + NationNameGenitive(sti(pchar.nation)) + ". Что тебе еще не ясно?";
						}
						if (GetSummonSkillFromName(pchar, SKILL_SNEAK) < (20+rand(50)+rand(50)))
						{
							link.l2.go = "PegYou";
						}
						else
						{
							link.l2.go = "NotPegYou";
						}
					}
					else
					{
						//==> по флагу
						// заглушка на пирата
						if (sti(pchar.nation) == PIRATE)
						{
    						dialog.text = RandPhraseSimple("Пираты в городе?! Ну дела... Хватай его!!", "Это пират!! Держи его!!!");
							link.l1 = RandPhraseSimple("Да, пират, ну и что?..", "Хех, попробуйте схватить...");
							link.l1.go = "fight"; 
							break;
						}
						if (findsubstr(pchar.location.from_sea, "_town" , 0) != -1) //если причалил в городе
						{
							link.l1 = "Ты что, не видишь, как на моем корабле развивается флаг " + NationNameGenitive(sti(pchar.nation)) + "?!";
						}
						else //если причалил не в городе
						{
							link.l1 = "Я бросил якорь у " + XI_ConvertString(GetIslandByCityName(npchar.city)+"Gen") + " под флагом " + NationNameGenitive(sti(pchar.nation)) + ". Что тебе еще не ясно?";
						}
						if (GetSummonSkillFromName(pchar, SKILL_SNEAK) < (20+rand(50)+rand(50)))
						{
							link.l1.go = "PegYou";
						}
						else
						{
							link.l1.go = "NotPegYou";
						}
					}
				}
				// <-- eddy. проверяем, не казачок ли.
				else
				{
					if (sti(NPChar.nation) == PIRATE)
					{
						if (sti(rColony.HeroOwn) == true) // наш горожанин
						{
			         		switch (rand(10))
							{
								case 0:
									dialog.text = "Эх капитан, как славно было с вами в море! Сколько кораблей мы потопили под вашей командой! А тут...";
									link.l1 = "А тут, друг мой, перед вами постоянно мелькают очаровательные женщины, которых в море не бывает.";
									link.l1.go = "exit";
								break;

								case 1:
									dialog.text = "Капитан, за что такое наказание?! Мы ведь не сухопутные крысы!";
									link.l1 = "Спокойно, моряк! Вы на важном и почетном посту, так что не нойте.";
									link.l1.go = "exit";
								break;

								case 2:
									dialog.text = "Как там в море, капитан? Мы когда-нибудь снова увидим его?";
									link.l1 = "Конечно, матрос! Сменитесь с караула, выйдете на пирс и наслаждайтесь морем сколько угодно.";
									link.l1.go = "exit";
								break;

								case 3:
									dialog.text = "Спешу пожаловаться, капитан: мы все стремимся снова в море. Эта служба на суше просто поперек горла стоит!";
									link.l1 = "Я устал от этого нытья! Выпивки вам хватает и на земле! Служи там, где тебя поставил капитан! А то кое-кого придется вздернуть для примера.";
									link.l1.go = "exit";
								break;

								case 4:
									dialog.text = "Скажу вам по секрету, капитан, новый губернатор - взяточник и казнокрад. Но это, конечно, не мое дело…";
									link.l1 = "Ты прав, корсар: твое дело - стоять на посту и следить за порядком. Ну а вздернуть губернатора на нок рее - это уже занятие для меня. Молодец!";
									link.l1.go = "exit";
								break;

								case 5:
									dialog.text = "Спасибо, что не забываете о нас капитан! Мы для вас и в огонь и в воду!";
									link.l1 = "Знаю я вас, проходимцев! Любите только золото. Сегодня в таверне попойка, я угощаю. Не забудь заглянуть.";
									link.l1.go = "exit";
								break;

								case 6:
									dialog.text = "Эх, капитан! Как давно мы не были в кровавой драке! Ну, какие из корсаров копы?!";
									link.l1 = "Не расслабляйся, корсар! Вражеские армады рыщут вокруг наших островов, так что кровавая баня может приключиться в любой момент.";
									link.l1.go = "exit";
								break;

								case 7:
									dialog.text = "Капитан, говорят королевские власти снова посылают сюда эскадру?";
									link.l1 = "Конечно, корсар. Пока мы живы, мира не будет никогда. И даже в Аду станем сражаться с чертями!";
									link.l1.go = "exit";
								break;

								case 8:
									dialog.text = "Йо-хо-хо! Какая знатная вчера у нас была попойка, капитан! Жалко, что вас не было.";
									link.l1 = "Ничего, я свое еще наверстаю. И вам, ребята, я не завидую.";
									link.l1.go = "exit";
								break;

								case 9:
									dialog.text = "Скажу вам по секрету, капитан, поскольку вы нас никогда не обижали, завалили мы вчера с ребятами здесь одну телочку...";
									link.l1 = "Эх, корсар, плачет по вам петля!";
									link.l1.go = "exit";
								break;

								case 10:
									dialog.text = "Кэп! Освободите вы меня от этой проклятой повинности! Ну, не могу я здесь блюстителя порядка разыгрывать!";
									link.l1 = "Ты лучше вспомни: ночную вахту на корабле стоять тоже не легко. Служба, брат, она везде служба.";
									link.l1.go = "exit";
								break;
							}
							link.l2 = RandPhraseSimple("Есть важное дело!", "У меня к тебе дело.");
							link.l2.go = "quests";//(перессылка в файл города)
							break;
						}
						else
						{ // пираты, не наши
							dialog.text = RandPhraseSimple("Чего тебе? Проходи мимо.", "Хватит загораживать дорогу, отвали.");
							link.l1 = RandPhraseSimple("Уже ушёл.", "Не нужно мне грубить. До свидания.");
							link.l1.go = "exit";
							link.l2 = RandPhraseSimple("Всего один маленький вопросик.", "Минутку внимания, я по делу.");
							link.l2.go = "quests";//(перессылка в файл города)
							break;
						}
					}
					else
					{ //если негодяй, имеющий НЗГ к дружественной или нейтральной нации
						if (ChangeCharacterNationReputation(pchar, sti(NPChar.nation), 0) <= -15)
						{
							dialog.text = RandPhraseSimple("Вы посмотрите, каков мерзавец! Посмел явится в " + XI_ConvertString("Colony" + npchar.city)+ ". Держи его!!", "Ха, я узнал тебя, негодяй! Хватай его!!");
							link.l1 = RandPhraseSimple("Аргх!..", "Ну вы сами напросились...");
							link.l1.go = "fight";
							break;						
						}
					}
					switch (rand(10))
					{
						case 0: ////////////////////////////////////////
							dialog.text = "Не видишь, я на посту! Не отвлекай меня.";
							link.l1 = "Хорошо, хорошо...";
							link.l1.go = "exit";
						break;

						case 1:
							dialog.text = "Ты хочешь сообщить мне о каких-то нарушениях?";
							link.l1 = "Нет, ничего такого, к тому же я капитан. Вижу, ты уже не слушаешь? Прощай.";
							link.l1.go = "exit";
						break;

						case 2: ///////////////////////////////////////////
							dialog.text = "Запомни, драки на улицах запрещены! Ты можешь обнажать свое оружие только если на тебя кто-то нападет.";
							link.l1 = "Хорошо, я приму это к сведению.";
							link.l1.go = "exit";
						break;

						case 3:
							dialog.text = "Какая погода, а я вынужден стоять здесь. Мое единственное развлечение заключается в отгонянии мух от своего лица.";
							link.l1 = "Сочувствую. Но ничем не могу помочь, работа у тебя такая.";
							link.l1.go = "exit";
						break;

						case 4: ///////////////////////////////////////////
							dialog.text = "Если вам хочется поболтать, то найдите себе кого-нибудь другого, а я должен следить за порядком, и мне некогда чесать с вами языком.";
							link.l1 = "Да нет, я просто проверял, жив ли ты еще, а то стоишь, как статуя.";
							link.l1.go = "exit";
						break;

						case 5: ////////////////////////////////////////////
							dialog.text = "Вот вы думаете, что поддерживать порядок это такая легкая работа? Как бы не так! Это очень ответственный и опасный труд. Вот помнится...";
							link.l1 = "Расскажешь свою историю как-нибудь в другой раз, а сейчас я спешу.";
							link.l1.go = "exit";
						break;

						case 6: ////////////////////////////////////////////
							dialog.text = "Проходи своей дорогой, и не отвлекай меня.";
							link.l1 = "Как скажешь, солдат.";
							link.l1.go = "exit";
						break;

						case 7:
							dialog.text = "Эй! Не принесешь ли мне стаканчик вина из таверны? Умираю от жажды.";
							link.l1 = "Да, а потом меня вздернут за спаивание солдат. Нет уж, дотерпи до смены, а там уж пей, сколько в тебя влезет.";
							link.l1.go = "exit";
						break;

						case 8://///////////////////////////////////////////
							dialog.text = "Ну почему ко мне подходят только такие неотесанные мужланы вроде тебя?! Неужели в нашем городе не осталось красивых девушек, с аппетитными ножками?";
							link.l1 = "И это называется 'нести службу'? Смотри, проворонишь какого-нибудь нарушителя спокойствия, вроде меня. Ха-ха!";
							link.l1.go = "exit";
						break;

						case 9://///////////////////////////////////////////
							dialog.text = "Ты выглядишь крепким парнем! Не хочешь поступить на службу? У нас просторная казарма, и двухразовое питание, плюс выпивка бесплатна.";
							link.l1 = "Соблазнительно, черт побери, но я вынужден отказаться. Казарменная муштра не для меня.";
							link.l1.go = "exit";
						break;

						case 10:
							dialog.text = "Эта жара... Я бы пол жизни отдал, что бы снова оказаться в Европе.";
							link.l1 = "Да, смотрю, для здешнего климата ты здоровьем не вышел.";
							link.l1.go = "exit";
						break;
					}
					link.l3 = "Нда... ясно. Я вот что хотел спросить...";
					link.l3.go = "quests";//(перессылка в файл города)
				}
			}
		break;
		//============================== ноды на разборки при распознавании =========================
		case "PegYou":
			dialog.text = RandPhraseSimple("Сдается мне, что это обман... Давай-ка пройдем в комендатуру, голубчик, там разберемся...", "Хм, что-то подсказывает мне, что ты не тот, за кого себя выдаешь... Немедленно сдайте оружие, " + GetAddress_Form(npchar) + ", и следуте за мной для дальнейшего разбирательства!");
			link.l1 = RandPhraseSimple("Как бы не так!", "После дождичка, в четверг...");
			link.l1.go = "fight";
			AddCharacterExpToSkill(pchar, SKILL_SNEAK, 80); // враг, которого узнали - потом будет умнее - бонус в скрытность
		break;
		case "NotPegYou":
			dialog.text = RandPhraseSimple("А-а-а, вижу... Все в порядке, вы можете идти, " + GetAddress_Form(pchar) + ".", "Что-то я немного подустал в карауле... Все в порядке, " + GetAddress_Form(pchar) + ", прошу прощения.");
			link.l1 = "Так-то!";
			link.l1.go = "exit";
		break;
		case "LicenceOk":
			iTemp = GetDaysContinueNationLicence(sti(npchar.nation));
			if (iTemp == -1)
			{
				dialog.text = "Ваша лицензия подлежит изъятию, так как просрочена и поэтому недействительна. Сдайте оружие и следуйте за мной для последующих разбирательств!";
				link.l1 = RandPhraseSimple("Как бы не так!", "После дождичка, в четверг...");
				link.l1.go = "fight";	
				TakeNationLicence(sti(npchar.nation));
				AddCharacterExpToSkill(pchar, SKILL_SNEAK, 20); // враг, которого узнали - потом будет умнее - бонус в скрытность
			}
			if (iTemp == 0)
			{
				dialog.text = "Хм, все верно. Однако позволю себе заметить, что срок действия вашей лицензии сегодня истекает. Я пропущу вас сейчас, но вам нужно будет сменить лицензию на действительную.";
				link.l1 = "Спасибо, я обзаведусь новой при первой же возможности.";
				link.l1.go = "exit";			
			}
			if (iTemp > 0 && iTemp <= 10)
			{
				dialog.text = "Хм, все верно. Однако позволю себе заметить, что срок действия вашей лицензии вскоре истекает - она действительна еще только " + FindRussianDaysString(iTemp) + ". Так что имейте в виду, " + GetAddress_Form(npchar) + ".";
				link.l1 = "Спасибо, я обзаведусь новой при первой же возможности.";
				link.l1.go = "exit";			
			}
			if (iTemp > 10)
			{
				dialog.text = LinkRandPhrase("Ну что же, очень хорошо, ваша лицензия действует еще " + FindRussianDaysString(iTemp) + ". Вы можете пройти.", "Все ясно, "+GetAddress_Form(npchar)+". Вы можете свободно проходить в город, ваша лицензия действует еще " + FindRussianDaysString(iTemp) + ". Прошу прощения за беспокойство.", "Все в порядке, " + GetAddress_Form(npchar) + ", не смею вас задерживать.");
				link.l1 = RandPhraseSimple("Отлично. Всего хорошего.", "Спасибо, офицер.");
				link.l1.go = "exit";
			}
		break;
		//============================== ноды маяка Порт Рояля =========================
		case "PortRoyal_Mayak":
			dialog.text = RandPhraseSimple("Эй, приятель, смотри ничего не сломай на маяке.", "Маяк - очень важный объект для города. Будь осторожен!");
			link.l1 = RandPhraseSimple("Хорошо, не переживай.", "Все будет в порядке.");
			link.l1.go = "exit";
			NextDiag.TempNode = "PortRoyal_Mayak";
		break;
		case "PortRoyal_Gans":
			dialog.text = LinkRandPhrase("Не нужно ходить возле орудий - это военный объект!", "Доступ к орудиям посторонних лиц категорически запрещен!", "Если я замечу, что ты ошиваешься возле орудий - тебе конец!");
			link.l1 = RandPhraseSimple("Я понял тебя.", "Хорошо, я все понял.");
			link.l1.go = "exit";
			NextDiag.TempNode = "PortRoyal_Gans";
		break;
		//=================== ноды квеста мэра. поиск шпиона на улице ==================
		case "SeekSpy_Checking":
			dialog.text = "Да, я подтверждаю. " + NPCharSexPhrase(&characters[sti(pchar.GenQuest.SeekSpy.BaseIdx)], "Он действительно местный житель.", "Она действительно местная жительница.");
			link.l1 = RandPhraseSimple("Я понял. Спасибо за помощь.", "Все ясно. Спасибо за помощь.");
			link.l1.go = "SeekSpy_Checking_1";
		break;
		case "SeekSpy_Checking_1":
			switch (pchar.GenQuest.SeekSpy.Type)
			{
				case "guardian": LAi_SetGuardianTypeNoGroup(npchar); break;
				case "patrol":   LAi_SetPatrolTypeNoGroup(npchar);   break;
				case "citizen":  LAi_SetCitizenTypeNoGroup(npchar);  break;
				case "merchant": LAi_SetMerchantTypeNoGroup(npchar); break;
			}
			LAi_SetCitizenTypeNoGroup(&characters[sti(pchar.GenQuest.SeekSpy.BaseIdx)]);
			NextDiag.CurrentNode = "First Time";
			DialogExit();
		break;


	}
}