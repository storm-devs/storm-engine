
void ProcessDialogEvent()
{
	ref NPChar;
	aref Link, NextDiag;

	DeleteAttribute(&Dialog,"Links");

	makeref(NPChar,CharacterRef);
	makearef(Link, Dialog.Links);
	makearef(NextDiag, NPChar.Dialog);

	string NodeName = Dialog.CurrentNode;
	string NodePrevName = "";
	if (CheckAttribute(NextDiag, "PrevNode")) NodePrevName = NextDiag.PrevNode;
	
	switch(Dialog.CurrentNode)
	{
		case "First time":
			NextDiag.TempNode = "First time";
			if (LAi_grp_playeralarm > 0)
			{
       			dialog.text = "Я не желаю с тобой разговаривать! Ты напал на моих сограждан!";
				link.l1 = "Как знаешь..."
				link.l1.go = "exit";
				break;
			}

			if(NPChar.quest.meeting == "0")
			{
				NPChar.quest.meeting = "1";
				if (CheckAttribute(loadedLocation, "storm"))
				{
					dialog.text = "Это твой корабль разбился последним? Ну что же, добро пожаловать! Я здесь содержу таверну, зовут меня " + GetFullName(npchar) + ". Тебе здорово повезло, приятель!!";
					link.l1 = "Да уж, повезло просто фатально!.. Меня зовут " + GetFullName(pchar) + ". Я был капитаном...";
					link.l1.go = "FS_1";
				}
				else				
				{
					dialog.Text = "А-а-а, так это твой корабль разбился последним? Ну что же, добро пожаловать! Я здесь содержу таверну, зовут меня " + GetFullName(npchar) + ". Так что заходи, ежели что...";
					Link.l1 = "Хорошо, " + npchar.name + ". Приятно познакомиться, так сказать... А меня зовут " + GetFullName(pchar) + ".";
					Link.l1.go = "First time";
				}
			}
			else
			{
				//базар в винном погребе
				if (pchar.questTemp.LSC == "toWineCellarGone" && npchar.location.locator == "goto10")
				{
					dialog.Text = "А-а, вот и ты!";
					link.l4 = "Именно! Теперь хотелось бы послушать тебя.";
					link.l4.go = "TizerCellar";
					PChar.quest.LSC_returnBarmen.over = "yes"; //снимаем таймер на опоздание, разговор состоялся
					break;
				}			
				//==> штатый диалог
				dialog.Text = LinkRandPhrase("Эй, " + GetAddress_Form(NPChar) + " " + PChar.name + "! " + TimeGreeting() + ".",
                                    "О-о-о, рад видеть нового гражданина нашего города...",
                                    "Рад тебя видеть у себя в таверне. Зачем пожаловал?");
				Link.l1 = "Я надеюсь, ты сможешь ответить на пару вопросов.";
				Link.l1.go = "int_quests";
				link.l2 = "Мне нужно переночевать.";
				link.l2.go = "room";
				Link.l3 = "Увы, я уже ухожу, " + NPChar.name + ". До встречи.";
				Link.l3.go = "exit";
				//==>> квесты
				if (pchar.questTemp.LSC == "AfterAdmiral" && GetQuestPastDayParam("questTemp.LSC") > 3)
				{
					link.l4 = "Дружище, мы договаривались с тобой, что ты пораскинешь мозгами по поводу Тизера Дэна...";
					link.l4.go = "Tizer";
				}
				if (pchar.questTemp.LSC == "toWineCellarLate")
				{
					link.l4 = "Послушай, я так и не нашел тебя в этом твоем винном погребе...";
					link.l4.go = "TizerCellarLate";
				}
				if (pchar.questTemp.LSC == "toDiffindoor" && CheckCharacterItem(pchar, "letter_LSC"))
				{
					link.l4 = "Я обследовал 'Диффиндур' и нашел там сундук Тизера. В нем, кроме прочего, было еще и очень интересное письмо...";
					link.l4.go = "TizerCellarOk_1";
				}
				if (pchar.questTemp.LSC == "afterFightInProtector")
				{
					link.l4 = "Послушай, тут такое дело...";
					link.l4.go = "AfterProtector_1";
				}
				if (pchar.questTemp.LSC == "toAdmNarvalRing" && CheckCharacterItem(pchar, "DOjeronRing"))
				{
					link.l4 = "Я сделал как ты сказал. Кольцо 'нарвалов' у меня.";
					link.l4.go = "NarvalRing_1";
				}
			}
		break;

		case "FS_1":
			dialog.text = "Ты счастливчик, капитан, просто пока этого не понимаешь. Если бы ты подошел к Городу часов пять назад - ты бы ни за что не сумел выбраться из воды.";
			link.l1 = "Я моряк, хорошо плаваю...";
			link.l1.go = "FS_2";
		break;
		case "FS_2":
			dialog.text = "Это ничего не значит, когда речь идет водах вокруг Города! Просто шторм пошел на убыль, и именно это дало тебе шанс.";
			link.l1 = "Понятно...";
			link.l1.go = "exit";
		break;

		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
		//крутим время
		case "room":
   			if (chrDisableReloadToLocation) //кто-то должен подойти к ГГ.
			{
				dialog.text = "Я думаю, что тебе не стоит сейчас спать. Поверь мне...";
				link.l1 = "Ладно.";
				link.l1.go = "exit";
				break;
			}
			dialog.text = "Ты можешь выспаться на кровати, что находится на верхнем ярусе. Когда тебя разбудить?";
			if(!isDay())
			{
				link.l1 = "Утром.";
				link.l1.go = "hall_day_wait";
			}
			else
			{
				link.l1 = "К ночи.";
				link.l1.go = "hall_night_wait";
				link.l2 = "Следующим утром.";
				link.l2.go = "hall_day_wait";
			}
			link.l3 = "Передумал. Обойдусь без сна.";
			link.l3.go = "exit";
		break;
		case "hall_night_wait":
			DialogExit();
			NextDiag.CurrentNode = NextDiag.TempNode;
			TavernWaitDate_LSC("wait_night");
		break;
		case "hall_day_wait":
			DialogExit();
			NextDiag.CurrentNode = NextDiag.TempNode;
			TavernWaitDate_LSC("wait_day");
		break;
		//вопросы
		case "int_quests":
			dialog.text = "Внимательно тебя слушаю, дружище.";
			if (!CheckAttribute(NextDiag, NodeName+".l5"))
			{
				link.l5 = "Погодка у вас, конечно, просто потрясающая!!";
				link.l5.go = "ansewer_5";
			}
			if (!CheckAttribute(NextDiag, NodeName+".l1"))
			{
				link.l1 = "Послушай, что это за остров?";
				link.l1.go = "ansewer_1";
			}
			if (!CheckAttribute(NextDiag, NodeName+".l2"))
			{
				link.l2 = "Как получилось, что я попал сюда?";
				link.l2.go = "ansewer_2";
			}
			if (!CheckAttribute(NextDiag, NodeName+".l3"))
			{
				link.l3 = "И что, отсюда действительно невозможно выбраться? Сколько кораблей вокруг, неужели нельзя сколотить один целый?";
				link.l3.go = "ansewer_3";
			}
			if (!CheckAttribute(NextDiag, NodeName+".l4"))
			{
				link.l4 = "Как вы здесь живете, черт возьми?..";
				link.l4.go = "ansewer_4";
			}
			if (!CheckAttribute(NextDiag, NodeName+".l6"))
			{
				link.l6 = "Послушай, ты знал такого человека - Тизера Дэна?";
				link.l6.go = "ansewer_6";
			}
			if (!CheckAttribute(NextDiag, NodeName+".l7") && pchar.questTemp.LSC == "AdmiralIsWaiting")
			{
				link.l7 = "Послушай, ты не знаешь, что такое Диффиндур?";
				link.l7.go = "ansewer_7";
			}
			if (!CheckAttribute(NextDiag, NodeName+".l8"))
			{
				link.l8 = "Что за кланы живут в Городе?";
				link.l8.go = "ansewer_8";
			}
			link.l10 = "Нет вопросов. Извини, приятель...";
			link.l10.go = "exit";
		break;
		case "ansewer_5":
			dialog.text = "Это еще ничего! Бывало и похуже. Штормы здесь - обычное дело.";
			link.l1 = "Понятно...";
			link.l1.go = "ansewer_5_1";
			NextDiag.(NodePrevName).l5 = true;
		break;
		case "ansewer_5_1":
			dialog.text = "Этот шторм закончится завтра ближе к вечеру. У нас положено по прибытии отмечаться у адмирала, так что если до окончания шторма ты не явишься к нему, то он найдет тебя сам.";
			link.l1 = "Адмирал?";
			link.l1.go = "ansewer_5_2";
		break;
		case "ansewer_5_2":
			dialog.text = "Чад Каппер, приятель. Не шути с этим делом, мой тебе совет!";
			link.l1 = "Ясно.";
			link.l1.go = "exit";
		break;
		case "ansewer_1":
			dialog.text = "Это Город Потерянных Кораблей, приятель. Если попал сюда, то забудь обо всем, чем жил раньше. Отсюда не вырвешься, к сожалению...";
			link.l1 = "Дьявол!!";
			link.l1.go = "exit";
			NextDiag.(NodePrevName).l1 = true;
		break;
		case "ansewer_2":
			dialog.text = "Не знаю, это тебя надо спросить, как ты сюда заплыл. Ты же капитан, своим кораблем сам и управлял. Это я ничего не знаю, сидел себе в каюте, вдруг крики, потом треск обшивки... Вот и все.";
			link.l1 = "И ничего не видел?";
			link.l1.go = "ansewer_2_1";
			NextDiag.(NodePrevName).l2 = true;
		break;
		case "ansewer_2_1":
			dialog.text = "Потом уже увидел, конечно, этот морской пейзажец, так сказать. Но как наш кэп умудрился сюда прийти - поняти не имею. Погиб он. Вообще-то, мало кто спасается после крушения, так что тебе повезло.";
			link.l1 = "Да уж...";
			link.l1.go = "exit";
		break;
		case "ansewer_3":
			dialog.text = "Ха!! И откуда ты такой умный взялся?.. Были попытки, но все неудачные. Получается, что люди берут материалы и попросту портят их. От этого Город серьезно страдает, так как это понижает его устойчивость. Если высвободить какой-нибудь корабль, то остров начинает двигаться, все переходы ломаются, каюты встают по другому\nВ общем, это боком выходит. Поэтому сейчас это запрещено нашим Законом. И уже давно!";
			link.l1 = "Понятно...";
			link.l1.go = "exit";
			NextDiag.(NodePrevName).l3 = true;
		break;
		case "ansewer_4":
			dialog.text = "Да вполне нормально. Хотя, знаешь, надоело все до смерти... А вообще, Город живет на основании Закона. О его положения тебе лучше узнать у адмирала Каппера.";
			link.l1 = "Это что за адмирал?";
			link.l1.go = "ansewer_4_1";
			NextDiag.(NodePrevName).l4 = true;
		break;
		case "ansewer_4_1":
			dialog.text = "Это, приятель, так называемый глава местного самоуправления, Чад Каппер его зовут. Говорят, что раньше он был пиратом, так что задирать его не стоит без дела.";
			link.l1 = "Никто и не собирается... А что, у вас действительно проходят выборы, и граждане выбирают адмирала?";
			link.l1.go = "ansewer_4_2";
		break;
		case "ansewer_4_2":
			dialog.text = "Ну, когда-то выборы были, но сейчас это уже не актуально. Чад хорошо справляется со своими обязанностями, никто не хочет ничего менять. Уже никто не хочет...";
			link.l1 = "Что значит 'уже'?";
			link.l1.go = "ansewer_4_3";
		break;
		case "ansewer_4_3":
			dialog.text = "До 'уже' были граждане, которые хотели стать адмиралом. Но теперь их нет с нами.";
			link.l1 = "Уплыли отсюда, что ли?";
			link.l1.go = "ansewer_4_4";
		break;
		case "ansewer_4_4":
			dialog.text = "Ну, можно и так сказать. На дно уплыли...";
			link.l1 = "Понятно.";
			link.l1.go = "exit";
		break;
		case "ansewer_6":
			dialog.text = "Знал. Он жил у нас несколько месяцев, потом погиб. А жаль, хороший был мужик.";
			link.l1 = "А вот и нет!";
			link.l1.go = "ansewer_6_1";
			NextDiag.(NodePrevName).l6 = true;
		break;
		case "ansewer_6_1":
			dialog.text = "Что, нехороший был мужик?";
			link.l1 = "Да я не об этом! Похоже, что он каким-то образом добрался до Большой земли...";
			link.l1.go = "ansewer_6_2";
		break;
		case "ansewer_6_2":
			dialog.text = "Дьявол!!! Не может быть!..";
			link.l1 = "Может. Правда, это, в итоге, не принесло ему счастья, он стал нищим...";
			link.l1.go = "ansewer_6_3";
		break;
		case "ansewer_6_3":
			dialog.text = "Черт! Черт! Черт!! Неужели это возможно?..";
			link.l1 = "Ты это о чем?";
			link.l1.go = "ansewer_6_4";
		break;
		case "ansewer_6_4":
			dialog.text = "Знаешь что, приятель, вот что я тебе скажу: никому не болтай об этом! Если адмирал узнает, что кто-то нарушил Закон, да еще в этом и преуспел... В общем, этому вестнику не поздоровится.";
			link.l1 = "Хм, не понимаю...";
			link.l1.go = "ansewer_6_5";
		break;
		case "ansewer_6_5":
			dialog.text = "Поймешь потом. А пока, ради твоего же блага, делай так, как я говорю. Не упоминай имя Тизера! Понял?";
			link.l1 = "Понял.";
			link.l1.go = "ansewer_6_6";
		break;
		case "ansewer_6_6":
			dialog.text = "Хорошо. А я пока пораскину мозгами об этой твоей новости...";
			link.l1 = "Ну ладно, пораскинь.";
			link.l1.go = "exit";
			AddQuestRecord("ISS_MainLine", "4");
			SaveCurrentQuestDateParam("questTemp.LSC"); //запомним дату базара
		break;
		case "ansewer_7":
			dialog.text = "Это флейт 'Фернандо Диффиндур', гиблое место.";
			link.l1 = "Будь добр, поясни...";
			link.l1.go = "ansewer_7_1";
			NextDiag.(NodePrevName).l7 = true;
		break;
		case "ansewer_7_1":
			dialog.text = "Флейт этот по идее должен был пойти ко дну - он буквально расколот пополам. Однако, как-то держиться на поверхости, зацепился, видать, за рифы. В общем, корабль этот особняком стоит, никто там не живет и жить не будет.";
			link.l1 = "Почему?";
			link.l1.go = "ansewer_7_2";
		break;
		case "ansewer_7_2":
			dialog.text = "Такие корабли, когда начинают уходить по воду, погружаются за несколько секунд. Так что надо быть сумасшедшим, чтобы там проводить время.";
			link.l1 = "Понятно...";
			link.l1.go = "exit";
		break;
		case "ansewer_8":
			dialog.text = "Есть такие у нас в Городе. Знаешь, всегда среди находятся есть такие, кто жить не может спокойно. Вот это они и есть - кланы...";
			link.l1 = "Хм, как понять?";
			link.l1.go = "ansewer_8_1";
			NextDiag.(NodePrevName).l8 = true;
		break;
		case "ansewer_8_1":
			dialog.text = "Ну, в эти кланы собираются каторжники, пираты... В общем, криминальные элементы, отбросы общества.";
			link.l1 = "Ты что это, и меня к отбосам относишь? Я ведь пират!";
			link.l1.go = "ansewer_8_2";
		break;
		case "ansewer_8_2":
			dialog.text = "Нет, ты необычный пират, ты - пиратский капитан. Вот, у тебя самого в команде мало было негодяев всех мастей?";
			link.l1 = "Ну, хватало, честно говоря.";
			link.l1.go = "ansewer_8_3";
		break;
		case "ansewer_8_3":
			dialog.text = "Вот это я и имею ввиду... В общем, отдали им два корабля, чтобы они у себя там обретались и остальным гражданам не мешали. Адмирал со своей милицией их держит в узде, иначе здесь был бы полный кошмар.";
			link.l1 = "Ну и как, конфликты бывают с ними?";
			link.l1.go = "ansewer_8_4";
		break;
		case "ansewer_8_4":
			dialog.text = "Были, конечно, но сейчас все тихо. Но имей ввиду, что тебе внутрь их кораблей заходить не стоит!";
			link.l1 = "Понятно. Ну что же, спасибо за информацию.";
			link.l1.go = "exit";
		break;
		// ------------------ квесты ------------------
		//бармен забивает стрелу в винном погребе
		case "Tizer":
			dialog.text = "Да-да, я пораскинул. Только говорить об этом здесь не могу. Давай встретимся у меня в винном погребе после полуночи.";
			link.l1 = "А где находится этот твой винный погреб?";
			link.l1.go = "Tizer_1";
		break;
		case "Tizer_1":
			dialog.text = "Ну, погребом я называю его по привычке, а вообще это одна из кают здесь. Нужно выйти на основную палубу 'Флерона' и с нее зайти в квартердек.";
			link.l1 = "Понятно. Ну что же, договорились.";
			link.l1.go = "Tizer_2";
		break;
		case "Tizer_2":
			dialog.text = "Не задерживайся, долго тебя ждать я не смогу.";
			link.l1 = "Понятно. Буду обязательно.";
			link.l1.go = "exit";
			pchar.questTemp.LSC = "toWineCellar";
			AddQuestRecord("ISS_MainLine", "5");
		break;
		//бармен ругается, что ГГ не пришел в погреб
		case "TizerCellarLate":
			dialog.text = "Это мне известно, ведь я ждал тебя там битый час! Черт возьми, нельзя же быть таким легкомысленным в таком серьезном деле!";
			link.l1 = "В каком-таком серьезном?";
			link.l1.go = "TizerCellarLate_1";
		break;
		case "TizerCellarLate_1":
			dialog.text = "Ище раз говорю, что я не могу об этом деле болтать с тобой прилюдно! Это безумие!";
			link.l1 = "Ладно, не кипятись. Давай еще раз попробуем встретиться, раз это так необходимо.";
			link.l1.go = "TizerCellarLate_2";
		break;
		case "TizerCellarLate_2":
			dialog.text = "Ну хорошо. Жду тебя там же, в погребе, сегодня после полуночи. Но на сей раз изволь явиться.";
			link.l1 = "Хорошо, буду обязательно.";
			link.l1.go = "exit";
			pchar.questTemp.LSC = "toWineCellar";
		break;
		//базар с барменом в погребе
		case "TizerCellar":
			dialog.text = "Ты никому не говорил о Тизере Дэне за это время?";
			link.l1 = "Нет, как мы и договаривались.";
			link.l1.go = "TizerCellar_1";
			LAi_CharacterDisableDialog(npchar);
			pchar.quest.LSC_returnBarmen_2.win_condition.l1 = "ExitFromLocation";
			pchar.quest.LSC_returnBarmen_2.win_condition.l1.location = pchar.location;
			pchar.quest.LSC_returnBarmen_2.function = "LSC_returnBarmen_2";
		break;
		case "TizerCellar_1":
			dialog.text = "Это хорошо. Итак, давай я изложу тебе результаты своих размышлений по этому поводу.";
			link.l1 = "С удовольствием выслушаю.";
			link.l1.go = "TizerCellar_2";
		break;
		case "TizerCellar_2":
			dialog.text = "Итак, ты, наверное, уже в курсе того, что Закон Города запрещает строить что-либо, на чем можно отсюда уплыть. Так вот, я просто ума не приложу, как это Тизер смог построить втихаря некое корыто, да еще и успешно на нем добраться до Большой земли.";
			link.l1 = "К сожалению, я лично с Тизером мне увидеться не довелось, иначе, без сомнения, я узнал бы все подробности этого дела.";
			link.l1.go = "TizerCellar_3";
		break;
		case "TizerCellar_3":
			dialog.text = "Да, жаль... Ну, давай смотреть на то, что имееем. А имеем мы загадку, которую нужно разрешить как можно быстрей. Прежде, чем что-то делать, необходимо в деталях понимать, как Тизер смог выбраться отсюда.";
			link.l1 = "А дальше что?";
			link.l1.go = "TizerCellar_4";
		break;
		case "TizerCellar_4":
			dialog.text = "Если граждане узнают, что Тизер добрался до Большой земли, то, без сомнения, многие захотят последовать его примеру. Только прежде, чем что-то говорить о Тизере Дэне, нужно иметь доказательства твоих утверждений.";
			link.l1 = "Но ведь я...";
			link.l1.go = "TizerCellar_5";
		break;
		case "TizerCellar_5":
			dialog.text = "Одного твоего слова мало, приятель\nЯ вот что думаю. Ты должен отправиться на флейт 'Фернандо Диффиндур'. Место это опасное чрезвыйчайно - кораль треснул пополам и непонятно как держиться на поверхности. Однако именно там Тизер Дэн проводил очень много времени. Полагаю, что именно там мы сможем найти ключ к тайне Тизера.";
			link.l1 = "Как туда добраться?";
			link.l1.go = "TizerCellar_6";
		break;
		case "TizerCellar_6":
			dialog.text = "Вплавь, флейт не примыкает к Городу непосредственно. Ближайшими к 'Диффиндуру' кораблями являются флейт 'Церес Смити' и барк 'Сан Габриэль'. Там же, на связке между двумя этими парусниками, ты сможешь спрыгнуть в воду.\nИ будь осторожен на 'Сан Габриэле', не заходи внутрь. Этот барк - собственность клана 'Каспер'!";
			link.l1 = "Ясно. Послушай, ты можешь хоть примерно назвать, что мы ищем на 'Диффиндуре'?";
			link.l1.go = "TizerCellar_7";
		break;
		case "TizerCellar_7":
			dialog.text = "Если бы я знал! Напомню тебе, что мы ищем доказательства того, что несчастный Тизер Дэн сумел добраться до Большой земли.";
			if (CheckCharacterItem(pchar, "letter_LSC"))
			{
				link.l1 = "Хм, ты знаешь, на 'Диффиндуре' я уже побывал. И нашел там сундук Тизера. В нем, кроме прочего, было еще и очень интересное письмо...";
				link.l1.go = "TizerCellarOk_1";
			}
			else
			{
				link.l1 = "Ты знаешь, у меня есть ключ, который друг Тизера отдал мне на Большой земле. Этот ключ Тизер Дэн наказал вручить тому, кто отправится на поиски Острова Справедливости, то есть Города Потерянных Кораблей.";
				link.l1.go = "TizerCellar_8";
			}
		break;
		case "TizerCellar_8":
			dialog.text = "Интересно, очень интересно... Однако, это, опять же, ничего не доказывает. Каппер выслушает тебя, а затем убьет на глазах остальных граждан как возмутителя спокойствия. И никто ему и слова скажет. Ключ? Да ключей в Городе полно, практически каждый сундук закрыт на ключ.";
			link.l1 = "Да, тоже верно.";
			link.l1.go = "TizerCellar_9";
		break;
		case "TizerCellar_9":
			dialog.text = "Однако, ключ - уже кое что. А приятель Тизера говорил про него?";
			link.l1 = "Что это ключ 'Диффиндур'.";
			link.l1.go = "TizerCellar_10";
		break;
		case "TizerCellar_10":
			dialog.text = "Точно! Мы на верном пути!\nОтправляйся к 'Диффиндуру'. Полагаю, что там ты найдешь предмет, к которому данный ключ и подойдет. Тизер, хоть и казался мне человеком с причудами, тем не менее был далеко не дурак, судя по всему...";
			link.l1 = "М-да, может быть. Но то, что он был человеком хорошим - это факт.";
			link.l1.go = "TizerCellar_11";
		break;
		case "TizerCellar_11":
			dialog.text = "Согласен! Ну, не будем терять времени. Я жду тебя с результатами обследования флейта.";
			link.l1 = "Скоро буду, дружище. Ожидай.";
			link.l1.go = "exit";
			AddQuestRecord("ISS_MainLine", "6");
			pchar.questTemp.LSC = "toDiffindoor"; //флаг линейки ГПК
		break;

		case "TizerCellarOk_1":
			dialog.text = "Что это за письмо?";
			link.l1 = "Прочти сам. Вот, держи.";
			link.l1.go = "TizerCellarOk_2";
			TakeItemFromCharacter(pchar, "letter_LSC");
		break;
		case "TizerCellarOk_2":
			dialog.text = "Так-так, очень интересно... Он пишет: '...так как мы долго и очень серьезно готовились.' Значит, он был не один...";
			link.l1 = "Да, судя по всему, ты прав.";
			link.l1.go = "TizerCellarOk_3";
		break;
		case "TizerCellarOk_3":
			dialog.text = "Хе-хе, без сомнения! Я неплохо знал Тизера, он ни за что на свете не сумел бы построить и дырявого плота - руки у него не из того места росли. Но раз он сумел-таки добраться до Большой земли, то значит его помощник, как пить дать, является специалистом корабельном деле\nХа! Да ведь это наш плотник, Андре Лабор!! Вот что, дружище, отправляйся к нему. Поговори с ним тихо и мирно, расскажи о Тизере Дэне. А то ведь он и не знает, что этот бродяга все-таки добрался до Большой земли!";
			link.l1 = "А это необходимо? Ведь письмо Тизера уже является доказательством моих слов.";
			link.l1.go = "TizerCellarOk_4";
		break;
		case "TizerCellarOk_4":
			dialog.text = "Уже лучше, но, тем не менее, совершенно непонятно, как Тизер умудрился провернуть такое дело. Я не думаю, что нам стоит требовать отмены положения Закона, не имея конкретного предложния взамен.";
			link.l1 = "Почему? Как отменим запрет на постройку кораблей, так сразу и займемся этим. Там и решим, что да как строить.";
			link.l1.go = "TizerCellarOk_5";
		break;
		case "TizerCellarOk_5":
			dialog.text = "Не все так просто, дружище. Мы должны дать людям надежду и цель, а не право ломать Город кто как пожелает. С отменой положения Закона мы сразу же должны принять новый, о постройке ОДНОГО корабля для ВСЕХ!";
			link.l1 = "Хм, пожалуй, ты прав...";
			link.l1.go = "TizerCellarOk_6";
		break;
		case "TizerCellarOk_6":
			dialog.text = "И еще нужно не забывать, что адмирал вряд ли будет стоять, слушать тебя и репу чесать в растерянности. Требовать отмены положения Закона, который внесен Коппером и ревностно им исполняется, - большой риск. Мы должны убедить адмирала в своей правоте, а сделать это будет непросто, как мне кажется.";
			link.l1 = "Почему непросто?";
			link.l1.go = "TizerCellarOk_7";
		break;
		case "TizerCellarOk_7":
			dialog.text = "Хм, как тебе сказать... Иногда я ловлю себя на мысли, что адмирал не очень-то желает убраться отсюда.";
			link.l1 = "Хм, интересно...";
			link.l1.go = "TizerCellarOk_8";
		break;
		case "TizerCellarOk_8":
			dialog.text = "В общем, все это мои домыслы, к ним мы вернемся позднее. А сейчас найди Андрэ Лабора и аккуратно выясни все о его участии в деле Тизера.";
			link.l1 = "Хорошо, я так и сделаю.";
			link.l1.go = "exit";
			pchar.questTemp.LSC = "toAndreLabor"; //флаг линейки ГПК
			AddQuestRecord("ISS_MainLine", "7");
			pchar.questTemp.LSC.checkBoxes = true; //шмалять по открытым сундукам ежесуточно
		break;

		case "AfterProtector_1":
			dialog.text = "Что случилось?";
			link.l1 = "Хм... Ну, в общем, поговорил я с плотником, этим Андре Лабором, и он попросил меня пройти в каюту корвета 'Протектор'. Я туда зашел, ну, и жду его, ничего такого не думаю даже... А этот Андрэ привел миллиционера на разговор со мной! Пришлось всех отправить на тот свет.";
			link.l1.go = "AfterProtector_2";
		break;
		case "AfterProtector_2":
			dialog.text = "Дьявол!! Это надо же! Ну, теперь тебе конец... Слушай, будь так добр, не рассказывай, что это я тебя надоумил. Тебя уже все равно убьют, а я еще пожить могу...";
			link.l1 = "Веди себя как мужчина! Никто меня не убьет, ведь никто и не знает, что это сделал именно я. Они не успели рассказать обо мне адмиралу.";
			link.l1.go = "AfterProtector_3";
		break;
		case "AfterProtector_3":
			dialog.text = "Ты в этом уверен?";
			link.l1 = "Они сам так сказали.";
			link.l1.go = "AfterProtector_4";
		break;
		case "AfterProtector_4":
			dialog.text = "Уже лучше, хотя успокаиваться смысла не вижу. Адмирал далеко не дурак, он тебя вычислит.";
			link.l1 = "И что ты предлагаешь?";
			link.l1.go = "AfterProtector_5";
		break;
		case "AfterProtector_5":
			dialog.text = "Хм, думаю, что надо все списать на один из кланов.";
			link.l1 = "На какой?";
			link.l1.go = "AfterProtector_6";
		break;
		case "AfterProtector_6":
			dialog.text = "На клан 'Нарвал', они послабее будут.";
			link.l1 = "А как это сделать? Я же не могу прийти к Чаду и сказать: 'Это не я, это все 'нарвалы'!'.";
			link.l1.go = "AfterProtector_7";
		break;
		case "AfterProtector_7":
			dialog.text = "У всех членов клана 'Нарвал' есть кольца с выгравированным изображением нарвала. Достань кольцо и иди с ним к адмиралу. Расскажешь ему о том, что нашел его на месте убийства милиционера и плотника.";
			link.l1 = "Ну и как мне найти это кольцо?";
			link.l1.go = "AfterProtector_8";
		break;
		case "AfterProtector_8":
			dialog.text = "В носовой части галеона 'Веласко' есть каюта, в которой 'нарвалы' хранят всякую ерунду. Это у них что-то вроде подсобки. Жди кого-нибудь из них там, а когда 'нарвал' появится – убей его.";
			link.l1 = "Понятно. Ну что же, так и поступлю.";
			link.l1.go = "exit";
			AddQuestRecord("ISS_MainLine", "10");
			pchar.questTemp.LSC = "toKillOneNarval";
			pchar.quest.LSC_KillOneNarval.win_condition.l1 = "locator";
			pchar.quest.LSC_KillOneNarval.win_condition.l1.location = "ProtectorFisher";
			pchar.quest.LSC_KillOneNarval.win_condition.l1.locator_group = "reload";
			pchar.quest.LSC_KillOneNarval.win_condition.l1.locator = "reload1";
			pchar.quest.LSC_KillOneNarval.function = "LSC_KillOneNarval";
		break;

		case "NarvalRing_1":
			dialog.text = NPCStringReactionRepeat("Отлично! Теперь дуй к адмиралу и расскажи, что нашел его в каюте 'Протектора'.", 
				"Я это понял. Не мне, а адмиралу его надо показывать.", 
				"Слушай, это уже не смешно. Дуй к адмиралу!",
                "Ну ты тупой, однако...", "block", 0, npchar, Dialog.CurrentNode);
			link.l1 = HeroStringReactionRepeat("Понял.", 
				"Да, конечно...",
                "Ага, обязательно!", 
				"Да, таким уродился. Все чего-то делаю одно и тоже. На что рассчитываю - непонятно...", npchar, Dialog.CurrentNode);
			link.l1.go = DialogGoNodeRepeat("exit", "", "", "", npchar, Dialog.CurrentNode);
		break;
	}
	NextDiag.PrevNode = NodeName;
}