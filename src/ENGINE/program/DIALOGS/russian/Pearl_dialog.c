
void ProcessDialogEvent()
{
	ref NPChar;
	aref Link, NextDiag;
	string sTemp;
	bool bOk;

	DeleteAttribute(&Dialog,"Links");

	makeref(NPChar,CharacterRef);
	makearef(Link, Dialog.Links);
	makearef(NextDiag, NPChar.Dialog);
	
	switch(Dialog.CurrentNode)
	{
		case "First time":
			dialog.text = "Это бага, сообщите Эдди.";
			link.l1 = "...";
			link.l1.go = "exit";
			NextDiag.TempNode = "First time";
		break;

		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
		// ==> глава поселения
		case "HeadMan":
			NextDiag.TempNode = "HeadMan";
			if (npchar.quest.meeting == "0")
			{
				dialog.text = "Добро пожаловать к нам на промысел. Рад видеть подданного Эскориала. Могу я узнать ваше имя?"; 
				link.l1 = "Меня зовут " + GetFullName(pchar) + ". Я капитан собственного судна.";
				link.l1.go = "HeadMan_1";
				npchar.quest.meeting = "1";
			}
			else
			{
				dialog.text = NPCStringReactionRepeat("Рад вас видеть вновь у себя в доме. Вы по делу?", 
					"Опять вы? Мы уже все проговорили, капитан.", 
					"Капитан, вам еще что-нибудь нужно?",
					"Капитан, я прошу вас не отвлекать меня!", "block", 1, npchar, Dialog.CurrentNode);
				link.l1 = HeroStringReactionRepeat("Нет, " + GetAddress_Form(NPChar) + ", просто так заскочил...", 
					"Да, конечно.",
					"Нет, " + GetAddress_Form(NPChar) + ", простите...", 
					"Хорошо...", npchar, Dialog.CurrentNode);
				link.l1.go = "exit";			
			}
			//-------- жемчужный генератор Шарпа ------------
			if (pchar.questTemp.Sharp != "begin" && pchar.questTemp.Sharp != "over" && pchar.questTemp.Sharp.SeekSpy == "begin")
			{
				link.l2 = "Скажите, а ловцы жемчуга, что промышляют в Карибском море, в вашем подчинении находятся?";
				link.l2.go = "SharpSeekSpy";
			}
			if (pchar.questTemp.Sharp.SeekSpy == "alwaysKnow")
			{
				link.l2 = "Я прибыл за наградой, сеньор.";
				link.l2.go = "SharpSeekSpy_10";
			}
		break;
		// <------- жемчужный генератор Шарпа
		case "SharpSeekSpy":
			dialog.text = "В нашем. А почему вы спрашиваете?";
			link.l1 = "Тяжело им, наверное, среди такого разгула пиратства. Среди корсаров известен некий капитан Шарп...";
			link.l1.go = "SharpSeekSpy_1";
		break;
		case "SharpSeekSpy_1":
			dialog.text = "Он известен и среди ловцов жемчуга, друг мой. Это настоящий бич для нас!\nТакое ощущение, что ему просто делать нечего, только за нашими тартанами гоняться! И узнает же как-то, подлец, кто где промысел ведет.";
			link.l1 = "Да, знает он много о ловцах, это факт...";
			link.l1.go = "SharpSeekSpy_2";
		break;
		case "SharpSeekSpy_2":
			dialog.text = "Мы принимаем все меры предосторожности, но бесполезно. Судя по всему, кто-то его информирует о наших планах.\nПослушайте, вы неспроста завели этот разговор. Вам что-то известно об этом деле.";
			link.l1 = "Только то, что я уже сказал. В Карибском море всем известен капитан Шарп...";
			link.l1.go = "SharpSeekSpy_3";
		break;
		case "SharpSeekSpy_3":
			dialog.text = "Ясно. Ну что же, хочу предложить вам работу, раз вы в курсе наших несчастий.";
			link.l1 = "И что за работа? Устранить Шарпа?";
			link.l1.go = "SharpSeekSpy_4";
		break;
		case "SharpSeekSpy_4":
			dialog.text = "Нет, это слишком просто. К тому же неэффективно. Ведь тот мерзавец, что сливает информацию о месте наших работ, просто найдет себе другого 'шарпа', и наши тартаны опять будут грабить.";
			link.l1 = "Логично.";
			link.l1.go = "SharpSeekSpy_5";
		break;
		case "SharpSeekSpy_5":
			dialog.text = "Я хочу, чтобы вы нашли эту крысу среди нас.";
			link.l1 = "Понятно. Теперь обсудим гонорар...";
			link.l1.go = "SharpSeekSpy_6";
		break;
		case "SharpSeekSpy_6":
			dialog.text = "Хм, ну давайте обсудим. Итак, я мог бы предожить вам жемчуг и деньги, но я предложу нечто, на мой взгляд, более интересное для вас.\nЭто комплект вооружения. В него входит бретерский пистолет, необычные боевые когти, секира и великолепная золотая кираса! Поверьте мне, одна кираса дорогого стоит!";
			link.l1 = "Хех, как все заманчиво...";
			link.l1.go = "SharpSeekSpy_7";
		break;
		case "SharpSeekSpy_7":
			dialog.text = "И прибавьте к этому крысиного бога, что я дам вам прямо сейчас, авансом\nНу, соглашайтесь!";
			link.l1 = "Согласен, черт меня дери! Еще бы я не согласился!";
			link.l1.go = "SharpSeekSpy_8";
			link.l2 = "Нет, пожалуй, я откажусь.";
			link.l2.go = "SharpSeekSpy_8over";
		break;

		case "SharpSeekSpy_8":
			dialog.text = "Тогда я ждут от вас хороших вестей, капитан! Отчитаться и получить обещанную награду вы сможете у любого из руководителей двух жемчужных поселений, все будут в курсе нашего договора.";
			link.l1 = "Я понял. Приступаю к поискам немедленно.";
			link.l1.go = "SharpSeekSpy_9";
		break;
		case "SharpSeekSpy_9":
			GiveItem2Character(pchar, "indian11");
			pchar.questTemp.Sharp.SeekSpy = "seekSpy";
            SetQuestHeader("SharpPearl_SeekSpy");
            AddQuestRecord("SharpPearl_SeekSpy", "1");
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;

		case "SharpSeekSpy_8over":
			dialog.text = "Странно. Вы приходите ко мне, заводите речи о Шарпе, а потом отказываетесь от работы. Все это подозрительно\nБоюсь, я не могу дать вам уйти живым отсюда.";
			link.l1 = "Что?!..";
			link.l1.go = "SharpSeekSpy_8fight";
		break;
		case "SharpSeekSpy_8fight":
			pchar.questTemp.Sharp.SeekSpy = "over";
			LAi_group_Attack(npchar, pchar);
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;

		case "SharpSeekSpy_10":
			dialog.text = "Та-а-ак, мне известно, кто вы. Ну, расскавайте, что вы сумели раскопать.";
			link.l1 = "Я нашел человека из ваших, кто сливал Шарпу информацию о ваших тартанах. Это Эмилио Гарсиа.";
			link.l1.go = "SharpSeekSpy_11";
		break;
		case "SharpSeekSpy_11":
			dialog.text = "Вы можете это доказать?";
			link.l1 = "Да. Мне удалось выследить его и настигнуть в пещере залива Сан-Хуан дель Норте как раз в момент разговора с доверенным лицом Шарпа. Я убил их обоих.";
			link.l1.go = "SharpSeekSpy_12";
		break;
		case "SharpSeekSpy_12":
			dialog.text = "Понятно... Ну что же, я верю вам. Ваша награда за проделанную работу - комплект боевой аммуниции. Верно?";
			link.l1 = "Точно так, сеньор.";
			link.l1.go = "SharpSeekSpy_13";
		break;
		case "SharpSeekSpy_13":
			dialog.text = "Тогда извольте получить. Все это отныне ваше.";
			link.l1 = "Очень раз, что смог оказаться вам полезен. Прощайте, сеньор.";
			link.l1.go = "exit";
			GiveItem2Character(pchar, "arm1");
			GiveItem2Character(pchar, "pistol5");
			GiveItem2Character(pchar, "cirass5");
			GiveItem2Character(pchar, "topor2");
			pchar.questTemp.Sharp.SeekSpy = "over";
            AddQuestRecord("SharpPearl_SeekSpy", "5");
			CloseQuestHeader("SharpPearl_SeekSpy");
		break;
		// >------- жемчужный генератор Шарпа
		case "HeadMan_1":
			dialog.text = "Могу я узнать, что вы делаете в этой глуши?";
			link.l1 = "Обследую территории, возможно в будущем эти знания мне пригодятся.";
			link.l1.go = "HeadMan_2";
		break;
		case "HeadMan_2":
			dialog.text = "Значит, вы исследователь. Что же, великолепно. Хочу ввести вас в курс дела относительно существующего у нас положения вещей.";
			link.l1 = "Слушаю внимательно.";
			link.l1.go = "HeadMan_3";
		break;
		case "HeadMan_3":
			dialog.text = "Жемчужные промыслы принадлежат Эскориалу, поэтому никто не может здесь покупать или продавать жемчуг.";
			link.l1 = "Понятно.";
			link.l1.go = "HeadMan_4";
		break;
		case "HeadMan_4":
			dialog.text = "Если вы попытаетесь проникнуть в мои сундуки - кара постигнет вас незамедлительно.";
			link.l1 = "Это тоже понятно.";
			link.l1.go = "HeadMan_5";
		break;
		case "HeadMan_5":
			dialog.text = "Ну что же, великолепно. Примите мои заверения в искреннем расположении к вам, " + GetAddress_Form(NPChar) + ".";
			link.l1 = "Благодарю вас.";
			link.l1.go = "exit";
		break;
		// ==> часовой у поселения
		case "GuardMan":
			NextDiag.TempNode = "GuardMan";
			dialog.text = NPCStringReactionRepeat("Здравствуйте! Позвольте представиться, меня зовут " + GetFullName(npchar) + ". Моя задача - предупредить поселение ловцов жемчуга о возможном нападении. Пираты, знаете ли... А вы кто?", 
				"Приветствую еще раз!", 
				"Стою тут один целыми днями, скучно, поговорить не с кем...",
                "Эх, черт возьми, приятная отдушина в наряде!", "block", 1, npchar, Dialog.CurrentNode);
			link.l1 = HeroStringReactionRepeat("Меня зовут " + GetFullName(pchar) + ". Я капитан, осматриваю эти земли.", 
				"И я тебя тоже...",
                "Понимаю.", 
				"Хех...", npchar, Dialog.CurrentNode);
			link.l1.go = DialogGoNodeRepeat("GuardMan_1", "exit", "exit", "exit", npchar, Dialog.CurrentNode);
		break;

		case "GuardMan_1":
			dialog.text = "Если вы в наше поселение, то добро пожаловать.";
			link.l1 = "Спасибо, приятель.";
			link.l1.go = "GuardMan_2";
		break;
		case "GuardMan_2":
			chrDisableReloadToLocation = false;
			npchar.quest.TalkOk = true; //флаг разговор состоялся, все ОК.
			LAi_SetCitizenTypeNoGroup(npchar);
			DialogExit();
		break;

		case "GuardManAllarm":
			dialog.text = "Тревога!! Это пират!!!";
			link.l1 = "Дьявол! Надо было тебя прикончить...";
			link.l1.go = "GuardManAllarm_1";
		break;
		case "GuardManAllarm_1":
			chrDisableReloadToLocation = false;
			LAi_SetWarriorTypeNoGroup(npchar);
			LAi_group_Attack(NPChar, Pchar);
			DialogExit();
		break;
		// ==> пиплы в поселении
		case "PearlMan":
			NextDiag.TempNode = "PearlMan";
			dialog.text = NPCStringReactionRepeat("Приветствую! Меня зовут " + GetFullName(npchar) + ". Чем я могу тебе помочь?", 
				"Приветствую еще раз!", 
				"В нашей глуши гости очень редки...",
                "Эх, прятно поболтать с новым человеком! А то эти морды уже опротивели.", "block", 1, npchar, Dialog.CurrentNode);
			link.l1 = HeroStringReactionRepeat("Да ничем особенным...", 
				"И я тебя тоже...",
                "Понимаю.", 
				"Хех...", npchar, Dialog.CurrentNode);
			link.l1.go = "PearlMan_1";
			if (pchar.questTemp.Sharp.SeekSpy == "seekSpy")
			{
				link.l2 = LinkRandPhrase("Слушай, мне тут ваш босс поручил кое-что... В общем, ты не в курсе, кто-нибудь с пиратами тут у вас встречается?", 
					"Ты не знаешь, приятель, кто у вас тут с пиратам связи имеет?", 
					"У меня к тебе вопрос, дружище. Кто тут у вас замечен в связях с пиратами?");
				link.l2.go = "PearlMan_Sharp_1";
			}
		break;
		case "PearlMan_1":
			dialog.text = "Мы тут жемчуг добываем, работаем на Эскориал, такое вот дело...";
			link.l1 = "Да я в курсе...";
			link.l1.go = "exit";
			link.l2 = "Слушай, а нельзя ли прикупить немного этих безделушек у производителя, так сказать, а?";
			link.l2.go = "PearlMan_2";
		break;
		case "PearlMan_2":
			if (rand(10) < 9)
			{
				dialog.text = "Да нет, что ты!! За кражу даже жемчужины - смерть!";
				link.l1 = "Хех, порядки тут у вас жестокие...";
				link.l1.go = "exit";
			}
			else
			{
				dialog.text = "У нас с этим делом жестко, можно запросто лишиться головы. Я этим не занимаюсь, но могу посоветовать тебе обратиться к местным индейцам, что работают вместе с нами. Им есть куда бежать в случае чего, они посмелее нас будут...";
				link.l1 = "Понятно, ну спасибо тебе.";
				link.l1.go = "exit";
			}
		break;
		case "PearlMan_Sharp_1":
			dialog.text = LinkRandPhrase("Ты чего, на голову больной? Иди отсюда, придурок!", 
				"Чего?!! Ты, часом, белены не объелся, идиот?! Вали отсюда со своими вопросами...", 
				"Ты в своем уме, родной? Какие пираты?! У нас?!! Шел бы ты отсюда, малахольный...");
			link.l1 = RandPhraseSimple("Не надо хамить...", "Что за выражения?!");
			link.l1.go = "exit";
		break;

		// ==> индейцы в поселении
		case "IndPearlMan":
			NextDiag.TempNode = "IndPearlMan";
			dialog.text = NPCStringReactionRepeat("Бледнолицый хочет говорить?", 
				"Снова ты, бледнолицый.", 
				"Бледнолицый любит говорить. Он похож на скво.",
                "Духи привели ко мне бледнолицего брата.", "block", 1, npchar, Dialog.CurrentNode);
			link.l1 = HeroStringReactionRepeat("Да.", 
				"Да, снова я.",
                "Очень поэтично.", 
				"Я тоже рад тебя видеть.", npchar, Dialog.CurrentNode);
			link.l1.go = "IndPearlMan_1";
		break;
		case "IndPearlMan_1":
			dialog.text = "Говори, зачем пришел.";
			link.l1 = "Да ничего особенного, послушать тебя хотел...";
			link.l1.go = "exit";
			link.l2 = "У тебя есть жемчуг на продажу?";
			link.l2.go = "IndPearlMan_2";
			if (pchar.questTemp.Sharp.SeekSpy == "seekSpy")
			{
				link.l3 = "Послушай, мой краснокожий друг! Я тут ищу среди ловцов жемчуга одного человека. Не поможешь мне?";
				link.l3.go = "IndPearlMan_Sharp_1";
			}
		break;
		//жемчужный генератор Шарпа, поиски осведомителя
		case "IndPearlMan_Sharp_1":
			dialog.text = "Что ищет бледнолицый? Говори...";
			link.l1 = "Да, понимаешь, есть подозрение, что кто-то из ваших кое-кому сообщает о том, где в Карибском море вы промысел вести будете.";
			link.l1.go = "IndPearlMan_Sharp_2";
		break;
		case "IndPearlMan_Sharp_2":
			dialog.text = "Я знаю что хочет бледнолицый...";
			link.l1 = "Чего?";
			link.l1.go = "IndPearlMan_Sharp_3";
		break;
		case "IndPearlMan_Sharp_3":
			dialog.text = "Слушай меня, бледнолицый брат, я много говорить не буду. Есть здесь человек, звать Эмилио Гарсиа. Его видели в пещере одной бухты.";
			link.l1 = "Какой бухты?";
			link.l1.go = "IndPearlMan_Sharp_4";
		break;
		case "IndPearlMan_Sharp_4":
			dialog.text = "Вы называете ее залив Сан-Хуан дель Норте. Эмилио часто приходит туда ночью, там он говорит с белым, имя которого никто не знает, а взгляд как у гремучей змеи.";
			link.l1 = "Почему имя никто не знает?";
			link.l1.go = "IndPearlMan_Sharp_5";
		break;
		case "IndPearlMan_Sharp_5":
			dialog.text = "Потому что он его не называл.";
			link.l1 = "А-а-а, понятно. Ну что же, спасибо тебе, мой краснокожий брат.";
			link.l1.go = "IndPearlMan_Sharp_6";
		break;
		case "IndPearlMan_Sharp_6":
			dialog.text = "Пусть тропа твоего пути будет прямой.";
			link.l1 = "И за это спасибо. Прощай...";
			link.l1.go = "exit";
			pchar.questTemp.Sharp.SeekSpy = "toLaVega";
            AddQuestRecord("SharpPearl_SeekSpy", "2");
			AddQuestUserData("SharpPearl_SeekSpy", "sName", npchar.name);
			pchar.quest.SharpSeekSpy_loginSpy.win_condition.l1 = "location";
			pchar.quest.SharpSeekSpy_loginSpy.win_condition.l1.location = "Shore55";
			pchar.quest.SharpSeekSpy_loginSpy.function = "SharpSeekSpy_loginSpy";	
		break;
		//продажа жемчуга
		case "IndPearlMan_2":
			if (!CheckAttribute(npchar, "quest.pearl")) npchar.quest.pearl = rand(1);
			if (sti(npchar.quest.pearl) == 0 || CheckAttribute(pchar, "quest.PearlToShore_ForSale.id"))
			{
				dialog.text = "Почему я должен развязать свой язык перед бледнолицым? Нет.";
				link.l1 = "Ничего не скажешь? Ну, как знаешь...";
				link.l1.go = "exit";
			}
			else
			{
				bOk = false;
				int num = FindNearCharacters(npchar, 5.0, -1.0, -1.0, 1.0, false, true);
				if(num > 0)
				{
					for(int i = 0; i < num; i++)
					{
						iTemp = chrFindNearCharacters[i].index;
						if (iTemp == GetMainCharacterIndex()) continue;
						sTemp = characters[iTemp].id;
						if (findsubstr(sTemp, "FightMan" , 0) != -1)
						{
							bOk = true;
							break;
						}
					}
				}				
				if (bOk)
				{
					dialog.text = "У меня есть слезы богов. А бледнолицый лает как собака, все слышат, все хотят дать пинка. Я ухожу...";
					link.l1 = "Чего?..";
					link.l1.go = "IndPearlMan_4";
				}
				else
				{
					dialog.text = "У меня есть слезы богов. Приходи к большой воде рядом, я буду там до первой луны.";
					link.l1 = "Понятно. Ближайшая бухта, до полуночи. Приду обязательно.";
					link.l1.go = "IndPearlMan_3";
				}
			}
		break;
		case "IndPearlMan_3":
			npchar.LifeDay = 1;
			SaveCurrentNpcQuestDateParam(npchar, "LifeTimeCreate");
            NextDiag.CurrentNode = "IndPearlMan_Sale";
			LAi_SetActorTypeNoGroup(NPChar);
			if (npchar.location == "Pearl_town_2")
			{
				LAi_ActorRunToLocation(NPChar, "reload", "reload1", "Shore51", "goto", "goto1", "PearlToShore_ForSale", 10.0);
			}
			else
			{
				LAi_ActorRunToLocation(NPChar, "reload", "reload1", "Shore49", "goto", "goto1", "PearlToShore_ForSale", 10.0);
			}
			pchar.quest.PearlToShore_ForSale.idx = npchar.index;
			npchar.quest.Pearl.bigQty = rand(30) + 10;
			npchar.quest.Pearl.smallQty = rand(60) + 20;
			chrDisableReloadToLocation = true;
			DialogExit();
		break;
		case "IndPearlMan_4":
			npchar.LifeDay = 0;
			LAi_group_Attack(NPChar, Pchar);
			SaveCurrentNpcQuestDateParam(npchar, "LifeTimeCreate");
			LAi_SetActorType(NPChar);
            LAi_ActorRunToLocation(NPChar, "reload", "reload1", "none", "", "", "", 10.0);
			DialogExit();
		break;

		case "IndPearlMan_Sale":
			dialog.text = "Ты пришел...";
			link.l1 = "Ну да, а цена какая?";
			link.l1.go = "IndPearlMan_Sale_1";
		break;
		case "IndPearlMan_Sale_1":
			dialog.text = "Десять - за большую слезу, пять - за маленькую.";
			link.l1 = "Беру, сколько у тебя есть?";
			link.l1.go = "IndPearlMan_Sale_2";
			link.l2 = "Нет, не буду брать, передумал.";
			link.l2.go = "IndPearlMan_Sale_Out";
		break;
		case "IndPearlMan_Sale_2":
			dialog.text = npchar.quest.Pearl.bigQty + " большая, " + npchar.quest.Pearl.smallQty + " маленькая.";
			link.l1 = "Хорошо, беру все!";
			link.l1.go = "IndPearlMan_Sale_3";
		break;
		case "IndPearlMan_Sale_3":
			int iTemp = (sti(npchar.quest.Pearl.bigQty)*10) + (sti(npchar.quest.Pearl.smallQty)*5);
			if (sti(pchar.money) >= iTemp)
			{
				dialog.text = "Духи улыбнулись мне, я доволен...";
				link.l1 = "Бывай, приятель...";
				AddMoneyToCharacter(pchar, -iTemp);
				TakeNItems(pchar, "jewelry11", sti(npchar.quest.Pearl.bigQty));
				TakeNItems(pchar, "jewelry12", sti(npchar.quest.Pearl.smallQty)); 
			}
			else
			{
				dialog.text = "Ты должен мне " + iTemp + " денег. У тебя нет денег, у меня нет слез богов. Прощай.";
				link.l1 = "Эх, жаль...";
			}
			link.l1.go = "IndPearlMan_Sale_Out";	
		break;
		case "IndPearlMan_Sale_Out":
			chrDisableReloadToLocation = true;
			LAi_LocationFightDisable(&Locations[FindLocation(pchar.location)], true);
			DialogExit();
			LAi_SetActorType(NPChar);
            LAi_ActorRunToLocation(NPChar, "reload", "reload1", "none", "", "", "CanFightCurLocation", -1);
		break;
	}
}
