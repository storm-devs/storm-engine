// boal 27.03.2004 Изабела Армани (романтическая ветка)
void ProcessDialogEvent()
{
	ref NPChar, d, sld, rColony;
	aref Link, NextDiag;
	string NPC_Meeting;
	int i, iMoney, iTemp;

	DeleteAttribute(&Dialog,"Links");

	makeref(NPChar,CharacterRef);
	makearef(Link, Dialog.Links);
	makeref(d, Dialog);
	makearef(NextDiag, NPChar.Dialog);

	string iDay, iMonth, sTemp;
	iDay = environment.date.day;
	iMonth = environment.date.month;
	string lastspeak_date = iday + " " + iMonth;
// ============================================================================
// ============================= блок angry ==========>>>>>>>>>>>>>>>>>>>>>>>>>
    if (CheckAttribute(npchar, "angry") && !CheckAttribute(npchar, "angry.ok"))
    {
        npchar.angry.ok = 1;
        if (!CheckAttribute(npchar, "angry.first")) //ловушка первого срабатывания
        {
            NextDiag.TempNode = NextDiag.CurrentNode;
            Dialog.CurrentNode = "AngryExitAgain";
            npchar.angry.first = 1;
        }
        else
        {
            switch (npchar.angry.kind) //сюда расписываем реакцию ангри. В npchar.angry.name пробелы удалены!!!
            {
                case "repeat":
					//когда достал обещаниями денег дать
                    if (npchar.angry.name == "NewLife_Budget_Add") Dialog.CurrentNode = "AngryRepeat_Budget_Add";
					//когда подарками достал, а сам не давал
					if (npchar.angry.name == "NewLife_Present") Dialog.CurrentNode = "AngryRepeat_Present";
                break;
				case "quest":
					//когда долги достали Изабеллу
					if (npchar.angry.name == "NewLife_Budget_Pay") Dialog.CurrentNode = "AngryRepeat_Budget_Pay";
					//когда достало Изабеллу пиратсво ГГ
					if (npchar.angry.name == "NewLife_KnowPirates") Dialog.CurrentNode = "AngryRepeat_KnowPirates";
				break;
            }
        }
    }
// <<<<<<<<<<<<<<<<<<<<<<======= блок angry ===================================
// ============================================================================	
	switch(Dialog.CurrentNode)
	{
		case "exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			NPChar.quest.meeting = NPC_Meeting;
			DialogExit();
		break;

		case "First time":
			NextDiag.TempNode = "first time";
			//был в спальне
			pchar.quest.wasInBeedroom = true;
			bDisableFastReload = true;
			// прерывание на выход, а то ход истории в руках геймера
			pchar.quest.RP_afterVisitBedroom.win_condition.l1 = "ExitFromLocation";
			pchar.quest.RP_afterVisitBedroom.win_condition.l1.location = pchar.location;
			pchar.quest.RP_afterVisitBedroom.win_condition = "RP_afterVisitBedroom";
            dialog.text = "Как вы сюда попали? Если вы немедленно не покините мою спальню, я позову мужа.";
			link.l1 = "Я " + GetFullName(pchar) + ". Могу я узнать имя столь прекрасной сеньориты?";
			link.l1.go = "Next_1";
			link.l2 = "Прошу меня простить. Я уже ухожу...";
			link.l2.go = "exit";
		break;
		
		case "Next_1":
			NextDiag.TempNode = "Next_2";
			
            dialog.text = "Сеньоры, а не сеньориты. Я - хозяйка этого дома, "+ NPChar.Name + " " + NPChar.LastName +
                          ". Зачем вы забрались в мои покои?";
            link.l1 = "Успокойтесь, сеньора, я - компаньон вашего мужа. Он предложил мне осмотреть ваш новый дом, а здесь так много комнат, что заблудиться совсем нетрудно.";
			link.l1.go = "Next_4";
			link.l2 = "Простое любопытство. Я уже ухожу.";
			link.l2.go = "exit";
		break;
		
		case "Next_2":
			NextDiag.TempNode = "Next_2";			
            dialog.text = "Вы еще здесь? Оставьте меня в покое!";
            link.l1 = "Могу я узнать кое-что?";
			link.l1.go = "Next_3";
            link.l2 = "Извините...";
			link.l2.go = "exit";
		break;
		
		case "Next_3":
			NextDiag.TempNode = "Next_2";			
            dialog.text = "Что вам от меня нужно? Поговорите с моим мужем. Я не хочу разговаривать с вами сейчас.";
            link.l1 = "Хорошо, я так и поступлю.";
			link.l1.go = "exit";
		break;
		
		case "Next_4":
            dialog.text = "Да, мы недавно купили этот дом. А какие общие дела у вас с моим мужем?";
            link.l1 = "Наши интересы пересекаются в коммерческой сфере влияния испанской короны.";
			link.l1.go = "Next_5";
            link.l2 = "Мы с ним давние друзья.";
			link.l2.go = "Next_6";
			link.l3 = "Должен откланяться. Извините.";
			link.l3.go = "Next_7";
		break;
		
		case "Next_5":
            dialog.text = "Интересы... коммерческой... пересекаются... - я от этого слишком далека. Ну, да ладно. Как вам нравится наш дом? Декор подбирала я сама.";
            link.l1 = "Просто бесподобно! У вас такой утонченный вкус!";
			link.l1.go = "Next_8";
            link.l2 = "Честно говоря, не впечатляет. Нечто похожее я видел в английской колонии.";
			link.l2.go = "Next_9";
			Pchar.RomanticQuest.TalkInShop = true;
		break;
		
		case "Next_6":
            dialog.text = "Друзья? Не думаю, что у моего мужа могут быть такие друзья. "+ Characters[GetCharacterIndex("Husband")].Name + "!!!";
            link.l1 = "Подождите! Я все объясню...";
			link.l1.go = "Romantic_Battle_in_Bedroom";
		break;
		
		case "Next_7":
			dialog.text = "Вы мне не нравитесь. Думаю, будет лучше, если я позову мужа. "+ Characters[GetCharacterIndex("Husband")].Name + "!!!";
            link.l1 = "Подождите! Я все объясню...";
			link.l1.go = "Romantic_Battle_in_Bedroom";
		break;
		
		case "Next_8":
		    NextDiag.TempNode = "Good_1";
            dialog.text = "Я польщена. Вы умеете расположить к себе даму, сеньор.";
            link.l1 = "Я просто сказал правду. А сейчас разрешите откланяться, не буду вам мешать...";
			link.l1.go = "exit";
			ChangeCharacterReputation(pchar, 3);
		break;
		
		case "Next_9":
		    NextDiag.TempNode = "Next_2";
            dialog.text = "По-моему, вы просто неотесанный болван!";
            link.l1 = "Ну... некоторые думают иначе...";
			link.l1.go = "exit";
			ChangeCharacterReputation(pchar, -4);
		break;
		
		case "Good_1":
		    NextDiag.TempNode = "Good_1";
            dialog.text = "Рада новой встрече. Какими судьбами?";
            link.l1 = "Просто зашел поприветствовать вас, прекрасная "+Npchar.Name+".";
			link.l1.go = "exit";
			if (!CheckAttribute(pchar, "quest.VisitStep")) pchar.quest.VisitStep = 1;
			else pchar.quest.VisitStep = makeint(pchar.quest.VisitStep)+1;
			pchar.quest.wasInBeedroom = true;
			bDisableFastReload = true;
			pchar.quest.RP_afterVisitBedroom.win_condition.l1 = "ExitFromLocation";
			pchar.quest.RP_afterVisitBedroom.win_condition.l1.location = pchar.location;
			pchar.quest.RP_afterVisitBedroom.win_condition = "RP_afterVisitBedroom";
		break;
		
		case "Romantic_Battle_in_Bedroom":
		    NextDiag.CurrentNode = "Next_2";
			DialogExit();
			AddDialogExitQuest("Romantic_Battle_in_Bedroom_1");
		break;
		
		case "Murder":
		    NextDiag.TempNode = "Murder";
            dialog.text = "Оставьте меня, грязный убийца!";
            link.l1 = "...";
			link.l1.go = "exit";
			PChar.quest.Romantic_TalkInShop.over = "yes";
		break;
		
		case "TalkInShop":
		    NextDiag.TempNode = "TalkInShop_2";
            dialog.text = "Капитан "+GetFullName(PChar)+"!";
            link.l1 = "Сеньора! Какая приятная встреча!";
			link.l1.go = "exit";
			//AddDialogExitQuest("Romantic_TalkInShop_3");
		break;
		
		case "TalkInShop_2":
            dialog.text = "Ради бога, капитан, оставьте его!";
            link.l1 = "Не раньше, чем этот негодяй попросит прощения!";
			link.l1.go = "TalkInShop_3";
		break;
		
		case "TalkInShop_3":
            dialog.text = "Да ведь это просто небольшое недоразумение.";
            link.l1 = "Тогда расскажите мне, что стряслось?";
			link.l1.go = "TalkInShop_4";
		break;
		
		case "TalkInShop_4":
            dialog.text = "Я пришла узнать, почему сегодня нам не принесли продукты, как было условлено. Я была уверена, что мой супруг заплатил сеньору "+
                          GetCharacterFullName("SanJuan_trader")+". Но, видимо, он позабыл. Оказалось, что у нас долг за целый месяц. У меня не оказалось с собой 1200 золотых. Я сказала, что принесу их позже, а сеньор " +
                          GetCharacterFullName("SanJuan_trader")+" решил, что я и вовсе отказываюсь платить.";
            link.l1 = "(торговцу) Получи свои деньги, ничтожный торгаш.";
			link.l1.go = "TalkInShop_5";
			link.l2 = "(торговцу) Ну, теперь-то он вам поверит, не так ли?";
			link.l2.go = "TalkInShop_6";
		break;
		
		case "TalkInShop_5":
            AddMoneyToCharacter(pchar, -1200);
		    NextDiag.TempNode = "Good_1";
            dialog.text = "Мне, право, неловко, капитан. Я тотчас же верну вам деньги. А теперь мне пора.";
            link.l1 = "Пустое, сеньора Изабелла. Я сполна вознагражден тем, что имел счастье увидиться с вами.";
			link.l1.go = "TalkInShop_Go";
		break;
		
		case "TalkInShop_6":
            NextDiag.CurrentNode = "TalkInShop_7";
			DialogExit();
			characters[GetCharacterIndex("SanJuan_trader")].dialog.currentnode = "Romantic_2";
			AddDialogExitQuest("Romantic_TalkInShop_3");
		break;
		
		case "TalkInShop_7":
		    NextDiag.TempNode = "Good_1";
            dialog.text = "Благодарю вас. Но мне пора. До свидания.";
            link.l1 = "Пустое, сеньора Изабелла. Я сполна вознагражден тем, что имел счастье увидиться с вами.";
			link.l1.go = "TalkInShop_Go";
		break;
		
		case "TalkInShop_Go":
            NextDiag.CurrentNode = "Good_1";
			DialogExit();
            LAi_ActorGoToLocation(NPChar, "reload", "reload1_back", "SanJuan_church", "goto", "goto1", "Romantic_TalkInShop_end", 4.0);
			chrDisableReloadToLocation = true;
		break;
		
		case "TalkInChurch":
		    //NextDiag.TempNode = "TalkInChurch_1";
            dialog.text = "Сеньор капитан, я рада вас видеть. Скажите, не вы ли капитан брига 'Пять ран Христовых'?";
            link.l1 = "'Пять ран Христовых'? Такому грешнику, как я, никогда не пришло бы в голову назвать свою посудину столь благочестивым именем. Впрочем, ради прекрасной сеньоры я готов сменить не только имя корабля, но и свое собственное.";
			link.l1.go = "TalkInChurch_1";
			DeleteAttribute(npchar, "talker"); //снимаем говорилку
		break;
		
		case "TalkInChurch_1":
            dialog.text = "Простите, капитан, я ошиблась.";
            link.l1 = "Постойте, cеньора! Отчего вы так печальны? Кто этот счастливец-капитан, судьба которого так вас тревожит? Быть может, я что-то слышал о нем и смогу вам помочь?";
			link.l1.go = "TalkInChurch_2";
		break;
		
		case "TalkInChurch_2":
            dialog.text = "Я помню, вы тоже капитан, правда? Бриг 'Пять ран Христовых' должен был еще вчера бросить якорь в гавани Сан-Хуана. Может быть, недавний шторм застал его на пути из Куманы?";
            link.l1 = "Увы, cеньора, я ничего не знаю о судьбе этого брига, но готов немедленно отправиться на поиски вашего храброго капитана, если вы подарите мне одну-единственную улыбку.";
			link.l1.go = "TalkInChurch_3";
		break;
		
		case "TalkInChurch_3":
            dialog.text = "Ах, сеньор, я жду вовсе не этого капитана, да хранит его Господь. На борту должен быть пассажир - мой брат дон Мигель де Вальдес. Я обещаю вам улыбку за добрую весть об этом корабле.";
            link.l1 = "Я немедля отплываю в Куману, прелестная сеньора!";
			link.l1.go = "TalkInChurch_4";
		break;
		
		case "TalkInChurch_4":
            dialog.text = "Я с нетерпением буду ждать от вас вестей о брате. Вы помните, где находится мой дом?";
            link.l1 = "Да, конечно. Всего доброго.";
			link.l1.go = "TalkInChurch_Go";
		break;
		
		case "TalkInChurch_Go":
			chrDisableReloadToLocation = true;
            NextDiag.CurrentNode = "WaitShip";
            Pchar.RomanticQuest.ShipToCumana = true;
			DialogExit();
			AddQuestRecord("Romantic_Line", "5");
			LAi_SetActorType(NPChar);
            LAi_ActorGoToLocation(NPChar, "reload", "reload1", "SanJuan_houseS1Bedroom", "goto", "goto4", "Romantic_TalkInChurch_end", 20.0);
            // немного веселой жизни
            TraderHunterOnMap();
		break;
		
		case "WaitShip":
		    NextDiag.TempNode = "WaitShip";
            dialog.text = "Есть какие-нибудь новости?";
            link.l1 = "Я сообщу, когда узнаю судьбу корабля 'Пять ран Христовых'. До встречи, "+Npchar.Name+ ".";
			link.l1.go = "exit";
			if (!CheckAttribute(pchar, "quest.VisitStep")) pchar.quest.VisitStep = 1;
			else pchar.quest.VisitStep = makeint(pchar.quest.VisitStep)+1;
			bDisableFastReload = true;
			pchar.quest.wasInBeedroom = true;
			pchar.quest.RP_afterVisitBedroom.win_condition.l1 = "ExitFromLocation";
			pchar.quest.RP_afterVisitBedroom.win_condition.l1.location = pchar.location;
			pchar.quest.RP_afterVisitBedroom.win_condition = "RP_afterVisitBedroom";
		break;
		// разговор с братом
		case "Brother_1":
            dialog.text = "Мигель! Как я рада тебя видеть! Я волновалась за тебя! От тебя так долго не было вестей! Как отец?";
            link.l1 = "Изабелла! Если бы не этот храбрый капитан, то я долго еще не смог бы добраться до Сан-Хуана. Ужасные штормы бушевали вокруг Куманы подобно демонам. Я принес ужасную весть - наш отец умер, Изабелла! Он был вынужден продать родовое поместье, чтобы оплатить твои векселя. Для него это была настоящая трагедия, и его сердце не вынесло этого!";
			link.l1.go = "Brother_2";
		break;		
		case "Brother_2":
            dialog.text = "Но, Мигель! Но я не выдавала никаких векселей! Это какое-то трагическое недоразумение!";
            link.l1 = "Сестра, ты еще не поблагодарила нашего замечательного капитана за столь любезно оказанную нам услугу. Полагаю, мы сможем выяснить наши семейные вопросы позже.";
			link.l1.go = "exit";
			AddDialogExitQuest("Romantic_Brother_House_3");
			NextDiag.TempNode = "Brother_3";
		break;		
		case "Brother_3":
            dialog.text = "Ах, капитан "+GetFullName(pchar)+"! Я благодарна вам от всего сердца! Простите меня! Я так потрясена смертью отца, что забыла о вас! Я буду рада видеть вас сегодня вечером в нашем доме!";
            link.l1 = "Сеньора Изабелла! Я всегда к вашим услугам. А сейчас прошу меня простить, я вынужден вас покинуть.";
			link.l1.go = "Brother_4";
		break;
		case "Brother_4":
			AddQuestRecord("Romantic_Line", "8");
			LAi_SetActorType(npchar);
			LAi_ActorGoToLocation(npchar, "reload", "reload2", "SanJuan_houseS1Bedroom", "goto", "goto4", "Romantic_TalkInChurch_end", 5.0);
			NextDiag.CurrentNode = "Temp_Thanks";
			DialogExit();
		break;		
		case "Temp_Thanks":
            NextDiag.TempNode = "Temp_Thanks";
            dialog.text = "О, храбрый капитан! Я жду вас сегодня к ужину!";
            link.l1 = "Всенепременно буду, сеньора Изабелла.";
			link.l1.go = "exit";
			if (!CheckAttribute(pchar, "quest.VisitStep")) pchar.quest.VisitStep = 1;
			else pchar.quest.VisitStep = makeint(pchar.quest.VisitStep)+1;
			bDisableFastReload = true;
			pchar.quest.wasInBeedroom = true;
			pchar.quest.RP_afterVisitBedroom.win_condition.l1 = "ExitFromLocation";
			pchar.quest.RP_afterVisitBedroom.win_condition.l1.location = pchar.location;
			pchar.quest.RP_afterVisitBedroom.win_condition = "RP_afterVisitBedroom";			
		break;
		// разговор с ГГ на взятом бриге
		case "BrigAbordage":
            dialog.text = "ЭТО ВЫ!!! Безжалостный и лицемерный убийца! Вор и пират!";
            link.l1 = "Сеньора, я не безгрешен, но мою честь не позволено задевать даже вам!";
			link.l1.go = "BrigAbordage_1";
		break;
		case "BrigAbordage_1":
            dialog.text = "Не притворяйтесь невинным! Сальватор рассказал мне, как вы подло убили моего брата Мигеля! Да! Убийца!";
            link.l1 = "И вы поверили? Сеньора! Я столько готов сделать ради вас, а вы полагаете, что я убил вашего брата? Сеньора...";
			link.l1.go = "BrigAbordage_2";
		break;
		case "BrigAbordage_2":
            dialog.text = "Мой брат мертв, а вы сбежали! Я не желаю слушать ваших оправданий!";
			if (CheckCharacterItem(pchar, "Order"))
			{			
				link.l1 = "Мне ни к чему оправдываться, ибо я сам видел, как Сальватор заколол вашего брата. Мигель выяснил, что именно Сальватор подделал векселя, убившие вашего отца. Он собирался доложить об этом властям, и тогда Сальватор убил его. Я нашел фальшивые векселя на теле вашего брата. Они залиты кровью дона Мигеля, но вполне читаемы. Вот они, взгляните...";
				link.l1.go = "BrigAbordage_8";
			}
			else
			{
				link.l1 = "Мне не к чему оправдываться, ибо я сам видел, как Сальватор зарезал вашего брата. Ваш брат выяснил, что именно Сальватор подделал векселя, убившие вашего отца. Дон Мигель собирался доложить об этом губернатору и тогда Сальватор убил его. Он пытался убить и меня, чтобы сокрыть свое преступление. Вы все равно бы мне не поверили, и я решил покинуть город на время.";
				link.l1.go = "BrigAbordage_3";
			}
		break;
		// --> нету поддельных векселей в наличие
		case "BrigAbordage_3":
            dialog.text = "Вы, капитан, не только кровавый убийца, но еще и грязный лжец! Чем вы можете подтвердить правоту ваших слов?";
            link.l1 = "Сеньора, я собственными ушами слышал, как ваш брат смело бросил в лицо Сальватору обвинения в подделке векселей. А затем собственными глазами видел, как Сальватор и двое убийц закололи бедного Мигеля! К сожалению, я не успел прийти на помощь...";
			link.l1.go = "BrigAbordage_4";
		break;
		case "BrigAbordage_4":
            dialog.text = "Вы хотите, чтобы я поверила вашим словам? А где эти 'поддельные векселя', из-за которых погиб мой брат?! Покажите мне их!";
            link.l1 = "У меня их нет, сеньора...";
			link.l1.go = "BrigAbordage_5";
		break;	
		case "BrigAbordage_5":
            dialog.text = "Поистине, нет пределов человеческой низости. Слушайте, капитан! Я ненавижу и глубоко презираю вас...";
            link.l1 = "Сеньора Изабелла, у нас нет времени на разговоры, корабль скоро пойдет ко дну. Я предлагаю продолжить эту беседу у меня на судне.";
			link.l1.go = "BrigAbordage_6";
		break;	
		case "BrigAbordage_6":
            dialog.text = "Слушайте, вы, мерзкий пират! Никакая сила в мире не заставит меня пойти с вами. Если мне суждено погибнуть, то пусть это будет здесь и сейчас. А вас, капитан, я проклинаю за все беды, которые вы принесли нашей семье!";
            link.l1 = "Черт возьми, Изабелла!!";
			link.l1.go = "BrigAbordage_7";
		break;	
		case "BrigAbordage_7":
            dialog.text = "Прощайте, более я не скажу ни слова...";
            link.l1 = "...";
			link.l1.go = "exit";
			LAi_SetPlayerType(pchar);
			LAi_CharacterDisableDialog(npchar);
			npchar.location = "none";
			npchar.location.group = "";
			npchar.location.locator = "";
			AddQuestRecord("Romantic_Line", "17");
			QuestSetCurrentNode("Atilla", "Isabella_Sink");
		break;	
		// --> поддельные векселя - у ГГ.
		case "BrigAbordage_8":
            dialog.text = "ЧТО?!! Сальватор?!! Это подлая ложь!! Векселя! Сальватор любит меня... Мерзавец! Но это письмо... и этот корабль... я совсем запуталась... Как я хочу, чтобы это был только сон, капитан " + GetFullName(pchar) + ".";
            link.l1 = "Письмо? Корабль? О чем вы говорите?";
			link.l1.go = "BrigAbordage_9";
			TakeItemFromCharacter(pchar, "Order");
		break;	
		case "BrigAbordage_9":
            dialog.text = "Капитан этого судна явился в мой дом с письмом, написанным рукой Сальватора. Муж писал о какой-то опасности, которая мне грозит. Он просил меня покинуть Сан-Хуан на корабле капитана, доставившего письмо. Но как только корабль покинул гавань Сан-Хуана, меня заперли в каюте. Только теперь я поняла, что попала в ловушку!";
            link.l1 = "А где сейчас Сальватор? Думаю, пришло время поговорить с ним.";
			link.l1.go = "BrigAbordage_10";
		break;	
		case "BrigAbordage_10":
            dialog.text = "Я не знаю, где мой муж... Господи, это просто невыносимо! Потерять отца и брата, а теперь, оказывается, что мой муж Сальватор - виновник всех этих бед!\n"+
				          "Могу я просить вас об услуге, капитан?";
            link.l1 = "Буду рад служить вам, сеньора Изабелла.";
			link.l1.go = "BrigAbordage_11";
		break;	
		case "BrigAbordage_11":
            dialog.text = "Не могли бы вы отвезти меня к моей кузине Росите Фернандес в Белиз?";
            link.l1 = "Конечно, сеньора. Но позвольте мне, раз мы рядом с Порто Белло, посетить этот город. Попытаюсь разыскать там Сальватора, ведь он ждет вашего прибытия.";
			link.l1.go = "BrigAbordage_12";
		break;	
		case "BrigAbordage_12":
            dialog.text = "Хорошо, капитан. Ну а после, уж будьте добры, доставьте меня к Росите...";
            link.l1 = "Все будет исполнено, сеньора.";
			link.l1.go = "exit";
			LAi_SetPlayerType(pchar);
			pchar.RomanticQuest = "SeekInPortoBello";
			AddQuestRecord("Romantic_Line", "18");
			AddPassenger(pchar, npchar, false);
			SetCharacterRemovable(npchar, false);			
			//засада, оставленная Сальватором в Порто Белло
            Pchar.quest.Romantic_AmbushInPortoBello.win_condition.l1 = "location";
            Pchar.quest.Romantic_AmbushInPortoBello.win_condition.l1.location = "PortoBello_houseF2";
            Pchar.quest.Romantic_AmbushInPortoBello.win_condition = "Romantic_AmbushInPortoBello";
			//кузину Роситу в дом Белиза
			sld = GetCharacter(NPC_GenerateCharacter("Rosita", "girl_11", "woman", "towngirl", 5, SPAIN, -1, false));
            sld.name 	= "Росита";
	        sld.lastname = "Фернандес";
			sld.Dialog.Filename = "Quest\Isabella\BrigCaptain.c";
	        sld.dialog.currentnode = "Rosita";
			LAi_SetStayType(sld);
			ChangeCharacterAddressGroup(sld, "Beliz_houseS4", "goto", "goto2");
		break;	
		//Изабелла в доме с засадой в Порто Белло
		case "AmbushPortoBello":
            dialog.text = "Сеньор " + pchar.name + ", я сошла с корабля на берег узнать, как у вас дела.";
            link.l1 = "Сеньора Изабелла, это весьма опрометчивый поступок...";
			link.l1.go = "AmbushPortoBello_1";
		break;	
		case "AmbushPortoBello_1":
            dialog.text = "Боже мой, что здесь произошло?!!";
            link.l1 = "Ох, Изабелла, вам очень не повезло с браком. Это - два бандита, которым Сальватор заказал ваше убийство.";
			link.l1.go = "AmbushPortoBello_2";
		break;	
		case "AmbushPortoBello_2":
            dialog.text = "Мое убийство?!";
			link.l1 = "Именно так. Как сказал один из этой парочки: 'придушить по-тихому'. Они выложили мне все - перепутали с капитаном брига, который вывез вас из Сан-Хуана. Сеньора, это просто счастье, что я вовремя об этом узнал и успел перехватить вас...";
			link.l1.go = "AmbushPortoBello_3";
		break;	
		case "AmbushPortoBello_3":
            dialog.text = "Право слово, капитан, это действительно так. Без всякого сомнения, я вам обязана жизнью...";
            link.l1 = "Пустое, Изабелла. Когда я вижу вас такой испуганной и беззащитной, в душе моей просыпается неведомое доселе чувство...";
			link.l1.go = "AmbushPortoBello_4";
		break;	
		case "AmbushPortoBello_4":
            dialog.text = "Ах, сеньор " + pchar.name + ", сейчас не время и, тем более, не место для такого рода беседы. Простите меня, капитан.\n"+
				          "Давайте покинем это ужасное место!";
            link.l1 = "Это вы меня простите, сеньора Изабелла, я действительно выбрал неудачное время... Мы уходим.";
			link.l1.go = "AmbushPortoBello_5";
		break;	
		case "AmbushPortoBello_5":
			AddQuestRecord("Romantic_Line", "24");
			LAi_SetActorType(npchar);
			LAi_ActorGoToLocation(npchar, "reload", "reload1", "none", "", "", "", 2.0);
			DialogExit();
		break;
		//Изабелла на выходе из дома Роситы в Белизе
		case "BelizTalk":
            dialog.text = "Ну вот, мы и прибыли. Что скажете, сеньор " + pchar.name + "?";
            link.l1 = "Сеньора, я выполнил вашу просьбу. Роситу я уже известил о вашем прибытии, она ждет вас. К тому же я осмотрел дом и узнал у вашей кузины, что Сальватор не появлялся в округе.";
			link.l1.go = "BelizTalk_1";
		break;
		case "BelizTalk_1":
            dialog.text = "Капитан, вы мой ангел-хранитель...";
            link.l1 = "Изабелла, скажите, могу я сейчас говорить о моих чувствах?";
			link.l1.go = "BelizTalk_2";
		break;
		case "BelizTalk_2":
            dialog.text = "Говорите, сеньор...";
            link.l1 = "Изабелла, вам известно, что я - капер. Занятие, недостойное кавалера такой дамы, как вы. Но я люблю вас, Изабелла, и хочу, чтобы вы знали это!";
			link.l1.go = "BelizTalk_3";
		break;
		case "BelizTalk_3":
            dialog.text = "Капитан " + pchar.name + ", вы благородный человек, настоящий рыцарь. Я обязана вам жизнью, но хочу сказать прямо, что мои чувства к вам не ограничиваются одной благодарностью за спасение...";
            link.l1 = "Сеньора, если я правильно вас понял - я могу надеяться на то, что однажды вы скажете мне 'да'?";
			link.l1.go = "BelizTalk_4";
		break;
		case "BelizTalk_4":
            dialog.text = "Вы все правильно поняли, мой ангел-хранитель.";
            link.l1 = "Я просто счастлив, черт возьми, и готов прыгать от радости, как мальчишка! Ох, простите меня, сеньора, за крепкое словцо - вырвалось...";
			link.l1.go = "BelizTalk_5";
		break;
		case "BelizTalk_5":
            dialog.text = "Ничего, капитан. Я постараюсь смягчить ваш нрав, если мне будет предоставлена такая возможность.";
            link.l1 = "Изабелла, я ваш до мозга костей. Но сейчас у нас осталось одно маленькое дельце - Сальватор.";
			link.l1.go = "BelizTalk_6";
		break;
		case "BelizTalk_6":
            dialog.text = "И что вы хотите предпринять?";
            link.l1 = "Не знаю, четких зацепок нет, Сальватор постоянно в движении. Но у меня есть один наш общий знакомый - пират по имени Атилла. Сальватор здорово ему задолжал...";
			link.l1.go = "BelizTalk_7";
		break;
		case "BelizTalk_7":
            dialog.text = "Ясно, капитан. Ну что же, мне пока нужно отдохнуть от пережитого, я будут ждать вас с хорошими новостями здесь, у моей кузины.";
            link.l1 = "До свидания, Изабелла.";
			link.l1.go = "BelizTalk_8";
		break;
		case "BelizTalk_8":
			QuestSetCurrentNode("Atilla", "SalvatorNews");
			NextDiag.CurrentNode = "BelizTalk_after";
			AddQuestRecord("Romantic_Line", "19");
			chrDisableReloadToLocation = false;
			RemovePassenger(pchar, npchar);
			LAi_SetActorType(npchar);
			LAi_ActorGoToLocation(npchar, "reload", "houseS4", "Beliz_houseS4", "goto", "goto1", "Romantic_TalkInChurch_end", 3.5);
			DialogExit();
		break;
		case "BelizTalk_after":
            dialog.text = "Капитан, какие у вас новости?";
            link.l1 = "Пока никаких, Изабелла.";
			link.l1.go = "exit";
			NextDiag.TempNode = "BelizTalk_after";
		break;
		//Изабелла в пещере
		case "IsabellaInCave":
            dialog.text = "Ах, вот и вы, " + pchar.name + "! Наконец-то я вас дождалась!";
            link.l1 = "Изабелла, вы живы! Слава Господу! В джунглях я нашел мертвую девушку, по всей видимости, это служанка, с которой вчера вы покинули дом Роситы.";
			link.l1.go = "IsabellaInCave_1";
		break;
		case "IsabellaInCave_1":
            dialog.text = "Бедная Канчита, Сальватор убил ее!";
            link.l1 = "Мерзавец! Расскажите мне быстро, Изабелла, что произошло?";
			link.l1.go = "IsabellaInCave_2";
		break;
		case "IsabellaInCave_2":
            dialog.text = pchar.name + ", мой муж - исчадие ада! Должна признатся, что я поступала очень опрометчиво - ежедневно выходила на прогулку за город. Вчера Сальватор с еще одним бандитом напали на нас, и этот убийца, не медля ни секунды, ударил Канчиту ножом. Боже мой!..";
            link.l1 = "Успокойтесь, Изабелла... Что было дальше?";
			link.l1.go = "IsabellaInCave_3";
		break;
		case "IsabellaInCave_3":
            dialog.text = "Дальше они приволокли меня сюда. Сальватор пытался заставить меня подписать доверенности на распоряжение имуществом бедного Мигеля, ведь после его смерти все досталось мне по завещанию.";
            link.l1 = "Так вот почему он не убил вас немедленно! Изабелла, это просто невиданная удача, что Сальватор узнал о завещании Мигеля.";
			link.l1.go = "IsabellaInCave_4";
		break;
		case "IsabellaInCave_4":
            dialog.text = "Я поняла, что как только я подпишу эти бумаги, то в ту же минуту буду мертва. Я собрала все свои силы и отказалась подписывать, несмотря ни на какие угрозы. Надеялась я лишь на то, что Росита сообщит вам о моей пропаже и вы сможете отыскать меня...";
            link.l1 = "Ох, Изабелла, я тоже хорош! Оставил вас в Белизе одну и без защиты! А ведь мог бы догадаться, во что это может вылиться...";
			link.l1.go = "IsabellaInCave_5";
		break;
		case "IsabellaInCave_5":
            dialog.text = "Не корите себя, " + pchar.name + ", ведь вы опять спасли меня.";
            link.l1 = "Изабелла, с вашими тюремщиками я разделался, а где сам Сальватор?";
			link.l1.go = "IsabellaInCave_6";
		break;
		case "IsabellaInCave_6":
            dialog.text = "Он ушел в город, но скоро обещал вернуться с жаровней.";
            link.l1 = "Какой жаровней?";
			link.l1.go = "IsabellaInCave_7";
		break;
		case "IsabellaInCave_7":
            dialog.text = "С жаровней и щипцами - чтобы прижигать мне пятки.";
            link.l1 = "Черт возьми! Я действительно появился здесь вовремя...";
			link.l1.go = "exit";
			LAi_SetActorType(npchar);
			pchar.RomanticQuest = "FoundIsabella";
			AddDialogExitQuest("Romantic_SalvatorInCave");
		break;
		//после схватки с Сальватором в пещере
		case "SalvatorIsDead":
            dialog.text = "Все кончено!";
            link.l1 = "Наконец-то, Изабелла...";
			link.l1.go = "SalvatorIsDead_1";
		break;
		case "SalvatorIsDead_1":
            dialog.text = "Сеньор " + pchar.name + ", прошу вас, проводите меня в дом Роситы.";
            link.l1 = "Сеньора Изабелла, раз так все вышло и вы теперь вдова, то позвольте мне преложить вам свою руку и сердце!";
			link.l1.go = "SalvatorIsDead_2";
		break;
		case "SalvatorIsDead_2":
            dialog.text = pchar.name + ", я принимаю ваше предложение.";
            link.l1 = "Отлично! А я пока попытаюсь найти то, что украл у вас Сальватор.";
			link.l1.go = "SalvatorIsDead_3";
		break;
		case "SalvatorIsDead_3":
            dialog.text = "Сеньор, делайте, что хотите, я же хочу попросту забыть обо всем, что произошло.\n"+
				          "Давайте сделаем таким образом: несколько дней я пробуду у Роситы, а затем самостоятельно доберусь до Сан-Хуана, ведь мне теперь ничего не угрожает.";
            link.l1 = "Да, Изабелла, полагаю, что опасности для вас более нет.";
			link.l1.go = "SalvatorIsDead_4";
		break;
		case "SalvatorIsDead_4":
            dialog.text = "В Сан-Хуане я приведу в порядок свои дела, верну свою девичью фамилию - де Вальдес, и где-то через месяц мы обвенчаемся в церкви города.";
            link.l1 = "Очень разумно, Изабелла...";
			link.l1.go = "SalvatorIsDead_5";
		break;
		case "SalvatorIsDead_5":
            dialog.text = "Но вы должны обещать мне, что бросите ваш нынешний род занятий - каперство.";
            link.l1 = "Сеньора, это серьезный шаг для меня. Я зарабатываю этим на жизнь. Но ради вас я готов на все... Я согласен, Изабелла!";
			link.l1.go = "SalvatorIsDead_6";
		break;
		case "SalvatorIsDead_6":
            dialog.text = "Очень хорошо, " + pchar.name + ". А теперь я прошу вас - отведите меня к моей кузине, я больше не могу здесь находиться...";
            link.l1 = "Хорошо, Изабелла.";
			link.l1.go = "SalvatorIsDead_7";
		break;
		case "SalvatorIsDead_7":
			Pchar.quest.Romantic_DelivToRosita.win_condition.l1 = "location";
			Pchar.quest.Romantic_DelivToRosita.win_condition.l1.location = "Beliz_houseS4";
			Pchar.quest.Romantic_DelivToRosita.win_condition = "Romantic_DelivToRosita";
			LAi_SetActorType(npchar);
			LAi_ActorFollowEverywhere(npchar, "", -1);
			DialogExit();
		break;
		//после спасения в доме у Роситы
		case "RositaHouse":
            dialog.text = "Капитан " + GetFullName(pchar) + ", я жду вас в Сан-Хуане через месяц...";
            link.l1 = "Хорошо, Изабелла. Но вы уверены, что доберетесь сами без проблем?";
			link.l1.go = "RositaHouse_1";
			AddQuestRecord("Romantic_Line", "22");
		break;
		case "RositaHouse_1":
            dialog.text = "Я буду в Сан-Хуане через месяц, будьте уверены...";
            link.l1 = "Хорошо, Изабелла.";
			link.l1.go = "exit";
			NextDiag.TempNode = "RositaHouse_1";
		break;
		//Изабелла вновь в Сан-Хуане
		case "InSanJuanAgain":
            dialog.text = "Сеньор " + pchar.name + ", вы прибыли...";
            link.l1 = "Да, сеньора Изабелла. Все в порядке?";
			link.l1.go = "InSanJuanAgain_1";
		break;
		case "InSanJuanAgain_1":
            dialog.text = "Да, все хорошо. Итак, мой ангел-хранитель, вы готовы к тихой семейной жизни?";
            link.l1 = "Да, Изабелла, хватит с меня крови.";
			link.l1.go = "InSanJuanAgain_2";
		break;
		case "InSanJuanAgain_2":
            dialog.text = "И вы все еще любите меня?..";
            link.l1 = "Изабелла, дорогая, как вы можете спрашивать такое?! Я без ума от вас!";
			link.l1.go = "InSanJuanAgain_3";
		break;
		case "InSanJuanAgain_3":
            dialog.text = "Хорошо, " + pchar.name + ". Тогда я скажу вам, что все уже приготовлено к венчанию. Завтра утром я жду вас в церкви у алтаря...";
            link.l1 = "Изабелла... Утром наши судьбы соединяться!";
			link.l1.go = "InSanJuanAgain_4";
		break;
		case "InSanJuanAgain_4":
            dialog.text = "Да, мой рыцарь. А сейчас я прошу вас покинуть мой дом, до свадьбы вам тут делать нечего...";
            link.l1 = "Хорошо, Изабелла, до завтра...";
			link.l1.go = "exit";
			NextDiag.TempNode = "InSanJuanAgain_after";
			LocatorReloadEnterDisable("SanJuan_town", "houseSp6", true); //закрываем двери дома.
			pchar.RomanticQuest = "WaitBeginWidding";
			AddQuestRecord("Romantic_Line", "23");
		break;
		case "InSanJuanAgain_after":
			dialog.text = NPCStringReactionRepeat("До свадьбы вам тут делать нечего...", "Сеньор, будьте благоразумны! До свадьбы - нельзя!", "Нет, нет и еще раз нет!",
                          "Сеньор 'мой возлюбленный', я прошу вас удалиться немедленно!", "block", 1, npchar, Dialog.CurrentNode);
			link.l1 = HeroStringReactionRepeat("Ах, Изабелла, как же мне выдержать все это?! Я хочу видеть вас сейчас...", "Изабелла, это проклятие какое-то. Я с ума сойду до завтра!",
                      "Каррамба! Но что делать?.. Зато я рад, что законы чести для моей невесты - не пустой звук...", "Ах, Изабелла, я вынужден подчиниться...", npchar, Dialog.CurrentNode);
			link.l1.go = DialogGoNodeRepeat("exit", "exit", "exit", "InSanJuanAgain_out", npchar, Dialog.CurrentNode);
			NextDiag.TempNode = "InSanJuanAgain_after";
		break;
		case "InSanJuanAgain_out":
			AddDialogExitQuest("Romantic_OutFromHouse");
			DialogExit();
		break;

		case "Cancel_Widding":
            dialog.text = "Как вы посмели не явиться в церковь на венчание?! Я отдала вам свою любовь, а вы так жестоко и бездарно ее растоптали!";
            link.l1 = "Изабелла...";
			link.l1.go = "exit";
			NextDiag.TempNode = "Cancel_Widding_1";
			LocatorReloadEnterDisable("SanJuan_town", "houseSp6", true); //закрываем дом
			CloseQuestHeader("Romantic_Line");
		break;
		case "Cancel_Widding_1":
            dialog.text = "Все, с меня хватит. Убирайтесь вон!";
            link.l1 = "Эх, Изабелла...";
			link.l1.go = "exit";
			NextDiag.TempNode = "Cancel_Widding_1";
		break;
		//свадьба
		case "TalkWidding":
            dialog.text = "Здравствуйте, " + pchar.name + ".";
            link.l1 = "Здравствуйте, Изабелла... Вы сегодня прекрасны, как никогда!";
			link.l1.go = "TalkWidding_1";
		break;
		case "TalkWidding_1":
            dialog.text = "Спасибо, " + pchar.name + ". Вы готовы, можно начинать?";
            link.l1 = "Конечно!";
			link.l1.go = "TalkWidding_2";
		break;
		case "TalkWidding_2":
            dialog.text = "Падре, вы можете начинать...";
            link.l1 = "...";
			link.l1.go = "exit";
			LAi_SetActorType(npchar);
			LAi_ActorTurnToLocator(npchar, "barmen", "stay");
			AddDialogExitQuest("Romantic_Padre");
		break;
		//драка в церкви
		case "AmbushInChurch":
            dialog.text = "Что происходит? " + pchar.name + ", я боюсь!";
            link.l1 = "Изабелла, в сторону!!";
			link.l1.go = "exit";
			AddDialogExitQuest("Romantic_fightInChurch_2");
		break;
		case "IsabellaIsMyWife":
			dialog.text = "Это были люди Сальватора? Но как же такое может быть - ты же убил его у меня на глазах?!";
			link.l1 = "Он нанял их до того, как я его прикончил.";
			link.l1.go = "IsabellaIsMyWife_1";
		break;
		case "IsabellaIsMyWife_1":
			dialog.text = "О, Господи, неужели я так и буду жить в страхе?";
			link.l1 = "Изабелла, это все! Сальватора больше нет, да и людей, которых он нанял, тоже.";
			link.l1.go = "IsabellaIsMyWife_2";
		break;
		case "IsabellaIsMyWife_2":
			dialog.text = "Очень на это надеюсь... " + pchar.name + ", я хочу домой как можно быстрей!";
			link.l1 = "Иди, дорогая, я немного задержусь, но вскоре буду дома.";
			link.l1.go = "IsabellaIsMyWife_3";
		break;
		case "IsabellaIsMyWife_3":
			rColony = GetColonyByIndex(FindColony("SanJuan"));
			dialog.text = "Дорогой, тебе нужно зайти к губернатору и лично рассказать ему все об этом происшествии. После такого нам нужно получить одобрение властей на проживание в городе.";
			if (sti(rColony.HeroOwn))
			{	
				link.l1 = "Изабелла, этот город принадлежит мне со всеми потрохами. Так что не переживай...";	
				LocatorReloadEnterDisable("SanJuan_town", "houseSp6", false);
			}
			else
			{
				link.l1 = "Хорошо, Изабелла, я все сделаю...";				
				pchar.RomanticQuest.MayorOk = 1; //флаг на базар с мэром Сан-Хуана
			}
			link.l1.go = "IsabellaIsMyWife_4";
		break;
		case "IsabellaIsMyWife_4":
			chrDisableReloadToLocation = true;
			LAi_SetActorType(npchar);
			LAi_ActorRunToLocation(npchar, "reload", "reload1", "SanJuan_houseSp6", "goto", "goto1", "Romantic_TalkInChurch_end", 15.0);
			NextDiag.TempNode = "NewLife_firstTime";
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
		//первый диалог после свадьбы
		case "NewLife_firstTime":
			rColony = GetColonyByIndex(FindColony("SanJuan"));
			dialog.text = "Ах, дорогой, как хорошо, что ты наконец-то пришел домой! Уладил все вопросы с происшествием в церкви?";
			if (sti(rColony.HeroOwn))
			{
				dialog.text = "Ах, дорогой, как хорошо, что ты наконец-то пришел домой! Все в порядке?";
				link.l1 = "Да, дорогая, все нормально...";
			}
			else
			{
				dialog.text = "Ах, дорогой, как хорошо, что ты наконец-то пришел домой! Уладил все вопросы с происшествием в церкви?";
				link.l1 = "Да, дорогая, я был на приеме у губернатора. Все в порядке - он нас поздравляет.";
			}
			link.l1.go = "NewLife_firstTime_1";
		break;
		case "NewLife_firstTime_1":
			dialog.text = "Ну что же, это очень хорошо. Теперь, если не возражаешь, давай обсудим принципы совместного проживания.";
			link.l1 = "Какие могут быть принципы, Изабелла? Я люблю тебя - вот основной принцип!";
			link.l1.go = "NewLife_firstTime_2";
		break;
		case "NewLife_firstTime_2":
			dialog.text = "И я тебя тоже, дорогой. Но семья - это не только любовь, это еще и совместный быт. Таким образом, помимо взаимной любви, у нас есть еще как минимум два принципа, по которым должна жить наша семья.";
			link.l1 = "Изабелла, я восхищен твоим благоразумием! Но, может быть, мы пока остановимся на первом принципе и претворим его в жизнь немедленно?";
			link.l1.go = "NewLife_firstTime_3";
		break;
		case "NewLife_firstTime_3":
			dialog.text = "Дорогой, всему свое время, не будь слишком настойчив! Давай сначала обсудим оставшиеся два, ты не против?..";
			link.l1 = "Дорогая, для тебя - все что угодно... Давай обсудим эти два принципа.";
			link.l1.go = "NewLife_firstTime_4";
		break;
		case "NewLife_firstTime_4":
			dialog.text = "Милый, я тебя обожаю... Итак, принцип номер два - финансы. Мы должны определить семейный бюджет. Твоим домом был корабль, но теперь твой дом здесь, и ты должен понимать, что обладание недвижимостью - это определенное финансовое бремя, то есть городские налоги.\n"+
				          "Кроме того, обеспечение наших жизненных потребностей также требует затрат...";
			link.l1 = "Изабелла, я готов отдать все, что имею!";
			link.l1.go = "NewLife_firstTime_5";
		break;
		case "NewLife_firstTime_5":
			dialog.text = "Хорошо, но начнем с малого... Что же, подведем итог: ежемесячно тебе нужно будет приносить в дом " + FindRussianMoneyString(MOD_SKILL_ENEMY_RATE*10000) + ".";
			link.l1 = "Как скажешь, Изабелла. Все - для тебя! Так, а что у нас третьим принципом?";
			link.l1.go = "NewLife_firstTime_6";
		break;
		case "NewLife_firstTime_6":
			dialog.text = "Третий принцип - это твое обещание бросить каперство. Обеспечивай семью другим способом, не надо топить корабли и захватывать города - Господь нам этого не простит...";
			link.l1 = "Ну что же, свои обещания я помню прекрасно. Я согласен с таким раскладом, дорогая.";
			link.l1.go = "exit";
			NextDiag.TempNode = "NewLife";
			AddDialogExitQuest("Romantic_afterFirstTime");
		break;
//************************************ новая жизнь бывшего корсара ********************************
		case "NewLife_hello":
			iDay = GetQuestPastDayParam("RomanticQuest");
			dialog.text = LinkRandPhrase("Здравствуй, дорогой!", "Приветствую своего мужа.", "Рада видеть своего супруга.");
			link.l1 = RandPhraseSimple("Здравствуй, Изабелла!", "Приветствую, моя дорогая...");
			link.l1.go = "NewLife";
			if (iDay > 40)
			{				
				dialog.text = RandPhraseSimple("Здравствуй дорогой! И сразу же объясни мне, пожалуйста, где тебя носило " + FindRussianDaysString(iDay) + "?", "Приветствую своего мужа, которого носило неизвестно где " + FindRussianDaysString(iDay) + ". Может, расскажешь, дорогой?");
				link.l1 = RandPhraseSimple("Изабелла, дорогая, я промотался все это время по делам.", "Дорогая, навалилось все разом как-то...");
				link.l1.go = "NewLife_hello_1";
			}
			if (GetQuestPastDayParam("RomanticQuest") > 3 && pchar.RomanticQuest == "OpenTheDoosOfHouse")
			{
				dialog.text = "Здравствуй, муж любезный. Я открыла тебе дверь только потому, что меня попросила об этом кузина Росита.";
				link.l1 = "Изабелла, я все понимаю, но и ты пойми меня - я моряк!";
				link.l1.go = "NewLife_hello_2";	
			}
		break;	
		case "NewLife_hello_1":
			dialog.text = "Послушай, я, конечно, понимаю, что ты капитан и работа у тебя такая - мотаться по морю. Но и ты меня пойми - я не собираюсь сидеть дома одна по нескольку месяцев к ряду! И учти, что если тебя не будет дома так долго, то я тебе в очередной раз просто дверь не открою!";
			link.l1 = RandPhraseSimple("Изабелла, я понимаю... Ты уж извини, что пришлось оставить тебя одну надолго.", "Прости меня, что тебе пришлось остаться одной надолго, я все понимаю...");
			link.l1.go = "NewLife";
			SaveCurrentQuestDateParam("RomanticQuest"); //обновим дату, от которой считаем отсутствие
		break;
		case "NewLife_hello_2":
			dialog.text = "Я хочу понимать только одно - интересы своей семьи. Если ты думаешь, что можешь являться сюда время от времени, то глубоко ошибаешься. У нас должна быть семья, а не балаган.";
			link.l1 = "Хорошо, Изабелла, я буду стараться не покидать тебя надолго...";
			link.l1.go = "NewLife";
			SaveCurrentQuestDateParam("RomanticQuest"); //обновим дату, от которой считаем отсутствие
		break;
		//======================== типовое меню ==========================
		case "NewLife":
			//проверяем, как там с финансами у нас
			iMoney = IsabellaCheckBudgetMoney();
			if (iMoney > 0)
			{
				dialog.text = NPCStringReactionRepeat("Дорогой, у нас проблема - мы в долгах, как в шелках. Необходимо внести " + FindRussianMoneyString(iMoney) + " и урегулировать тем самым налоговые проблемы.", 
					"За сегодняшний день я второй раз ставлю вас в известность о финансовых проблемах. Мы задолжали " + FindRussianMoneyString(iMoney) + ".", 
					"В третий раз за сегодня я напоминаю о наших долгах, составляющих " + FindRussianMoneyString(iMoney) + ".", 
					"Который уже раз напоминая об огромной проблеме - долгах в размере " + FindRussianMoneyString(iMoney) + ".", "block", 3, npchar, Dialog.CurrentNode);
				link.l1 = HeroStringReactionRepeat(RandPhraseSimple("Каррамба!! Как же я запустил это дело?!", "Надо же, как все запущено в семейных делах..."), 
					"Второй раз? Вот незадача...", 
					"Уже в третий?! Да уж...", 
					"Я помню, Изабелла...", npchar, Dialog.CurrentNode);
				link.l1.go = "NewLife_Budget_Pay";
				break;
			}
			//--> проверяем статистику на пиратство
			if (CheckAttribute(pchar, "RomanticQuest.Cheking.GrabbingTown") && pchar.RomanticQuest.Cheking != "Ok") 
			{
				if (rand(sti(pchar.RomanticQuest.Cheking.GrabbingTown)*150) >= GetCharacterSkill(pchar, "Sneak"))//ранд типа слухи не дошли
				{
					dialog.text = RandPhraseSimple("Послушай, вокруг только и говорят, что ты разграбил какой-то город. Это правда?", "Слушай, тут ходят слухи, что ты опять взялся за старое и разграбил какой-то город. Правда ли это?");
					link.l1 = LinkRandPhrase("Ну что ты, дорогая, это клевета!", "Дорогая, ничего такого я не делал.", "Нет, милая, это неправда, я же обещал!");
					link.l1.go = "NewLife_KnowPirates";
					IsabellaSetCurrentState("GrabbingTown");
					break;
				}
			}
			if (CheckAttribute(pchar, "RomanticQuest.Cheking.TakeTown") && pchar.RomanticQuest.Cheking != "Ok") 
			{
				if (rand(sti(pchar.RomanticQuest.Cheking.TakeTown)*140) >= GetCharacterSkill(pchar, "Sneak"))//ранд типа слухи не дошли
				{
					dialog.text = RandPhraseSimple("Слушай, ходят слухи, что ты захватил какой-то город. Это правда?", "Слушай, тут ходят слухи, что ты опять взялся за старое и захватил какой-то город. Правда ли это?");
					link.l1 = LinkRandPhrase("Ну что ты, дорогая, это клевета!", "Дорогая, это ничего такого я не делал.", "Нет, милая, это неправда, я же обещал!");
					link.l1.go = "NewLife_KnowPirates";
					IsabellaSetCurrentState("TakeTown");
					break;
				}
			}
			if (CheckAttribute(pchar, "RomanticQuest.Cheking.KillFort") && pchar.RomanticQuest.Cheking != "Ok") 
			{
				if (rand(sti(pchar.RomanticQuest.Cheking.KillFort)*130) >= GetCharacterSkill(pchar, "Sneak"))//ранд типа слухи не дошли
				{
					dialog.text = RandPhraseSimple("Послушай, до меня тут дошли слухи, что ты захватил какой-то форт. Это правда?", "Слушай, тут ходят слухи, что ты опять взялся за старое и захватил какой-то форт. Правда ли это?");
					link.l1 = LinkRandPhrase("Ну что ты, дорогая, это клевета!", "Дорогая, это ничего такого я не делал.", "Нет, милая, это неправда, я же обещал!");
					link.l1.go = "NewLife_KnowPirates";
					IsabellaSetCurrentState("KillFort");
					break;
				}
			}
			if (CheckAttribute(pchar, "RomanticQuest.Cheking.AbordShip") && pchar.RomanticQuest.Cheking != "Ok") 
			{
				if (rand(sti(pchar.RomanticQuest.Cheking.AbordShip)*70) >= GetCharacterSkill(pchar, "Sneak"))//ранд типа слухи не дошли
				{
					dialog.text = RandPhraseSimple("До меня дошли ужасные слухи, что ты берешь на абордаж корабли. Это правда?", "Слушай, тут ходят слухи, что ты опять взялся за старое - берешь на абордаж и грабишь корабли. Правда ли это?");
					link.l1 = LinkRandPhrase("Ну что ты, дорогая, это клевета!", "Дорогая, это ничего такого я не делал.", "Нет, милая, это неправда, я же обещал!");
					link.l1.go = "NewLife_KnowPirates";
					IsabellaSetCurrentState("AbordShip");
					break;
				}
			}
			if (CheckAttribute(pchar, "RomanticQuest.Cheking.KillShip") && pchar.RomanticQuest.Cheking != "Ok") 
			{
				if (rand(sti(pchar.RomanticQuest.Cheking.KillShip)*50) >= GetCharacterSkill(pchar, "Sneak"))//ранд типа слухи не дошли
				{
					dialog.text = RandPhraseSimple("Послушай, люди поговаривают, что ты топишь корабли. Это правда?", "Слушай, тут ходят слухи, что ты опять взялся за старое - топишь корабли. Правда ли это?");
					link.l1 = LinkRandPhrase("Ну что ты, дорогая, это клевета!", "Дорогая, это ничего такого я не делал.", "Нет, милая, это неправда, я же обещал!");
					link.l1.go = "NewLife_KnowPirates";
					IsabellaSetCurrentState("KillShip");
					break;
				}
			}
			//<-- проверяем статистику на пиратство
			//        -------  ОСНОВНОЕ МЕНЮ  ----------
			dialog.text = LinkRandPhrase("Дорогой, я слушаю тебя...", "Что хочет мой дорогой муж?", "Сеньор " + pchar.lastname + ", ваша жена слушает вас внимательнейшим образом...");
			link.l1 = "Да так, просто поговорить... Соскучился, знаешь ли...";
			link.l1.go = "NewLife_JustTalk";
			link.l2 = "Давай посмотрим, в каком состоянии наш семейный бюджет...";
			link.l2.go = "NewLife_Budget";
			link.l3 = "Дорогая, я принес тебе подарок.";
			link.l3.go = "NewLife_Present";
			link.l4 = "Послушай, милая, по поводу супружеского долга...";
			link.l4.go = "NewLife_Sex";
			link.l5 = "Спать хочу, просто сил нет...";
			link.l5.go = "NewLife_Sleep";
		break;
		//=========================== Бюджет =================================
		case "NewLife_Budget":	
			iMoney = IsabellaCheckBudgetMoney();
			if (iMoney > -(MOD_SKILL_ENEMY_RATE*10000)) //деньги на исходе
			{
				if (iMoney == 0)
				{
					dialog.text = RandPhraseSimple("Долгов пока нет, но деньги закончились, дорогой...", "Разве что долгов нет и судебный исполнитель в дверь не ломится... А так деньги кончились совсем.");
				}
				else
				{
					dialog.text = "Долгов пока нет, но деньги кончаются. У меня осталось " + FindRussianMoneyString(-iMoney) + ".";
					link.l1 = RandPhraseSimple("Ну, придет время - будем решать...", "Мне сейчас не до этого, расходуй то, что есть...");
					link.l1.go = "NewLife";
				}
				link.l2 = RandPhraseSimple("Ну что же, тогда подкинем тебе пиастров, дорогуша!", "Нужно срочно пополнить бюджет, давай этим и займемся!");
				link.l2.go = "NewLife_Budget_Add";
			}
			else //профицит
			{
				dialog.text = RandPhraseSimple("Никаких проблем, дорогой, денег пока хватает - " + FindRussianMoneyString(-iMoney) + " еще у меня есть.", "Все нормально, милый, денег в нашем бюджете достаточно - " + FindRussianMoneyString(-iMoney) + ".");
				link.l1 = RandPhraseSimple("Ну что же, это радует.", "Прекрасно, дорогая, очень хорошо!");
				link.l1.go = "NewLife";			
				link.l2 = "Хочу еще оставить тебе денег, чтобы ты ни в чем не нуждалась.";
				link.l2.go = "NewLife_Budget_Add";
			}
		break;
		case "NewLife_Budget_Add":			
            dialog.text = NPCStringReactionRepeat("Хорошо, дорогой. Сколько денег ты хочешь мне оставить?", "Зачем ты хочешь мне что-то оставить уже второй раз на дню - не пойму... Так сколько ты хочешь оставить?", "Ты что, издеваешься надо мной? Третий раз на дню ты хочешь дать мне денег! Сразу все нельзя решить?", "Ну все, ты мне надоел - зануда нестерпимый! Не буду больше с тобой разговаривать!", "repeat", 10, npchar, Dialog.CurrentNode);
			if (sti(pchar.money) >= 50000 && npchar.quest.repeat.NewLife_Budget_Add.ans != "3")
			{
				link.l1 = HeroStringReactionRepeat("50 тысяч.", "Хм, 50 тысяч, хочу оставить...", "Кхе... 50 тысяч хочу оставить тебе.", "...", npchar, Dialog.CurrentNode);
				link.l1.go = "NewLife_Budget_50";
			}
			if (sti(pchar.money) >= 100000 && npchar.quest.repeat.NewLife_Budget_Add.ans != "3")
			{
				link.l2 = HeroStringReactionRepeat("100 тысяч.", "100 тысяч...", "100 тысяч...", "...", npchar, Dialog.CurrentNode);
				link.l2.go = "NewLife_Budget_100";
			}
			if (sti(pchar.money) >= 200000 && npchar.quest.repeat.NewLife_Budget_Add.ans != "3")
			{
				link.l3 = HeroStringReactionRepeat("200 тысяч.", "200 тысяч...", "200 тысяч...", "...", npchar, Dialog.CurrentNode);
				link.l3.go = "NewLife_Budget_200";
			}
			if (sti(pchar.money) >= 500000 && npchar.quest.repeat.NewLife_Budget_Add.ans != "3")
			{
				link.l4 = HeroStringReactionRepeat("500 тысяч.", "500 тысяч...", "500 тысяч...", "...", npchar, Dialog.CurrentNode);
				link.l4.go = "NewLife_Budget_500";
			}
			link.l5 = HeroStringReactionRepeat("Кхе... Пока ничего, Изабелла... С деньгами туговато...", "Так вышло, дорогая... Извини, но денег нет.", "Сразу как-то не получилось, Изабелла... А денег нет.", "Изабелла, ну как же так?..", npchar, Dialog.CurrentNode);
			link.l5.go = "NewLife_Budget_No";
		break;
		case "NewLife_Budget_50":
			dialog.text = "Хорошо, " + pchar.name + " спасибо тебе.";
			link.l1 = "Это моя обязанность, Изабелла...";
			link.l1.go = "NewLife";
			IsabellaAddMoneyBudget(50000);
		break;
		case "NewLife_Budget_100":
			dialog.text = "Хорошо, милый. 100 тысяч - это немало, спасибо тебе.";
			link.l1 = "Не за что, милая...";
			link.l1.go = "NewLife";
			IsabellaAddMoneyBudget(100000);
		break;
		case "NewLife_Budget_200":
			dialog.text = "Очень хорошо, дорогой. 200 тысяч - хорошее подспорье семейному бюджету.";
			link.l1 = "Я рад, что ты довольна, Изабелла...";
			link.l1.go = "NewLife";
			IsabellaAddMoneyBudget(200000);
		break;
		case "NewLife_Budget_500":
			dialog.text = "Так много! Где ты взял столько денег?";
			link.l1 = "Заработал, где же еще?";
			link.l1.go = "NewLife";
			IsabellaAddMoneyBudget(500000);
		break;
		case "NewLife_Budget_No":
            dialog.text = LinkRandPhrase("Ну, и зачем было тогда предлагать?", "Зачем тогда ты об этом говоришь?" , "А зачем тогда об этом говорить?");
			link.l1 = LinkRandPhrase("Я ошибся, дорогая, извини...", "Извини, дорогая, что-то с памятью моей стало...", "С головой что-то не очень хорошо у меня...");
			link.l1.go = "NewLife";
		break;
		//заплатить долги и понулить счетчик бюджета
		case "NewLife_Budget_Pay":
			iMoney = IsabellaCheckBudgetMoney();
			dialog.text = NPCStringReactionRepeat("Верно, капитан " + pchar.lastname + ", вы изрядно запустили это дело! Так что извольте погасить задолженность!", 
				"Вы должны были найти деньги на погашение задолженности!", 
				"Капитан, я который раз прошу вас прошу погасить нашу задолженность по налогам!", 
				"Капитан, я устала повторять одно и то же. Я все говорю, говорю, а воз финансовых проблем и ныне там. Вот что я вам скажу, муженек: я и сама в состоянии справится с проблемами! Ну, а вас я видеть не желаю!", "quest", 30, npchar, Dialog.CurrentNode);
			if (sti(pchar.money) >= iMoney)
			{
				link.l1 = HeroStringReactionRepeat("Хорошо, дорогая, я готов погасить задолженность.", "И я их нашел, Изабелла! Я готов рассчитаться с долгами!", "Изабелла, дорогая, я все прекрасно помню. Я нашел деньги и готов закрыть долги.", "Изабелла, как раз сейчас...", npchar, Dialog.CurrentNode);
				link.l1.go = "NewLife_Budget_Pay_2";
				link.l2 = "Понимаешь, Изабелла, прямо сейчас у меня нет денег...";
				link.l2.go = "NewLife_Budget_Pay_1";	
			}
			else
			{
				link.l1 = HeroStringReactionRepeat("Послушай, дорогуша, у меня нет сейчас " + FindRussianMoneyString(iMoney) + "...", "Должен был, но не нашел...", "Изабелла, нет у меня сейчас денег!", "Вот, значит как?..", npchar, Dialog.CurrentNode);
				link.l1.go = "NewLife_Budget_Pay_1";
			}
		break;
		case "NewLife_Budget_Pay_1":
			dialog.text = RandPhraseSimple("Так идите и найдите! Я не собираюсь из-за вас попадать в долговую яму!", "Послушайте меня, муженек! Я не желаю, чтобы ко мне в дом через день заходил судебный исполнитель и требовал выплат задолженностей! Разыщите деньги, это ваша прямая обязанность!");
			link.l1 = RandPhraseSimple("Хорошо, Изабелла, как скажете...", "Я понял, Изабелла, буду стараться...");
			link.l1.go = "NewLife_exit";
		break;
		case "NewLife_Budget_Pay_2":
            dialog.text = RandPhraseSimple("Ну, что же, превосходно! Я очень рада, что мой муж - ответственный человек.", "Прекрасно, дорогой! Ты - настоящий мужчина...");
			link.l1 = RandPhraseSimple("Да уж...", "Хех, еще бы!");
			link.l1.go = "NewLife";
			iMoney = IsabellaCheckBudgetMoney();
			AddMoneyToCharacter(pchar, -iMoney);
			IsabellaNullBudget();
		break;
		//=========================== Изабелла узнала, что ГГ опять пиратствует =================================
		case "NewLife_KnowPirates":
			pchar.RomanticQuest.Cheking = "Ok"; //флаг "не проверять больше на пиратство" в этот раз
			if (rand(12) < GetCharacterSPECIAL(pchar, "Charisma")) //поверила, дура :)
			{
				dialog.text = LinkRandPhrase("Хм, я верю тебе, " + pchar.name + "...", "Что же, лучше я поверю тебе, чем каким-то слухам!", "Хотелось бы верить, " + pchar.name + ", тебе, а не домыслам...");
				link.l1 = RandPhraseSimple("Ну вот и славненько, дорогая.", "Как здорово! Быстренько порешали - и конец недоразумению...");
				link.l1.go = "NewLife";
			}
			else //не поверила, и правильно сделала
			{
				dialog.text = NPCStringReactionRepeat("Знаешь, милый, а я склонна поверить этим слухам и домыслам. Однако, точно я доказать ничего не могу, так что пусть это будет тебе первым предупреждением.", 
					"Пожалуй, я тебе не поверю - слишком много об этом говорят... Считай это вторым предупреждением.", 
					"Хм, послушай, я вот не пойму, ты меня за дуру принимаешь? Все только об этом и говорят, а он отнекивается! Похоже, что ты не держишь своего слова. В общем, это было последнее предупреждение, " + pchar.name + ".", 
					"Все, капитан " + pchar.lastname + ", доверия больше к тебе больше нет. Ты водил меня за нос! Не желаю с тобой разговаривать!", "quest", 80, npchar, Dialog.CurrentNode);
				link.l1 = HeroStringReactionRepeat("Хорошо, дорогая. Я считаю, что один раз я предупрежден.", 
					"Хороша, дорогая, пусть это будет предупреждением номер два.", 
					"М-м-м, хорошо, Изабелла, пусть это будет последним предупреждением.", 
					"Эх, Изабелла...", npchar, Dialog.CurrentNode);
				link.l1.go = "NewLife";	
			}
		break;
		//=========================== Секс =================================
		case "NewLife_Sex":			
			if (GetQuestPastMonthParam("RomanticQuest") > 2 && GetNpcQuestPastDayParam(npchar, "sex") != 0)
			{
				dialog.text = "Что моряк, ты слишком долго плавал?";
				link.l1 = "Ну-у-у, есть такое...";
				link.l1.go = "NewLife_Sex_1";
			}
			else
			{
				dialog.text = NPCStringReactionRepeat("О чем это ты?", 
					"А-а-а, я догадываюсь, о чем ты...", 
					"Долга? И что я тебе должна?", 
					"Хм, не понимаю...", "cycle", 0, npchar, Dialog.CurrentNode);
				link.l1 = HeroStringReactionRepeat("Да все о том же, Изабелла, о любви...", 
					"Именно об этом, дорогая - о любви...", 
					"Я говорю о долге любви, дорогая...", 
					"Изабелла, я говорю о любви, дорогая...", npchar, Dialog.CurrentNode);
				link.l1.go = "NewLife_Sex_5";
			}			
		break;
		case "NewLife_Sex_1":
			SaveCurrentNpcQuestDateParam(npchar, "sex"); //запоминаем секс
			dialog.text = "Я бы на твоем месте, дорогой, не отлучалась так надолго из дома...";
			link.l1 = "Это почему?";
			link.l1.go = "NewLife_Sex_2";
		break;
		case "NewLife_Sex_2":
			dialog.text = "Потому, что жена у тебя - красивая женщина, если ты этого раньше не замечал. А в Сан-Хуане не один женщины живут, есть еще много интересных кабальеро...";
			link.l1 = "Ать, дьявол!! Изабелла, ты чего это такое говоришь?!";
			link.l1.go = "NewLife_Sex_3";
		break;
		case "NewLife_Sex_3":
			dialog.text = "То и говорю, чтобы ты дома бывал почаще, дорогой!";
			link.l1 = "Ладно, Изабелла, я тебя понял... Давай не будем ссориться, милая...";
			link.l1.go = "NewLife_Sex_4";
		break;
		case "NewLife_Sex_4":
			if (rand(6) < 5)
			{
				dialog.text = "Ну хорошо... Я иду в спальню...";
				link.l1 = "Я - за тобой!";
				link.l1.go = "NewLife_GoSex";
			}
			else
			{
				dialog.text = "Хорошо, ссориться не будем, милый. Но у меня сегодня мигрень, так что ничего не выйдет. Извини...";
				link.l1 = "Эх, как не вовремя!..";
				link.l1.go = "NewLife_exit";
			}
		break;

		case "NewLife_Sex_Late":
			dialog.text = "Знаешь, что дорогой, я не собираюсь ждать тебя тут вечно в полной боевой готовности... Ты упустил свой шанс!";
			link.l1 = "Изабелла, ну как же так?..";
			link.l1.go = "NewLife_Sex_Late_1";			
		break;
		case "NewLife_Sex_Late_1":
			chrDisableReloadToLocation = true;
			bDisableFastReload = true; 
			LAi_SetActorType(npchar);
			LAi_ActorGoToLocation(npchar, "reload", "reload1", "SanJuan_houseSp6", "goto", "goto1", "Romantic_TalkInChurch_end", 3.5);
			NextDiag.TempNode = "NewLife";
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;

		case "NewLife_Sex_5":
			if (GetNpcQuestPastDayParam(npchar, "sex") > 3)
			{
				dialog.text = NPCStringReactionRepeat("М-м-м... Такие разговоры мне по душе...", 
					"О любви? Ну что же, продолжай, милый...", 
					"Любовь, это интересно... Так что же?..", 
					"Ах, эта любовь... Я порой делаю такие глупости!..", "block", 60, npchar, Dialog.CurrentNode);
				link.l1 = HeroStringReactionRepeat("Изабелла, я сгораю от страсти!!", 
					"Изабелла, я страстно желаю разделить с тобой ложе!", 
					"Изабелла, желание обладать тобой переполняет меня...", 
					"Ну какие это глупости, Изабелла?! Давай быстренько пойдем в спальню, дорогая?..", npchar, Dialog.CurrentNode);
				link.l1.go = DialogGoNodeRepeat("NewLife_Sex_10", "NewLife_Sex_11", "NewLife_Sex_12", "NewLife_Sex_13", npchar, Dialog.CurrentNode);
			}
			else //слишком часто, не прокатит
			{
				dialog.text = LinkRandPhrase("Слишком часто о любви говорить нельзя, дорогой. Так что извини, но ничего не выйдет...", "Нет, дорогой, частые разговоры о любви вредны для души и тела, так говорит падре...", "Ах, милый, я не хочу говорить на эту тему слишком часто, не утомляй меня, пожалуйста. Через несколько дней, дорогой...");
				link.l1 = RandPhraseSimple("Гм... Кхе... Жаль...", "М-да, я разочарован...");
				link.l1.go = "NewLife_exit";
			}
			SaveCurrentNpcQuestDateParam(npchar, "sex"); //запоминаем секс
		break;
		case "NewLife_Sex_10":
			dialog.text = "Тогда я жду тебя в спальне, дорогой!";
			link.l1 = "Изабелла, я иду следом...";
			link.l1.go = "NewLife_GoSex";
		break;
		case "NewLife_Sex_11":
			if (rand(10) < 8)
			{
				dialog.text = "Разделить ложе?.. Ну, хорошо. Я иду в спальню...";
				link.l1 = "Ох, Изабелла...";
				link.l1.go = "NewLife_GoSex";
			}
			else //не свезло
			{
				dialog.text = LinkRandPhrase("У меня болит голова, дорогой, не сегодня...", "Я себя сегодня плохо чувствую, дорогой. Не сейчас...", "Милый, давай отложим... Я не могу - плохое самочувствие.");
				link.l1 = RandPhraseSimple("Эх, как не вовремя!..", "Черт!.. Прости, Изабелла, вырвалось...");
				link.l1.go = "NewLife_exit";
			}
		break;
		case "NewLife_Sex_12":
			if (rand(10) < 5)
			{
				dialog.text = "Желание... М-м-м, ну хорошо. Я жду тебя в спальне, дорогой...";
				link.l1 = "Изабелла, я мигом!..";
				link.l1.go = "NewLife_GoSex";
			}
			else //не свезло
			{
				dialog.text = LinkRandPhrase("У меня болит голова, дорогой, не сегодня...", "Я себя сегодня плохо чувствую, дорогой. Не сейчас...", "Милый, давай отложим... Я не могу - плохое самочувствие.");
				link.l1 = RandPhraseSimple("Эх, как не вовремя!..", "Черт!.. Прости, Изабелла, вырвалось...");
				link.l1.go = "NewLife_exit";
			}
		break;
		case "NewLife_Sex_13":
			if (rand(10) < 2)
			{
				dialog.text = "Дорогой, ты нахал!.. Ну ладно, уговорил...";
				link.l1 = "Изабелла...";
				link.l1.go = "NewLife_GoSex";
			}
			else //не свезло
			{
				dialog.text = "Никуда мы не пройдем! И впредь не позволяй себе вульгарных фраз!";
				link.l1 = "Хорошо, Изабелла, извини...";
				link.l1.go = "NewLife_exit";
			}
		break;
		//идем в спальню
		case "NewLife_GoSex":
			chrDisableReloadToLocation = true;
			bDisableFastReload = true; 
			NextDiag.TempNode = "NewLife";
			NextDiag.CurrentNode = NextDiag.TempNode;
			SaveCurrentNpcQuestDateParam(npchar, "sex"); //запомнить секс
			Pchar.quest.Romantic_Sex.win_condition.l1 = "location";
			Pchar.quest.Romantic_Sex.win_condition.l1.location = "SanJuan_houseS1Bedroom";
			Pchar.quest.Romantic_Sex.win_condition = "Romantic_Sex";
			SetTimerCondition("Romantic_IsabellaBackToHall", 0, 0, 1, false);
			LAi_SetActorType(npchar);
			LAi_ActorGoToLocation(npchar, "reload", "reload2", "SanJuan_houseS1Bedroom", "goto", "goto6", "Romantic_TalkInChurch_end", 20.0);
			DialogExit();
		break;
		//после секса
		case "NewLife_afterSex":			
			dialog.text = NPCStringReactionRepeat("М-м-м... Милый, это было... великолепно!", 
				"Ну, как у тебя дела, дорогой? Все хорошо?", 
				"Что-то ты сегодня не в форме...", 
				"Ах, любовь... Это прекрасно...", "block", 30, npchar, Dialog.CurrentNode);
			link.l1 = HeroStringReactionRepeat("Спасибо, дорогая...", 
				"Прекрасно, милая, ты была просто великолепна!", 
				"Гм... Прости, дорогая, так вышло...", 
				"Полностью с тобой согласен, дорогая.", npchar, Dialog.CurrentNode);
			link.l1.go = "NewLife_afterSex_1";
		break;
		case "NewLife_afterSex_1":
			chrDisableReloadToLocation = true;
			bDisableFastReload = true; 
			NextDiag.TempNode = "NewLife";
			NextDiag.CurrentNode = NextDiag.TempNode;
			LAi_SetActorType(npchar);
			LAi_ActorGoToLocation(npchar, "reload", "reload1", "SanJuan_houseSp6", "goto", "goto2", "Romantic_TalkInChurch_end", 20.0);
			Pchar.quest.Romantic_IsabellaBackToHall.over = "yes";
			DialogExit();
		break;
		//=========================== Сон =================================
		case "NewLife_Sleep":
			dialog.text = "Когда тебя разбудить, дорогой?";
			if(!isDay())
			{
				link.l1 = "Утром, будь добра...";
				link.l1.go = "NewLife_Sleep_day";
			}
			else
			{
				link.l1 = "Мне нужно встать ночью, Изабелла.";
				link.l1.go = "NewLife_Sleep_night";
				link.l2 = "Хочу отоспаться. Разбуди следующим утром.";
				link.l2.go = "NewLife_Sleep_day";
			}
		break;
		case "NewLife_Sleep_day":
			dialog.text = "Хорошо, разбужу тебя утром.";
			link.l1 = "Ну, здорово. Тогда я пошел в спальню...";
			link.l1.go = "room_day_wait";
			link.l2 = "Ты знаешь, передумал. Не буду пока спать.";
			link.l2.go = "NewLife_exit";
		break;
		case "NewLife_Sleep_night":
			dialog.text = "Хорошо, разбужу тебя после полуночи.";
			link.l1 = "Отлично, дорогая. Тогда я пошел в спальню...";
			link.l1.go = "room_night_wait";
			link.l2 = "Ты знаешь, передумал. Лучше проведу это время на ногах.";
			link.l2.go = "NewLife_exit";
		break;
		case "room_day_wait":
			DialogExit();
			NextDiag.TempNode = "NewLife_hello";
			NextDiag.CurrentNode = NextDiag.TempNode;
			TavernWaitDate("wait_day");
			AddDialogExitQuest("sleep_in_home");
		break;
		case "room_night_wait":
			DialogExit();
			NextDiag.TempNode = "NewLife_hello";
			NextDiag.CurrentNode = NextDiag.TempNode;	
			TavernWaitDate("wait_night");
			AddDialogExitQuest("sleep_in_home");
		break;
		//=========================== Просто разговор =================================
		case "NewLife_JustTalk":
			if (CheckAttribute(pchar, "RomanticQuest.Atilla"))
			{
				switch (pchar.RomanticQuest.Atilla)
				{
					case "YouAreBaster": 
						dialog.text = "Послушай, я слышала, что ты нехорошо поступил со своим другом - Атиллой. Говорят, что ты дал ему очень мало денег, а я ведь знаю, столько он сделал для нас... Я просто в шоке!";
						link.l1 = "Дорогая, это все уже прошлое, нечего его будоражить...";
						link.l1.go = "NewLife_exit";
					break;
					case "YouAreNormal": 
						dialog.text = "Дорогой, я слышала, что ты отблагодарил своего друга Атиллу. Это достойный поступок, я рада, что мой муж - настоящий мужчина, человек чести!";
						link.l1 = "Изабелла, могло ли быть иначе?..";
						link.l1.go = "NewLife_exit";
					break;
					case "YouAreGood": 
						dialog.text = "Дорогой, ты мне ничего не говорил, но мне довелось узнать, что ты отдал своему другу Атилле очень большую сумму. Полагаю, это благодарность за все то, что он сделал для нас. Дорогой, ты самый лучший человек на свете! Я в этом убедилась очередной раз.";
						link.l1 = "Изабелла, Ну что ты... Не стоит право, я всего лишь вернул долг...";
						link.l1.go = "NewLife_exit";
					break;
				}
				DeleteAttribute(pchar, "RomanticQuest.Atilla");
				break;
			}
			switch (rand(1))
			{
				case 0: 
					dialog.text = LinkRandPhrase("Ах, дорогой, как мне приятно твое внимание...", pchar.name + ", ты настоящий рыцарь...", "Ах, Господи, какое счастье, что ты послал мне в мужья " + GetMainCharacterNameGen() + "...");
					link.l1 = RandPhraseSimple("Мне тоже повезло с тобой, дорогая...", "Знаешь, Изабелла, я часто думаю: как хорошо, что мы решили связать свои судьбы...");
					link.l1.go = "NewLife_exit";
				break;
				case 1: 
					dialog.text = LinkRandPhrase("Дорогой, а ты знаешь, губернатор города интересовался, как у нас дела.", "Губернатор спрашивал меня на днях, как у тебя дела, дорогой.", "Вчера я встретила губернатора на улице и мы поговорили. Он интересовался, как у тебя дела.");
					link.l1 = RandPhraseSimple("Да все в порядке, так ему и скажи в следующий раз.", "Не сказал бы, что мне льстит внимание губернатора к тебе...");
					link.l1.go = "NewLife_exit";
				break;
			}
		break;
		//=========================== Подарки =================================
		case "NewLife_Present":
			dialog.text = RandPhraseSimple("И что ты хочешь подарить мне, дорогой?", "Давай быстрее говори, что ты мне надумал подарить?");
			iTemp = 1;
			for (i=18; i>=1; i--)
			{
				if (i==8 || i==9) continue;
				if (CheckCharacterItem(pchar, "jewelry"+i))
				{
					sTemp = "l"+iTemp;
					link.(sTemp) = XI_ConvertString("jewelry"+i+"Gen") + ".";
					link.(sTemp).go = "NewLife_jewelry_"+i;
					iTemp++;				
				}
			}
			if (iTemp == 1)
			{
				dialog.text = NPCStringReactionRepeat("И что ты хочешь подарить мне, дорогой?", 
					"Опять что-то хочешь подарить? Хм, ну и что это?", 
					pchar.name + ", ты что, издеваешься с этими подарками? Что на этот раз?", 
					"Ну все, ты меня достал, тупица! Я сердита на тебя и не желаю с тобой разговаривать!", "repeat", 20, npchar, Dialog.CurrentNode);
				link.l1 = HeroStringReactionRepeat("Да ничего такого...", 
					"Ничего, достойного тебя у меня нет, извини...", 
					"Изабелла, я опять ничего не нашел для тебя, прости...", 
					"Изабелла, извини!..", npchar, Dialog.CurrentNode);
				link.l1.go = "NewLife_exit";
			}
		break;
		case "NewLife_jewelry_1":
			dialog.text = LinkRandPhrase("Спасибо, дорогой, ты очень любезен. Сапфиры мне очень нравятся.", "Ах, как он играет на солнце! Спасибо, милый..." , "Великолепный камень! Спасибо, дорогой...");
			link.l1 = LinkRandPhrase("Да не за что, дорогая...", "Я хочу, чтобы ты была самой красивой женщиной на свете!", "Все - для тебя, милая...");
			link.l1.go = "NewLife_exit";			
			TakeItemFromCharacter(pchar, "jewelry1");
		break;
		case "NewLife_jewelry_2":
			if (rand(4)==1)
			{
				dialog.text = "Ах, алмаз... Я просто с ума схожу! Милый, давай поднимемся наверх!";
				link.l1 = "С удовольствием, милая.";
				link.l1.go = "NewLife_GoSex";				
			}
			else
			{
				dialog.text = LinkRandPhrase("Спасибо, дорогой, ты очень любезен. Алмазы мне очень нравятся.", "Ах, как он играет на солнце! Спасибо, милый..." , "Великолепный камень! Спасибо, дорогой...");
				link.l1 = LinkRandPhrase("Да не за что, дорогая...", "Я хочу, чтобы ты была самой красивой женщиной на свете!", "Все - для тебя, милая...");
				link.l1.go = "NewLife_exit";			
			}
			TakeItemFromCharacter(pchar, "jewelry2");
		break;
		case "NewLife_jewelry_3":
			if (rand(10)==1)
			{
				dialog.text = "Ой, это же рубин! Я так люблю эти кроваво-красные камни! Дорогой, ты меня очень обрадовал, а теперь я хочу подняться с тобой наверх...";
				link.l1 = "И я тоже этого хочу, милая...";
				link.l1.go = "NewLife_GoSex";				
			}
			else
			{
				dialog.text = LinkRandPhrase("Спасибо, дорогой, ты очень любезен. Рубины мне очень нравятся.", "Ах, как он играет на солнце! Спасибо, милый..." , "Великолепный камень! Спасибо, дорогой...");
				link.l1 = LinkRandPhrase("Да не за что, дорогая...", "Я хочу, чтобы ты была самой красивой женщиной на свете!", "Все - для тебя, милая...");
				link.l1.go = "NewLife_exit";			
			}
			TakeItemFromCharacter(pchar, "jewelry3");
		break;
		case "NewLife_jewelry_4":
			if (rand(3)==1)
			{
				dialog.text = "Ах, это же мой любимый камень - изумруд! Ты меня балуешь, милый... Дорогой, я хочу подняться с тобой наверх...";
				link.l1 = "И я тоже этого хочу, милая...";
				link.l1.go = "NewLife_GoSex";				
			}
			else
			{
				dialog.text = LinkRandPhrase("Спасибо, дорогой, ты очень любезен. Изумруды мне очень нравятся.", "Ах, как он играет на солнце! Спасибо, милый..." , "Великолепный камень! Спасибо, дорогой...");
				link.l1 = LinkRandPhrase("Да не за что, дорогая...", "Я хочу, чтобы ты была самой красивой женщиной на свете!", "Все - для тебя, милая...");
				link.l1.go = "NewLife_exit";			
			}
			TakeItemFromCharacter(pchar, "jewelry4");
		break;
		case "NewLife_jewelry_5":
			dialog.text = LinkRandPhrase("М-м-м, золото! Очень хороший подарок, спасибо, дорогой.", "Ах, а какой он формы, этот самородок! Спасибо, дорогой." , "Великолепный самородок! Спасибо, дорогой...");
			link.l1 = LinkRandPhrase("Да не за что, дорогая...", "Я хочу, чтобы ты была самой богатой женщиной на Карибах!", "Все - для тебя, милая...");
			link.l1.go = "NewLife_exit";	
			TakeItemFromCharacter(pchar, "jewelry5");
		break;
		case "NewLife_jewelry_6":
			dialog.text = LinkRandPhrase("Спасибо, дорогой, ты очень любезен. Украшения с сапфирами мне очень идут.", "Ах, как хорошо сапфир смотрится в серебряной оправе! Спасибо, милый..." , "Великолепное кольцо! Спасибо, дорогой...");
			link.l1 = LinkRandPhrase("Да не за что, дорогая...", "Я хочу, чтобы ты была самой красивой женщиной на свете!", "Все - для тебя, милая...");
			link.l1.go = "NewLife_exit";			
			TakeItemFromCharacter(pchar, "jewelry6");
		break;
		case "NewLife_jewelry_7":
			if (rand(2)==1)
			{
				dialog.text = "Ах, кольцо с моим любимым изумрудом! Ты меня балуешь, милый... Дорогой, я хочу подняться с тобой наверх...";
				link.l1 = "И я тоже этого хочу, милая...";
				link.l1.go = "NewLife_GoSex";				
			}
			else
			{
				dialog.text = LinkRandPhrase("Спасибо, дорогой, ты очень любезен. Кольца с изумрудами мне очень к лицу...", "Ах, как он играет на солнце в золотой оправе! Спасибо, милый..." , "Великолепный камень в достойной оправе! Спасибо, дорогой...");
				link.l1 = LinkRandPhrase("Да не за что, дорогая...", "Я хочу, чтобы ты была самой красивой женщиной на свете!", "Все - для тебя, милая...");
				link.l1.go = "NewLife_exit";			
			}
			TakeItemFromCharacter(pchar, "jewelry7");
		break;
		case "NewLife_jewelry_10":
			dialog.text = LinkRandPhrase("Спасибо, дорогой, ты очень любезен. Украшения с сапфирами мне очень идут.", "Ах, как хорошо сапфир смотрится в золотой оправе! Спасибо, милый..." , "Великолепное кольцо! Спасибо, дорогой...");
			link.l1 = LinkRandPhrase("Да не за что, дорогая...", "Я хочу, чтобы ты была самой красивой женщиной на свете!", "Все - для тебя, милая...");
			link.l1.go = "NewLife_exit";
			TakeItemFromCharacter(pchar, "jewelry10");
		break;
		case "NewLife_jewelry_11":
			dialog.text = LinkRandPhrase("Спасибо, дорогой, ты очень любезен. Такая крупная жемчужина восхитительно смотрится!", "Жемчуг... Промыслы на жемчужном берегу работают исправно. Спасибо, дорогой." , "Великолепная жемчужина! Спасибо, дорогой...");
			link.l1 = LinkRandPhrase("Да не за что, дорогая...", "Я хочу, чтобы ты была самой красивой женщиной на свете!", "Все - для тебя, милая...");
			link.l1.go = "NewLife_exit";
			TakeItemFromCharacter(pchar, "jewelry11");
		break;
		case "NewLife_jewelry_12":
			dialog.text = LinkRandPhrase("Спасибо, дорогой, ты очень любезен...", "Жемчуг... Промыслы на жемчужном берегу работают исправно..." , "Великолепная жемчужина! Спасибо, дорогой...");
			link.l1 = RandPhraseSimple("Кхе...", "Гм...");
			link.l1.go = "NewLife_exit";
			TakeItemFromCharacter(pchar, "jewelry12");
		break;
		case "NewLife_jewelry_13":
			dialog.text = LinkRandPhrase("О-о-о, какая интересная безделушка, эта камея... Спасибо, дорогой.", "Интересная безделица, милый. Спасибо тебе..." , "Хорошее украшение! Спасибо, дорогой...");
			link.l1 = LinkRandPhrase("Да не за что, дорогая...", "Я хочу, чтобы ты была самой красивой женщиной на свете!", "Все - для тебя, милая...");
			link.l1.go = "NewLife_exit";
			TakeItemFromCharacter(pchar, "jewelry13");
		break;
		case "NewLife_jewelry_14":
			if (rand(1)==1)
			{
				dialog.text = "Ах, милый, эта брошь достойна королевы... Я хочу немедленно подняться с тобой наверх...";
				link.l1 = "И я тоже этого хочу...";
				link.l1.go = "NewLife_GoSex";				
			}
			else
			{
				dialog.text = LinkRandPhrase("Спасибо, дорогой, ты очень любезен. Эта золотая брошь очень красива...", "Ах, как она хорошо смотрится на ярком свете! Спасибо, милый..." , "Великолепные камни в достойной оправе! Спасибо, дорогой...");
				link.l1 = LinkRandPhrase("Да не за что, дорогая...", "Я хочу, чтобы ты была самой красивой женщиной на свете!", "Все - для тебя, милая...");
				link.l1.go = "NewLife_exit";			
			}
			TakeItemFromCharacter(pchar, "jewelry14");
		break;
		case "NewLife_jewelry_15":
			if (rand(1)==1)
			{
				dialog.text = "Ах, милый, эти подвески... Я просто без ума от тебя и хочу немедленно подняться с тобой наверх...";
				link.l1 = "Я не против, дорогая.";
				link.l1.go = "NewLife_GoSex";				
			}
			else
			{
				dialog.text = LinkRandPhrase("Спасибо, дорогой, ты очень любезен. Эта изумрудные подвески очень мне идут...", "Ах, как изумруды хорошо смотрится на ярком свете! Спасибо, милый..." , "Великолепные камни в достойной оправе! Спасибо, дорогой...");
				link.l1 = LinkRandPhrase("Да не за что, дорогая...", "Я хочу, чтобы ты была самой красивой женщиной на свете!", "Все - для тебя, милая...");
				link.l1.go = "NewLife_exit";			
			}
			TakeItemFromCharacter(pchar, "jewelry15");
		break;
		case "NewLife_jewelry_16":
			dialog.text = LinkRandPhrase("Дорогой, оставь это ожерелье себе, я не обижусь...", "Хм, и ты думаешь, я буду такое носить?" , "Ну и вкус у тебя, дорогой. Да ты просто мужлан, если предлагаешь мне такое надеть!");
			link.l1 = RandPhraseSimple("Да? А-а-а, ну ладно...", "Что-то я ошибся...");
			link.l1.go = "NewLife_exit";
		break;
		case "NewLife_jewelry_17":
			dialog.text = LinkRandPhrase("А-а-а, серебро! Ну что же, возьму, в хозяйстве пригодится...", "Серебро?.. Ну ладно, давай..." , "Серебряный самородок? Хм, серебро ценится в больших количествах. Но ладно, давай заберу...");
			link.l1 = RandPhraseSimple("Да не за что, дорогая...", "Я хочу, чтобы ты была самой богатой женщиной на Карибах!");
			link.l1.go = "NewLife_exit";	
			TakeItemFromCharacter(pchar, "jewelry17");
		break;
		case "NewLife_jewelry_18":
			if (rand(8)==1)
			{
				dialog.text = "Изумруды и рубины - мои любимые камни. Это колечко просто великолепно, ты меня балуешь, милый... Дорогой, я хочу подняться с тобой наверх...";
				link.l1 = "И я тоже этого хочу...";
				link.l1.go = "NewLife_GoSex";				
			}
			else
			{
				dialog.text = LinkRandPhrase("Спасибо, дорогой, ты очень любезен, это кольцо с рубином очень красиво...", "Очень хорошо смотрится рубин в золоте! Спасибо, милый..." , "Великолепный камень в достойной оправе! Спасибо, дорогой...");
				link.l1 = LinkRandPhrase("Да не за что, дорогая...", "Я хочу, чтобы ты была самой красивой женщиной на свете!", "Все - для тебя, милая...");
				link.l1.go = "NewLife_exit";			
			}
			TakeItemFromCharacter(pchar, "jewelry18");
		break;
		//выход
		case "NewLife_exit":
			NextDiag.TempNode = "NewLife";
			NextDiag.CurrentNode = NextDiag.TempNode;
			LAi_SetActorType(npchar);
			LAi_ActorGoToLocator(npchar, "goto", "goto"+(rand(3)+1), "Romantic_TalkInChurch_end", -1);
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
// ======================== блок нод angry ===============>>>>>>>>>>>>>>>
		//==> попытки дать денег
		case "AngryRepeat_Budget_Add":
            dialog.text = RandPhraseSimple("Я не желаю говорить с таким занудой! Разве что серебряное и золотое колечко с сапфирами растопят мое сердце...", "Не хочу разговаривать, я обиделась на тебя, зануду и скрягу! Вообще, я хочу серебряное и золотое кольцо с сапфирами...");
			link.l1 = "Эх, Изабелла...";
		    link.l1.go = "AngryExitAgain";
            if (CheckAttribute(npchar, "angry.terms")) //примиряемся 10 дней.
            {
                if (GetNpcQuestPastDayParam(npchar, "angry.terms") > sti(npchar.angry.terms))
                {
         			dialog.text = "Ну, хорошо, муженек, я согласна с тобой общаться. Но не будь таким скрягой! И тем более, не обещай ничего, когда денег нет!";
        			link.l1 = "Хорошо, Изабелла, я буду стараться...";
        			link.l1.go = NextDiag.TempNode;
        			CharacterDelAngry(npchar);
                }
				else //примирение через кольцо с сапфиром
				{
					if (CheckCharacterItem(pchar, "jewelry6") && CheckCharacterItem(pchar, "jewelry10"))
					{
         				dialog.text = "Ах, ты принес мне то, что я хотела - золотое и серебряное кольца с сапфиром! Хм, ты знаешь, как мне угодить... Ну хорошо, я готова с тобой общаться...";
        				link.l1 = "Уф-ф-ф, здорово!";
        				link.l1.go = NextDiag.TempNode;
        				CharacterDelAngry(npchar);
						TakeItemFromCharacter(pchar, "jewelry6");
						TakeItemFromCharacter(pchar, "jewelry10");
					}
				}
            }	
    	break;
		//==> вовремя не рассчитался с долгами
		case "AngryRepeat_Budget_Pay":
            dialog.text = RandPhraseSimple("Послушайте, муженек, вы не способны обеспечить существование жены, мне пришлось самой решать наши финансовые проблемы. Это никуда не годится!", "Подумать только, я сама должна решать все вопросы в семье! Ваша неспособность обеспечить семью меня поражает!");
			link.l1 = "Сеньора Изабелла, так получилось...";
		    link.l1.go = "AngryExitAgain";
            if (CheckAttribute(npchar, "angry.terms")) //примиряемся 30 дней.
            {
                if (GetNpcQuestPastDayParam(npchar, "angry.terms") > sti(npchar.angry.terms))
                {
         			dialog.text = "Ну хорошо, я согласна на примирение, но всю образовавшуюся задолженность тебе нужно погасить!";
        			link.l1 = "Хорошо, Изабелла, я сделаю все, что ты скажешь.";
        			link.l1.go = NextDiag.TempNode;
        			CharacterDelAngry(npchar);
                }
				else //примирение через изумруды
				{
					if (CheckCharacterItem(pchar, "jewelry4") && CheckCharacterItem(pchar, "jewelry7") && CheckCharacterItem(pchar, "jewelry15"))
					{
         				dialog.text = "Ах ты негодник! Ты принес мне украшения с изумрудами... Обожаю эти камни, они так мне нравятся!.. Ну хорошо, я согласна на примирение, но всю образовавшуюся задолженность тебе нужно погасить!";
        				link.l1 = "Хорошо, Изабелла, хорошо...";
        				link.l1.go = NextDiag.TempNode;
        				CharacterDelAngry(npchar);
						TakeItemFromCharacter(pchar, "jewelry4");
						TakeItemFromCharacter(pchar, "jewelry7");
						TakeItemFromCharacter(pchar, "jewelry15");
					}
				}
            }
    	break;
		//==> если попался на пиратстве
		case "AngryRepeat_KnowPirates":
            dialog.text = RandPhraseSimple("Вы нарушили свое слово, данное мне перед свадьбой! Я не желаю вас видеть!", "Ты же обещал, что с каперством покончено, а сам и не думал прекращать проливать кровь невинных!");
			link.l1 = "Изабелла, я не хотел, чтобы так вышло...";
		    link.l1.go = "AngryExitAgain";
            if (CheckAttribute(npchar, "angry.terms")) //примиряемся 80 дней.
            {
                if (GetNpcQuestPastDayParam(npchar, "angry.terms") > sti(npchar.angry.terms))
                {
         			dialog.text = "Вот что, супруг! Я готова к примирению, но ты должен впредь держать свое слово!";
        			link.l1 = "Хорошо, Изабелла, я сделаю все, что ты скажешь. Пиратства больше не будет, дорогая...";
        			link.l1.go = NextDiag.TempNode;
        			CharacterDelAngry(npchar); 
					IsabellaSetCurrentState("All");
                }
				else //примирение через алмаз, рубины и большую жемчужину
				{
					if (CheckCharacterItem(pchar, "jewelry1") && CheckCharacterItem(pchar, "jewelry2") && CheckCharacterItem(pchar, "jewelry3") && CheckCharacterItem(pchar, "jewelry18") && CheckCharacterItem(pchar, "jewelry11"))
					{
         				dialog.text = "Ох, " + pchar.name + ", ты знаешь, как сделать, чтобы я не сердилась... Но ты должен держать свое слово и покончить с каперством!";
        				link.l1 = "Ах, Изабелла, я пусть будет так. Я же не могу без тебя...";
        				link.l1.go = NextDiag.TempNode;
        				CharacterDelAngry(npchar);
						TakeItemFromCharacter(pchar, "jewelry1");
						TakeItemFromCharacter(pchar, "jewelry2");
						TakeItemFromCharacter(pchar, "jewelry3");
						TakeItemFromCharacter(pchar, "jewelry18");
						TakeItemFromCharacter(pchar, "jewelry11");
						IsabellaSetCurrentState("All");
					}
				}
			}
    	break;
		//==> если достал подарками
		case "AngryRepeat_Present":
            dialog.text = RandPhraseSimple("Нет, ну просто невозможно жить с таким человеком! И зачем нужно было задаривать меня несуществующим подарками?! Слов просто нет!!", "Не хочу разговаривать с таким непроходимым тупицей и нахалом! Это же надо - дарить жене несуществующие подарки! Ну, спасибо, дорогой!..");
			link.l1 = RandPhraseSimple("Изабелла, ты ко мне несправедлива...", "Что-то нашло на меня, Изабелла. Я не нарочно...");
		    link.l1.go = "AngryExitAgain";
            if (CheckAttribute(npchar, "angry.terms")) //примиряемся 20 дней.
            {
                if (GetNpcQuestPastDayParam(npchar, "angry.terms") > sti(npchar.angry.terms))
                {
         			dialog.text = "Ну хорошо, я готова с тобой помириться. Но ты не будь таким мужланом и не предлагай подарков, если у тебя их нет!";
        			link.l1 = "Хорошо, дорогая, так больше делать я не буду...";
        			link.l1.go = NextDiag.TempNode;
        			CharacterDelAngry(npchar);
                }
				else //примирение через злато-серебро
				{
					if (CheckCharacterItem(pchar, "jewelry5") && CheckCharacterItem(pchar, "jewelry17"))
					{
						if (sti(pchar.items.jewelry5) >= 3 && sti(pchar.items.jewelry17) >=5)
						{
         					dialog.text = "Ну надо же!.. Ты принес единственное, что я сейчас действительно хочу - золотые и серебряные слитки... Ну хорошо, я готова возобновить общение. Но только чтобы не было повторений этого издевательства с подарками!!";
        					link.l1 = "Хорошо, Изабелла, как скажешь...";
        					link.l1.go = NextDiag.TempNode;
        					CharacterDelAngry(npchar);
							TakeNItems(pchar, "jewelry5", -3);
							TakeNItems(pchar, "jewelry17", -5);
						}
					}
				}
			}
    	break;
		case "AngryExitAgain":
            DialogExit();
            DeleteAttribute(npchar, "angry.ok");
		break;

// <<<<<<<<<<<<============= блок нод angry =============================
	}
}
