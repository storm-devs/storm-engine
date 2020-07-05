
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
					dialog.text = LinkRandPhrase("Ха, новый гражданин! Полагаю, что шторм не помешает нам познакомиться. Я - " + GetFullName(npchar) + ", состою в здешней милиции.", 
						"Так и есть единственный спасшийся с последнего корабля в этом шторме? Ну что же, добро пожаловать в Город. Меня зовут " + GetFullName(npchar) + ", я работаю в милиции.",
						"Вижу нового гражданина Города! Добро пожаловать, как говорится, а на шторм уже можете не обращать внимания. Мое имя - " + GetFullName(npchar) + ", я числюсь в местной миллиции.");
					link.l1 = RandPhraseSimple("Привет, я - " + GetFullName(pchar) + " Что за миллиция?", "Приветствую. Меня зовут " + GetFullName(pchar) + " А что это за миллиция?");
					link.l1.go = "FT_1";
				}
				else
				{				
					dialog.text = LinkRandPhrase("Ха, новый гражданин! Ну, здравствуй и давай знакомится. Я - " + GetFullName(npchar) + ", состою в здешней милиции.", 
						"Так и есть единственный спасшийся с последнего корабля? Ну что же, добро пожаловать в Город. Меня зовут " + GetFullName(npchar) + ", я работаю в милиции.",
						"Вижу нового гражданина Города! Добро пожаловать, как говорится. Мое имя - " + GetFullName(npchar) + ", я числюсь в местной миллиции.");
					link.l1 = RandPhraseSimple("Привет, я - " + GetFullName(pchar) + " Что за миллиция?", "Приветствую. Меня зовут " + GetFullName(pchar) + " А что это за миллиция?");
					link.l1.go = "FT_1";
				}
			}
			else
			{
				dialog.text = LinkRandPhrase("Где-то преступление?", "В чем дело, в Городе беспорядки?", "Что-то случилось?");
				link.l1 = RandPhraseSimple("Слушай, я ищу одного человека. Не знаешь, где он сейчас?", "Мне нужно найти одного человека. Ты не видел его где-нибудь поблизости?");
				link.l1.go = "SeekCitizen";
				link.l2 = RandPhraseSimple("Хочу задать тебе вопрос.", "У меня есть к тебе вопрос.");
				link.l2.go = "int_quests";
				link.l10 = "Нет, все нормально.";
				link.l10.go = "exit";
			}
		break;

		case "FT_1":
			dialog.text = LinkRandPhrase("Миллиция обеспевивает порядок и соблюдение Закона гражданами Города.", 
				"Миллиция смотрит тут за всем происходищем. В общем, это власть в Городе...", 
				"В обычных городах есть губернатор с гарнизоном солдат. Здесь губернатор - наш адмирал, а гарнизон - это мы, миллиционеры.")
			link.l1 = RandPhraseSimple("Понятно...", "Теперь ясно, спасибо за разъяснения.");
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
			dialog.text = LinkRandPhrase("Внимательно тебя слушаю, дружище.", "Говори, я слушаю.", "Внимательно слушаю.");
			if (!CheckAttribute(NextDiag, NodeName+".l1"))
			{
				link.l1 = LinkRandPhrase("Послушай, что это за остров?", "Что это за остров странный?", "Слушай, дружище, а что это за остров такой?");
				link.l1.go = "ansewer_1";
			}
			if (!CheckAttribute(NextDiag, NodeName+".l2"))
			{
				link.l2 = LinkRandPhrase("А ты не пробовал вырваться отсюда?", "Может, есть возможность свалить с этого места?", "Не пробовал уплыть отсюда?");
				link.l2.go = "ansewer_2";
			}
			link.l10 = RandPhraseSimple("Нет вопросов. Извини, дружище...", "Что-то из головы вылетело, забыл...");
			link.l10.go = "exit";
		break;
		case "ansewer_1":
			dialog.text = LinkRandPhrase("Этот остров называется Городом Потерянных Кораблей. Ты теперь гражданин этого Города.", 
				"Очень необычное место, не правда ли? Остров называется Городом Потерянных Кораблей. Теперь и ты будешь жить с нами.", 
				"Это Город Потерянных Кораблей. Весь остров состоит из кораблей, которые сюда занесла нелегкая...");
			link.l1 = RandPhraseSimple("Понятно.", "Ясно...");
			link.l1.go = "exit";
			NextDiag.(NodePrevName).l1 = true;
		break;
		case "ansewer_2":
			dialog.text = LinkRandPhrase("А зачем? Нас и здесь неплохо кормят...", 
				"Это запрещено Законом. А вообще-то, лично я и не хочу, меня здесь все устраивает.", 
				"Ты знаешь, здесь есть все, что я хотел получить от жизни. Зачем мне искать лучшей доли?");
			link.l1 = RandPhraseSimple("Понятно.", "Ясно...");
			link.l1.go = "exit";
			NextDiag.(NodePrevName).l2 = true;
		break;


		case "_1":
			dialog.text = "";
			link.l1 = "";
			link.l1.go = "_2";
		break;
		case "_2":
			dialog.text = "";
			link.l1 = "";
			link.l1.go = "_3";
		break;
		case "_3":
			dialog.text = "";
			link.l1 = "";
			link.l1.go = "_4";
		break;
	}
	NextDiag.PrevNode = NodeName;
}
