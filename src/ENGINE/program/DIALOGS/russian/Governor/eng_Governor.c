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
		break;
		
		case "work_1":  // работа на благо короны - линейка нации
            // сюда вход только с патентом, проверка выше
            dialog.text = LinkRandPhrase("Вы английский капер и должны держать данное мне слово! Прошу немедленно приступить к выполнению возложенного на вас поручения!",
                                         "До тех пор, пока вы не выполните мое задание, мне не о чем с вами разговаривать.",
                                         "Я жду вашего доклада о выполнении ответственного поручения, а не пустых разговоров!");
            link.l1 = RandPhraseSimple("Приступаю немедленно.", "Я уже в процессе выполнения...");
            link.l1.go = "exit";
            
			switch (pchar.questTemp.State)   // что делаем в данный момент
            {
                case "empty":   // Нет взятых квестов
                    if (GetQuestPastDayParam("questTemp") < sti(pchar.questTemp.Waiting_time))
                    {
                        dialog.text = LinkRandPhrase("Пока у меня нет особых поручений для вас. Заходите ко мне время от времени.",
                                                     "Не сейчас, я слишком занят!",
                                                     "Ваша ловкость и отвага пока не востребованны, занимайтесь своим каперскими делами, друг мой.");
                        link.l1 = "Хорошо, сэр.";
                        link.l1.go = "exit";
                    }
                    else
                    {
						switch(pchar.questTemp.CurQuestNumber)   // Взятие квестов начиная со второго
                        {
                            case "2":
                                dialog.text = "У меня есть для вас задание.";
                                link.l1 = "Я готов, сэр.";
                                link.l1.go = "Step_2_1";
                            break;
                            case "3":   // раньше тройки не было, теперь нужно для того, чтобы ГГ мог взять квест №4 без квеста №3.
                                dialog.text = "Зайдите ко мне через два дня, у меня назревает дело для вас.";
                                link.l1 = "Хорошо, сэр.";
                                link.l1.go = "exit";
                                pchar.questTemp.Waiting_time = "2";
                                SaveCurrentQuestDateParam("questTemp");
                                pchar.questTemp.CurQuestNumber = "4";
                            break;
                            case "4":
                                dialog.text = "У меня есть для вас дело - разведывательная миссия. Я должен знать, что собираются предпринять голландцы в связи с торговой войной. Проберитесь на Кюрасао и постарайтесь собрать как можно больше информации о происходящем там. Меня интересует все, что происходит в городе, вплоть до мелочей.";
                                link.l1 = "Я готов, сэр.";
                                link.l1.go = "Step_4_0";
                            break;
                            case "5":
                                dialog.text = "А, вот и вы. Я ждал вас.";
                                link.l1 = "Я к вашим услугам.";
                                link.l1.go = "Step_5_1";
                            break;
                            case "6":
                                dialog.text = "Капитан каперского фрегата Моррис Уильямс просит у меня разрешения встать на рейде в Порт Рояле.";
                                link.l1 = "Мне знаком капитан Уильямс.";
                                link.l1.go = "Step_6_1";
                            break;
                            case "7":
                                dialog.text = "С огромным сожалением должен вам сообщить, что распоряжение британского правительства вернуть груз, захваченный Моррисом Уильямсом испанскому владельцу, привел к тяжелым последствиям для Ямайки.";
                                link.l1 = "Хм... не очень понимаю, сэр...";
                                link.l1.go = "Step_7_1";
                                // ==> Убираем Морриса Уильямса
                                sld = characterFromID("Morris_Willams");
                        	    sld.LifeDay = 1; // уберем нпс на след.день.
                        	    SaveCurrentNpcQuestDateParam(sld, "LifeTimeCreate");
                            break;
                            case "8":
                                dialog.text = "Вы опять нужны мне, друг мой. Мне необходимо направить надежного человека к главе поселения буканьеров Ла Вега, что на Эспаньоле. Да вы же знаете Эдварда Мэнсфилда, по моему поручению вы уже были у него. Помните?";
                                link.l1 = "Я помню, сэр. Я пытался уговорить его организовать нападение на Кюрасао.";
                                link.l1.go = "Step_8_1";
                            break;
                            case "9":
                                dialog.text = "Готовы ли вы выполнить серьезную задачу по штурму испанского города?";
                                link.l1 = "Да, я достаточно подготовлен.";
                                link.l1.go = "Step_9_1";
                                link.l2 = "Нет, сэр. У меня недостаточно кораблей для такого серьезного мероприятия. Я подготовлюсь как следует и вернусь позже.";
                                link.l2.go = "exit";
                            break;
                            case "10":
                                dialog.text = "Правительство Испании снарядило против ямайских корсаров эскадру из семи линейных кораблей под командованием адмиралов де Бустоса и дель Кампо-и-Эспиноса. Они уже подошли к Ямайке. Момент выбран как нельзя лучше, вы сами видели, что на рейде Порт Рояля, кроме вас, нет больше корсаров, все с Морганом в очередной охоте на испанцев. Вы должны отбить нападение. В ваше распоряжение переходят корабли охранения, это все, чем я могу вам помочь. Вперед, и да поможет нам Господь!";
                                link.l1 = "Хорошо, сэр. Полагаю, отобьемся.";
                                link.l1.go = "exit";
                                pchar.questTemp.State = "SpanishAttack_ShipsInShore";
                                SetQuestHeader("Eng_Line_10_SpanishAttack");
                                AddQuestRecord("Eng_Line_10_SpanishAttack", "1");
                                Pchar.quest.SpanishAttack_Battle.win_condition.l1 = "location";
                                Pchar.quest.SpanishAttack_Battle.win_condition.l1.location = "Jamaica";
                                Pchar.quest.SpanishAttack_Battle.win_condition = "SpanishAttack_Battle";
                                ChangeCharacterAddress(characterFromID("Henry Morgan"), "None", "");
                            break;
                            case "11":
                                dialog.text = "Вы как нельзя кстати! У меня для вас важнейшее поручение.";
                                link.l1 = "Слушаю вас, сэр.";
                                link.l1.go = "Step_11_1";
                            break;
                            case "12":
                                dialog.text = "Друг мой, у меня для вас несколько необычное поручение...";
                                link.l1 = "Внимательно слушаю.";
                                link.l1.go = "Step_12_1";
                            break;
                            case "13":
                                dialog.text = "Очень рад видеть вас у себя. И вы очень кстати, сэр.";
                                link.l1 = "Сэр?";
                                link.l1.go = "Step_12_5";
                            break;
                        }
                    }
                break;

                case "":           // Квест №1 - сопровождение Альбермаля
                    dialog.text = "У меня есть для вас очень важное поручение. Вы здесь человек новый, я не уверен, стоит ли мне доверять вам, но у меня, похоже, нет выбора.";
                    link.l1 = "Буду рад выполнить ваше поручение.";
                    link.l1.go = "Step_1_1";
                    link.l2 = "Извините, но я в данный момент решаю другую задачу.";
                    link.l2.go = "exit";
                break;
                case "Albermal_good_time":                  // Геймер успел доставить Альбермаля за неделю.
                    dialog.text = "В доставленном вами письме герцог Альбермаль дает вам весьма лестную оценку. Герцог - мой родственник, именно ему я обязан назначением на эту должность. Если бы не его активные и своевременные действия в Англии, не известно, чем бы закончилось мое губернаторство на Барбадосе...\n"+
                                  "Вы блестяще выполнили поставленную задачу. Обещанная награда составляет 50000. Вы заслужили эти деньги и мою признательность.\n"+
                                  "Рекомендация герцога Альбермаля - лучшее поручительство для меня. Если вы готовы продолжить сотрудничество с Англией, то прошу заходить ко мне время от времени.";
                    link.l1 = "Спасибо, сэр. Вы можете рассчитывать на меня в дальнейшем.";
                    link.l1.go = "exit";
    				AddMoneyToCharacter(pchar, 50000);
    				ChangeCharacterReputation(pchar, 5);
					ChangeCharacterNationReputation(pchar, sti(NPChar.nation), 2);
    				AddCharacterExpToSkill(pchar, "Defence", 100);
    				AddTitleNextRate(sti(NPChar.nation), 1);
                    CloseQuestHeader("Eng_Line_1_Albermal");
                    pchar.questTemp.State = "empty";
                    SaveCurrentQuestDateParam("questTemp");
                    TakeItemFromCharacter(pchar, "letter_Albermal");
					//слухи
					AddSimpleRumour("Вы слышали, капитана " + GetMainCharacterNameGen() + " наш генерал-губернатор очень хвалил. Тот, якобы, сумел тайно переправить с острова его лондонского дядю и покровителя - пэра Англиии, герцога Альбермаля. Вы не знаете, кто такой Альбермаль?! Скажу я вам по секрету, что только благодаря ему сэр Томас Мэдифорд исполняет обязанности губернатора Ямайки, иначе ему бы не поздоровилось...", ENGLAND, 5, 1);
                break;
                case "Albermal_late":  // Геймер не успел за неделю доставить Альбермаля.
                    dialog.text = "Что ж, в доставленном вами письме Альбермаль извещает меня о своем благополучном прибытии в Сент-Джонс. Вы отбили нападение испанского брига, но вы не смогли доставить герцога до места назначения в срок. Ваша награда составляет 20000. Я бы не сказал, что доволен вами в полной мере.";
                    link.l1 = "Сэр, прошу извинить за опоздание, но меня задержали обстоятельства непреодолимой силы. Надеюсь, я могу рассчитывать на получение особых поручений в дальнейшем?";
                    link.l1.go = "Step_1_4";
                break;
                case "after_talks_with_pirates":  // После опроса пиратов
                    dialog.text = "Вы заручились поддержкой пиратов? Когда Мэнсфилд планирует акцию?";
                    link.l1 = "Сэр, к сожалению, мне не удалось договориться ни с одним из указанных вами пиратов. Везде мной получен отказ атаковать Кюрасао. Причины, по которым корсары отказались произвести нападение, различны.";
                    link.l1.go = "Step_2_3";
                break;
                case "Intelligence_Curacao_Sink":  // Голландский флейт с инфой на борту был потоплен, провал миссии
                    dialog.text = "Докладывайте, что вы узнали.";
                    link.l1 = "К сожалению, мне не удалось захватить корабль, на котором я должен был найти необходимые бумаги. Мне не удалось узнать ничего толкового.";
                    link.l1.go = "Step_4_1";
                break;
                case "Intelligence_Curacao_Boarding":  // Миссия выполнена, голландский флейт взят на абордаж
                    dialog.text = "Докладывайте, что вы узнали.";
                    link.l1 = "Вот документ, который был найден на захваченном голландском бриге. Это приказ коменданту форта Оранж срочно укрепить город.";
                    link.l1.go = "Step_4_2";
                break;
                case "Intelligence_Curacao_BoardingNotFoundLetter":  // Миссия провалена, голландский флейт взят на абордаж, но письмо не найдено
                    dialog.text = "Докладывайте, что вы узнали.";
                    link.l1 = "Я получил информацию о голландском бриге-курьере. Мне удалось взять этот бриг на абордаж, но никаких документов я там не нашел.";
                    link.l1.go = "Step_4_1";
                break;
                case "Intelligence_Curacao_OutPrison":  // Миссия провалена, ГГ побывал в тюрьме
                    dialog.text = "Докладывайте, что вы узнали.";
                    link.l1 = "К сожалению, я был схвачен и заключен в тюрьму. Мне удалось бежать, но я ничего не узнал.";
                    link.l1.go = "Step_4_4";
                break;
                case "Intelligence_Curacao_FightGard": // Миссия провалена, ГГ дрался и ушел из города ни с чем
                    dialog.text = "Докладывайте, что вы узнали.";
                    link.l1 = "К сожалению, в процессе выполнения задания меня разоблачили. Был бой, и я едва вырвался из города. Теперь о моем появлении на Кюрасао не может быть и речи. Ничего интересного для вас мне узнать не удалось.";
                    link.l1.go = "Step_4_5";
                break;
                case "AttackFortOrange_WeWin":   // Форт Оранж взят.
                    dialog.text = "Вы весь в крови.";
                    link.l1 = "Это голландская кровь... Мы сделали это, Форт Оранж разграблен. Моя команда неплохо поживилась. Задание выполнено.";
                    link.l1.go = "Step_5_2";
                break;
                case "MorrisWillams_AfterFirstTalkingWithMorris": // Получено согласие Морриса Уильямса зайти в Порт Рояль.
                    dialog.text = "Так, что скажете, " + GetFullName(pchar) + "?";
                    link.l1 = "Сэр, ваше поручение выполнено, Моррис Уильямc заходит в порт.";
                    link.l1.go = "Step_6_2";
                break;
                case "MorrisWillams_ToModifordForCoolTalking": // На разборки после разговора с Уильямсом в таверне
                    dialog.text = "А-а-а, вот и вы опять, " + GetFullName(pchar) + ". Слушаю вас внимательно.";
                    link.l1 = "Черт возьми! Что происходит?";
                    link.l1.go = "Step_6_3";
                break;
                case "MorrisWillams_ArrestedToModiford": // На разборки после разговора с начальником тюрьмы
                    dialog.text = "А-а-а, вот и вы опять, " + GetFullName(pchar) + ". Что привело вас ко мне на этот раз?";
                    link.l1 = "Сэр, мне нужно поговорить с вами.";
                    link.l1.go = "Step_6_6";
                break;
                case "MorrisWillams_ModifordGood5000":
                    dialog.text = "А-а-а, друг мой, вы что, уже успели потратить пять тысяч вознаграждения и вам требуется работа?";
                    link.l1 = "Нет, я зашел поговорить на отвлеченные темы.";
                    link.l1.go = "exit";
                break;
                case "MorrisWillams_ModifordPolitic":
                    dialog.text = "Мне нечего вам более сказать. Политика вершит судьбы государств, нечего и думать о судьбах отдельных каперов...";
                    link.l1 = "Очень жаль, сэр.";
                    link.l1.go = "exit";
                break;
                case "MorrisWillams_SeekSpanishGoods":
                    dialog.text = "Ну что, друг мой, вы уже успокоились по поводу Уильямса? Мой вам совет - оставьте это дело, бедняге не повезло. Но такова жизнь, кто-то должен быть проигравшим...";
                    link.l1 = "Сэр, я в крайней степени ценю ваш добрый совет. Постараюсь ему следовать...";
                    link.l1.go = "exit";
                break;
                case "MorrisWillams_PinnaceBoard":
                    dialog.text = "Слышал, вы взяли на абордаж испанский пинас прямо возле Сан-Хуана под огнем форта и сторожевых линейных кораблей?!! Надо сказать, рискованная операция...";
                    link.l1 = "Все хорошо, что хорошо кончается, сэр...";
                    link.l1.go = "exit";
                break;
                case "MorrisWillams_PinnaceBoardNotFoundMoney":
                    dialog.text = "Слышал, вы взяли на абордаж испанский пинас прямо возле Сан-Хуана?!! Скажите, а риск был оправдан?";
                    link.l1 = "Нет, сэр. Ничего толкового в трюмах...";
                    link.l1.go = "exit";
                break;
                case "MorrisWillams_PinnaceSink":
                    dialog.text = "Слышал, вы потопили испанский пинас прямо возле Сан-Хуана. И зачем лезть прямо под огонь форта и кораблей охранения?";
                    link.l1 = "Так вышло, сэр...";
                    link.l1.go = "exit";
                break;
                case "MorrisWillams_DieHard":
                    dialog.text = "Начальник порта доложил мне, что вы бросились в погоню за испанским пинасом. Скажите, друг мой, удачно? (с издевкой)";
                    link.l1 = "Нет, сэр. Старался зря...";
                    link.l1.go = "exit";
                break;
                case "SevenTreasures_GoOn":
                    dialog.text = "Вы нашли золото?";
                    link.l1 = "Нет еще, но я беспрерывно веду поиски. Задача непроста...";
                    link.l1.go = "exit";
                    if (CheckCharacterItem(pchar, "jewelry5"))
                    {
                        link.l1 = "Кое-что есть.";
                        link.l1.go = "Step_7_4";
                    }
                    link.l2 = "Знаете, генерал-губернатор, мне это надоело до смерти. Я более не желаю носиться по Карибам и искать для вас золото.";
                    link.l2.go = "Step_7_10";
                break;
                case "ToMansfield_CatchToPardal":
                    dialog.text = "Вы привезли золото от Мэнсфилда?";
                    link.l1 = "Сэр, поселение буканьеров подверглось нападению испанцев. Мэнсфилд убит, золото похищено. Адмиралом испанской эскадры, по всей видимости, был Маноэль Риверо Пардаль.";
                    link.l1.go = "Step_8_3";
                break;
                case "ToMansfield_NotFoundGaleon":
                    dialog.text = "Вы привезли золото от Мэнсфилда?";
                    link.l1 = "Сэр, Ла Вега подверглась нападенияю испанцев. Мэнсфилд убит, золото похищено. Адмиралом испанской эскадры был Маноэль Риверо Пардаль. Сэр, я проверил все вплоть до Сантьяго, но не встретил напавших испанцев.";
                    if (CheckAttribute(npchar , "TempQuest.KnowAboutModiford"))
                    {
                        dialog.text = "Вы смогли настигнуть испанцев?";
                        link.l1 = "Нет, сэр. Мне это не удалось. Я проверил все вплоть до Сантьяго, но не встретил галеонов.";
                    }
                    link.l1.go = "Step_8_5";
                break;
                case "ToMansfield_Boarding":
                    dialog.text = "Вы привезли золото от Мэнсфилда?";
                    link.l1 = "Сэр, Ла Вега подверглась нападению испанцев. Мэнсфилд убит, золото похищено. Адмиралом испанской эскадры был Маноэль Риверо Пардаль. Мне удалось настигнуть один из трех нападавших галеонов и взять его на абордаж. Золота там не было, но в каюте капитана я нашел письмо Пардаля губернатору Сантьяго Хименесу. Вот оно.";
                    if (CheckAttribute(npchar , "TempQuest.KnowAboutModiford"))
                    {
                        dialog.text = "Вы смогли настигнуть испанцев?";
                        link.l1 = "Рядом с Сантьяго я сумел взять на абордаж испанский галеон, один из тех, что участвовали в нападении на буканьеров. Золота там не было, но в каюте капитана было найдено письмо от Пардаля губернатору Сантьяго Хименесу, вот оно.";
                    }
                    link.l1.go = "Step_8_5";
                    npchar.TempQuest.KindResult = 1; // признак результата завершения квеста, 1 == было захвачено письмо
        			AddCharacterExpToSkill(pchar, "Cannons", 600);
        			AddCharacterExpToSkill(pchar, "Sailing", 750);
        			AddCharacterExpToSkill(pchar, "Defence", 550);
        			AddCharacterExpToSkill(pchar, "Accuracy", 700);
        			AddTitleNextRate(sti(NPChar.nation), 1);
                break;
                case "ToMansfield_BoardingNotFoundLetter":
                    dialog.text = "Вы привезли золото от Мэнсфилда?";
                    link.l1 = "Сэр, Ла Вега подверглась нападению испанцев. Мэнсфилд убит, золото похищено. Адмиралом испанской эскадры был Маноэль Риверо Пардаль. Мне удалось настигнуть один из трех нападавших галеонов и взять его на абордаж. Золота я там не нашел. По-видимому, оно находилось на другом корабле. Куда ушли два других галеона, мне неизвестно.";
                    if (CheckAttribute(npchar , "TempQuest.KnowAboutModiford"))
                    {
                        dialog.text = "Вы смогли настигнуть испанцев?";
                        link.l1 = "Рядом с Сантьяго я сумел взять на абордаж испанский галеон, один из тех, что участвовали в нападении на буканьеров. Золота я там не нашел. По видимому оно находилось на другом корабле. Куда ушли два других галеона, мне неизвестно.";
                    }
                    link.l1.go = "Step_8_5";
        			AddCharacterExpToSkill(pchar, "Repair", 40);
        			AddCharacterExpToSkill(pchar, "Commerce", 50);
        			AddCharacterExpToSkill(pchar, "Defence", 20);
        			AddCharacterExpToSkill(pchar, "Leadership", 10);
        			AddTitleNextRate(sti(NPChar.nation), 0.5);
                break;
                case "ToMansfield_GaleonSink":
                    dialog.text = "Вы привезли золото от Мэнсфилда?";
                    link.l1 = "Сэр, Ла Вега подверглась нападенияю испанцев. Мэнсфилд убит, золото похищено. Адмиралом испанской эскадры был Маноэль Риверо Пардаль. Мне удалось настигнуть один из трех нападавших галеонов и потопить его. Куда ушли остальные два галеона мне неизвестно.";
                    if (CheckAttribute(npchar , "TempQuest.KnowAboutModiford"))
                    {
                        dialog.text = "Вы смогли настигнуть испанцев?";
                        link.l1 = "Рядом с Сантьяго я сумел взять на абордаж испанский галеон, один из тех, что учавствовали в нападении на буканьеров. Мне удалось настигнуть один из трех нападавших галеонов и потопить его. Куда ушли два других галеона, мне неизвестно.";
                    }
                    link.l1.go = "Step_8_5";
        			AddCharacterExpToSkill(pchar, "Repair", 5);
        			AddCharacterExpToSkill(pchar, "Commerce", 10);
        			AddCharacterExpToSkill(pchar, "Cannons", -50);
        			AddCharacterExpToSkill(pchar, "Accuracy", -70);
        			AddCharacterExpToSkill(pchar, "Fencing", -100);
					ChangeCharacterNationReputation(pchar, sti(NPChar.nation), -1);
                break;
                case "ToMansfield_DidNotFindBecaners":
                    dialog.text = "Вы привезли золото от Мэнсфилда?";
                    link.l1 = "Сэр, когда я прибыл в Ла Вегу, то он оказался пуст! Ни единой живой души. Я обыскал окрестности, но ничего не нашел.";
                    link.l1.go = "Step_8_5";
                    npchar.TempQuest.KindResult = 2; // признак результата завершения квеста, 2 == вообще не в курсе, что произошло
        			AddCharacterExpToSkill(pchar, "Cannons", 10);
        			AddCharacterExpToSkill(pchar, "Pistol", -200);
        			AddCharacterExpToSkill(pchar, "Defence", -100);
        			AddCharacterExpToSkill(pchar, "Accuracy", -210);
        			AddCharacterExpToSkill(pchar, "Fencing", -400);
					ChangeCharacterNationReputation(pchar, sti(NPChar.nation), -1);
                break;
                case "AttackSantiago_GoodWork":
                    dialog.text = "Я уже знаю о ваших подвигах. Это блестящая победа, друг мой.";
                    link.l1 = "Спасибо, сэр. Думаю, вам будет приятно узнать, что губернатор Сантьяго является моим пленником и находится у меня на борту.";
                    link.l1.go = "Step_9_4";
                break;
                case "SpanishAttack_GoodWork":
                    dialog.text = "Черт возьми, вы буквально спасли всех нас! Город Порт-Рояль обязан вам жизнью!";
                    link.l1 = "Да уж, испанцы собрали силы, о наличии которых я и догадываться не мог...";
                    link.l1.go = "Step_10_1";
                break;
                case "OccupyMein_GoOn":
                    dialog.text = "Вы еще не захватили ни Куманы, ни Каракаса. Все побережье Мейна до сих пор во власти испанцев...";
                    link.l1 = "Сэр, я этим занимаюсь. Задача не проста и требует серьезной подготовки. Нападение будет совершено по мере готовности.";
                    link.l1.go = "exit";
                    if (CheckAttribute(pchar, "questTemp.Q11_Caracas"))
                    {
                        dialog.text = "Вы захватили Каракас! Это прекрасно, друг мой. Но Кумана еще не захвачена. Поторопитесь!";
                        link.l1 = "Хорошо, сэр.";
                        link.l1.go = "exit";
                    }
                    if (CheckAttribute(pchar, "questTemp.Q11_Cumana"))
                    {
                        dialog.text = "Кумана принадлежит Англии, и это ваша заслуга! Превосходно, друг мой! Но я жду от вас так же захвата Каракаса, который также должен быть захвачен!";
                        link.l1 = "Я все сделаю, сэр.";
                        link.l1.go = "exit";
                    }
                    if (CheckAttribute(pchar, "questTemp.Q11_Caracas") && CheckAttribute(pchar, "questTemp.Q11_Cumana"))
                    {
                        dialog.text = "Кумана и Каракас захвачены! Теперь это побережье Мейна принадлежит Англии!";
                        link.l1 = "Да, сэр. Мы сделали это.";
                        link.l1.go = "Step_11_2";
                    }
                break;
                case "ColonelLinch_ArrestModiford":
                    dialog.text = "Рассказывайте, что вы выяснили? Что происходит в Сент-Джонсе?";
                    link.l1 = "Сэр, в резиденции губернатора Сент-Джонса я встретил полковника Томаса Линча.";
                    link.l1.go = "Step_12_2";
                break;
                case "ColonelLinch_ModifordArrested":
                    dialog.text = "Вы выполнили поставленную задачу, Мэдифорд арестован и помещен в тюрьму. Сейчас начинается всестороннее расследование его деятельности на посту генерал-губернатора Ямайки. Я очень хотел бы видеть вас у себя примерно через месяц.";
                    link.l1 = "Хорошо, сэр.";
                    link.l1.go = "exit";
                    pchar.questTemp.State = "empty";
                    SaveCurrentQuestDateParam("questTemp");
                    pchar.questTemp.CurQuestNumber = "13";
                    pchar.questTemp.Waiting_time = "30";
                    CloseQuestHeader("Eng_Line_12_ColonelLinch");
                break;
                case "QuestLineBreake":
                    dialog.text = "Я не имею дело с людьми, нарушающими свое слово. Мне больше нечего вам сказать.";
                    link.l1 = "...";
                    link.l1.go = "exit";
                    bWorldAlivePause   = false; // Конец линейки
                break;
                case "EndOfQuestLine":
                    dialog.text = "Сэр, я доверяю вашему видению ситуации в Карибском море. Поэтому вам предоставлена полная свобода действий в интересах Англии.";
                    link.l1 = "Я понял, сэр. Благодарю за доверие.";
                    link.l1.go = "exit";
                    bWorldAlivePause   = false; // Конец линейки
                break;
             }
		break;

		case "Step_1_1":
            dialog.text = "Сейчас у меня гостит мой близкий родственник, герцог Альбермаль. Его миссия здесь закончена, и я должен срочно препроводить его в Британию. На острове Антигуа, в Сент-Джонсе, герцога ждет фрегат королевских военно-морских сил. Доставьте герцога в Сент-Джонс, в резиденцию к губернатору.\n"+
                          "Немедленно отправляйтесь и будьте осторожны, в Порт Рояле отловили уже двоих испанских шпионов и я абсолютно уверен, что в городе еще остались агенты. Я должен исходить из того, что о присутствии здесь столь высокопоставленной английской персоны испанцы знают. Кроме того, мне докладывают о странной активности испанских военных судов вокруг Ямайки.";
            link.l1 = "Хм, действительно, лучше рассчитывать на худшее.";
            link.l1.go = "Step_1_2";
        break;

        case "Step_1_2":
            dialog.text = "Именно в данный момент мне нужен капитан с не привлекающим внимание судном. Мой расчет основан на внезапности. Вас никто не знает, ваш парусник также не известен. У вас меньше всего риска благополучно доставить герцога Альбермаля к Антигуа.";
            link.l1 = "Полагаю, вы правы, сэр.";
            link.l1.go = "Step_1_3";
        break;

        case "Step_1_3":
            dialog.text = "Сделайте это в течение двух недель, и вы будете щедро вознаграждены. За герцога Альбермаля отвечаете головой. Он сейчас находится у меня в резиденции в отдельном кабинете. Приступайте, как только будете готовы, и не тяните с этим.";
            link.l1 = "Я понял вас, сэр.";
            link.l1.go = "exit";
            SaveCurrentQuestDateParam("questTemp");
			// остальные линейки в сад -->
			pchar.questTemp.NationQuest = ENGLAND;
			// остальные линейки в сад <--
            pchar.questTemp.State = "to_Albermal";               // переменная состояния квеста для линеек
            sld = GetCharacter(NPC_GenerateCharacter("Albermal", "Albermal", "man", "man2", 10, ENGLAND, -1, false));
        	sld.name 	= "герцог";
        	sld.lastname = "Альбермаль";
            sld.Dialog.Filename = "Quest\EngLineNpc_1.c";
        	LAi_SetHuberStayType(sld);
        	LAi_group_MoveCharacter(sld, "ENGLAND_CITIZENS");
            ChangeCharacterAddressGroup(&characters[GetCharacterIndex("Albermal")], "PortRoyal_RoomTownhall", "goto", "goto3");
            AddQuestRecord("Eng_Line_1_Albermal", "1");
        break;
            
        case "Step_1_4":
            dialog.text = "Думаю, да. Но впредь будьте более тщательны в исполнении моих заданий. Вы - английский капер. Интересы Англии превыше всего!";
            link.l1 = "Вы правы, сэр. Вы можете твердо рассчитывать на то, что я буду более исполнителен.";
            link.l1.go = "exit";
			AddMoneyToCharacter(pchar, 20000);
			pchar.questTemp.Waiting_time = 10;
			ChangeCharacterReputation(pchar, 1);
			ChangeCharacterNationReputation(pchar, sti(NPChar.nation), 1);
            CloseQuestHeader("Eng_Line_1_Albermal");
            pchar.questTemp.State = "empty";
            SaveCurrentQuestDateParam("questTemp");
            TakeItemFromCharacter(pchar, "letter_2");
            BackItemDescribe("letter_2");
			AddCharacterExpToSkill(pchar, "Leadership", -70);
			//слухи			
			AddSimpleRumour("Вы знаете, капитан " + GetFullName(pchar) + " сумел выполнить очень опасное задание Мэдифорда. Ему нужно было доставить в безопасное место его герцога Альбермаля... Вы не знаете, кто такой Альбермаль?! Скажу я вам по секрету, что только благодаря ему сэр Томас Мэдифорд исполняет обязанности губернатора Ямайки... Только капитан со сроками подвел.", ENGLAND, 5, 1);
        break;

        case "Step_2_1":
            dialog.text = "В Европе идет торговая война с Голландией. Англо-франкский флот подошел к берегам Голландии, но, к величайшему удивлению, был разгромлен. Сейчас, по приказу правительства Англии, я должен организовать нападение на голландскую колонию Кюрасао.\n"+
                          "Однако, в моем распоряжении нет сейчас сил, способных справиться с такой крепостью, как Виллемстад. Я привлеку для нападения английских каперов. По сути, мне очень выгодно держать импровизированный пиратский флот в Порт Рояле - хорошая защита и совершенно бесплатно для короны! Хех...";
            link.l1 = "В чем заключается мое задание?";
            link.l1.go = "Step_2_2";
        break;

        case "Step_2_2":
            dialog.text = "Мне необходимо, чтобы вы нашли Эдварда Мэнсфилда в поселении буканьеров Ла Вега, что на Эспаньоле, Генри Моргана, находящегося в данный момент в Сент-Джонсе, Джекмена в пиратском поселении на Бермудах, и Джона Морриса на острове Тринидад. Вы должны получить согласие этих пиратов произвести нападение на Кюрасао, которое возглавит Мэнсфилд. Выполняйте как можно быстрей, я рассчитываю на вас.";
            link.l1 = "Приступаю немедленно, сэр.";
            link.l1.go = "Step_2_21";
        break;
        
        case "Step_2_21":
            dialog.text = "И отдельно я бы хотел поговорить о Моргане. Дело в том, что его дом на Антигуа представляет из себя настоящую крепость. Так просто туда не попасть. Вам надлежит пробраться в дом через подземелье, находящееся под Сент-Джонсом.";
            link.l1 = "Я понял, сэр. Спасибо за предупреждение. Полагаю, разберусь, что к чему.";
            link.l1.go = "exit";
            SaveCurrentQuestDateParam("questTemp");
            pchar.questTemp.State = "go_talks_with_pirates";          // переменная состояния квеста для линеек
            AddQuestRecord("Eng_Line_2_Talking", "1");
            // ==>  Помещаем Моргана на Антигуа и даем ноду доступа к квестам
            ChangeCharacterAddressGroup(&characters[GetCharacterIndex("Henry Morgan")], "SentJons_HouseF3", "goto", "goto1");
            QuestSetCurrentNode("Henry Morgan", "EngLine_quests");
            sld = characterFromID("Henry Morgan");
            LAi_SetHuberStayType(sld);
            LAi_SetLoginTime(sld, 0.0, 24.0);
            // ==>  Генерим и помещаем Джона Морриса в Бриджтаун и даем ноду доступа к квестам
            sld = GetCharacter(NPC_GenerateCharacter("John Morris", "bocman", "man", "man2", 38, PIRATE, -1, false));
        	sld.name 	= "Джон";
        	sld.lastname = "Моррис";
            sld.Dialog.Filename = "Quest\JohnMorris.c";
            FantomMakeCoolSailor(sld, SHIP_GALEON_H, "Лагуна", CANNON_TYPE_CANNON_LBS32, 90, 90, 90);
            FantomMakeCoolFighter(sld, 35, 90, 50, BLADE_LONG, "pistol3", 150);
            LAi_NoRebirthDisable(sld);
			LAi_RebirthOldName(sld);
			sld.RebirthPhantom = true; //не тереть фантома после смерти.  
           	LAi_SetHuberType(sld);
        	LAi_SetLoginTime(sld, 0.0, 24.0);
        	LAi_SetHP(sld, 180.0, 180.0);
            ChangeCharacterAddressGroup(sld, "PortSpein_houseF2", "sit", "governor");
            LAi_LocationFightDisable(&Locations[FindLocation("PortSpein_houseF2")], true);
            // ==>  Даем ноду доступа к квестам Джону Моррису
            QuestSetCurrentNode("John Morris", "EngLine_quests");
            // ==>  Даем ноду доступа к квестам Джекмену
            QuestSetCurrentNode("Jackman", "EngLine_quests");
            // ==>  Даем ноду доступа к квестам Манфсфилду
            QuestSetCurrentNode("Edward Mansfield", "EngLine_quests");
            // ==>  Оформляем счетчик посещения корсаров, в итоге должен == 4
            pchar.questTemp.Count = "0";
        break;

        case "Step_2_3":
            dialog.text = "Как вы считаете, что-либо можно изменить в дальнейшем?";
            link.l1 = "Сложно сказать, но не думаю, сэр. Всех четверых не уговорить точно.";
            link.l1.go = "Step_2_4";
        break;

        case "Step_2_4":
            if (GetQuestPastDayParam("questTemp") < 35)
            {
                dialog.text = "Хм, понятно. Что ж, несмотря на отрицательный результат миссии, я доволен вашей расторопностью и исполнительностью. Вам полагается 15 000 вознаграждения. Надеюсь, впредь я могу пользоваться вашими услугами?";
                link.l1 = "Конечно, сэр. Буду рад помочь вам.";
                AddMoneyToCharacter(pchar, 15000);
				ChangeCharacterReputation(pchar, 3);
				AddCharacterExpToSkill(pchar, "Repair", 150);
				AddCharacterExpToSkill(pchar, "Sailing", 100);
				AddTitleNextRate(sti(NPChar.nation), 2);
				ChangeCharacterNationReputation(pchar, sti(NPChar.nation), 1);
            }
            else
            {
                dialog.text = "Я крайне разочарован, приказ английского правительства не будет выполнен. Это плохо, к тому же вы были недостаточно расторопны при выполнении данного задания. Вы затратили " +
                                GetQuestPastDayParam("questTemp") + " дней! Оплата будет соответствующей, то есть низкой.";
                link.l1 = "Что ж, вам видней.";;
                AddMoneyToCharacter(pchar, 800);
				ChangeCharacterReputation(pchar, 1);
				ChangeCharacterNationReputation(pchar, sti(NPChar.nation), -1);
            }
            link.l1.go = "exit";
			//слухи
			AddSimpleRumour("Вы знаете, капитан " + GetFullName(pchar) + " поработал на побегушках у Мэдифорда - мотался по всему Карибскому морю из конца в конец, даже к Моргану наведывался...", ENGLAND, 5, 1);
            DeleteAttribute(pchar, "questTemp.Count");
            pchar.questTemp.State = "empty";
            AddQuestRecord("Eng_Line_2_Talking", "6");
            CloseQuestHeader("Eng_Line_2_Talking");
            SaveCurrentQuestDateParam("questTemp");
            pchar.questTemp.CurQuestNumber = "3";
            pchar.questTemp.Waiting_time = "70";    // Чтобы Мэдифорд дал квест №4 через 70 дней, даже если ГГ не зашел к Моргану
            // ==> Моргана - в резиденцию на Ямайке
            LAi_SetHuberType(characterFromID("Henry Morgan"));
            QuestSetCurrentNode("Henry Morgan", "hot_business");
            ChangeCharacterAddressGroup(&characters[GetCharacterIndex("Henry Morgan")], "PortRoyal_houseS1", "sit", "sit2");
        break;

        case "Step_4_0":
            dialog.text = "Для того, чтобы облегчить выполнение поручения, я даю вам лицензию Голландской Вест-Индской компании сроком на 20 дней, которая позволит вам спокойно войти в город. Приступайте немедленно.";
            link.l1 = "Хорошо, сэр.";
            link.l1.go = "exit";
			GiveNationLicence(HOLLAND, 20);
			pchar.questTemp.State = "Intelligence_Curacao_tavern";
			SetQuestHeader("Eng_Line_4_Intelligence_Curacao");
			AddQuestRecord("Eng_Line_4_Intelligence_Curacao", "1");
			SaveCurrentQuestDateParam("questTemp");
		break; 

        case "Step_4_1":
            dialog.text = "Это очень плохо, я в вас разочарован. Вы можете появиться здесь позже, когда мне удастся все-таки получить нужную информацию.";
            link.l1 = "Конечно, сэр. Извините, что не смог помочь вам.";
            link.l1.go = "exit";
			TakeNationLicence(HOLLAND);
            CloseQuestHeader("Eng_Line_4_Intelligence_Curacao");
            pchar.questTemp.State = "empty";
            pchar.questTemp.CurQuestNumber = "5";
            SaveCurrentQuestDateParam("questTemp");
            pchar.questTemp.Waiting_time = "30";    //только через месяц следующий квест, т.к. Мэдифорд будет добывать инфу без участия ГГ
			AddCharacterExpToSkill(pchar, "Sailing", -140);
			AddCharacterExpToSkill(pchar, "Sneak", -180);
			ChangeCharacterNationReputation(pchar, sti(NPChar.nation), -1);
			//слухи
			AddSimpleRumour("Говорят, что вы не смогли провести разведку на Кюрасао. Хм, надо же...", ENGLAND, 5, 1);
        break;

        case "Step_4_2":
            dialog.text = "Вы сумели раздобыть сам документ?! Прекрасно, я в вас не ошибся.\n"+
                          "Однако, голландцы узнали о наших планах... Я прошу вас появиться у меня несколько позже, я должен подумать. За отлично выполненное задание вам полагается вознаграждение в размере 100 000.";
            link.l1 = "Сэр, мне пришлось подкупить ростовщика в Виллемстаде. Сумма подкупа составляет как раз сто тысяч. Этим вознаграждением вы всего лишь компенсируете мои затраты.";
            link.l1.go = "Step_4_3";
        break;

        case "Step_4_3":
            dialog.text = "Ну что ж, пусть вам выдадут 110 000, я распоряжусь. Теперь оставьте меня на время. Прошу вас заглянуть ко мне через пару дней.";
            link.l1 = "Огромное вам спасибо, сэр. Не премину заглянуть позже.";
            link.l1.go = "exit";
            AddQuestRecord("Eng_Line_4_Intelligence_Curacao", "10");
            CloseQuestHeader("Eng_Line_4_Intelligence_Curacao");
            pchar.questTemp.State = "empty";
            pchar.questTemp.CurQuestNumber = "5";
            SaveCurrentQuestDateParam("questTemp");
            pchar.questTemp.Waiting_time = "2";
            AddMoneyToCharacter(pchar, 110000);
			AddCharacterExpToSkill(pchar, "Sailing", 250);
			AddCharacterExpToSkill(pchar, "Sneak", 170);
			AddCharacterExpToSkill(pchar, "Grappling", 400);
			AddTitleNextRate(sti(NPChar.nation), 2);
			TakeItemFromCharacter(pchar, "letter_open");
			BackItemDescribe("letter_open");
			TakeNationLicence(HOLLAND);
			ChangeCharacterNationReputation(pchar, sti(NPChar.nation), 1);
			//слухи
			AddSimpleRumour("Говорят, что вы проникли в Виллемстад и сумели разузнать все голландские секреты. Полагаю, что теперь мы будем лучше подготовлены к надвигающейся войне...",ENGLAND, 5, 1);
        break;

        case "Step_4_4":
            dialog.text = "То, что вы побывали в голландской тюрьме, меня волнует мало. Мне нужна информация, а у вас ее нет. Я разочарован. Вы можете появиться здесь позже, когда мне удастся все-таки получить нужную информацию.";
            link.l1 = "Конечно, сэр. Извините, что не смог помочь вам.";
            link.l1.go = "exit";
            CloseQuestHeader("Eng_Line_4_Intelligence_Curacao");
            pchar.questTemp.State = "empty";
            pchar.questTemp.CurQuestNumber = "5";
            SaveCurrentQuestDateParam("questTemp");
            pchar.questTemp.Waiting_time = "30";    // только через месяц следующий квест, т.к. Мэдифорд будет добывать инфу без участия ГГ
			AddCharacterExpToSkill(pchar, "Sailing", -150);
			AddCharacterExpToSkill(pchar, "Sneak", -200);
			AddCharacterExpToSkill(pchar, "Leadership", -150);
			ChangeCharacterNationReputation(pchar, sti(NPChar.nation), -1);
			//слухи
			AddSimpleRumour("Говорят, что вы умудрились побывать в голландской тюрьме. Ну и как там, крысы есть?", ENGLAND, 5, 1);
        break;

        case "Step_4_5":
            dialog.text = "Вы показали с хорошей стороны себя как воин, но там где требуется хитрость, вы справиться не смогли.\n"+
                          "Мне нужно будет добывать нужную мне информацию через других людей и когда я добуду ее, возможно, вы будете мне нужны.";
            link.l1 = "Я понял, сэр. Извините, что не смог помочь вам.";
            link.l1.go = "exit";
            CloseQuestHeader("Eng_Line_4_Intelligence_Curacao");
            pchar.questTemp.State = "empty";
            pchar.questTemp.CurQuestNumber = "5";
            SaveCurrentQuestDateParam("questTemp");
			TakeNationLicence(HOLLAND);
            pchar.questTemp.Waiting_time = "30";    // только через месяц следующий квест, т.к. Мэдифорд будет добывать инфу без участия ГГ
			AddCharacterExpToSkill(pchar, "Sailing", -150);
			AddCharacterExpToSkill(pchar, "Sneak", -200);
			AddCharacterExpToSkill(pchar, "Fencing", -300);
			ChangeCharacterNationReputation(pchar, sti(NPChar.nation), -1);
			//слухи
			AddSimpleRumour("Говорят, вы устроили в Виллемстаде хорошую заварушку...", ENGLAND, 5, 1);
        break;
        
        case "Step_5_1":
            dialog.text = "Что ж, теперь мы имеем информацию о том, что голландцы спешно пытаются укрепить свое поселение на Ямайке - Форт Оранж. В принципе, то, что они поселились на другом конце острова - для меня проблемы не составляло, но только пока не началась торговая война.\n"+
                          "Теперь мой долг разорить голландское поселение на английской территории. Я поручаю это вам.\n"+
                          "Весь захваченный приз в Форте Оранж - ваш. Действуйте.";
            link.l1 = "Очень хорошо, сэр.";
            link.l1.go = "exit";
         	pchar.GenQuestBox.Trinidad_Cave = true;
            pchar.GenQuestBox.FortOrange_townhall.box1.items.jewelry3 = Rand(5)+1;
            pchar.GenQuestBox.FortOrange_townhall.box1.items.jewelry4 = Rand(5)+1;
            pchar.GenQuestBox.FortOrange_townhall.box1.items.jewelry5 = Rand(5)+1;
            pchar.GenQuestBox.FortOrange_townhall.box1.items.jewelry7 = Rand(5)+1;
            pchar.GenQuestBox.FortOrange_townhall.box1.items.jewelry8 = Rand(5)+1;
            pchar.GenQuestBox.FortOrange_townhall.box1.items.jewelry10 = Rand(5)+1;
            pchar.GenQuestBox.FortOrange_townhall.box1.items.jewelry14 = Rand(5)+1;
            pchar.GenQuestBox.FortOrange_townhall.box1.items.jewelry17 = Rand(5)+1;
            pchar.GenQuestBox.FortOrange_townhall.box1.items.cirass4 = 1;
			locations[FindLocation("FortOrange_townhall")].models.day.charactersPatch = "SmallResidence_BoxPatch";
			pchar.questTemp.State = "AttackFortOrange_GoToFort";
            SetQuestHeader("Eng_Line_5_AttackFortOrange");
            AddQuestRecord("Eng_Line_5_AttackFortOrange", "1");
            LAi_LocationFightDisable(&Locations[FindLocation("FortOrange_townhall")], false);
            Pchar.quest.AttackFortOrange_GoToFort.win_condition.l1 = "location";
            Pchar.quest.AttackFortOrange_GoToFort.win_condition.l1.location = "FortOrange_ExitTown";
            Pchar.quest.AttackFortOrange_GoToFort.win_condition = "AttackFortOrange_GoToFort";
        break;

        case "Step_5_2":
            dialog.text = "Да-а-а, хотел бы я видеть форт сразу после вашей работы... Один ваш вид чего стоит - просто демон во плоти!\n"+
                          "Я искренне уважаю отважных каперов, не боящихся лить ручейки красного цвета... Слава героям!\n"+
                          "Надеюсь видеть вас у себя чаще, мой друг.";
            link.l1 = "Бесспорно, сэр, вы будете видеть меня периодически... Я - капер, а дружба сэра Томаса Мэдифорда дорогого стоит. А сейчас разрешите откланяться - дела...";
            link.l1.go = "Step_5_3";
        break;

        case "Step_5_3":
            dialog.text = "Что ж, понимаю... По всему видать, где-то стонет девица в предвкушении вашего появления... Ха-ха.\n"+
                          "Друг мой, я искренне желаю вам провести время хорошо, с такой же полезной нагрузкой, так сказать... Талантливый человек талантлив во всем.\n"+
                          "Ну, не буду задерживать. До встречи...";
            link.l1 = "До свидания, сэр.";
            link.l1.go = "exit";
            pchar.questTemp.State = "empty";
            SaveCurrentQuestDateParam("questTemp");
            CloseQuestHeader("Eng_Line_5_AttackFortOrange");
            pchar.questTemp.CurQuestNumber = "6";
            pchar.questTemp.Waiting_time = "30"; // По идее геймер должен испытывать угрызения совести после этого задания. Пусть поболтается просто так с месяц.
            // ==> вертаем разграбленному форту Оранж нормальную жизнь.
            SetLocationCapturedState("FortOrange_town", false);
            LAi_LocationFightDisable(&Locations[FindLocation("FortOrange_townhall")], true);
			//--> огонь и пламень убираем
			DeleteAttribute(&locations[FindLocation("FortOrange_town")], "hidden_effects");
			DeleteAttribute(&locations[FindLocation("FortOrange_ExitTown")], "hidden_effects");	
			//<-- огонь и пламень
			locations[FindLocation("FortOrange_townhall")].models.day.charactersPatch = "SmallResidence_patch";
			AddCharacterExpToSkill(pchar, "Pistol", 250);
			AddCharacterExpToSkill(pchar, "Leadership", 250);
			AddCharacterExpToSkill(pchar, "Repair", 350);
			AddTitleNextRate(sti(NPChar.nation), 2);
			ChangeCharacterNationReputation(pchar, sti(NPChar.nation), 1);
			//слухи
			AddSimpleRumour("То, что вы устроили в Форте Оранж - это, знаете ли, впечатляет... Несчастные голландцы...", ENGLAND, 5, 1);
        break;
        
        case "Step_6_1":
            dialog.text = "Что ж, это хорошо. Так вот, он идет после удачного похода на испанцев, в его трюме груз красного дерева, серебра и индиго.\n"+
                          "Ему нужно продать трофеи, пусть заходит для этих целей в Порт Рояль. Передайте ему лично мое разрешение на продажу приза в английской колонии.\n"+
                          "Да, и я хочу, чтобы вы сказали это Уильямсу без свидетелей.";
            link.l1 = "Нет ничего проще.";
            link.l1.go = "exit";
            pchar.questTemp.State = "MorrisWillams_GoToMorrisBegin";
            SetQuestHeader("Eng_Line_6_MorrisWillams");
            AddQuestRecord("Eng_Line_6_MorrisWillams", "1");

			Pchar.quest.MorrisWillams_ShipToPort.win_condition.l1 = "location";
			Pchar.quest.MorrisWillams_ShipToPort.win_condition.l1.location = "Jamaica";
			Pchar.quest.MorrisWillams_ShipToPort.win_condition = "MorrisWillams_ShipToPort";
        break;

        case "Step_6_2":
            dialog.text = "Мне уже доложили. Вот вам 5000 за труды.";
            link.l1 = "Это не составило мне никакого труда, но от денег не откажусь.";
            link.l1.go = "exit";
            pchar.questTemp.State = "MorrisWillams_ModifordGood5000";
            AddMoneyToCharacter(pchar, 5000);
            AddTitleNextRate(sti(NPChar.nation), 0.5);
			ChangeCharacterNationReputation(pchar, sti(NPChar.nation), 1);
            AddQuestRecord("Eng_Line_6_MorrisWillams", "3");
            group_DeleteGroup("Willams_Group");

			Pchar.quest.MorrisWillams_ShipToPort.win_condition.l1 = "location";
			Pchar.quest.MorrisWillams_ShipToPort.win_condition.l1.location = Pchar.location.from_sea;
			Pchar.quest.MorrisWillams_ShipToPort.win_condition = "MorrisWillams_MeetWithManFromWillams";
        break;

        case "Step_6_3":
            dialog.text = "Что за манеры? В чем дело?";
            link.l1 = "Вы конфисковали груз капера Морриса Уильямса! А его пригласил в Порт Рояль я!";
            link.l1.go = "Step_6_4";
        break;

        case "Step_6_4":
            dialog.text = "Не Вы, а я пригласил его в Порт Рояль. Вы выполняли роль вестового, не более. Что касается груза, то он возвращен испанскому собственнику.";
            link.l1 = "Я своим ушам не верю! Зачем?!!";
            link.l1.go = "Step_6_5";
        break;

        case "Step_6_5":
            dialog.text = "Отношения между Англией и Испанией налаживаются. Это жест доброй воли.\n"+
                          "Друг мой, не лезьте в политику, вы ничего не сможете изменить. До свидания.";
            link.l1 = "У меня просто нет слов... Что ж, до свидания, сэр.";
            link.l1.go = "exit";
            pchar.questTemp.State = "MorrisWillams_ModifordPolitic";
            AddQuestRecord("Eng_Line_6_MorrisWillams", "5");
            QuestSetCurrentNode("Morris_Willams", "First time");
			//слухи
			AddSimpleRumour("Капитан, неужели это вы так подставили беднягу Уильямса?! Никто не думал, что вы способны на такое...", ENGLAND, 5, 1);
        break;

        case "Step_6_6":
            dialog.text = "Что стряслось, друг мой?";
            link.l1 = "Моррис Уильямс томится в тюрьме, причем по моей вине. Я считаю крайне необходимым освободить его.";
            link.l1.go = "Step_6_7";
        break;

        case "Step_6_7":
            dialog.text = "Что ж, я вижу, что вы человек благородный. Мне также не доставляет удовольствия держать хорошего капера под стражей, но я вынужден это сделать. Закон есть закон.\n"+
                          "Советую вам обратиться к местному ростовщику, именно вследствие неуплаты долга ему Моррис находится в тюрьме.";
            link.l1 = "Спасибо за совет.";
            link.l1.go = "exit";
            pchar.questTemp.State = "MorrisWillams_ArrestedToUsurer";
            AddQuestRecord("Eng_Line_6_MorrisWillams", "17");
        break;
        
        case "Step_7_1":
            dialog.text = "Дело в том, что английские корсары перестали заходить с призовым товаром, захваченным у испанцев. Они теперь заходят только на ремонт с минимально необходимым количеством денег.\n"+
                          "Надеюсь, вам не нужно объяснять, сколько Англия теряет каждый месяц на неоплатах десятипроцентной пошлины от размера захваченного груза, а сколько теряют ямайские негоцианты!!!\n"+
                          "Корсары теперь либо прячут грузы по укромным местам на островах, либо пытаются тащиться в Портсмут и там реализовывать товар, либо продают захваченные грузы через французских посредников на Тортуге.\n"+
                          "То есть, ситуация ясна как день: походы на испанцев продолжаются, а мы не получаем с этого НИЧЕГО!\n"+
                          "Совет Ямайки принял решение вновь приступить к раздаче каперских патентов для наших корсаров. Ваше же поручение весьма деликатно и заключается в следующем: мне стало известно, что сразу семь известных корсаров спрятали золото в пещерах по всему региону.\n"+
                          "Мне стали известны примерные места хранения кладов:\n"+
                          "1. Томас Уитсон - близ Картахены\n"+
                          "2. Адриан Сварт - на острове Тринидад\n"+
                          "3. Капитан Гей - на острове Теркс\n"+
                          "4. Капитан Купер - на острове Доминика\n"+
                          "5. Джордж Бренинхем - на острове Кайман\n"+
                          "6. Капитан Гудли - на Кубе\n"+
                          "7. Абрахам Блаувельт - на Гваделупе\n"+
                          "Найдите все эти клады и доставьте их мне. Думаю, лишним будет говорить, что миссия строго секретна, это в ваших интересах...";
            link.l1 = "Понимаю. Полагаю, я смогу решить данный вопрос, сэр.";
            link.l1.go = "Step_7_3";
            link.l2 = "Сэр, мне не очень понятна суть поручения. Клады корсаров - это их личное дело. Я здесь ни при чем...";
            link.l2.go = "Step_7_2";
        break;

        case "Step_7_2":
            dialog.text = "Я весьма озадачен вашим поведением...";
            link.l1 = "Сэр, я сожалею, но ничем не могу помочь.";
            link.l1.go = "exit";
            ChangeCharacterReputation(pchar, 3);
            pchar.questTemp.State = "empty";
            pchar.questTemp.CurQuestNumber = "8";
            SaveCurrentQuestDateParam("questTemp");
            pchar.questTemp.Waiting_time = "35";
            AddCharacterExpToSkill(pchar, "Leadership", -500);
            AddCharacterExpToSkill(pchar, "Cannons", -300);
            AddCharacterExpToSkill(pchar, "Sneak", -350);
			//слухи
			AddSimpleRumour("Вы знаете, капитан " + GetFullName(pchar) + " отказался выполнять поручение Мэдифорда по поиску пиратских заначек...", ENGLAND, 5, 1);
        break;

        case "Step_7_3":
            dialog.text = "По моим оценкам золота во всех семи кладах 2100 слитков! Доставьте мне все ЭТО...";
            link.l1 = "Хорошо, сэр. Приступаю немедленно.";
            link.l1.go = "exit";
            ChangeCharacterReputation(pchar, -4);
            pchar.questTemp.State = "SevenTreasures_GoOn";
            pchar.questTemp.GoldQtyGaveModiford = "0"; // кол-во сданное Мэдифорду
            pchar.questTemp.GoldQtyForSeek = "2100"; // кол-во, которое еще нужно разыскать
            SaveCurrentQuestDateParam("questTemp");
            SetQuestHeader("Eng_Line_7_SevenTresures");
            AddQuestRecord("Eng_Line_7_SevenTresures", "1");
            // ==> клад Томаса Уитсона в пещере близ Картахены.
         	pchar.GenQuestBox.Cartahena_Cave = true;
            pchar.GenQuestBox.Cartahena_Cave.box1.items.jewelry5 = 300;
            pchar.GenQuestBox.Cartahena_Cave.box1.items.statue1 = 1;
            pchar.GenQuestBox.Cartahena_Cave.box1.items.indian17 = 1;
            pchar.GenQuestBox.Cartahena_Cave.box1.items.pistol2 = 1;
            // ==> клад Адриана Сварта на Тринидаде.
         	pchar.GenQuestBox.Trinidad_Grot = true;
            pchar.GenQuestBox.Trinidad_Grot.box1.items.jewelry5 = 450;
            pchar.GenQuestBox.Trinidad_Grot.box1.items.lead1 = 1;
            pchar.GenQuestBox.Trinidad_Grot.box1.items.potion2 = 10;
            pchar.GenQuestBox.Trinidad_Grot.box1.items.potionwine = 3;
            pchar.GenQuestBox.Trinidad_Grot.box1.items.jewelry2 = 20;
            pchar.GenQuestBox.Trinidad_Grot.box1.items.indian6 = 1;
            pchar.GenQuestBox.Trinidad_Grot.box1.items.indian11 = 1;
            pchar.GenQuestBox.Trinidad_Grot.box1.items.indian15 = 1;
            pchar.GenQuestBox.Trinidad_Grot.box1.items.indian12 = 1;
            // ==> клад Капитана Гея в гроте на Терксе.
         	pchar.GenQuestBox.Terks_Grot = true;
            pchar.GenQuestBox.Terks_Grot.box1.items.jewelry5 = 250;
            pchar.GenQuestBox.Terks_Grot.box1.items.blade10 = 1;
            pchar.GenQuestBox.Terks_Grot.box1.items.potion2 = 5;
            pchar.GenQuestBox.Terks_Grot.box1.items.indian18 = 1;
            pchar.GenQuestBox.Terks_Grot.box1.items.jewelry7 = 5;
            // ==> клад Капитана Купера на Домнинике.
         	pchar.GenQuestBox.Dominica_Grot = true;
            pchar.GenQuestBox.Dominica_Grot.box1.items.jewelry5 = 150;
            pchar.GenQuestBox.Dominica_Grot.box1.items.pistol4 = 1;
            pchar.GenQuestBox.Dominica_Grot.box1.items.potion1 = 5;
            pchar.GenQuestBox.Dominica_Grot.box1.items.potionrum = 2;
            // ==> клад Джоржа Бренинхема на Каймане.
         	pchar.GenQuestBox.Caiman_Grot = true;
            pchar.GenQuestBox.Caiman_Grot.box1.items.jewelry5 = 350;
            pchar.GenQuestBox.Caiman_Grot.box1.items.blade2 = 1;
            pchar.GenQuestBox.Caiman_Grot.box1.items.potion5 = 10;
            pchar.GenQuestBox.Caiman_Grot.box1.items.indian18 = 1;
            // ==> клад капитана Гудли на Кубе.
         	pchar.GenQuestBox.Cuba_Grot = true;
            pchar.GenQuestBox.Cuba_Grot.box1.items.jewelry5 = 350;
            pchar.GenQuestBox.Cuba_Grot.box1.items.topor2 = 1;
            pchar.GenQuestBox.Cuba_Grot.box1.items.potion2 = 5;
            pchar.GenQuestBox.Cuba_Grot.box1.items.indian10 = 1;
            // ==> клад Абрахама Блаувельта в катакомбах Гаити рядом с городом.
         	pchar.GenQuestBox.Guadeloupe_Cave = true;
            pchar.GenQuestBox.Guadeloupe_Cave.box1.items.jewelry5 = 250;
            pchar.GenQuestBox.Guadeloupe_Cave.box1.items.jewelry14= 10;
            pchar.GenQuestBox.Guadeloupe_Cave.box1.items.potion3 = 5;
            pchar.GenQuestBox.Guadeloupe_Cave.box1.items.mineral8 = 1;
			pchar.GenQuestBox.Guadeloupe_Cave.box4.items.jewelry5 = 250;
            pchar.GenQuestBox.Guadeloupe_Cave.box4.items.jewelry14= 10;
            pchar.GenQuestBox.Guadeloupe_Cave.box4.items.potion3 = 5;
            pchar.GenQuestBox.Guadeloupe_Cave.box4.items.mineral8 = 1;
        break;

        case "Step_7_4":
            dialog.text = "Та-а-ак, давайте посмотрим...";
            link.l1 = "Итак, сэр, сейчас у меня золота " + pchar.items.jewelry5 + " слитков. Я готов сдать его вам.";
            link.l1.go = "Step_7_5";
            link.l2 = "Сэр, у меня с собой " + pchar.items.jewelry5 + " слитков золота. Но это золото я добыл не в результате поставленной вами задачи. Так что это золото я оставляю себе.";
            link.l2.go = "Step_7_7";
        break;

        case "Step_7_5":
            if ((sti(pchar.questTemp.GoldQtyGaveModiford) + sti(pchar.items.jewelry5)) <= 2100)
            {
                pchar.questTemp.GoldQtyGaveModiford = sti(pchar.questTemp.GoldQtyGaveModiford) + sti(pchar.items.jewelry5);
                pchar.questTemp.GoldQtyForSeek = 2100 - sti(pchar.questTemp.GoldQtyGaveModiford);
                if (pchar.questTemp.GoldQtyForSeek != "0")
                {
                    dialog.text = "Очень хорошо, " + pchar.items.jewelry5 + " слитков золота я принимаю у вас. Итого вы сдали " + pchar.questTemp.GoldQtyGaveModiford + " слитков золота. Соответственно, вам осталось разыскать и доставить мне еще " + pchar.questTemp.GoldQtyForSeek + " слитков.";
                    link.l1 = "Я понял, сэр. Поиски будут продолжены.";
                    link.l1.go = "exit";
                    AddQuestRecord("Eng_Line_7_SevenTresures", "3");
                    AddQuestUserData("Eng_Line_7_SevenTresures", "GoldQtyGaveModiford", pchar.questTemp.GoldQtyGaveModiford);
                    AddQuestUserData("Eng_Line_7_SevenTresures", "GoldQtyForSeek", pchar.questTemp.GoldQtyForSeek);
                }
                else
                {
                    dialog.text = "Вы выполнили поставленную задачу. Вами доставлено и сдано мне, как я и предполагал, 2100 слитков золота. Ваши 10 процентов составляют 70 тысяч.";
                    link.l1 = "Спасибо, сэр.";
                    link.l1.go = "Step_7_6";
                }
                DeleteAttribute(pchar, "items.jewelry5");
            }
            else
            {
                pchar.questTemp.GoldQtyForSeek = sti(pchar.questTemp.GoldQtyGaveModiford) + sti(pchar.items.jewelry5) - 2100);
                dialog.text = "Ого, Вы нашли золота больше, чем я предполагал! У вас сейчас " + pchar.items.jewelry5 + " слитков золота. До этого вы сдали " + pchar.questTemp.GoldQtyGaveModiford + " слитков золота. Соответственно перевыполнение плана на " + pchar.questTemp.GoldQtyForSeek + " слитков.";
                link.l1 = "Сэр, 'перевыполнение плана' - это мое личное золото, к выполненному заданию оно не имеет никакого отношения.";
                link.l1.go = "Step_7_6";
                pchar.items.jewelry5 = pchar.questTemp.GoldQtyForSeek;
            }
        break;

        case "Step_7_6":
            dialog.text = "Ну что же, я очень и очень доволен вами. Наше сотрудничество для меня приятно весьма. Буду всегда рад видеть вас у себя.";
            link.l1 = "Спасибо, сэр. Буду заходить к вам периодически.";
            link.l1.go = "exit";
            AddMoneyToCharacter(pchar, 70000);
            pchar.questTemp.State = "empty";
            SaveCurrentQuestDateParam("questTemp");
            pchar.questTemp.CurQuestNumber = "8";
            pchar.questTemp.Waiting_time = "2";
            AddQuestRecord("Eng_Line_7_SevenTresures", "2");
            CloseQuestHeader("Eng_Line_7_SevenTresures");
            DeleteQuestHeader("Eng_Line_7_Second");
            DeleteAttribute(pchar, "questTemp.GoldQtyGaveModiford");
            DeleteAttribute(pchar, "questTemp.GoldQtyForSeek");
			AddCharacterExpToSkill(pchar, "Repair", 500);
			AddCharacterExpToSkill(pchar, "Commerce", 550);
			AddCharacterExpToSkill(pchar, "Defence", 450);
			AddCharacterExpToSkill(pchar, "Leadership", 400);
			AddTitleNextRate(sti(NPChar.nation), 2);
			ChangeCharacterNationReputation(pchar, sti(NPChar.nation), 1);
			//слухи
			AddSimpleRumour("Ходят неясные слухи, что некий капитан помог Мэдифорду обчистить клады некоторых пиратов. А ведь Мэдифорд сам виноват, что каперы стали прятать награбленное по пещерам и гротам...", ENGLAND, 5, 1);
        break;

        case "Step_7_7":
            dialog.text = RandPhraseSimple("И какого черта тогда вы трясете им у меня перед глазами?", "Впредь прошу вас говорить только о МОЕМ золоте.");
            link.l1 = "Сэр, вы меня спросили - я вам ответил как есть. Прошу меня извинить.";
            link.l1.go = "exit";
        break;
 
        case "Step_7_10":
			dialog.text = "Хм, таким тоном выражать здесь свои мысли я вам не рекомендую. Я разочарован вами и налагаю на вас штраф!\nБолее я вас не задерживаю, вы можете идти.";
            link.l1 = "Сожалею, что доставил вам неудобство, сэр.";
            link.l1.go = "exit";
			DeleteAttribute(pchar, "GenQuestBox.Cartahena_Cave");
			DeleteAttribute(pchar, "GenQuestBox.Trinidad_Grot");
			DeleteAttribute(pchar, "GenQuestBox.Terks_Grot");
			DeleteAttribute(pchar, "GenQuestBox.Dominica_Grot");
			DeleteAttribute(pchar, "GenQuestBox.Caiman_Grot");
			DeleteAttribute(pchar, "GenQuestBox.Cuba_Grot");
			DeleteAttribute(pchar, "GenQuestBox.Guadeloupe_Cave");
            pchar.questTemp.State = "empty";
            SaveCurrentQuestDateParam("questTemp");
            pchar.questTemp.CurQuestNumber = "8";
            pchar.questTemp.Waiting_time = "20";
            AddQuestRecord("Eng_Line_7_SevenTresures", "4");
            CloseQuestHeader("Eng_Line_7_SevenTresures");
            DeleteQuestHeader("Eng_Line_7_Second");
			ChangeCharacterNationReputation(pchar, sti(NPChar.nation), -2);
			ChangeCharacterReputation(pchar, -5);
			AddMoneyToCharacter(pchar, -150000);
        break;

        case "Step_8_1":
            dialog.text = "Именно! Ну что ж, тем лучше.\n"+
                          "У Мэнсфилда приготовлена для нас крупная сумма в золотых слитках. Мне нужно доставить это золото в Порт Рояль. Это дело я могу доверить только вам.";
            link.l1 = "Хорошо, сэр. Я как-то ограничен во времени?";
            link.l1.go = "Step_8_2";
        break;

        case "Step_8_2":
            dialog.text = "Нет, торопиться особо некуда. Но и тянуть не стоит.";
            link.l1 = "Я понял. Как только я буду готов, то сразу выйду в море.";
            link.l1.go = "exit";
            pchar.questTemp.State = "ToMansfield_GoOn";
            SetQuestHeader("Eng_Line_8_ToMansfield");
            AddQuestRecord("Eng_Line_8_ToMansfield", "1");
            Pchar.quest.ToMansfield_IntoTheTown.win_condition.l1 = "location";
            Pchar.quest.ToMansfield_IntoTheTown.win_condition.l1.location = "LaVega_town";
            Pchar.quest.ToMansfield_IntoTheTown.win_condition = "ToMansfield_IntoTheTown";
            SetLocationCapturedState("LaVega_town", true);
			//--> огонь и пламень
			locations[FindLocation("LaVega_town")].hidden_effects = true;
			locations[FindLocation("LaVega_ExitTown")].hidden_effects = true;	
			//<-- огонь и пламень
            Log_QuestInfo("Форт буканьеров пуст и горит.");
        break;

        case "Step_8_3":
            dialog.text = "Черт побери!!! Вам как-нибудь удалось прояснить ситуацию?";
            link.l1 = "Вроде бы испанцы ушли на Сантьяго. Я постараюсь их перехватить.";
            link.l1.go = "Step_8_4";
        break;

        case "Step_8_4":
            dialog.text = "Очень хорошо. Я прошу вас приложить все силы для поиска.";
            link.l1 = "Я понял, сэр.";
            link.l1.go = "exit";
            npchar.TempQuest.KnowAboutModiford = 1;
        break;

        case "Step_8_5":
            dialog.text = "Мне нужно подумать. Зайдите ко мне дней через десять.";
            link.l1 = "Хорошо, сэр.";
            link.l1.go = "exit";
            TakeItemFromCharacter(pchar, "letter_open");
			BackItemDescribe("letter_open");
            DeleteAttribute(npchar, "TempQuest.KnowAboutModiford");
            SaveCurrentQuestDateParam("questTemp");
            pchar.questTemp.State = "empty";
            pchar.questTemp.CurQuestNumber = "9";
            pchar.questTemp.Waiting_time = "10";
            CloseQuestHeader("Eng_Line_8_ToMansfield");
            SetLocationCapturedState("LaVega_town", false);
			//--> огонь и пламень убираем
			DeleteAttribute(&locations[FindLocation("LaVega_town")], "hidden_effects");
			DeleteAttribute(&locations[FindLocation("LaVega_ExitTown")], "hidden_effects");	
			//<-- огонь и пламень
            // ==> Мэнсфилд убит, теперь он будет Алистером Гудом.
            sld = characterFromID("Edward Mansfield");
            LAi_SetStayTypeNoGroup(sld);
            sld.model = "bocman";
        	sld.name 	= "Алистер";
        	sld.lastname = "Гуд";
            SendMessage(sld, "lss", MSG_CHARACTER_SETMODEL, sld.model, sld.model.animation);
            FaceMaker(sld);
            QuestSetCurrentNode("Edward Mansfield", "No_Mansfield_first_time");
            ChangeCharacterAddressGroup(&characters[GetCharacterIndex("Edward Mansfield")], "LaVega_townhall", "sit", "sit1");
			AddCharacterExpToSkill(pchar, "Fortune", 500);
			AddCharacterExpToSkill(pchar, "Sailing", 550);
			AddCharacterExpToSkill(pchar, "FencingLight", 450);
			AddCharacterExpToSkill(pchar, "FencingHeavy", 400);
			AddTitleNextRate(sti(NPChar.nation), 2);
			ChangeCharacterNationReputation(pchar, sti(NPChar.nation), 1);
			//слухи
			AddSimpleRumour("Ходят слухи, что поселение буканьеров на Эспаньоле разорено испанцами...", ENGLAND, 5, 1);
		break;
        
        case "Step_9_1":
            pchar.questTemp.State = "AttackSantiago_GoOn";
            if (!CheckAttribute(npchar, "TempQuest.KindResult")) // письмо не получено, но в курсе нападения испанцев
            {
                dialog.text = "Я предпринял определенные действия и мне удалось совершенно точно установить, кто стоит за нападением испанцев на Ла Вегу.";
                link.l1 = "И кто же это?";
                link.l1.go = "Step_9_2";
            }
            else if (npchar.TempQuest.KindResult == 2) // признак результата завершения квеста, 2 == вообще не в курсе, что произошло
            {
                dialog.text = "Хочу довести до вашего сведения, что бойня в Ла Веге явилась результатом нападения испанцев.";
                link.l1 = "Ясно, сэр.";
                link.l1.go = "Step_9_2";
            }
            else // признак результата завершения квеста, 1 == было захвачено письмо
            {
                dialog.text = "Благодаря найденному вами на испанском галеоне письму, мы теперь четко можем себе представлять, кто стоит за нападением испанцев на Ла Вегу.";
                link.l1 = "Да, сэр. Письмо я прочел.";
                link.l1.go = "Step_9_2";
            }
            Log_QuestInfo("Задание получено.");
            DeleteAttribute(npchar, "TempQuest.KindResult");
			characters[GetCharacterIndex("Santiago_Mayor")].dialog.captureNode = "EL9_SantiagoAttack"; //капитулянтская нода мэра
        break;

        case "Step_9_2":
            dialog.text = "Данное нападение было организовано губернатором Сантьяго Хосе Санчо Хименесом. Для исполнения этой операции он привлек Маноэля Риверо Пардаля, единственного испанца-пирата в этих водах.";
            link.l1 = "Что мне нужно сделать?";
            link.l1.go = "Step_9_3";
        break;

        case "Step_9_3":
            dialog.text = "Вам нужно напасть на Сантьяго и доставить мне губернатора Хосе Санчо Хименеса. Мне нужен Хименес, город - ваш.";
            link.l1 = "Я понял, сэр.";
            link.l1.go = "exit";
            SetQuestHeader("Eng_Line_9_AttackSantiago");
            AddQuestRecord("Eng_Line_9_AttackSantiago", "1");
        break;

        case "Step_9_4":
            dialog.text = "Черт возьми, вы действительно незаменимый человек, " + pchar.name + ". Мэнсфилд отомщен на славу!";
            link.l1 = "Что мне делать с Хименесом?";
            link.l1.go = "Step_9_5";
            QuestSetCurrentNode("Santiago_Mayor", "First time");
        break;

        case "Step_9_5":
            dialog.text = "Сдайте его моему дежурному офицеру. Я завтра займусь им, сегодня для этого у меня слишком хорошее настроение.\n"+
                          "Вам, я полагаю, нужно отдохнуть и привести себя в порядок после столь значительных свершений. Не смею задерживать, заходите ко мне в любое время.\n"+
                          "Да, и еще... Зайдите к Моргану, я думаю, ему будет очень приятно узнать, что месть за Эдварда Мэнсфилда свершилась так скоро и эффективно. Мэнсфилд был для Моргана как отец...";
            link.l1 = "Хорошо, сэр. Обязательно зайду.";
            link.l1.go = "exit";
            QuestSetCurrentNode("Henry Morgan", "Give_blade");
            sld = characterFromID("Himenes");
            RemovePassenger(pchar, sld);
    	    sld.LifeDay = 1; // уберем нпс на след.день.
    	    SaveCurrentNpcQuestDateParam(sld, "LifeTimeCreate");
            pchar.questTemp.State = "empty";
            SaveCurrentQuestDateParam("questTemp");
            pchar.questTemp.CurQuestNumber = "10";
            pchar.questTemp.Waiting_time = "20";
            CloseQuestHeader("Eng_Line_9_AttackSantiago");
			AddCharacterExpToSkill(pchar, "Repair", 550);
			AddCharacterExpToSkill(pchar, "Cannons", 760);
			AddCharacterExpToSkill(pchar, "Defence", 450);
			AddCharacterExpToSkill(pchar, "Pistol", 300);
			AddTitleNextRate(sti(NPChar.nation), 2);
			ChangeCharacterNationReputation(pchar, sti(NPChar.nation), 1);
			//слухи
			AddSimpleRumour("Капитан, все только о том и говорят, что вы привезли в Порт Рояль пленного испанского губернатора! Вы настоящие герой, капитан...", ENGLAND, 5, 1);
        break;
        
        case "Step_10_1":
            dialog.text = "Испанцы решили просто покончить с нами. Собрать такое количество линейных кораблей здесь - это просто невероятно! Но мы справились с ними!";
            link.l1 = "Да, сэр. Хоть это и было в крайней степени сложно, тем не менее, горожане могут вздохнуть спокойно. А испанцы, полагаю, вряд ли оправятся от такого удара...";
            link.l1.go = "Step_10_2";
        break;

        case "Step_10_2":
            dialog.text = "Согласен с вами, друг мой. Полагаю, что суммы в 80000 хватит, чтобы привести ваши корабли в порядок и набрать новую команду. Вы нужны Англии, друг мой, и Англия вас не забудет!\n"+
                          "Простите, но сейчас мне нужно подумать об ответных мерах... Прошу вас заглянуть ко мне через неделю.";
            link.l1 = "Хорошо, я постараюсь быть у вас в назначенный срок, сэр.";
            link.l1.go = "exit";
            ChangeCharacterAddressGroup(&characters[GetCharacterIndex("Henry Morgan")], "PortRoyal_houseS1", "sit", "sit2");
            pchar.questTemp.State = "empty";
            SaveCurrentQuestDateParam("questTemp");
            pchar.questTemp.CurQuestNumber = "11";
            pchar.questTemp.Waiting_time = "7";
            CloseQuestHeader("Eng_Line_10_SpanishAttack");
			AddCharacterExpToSkill(pchar, "Repair", 600);
			AddCharacterExpToSkill(pchar, "Leadership", 550);
			AddCharacterExpToSkill(pchar, "Commerce", 600);
			AddCharacterExpToSkill(pchar, "Accuracy", 400);
			AddTitleNextRate(sti(NPChar.nation), 3);
			ChangeCharacterNationReputation(pchar, sti(NPChar.nation), 1);
			AddMoneyToCharacter(pchar, 80000);
			//слухи
			AddSimpleRumour("Капитан, вы просто спасли нас всех. Жители города как один признательны вам за ваш подвиг.", ENGLAND, 10, 6);
        break;
        
        case "Step_11_1":
            dialog.text = "Ответом на нападение испанцев на Порт Рояль будет наше нападение на испанские города.\n"+
                          "Рядом с Тринидадом на континенте находится два испанских города: Кумана и Каракас. Захватите эти города и приведите население к присяге Его Величеству Карлу II Стюарту.\n"+
                          "Содействуйте назначению новой администрации для нормализации городского управления и скорейшего сбора налогов и пошлин.";
            link.l1 = "Я понял, сэр. Приступаю по мере готовности.";
            link.l1.go = "exit";
            SetQuestHeader("Eng_Line_11_OccupyMein");
            AddQuestRecord("Eng_Line_11_OccupyMein", "1");
            pchar.questTemp.State = "OccupyMein_GoOn";
			characters[GetCharacterIndex("Cumana_Mayor")].dialog.captureNode = "EngLine11Quest_OccupyMein"; //капитулянтская нода мэра
			characters[GetCharacterIndex("Caracas_Mayor")].dialog.captureNode = "EngLine11Quest_OccupyMein"; //капитулянтская нода мэра
        break;

        case "Step_11_2":
            dialog.text = "Превосходно, друг мой. Просто великолепно! Захваченная на острове добыча по праву принадлежит вам!\n"+
                          "Я прошу вас быть у меня в резиденции через месяц. У меня опять будет для вас дело.";
            link.l1 = "Хорошо, сэр.";
            link.l1.go = "exit";
            DeleteAttribute(pchar, "questTemp.Q11_Cumana");
            DeleteAttribute(pchar, "questTemp.Q11_Caracas");
            pchar.questTemp.State = "empty";
            SaveCurrentQuestDateParam("questTemp");
            pchar.questTemp.CurQuestNumber = "12";
            pchar.questTemp.Waiting_time = "30";
            CloseQuestHeader("Eng_Line_11_OccupyMein");
			AddCharacterExpToSkill(pchar, "Repair", 1000);
			AddCharacterExpToSkill(pchar, "Commerce", 1600);
			AddCharacterExpToSkill(pchar, "Accuracy", 1000);
			AddCharacterExpToSkill(pchar, "Defence", 1100);
			AddCharacterExpToSkill(pchar, "Cannons", 1300);
			AddCharacterExpToSkill(pchar, "Sailing", 1200);
			AddTitleNextRate(sti(NPChar.nation), 2);
			ChangeCharacterNationReputation(pchar, sti(NPChar.nation), 1);
			//слухи
			AddSimpleRumour("Капитан, похоже, что скоро вам дадут прозвище " + pchar.name + "-Завоеватель!", ENGLAND, 5, 1);
        break;
        
        case "Step_12_1":
            dialog.text = "Что-то необычное происходит в Сент-Джонсе. Я не получаю положенных уведомлений о состоянии дел от губернатора города.\n"+
                          "Вы являетесь моим доверенным лицом, поэтому я прошу вас отправиться в резиденцию Сент-Джонса и прояснить ситуацию у губернатора.";
            link.l1 = "Хорошо, сэр. Не думаю, что существуют какие-либо проблемы, но я все узнаю.";
            link.l1.go = "exit";
            SetQuestHeader("Eng_Line_12_ColonelLinch");
            AddQuestRecord("Eng_Line_12_ColonelLinch", "1");
            pchar.questTemp.State = "ColonelLinch_GoToKingstown";

            Pchar.quest.ColonelLinch_IntoTheResidence.win_condition.l1 = "location";
            Pchar.quest.ColonelLinch_IntoTheResidence.win_condition.l1.location = "SentJons_townhall";
            Pchar.quest.ColonelLinch_IntoTheResidence.win_condition = "ColonelLinch_IntoTheResidence";
        break;

        case "Step_12_2":
            dialog.text = "Хм... И что?";
            link.l1 = "По приказу английского правительства вы арестованы, сэр...";
            link.l1.go = "Step_12_3";
        break;

        case "Step_12_3":
            dialog.text = "Не так быстро... Офицер, ко мне!!!";
            link.l1 = "Проклятье!!!";
            link.l1.go = "Step_12_4";
        break;

        case "Step_12_4":
            pchar.nation = PIRATE;
            LAi_SetImmortal(npchar, true);
            LAi_SetImmortal(characterFromID("PortRoyal_Mayor"), true);
            bDisableFastReload = true; // закрыть переходы.
            chrDisableReloadToLocation = true; // закрыть выход из локации.
            LAi_LocationFightDisable(&Locations[FindLocation(pchar.location)], false);
            LAi_NoRebirthEnable(npchar);
            LAi_SetActorType(npchar);
            LAi_ActorAfraid(npchar, pchar, false);

            LAi_group_Register("EnemyFight");
           	float locx, locy, locz;
            GetCharacterPos(pchar, &locx, &locy, &locz);
            string SModel;
            for (int i=1; i<=3; i++)
            {
                SModel = "off_eng_"+i;
                if (i == 3) SModel = "sold_eng_3";
                sld = GetCharacter(NPC_GenerateCharacter("EngSolder"+i, SModel, "man", "man", 30, ENGLAND, 0, true));
                FantomMakeCoolFighter(sld, 30, 100, 100, BLADE_LONG, "pistol3", 250);
            	LAi_SetWarriorType(sld);
                LAi_group_MoveCharacter(sld, "EnemyFight");
               	ChangeCharacterAddressGroup(sld, "PortRoyal_townhall", "goto", LAi_FindFarLocator("goto", locx, locy, locz));
            }
            LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
            LAi_group_FightGroups("EnemyFight", LAI_GROUP_PLAYER, true);
            LAi_group_SetCheck("EnemyFight", "ColonelLinch_AfterFight");
            NextDiag.CurrentNode = NextDiag.TempNode;
            DialogExit();
			//слухи
			AddSimpleRumour("Ну и дела у нас происходят!! Губернатор Мэдифорд арестован и помещен в тюрьму одним из своих приближенных каперов! Ну, дела!..", ENGLAND, 5, 1);
        break;

        case "Step_12_5":
            dialog.text = "Не удивляйтесь! Но давайте все по порядку.\n"+
                          "С огромным удовольствием должен сообщить, что ваши заслуги перед отечеством оценены в должной мере.\n"+
                          "Я имею честь сообщить о возведении вас в рыцарское звание. Отныне перед вашим именем всегда должна будет употребляться приставка сэр.\n"+
                          "Также, в результате проведенного расследования деятельности Мэдифорда, найдено его огромное состояние. Часть этих денег принадлежит вам, сэр. Это триста тысяч золотом. Извольте получить.";
            link.l1 = "Благодарю вас. Надеюсь, что и далее я смогу быть так же полезен Его Величеству.";
            link.l1.go = "Step_12_6";
        break;

        case "Step_12_6":
            dialog.text = "Я так же рассчитываю на это.\n"+
                          "Надеюсь, что с вашим участием, сэр, Карибское море станет более английским. И не забывайте, что Испания нам не враг. Пока, во всяком случае...";
            link.l1 = "Я понял, сэр. Приложу все усилия для процветания английских колоний в Карибском море.";
            link.l1.go = "exit";
			pchar.name = "сэр " + pchar.name;
            AddMoneyToCharacter(pchar, 300000);
            AddTitleNextRate(sti(NPChar.nation), 4);
			ChangeCharacterNationReputation(pchar, sti(NPChar.nation), 20);
            SetNationRelationBoth(SPAIN, ENGLAND, RELATION_NEUTRAL);
            DeleteAttribute(pchar, "questTemp.Waiting_time");
            pchar.questTemp.State = "EndOfQuestLine";
			bWorldAlivePause   = false; // Конец линейки
        break;
        
        
	}
	UnloadSegment(NPChar.FileDialog2);  // если где-то выход внутри switch  по return не забыть сделать анлод
}
