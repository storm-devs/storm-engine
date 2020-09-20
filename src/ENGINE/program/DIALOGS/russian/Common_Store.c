// boal 08/04/04 общий диалог торговцев
#include "DIALOGS\russian\Rumours\Common_rumours.c"  //homo 25/06/06

void ProcessDialogEvent()
{
	ref NPChar;
	aref Link, NextDiag;

	DeleteAttribute(&Dialog,"Links");

	makeref(NPChar,CharacterRef);
	makearef(Link, Dialog.Links);
	makearef(NextDiag, NPChar.Dialog);
	

	string iDay, iMonth, sTemp, sMoney;
	
	iDay = environment.date.day;
	iMonth = environment.date.month;
	string lastspeak_date = iday + " " + iMonth;
	
	int iMoney;
	int iQuantityGoods;
	int iTradeGoods;
	int iTmp;
	
    bool  ok;

	int iTest;
    iTest = FindColony(NPChar.City); // город магазина
    ref rColony;
	if (iTest != -1)
	{
		rColony = GetColonyByIndex(iTest);
	}
	
    int iSeaGoods = LanguageOpenFile("ShipEatGood.txt"); // нужно заменить на GetGoodsNameAlt(idx)

    if (!CheckAttribute(npchar, "quest.item_date"))
    {
        npchar.quest.item_date = "";
    }
    if (!CheckAttribute(npchar, "quest.trade_date"))
    {
        npchar.quest.trade_date = "";
    }
    
    // вызов диалога по городам -->
    NPChar.FileDialog2 = "DIALOGS\" + LanguageGetLanguage() + "\Store\" + NPChar.City + "_Store.c";
    if (LoadSegment(NPChar.FileDialog2))
	{
        ProcessCommonDialog(NPChar, Link, NextDiag);
		UnloadSegment(NPChar.FileDialog2);
	}
    // вызов диалога по городам <--
    ProcessCommonDialogRumors(NPChar, Link, NextDiag);//homo 25/06/06
	switch(Dialog.CurrentNode)
	{
		case "First time":
			if (LAi_grp_playeralarm > 0)
			{
       			dialog.text = NPCharRepPhrase(pchar, 
					LinkRandPhrase("В городе поднята тревога, тебя всюду ищут! На твоем месте я бы не стал здесь задерживаться.", "Вся городская стража рыщет по городу в поисках тебя. Я не идиот и разговаривать с тобой не буду!", "Беги, приятель, пока солдаты не сделали из тебя решето..."), 
					LinkRandPhrase("Что тебе нужно, негодяй?! Городская стража уже взяла твой след, далеко тебе не уйти, грязный пират!", "Грязный убийца, вон из моего дома! Стража!!", "Я не боюсь тебя, мерзавец! Скоро тебя повесят в нашем форте, далеко тебе не уйти..."));
				link.l1 = NPCharRepPhrase(pchar,
					RandPhraseSimple("Хех, тревога для меня не проблема...", "Им меня ни за что не поймать."), 
					RandPhraseSimple("Заткни свою пасть, " + GetWorkTypeOfMan(npchar, "") + ", а не то вырву твой поганый язык!", "Хех, " + GetWorkTypeOfMan(npchar, "") + ", а все туда же - пиратов ловить! Вот что я тебе скажу, приятель: сиди тихо и будешь жить..."));
				link.l1.go = "fight";
				break;
			}
			//homo Линейка Блада
            if (Pchar.questTemp.CapBloodLine == true )
            {
                dialog.Text = LinkRandPhrase("Эй, доктор Блад! " + TimeGreeting() + ".",
                                    "Рад видеть Вас, Питер Блад.",
                                    "Хорошо, что Вы заглянули ко мне, " + GetFullName(pchar) + ". Как поживает полковник Бишоп?");
                Link.l1 = "Увы, я уже ухожу, " + NPChar.name + ". До встречи.";
				Link.l1.go = "exit";
				NextDiag.TempNode = "First time";
				break;
            }
            dialog.text = NPCharRepPhrase(npchar, "Новый клиент - новое золото. "
							+ LinkRandPhrase("Деньги не имеют запаха. ", "Пиастры не пахнут. ","Одной добродетелью сыт не будешь! ")+
							+GetFullName(npchar)+  " к вашим услугам!",
			                + LinkRandPhrase("Приятно познакомится с новым клиентом, ", "Рад новому клиенту, ", "Добро пожаловать, ")
							+ GetAddress_Form(NPChar)+ ". Мое имя "  + GetFullName(npchar)+
							+ RandPhraseSimple(", мой магазин к вашим услугам.",", я полностью в вашем распоряжении."));
			link.l1 = "Позвольте представиться. " +RandPhraseSimple("Меня зовут ","Я - ")+ "капитан "+ GetFullName(pchar)+".";
			link.l1.go = "node_1";
			NextDiag.TempNode = "Second time";
		break;

		case "second time":
			if (LAi_group_GetPlayerAlarm() > 0)
			{
       			dialog.text = NPCharRepPhrase(pchar, 
					LinkRandPhrase("В городе поднята тревога, тебя всюду ищут! На твоем месте я бы не стал здесь задерживаться.", "Вся городская стража рыщет по городу в поисках тебя. Я не идиот и разговаривать с тобой не буду!", "Беги, приятель, пока солдаты не сделали из тебя решето..."), 
					LinkRandPhrase("Что тебе нужно, негодяй?! Городская стража уже взяла твой след, далеко тебе не уйти, грязный пират!", "Грязный убийца, вон из моего дома! Стража!!", "Я не боюсь тебя, мерзавец! Скоро тебя повесят в нашем форте, далеко тебе не уйти..."));
				link.l1 = NPCharRepPhrase(pchar,
					RandPhraseSimple("Хех, тревога для меня не проблема...", "Им меня ни за что не поймать."), 
					RandPhraseSimple("Заткни свою пасть, " + GetWorkTypeOfMan(npchar, "") + ", а не то вырву твой поганый язык!", "Хех, " + GetWorkTypeOfMan(npchar, "") + ", а все туда же - пиратов ловить! Вот что я тебе скажу, приятель: сиди тихо и будешь жить..."));
				link.l1.go = "fight";
				break;
			}
            /*if (GetNationRelation2MainCharacter(sti(NPChar.nation)) == RELATION_ENEMY && sti(NPChar.nation) != PIRATE)
			{
				dialog.text = NPCharRepPhrase(npchar,
                     		  LinkRandPhrase("Разрази меня гром! ", "Дьявол!!! ", "Тысяча чертей!!! ")
							+ LinkRandPhrase("Неприятель ","Противник ", "Проклятый шпион ")+ LinkRandPhrase("в городе! ","прямо по курсу! ", "в кубрике! ") + RandPhraseSimple("Стража!", "Солдаты!"),
							  LinkRandPhrase("Пресвятая дева","Господи Иисусе", "Святой Себастьян")+ "! "+ LinkRandPhrase("Караул! " ,"Помогите! ","Да вы шпион! ")
							+ LinkRandPhrase("В городе враги! ","Лазутчики в городе! ","Предательство! ")+"Солдаты, сюда!!");
				link.l1 = pcharrepphrase(LinkRandPhrase("Закрой пасть!","Тишина на палубе!","Молчать!")+ " Или я "
											  + LinkRandPhrase("вырву твой язык и скормлю его псам, ", "вышибу тебе мозги, ","удушу тебя твоими же кишками, ")+ LinkRandPhrase("каналья!", "торгаш!", "мерзавец!"),
							                    LinkRandPhrase("Ради бога, ", "Прошу вас, ", "Немедленно ")+LinkRandPhrase("закройте рот!", "прекратите верещать, как свинья на бойне!", "заткнитесь, наконец!")
											  + LinkRandPhrase(" Или я не дам за вашу жизнь и фартинга!"," Меня уже здесь нет! И никогда не было! Вы поняли?"," Или мне придется вас убить на месте."));
				link.l1.go = "fight";
				break;
			}*/
   			// проверка награды
			/*if (sti(NPChar.nation) != PIRATE && ChangeCharacterHunterScore(Pchar, NationShortName(sti(NPChar.nation)) + "hunter", 0) >= 40)
			{
				dialog.text = NPCharRepPhrase(npchar,
                     LinkRandPhrase("Разрази меня гром! ", "Дьявол!!! ", "Тысяча чертей!!! ")+GetFullName(pchar)+ ", по прозвищу "
							+ LinkRandPhrase("'Палач', ","'Окорок', ", "'Потрошитель', ") +LinkRandPhrase(" в городе! "," прямо по курсу! ", " с правого борта! ") + RandPhraseSimple("Стража!", "Солдаты!"),
							  LinkRandPhrase("Пресвятая дева","Господи Иисусе", "Святой Себастьян")+ "! Это сам "+GetFullName(pchar)+ LinkRandPhrase("! Ловите " ,"! Хватайте ","! Держите ")
							+ LinkRandPhrase("висельника! ","вора и убийцу! ","беглого каторжника! ")+"Солдаты, сюда!!");
				link.l1 =      pcharrepphrase(LinkRandPhrase("Закрой пасть!","Тишина на палубе!","Молчать!")+ " А не то я"
											  + LinkRandPhrase(" вырву твой язык, каналья!", " выбью из тебя мозги!"," тебе кишки выпущу!"),
							                    LinkRandPhrase("Ради бога замолчите!", "Прошу вас! Не кричите!", "К чему весь этот шум?")
											  + LinkRandPhrase(" Я уже ухожу!"," Меня уже здесь нет!"," Я не хочу вас убивать!"));
				link.l1.go = "fight";
				break;
			}    */
			dialog.text = NPCharRepPhrase(npchar,
			                 pcharrepphrase(LinkRandPhrase("Капитан ", "Да это сам ", "Кого я вижу? Это же ") +GetFullName(pchar)+"! "
							                 + LinkRandPhrase("Тебя еще не вздернули на рее? Ха-ха!","Говорят, ты был в гостях у морского дьявола!", "А я слышал, тебя повесили в Порт-Рояле.")
											  + RandPhraseSimple(" Рад тебя видеть!", " Добро пожаловать!"),
							                LinkRandPhrase("Здравствуйте, ", "Добро пожаловать, ", "Добрый день, ") + "капитан"
											+ RandPhraseSimple(". Вам понравился ",". Приглянулся ли вам ")+" наш"+ RandPhraseSimple(" остров? "," городок? ")
											+ LinkRandPhrase("Тут совсем не плохо,","Отличное место,", "Здесь красивые девушки,")+RandPhraseSimple(" не правда ли?", " не так ли?")),
							 pcharrepphrase(LinkRandPhrase("Капитан ","Ах, это опять вы, ","Вы меня напугали, ") + GetFullName(pchar)
							 				+ LinkRandPhrase(". Вашим именем матери уже стращают непослушных детей!",". Я надеюсь, у вас нет трений с законом?",". Ваша репутация оставляет желать лучшего.")
											 +LinkRandPhrase(" Чему обязан за ваш визит?"," Что вам угодно?", " Зачем вы пришли?"),
							                "Добро пожаловать, капитан " +GetFullName(pchar)+ LinkRandPhrase("! Немного штормит сегодня",
											"! Похоже, собирается шторм","! Поднимается сильный ветер") +", не правда ли?"));

			link.l1 = NPCharRepPhrase(npchar,
			            pcharrepphrase(LinkRandPhrase("Ты все еще жив, ", "И я тебе рад, ","Мы с тобой знали времена и получше, ")
									  +LinkRandPhrase("каналья!","дружище!","морской волк!") +" А я думал, "
									  +LinkRandPhrase("твоя жена уже сгрызла тебя с элем!","тебя сожрали термиты!", "твоя старуха сжила тебя со свету!"),
						               LinkRandPhrase("Добрый день, ","Доброго здоровья, ","Привет, ") + GetAddress_FormToNPC(NPChar)+" " +GetFullName(Npchar)
									   +RandPhraseSimple(". Вы правы, ",". Совершенно согласен, ")
									   +LinkRandPhrase("а местные мулатки просто очаровательны", "с этим трудно спорить", "а здешние дамы вскружат голову кому угодно")+ "."),
						pcharrepphrase(RandPhraseSimple("К черту такие разговоры, ","Поворачивай на другой галс, ") +GetFullName(Npchar)+ "! "
						              +LinkRandPhrase("Болтаешь, как попугай!", "И не морочь мне голову!","Твои слова не дороже сухаря!")
						              +RandPhraseSimple(" Я пришёл в магазин!"," А зачем ты здесь сидишь?"),
						               "Добрый день, " + GetAddress_FormToNPC(NPChar)+ "! Да, но вчера " + LinkRandPhrase("был полный штиль.",
									    "была отличная погода.", "не было ни ветерка.")));
			link.l1.go = "node_1";
			NextDiag.TempNode = "Second time";
		break;

		case "node_1":
			dialog.text = NPCStringReactionRepeat(NPCharRepPhrase(npchar,
			                                         pcharrepphrase(RandPhraseSimple("Свистать всех наверх! ","Трюм к осмотру! ") +LinkRandPhrase("Что нужно джентельмену удачи в моем магазине, хе-хе?",
													                                 "Какой приятный сюрприз для бедного старого торговца! Хе-хе!", "Каким попутным ветром тебя занесло в мой магазин?"),
            														LinkRandPhrase("Открыть порты!", "Крючья к борту!","Держать прямо по курсу!")+RandPhraseSimple(" У меня вы найдете"," В моем магазине")
																	+LinkRandPhrase(" самые лучшие цены на острове."," прекрасный выбор товаров."," товары самого высшего качества.")
																	+RandPhraseSimple(" Я к вашим услугам"," Все, что пожелаете")+", капитан!"),
													 pcharrepphrase(RandPhraseSimple("Любые товары для джентльменов удачи! Хе-хе! Пока их не повесили сушиться на солнышке!",
													                                 "Ветер качает повешенных в порту и разносит звон цепей. Но пиастры не пахнут, верно?!"),
                 												"Желаете " +LinkRandPhrase("купить лучшие товары " + NationNameGenitive(sti(NPChar.nation)),"купить или продать груз", "получить выгодный фрахт")
																	+RandPhraseSimple("? Я полностью в вашем распоряжении!","? Мой магазин к вашим услугам!")
																	+RandPhraseSimple(" Рад вас видеть"," Располагайтесь") +", капитан!")),
				                                  NPCharRepPhrase(npchar,
												  	 pcharrepphrase("Мой тебе совет, кэп: не жалей денег на картечь. Обыскивать проще мертвых!",
													                "Я с первого взгляда увидел, что ты ловкий малый. " +RandPhraseSimple("Но теперь я вижу, что ты прямо герой.", "Ты молод, но "
																	+RandPhraseSimple("ум у тебя острый.", "тебя не обманешь."))),
													 pcharrepphrase("Я готов покупать у вас весь груз с захваченных призов. Хотите купить золото или серебро? Оно всегда в цене!",
													                "Торговое мореплавание - выгодный бизнес, капитан " + GetFullName(pchar)+", не так ли?")),
												  NPCharRepPhrase(npchar,
												     RandPhraseSimple("Все семьдесят пять не вернулись домой! Они потонули в пучине морской! " ,
													   RandPhraseSimple("Пятнадцать человек на сундук мертвеца!","Пей, и дьявол тебя доведет до конца!") +" Йохо-хо! И бутылка рома. ")
													   +RandPhraseSimple("Чего надо, кэп?", "Что вы хотите купить, капитан?"),
													 pcharrepphrase("А вы любите поторговаться, капитан " + GetFullName(pchar)+". Как постоянному клиенту, я предлагаю вам очень выгодные цены!",
													                "Ну хорошо, капитан " + GetFullName(pchar)+", из уважения к вам я готов еще немного уступить в цене! Что вы хотите?")),
												  NPCharRepPhrase(npchar,
												     "Клянусь, мне тошно разговаривать с тобой. "
													 +LinkRandPhrase("Купи два-три земляных ореха. Их так любят свиньи. ","Ты ждешь, что здесь тебе сапоги будут чистить ромом? ",
													 pcharrepphrase("Знаю я вашего брата. Налакаетесь рому - и на виселицу.","Как это твоя мамаша отпустила тебя в море! В море!!! Уж лучше бы ты стал портным...")),
													 pcharrepphrase(RandPhraseSimple("Я полагал, ","Я надеялся, ")+RandPhraseSimple("что вы покинули наш остров.","что больше никогда вас не увижу.")
													 +RandPhraseSimple(" Мертвецы висят у вас на шее, как мельничные жернова..."," Скольких моряков вы отправили гнить среди кораллов?!"),
													 "Капитан " + GetFullName(pchar)+", ваша страсть к коммерции превосходит все мои ожидания!"
													 +RandPhraseSimple(" Хотите сделать последние покупки перед отплытием?", " Вы хотите купить что-то особенное?"))),
										"cycle", 10, npchar, Dialog.CurrentNode);
			link.l1 = HeroStringReactionRepeat(NPCharRepPhrase(npchar,
			                                        pcharrepphrase("Давай, показывай, что у тебя есть, старый пират!",
													               "Я всего лишь хочу посмотреть на ваши товары, любезный."),
													pcharrepphrase("Клянусь виселицей, " +GetFullName(npchar)+", я не позволю тебе водить меня за нос! Живо показывай свои товары!",
													               "Давайте посмотрим ваши товары, " +GetAddress_FormToNPC(NPChar)+ ".")),
											   NPCharRepPhrase(npchar,
											        pcharrepphrase("Это точно! Мертвые не кусаются! Ну, что у тебя в трюме есть?",
													               "Вы, милейший, лучше займитесь своим делом. Какие товары можете мне предложить?"),
												    pcharrepphrase("Однажды и ты станешь моим призом, хе-хе. Шучу. Давай свои товары.",
													               "Давайте обсудим торговые дела, " +GetAddress_FormToNPC(NPChar)+ " " +GetFullName(npchar)+ "")),
											   NPCharRepPhrase(npchar,
											        pcharrepphrase("Что мне надо? Хочу купить кое-что!",
													               "Пьянство вас сведет в могилу, " +GetFullName(npchar)+ ". Мне нужно посмотреть ваши товары."),
												    pcharrepphrase("Знаю я твои скидки! Два фунта какао по цене трех! Живо показывай свои товары",
													               "Это всегда приятно. Давайте поторгуемся.")),
											  "Показывай товары, меньше слов и больше дела. Я тороплюсь.",
											  npchar, Dialog.CurrentNode);
			link.l1.go = "market";
			link.l2 = HeroStringReactionRepeat(NPCharRepPhrase(npchar,
			                                        pcharrepphrase("Эх, так бы и прирезал кого-нибудь! Ладно, давай к делу.",
													               "Перейдем к другим делам, милейший"),
													pcharrepphrase("Каррамба! Я совсем по другому делу!",
													               "Мне нужно с вами поговорить по делу, "+GetAddress_FormToNPC(NPChar)+" " +GetFullName(npchar)+".")),
											   NPCharRepPhrase(npchar,
											        pcharrepphrase("У меня к тебе дело, старый пират!",
													               "Перейдем к делам, милейший!"),
													pcharrepphrase("Клянусь дьяволом, тебе меня не обмануть, мошенник! У меня другое дело!",
													               "Полностью согласен, " +GetAddress_FormToNPC(NPChar)+ " " +GetFullName(npchar)+ ", я хочу обсудить с вами иные дела.")),
											   NPCharRepPhrase(npchar,
											        pcharrepphrase("Уже надрался, приятель?! У меня к тебе дело.",
													               "Я пришел не торговать, любезный. У меня другое дело."),
													pcharrepphrase("Разрази тебя гром с твоими ценами, негодяй! У меня дело к тебе.",
													               "Скидки всегда приятны, " + GetAddress_FormToNPC(NPChar)+ " " +GetFullName(npchar)+ ", но я по другому делу.")),
											   NPCharRepPhrase(npchar,
											        pcharrepphrase("Придержи язык, старый пьяница, у меня дело к тебе",
													               "Тошнит вас от рома, я полагаю. Впрочем, я от вас тоже не в восторге. Перейдем к делу."),
													pcharrepphrase("Ты и так уже обобрал меня до нитки, мошенник! У меня к тебе дело.",
													               "Ваша ирония неуместна, у меня к вам дело иного порядка")),
											   npchar, Dialog.CurrentNode);
			link.l2.go = "business";
			link.l3 = HeroStringReactionRepeat(NPCharRepPhrase(npchar,
			                                        pcharrepphrase("Сначала дело, потом веселье. Хочу у тебя узнать кое-что.",
													               "Не сейчас, мне нужна информация."),
													pcharrepphrase("К черту цены! Сначала ты скажешь все, что мне нужно.",
													               "С удовольствием. Я хочу узнать кое-что.")),
											   NPCharRepPhrase(npchar,
											        pcharrepphrase("Спасибо за совет, старый убийца. Мне надо кое-что узнать.",
													               "У меня определенный вопрос."),
												    pcharrepphrase("Золото подождет. Я хочу узнать кое-что.",
													               "Да, я согласен. Хочу вас спросить кое о чем.")),
											   NPCharRepPhrase(npchar,
											        pcharrepphrase("Я думаю, ты знаешь, о чем я хочу спросить.",
													               "Я надеюсь, вы ответите мне на мой вопрос."),
													pcharrepphrase("У тебя в голове есть что-нибудь кроме ценников? Об этом я и хочу спросить.",
													               "Речь не об этом. Мне нужна кое-какая информация.")),
											   NPCharRepPhrase(npchar,
											        pcharrepphrase("Не кипятись, старина! Я просто хочу спросить тебя.",
													               "Не волнуйтесь так, вас хватит удар. Ответьте мне на вопрос, и я уйду"),
													pcharrepphrase("Не жду от тебя умного слова - ум у тебя тараканий. Но ты кое-что знаешь.",
													               "Я не задержу вас надолго. Всего один вопрос.")),
											   npchar, Dialog.CurrentNode);
			link.l3.go = "quests";
			//--> eddy. Аскольд, базар с рендомным торговцем
			if (pchar.questTemp.Ascold == "Seek_powder" && pchar.questTemp.Ascold.TraderId == npchar.id && !CheckAttribute(Pchar, "RomanticQuest.TalkInShop")) 
			{
				link.l3.go = "AscoldTrader";
			}
			if (pchar.questTemp.Ascold == "PowderWasSeek" && pchar.questTemp.Ascold.TraderId == npchar.id) 
			{
				link.l3.go = "AscoldTrader_WasSeek";
			}
			//<-- eddy. Аскольд, базар с рендомным торговцем
			link.l4 = HeroStringReactionRepeat(NPCharRepPhrase(npchar,
			                                        pcharrepphrase("Пока, старый пропойца.",
													               "Всего доброго, "+GetAddress_FormToNPC(NPChar)+" " +GetFullName(npchar)+"."),
													pcharrepphrase("Каррамба! Никуда не уходи. Я еще зайду.",
													               "Приятно было вас видеть, "+GetAddress_FormToNPC(NPChar)+" " +GetFullName(npchar)+"! До свидания!")),
											   NPCharRepPhrase(npchar,
											        pcharrepphrase("Отдать швартовы!",
													               "До свидания, милейший."),
													pcharrepphrase("Эй! Ты что там прячешь? Ром? Нет!?! Ну ладно, пойду, горло промочу.",
													               "Прошу меня извинить, у меня срочные дела в городе.")),
											   NPCharRepPhrase(npchar,
											        pcharrepphrase("Хех! Еще увидимся!",
													               "Мне надо идти. Прощайте, "+GetAddress_FormToNPC(NPChar)+" " +GetFullName(npchar)+"!"),
													pcharrepphrase("И не смей мне перечить! Недосуг мне тут с тобой спорить!",
													               "Пожалуй, стоит проверить, как дела на корабле. Прощайте, "+GetAddress_FormToNPC(NPChar)+" " +GetFullName(npchar)+"!")),
											   NPCharRepPhrase(npchar,
											        pcharrepphrase("Что с тобой, старина? Это же я - " +GetFullName(pchar)+ "! Проспись, я потом зайду.",
													               "Боюсь, ваша печень прикончит вас раньше, чем я еще раз зайду сюда."),
													pcharrepphrase("Считай свои пиастры, пройдоха! Я еще зайду.",
													               "Рад был вас видеть, " + GetAddress_FormToNPC(NPChar)+" " +GetFullName(npchar)+"!")),
											  npchar, Dialog.CurrentNode);
			link.l4.go = "exit";
		break;

		case "market":
//navy -->
			//занят ПГГ
			if (CheckFreeServiceForNPC(NPChar, "Store") != -1)	 // to_do имя сунуть
			{
				dialog.text = "Извините, но я сейчас занят. Другие клиенты! Заходите завтра.";
				link.l1 = "Да что вы говорите! Ладно, зайду попозже.";
				link.l1.go = "exit";
				break;
			}
//navy <--
			dialog.text = RandPhraseSimple("У меня есть сабли, пистолеты, пушечные ядра, ценная древесина и другие товары! Что вас интересует?", "Хотите купить золото или серебро? Или, может, присмотреть отличный клинок?");
			link.l1 = pcharrepphrase(LinkRandPhrase("В моем трюме полно добычи! Пиастры не пахнут, не так ли?",
			                         "Мне нужно освободить свой трюм от добычи и набить его твоим золотом. Ха-ха!",
									 "За полновесные дублоны ты и родную мать продашь! Но мне всего лишь нужно пополнить запасы."),
									 LinkRandPhrase("Хочу продать груз и пополнить запасы.",
			                         "Я хочу купить товар на продажу.",
									 "Покажи мне ядра, бомбы и прочие товары."));
			link.l1.go = "trade_1";
			link.l2 = LinkRandPhrase("Покажи мне сабли, пистолеты, и что там у тебя еще есть?",
			                         "Мне не помешает пара новых пистолетов, пули и красивый перстень.",
									 "Драгоценные камни, диковинные идолы - вот что меня интересует.");
			link.l2.go = "items";
			link.l3 = pcharrepphrase(RandPhraseSimple("Отчаливаю без торговли. Бывай!",
			                                          "Пойду, горло промочу. Никуда не уходи."),
									 RandPhraseSimple("Нет, мне сейчас не до покупок. Прощайте.",
									                  "Извините, у меня срочное дело. До свиданья!"));
			link.l3.go = "exit";
		break;

		case "trade_1":
            ok = (rColony.from_sea == "") || (Pchar.location.from_sea == rColony.from_sea);
		    if (sti(Pchar.Ship.Type) != SHIP_NOTUSED && ok)
			{
			    NextDiag.CurrentNode = NextDiag.TempNode;
			    DialogExit();
			    Pchar.PriceList.StoreManIdx = rColony.index; // boal 27.02.05
			    LaunchStore(sti(rColony.StoreNum));
			}
			else
			{
			    dialog.text = NPCharRepPhrase(npchar, pcharrepphrase("Дуришь меня? А где же твой корабль? У пирса его нет!",
				                                                     "Клянусь дьяволом, тебе не обмануть меня! У пирса нет твоего корабля!"),
													  pcharrepphrase("Я не вижу вашего корабля в порту, капитан " +GetFullName(pchar)+ ". Надеюсь, это не 'Летучий голландец'?",
													                 "Капитан, удобнее грузить товар с пирса. Пришвартуйте корабль и приходите."));
				link.l1 = NPCharRepPhrase(npchar, pcharrepphrase("" +RandSwear()+"Какая неприятность!!! Ладно, старый проныра, еще увидимся!",
				                                                 "Я не хотел вас обмануть " +GetFullName(npchar)+ ", корабль на другой стороне острова."),
												  pcharrepphrase("Нет. Мой корабль называется Black Perl. Что это ты побелел? Ха-ха! Шутка!",
												                 "Спасибо за совет, обязательно им воспользуюсь."));
			    link.l1.go = "exit";//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			}
		break;

		case "items":
			if (npchar.quest.item_date != lastspeak_date)
			{
				GiveItemToTrader(npchar);
				npchar.quest.item_date = lastspeak_date;
			}
			Nextdiag.CurrentNode = Nextdiag.TempNode;
			DialogExit();
			LaunchItemsTrade(NPChar);
		break;

		case "business":
			iTest = 0;
    		//квест Синей Птицы, начальный диалог
			if (pchar.questTemp.BlueBird == "begin" && sti(npchar.nation) == GetBaseHeroNation() && sti(npchar.nation) != PIRATE)
			{
				dialog.text = RandPhraseSimple("Капитан, прошу вас, помогите нам!!!", "Капитан, я прошу у вас помощи от имени всех торговцев!");
				link.l1 = "Что случилось? Чем я могу вам помочь?"; 
				link.l1.go = "RBlueBird";
				break;
			}			
//navy -->
			//занят ПГГ
			iTmp = CheckAvailableTaskForNPC(NPChar, PGG_TASK_WORKONSTORE);
			if (iTmp != -1)
			{
				dialog.text = "Была у меня работенка, но " + GetFullName(&Characters[iTmp]) + " уже взялся выполнить ее для меня.";
				link.l1 = "Да что вы говорите! Ладно, зайду попозже.";
				link.l1.go = "exit";
				break;
			}
//navy <--
			dialog.text = NPCharRepPhrase(npchar,"Дела?! Выкладывай все по порядку!","Я вас слушаю. О каком деле идет речь?");
            ok = (rColony.from_sea == "") || (Pchar.location.from_sea == rColony.from_sea);
            if (sti(Pchar.Ship.Type) != SHIP_NOTUSED && ok)
			{
    			/*if (CheckAttribute(pchar, "CargoQuest.iQuantityGoods"))
    			{
    				int iQuantityShipGoods = pchar.CargoQuest.iQuantityGoods;
    				int iQuestTradeGoods = pchar.CargoQuest.iTradeGoods;
    			}  */
    			if (CheckQuestAttribute("generate_trade_quest_progress", "begin") || CheckQuestAttribute("generate_trade_quest_progress",  "failed"))
    			{
    				if (pchar.CargoQuest.TraderID == npchar.id )//&&  GetSquadronGoods(pchar, iQuestTradeGoods) >= iQuantityShipGoods)
    				{
    					link.l1 = RandPhraseSimple("В трюме моего корабля груз для вашего магазина.", "Мой корабль зафрахтован для доставки груза в ваш магазин. Товар уже выгружают.");
    					link.l1.go = "generate_quest_2";
    				}
    				else
    				{
                        if (!CheckQuestAttribute("generate_trade_quest_progress",  "failed") && pchar.CargoQuest.GiveTraderID == npchar.id && GetNationRelation2MainCharacter(sti(characters[GetCharacterIndex(pchar.CargoQuest.TraderID)].nation)) == RELATION_ENEMY)
                        {
                            link.l1 = "К сожалению, я вынужден отказаться от взятых обязательств. Политическая обстановка не позволяет мне доставить груз.";
            				link.l1.go = "generate_quest_cannot_done";
                        }
                        else
                        {
                            if (rand(1) == 1)
                            {
                                link.l1 = RandPhraseSimple("Я хочу предложить вам зафрахтовать мое судно для перевозки груза.",
								                           "У меня прекрасный корабль, и я могу доставить любой груз, куда только вы пожелаете.");
            				    link.l1.go = "generate_quest_not_closed";
        				    }
    				    }
    				}
    			}
    			else
    			{
    				link.l1 = "Не хотите зафрахтовать судно, " +GetAddress_FormToNPC(NPChar)+ "? У меня надежный корабль и опытные матросы.";
    				link.l1.go = "generate_quest";
    			}
    			// --> на кредитный генератор
    			if (CheckAttribute(pchar, "GenQuest.LoanChest.TakeChest") && sti(pchar.GenQuest.LoanChest.TargetIdx) == sti(NPChar.index))
    			{
	                link.l3 = pcharrepphrase(RandPhraseSimple("Эх, вымотался в море... Вот что, приятель, денежный вопрос стоит остро.",
	                                                          "Ну что, старый плут, давай обсудим дела наши денежные."),
	                                        RandPhraseSimple("Я хотел бы обсудить с вами финансовые вопросы.",
	                                                         "Давайте обсудим финансовые вопросы, нам есть о чем переговорить."));,
	
	                link.l3.go = "LoanForAll";
                }
				//квест мэра - на связь с нашим шпионом
  				if (CheckAttribute(pchar, "GenQuest.Intelligence") && pchar.GenQuest.Intelligence.SpyId == npchar.id && pchar.GenQuest.Intelligence == "") 
				{
					link.l4 = RandPhraseSimple("Я здесь по поручению одного человека. Его зовут губернатор " + GetFullName(characterFromId(pchar.GenQuest.Intelligence.MayorId)) + ".", 
						GetFullName(characterFromId(pchar.GenQuest.Intelligence.MayorId)) + " прислал меня к вам. Я должен кое-что забрать...");
					link.l4.go = "IntelligenceForAll";
				}	
				// ----------------- квест получения Синей Птицы, сдаем квест -----------------
				if (pchar.questTemp.BlueBird == "weWon" && pchar.questTemp.BlueBird.traiderId == npchar.id)
				{
					link.l1 = "Хочу вас уведомить, что мне удалось справиться с вашим поручением. Шебека 'Синяя Птица' не будет более досаждать торговцам. Я прибыл за условленным гонораром.";
					link.l1.go = "RBlueBirdWon";
				}
				if (pchar.questTemp.BlueBird == "DieHard" && pchar.questTemp.BlueBird.traiderId == npchar.id)
				{
					link.l1 = "Должен вам сообщить, что мне не удалось потопить шебеку 'Синяя Птица'. Я сделал все, что мог...";
					link.l1.go = "RBlueBirdDieHard";
				}
				if (pchar.questTemp.BlueBird == "returnMoney" && pchar.questTemp.BlueBird.traiderId == npchar.id && sti(pchar.questTemp.BlueBird.count) > 0)
				{
					if (sti(pchar.questTemp.BlueBird.count) < 5)
					{
						link.l1 = "Как собственные флейты ваших коллег поживают? Все нормально?";
						link.l1.go = "RBlueBird_retMoney_1";
					}
					else
					{
						link.l1 = "Вы еще не одумались, не желаете отдать мне мои честно заработанные деньги?";
						link.l1.go = "RBlueBird_retMoney_3";
					}
				}
				// ----------------- квест получения Синей Птицы, продолжение -----------------
				//homo 25/06/06 слухи
                link.l6 = pcharrepphrase(RandPhraseSimple("Расскажешь мне последние сплетни? Все равно торчишь тут целый день.",
                                                          "Не суетись, старый мошенник, а лучше расскажи, как течет жизнь на суше?"),
                                        RandPhraseSimple("Кажется, что провел в море целую вечность. Что новенького в ваших краях?",
                                                         "Вы, наверное, знаете все последние новости? Что важного произошло?"));,

    		    link.l6.go = "rumours_trader";
				// <-- homo
				link.l99 = "Впрочем, сейчас не до разговоров.";
    			link.l99.go = "no_quests";
			}
			else
			{
                link.l1 = NPCharRepPhrase(npchar, pcharrepphrase("" +RandSwear()+"Я оставил корабль на другой стороне острова. Зайду попозже!", "Прошу меня простить, " +GetFullName(npchar)+ ", я зайду позже. Мой корабль еще не подошел к пирсу."), pcharrepphrase("Дьявол! Эти ротозеи не поставили корабль под погрузку! Зайду позже!", "Извините, я должен приказать поставить свой корабль к пирсу!"));
                link.l1.go = "exit";
    			// --> на кредитный генератор
    			if (CheckAttribute(pchar, "GenQuest.LoanChest.TakeChest") && sti(pchar.GenQuest.LoanChest.TargetIdx) == sti(NPChar.index))
    			{
	                link.l3 = pcharrepphrase(RandPhraseSimple("Эх, вымотался в море... Вот что, приятель, денежный вопрос стоит остро.",
	                                                          "Ну что, старый плут, давай обсудим дела наши денежные."),
	                                        RandPhraseSimple("Я хотел бы обсудить с вами финансовые вопросы.",
	                                                         "Давайте обсудим финансовые вопросы, нам есть о чем переговорить."));,
	
	                link.l3.go = "LoanForAll";
                }
				//квест мэра - на связь с нашим шпионом
  				if (CheckAttribute(pchar, "GenQuest.Intelligence") && pchar.GenQuest.Intelligence.SpyId == npchar.id && pchar.GenQuest.Intelligence == "") 
				{
					link.l7 = RandPhraseSimple("Я здесь по поручению одного человека. Его зовут губернатор " + GetFullName(characterFromId(pchar.GenQuest.Intelligence.MayorId)) + ".", 
						GetFullName(characterFromId(pchar.GenQuest.Intelligence.MayorId)) + " прислал меня к вам. Я должен кое-что забрать...");
					link.l7.go = "IntelligenceForAll";
				}	
			}
		break;
		
		case "generate_quest":
			if (npchar.quest.trade_date != lastspeak_date || bBettaTestMode)
			{
				npchar.quest.trade_date = lastspeak_date;
				//проверка враждебности нам страны торговца
				if (sti(NPChar.nation) != PIRATE && GetNationRelation2MainCharacter(sti(NPChar.nation)) == RELATION_ENEMY)
				{
					dialog.text = "Нет, не нужно! По крайней мере, до тех пор, пока вы враждуете с " + NationNameAblative(sti(NPChar.nation)) + ".";
					link.l1 = "В таком случае, прощайте.";
					link.l1.go = "exit";
				}
				else
				{
                    // проверка на проф пригодность -->
                    if (isBadReputation(pchar, 42))
                    {
                        dialog.text = RandSwear() + "Ты ведь " + GetFullName(pchar)+ ", не так ли? О тебе " +
                                      RandPhraseSimple("ходят ужасные слухи","говорят не лучшим образом") + ", я не хочу иметь с тобой дело.";
						link.l1 = RandPhraseSimple("Эх... стоит человеку раз оступиться, и все будут в него плевать.",
						                           RandSwear()+ "Что же, в море есть занятия и поинтереснее, "+GetFullName(npchar)+".");
						link.l1.go = "exit";
                        break;
                    }
                    // проверка на проф пригодность <--
                    int iTradeNation = GenerateNationTrade(sti(NPChar.nation));

					if (iTradeNation < 0)
					{
						dialog.text = NPCharRepPhrase(npchar, "Эх, "+GetAddress_Form(NPChar)+", не до фрахтов сегодня. Приходите завтра.","Зайдите завтра. Возможно, я сделаю вам выгодное предложение.");
						link.l1 = "Хорошо, я зайду завтра.";
						link.l1.go = "exit";
					}
					else
					{
                        int storeMan = findStoreMan(NPChar, iTradeNation);
                        if (storeMan > 0)
                        {
                            //проверяем импорт/экспорт
    						iTradeGoods = rand(GOOD_BRICK); //Рабы и золото не даем, бомбы и еду - да!!
    						//проверяем свободное место (при этом должно вмещаться по меньшей мере 100 единиц выбранного груза
    						RecalculateSquadronCargoLoad(pchar); // fix неверное место
    						iQuantityGoods = GetSquadronFreeSpace(pchar, iTradeGoods);
    						if (iQuantityGoods < 100)// это в шт. товара
    						{
    							dialog.text = NPCharRepPhrase(npchar, "В твою жалкую посудину не поместится весь груз, сегодня сделки не будет.","К сожалению, капитан "+GetFullName(pchar)+", для доставки груза мне нужен более вместительный корабль, чем ваш.");
    							link.l1 = NPCharRepPhrase(npchar, "У меня достойный корабль, но я понял вашу мысль. До свиданья.","Я вас понимаю, бизнес прежде всего. Прощайте.");
    							link.l1.go = "exit";
    						}
    						else
    						{
    							iQuantityGoods = iQuantityGoods - rand(makeint(iQuantityGoods/3)) - 10;
    							iMoney = makeint((iQuantityGoods * sti(Goods[iTradeGoods].Weight) / sti(Goods[iTradeGoods].Units)) * (4+rand(3) + GetSummonSkillFromNameToOld(pchar, SKILL_COMMERCE)) + 0.5);

    							pchar.CargoQuest.iTradeGoods = iTradeGoods;
    							pchar.CargoQuest.iQuantityGoods = iQuantityGoods;
    							pchar.CargoQuest.iMoney = iMoney;
    							pchar.CargoQuest.iTradeNation = iTradeNation;
    							pchar.CargoQuest.iDaysExpired = 25 + rand(5);
    							
    							pchar.CargoQuest.iTradeColony = Characters[storeMan].city;
    							pchar.CargoQuest.iTradeIsland = GetIslandByCityName(Characters[storeMan].city);
    							pchar.CargoQuest.TraderID     = Characters[storeMan].id;
    							pchar.CargoQuest.GiveTraderID = NPChar.id;
                                SaveCurrentQuestDateParam("CargoQuest");
    							string sNation = XI_ConvertString("Colony"+Characters[storeMan].city);
                                sTemp = "";
                                if (pchar.CargoQuest.iTradeIsland != Characters[storeMan].city)
								{
                                    sTemp = ", что на " + XI_ConvertString(pchar.CargoQuest.iTradeIsland+"Dat");
                                }
                                dialog.text =  "О! Я как раз хотел сам попросить вас об оказании мне одной услуги. Дело в том, что мне нужно доставить груз " +
                                               GetGoodsNameAlt(iTradeGoods)+ " в количестве " + FindRussianQtyString(iQuantityGoods) + " в город " +
                                           sNation + sTemp + ", и как можно быстрее. Если вы успеете сделать это за " +  FindRussianDaysString(makeint(pchar.CargoQuest.iDaysExpired)) +
                                           ", то по прибытии на место получите " +
                                           FindRussianMoneyString(iMoney) + " вознаграждения. Ну как?";
    							link.l1 = "Думаю, что я согласен.";
    							link.l1.go = "exit_trade";
    							link.l2  = "Не думаю, что мне это подходит.";
    							link.l2.go = "exit";
    						}
                        }
                        else
                        {
                            dialog.text = "Политическая обстановка в архипелаге не позволяет мне торговать. "+
                                          XI_ConvertString(NationShortName(sti(NPChar.nation))+"hunter") + " во вражде со всеми, и магазин остался только у меня.";
    						link.l1 = "О! Сожалею. Всего хорошего";
    						link.l1.go = "exit";
                        }
					}
				}
			}
			else
			{
				dialog.text = NPCharRepPhrase(npchar, "Давай проваливай, нет у меня времени на пустые разговоры!","Я же уже говорил вам сегодня, что у меня ничего для вас нет.");
				link.l1 = NPCharRepPhrase(npchar, "Ты обязан разговаривать учтиво, или я научу тебя вежливости!", "О, извините.");
				link.l1.go = "exit";
			}
		break;

		case "exit_trade":
			AddDialogExitQuest("trade_quest_open");
			Nextdiag.CurrentNode = Nextdiag.TempNode;
			DialogExit();
		break;

		case "generate_quest_2":
			if (pchar.quest.generate_trade_quest_progress == "failed")
			{
				dialog.text = RandSwear() + "Вы вспомнили о порученном вам деле слишком поздно, не находите? Мне пришлось заказывать товар повторно. Прощайте.";
				link.l1 = "Простите меня, но я так спешил. Возможно, если я покрою ваши убытки, это восстановит мою деловую репутацию?";
				link.l1.go = "generate_quest_failed";
				link.l9 = "Всего хорошего.";
				link.l9.go = "exit";
				ChangeCharacterReputation(pchar, -1);
			}
			else
			{
                iTradeGoods    =  makeint(pchar.CargoQuest.iTradeGoods);
                iQuantityGoods =  makeint(pchar.CargoQuest.iQuantityGoods);
                
                dialog.text = "Точно! Я его давно ждал. Вы должны доставить мне груз " +
                GetGoodsNameAlt(iTradeGoods) + " в количестве " + FindRussianQtyString(iQuantityGoods) + " и получить за это " +
                FindRussianMoneyString(sti(pchar.CargoQuest.iMoney)) + ".";
				link.l1 = "Именно так.";
				link.l1.go = "generate_quest_ready";
			}
		break;
		
		case "generate_quest_ready":
            iTradeGoods    =  makeint(pchar.CargoQuest.iTradeGoods);
            iQuantityGoods =  makeint(pchar.CargoQuest.iQuantityGoods);
            // блок кидалова нас на бабки -->
            if (rand(8) == 1 && sti(Goods[iTradeGoods].Cost) <= 44 && GetQuestPastDayParam("CargoQuest") > 5)
            {
                dialog.text = RandSwear() + "Я вынужден извиниться перед вами, "+GetAddress_Form(NPChar)+
                              ". Дело в том, что я " + RandPhraseSimple("в затруднительном положении и не смогу оплатить ваши услуги"," уже взял этот груз в другом месте по более выгодной цене")+
                              ". В качестве оплаты вашего фрахта я передаю вам весь доставленный вами груз.";
				link.l1 = RandSwear() + RandPhraseSimple("И я берег этот чертов груз от всех напастей!!! Я две сотни крыс из-за него убил!",
                                                         "Вы ставите меня в безвыходное положение, и мне придется согласиться.");
				link.l1.go = "exit";
				ChangeCharacterReputation(pchar, 2);
				AddCharacterExpToSkill(pchar, "Sailing", 60);
				AddCharacterExpToSkill(pchar, "Leadership", 20);
				AddCharacterExpToSkill(pchar, "COMMERCE", 15);

				pchar.quest.generate_trade_quest_progress = "";
				pchar.quest.generate_trade_quest.over = "yes";
                
                AddQuestRecord("DELIVERY_TRADE_QUEST", "4");
                AddQuestUserData("DELIVERY_TRADE_QUEST", "sGoodGen", GetGoodsNameAlt(iTradeGoods));
			    AddQuestUserData("DELIVERY_TRADE_QUEST", "sTargetColony",XI_ConvertString("Colony"+pchar.CargoQuest.iTradeColony+"Dat"));
                CloseQuestHeader("DELIVERY_TRADE_QUEST");
                break;
            }
            // блок кидалова нас на бабки <--
			if (GetSquadronGoods(pchar, iTradeGoods) < iQuantityGoods)
			{
                iTmp = iQuantityGoods - GetSquadronGoods(pchar, iTradeGoods);
                dialog.text = "Вы привезли не весь товар!! Согласно условиям договора недостает еще "
                              + FindRussianQtyString(iTmp) + " груза.";
				link.l9 = RandPhraseSimple("Да, действительно. Придется восполнить недостачу. Зайду еще раз.",
                                           RandSwear() +"Дьявольские крысы попортили чертов груз. Я восполню недостачу и зайду еще раз.");
				link.l9.go = "exit";
                AddQuestRecord("DELIVERY_TRADE_QUEST", "8");
                AddQuestUserData("DELIVERY_TRADE_QUEST", "sHaveNotQty", iTmp);
                AddQuestUserData("DELIVERY_TRADE_QUEST", "sGoodGen", GetGoodsNameAlt(iTradeGoods));
			}
			else
			{
                dialog.text = "Замечательно! Вот ваша награда, капитан "+GetFullName(pchar)+".";
				link.l1 = "Приятно с вами работать.";
				link.l1.go = "exit";
				ChangeCharacterReputation(pchar, 3);
				AddCharacterExpToSkill(pchar, "Sailing", 100);
				AddCharacterExpToSkill(pchar, "Leadership", 30);
				AddCharacterExpToSkill(pchar, "COMMERCE", 100);

				AddMoneyToCharacter(pchar, makeint(pchar.CargoQuest.iMoney));
				pchar.quest.generate_trade_quest_progress = "";
				pchar.quest.generate_trade_quest.over = "yes";
				RemoveCharacterGoods(pchar, makeint(pchar.CargoQuest.iTradeGoods), makeint(pchar.CargoQuest.iQuantityGoods));
				
                OfficersReaction("good");
                
                AddQuestRecord("DELIVERY_TRADE_QUEST", "2");
                AddQuestUserData("DELIVERY_TRADE_QUEST", "sGoodGen", GetGoodsNameAlt(iTradeGoods));
			    AddQuestUserData("DELIVERY_TRADE_QUEST", "sTargetColony",XI_ConvertString("Colony"+pchar.CargoQuest.iTradeColony+"Gen"));
                CloseQuestHeader("DELIVERY_TRADE_QUEST");
			}
		break;
		
		case "generate_quest_failed":
            iTradeGoods    =  makeint(pchar.CargoQuest.iTradeGoods);
            iQuantityGoods =  makeint(pchar.CargoQuest.iQuantityGoods);
            iMoney =  3*makeint(iQuantityGoods * sti(Goods[iTradeGoods].Cost) / sti(Goods[iTradeGoods].Units));
			dialog.text = "Хорошо... Сумма в " + FindRussianMoneyString(iMoney) + " как раз покроет мои убытки от нарушения вами сроков доставки товара.";
			link.l1 = "Ого! Нет уж! Пожалуй, я не буду больше вообще возить эти чертовы грузы! В море их и так достаточно!";
			link.l1.go = "exit";
            if (sti(pchar.Money) >= iMoney)
            {
    			link.l2 = "Вот требуемая сумма";
    			link.l2.go = "generate_quest_failed_2";
			}
            else
            {
                AddQuestRecord("DELIVERY_TRADE_QUEST", "7");
                AddQuestUserData("DELIVERY_TRADE_QUEST", "sMoney", FindRussianMoneyString(iMoney));
    		    AddQuestUserData("DELIVERY_TRADE_QUEST", "sTargetColony",XI_ConvertString("Colony"+pchar.CargoQuest.iTradeColony+"Gen"));
            }
		break;
		
		case "generate_quest_failed_2":
            iTradeGoods    =  makeint(pchar.CargoQuest.iTradeGoods);
            iQuantityGoods =  makeint(pchar.CargoQuest.iQuantityGoods);
            iMoney =  3*makeint(iQuantityGoods * sti(Goods[iTradeGoods].Cost) / sti(Goods[iTradeGoods].Units));
			dialog.text = "Замечательно. Теперь я могу сообщить своим коллегам, что с вами все же можно иметь дело.";
			link.l1 = "Спасибо! В следующий раз я не подведу.";
			link.l1.go = "exit";
			ChangeCharacterReputation(pchar, 10);
			AddCharacterExpToSkill(pchar, "Sailing", -100);
			AddCharacterExpToSkill(pchar, "Leadership", -50);
			AddCharacterExpToSkill(pchar, "COMMERCE", -100);

			AddMoneyToCharacter(pchar, -iMoney);
			pchar.quest.generate_trade_quest_progress = "";
			pchar.quest.generate_trade_quest.over = "yes";

            AddQuestRecord("DELIVERY_TRADE_QUEST", "5");
            AddQuestUserData("DELIVERY_TRADE_QUEST", "sMoney", FindRussianMoneyString(iMoney));
            AddQuestUserData("DELIVERY_TRADE_QUEST", "sGoodGen", GetGoodsNameAlt(iTradeGoods));
		    AddQuestUserData("DELIVERY_TRADE_QUEST", "sTargetColony",XI_ConvertString("Colony"+pchar.CargoQuest.iTradeColony+"Gen"));
            CloseQuestHeader("DELIVERY_TRADE_QUEST");
		break;
		
		case "generate_quest_not_closed":
            iTradeGoods    =  makeint(pchar.CargoQuest.iTradeGoods);
            iQuantityGoods =  makeint(pchar.CargoQuest.iQuantityGoods);

            dialog.text = "Хм. Я слышал, что вы еще не выполнили обязательства по предыдущему фрахту, а уже просите новый. Вы должны были доставить груз " +
            LanguageConvertString(iSeaGoods, "seg_" + Goods[iTradeGoods].Name)+ " в " + XI_ConvertString("Colony"+pchar.CargoQuest.iTradeColony) + ".";
			link.l1 = "Да, точно! Вы правы!";
			link.l1.go = "exit";
		break;
		
		case "generate_quest_cannot_done":
            dialog.text = RandSwear() + GetAddress_Form(NPChar) +", вы подводите меня!!! Может, попытаетесь как-то решить эту проблему?";
			link.l1 = "Хорошо. Я постараюсь доставить груз.";
			link.l1.go = "exit";
			link.l2 = "Нет. Не смогу. Сожалею";
			link.l2.go = "generate_quest_cannot_done_2";
			
		break;
		
		case "generate_quest_cannot_done_2":
		    iTradeGoods    =  makeint(pchar.CargoQuest.iTradeGoods);
            iQuantityGoods =  makeint(pchar.CargoQuest.iQuantityGoods);
            if (GetSquadronGoods(pchar, iTradeGoods) < iQuantityGoods)
			{
				dialog.text = "Минуточку! Но на корабле нет такого количества товара!! Вам недостает еще "
                              + FindRussianQtyString(iQuantityGoods - GetSquadronGoods(pchar, iTradeGoods)) + " груза.";
				link.l9 = RandPhraseSimple("Да, действительно. Придется восполнить недостачу.",
                                           RandSwear() +"Корабельные крысы попортили груз. Я докуплю товар и зайду еще раз.");
				link.l9.go = "exit";
			}
			else
			{
                dialog.text = "Ладно, я понимаю...  принимаю товар обратно.";
    			link.l1 = "Спасибо. В следующий раз я не подведу!";
    			link.l1.go = "exit";
    			ChangeCharacterReputation(pchar, -3);

				pchar.quest.generate_trade_quest_progress = "";
				pchar.quest.generate_trade_quest.over = "yes";
				RemoveCharacterGoods(pchar, makeint(pchar.CargoQuest.iTradeGoods), makeint(pchar.CargoQuest.iQuantityGoods));
                
                AddQuestRecord("DELIVERY_TRADE_QUEST", "6");
                AddQuestUserData("DELIVERY_TRADE_QUEST", "sGoodQty", FindRussianQtyString(sti(pchar.CargoQuest.iQuantityGoods)));
                AddQuestUserData("DELIVERY_TRADE_QUEST", "sGoodGen", GetGoodsNameAlt(iTradeGoods));
    		    AddQuestUserData("DELIVERY_TRADE_QUEST", "sTargetColony",XI_ConvertString("Colony"+pchar.CargoQuest.iTradeColony+"Dat"));
                CloseQuestHeader("DELIVERY_TRADE_QUEST");
            }
		break;

		case "no_quests":
			dialog.text = "В данный момент я не нуждаюсь в ваших услугах.";
			link.l1 = "Очень жаль. Тогда давайте поговорим о другом.";
			link.l1.go = "node_1";
		break;
		
        case "fight":
			DialogExit();
            NextDiag.CurrentNode = NextDiag.TempNode;
			LAi_group_Attack(NPChar, Pchar);
		break;
		
		case "Exit":
			if (pchar.questTemp.Ascold == "canTakeQuest" && pchar.questTemp.Ascold.TraderId == npchar.id)//eddy. Аскольд, начало
			{
				dialog.Text = "Ты много где бываешь. Слушай, если встретишь где-нибудь порошок египетской мумии, то будь добр, привези его мне. У меня есть серьезный клиент на это дело.";
				Link.l1 = "Хорошо.";
				Link.l1.go = "exit";
				SaveCurrentQuestDateParam("questTemp.Ascold");
				pchar.questTemp.Ascold = "Seek_powder";
				AddQuestRecord("Ascold", "1");
				AddQuestUserData("Ascold", "sName", GetFullName(npchar));
				AddQuestUserData("Ascold", "sCity", XI_ConvertString("Colony" + npchar.city + "Dat"));
			}
			else
			{
				Nextdiag.CurrentNode = Nextdiag.TempNode;
				DialogExit();
			}
		break;
	}
	LanguageCloseFile(iSeaGoods);
}

int findStoreMan(ref NPChar, int iTradeNation)
{
    ref ch;
	int n;
    int storeArray[30];
    int howStore = 0;

	for(n=0; n<MAX_CHARACTERS; n++)
	{
		makeref(ch,Characters[n]);
        if (CheckAttribute(ch, "Dialog.Filename") && ch.Dialog.Filename == "Common_Store.c") // магазин
		{
            if (sti(ch.nation) !=  iTradeNation) continue;
            if (NPChar.id == ch.id) continue;
            if (NPChar.id == "Panama_trader") continue; //нельзя доплыть
            if (ch.location == "none") continue; // фикс для новых, невидимых до поры островов
            if (GetIslandByCityName(ch.city) == GetIslandByCityName(NPChar.city)) continue; // хрен вам, а не читы!
            storeArray[howStore] = n;
            howStore++;
		}
    }
    if (howStore == 0)
    {
        return -1;
    }
    else
    {
        return storeArray[rand(howStore-1)];
    }
}
