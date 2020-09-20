
void ProcessDialogEvent()
{
	ref NPChar, sld;
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
			if (npchar.quest.meeting == "0")
			{
				npchar.quest.meeting = "1";
				if (CheckAttribute(loadedLocation, "storm"))
				{
					dialog.text = "Приветствую нового гражданина Города! Очень рады тебя видеть. Извини, забыл представиться. Меня зовут " + GetFullName(npchar) + ", я здесь живу.";
					link.l1 = "Это я понял. Ну а я - " + GetFullName(pchar) + ". Слушай, вы что, в постоянных штормах живете?";
					link.l1.go = "FS_1";
				}
				else
				{
					dialog.text = "Приветствую нового гражданина Города! Очень рады тебя видеть. Извини, забыл представиться. Меня зовут " + GetFullName(npchar) + ", я здесь живу.";
					link.l1 = "Это я понял. Ну а я - " + GetFullName(pchar) + ".";
					link.l1.go = "FT_1";
				}
			}
			else
			{
				dialog.text = "Ты что-то хотел?";
				link.l1 = "Слушай, я ищу одного человека. Не знаешь, где он сейчас?";
				link.l1.go = "SeekCitizen";
				link.l2 = "Хочу задать тебе вопрос.";
				link.l2.go = "int_quests";
				link.l10 = "Ничего...";
				link.l10.go = "exit";
			}
		break;
		case "FS_1":
			dialog.text = "Да нет, что ты, завтра к вечеру уже все устаканится. Кстати, если ты еще не был у адмирала, то советую поторопиться. Он находится на варшипе 'Сан Августин'. До конца шторма тебе надо к нему попасть.";
			link.l1 = "Хорошо, я понял.";
			link.l1.go = "exit";
		break;
		case "FT_1":
			dialog.text = "Если ты еще не был у адмирала, то советую поторопиться. Он находится на варшипе 'Сан Августин'.";
			link.l1 = "Хорошо, я понял.";
			link.l1.go = "exit";
		break;

		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
		//ищем человека
		case "SeekCitizen":
			dialog.text = "А кого ты ищешь?";
			Link.l1.edit = 3;
			Link.l1 = "";
			Link.l1.go = "SeekCitizen_Choice_1";
		break;
		case "SeekCitizen_Choice_1":
			sld = CheckLSCCitizen();
			if (sld.id == "none")
			{
				dialog.text = "Я не понимаю, о ком ты говоришь. Мне нужно знать имя и фамилию этого человека.";
				Link.l1 = "Понятно. Давай я еще попробую назвать.";
				Link.l1.go = "SeekCitizen_Choice_2";				
				Link.l2 = "Спасибо, я лучше сам поищу.";
				Link.l2.go = "exit";	
			}
			else
			{
				if (sld.id == npchar.id)
				{
					dialog.text = "Так это же я!";
					link.l1 = "Надо же, точно на тебя вышел!";
					link.l1.go = "exit";
					break;
				}				
				if (sld.sex == "man")
				{
					dialog.text = GetFullName(sld) + ", ты о нем говоришь?";
					Link.l1 = "Да-да, точно, это он.";
					Link.l1.go = "SeekCitizen_agree";				
					Link.l2 = "Нет, не о нем. Давай еще раз назову.";
					Link.l2.go = "SeekCitizen_Choice_2";
				}
				else
				{
					dialog.text = GetFullName(sld) + ", ты о ней говоришь?";
					Link.l1 = "Ага, именно о ней.";
					Link.l1.go = "SeekCitizen_agree";				
					Link.l2 = "Нет, не о ней. Слушай, давай я еще раз попробую назвать.";
					Link.l2.go = "SeekCitizen_Choice_2";
				}
				Link.l3 = "Знаешь, не хочу я больше ни о ком спрашивать. До свидания.";
				Link.l3.go = "exit";
				npchar.quest.seekIdx = sld.index;
			}
		break;
        case "SeekCitizen_Choice_2":
			dialog.text = "Тогда назови еще раз имя и фамилию.";
			Link.l1.edit = 3;
			Link.l1 = "";
			Link.l1.go = "SeekCitizen_Choice_1";
		break;

		case "SeekCitizen_agree":
			sld = &characters[sti(npchar.quest.seekIdx)];
			npchar.quest.seekIdx.where = WhereLSCCitizen(sld);
			if (npchar.quest.seekIdx.where == "none")
			{
				if (sld.sex == "man")
				{
					dialog.text = LinkRandPhrase("Не знаю, давно его не видел.", "Черт его знает, где он...", "Не видел его уже прилично, так что не ведаю.");
					link.l1 = RandPhraseSimple("Понятно.", "Жаль...");
					link.l1.go = "exit";
				}
				else
				{
					dialog.text = LinkRandPhrase("Не видел ее уже довольно давно, сожалею.", "Понятия не имею, где она сейчас.", "Хм, я не знаю, где она может сейчас быть.");
					link.l1 = RandPhraseSimple("Ясно.", "Жаль...");
					link.l1.go = "exit";
				}
			}
			else
			{
				if (sld.sex == "man")
				{
					if (sld.location == "LostShipsCity_town")  
					{						
						string Str1 = npchar.location.locator;
						string Str2 = sld.location.locator;
						if (npchar.location == sld.location && strcut(Str1, 0, 5) == strcut(Str2, 0, 5))
						{
							dialog.text = LinkRandPhrase("Он здесь, на этом корабле. Ищи внимательней.", "Ха, так он здесь, на этом корабле!", "Он на этом корабле, странно, что ты еще не увидел его.");
						}
						else
						{
							dialog.text = LinkRandPhrase("Я видел его совсем недавно " + npchar.quest.seekIdx.where + ".", "Недавно встретил его " + npchar.quest.seekIdx.where + ". Так что ищи его там.", "Насколько я знаю, сейчас он находится " + npchar.quest.seekIdx.where + ".");
						}
					}
					else
					{
						if (npchar.location == sld.location)
						{
							dialog.text = LinkRandPhrase("Так он здесь, " + npchar.quest.seekIdx.where + ". Смотри внимательней.", "Так ведь он здесь, среди нас!", "Куда ты смотришь? Он же здесь ошивается.");
						}
						else
						{
							dialog.text = LinkRandPhrase("Я видел его совсем недавно " + npchar.quest.seekIdx.where + ".", "Недавно встретил его " + npchar.quest.seekIdx.where + ". Так что ищи его там.", "Насколько я знаю, сейчас он находится " + npchar.quest.seekIdx.where + ".");
						}
					}
					link.l1 = RandPhraseSimple("Ага, понятно. Спасибо тебе, приятель.", "Спасибо тебе!");
					link.l1.go = "exit";
				}
				else
				{
					if (sld.location == "LostShipsCity_town")  
					{						
						string Str3 = npchar.location.locator;
						string Str4 = sld.location.locator;
						if (npchar.location == sld.location && strcut(Str3, 0, 5) == strcut(Str4, 0, 5))
						{
							dialog.text = LinkRandPhrase("Она здесь, на этом корабле. Ищи внимательней.", "Ха, так она здесь, на этом корабле!", "Она на этом корабле, странно, что ты еще не увидел ее.");
						}
						else
						{
							dialog.text = LinkRandPhrase("Я видел ее совсем недавно " + npchar.quest.seekIdx.where + ".", "Недавно встретил ее " + npchar.quest.seekIdx.where + ". Мы даже поговорили.", "Насколько я знаю, сейчас она находится " + npchar.quest.seekIdx.where + ".");
						}
					}
					else
					{
						if (npchar.location == sld.location)
						{
							dialog.text = LinkRandPhrase("Так она здесь, " + npchar.quest.seekIdx.where + ". Смотри внимательней.", "Так ведь она здесь, среди нас!", "Куда ты смотришь? Она же где-то здесь ходит.");
						}
						else
						{
							dialog.text = LinkRandPhrase("Я видел ее совсем недавно " + npchar.quest.seekIdx.where + ".", "Недавно встретил ее " + npchar.quest.seekIdx.where + ". Мы даже поговорили с ней.", "Насколько я знаю, сейчас она находится " + npchar.quest.seekIdx.where + ".");
						}
					}
					link.l1 = RandPhraseSimple("Ага, понятно. Спасибо тебе, приятель.", "Спасибо тебе!");
					link.l1.go = "exit";
				}
			}
		break;
		//вопросы
		case "int_quests":
			dialog.text = "Внимательно тебя слушаю, дружище.";
			if (!CheckAttribute(NextDiag, NodeName+".l1"))
			{
				link.l1 = "Послушай, что это за остров?";
				link.l1.go = "ansewer_1";
			}
			if (!CheckAttribute(NextDiag, NodeName+".l2"))
			{
				link.l2 = "Слушай, а ты не знаешь, почему здесь так много кораблей собралось? Это же из ряда вон выходящий случай! И почему они не тонут?";
				link.l2.go = "ansewer_2";
			}
			if (!CheckAttribute(NextDiag, NodeName+".l3"))
			{
				link.l3 = "А что за человек, адмирал?";
				link.l3.go = "ansewer_3";
			}
			if (!CheckAttribute(NextDiag, NodeName+".l4"))
			{
				link.l4 = "Что у вас тут есть интересного?";
				link.l4.go = "ansewer_4";
			}
			link.l10 = "Нет вопросов. Извини, приятель...";
			link.l10.go = "exit";
		break;
		case "ansewer_1":
			dialog.text = "Это Город Потерянных Кораблей, приятель. Если попал сюда, то забудь обо всем, чем жил раньше. Отсюда не вырвешься, к сожалению...";
			link.l1 = "Дьявол!!";
			link.l1.go = "exit";
			NextDiag.(NodePrevName).l1 = true;
		break;
		case "ansewer_2":
			dialog.text = "Точно никто не знает. Вроде бы какое-то течение очень сильное как будто заворачивает все корабли сюда. Водоросли еще...";
			link.l1 = "Водоросли? Хм... Может под нами какая-то мель есть, рифы?..";
			link.l1.go = "ansewer_2_1";
			NextDiag.(NodePrevName).l2 = true;
		break;
		case "ansewer_2_1":
			dialog.text = "Не знаю, и тебе не советую узнавать. Плавать на поверхности можно без опаски, но вот стоит поднырнуть... В общем, кто пытался, тех мы не больше не видели.";
			link.l1 = "И утопленники даже не всплывали потом?";
			link.l1.go = "ansewer_2_2";
		break;
		case "ansewer_2_2":
			dialog.text = "Нет, даже края их одежды никто видел больше.";
			link.l1 = "Черт, жутковато...";
			link.l1.go = "ansewer_2_3";
		break;
		case "ansewer_2_3":
			dialog.text = "Ничего, привыкнешь.";
			link.l1 = "Что-то как-то не хочется привыкать...";
			link.l1.go = "ansewer_2_4";
		break;
		case "ansewer_2_4":
			dialog.text = "Понимаю, но придется. А не сумеешь обтесаться здесь - с ума сойдешь. И тогда тебе конец по Закону!";
			link.l1 = "Чего?!";
			link.l1.go = "ansewer_2_5";
		break;
		case "ansewer_2_5":
			dialog.text = "Отправишься на дно с грузом помоев на шее. Так-то!";
			link.l1 = "Да уж, порядочки у вас, однако...";
			link.l1.go = "exit";
		break;
		case "ansewer_3":
			dialog.text = "Адмирал - это губернатор нашего Города. Вообще-то раньше адмирала выбирали, но сейчас Чад Коппер не очень-то жаждет этого, по всему видать...";
			link.l1 = "Это с чето ты так решил?";
			link.l1.go = "ansewer_3_1";
			NextDiag.(NodePrevName).l3 = true;
		break;
		case "ansewer_3_1":
			dialog.text = "Да с того, что выборы адмирала - как табу стали в Городе. Об этом не говорят... И тебе не советую.";
			link.l1 = "Хм, понятно.";
			link.l1.go = "exit";
		break;
		case "ansewer_4":
			dialog.text = "Интересного? Ну, не знаю, что тебя интересует... Есть таверна, магазин, церковь, ну и резиденцию, естественно.";
			link.l1 = "Понятно.";
			link.l1.go = "exit";
			NextDiag.(NodePrevName).l4 = true;
		break;
	}
	NextDiag.PrevNode = NodeName;
}
