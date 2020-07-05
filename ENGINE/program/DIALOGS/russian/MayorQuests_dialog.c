void ProcessDialogEvent()
{
	ref NPChar, sld;
	aref Link, Diag;
	int i;
	string sTemp;
	
	DeleteAttribute(&Dialog,"Links");

	makeref(NPChar,CharacterRef);
	makearef(Link, Dialog.Links);
	makearef(Diag, NPChar.Dialog);

	switch(Dialog.CurrentNode)
	{
		case "exit":
			Diag.CurrentNode = Diag.TempNode;
			DialogExit();			
		break;
		//----------------- уничтожение банды ----------------------
		case "DestroyGang_begin":
			chrDisableReloadToLocation = false;
			LAi_LocationFightDisable(&Locations[FindLocation(pchar.location)], false); //боевке можно
			DeleteAttribute(&locations[FindLocation(pchar.GenQuest.DestroyGang.Location)], "DisableEncounters"); //энкаунтеры можно 
			for(i = 1; i <= 3; i++)
			{
				if (GetCharacterIndex("MayorQuestGang_" + i) == -1) continue;
				sld = CharacterFromID("MayorQuestGang_" + i);
				LAi_type_actor_Reset(sld);
				LAi_RemoveCheckMinHP(sld);
			}
			sTemp = GetFullName(&characters[GetCharacterIndex(pchar.GenQuest.DestroyGang.MayorId)]);
			dialog.text = LinkRandPhrase("Деньги на бочку приятель! Меня зовут " + GetFullName(npchar) + ", и я не терплю возражений в свой адрес...",
				"Покажи-ка мне содержимое своего кошелька, приятель, и побыстрее! Меня зовут " + GetFullName(npchar) + ", и, я надеюсь, ты слышал обо мне...",
				"Выкладывай все ценное, что у тебя есть, я реквизирую содержимое твоего кошелька. И побыстрей, так как я, " + GetFullName(npchar) + ", не очень терпелив. Но зато очччень кровожаден!");
			Link.l1 = LinkRandPhrase("Хех, так ты и есть самый " + GetFullName(npchar) + ", о котором местный губернатор " + sTemp + " только и говорит?",
				"О-о, так ты и есть тот самый бандит, за которым охотится местный губернатор " + sTemp + " ?!", 
				"Рад видеть тебя, " + GetFullName(npchar) + ". Местный губернатор, " + sTemp + ", только о тебе и говорит...");
			Link.l1.go = "DestroyGang_1";
		break;		
		case "DestroyGang_1":
			dialog.text = LinkRandPhrase("Да, я известен в этих краях, хе-хе.. Стоп, а ты не очередной ли пес губернатора, которого он пытается натравить на меня?",
				"Губернатор - мой большой друг, чего скрывать? Ха, а не очередной ли ты герой, отправленный им за моей головой?",
				"'Дружеское' расположение губернатора ко мне - факт, хе-хе. А ты откуда об этом знаешь? Может, он послал тебя по мою душу?");
			Link.l1 = LinkRandPhrase("Точно так, мерзавец. Готовься к смерти!", "Надо же, какой ты догадливый. Ну что же, пора приступать к твоей ликвидации. Поговорили, хватит уже...", "Да, это я и есть. Доставай оружие, приятель! Посмотрим какого цвета у тебя кровь.");
			Link.l1.go = "DestroyGang_ExitFight";	
			Link.l2 = LinkRandPhrase("Да нет, что ты! Мне эти проблемы совершенно ни к чему...", "Нет, я за такие дела не берусь, не герой, в общем...", "Нет-нет, ни в коем разе! Мне проблемы ни к чему...");
			Link.l2.go = "DestroyGang_2";	
		break;
		case "DestroyGang_2":
			dialog.text = LinkRandPhrase("Так-то лучше, дружище... А теперь проваливай!",
				"И это правильное решение, знал бы ты, сколько героев я отправил на тот свет... Ладно, меньше лирики. Вали отсюда, мешок с дерьмом!",
				"Молодец, хвалю! Очень мудрая жизненная позиция - не лезть куда не просят... Ладно, проваливай, приятель.");
			Link.l1 = "До свидания, желаю здравствовать...";
			Link.l1.go = "DestroyGang_ExitAfraid";	
		break;

		case "DestroyGang_ExitAfraid":
			pchar.GenQuest.DestroyGang = "Found"; //флаг нашел, но струсил
			npchar.money = AddMoneyToCharacter(npchar, sti(pchar.money));
			pchar.money = 0;
			LAi_SetWarriorType(npchar);
			LAi_SetImmortal(npchar, false);
			LAi_SetCheckMinHP(npchar, LAi_GetCharacterHP(npchar)-1, false, "DestroyGang_SuddenAttack");
			for(i = 1; i < 4; i++)
			{
				if (GetCharacterIndex("MayorQuestGang_" + i) == -1) continue;	
				sld = CharacterFromID("MayorQuestGang_" + i);
				LAi_SetActorType(sld);
				LAi_ActorFollow(sld, npchar, "", -1);
				LAi_SetImmortal(sld, true);
				LAi_SetCheckMinHP(sld, LAi_GetCharacterHP(sld)-1, false, "DestroyGang_SuddenAttack");
			}
			DialogExit();
		break;

		case "DestroyGang_ExitFight":
			for(i = 0; i < 4; i++)
			{
				if (GetCharacterIndex("MayorQuestGang_" + i) == -1) continue;
				sld = CharacterFromID("MayorQuestGang_" + i);
				LAi_SetWarriorType(sld);
				LAi_group_MoveCharacter(sld, "EnemyFight");
			}
			LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
			LAi_group_FightGroups("EnemyFight", LAI_GROUP_PLAYER, true);
			LAi_group_SetCheck("EnemyFight", "DestroyGang_Afrer");
			DialogExit();	
			AddDialogExitQuest("MainHeroFightModeOn");
		break;
		//----------------- поиски лазутчика в коммонах ----------------------
		case "SeekSpy_house":
			chrDisableReloadToLocation = false;
			dialog.text = "Что вам нужно, " + GetAddress_Form(PChar) + "?";
			Link.l1 = "Мне нужно узнать, кто ты такой. Немедленно говори, кто ты и что ты здесь делаешь?";
			Link.l1.go = "SeekSpy_1";	
		break;
		case "SeekSpy_1":
			dialog.text = "Ну, а сам-то ты кто такой, чтобы я отвечал на твои вопросы?";
			Link.l1 = "Я действую по поручению губернатора, ищу вражеского лазутчика!";
			Link.l1.go = "SeekSpy_2";	
		break;
		case "SeekSpy_2":
			dialog.text = "Ну а я здесь при чем?";
			Link.l1 = "Может и не при чем, но давай-ка, для начала, пройдем в комендатуру. Там разберемся...";
			Link.l1.go = "SeekSpy_3";	
		break;
		case "SeekSpy_3":
			dialog.text = "Нет, в комендатуру я не пойду. Считай, что нашел лазутчика. Ну а теперь попробуй выйти отсюда живым.";
			Link.l1 = "Хех, да уж выйду как нибудь...";
			Link.l1.go = "SeekSpy_fight";	
		break;
		case "SeekSpy_fight":
			LAi_SetWarriorType(npchar);
			LAi_group_MoveCharacter(npchar, "EnemyFight");
			LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
			LAi_group_FightGroups("EnemyFight", LAI_GROUP_PLAYER, true);
			LAi_group_SetCheck("EnemyFight", "SeekSpy_Afrer");
			DialogExit();
			AddDialogExitQuest("MainHeroFightModeOn");
		break;
		//----------------- поиски лазутчика на улице ----------------------
		case "SeekSpy_street":
			Diag.TempNode = "SeekSpy_street";
            dialog.Text = NPCharRepPhrase(npchar,
		            PCharRepPhrase(LinkRandPhrase("А, это ты, "+GetFullName(Pchar)
                    +", старый пройдоха", "Рад тебя видеть старина "
                    + Pchar.name, "О, да это же сам капитан "
                        +GetFullName(Pchar))
                                    +LinkRandPhrase(". Думал тебя уже нет на этом свете!", ". И, наверное, как всегда пьян.", ". И, как вижу, еще на свободе!")
                                    +LinkRandPhrase(" Ну, говори чего тебе нужно?", " Чего тебе на этот раз?", " Опять тревожишь меня по пустякам?"),
                                    LinkRandPhrase(TimeGreeting() + ", капитан ", "Здравствуйте "+GetAddress_Form(NPChar)+" ", "Приветствую вас, капитан ")+GetFullName(Pchar)
                                    +LinkRandPhrase(". Что может быть нужно, такому почтенному человеку как вы от меня?", ". Зачем вы пришли?", ". Что на этот раз вы хотите узнать?")
                                    ),
                                    
                    PCharRepPhrase(LinkRandPhrase("Здравствуйте капитан ", "Приветствую, ах это вы "+GetAddress_Form(NPChar)+" ", "А, капитан ")
                                    +GetFullName(Pchar)
                                    +LinkRandPhrase(" я думал"+NPCharSexPhrase(NPChar, " ", "а ")+"мы больше с вами не встретимся,",
                                                        " не скажу, что рад"+NPCharSexPhrase(NPChar, " ", "а ")+"вас видеть, но", " вижу, вы еще живы, прискорбно, а")
                                    +LinkRandPhrase(" что вам нужно?", " зачем вы пришли?", " чем могу быть "+NPCharSexPhrase(NPChar, "полезен?", "полезна?")),
                                    LinkRandPhrase(TimeGreeting() + ", капитан " + GetFullName(Pchar) + ". Чем могу помочь?",
                                    "О, да это же сам капитан " + GetFullName(Pchar) + "! Что вы хотите узнать на этот раз?",
                                    "Здравствуйте, " + GetAddress_Form(NPChar) + " " + Pchar.lastname + ". Вы что-то хотели?"))
                                    );                  
                        
	   		link.l1 = PCharRepPhrase(RandPhraseSimple("Тебе послышалось, я пойду.", "Нет, ничего - просто шел в таверну."),
                                    RandPhraseSimple("Ладно, ничего. Удачи!", "Просто осматриваю город. До свидания."));
			link.l1.go = "exit";
			link.l2 = RandPhraseSimple("У меня есть вопрос к тебе.", "Мне нужна информация.");
			link.l2.go = "exit";//(перессылка в файл города)
			// --> квестовый генератор мэра, поиск шпиёна на улице. eddy
			if (CheckAttribute(pchar, "GenQuest.SeekSpy.City") && pchar.location == (npchar.city + "_town"))
	        {
				link.l4 = NPCharSexPhrase(npchar, LinkRandPhrase("Стойте! Немедленно скажите мне, кто вы такой?! Я ищу вражеского лазутчика по поручению губернатора " + XI_ConvertString("Colony"+characters[GetCharacterIndex(pchar.GenQuest.SeekSpy.MayorId)].city+"Gen") + "!", 
					"Немедленно стой! Я действую в интересах города " + XI_ConvertString("Colony"+characters[GetCharacterIndex(pchar.GenQuest.SeekSpy.MayorId)].city+"Gen") + " по поручению губернатора. Ищу городе вражеского агента.", 
					"Стоять, приятель! Местный губернатор " + characters[GetCharacterIndex(pchar.GenQuest.SeekSpy.MayorId)].lastname + " очень хочет разыскать шпиона в городе."), 
					LinkRandPhrase("Женщина, стойте! Немедленно скажите мне, кто вы такая?! Я ищу вражеского лазутчика по поручению губернатора " + XI_ConvertString("Colony"+characters[GetCharacterIndex(pchar.GenQuest.SeekSpy.MayorId)].city+"Gen") + "!", 
					"Женщина, я должен вас задержать! Я действую в интересах города " + XI_ConvertString("Colony"+characters[GetCharacterIndex(pchar.GenQuest.SeekSpy.MayorId)].city+"Gen") + " по поручению губернатора. Ищу городе вражеского агента.", 
					"Постой, красавица! Местный губернатор " + characters[GetCharacterIndex(pchar.GenQuest.SeekSpy.MayorId)].lastname + " очень хочет разыскать шпиона в городе..."));
				link.l4.go = "SitySpy";
			}
			// <-- квестовый генератор мэра, поиск шпиёна на улице
			link.l5 = PCharRepPhrase("Хотел кое-что узнать об этом городишке!", "Хотел узнать кое-что о городе.");
			link.l5.go = "new question";
		break;

		case "new question":
            dialog.text = NPCharRepPhrase(npchar,
		                PCharRepPhrase(LinkRandPhrase("О, да я рад"+NPCharSexPhrase(NPChar, " ", "а ")+" тебе как выпивке, спрашивай, чего хочешь.", "Любишь поболтать, капитан? Что ж, я тоже... Особенно за кружечкой рома.", "Да, " + PChar.name + "?"),
                                        LinkRandPhrase("Ну, чего тебе еще?", "Вижу, капитан "+PChar.name+", ты охотник поболтать? ", "Замучил вопросами, капитан, лучше бы угостил ромом.")),
		                PCharRepPhrase(LinkRandPhrase("Что вы хотели узнать "+ GetAddress_Form(NPChar) + "?", "Я вас слушаю капитан.", "Мне нужно идти, поэтому спрашивайте скорее капитан."),
                                        LinkRandPhrase("Всегда рад"+NPCharSexPhrase(NPChar, " ", "а ")+" приятному собеседнику, " + GetAddress_Form(NPChar) + " " + PChar.lastname + ". Говорите.",
                                                        "Да, " + GetAddress_Form(NPChar) + "?",
                                                        "Любите поболтать, капитан? Что ж, я тоже..."))
                            );

            // homo 25/06/06
			link.l1 = LinkRandPhrase ("Какие байки в ходу в здешней таверне?",
                                    "Что новенького в этих краях?",
                                    "Как течет жизнь на суше?");
			link.l1.go = "rumours_citizen";
			link.l2 = LinkRandPhrase("Не объясните мне дороги?", "Что-то я никак не найду дороги кое-куда...", "Подскажите дорогу...");
			link.l2.go = "exit";  //(перессылка в файл города)
			link.l3 = "Расскажите мне о вашей колонии.";
			link.l3.go = "colony";
            link.l4 = RandPhraseSimple("Есть важное дело!", "У меня к вам дело.");
			link.l4.go = "exit";  //(перессылка в файл города)
			link.l5 = PCharRepPhrase(RandPhraseSimple("Тебе послышалось, я пойду.", "Нет, ничего - просто шел в таверну."),
                                        RandPhraseSimple("Ладно, ничего. Удачи!", "Просто осматриваю город. До свидания."));
			link.l5.go = "exit";
		break;
		
		case "colony":
			dialog.text = LinkRandPhrase("Честно говоря, я не слишком-то разбираюсь в этом. Но кое-что могу рассказать.",
                           "Что же вас интересует в нашей колонии?", "Всегда пожалуйста. О чем вы хотите услышать?");
            switch(Rand(1))
			{
				case 0:
					link.l1 = "Что это за город?";
					link.l1.go = "colony_town";
				break;

				case 1:
					link.l1 = "Что вы знаете о форте, защищающем город?";
					link.l1.go = "fort";
				break;
            }
			link.l2 = "Поговорим о чем-нибудь другом.";
			link.l2.go = "new question";
		break;
	  // to_do
		case "colony_town":
            dialog.text = "О, да вы издеваетесь? Или шутите? Посмотрите в правый угол экрана.";
			link.l1 = "Точно, туплю.";
		    link.l1.go = "exit";
		break;
		
		case "fort":
            dialog.text = "Ничего про это не знаю.";
			link.l1 = "Жаль.";
		    link.l1.go = "exit";
		break;
		//==> поймал засланца
		case "SitySpy":
			dialog.text = "Ну а я здесь при чем? Я местный житель!";
			link.l1 = "Кто может подтвердить твои слова?";
		    link.l1.go = "SitySpy_1";
		break;
		case "SitySpy_1":
			dialog.text = "Все могут подтвердить, все!!",
			link.l1 = "Все - это кто? Назови имя хотя бы одного горожанина " + XI_ConvertString("Colony"+characters[GetCharacterIndex(pchar.GenQuest.SeekSpy.MayorId)].city+"Gen") + "!";
			link.l1.go = "SitySpy_3";
		break;
		case "SitySpy_3":
            dialog.text = "Эээ.. Сейчас... Ммм..";
			link.l1 = "Все ясно, приятель, тебя то я и ищу! Следуй за мной, иначе я прикончу тебя на месте. Тебе все понятно?!";
		    link.l1.go = "SitySpy_4";
		break;
		case "SitySpy_4":
            dialog.text = "Черт! Надо же было так глупо вляпаться!.. Все ясно, буду следовать за тобой - мне моя жизнь еще дорога...";
			link.l1 = "Ну вот и славно. Мы идем губернатору, он хотел тебя видеть...";
		    link.l1.go = "SitySpy_5";
		break;
		case "SitySpy_5":
			sTemp = GetFullName(&characters[GetCharacterIndex(pchar.GenQuest.SeekSpy.MayorId)]);
			AddQuestRecord("MayorsQuestsList", "5");
			AddQuestUserData("MayorsQuestsList", "ColonyName", XI_ConvertString("Colony"+characters[GetCharacterIndex(pchar.GenQuest.SeekSpy.MayorId)].city+"Gen"));
			AddQuestUserData("MayorsQuestsList", "MayorName", sTemp);
			// слухи
			AddSimpleRumour(RandPhraseSimple("Вы знаете, губернатор " + sTemp + " поручил некоему капитану " + GetMainCharacterNameGen() + " найти в городе вражеского лазутчика. И вы знаете, он его нашел прямо на улицах!", 
				"Наш губернатор зал задание капитану " + GetMainCharacterNameGen() + " разыскать в городе вражеского агента. И этот капитан нашел лазутчика среди горожан!"), sti(characters[sti(pchar.GenQuest.SeekSpy.MayorId)].nation), 5, 1);			
			pchar.quest.AllMayorsQuests_Late.over = "yes"; //снимаем общий таймер
			pchar.GenQuest.SeekSpy = "FoundSpy"; //флаг нашел шпиона
			DeleteAttribute(pchar, "GenQuest.SeekSpy.City"); //не опрашивать более в городе
			LocatorReloadEnterDisable(pchar.location, "gate_back", true);
			LocatorReloadEnterDisable(pchar.location, "reload1_back", true);
			LocatorReloadEnterDisable(pchar.location, "reload2_back", true);
			LAi_SetActorType(npchar);
			LAi_ActorFollowEverywhere(npchar, "", -1);
			DialogExit();
		break;
		//----------------- найти и уничтожить пирата в море ----------------------
		case "DestroyPirate_Abordage": //абордаж
			if (CheckAttribute(pchar, "GenQuest.DestroyPirate.wasTalkInDeck"))
			{
				dialog.text = "Ты все-таки решил вернуться...";
				Link.l1 = "Да, забыл кое-что...";
				Link.l1.go = "DP_Abordage_1";
			}
			else
			{
				dialog.text = "Кто ты такой, черт тебя дери?!";
				Link.l1 = "Я пришел за твоей головой, приятель. Губернатор " + XI_ConvertString("Colony"+pchar.GenQuest.DestroyPirate.MayorId+"Gen") + " хочет видеть ее прибитой к городским воротам.";
				Link.l1.go = "DP_Abordage_2";
			}
		break;
		case "DP_Abordage_1":
			dialog.text = "Дьявол, недооценил я тебя сразу.\nНу что, продолжим, пожалуй?..";
			Link.l1 = "Продолжим, приятель!";
			Link.l1.go = "DP_Abordage_fight";	
		break;
		case "DP_Abordage_2":
			dialog.text = "Хех, понятно... Ну, я уж постараюсь не доставить ему такого удовольствия!";
			Link.l1 = "Постарайся...";
			Link.l1.go = "DP_Abordage_fight";	
		break;
 		case "DP_Abordage_fight":
			LAi_SetCurHP(npchar, rand(sti(npchar.chr_ai.hp_max)) + 5);
			LAi_GetCharacterMaxEnergy(npchar);
			QuestAboardCabinDialogExitWithBattle("");
			DialogExit();
		break;
		//разговор на палубе
		case "DestroyPirate_Deck":
			pchar.GenQuest.DestroyPirate.wasTalkInDeck = true; //флаг на палубный базар
			dialog.text = "Рад видеть тебя на моем корабле, приятель. Зачем пожаловал?";
			Link.l1 = "Я действую по поручению губернатора " + XI_ConvertString("Colony"+pchar.GenQuest.DestroyPirate.MayorId+"Gen") + ", разыскиваю пирата, орудующего в здешних водах в последнее время.";
			Link.l1.go = "DestroyPirate_1";	
		break;
		case "DestroyPirate_1":
			dialog.text = "Не могу сказать, что это достойное занятие для капитана...";
			Link.l1 = "Что это значит?!";
			Link.l1.go = "DestroyPirate_2";	
		break;
		case "DestroyPirate_2":
			dialog.text = "Ровно то, что я сейчас тебе говорю. Ловить свободных корсаров, чтобы потом местный губернатор увешал нашими трупами окрестности города - занятие недостойное капитана корабля.";
			Link.l1 = "Так вы и есть те самые пираты...";
			Link.l1.go = "DestroyPirate_3";	
		break;
		case "DestroyPirate_3":
			dialog.text = "Именно так, губернаторская шестерка.\nДа уж, положеньице у тебя незавидное - один, на моем корабле...";
			Link.l1 = "Ну, и что ты собираешься делать дальше?";
			Link.l1.go = "DestroyPirate_4";	
		break;
		case "DestroyPirate_4":
			int lngFileID;
			if (pchar.GenQuest.DestroyPirate.fortPlace == "1")
			{
				pchar.GenQuest.DestroyPirate.outShoreId = GetIslandRandomShoreId(pchar.GenQuest.DestroyPirate.Areal);
				sld = characterFromId(pchar.GenQuest.DestroyPirate.MayorId);
				lngFileID = LanguageOpenFile("LocLables.txt");
				sTemp = LanguageConvertString(lngFileID, pchar.GenQuest.DestroyPirate.outShoreId);
				dialog.text = "Хм, дальше... Я слишком близко подошел к форту, поэтому не буду рисковать и придержу тебя на время здесь. Своим ты скажешь, что пойдешь на моем корабле до места, под названием " + sTemp + ", а они пусть ждут тебя в " + XI_ConvertString("Colony"+sld.city+"Dat") + 
					". В этом случае я оставлю тебе жизнь. Иначе тебе вспорят брюхо прямо здесь, на палубе.\nКак перспективы на будущее?";
				Link.l1 = "Не скажу, что заманчивые. Откуда мне знать, что ты сдержишь слово, когда форт тебя не достанет?";
				Link.l1.go = "DestroyPirate_5";					
				break;
			}
			if (GetCompanionQuantity(pchar) == 1 && makeint(pchar.GenQuest.DestroyPirate.shipState) > 1)
			{
				dialog.text = "Пожалуй, я отпущу тебя, губернаторский пес. Заплатишь за мое великодушие своей наличностью.\nВ общем, проваливай, пока я добрый...";
				Link.l1 = "Спасибо и на этом. Прощай...";
				Link.l1.go = "DP_WithoutFight";	
				Link.l2 = "Знаешь что, я пожалуй перережу тебе глотку лучше, пока вся твоя команда не сбежалась...";
				Link.l2.go = "DP_FightInDeck";	
			}
			else
			{
				pchar.GenQuest.DestroyPirate.outShoreId = GetIslandRandomShoreId(pchar.GenQuest.DestroyPirate.Areal);
				sld = characterFromId(pchar.GenQuest.DestroyPirate.MayorId);
				lngFileID = LanguageOpenFile("LocLables.txt");
				sTemp = LanguageConvertString(lngFileID, pchar.GenQuest.DestroyPirate.outShoreId);
				dialog.text = "А дальше ты скажешь своим, что пойдешь на моем корабле до места, под названием " + sTemp + ", а они пусть ждут тебя в " + XI_ConvertString("Colony"+sld.city+"Dat") + 
					". В этом случае я оставлю тебе жизнь. Иначе тебе вспорят брюхо прямо здесь, на палубе.\nКак перспективы на будущее?";
				Link.l1 = "Не скажу, что заманчивые. Откуда мне знать, что ты сдержишь слово, когда я буду совсем один?";
				Link.l1.go = "DestroyPirate_5";	
			}
		break;
		case "DestroyPirate_5":
			dialog.text = "Хех, знать ты не можешь, но у тебя будет мое слово. А так тебя ждет смерть прямо сейчас. Мне кажется, выбор очевиден...";
			Link.l1 = "Хм, ну что же, я согласен. Надеюсь, ты сдержись слово...";
			Link.l1.go = "DP_toShore";	
			Link.l2 = "По мне, так оно и есть. И выбор этот - дать бой, здесь и прямо сейчас. К оружию, мерзавцы!";
			Link.l2.go = "DP_FightInDeck";	
		break;

		case "DP_toShore":
			dialog.text = "Сдержу, не бойся.";
			Link.l1 = "Тогда я готов. За дело.";
			Link.l1.go = "exit";
			AddDialogExitQuest("DestroyPirate_toShore");
		break;

		case "DP_WithoutFight":
			dialog.text = "Еще раз увижу тебя - будешь болтаться на рее, имей в виду...";
			Link.l1 = "Понятно...";
			Link.l1.go = "exit";
			Diag.TempNode = "DP_WithoutFight_again";
			AddDialogExitQuest("DestroyPirate_WithoutFight");
		break;
		case "DP_WithoutFight_again":
			dialog.text = "Ты еще здесь? Я бы на твоем месте не стал здесь задерживаться...";
			Link.l1 = "Ухожу.";
			Link.l1.go = "exit";
			Diag.TempNode = "DP_WithoutFight_again";
		break;

		case "DP_FightInDeck":
			dialog.text = "Надо же, какой храбрец! Ценю смелость, жаль будет тебя убивать...\nЭй, на палубе, валите этого выскочку аккуратно, я пока подготовлю корабль к бою. Только без лишнего шума!";
			Link.l1 = "Жаль, что ты нас покидаешь...";
			Link.l1.go = "DP_FightInDeck_1";
		break;
		case "DP_FightInDeck_1":
			pchar.GenQuest.DestroyPirate.FightAfterDeck = true; //после резни на палубе - драка в море
			float locx, locy, locz;
			GetCharacterPos(pchar, &locx, &locy, &locz);
			LAi_SetActorType(npchar);
			LAi_ActorRunToLocation(npchar, "reload", LAi_FindNearestFreeLocator("reload", locx, locy, locz), "none", "", "", "DestroyPirate_FightInDeck", 3.5);
			DialogExit();			
		break;


	}
}
