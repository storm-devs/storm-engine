// диалог по городам
void ProcessCommonDialogEvent(ref NPChar, aref Link, aref NextDiag)
{
    ref sld;
    switch (Dialog.CurrentNode)
	{
		case "quests":
			dialog.text = NPCStringReactionRepeat(RandPhraseSimple("Что вы хотели? Спрашивайте.", "Я слушаю вас, что за вопрос?"), "Второй раз за день вы пытаетесь задать ворпос...", "В третий раз за день вы опять пытаетесь задать вопрос...",
                          "Да когда же это кончится?! У меня дел полно по управлению делами города, а ты все вопросы пытаешься задать!", "block", 1, npchar, Dialog.CurrentNode);
			link.l1 = HeroStringReactionRepeat(RandPhraseSimple("Я передумал...", "Не сейчас, не место и не время..."), "Да, верно... Но не сейчас, позже...",
                      "Задам, задам... Только позже...", "Извините, " + GetAddress_FormToNPC(NPChar) + "...", npchar, Dialog.CurrentNode);
			link.l1.go = "exit";
            //===> Проверка состояния текущего квеста
            if (pchar.questTemp.State == "Sp5LaVegaAttack_toSantiago")
            {
    			dialog.text = "Я вас слушаю, сеньор.";
    			link.l1 = "Я командирован в ваше распоряжение генерал-губернатором Гаваны.";
    			link.l1.go = "Step_S5_1";
            }
            if (pchar.questTemp.State == "Sp5LaVegaAttack_WeWon")
            {
    			dialog.text = "Что скажете, офицер?";
    			link.l1 = "Все сделано в лучшем виде. Поселение буканьеров Ла Вега уничтожено, их главарь, Мэнсфилд, убит мной собственноручно.";
    			link.l1.go = "Step_S5_6";
            }
            //<=== Проверка состояния текущего квеста
		break;

		case "Cupture_after":
            dialog.text = RandPhraseSimple("Вы уже все забрали. Что вам еще нужно?", "Неужели осталось еще что-то, что вы не прихватили?");
            link.l1 = RandPhraseSimple("Осматриваюсь напоследок...", "Проверяю, может забыл что забрать...");
            link.l1.go = "exit";
            NextDiag.TempNode = "Cupture_after";
		break;

        //********************** Девятый квест англ.линейки: нападение на Сантьяго ************************
        case "EL9_SantiagoAttack":
			dialog.text = "Ты победил, ладрон!";
			link.l1 = "Сеньор, я не люблю, когда меня так называют. Я бы предпочел, чтобы меня называли капером.";
			link.l1.go = "Step_E9_1";
			AfterTownBattle();
        break;
        case "Step_E9_1":
            dialog.text = "Обойдешься, английская свинья.";
			link.l1 = "Боюсь, что нет, сеньор. Дело в том, что мы уходим не одни. Вы составите нам компанию в путешествии к берегам Ямайки. Сэр Томас Мэдифорд жаждет встречи с вами.";
			link.l1.go = "Step_E9_2";
        break;
        case "Step_E9_2":
            dialog.text = "Что?!!! По какому праву?!!!";
			link.l1 = "По праву победителя, сеньор. А посему, так как нам, в силу обстоятельств, некоторое время придется общаться, я просил бы вас называть меня именно так - 'капер', но не 'ладрон'.";
			link.l1.go = "Step_E9_3";
        break;
        case "Step_E9_3":
            dialog.text = "Маноэль Риверо Пардаль уже спешит ко мне на выручку. Далеко тебе не уйти...";
			link.l1 = "Мне не в диковинку встреча с испанцем в море. Пардаль так Пардаль, никаких проблем. Собирайтесь, досточтимый сеньор. И побыстрее!";
			link.l1.go = "Step_E9_4";
        break;
        case "Step_E9_4":
            Statistic_AddValue(Pchar, NationShortName(sti(NPChar.nation)) + "_GrabbingTown", 1); // статистика
			AddQuestRecord("Eng_Line_9_AttackSantiago", "2");
			chrDisableReloadToLocation = false;
            Pchar.quest.AttackSantiago_FightWithPardal.win_condition.l1 = "location";
            Pchar.quest.AttackSantiago_FightWithPardal.win_condition.l1.location = "Cuba1";
            Pchar.quest.AttackSantiago_FightWithPardal.win_condition = "AttackSantiago_FightWithPardal";
            // ==> Меняем имя и модель у губера,
            npchar.model = "usurer_3";
            npchar.name 	= "Грегорио Флоридо";
        	npchar.lastname = "де Бальбоа";
        	SendMessage(npchar, "lss", MSG_CHARACTER_SETMODEL, npchar.model, npchar.model.animation);
        	FaceMaker(npchar);
            // ==> а пленным делаем Альбермаля.
            sld = GetCharacter(NPC_GenerateCharacter("Himenes", "huber_spa", "man", "man", 10, SPAIN, -1, false));
            LAi_SetStayTypeNoGroup(sld);
            sld.name 	= "Хосе Санчо";
        	sld.lastname = "Хименес";
            SetCharacterRemovable(sld, false);
            AddPassenger(pchar, sld, true);
			SetReturn_Gover_Dialog_Exit(NPChar);
            DoReloadCharacterToLocation("Santiago_town",  "reload", "reload3");
            NextDiag.CurrentNode = "Cupture_after";
            chrDisableReloadToLocation = false;
            DialogExit();
        break;
        //********************** Квест №5, Нападение на Ла Вегу. ************************
        case "Step_S5_1":
            dialog.text = "Да, очень хорошо, что вы здесь. Вы готовы к ведению боевых действий на суше?";
			link.l1 = "Всегда готов.";
			link.l1.go = "Step_S5_2";
        break;
        case "Step_S5_2":
            dialog.text = "Прекрасно. В таком случае, извольте выслушать то, что вам надлежит выполнить.";
			link.l1 = "Я весь внимание.";
			link.l1.go = "Step_S5_3";
        break;
        case "Step_S5_3":
            dialog.text = "Вы должны атаковать и уничтожить поселение буканьеров на Эспаньоле, называемое Ла Вега. Заправляет там всем некий Эдвард Мэнсфилд, старый ладрон. Данное поселение подлежит тотальному уничтожению, все, найденное там - ваша законная добыча. Эспаньола должна быть полностью испанской, и первое препятствие на пути к этому - Ла Вега. Я ожидаю новую партию колонистов, им нужно пространство для жизни.";
			link.l1 = "Понятно. Я могу приступать?";
			link.l1.go = "Step_S5_4";
        break;
        case "Step_S5_4":
            dialog.text = "Конечно, и чем раньше, тем лучше. Я еще раз хотел бы заострить ваше внимание на том, что поселение должно быть совершенно уничтожено, там не должно остаться ни одного живого человека. Эспаньола - наш остров, на нем нет места этому сброду.";
			link.l1 = "Я понимаю и немедленно приступаю к выполнению задачи.";
			link.l1.go = "Step_S5_5";
        break;
        case "Step_S5_5":
            dialog.text = "Превосходно! Дон Франсиско отрекомендовал вас как крайне способного и исполнительного боевого офицера. Я жду вас с хорошими новостями.";
			link.l1 = "Могу вас уверить, что Ла Вега в скором времени перестанет существовать.";
			link.l1.go = "exit";
            pchar.questTemp.State = "Sp5LaVegaAttack_AttackLaVega";
            AddQuestRecord("Spa_Line_5_LaVegaAttack", "2");
            Pchar.quest.Sp5LaVegaAttack_AttackGoOn.win_condition.l1 = "location";
            Pchar.quest.Sp5LaVegaAttack_AttackGoOn.win_condition.l1.location = "LaVega_ExitTown";
            Pchar.quest.Sp5LaVegaAttack_AttackGoOn.win_condition = "Sp5LaVegaAttack_AttackGoOn";
            pchar.GenQuestBox.LaVega_townhall.box1.items.jewelry3 = Rand(7)+1;
            pchar.GenQuestBox.LaVega_townhall.box1.items.jewelry4 = Rand(7)+1;
            pchar.GenQuestBox.LaVega_townhall.box1.items.jewelry5 = Rand(7)+1;
            pchar.GenQuestBox.LaVega_townhall.box1.items.jewelry7 = Rand(7)+1;
            pchar.GenQuestBox.LaVega_townhall.box1.items.jewelry8 = Rand(7)+1;
            pchar.GenQuestBox.LaVega_townhall.box1.items.jewelry10 = Rand(7)+1;
            pchar.GenQuestBox.LaVega_townhall.box1.items.jewelry14 = Rand(7)+1;
            pchar.GenQuestBox.LaVega_townhall.box1.items.jewelry17 = Rand(7)+1;
            pchar.GenQuestBox.LaVega_townhall.box1.items.cirass4 = 1;
			locations[FindLocation("LaVega_townhall")].models.day.charactersPatch = "SmallResidence_BoxPatch";
			sld = characterFromId("Edward Mansfield");
			sld.Default.Crew.Quantity    = 1700;
			sld.Default.Crew.MinQuantity = 1100;
			sld.from_sea = ""; // для захвата с суши
			sld.Default  = "LaVega_townhall";  // чтоб сухопутные города вернули население
			sld.Default.BoardLocation = "LaVega_ExitTown";
        break;
        case "Step_S5_6":
            dialog.text = "Прекрасно, сеньор! Просто великолепно! Дон Франсиско Орегон-и-Гаскон может по праву гордится тем, что у него на службе находятся такие офицеры.";
			link.l1 = "Буканьеры - достойный противник, я рад, что сумел выполнить поставленную задачу... Сеньор, а вознаграждение?";
			link.l1.go = "Step_S5_7";
        break;
        case "Step_S5_7":
            dialog.text = "Ах да... Выполнение столь сложного задания оценивается городом Сантьяго в 100000 пиастров. Извольте получить.";
			link.l1 = "Великолепно. Сеньор Хименес, мне было приятно иметь с вами дело. Прощайте.";
			link.l1.go = "exit";
			locations[FindLocation("LaVega_townhall")].models.day.charactersPatch = "SmallResidence_patch";
            AddMoneyToCharacter(pchar, 100000);
            pchar.questTemp.State = "Sp5LaVegaAttack_BackToHavana";
            AddQuestRecord("Spa_Line_5_LaVegaAttack", "4");
        break;
	}
    UnloadSegment(NPChar.FileDialog2);  // если где-то выход внутри switch  по return не забыть сделать анлод
}

