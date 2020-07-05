
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
					dialog.text = "С прибытием в Город Потерянных Кораблей, капитан. Я " + GetFullName(npchar) + ". Ну, как вам наша погода? Весело, верно ведь?!";
					link.l1 = "Весело - не то слово!.. " + GetFullName(pchar) + " к вашим услугам, месье " + npchar.lastname + ". И часто у вас такое творится?";
					link.l1.go = "FS_1";
				}
				else
				{
					dialog.text = "С прибытием в Город Потерянных Кораблей, капитан. Я " + GetFullName(npchar) + ".";
					link.l1 = GetFullName(pchar) + " к вашим услугам, месье " + npchar.lastname + ". Откуда вы?.";
					link.l1.go = "FT_1";
				}
			}
			else
			{
				dialog.text = "Что вы хотели, месье?";
				link.l1 = "Послушайте, я ищу одного человека. Не знаете, где он сейчас?";
				link.l1.go = "SeekCitizen";
				link.l2 = "Хочу задать вам вопрос.";
				link.l2.go = "int_quests";
				link.l3 = "Ничего...";
				link.l3.go = "exit";
				if (pchar.questTemp.LSC == "toAndreLabor")
				{
					link.l4 = "Приятель, как дела, как жизнь?";
					link.l4.go = "Tizer_1";
				}

			}
		break;
		case "FS_1":
			dialog.text = "Бывает, месье... Я поначалу просто от страха умирал в такие дни, а сейчас привык.";
			link.l1 = "Понятно... Скажите, откуда вы?";
			link.l1.go = "FT_1";
		break;
		case "FT_1":
			dialog.text = "В своё время я был в подмастерье у хозяина верфи на Тортуге, а после служил плотником на борту фрегата 'Андромеда'. К сожалению, мы оказались не в том месте и не в то время.";
			link.l1 = "Что произошло?";
			link.l1.go = "FT_2";
		break;
		case "FT_2":
			dialog.text = "Очень много всего, месье, очень много. И всё сразу, представьте себе! Сперва рабы подняли восстание, а в самый разгар подавления нас атаковали охотники за головами, потому что у нас на борту находился один... человек, который был им нужен.\n"+
				"Жаркая абордажная схватка, а потом ещё и шторм начался, а точнее будто нас вынесло в какую-то штормовую зону. Далее наш фрегат наткнулся на скалы или рифы, дал течь и пошел ко дну. Я схватился за пустой боченок и сумел привязаться к нему. Это спасло мне жизнь. Сутки меня болтало по океану, а потом вынесло сюда.";			
			link.l1 = "Лихо...";
			link.l1.go = "FT_3";
		break;
		case "FT_3":
			dialog.text = "Вы ведь очень хотите убраться отсюда, не так ли?";
			link.l1 = "А считаете, что это возможно?";
			link.l1.go = "FT_4";
		break;
		case "FT_4":
			dialog.text = "Не знаю, я о таком не слышал. Если бы мне дали возможность что-нибудь смастерить, я бы точно попытался. Но это запрещено Законом...";
			link.l1 = "А может попытаемся вместе?";
			link.l1.go = "FT_5";
		break;
		case "FT_5":
			dialog.text = "Нет, приятель, я пас. С нашим адмиралом можно можно оказаться на дне гораздо раньше, чем снимешься с якоря.";
			link.l1 = "Да уж... Ну ладно, приятно было познакомиться, " + npchar + ". Всего хорошего.";
			link.l1.go = "exit";
		break;

		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
		//ищем человека
		case "SeekCitizen":
			dialog.text = "А кого вы ищете?";
			Link.l1.edit = 3;
			Link.l1 = "";
			Link.l1.go = "SeekCitizen_Choice_1";
		break;
		case "SeekCitizen_Choice_1":
			sld = CheckLSCCitizen();
			if (sld.id == "none")
			{
				dialog.text = "Я не понимаю, о ком вы говорите. Мне нужно знать имя и фамилию этого человека.";
				Link.l1 = "Понятно. Давате я еще попробую назвать.";
				Link.l1.go = "SeekCitizen_Choice_2";				
				Link.l2 = "Спасибо, я лучше сам поищу.";
				Link.l2.go = "exit";	
			}
			else
			{
				if (sld.id == npchar.id)
				{
					dialog.text = "Так это же я!";
					link.l1 = "Надо же, точно на вас вышел!";
					link.l1.go = "exit";
					break;
				}				
				if (sld.sex == "man")
				{
					dialog.text = GetFullName(sld) + ", вы о нем говорите?";
					Link.l1 = "Да-да, точно, это он.";
					Link.l1.go = "SeekCitizen_agree";				
					Link.l2 = "Нет, не о нем. Даваие еще раз назову.";
					Link.l2.go = "SeekCitizen_Choice_2";
				}
				else
				{
					dialog.text = GetFullName(sld) + ", вы о ней говорите?";
					Link.l1 = "Ага, именно о ней.";
					Link.l1.go = "SeekCitizen_agree";				
					Link.l2 = "Нет, не о ней. Послушайте, давайте я еще раз попробую назвать.";
					Link.l2.go = "SeekCitizen_Choice_2";
				}
				Link.l3 = "Знаете, не хочу я больше ни о ком спрашивать. До свидания.";
				Link.l3.go = "exit";
				npchar.quest.seekIdx = sld.index;
			}
		break;
        case "SeekCitizen_Choice_2":
			dialog.text = "Тогда назовите еще раз имя и фамилию.";
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
							dialog.text = LinkRandPhrase("Он здесь, на этом корабле. Ищите внимательней.", "Ха, так он здесь, на этом корабле!", "Он на этом корабле, странно, что вы еще не увидели его.");
						}
						else
						{
							dialog.text = LinkRandPhrase("Я видел его совсем недавно " + npchar.quest.seekIdx.where + ".", "Недавно встретил его " + npchar.quest.seekIdx.where + ". Так что ищите его там.", "Насколько я знаю, сейчас он находится " + npchar.quest.seekIdx.where + ".");
						}
					}
					else
					{
						if (npchar.location == sld.location)
						{
							dialog.text = LinkRandPhrase("Так он здесь, " + npchar.quest.seekIdx.where + ". Смотрите внимательней.", "Так ведь он здесь, среди нас!", "Куда вы смотрите? Он же здесь ошивается.");
						}
						else
						{
							dialog.text = LinkRandPhrase("Я видел его совсем недавно " + npchar.quest.seekIdx.where + ".", "Недавно встретил его " + npchar.quest.seekIdx.where + ". Так что ищите его там.", "Насколько я знаю, сейчас он находится " + npchar.quest.seekIdx.where + ".");
						}
					}
					link.l1 = RandPhraseSimple("Ага, понятно. Спасибо вам.", "Спасибо вам!");
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
							dialog.text = LinkRandPhrase("Она здесь, на этом корабле. Ищите внимательней.", "Ха, так она здесь, на этом корабле!", "Она на этом корабле, странно, что вы еще не увидели ее.");
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
							dialog.text = LinkRandPhrase("Так она здесь, " + npchar.quest.seekIdx.where + ". Смотрите внимательней.", "Так ведь она здесь, среди нас!", "Куда вы смотрите? Она же где-то здесь ходит.");
						}
						else
						{
							dialog.text = LinkRandPhrase("Я видел ее совсем недавно " + npchar.quest.seekIdx.where + ".", "Недавно встретил ее " + npchar.quest.seekIdx.where + ". Мы даже поговорили с ней.", "Насколько я знаю, сейчас она находится " + npchar.quest.seekIdx.where + ".");
						}
					}
					link.l1 = RandPhraseSimple("Ага, понятно. Спасибо вам, "+GetAddress_Form(NPChar)+".", "Спасибо вам!");
					link.l1.go = "exit";
				}
			}
		break;
		//вопросы
		case "int_quests":
			dialog.text = "Внимательно вас слушаю, дружище.";
			if (!CheckAttribute(NextDiag, NodeName+".l5"))
			{
				link.l5 = "Слушай, а на чем эти корабли держаться в шторм?";
				link.l5.go = "ansewer_5";
			}
			if (!CheckAttribute(NextDiag, NodeName+".l1"))
			{
				link.l1 = "Почему никто не объединяется против этих запретов в Законе?";
				link.l1.go = "ansewer_1";
			}
			if (!CheckAttribute(NextDiag, NodeName+".l2"))
			{
				link.l2 = "Слушай, ты не знаешь, откуда взялся этот Город?";
				link.l2.go = "ansewer_2";
			}
			if (!CheckAttribute(NextDiag, NodeName+".l3"))
			{
				link.l3 = "Не случалось, чтобы сюда заплывал корабль или эскадра, предварительно не затонув в крушении?";
				link.l3.go = "ansewer_3";
			}
			if (!CheckAttribute(NextDiag, NodeName+".l4"))
			{
				link.l4 = "Неужели сюда не попадают высокопоставленные чины, офицеры?";
				link.l4.go = "ansewer_4";
			}
			link.l10 = "Нет вопросов. Извините...";
			link.l10.go = "exit";
		break;
		case "ansewer_5":
			dialog.text = "Понятия не имею. И тебе не советую узнавать - опасно очень.";
			link.l1 = "Почему опасно?";
			link.l1.go = "ansewer_5_1";
		break;
		case "ansewer_5_1":
			dialog.text = "Хе, чтобы узнать, надо нырнуть под корабли. А кто ныряет, тот не выныривает...";
			link.l1 = "Теперь понятно.";
			link.l1.go = "exit";
			NextDiag.(NodePrevName).l5 = true;
		break;
		case "ansewer_1":
			dialog.text = "Хмм, объединиться? Ну, попробуй – попутного ветра в горбатую спину. Я... мы любим казни, нам нравится смотреть на тебе подобных, чьи силуэты медленно растворяются в морской пучине.";
			link.l1 = "Не горю желанием взглянуть на мир из-под воды.";
			link.l1.go = "exit";
			NextDiag.(NodePrevName).l1 = true;
		break;
		case "ansewer_2":
			dialog.text = "Да черт его знает! Я обычный плотник, мне сложно рассуждать о вопросах естествознания.";
			link.l1 = "Ну, хоть какое-то мнение у тебя есть?";
			link.l1.go = "ansewer_2_1";
			NextDiag.(NodePrevName).l2 = true;
		break;
		case "ansewer_2_1":
			dialog.text = "Да так, в общих чертах. Я думаю, что под Городом находятся рифы, которые и удерживают все эти корабли. Сначала зацепился один, за него другой, а потом все это и наросло.";
			link.l1 = "Ха! Интересная теория...";
			link.l1.go = "exit";
		break;
		case "ansewer_3":
			dialog.text = "Не припоминаю такого.";
			link.l1 = "Но это возможно?";
			link.l1.go = "ansewer_3_1";
			NextDiag.(NodePrevName).l3 = true;
		break;
		case "ansewer_3_1":
			dialog.text = "Только, если случится чудо. Сам посуди, вокруг скалы и рифы...";
			link.l1 = "Хм, это точно.";
			link.l1.go = "exit";
		break;
		case "ansewer_4":
			dialog.text = "Попадают, куда им деться.";
			link.l1 = "И что?";
			link.l1.go = "ansewer_4_1";
			NextDiag.(NodePrevName).l3 = true;
		break;
		case "ansewer_4_1":
			dialog.text = "А что? Каппер - пират!";
			link.l1 = "Он убивает их?";
			link.l1.go = "ansewer_4_2";
		break;
		case "ansewer_4_2":
			dialog.text = "Вовсе не обязательно. Если они вовремя понимают, где их место и в каком направлении отправились все их привилегии, то остаются жить.";
			link.l1 = "Забавно.";
			link.l1.go = "exit";
		break;
		//квестовые диалоги
		case "Tizer_1":
			dialog.text = "Да все в порядке.";
			link.l1 = "Слушай, а ты не можешь построить какое-нибудь плавательное средство. Плот или кораблик какой...";
			link.l1.go = "Tizer_2";
		break;
		case "Tizer_2":
			dialog.text = "Хе, а ты Закон-то знаешь? Это запрещено!";
			link.l1 = "Да ладно тебе! Тизеру Дэну ты помог, помоги и остальным нуждающимся.";
			link.l1.go = "Tizer_3";
		break;
		case "Tizer_3":
			dialog.text = "Тизеру Дэну? Хо!.. Ты меня изрядно удивил, приятель. Тизер Дэн мертв.";
			link.l1 = "В данный конкретный момент – да. А раньше – нет. Хе-хе...";
			link.l1.go = "Tizer_4";
		break;
		case "Tizer_4":
			dialog.text = "М-да, что-то непонятно... Знаешь что, давай об этом деле поговорим в другом месте. Тебе известно где находится корвет 'Протектор'? Иди туда и подожди меня в каюте. Ну я следом, чтобы нам не вместе тусоваться.";
			link.l1 = "Здравая мысль! Я расскажу тебе такую новость, которая, полагаю, тебя здорово обрадует. Так что не заставляй меня ждать.";
			link.l1.go = "Tizer_5";
		break;
		case "Tizer_5":
			dialog.text = "Ни за что на свете, дружище!";
			link.l1 = "Хорошо, буду ждать тебя на 'Протекторе'.";
			link.l1.go = "exit";
			AddQuestRecord("ISS_MainLine", "8");
			pchar.questTemp.LSC = "toProtector";
			pchar.quest.LSC_enterToProtector.win_condition.l1 = "location";
			pchar.quest.LSC_enterToProtector.win_condition.l1.location = "ProtectorFisher";
			pchar.quest.LSC_enterToProtector.function = "LSC_enterToProtector";
		break;

		case "Tizer_6":
			dialog.text = "";
			link.l1 = "";
			link.l1.go = "Tizer_7";
		break;


	}
	NextDiag.PrevNode = NodeName;
}
