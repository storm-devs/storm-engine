
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
					dialog.text = "О, новый капитан! Кажется, именно ты пришел последним из этого шторма.";
					link.l1 = "Добрый день. Моё имя " + pchar.name + ", " + GetFullName(pchar) + ". До этого проклятого шторма я был капитаном...";
					link.l1.go = "FT_1";
				}
				else
				{				
					dialog.text = "О, новый капитан! Кажется, именно ты пришел последним.";
					link.l1 = "Добрый день. Моё имя " + pchar.name + ", " + GetFullName(pchar) + ". До крушения я был капитаном...";
					link.l1.go = "FT_1";
				}
			}
			else
			{
				dialog.text = "Ты что-то хотел от меня?";
				link.l1 = "Слушай, я ищу одного человека. Не знаешь, где он сейчас?";
				link.l1.go = "SeekCitizen";
				link.l2 = "Хочу задать тебе вопрос.";
				link.l2.go = "int_quests";
				link.l10 = "Ничего...";
				link.l10.go = "exit";
			}
		break;
		case "FT_1":
			dialog.text = "Хм, ты знаешь, мне кажется, что это имя мне знакомо. Капитан... Ну да ладно. Я Стефан Клейн, местный... житель.";
			link.l1 = "Как давно ты здесь, Том?";
			link.l1.go = "FT_2";
		break;
		case "FT_2":
			dialog.text = "Уже лет пять, дай бог памяти. Привык, приспособился – теперь живу и даже не жалуюсь, встречаю новичков.";
			link.l1 = "А ты как попал сюда?";
			link.l1.go = "FT_3";
		break;
		case "FT_3":
			dialog.text = "Ох, это долгая история, капитан...";
			link.l1 = "Расскажи, мне интересно.";
			link.l1.go = "FT_4";
			link.l2 = "Увы, на долгие истории времени у меня нет.";
			link.l2.go = "exit";
		break;
		case "FT_4":
			dialog.text = "Что ж, хорошо. Галеон 'Лидер' вышел в море от Белиза с большим грузом досок, парусины и красного дерева ясным апрельским утром, но к полудню разыгрался шторм столь ужасный, что даже наш бесстрашный капитан – храни господь его душу – бросил попытки управиться с кораблём.\n"+
				"'Лидер' сбился с курса и, гонимый ветром, напоролся на риф неподалёку отсюда при скорости настолько высокой, что камень, легко проткнув обшивку, прошёл по всей длине корабля, не оставив нам шансов на спасение. Не будучи бывалым моряком, я испугался пуще всех – уж очень не хотелось мне погибнуть вот так, посреди моря... и бросился за борт ещё до того, как галеон скрылся в пучине.\n"+
				"Я плыл, плыл, плыл, пока кровь в венах не закипела и не превратилась в кислоту... потом я проплыл ещё немного, но, конечно, не знал, куда направляюсь – меня направляли волны.\n"+
				"Увидев вдалеке это место, я подумал, что это мираж, но тут силы меня покинули, а после... а после я оказался здесь.";
			link.l1 = "Легко удалось тут освоиться?";
			link.l1.go = "FT_5";
		break;
		case "FT_5":
			dialog.text = "Сперва было тяжко, конечно. Я много думал о тех, кто погиб в тот день... Это трудно – быть единственным выжившим, не так ли? Но время лечит...";
			link.l1 = "Что ж, спасибо за беседу, Стефан. Я рад знакомству.";
			link.l1.go = "FT_6";
		break;
		case "FT_6":
			dialog.text = "Я тоже очень рад, капитан " + pchar.lastname + ".";
			link.l1 = "Всего наилучшего.";
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
			if (!CheckAttribute(NextDiag, NodeName+".l3"))
			{
				link.l3 = "Частенько тут штормит?";
				link.l3.go = "ansewer_3";
			}	
			if (!CheckAttribute(NextDiag, NodeName+".l1"))
			{
				link.l1 = "Слушай, а что это за место такое?";
				link.l1.go = "ansewer_1";
			}			
			if (!CheckAttribute(NextDiag, NodeName+".l2"))
			{
				link.l2 = "Чем вы занимаетесь тут?";
				link.l2.go = "ansewer_2";
			}
			link.l10 = "Нет вопросов. Извини, приятель...";
			link.l10.go = "exit";
		break;
		case "ansewer_1":
			dialog.text = "Это Город Потерянных Кораблей, приятель. И теперь ты здесь прописан навсегда. Добро пожаловать!";
			link.l1 = "Каррамба!";
			link.l1.go = "exit";
			NextDiag.(NodePrevName).l1 = true;
		break;
		case "ansewer_2":
			dialog.text = "А чем заниматься людям, ограниченным небольшим пространством, капитан? Мы существуем.";
			link.l1 = "Интересное занятие, думаю. Придётся попробовать.";
			link.l1.go = "exit";
			NextDiag.(NodePrevName).l2 = true;
		break;
		case "ansewer_3":
			dialog.text = "Бывает, а часто или нет - сложно сказать. Кому-то часто, кому-то нет...";
			link.l1 = " В смысле?";
			link.l1.go = "ansewer_3_1";
			NextDiag.(NodePrevName).l3 = true;
		break;
		case "ansewer_3_1":
			dialog.text = "Кто штормы не любит, тому часто. А кому все равно... ну, сам понимаешь.";
			link.l1 = "Да, теперь ясно.";
			link.l1.go = "exit";
		break;
	}
	NextDiag.PrevNode = NodeName;
}
