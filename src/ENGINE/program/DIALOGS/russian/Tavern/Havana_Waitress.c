// диалог по городам
void ProcessCommonDialogEvent(ref NPChar, aref Link, aref NextDiag)
{
	ref sld;
    float locx, locy, locz, fTemp;
	switch (Dialog.CurrentNode)
	{
		case "quests":
			dialog.text = NPCStringReactionRepeat(RandPhraseSimple("Вопросы, " + GetAddress_Form(NPChar) + "?", "Что вам угодно, " + GetAddress_Form(NPChar) + "?"), "Хм, уж не пристаете ли вы к мне, " + GetAddress_Form(NPChar) + "? Опять со странным вопросами...", "В течение этого дня вы уже третий раз говорите о каком-то вопросе... Это знаки внимания?",
                          "Опять вопросы будете задавать, " + GetAddress_Form(NPChar) + "?", "block", 1, npchar, Dialog.CurrentNode);
			link.l1 = HeroStringReactionRepeat(RandPhraseSimple("Я передумал...", "Сейчас мне не о чем говорить"), "Да нет, что ты, красавица...",
                      "Никак нет, дорогуша, склероз...", "Да нет, какие вопросы...", npchar, Dialog.CurrentNode);
			link.l1.go = "exit";
			// ==> Проверяем поле состояния квестов.
            if (pchar.questTemp.State == "Sp4Detection_FoundLetter")
            {
    		    dialog.text = "Я вас слушаю, сеньор.";
    			link.l1 = "Послушай, красавица, ты не замечала в таверне ничего необычного в последнее время?";
    			link.l1.go = "Step_S4_1";
			}
            if (pchar.questTemp.State == "Fr3TakeAnna_toHavana")
            {
    		    dialog.text = "Я вас слушаю, сеньор.";
    			link.l1 = "Милая девочка, если твоя душа такая же ангельская, как и внешность, то ты, конечно, не откажешься мне помочь.";
    			link.l1.go = "Step_F3_1";
			}
            if (pchar.questTemp.State == "Fr3TakeAnna_WaitOneDay")
            {
				if (GetQuestPastDayParam("questTemp") > 0)
				{
    				dialog.text = "Ах, вот и вы сеньор! Как раз вовремя.";
    				link.l1 = "Ага, вижу, что ты передала кольцо...";
    				link.l1.go = "Step_F3_4";
				}
				else
				{
    				dialog.text = "Сеньор, я еще не выполнила вашего поручения. Подождите, будте терпеливы, прошу вас...";
    				link.l1 = "Хорошо, дорогуша, жду...";
    				link.l1.go = "exit";
				}
			}
            // <== Проверяем поле состояния квестов.
		break;
//========================== Исп. линейка, квест №4. Расследование убийства ==================================
 	 	case "Step_S4_1":
			dialog.text = "Нет, сеньор, все в порядке.";
			link.l1 = "Странных людей не замечала, ничего такого необычного?";
			link.l1.go = "Step_S4_2";
        break;
 	 	case "Step_S4_2":
			dialog.text = "Странных людей у нас в таверне много, всех и не упомнишь...";
			link.l1 = "А незнакомцев в последнее время не видела здесь?";
			link.l1.go = "Step_S4_3";
        break;
 	 	case "Step_S4_3":
			dialog.text = "Бывают разные люди... Послушайте, сеньор, мне нужно работать. Не мешайте мне, пожалуйста.";
			link.l1 = "Вынужден помешать. Ты знаешь об убийстве в доме коменданта Гаваны дона Хосе Рамиреса де Лейва?";
			link.l1.go = "Step_S4_4";
        break;
 	 	case "Step_S4_4":
			dialog.text = "(бледнеет, пауза) Знаю...";
			link.l1 = "Так вот, я расследую это дело по поручению генерал-губернатора Гаваны. Я вижу, что ты что-то знаешь. Отвечай немедленно или сегодня же окажешься в тюрьме!";
			link.l1.go = "Step_S4_5";
        break;
 	 	case "Step_S4_5":
			dialog.text = "Хорошо, сеньор, я все скажу! Только пообещайте мне, что ничего плохого мне не сделаете.";
			link.l1 = "Обещаю, что если ты не убийца - я ничего тебе не сделаю.";
			link.l1.go = "Step_S4_6";
        break;
 	 	case "Step_S4_6":
			dialog.text = "Я не убийца сеньор, конечно же нет...";
			link.l1 = "Да уж, завалить пятерых здоровенных мужиков в полной боевой выкладке - не официантское это дело. А теперь говори все, что знаешь.";
			link.l1.go = "Step_S4_7";
        break;
 	 	case "Step_S4_7":
			dialog.text = "Сеньор, я кажется знаю человека, который убил дона Хосе и увез донну Анну...";
			link.l1 = "Кто он?";
			link.l1.go = "Step_S4_8";
        break;
 	 	case "Step_S4_8":
			dialog.text = "Он появился у нас неожиданно, не припомню, чтобы раньше я его видела. Так вот, этот незнакомец, по разговору француз, попросил передать донне Анне кольцо. Я и передала, что в этом плохого? Донна получила кольцо и с минуту ничего не говорила, только смотрела отсутствующим взглядом в стену. Затем приказала подождать, тут же написала записку и приказала передать этому французу.\n"+
                          "Когда я получила записку, то заподозрила неладное. Но все равно отдала ее французу, так как по всему видно, что это страшный человек. Для такого отправить человека к праотцам легче, чем выпить стакан вина. А взгляд у него колючий, как острие шпаги. Скорей всего это один из тех дьяволов, которых мы называем ладроны, а сами они себя называют флибустьерами.";
			link.l1 = "Черт возьми! Значит, это ты известила донну Анну... Что тебе еще известно?";
			link.l1.go = "Step_S4_9";
        break;
 	 	case "Step_S4_9":
			dialog.text = "Больше ничего сеньор. Этот человек забрал записку и ушел. А утром по городу разнеслась весть о том, что дон Хуан мертв. Вот тут-то я и догадалась, что к чему. Только никому не сказала, боялась, что меня причислят к соучастникам преступления. Сеньор, вы обещали не делать мне ничего плохого. Прошу вас, сдержите свое слово!";
			link.l1 = "Хорошо, я оставлю тебя на свободе. Хотя понятно, что это ты привела убийцу в дом дона Хосе. Однако полагаю, что если не через тебя, то как-нибудь иначе он все равно проник бы к донне Анне. Так что живи пока...";
			link.l1.go = "Step_S4_10";
        break;
 	 	case "Step_S4_10":
			dialog.text = "Спасибо, сеньор, да хранит вас Господь! Я очень сожалею, что так все вышло, но я ни в чем не виновата!";
			link.l1 = "Если увидишь этого француза еще раз - немедленно вызывай стражу. Хотя, вряд ли он появится здесь еще раз.";
			link.l1.go = "exit";
            pchar.questTemp.State = "Sp4Detection_toGovernor";
            AddQuestRecord("Spa_Line_4_MirderDetection", "3");
        break;
//========================== Фр. линейка, квест №3. Увезти донну Анну ==================================
 	 	case "Step_F3_1":
			dialog.text = "Ну что вы, право... Такому благородному кабальеро отказать ни в чем невозможно!";
			link.l1 = "Мне нужно, чтобы ты отнесла это кольцо донне Анне Рамирес де Лейва, жене коменданта. Но только так, чтобы об этом не знала ни одна живая душа!";
			link.l1.go = "Step_F3_2";
        break;
 	 	case "Step_F3_2":
			dialog.text = "Ах, у донны Анны и так много красивых колец, зачем ей еще одно? А у бедной девушки нет ничего...";
			link.l1 = "Плутовка, я понял, на что ты намекаешь! Ну, держи 500 монет, и еще столько же получишь, когда принесешь ответ от донны Анны. На эти деньги ты купишь себе сколько угодно украшений.";
			link.l1.go = "Step_F3_3";
			AddMoneyToCharacter(pchar, -500);
        break;
 	 	case "Step_F3_3":
			dialog.text = "Спасибо, сеньор! Вам придется подождать день-другой. Я должна сдать смену и найти время для того, чтобы сходить к дому сеньоры де Лейва. Да и случай нужен подходящий - так просто к ним в дом не заявишься...";
			link.l1 = "Очень хорошо, милая. Я жду тебя...";
			link.l1.go = "exit";
			TakeItemFromCharacter(pchar, "DOjeronRing");
			pchar.questTemp.State = "Fr3TakeAnna_WaitOneDay";
			SaveCurrentQuestDateParam("questTemp");
			AddQuestRecord("Fra_Line_3_TakeAnna", "2");
        break;
 	 	case "Step_F3_4":
			dialog.text = "Да, передала.";
			link.l1 = "Хорошо. А теперь расскажи мне, что она тебе сказала...";
			link.l1.go = "Step_F3_5";
        break;
 	 	case "Step_F3_5":
			dialog.text = "А разве сеньор не хочет еще помочь бедной девушке?\n"+
				          "Ох, сеньор, прошу вас, не сжимайте мне руку так сильно! У меня потом останутся синяки от ваших пальцев!";
			link.l1 = "Ладно... Послушай, я прошу тебя говорить быстро и по существу. Не тяни кота за хвост, милая - целее будешь...";
			link.l1.go = "Step_F3_6";
        break;
 	 	case "Step_F3_6":
			dialog.text = "Хорошо, сеньор... Я отдала ей ваш перстень. И знаете что, сеньор? Когда она его увидела, то побледнела, как полотно.";
			link.l1 = "И что? Что она сказала?";
			link.l1.go = "Step_F3_7";
        break;
 	 	case "Step_F3_7":
			dialog.text = "Ничего особенного. Приказала подождать, тут же написала записку и передала ее мне.";
			link.l1 = "Давай записку сюда!";
			link.l1.go = "Step_F3_8";
        break;
 	 	case "Step_F3_8":
			dialog.text = "Хорошо, сеньор, только не делайте мне снова больно... Вот она, возьмите.";
			link.l1 = "Молодец, хорошая девочка. Не бойся, возьми вот еще денег, купи себе что-нибудь... И держи язык за зубами, красавица. Тебе понятно?";
			link.l1.go = "Step_F3_9";
        break;
 	 	case "Step_F3_9":
			dialog.text = "Понятно, сеньор, чай не дурой уродилась.";
			link.l1 = "Ну вот и славно.";
			link.l1.go = "exit";
			AddMoneyToCharacter(pchar, -500);
			AddQuestRecord("Fra_Line_3_TakeAnna", "3");
            ChangeItemDescribe("letter_open", "itmdescr_letter_open_FraLineQ3");
            GiveItem2Character(pchar,"letter_open");
			pchar.questTemp.State = "Fr3TakeAnna_WaitMidnight";
        break;
//========================== Тревога в таверне Гаваны ==================================
		case "Allarm": 
    		dialog.text = "Я узнала его! Стража! Кто-нибудь, позовите стражу, этого ладрона нужно задержать!";
    		link.l1 = "А-ть, каррамба!!";
    		link.l1.go = "Step_F_Allarm";
			NextDiag.TempNode = "First time";
        break;
		case "Step_F_Allarm":
			LAi_LocationFightDisable(&Locations[FindLocation(pchar.location)], false);
			chrDisableReloadToLocation = true; // закрыть выход из локации.
			GetCharacterPos(pchar, &locx, &locy, &locz);
            for (int i=1; i<=3; i++)
            {
                sld = GetCharacter(NPC_GenerateCharacter("SpaGuard"+i, "sold_spa_"+(rand(7)+1), "man", "man", 20, SPAIN, 0, true));
				FantomMakeCoolFighter(sld, 20, 60, 50, BLADE_LONG, "pistol3", 30);
				LAi_SetWarriorType(sld);
                LAi_group_MoveCharacter(sld, "EnemyFight");
                ChangeCharacterAddressGroup(sld, "Havana_tavern", "goto",   LAi_FindFarLocator("goto", locx, locy, locz));
            }
            LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
            LAi_group_FightGroups("EnemyFight", LAI_GROUP_PLAYER, true);
            LAi_group_SetCheck("EnemyFight", "CannotFightCurLocation");
			SetNationRelation2MainCharacter(SPAIN, RELATION_ENEMY); //на всякий случай	
			LAi_SetWaitressType(npchar);
			LAi_group_MoveCharacter(npchar, "SPAIN_CITIZENS");
			LAi_group_Attack(NPChar, Pchar);
			DialogExit();
        break;

	}
	UnloadSegment(NPChar.FileDialog2);  // если где-то выход внутри switch  по return не забыть сделать анлод
}

