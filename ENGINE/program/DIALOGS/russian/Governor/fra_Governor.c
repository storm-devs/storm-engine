// диалог по городам
void ProcessCommonDialogEvent(ref NPChar, aref Link, aref NextDiag)
{
    ref sld;
	switch (Dialog.CurrentNode)
	{
		case "quests":
            dialog.text = RandPhraseSimple("Какие вопросы?", "Что вам угодно?");
			link.l1 = RandPhraseSimple("Я передумал...", "Сейчас мне не о чем говорить");
		    link.l1.go = "exit";

            //******************** голландская линейка ***************************
        	switch (pchar.questTemp.State)   // что делаем в данный момент
            {
				case "DelivLettTortuga_toDOjeron": // Квест №7 голл. линейки. Письмо от Стэвезанта
					link.l1 = "Месье, я уполномочен доставить вам дипломатическую депешу от генерал-губернатора Кюрасао Питера Стэвезанта. Извольте получить.";
					link.l1.go = "Step_H7_1";
				break;
				case "DelivLettTortuga_WaitTwoHours":
					if (GetQuestPastTimeParam("questTemp") > 1)
					{
						link.l1 = "Месье генерал-губернатор, я прибыл.";
						link.l1.go = "Step_H7_3";
					}
					else
					{
						dialog.text = "Вы пришли слишком рано, я же сказал вам - через два часа!";
						link.l1 = "Простите, месье, я поторопился.";
						link.l1.go = "exit";
					}
				break;
				case "DelivLettTortuga_AfterRape": //Голландская линейка, квест №7. после грабежа.
					link.l1 = "Месье генерал-губернатор, это опять я!";
					link.l1.go = "Step_H7_5";
				break;
			}
		break;
		//******************** французская линейка ***************************
		case "work_1":  // работа на благо короны - линейка нации
            // сюда вход только с патентом, проверка выше
            dialog.text = LinkRandPhrase("Вы находитесь на службе Франции! Прошу немедленно приступить к выполнению возложенного на вас поручения!",
                                         "Я не буду скрывать, что от ваших успешных действий многое зависит. Поэтому приступайте к выполнению взятых на себя обязательств!",
                                         "Я жду вашего доклада о выполнении моего ответственного поручения, но не пустых перетолков.");
            link.l1 = RandPhraseSimple("Приступаю немедленно, месье генерал-губернатор.", "Я уже в процессе выполнения...");
            link.l1.go = "exit";

        	switch (pchar.questTemp.State)   // что делаем в данный момент
            {
                case "empty":   // Нет взятых квестов
                    if (GetQuestPastDayParam("questTemp") < sti(pchar.questTemp.Waiting_time))
                    {
                        dialog.text = LinkRandPhrase("В данный момент у меня нет для вас ответственных поручений. Вы можете появиться у меня в резиденции позже...",
                                                     "Прошу меня извинить, я очень сильно занят в данный момент!",
                                                     "Занимайтесь своими делами до поры, сейчас у меня нет для вас заданий.");
                        link.l1 = "Хорошо, месье генерал-губернатор.";
                        link.l1.go = "exit";
                    }
                    else
                    {
						switch(pchar.questTemp.CurQuestNumber)   // Взятие квестов начиная со второго
                        {
                            case "2":
                                dialog.text = "Мне необходимо, чтобы вы доставили это письмо как можно скорее на Кюрасао, генерал-губернатору Питеру Стэвезанту. И чем быстрее, тем выше будет награда.";
								link.l1 = "Хорошо, месье генерал-губернатор, я сделаю это. Давайте свое письмо.";
                                link.l1.go = "Step_2_1";
                            break;
                            case "3":
                                dialog.text = "Сударь, у меня для вас поручение весьма деликатного свойства. Вы честны, умеете держать слово, что уже не раз доказывали... Я хочу вверить в ваши руки честь дорогого мне человека...";
								link.l1 = "Месье, давайте ближе к делу...";
                                link.l1.go = "Step_3_1";
                            break;
                            case "4":
                                dialog.text = "Сударь, вы доказали свою преданность и честь, поэтому я доверяю вам важнейшее и совершенно секретное дело.";
								link.l1 = "Надеюсь, оно все же связано с морем?";
                                link.l1.go = "Step_4_1";
                            break;
                            case "5":
                                dialog.text = "Сударь, для вас снова поручение деликатного свойства. У меня есть серьезные опасения за жизнь донны де Лейва. Навестите ее, пожалуйста, она сейчас у меня в резиденции в соседней комнате. Сеньора посвятит вас в подробности.";
								link.l1 = "Я сделаю это немедленно.";
                                link.l1.go = "exit";
								LAi_group_Delete("PeaceGroup");
    							pchar.questTemp.State = "Fr5AnnaHant_toAnna";
								AddQuestRecord("Fra_Line_5_KillAnnaHanters", "1");
								sld = GetCharacter(NPC_GenerateCharacter("AnnaDeLeiva", "AnnaDeLeiva", "woman", "towngirl2", 10, SPAIN, -1, false));
								sld.Dialog.Filename = "Quest\FraLineNpc_1.c";
								sld.name = "Анна Рамирес";
								sld.lastname = "де Лейва";
								LAi_SetStayType(sld);
								LAi_group_MoveCharacter(sld, "PeaceGroup");
								ChangeCharacterAddressGroup(sld, "Tortuga_townhallRoom", "goto","goto2");
                            break;
                            case "6":
                                dialog.text = "Сударь, срочно доставьте этот пакет господину Жану Давиду, который сейчас находится на Гваделупе. Только не пытайтесь отыскать там Жана Давида, ха-ха, он более известен как Франсуа Олоне.";
								link.l1 = "Тот самый знаменитый флибустьер?";
                                link.l1.go = "Step_6_1";
                            break;
                            case "7":
                                dialog.text = "Замечательно, что вы пришли, я как раз собирался поручить разыскать вас. У меня к вам дело, которое не терпит отлагательств.";
								link.l1 = "Тогда скорей приступим к нему!";
                                link.l1.go = "Step_7_1";
                            break;
                            case "8":
                                dialog.text = "Сударь, мне поступило распоряжение направить сметливого, исполнительного, верного и достаточно авторитетного флибустьера в распоряжение морского интенданта Франции маркиза Бонрепо, который сейчас находится на Гваделупе. Вашу кандидатуру я считаю достойной.";
								link.l1 = "Благодарю за доверие. Но в чем заключается мое задание?";
                                link.l1.go = "Step_8_1";
                            break;
                            case "9":
                                dialog.text = "Барон, мне приказано назначить вас командиром специального карательного морского корпуса и придать в ваше распоряжение уже известный вам линейный корабль 'Солей Руаяль'. Ваши следующие задачи будут состоять в военных действиях против Испании в рамках деволюционной войны. Первая задача - отбить нападение испанской эскадры на Порт-о-Принс.";
								link.l1 = "Хм, видимо испанцы хотят владеть Эспаньолой единолично...";
                                link.l1.go = "Step_9_1";
                            break;
                            case "10":
                                dialog.text = "Так, барон, я рад вас видеть. Вы как нельзя вовремя!";
								link.l1 = "Я слушаю вас, сударь.";
                                link.l1.go = "Step_10_1";
                            break;
                            case "11":
                                dialog.text = "Итак, барон, Франция ждет от вас новых свершений. На этот раз военная операция, весьма сходная с предыдущей.";
								link.l1 = "Захват города, месье?";
                                link.l1.go = "Step_11_1";
                            break;
                            case "12":
                                dialog.text = "Барон, деволюционная война закончена великолепной победой нашего любимого Короля-Солнце Людовика XIV!!!";
								link.l1 = "Виват, месье генерал-губернатор!";
                                link.l1.go = "Step_12_1";
                            break;
						}
					}
				break;
                case "":           // Квест №1, Охрана пирата Леграна.
                    dialog.text = "Сударь, знаете ли вы Пьера Леграна? Он на днях, только представьте, на люггере захватил испанский военный галеон с прекрасной добычей. Корабль и груз продан, выкуп за испанцев получен, сумма в королевскую казну уплачена. Легран стал обеспеченным человеком и хочет вернуться во Францию. Что ж, я уважаю его решение.";
                    link.l1 = "И ему желательно уйти тихо?";
                    link.l1.go = "Step_1_1";
                break;
                case "Fr1Legran_backGovernor":
                    dialog.text = "Месье Легран сумел мне сообщить, что с ним все в порядке. Кстати, он очень лестно о вас отозвался. Я рад, что не ошибся в выборе честного и надежного человека.";
                    link.l1 = "Спасибо за похвалу, месье...";
                    link.l1.go = "Step_1_2";
                break;
                case "Fr1Legran_LegranKilled":
                    dialog.text = "А-а-а, это вы... Ну что скажете?";
                    link.l1 = "Я выполнил ваше задание, все в порядке. Более того, я помог Леграну разделаться с бандитами, которые пытались его ограбить.";
                    link.l1.go = "Step_1_3";
                break;
                case "Fr1Legran_LegranIsDead":
                    dialog.text = "А-а-а, это вы... Ну что скажете?";
                    link.l1 = "Господин генерал-губернатор, мне неприятно это сообщать, но на Мартинике нас ждала засада. Мы с Леграном вступили в схватку. К сожалению, Легран погиб в этом бою...";
                    link.l1.go = "Step_1_5";
                break;
                case "Fr2Letter_SeekProblems":
                    dialog.text = "Так, капитан, рассказывайте, что у вас?";
                    link.l1 = "Ваше письмо доставлено адресату, месье.";
                    link.l1.go = "Step_2_3";
                break;
                case "Fr2Letter_NiceResult":
                    dialog.text = "Так, капитан, рассказывайте, что у вас?";
                    link.l1 = "Скажу я вам, месье д'Ожерон, что попал я в изрядную переделку...";
                    link.l1.go = "Step_2_6";
                break;
                case "Fr3TakeAnna_BadResult":
                    dialog.text = "Я ждал вас. Что скажете, месье?";
                    link.l1 = "Я не смог привести донну Анну, я сожалею, что так вышло...";
                    link.l1.go = "Step_3_5";
                break;
                case "Fr3TakeAnna_NiceResult":
                    dialog.text = "Сударь, я даже не знаю, как мне вас благодарить!";
                    link.l1 = "Это было не сложно, месье...";
                    link.l1.go = "Step_3_7";
                break;
                case "Fr4SoleiRoyal_DieHard":
                    dialog.text = "Сударь, доложите о выполнении задания.";
                    link.l1 = "Месье генерал-губернатор, близ Доминики я не застал французской эскадры, зато на меня напала испанская эскадра Хуана Галено. Я не смог противостоять ей, мне пришлось покинуть место сражения...";
                    link.l1.go = "Step_4_7";
                break;
                case "Fr4SoleiRoyal_SoleiRoyalSunk":
                    dialog.text = "Сударь, доложите о выполнении задания.";
                    link.l1 = "Месье генерал-губернатор, близ Доминики я не застал французской эскадры, зато на меня напала испанская эскадра Хуана Галено. Я отразил нападение, но в результате боя 'Солей Руаяль' был потоплен. Мне очень жаль...";
                    link.l1.go = "Step_4_9";
                break;
                case "Fr4SoleiRoyal_backTortuga":
                    dialog.text = "Сударь, доложите о выполнении задания.";
                    link.l1 = "Все в порядке, месье, хоть и не без проблем. Я отразил нападение испанской эскадры под командованием того самого Хуана Галено, французской эскадры близ Доминики я не нашел, поэтому пришлось доставить 'Солей Руаяль' в Бас Тер.";
                    link.l1.go = "Step_4_11";
                break;
                case "Fr5AnnaHant_GoodWork":
                    dialog.text = "Сударь, я начинаю думать, что нет такого поручения, которое вы не могли бы выполнить. Вот ваша награда, и донна де Лейва хочет лично поблагодарить вас.";
                    link.l1 = "Спасибо, месье генерал-губернатор...";
                    link.l1.go = "exit";
					DeleteAttribute(&locations[FindLocation("Mayak10")], "DisableEncounters"); //откроем энканутеры
					pchar.questTemp.State = "empty";
					SaveCurrentQuestDateParam("questTemp");
					pchar.questTemp.CurQuestNumber = "6";
					pchar.questTemp.Waiting_time = "7";
					CloseQuestHeader("Fra_Line_5_KillAnnaHanters");
					AddMoneyToCharacter(pchar, 5000);
                break;
                case "Fr6Olone_OloneIsDead":
                    dialog.text = "Что скажете, капитан? Все прошло удачно?";
                    link.l1 = "Письмо доставлено, только вот с Олоне проблема вышла...";
                    link.l1.go = "Step_6_3";
                break;
                case "Fr6Olone_OloneIsLive":
                    dialog.text = "Что скажете, капитан? Все прошло удачно?";
                    link.l1 = "Да, все в порядке, месье. Ко всему прочему, мы успели захватить Куману и взяли изрядный приз.";
                    link.l1.go = "Step_6_7";
                break;
                case "Fr6Olone_TakeMoney":
                    dialog.text = "Что скажете, капитан? Все прошло удачно?";
                    link.l1 = "Да, месье, письмо доставлено адресату лично в руки. Все в порядке.";
                    link.l1.go = "Step_6_9";
                break;
                case "Fr7RockBras_toSeekPlace":
                    dialog.text = "Так, капитан. Расскажите мне о результатах миссии по спасению Рока Бразильца.";
                    link.l1 = "Пока я не сумел найти его, но я продолжу поиски...";
                    link.l1.go = "exit";
					link.l2 = "Сударь, я искал его повсюду, но найти так и не сумел. Я должен признаться, что все бесполезно...";
                    link.l2.go = "Step_7_6";
                break;
                case "Fr7RockBras_RockIsDead":
                    dialog.text = "Так, капитан, я ждал вас. Есть какие-нибудь новости о Бразильце?";
                    link.l1 = "Новости есть, месье генерал-губернатор. Однако, они неутешительные - Рок Бразилец погиб при побеге из застенков Инквизиции...";
                    link.l1.go = "Step_7_7";
                break;
                case "Fr7RockBras_RockIsSaved":
                    dialog.text = "Вижу, капитан, все в порядке. Рок уже рассказал, что ему довелось пережить в Инквизиции и как вы вытащили его оттуда. Я в восхищении!";
                    link.l1 = "Месье, это было совершенно обычное дело...";
                    link.l1.go = "Step_7_8";
                break;
                case "Fr7RockBras_RockIsAgony":
                    dialog.text = "Так, капитан, я ждал вас. Есть какие-нибудь новости о Бразильце?";
                    link.l1 = "Новости есть, месье генерал-губернатор. Однако, они неутешительные - Инквизиция замучила Рока Бразильца до смерти. Он испустил дух у меня на руках...";
                    link.l1.go = "Step_7_7";
                break;
                case "Fr8ThreeCorsairs_backGovernor":
                    dialog.text = "Итак, господин " + GetFullName(pchar) + ", я счастлив поздравить вас с титулом барона! Поверьте, редко кто удостаивается такой чести.";
                    link.l1 = "Признаться, сударь, я не очень понимаю, зачем он мне? Тем более, что достался он мне недешево...";
                    link.l1.go = "Step_8_2";
                break;
                case "Fr9GuardPP_GoodWork":
					if (LAi_IsDead(characterFromId("SoleiRoyalCaptain")))
					{
						dialog.text = "Сударь, вы потеряли 'Солей Руаяль'!";
						link.l1 = "Я спас Порт-о-Принс и пустил на дно мощнейшую эскадру испанцев!";
						link.l1.go = "Step_9_4";
					}
					else
					{
						dialog.text = "Барон, мне уже известно о вашей победе. Что я могу сказать? Прекрасно, друг мой, просто великолепно!";
						link.l1 = "Первая военная операция выполнена, месье генерал-губернатор...";
						link.l1.go = "Step_9_2";					
					}
                break;
                case "Fr9GuardPP_SoleiRoyalSunk":
                    dialog.text = "Сударь, вы потеряли 'Солей Руаяль'!";
                    link.l1 = "Я спас Порт-о-Принс и пустил на дно мощнейшую эскадру испанцев!";
                    link.l1.go = "Step_9_4";
                break;
                case "Fr9GuardPP_DieHard":
                    dialog.text = "Сударь, вы не сумели защитить Порт-о-Принс! В чем дело?!";
                    link.l1 = "Месье губернатор, испанцы сумели собрать эскадру невиданной силы! Мне пришлось отступить...";
                    link.l1.go = "Step_9_6";
                break;
                case "Fr9GuardPP_Late":
                    dialog.text = "Сударь, вы не сумели защитить Порт-о-Принс! В чем дело?!";
                    link.l1 = "Я не успел вовремя, месье генерал-губернатор, когда я подошел к городу, он уже был испанским...";
                    link.l1.go = "Step_9_7";
                break;
                case "Fr10OccupySD_toWeWon":
					dialog.text = "Что скажете, барон? Как прошла операция?";
					if (LAi_IsDead(characterFromId("SoleiRoyalCaptain")))
					{
						link.l1 = "Я захватил Санто-Доминго, теперь этот город принадлежит Франции! Но не все прошло гладко...";
						link.l1.go = "Step_10_4";
					}
					else
					{
						link.l1 = "Я захватил Санто-Доминго, теперь этот город принадлежит Франции!";
						link.l1.go = "Step_10_6";
					}
                break;
                case "Fr11OccupySC_toWeWon":
					dialog.text = "Барон, я жду вашего доклада, насколько успешна операция по захвату Санта Каталины?";
					if (LAi_IsDead(characterFromId("SoleiRoyalCaptain")))
					{
						link.l1 = "Санта-Каталина захвачена и приведена к присяге Людовику XIV. Это хорошие новости, а плохие - я потерял 'Солей Руаяль'...";
						link.l1.go = "Step_11_3";
					}
					else
					{
						link.l1 = "Санта-Каталина захвачена и приведена к присяге Людовику XIV.";
						link.l1.go = "Step_11_5";
					}
                break;
                case "QuestLineBreake":
                    dialog.text = "Я не имею дело с людьми, нарушающими свое слово. Мне больше нечего вам сказать.";
                    link.l1 = "...";
                    link.l1.go = "exit";
                    bWorldAlivePause   = false; // Конец линейки
                break;	
                case "EndOfQuestLine":
                    dialog.text = "Действуйте самостоятельно в интересах Франции, о флибустьерах тоже не забывайте. Война закончилась, пора обустроить свою жизнь, барон...";
                    link.l1 = "Хорошо, сударь.";
                    link.l1.go = "exit";
                    bWorldAlivePause   = false; // Конец линейки
                break;
			}
		break;
        //********************** Квест №7 голландки, Доставка письма д'Ожерону от Стэвезанта ************************
        case "Step_H7_1":
            dialog.text = "Прекрасно.";
            link.l1 = "За сим, разрешите откланяться...";
            link.l1.go = "Step_H7_2";
            TakeItemFromCharacter(pchar, "letter_2");
            BackItemDescribe("letter_2");
        break;
        case "Step_H7_2":
            dialog.text = "Подождите. Зайдите ко мне через два часа, я должен ознакомиться с содержанием данного документа. Возможно, будет ответ.";
            link.l1 = "Я понял, буду у вас в резиденции через два часа, месье.";
            link.l1.go = "exit";
            AddQuestRecord("Hol_Line_7_DelivLettTortuga", "2");
            pchar.questTemp.State = "DelivLettTortuga_WaitTwoHours";
            SaveCurrentQuestDateParam("questTemp");
        break;
        case "Step_H7_3":
            dialog.text = "Да, хорошо. Итак, я ознакомился с содержанием депеши. У меня действительно есть ответ для Стэвезанта, прошу доставить данный пакет в Кюрасао.";
            link.l1 = "Хорошо, месье. Теперь я могу удалиться?";
            link.l1.go = "Step_H7_4";
        break;
        case "Step_H7_4":
            dialog.text = "Да, конечно. Удачи вам на обратном пути. С началом деволюционной войны испанцы серьезно активизировались. Так что будете очень осторожны. Если что-либо из нашей переписки попадет к испанцам, то они смогут доказать наши согласованные действия против Испании. Молодой Республике Соединенных Провинций в этом случае не поздоровится.";
            link.l1 = "Я понял, месье. Прощайте и удачи вам также.";
            link.l1.go = "exit";
            AddQuestRecord("Hol_Line_7_DelivLettTortuga", "3");
            pchar.questTemp.State = "DelivLettTortuga_SnapMeeting";
            ChangeItemDescribe("letter_1", "itmdescr_letter_1_HolLineQ7");
            GiveItem2Character(pchar,"letter_1");
            LocatorReloadEnterDisable("Tortuga_town", "gate_back", true);
            LocatorReloadEnterDisable("Tortuga_town", "reload1_back", true);
            LocatorReloadEnterDisable("Tortuga_town", "reload2_back", true);
            LocatorReloadEnterDisable("Tortuga_tavern", "reload2_back", false);
            LAi_LocationFightDisable(&Locations[FindLocation("Tortuga_tavern_upstairs")], true);
            Pchar.quest.DelivLettTortuga_SnapMeeting.win_condition.l1 = "location";
            Pchar.quest.DelivLettTortuga_SnapMeeting.win_condition.l1.location = "Tortuga_town";
            Pchar.quest.DelivLettTortuga_SnapMeeting.win_condition = "DelivLettTortuga_SnapMeeting";
        break;
        case "Step_H7_5":
            dialog.text = "Я вижу. А почему вы еще здесь? Похоже, что и никуда не торопитесь!";
            link.l1 = "Месье, написанная вами депеша похищена. Меня ударили по голове, и я пролежал без сознания все это время!";
            link.l1.go = "Step_H7_6";
        break;
        case "Step_H7_6":
            dialog.text = "Немедленно отправляйтесь к начальнику порта с моим распоряжением указать вам, что за судно покинуло Тортугу в течение последних двух часов. Затем догоните их, возьмите судно на абордаж и найдите пакет!";
            link.l1 = "Я понял, месье, спасибо за помощь!";
            link.l1.go = "exit";
            pchar.questTemp.State = "DelivLettTortuga_toPortMan";
        break;
        //********************** Квест №1 французской линейки, охрана Леграна ************************
        case "Step_1_1":
            dialog.text = "Вы верно уловили. Месье Леграну нужно тайно попасть на Мартинику, потом он сумеет отправиться оттуда рейсовым кораблем в Дьепп. Но он «сидит» на куче золота и это знают все, поэтому есть серьезные опасения за благополучный исход возвращения в Старый Свет.\n"+
				          "Отправляйтесь в таверну - там вы найдете Леграна. Ваша задача — доставить его в бухту Ле Марен, что на Мартинике. Пьер отблагодарит вас, сударь.";
            link.l1 = "Хорошо, месье, я сделаю все так, как вы сказали.";
            link.l1.go = "exit";
            SaveCurrentQuestDateParam("questTemp");
    		// остальные линейки в сад -->
    		pchar.questTemp.NationQuest = FRANCE;
    		// остальные линейки в сад <--
    		pchar.questTemp.State = "Fr1Legran_toTavern";
            AddQuestRecord("Fra_Line_1_LegranDis", "1");
            sld = GetCharacter(NPC_GenerateCharacter("Legran", "officer_9", "man", "man", 40, FRANCE, -1, false));
			FantomMakeCoolFighter(sld, 40, 90, 40, "blade28", "pistol4", 120);
			sld.name = "Пьер";
			sld.lastname = "Легран";
            sld.Dialog.Filename = "Quest\FraLineNpc_1.c";
			sld.SaveItemsForDead = true; // сохранять на трупе вещи
			sld.DontClearDead = true; // не убирать труп через 200с
			sld.money = 1000000;
			sld.items.jewelry5 = 30;
			sld.items.indian6 = 1;
			sld.items.jewelry14 = 12;
			sld.items.indian2 = 1;
			sld.items.indian8 = 1;
			sld.items.indian7 = 1;
			sld.items.indian12 = 1;
			sld.items.jewelry2 = 20;
			GiveItem2Character(sld, "cirass4");
			FreeSitLocator("Tortuga_tavern", "sit3");
            LAi_SetSitType(sld);
            ChangeCharacterAddressGroup(sld, "Tortuga_tavern", "sit", "sit3");
        break;		
		case "Step_1_2":
            dialog.text = "Ну что же, в данный момент я вас не задерживаю, но прошу не пропадать надолго из поля моего зрения - для флибустьера у меня частенько бывает работа.";
            link.l1 = "Хорошо, месье генерал-губернатор, я буду наведываться к вам время от времени.";
            link.l1.go = "exit";
            pchar.questTemp.State = "empty";
            SaveCurrentQuestDateParam("questTemp");
            pchar.questTemp.CurQuestNumber = "2";
            pchar.questTemp.Waiting_time = "7";
			CloseQuestHeader("Fra_Line_1_LegranDis");
			AddTitleNextRate(sti(NPChar.nation), 2);
			AddCharacterExpToSkill(pchar, "Sneak", -170);
			AddCharacterExpToSkill(pchar, "Commerce", 90);
			AddCharacterExpToSkill(pchar, "Fencing", 80);
			AddCharacterExpToSkill(pchar, "Fortune",70);
			ChangeCharacterNationReputation(pchar, sti(NPChar.nation), 1);
        break;
        case "Step_1_3":
            dialog.text = "Хм, все это прекрасно, но вот незадача - сам Легран пропал. Он должен был перед отъездом из Форт де Франса отписать мне, но письма нет, да и сам он как в воду канул. Что вы думаете об этом?";
            link.l1 = "Я не в курсе, месье...";
            link.l1.go = "Step_1_4";
        break;
        case "Step_1_4":
            dialog.text = "Странно все это... Вот что, сударь, я не желаю больше иметь с вами дело. Более того, если я когда-нибудь узнаю, что это вы убили Леграна - мне сложно описать словами, что с вами будет.\n"+
				          "Я вас не задерживаю!";
            link.l1 = "Я понял, месье...";
            link.l1.go = "exit";
            pchar.questTemp.State = "QuestLineBreake";
			CloseQuestHeader("Fra_Line_1_LegranDis");
            DeleteAttribute(pchar, "questTemp.Waiting_time");
			ChangeCharacterNationReputation(pchar, sti(NPChar.nation), -3);
			bWorldAlivePause   = false; // Конец линейки
			ChangeCharacterReputation(pchar, -10);
			AddCharacterExpToSkill(pchar, "Sneak", -170);
			//слухи			
			AddSimpleRumour("Вы слышали, капитана " + GetMainCharacterNameGen() + " наш генерал-губернатор выставил из резиденции за какую-то темную историю, связанную с Леграном... Теперь у него 'волчий билет.'", FRANCE, 5, 1);
        break;
        case "Step_1_5":
            dialog.text = "Да, мне уже известно об этом печальном событии... Как жаль, что Леграну так и не удалось вернуться к нормальной жизни, как жаль...";
            link.l1 = "Я знал его совсем чуть-чуть, но мне он показался хорошим малым.";
            link.l1.go = "Step_1_6";
        break;
		case "Step_1_6":
            dialog.text = "И это действительно так, месье " + pchar.lastname + ". Ну что же, печально, что так вышло, но, тем не менее, я благодарен вам за содействие. Поэтому прошу вас заглядывать ко мне время от времени - возможно, подвернется работа для флибустьера.";
            link.l1 = "Хорошо, месье д'Ожерон, буду наведываться в ваше резиденцию.";
            link.l1.go = "Step_1_7";
        break;
        case "Step_1_7":
            dialog.text = "Одну минуту, сударь! Верните деньги, которые были у Леграна, они вам не принадлежат.";
            link.l1 = "Хм, месье...";
            link.l1.go = "Step_1_8";
        break;
        case "Step_1_8":
            dialog.text = "Я никогда не поверю в то, что вы их не забрали. Поэтому верните мне 1200000 монет, и я продолжу с вами сотрудничество.";
            link.l1 = "А почему так много - 1200000?";
            link.l1.go = "Step_1_9";
        break;
        case "Step_1_9":
            dialog.text = "Потому что я знаю, сколько денег было у Леграна."; 
			link.l1 = "И что же теперь мне делать?";
			link.l1.go = "Step_1_10";
        break;
        case "Step_1_10":
            dialog.text = "Пока вы не вернете мне деньги Леграна целиком и полностью, у меня в резиденции вам делать нечего.";
			if (sti(pchar.money) >= 1200000)
			{
				link.l1 = "Хорошо, месье, забирайте...";
				link.l1.go = "Step_1_11";
				AddMoneyToCharacter(pchar, -1200000);
				NextDiag.TempNode = "First time";
			}
			else
			{
				link.l1 = "Пока у меня нет таких денег. Но я раздобуду и принесу их вам.";
				link.l1.go = "exit";
				NextDiag.TempNode = "Step_1_10";
			}
        break;

		case "Step_1_11":
            dialog.text = "Ну что же, мы в расчете. Вы можете теперь бывать у меня, я буду давать вам работу время от времени, сударь."; 
			link.l1 = "Очень хорошо, господин генерал-губернатор.";
			link.l1.go = "exit";
            pchar.questTemp.State = "empty";
            SaveCurrentQuestDateParam("questTemp");
            pchar.questTemp.CurQuestNumber = "2";
            pchar.questTemp.Waiting_time = "15";
			CloseQuestHeader("Fra_Line_1_LegranDis");
			AddTitleNextRate(sti(NPChar.nation), 1);
			AddCharacterExpToSkill(pchar, "Leadership", 100);
			AddCharacterExpToSkill(pchar, "Pistol", 150);
			AddCharacterExpToSkill(pchar, "Grappling", 120);
			ChangeCharacterReputation(pchar, 5);
			ChangeCharacterNationReputation(pchar, sti(NPChar.nation), 1);
			//слухи
			AddSimpleRumour("Вы знаете, капитан " + GetFullName(pchar) + " был нанят генерал-губернатором для сопровождения славного корсара Леграна. И он с честью выполнил это поручение!", FRANCE, 5, 1);
        break;
        //********************** Квест №2 доставка письма на Кюрасао ************************
        case "Step_2_1":
            dialog.text = "Петер Стэвезант, генерал-губернатор Кюрасао, мой личный друг и друг Республики Соединенных провинций. Поэтому не вздумайте учудить там что-нибудь эдакое, иначе, по прибытии на Тортугу, я спущу с вас шкуру.";
			link.l1 = "Месье Бертран, я флибустьер, но разве похож на безответственного типа? Все будет сделано в лучшем виде, можете не беспокоиться.";
            link.l1.go = "Step_2_2";
        break;
        case "Step_2_2":
            dialog.text = "Ну что же, очень хорошо. Ступайте, сударь.";
            link.l1 = "До свидания, месье.";
            link.l1.go = "exit";
            SaveCurrentQuestDateParam("questTemp");
    		pchar.questTemp.State = "Fr2Letter_toCuracao";
            AddQuestRecord("Fra_Line_2_DelivLetter", "1");
            ChangeItemDescribe("letter_1", "itmdescr_letter_1_FraLineQ1");
            GiveItem2Character(pchar,"letter_1");
        break;	
        case "Step_2_3":
            dialog.text = "Все прошло нормально, без неприятностей?";
			link.l1 = "Ну, как вам сказать, не совсем. Меня сначала арестовали непонятно почему, потом выпустили...";
            link.l1.go = "Step_2_4";
        break;
        case "Step_2_4":
            dialog.text = "А за что вас могли арестовать?";
			link.l1 = "Понятия не имею. Полагаю, за внешний вид. Но в итоге все кончилось хорошо, месье. Так что сейчас причин для беспокойства нет.";
            link.l1.go = "Step_2_5";
        break;
        case "Step_2_5":
            dialog.text = "М-да, странно все это... Ну да ладно. На данный момент у меня нет для вас работы, так что заглядывайте ко мне в резиденцию время от времени, возможно, что-нибудь подвернется.";
			link.l1 = "Хорошо, месье генерал-губернатор.";
            link.l1.go = "exit";
			group_DeleteGroup("Spa_Attack");
            pchar.questTemp.State = "empty";
            SaveCurrentQuestDateParam("questTemp");
            pchar.questTemp.CurQuestNumber = "3";
            pchar.questTemp.Waiting_time = "20";
			CloseQuestHeader("Fra_Line_2_DelivLetter");
			DeleteAttribute(pchar, "questTemp.Out");
			DeleteAttribute(pchar, "questTemp.shore");
			AddCharacterExpToSkill(pchar, "Sneak", -100);
			AddCharacterExpToSkill(pchar, "Accuracy", 200);
			ChangeCharacterReputation(pchar, -1);
			ChangeCharacterNationReputation(pchar, sti(NPChar.nation), -1);
			//слухи
			AddSimpleRumour("Вы слышали, капитан " + GetFullName(pchar) + " исполнял обязанности вестового нашего генерал-губернатора. И знаете, странная история приключилась с ним в Виллемстаде, очень странная...", FRANCE, 5, 1);
        break;
        case "Step_2_6":
            dialog.text = "Что случилось, сударь?";
			link.l1 = "Ох, месье, давайте по порядку. Сначала меня бросили в тюрьму и хотели повесить, но на утро следующего дня освободили.";
            link.l1.go = "Step_2_7";
        break;
        case "Step_2_7":
            dialog.text = "Как повесить? Вы же были моим вестовым!";
			link.l1 = "Думали, что я лазутчик бандитов, промышляющих грабежом голландских судов близ Кюрасао. В общем, в итоге я нашел эту банду и покончил с ними. Это оказались испанские каперы, как вам это?!";
            link.l1.go = "Step_2_8";
        break;
        case "Step_2_8":
            dialog.text = "Испанские каперы?! Ха-ха, это звучит о-о-очень убедительно.\n"+
				          "Это хорошо, что вы смогли проявить себя в Виллемстаде. Я рад, что поручил это задание такому дотошному флибустьеру.\n"+
						  "Сейчас я вас более задерживать не смею - отдыхайте. Но через неделю-другую прошу явиться ко мне в резиденцию, я подберу для вас еще работу.";
			link.l1 = "Отлично, месье генерал-губернатор. Буду непременно.";
            link.l1.go = "exit";
			group_DeleteGroup("Spa_Attack");
            pchar.questTemp.State = "empty";
            SaveCurrentQuestDateParam("questTemp");
            pchar.questTemp.CurQuestNumber = "3";
            pchar.questTemp.Waiting_time = "10";
			CloseQuestHeader("Fra_Line_2_DelivLetter");
			DeleteAttribute(pchar, "questTemp.Out");
			DeleteAttribute(pchar, "questTemp.shore");
			AddCharacterExpToSkill(pchar, "Sneak", 150);
			AddCharacterExpToSkill(pchar, "Sailing", 100);
			AddCharacterExpToSkill(pchar, "FencingLight", 150);
			AddCharacterExpToSkill(pchar, "Fencing", 150);
			AddCharacterExpToSkill(pchar, "FencingHeavy", 120);
			AddTitleNextRate(sti(NPChar.nation), 2);
			ChangeCharacterReputation(pchar, 2);
			ChangeCharacterNationReputation(pchar, sti(NPChar.nation), 2);
			//слухи
			AddSimpleRumour("Вы слышали, капитан " + GetFullName(pchar) + " исполнял обязанности вестового нашего генерал-губернатора. И знаете, странная история приключилась с ним в Виллемстаде - его арестовали. Но к чести французов, он сумел выпутаться из ситуации. Д'Ожерон, должно быть, доволен...", FRANCE, 5, 1);
		break;
        //********************** Квест №3 похищение донны Анны ************************
        case "Step_3_1":
            dialog.text = "О вас ходит молва, что вы отчаянный головорез, и я знаю, что для вас слово 'честь' не пустой звук. Сознаюсь, мне нелегко довериться вам, но полагаю, это лучший вариант... Моя жизнь в ваших руках, и вам решать, разменяете ли вы ее на золото или докажете, что вы рыцарь!";
			link.l1 = "Черт возьми, любой скажет, что я умею резать глотки, но никто не посмеет сказать, что я подлец и предатель! Я слушаю вас.";
            link.l1.go = "Step_3_2";
        break;
        case "Step_3_2":
            dialog.text = "Вы, наверное, в курсе, что я приказал доставить из Франции подруг для наших отважных флибустьеров. Теперь Тортуга - рай для пирата, всюду флюиды любви, женский смех кружит голову... Но увы, не все вкушают райские плоды.";
			link.l1 = "Амур избрал мишенью не только буканьеров?";
            link.l1.go = "Step_3_3";
        break;
        case "Step_3_3":
            dialog.text = "Дело не во мне. Есть дама, которая будет счастлива вдохнуть вольный воздух нашей колонии, но вынуждена сидеть взаперти под властью жестокого супруга... Вызволить ее из темницы, в которой она пребывает - дело, достойное кавалера!";
			link.l1 = "Слава Создателю, вы наконец-то добрались до сути. Кого я должен спасти?";
            link.l1.go = "Step_3_4";
        break;
        case "Step_3_4":
            dialog.text = "Значит, вы беретесь за это дело? Отлично! Дама, о которой я говорю, - супруга дона Хосе Рамиреса де Лейва, коменданта Гаваны. Вам надлежит проникнуть в город и найти способ вывести ее оттуда.\n"+
						  "Для того, чтобы вы могли сделать это успешно, я даю вам Испанскую торговую лицензию Эскориала сроком на 60 дней, с ней вам будет разрешено заходить в испанские порты и посещать испанские города. И позаботьтесь о том, чтобы на вашем корабле развивался дружественный Испании флаг.\n"+
				          "Вот, возьмите это кольцо, по нему донна Анна Рамирес де Лейва узнает, что вы - мой доверенный человек. И помните, честь этой достойной женщины - в ваших руках.";
			link.l1 = "Я понял вас, месье. Признаюсь, непривычное дело, однако ценю ваше доверие... Все будет сделано в лучшем виде - не сомневайтесь.";
            link.l1.go = "exit";
            GiveNationLicence(SPAIN, 60);
			GiveItem2Character(pchar,"DOjeronRing");
    		pchar.questTemp.State = "Fr3TakeAnna_toHavana";
            AddQuestRecord("Fra_Line_3_TakeAnna", "1");
			LocatorReloadEnterDisable("Havana_town", "houseS1", true);
        break;
        case "Step_3_5":
            dialog.text = "Дьявол! А я рассчитывал на вашу сообразительность и сноровку. А что случилось?";
			link.l1 = "Да ничего особенного, месье. Я просто не сумел к ней подобраться - жена коменданта столицы, как-никак...";
            link.l1.go = "Step_3_6";
        break;
        case "Step_3_6":
            dialog.text = "М-да, печально... Ну что же, это была моя личная просьба к вам, поэтому перед Францией вы ни в чем не виноваты.\n"+
				          "Я прошу вас заходить ко мне в дальнейшем, возможно, у меня будет для вас дело.";
			link.l1 = "Хорошо, месье генерал-губернатор...";
            link.l1.go = "exit";
            pchar.questTemp.State = "empty";
            SaveCurrentQuestDateParam("questTemp");
            pchar.questTemp.CurQuestNumber = "4";
            pchar.questTemp.Waiting_time = "20";
			CloseQuestHeader("Fra_Line_3_TakeAnna");
            TakeItemFromCharacter(pchar, "letter_open");
            BackItemDescribe("letter_open");
			TakeNationLicence(SPAIN);
			ChangeCharacterReputation(pchar, 10);
			AddCharacterExpToSkill(pchar, "Sneak", -100);
			AddCharacterExpToSkill(pchar, "Sailing", -100);
			ChangeCharacterNationReputation(pchar, sti(NPChar.nation), -1);
			//слухи
			AddSimpleRumour("Ходят слухи, что капитан " + GetFullName(pchar) + " провалил какое-то очень важное задание д'Ожерона, связанное с амурными делами...", FRANCE, 5, 1);
		break;
        case "Step_3_7":
			dialog.text = "Тем не менее, примите от меня скромное вознаграждение.\n"+
				          "И черт возьми, вы уж оставьте меня одного, мне сейчас не до административных вопросов. Заходите через некоторое время...";
			link.l1 = "Хорошо, месье генерал-губернатор. Обязательно буду позже...";
            link.l1.go = "exit";
            pchar.questTemp.State = "empty";
            SaveCurrentQuestDateParam("questTemp");
            pchar.questTemp.CurQuestNumber = "4";
            pchar.questTemp.Waiting_time = "40";
			CloseQuestHeader("Fra_Line_3_TakeAnna");
            TakeItemFromCharacter(pchar, "letter_open");
            BackItemDescribe("letter_open");
			TakeNationLicence(SPAIN);
			AddMoneyToCharacter(pchar, 25000);
            sld = characterFromId("AnnaDeLeiva");
            RemovePassenger(pchar, sld);
			DeleteAttribute(sld, "RebirthPhantom");
         	sld.LifeDay = 0; // уберем нпс
			pchar.questTemp.AnnaIsLife = true; //но оставим флаг на пятый квест
			//==> вешаем прерывание на узнавание в таверне.
			Pchar.quest.Fr3_AfterAnna_IKnowYou.win_condition.l1 = "location";
			Pchar.quest.Fr3_AfterAnna_IKnowYou.win_condition.l1.location = "Havana_tavern";
			Pchar.quest.Fr3_AfterAnna_IKnowYou.win_condition = "Fr3_AfterAnna_IKnowYou";			
			AddTitleNextRate(sti(NPChar.nation), 3);
			ChangeCharacterReputation(pchar, 5);
			ChangeCharacterNationReputation(pchar, sti(NPChar.nation), 1);
			AddCharacterExpToSkill(pchar, "Leadership", 150);
			AddCharacterExpToSkill(pchar, "Repair", 200);
			AddCharacterExpToSkill(pchar, "Sailing", 150);
			AddCharacterExpToSkill(pchar, "Repair", 250);
			//слухи
			AddSimpleRumour("Ходят слухи, что именно капитан " + GetFullName(pchar) + " проявил чудеса храбрости и находчивости, и сумел доставить сюда испанскую пассию д'Ожерона - донну Анну...", FRANCE, 5, 1);
		break;

        //********************** Квест №4  Солей Руаяль ************************		
		case "Step_4_1":
            dialog.text = "Да, я знаю, что вы моряк, а не любитель альковных тайн. Но я вас успокою: нынешнее дело в вашем вкусе. Недавно в метрополии спущен на воду самый мощный и прекрасный корабль мира - 'Солей Руаяль'. Его Величество заинтересован в безопасности французских колоний в Вест-Индии, и для нас большая честь, что этот корабль направлен именно к нам.";
			link.l1 = "Когда же он прибудет?";
            link.l1.go = "Step_4_2";
        break;
		case "Step_4_2":
            dialog.text = "Он уже здесь, на рейде. А теперь он должен отбыть к Доминике. Вам надлежит сопровождать его до этого острова.";
			link.l1 = "С моим кораблем - охранять лучший корабль Франции? Хм, странно это...";
            link.l1.go = "Step_4_3";
        break;
		case "Step_4_3":
            dialog.text = "Ничего странного. Два корабля всегда лучше, чем один, а вы храбрый и умелый капитан. К тому же у меня есть основания полагать, что испанцы собираются устроить охоту на 'Солей Руаяль'. Трем или четырем галеонам с абордажной командой на борту вполне по силам захватить даже самый мощный корабль.\n"+
				          "Более того, мне достоверно известно, что эти галеоны под командованием некоего Хуана Галено уже наготове. Положите жизнь, если это будет необходимо, но нельзя допустить, чтобы наш флагман попал в руки Эскориала. Или вы вернетесь с докладом об успешном выполнении миссии, или лучше не возвращайтесь...";
			link.l1 = "Положить жизнь... Я далеко не трус, но звучит это не очень заманчиво!";
            link.l1.go = "Step_4_4";
        break;
		case "Step_4_4":
            dialog.text = "Я понял намек, капитан. Надеюсь, заманчиво прозвучит обещание щедрой награды за эту работу?";
			link.l1 = "А зачем везти корабль именно к Доминике? Это же необитаемый остров...";
            link.l1.go = "Step_4_5";
        break;
		case "Step_4_5":
            dialog.text = "Там вы должны будете соединиться с эскадрой из Гваделупы. Ее должен отправить к Доминике генерал-губернатор Бас-Тера " + GetFullName(characterFromId("BasTer_Mayor")) + ".";
			link.l1 = "Понятно. Ну что же, я согласен выполнить данное поручение.";
            link.l1.go = "Step_4_6";
        break;
		case "Step_4_6":
            dialog.text = "Отлично. Итак, 'Солей Руаяль' переходит под ваше начало. Прошу вас немедленно явиться к начальнику порта, он оформит капитана 'Солей Руаяль' к вам в компаньоны.";
			link.l1 = "Мы отправляемся, месье.";
            link.l1.go = "exit";
			pchar.questTemp.State = "Fr4SoleiRoyal_toPortMan";
			AddQuestRecord("Fra_Line_4_SoleiRoyal", "1");
        break;
		case "Step_4_7":
            dialog.text = "Поверить не могу... Вы бросили 'Солей Руаяль', чтобы спастись самому?!";
			link.l1 = "Мне пришлось это сделать месье...";
            link.l1.go = "Step_4_8";
        break;
		case "Step_4_8":
            dialog.text = "Мне все ясно - вы форменный трус, капитан. Прошу вас удалиться немедленно, я более не желаю вас видеть. Может быть, когда-нибудь вы сможете рассчитывать на мою благосклонность, но не в ближайшее время...";
			link.l1 = "Хорошо, месье, я понял.";
            link.l1.go = "Step_4_check";
			pchar.questTemp.State = "QuestLineBreake";
			bWorldAlivePause   = false; // Конец линейки
			CloseQuestHeader("Fra_Line_4_SoleiRoyal");
            DeleteAttribute(pchar, "questTemp.Waiting_time");
			ChangeCharacterReputation(pchar, -10);
			AddCharacterExpToSkill(pchar, "Sailing", -250);
			AddCharacterExpToSkill(pchar, "Grappling", -250);
			AddCharacterExpToSkill(pchar, "Defence", -250);
			AddCharacterExpToSkill(pchar, "Cannons", -250);
			//слухи
			AddSimpleRumour("Ходят слухи, что наш генерал-губернатор выставил вас из резиденции за то, что вы бросили в бою 'Сулей Руаяль'. Хех, поделом...", FRANCE, 5, 1);
        break;
		case "Step_4_9":
            dialog.text = "Черт возьми, вы не сумели защитить 'Солей Руаяль'... Это катастрофа...";
			link.l1 = "Месье генерал-губернатор, я сделал все, что мог...";
            link.l1.go = "Step_4_10";
        break;
		case "Step_4_10":
            dialog.text = "Очень плохо старались, раз не смогли выполнить элементарного поручения. Вы топите испанцев десятками, а тут не смогли все утрясти! Я разочарован в вас в крайней степени и более не желаю иметь с вами дело!\n"+
				          "А теперь оставьте меня!";
			link.l1 = "Хорошо, месье...";
            link.l1.go = "Step_4_check";
			pchar.questTemp.State = "QuestLineBreake";
			bWorldAlivePause   = false; // Конец линейки
			CloseQuestHeader("Fra_Line_4_SoleiRoyal");
            DeleteAttribute(pchar, "questTemp.Waiting_time");
			ChangeCharacterReputation(pchar, -10);
			ChangeCharacterNationReputation(pchar, sti(NPChar.nation), -2);
			AddCharacterExpToSkill(pchar, "Sailing", -250);
			AddCharacterExpToSkill(pchar, "Grappling", -250);
			AddCharacterExpToSkill(pchar, "Defence", -250);
			AddCharacterExpToSkill(pchar, "Cannons", -250);
			//слухи
			AddSimpleRumour("Ходят слухи, что наш генерал-губернатор выставил вас из резиденции за то, что вы умудрились утопить 'Сулей Руаяль'. Хех, поделом...", FRANCE, 5, 1);
        break;
		case "Step_4_11":
            dialog.text = "Хм, непредвиденные обстоятельства... Что же, вы приняли правильное решение, сударь. Я поздравляю вас с удачным завершением очередного дела и прошу принять вознаграждение - 28000 золотых.";
			link.l1 = "Спасибо, месье генерал-губернатор...";
            link.l1.go = "Step_4_12";
        break;
		case "Step_4_12":
            dialog.text = "Я прошу вас быть у меня в резиденции через неделю-другую, возможно, у меня найдется для вас новое дело.";
			link.l1 = "Хорошо, сударь. Буду у вас всенепременно.";
            link.l1.go = "Step_4_check";
			AddMoneyToCharacter(pchar, 28000);  
            pchar.questTemp.Waiting_time = "8";
        break;
		case "Step_4_check":
			if (CheckAttribute(pchar, "questTemp.AnnaIsLife"))
			{
				pchar.questTemp.CurQuestNumber = "5";
				DeleteAttribute(pchar, "questTemp.AnnaIsLife");
			}
			else pchar.questTemp.CurQuestNumber = "6";
			SaveCurrentQuestDateParam("questTemp");
			pchar.questTemp.State = "empty";
			CloseQuestHeader("Fra_Line_4_SoleiRoyal");
			AddTitleNextRate(sti(NPChar.nation), 3);
			ChangeCharacterReputation(pchar, 5);
			ChangeCharacterNationReputation(pchar, sti(NPChar.nation), 1);
			AddCharacterExpToSkill(pchar, "Sailing", 180);
			AddCharacterExpToSkill(pchar, "Grappling", 200);
			AddCharacterExpToSkill(pchar, "Defence", 170);
			AddCharacterExpToSkill(pchar, "Cannons", 220);
			AddCharacterExpToSkill(pchar, "Fortune", 150);
			//слухи
			AddSimpleRumour("Ходят слухи, что при сопровождении 'Сулей Руаяля' на вас напали испанцы! Позвольте выразить вам свое восхищение - вы прекрасный моряк и настоящий воин!", FRANCE, 5, 1);
			DialogExit();
        break;
        //********************** Квест №6. Доставка письма Олоне ************************	
		case "Step_6_1":
            dialog.text = "Именно он. И учтите, что это письмо ни при каких обстоятельствах не должно оказаться в чужих руках. Если вы попадете в переделку, то даже погибнуть не имеете права прежде, чем уничтожите пакет.";
			link.l1 = "Вы полагаете, найдутся желающие посмотреть, что в этом письме?";
            link.l1.go = "Step_6_2";
        break;
		case "Step_6_2":
            dialog.text = "В любопытных недостатка никогда не было. Отправляйтесь немедля, награду вам выдаст сам месье Давид.";
			link.l1 = "Хорошо, месье генерал-губернатор. Надеюсь, она будет достойной.";
            link.l1.go = "exit";
			LAi_group_Delete("PeaceGroup");
    		pchar.questTemp.State = "Fr5Olone_toGuadeloupe";
            AddQuestRecord("Fra_Line_6_Olone", "1");
            ChangeItemDescribe("letter_1", "itmdescr_letter_1_FraLineQ6");
            GiveItem2Character(pchar,"letter_1");
			Pchar.quest.Fr6Olone_GuadeloupeBattle.win_condition.l1 = "location";
			Pchar.quest.Fr6Olone_GuadeloupeBattle.win_condition.l1.location = "Guadeloupe";
			Pchar.quest.Fr6Olone_GuadeloupeBattle.win_condition = "Fr6Olone_GuadeloupeBattle";
			//==> Олоне
            sld = GetCharacter(NPC_GenerateCharacter("Olone", "BigPirate", "man", "man", 45, FRANCE, -1, false));
            FantomMakeCoolSailor(sld, SHIP_FRIGATE, "Кровавый демон", CANNON_TYPE_CULVERINE_LBS32, 90, 80, 80);
			FantomMakeCoolFighter(sld, 45, 100, 90, "blade28", "pistol5", 200);
			sld.name = "Франсуа";
			sld.lastname = "Олоне";
            sld.Dialog.Filename = "Quest\FraLineNpc_1.c";
			sld.CompanionEnemyEnable = false; 
			sld.Abordage.Enable = false; // НЕЛЬЗЯ!
			sld.RebirthPhantom = true;
			ChangeCharacterReputation(sld, -100);
            LAi_SetWarriorType(sld);
			LAi_group_MoveCharacter(sld, "PeaceGroup");
            ChangeCharacterAddressGroup(sld, "BasTer_houseF1", "goto", "goto5");
        break;
		case "Step_6_3":
            dialog.text = "Что случилось?";
			link.l1 = "Месье генерал-губернатор, мы с Олоне договорились взять Куману... Взяли, но потом вышла ссора... В общем, Олоне мертв, сударь...";
            link.l1.go = "Step_6_4";
        break;
		case "Step_6_4":
            dialog.text = "Вот это да... И кто же этот герой, что осмелился бросить вызов самому Франсуа Олоне?";
			link.l1 = "Месье, это ваш покорный слуга...";
            link.l1.go = "Step_6_5";
        break;
		case "Step_6_5":
            dialog.text = "Да уж, вы, сударь, отчаянный головорез! Олоне - демон во плоти.\n"+
				          "Не сказал бы, что мне его смерть по душе, но и печалиться особо не буду. Тем более, что такого рода схватки не редкость среди Берегового Братства.";
			link.l1 = "Это верно, месье генерал-губернатор.";
            link.l1.go = "Step_6_6";
        break;
		case "Step_6_6":
            dialog.text = "Ну что же, я вас не задерживаю более. Пока у меня нет для вас дела, но вы можете заходить ко мне время от времени...";
			link.l1 = "Хорошо, сударь, именно так и поступлю.";
            link.l1.go = "exit";
            pchar.questTemp.State = "empty";
            SaveCurrentQuestDateParam("questTemp");
            pchar.questTemp.CurQuestNumber = "7";
            pchar.questTemp.Waiting_time = "30";
			CloseQuestHeader("Fra_Line_6_Olone");
			AddTitleNextRate(sti(NPChar.nation), 1);
			ChangeCharacterReputation(pchar, -2);
			ChangeCharacterNationReputation(pchar, sti(NPChar.nation), -1);
			AddCharacterExpToSkill(pchar, "Repair", 170);
			AddCharacterExpToSkill(pchar, "Sneak", -230);
			AddCharacterExpToSkill(pchar, "Fencing", -230);
			//слухи
			AddSimpleRumour("Ходят слухи, что вы расправились с самим Франсуа Олоне! Да уж, скажу я вам, отчаянной вы храбрости человек...", FRANCE, 5, 1);
        break;
		case "Step_6_7":
            dialog.text = "О, это прекрасно. По сути своей, именно взятия Куманы я и добивался. Ну что же, прекрасная работа, друг мой. Я вполне доволен вашей расторопностью.";
			link.l1 = "Спасибо за добрые слова в мой адрес, сударь. За сим разрешите откланяться - дела...";
            link.l1.go = "Step_6_8";
        break;
		case "Step_6_8":
            dialog.text = "Да, конечно, не смею вас задерживать, капитан. Но не забывайте заглядывать в мою резиденцию время от времени.";
			link.l1 = "Хорошо, месье генерал-губернатор.";
            link.l1.go = "exit";
            pchar.questTemp.State = "empty";
            SaveCurrentQuestDateParam("questTemp");
            pchar.questTemp.CurQuestNumber = "7";
            pchar.questTemp.Waiting_time = "7";
			CloseQuestHeader("Fra_Line_6_Olone");
			AddTitleNextRate(sti(NPChar.nation), 4);
			ChangeCharacterReputation(pchar, 6);
			AddCharacterExpToSkill(pchar, "Grappling", 220);
			AddCharacterExpToSkill(pchar, "Sneak", 160);
			AddCharacterExpToSkill(pchar, "Defence", 280);
			AddCharacterExpToSkill(pchar, "Accuracy", 190);
			AddCharacterExpToSkill(pchar, "Fencing", 150);
			AddCharacterExpToSkill(pchar, "Fortune", 250);
			ChangeCharacterNationReputation(pchar, sti(NPChar.nation), 2);
			//слухи
			AddSimpleRumour("По городу ходят слухи, что вы разграбили Куману с Франсуа Олоне. Испанские доны, видимо, здорово обделались! ", FRANCE, 5, 1);
        break;
		case "Step_6_9":
            dialog.text = "Что предпринял Олоне в связи с депешей?";
			link.l1 = "Он предложил мне поход на Куману. К сожалению, я вынужден был отказаться. Однако, полагаю, что он и без меня справится, имея в составе своей эскадры три корабля.";
            link.l1.go = "Step_6_10";
        break;
		case "Step_6_10":
            dialog.text = "Ну что же, будем надеяться, что мой план сработает... Вас я больше не смею задерживаться сударь. Не забывайте бывать у меня время от времени, вы можете понадобиться.";
			link.l1 = "Хорошо, месье генерал-губернатор.";
            link.l1.go = "exit";
            pchar.questTemp.State = "empty";
            SaveCurrentQuestDateParam("questTemp");
            pchar.questTemp.CurQuestNumber = "7";
            pchar.questTemp.Waiting_time = "15";
			CloseQuestHeader("Fra_Line_6_Olone");
			AddTitleNextRate(sti(NPChar.nation), 2);	
			ChangeCharacterReputation(pchar, 2);
			AddCharacterExpToSkill(pchar, "Sneak", 190);
			AddCharacterExpToSkill(pchar, "Defence", 150);
			AddCharacterExpToSkill(pchar, "Accuracy", 220);
			ChangeCharacterNationReputation(pchar, sti(NPChar.nation), 1);
        break;
        //********************** Квест №7. Вызволение из тюрьмы Рока Бральзильца ************************	
		case "Step_7_1":
            dialog.text = "К испанцам в лапы попал известный флибустьер Рок Бразилец. Милости ему ждать не приходится, поскольку он преизрядно пощипал испанцев. Разумеется, прежде чем познакомить Рока с конопляной тетушкой, они захотят выяснить, где он припрятал отобранное у них золото. Но это даже не самое главное.";
			link.l1 = "Золото - не главное?! А что же тогда?";
            link.l1.go = "Step_7_2";
        break;
		case "Step_7_2":
            dialog.text = "Святые отцы инквизиции умеют развязывать языки, и я опасаюсь, что Бразилец расскажет не только про золото...";
			link.l1 = "... но и про письма, которые, как я полагаю, доставляются по вашему поручению не только Жану Давиду...";
            link.l1.go = "Step_7_3";
        break;
		case "Step_7_3":
            dialog.text = "Совершенно верно. Так что вы понимаете: действовать нужно без промедления. Мне донесли, что Рока отправили в гаванскую тюрьму. Вы должны пробраться туда как можно быстрее и вытащить его из плена любой ценой. Я хочу, чтобы Бразилец был здесь, и по возможности живой и здоровый.";
			link.l1 = "Но если он погибнет...";
            link.l1.go = "Step_7_4";
        break;
		case "Step_7_4":
            dialog.text = "Тогда испанцы ничего не узнают, но не узнаем и мы. А у меня есть основания полагать, что Рок в благодарность за спасение предоставит во владение правительству некоторую часть своих сокровищ...";
			link.l1 = "Задание непростое, но как раз по мне! Я отправляюсь немедленно.";
            link.l1.go = "Step_7_5";
        break;
		case "Step_7_5":
            dialog.text = "Очень хорошо. И еще - в городе вам необходимо действовать скрытно, по возможности не поднимая тревоги. Для этого я вновь даю вам верительную грамоту Испанской торговой компании.";
			link.l1 = "Огромное спасибо, месье генерал-губернатор.";
            link.l1.go = "exit";
			LAi_group_Delete("PeaceGroup");
			GiveNationLicence(SPAIN, 20);
			SetNationRelation2MainCharacter(SPAIN, RELATION_NEUTRAL);
    		pchar.questTemp.State = "Fr7RockBras_toSeekPlace";
            AddQuestRecord("Fra_Line_7_RockBras", "1");
			SaveCurrentQuestDateParam("questTemp");
			//==> Бразильца - в инквизицию. 
            sld = GetCharacter(NPC_GenerateCharacter("RockBrasilian", "bocman", "man", "man2", 30, PIRATE, -1, false));
			FantomMakeCoolFighter(sld, 30, 90, 90, "", "", 80);
            sld.Dialog.Filename    = "Quest\FraLineNpc_2.c";
			sld.RebirthPhantom = true;
        	sld.name 	= "Рок";
        	sld.lastname 	= "Бразилец";
			LAi_LoginInCaptureTown(sld, true);
			LAi_NoRebirthEnable(sld);
			LAi_SetLoginTime(sld, 0.0, 24.0);
			LAi_SetActorType(sld); 
			LAi_SetPoorType(sld);
			LAi_group_MoveCharacter(sld, "PeaceGroup");
			ChangeCharacterAddressGroup(sld, "Santiago_Incquisitio", "prison", "prison9");
        break;
		case "Step_7_6":
            dialog.text = "Хм, бесполезно в вашем исполнении, месье. Не говорите за весь Новый Свет.\n"+
				          "Очень жаль, сударь, что вы не сумели выполнить задания, очень жаль... Пожалуй, вы мне больше не нужны. Вы можете заглядывать ко мне в резиденцию периодически, возможно, и для вас найдется работа...";
			link.l1 = "Хорошо, месье генерал-губернатор.";
            link.l1.go = "Step_7_exit";	
			ChangeCharacterReputation(pchar, -10);
			ChangeCharacterNationReputation(pchar, sti(NPChar.nation), -1);
			AddCharacterExpToSkill(pchar, "Repair", -170);
			AddCharacterExpToSkill(pchar, "Defence", -130);
			AddCharacterExpToSkill(pchar, "Cannons", -150);
			//слухи
			AddSimpleRumour("Говорят, что капитан " + GetFullName(pchar) + " не сумел вытащить Рока Бразильца из испанских застенков. Плохо, он был хорошим человеком...", FRANCE, 5, 1);
            pchar.questTemp.Waiting_time = "35";
        break;
		case "Step_7_7":
            dialog.text = "М-да, печально... Он был хорошим капером и настоящим человеком. Очень жаль.\n"+
				          "Ну что же, я полагаю, вы сделали все возможное для его спасения. Поэтому я приглашаю вас быть у меня в резиденции через пару недель - я найду для вас новое дело.";
			link.l1 = "Хорошо, сударь, буду непременно.";
            link.l1.go = "Step_7_exit";
			AddTitleNextRate(sti(NPChar.nation), 1);
			ChangeCharacterReputation(pchar, 1);
			ChangeCharacterNationReputation(pchar, sti(NPChar.nation), 1);
			AddCharacterExpToSkill(pchar, "Repair", 100);
			AddCharacterExpToSkill(pchar, "Defence", 100);
			AddCharacterExpToSkill(pchar, "Cannons", 100);
			//слухи
			AddSimpleRumour("Говорят, что капитан " + GetFullName(pchar) + " не сумел вытащить Рока Бразильца из испанских застенков. Плохо, он был хорошим человеком...", FRANCE, 5, 1);
            pchar.questTemp.Waiting_time = "15";
        break;
		case "Step_7_exit":
            pchar.questTemp.State = "empty";
            SaveCurrentQuestDateParam("questTemp");
            pchar.questTemp.CurQuestNumber = "8";
			CloseQuestHeader("Fra_Line_7_RockBras");
			TakeNationLicence(SPAIN);
			SetNationRelation2MainCharacter(SPAIN, RELATION_ENEMY);
            sld = characterFromId("RockBrasilian");
			DeleteAttribute(sld, "RebirthPhantom");
         	sld.LifeDay = 0; // уберем нпс
			DialogExit();
        break;
		case "Step_7_8":
            dialog.text = "Не скромничайте, капитан. Наградой за выполненную работу является 30 тысяч монет. Извольте получить.";
			link.l1 = "Спасибо, месье генерал-губернатор...";
            link.l1.go = "Step_7_9";
        break;
		case "Step_7_9":
            dialog.text = "Я, как всегда, прошу вас заглядывать в мою резиденцию время от времени, полагаю, что в ближайшее время вы будете мне нужны.";
			link.l1 = "Хорошо, сударь, обязательно буду навещать вас.";
            link.l1.go = "Step_7_Rock";
			AddMoneyToCharacter(pchar, 30000);
            pchar.questTemp.State = "empty";
            SaveCurrentQuestDateParam("questTemp");
            pchar.questTemp.CurQuestNumber = "8";
			CloseQuestHeader("Fra_Line_7_RockBras");
            TakeNationLicence(SPAIN);
			SetNationRelation2MainCharacter(SPAIN, RELATION_ENEMY);	
			AddTitleNextRate(sti(NPChar.nation), 2);
			ChangeCharacterReputation(pchar, 6);
			ChangeCharacterNationReputation(pchar, sti(NPChar.nation), 1);
			AddCharacterExpToSkill(pchar, "Repair", 200);
			AddCharacterExpToSkill(pchar, "Sneak", 300);
			AddCharacterExpToSkill(pchar, "Cannons", 250);
			AddCharacterExpToSkill(pchar, "Accuracy", 350);
			//слухи
			AddSimpleRumour("Говорят, что капитан " + GetFullName(pchar) + " вытащил Рока Бразильца из испанских застенков. Радостная весть, скажу я вам, Рока здесь уважают.", FRANCE, 5, 1);
        break;
		case "Step_7_Rock":
			sld = characterFromId("RockBrasilian");
			RemovePassenger(PChar, sld);
			LAi_SetActorType(sld);
			LAi_CharacterEnableDialog(sld);
			LAi_ActorDialog(sld, pchar, "", 0, 0);
			DialogExit();
        break;
        //********************** Квест №8. Встреча с тремя корсарами ************************	
		case "Step_8_1":
            dialog.text = "Пока только в том, чтобы как можно скорее явиться к маркизу. Подробности мне неизвестны, судя по всему, дело будет весьма секретным.";
			link.l1 = "Что ж, мне не в первой. Никаких проблем, я отправляюсь...";
            link.l1.go = "exit";
    		pchar.questTemp.State = "Fr8ThreeCorsairs_toBonrepo";
            AddQuestRecord("Fra_Line_8_ThreeCorsairs", "1");
			//==> маркиз Бонрепо.
            sld = GetCharacter(NPC_GenerateCharacter("Bonrepo", "huber_fra", "man", "man", 20, FRANCE, -1, false));
            sld.Dialog.Filename    = "Quest\FraLineNpc_1.c";
        	sld.name 	= "Маркиз";
        	sld.lastname 	= "Бонрепо";
			LAi_SetStayType(sld);
			LAi_group_MoveCharacter(sld, "FRANCE_CITIZENS");
			ChangeCharacterAddressGroup(sld, "BasTer_townhall", "goto", "governor1");
        break;
		case "Step_8_2":
            dialog.text = "Капитан, не все в этой жизни измеряется деньгами. Честь дворянина и уважение окружающих не купить ни за какие деньги.";
			link.l1 = "Это верно, месье...";
            link.l1.go = "Step_8_3";
        break;
		case "Step_8_3":
            dialog.text = "Ну что же, пока новых просьб к вам у меня нет, но я буду счастлив, если вы будете время от времени навещать меня - возможно, нам еще предстоят совместные дела!";
			link.l1 = "Хорошо, сударь. Я, как французский дворянин, барон, буду у бывать у вас в резиденции чаще.";
            link.l1.go = "exit";
            pchar.questTemp.State = "empty";
			pchar.questTemp.Waiting_time = "12";
            SaveCurrentQuestDateParam("questTemp");
            pchar.questTemp.CurQuestNumber = "9";
			CloseQuestHeader("Fra_Line_8_ThreeCorsairs");
			AddTitleNextRate(sti(NPChar.nation), 3);
			ChangeCharacterReputation(pchar, 5);
			AddCharacterExpToSkill(pchar, "Sailing", 290);
			AddCharacterExpToSkill(pchar, "Leadership", -100);
			AddCharacterExpToSkill(pchar, "Commerce", 250);
			AddCharacterExpToSkill(pchar, "Pistol", 350);
			//слухи
			AddSimpleRumour("Говорят, что капитану " + GetMainCharacterNameDat() + " пожалован дворянский титул барона! Ну, дела...", FRANCE, 5, 1);
        break;
        //********************** Квест №9. Отбить нападение на Порт-о-Принс ************************	
		case "Step_9_1":
            dialog.text = "Это так, барон. Они уже напали на поселение буканьеров Ла Вега, теперь, по всей видимости, очередь за Порт-о-Принсом.";
			link.l1 = "Я понял, сударь. Я могу приступать к выполнению задачи?";
            link.l1.go = "Step_9_11";
        break;
		case "Step_9_11":
            dialog.text = "В принципе, да. Однако, мне нужно довести до вас следующее - 'Солей Руаяль' придается вам в качестве усиления эскадры.\n"+
				          "Вы ни в коем случае не должны потерять этот корабль, это будет означать провал миссии. И вы не сможете больше работать на французское правительство.";
			link.l1 = "Хм, ну что же, я приму все меры к тому, чтобы великолепный 'Солей Руаяль' серьезно не пострадал.";
            link.l1.go = "exit";
    		pchar.questTemp.State = "Fr9GuardPP_toPortPax";
            AddQuestRecord("Fra_Line_9_GuardPortPax", "1");
			sld = GetCharacter(NPC_GenerateCharacter("SoleiRoyalCaptain", "off_fra_2", "man", "man", 35, FRANCE, -1, true));
			sld.Ship.Type = CreateBaseShip(SHIP_SOLEYRU);
			sld.Ship.Name = "Сулей Руаяль";
			SetBaseShipData(sld);
			SetCrewQuantityFull(sld);
			Fantom_SetBalls(sld, "pirate");	
			FantomMakeCoolFighter(sld, 35, 80, 70, BLADE_LONG, "pistol3", 100);	
			sld.CompanionEnemyEnable = false;
			sld.Abordage.Enable = false; // НЕЛЬЗЯ!
			SetCompanionIndex(pchar, -1, sti(sld.index));
			SetShipRemovable(sld, false);
			pchar.TempPerks.QuestTroopers = true; //перк квестового десанта
			SetTimerCondition("Fr9GuardPP_Late", 0, 0, 5, false);
            Pchar.quest.Fr9GuardPP_Fight.win_condition.l1 = "location";
            Pchar.quest.Fr9GuardPP_Fight.win_condition.l1.location = "Hispaniola2";
            Pchar.quest.Fr9GuardPP_Fight.win_condition = "Fr9GuardPP_Fight";
        break;
		case "Step_9_2":
            dialog.text = "Верно, барон, и выполнена блестяще. Французское правительство предусмотрело определенную сумму на ремонт кораблей и пополнение личного состава - 5 тысяч золотых... Извольте получить.";
			link.l1 = "Всего 5 тысяч?! Каррамба, это меньше, чем ничего!";
            link.l1.go = "Step_9_3";
        break;
		case "Step_9_3":
            dialog.text = "Барон, эта война требует от всех нас максимально напряжения сил. Я прошу вас понять ситуацию - денег катастрофически не хватает. Поэтому прошу вас принять сложившуюся ситуацию как данность.\n"+
				          "Сейчас я вас задерживать не смею - подготовьтесь к очередным боевым действиям. И помните - вы нужны Франции!";
			link.l1 = "Гм, ну что же, если я так нужен Франции - она меня получит. Буду у вас в резиденции после того, как приведу в порядок свои дела, сударь. До свидания.";
            link.l1.go = "exit";
            pchar.questTemp.State = "empty";
			pchar.questTemp.Waiting_time = "18";
            SaveCurrentQuestDateParam("questTemp");
            pchar.questTemp.CurQuestNumber = "10";
			CloseQuestHeader("Fra_Line_9_GuardPortPax");
			AddTitleNextRate(sti(NPChar.nation), 2);
			ChangeCharacterReputation(pchar, 7);
			ChangeCharacterNationReputation(pchar, sti(NPChar.nation), 1);
			AddCharacterExpToSkill(pchar, "Repair", 270);
			AddCharacterExpToSkill(pchar, "Sneak", 260);
			AddCharacterExpToSkill(pchar, "Cannons", 300);
			AddCharacterExpToSkill(pchar, "FencingLight", 270);
			AddCharacterExpToSkill(pchar, "FencingHeavy", 230);
			AddCharacterExpToSkill(pchar, "Fencing", 250);
			//слухи
			AddSimpleRumour("Говорят, что капитан " + GetFullName(pchar) + " спас жителей Порт-о-Принс от испанцев!", FRANCE, 5, 1);
        break;
		case "Step_9_4":
            dialog.text = "Но вы потеряли 'Солей Руаяль', капитан, и этому нет оправдания!";
			link.l1 = "Я сделал все, что было в моих силах...";
            link.l1.go = "Step_9_5";
        break;
		case "Step_9_5":
            dialog.text = "Вы сделали недостаточно! В общем, потеря такого корабля привела к тому, что Франция отказывается от ваших услуг, сударь. Прощайте.";
			link.l1 = "И это после всего, что я сделал для Франции?! Прощайте, сударь, мне и самому все это уже изрядно надоело...";
            link.l1.go = "exit";
			pchar.questTemp.State = "QuestLineBreake";
			bWorldAlivePause   = false; // Конец линейки
			CloseQuestHeader("Fra_Line_9_GuardPortPax");
            DeleteAttribute(pchar, "questTemp.Waiting_time");
			ChangeCharacterReputation(pchar, -10);
			ChangeCharacterNationReputation(pchar, sti(NPChar.nation), -2);
			AddCharacterExpToSkill(pchar, "Repair", -270);
			AddCharacterExpToSkill(pchar, "Sneak", -200);
			AddCharacterExpToSkill(pchar, "FencingLight", -230);
			AddCharacterExpToSkill(pchar, "FencingHeavy", -230);
			AddCharacterExpToSkill(pchar, "Fencing", -230);
			//слухи
			AddSimpleRumour("Ходят слухи, что наш генерал-губернатор выставил вас из резиденции за то, что вы умудрились утопить 'Сулей Руаяль'. Хех, поделом...", FRANCE, 5, 1);
        break;
		case "Step_9_6":
            dialog.text = "Черт возьми, да вы просто форменный трус! Я не желаю иметь с вами ничего общего. Прощайте, барон...";
			link.l1 = "Прощайте, сударь, мой пламенный привет маркизу Бонрепо...";
            link.l1.go = "exit";
			pchar.questTemp.State = "QuestLineBreake";
			bWorldAlivePause   = false; // Конец линейки
			CloseQuestHeader("Fra_Line_9_GuardPortPax");
            DeleteAttribute(pchar, "questTemp.Waiting_time");
			ChangeCharacterReputation(pchar, -10);
			ChangeCharacterNationReputation(pchar, sti(NPChar.nation), -2);
			AddCharacterExpToSkill(pchar, "Repair", -270);
			AddCharacterExpToSkill(pchar, "Sneak", -200);
			AddCharacterExpToSkill(pchar, "FencingLight", -230);
			AddCharacterExpToSkill(pchar, "FencingHeavy", -230);
			AddCharacterExpToSkill(pchar, "Fencing", -230);
			//слухи
			AddSimpleRumour("Ходят слухи, что наш генерал-губернатор выставил вас из резиденции за то, что вы бросили в бою 'Сулей Руаяль'. Хех, поделом...", FRANCE, 5, 1);
        break;
		case "Step_9_7":
            dialog.text = "Дьявол, как же так?! Здесь два дня ходу!! В общем так, я не желаю иметь с вами ничего общего. Прощайте, барон...";
			link.l1 = "Прощайте, сударь, мой пламенный привет маркизу Бонрепо...";
            link.l1.go = "exit";
			sld = characterFromId("SoleiRoyalCaptain");
			RemoveCharacterCompanion(pchar, sld);
			sld.LifeDay = 0;
			pchar.questTemp.State = "QuestLineBreake";
			CloseQuestHeader("Fra_Line_9_GuardPortPax");
            DeleteAttribute(pchar, "questTemp.Waiting_time");
			bWorldAlivePause   = false; // Конец линейки
			CloseQuestHeader("Fra_Line_9_GuardPortPax");
			ChangeCharacterReputation(pchar, -10);
			ChangeCharacterNationReputation(pchar, sti(NPChar.nation), -2);
			AddCharacterExpToSkill(pchar, "Repair", -270);
			AddCharacterExpToSkill(pchar, "Sneak", -200);
			AddCharacterExpToSkill(pchar, "FencingLight", -230);
			AddCharacterExpToSkill(pchar, "FencingHeavy", -230);
			AddCharacterExpToSkill(pchar, "Fencing", -230);
			//слухи
			AddSimpleRumour("Ходят слухи, что наш генерал-губернатор выставил вас из резиденции за то, что вы не сумели вовремя прибыть к Порт-о-Принсу и город был разорен испанцами. Поделом вам!", FRANCE, 5, 1);
        break;
        //********************** Квест №10. Захват Санто-Доминго ************************	
		case "Step_10_1":
            dialog.text = "Настало время дать адекватный ответ на попытку захвата Порт-о-Принса. И, как мне кажется, лучшим ответом будет воплощение испанского плана захвата Эспаньолы, только в зеркальном отображении...";
			link.l1 = "Месье генерал-губернатор, прошу вас говорить менее витиевато.";
            link.l1.go = "Step_10_2";
        break;
		case "Step_10_2":
            dialog.text = "Хорошо, барон. Итак, я поручаю вам захватить Санто-Доминго! Таким образом, вся Эспаньола будет принадлежать целиком одной державе, но не Испании, а Франции. Теперь понятно?";
			link.l1 = "Понятно, сударь. Скажите, как насчет оплаты данной военной операции? Я не могу содержать 'Солей Руаяль' и вести войну за свой счет!";
            link.l1.go = "Step_10_3";
        break;
		case "Step_10_3":
            dialog.text = "Да, я понимаю. Вопрос по сути своей решен - после захвата Санто-Доминго вы получите очень весомое вознаграждение.";
			link.l1 = "Ну что же, сударь, я верю вам. Итак, я приступаю к операции, ждите меня с хорошими новостями.";
            link.l1.go = "exit";
    		pchar.questTemp.State = "Fr10OccupySD_toSantoDomingo";
            AddQuestRecord("Fra_Line_10_OccupySantoDomingo", "1");
			CloseQuestHeader("Fra_Line_12_EndOfWar");
            DeleteAttribute(pchar, "questTemp.Waiting_time");
			characters[GetCharacterIndex("SantoDomingo_Mayor")].dialog.captureNode = "FraLine10Quest_OccupySD"; //капитулянтская нода мэра
        break;
		case "Step_10_4":
            dialog.text = "Вы потеряли 'Солей Руаяль'!!";
			link.l1 = "Битва была жаркой, месье генерал-губернатор, корабль-красавец действительно пошел на дно. Мне очень жаль...";
            link.l1.go = "Step_10_5";
        break;
		case "Step_10_5":
            dialog.text = "Ваши сожаления мне не нужны. Потеря данного корабля - ужасная трагедия для Франции. Поэтому я больше не хочу иметь с вами дела, французское правительство более не поручит вам ничего. Прощайте.";
			link.l1 = "Хм, как знаете, сударь...";
            link.l1.go = "exit";
			CloseQuestHeader("Fra_Line_10_OccupySantoDomingo");
			pchar.questTemp.State = "QuestLineBreake";
			CloseQuestHeader("Fra_Line_12_EndOfWar");
            DeleteAttribute(pchar, "questTemp.Waiting_time");
			bWorldAlivePause   = false; // Конец линейки
			ChangeCharacterReputation(pchar, -10);
			ChangeCharacterNationReputation(pchar, sti(NPChar.nation), -1);
			AddCharacterExpToSkill(pchar, "Repair", -170);
			AddCharacterExpToSkill(pchar, "Sneak", -100);
			AddCharacterExpToSkill(pchar, "FencingLight", -130);
			AddCharacterExpToSkill(pchar, "FencingHeavy", -130);
			AddCharacterExpToSkill(pchar, "Fencing", -130);
			//слухи
			AddSimpleRumour("Ходят слухи, что наш генерал-губернатор выставил вас из резиденции за то, что вы умудрились утопить 'Сулей Руаяль'. Хех, поделом...", FRANCE, 5, 1);
        break;
		case "Step_10_6":
            dialog.text = "Прекрасно, друг мой, просто великолепно!\n"+
				          "Теперь о теме, очень важной для вас - о вознаграждении. Я сумел выбить из бюджета на финансирование вашей экспедиции неслыханную сумму - 40 тысяч золотых... Они ваши, барон!";
			link.l1 = "Отлично, месье генерал-губернатор. Уже гораздо лучше!";
            link.l1.go = "Step_10_7";
        break;
		case "Step_10_7":
            dialog.text = "Я очень рад, что вы довольны... Ну что же, задерживать вас я не смею более, прошу вас заняться подготовкой к другому делу. Приведите в порядок свою эскадру и будьте у меня примерно через месяц, барон.";
			link.l1 = "Хорошо, месье генерал-губернатор, буду в означенные сроки.";
            link.l1.go = "exit";
			AddMoneyToCharacter(pchar, 40000);
            pchar.questTemp.State = "empty";
			pchar.questTemp.Waiting_time = "30";
            SaveCurrentQuestDateParam("questTemp");
            pchar.questTemp.CurQuestNumber = "11";
			CloseQuestHeader("Fra_Line_10_OccupySantoDomingo");
			AddTitleNextRate(sti(NPChar.nation), 3);
			ChangeCharacterReputation(pchar, 7);
			ChangeCharacterNationReputation(pchar, sti(NPChar.nation), 1);
			AddCharacterExpToSkill(pchar, "Pistol", 330);
			AddCharacterExpToSkill(pchar, "Fortune", 360);
			AddCharacterExpToSkill(pchar, "Accuracy", 320);
			AddCharacterExpToSkill(pchar, "Grappling", 350);
			AddCharacterExpToSkill(pchar, "Defence", 350);
			//слухи
			AddSimpleRumour("Говорят, что вы захватили Санто-Доминго! Прекрасная работа, капитан, просто великолепная!", FRANCE, 5, 1);
        break;
        //********************** Квест №11. Захват Санта-Каталины ************************
		case "Step_11_1":
            dialog.text = "Верно, сударь.";
			link.l1 = "Понятно... И какой город станет французским через некоторое время?";
            link.l1.go = "Step_11_2";
        break;
		case "Step_11_2":
            dialog.text = "Санта-Каталина, что на Мейне. Этот город контролирует жемчужные промыслы на западном берегу Нового Света. Французское правительство полагает, что добыча жемчуга будет нам сейчас очень кстати...";
			link.l1 = "Совершенно согласен с французским правительством. Я готов к выполнению данной операции. Приступаю немедленно.";
            link.l1.go = "exit";
    		pchar.questTemp.State = "Fr11OccupySC_toSantaCatalina";
            AddQuestRecord("Fra_Line_11_OccupySantaCatalina", "1");
			characters[GetCharacterIndex("SantaCatalina_Mayor")].dialog.captureNode = "FraLine11Quest_OccupySC"; //капитулянтская нода мэра
        break;
		case "Step_11_3":
            dialog.text = "Дьявол, это очень и очень плохо. Потеря этого корабля делает невозможным наше сотрудничество в дальнейшем.";
			link.l1 = "Неужели он так вам дорог?";
            link.l1.go = "Step_11_4";
        break;
		case "Step_11_4":
            dialog.text = "Даже не во мне дело, 'Солей Руаяль' - гордость королевского флота Франции. Этот шедевр кораблестроения доверили вам, полагаясь на ваши способности флотоводца. Вы не оправдали высокого доверия, поэтому я вынужден отказать вам в дальнейшем сотрудничестве.";
			link.l1 = "Ну что же, быть посему. Меня так же изрядно утомили задачи, совершенно не обеспеченные финансами. Прощайте, сударь.";
            link.l1.go = "Step_11_2";
			CloseQuestHeader("Fra_Line_11_OccupySantaCatalina");
            DeleteAttribute(pchar, "questTemp.Waiting_time");
			pchar.questTemp.State = "QuestLineBreake";
			bWorldAlivePause   = false; // Конец линейки
			DeleteAttribute(pchar, "TempPerks.QuestTroopers"); //снятия перка квестового десанта
			AddCharacterExpToSkill(pchar, "Repair", -550);
			AddCharacterExpToSkill(pchar, "Pistol", -530);
			AddCharacterExpToSkill(pchar, "Fortune", -560);
			AddCharacterExpToSkill(pchar, "Accuracy", -520);
			AddCharacterExpToSkill(pchar, "Grappling", -550);
			ChangeCharacterReputation(pchar, -10);
			ChangeCharacterNationReputation(pchar, sti(NPChar.nation), -2);
			//слухи
			AddSimpleRumour("Ходят слухи, что наш генерал-губернатор выставил вас из резиденции за то, что вы умудрились утопить 'Сулей Руаяль'. Хех, поделом...", FRANCE, 5, 1);
        break;
		case "Step_11_5":
            dialog.text = "Превосходно! Вы очередной раз доказали, что оказанное вам доверие совершенно оправдано.";
			link.l1 = "Месье генерал-губернатор, как насчет финансирования операции?";
            link.l1.go = "Step_11_6";
        break;
		case "Step_11_6":
            dialog.text = "Все трофеи, захваченные в Санта Каталине, по праву принадлежат вам.";
			link.l1 = "Я хотел бы обсудить вопрос о моем участии в разработке жемчужного промысла, месье...";
            link.l1.go = "Step_11_7";
        break;
		case "Step_11_7":
            dialog.text = "Это невозможно, сударь. Данный вопрос на контроле у Жана-Батиста Кольбера, премьер-министра Франции! Барон, негоже делать деньги в то время, когда Франция прилагает сверхусилия в этой войне.";
			link.l1 = "Хм, пожалуй, вы правы, сударь...";
            link.l1.go = "Step_11_8";
        break;
		case "Step_11_8":
            dialog.text = "Ну что же, я искренне рад, что мы хорошо понимаем друг друга. Барон, я не смею вас задерживать сейчас, прошу вас привести в порядок свою эскадру и быть готовым к новым подвигам на благо Франции и нашего великого Короля-Солнце!";
			link.l1 = "Месье генерал-губернатор, я исполню вашу просьбу. До встречи.";
            link.l1.go = "exit";
            pchar.questTemp.State = "empty";
			pchar.questTemp.Waiting_time = "10";
            SaveCurrentQuestDateParam("questTemp");
            pchar.questTemp.CurQuestNumber = "12";
			DeleteAttribute(pchar, "TempPerks.QuestTroopers"); //снятия перка квестового десанта
			CloseQuestHeader("Fra_Line_11_OccupySantaCatalina");
			AddTitleNextRate(sti(NPChar.nation), 2);
			ChangeCharacterReputation(pchar, 6);
			ChangeCharacterNationReputation(pchar, sti(NPChar.nation), 5);
			AddCharacterExpToSkill(pchar, "Pistol", 430);
			AddCharacterExpToSkill(pchar, "Fortune", 460);
			AddCharacterExpToSkill(pchar, "Accuracy", 420);
			AddCharacterExpToSkill(pchar, "Grappling", 450);
			//слухи
			AddSimpleRumour("Ходят слухи, что вы захватили Санта Каталину. А потом пытались получить долю от жемчужных разработок у д'Ожерона. Хех, раскатал губу, называется...", FRANCE, 5, 1);
        break;
        //********************** Квест №12. Завершение линейки ************************
		case "Step_12_1":
            dialog.text = "Вы также внесли вклад в эту победу. По сути дела, именно вы обеспечили решительный разгром сил испанцев в Карибском море.\n"+
				          "Жан-Барист Кольбер отметил ваши удачные действия на докладе в Лувре, ваше имя теперь известно самому Людовику XIV!";
			link.l1 = "Я очень и очень рад. Скажите, месье, а денежных поощрений по итогам войны не предусмотрено?";
            link.l1.go = "Step_12_2";
        break;
		case "Step_12_2":
            dialog.text = "Об этом мне ничего не известно. Впрочем, мы можете задать этот вопрос маркизу Бонрепо. Ваше следующее задние - добраться до Бас-Тера, что на Гваделупе, и встретиться там с маркизом. Он ожидает вас.";
			link.l1 = "Отлично, месье! Этого шанса я не упущу... Я выступаю немедленно.";
            link.l1.go = "Step_12_3";
        break;
		case "Step_12_3":
            dialog.text = "И приведите в порядок 'Солей Руаяль', он должен быть в полной боевой готовости.";
			link.l1 = "Хорошо, сударь.";
            link.l1.go = "exit";
			pchar.questTemp.State = "Fr12EndWar_toBorepo";
			AddQuestRecord("Fra_Line_12_EndOfWar", "1");
			//==> маркиз Бонрепо.
            sld = GetCharacter(NPC_GenerateCharacter("Bonrepo", "huber_fra", "man", "man", 20, FRANCE, -1, false));
            sld.Dialog.Filename    = "Quest\FraLineNpc_1.c";
        	sld.name 	= "Маркиз";
        	sld.lastname 	= "Бонрепо";
			LAi_SetStayType(sld);
			LAi_group_MoveCharacter(sld, "FRANCE_CITIZENS");
			ChangeCharacterAddressGroup(sld, "BasTer_townhall", "goto", "governor1");
        break;
	}
	UnloadSegment(NPChar.FileDialog2);  // если где-то выход внутри switch  по return не забыть сделать анлод
}
