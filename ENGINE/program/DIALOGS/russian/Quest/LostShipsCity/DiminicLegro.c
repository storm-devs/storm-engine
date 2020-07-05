
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
					dialog.text = "Мои поздравления новоприбывшему, сейчас как раз время. Я " + GetFullName(npchar) + ", меня еще зовут здесь Счастливым.";
					link.l1 = "Ну, здравствуй, " + pchar.name + ". Моё имя " + GetFullName(pchar) + ", будем знакомы. А что значит 'вовремя'?";
					link.l1.go = "FS_1";
				}
				else
				{
					dialog.text = "Мои поздравления новоприбывшему. Я " + GetFullName(npchar) + ", меня еще зовут здесь Счастливым.";
					link.l1 = "Ну, здравствуй, " + pchar.name + ". Моё имя " + GetFullName(pchar) + ", будем знакомы.";
					link.l1.go = "FT_1";
				}
			}
			else
			{
				dialog.text = "Что вы хотели, сэр?";
				link.l1 = "Слушай, я ищу одного человека. Не знаешь, где он сейчас?";
				link.l1.go = "SeekCitizen";
				link.l2 = "Хочу задать тебе вопрос.";
				link.l2.go = "int_quests";
				link.l10 = "Ничего...";
				link.l10.go = "exit";
			}
		break;
		case "FS_1":
			dialog.text = "Шторм спадает, и все везунчики выползают из воды.";
			link.l1 = "Понятно. Ну, ты тоже в их числе!";
			link.l1.go = "FS_2";
		break;
		case "FS_2":
			dialog.text = "Это верно, но я - абсолютный везунчик. Дело в том, что мой корабль разбился в самый пик шторма!";
			link.l1 = "Ого, вот это да! Расскажи, пожалуйста.";
			link.l1.go = "FT_3";
		break;
		case "FT_1":
			dialog.text = "Что ж, очень приятно.";
			link.l1 = "А почему тебя называют счастливым?";
			link.l1.go = "FT_2";
		break;
		case "FT_2":
			dialog.text = "Тому причин более чем достаточно. Я спасся после кораблекрушения, как и большинство местных, но моё крушение было особенным...";
			link.l1 = "И что же особенного было в твоём крушении?";
			link.l1.go = "FT_3";
		break;
		case "FT_3":
			dialog.text = "Ха, это было просто дьявольское крушение, чёрт побери! В наш фрегат ударила молния, и мигом начался пожар, после чего огонь как-то очень быстро пробрался в пороховой погреб.\n"+
				"Нет, ты только представь! Корабль за считанные минуты превратился в груду полыхающих обломком, а я даже не обжёгся! Более того, я находился неподалёку от трюма в момент начала катастрофы, и Город явился вовсе не с пустыми руками – при мне были драгоценности, деньги, редкие артефакты...";
			link.l1 = "Адмирал не конфисковал всё это?";
			link.l1.go = "FT_4";
		break;
		case "FT_4":
			dialog.text = "Адмирал – на редкость сообразительный алчный мерзавец, но я любого жулика сумею обжулить. О предстоящей встрече с Каппером мне сообщили заранее, и многое я успел принять меры, так сказать. Хотя, на какой ляд мне сдались эти попрякушли здесь - самому непонятно...";
			link.l1 = "Хм... Слушай, а что это за меры ты предпринял? Очень интересно узнать...";
			link.l1.go = "FT_5";
		break;
		case "FT_5":
			dialog.text = "Не скажу, приятель, ты уж извини. Мое счастье - оно только мое, боюсь потерять. Я суеверный.";
			link.l1 = "Да уж, это точно. А чем ты здесь занимаешься?";
			link.l1.go = "FT_6";
		break;
		case "FT_6":
			dialog.text = "Да ничем! Пути назад нет, а потому мне и искать его не хочется. Я просто живу сегодняшним днём... и мне это нравится, чёрт возьми!";
			link.l1 = "Что ж, это тоже позиция. Спасибо за беседу... Слушай, а может передумаешь и расскажешь мне о мерах?";
			link.l1.go = "FT_7";
		break;
		case "FT_7":
			dialog.text = "Прости, дружище, никак невозможно. Могу только намекнуть, что все надо кое-куда сложить перед встречей с Чадом... Это все.";
			link.l1 = "Понятно... Ну что ж, спасибо и на этом.";
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
			if (!CheckAttribute(NextDiag, NodeName+".l4"))
			{
				link.l4 = "Я так понимаю, что штормы здесь - дело привычное.";
				link.l4.go = "ansewer_4";
			}
			if (!CheckAttribute(NextDiag, NodeName+".l1"))
			{
				link.l1 = "А здесь можно?..";
				link.l1.go = "ansewer_1";
			}
			if (!CheckAttribute(NextDiag, NodeName+".l2"))
			{
				link.l2 = "Как в городе решаются личные проблемы? Неужели люди здесь настолько законопослушные, что не устраивают драк и не убивают друг друга?";
				link.l2.go = "ansewer_2";
			}
			if (!CheckAttribute(NextDiag, NodeName+".l3"))
			{
				link.l3 = "Есть шанс добраться вплавь... куда-нибудь?";
				link.l3.go = "ansewer_3";
			}
			link.l10 = "Нет вопросов. Извини, приятель...";
			link.l10.go = "exit";
		break;
		case "ansewer_4":
			dialog.text = "Да я бы не сказал. Просто, что не шторм - так сущий ад! Все скрипит, шатается... Страшновато, в общем.";
			link.l1 = "Понятно.";
			link.l1.go = "exit";
			NextDiag.(NodePrevName).l4 = true;
		break;
		case "ansewer_1":
			dialog.text = "Можно всё то, что не запрещено Законом. Можно,... только осторожно.";
			link.l1 = "Я учту.";
			link.l1.go = "exit";
			NextDiag.(NodePrevName).l1 = true;
		break;
		case "ansewer_2":
			dialog.text = "Всякое бывает, конечно. Чад со своей милицией может кого-угодно утихомитить. Просто объявляется, что гражданин сошел с ума - и все дела.";
			link.l1 = "Неплохо... А что, действительно у людей башню сносит?";
			link.l1.go = "ansewer_2_1";
			NextDiag.(NodePrevName).l2 = true;
		break;
		case "ansewer_2_1":
			dialog.text = "Бывает...";
			link.l1 = "Ясно. Ну а остальной народ как разбирается друг с другом?";
			link.l1.go = "ansewer_2_2";
		break;
		case "ansewer_2_2":
			dialog.text = "Втихаря. Бывает, кое-кто исчезает без следа... В общем, не пойман - не вор.";
			link.l1 = "Угу, понятно...";
			link.l1.go = "exit";
		break;
		case "ansewer_3":
			dialog.text = "Конечно, приятель! Прямиком в преисподнею!";
			link.l1 = "Благодарю.";
			link.l1.go = "exit";
			NextDiag.(NodePrevName).l3 = true;
		break;
	}
	NextDiag.PrevNode = NodeName;
}
