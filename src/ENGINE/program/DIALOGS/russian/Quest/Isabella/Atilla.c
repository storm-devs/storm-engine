
void ProcessDialogEvent()
{
	ref NPChar, sld;
	aref Link, NextDiag;

	DeleteAttribute(&Dialog,"Links");

	makeref(NPChar,CharacterRef);
	makearef(Link, Dialog.Links);
	makearef(NextDiag, NPChar.Dialog);
	
	switch(Dialog.CurrentNode)
	{
		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	
		case "First time":
			dialog.text = "Чтоб мне лопнуть! Старина " + GetFullName(PChar) + "!";
			link.l1 = "Атилла! Здорово, старый морской черт!";
			link.l1.go = "port_1";		
			NextDiag.TempNode = "First time";
			DeleteAttribute(npchar, "talker"); //снимаем говорилку
		break;

		case "port_1":
		    sld = characterFromID("Atilla");
			dialog.text = "Ты гляди, разоделся, что твой дон!  Небось, и в карманах ветер не свищет! Не предложишь выпивку старому знакомому?";
			if (pchar.RomanticQuest == "")
			{
			    link.l1 = "Я чертовски рад тебя видеть, но сейчас не могу, дружище. В другой раз...";
				link.l1.go = "Exit";
			}
			else
			{
				link.l1 = "Ха-ха-ха! За такую встречу грех не опрокинуть по кружке доброго ямайского рома!";
				link.l1.go = "port_1_1";
			}
		break;

		case "port_1_1":
			dialog.text = "Лопни моя селезнка, за такую встречу выпить надо! Немедленно в таверну...";
			link.l1 = "Согласен - в таверну!";
			link.l1.go = "port_toTavern";
		break;

		case "port_toTavern":
			DialogExit();
			NextDiag.CurrentNode = "Tavern_1"; 
			chrDisableReloadToLocation = true; 
			bDisableFastReload = true; 
			FreeSitLocator("Pirates_tavern", "sit3");
			LAi_SetActorType(npchar);
			LAi_ActorRunToLocation(npchar, "reload", "reload4_back", "Pirates_tavern", "sit", "sit3", "Romantic_Pirat_toTavern", -1);
		break;

		case "Tavern_1":
			LAi_SetSitType(Pchar);
			dialog.text = "Черт возьми, "+GetFullName(PChar)+"! Славное дельце мы с тобой провернули тогда в Каракасе, разрази меня гром!";
			link.l1 = "Да уж... Многих каталонских дьяволов ты отправил к праотцам в том бою. За ту лихую рубку тебе и дали это прозвище - Атилла... С тех пор тебе, я вижу, не часто улыбалась удача.";
			link.l1.go = "Tavern_2";
		break;
		
		case "Tavern_2":
			dialog.text = "Аррргх! Вот что я тебе скажу - никогда не верь этим каталонским донам! Ходил я с одним. Взяли как-то раз французскую посудину. Среди пассажиров ювелир оказался.  До бумажек его - векселей всяких - мне дела нет, а в побрякушках-то я знаю толк. Матерь Божья, стоили они столько, что можно было купить половину Эспаньолы и губернаторский дом с самим губернатором впридачу. Ну, поделили все честь по чести, нашли в трюме пять бочонков рома, напились на радостях...";
			link.l1 = "Хех, ну как всегда...";
			link.l1.go = "Tavern_3";
		break;
		
		case "Tavern_3":
			dialog.text = "Так-то оно так, но вот наутро оказалось, что приятель мой не лыком шит. Отвязал ночью ялик, прихватил все цацки и был таков.";
			link.l1 = "И ты спустил ему это с рук? Чтоб меня акула проглотила! Да я бы...";
			link.l1.go = "Tavern_4";
		break;
		
		case "Tavern_4":
			dialog.text = "Что ты бы, салага?! Бежал-то он на испанский остров. А мне туда пути заказаны. Ну, да Бог с ним. Мог ведь просто глотку мне, дурню пьяному, перерезать - и никто бы слова не сказал.";
			link.l1 = "Хм, что-то ты стал больно меланхоличен, дружище.";
			link.l1.go = "Tavern_5";
		break;
		
		case "Tavern_5":
			dialog.text = "Время идет, да и я старею. Не так просто мне уже отправить человека на тот свет...  А Сальватор, говорят, на мои денежки свадебку сыграл. Живет теперь в Сан-Хуане...";
			link.l1 = "Черт меня побери!  Атилла, дружище, а не навестить ли мне это тихое семейное гнездышко? Глядишь, в жениной шкатулке твои побрякушки отыщутся, а?";
			link.l1.go = "Tavern_6";
		break;
		
		case "Tavern_6":
			dialog.text = "Ха-ха-ха! Сдается мне, ты не ради побрякушек в ее спальню пробраться надумал!  Да нет, приятель, раз уплыла добыча из моих рук - видно, так тому и быть... Ну, пора мне. Даст Бог - еще свидимся.";
			link.l1 = "Прощай, Атилла.";
			link.l1.go = "Tavern_Out";
		break;
		
		case "Tavern_Out":
			NextDiag.CurrentNode = "Tavern_Bye";
			DialogExit();
			AddDialogExitQuest("Romantic_Pirat_toTavern_end");			
		break;

		case "Tavern_Bye":
            NextDiag.TempNode = "Tavern_Bye";
			dialog.text = "Сейчас допью и пойду на корабль...";
			link.l1 = "Не буду мешать.";
			link.l1.go = "exit";
		break;
		//доводим новости до сведения Атиллы
		case "TalkingNews":
			dialog.text = "Ох, кого я вижу! " + GetFullName(pchar) + " собственной персоной!";
			link.l1 = "Здорово, Атилла! Дела у тебя, похоже, налаживаются потихоньку - дом купил...";
			link.l1.go = "TalkingNews_1";
		break;
		case "TalkingNews_1":
			dialog.text = "Появились деньжата, вот и решил вложить в недвижимость. Хотя, по чести сказать, дом здесь - это далеко не дом на Тортуге...";
			link.l1 = "Ну, тоже крыша.";
			link.l1.go = "TalkingNews_2";
		break;
		case "TalkingNews_2":
			dialog.text = "Хм, верно! Ну, говори, зачем пожаловал ко мне?";
			link.l1 = "Да ты знаешь, тут такое дело, что в двух словах не расскажешь... В общем, навестил я твоего приятеля Сальватора Олевареса.";
			link.l1.go = "TalkingNews_3";
		break;
		case "TalkingNews_3":
			dialog.text = "Хех, вот это новость! Как поживает этот проходимец? Правду говорили - женился?";
			link.l1 = "Да, правду. Жена его, Изабелла - очень красивая женщина... И порядочная, к тому же.";
			link.l1.go = "TalkingNews_4";
		break;
		case "TalkingNews_4":
			dialog.text = "Хех, это с чего ты так решил? Женщины - народ коварный, за внешней красотой может скрываться большое зло.";
			link.l1 = "Зло - это Сальватор! Я тебе не рассказал о случившемся, так что послушай.";
			link.l1.go = "TalkingNews_5";
		break;
		case "TalkingNews_5":
			dialog.text = "С удовольствием...";
			link.l1 = "В общем, прибыл я на Сан-Хуан после разговора с тобой и начал выяснять, что к чему. Много чего там случилось, но в итоге оказалось, что Сальватор обокрал свою жену и у меня на глазах убил ее брата.";
			link.l1.go = "TalkingNews_6";
		break;
		case "TalkingNews_6":
			dialog.text = "Да уж, разошелся, мерзавец...";
			link.l1 = "Он меня оклеветал и меня пытались арестовать... Когда мне удалось ситуацию урегулировать как-то - Сальватор как в воду канул. А Изабелла в доме закрылась, никого не пускает...";
			link.l1.go = "TalkingNews_7";
		break;
		case "TalkingNews_7":
			dialog.text = "Ну что же, друг, спасибо тебе за попытку восстановить справедливость. Что думаешь дальше делать?";
			link.l1 = "Думаю найти Сальватора и выпустить ему кишки. Еще с Изабеллой нужно объясниться... Она думает, что это я убил ее брата.";
			link.l1.go = "TalkingNews_8";
		break;
		case "TalkingNews_8":
			dialog.text = "У-у-у, приятель, а тебе не все равно?";
			link.l1 = "Нет, Атилла, уже нет...";
			link.l1.go = "TalkingNews_9";
		break;
		case "TalkingNews_9":
			dialog.text = "L'amour... Qu'ici faire?..";
			link.l1 = "Чего?..";
			link.l1.go = "TalkingNews_10";
		break;
		case "TalkingNews_10":
			dialog.text = "Это французский... Ничего, все в порядке..\n"+
				          "И где искать-то думаешь?";
			link.l1 = "Не знаю, ума просто не приложу.";
			link.l1.go = "TalkingNews_11";
		break;
		case "TalkingNews_11":
			dialog.text = "Послушай, ты выкурил Сальватора из Сан-Хуана, и это очень хорошо. Теперь я могу подключиться к поискам.";
			link.l1 = "И ты сможешь найти его?";
			link.l1.go = "TalkingNews_12";
		break;
		case "TalkingNews_12":
			dialog.text = "Думаю, да. Не ты один с уважением относишься к Атилле, есть и еще у этого пирата друзья...";
			link.l1 = "Ну что же, было бы здорово. А мне что делать?";
			link.l1.go = "TalkingNews_13";
		break;
		case "TalkingNews_13":
			dialog.text = "Занимайся своими делами пока, ну а я прямо сейчас займусь поисками. Помочь ты мне ничем не сможешь, только помешаешь. Через два месяца встретимся здесь, у меня в доме. Тогда и расскажу тебе все, что сумел узнать.";
			link.l1 = "Хорошо, Атилла. Ну что же, удачи тебе.";
			link.l1.go = "TalkingNews_14";
		break;
		case "TalkingNews_14":
			SetTimerCondition("Romantic_AtillaToHouse", 0, 2, 0, false);
			AddQuestRecord("Romantic_Line", "11");
            LAi_SetActorType(npchar);
			ChangeCharacterAddressGroup(npchar, pchar.location, "goto", "goto2");
            LAi_ActorRunToLocation(npchar, "reload", "reload1", "none", "", "", "", 3.0);
			//открываем дом Сальватора и убираем Изабеллу, ставим наемных убийц в дом.
			LocatorReloadEnterDisable("SanJuan_town", "houseSp6", false);
			LocatorReloadEnterDisable("SanJuan_houseSp6", "reload2", false);
			ChangeCharacterAddress(characterFromId("Isabella"), "none", "");
            Pchar.quest.Romantic_AmbushInHouse.win_condition.l1 = "location";
            Pchar.quest.Romantic_AmbushInHouse.win_condition.l1.location = "SanJuan_houseSp6";
            Pchar.quest.Romantic_AmbushInHouse.win_condition = "Romantic_AmbushInHouse";
			DialogExit();
		break;
		//через два месяца по завершению поисков
		case "AtillaInHouse":
			dialog.text = "А, вот и ты, " + GetFullName(pchar) + ". Проходи, рад тебя видеть.";
			link.l1 = "Атилла, ну как у тебя дела? Узнал что-нибудь новое о нашем большом друге?";
			link.l1.go = "AtillaInHouse_1";
		break;
		case "AtillaInHouse_1":
			dialog.text = "Узнал, " + pchar.name + ", как же иначе?.. В общем, Сальватора ты здорово растормошил. Тот не сидит теперь на одном месте, постоянно перемещается.";
			link.l1 = "И как же его теперь отловить?";
			link.l1.go = "AtillaInHouse_2";
		break;
		case "AtillaInHouse_2":
			dialog.text = "Подумать надо...";
			link.l1 = "Послушай, а про Изабеллу что-нибудь известно?";
			link.l1.go = "AtillaInHouse_3";
		break;
		case "AtillaInHouse_3":
			dialog.text = "Известно, приятель. Похоже, что Сальватор выманил ее из Сан-Хуана. Он прислал за ней пиратский бриг 'Восторженный' и, по слухам, кэп взял курс на Порто Белло.";
			link.l1 = "И зачем она ему? Он уже разорил ее...";
			link.l1.go = "AtillaInHouse_4";
		break;
		case "AtillaInHouse_4":
			dialog.text = "Полагаю, чтобы спрятать концы в воду, приятель. Не жить твоей Изабелле, если она попадет в руки Сальватора.";
			link.l1 = "Аргх, когда бриг вышел из Сан-Хуана? Я должен успеть перехватить его!";
			link.l1.go = "AtillaInHouse_5";
		break;
		case "AtillaInHouse_5":
			dialog.text = "Если хочешь увидеть Изабеллу живой - то да. Бриг вышел из Сан-Хуана три дня тому назад. По моим расчетам, он будет у Порто Белло дней через пятнадцать-шестнадцать, не более.";
			link.l1 = "Значит, мне надо торопится... Спасибо тебе, Атилла!";
			link.l1.go = "AtillaInHouse_6";
		break;
		case "AtillaInHouse_6":
			dialog.text = "Приятель, ты не забудь ма-а-аленкую деталь - где-то в Порто Белло должен находиться и сам Сальватор. Он ждет там Изабеллу.";
			link.l1 = "Хех, а ведь верно! Атилла, я должен спешить. Если все сложится нормально, то я верну тебе потерянные драгоценности.";
			link.l1.go = "AtillaInHouse_7";
		break;
		case "AtillaInHouse_7":
			dialog.text = "Хорошо, " + pchar.name + ". Удачи тебе!";
			link.l1 = "Спасибо, Атилла.";
			link.l1.go = "exit";
			NextDiag.TempNode = "AtillaInHouse_Again";
			AddQuestRecord("Romantic_Line", "13");
			pchar.RomanticQuest = "AttackBrig"; //флаг для кэпа на абордаже брига
            Pchar.quest.Romantic_AttackBrig.win_condition.l1 = "location";
            Pchar.quest.Romantic_AttackBrig.win_condition.l1.location = "PortoBello";
            Pchar.quest.Romantic_AttackBrig.win_condition = "Romantic_AttackBrig";
			SetTimerCondition("Romantic_BrigTimeOver", 0, 0, 16, false);
		break;
		case "AtillaInHouse_Again":
			dialog.text = "Ну, как успехи?";
			link.l1 = "Пока никак...";
			link.l1.go = "exit";
		break;
		// после погони за бригом Восторженный, если не догнал его вообще
		case "Brig_Late":
			dialog.text = "Ну, рассказывай, как у тебя дела?";
			link.l1 = "Дела неважно... Не сумел я догнать этот бриг, даже кормы не видел...";
			link.l1.go = "Brig_Late_1";
		break;
		case "Brig_Late_1":
			dialog.text = "Хм, плохо... Похоже, что Изабеллы уже нет в живых...";
			link.l1 = "Наверное, ты прав. Как подумаю об этом - сердце разрывается на части...";
			link.l1.go = "Brig_Late_2";
		break;
		case "Brig_Late_2":
			dialog.text = "Слезами горю не помочь, чего уж теперь-то убиваться? Это приключение для тебя закончено, надо жить дальше.";
			link.l1 = "Ты прав, Атилла. Спасибо тебе за поддержку. Прощай...";
			link.l1.go = "exit";
			NextDiag.TempNode = "Last_Talk";
			CloseQuestHeader("Romantic_Line");
			DeleteAttribute(pchar, "RomanticQuest");
		break;
		case "Last_Talk":
			dialog.text = "Скоро и я должен буду выйти в море - надо на что-то жить...";
			link.l1 = "Удачи тебе, Атилла.";
			link.l1.go = "exit";
			NextDiag.TempNode = "Last_Talk";
		break;
		// если догнал бриг свалил или ГГ ушел на карту
		case "Brig_DieHard":
			dialog.text = "Ну, что, приятель, какие новости?";
			link.l1 = "Новости плохие. У Порто Белло нагнал я бриг, но взять его на абордаж не сумел...";
			link.l1.go = "Brig_DieHard_1";
		break;
		case "Brig_DieHard_1":
			dialog.text = "М-да, это очень плохо. Изабеллы, поди, уже и нет в живых...";
			link.l1 = "Каррамба! По всей видимости, так оно и есть... Не могу об этом думать без слез...";
			link.l1.go = "Brig_Late_2";
		break;
		// если догнал бриг, но утопил его
		case "Brig_Sunk":
			dialog.text = "Ну, рассказывай, как у тебя дела?";
			link.l1 = "Дела неважно... У Порто Белло нагнал я бриг, но взять его на абордаж не сумел - тот пошел ко дну во время боя. Никто не выжил...";
			link.l1.go = "Brig_Sunk_1";
		break;
		case "Brig_Sunk_1":
			dialog.text = "Да уж, приятель, плохо дело... Не позавидуешь тебе - потерял женщину, которая тебе нравилась...";
			link.l1 = "Не могу об этом говорить - сердце разрывается на части...";
			link.l1.go = "Brig_Late_2";
		break;
		// если Изабелла утонула с бригом, нет векселей
		case "Isabella_Sink":
			dialog.text = "Ну, рассказывай, как у тебя дела?";
			link.l1 = "Дела неважно... У Порто Белло нагнал я бриг, и взял его на абордаж.";
			link.l1.go = "Isabella_Sink_1";
		break;
		case "Isabella_Sink_1":
			dialog.text = "Что, твоей Изабеллы там не было?";
			link.l1 = "Была, вот только не поверила она мне. И забрать ее с тонущего корабля не смог, она предпочла пойти ко дну вместе с бригом. Вот так, Атилла...";
			link.l1.go = "Brig_Late_2";
		break;
		// узнать что-нибудь новое о Сальватора
		case "SalvatorNews":
			dialog.text = "Кого я вижу? " + GetFullName(pchar) + " собственной персоной!";
			link.l1 = "Здравствуй, Атилла! Как дела, приятель?";
			link.l1.go = "SalvatorNews_1";
		break;
		case "SalvatorNews_1":
			dialog.text = "У меня все в порядке. Как у тебя дела двигаются?";
			link.l1 = "С переменным успехом. Не могу найти Сальватора, только на подручных его натыкаюсь.";
			link.l1.go = "SalvatorNews_2";
		break;
		case "SalvatorNews_2":
			dialog.text = "Понятно...";
			link.l1 = "Слушай, ты сам ничего о нем не слышал?";
			link.l1.go = "SalvatorNews_3";
		break;
		case "SalvatorNews_3":
			dialog.text = "Конкретного мало. Нанимает всякий сброд по притонам, платит им за работу, ну а сам исчезает. Трудно его отловить.";
			link.l1 = "Хм, да уж... Ума не приложу, что делать?";
			link.l1.go = "SalvatorNews_4";
		break;
		case "SalvatorNews_4":
			dialog.text = "Идти к Белизу и искать его там.";
			link.l1 = "А почему к Белизу?";
			link.l1.go = "SalvatorNews_5";
		break;
		case "SalvatorNews_5":
			dialog.text = "Потому, что Сальватор туда направился. Это последнее, что я о нем слышал буквально пару дней назад.";
			link.l1 = "Дьявол! Я же отвез в Белиз Изабеллу!!!";
			link.l1.go = "SalvatorNews_6";
		break;
		case "SalvatorNews_6":
			dialog.text = "А-а-а, ну теперь понятно, что ему там понадобилось. Я бы на твоем месте времени даром не терял...";
			link.l1 = "А я и не буду... Прощай, Атилла, спасибо за помощь!";
			link.l1.go = "exit";
			NextDiag.TempNode = "AtillaInHouse_Again";
			AddQuestRecord("Romantic_Line", "20");
			ChangeCharacterAddress(characterFromId("Isabella"), "nonr", "");
			QuestSetCurrentNode("Rosita", "BackRosita");
			//Изабеллу в шахту
			sld = characterFromId("Isabella");
			LAi_SetStayType(sld);
			ChangeCharacterAddressGroup(sld, "Beliz_Cave_2", "goto", "goto5");
			sld.dialog.currentnode = "IsabellaInCave";
		break;
		//Изабелла убита в церкви.
		case "IsabellaIsDead":
			dialog.text = "О-о-о, капитан " + GetFullName(pchar) + ". Ну как дела, приятель?";
			link.l1 = "Атилла, я потерял Изабеллу...";
			link.l1.go = "IsabellaIsDead_1";
		break;
		case "IsabellaIsDead_1":
			dialog.text = "Что случилось?";
			link.l1 = "Я убил Сальватора и мы должны были обвенчаться с Изабеллой в Сан-Хуане. На церемонии в церковь воралась толпа головорезов, и они убили ее... Их нанял Сальватор, когда был еще жив.";
			link.l1.go = "IsabellaIsDead_2";
		break;
		case "IsabellaIsDead_2":
			dialog.text = "М-да, соболезную. Но что поделать? Надо как-то жить дальше...";
			link.l1 = "Да, Атилла, верно...";
			link.l1.go = "exit";
			npchar.LifeDay = 0;
			NextDiag.TempNode = "IsabellaIsDead_after";
			CloseQuestHeader("Romantic_Line");
		break;
		case "IsabellaIsDead_after":
			dialog.text = "Эх, Атилла, как мне тяжко...";
			link.l1 = "Понимаю, приятель...";
			link.l1.go = "exit";
			NextDiag.TempNode = "IsabellaIsDead_after";
			npchar.lifeDay = 0;
		break;
		//если ГГ отбился в церкви
		case "IsabellaIsMyWife":
			dialog.text = "Здорово, приятель! Как жизнь?";
			link.l1 = "Жизнь прекрасна, Атилла! Все великолепно! Мы обвенчались с Изабеллой, теперь я женат.";
			link.l1.go = "IsabellaIsMyWife_1";
			npchar.lifeDay = 0;
		break;
		case "IsabellaIsMyWife_1":
			dialog.text = "Могу поздравить, это дело хорошее. А то ведь жизнь пирата не очень длинна, надо вовремя остановиться...";
			link.l1 = "Да, это верно. Изабелла вышла замуж при условии, что я завяжу с каперством.";
			link.l1.go = "IsabellaIsMyWife_2";
		break;
		case "IsabellaIsMyWife_2":
			dialog.text = "Ну, может оно и верно, приятель...";
			link.l1 = "Наверное, да, Атилла. Ну, прощай...";
			link.l1.go = "exit";
			link.l2 = "Атилла, тут такое дело... Я хочу отблагодарить тебя за все.";
			link.l2.go = "IsabellaIsMyWife_3";
			NextDiag.TempNode = "IsabellaIsMyWife_after";
			pchar.RomanticQuest.Atilla = "YouAreBaster";
			npchar.LifeDay = 0;
		break;
		case "IsabellaIsMyWife_after":
			dialog.text = "Опять кэп " + pchar.name + " пожаловал. Что тебе нужно от Атиллы на этот раз?";
			link.l1 = "Ничего особенного, просто так заглянул, приятель...";
			link.l1.go = "exit";
			NextDiag.TempNode = "IsabellaIsMyWife_after";
		break;
		case "IsabellaIsMyWife_3":
			dialog.text = "За что именно?";
			link.l1 = "Ну как, за твою помощь, сколько раз ты меня выручал! Да и вообще, я с Изабеллой познакомился по твой наводке, так сказать...";
			link.l1.go = "IsabellaIsMyWife_4";
		break;
		case "IsabellaIsMyWife_4":
			dialog.text = "Хех, отказываться не буду, приятель...";
			if (sti(pchar.money) >= 1000)
			{
				link.l1 = "Ну, тогда прими тысячу монет, Атилла!";
				link.l1.go = "AddRepa_1";
			}
			if (sti(pchar.money) >= 70000)
			{
				link.l2 = "Я приготовил для тебя 70 тысяч монет.";
				link.l2.go = "AddRepa_2";
			}
			if (sti(pchar.money) >= 150000)
			{
				link.l3 = "Твоя доля в этом деле составляет 150 тысяч монет.";
				link.l3.go = "AddRepa_3";
			}
			if (sti(pchar.money) >= 300000)
			{
				link.l4 = "Твоя доля в этом деле составляет 300 тысяч монет.";
				link.l4.go = "AddRepa_4";
			}
		break;
		case "AddRepa_1":
			dialog.text = "Удивил, " + pchar.name + "... Ну что же, спасибо и на этом. Дареному коню, как извесно, в зубы не глядят...";
			link.l1 = "Да не за что, приятель! Будь здоров...";
			link.l1.go = "exit";
			AddMoneyToCharacter(pchar, -1000);
			ChangeCharacterReputation(pchar, -10);
		break;	
		case "AddRepa_2":
			dialog.text = "Ну, здорово! Я благодарен тебе, " + GetFullName(pchar) + ".";
			link.l1 = "Все нормально, Атилла. Будь здоров...";
			link.l1.go = "exit";
			AddMoneyToCharacter(pchar, -70000);
			pchar.RomanticQuest.Atilla = "YouAreNormal";
			ChangeCharacterReputation(pchar, 5);
		break;	
		case "AddRepa_3":
			dialog.text = "Ох ты, здорово! Хоть я особо и не напрягался...";
			link.l1 = "Атилла, ты, может, и не особо напрягался, зато все твои усилия были очень даже по делу! Будь здоров...";
			link.l1.go = "exit";
			AddMoneyToCharacter(pchar, -150000);
			pchar.RomanticQuest.Atilla = "YouAreNormal";
			ChangeCharacterReputation(pchar, 15);
		break;
		case "AddRepa_4":
			dialog.text = "Эх, черт меня дери!! Это ж какие деньжищи!\n"+
				          "Ну, спасибо, приятель, удружил - так удружил. Куплю теперь себе конуру поприличней... Да нет, наверное, в Старый Свет вернусь, засиделся здесь...";
			link.l1 = "Делай, как знаешь, приятель! Будь здоров...";
			link.l1.go = "exit";
			AddMoneyToCharacter(pchar, -300000);
			pchar.RomanticQuest.Atilla = "YouAreGood";
			ChangeCharacterReputation(pchar, 30);
		break;	

	}
}
