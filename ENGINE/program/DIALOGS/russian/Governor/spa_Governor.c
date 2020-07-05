// диалог по городам
void ProcessCommonDialogEvent(ref NPChar, aref Link, aref NextDiag)
{
    ref sld;
    int i, iTemp, iMoney, ShipType, Rank;
    switch (Dialog.CurrentNode)
	{
		case "quests":
            dialog.text = RandPhraseSimple("Какие вопросы?", "Что вам угодно?");
			link.l1 = RandPhraseSimple("Я передумал...", "Сейчас мне не о чем говорить");
		    link.l1.go = "exit";
		break;

		case "work_1":  // работа на благо короны - линейка нации
            // сюда вход только с патентом, проверка выше
            dialog.text = LinkRandPhrase("Вы находитесь на службе Испании! Прошу немедленно приступить к выполнению возложенного на вас поручения!",
                                         "Немедленно приступайте к выполнению взятых на себя обязательств!",
                                         "Я жду вашего доклада о выполнении моего ответственного поручения, но не разговоров.");
            link.l1 = RandPhraseSimple("Приступаю немедленно, сеньор генерал-губернатор", "Я уже в процессе выполнения...");
            link.l1.go = "exit";

        	switch (pchar.questTemp.State)   // что делаем в данный момент
            {
                case "empty":   // Нет взятых квестов
                    if (GetQuestPastDayParam("questTemp") < sti(pchar.questTemp.Waiting_time))
                    {
                        dialog.text = LinkRandPhrase("В данный момент у меня нет для вас ответственных поручений. Вы можете появиться у меня в резиденции позже...",
                                                     "Прошу меня извинить, я очень сильно занят в данный момент!",
                                                     "Занимайтесь своими делами до поры, сейчас у меня нет для вас заданий.");
                        link.l1 = "Хорошо, сеньор генерал-губернатор.";
                        link.l1.go = "exit";
                    }
                    else
                    {
						switch(pchar.questTemp.CurQuestNumber)   // Взятие квестов начиная со второго
                        {
                            case "2":
                                dialog.text = "А-а-а, вот и вы, " + pchar.name + "! Рассказывают, что вы проявили редкое мужество и находчивость! Рад сообщить, что спасенные вами люди уже находятся на пути в Кастилию. Их благодарность буквально не знает границ. Наши друзья в Порт-Рояле сообщают прямо-таки фантастические вещи, говорят, будто вы изрубили английских тюремщиков в куски!";
                                link.l1 = "У страха глаза велики, дон Франсиско...";
                                link.l1.go = "Step_2_1";
                            break;
                            case "3":
                                dialog.text = "Рад видеть вас вновь, друг мой. Хочу предложить вам одно дело, и полагаю, что оно будет вам по вкусу.";
                                link.l1 = "Я весь внимание.";
                                link.l1.go = "Step_3_1";
                            break;
                            case "4":
                                dialog.text = "Послушайте, у меня весьма необычное задание на этот раз.";
                                link.l1 = "Внимательно слушаю, дон Франсиско.";
                                link.l1.go = "Step_4_1";
                            break;
                            case "5":
                                dialog.text = "Я командирую вас в распоряжение губернатора Сантьяго Хосе Санчо Хименеса.";
                                link.l1 = "Что я должен буду делать?";
                                link.l1.go = "Step_5_1";
                            break;
                            case "6":
                                dialog.text = "У меня есть для вас задание. Давайте немного предыстории...";
                                link.l1 = "Слушаю внимательно, дон Франсиско.";
                                link.l1.go = "Step_6_1";
                            break;
                            case "7":
                                dialog.text = "Наш славный корсар Маноэль Риверо Пардаль сейчас рыщет у берегов Антигуа в поисках британских торговых судов. До меня дошла информация, что на его перехват отправились французские флибустьеры под командованием Моисея Воклейна. Поспешите к Антигуа и окажите необходимую помощь Пардалю. Он - символ нашего мужества!";
                                link.l1 = "Я понял, дон Франсиско. Приступаю немедленно.";
                                link.l1.go = "exit";
								pchar.questTemp.State = "Sp7SavePardal_toAntigua";
								AddQuestRecord("Spa_Line_7_SavePardal", "1");
								Pchar.quest.Sp7SavePardal_FightNearAntigua.win_condition.l1 = "location";
								Pchar.quest.Sp7SavePardal_FightNearAntigua.win_condition.l1.location = "Antigua";
								Pchar.quest.Sp7SavePardal_FightNearAntigua.win_condition = "Sp7SavePardal_FightNearAntigua";
							break;
                            case "8":
                                dialog.text = "Очень хорошо, что вы появились так вовремя. У меня есть очень важное задание для вас. Дело в том, что известный флибустьер капитан Ансель сумел объединить английских и французских каперов с целью нападения на Куману.";
                                link.l1 = "Дон Франсиско, а вы располагаете информацией о численности и характере данной эскадры?";
                                link.l1.go = "Step_8_1";
                            break;
                            case "9":
                                dialog.text = "Вам надлежит немедленно отправиться в Порто Белло. Там вас ожидает губернатор города, он поставит вам задачу.";
                                link.l1 = "Сеньор, не могли бы вы в общих чертах описать, что мне предстоит сделать?";
                                link.l1.go = "Step_9_1";
                            break;
                            case "10":
                                dialog.text = "Я прошу вас немедленно отправиться к губернатору города Маракайбо для выполнения ответственного поручения.";
                                link.l1 = "Опять, дон Франсиско?";
                                link.l1.go = "Step_10_1";
                            break;
                            case "11":
                                dialog.text = "Вы готовы к серьезной операции по штурму укрепленных городов?";
                                link.l1 = "Всегда готов, сеньор генерал-губернатор";
                                link.l1.go = "Step_11_1";
                            break;
                            case "12":
                                dialog.text = "А-а-а, вот и вы, капитан! Я ждал вас...";
                                link.l1 = "Всегда к вашим услугам, дон Франсиско.";
                                link.l1.go = "Step_12_1";
                            break;
						}
                    }
                break;
                
                case "":           // Квест №1, Спасти горожан от Моргана.
                    dialog.text = "Так-так, капитан, я был проинформирован о Вашем прибытии.";
                    link.l1 = "Для меня это большая честь, дон Франсиско. Никак не ожидал, что моя скромная персона удостоится внимания столь высокопоставленного сеньора.";
                    link.l1.go = "Step_1_1";
                break;
                case "SaveFromMorgan_OutOfPrison":
                    dialog.text = "Докладывайте, что у вас с вызволением пленных.";
                    link.l1 = "Ваше приказание выполнено, сеньор генерал-губернатор.";
                    link.l1.go = "Step_1_7";
                break;
                case "Inquisition_backBad":
                    dialog.text = "А-а-а, вот вы и вернулись, очень хорошо. Как прошло дело, которое вам поручила Святая Инквизиция?";
                    link.l1 = "Не совсем хорошо, дон Хуан. Но ничего страшного не произошло, что, собственно, и радует.";
                    link.l1.go = "Step_2_3";
                break;
                case "Inquisition_backPartGood":
                    dialog.text = "А-а-а, вот вы и вернулись, очень хорошо. Как прошло дело, которое вам поручила Святая Инквизиция?";
                    link.l1 = "Более или менее нормально, дон Хуан. Задние выполнено частично, что тоже, собственно, неплохо.";
                    link.l1.go = "Step_2_3";
                break;
                case "Inquisition_backAllGood":
                    dialog.text = "А-а-а, вот вы и вернулись, очень хорошо. Как прошло дело, которое вам поручила Святая Инквизиция?";
                    link.l1 = "Успешно в крайней степени, дон Франсиско. Все сделано в наилучшем виде, Святая Инквизиция рада донельзя.";
                    link.l1.go = "Step_2_3";
                break;
                case "TakeRockBras_RockSink":
                    dialog.text = "Так, ну что вы мне скажете по поводу Рока Бразильца?";
                    link.l1 = "Я атаковал его корвет в прибрежных водах Маракайбо, где он охотился на наши галеоны. Сожалею, дон Франсиско, но взять его живым мне не удалось, он утонул вместе со своим кораблем...";
                    link.l1.go = "Step_3_6";
                break;
                case "TakeRockBras_DieHardRock":
                    dialog.text = "Так, ну что вы мне скажете по поводу Рока Бразильца?";
                    link.l1 = "Я намеревался атаковать его корвет в прибрежных водах Маракайбо, где он охотился на наши галеоны. К сожалению, по причинам, от меня не зависящим, мне пришлось покинуть воды, где находился Рок Бразилец. Он ушел от меня...";
                    link.l1.go = "Step_3_6";
                break;
                case "TakeRockBras_RockTaken":
                    dialog.text = "Так, ну что вы мне скажете по поводу Рока Бразильца?";
                    link.l1 = "Он у меня в трюме, дон Франсиско.";
                    link.l1.go = "Step_3_7";
                break;
                case "TakeRockBras_RockGiven":
                    dialog.text = "Да-да, меня уже известили об успешной доставке Бразильца в Сантьяго. Могу поздравить.";
                    link.l1 = "Спасибо, дон Франсиско. Чем я еще могу быть вам полезен?";
                    link.l1.go = "Step_3_8";
                break;
                case "TakeRockBras_WaitTwoDays":
                if (GetQuestPastDayParam("questTemp") > 2)
                    {
                        dialog.text = "Итак, как я и предполагал, хваленый корсар не смог долго выносить методы допроса с пристрастием, принятые в Святой Инквизиции.";
                        link.l1 = "Так его подвергли пыткам?";
                        link.l1.go = "Step_3_9";
                    }
                    else
                    {
                        dialog.text = "Я просил вас зайти через два дня, не раньше.";
                        link.l1 = "Простите, сеньор генерал-губернатор.";
                        link.l1.go = "exit";
                    }
                break;
                case "TakeRockBras_SeekRocksMoney":
                    dialog.text = "Ну что, докладывайте, что вам удалось разыскать из кладов Бразильца?";
                    link.l1 = "Пока ничего нового, сеньор генерал-губернатор. Но я в поиске...";
                    link.l1.go = "exit";
                    if (sti(pchar.money) > 500000)
                    {
                        link.l2 = "Дон Франсиско, я готов отчитаться, поиски завершены.";
                        link.l2.go = "Step_3_10";
                    }
                break;
                case "Sp4Detection_toMirderPlace":
                    dialog.text = "Вы что-нибудь сумели разузнать об этом деле?";
                    link.l1 = "Я работаю, дон Франсиско, но пока ничего узнать не удалось...";
                    link.l1.go = "exit";
                break;
                case "Sp4Detection_toGovernor":
                    dialog.text = "Докладывайте, что вы узнали об этом убийстве.";
                    link.l1 = "Мне удалось узнать, что предполагаемый убийца по описанию похож на ладрона. Полагаю, что следы преступления ведут на Тортугу.";
                    link.l1.go = "Step_4_6";
                break;
                case "Sp4Detection_toDEstre":
                    dialog.text = "Жду с нетерпением вашего доклада о расследовании на Тортуге";
                    link.l1 = "Дон Франсиско, на Тортуге я узнал о том, что некая донна Анна находится у Бертрана д'Ожерона на содержании. Можно предположить, что это и есть донна Анна Рамирес де Лейва, но заявлять это с уверенностью я не могу.";
                    link.l1.go = "Step_4_9";
                break;
                case "Sp4Detection_toDEstre_1":
                    dialog.text = "Жду с нетерпением вашего доклада о расследовании на Тортуге";
                    link.l1 = "Дон Франсиско, на Тортуге я узнал о том, что некая донна Анна находится у Бертрана д'Ожерона на содержании. Можно предположить, что это и есть донна Анна Рамирес де Лейва, но заявлять это с уверенностью я не могу.";
                    link.l1.go = "Step_4_9";
                break;
                case "Sp4Detection_toDEstre_2":
                    dialog.text = "Жду с нетерпением вашего доклада о расследовании на Тортуге";
                    link.l1 = "Дон Франсиско, на Тортуге я узнал о том, что некая донна Анна находится у Бертрана д'Ожерона на содержании. К тому же, мне удалось настигнуть доверенное лицо генерал-губернатора - некоего Анри д'Эстре.";
                    link.l1.go = "Step_4_10";
                break;
                case "Sp4Detection_DEstreIsDead":
                    dialog.text = "Докладывайте, как продвигается ваше расследование?";
                    link.l1 = "Я все узнал. Дело в том, что жена дона Хосе Рамиреса де Лейва по собственному желанию сбежала к генерал-губернатору Тортуги д'Ожерону.";
                    link.l1.go = "Step_4_14";
                break;
                case "Sp5LaVegaAttack_BackToHavana":
                    dialog.text = "Да-да, я уже знаю о вашем новом успешном предприятии. Что же, я не сомневался, что место на Эспаньоле для расселения наших колонистов из Старого Света будет должным образом очищено.";
                    link.l1 = "С буканьерами покончено, дон Франсиско. А Эдвард Мэнсфилд был убит лично мной!";
                    link.l1.go = "Step_5_3";
                break;
                case "Sp6TakeMess_waitMessanger":
                    dialog.text = "Та-а-ак, рад видеть доблестного капитана.";
                    link.l1 = "И я вас также рад видеть, дон Франсиско.";
                    link.l1.go = "Step_6_4";
                break;
                case "Sp6TakeMess_Action":
                    dialog.text = "Та-а-ак, рад видеть доблестного капитана.";
                    link.l1 = "И я вас также рад видеть, дон Франсиско.";
                    link.l1.go = "Step_6_4";
                break;
                case "Sp7SavePardal_PardalIsSink":
                    dialog.text = "Докладывайте о результатах миссии.";
                    link.l1 = "Сеньор, я не смог спасти Пардаля, его галеоны потоплены многократно превосходящими силами. Сам Маноэль погиб...";
                    link.l1.go = "Step_7_1";
                break;
                case "Sp7SavePardal_GoodWork":
                    dialog.text = "Докладывайте о результатах миссии.";
                    link.l1 = "Сеньор, информация, полученная вами, оказалась достоверной. Я шел к Антигуа на всех парусах и подоспел к Пардалю как нельзя вовремя - два его галеона уже вели бой с превосходящими силами французских пиратов. Совместными усилиями нам удалось одолеть сильного врага, корабли флибустьеров пошли ко дну!";
                    link.l1.go = "Step_7_3";
                break;
                case "Sp7SavePardal_2GoodWork":
                    dialog.text = "Докладывайте о результатах миссии.";
                    link.l1 = "Сеньор, информация, полученная вами, оказалась достоверной. Я шел к Антигуа на всех парусах и подоспел к Пардалю как нельзя вовремя - два его галеона уже вели бой с превосходящими силами французских пиратов. Совместными усилиями нам удалось одолеть сильного врага, корабли флибустьеров пошли ко дну!";
                    link.l1.go = "Step_7_3";
                break;
                case "Sp7SavePardal_DieHard":
                    dialog.text = "Докладывайте о результатах миссии.";
                    link.l1 = "Сеньор генерал-губернатор, к сожалению, это задача оказалась мне не по зубам. Я не смог помочь Пардалю.";
                    link.l1.go = "Step_7_6";
                break;
                case "Sp8SaveCumana_YouLate":
                    dialog.text = "Часть беженцев из Куманы уже прибыла сюда. Вы не сумели спасти город и его жителей.";
                    link.l1 = "Я мчался к Кумане на всех парусах, нигде не задерживаясь даже на минуту. Но все равно не успел вовремя, сделать это было просто невозможно.";
                    link.l1.go = "Step_8_5";
					pchar.questTemp.Waiting_time = "20";
                break;
                case "Sp8SaveCumana_DieHard":
                    dialog.text = "Часть беженцев из Куманы уже прибыла сюда. Вы не сумели спасти город и его жителей.";
                    link.l1 = "Собранная эскадра ладронов была сильна настолько, что справиться с ней мы не смогли. С теми силами, которыми я располагал, сделать это было просто невозможно.";
                    link.l1.go = "Step_8_5";
					pchar.questTemp.Waiting_time = "40";
                break;
                case "Sp8SaveCumana_GoodWork":
                    dialog.text = "Мне уже доложили, что Кумана спасена. Великолепно!";
                    link.l1 = "Мы сумели потопить флот ладронов. Никто не ушел живым...";
                    link.l1.go = "Step_8_6";
                break;
                case "Sp9SaveCumana_BackHavana":
                    dialog.text = "Докладывайте, что у вас произошло?";
                    link.l1 = "Дон Франсиско, я получил от генерал-губернатора Порто Белло задание сопроводить четыре золотых галеона до необитаемого острова Кайман...";
                    link.l1.go = "Step_9_2";
                break;
                case "Sp10Maracaibo_DieHard":
                    dialog.text = "Мне донесли, что Маракайбо захвачен и там установлен протекторат Англии!";
                    link.l1 = "Да, наверное, сеньор.";
                    link.l1.go = "Step_10_6";
                break;
                case "Sp10Maracaibo_GoodWork":
                    dialog.text = "Я уже знаю, вы блестяще справились с поставленной задачей!";
                    link.l1 = "На этот раз пришлось пустить ко дну корабли английских пиратов.";
                    link.l1.go = "Step_10_11";
                break;
                case "Sp10Maracaibo_2GoodWork":
                    dialog.text = "Я уже знаю, вы блестяще справились с поставленной задачей!";
                    link.l1 = "На этот раз пришлось пустить ко дну корабли английских пиратов.";
                    link.l1.go = "Step_10_9";
                break;
                case "DestrHolland_GoOn":
                    dialog.text = "Вы пока не сумели захватить ни Виллемстада, ни Мариго...";
                    link.l1 = "Сеньор генерал-губернатор, я этим занимаюсь. Задача непроста, требует серьезной подготовки.";
                    link.l1.go = "exit";
                    if (CheckAttribute(pchar, "questTemp.Q11_Villemstad"))
                    {
                        dialog.text = "Вы разорили Виллемстад, отлично! Теперь очередь за Мариго!";
                        link.l1 = "Точно так, дон Франсиско...";
                        link.l1.go = "exit";
                    }
                    if (CheckAttribute(pchar, "questTemp.Q11_Marigo"))
                    {
                        dialog.text = "Превосходно, Мариго разорен! Но я жду от вас так же захвата и разорения Виллемстада.";
                        link.l1 = "Я все сделаю, сеньор губернатор.";
                        link.l1.go = "exit";
                    }
                    if (CheckAttribute(pchar, "questTemp.Q11_Villemstad") && CheckAttribute(pchar, "questTemp.Q11_Marigo"))
                    {
                        dialog.text = "Мариго и Виллемстад взяты штурмом и разорены! Прекрасная работа, друг мой!";
                        link.l1 = "Дело сделано, дон Франсиско...";
                        link.l1.go = "Step_11_3";
                    }
                break;
                case "OccupyPortPax_WeWin":
                    dialog.text = "Ну что же, Эскориал можно поздравить с достойным приобретением. Остров Эспаньола - наш!";
                    link.l1 = "Точно так, сеньор генерал-губернатор. Мне удалось захватить Порт-о-Принс.";
                    link.l1.go = "Step_12_4";
                break;
                case "QuestLineBreake":
                    dialog.text = "Я не имею дело с людьми, нарушающими свое слово. Мне больше нечего вам сказать.";
                    link.l1 = "...";
                    link.l1.go = "exit";
                    bWorldAlivePause   = false; // Конец линейки
                break;
                case "EndOfQuestLine":
                    dialog.text = "Дон " + pchar.name + ", Эскориал предоставляет вам полную свободу действий в интересах Испании. Действуйте самостоятельно.";
                    link.l1 = "Хорошо, дон Франсиско.";
                    link.l1.go = "exit";
                    bWorldAlivePause   = false; // Конец линейки
                break;
			}
        break;
        //********************** Квест №1, Спасти горожан от Моргана. ************************
        case "Step_1_1":
            dialog.text = "Мое внимание можно заслужить делами на благо великой Кастилии, коих у вас, капитан, пока нет.\n"+
                          "Но у вас есть корабль и испанский патент, что немаловажно для моей страны. Особенно сейчас, когда враги Испании подняли голову и пытаются взять реванш за все свои прошлые поражения.";
            link.l1 = "Неужели положение Испании настолько серьезно?";
            link.l1.go = "Step_1_2";
        break;
        case "Step_1_2":
            dialog.text = "Да, хотя на первый взгляд все в порядке. Богатство и могущество Испании всегда внушали зависть европейским монархиям. Сейчас же, когда наши враги не гнушаются привлекать бандитов всех мастей, положение стало удручающе тяжелым.\n"+
                          "Участились нападения на наши колонии и торговые караваны, похищения знатных и богатых граждан с целью получения выкупа стали рядовым явлением. Пираты буквально заполонили все торговые пути, что сделало плавание в этих водах очень опасным.\n"+
                          "Эскориал в бешенстве, ее величество королева-мать требует незамедлительных и решительных действий по пресечению незаконной деятельности ладронов всех мастей. Для достижения сей цели принято решение бороться с врагом его же методами, то есть нанимать каперов на службу короне.";
            link.l1 = "Похоже, что я вовремя...";
            link.l1.go = "Step_1_3";
        break;
        case "Step_1_3":
            dialog.text = "Похоже, что так оно и есть. Еще вчера такой субъект, как вы, мог рассчитывать только на костер Святой Инквизиции или пеньковую веревку. Но времена изменились. Докажите свою преданность Господу нашему и Кастилии, и будете иметь все - богатство, славу, почести и награды.";
            link.l1 = "Звучит неплохо. Я готов приступать непосредственно к выполнению работы.";
            link.l1.go = "Step_1_4";
        break;
        case "Step_1_4":
            dialog.text = "Ну, тогда приступим к делу.\n"+
                          "Английскими пиратами захвачены три наших уважаемых гражданина. А выкуп, который запрашивает известный своим коварством Генри Морган, превышает все разумные и мыслимые пределы. Этот бандит требует 500 тысяч золотом! Вам нужно будет пробраться в Порт-Рояль, вызволить из тюрьмы наших граждан и в целости доставить их ко мне.\n"+
                          "Вы должны скрытно пробраться в тюрьму и неожиданно напасть на охрану. Пленных стерегут очень тщательно, так что о побеге станет известно незамедлительно. В связи с этим высадку предпочтительнее произвести в укромной бухте, вдали от форта. Ни при каких обстоятельствах не подвергайте опасности жизни вверенных вам людей, это категорическое требование.\n"+
                          "Вам все понятно?";
            link.l1 = "Да, сеньор генерал-губернатор.";
            link.l1.go = "Step_1_5";
        break;
        case "Step_1_5":
            dialog.text = "У вас есть месяц на освобождение пленных, не больше. Готовьтесь и отправляйтесь в путь.";
            link.l1 = "Немедленно приступаю.";
            link.l1.go = "Step_1_6";
        break;
        case "Step_1_6":
            SaveCurrentQuestDateParam("questTemp");
    		// остальные линейки в сад -->
    		pchar.questTemp.NationQuest = SPAIN;
    		// остальные линейки в сад <--
    		pchar.questTemp.State = "SaveFromMorgan_toPrison";
            AddQuestRecord("Spa_Line_1_SaveCitizens", "1");
            for (i=1; i<=3; i++)
            {
                sld = GetCharacter(NPC_GenerateCharacter("SpaPrisoner"+i, "usurer_"+i, "man", "man", 10, SPAIN, -1, false));
                sld.Dialog.Filename = "Quest\SpaLineNpc_1.c";
                LAi_SetCitizenType(sld);
            	LAi_group_MoveCharacter(sld, "SPAIN_CITIZENS");
                ChangeCharacterAddressGroup(sld, "PortRoyal_Prison", "goto", "goto9");
            }
            NextDiag.CurrentNode = NextDiag.TempNode;
            DialogExit();
        break;
        case "Step_1_7":
            dialog.text = "Ну что же, вами сделан первый успешный шаг на тернистом пути карьеры для ее Величества Королевы-матери Анны Австрийской. Получите честно заработанные 50 тысяч. Рад, что не ошибся в вас.";
            link.l1 = "Благодарю, господин генерал-губернатор. Кстати, вы прекрасный математик.";
            link.l1.go = "Step_1_8";
        break;
        case "Step_1_8":
            dialog.text = "О чем вы?";
            link.l1 = "Ну как же, если к моим полученным 50-ти тысячам добавить один '0', то выйдет сумма, которую вы не захотели отдавать Генри Моргану.";
            link.l1.go = "Step_1_9";
        break;
        case "Step_1_9":
            dialog.text = "А-а-а, вы про это. Мы, конечно, можем себе позволить внести такой баснословный выкуп, доказывая тем самым, что нам дороги наши граждане, но тогда 50 тысяч не оттягивали бы так уютно ваш карман, не так ли?";
            link.l1 = "О да, сеньор. Вы привели в крайней степени поучительный пример!";
            link.l1.go = "Step_1_10";
        break;
        case "Step_1_10":
            dialog.text = "В таком случае, не смею вас больше задерживать... Наведывайтесь ко мне в резиденцию время от времени.";
            link.l1 = "Хорошо, сеньор.";
            link.l1.go = "exit";
            pchar.questTemp.State = "empty";
            SaveCurrentQuestDateParam("questTemp");
            pchar.questTemp.CurQuestNumber = "2";
            pchar.questTemp.Waiting_time = "7";
            CloseQuestHeader("Spa_Line_1_SaveCitizens");
            AddMoneyToCharacter(pchar, 50000);
            for (i=1; i<=3; i++)
            {
                sld = characterFromId("SpaPrisoner"+i);
                RemovePassenger(pchar, sld);
         	    sld.LifeDay = 0; // уберем нпс
            }
			AddTitleNextRate(sti(NPChar.nation), 1);
			AddCharacterExpToSkill(pchar, "Leadership", 100);
			AddCharacterExpToSkill(pchar, "Pistol", 150);
			AddCharacterExpToSkill(pchar, "Grappling", 120);
			ChangeCharacterReputation(pchar, 5);
			ChangeCharacterNationReputation(pchar, sti(NPChar.nation), 1);
			//слухи
			AddSimpleRumour("Вы знаете, наш славный генерал-губернатор Орегон-и-Гаскон нанял капитана " + GetMainCharacterNameGen() + " для освобождения наших граждан, захваченных этим дьяволом Морганом. И вы знаете, поручение было успешно выполнено.", SPAIN, 5, 1);
		break;
        //********************** Квест №2, задания Инквизиции. ************************
        case "Step_2_1":
            dialog.text = "Ваша скромность делает вам честь... Ну что же, пришло время для новой работы. К нам прибыл высокопоставленный иезуит Антониу де Соуза, он попросил предоставить в его распоряжение хладнокровного, мужественного и честного офицера.\n"+
                          "Я порекомендовал вас. Отправляйтесь в Сантьяго, там находится управление Инквизиции по Карибскому морю, и найдите де Соуза. Делайте все, что он прикажет, не задумываясь ни о чем. У вас есть вопросы?";
            link.l1 = "Нет, сеньор генерал-губернатор.";
            link.l1.go = "Step_2_2";
        break;
        case "Step_2_2":
            dialog.text = "Тогда ступайте.";
            link.l1 = "...";
            link.l1.go = "exit";
    		pchar.questTemp.State = "Inquisition_toDeSouza";
            AddQuestRecord("Spa_Line_2_Inquisition", "1");
            SaveCurrentQuestDateParam("questTemp");
            sld = GetCharacter(NPC_GenerateCharacter("AntonioDeSouza", "priest", "man", "man", 10, SPAIN, -1, false));
        	sld.name 	= "Антониу";
        	sld.lastname = "де Соуза";
			sld.model.animation = "man2";
            sld.Dialog.Filename = "Quest\AntonioDeSouza.c";
            LAi_SetHuberType(sld);
        	LAi_group_MoveCharacter(sld, "SPAIN_CITIZENS");
            ChangeCharacterAddressGroup(sld, "Santiago_Incquisitio", "sit", "armchair1");
        break;
        case "Step_2_3":
            dialog.text = "Ну вот и славно. Вы отдохните пару недель от трудов праведных на благо Церкви, а потом прошу ко мне на прием. Полагаю, что найду вам новое дело.";
            link.l1 = "Всегда к вашим услугам, сеньор!";
            link.l1.go = "exit";
            pchar.questTemp.State = "empty";
            SaveCurrentQuestDateParam("questTemp");
            pchar.questTemp.CurQuestNumber = "3";
            pchar.questTemp.Waiting_time = "15";
            CloseQuestHeader("Spa_Line_2_Inquisition");
			AddCharacterExpToSkill(pchar, "Sneak", 150);
			AddCharacterExpToSkill(pchar, "Sailing", 100);
			AddCharacterExpToSkill(pchar, "FencingLight", 150);
			AddCharacterExpToSkill(pchar, "Fencing", 150);
			AddCharacterExpToSkill(pchar, "FencingHeavy", 120);
			AddTitleNextRate(sti(NPChar.nation), 1);
			ChangeCharacterReputation(pchar, 2);
			ChangeCharacterNationReputation(pchar, sti(NPChar.nation), 1);
			//слухи
			AddSimpleRumour("Ходят слухи, что капитан " + GetFullName(pchar) + " подрядился выполнять поручения самого де Соуза! Инквизиция... При одном этом слове - дрожь по телу.", SPAIN, 5, 1);
        break;
        //********************** Квест №3, захватить Рока Бразица. ************************
        case "Step_3_1":
            dialog.text = "Итак, вы что-нибудь знаете о ладроне Роке Бразильце?";
            link.l1 = "Хм, известная личность, очень опасный капер. Ошивается на Тортуге.";
            link.l1.go = "Step_3_2";
        break;
        case "Step_3_2":
            dialog.text = "Да, это так. Настоящая головная боль для нас.\n"+
                          "Так вот, я намерен избавится от этой мигрени сам и избавить от нее наших колонистов. Я хочу, чтобы вы захватили этого пресловутого Рока Бразильца. И что очень важно - доставили его живым.";
            link.l1 = "А это обязательно? Драка будет знатная, в этом можно быть уверенным.";
            link.l1.go = "Step_3_3";
        break;
        case "Step_3_3":
            dialog.text = "И что? Я не очень понимаю вас, сеньор " + pchar.name + ". Вы испугались?!";
            link.l1 = "Дон Франсиско, я не боюсь ни черта, ни дьявола. Полагаю, что вы имели возможность в этом убедиться! Но я имею в виду то, что не могу контролировать абордаж полностью. В такой рубке может погибнуть кто угодно.";
            link.l1.go = "Step_3_4";
        break;
        case "Step_3_4":
            dialog.text = "Я это понимаю. Но все же повторю - Рок Бразилец нужен мне живым. Вернее, не мне, а хорошо известному вам монсеньору Антониу де Соуза. Это требование Святой Инквизиции, вы понимаете, насколько это важно для нас?";
            link.l1 = "Да, сеньор, понимаю. Ну что же, я могу обещать вам, что предприму все возможное для успешной реализации данного поручения в полном объеме.";
            link.l1.go = "Step_3_5";
        break;
        case "Step_3_5":
            dialog.text = "Превосходно! Жду, когда ваш корабль с Роком Бразильцем в трюме станет на рейд в бухте Сантьяго.";
            link.l1 = "До свидания, сеньор.";
            link.l1.go = "exit";
            pchar.questTemp.State = "TakeRockBras_toTortuga";
            AddQuestRecord("Spa_Line_3_RockBrasilian", "1");
        break;
        case "Step_3_6":
            dialog.text = "Хм, это плохо. Я разочарован в вас.\n"+
                          "Сейчас прошу оставить меня одного... Однако, вы можете бывать у меня на приеме время от времени, возможно, и для вас найдется дело...";
            link.l1 = "Я понял, сеньор генерал-губернатор. Спасибо...";
            link.l1.go = "exit";
            pchar.questTemp.State = "empty";
            SaveCurrentQuestDateParam("questTemp");
            pchar.questTemp.CurQuestNumber = "4";
            pchar.questTemp.Waiting_time = "30";
            CloseQuestHeader("Spa_Line_3_RockBrasilian");
			ChangeCharacterReputation(pchar, -3);
			ChangeCharacterNationReputation(pchar, sti(NPChar.nation), -1);
			AddCharacterExpToSkill(pchar, "Sneak", -100);
			AddCharacterExpToSkill(pchar, "Sailing", -100);
			//слухи
			AddSimpleRumour("Вы слышали, капитан " + GetFullName(pchar) + " провалил задание генерал-губернатора - не смог захватить этого жуткого ладрона Рока Бразильца. Неудачник, одним словом...", SPAIN, 5, 1);
        break;
        case "Step_3_7":
            dialog.text = "Отлично! Теперь отправляйтесь в Сантьяго и сдайте его монсеньору Антонио де Соуза.";
            link.l1 = "Хорошо, сеньор, будет исполнено.";
            link.l1.go = "exit";
        break;
        case "Step_3_8":
            dialog.text = "Зайдите ко мне через пару дней, монсеньор Антонио должен поработать над этим Бразильцем, должны быть новости.";
            link.l1 = "Хорошо, буду у вас через два дня, сеньор.";
            link.l1.go = "exit";
            AddQuestRecord("Spa_Line_3_RockBrasilian", "9");
            pchar.questTemp.State = "TakeRockBras_WaitTwoDays";
            SaveCurrentQuestDateParam("questTemp");
        break;
        case "Step_3_9":
            dialog.text = "Конечно, не мораль же ему монсеньор де Соуза читал! Рок Бразилец получил ровно то, чего заслуживал.\n"+
                          "Он не раз пытал испанских граждан ради спрятанных ими денег, ну а теперь под пытками Святой Инквизиции Рок выдал местонахождение собственного клада. Он разделил и спрятал его в нескольких места на Кубе, Эспаньоле и в окрестностях Белиза.\n"+
                          "Вы должны найти и доставить мне схроны Бразильца. По предварительным оценкам в них, в общей сложности, около 500000 пиастров!";
            link.l1 = "Неплохо, пожалуй! Сеньор, а точного местонахождения клада удалось добиться?";
            link.l1.go = "Step_3_13";
        break;
        case "Step_3_13":
            dialog.text = "Нет, к сожалению...";
            link.l1 = "Может, стоит немного подождать и монсеньор Антонио даст необходимые сведения?";
            link.l1.go = "Step_3_14";
        break;
        case "Step_3_14":
            dialog.text = "Это уже невозможно - Рок Бразилец отдал душу Господу нашему. Аминь...";
            link.l1 = "Аминь... Сеньор, я могу рассчитывать на часть найденных сокровищ?";
            link.l1.go = "Step_3_15";
        break;
        case "Step_3_15":
            dialog.text = "Бесспорно. Пятая часть найденных пиастров Бразильца - ваша. И не носите мне все это в отдельности, приходите сразу, когда наберете требуемую сумму - 500 тысяч.";
            link.l1 = "Хорошо, сеньор генерал-губернатор, я немедленно приступаю к поискам.";
            link.l1.go = "exit";
         	pchar.GenQuestBox.Cuba_Grot = true;
            pchar.GenQuestBox.Cuba_Grot.box1.items.gold = 150000;
            pchar.GenQuestBox.Cuba_Grot.box1.items.indian17 = 1;
            pchar.GenQuestBox.Cuba_Grot.box1.items.indian10 = 1;
            pchar.GenQuestBox.Cuba_Grot.box1.items.indian18 = 1;
         	pchar.GenQuestBox.Hispaniola_Cave = true;
            pchar.GenQuestBox.Hispaniola_Cave.box2.items.gold = 150000;
            pchar.GenQuestBox.Hispaniola_Cave.box2.items.jewelry5 = 250;
            pchar.GenQuestBox.Hispaniola_Cave.box2.items.jewelry14= 10;
            pchar.GenQuestBox.Hispaniola_Cave.box2.items.potion3 = 5;
            pchar.GenQuestBox.Hispaniola_Cave.box2.items.mineral8 = 1;
            pchar.GenQuestBox.Hispaniola_Cave.box2.items.jewelry7 = 5;
            pchar.GenQuestBox.Hispaniola_Cave.box2.items.jewelry2 = 20;
         	pchar.GenQuestBox.Beliz_Cave = true;
            pchar.GenQuestBox.Beliz_Cave.box1.items.gold = 50000;
            pchar.GenQuestBox.Beliz_Cave.box1.items.indian6 = 1;
            pchar.GenQuestBox.Beliz_Cave.box1.items.indian11 = 1;
            pchar.GenQuestBox.Beliz_Cave.box1.items.indian15 = 1;
            pchar.GenQuestBox.Beliz_Cave.box1.items.indian12 = 1;
            pchar.GenQuestBox.Beliz_Cave.box2.items.gold = 150000;
            pchar.GenQuestBox.Beliz_Cave.box2.items.potion5 = 10;
            pchar.GenQuestBox.Beliz_Cave.box2.items.potionrum = 2;
            pchar.GenQuestBox.Beliz_Cave.box2.items.potion4 = 10;
            pchar.questTemp.State = "TakeRockBras_SeekRocksMoney";
            AddQuestRecord("Spa_Line_3_RockBrasilian", "8");
        break;
        case "Step_3_10":
            dialog.text = "Великолепно! Теперь давайте подведем итог... Вы вносите в казну Эскориала 500 тысяч, следовательно, ваша доля составляет 100 тысяч пиастров.";
            link.l1 = "Благодарю, сеньор генерал-губернатор.";
            link.l1.go = "Step_3_11";
        break;
        case "Step_3_11":
            dialog.text = "Хорошо... Сейчас я не смею вас задерживать, но через неделю прошу быть у меня на приеме.";
            link.l1 = "Хорошо, дон Франсиско, буду у вас в указанное время. А сейчас разрешите откланяться...";
            link.l1.go = "exit";
            AddMoneyToCharacter(pchar, -400000);
            pchar.questTemp.State = "empty";
            SaveCurrentQuestDateParam("questTemp");
            pchar.questTemp.CurQuestNumber = "4";
            pchar.questTemp.Waiting_time = "5";
            CloseQuestHeader("Spa_Line_3_RockBrasilian");
			AddTitleNextRate(sti(NPChar.nation), 1);
			ChangeCharacterReputation(pchar, 5);
			ChangeCharacterNationReputation(pchar, sti(NPChar.nation), 1);
			AddCharacterExpToSkill(pchar, "Leadership", 150);
			AddCharacterExpToSkill(pchar, "Repair", 200);
			AddCharacterExpToSkill(pchar, "Sailing", 150);
			AddCharacterExpToSkill(pchar, "Repair", 250);
			//слухи
			AddSimpleRumour("Вы не слышали прекрасную новость? Некий капитан " + GetFullName(pchar) + " захватил этого мерзкого ладрона Рока Бразильца. Хвала Господу Всемогущему!", SPAIN, 5, 1);
        break;
        //********************** Квест №4, расследование убийства. ************************
        case "Step_4_1":
            dialog.text = "Гавана взбудоражена странным преступлением. Самое странное то, что совершенно непонятно, как это произошло.\n"+
                          "Комендант Гаваны, дон Хосе Рамирес де Лейва, был убит в собственном доме вместе с четырьмя слугами. Все были в полной боевой амуниции, как будто собрались на войну.";
            link.l1 = "Вот это да! И никто ничего не слышал? Кухарка, например?";
            link.l1.go = "Step_4_2";
        break;
        case "Step_4_2":
            dialog.text = "Нет, в доме кроме его жены больше никого не было. Накануне дон Хосе учинил жуткий скандал с женой и отправил всех слуг женского пола восвояси.";
            link.l1 = "А что же говорит его жена?";
            link.l1.go = "Step_4_3";
        break;
        case "Step_4_3":
            dialog.text = "Она исчезла...";
            link.l1 = "Как исчезла, совсем?";
            link.l1.go = "Step_4_4";
        break;
        case "Step_4_4":
            dialog.text = "Нет, частично... Думайте о том, что говорите!";
            link.l1 = "Прошу прощения, дон Франсиско. Странно все это... Вы знаете, это не мой профиль, я не сыщик. Пусть этим занимается комендатура.";
            link.l1.go = "Step_4_5";
        break;
        case "Step_4_5":
            dialog.text = "Дон Хосе и был комендантом Гаваны. Послушайте, я не могу вас заставить заниматься расследованием преступлений, но именно сейчас я прошу сделать это.\n"+
                          "Город взволнован, сами понимаете. Да еще это исчезновение... Мы обыскали все вокруг - безрезультатно. Мне нужны эффективные меры в расследовании немедленно, и вы - наилучшая кандидатура на данный момент.";
            link.l1 = "Хорошо, сеньор генерал-губернатор. Я сделаю все, что смогу.";
            link.l1.go = "exit";
            pchar.questTemp.State = "Sp4Detection_toMirderPlace";
            AddQuestRecord("Spa_Line_4_MirderDetection", "1");
         	pchar.GenQuestBox.Havana_houseS1Bedroom = true;
            pchar.GenQuestBox.Havana_houseS1Bedroom.box2.items.letter_notes = 1;
        break;
        case "Step_4_6":
            dialog.text = "Тортуга - прибежище бандитов всех мастей... А что донна Анна?";
            link.l1 = "Сложно предположить... Но вот что я нашел на месте преступления - недописанное письмо...";
            link.l1.go = "Step_4_7";
        break;
        case "Step_4_7":
            dialog.text = "(читает) Хм, даже не знаю, что сказать... Вам нужно проникнуть на Тортугу и разобраться, что к чему.";
            link.l1 = "Хорошо, дон Франсиско, я все сделаю.";
            link.l1.go = "Step_4_8";
            TakeItemFromCharacter(pchar, "letter_notes");
        break;
        case "Step_4_8":
            dialog.text = "Вручаю вам лицензию Французской Вест-Индской компании, сроком действия на 50 дней. Данный документ позволит вам беспрепятственно передвигаться по Тортуге. И надеюсь, вы догадаетесь поднять французский флаг на судне...";
            link.l1 = "Конечно, дон Франсиско, это очень кстати. Пожалуй, я приступлю к выполнению задачи.";
            link.l1.go = "exit";
            GiveNationLicence(FRANCE, 50);
            pchar.questTemp.State = "Sp4Detection_toTortuga";
            AddQuestRecord("Spa_Line_4_MirderDetection", "4");
            sld = GetCharacter(NPC_GenerateCharacter("ServantDEstre", "Barmen_13", "man", "man", 10, FRANCE, -1, false));
        	sld.name 	= "Гвидо";
        	sld.lastname = "";
            sld.Dialog.Filename = "Quest\SpaLineNpc_1.c";
            LAi_SetStayType(sld);
        	LAi_group_MoveCharacter(sld, "FRANCE_CITIZENS");
            ChangeCharacterAddressGroup(sld, "Tortuga_houseS2", "goto", "goto2");
        break;
        case "Step_4_9":
            dialog.text = "Одни предположения... Хм, этого мало, чтобы сделать твердые выводы.\n"+
                          "Я недоволен вами. Попрошу вас оставить меня одного, у меня нет дел для вас.\n"+
                          "Впрочем, вы можете заходить ко мне на прием переодически, возможно, что-нибудь незначительное и подвернется...";
            link.l1 = "Хорошо, сеньор генерал-губернатор...";
            link.l1.go = "exit";
            pchar.questTemp.State = "empty";
            SaveCurrentQuestDateParam("questTemp");
            pchar.questTemp.CurQuestNumber = "5";
            pchar.questTemp.Waiting_time = "30";
            CloseQuestHeader("Spa_Line_4_MirderDetection");
            TakeNationLicence(FRANCE);
			ChangeCharacterReputation(pchar, -10);
			ChangeCharacterNationReputation(pchar, sti(NPChar.nation), -1);
			AddCharacterExpToSkill(pchar, "Sailing", -250);
			AddCharacterExpToSkill(pchar, "Grappling", -250);
			AddCharacterExpToSkill(pchar, "Defence", -250);
			AddCharacterExpToSkill(pchar, "Cannons", -250);
			//слухи
			AddSimpleRumour("Вы знаете, наш генерал-губернатор поручил капитану " + GetMainCharacterNameDat() + " расследование убийства коменданта де Лейва. Однако, сыщиком капитан оказался некудышним...", SPAIN, 5, 1);
        break;
        case "Step_4_10":
            dialog.text = "Продолжайте...";
            link.l1 = "Я настиг этого флибустьера, но убить его мне не удалось - он очень силен, мне пришлось ретироваться.";
            link.l1.go = "Step_4_11";
        break;
        case "Step_4_11":
            dialog.text = "Оно и понятно. Убить пятерых вооруженных до зубов человек, готовых к встрече с ним - это вам не в чучело на плацу шпагой потыкать...";
            link.l1 = "Так вот, из разговора с ним мне удалось узнать, что пассия д'Ожерона - именно донна Анна Рамирес де Лейва. Вот, собственно, и все...";
            link.l1.go = "Step_4_12";
        break;
        case "Step_4_12":
            dialog.text = "Ну что же, если бы дон Хосе относился к донне Анне должным образом, то, полагаю, ничего бы этого не произошло. Нужно признать, что донна Анна здорово страдала от нравов своего супруга и порядков, которые он установил в своем доме. Тем не менее, он имел на это право...";
            link.l1 = "Сеньор генерал-губернатор, я могу считать расследование завершенным?";
            link.l1.go = "Step_4_13";
        break;
        case "Step_4_13":
            dialog.text = "Да, конечно. Ваша награда составляет 3 тысячи пиастров. Если бы вы сумели убить это 'доверенное лицо', то награда была бы иной.\n"+
                          "А сейчас прошу вас оставить меня, мне нужно отписать родственникам де Лейва в Кастилию о постигшей нас утрате.";
            link.l1 = "Хорошо, дон Франсиско, до свидания...";
            link.l1.go = "exit";
            pchar.questTemp.State = "empty";
            SaveCurrentQuestDateParam("questTemp");
            pchar.questTemp.CurQuestNumber = "5";
            pchar.questTemp.Waiting_time = "10";
            CloseQuestHeader("Spa_Line_4_MirderDetection");
            AddMoneyToCharacter(pchar, 3000);
            TakeNationLicence(FRANCE);
			AddTitleNextRate(sti(NPChar.nation), 0.5);
			ChangeCharacterNationReputation(pchar, sti(NPChar.nation), 1);
			AddCharacterExpToSkill(pchar, "Grappling", 100);
			AddCharacterExpToSkill(pchar, "Defence", 170);
			AddCharacterExpToSkill(pchar, "Cannons", 120);
			ChangeCharacterReputation(pchar, 2);
			//слухи
			AddSimpleRumour("Вы знаете, наш генерал-губернатор поручил капитану " + GetMainCharacterNameDat() + " расследование убийства коменданта де Лейва. Капитан нашел убийцу, им оказался какой-то француз. Но отомстить не сумел.", SPAIN, 5, 1);
        break;
        case "Step_4_14":
            dialog.text = "Мне сложно в это поверить...";
            link.l1 = "Тем не менее, это правда. Сейчас она находится на Торгуте и весьма счастлива, похоже.";
            link.l1.go = "Step_4_15";
        break;
        case "Step_4_15":
            dialog.text = "Ну что же, если бы дон Хосе относился к донне Анне подобающим образом, то, полагаю, ничего бы этого не произошло. Нужно признать, что донна Анна здорово страдала от нравов своего супруга и порядков, которые он установил в своем доме. Тем не менее, он имел на это право... Так кто же убил дона Хосе?";
            link.l1 = "Французский флибустьер Анри д'Эстре, доверенное лицо генерал-губернатора д'Ожерона.";
            link.l1.go = "Step_4_16";
        break;
        case "Step_4_16":
            dialog.text = "Проклятье! Не скажу, что мне уж очень нравился дон Хосе, однако терпеть такое у себя под носом просто недопустимо! Должно свершиться возмездие!";
            link.l1 = "Оно уже свершилось - убийца дона Хосе мертв. Я разыскал его на Тортуге и убил.";
            link.l1.go = "Step_4_17";
        break;
        case "Step_4_17":
            dialog.text = "Черт возьми! Вы действительно отважный офицер и человек чести, к тому же. Я благодарен вам за проявленное мужество.";
            link.l1 = "Спасибо, дон Франсиско.";
            link.l1.go = "Step_4_18";
        break;
        case "Step_4_18":
            dialog.text = "Вы свободны. Ну а мне нужно заняться тем, чтобы донна Анна не была так счастлива. Полагаю, жить ей осталось недолго...";
            link.l1 = "Вы намерены подослать к ней убийцу?";
            link.l1.go = "Step_4_19";
        break;
        case "Step_4_19":
            dialog.text = "Я? Ни в коей мере, у меня есть дела и поважней. А вот подробно сообщить родственникам дона Хосе о результатах вашего расследования я считаю свой обязанностью. Учитывая критерии кастильских законов чести, жить донне Анне осталось от силы полгода...";
            link.l1 = "Понятно. Ну что же, это, собственно, уже не мое дело. Сеньор генерал-губернатор, я могу считать расследование завершенным?";
            link.l1.go = "Step_4_20";
        break;
        case "Step_4_20":
            dialog.text = "Да, конечно. Вы блестяще справились с поставленной задачей, ваша награда составляет 120 тысяч пиастров.\n"+
                          "А сейчас прошу вас оставить меня. И не забывайте заглядывать ко мне в резиденцию время от времени...";
            link.l1 = "Хорошо, дон Франсиско, до свидания...";
            link.l1.go = "exit";
            pchar.questTemp.State = "empty";
            SaveCurrentQuestDateParam("questTemp");
            pchar.questTemp.CurQuestNumber = "5";
            pchar.questTemp.Waiting_time = "4";
            CloseQuestHeader("Spa_Line_4_MirderDetection");
            AddMoneyToCharacter(pchar, 120000);
            TakeNationLicence(FRANCE);
			AddTitleNextRate(sti(NPChar.nation), 3);
			ChangeCharacterReputation(pchar, 5);
			ChangeCharacterNationReputation(pchar, sti(NPChar.nation), 1);
			AddCharacterExpToSkill(pchar, "Sailing", 180);
			AddCharacterExpToSkill(pchar, "Grappling", 200);
			AddCharacterExpToSkill(pchar, "Defence", 170);
			AddCharacterExpToSkill(pchar, "Cannons", 220);
			AddCharacterExpToSkill(pchar, "Fortune", 150);
			//слухи
			AddSimpleRumour("Вы знаете, наш генерал-губернатор поручил капитану " + GetMainCharacterNameDat() + " расследование убийства коменданта де Лейва. Капитан нашел убийцу, им оказался какой-то француз, и отправил его на тот свет.", SPAIN, 5, 1);
        break;
        //********************** Квест №5, Нападение на Ла Вегу. ************************
        case "Step_5_1":
            dialog.text = "Все узнаете у Хименеса. На ближайшее время вы поступаете полностью в его распоряжение. Единственное, что я могу вам сказать - скучно не будет, предстоит серьезная боевая операция.";
            link.l1 = "Подробности...";
            link.l1.go = "Step_5_2";
        break;
        case "Step_5_2":
            dialog.text = "Все у Хименеса. Выдвигайтесь в Сантьяго немедленно.";
            link.l1 = "Хорошо, сеньор.";
            link.l1.go = "exit";
            pchar.questTemp.State = "Sp5LaVegaAttack_toSantiago";
            AddQuestRecord("Spa_Line_5_LaVegaAttack", "1");
        break;
        case "Step_5_3":
            dialog.text = "Хм, отлично. Мэнсфилд был авторитетным ладроном, это несомненный успех испанского оружия. Я поздравляю вас с этой победой!\n"+
                          "Скажите, проведение этой войсковой операции было в достаточной мере оплачено губернатором Сантьяго?";
            link.l1 = "Да, сеньор генерал-губернатор, в достаточной мере. Все в порядке";
            link.l1.go = "Step_5_4";
        break;
        case "Step_5_4":
            dialog.text = "Ну что же, великолепно! Жду вас у себя через некоторое время, возможно у меня будет для вас еще работа.";
            link.l1 = "Буду навещать вас время от времени, сеньор.";
            link.l1.go = "exit";
            pchar.questTemp.State = "empty";
            SaveCurrentQuestDateParam("questTemp");
            pchar.questTemp.CurQuestNumber = "6";
            pchar.questTemp.Waiting_time = "7";
            CloseQuestHeader("Spa_Line_5_LaVegaAttack");
			//--> огонь и пламень убираем
			DeleteAttribute(&locations[FindLocation("LaVega_town")], "hidden_effects");
			DeleteAttribute(&locations[FindLocation("LaVega_ExitTown")], "hidden_effects");	
			//<-- огонь и пламень
            //===> Эдвард убит :( теперь Ла Вегой рулит капитан Купер.
            sld = characterFromID("Edward Mansfield");
            sld.name 	= "Капитан";
        	sld.lastname = "Купер";
        	sld.model.animation = "man"
            sld.model = "officer_8";
            SendMessage(sld, "lss", MSG_CHARACTER_SETMODEL, sld.model, sld.model.animation);
            FaceMaker(sld);
            ChangeCharacterAddressGroup(sld, "LaVega_townhall", "sit", "sit1");
			AddTitleNextRate(sti(NPChar.nation), 1);
			AddCharacterExpToSkill(pchar, "FencingLight", 350);
			AddCharacterExpToSkill(pchar, "Fencing", 350);
			AddCharacterExpToSkill(pchar, "FencingHeavy", 320);
			ChangeCharacterReputation(pchar, 2);
			ChangeCharacterNationReputation(pchar, sti(NPChar.nation), 1);
			//слухи
			AddSimpleRumour("Говорят, что это вы отправили на тот свет главу буканьеров Мэнсфилда. Давно пора, скажу я вам...", SPAIN, 5, 1);
        break;
        //********************** Квест №6, Разведка на Тортуге. ************************
        case "Step_6_1":
            dialog.text = "Дело в том, что французы и голландцы, судя по всему, втайне взаимодействуют между собой. Оно и понятно, Людовик XIV ведет с нами деволюционную войну за голландские области в Европе. Сами Нидерланды не отваживаются открыто противостоять нам, однако их финансовая помощь французам играет очень важную, если не решающую роль в войне - всем известно, что у Людовика проблемы с деньгами.\n"+
                          "Ваша задача заключается в следующем: вам необходимо вновь тайно пробраться на Тортугу и попытаться перехватить что-либо из государственной переписки между генерал-губернатором Кюрасао Петером Стэвезантом и генерал-губернатором Тортуги Бертраном д'Ожероном.\n"+
                          "Вестовые с дипломатической почтой появляются там регулярно, так что вам нужно будет перехватить кого-нибудь из них. Мне нужен документ, который наше дипломатическое ведомство сможет использовать в качестве доказательства согласования действий голландцев и французов.";
            link.l1 = "Я понял, сеньор. Я каким-то образом ограничен в сроках выполнения?";
            link.l1.go = "Step_6_2";
        break;
        case "Step_6_2":
            dialog.text = "Ни в коей мере. Единственное, о чем я хочу вас попросить - не устраивайте открытую бойню на Тортуге, мне дороги храбрые и исполнительные офицеры.";
            link.l1 = "Дон Франсиско, я постараюсь выжить при любых обстоятельствах.";
            link.l1.go = "Step_6_3";
        break;
        case "Step_6_3":
            dialog.text = "И еще один момент. Я вручаю вам лицензию Французской Вест-Индской компании сроком на 60 дней. Вы должны предъявить эту бумагу начальнику порта и вас никто ни в чем не заподозрит.\n"+
                          "Ну что же, все инструкции вами получены. Вы можете приступать к выполнению задания.";
            link.l1 = "Приступаю, сеньор генерал-губернатор.";
            link.l1.go = "exit";
            pchar.questTemp.State = "Sp6TakeMess_toTotuga";
            AddQuestRecord("Spa_Line_6_TakeMessangeer", "1");
			GiveNationLicence(FRANCE, 60);
        break;
        case "Step_6_4":
            dialog.text = "Ну, докладывайте об исполнении задания.";
			if (CheckCharacterItem(pchar, "letter_1"))
			{
				link.l1 = "Мне удалось добыть дипломатическую переписку между генерал-губернаторами Тортуги и Кюрасао.";
				link.l1.go = "Step_6_6";
			}
			else
			{
				link.l1 = "Я не смог добыть бумаги до сих пор. Мне кажется, что все бесполезно и это задание мне уже не выполнить...";
				link.l1.go = "Step_6_5";			
			}
        break;
        case "Step_6_5":
            dialog.text = "Я разочарован в вас, очень плохо... Ну что же, вы мне не нужны более. Можете появиться у меня позднее, возможно я найду непыльную работенку для такого бестолкового офицера...";
            link.l1 = "Хорошо, сеньор генерал-губернатор...";
            link.l1.go = "exit";
            pchar.questTemp.State = "empty";
            SaveCurrentQuestDateParam("questTemp");
            pchar.questTemp.CurQuestNumber = "7";
            pchar.questTemp.Waiting_time = "40";
            CloseQuestHeader("Spa_Line_6_TakeMessangeer");
            BackItemDescribe("letter_1");
  			TakeNationLicence(FRANCE);
			LocatorReloadEnterDisable("Tortuga_tavern", "reload2_back", true);
			DeleteAttribute(pchar, "questTemp.State.Open");
			LAi_group_SetLookRadius("FRANCE_CITIZENS", LAI_GROUP_DEF_LOOK);
			ChangeCharacterReputation(pchar, -5);
			ChangeCharacterNationReputation(pchar, sti(NPChar.nation), -2);
			AddCharacterExpToSkill(pchar, "Repair", -170);
			AddCharacterExpToSkill(pchar, "Defence", -130);
			AddCharacterExpToSkill(pchar, "Cannons", -150);
			//слухи
			AddSimpleRumour("Наш генерал-губернатор отправил капитана " + GetMainCharacterNameGen() + " на Тортугу с каким-то очень важным поручением. Капитан вернулся ни с чем, одно слово - неудачник...", SPAIN, 5, 1);
         break;
        case "Step_6_6":
            dialog.text = "Превосходно, " + pchar.name + ", просто великолепно!";
            link.l1 = "Спасибо, дон Франсиско...";
            link.l1.go = "Step_6_7";
        break;
        case "Step_6_7":
            dialog.text = "Теперь у нас в руках серьезный аргумент в дипломатической войне... Вы проделали огромную работу.";
            link.l1 = "Действительно, пришлось потрудиться, что тут скажешь...";
            link.l1.go = "Step_6_8";
        break;
        case "Step_6_8":
            dialog.text = "50 тысяч монет - вот ваша награда! Извольте получить... Прошу вас не удаляться надолго из Гаваны, вы в скором времени будете мне нужны.";
            link.l1 = "Хорошо, сеньор генерал-губернатор.";
            link.l1.go = "exit";
			AddMoneyToCharacter(pchar, 50000);
            pchar.questTemp.State = "empty";
            SaveCurrentQuestDateParam("questTemp");
            pchar.questTemp.CurQuestNumber = "7";
            pchar.questTemp.Waiting_time = "9";
            CloseQuestHeader("Spa_Line_6_TakeMessangeer");
			TakeItemFromCharacter(pchar, "letter_1");
            BackItemDescribe("letter_1");
			TakeNationLicence(FRANCE);
			LocatorReloadEnterDisable("Tortuga_tavern", "reload2_back", true);
			DeleteAttribute(pchar, "questTemp.State.Open");
			LAi_group_SetLookRadius("FRANCE_CITIZENS", LAI_GROUP_DEF_LOOK);
			AddTitleNextRate(sti(NPChar.nation), 1);
			ChangeCharacterReputation(pchar, 3);
			ChangeCharacterNationReputation(pchar, sti(NPChar.nation), 1);
			AddCharacterExpToSkill(pchar, "Repair", 200);
			AddCharacterExpToSkill(pchar, "Sneak", 300);
			AddCharacterExpToSkill(pchar, "Cannons", 250);
			AddCharacterExpToSkill(pchar, "Accuracy", 350);
			//слухи
			AddSimpleRumour("Говорят, что капитан " + GetFullName(pchar) + ", удачно провернул сверхопасное дело на Тортуге. М-да, мужеством этого человека можно только восхищаться...", SPAIN, 5, 1);
		break; 
        //********************** Квест №7, Спасти Пардаля. ************************
        case "Step_7_1":
            dialog.text = "Дьявол, это очень и очень плохо. Боевой дух поселенцев серьезно упадет с этим известием...";
            link.l1 = "Мне очень жаль, сеньор, я сделал все, что мог.";
            link.l1.go = "Step_7_2";
        break;
        case "Step_7_2":
            dialog.text = "Ну что же, сделанного, вернее, не сделанного уже не воротишь. Отправляйтесь в отпуск. Через некоторое время можете навестить меня, возможно, у меня будет дело для вас.";
            link.l1 = "Хорошо, дон Франсиско. До свидания...";
            link.l1.go = "exit";
            pchar.questTemp.State = "empty";
            SaveCurrentQuestDateParam("questTemp");
            pchar.questTemp.CurQuestNumber = "8";
            pchar.questTemp.Waiting_time = "20";
            CloseQuestHeader("Spa_Line_7_SavePardal"); 
		break;
        case "Step_7_3":
            dialog.text = "Великолепно, друг мой, просто прекрасно!";
            link.l1 = "Сеньор, это был хороший бой, Маноэль, полагаю, расскажет вам подробности схватки...";
            link.l1.go = "Step_7_4";
        break;
        case "Step_7_4":
            dialog.text = "Без сомнения... Ну а ваши героические усилия достойны всяческой похвалы и высокой награды. 25000 реалов - вот ваша награда!";
            link.l1 = "Гм, сумма... сумма... Да, собственно, не в деньгах дело!";
            link.l1.go = "Step_7_5";
        break;
        case "Step_7_5":
            dialog.text = "Слова настоящего солдата! Я в вас не ошибался.\n"+ 
				          "Сейчас я вас не задерживаю, только попрошу через неделю быть у меня в резиденции - намечается дело для отважного капитана. Кажется, я знаю, кто этот капитан...";
            link.l1 = "Хорошо, дон Франсиско, буду у вас примерно в это время.";
            link.l1.go = "exit";
			AddMoneyToCharacter(pchar, 25000);
            pchar.questTemp.State = "empty";
            SaveCurrentQuestDateParam("questTemp");
            pchar.questTemp.CurQuestNumber = "8";
            pchar.questTemp.Waiting_time = "5";
            CloseQuestHeader("Spa_Line_7_SavePardal");
			AddTitleNextRate(sti(NPChar.nation), 2);
			ChangeCharacterReputation(pchar, 3);
			ChangeCharacterNationReputation(pchar, sti(NPChar.nation), 1);
			AddCharacterExpToSkill(pchar, "Sneak", 250);
			AddCharacterExpToSkill(pchar, "Sailing", 300);
			AddCharacterExpToSkill(pchar, "FencingLight", 150);
			AddCharacterExpToSkill(pchar, "Fencing", 150);
			AddCharacterExpToSkill(pchar, "FencingHeavy", 120);
			//слухи
			AddSimpleRumour("Говорят, что вы спасли нашего героя Пардаля! Позвольте выразить вам свое восхищение, вы настоящий воин!", SPAIN, 5, 1);
        break;
        case "Step_7_6":
            dialog.text = "М-да, я разочарован, очень плохо... Ну что же, я вас больше не задерживаю. Через некоторое время можете прибыть ко мне на прием, возможно, я и найду дело для такого бестолкового капитана...";
            link.l1 = "Хорошо, дон Франсиско, извините...";
            link.l1.go = "exit";
            pchar.questTemp.State = "empty";
            SaveCurrentQuestDateParam("questTemp");
            pchar.questTemp.CurQuestNumber = "8";
            pchar.questTemp.Waiting_time = "40";
            CloseQuestHeader("Spa_Line_7_SavePardal"); 
			ChangeCharacterReputation(pchar, -4);
			ChangeCharacterNationReputation(pchar, sti(NPChar.nation), -1);
			AddCharacterExpToSkill(pchar, "Sailing", -250);
			AddCharacterExpToSkill(pchar, "Grappling", -250);
			AddCharacterExpToSkill(pchar, "Defence", -250);
			AddCharacterExpToSkill(pchar, "Cannons", -250);
			//слухи
			AddSimpleRumour("Ходят слухи, что Пардаль погиб во многом по вашей вине. Печально это, сеньор трус...", SPAIN, 5, 1);
        break;
//********************** Квест №8, Отбить нападение на Куману. ************************
        case "Step_8_1":
            dialog.text = "К сожалению, только в общих чертах. Мне донесли, что в составе их эскадры есть даже линейный корабль первого класса, что совершенно нехарактерно для пиратов. Сомневаться не приходиться, что данное нападение спонсируют власти Тортуги...";
            link.l1 = "И мы не можем каким-то образом им помешать?";
            link.l1.go = "Step_8_2";
        break;
        case "Step_8_2":
            dialog.text = "Эх, друг мой, нам довелось жить с вами в очень тяжелое время... Испания, наша родина, терпит удары со всех сторон. В Старом Свете мы теряем северные провинции в деволюционной войне с Людовиком, здесь усиливается давление на нас со стороны конкурирующих держав...\n"+
				          "Что и говорить, похоже, что власть Папы в скором времени серьезно поколеблется в мире...";
            link.l1 = "Печально это, сеньор...";
            link.l1.go = "Step_8_3";
        break;
        case "Step_8_3":
            dialog.text = "Однако, нам с вами не пристало впадать в уныние. Мы находимся на острие клинка, карающего наших врагов во имя Испании и Господа Всемогущего! И раз нам опять бросают вызов, мы должны на него достойно ответить.\n"+
				          "Уже не раз проходимцы и преступники всех мастей, коими являются все ладроны, захватывали наши города, пытали и убивали наших колонистов...\n"+
						  "На этот раз мы должны обеспечить надежную защиту Кумане, это и является вашей задачей. Немедленно отправляйтесь к Кумане и отразите нападение ладронов под командованием капитана Анселя.";
            link.l1 = "Я все понял, дон Франсиско. Будет исполнено.";
            link.l1.go = "Step_8_4";
        break;
		case "Step_8_4":
            dialog.text = "Отлично! И помните, что в ваших руках, в руках ваших солдат и матросов, находится судьба целого города! Не подведите меня.";
            link.l1 = "Ладроны будут уничтожены, можете не сомневаться, сеньор.";
            link.l1.go = "exit";
            pchar.questTemp.State = "Sp8SaveCumana_toCumana";
            AddQuestRecord("Spa_Line_8_SaveCumana", "1");
			SetTimerCondition("Sp8SaveCumana_RescueTimeOver", 0, 0, 20, false);
            Pchar.quest.Sp8SaveCumana_Battle.win_condition.l1 = "location";
            Pchar.quest.Sp8SaveCumana_Battle.win_condition.l1.location = "Cumana";
            Pchar.quest.Sp8SaveCumana_Battle.win_condition = "Sp8SaveCumana_Battle";
        break;
		case "Step_8_5":
            dialog.text = "Все одно, я очень недоволен вами. Вы можете появиться у меня в резиденции несколько позже, а пока у меня нет дел для вас. Офицеры, проваливающие операции, мне требуются не часто...";
            link.l1 = "Я понял, дон Франсиско.";
            link.l1.go = "exit";
            pchar.questTemp.State = "empty";
            SaveCurrentQuestDateParam("questTemp");
            pchar.questTemp.CurQuestNumber = "9";
            CloseQuestHeader("Spa_Line_8_SaveCumana");
			SetLocationCapturedState("Cumana_town", false);
			ChangeCharacterReputation(pchar, -2);
			ChangeCharacterNationReputation(pchar, sti(NPChar.nation), -1);
			AddCharacterExpToSkill(pchar, "Repair", -270);
			AddCharacterExpToSkill(pchar, "Sneak", -260);
			AddCharacterExpToSkill(pchar, "Cannons", -300);
			AddCharacterExpToSkill(pchar, "FencingLight", -270);
			AddCharacterExpToSkill(pchar, "FencingHeavy", -230);
			AddCharacterExpToSkill(pchar, "Fencing", -250);
 			//слухи
			AddSimpleRumour("Говорят, что капитан " + GetFullName(pchar) + " не сумел спасти жителей Куманы! Печальная участь...", SPAIN, 5, 1);
        break;
		case "Step_8_6":
            dialog.text = "Отлично! Хороший удар по врагам Испании в Карибском море. В награду за проявленное мужество прошу принять от испанской короны 100000 золотых!";
            link.l1 = "С благодарностью, сеньор. А теперь позвольте откланяться, мне нужно привести в порядок корабли после боя...";
            link.l1.go = "Step_8_7";
        break;
		case "Step_8_7":
            dialog.text = "Хорошо, друг мой. Прошу вас не пропадать надолго из города, очень может быть, что вы снова понадобитесь мне.";
            link.l1 = "Я понял, дон Франсиско.";
            link.l1.go = "exit";
			AddMoneyToCharacter(pchar, 100000);
            pchar.questTemp.State = "empty";
			pchar.questTemp.Waiting_time = "7";
            SaveCurrentQuestDateParam("questTemp");
            pchar.questTemp.CurQuestNumber = "9";
            CloseQuestHeader("Spa_Line_8_SaveCumana");
			ChangeCharacterReputation(pchar, 2);
			ChangeCharacterNationReputation(pchar, sti(NPChar.nation), 1);
			AddTitleNextRate(sti(NPChar.nation), 2);
			AddCharacterExpToSkill(pchar, "Repair", 270);
			AddCharacterExpToSkill(pchar, "Sneak", 260);
			AddCharacterExpToSkill(pchar, "Cannons", 300);
			AddCharacterExpToSkill(pchar, "FencingLight", 270);
			AddCharacterExpToSkill(pchar, "FencingHeavy", 230);
			AddCharacterExpToSkill(pchar, "Fencing", 250);
 			//слухи
			AddSimpleRumour("Капитан, говорят, что это вы спасли Куману! Позвольте вам выразить свое восхищение, сеньор...", SPAIN, 5, 1);
        break;
//********************** Квест №9, Четыре золотых галеона. ************************
		case "Step_9_1":
            dialog.text = "Задание будет заключаться в сопровождении кораблей. Более я ничего сказать не могу, все инструкции – у губернатора Порто Белло. Извольте получить верительное письмо на ваше имя к губернатору Порто Белло.";
            link.l1 = "Я понял, сеньор. Приступаю немедленно.";
            link.l1.go = "exit";
            pchar.questTemp.State = "Sp9SaveCumana_toPortoBello";
            AddQuestRecord("Spa_Line_9_FourGolgGaleons", "1");
            ChangeItemDescribe("letter_1", "itmdescr_letter_1_SpaLineQ10");
            GiveItem2Character(pchar,"letter_1");
        break;
		case "Step_9_2":
            dialog.text = "Вы должны были сопроводить галеоны до Белиза, при чем здесь Кайман?!";
            link.l1 = "Сеньор, я понятия не имею, при чем здесь Кайман, но мне было обещано, что у Каймана меня встретит испанская военная эскадра.";
            link.l1.go = "Step_9_3";
        break;
		case "Step_9_3":
            dialog.text = "И что, встретила?";
            link.l1 = "Испанская военная эскадра - нет, зато теплую встречу мне подготовили пять кораблей ладронов. Они явно поджидали именно мои галеоны.";
            link.l1.go = "Step_9_4";
        break;
		case "Step_9_4":
            dialog.text = "Почему вы так решили?";
            link.l1 = "Сеньор, ну что делать целой эскадре пиратов у необитаемого острова посреди пустынной части Карибского моря? Можно, конечно, предположить, что они собрались в карты перекинуться, но это маловероятно.";
            link.l1.go = "Step_9_5";
        break;
		case "Step_9_5":
            dialog.text = "Черт возьми, похоже, что мы имеем дело либо с чудовищной ошибкой, либо с предательством. С этим я разберусь, а пока скажите мне, все ли галеоны целы?";
			iTemp = 0;
			iMoney = 0;
            for (i=1; i<=COMPANION_MAX; i++)
            {
                ShipType = GetCompanionIndex(pchar,i);
                if(ShipType != -1)
                {
                	sld = GetCharacter(ShipType);
                    ShipType = sti(sld.ship.type);
                    Rank = sti(RealShips[ShipType].basetype);
                    if (Rank == 9) 
					{
						iTemp++;
						iMoney = iMoney + sti(sld.Ship.Cargo.Goods.Gold);
					}
                }
            } 
			npchar.quest.money = iMoney;
			if (iTemp < 4 && iTemp > 0)
            {
                if (iTemp == 1)
                {
					link.l1 = "Сеньор, нападение было внезапным. Мне удалось сохранить только один галеон.";
					link.l1.go = "Step_9_6";
                }
                else
                {
					link.l1 = "Сеньор, нападение было внезапным. Мне удалось сохранить " + iTemp + " галеона...";
					link.l1.go = "Step_9_6";
                }
            }
            else
            {
				if (iTemp == 0)
				{
					link.l1 = "Сеньор, нападение ладронов было внезапным и отличалось крайней яростью. Я потерял все галеоны...";
					link.l1.go = "Step_9_8";				
				}
				else
				{
					link.l1 = "Сеньор, хоть нападение ладронов и отличалось внезапностью, мне удалось сохранить все вверенные мне галеоны.";
					link.l1.go = "Step_9_11";
				}
            }
        break;
		case "Step_9_6":
            dialog.text = "Это очень плохо. Я понимаю, что вы оказались в тяжелейшей ситуации, но вы могли бы быть более расторопным. А сейчас мне нужно отчитываться в Мадриде за серьезную недостачу в поставках золота... И это тогда, когда Испания ведет изнурительную борьбу в Европе!\n"+
				          "М-да, очень и очень плохо. Я рассчитывал на вас, а вы не сумели оправдать моего доверия.";
            link.l1 = "Дон Франсиско, поверьте мне, я сделал все, что мог. И даже более того... Однако силы были слишком неравны, а у меня был всего один корабль для защиты четырех галеонов! Кстати, по настоянию губернатора Порто Белло. Может быть, спросить по всей строгости закона с него?";
            link.l1.go = "Step_9_7";
        break;
		case "Step_9_7":
            dialog.text = "Может быть, но это не ваше дело. Я понимаю вас, но изложенные вами многочисленные причины поражения не умаляют вашей вины.";
            link.l1 = "Я понимаю...";
            link.l1.go = "Step_9_71";
		break;
		case "Step_9_71":
			if (sti(npchar.quest.money) < (iTemp*1200))
			{
				dialog.text = "Поэтому я прошу вас покинуть меня сейчас. В дальнейшем вы можете заходить ко мне в резиденцию, возможно и для вас найдется подходящее задание...";
				link.l1 = "Спасибо, дон Франсиско.";
				link.l1.go = "Step_9_exit";
				pchar.questTemp.Waiting_time = "20";
				pchar.questTemp.State = "empty";
				SaveCurrentQuestDateParam("questTemp");
				pchar.questTemp.CurQuestNumber = "10";
			}
			else
			{
				dialog.text = "Э-э-э, да вы, сеньор, никак жульничать вздумали?! На галеонах не хватает золота! Я не желаю более иметь с вами дела ни под каким соусом. Прощайте!";
				link.l1 = "Гм, ну как же...";
				link.l1.go = "Step_9_exit";
				pchar.questTemp.State = "QuestLineBreake";	
				ChangeCharacterReputation(pchar, -5);
				ChangeCharacterNationReputation(pchar, sti(NPChar.nation), -1);
				AddCharacterExpToSkill(pchar, "Sailing", -300);
				AddCharacterExpToSkill(pchar, "Leadership", -300);
				AddCharacterExpToSkill(pchar, "Commerce", -300);
				AddCharacterExpToSkill(pchar, "Pistol", -300);
				//слухи
				AddSimpleRumour("Капитан, говорят, вас поймали на жульничестве. И что генерал-губернатор выставил вас из резиденции. М-да, теперь туда вам пусть заказан...", SPAIN, 5, 1);
			}
        break;
		case "Step_9_8":
            dialog.text = "Черт возьми!!! Как же такое могло случиться?! Я понимаю, что вы оказались в тяжелейшей ситуации, но вы могли бы быть более расторопным. А сейчас мне нужно отчитываться в Мадриде за серьезную недостачу в поставках золота... И это тогда, когда Испания ведет изнурительную борьбу в Европе!";
            link.l1 = "Дон Франсиско, поверьте мне, я сделал все, что мог. К тому же я попал в ловушку из-за губернатора Порто Белло!";
            link.l1.go = "Step_9_10";
        break;
		case "Step_9_10":
            dialog.text = "Я даже слушать ваших оправданий не желаю. Идите вон!\n"+
				          "Впрочем, вы можете заходить ко мне, возможно, будет кое-какая работенка для такого бездаря...";
            link.l1 = "Хорошо, дон Франсиско.";
            link.l1.go = "exit";
            pchar.questTemp.State = "empty";
			pchar.questTemp.Waiting_time = "60";
            SaveCurrentQuestDateParam("questTemp");
            pchar.questTemp.CurQuestNumber = "10";
            CloseQuestHeader("Spa_Line_9_FourGolgGaleons"); 
        break;
		case "Step_9_11":
            dialog.text = "Находящееся на них золото тоже в сохранности?";
			if (sti(npchar.quest.money) >= 5000)
			{
				link.l1 = "Да, дон Франсиско, все в порядке.";
				link.l1.go = "Step_9_12";			
			}
			else
			{
				if(sti(npchar.quest.money) > 4800)
				{
					link.l1 = "Не совсем, дон Франсиско. Я имею на на галеонах " + FindRussianQtyString(sti(npchar.quest.money)) + " золота.";
					link.l1.go = "Step_9_14";	
				}
				else
				{
					link.l1 = "Хм, не совсем, дон Франсиско... Я имею на на галеонах " + FindRussianQtyString(sti(npchar.quest.money)) + " золота.";
					link.l1.go = "Step_9_16";				
				}
			}
        break;
		case "Step_9_12":
            dialog.text = "Это очень хорошо. Вы с честью сумели выпутаться из сложнейшей ситуации. Поэтому 220000 пиастров будет вам достойной наградой.";
            link.l1 = "Я благодарен вам, сеньор, за оценку моих деяний. Если я вам более не нужен, то разрешите мне покинуть вас на некоторое время.";
            link.l1.go = "Step_9_13";
        break;
		case "Step_9_13":
            dialog.text = "Хорошо, занимайтесь своими делами. Не забывайте, однако, навещать меня время от времени.";
            link.l1 = "Хорошо, дон Франсиско.";
            link.l1.go = "Step_9_exit";
			pchar.questTemp.Waiting_time = "7";
			pchar.questTemp.State = "empty";
			AddMoneyToCharacter(pchar, 220000);
			AddTitleNextRate(sti(NPChar.nation), 2);
			ChangeCharacterReputation(pchar, 5);
			ChangeCharacterNationReputation(pchar, sti(NPChar.nation), 2);
			AddCharacterExpToSkill(pchar, "Sailing", 290);
			AddCharacterExpToSkill(pchar, "Leadership", 100);
			AddCharacterExpToSkill(pchar, "Commerce", 250);
			AddCharacterExpToSkill(pchar, "Pistol", 350);
			//слухи
			AddSimpleRumour("Капитан, повсюду говорят, что, сопровождая галеоны с золотом, вы попали в засаду! Как вам удалось выпутаться из этой ситуации с честью - ума не приложу...", SPAIN, 5, 1);
        break;
		case "Step_9_14":
            dialog.text = "Ну что же, эти потери оправданны в вашей ситуации. Однако вознаграждение будет выплачено вам за вычетом недостающего золота. Итого ваша награда составляет 200000 золотых.\n"+
				          "Полагаю, мы смело можем утверждать, что вы с честью сумели выпутаться из сложнейшей ситуации.";
            link.l1 = "Я благодарен вам, сеньор, за оценку моих деяний. Если я вам более не нужен, то разрешите мне покинуть вас на некоторое время.";
            link.l1.go = "Step_9_15";
        break;
		case "Step_9_15":
            dialog.text = "Хорошо, занимайтесь своими делами. Не забывайте, однако, навещать меня время от времени.";
            link.l1 = "Хорошо, дон Франсиско.";
            link.l1.go = "Step_9_exit";
			pchar.questTemp.Waiting_time = "12";
			pchar.questTemp.State = "empty";
            SaveCurrentQuestDateParam("questTemp");            
			AddMoneyToCharacter(pchar, 200000);
			AddTitleNextRate(sti(NPChar.nation), 0.5);
			ChangeCharacterReputation(pchar, 1);
			ChangeCharacterNationReputation(pchar, sti(NPChar.nation), 1);
			AddCharacterExpToSkill(pchar, "Sailing", 190);
			AddCharacterExpToSkill(pchar, "Leadership", 70);
			AddCharacterExpToSkill(pchar, "Commerce", 150);
			AddCharacterExpToSkill(pchar, "Pistol", 150);
			//слухи
			AddSimpleRumour("Капитан, повсюду говорят, что, сопровождая галеоны с золотом, вы попали в засаду! Вы потеряли часть галеонов, но это ничего! Главное, что утерли нос ладронам!", SPAIN, 5, 1);
		break;
		case "Step_9_16":
			dialog.text = "Куда вы могли деть " + FindRussianQtyString(5000-sti(npchar.quest.money)) + " золота?! Это просто возмутительно и недопустимо ни в коей мере.";
            link.l1 = "Сеньор генерал-губернатор, вы же знаете, в какую переделку я попал и что нам довелось пережить. Паника на галеонах, творилось черт знает что...";
            link.l1.go = "Step_9_17";
        break;
		case "Step_9_17":
            dialog.text = "Это не оправдание! Впрочем, спор не имеет смысла... Я не желаю более иметь с вами дела ни под каким соусом. Прощайте!";
            link.l1 = "Гм, ну что же...";
            link.l1.go = "Step_9_exit";
			pchar.questTemp.State = "QuestLineBreake";
			ChangeCharacterReputation(pchar, -3);
			ChangeCharacterNationReputation(pchar, sti(NPChar.nation), -1);
			AddCharacterExpToSkill(pchar, "Sailing", -300);
			AddCharacterExpToSkill(pchar, "Leadership", -300);
			AddCharacterExpToSkill(pchar, "Commerce", -300);
			AddCharacterExpToSkill(pchar, "Pistol", -300);
			//слухи
			AddSimpleRumour("Капитан, говорят, что генерал-губернатор выставил вас из резиденции. М-да, теперь туда вам пусть заказан...", SPAIN, 5, 1);
		break;
		case "Step_9_exit":
			pchar.questTemp.CurQuestNumber = "10";
            CloseQuestHeader("Spa_Line_9_FourGolgGaleons");
            for (i=1; i<=4; i++)
            {
                sld = characterFromID("Captain_"+i);
				if (sld.id != "none")
				{
					RemoveCharacterCompanion(pchar, sld);
					sld.LifeDay = 0;
				}
            }
            DialogExit();
        break;
//********************** Квест №10, Защита Маракайбо ************************
		case "Step_10_1":
            dialog.text = "Да, опять!";
            link.l1 = "Сеньор, насколько вам известно, выполняя поручение губернатора Порто Белло, я попал в серьезную переделку...";
            link.l1.go = "Step_10_2";
        break;
		case "Step_10_2":
            dialog.text = "Данная проблема явилась результатом несогласованных действий. Меры приняты, такого более не повториться.";
            link.l1 = "А какие задачи мне предстоит выполнить в Маракайбо?";
            link.l1.go = "Step_10_3";
        break;
		case "Step_10_3":
            dialog.text = "Я отряжаю вас туда для усиления города, эскадры и гарнизона города. Вашей задачей будет являться защита Маракайбо от враждебных действий.";
            link.l1 = "Насколько я подчиняюсь губернатору города?";
            link.l1.go = "Step_10_4";
        break;
		case "Step_10_4":
            dialog.text = "В рамках выполнения глобальной задачи по защите города вы подчиняетесь губернатору Маракайбо. В вашу задачу не входят операции, не связанные непосредственно с этим.";
            link.l1 = "Отлично! Ну что же, я готов приступать к выполнению задачи.";
            link.l1.go = "Step_10_5";
        break;
		case "Step_10_5":
            dialog.text = "Великолепно! Отправляйтесь к Маракайбо как можно быстрей.";
            link.l1 = "Хорошо, дон Франсиско, мой переход к Маракайбо не заставит себя ждать.";
            link.l1.go = "exit";
            pchar.questTemp.State = "Sp10Maracaibo_toMaracaibo";
            AddQuestRecord("Spa_Line_10_Maracaibo", "1");
        break;
		case "Step_10_6":
            dialog.text = "Но я же специально командировал вас для защиты города!";
            link.l1 = "Дон Франсиско, силы пиратов были несоизмеримо больше моих. Я не смог оказать им достойного сопротивления, мне пришлось покинуть поле боя, дабы остаться в живых...";
            link.l1.go = "Step_10_7";
        break;
		case "Step_10_7":
            dialog.text = "Ушам своим не верю! ";
            link.l1 = "Увы, но это так...";
            link.l1.go = "Step_10_8";
        break;
		case "Step_10_8":
            dialog.text = "Я более не жалю иметь с вами никакого дела. Таких трусов и разгильдяев не было еще на службе Испании! Какой позор... Все, разговор с вами окончен раз и навсегда, прощайте.";
            link.l1 = "...";
            link.l1.go = "exit";
			CloseQuestHeader("Spa_Line_10_Maracaibo"); 
            DeleteAttribute(pchar, "questTemp.Waiting_time");
			pchar.questTemp.State = "QuestLineBreake";
            bWorldAlivePause   = false; // Конец линейки
			ChangeCharacterReputation(pchar, -5);
			ChangeCharacterNationReputation(pchar, sti(NPChar.nation), -2);
			AddCharacterExpToSkill(pchar, "Pistol", -430);
			AddCharacterExpToSkill(pchar, "Fortune", -460);
			AddCharacterExpToSkill(pchar, "Accuracy", -420);
			AddCharacterExpToSkill(pchar, "Grappling", -450);
			AddCharacterExpToSkill(pchar, "Sailing", -300);
			AddCharacterExpToSkill(pchar, "Leadership", -300);
			AddCharacterExpToSkill(pchar, "Commerce", -300);
			//слухи
			AddSimpleRumour("Капитан, говорят, что генерал-губернатор выставил вас из резиденции за трусость. М-да, теперь туда вам пусть заказан...", SPAIN, 5, 1);
        break;
		case "Step_10_9":
            dialog.text = "Превосходно! Я в вас не сомневался.";
            link.l1 = "Губернатор Маракайбо, сеньор " + Characters[GetCharacterIndex("Maracaibo_Mayor")].name + ", велел передать, что благодарен вам за так вовремя оказанную помощь.";
            link.l1.go = "Step_10_10";
        break;
		case "Step_10_10":
            dialog.text = "Хорошо... Вас он отблагодарил?";
            link.l1 = "Да, сеньор, все нормально...";
            link.l1.go = "Step_10_11";
        break;
		case "Step_10_11":
            dialog.text = "Ну что же, великолепно! Сейчас я попрошу вас отдыхать и набираться сил для последующих сражений. И не пропадайте надолго из города, возможно, что вы будете мне нужны в ближайшее время.";
            link.l1 = "Хорошо, дон Франсиско.";
            link.l1.go = "exit";
            pchar.questTemp.State = "empty";
			pchar.questTemp.Waiting_time = "15";
            SaveCurrentQuestDateParam("questTemp");
            pchar.questTemp.CurQuestNumber = "11";
            CloseQuestHeader("Spa_Line_10_Maracaibo"); 
			ChangeCharacterReputation(pchar, 5);
			AddTitleNextRate(sti(NPChar.nation), 2);
			ChangeCharacterNationReputation(pchar, sti(NPChar.nation), 1);
			AddCharacterExpToSkill(pchar, "Pistol", 430);
			AddCharacterExpToSkill(pchar, "Fortune", 460);
			AddCharacterExpToSkill(pchar, "Accuracy", 420);
			AddCharacterExpToSkill(pchar, "Grappling", 450);
			//слухи
			AddSimpleRumour("Капитан, говорят, что вы сумели отстоять Маракайбо! Без преувеличения, это - настоящий подвиг, сеньор!", SPAIN, 5, 1);
        break;
//********************** Квест №11, Нападение на Кюрасао и Сан Мартин ************************
		case "Step_11_1":
            dialog.text = "Очень хорошо! Итак, слушайте задачу.\n"+
				          "В рамках продолжающейся деволюционной войны с Францией за испанские владения в Голландии, первой и наиважнейшей задачей для нас является разорение Кюрасао и Сан Мартина. Из перехваченной депеши стало известно, что голландцы выделяют огромные суммы на военные нужды Франции.\n"+
						  "Мы не можем это доказать точно, так как вся переписка зашифрована особым образом, смысл письма совершенно иной. Но тем не менее мы должны покончить с этим, торговому могуществу Нидерландов должен быть положен конец!\n"+
						  "Вам необходимо взять штурмом и подвергнуть разорению голландские поселения Виллемстад, что на острове Кюрасао, и Мариго, что на острове Сан Мартин. Все трофеи, захваченные в результате операции, остаются за вами.\n"+
						  "Вы готовы к осуществлению такой задачи?";
            link.l1 = "Да, дон Франсиско. Я каким-то образом ограничен в сроках?";
            link.l1.go = "Step_11_2";
        break;
		case "Step_11_2":
            dialog.text = "Ни в коей мере.";
            link.l1 = "Ну что же, очень хорошо. Я приступаю.";
            link.l1.go = "exit";
            SetNationRelation2MainCharacter(HOLLAND, RELATION_ENEMY); //ссорим ГГ и голландцев. 
            SetNationRelationBoth(HOLLAND, SPAIN, RELATION_ENEMY);
            SetQuestHeader("Spa_Line_11_DestrHolland");
            AddQuestRecord("Spa_Line_11_DestrHolland", "1");
            pchar.questTemp.State = "DestrHolland_GoOn";
			characters[GetCharacterIndex("Villemstad_Mayor")].dialog.captureNode = "SpaLine11Quest_DestrHol"; //капитулянтская нода мэра
			characters[GetCharacterIndex("Marigo_Mayor")].dialog.captureNode = "SpaLine11Quest_DestrHol"; //капитулянтская нода мэра       
		break;
		case "Step_11_3":
            dialog.text = "И сделано отлично! Голландцы получили по рукам, надеюсь, это их заставит быть более прижимистыми в тратах, ха-ха.\n"+
				          "Захваченная на острове добыча по праву принадлежит вам! Я прошу вас явиться ко мне на прием через месяц. У меня опять назревает для вас дело.";
            link.l1 = "Хорошо, дон Франсиско. Обязательно буду у вас примерно в это время.";
            link.l1.go = "exit";
			AddTitleNextRate(sti(NPChar.nation), 2);
            DeleteAttribute(pchar, "questTemp.Q11_Villemstad");
            DeleteAttribute(pchar, "questTemp.Q11_Marigo");
            pchar.questTemp.State = "empty";
			pchar.questTemp.Waiting_time = "30";
            SaveCurrentQuestDateParam("questTemp");
            pchar.questTemp.CurQuestNumber = "12";
            CloseQuestHeader("Spa_Line_11_DestrHolland"); 
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
			AddSimpleRumour("Вы нас спасли, капитан! Отразить такое мощное нападение - это просто великолепно!", SPAIN, 5, 5);
        break;
//********************** Квест №12, Захват Порт-о-Принса ************************
		case "Step_12_1":
            dialog.text = "Вы нужны мне для захвата еще одного города. Только на этот раз не с целью разорения, а с целью утверждения власти Испании.";
            link.l1 = "Великолепно, сеньор генерал-губернатор. Что за город?";
            link.l1.go = "Step_12_2";
        break;
		case "Step_12_2":
            dialog.text = "Этот город - ныне французский Порт-о-Принс. Вы уже сумели решить проблему буканьеров Ла Веги, теперь очередь за этой колонией. Эспаньола должна стать полностью испанской!";
            link.l1 = "Понимаю... Я как-то ограничен в сроках?";
            link.l1.go = "Step_12_3";
        break;
		case "Step_12_3":
            dialog.text = "Опять же - нет. Взятие городов - задача непростая, посему было бы безрассудно с моей стороны требовать каких бы то ни было сроков. Подготовьтесь как следует к взятию и приступайте. Все трофеи, как всегда, ваша собственность.";
            link.l1 = "Отлично, сеньор генерал-губернатор. Я приступаю.";
            link.l1.go = "exit";
            SetQuestHeader("Spa_Line_12_OccupyPortPax");
            AddQuestRecord("Spa_Line_12_OccupyPortPax", "1");
            pchar.questTemp.State = "OccupyPortPax_GoOn";
			characters[GetCharacterIndex("PortPax_Mayor")].dialog.captureNode = "SpaLine12Quest_PortPax"; //капитулянтская нода мэра 
        break;
		case "Step_12_4":
            dialog.text = "Прекрасно! Я немедленно извещу всех заинтересованных лиц об этой грандиозной новости. Великолепная победа, капитан, просто сказочная удача!";
            link.l1 = "Хорошая подготовка и боевое мастерство - ничего сверхъестественного, сеньор...";
            link.l1.go = "Step_12_5";
        break;
		case "Step_12_5":
            dialog.text = "Бесспорно, скромность украшает воина. Ну что же, я откровенно рад за вас, друг мой, вы стали самым значимым капитаном в Карибском море. Мои поздравления!";
            link.l1 = "Спасибо, дон Франсиско...";
            link.l1.go = "Step_12_6";
        break;
		case "Step_12_6":
            dialog.text = "Итак, основные задачи, поставленные передо мной Испанией, выполнены. Отныне я даю вам свободу действий в интересах королевства. Я очень надеюсь на то, что ваши последующие деяния только умножат могущество Испании в этих водах.";
            link.l1 = "Можете в этом не сомневаться, сеньор генерал-губернатор. За сим разрешите откланяться.";
            link.l1.go = "exit";
            AddTitleNextRate(sti(NPChar.nation), 4);
            DeleteAttribute(pchar, "questTemp.Waiting_time");
            pchar.questTemp.State = "EndOfQuestLine"; 
			bWorldAlivePause   = false; // Конец линейки
			AddTitleNextRate(sti(NPChar.nation), 4);
			ChangeCharacterReputation(pchar, 5);
			ChangeCharacterNationReputation(pchar, sti(NPChar.nation), 20);
			CloseQuestHeader("Spa_Line_12_OccupyPortPax"); 
			AddCharacterExpToSkill(pchar, "Repair", 630);
			AddCharacterExpToSkill(pchar, "Sneak", 490);
			AddCharacterExpToSkill(pchar, "FencingLight", 530);
			AddCharacterExpToSkill(pchar, "FencingHeavy", 530);
			AddCharacterExpToSkill(pchar, "Fencing", 530);
			AddCharacterExpToSkill(pchar, "Pistol", 530);
			AddCharacterExpToSkill(pchar, "Fortune", 660);
			AddCharacterExpToSkill(pchar, "Accuracy", 520);
			AddCharacterExpToSkill(pchar, "Grappling", 750);
			//слухи
			AddSimpleRumour("Капитан, говорят, что вы сумели захватить Порт-о-Принс! Неужели теперь весь острова наш?! Великолепно!!", SPAIN, 5, 1);
		break;
	}
	UnloadSegment(NPChar.FileDialog2);  // если где-то выход внутри switch  по return не забыть сделать анлод
}
