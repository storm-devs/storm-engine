// диалог по городам
void ProcessCommonDialogEvent(ref NPChar, aref Link, aref NextDiag)
{
    switch (Dialog.CurrentNode)
	{
		case "quests":
			dialog.text = NPCStringReactionRepeat(RandPhraseSimple("Какие вопросы?", "Что вам угодно, " + GetAddress_Form(NPChar) + "?"), "Совсем недавно вы пытались задать мне вопрос " + GetAddress_Form(NPChar) + "...", "В течение этого дня вы уже третий раз говорите о каком-то вопросе...",
                          "Опять вопросы будем задавать?", "block", 1, npchar, Dialog.CurrentNode);
			link.l1 = HeroStringReactionRepeat(RandPhraseSimple("Я передумал...", "Сейчас мне не о чем говорить"), "Хм, что-то с памятью моей стало...",
                      "Да уж, действительно в третий раз...", "Да нет, какие вопросы...", npchar, Dialog.CurrentNode);
			link.l1.go = "exit";
			// ==> Проверяем поле состояния квестов.
			switch(pchar.questTemp.State)
            {
                case "RevengeMansfield_toTavern": //Голландская линейка, квест №6. Узнать инфу о губере.
                    link.l2 = "Как вы вообще поживаете? Как город, процветает?";
                    link.l2.go = "Step_H6_1";
                break;
                case "Fr7RockBras_toSeekPlace": //Фр. линейка, квест №7. Узнать инфу о Роке Бразильце.
					link.l1 = "Я слышал, что известный пират, Рок Бразилец, попал в руки к Инквизиции...";
					link.l1.go = "Step_F7_1";
                break;
            }
            // <== Проверяем поле состояния квестов.
		break;
//**************************** Голл. линейка, квест №6. Узнать для Моргана инфу о губере Сантьяго ************************
 	 	case "Step_H6_1":
			dialog.text = "Хвала Всемогущему Господу - у нас все хорошо. Город процветает.";
			link.l1 = "В окружении врагов довольно сложно процветать. Как вам это удается?";
			link.l1.go = "Step_H6_2";
        break;
 	 	case "Step_H6_2":
			dialog.text = "Наш губернатор - Хосе Санчо Хименес, умеет решать проблемы. Недавно он избавился от поселения буканьеров на Эспаньоле. Хорошенькое было дело, скажу я вам!";
			link.l1 = "Вот это да! А что, буканьеры вам сильно мешали?";
			link.l1.go = "Step_H6_3";
        break;
 	 	case "Step_H6_3":
			dialog.text = "Да нет, по мне, так с ними было удобней. Я всегда имел хорошее свежее мясо для своей кухни. И дешевое, к тому же. А сейчас с этим стало сложней, все дорожает.";
			link.l1 = "Хм, зачем же тогда ваш губернатор так поступил?";
			link.l1.go = "Step_H6_4";
        break;
 	 	case "Step_H6_4":
			dialog.text = "А черт его знает. Наверное потому, что все они были сплошь - грязные французы. Эспаньола должна быть испанской!";
			link.l1 = "Ну что же, пусть будет... Послушай, я хочу наняться к вашему губернатору на службу. Что порекомендуешь? Он вообще в городе?";
			link.l1.go = "Step_H6_5";
        break;
 	 	case "Step_H6_5":
			dialog.text = "Да, сейчас он здесь. А наняться можешь легко, если обратишься к нему лично.";
			link.l1 = "Понятно... А никуда уезжать не собирается в ближайшее время, ты не в курсе?";
			link.l1.go = "Step_H6_6";
        break;
 	 	case "Step_H6_6":
			dialog.text = "Да нет, никуда. У нас скоро праздник города, поэтому точно будет здесь... Эй, а это тебе зачем?";
			link.l1 = "Да так, просто...";
			link.l1.go = "Step_H6_7";
        break;
 	 	case "Step_H6_7":
			dialog.text = "Просто, да не просто. Если наняться хочешь на службу, то иди и нанимайся. А зачем тебе знать, где будет губернатор в ближайшее время?";
			link.l1 = "Да что ты так взъерепенился, не понимаю?! Сиди себе, жарь мясо, торгуй салом...";
			link.l1.go = "Step_H6_8";
        break;
 	 	case "Step_H6_8":
			dialog.text = "Что-то ты подозрителен дюже, мил человек.\n"+
                          "Ой, сеньоры офицеры, вы очень вовремя! Задержите этого человека до выяснения, похоже, что это шпион!!!";
			link.l1 = "А-а-ть, дъявол!..";
			link.l1.go = "Step_H6_9";
        break;
 	 	case "Step_H6_9":
			LAi_LocationFightDisable(&Locations[FindLocation(pchar.location)], false);
            chrDisableReloadToLocation = true; // закрыть выход из локации.
            AddQuestRecord("Hol_Line_6_RevengeMansfield", "3");
            QuestSetCurrentNode("Henry Morgan", "HolLine6_fromSantiago");
            //==> потом вернуть
            LAi_LocationFightDisable(&Locations[FindLocation(pchar.location)], false);
            //<== потом вернуть
            LAi_group_MoveCharacter(npchar, "SPAIN_CITIZENS");
            LAi_group_MoveCharacter(characterFromID("Santiago_waitress"), "SPAIN_CITIZENS");
            LAi_group_Delete("QuestGroup");
            SetNationRelation2MainCharacter(SPAIN, RELATION_ENEMY); //если флаг сменен.
            ref sld;
            for (int i=1; i<=2; i++)
            {
                sld = GetCharacter(NPC_GenerateCharacter("Solder"+i, "off_spa_"+i, "man", "man", 30, SPAIN, 0, true));
                FantomMakeCoolFighter(sld, 30, 100, 80, "blade33", "pistol3", 120);

            	LAi_SetWarriorType(sld);
                LAi_group_MoveCharacter(sld, "EnemyFight");
               	ChangeCharacterAddressGroup(sld, pchar.location, "reload", "reload1");
            }
            LAi_group_SetLookRadius("EnemyFight", 100);
            LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
            LAi_group_FightGroups("EnemyFight", LAI_GROUP_PLAYER, true);
            LAi_group_SetCheck("EnemyFight", "CannotFightCurLocation");
            DialogExit();
        break;
//**************************** Фр. линейка, квест №7. Узнать инфу о Роке Бразильце ************************
 	 	case "Step_F7_1":
			dialog.text = NPCStringReactionRepeat("Верно, он сейчас у святых отцов.", "Вы уже спрашивали меня о Бризильце.", 
				          "Опять о Бразильце?", "Ну сколько можно об этих пиратах, давайте о другом поговорим...", "block", 0, npchar, Dialog.CurrentNode);
			link.l1 = HeroStringReactionRepeat("Хех, и когда же его повесят теперь?", "Спрашивал? Ну да, наверное...", "Да, точно, спрашивал, извини...", "Можно и о другом...", npchar, Dialog.CurrentNode);
			link.l1.go = DialogGoNodeRepeat("Step_F7_2", "none", "none", "none", npchar, Dialog.CurrentNode);
        break;
 	 	case "Step_F7_2":
			dialog.text = "Это теперь должен сказать сам Антонио де Соуза! Делами Святой Инквизиции здесь теперь он заправляет.";
			link.l1 = "Ну а сама-то Инквизиция где находится?";
			link.l1.go = "Step_F7_3";
        break;
 	 	case "Step_F7_3":
			dialog.text = "Да где же ей находиться? Здесь, в городе, сеньор.\n"+
				          "А вообще, сеньор, лучше бы нам прекратить эти разговоры. Не к добру это, поминать святых отцов всуе...";
			link.l1 = "Ладно, будь здоров, трактирщик.";
			link.l1.go = "exit";
        break;
	}
	UnloadSegment(NPChar.FileDialog2);  // если где-то выход внутри switch  по return не забыть сделать анлод
}

