// диалог по городам
void ProcessCommonDialogEvent(ref NPChar, aref Link, aref NextDiag)
{
    ref sld;
    int i, tempQty, chComp, iShip, iBaseType;
    string tempStr;
    switch (Dialog.CurrentNode)
	{
		case "quests":
            dialog.text = RandPhraseSimple("Какие вопросы?", "Что вам угодно?");
			link.l1 = RandPhraseSimple("Я передумал...", "Сейчас мне не о чем говорить");
		    link.l1.go = "exit";
			if (pchar.questTemp.State == "Fr2Letter_toCuracao")
			{
                dialog.text = "Я слушаю вас.";
                link.l1 = "Господин генерал-губернатор, мне поручено доставить вам письмо.";
                link.l1.go = "Step_F2_1";			
			}
			if (pchar.questTemp.State == "Fr2Letter_NiceResult")
			{
                dialog.text = "Опять вы? Опять пакет из Тортуги?";
                link.l1 = "На этот раз нет, месье генерал-губернатор. У меня есть хорошие новости для вас.";
                link.l1.go = "Step_F2_4";			
			}
			if (pchar.questTemp.LSC == "toTalkStuvesant")
			{
				link.l1 = "Я хочу спросить об одном человеке. Не откажите в любезности ответить мне.";
				link.l1.go = "TizerFind";
			}
		break;

		case "work_1":  // работа на благо короны - линейка нации
            // сюда вход только с патентом, проверка выше
            dialog.text = LinkRandPhrase("Вы находитесь на службе Республике Соединенных Провинций! Прошу немедленно приступить к выполнению возложенного на вас поручения!",
                                         "Я не буду скрывать, что от ваших успешных действий многое зависит. Поэтому приступайте к выполнению взятых на себя обязательств!",
                                         "Я жду вашего доклада о выполнении моего ответственного поручения, но не бестолковых перетолков.");
            link.l1 = RandPhraseSimple("Приступаю немедленно, минхер", "Я уже в процессе выполнения...");
            link.l1.go = "exit";
            
        	switch (pchar.questTemp.State)   // что делаем в данный момент
            {
                case "empty":   // Нет взятых квестов
                    if (GetQuestPastDayParam("questTemp") < sti(pchar.questTemp.Waiting_time))
                    {
                        dialog.text = LinkRandPhrase("В данный момент у меня нет для вас ответственных поручений. Вы можете появиться у меня в резиденции позже...",
                                                     "Прошу меня извинить, я очень сильно занят в данный момент!",
                                                     "Занимайтесь своими каперскими делами до поры, сейчас у меня нет для вас заданий.");
                        link.l1 = "Хорошо, минхер.";
                        link.l1.go = "exit";
                    }
                    else
                    {
						switch(pchar.questTemp.CurQuestNumber)   // Взятие квестов начиная со второго
                        {
                            case "2":
                                dialog.text = "У меня есть для вас новое поручение. Необходимо закупить в форте Оранж приготовленную для Кюрасао партию кофе, черного, красного и сандалового дерева. Отправляйтесь немедленно, вот необходимые для этого бумаги, вручите их коменданту форта Оранж.";
                                link.l1 = "Вы сказали - закупить?";
                                link.l1.go = "Step_2_1";
                            break;
                            case "3":
                                dialog.text = "Должен сообщить вам, что мы находимся в состоянии войны с Англией, началась очередная торговая война.";
                                link.l1 = "Вот как...";
                                link.l1.go = "Step_3_1";
                            break;
                            case "4":
                                if (CheckAttribute(pchar, "questTemp.GoodWormRusult"))
                                {
                                    dialog.text = "Вы добыли важнейшую информацию. Мэдифорду сейчас не хватает сил, чтобы доставить нам серьезные неприятности в Кюрасао, однако плохо укрепленный форт Оранж, находящийся у них под боком на Ямайке, они хотят уничтожить.";
                                    link.l1 = "Да, минхер, это мне известно.";
                                    link.l1.go = "Step_4_1";
                                }
                                else
                                {
                                    dialog.text = "Вы не сумели должным образом выполнить предыдущее задание. Вместо вас данное задание с успехом было выполнено другими храбрецами. Теперь я предоставляю вам возможность реабилитироваться. В результате полученной информации я сделал вывод, что Мэдифорду сейчас не хватает сил, чтобы доставить нам серьезные неприятности в Кюрасао, однако плохо укрепленный форт Оранж, находящийся у них под боком на Ямайке, они хотят уничтожить.";
                                    link.l1 = "Понятно...";
                                    link.l1.go = "Step_4_1";
                                }
                            break;
                            case "5":
                                dialog.text = "Итак, вас ждет следующее задание. Заключается оно в том, что вам нужно сопроводить три груженных флейта в поселение буканьеров Ла Вега к Эдварду Мэнфилду.";
                                link.l1 = "Я понял, минхер.";
                                link.l1.go = "Step_5_1";
                            break;
                            case "6":
                                dialog.text = "Вот и вы, я ждал вас.";
                                link.l1 = "К вашим услугам, минхер.";
                                link.l1.go = "Step_6_1";
                            break;
                            case "7":
                                dialog.text = "У меня для вас есть серьезная миссия. Мне необходимо доставить депешу генерал-губернатору Тортуги Бертрану д'Ожерону. Связано это с нашими действиями в деволюционной войне Франции против Испании. Данная переписка строго секретна, поэтому прошу вас понимать, насколько я доверю вам. Данная депеша не должна попасть никому, кроме генерал-губернатора Тортуги, это категорическое требование!";
                                link.l1 = "Я понял, минхер. Все будет исполнено в надлежащем виде, именно месье д'Ожерон получит данную депешу.";
                                link.l1.go = "Step_7_1";
                            break;
                            case "8":
                                dialog.text = "У меня к вам личная просьба. Мой друг, глава общины янсенистов Кюрасао, которого вы привезли на остров, просит командировать к нему сметливого офицера. Лучше вашей кандидатуры у меня никого нет. Просьба его заключается в поиске чего-то, очень ему нужного. В общем, Аарон расскажет вам все сам, он сейчас в своем доме, навестите его.";
                                link.l1 = "Я понял, минхер. Немедленно отправлюсь к нему.";
                                link.l1.go = "exit";
                                pchar.questTemp.State = "SeekBible_toAaron";
                                AddQuestRecord("Hol_Line_8_SeekBible", "1");
                                SaveCurrentQuestDateParam("questTemp");
								sld = characterFromId("Chumakeiro");
								LAi_SetStayTypeNoGroup(sld);
                                ChangeCharacterAddressGroup(sld, "Villemstad_houseS3", "goto", "goto1");
                            break;
                            case "9":
                                dialog.text = "Мы испытываем серьезные трудности с постройкой на наших европейских верфях линейных кораблей с большим водоизмещением - мелководные гавани, знаете ли... Я вчера размышлял над этим печальным фактом, и тут мне пришла в голову гениальная мысль!";
                                link.l1 = "Какая же, минхер?";
                                link.l1.go = "Step_9_1";
                            break;
                            case "10":
                                dialog.text = "Черт возьми, " + pchar.name + ", вы были правы, когда предлагали мне оставить добытую вами эскадру здесь, в Карибском море...";
                                link.l1 = "Что случилось, минхер?";
                                link.l1.go = "Step_10_1";
                            break;
                            case "11":
                                dialog.text = "Вот и вы! Я ждал вашего появления в резиденции.";
                                link.l1 = "Что случилось, минхер?";
                                link.l1.go = "Step_11_1";
                            break;
                            case "12":
                                dialog.text = "Спешу уведомить вас, что определен испанский город, который подвергнется нападению. Это Маракайбо. Жители города должны будут присягнуть на верность Республике Соединенных Провинций!";
                                link.l1 = "Хороший выбор, минхер.";
                                link.l1.go = "Step_12_1";
                            break;
                        }
                    }
                break;

                case "":           // Квест №1, доставка главы янсенистов мистера Аарона.
                    dialog.text = "Ну что же, у меня есть для вас весьма важное для города задание. Полагаю, оно будет вам по зубам.\n"+
                                  "Дело вот в чем: к нам на Кюрасао в последнее время стало прибывать довольно много протестантов из Европы, в основном из Фландрии и Бургундии, где свирепствует инквизиция. Недавно к нам прибыли так же сторонники учения Янсения. Дабы возглавить общину янсенистов, французским рейсовым кораблем из Голландии на Сен-Мартин прибыл влиятельный янсенист Аарон Мендес Чумакейро.\n"+
                                  "Это крупный негоциант, мой старый знакомый и большой друг. Направляйтесь в Мариго и привезите его ко мне, местная община янсенистов очень нуждается в нем.";
                    link.l1 = "Позвольте вас спросить, а кто такие янсенисты?";
                    link.l1.go = "Step_1_1";
                break;
                case "SeekChumakeiro_GoToVillemstad":
                    dialog.text = "Ну что же, Аарон уже мне все рассказал. Хорошая работа, минхер. Заходите ко мне через некоторое время, мне кажется, что я буду пользоваться вашими услугами и впредь.";
                    link.l1 = "Хорошо, минхер.";
                    link.l1.go = "exit";
                    pchar.questTemp.State = "empty";
                    SaveCurrentQuestDateParam("questTemp");
                    pchar.questTemp.CurQuestNumber = "2";
                    pchar.questTemp.Waiting_time = "7";
                    CloseQuestHeader("Hol_Line_1_Chumakeiro");
					AddTitleNextRate(sti(NPChar.nation), 1);
					AddCharacterExpToSkill(pchar, "Leadership", 100);
					AddCharacterExpToSkill(pchar, "Pistol", 150);
					AddCharacterExpToSkill(pchar, "Grappling", 120);
					ChangeCharacterReputation(pchar, 5);
					ChangeCharacterNationReputation(pchar, sti(NPChar.nation), 1);
					//слухи
					AddSimpleRumour("Вы знаете, наш генерал-губернатор Стэвезант нанял капитана " + GetMainCharacterNameGen() + " для доставки его друга из Мариго сюда, в Виллемстад. И вы знаете, поручение было успешно выполнено.", HOLLAND, 5, 1);
                break;
                case "TakeFoodCuracao_toOrangeStore":
                    dialog.text = "Итак, капитан, с чем пожаловали в Кюрасао?";
                    link.l1 = "Выполняю ваше поручение по доставке груза из Форта Оранж, минхер.";
                    if (GetQuestPastDayParam("questTemp") > 60)
                    {
                        dialog.text = "Я просил вас доставить мне всю партия в течение 60 дней. Вы не справились....";
                        link.l1 = "Верно, я не уложился по времени - были серьезные проблемы с ветром...";
                    }
                    link.l1.go = "Step_2_5";
                break;
                case "WormEnglPlans_SurrenderWorkMorgan":
                    dialog.text = "Вот и вы, наконец-то! Рассказывайте немедленно, что вы узнали.";
                    link.l1 = "Мэнсфилд ничего не знает о планах Мэдифорда. Но я узнал, что Мэдифорд пытался привлечь корсаров к нападению на Кюрасао. Таким образом, можно предположить, что приказ на открытие боевых действий против Нидерландов в Карибском море Мэдифордом получен.";
                    link.l1.go = "Step_3_3";
                break;
                case "WormEnglPlans_SaveOfficer":
                    dialog.text = "Вот и вы, наконец-то! Рассказывайте немедленно, что вам удалось выяснить.";
                    link.l1 = "Мэнсфилд ничего не знает о планах Мэдифорда. Адмирал корсаров Генри Морган предполагает, что приказ атаковать голландские поселения есть.";
                    link.l1.go = "Step_3_3";
                break;
                case "WormEnglPlans_GoodRusultWorm":
                    dialog.text = "Что вам удалось выяснить?";
                    link.l1 = "Мэдифорд готовит нападение на форт Оранж. Идет подготовка к штурму силами гарнизона Порт Рояля. Мэдифорд пытался уговорить корсаров напасть на Кюрасао, но у него ничего с этим не вышло. Так что сил для более крупной операции, нежели штурм Форта Оранж, у Мэдифорда нет.";
                    link.l1.go = "Step_3_7";
                break;
                case "DefenceOrange_LateRiseOrange":
                    dialog.text = "Докладывайте, что с Фортом Оранж?";
                    link.l1 = "Минхер, я не смог прийти на помощь форту Оранж. Несчастные жители истреблены, форта Оранж как голландского поселения более не существует...";
                    link.l1.go = "Step_4_2";
                break;
                case "DefenceOrange_DieHard":
                    dialog.text = "Докладывайте, что с Фортом Оранж?";
                    link.l1 = "Минхер, я не смог прийти на помощь Форту Оранж. Мы дали бой, но англичане оказались сильней. Нам пришлось спасаться бегством... К сожалению, форта Оранж как голландского поселения более не существует...";
                    link.l1.go = "Step_4_2";
                break;
                case "DefenceOrange_LandingDestroyed":
                    dialog.text = "Докладывайте, что с Фортом Оранж?";
                    link.l1 = "Минхер, задание выполнено. Все корабли и десантные части англичан уничтожены при попытке прорваться в город. Ему более ничего не угрожает.";
                    link.l1.go = "Step_4_4";
                break;
                case "DefenceOrange_SquadronDestroyed":
                    dialog.text = "Докладывайте, что с Фортом Оранж?";
                    link.l1 = "Минхер, мне удалось уничтожить корабли англичан на море. Но, к сожалению, я не смог противостоять наземными силам британцев. Форт Оранж разрушен...";
                    link.l1.go = "Step_4_5";
                break;
                case "ThreeFleutes_DieHard":
                    dialog.text = "Мне уже доложили, что вы не смогли сберечь вверенные вам флейты. Более того, вы сбежали с поля боя!";
                    link.l1 = "Минхер, у берегов Эспаньолы на меня напала целая эскадра испанцев! Силы были слишком неравны...";
                    link.l1.go = "Step_5_3";
                break;
                case "ThreeFleutes_AllFleuts":
                    dialog.text = "Докладывайте, что у вас произошло?";
                    link.l1 = "Минхер, у берегов Эспаньолы на меня напала целая эскадра испанцев! Но я сумел сохранить вверенные мне корабли.";
                    link.l1.go = "Step_5_4";
                break;
                case "ThreeFleutes_LostFleuts":
                    dialog.text = "Мне уже доложили, что вы не смогли сберечь вверенные вам флейты. Это очень плохо.";
                    link.l1 = "Минхер, у берегов Эспаньолы на меня напала целая эскадра испанцев!";
                    link.l1.go = "Step_5_9";
                break;
                case "RevengeMansfield_FromMorganWithResult":
                    dialog.text = "Так, рад вас видеть. Какие новости, что вы мне расскажете?";
                    link.l1 = "Ну, что я могу сказать? Не позавидуешь городу Сантьяго и его губернатору!";
                    link.l1.go = "Step_6_2";
                break;
                case "DelivLettTortuga_NotFoundLetter":
                    dialog.text = "Доложите о результатах своей миссии.";
                    link.l1 = "Я успешно доставил вашу депешу д'Ожерону.";
                    link.l1.go = "Step_7_2";
                break;
                case "DelivLettTortuga_LetterFound":
                    dialog.text = "Доложите о результатах своей миссии.";
                    link.l1 = "Я успешно доставил вашу депешу д'Ожерону.";
                    link.l1.go = "Step_7_8";
                break;
                case "DelivLettTortuga_WaitingNews":
                    if (GetQuestPastDayParam("questTemp") > 30)
                    {
                        dialog.text = "Ну что же, нам всем крупно повезло. Д'Ожерон, хитрый лис, догадался зашифровать письмо известным мне способом. Поэтому прямо доказать испанцы ничего не смогут.";
                        link.l1 = "Слава Господу!";
                        link.l1.go = "Step_7_20";
                    }
                    else
                    {
                        dialog.text = "Я еще не получил известия от д'Ожерона. Зайдите позже.";
                        link.l1 = "Хорошо, минхер.";
                        link.l1.go = "exit";
                    }
                break;
                case "SeekBible_BadResult":
                    dialog.text = "Чумакейро довел до меня информацию, что вы не сумели выполнить его поручение.";
                    link.l1 = "В общем-то, да. Я сделал все, что мог.";
                    link.l1.go = "Step_8_1";
                break;
                case "SeekBible_GoodResult":
                    dialog.text = "Друг мой, Аарон и его община находятся в религиозном экстазе. Он мне сказал, что вы сумели сделать нечто очень важное для него. Скажите мне, что это?";
                    link.l1 = "Я просто разыскал для него один утерянный предмет. Книгу, древний христианский трактат.";
                    link.l1.go = "Step_8_2";
                break;
                case "TakeThreeShips_toAbordage":
                    dialog.text = "Та-а-ак, что вы скажете мне по поводу вашего задания? Как идет захват кораблей?";
                    tempQty = 0;
                    for (i=1; i<=COMPANION_MAX; i++)
                	{
                        chComp = GetCompanionIndex(pchar,i);
                        if(chComp != -1)
                		{
                			sld = GetCharacter(chComp);
                    		iShip = sti(sld.ship.type);
                    		iBaseType = sti(RealShips[iShip].basetype);
                            if (iBaseType == 17)
                            {
                                tempQty++;
                                RemoveCharacterCompanion(pchar, sld);
                                AddPassenger(pchar, sld, false);
                                AddMoneyToCharacter(pchar, 50000);
                            }
                        }
                    }
                    if (tempQty == 0)
                    {
                        link.l1 = "Пока ничего достойного вашего внимания мне захватить не удалось. Но я работаю над этим, ждите, минхер...";
                        link.l1.go = "exit";
                    }
                    else
                    {
                        if (sti(pchar.questTemp.QtyShips) > tempQty)
                        {
                            if (tempQty > 1)
                            {
                                link.l1 = "Неплохо, минхер. Я готов сдать " + tempQty +  " корабля.";
                            }
                            else
                            {
                                link.l1 = "Неплохо, минхер. Я готов сдать один корабль.";
                            }
                            link.l1.go = "Step_9_5";
                            pchar.questTemp.QtyShips = sti(pchar.questTemp.QtyShips) - tempQty;
                            AddQuestRecord("Hol_Line_9_TakeThreeShips", "2");
                            AddQuestUserData("Hol_Line_9_TakeThreeShips", "QtyTook", tempQty);
                            AddQuestUserData("Hol_Line_9_TakeThreeShips", "QtyElse", pchar.questTemp.QtyShips);
                        }
                        else
                        {
                            AddQuestRecord("Hol_Line_9_TakeThreeShips", "3");
                            link.l1 = "Великолепно, минхер. Задание выполнено полностью.";
                            link.l1.go = "Step_9_6";
                            DeleteAttribute(pchar, "questTemp.QtyShips");
                        }
                     }
                break;
                case "SpaAttackCuracao_GoodWork":
                    dialog.text = "Я все видел! Это было просто великолепно!";
                    link.l1 = "Спасибо, мое участие в битве было достаточно скромным...";
                    link.l1.go = "Step_10_3";
                break;
                case "SpaAttackSentMartin_Late":
                    dialog.text = "Докладывайте, вы сумели защитить Сан Мартин?";
                    link.l1 = "Минхер, я сделал все, чтобы успеть, было слишком поздно... Мариго теперь испанский город.";
                    link.l1.go = "Step_11_5";
                break;
                case "SpaAttackSentMartin_GoodWork":
                    dialog.text = "Докладывайте, вы сумели защитить Сан Мартин?";
                    link.l1 = "Я успел, минхер. Осада испанской эскадры с города Сен-Мартин снята. Испанцы разгромлены.";
                    link.l1.go = "Step_11_8";
                break;
                case "OccupyMaracaibo_toWeWon":
                    dialog.text = "Уже знаю, друг мой, уже доложили о вашей блистательной победе!";
                    link.l1 = "Минхер, мне удалось захватить Маракайбо. Теперь это голландская колония.";
                    link.l1.go = "Step_12_3";
                break;
                case "QuestLineBreake":
                    dialog.text = "Я не имею дело с людьми, нарушающими свое слово. Мне больше нечего вам сказать.";
                    link.l1 = "...";
                    link.l1.go = "exit";
                    bWorldAlivePause   = false; // Конец линейки
                break;
                case "EndOfQuestLine":
                    dialog.text = "Действуйте самостоятельно в интересах Нидерландов, я рассчитываю на ваш здравый смысл.";
                    link.l1 = "Хорошо, минхер.";
                    link.l1.go = "exit";
                    bWorldAlivePause   = false; // Конец линейки
                break;
            }
        break;
        //********************** Квест №1, доставка главы янсенистов мистера Аарона. ************************
        case "Step_1_1":
            dialog.text = "Христианское религиозное течение, если вам это интересно.\n"+
                          "Началось оно с написания Янсением в 1640 году теологического трактата об Августине. Иезуиты подчеркивают близость янсенистской доктрины с протестантскими догмами и преследуют последователей этого религиозного течения. Людовик XIV также объявил янсенистов вне закона.\n"+
                          "Устройство общины здесь, под моей опекой, является весьма важным шагом для развития голландских колоний на Карибах. Дело в том, что большинство из них - это дворяне, офицеры, буржуа и крупные негоцианты. Такие люди как минимум не будут здесь лишними.\n"+
                          "Ну что же, если вы готовы, то прошу приступать немедленно к выполнению поставленной задачи.";
            link.l1 = "Я готов, минхер. Ожидайте, я скоро вернусь.";
            link.l1.go = "Step_1_2";
            link.l2 = "Извините, но в данный момент я очень занят. Позже я готов буду выполнить это ваше поручение.";
            link.l2.go = "exit";
        break;
        case "Step_1_2":
            SaveCurrentQuestDateParam("questTemp");
    		// остальные линейки в сад -->
    		pchar.questTemp.NationQuest = HOLLAND;
    		// остальные линейки в сад <--
    		pchar.questTemp.State = "SeekChumakeiro_ToAaronHouse";
            sld = GetCharacter(NPC_GenerateCharacter("Chumakeiro", "usurer_4", "man", "man", 10, HOLLAND, -1, false));
        	sld.name 	= "Аарон Мендес";
        	sld.lastname = "Чумакейро";
            sld.Dialog.Filename = "Quest\HolLineNpc_1.c";
        	LAi_SetHuberStayType(sld);
        	LAi_group_MoveCharacter(sld, "HOLLAND_CITIZENS");
            ChangeCharacterAddressGroup(sld, "Marigo_RoomHouseF1", "goto", "goto1");
            AddQuestRecord("Hol_Line_1_Chumakeiro", "1");
            Pchar.quest.SeekChumakeiro_intoMarigoHouse.win_condition.l1 = "location";
            Pchar.quest.SeekChumakeiro_intoMarigoHouse.win_condition.l1.location = "Marigo_houseF1";
            Pchar.quest.SeekChumakeiro_intoMarigoHouse.win_condition = "SeekChumakeiro_intoMarigoHouse";
            NextDiag.CurrentNode = NextDiag.TempNode;
            DialogExit();
        break;
        //********************** Квест №2, закупка и доставка товара из форта Оранж. ************************
        case "Step_2_1":
            dialog.text = "Именно закупить. Для закупки этой партии товара Оранж дает нам очень хорошие цены. Как только вы вручите мою верительную грамоту губернатору форта - вы получите льготные цены.\n"+
                          "Приготовленная для вас партия составляет по 400 единиц каждого наименования. Цены для вас будут примерно в десять раз дешевле от конъюнктурных.\n"+
                          "В соответствии с моими расчетами я выделяю вам на закупку 17700 золотых. Извольте получить.";
            link.l1 = "Если вы рассчитывали данную операцию, то не подскажете мне, сколько эта партия составляет в центнерах? Мне нужно планировать перевозку.";
            link.l1.go = "Step_2_2";
        break;
        case "Step_2_2":
            dialog.text = "Конечно, подскажу. В общей сложности вес всей партии составляет 6800 центнеров.";
            link.l1 = "Хм, немало... Минхер, я свободный капитан, и мне эти операции с перевозками грузов не очень по душе...";
            link.l1.go = "Step_2_3";
        break;
        case "Step_2_3":
            dialog.text = "Вот потому, что вы, так сказать, свободный капитан, я именно вас и привлекаю к выполнению данного поручения. Мне этот груз нужен здесь в целости и сохранности, и мне кажется, что именно свободный капитан сможет обеспечить его целостность и сохранность.\n"+
                          "Кругом шныряют испанцы, англичане к нам не проявляют дружеских чувств, пиратов кругом полно... Так что прошу вас понять меня верно...\n"+
                          "Отказа от выполнения данного поручения я от вас не приму. Приступайте! И помните, что вся партия, все 6800 центнеров, нужна мне здесь, в Виллемстаде, не позже, чем через два месяца.";
            link.l1 = "Хм, м-да... Ну хорошо, минхер, я все сделаю. Вы не оставляете мне выбора.";
            link.l1.go = "Step_2_4";
        break;
        case "Step_2_4":
            dialog.text = "Очень рад, что вы согласились. Ну что же, я жду вас с грузом.";
            link.l1 = "До встречи, минхер.";
            link.l1.go = "exit";
            pchar.questTemp.State = "TakeFoodCuracao_toOrangeMayor";
            AddQuestRecord("Hol_Line_2_TakeFoodCuracao", "1");
            ChangeItemDescribe("Trust", "itmdescr_Trust_HolLineQ2");
            GiveItem2Character(pchar,"Trust");
            AddMoneyToCharacter(pchar, 17700);
            SaveCurrentQuestDateParam("questTemp");
            pchar.questTemp.EbonyQtyElseNeed = 400;
            pchar.questTemp.CoffeeQtyElseNeed = 400;
            pchar.questTemp.MahoganyQtyElseNeed = 400;
            pchar.questTemp.SandalQtyElseNeed = 400;
        break;
        case "Step_2_5":
            if (CheckAttribute(pchar, "questTemp.EbonyQtyElseNeed")) pchar.questTemp.EbonyQty = pchar.Ship.Cargo.Goods.Ebony;
            if (CheckAttribute(pchar, "questTemp.CoffeeQtyElseNeed")) pchar.questTemp.CoffeeQty = pchar.Ship.Cargo.Goods.Coffee;
            if (CheckAttribute(pchar, "questTemp.MahoganyQtyElseNeed")) pchar.questTemp.MahoganyQty = pchar.Ship.Cargo.Goods.Mahogany;
            if (CheckAttribute(pchar, "questTemp.SandalQtyElseNeed")) pchar.questTemp.SandalQty = pchar.Ship.Cargo.Goods.Sandal;
            for (i=1; i<=COMPANION_MAX; i++)
        	{
                chComp = GetCompanionIndex(pchar,i);
                if(chComp != -1)
        		{
        			sld = GetCharacter(chComp);
                    if (CheckAttribute(pchar, "questTemp.EbonyQtyElseNeed")) pchar.questTemp.EbonyQty = sti(pchar.questTemp.EbonyQty) + sti(sld.Ship.Cargo.Goods.Ebony);
                    if (CheckAttribute(pchar, "questTemp.CoffeeQtyElseNeed")) pchar.questTemp.CoffeeQty = sti(pchar.questTemp.CoffeeQty) + sti(sld.Ship.Cargo.Goods.Coffee);
                    if (CheckAttribute(pchar, "questTemp.MahoganyQtyElseNeed")) pchar.questTemp.MahoganyQty = sti(pchar.questTemp.MahoganyQty) + sti(sld.Ship.Cargo.Goods.Mahogany);
                    if (CheckAttribute(pchar, "questTemp.SandalQtyElseNeed")) pchar.questTemp.SandalQty = sti(pchar.questTemp.SandalQty) + sti(sld.Ship.Cargo.Goods.Sandal);
                }
            }
            tempStr = "Минхер, я привез следующие товары.";
            if (CheckAttribute(pchar, "questTemp.EbonyQty"))
            {
                if (sti(pchar.questTemp.EbonyQty > 0)) tempStr = tempStr + " Черного дерева - " + pchar.questTemp.EbonyQty + " штук.";
            }
            if (CheckAttribute(pchar, "questTemp.CoffeeQty"))
            {
                if (sti(pchar.questTemp.CoffeeQty > 0)) tempStr = tempStr + " Кофе - " + pchar.questTemp.CoffeeQty + " штук.";
            }
            if (CheckAttribute(pchar, "questTemp.MahoganyQtyElseNeed"))
            {
                if (sti(pchar.questTemp.MahoganyQty > 0)) tempStr = tempStr + " Красного дерева - " + pchar.questTemp.MahoganyQty + " штук.";
            }
            if (CheckAttribute(pchar, "questTemp.SandalQtyElseNeed"))
            {
                if (sti(pchar.questTemp.SandalQty > 0)) tempStr = tempStr + " Сандалового дерева - " + pchar.questTemp.SandalQty + " штук.";
            }
            dialog.text = "Говорите, сколько и чего вы привезли из форта Оранж.";
            link.l1 = tempStr;
            link.l1.go = "Step_2_6";
            if (tempStr == "Минхер, я привез следующие товары.")
            {
                link.l1 = "Минхер, я в процессе выполнения. Пока ничего нового доставить не сумел, прошу меня извинить...";
                link.l1.go = "exit";
                DeleteAttribute(pchar, "questTemp.EbonyQty");
                DeleteAttribute(pchar, "questTemp.CoffeeQty");
                DeleteAttribute(pchar, "questTemp.MahoganyQty");
                DeleteAttribute(pchar, "questTemp.SandalQty");
            }
        break;
        case "Step_2_6":
            tempStr = "Итак, давайте подведем итог.";
            if (CheckAttribute(pchar, "questTemp.EbonyQtyElseNeed"))
            {
                HolLine2_NullGoods();
                if (sti(pchar.questTemp.EbonyQty) >= sti(pchar.questTemp.EbonyQtyElseNeed))
                {
                    tempStr = tempStr + " Вы полностью осуществили поставку черного дерева.";
                    HolLine2_NullGoods();
                    AddCharacterGoods(pchar, GOOD_EBONY, sti(pchar.questTemp.EbonyQty)-sti(pchar.questTemp.EbonyQtyElseNeed));
                    DeleteAttribute(pchar, "questTemp.EbonyQtyElseNeed");
                }
                else
                {
                    if (sti(pchar.questTemp.EbonyQty > 0))
                    {
                        tempStr = tempStr + " Я принимаю у вас " + pchar.questTemp.EbonyQty + " единиц черного дерева.";
                        pchar.questTemp.EbonyQtyElseNeed = sti(pchar.questTemp.EbonyQtyElseNeed) - sti(pchar.questTemp.EbonyQty);
                        HolLine2_NullGoods();
                    }
                }
            }
            if (CheckAttribute(pchar, "questTemp.CoffeeQtyElseNeed"))
            {
                if (sti(pchar.questTemp.CoffeeQty) >= sti(pchar.questTemp.CoffeeQtyElseNeed))
                {
                    tempStr = tempStr + " Вы поставили всю партию кофе.";
                    HolLine2_NullGoods();
                    AddCharacterGoods(pchar, GOOD_COFFEE, sti(pchar.questTemp.CoffeeQty)-sti(pchar.questTemp.CoffeeQtyElseNeed));
                    DeleteAttribute(pchar, "questTemp.CoffeeQtyElseNeed");
                }
                else
                {
                    if (sti(pchar.questTemp.CoffeeQty > 0))
                    {
                        tempStr = tempStr + " Я принимаю у вас " + pchar.questTemp.CoffeeQty + " единиц кофе.";
                        pchar.questTemp.CoffeeQtyElseNeed = sti(pchar.questTemp.CoffeeQtyElseNeed) - sti(pchar.questTemp.CoffeeQty);
                        HolLine2_NullGoods();
                    }
                }
            }
            if (CheckAttribute(pchar, "questTemp.MahoganyQtyElseNeed"))
            {
                if (sti(pchar.questTemp.MahoganyQty) >= sti(pchar.questTemp.MahoganyQtyElseNeed))
                {
                    tempStr = tempStr + " Вы доставили всю партию красного дерева.";
                    HolLine2_NullGoods();
                    AddCharacterGoods(pchar, GOOD_MAHOGANY, sti(pchar.questTemp.MahoganyQty)-sti(pchar.questTemp.MahoganyQtyElseNeed));
                    DeleteAttribute(pchar, "questTemp.MahoganyQtyElseNeed");
                }
                else
                {
                    if (sti(pchar.questTemp.MahoganyQty > 0))
                    {
                        tempStr = tempStr + " Я принимаю у вас " + pchar.questTemp.MahoganyQty + " единиц красного дерева.";
                        pchar.questTemp.MahoganyQtyElseNeed = sti(pchar.questTemp.MahoganyQtyElseNeed) - sti(pchar.questTemp.MahoganyQty);
                        HolLine2_NullGoods();
                    }
                }
            }
            if (CheckAttribute(pchar, "questTemp.SandalQtyElseNeed"))
            {
                if (sti(pchar.questTemp.SandalQty) >= sti(pchar.questTemp.SandalQtyElseNeed))
                {
                    tempStr = tempStr + " Вы сумели привезти всю партию сандала.";
                    HolLine2_NullGoods();
                    AddCharacterGoods(pchar, GOOD_SANDAL, sti(pchar.questTemp.SandalQty)-sti(pchar.questTemp.SandalQtyElseNeed));
                    DeleteAttribute(pchar, "questTemp.SandalQtyElseNeed");
                }
                else
                {
                    if (sti(pchar.questTemp.SandalQty > 0))
                    {
                        tempStr = tempStr + " Я принимаю у вас " + pchar.questTemp.SandalQty + " единиц сандала.";
                        pchar.questTemp.SandalQtyElseNeed = sti(pchar.questTemp.SandalQtyElseNeed) - sti(pchar.questTemp.SandalQty);
                        HolLine2_NullGoods();
                    }
                }
            }
            DeleteAttribute(pchar, "questTemp.EbonyQty");
            DeleteAttribute(pchar, "questTemp.CoffeeQty");
            DeleteAttribute(pchar, "questTemp.MahoganyQty");
            DeleteAttribute(pchar, "questTemp.SandalQty");
            dialog.text = tempStr;
            link.l1 = "Очень хорошо, минхер.";
            link.l1.go = "Step_2_7";
        break;
        case "Step_2_7":
            tempStr = "";
            if (!CheckAttribute(pchar, "questTemp.EbonyQtyElseNeed") && !CheckAttribute(pchar, "questTemp.CoffeeQtyElseNeed") && !CheckAttribute(pchar, "questTemp.MahoganyQtyElseNeed") && !CheckAttribute(pchar, "questTemp.SandalQtyElseNeed"))
            {
                if (GetQuestPastDayParam("questTemp") > 60)
                {
                    dialog.text = "Вы полностью осуществили поставку, это хорошо. Однако моя просьба к вам заключала в себе доставку товара в течение двух месяцев. Это вы выполнить не смогли.";
                    link.l1 = "Минхер, задача оказалась непроста, у меня не был достаточно вместительных кораблей для ускорения перевозки...";
                    link.l1.go = "Step_2_8";
                }
                else
                {
                    dialog.text = "Вы полностью осуществили поставку. Более того, вы осуществили ее в срок! Хочу довести до вашего сведения, что получение данного груза в срок имело для Кюрасао важность чрезвычайную.";
                    link.l1 = "Я рад, что сумел выполнить задачу в лучшем виде.";
                    link.l1.go = "Step_2_11";
                }
            }
            else
            {
                dialog.text = "Я прошу вас закончить начатое дело. Мне нужна вся партия из форта Оранж, до последнего центнера.";
                link.l1 = "Хорошо, минхер, все будет доставлено в Виллемстад.";
                link.l1.go = "exit";
                AddQuestRecord("Hol_Line_2_TakeFoodCuracao", "3");
                if (CheckAttribute(pchar, "questTemp.EbonyQtyElseNeed")) AddQuestUserData("Hol_Line_2_TakeFoodCuracao", "EbonyQtyElseNeed", pchar.questTemp.EbonyQtyElseNeed);
                else AddQuestUserData("Hol_Line_2_TakeFoodCuracao", "EbonyQtyElseNeed", "выполнено");
                if (CheckAttribute(pchar, "questTemp.CoffeeQtyElseNeed")) AddQuestUserData("Hol_Line_2_TakeFoodCuracao", "CoffeeQtyElseNeed", pchar.questTemp.CoffeeQtyElseNeed);
                else AddQuestUserData("Hol_Line_2_TakeFoodCuracao", "CoffeeQtyElseNeed", "выполнено");
                if (CheckAttribute(pchar, "questTemp.MahoganyQtyElseNeed")) AddQuestUserData("Hol_Line_2_TakeFoodCuracao", "MahoganyQtyElseNeed", pchar.questTemp.MahoganyQtyElseNeed);
                else AddQuestUserData("Hol_Line_2_TakeFoodCuracao", "MahoganyQtyElseNeed", "выполнено");
                if (CheckAttribute(pchar, "questTemp.SandalQtyElseNeed")) AddQuestUserData("Hol_Line_2_TakeFoodCuracao", "SandalQtyElseNeed", pchar.questTemp.SandalQtyElseNeed);
                else AddQuestUserData("Hol_Line_2_TakeFoodCuracao", "SandalQtyElseNeed", "выполнено");
            }
        break;
        case "Step_2_8":
            dialog.text = "Я понимаю, однако и вы должны понимать, что я руководствуюсь глобальными планам и мне просто необходимо четкое выполнение моих инструкций./n"+
                          "В связи с вышеуказанной причиной, размеры вознаграждение за проведение данной торговой операции сильно сокращаются.\n"+
                          "Я мог вам выделить не более 15000 золотых.";
            link.l1 = "Хм, кхе, м-м-м... Ну что же, спасибо и на этом...";
            link.l1.go = "Step_2_9";
        break;
        case "Step_2_9":
            dialog.text = "Всегда к вашим услугам, капитан. Заходите ко мне время от времени, возможно у меня найдется для вас еще работа.";
            link.l1 = "Хорошо, минхер.";
            link.l1.go = "Step_2_10";
            AddMoneyToCharacter(pchar, 15000);
            pchar.questTemp.Waiting_time = "30";
			AddCharacterExpToSkill(pchar, "Sneak", 150);
			AddCharacterExpToSkill(pchar, "Sailing", 100);
			AddTitleNextRate(sti(NPChar.nation), 1);
			ChangeCharacterReputation(pchar, 1);
			ChangeCharacterNationReputation(pchar, sti(NPChar.nation), 1);
			//слухи
			AddSimpleRumour("Вы слышали, капитан " + GetFullName(pchar) + " взялся за торговую операцию - доставку продовольствия из форта Оранж. Задачу он выполнил, но в установленные сроки не уложился.", HOLLAND, 5, 1);
        break;
        case "Step_2_10":
            AddQuestRecord("Hol_Line_2_TakeFoodCuracao", "4");
            CloseQuestHeader("Hol_Line_2_TakeFoodCuracao");
			DeleteAttribute(pchar, "GenQuest.HL2_AgainDiscount");  //сносим дисконтные цены
        	pchar.GenQuest.StoreGoods.StoreIdx = FortOrange_STORE; //восстанавливаем старые цены и кол-во
        	pchar.GenQuest.StoreGoods.HL2_QtyPriceIsBack = true;
            pchar.questTemp.State = "empty";
            SaveCurrentQuestDateParam("questTemp");
            pchar.questTemp.CurQuestNumber = "3";
            NextDiag.CurrentNode = NextDiag.TempNode;
            DialogExit();
        break;
        case "Step_2_11":
            dialog.text = "Итак, сумма вознаграждения за успешное выполнение данного поручения составляет 75000 золотых. Согласитесь, что это - достойное вознаграждение.";
            link.l1 = "Бесспорно, минхер...";
            link.l1.go = "Step_2_12";
        break;
        case "Step_2_12":
            dialog.text = "Вот и прекрасно! А сейчас прошу вас покинуть меня - дела... Навестите меня через дней, эдак, несколько.";
            link.l1 = "Хорошо, минхер, я постараюсь.";
            link.l1.go = "Step_2_10";
            AddMoneyToCharacter(pchar, 75000);
            pchar.questTemp.Waiting_time = "2";
			AddCharacterExpToSkill(pchar, "Sneak", 150);
			AddCharacterExpToSkill(pchar, "Sailing", 100);
			AddCharacterExpToSkill(pchar, "FencingLight", 150);
			AddCharacterExpToSkill(pchar, "Fencing", 150);
			AddCharacterExpToSkill(pchar, "FencingHeavy", 120);
			AddTitleNextRate(sti(NPChar.nation), 2);
			ChangeCharacterReputation(pchar, 2);
			ChangeCharacterNationReputation(pchar, sti(NPChar.nation), 2);
			//слухи
			AddSimpleRumour("Вы слышали, капитан " + GetFullName(pchar) + " взялся за торговую операцию - доставку продовольствия из форта Оранж. Задачу он выполнил, да еще и сделал это быстро!", HOLLAND, 5, 1);
        break;
        //********************** Квест №3, Узнать о планах англичан в связи с торговой войной ************************
        case "Step_3_1":
            dialog.text = "Эпицентр войны находится в прибрежных водах Европы и в Индийском океане, однако и здесь, в Карибском море, эта война окажет свое влияние.\n"+
                          "Мне необходима информация о планах англичан, точная и абсолютно достоверная. На острове Эспаньола есть поселение буканьеров, проживают там в основном французы, но голландцев также немало. Так вот, глава буканьеров - Эдвард Мэнсфилд.\n"+
                          "Его имя переделано на английский лад, его настоящая фамилия Эдвард Мансвельт, он голландец. Эдвард - доверенное лицо английского генерал-губернатора сэра Томаса Мэдифорда в делах грабежа испанцев.";
            link.l1 = "Хм, интересно...";
            link.l1.go = "Step_3_2";
        break;
        case "Step_3_2":
            dialog.text = "Ваше задание заключается в следующем: доставьте это письмо Мэнсфилду, в письме просьба о помощи. Уговорите его рассказать, что он знает о планах англичан. Сомневаться в том, что они атакуют нас, не приходится. Мы должны знать, что они собираются предпринять.";
            link.l1 = "Я понял, минхер. Приступаю к выполнению.";
            link.l1.go = "exit";
            SaveCurrentQuestDateParam("questTemp");
    		pchar.questTemp.State = "WormEnglPlans_toMansfield";
    		AddQuestRecord("Hol_Line_3_WormEnglishPlans", "1");
            ChangeItemDescribe("letter_1", "itmdescr_letter_1_HolLineQ3");
            GiveItem2Character(pchar,"letter_1");
            QuestSetCurrentNode("Edward Mansfield", "HolLine3_WormEngPlans");
        break;
        case "Step_3_3":
            dialog.text = "Докладывайте дальше.";
            link.l1 = "Собственно, докладывать больше нечего. Это все.";
            link.l1.go = "Step_3_4";
        break;
        case "Step_3_4":
            dialog.text = "Получается, что у нас есть одни предположения и никакой конкретики. Вы узнали что-либо о конкретных планах англичан?";
            link.l1 = "Нет, минхер. Ничего конкретного мне узнать не удалось.";
            link.l1.go = "Step_3_5";
        break;
        case "Step_3_5":
            dialog.text = "Вы не справились с заданием. Мне придется привлекать других, более расторопных людей к выполнению данной задачи. А сейчас я имею только потерянное время. Я очень разочарован в вас.";
            link.l1 = "Минхер, прошу вас дать мне возможность реабилитироваться.";
            link.l1.go = "Step_3_6";
        break;
        case "Step_3_6":
            dialog.text = "Только не в этом деле - слишком много потеряно времени. Ну а так, заходите ко мне время от времени, возможно, я смогу найти для вас подходящее дело.";
            link.l1 = "Спасибо, минхер.";
            link.l1.go = "exit";
            AddQuestRecord("Hol_Line_3_WormEnglishPlans", "5");
            CloseQuestHeader("Hol_Line_3_WormEnglishPlans");
            pchar.questTemp.State = "empty";
            SaveCurrentQuestDateParam("questTemp");
            pchar.questTemp.CurQuestNumber = "4";
            pchar.questTemp.Waiting_time = "30";
			ChangeCharacterReputation(pchar, -3);
			ChangeCharacterNationReputation(pchar, sti(NPChar.nation), -1);
			AddCharacterExpToSkill(pchar, "Sneak", -100);
			AddCharacterExpToSkill(pchar, "Sailing", -100);
			//слухи
			AddSimpleRumour("Ходят слухи, что капитан " + GetFullName(pchar) + " провалил некое секретное задание Стэвезанта, связанное с большой политикой...", HOLLAND, 5, 1);
        break;
        case "Step_3_7":
            dialog.text = "Великолепно! Теперь мне известно все, что нужно. Зайдите ко мне через пару дней, я приму решение по поводу ответных мер на данную угрозу со стороны англичан... Вы хорошо поработали, ваше вознаграждение составляет 50000.";
            link.l1 = "Спасибо, минхер.";
            link.l1.go = "exit";
            CloseQuestHeader("Hol_Line_3_WormEnglishPlans");
            pchar.questTemp.State = "empty";
            SaveCurrentQuestDateParam("questTemp");
            pchar.questTemp.CurQuestNumber = "4";
            pchar.questTemp.Waiting_time = "3";
            pchar.questTemp.GoodWormRusult = true;
            AddMoneyToCharacter(pchar, 50000);
			AddTitleNextRate(sti(NPChar.nation), 2);
			ChangeCharacterReputation(pchar, 5);
			ChangeCharacterNationReputation(pchar, sti(NPChar.nation), 1);
			AddCharacterExpToSkill(pchar, "Leadership", 150);
			AddCharacterExpToSkill(pchar, "Repair", 200);
			AddCharacterExpToSkill(pchar, "Sailing", 150);
			AddCharacterExpToSkill(pchar, "Repair", 250);
			//слухи
			AddSimpleRumour("Ходят слухи, что именно капитан " + GetFullName(pchar) + " сумел узнать о планах англичан в новой войне.", HOLLAND, 5, 1);
        break;
        //********************** Квест №4, Защитить форт Оранж ************************
        case "Step_4_1":
            dialog.text = "Немедленно отправляйтесь к Форту Оранж, вы должны успеть перехватить английскую экспедицию, иначе нашему поселению конец, они уничтожат форт и захватят в плен всех. Отправляйтесь сию же минуту, вы должны понимать, что от вашей расторопности сейчас многое зависит.";
            if (CheckAttribute(pchar, "questTemp.GoodWormRusult"))
            {
                link.l1 = "Я понял, минхер. Приступаю немедленно.";
            }
            else
            {
                link.l1 = "Я понял, минхер, спасибо за доверие. Будте уверены - на этот раз я вас не подведу! Приступаю немедленно.";
            }
            link.l1.go = "exit";
            DeleteAttribute(pchar, "questTemp.GoodWormRusult");
            AddQuestRecord("Hol_Line_4_DefenceOrange", "1");
            SaveCurrentQuestDateParam("questTemp");
            pchar.questTemp.State = "DefenceOrange_toFortOrange";
            Pchar.quest.DefenceOrange_FightNearJamaica.win_condition.l1 = "location";
            Pchar.quest.DefenceOrange_FightNearJamaica.win_condition.l1.location = "Jamaica";
            Pchar.quest.DefenceOrange_FightNearJamaica.win_condition = "DefenceOrange_FightNearJamaica";
			locations[FindLocation("Shore35")].DisableEncounters = true;
        break;
        case "Step_4_2":
            dialog.text = "Черт возьми! Мы не смогли защитить граждан Республики Соединенных Провинций от кровожадных англичан. Это очень плохо, наше и без того скромное присутствие в Карибском регионе теперь совершенно ослаблено.";
            link.l1 = "Мне очень жаль, я сделал все, что мог...";
            link.l1.go = "Step_4_3";
        break;
        case "Step_4_3":
            dialog.text = "Ну что же, тем не менее, я прошу вас заглядывать ко мне время от времени. Возможно, что у меня найдется для вас какое-нибудь дело.";
            link.l1 = "Хорошо, минхер. Обязательно буду навещать вас.";
            link.l1.go = "exit";
			//--> огонь и пламень убираем
			DeleteAttribute(&locations[FindLocation("FortOrange_town")], "hidden_effects");
			DeleteAttribute(&locations[FindLocation("FortOrange_ExitTown")], "hidden_effects");	
			//<-- огонь и пламень
			DeleteAttribute(&locations[FindLocation("Shore35")], "DisableEncounters"); //энкаутеры вернем
            CloseQuestHeader("Hol_Line_4_DefenceOrange");
            pchar.questTemp.State = "empty";
            SaveCurrentQuestDateParam("questTemp");
            pchar.questTemp.CurQuestNumber = "5";
            pchar.questTemp.Waiting_time = "30";
			ChangeCharacterReputation(pchar, -10);
			ChangeCharacterNationReputation(pchar, sti(NPChar.nation), -1);
			AddCharacterExpToSkill(pchar, "Sailing", -250);
			AddCharacterExpToSkill(pchar, "Grappling", -250);
			AddCharacterExpToSkill(pchar, "Defence", -250);
			AddCharacterExpToSkill(pchar, "Cannons", -250);
			//слухи
			AddSimpleRumour("Говорят, что это не сумели защитить несчастных жителей Форта Оранж от англичан. М-да, очень жаль...", HOLLAND, 5, 1);
        break;
        case "Step_4_4":
            dialog.text = "Великолепно! Я искренне рад, что именно вы сумели проявить себя в столь опасной ситуации. Ваше вознаграждение за проделанную работу составляет 100000 монет. К тому же, я прошу вас зайти несколько позже, у меня будет для вас другое поручение.";
            link.l1 = "Хорошо, минхер. Обязательно появлюсь в вашей резиденции не позже, чем через месяц.";
            link.l1.go = "exit";
			AddTitleNextRate(sti(NPChar.nation), 1);
            AddMoneyToCharacter(pchar, 100000);
            CloseQuestHeader("Hol_Line_4_DefenceOrange");
            pchar.questTemp.State = "empty";
            SaveCurrentQuestDateParam("questTemp");
            pchar.questTemp.CurQuestNumber = "5";
            pchar.questTemp.Waiting_time = "5";
            //==> на всякий случай дублируем снятие восторгов.
            QuestSetCurrentNode("FortOrange_Mayor", "First time");
            QuestSetCurrentNode("FortOrange_waitress", "First time");
            QuestSetCurrentNode("FortOrange_tavernkeeper", "First time");
            QuestSetCurrentNode("FortOrange_trader", "First time");
			DeleteAttribute(&locations[FindLocation("Shore35")], "DisableEncounters"); //энкаутеры вернем
			AddTitleNextRate(sti(NPChar.nation), 2);
			ChangeCharacterReputation(pchar, 5);
			ChangeCharacterNationReputation(pchar, sti(NPChar.nation), 1);
			AddCharacterExpToSkill(pchar, "Sailing", 180);
			AddCharacterExpToSkill(pchar, "Grappling", 200);
			AddCharacterExpToSkill(pchar, "Defence", 170);
			AddCharacterExpToSkill(pchar, "Cannons", 220);
			AddCharacterExpToSkill(pchar, "Fortune", 150);
			//слухи
			AddSimpleRumour("Говорят, что это именно вы защитили жителей Форта Оранж от англичан. Позвольте выразить вам свое восхищение!", HOLLAND, 5, 1);
        break;
        case "Step_4_5":
            dialog.text = "Черт возьми! Вы сделали половину дела, и вам не хватило совсем немного до спасения несчастных жителей форта!";
            link.l1 = "Это так, минхер. Для сухопутной атаки у меня не осталось сил. Сожалею...";
            link.l1.go = "Step_4_6";
        break;
        case "Step_4_6":
            dialog.text = "М-да, печально. Несчастные жители форта Оранж\nНу что же, несмотря на провал миссии, я оценил ваше старание. Вы можете рассчитывать на то, что будете и далее получать у меня задания.";
            link.l1 = "Я понял, минхер. Спасибо.";
            link.l1.go = "exit";
			//--> огонь и пламень убираем
			DeleteAttribute(&locations[FindLocation("FortOrange_town")], "hidden_effects");
			DeleteAttribute(&locations[FindLocation("FortOrange_ExitTown")], "hidden_effects");	
			//<-- огонь и пламень
			DeleteAttribute(&locations[FindLocation("Shore35")], "DisableEncounters"); //энкаутеры вернем
            CloseQuestHeader("Hol_Line_4_DefenceOrange");
            pchar.questTemp.State = "empty";
            SaveCurrentQuestDateParam("questTemp");
            pchar.questTemp.CurQuestNumber = "5";
            pchar.questTemp.Waiting_time = "15";
			ChangeCharacterReputation(pchar, -4);
			ChangeCharacterNationReputation(pchar, sti(NPChar.nation), -1);
			AddCharacterExpToSkill(pchar, "Sailing", -150);
			AddCharacterExpToSkill(pchar, "Grappling", -150);
			AddCharacterExpToSkill(pchar, "Defence", -150);
			AddCharacterExpToSkill(pchar, "Cannons", -150);
			Pchar.quest.DefenceOrange_FightInShore.over = "yes";
			Pchar.quest.DefenceOrange_FightInExitTown.over = "yes";
			//слухи
			AddSimpleRumour("Говорят, что вы разгромили английскую эскару у форта Оранж, но в наземном бою англичане были сильней. Очень, очень жаль...", HOLLAND, 5, 1);
        break;		
		
		//********************** Квест №5, Сопроводить три флейта к Мэнсфилду ************************
        case "Step_5_1":
            dialog.text = "Это еще не все. Вы назначаетесь командиром эскадры. В вашу компетенцию входит продажа груза в магазине у флибустьеров и закупка у них же продовольствия. Туши, что они забивают сотнями, очень неплохого качества. Естественно, для того, чтобы сделка была выгодна, вы должны позаботиться о том, чтобы все флейты достигли берегов Эспаньолы в целости и сохранности.";
            link.l1 = "Мне все понятно.";
            link.l1.go = "Step_5_2";
        break;
        case "Step_5_2":
            dialog.text = "Тогда действуйте, но без излишней спешки. Будьте осмотрительны. И помните, что вы должны доставить мне на флейтах 7000 центнеров продуктов. Флейты передаст в ваше распоряжение начальник порта.";
            link.l1 = "Хорошо, минхер.";
            link.l1.go = "exit";
            AddQuestRecord("Hol_Line_5_ThreeFleutes", "1");
            SaveCurrentQuestDateParam("questTemp");
            pchar.questTemp.State = "ThreeFleutes_toPortMan";
        break;
        case "Step_5_3":
            dialog.text = "Мне сложно говорит с капитаном, оказавшимся элементарным трусом! Прошу вас покинуть меня немедленно, я разочарован в крайней степени.";
            link.l1 = "Хорошо, минхер. Хочу сказать, что я очень сожалею...";
            link.l1.go = "Step_5_10";
            pchar.questTemp.Waiting_time = "100";
			ChangeCharacterReputation(pchar, -10);
			ChangeCharacterNationReputation(pchar, sti(NPChar.nation), -1);
			AddCharacterExpToSkill(pchar, "Sailing", -250);
			AddCharacterExpToSkill(pchar, "Grappling", -250);
			AddCharacterExpToSkill(pchar, "Defence", -250);
			AddCharacterExpToSkill(pchar, "Cannons", -250);
			//слухи
			AddSimpleRumour("Ходят слухи, что наш генерал-губернатор выставил вас из резиденции за трусость!", HOLLAND, 5, 1);
        break;

        case "Step_5_4":
            tempQty = sti(pchar.Ship.Cargo.Goods.Food);     
			for (i=1; i<=COMPANION_MAX; i++)
        	{
                chComp = GetCompanionIndex(pchar,i);
                if(chComp != -1)
        		{
					sld = GetCharacter(chComp);
					iShip = sti(sld.ship.type);
                    iBaseType = sti(RealShips[iShip].basetype); 
                    if (iBaseType == 7) tempQty = tempQty + sti(sld.Ship.Cargo.Goods.Food); //считаем только у флейтов
                }
            }
            tempQty = tempQty / 10; //вес от кол-ва
            dialog.text = "Превосходно! Иного я и не ожидал. Теперь скажите мне, вы привезли продовольствие в необходимом мне количестве 7000 центнеров?";
            if (tempQty >= 7000)
            {
                link.l1 = "Да, минхер, все в порядке.";
                link.l1.go = "Step_5_5";
            }
            else
            {
                link.l1 = "Минхер, этого я сделать не сумел. Всего мной доставлено продовольствия " + tempQty + " центнеров";
                link.l1.go = "Step_5_8";
            }
        break;
        case "Step_5_5":
            dialog.text = "Ну что же, вы не только хороший воин, но и рачительный хозяин. Я благодарен вам за успешное проведение этой очень опасной операции. Ваша награда составляет 60000 монет, извольте получить. Доставленное продовольствие прошу сдать.";
            link.l1 = "Спасибо, минхер.";
            link.l1.go = "Step_5_6";
            tempQty = sti(pchar.Ship.Cargo.Goods.Food);
            for (i=1; i<=3; i++)
            {
                sld = characterFromID("Captain_"+i);
				if (sld.id != "none")
				{
        			tempQty = tempQty + sti(sld.Ship.Cargo.Goods.Food);
					RemoveCharacterCompanion(pchar, sld);
					sld.LifeDay = 0;
				}
            }
            pchar.Ship.Cargo.Goods.Food = tempQty - 70000; //в пачке - 10 шт.
            AddMoneyToCharacter(pchar, 60000);
            pchar.questTemp.Waiting_time = "2";
			AddTitleNextRate(sti(NPChar.nation), 2);
			ChangeCharacterReputation(pchar, 5);
			ChangeCharacterNationReputation(pchar, sti(NPChar.nation), 1);
			AddCharacterExpToSkill(pchar, "Sailing", 180);
			AddCharacterExpToSkill(pchar, "Grappling", 200);
			AddCharacterExpToSkill(pchar, "Defence", 170);
			AddCharacterExpToSkill(pchar, "Cannons", 220);
			AddCharacterExpToSkill(pchar, "Fortune", 150);
        break;
        case "Step_5_6":
            dialog.text = "Ну что же, через несколько дней я жду вас у себя.";
            link.l1 = "Я понял, минхер. Прежде чем я уйду, я должен сообщить очень плохую новость - Эдвард Мэнсфилд убит при нападении испанцев на поселение буканьеров.";
            link.l1.go = "Step_5_7";
        break;
        case "Step_5_7":
            dialog.text = "Черт возьми, какая ужасная новость! Что же, спасибо за информацию, друг мой. Мэнсфилда жаль...";
            link.l1 = "Да, минхер. Он был хорошим человеком... За сим разрешите откланяться.";
            link.l1.go = "exit";
            CloseQuestHeader("Hol_Line_5_ThreeFleutes");
            pchar.questTemp.State = "empty";
            SaveCurrentQuestDateParam("questTemp");
            pchar.questTemp.CurQuestNumber = "6";
        break;
        case "Step_5_8":
            if (tempQty > 4000)
            {
                dialog.text = "Ну что же, вы хороший воин, но вы не рачительный хозяин. Тем не менее, я благодарен вам за успешную в целом операцию. Ваша награда составляет 20000 монет, извольте получить. Все продовольствие я изымаю.";
                link.l1 = "Хорошо, минхер.";
                AddMoneyToCharacter(pchar, 20000);
				AddTitleNextRate(sti(NPChar.nation), 3);
            }
            else
            {
                dialog.text = "Шутить изволите, мой дорогой? А где остальное продовольствие? Да вы, я вижу, аферист! Награды вам не видать.";
                link.l1 = "Нет, минхер, я честно пытался выполнить свои обязанности! Это все, что я смог привезти...";
            }
            link.l1.go = "Step_5_6";
            for (i=1; i<=3; i++)
            {
                sld = characterFromID("Captain_"+i);
				if (sld.id != "none")
				{
					RemoveCharacterCompanion(pchar, sld);
					sld.LifeDay = 0;
				}
            }
            pchar.Ship.Cargo.Goods.Food = 0;
            pchar.questTemp.Waiting_time = "7";
			ChangeCharacterReputation(pchar, -5);
			ChangeCharacterNationReputation(pchar, sti(NPChar.nation), -2);
			AddCharacterExpToSkill(pchar, "Sailing", -250);
			AddCharacterExpToSkill(pchar, "Grappling", -250);
			AddCharacterExpToSkill(pchar, "Defence", -250);
			AddCharacterExpToSkill(pchar, "Cannons", -250);
        break;
        case "Step_5_9":
            dialog.text = "Это не может являться оправданием! Я доверил вам голландские корабли с элементарным заданием. Вы не смогли его выполнить. Я не желаю вас видеть!";
            link.l1 = "Хорошо, минхер. Я удаляюсь, хотя вины за собой не чувствую.";
            link.l1.go = "Step_5_10";
            pchar.questTemp.Waiting_time = "30";
			ChangeCharacterReputation(pchar, -3);
			ChangeCharacterNationReputation(pchar, sti(NPChar.nation), -1);
			AddCharacterExpToSkill(pchar, "Sailing", -150);
			AddCharacterExpToSkill(pchar, "Grappling", -150);
			AddCharacterExpToSkill(pchar, "Defence", -150);
			AddCharacterExpToSkill(pchar, "Cannons", -150);
            for (i=1; i<=COMPANION_MAX; i++)
        	{
                chComp = GetCompanionIndex(pchar,i);
                if(chComp != -1)
        		{
        			sld = GetCharacter(chComp);
        			tempStr = sld.id;
                    if (findsubstr(tempStr, "Captain_", 0) != -1)
                    {
                        RemoveCharacterCompanion(pchar, sld);
                        sld.LifeDay = 0;
                    }
                }
            }
        break;
        case "Step_5_10":
            dialog.text = "Вероятно, вы сделали все, что смогли. Может быть я и смогу подобрать еще какую-нибудь работу для вас, но не сейчас...";
            link.l1 = "Я понял, минхер. Прежде чем я уйду, я должен сообщить очень плохую новость - Эдвард Мэнсфилд убит при нападении испанцев на поселение буканьеров.";
            link.l1.go = "Step_5_11";
        break;
        case "Step_5_11":
            dialog.text = "Черт возьми, мало мне на сегодня неприятных известий! Тем не менее, спасибо за информацию. Мэнсфилда жаль...";
            link.l1 = "Да, минхер. Он был хорошим человеком. За сим разрешите откланяться.";
            link.l1.go = "exit";
			CloseQuestHeader("Hol_Line_5_ThreeFleutes");
            pchar.questTemp.State = "empty";
            SaveCurrentQuestDateParam("questTemp");
            pchar.questTemp.CurQuestNumber = "6";
			//слухи
			AddSimpleRumour("Говорят, то вы привезли новость о смерти Эдварда Мэнсфилда. Жаль старика...", HOLLAND, 5, 1);
		break;
        //********************** Квест №6, узнать о планах мщения за Мэнсфилда ************************
        case "Step_6_1":
            dialog.text = "Нападение испанцев на поселение буканьеров и смерть Мэнсфилда серьезнейшим образом повлияет на баланс сил в Карибском регионе. Я не уверен, должны ли мы что-либо предпринимать в связи с этим.\n"+
                          "Вполне возможно, что Береговое Братство вознамерится само, как это было не раз, отомстить за смерть такого уважаемого флибустьера. Поэтому, нам необходимо согласовать свои действия.\n"+
                          "Отправляйтесь к Генри Моргану и узнайте у него, что они собираются предпринять. Если Морган решит задействовать вас в операции возмездия, то считайте мое разрешение на ваше участие полученным.";
            link.l1 = "Хорошо, минхер. Я с удовольствием поучаствую в таком благородном деле.";
            link.l1.go = "exit";
            AddQuestRecord("Hol_Line_6_RevengeMansfield", "1");
            pchar.questTemp.State = "RevengeMansfield_toMorgan";
            QuestSetCurrentNode("Henry Morgan", "HolLine6_fromStuvesant");
            SaveCurrentQuestDateParam("questTemp");
        break;
        case "Step_6_2":
            dialog.text = "Та-а-ак, продолжайте.";
            link.l1 = "Морган поручил мне выполнить разведывательную миссию. Я выяснил, что губернатор Хименес в ближайшее время будет находиться в городе. Судьба Сантьяго предрешена.";
            link.l1.go = "Step_6_3";
        break;
        case "Step_6_3":
            dialog.text = "Это хорошо... Морган не посчитал нужным задействовать вас в штурме города?";
            link.l1 = "Нет, непосредственно в штурме будут участвовать англичане, по известным причинам мы не можем объединить эскадры.";
            link.l1.go = "Step_6_4";
        break;
        case "Step_6_4":
            dialog.text = "Это верно. Ну что же, как говориться - к лучшему! Главное, что Мансвельт будет отомщен, он был хорошим человеком и достойным сыном своего отечества.";
            link.l1 = "Полностью согласен с вами, минхер.";
            link.l1.go = "Step_6_5";
        break;
        case "Step_6_5":
            dialog.text = "За успешно выполненное задание вы можете получить вознаграждение в размере 20000 монет. Прошу вас зайти ко мне в резиденцию через некоторое время. У меня вновь будет для вас задание.";
            link.l1 = "Хорошо, минхер.";
            link.l1.go = "exit";
            AddMoneyToCharacter(pchar, 20000);
            CloseQuestHeader("Hol_Line_6_RevengeMansfield");
            pchar.questTemp.State = "empty";
            SaveCurrentQuestDateParam("questTemp");
            pchar.questTemp.CurQuestNumber = "7";
            pchar.questTemp.Waiting_time = "4";
			AddCharacterExpToSkill(pchar, "FencingLight", 350);
			AddCharacterExpToSkill(pchar, "Fencing", 350);
			AddCharacterExpToSkill(pchar, "FencingHeavy", 320);
			AddTitleNextRate(sti(NPChar.nation), 1);
			ChangeCharacterReputation(pchar, 2);
			ChangeCharacterNationReputation(pchar, sti(NPChar.nation), 1);
			//слухи
			AddSimpleRumour("Ходят слухи, что вам удалось провести успешную разведывательную миссию у испанцев... Мои поздравления.", HOLLAND, 5, 1);
        break;
        //********************** Квест №7, Доставка письма д'Ожерону ************************
        case "Step_7_1":
            dialog.text = "В случае, если вы подвергнетесь нападению, данный пакет должен быть немедленно уничтожен вне зависимости от предполагаемого исхода боя.";
            link.l1 = "Хорошо, минхер. Я приступаю к выполнению.";
            link.l1.go = "exit";
            AddQuestRecord("Hol_Line_7_DelivLettTortuga", "1");
            pchar.questTemp.State = "DelivLettTortuga_toDOjeron";
            SaveCurrentQuestDateParam("questTemp");
            ChangeItemDescribe("letter_2", "itmdescr_letter_2_HolLineQ7");
            GiveItem2Character(pchar,"letter_2");
        break;
        case "Step_7_2":
            dialog.text = "Великолепно! Я доволен, что все так удачно разрешилось.";
            link.l1 = "Хм, не совсем удачно, минхер. Дело в том, что месье д'Ожерон написал ответную депешу для вас.";
            link.l1.go = "Step_7_3";
        break;
        case "Step_7_3":
            dialog.text = "А почему неудачно? Все в порядке, давайте ее сюда.";
            tempStr = "Кхе-кхе... Дело в том, что она была похищена у меня прямо в Тортуге.";
            switch (pchar.questTemp.State.KingResult)
            {
                case "BrigNotFound":
                    tempStr = tempStr + " Я бросился вдогонку за бригом похитителей, но не смог вернуть депешу.";
                break;
                case "BrigSunk":
                    tempStr = tempStr + " Я бросился вдогонку за бригом похитителей и потопил его, но депешу найти не смог. Где она - мне неизвестно.";
                break;
                case "BrigAborded":
                    tempStr = tempStr + " Я бросился вдогонку за бригом и взял его на абордаж. От капитана захваченного брига я узнал, что нападение на меня организовали испанцы. Но самой депеши на бриге уже не было, она попала к испанцам.";
                break;
                case "WarshipSunk":
                    tempStr = tempStr + " Я бросился вдогонку за бригом и взял его на абордаж. От капитана захваченного брига я узнал, что нападение на меня организовали испанцы. Я настиг галеон, на котором они уходили в сторону Санта-Каталины, но в бою галеон был потоплен, депеша не найдена. Где она в настоящий момент - мне не известно.";
                break;
            }
            link.l1 = tempStr;
            link.l1.go = "Step_7_4";
        break;
        case "Step_7_4":
            dialog.text = "Что?!!!";
            link.l1 = "Я очень сожалею, минхер...";
            link.l1.go = "Step_7_5";
        break;
        case "Step_7_5":
            dialog.text = "Вы хоть понимаете, что натворили?!";
            link.l1 = "Примерно, минхер...";
            link.l1.go = "Step_7_6";
        break;
        case "Step_7_6":
            dialog.text = "В общем, так. Я не желаю вам видеть у себя до тех пор, пока не выясню у д'Ожерона содержание депеши. И молитесь, чтобы там ничего 'такого' не было!";
            link.l1 = "Я понял, минхер. Я еще раз прошу прощения...";
            link.l1.go = "Step_7_7";
        break;
        case "Step_7_7":
            dialog.text = "Идите вон!!!";
            link.l1 = "Хорошо, минхер.";
            link.l1.go = "exit";
            SaveCurrentQuestDateParam("questTemp");
            pchar.questTemp.State = "DelivLettTortuga_WaitingNews";
            DeleteAttribute(pchar, "questTemp.State.KingResult");
            BackItemDescribe("letter_1");
			ChangeCharacterReputation(pchar, -10);
			ChangeCharacterNationReputation(pchar, sti(NPChar.nation), -1);
			AddCharacterExpToSkill(pchar, "Repair", -170);
			AddCharacterExpToSkill(pchar, "Defence", -130);
			AddCharacterExpToSkill(pchar, "Cannons", -150);
			//слухи
			AddSimpleRumour("Говорят, что капитан " + GetFullName(pchar) + " был буквально выставлен из резиденции генерал-губернатора...", HOLLAND, 5, 1);
        break;
        
        case "Step_7_20":
            dialog.text = "И это более чем верно! Сейчас я прошу вас покинуть меня, у меня нет для вас никаких дел. Возможно, в будущем что-либо подвернется, но только не сейчас.";
            link.l1 = "Хорошо, минхер. Спасибо, минхер.";
            link.l1.go = "exit";
            CloseQuestHeader("Hol_Line_7_DelivLettTortuga");
            pchar.questTemp.State = "empty";
            SaveCurrentQuestDateParam("questTemp");
            pchar.questTemp.CurQuestNumber = "8";
            pchar.questTemp.Waiting_time = "40";
			ChangeCharacterReputation(pchar, -3);
			ChangeCharacterNationReputation(pchar, sti(NPChar.nation), -2);
			AddCharacterExpToSkill(pchar, "Repair", -170);
			AddCharacterExpToSkill(pchar, "Defence", -130);
			AddCharacterExpToSkill(pchar, "Cannons", -150);
        break;

        case "Step_7_8":
            dialog.text = "Великолепно! Я доволен, что все так удачно разрешилось.";
            link.l1 = "Да, более чем. Вы себе даже представить не можете - испанцы напали на меня прямо в Тортуге.";
            link.l1.go = "Step_7_9";
        break;
        case "Step_7_9":
            dialog.text = "Вот это да! И что?";
            link.l1 = "Они хотели отобрать у меня ответную депешу д'Ожерона.";
            link.l1.go = "Step_7_10";
        break;
        case "Step_7_10":
            dialog.text = "Она у вас?";
            link.l1 = "Конечно, минхер. Все в порядке, вот она!";
            link.l1.go = "Step_7_11";
        break;
        case "Step_7_11":
            dialog.text = "Я более чем благодарен вам за вашу расторопность и смелость. Возможно, вы предотвратили политический кризис в Европе.";
            link.l1 = "Месье д'Ожерон обрисовал мне ситуацию, я понимаю, о чем идет речь.";
            link.l1.go = "Step_7_12";
            TakeItemFromCharacter(pchar, "letter_1");
            BackItemDescribe("letter_1");
        break;
        case "Step_7_12":
            dialog.text = "Ну что же, с удовольствием объявляю вам, что награда за выполнение миссии составляет 150000.";
            link.l1 = "Великолепно, минхер! И спасибо.";
            link.l1.go = "Step_7_13";
        break;
        case "Step_7_13":
            dialog.text = "Жду вас у себя через некоторое время. Полагаю, что у меня будут еще задания для ТАКОГО офицера!";
            link.l1 = "Непременно буду появляться у вас периодически.";
            link.l1.go = "exit";
            CloseQuestHeader("Hol_Line_7_DelivLettTortuga");
            pchar.questTemp.State = "empty";
            SaveCurrentQuestDateParam("questTemp");
            pchar.questTemp.CurQuestNumber = "8";
            pchar.questTemp.Waiting_time = "10";
            DeleteAttribute(pchar, "questTemp.State.KingResult");
            AddMoneyToCharacter(pchar, 150000);
			AddTitleNextRate(sti(NPChar.nation), 2);
			ChangeCharacterReputation(pchar, 6);
			ChangeCharacterNationReputation(pchar, sti(NPChar.nation), 1);
			AddCharacterExpToSkill(pchar, "Repair", 200);
			AddCharacterExpToSkill(pchar, "Sneak", 300);
			AddCharacterExpToSkill(pchar, "Cannons", 250);
			AddCharacterExpToSkill(pchar, "Accuracy", 350);
			//слухи
			AddSimpleRumour("Говорят, что капитан " + GetFullName(pchar) + ", выполняя функции вестового Стэвезанта, попал в серьезнейшую переделку - на него неожиданно напали испанцы прямо в Тортуге. Но он вышел из схватки победителем.", HOLLAND, 5, 1);
        break;
        //********************** Квест №8, Поиски Евангелие для Аарона ************************
        case "Step_8_1":
            dialog.text = "Но этого оказалось недостаточно. Я поражен вашим провалом, вы не смогли выполнить простейшего задания. Ну что же, заглядывайте ко мне время от времени, возможно, я смогу подобрать для вас то, что будет вам по силам.";
            link.l1 = "Хорошо, минхер.";
            link.l1.go = "exit";
            CloseQuestHeader("Hol_Line_8_SeekBible");
            pchar.questTemp.State = "empty";
            SaveCurrentQuestDateParam("questTemp");
            pchar.questTemp.CurQuestNumber = "9";
            pchar.questTemp.Waiting_time = "30";
			ChangeCharacterReputation(pchar, -2);
			ChangeCharacterNationReputation(pchar, sti(NPChar.nation), -1);
			AddCharacterExpToSkill(pchar, "Repair", -270);
			AddCharacterExpToSkill(pchar, "Sneak", -260);
			AddCharacterExpToSkill(pchar, "Cannons", -300);
			AddCharacterExpToSkill(pchar, "FencingLight", -270);
			AddCharacterExpToSkill(pchar, "FencingHeavy", -230);
			AddCharacterExpToSkill(pchar, "Fencing", -250);
 			//слухи
			AddSimpleRumour("Говорят, что капитан " + GetFullName(pchar) + " взялся за выполнения необычного задания - поиски какого-то загадочного библейского трактата. Взялся, но не сделал...", HOLLAND, 5, 1);
		break;
        case "Step_8_2":
            dialog.text = "А-а-а, вот он что... Ну что же, прекрасно. Надеюсь, он отблагодарил вас должным образом?";
            link.l1 = "Более чем, минхер.";
            link.l1.go = "Step_8_3";
        break;
        case "Step_8_3":
            dialog.text = "Великолепно. Пожалуй, с этим все. Вы пока мне не нужны, можете заниматься своими делами. А вот через пару недель прошу заглянуть ко мне, наверняка у меня будет для вас новое задание.";
            link.l1 = "Всегда к вашим услугам, минхер.";
            link.l1.go = "exit";
            CloseQuestHeader("Hol_Line_8_SeekBible");
            pchar.questTemp.State = "empty";
            SaveCurrentQuestDateParam("questTemp");
            pchar.questTemp.CurQuestNumber = "9";
            pchar.questTemp.Waiting_time = "2";
			TakeItemFromCharacter(pchar, "LegransMap");
			ChangeCharacterReputation(pchar, 2);
			AddTitleNextRate(sti(NPChar.nation), 2);
			ChangeCharacterNationReputation(pchar, sti(NPChar.nation), 1);
			AddCharacterExpToSkill(pchar, "Repair", 270);
			AddCharacterExpToSkill(pchar, "Sneak", 260);
			AddCharacterExpToSkill(pchar, "Cannons", 300);
			AddCharacterExpToSkill(pchar, "FencingLight", 270);
			AddCharacterExpToSkill(pchar, "FencingHeavy", 230);
			AddCharacterExpToSkill(pchar, "Fencing", 250);
 			//слухи
			AddSimpleRumour("Говорят, что капитан " + GetFullName(pchar) + " взялся за выполнения необычного задания - поиски какого-то загадочного библейского трактата. И говорят, что нашел.", HOLLAND, 5, 1);
        break;
        //********************** Квест №9, Захват линейных кораблей. ************************
        case "Step_9_1":
            dialog.text = "Если мы не можем строить большие суда, мы должны их захватывать!";
            link.l1 = "Потрясающе! Просто, как все гениальное...";
            link.l1.go = "Step_9_2";
        break;
        case "Step_9_2":
            dialog.text = "Хм, верно... Итак, я прошу вас предоставить в распоряжение голландского флота не менее четырех кораблей первого класса, то есть мановаров.";
            link.l1 = "Хм... Неплохо, пожалуй...";
            link.l1.go = "Step_9_3";
        break;
        case "Step_9_3":
            dialog.text = "Я тоже так думаю. Захватите как минимум четыре корабля, приведите их на Кюрасао, в порт Виллемстада, и сдайте лично мне. Каждый из сданных кораблей вам будет выплачено 50000 монет. Как вы считает, эта задача вам по силам?";
            link.l1 = "Полагаю, да.";
            link.l1.go = "Step_9_4";
        break;
        case "Step_9_4":
            dialog.text = "Тогда не будем терять времени - приступайте немедленно.";
            link.l1 = "Хорошо, минхер, приступаю немедленно.";
            link.l1.go = "exit";
            pchar.questTemp.QtyShips = 4;
            pchar.questTemp.State = "TakeThreeShips_toAbordage";
            AddQuestRecord("Hol_Line_9_TakeThreeShips", "1");
            SaveCurrentQuestDateParam("questTemp");
        break;
        case "Step_9_5":
            if (sti(pchar.questTemp.QtyShips)>1)
            {
                dialog.text = "Превосходно, но задача еще не выполнена полностью, вам нужно обеспечить наш флот еще " + pchar.questTemp.QtyShips + " кораблями. Не теряйте время, я жду от вас хороших вестей.";
            }
            else
            {
                dialog.text = "Превосходно, но задача еще не выполнена полностью, вам обеспечить наш флот еще одним кораблем. Не теряйте время, я жду от вас хороших вестей.";
            }
            link.l1 = "Хорошо, минхер, я сделаю все, что нужно.";
            link.l1.go = "exit";
        break;
        case "Step_9_6":
            dialog.text = "Отлично! Я немедленно распоряжусь отправить захваченные корабли в Индийский океан.";
            link.l1 = "Почему туда, позвольте спросить, минхер?";
            link.l1.go = "Step_9_7";
        break;
        case "Step_9_7":
            dialog.text = "Потому что там идет активное противостояние с Португалией. И именно сейчас в действиях португальцев наметился определенный успех. Полагаю, что данная эскадра восстановит паритет.";
            link.l1 = "Ну что же, смотрите сами. Но я бы не пребывал в таком радужном настроении относительно ситуации здесь, в Карибском море.";
            link.l1.go = "Step_9_8";
        break;
        case "Step_9_8":
            dialog.text = "Что вы имеете в виду?";
            link.l1 = "А вы не предполагаете, что эти корабли могут пригодиться здесь? Для дополнительной охраны голландских поселений, к примеру.";
            link.l1.go = "Step_9_9";
        break;
        case "Step_9_9":
            dialog.text = "Ну что вы, нет, конечно. Ничего нам не угрожает, будьте спокойны за судьбу голландских поселений.";
            link.l1 = "Хорошо, минхер. Однако спокойствие мне кажется обманчивым. К тому же, Голландия, как-никак, находится в состоянии войны с Англией.";
            link.l1.go = "Step_9_10";
        break;
        case "Step_9_10":
            dialog.text = "Активная фаза боевых действий прошла. Англичанам не до нас, у них проблемы с испанцами. Собственно, оценка политической ситуации - не ваша проблема.";
            link.l1 = "Я просто высказал свое мнение...";
            link.l1.go = "Step_9_11";
        break;
        case "Step_9_11":
            dialog.text = "Ваше мнение учтено. А сейчас я попрошу вас оставить меня одного. Но через некоторое время я хотел бы видеть вас у себя.";
            link.l1 = "Хорошо, минхер.";
            link.l1.go = "exit";
			AddTitleNextRate(sti(NPChar.nation), 1);
            CloseQuestHeader("Hol_Line_9_TakeThreeShips");
            pchar.questTemp.State = "empty";
            SaveCurrentQuestDateParam("questTemp");
            pchar.questTemp.CurQuestNumber = "10";
            pchar.questTemp.Waiting_time = "20";
			AddTitleNextRate(sti(NPChar.nation), 2);
			ChangeCharacterReputation(pchar, 5);
			ChangeCharacterNationReputation(pchar, sti(NPChar.nation), 1);
			AddCharacterExpToSkill(pchar, "Sailing", 290);
			AddCharacterExpToSkill(pchar, "Leadership", 100);
			AddCharacterExpToSkill(pchar, "Commerce", 250);
			AddCharacterExpToSkill(pchar, "Pistol", 350);
			//слухи
			AddSimpleRumour("Капитан, вы захватили такое количество линейных кораблей, что просто дух захватывает!", HOLLAND, 5, 1);
        break;
        //********************** Квест №10, Оборона Кюрасао. ************************
        case "Step_10_1":
            dialog.text = "Мне доложили, что к Кюрасао подходит крупная испанская эскадра. Не знаю, сможем ли мы выдержать нападение... Я ума не приложу, каким образом Испания сумела собрать такие силы в карибских водах, да еще в такой короткий срок.\n"+
                          "Однако, подумаем потом, если нам удастся выжить. Сейчас я прошу приложить все ваши знания и умения к тому, чтобы помочь форту и эскадре охранения отразить нападение. Без вашей помощи, я совершенно уверен, нас сомнут.";
            link.l1 = "Я постараюсь сделать все, что в моих силах. Полагаю, отобьемся.";
            link.l1.go = "Step_10_2";
        break;
        case "Step_10_2":
            dialog.text = "Ну что же, я рассчитываю на вас!";
            link.l1 = "Увидимся после битвы.";
            link.l1.go = "exit";
            pchar.questTemp.State = "SpaAttackCuracao_toBattle";
            AddQuestRecord("Hol_Line_10_SpaAttackCuracao", "1");
            SaveCurrentQuestDateParam("questTemp");
            Pchar.quest.SpaAttackCuracao_Battle.win_condition.l1 = "location";
            Pchar.quest.SpaAttackCuracao_Battle.win_condition.l1.location = "Curacao";
            Pchar.quest.SpaAttackCuracao_Battle.win_condition = "SpaAttackCuracao_Battle";
        break;
        case "Step_10_3":
            dialog.text = "Я имею другое мнение по этому поводу - ваше участие в сражении было решающим! Я не думаю, что деньги для вас - главное в жизни, однако прошу вас принять в награду за спасение города 200000 монет.";
            link.l1 = "Право же, минхер, я, возможно, и не заслуживаю такого...";
            link.l1.go = "Step_10_4";
        break;
        case "Step_10_4":
            dialog.text = "Ничего не хочу слушать, таких людей Нидерланды должны всячески поощрять в свершениях. И все на этом! А теперь мне нужно собрать максимальные сведения о том, зачем испанцы напали на Кюрасао. Прошу вас зайти ко мне на днях, по всей видимости, это еще не все...";
            link.l1 = "Хорошо, минхер, буду у вас в это время.";
            link.l1.go = "exit";
			AddTitleNextRate(sti(NPChar.nation), 1);
            AddMoneyToCharacter(pchar, 200000);
            CloseQuestHeader("Hol_Line_10_SpaAttackCuracao");
            pchar.questTemp.State = "empty";
            SaveCurrentQuestDateParam("questTemp");
            pchar.questTemp.CurQuestNumber = "11";
            pchar.questTemp.Waiting_time = "3";
			AddTitleNextRate(sti(NPChar.nation), 1);
			ChangeCharacterReputation(pchar, 7);
			ChangeCharacterNationReputation(pchar, sti(NPChar.nation), 1);
			AddCharacterExpToSkill(pchar, "Repair", 270);
			AddCharacterExpToSkill(pchar, "Sneak", 260);
			AddCharacterExpToSkill(pchar, "Cannons", 300);
			AddCharacterExpToSkill(pchar, "FencingLight", 270);
			AddCharacterExpToSkill(pchar, "FencingHeavy", 230);
			AddCharacterExpToSkill(pchar, "Fencing", 250);
			//слухи
			AddSimpleRumour("Вы нас спасли, капитан! Отразить такое мощное нападение - это просто великолепно!", HOLLAND, 5, 5);
        break;
        //********************** Квест №11, Спасение Мариго от испанцев. ************************
        case "Step_11_1":
            dialog.text = "Действительно случилось. Похоже, что мы доигрались в 'помощь французам'...";
            link.l1 = "Как это понимать, минхер?";
            link.l1.go = "Step_11_2";
        break;
        case "Step_11_2":
            dialog.text = "Вы знаете, что Людовик XIV ведет деволюционную войну с Испанией за так называемое испанское наследство - Фландрию и Бургундию. Эти территории, хоть и не входят в Республику Соединенных Провинций, тем не менее этнически и культурно связаны с Голландией. Поэтому мы всячески помогаем французам в этой войне - финансово, естественно.\n"+
                          "Похоже, что Испания всерьез решила перекрыть этот денежный ручей, которым Людовик щедро оплачивает проведение войны. Шаг, надо сказать, выверенный, так как Франция находится в очень тяжелом финансовом положении и без наших денег Людовику просто нечем будет платить за ведение войны.\n"+
                          "Сегодня я узнал, что Кюрасао - не единственная голландская колония, которая подверглась нападению. В осаде также Сан Мартин.";
            link.l1 = "Черт возьми...";
            link.l1.go = "Step_11_3";
        break;
        case "Step_11_3":
            dialog.text = "Я прошу вас собрать все силы и немедленно двинуться к Сан Мартину на помощь осажденным. Вы должны постараться успеть спасти город от уничтожения.";
            link.l1 = "Я приложу к этому все свои силы, минхер. Положитесь на меня.";
            link.l1.go = "Step_11_4";
        break;
        case "Step_11_4":
            dialog.text = " Да, похоже, что другого мне просто не остается.";
            link.l1 = "Выступаю немедленно.";
            link.l1.go = "exit";
            pchar.questTemp.State = "SpaAttackSentMartin_toBattle";
            AddQuestRecord("Hol_Line_11_SpaAttackSentMartin", "1");
            SaveCurrentQuestDateParam("questTemp");
            Pchar.quest.SpaAttackSentMartin_Battle.win_condition.l1 = "location";
            Pchar.quest.SpaAttackSentMartin_Battle.win_condition.l1.location = "SentMartin";
            Pchar.quest.SpaAttackSentMartin_Battle.win_condition = "SpaAttackSentMartin_Battle";
            SetTimerCondition("SpaAttackSentMartin_YouLate", 0, 0, 10, false);
        break;
        case "Step_11_5":
            dialog.text = "Это плохо. Но ничего не поделаешь, такова жизнь.";
            link.l1 = "Мне очень жаль...";
            link.l1.go = "Step_11_6";
        break;
        case "Step_11_6":
            dialog.text = "Вы сделали все, что смогли, я слишком поздно известил вас. Ну, хватит об этом! Я прошу вас прибыть ко мне в резиденцию примерно через месяц. Не теряйте времени даром, приведите в порядок свои корабли, наберите команду, пополните боезапас. В общем, подготовьтесь как следует к ответным действиям. Мне кажется, что наступил момент, когда Нидерланды смогут увеличить число своих колониальных владений в Карибском море.";
            link.l1 = "И какой город должен будет стать голландским?";
            link.l1.go = "Step_11_7";
        break;
        case "Step_11_7":
            dialog.text = "Пока сам не знаю. Все зависит от информации, которую я получу в ближайшее время.";
            link.l1 = "Я понял, минхер. Через месяц буду у вас.";
            link.l1.go = "exit";
            CloseQuestHeader("Hol_Line_11_SpaAttackSentMartin");
            pchar.questTemp.State = "empty";
            SaveCurrentQuestDateParam("questTemp");
            pchar.questTemp.CurQuestNumber = "12";
            pchar.questTemp.Waiting_time = "30";
        break;
        case "Step_11_8":
            dialog.text = "Великолепно, друг мой, просто великолепно. Я в вас никогда не сомневался.";
            link.l1 = "Рад слышать, минхер.";
            link.l1.go = "Step_11_9";
        break;
        case "Step_11_9":
            dialog.text = "Ваша награда за спасение Сан Мартина составляет 200000 монет. Извольте получить.";
            link.l1 = "Спасибо, минхер.";
            link.l1.go = "Step_11_10";
            AddMoneyToCharacter(pchar, 200000);
			AddTitleNextRate(sti(NPChar.nation), 2);
			ChangeCharacterReputation(pchar, 6);
			ChangeCharacterNationReputation(pchar, sti(NPChar.nation), 1);
			AddCharacterExpToSkill(pchar, "Pistol", 430);
			AddCharacterExpToSkill(pchar, "Fortune", 460);
			AddCharacterExpToSkill(pchar, "Accuracy", 420);
			AddCharacterExpToSkill(pchar, "Grappling", 450);
			//слухи
			AddSimpleRumour("Говорят, вы спасли Сан Мартин от уничтожения! Прекрасная работа, капитан!", HOLLAND, 5, 1);
        break;
        case "Step_11_10":
            dialog.text = "Также я прошу вас прибыть ко мне в резиденцию примерно через месяц. Не теряйте времени даром, приведите в порядок свои корабли, наберите команду, пополните боезапас. В общем, подготовьтесь как следует к ответным действиям. Испанцы обессилены, и именно сейчас наступил момент, когда Нидерланды смогут увеличить число своих колониальных владений в Карибском море.";
            link.l1 = "И какой город должен будет стать голландским?";
            link.l1.go = "Step_11_7";
        break;
        //********************** Квест №12, Захват Венесуэллы. ************************
        case "Step_12_1":
            dialog.text = "Я тоже так думаю. Собственно, приступайте.";
            link.l1 = "Я готов к операции, город Маракайбо будет голландским!";
            link.l1.go = "Step_12_2";
        break;
        case "Step_12_2":
            dialog.text = "Да будет так!";
            link.l1 = "Выступаю немедленно, минхер.";
            link.l1.go = "exit";
            pchar.questTemp.State = "OccupyMaracaibo_toFight";
            AddQuestRecord("Hol_Line_12_OccupyMaracaibo", "1");
            SaveCurrentQuestDateParam("questTemp");
			characters[GetCharacterIndex("Maracaibo_Mayor")].dialog.captureNode = "HolLine12Quest_Occupy"; //капитулянтская нода мэра
        break;
        case "Step_12_3":
            dialog.text = "Я уже отправил доклад об этом приобретении в метрополию. Наше государство, наша молодая нация жаждет побед! И мы делаем все для процветания Республики Соединенных Провинций.\n"+
                          "Итак, вы славно потрудились. У меня больше нет для вас заданий. Но я искренне надеюсь, что ваши дальнейшие действия в Карибском регионе будут такими же эффективными и такими же эффектными.";
            link.l1 = "Полагаю, вы можете на меня рассчитывать, минхер. Благо Нидерландов превыше всего!";
            link.l1.go = "Step_12_4";
        break;
        case "Step_12_4":
            dialog.text = "Вот слова великого воина и настоящего патриота! А сейчас я прошу принять от меня 300000 монет в качестве платы за вашу преданную службу.";
            link.l1 = "Спасибо, минхер.";
            link.l1.go = "Step_12_5";
            AddMoneyToCharacter(pchar, 300000);
        break;
        case "Step_12_5":
            dialog.text = "Ну, теперь скажите мне, какие у вас планы?";
            link.l1 = "Отоспаться, минхер. А дальше посмотрим.";
            link.l1.go = "Step_12_6";
        break;
        case "Step_12_6":
            dialog.text = "Ну что же, не смею вам мешать.";
            link.l1 = "До свидания, минхер.";
            link.l1.go = "exit";
            DeleteAttribute(pchar, "questTemp.Waiting_time");
            pchar.questTemp.State = "EndOfQuestLine";
			CloseQuestHeader("Hol_Line_12_OccupyMaracaibo");
			bWorldAlivePause   = false; // Конец линейки
			AddTitleNextRate(sti(NPChar.nation), 4);
			ChangeCharacterReputation(pchar, 5);
			ChangeCharacterNationReputation(pchar, sti(NPChar.nation), 20);
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
			AddSimpleRumour("Капитан, говорят, что вы сумели захватить Венесуэлу! Вот это да...", HOLLAND, 5, 1);
        break;

        //********************** Французская линейка. Квест №2, доставка письма д'Ожерона. ************************
        case "Step_F2_1":
            dialog.text = "Да-да, благодарю вас... И вы утверждаете, что это письмо вручил вам сам генерал-губернатор д`Ожерон?";
            link.l1 = "Разумеется! Я имею честь состоять на службе короля Франции и облечен доверием генерал-губернатора.";
            link.l1.go = "Step_F2_2";
            TakeItemFromCharacter(pchar, "letter_1");
            BackItemDescribe("letter_1");
        break;
        case "Step_F2_2":
            dialog.text = "Но вы не офицер. Разве у д`Ожерона больше нет солдат? А вот мне кажется, что вы состоите на службе Берегового братства и перехватили это письмо, а сюда пришли шпионить! Стража, арестовать его!!!";
            link.l1 = "Послушайте же, месье...";
            link.l1.go = "Step_F2_3";
        break;
        case "Step_F2_3":
			pchar.questTemp.State = "Fr2Letter_Arrest";
			for (i=1; i<=3; i++)
			{
				tempStr = "sold_hol_"+i;
				if (i==3) tempStr = "off_hol_1";
				sld = GetCharacter(NPC_GenerateCharacter("Guard_"+i, tempStr, "man", "man", 30, HOLLAND, 0, true));
				sld.Dialog.Filename = "Quest\FraLineNpc_2.c";
				FantomMakeCoolFighter(sld, 30, 70, 60, "blade34", "pistol2", 70);
         		ChangeCharacterAddressGroup(sld, pchar.location, "reload", "reload1_back");
				LAi_SetWarriorType(sld);
			}
			LAi_SetActorType(pchar);
            LAi_SetActorType(sld);
            SetActorDialogAny2Pchar(sld.id, "", -1, 0.0);
		    LAi_ActorFollow(pchar, sld, "ActorDialog_Any2Pchar", 0.0);
			DialogExit();
        break;
 		case "Step_F2_4":
    		dialog.text =  NPCStringReactionRepeat("Внимательно вас слушаю.",
            "На эту тему мы уже говорили...", "Опять двадцать пять?", "Прекратите!", "block", 0, npchar, Dialog.CurrentNode);
    		link.l1 = HeroStringReactionRepeat("Месье генерал-губернатор, насколько мне стало известно, близ Кюрасао орудовали пираты, которые потопили два десятка кораблей.", "Да, точно...", "Нет, извините...", "Пардон...", npchar, Dialog.CurrentNode);
    		link.l1.go = DialogGoNodeRepeat("Step_F2_5", "none", "none", "none", npchar, Dialog.CurrentNode);
 		break;
        case "Step_F2_5":
            dialog.text = "Это так. Ну и, собственно, что?";
            link.l1 = "Мне удалось выследить бандитов в городе и по их следам выйти на галеон, который и охотился на голландские суда. Этими бандитами оказались испанские каперы.";
            link.l1.go = "Step_F2_6";
        break;
        case "Step_F2_6":
            dialog.text = "И где этот галеон?";
            link.l1 = "На дне близ Пальмового берега. Я взял его на абордаж и перебил команду. Капитан перед смертью был мной допрошен, так что сомнений быть не может - этот тот самый галеон, не дававший вам покоя.";
            link.l1.go = "Step_F2_7";
        break;
        case "Step_F2_7":
            dialog.text = "Ну что я могу сказать - это просто великолепно! Вы оказали городу огромную услугу. Признаться, я думал, что это дело рук Берегового братства...";
            link.l1 = "Я это понял, месье, поэтому и предпринял поиски. Флибустьеры не нападают на голландские суда.";
            link.l1.go = "Step_F2_8";
        break;
        case "Step_F2_8":
            dialog.text = "Хорошо, что я вас не повесил... Ну что же, полагаю, вы заслужили награду. Я готов выплатить вам 20000 золотых за потопленный пиратский галеон. Это вас устроит?";
            link.l1 = "Без сомнения, сударь, этого достаточно. Теперь разрешите откланяться - мне пора в обратный путь. Прощайте.";
            link.l1.go = "exit";
			AddMoneyToCharacter(pchar, 20000);
			AddQuestRecord("Fra_Line_2_DelivLetter", "6");
        break;
		// --------------------- линейка ГПК, вопрос о Тизере ---------------------
		case "TizerFind":
			dialog.text = "Если я знаю этого человек, то, без сомнения, отвечу вам, капитан.";
			link.l1 = "Меня интересует Тизер Дэн, минхер.";
			link.l1.go = "TizerFind_1";
		break;
		case "TizerFind_1":
			dialog.text = "Очень странно, что вас интересует нищий бродяга, каторжник и бывший раб!";
			link.l1 = "Каторжник?";
			link.l1.go = "TizerFind_2";
		break;
		case "TizerFind_2":
			dialog.text = "Именно так! За участие в смуте он был лишен прав гражданства Нидерландов и сослан на каторгу. Однако, корабль, на котором его перевозили, был захвачен пиратами, а сам этот Тизер был продан ими в рабство...";
			link.l1 = "Черт возьми, какая интересная судьба!";
			link.l1.go = "TizerFind_3";
		break;
		case "TizerFind_3":
			dialog.text = "Не вижу ничего интересного! И почему это вы интересуетесь преступниками?";
			link.l1 = "Минхер, дело все в том, что... В общем, я тут озаботился собирательством образцов народного творчества. Хочу, знаете ли, по возвращении в Старый Свет написать книгу о своих приключениях в Новом. Вот так-с... А Тизер - очень колоритная фигура!";
			link.l1.go = "TizerFind_4";
		break;
		case "TizerFind_4":
			dialog.text = "Вы что, серьезно об этом говорите?";
			link.l1 = "Абсолютно. Мне довелось узнать от жителей вашего города о тех байках, которые Тизер рассказывал прилюдно. Очень интересно, знаете ли... Я, конечно, понимаю, что все это враки, но какие занимательные! Идеальный материал для книги.";
			link.l1.go = "TizerFind_5";
		break;
		case "TizerFind_5":
			dialog.text = "Хм, я поощряю сочинительство. Это новые земли, новые герои, новые горизонты бескрайнего мирового океана... М-да, очень интересно. Но от вас, капитан, я никак не ожидал таких наклонностей!";
			link.l1 = "Минхер, я - еще тот капитан...";
			link.l1.go = "TizerFind_6";
		break;
		case "TizerFind_6":
			dialog.text = "Понятно. Ну что ж, я был бы рад вам помочь относительно Тизера Дэна, но я не знаю где он. И не имею ни малейшего представления. Дело в том, что этот бродяга не раз нарушал общественный порядок, и я вынужден был применять к нему репрессивные меры. Полагаю, что он убыл с острова.";
			link.l1 = "Печально... И я так понял, что вы не знаете куда.";
			link.l1.go = "TizerFind_7";
		break;
		case "TizerFind_7":
			dialog.text = "Я уже сказал, что не имею ни малейшего понятия. Я предлагаю вам описать Кюрасао как есть, без идиотских бредней бывшего раба. Поверьте мне, от этого ваше сочинение только выиграет.";
			link.l1 = "Ну что же, я поищу материал на вашем острове. Спасибо вам!";
			link.l1.go = "exit";
			pchar.questTemp.LSC = "toLicencer";
			AddQuestRecord("ISS_PoorsMurder", "9");
		break;
	}
	UnloadSegment(NPChar.FileDialog2);  // если где-то выход внутри switch  по return не забыть сделать анлод
}
