
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
					dialog.text = "Привет! Мы еще с тобой не знакомы. Не думаю, что для знакомства стоит ждать окончания шторма, так что представлюсь. Меня зовут " + GetFullName(npchar) + ".";
					link.l1 = "А меня " + GetFullName(pchar) + ".";
					link.l1.go = "FS_1";
				}
				else
				{
					dialog.text = "Привет! Мы еще с тобой не знакомы. Меня зовут " + GetFullName(npchar) + ".";
					link.l1 = "А меня " + GetFullName(pchar) + ".";
					link.l1.go = "FT_1";
				}
			}
			else
			{
				dialog.text = "Ты что-то хотел, дружище?";
				link.l1 = "Слушай, я ищу одного человека. Не знаешь, где он сейчас?";
				link.l1.go = "SeekCitizen";
				link.l2 = "Хочу задать тебе вопрос.";
				link.l2.go = "int_quests";
				link.l10 = "Ничего...";
				link.l10.go = "exit";
			}
		break;
		case "FS_1":
			dialog.text = "Как настроение?";
			link.l1 = "Нормально. Шторм меня уже не пугает. Так что все в порядке, осваиваюсь потихоньку.";
			link.l1.go = "FT_3";
		break;
		case "FT_1":
			dialog.text = "Ну что же, теперь и я тебя увидел. Как вообще настроение после такого?";
			link.l1 = "Чего 'такого'?";
			link.l1.go = "FT_2";
		break;
		case "FT_2":
			dialog.text = "После крушения, конечно. Вот я, помню, долго не мог прийти в себя...";
			link.l1 = "Ну, мне раскачиваться некогда. Нужно осваиваться здесь.";
			link.l1.go = "FT_3";
		break;
		case "FT_3":
			dialog.text = "И это правильно, приятель. Нельзя давать печали овладеть твоим разумом, иначе тут долго не протянешь! У некоторых крыша и едет, такой хорошо если себя погубит, так еще, бывало, и Городу навредит. Сам понимаешь, мы тут и так непонятно на чем держимся, а как начинают корали ходить - так вообще жутко. Скрип, треск, палубы под ногами пляшут...";
			link.l1 = "Да уж... И что, такое часто бывает?";
			link.l1.go = "FT_4";
		break;
		case "FT_4":
			dialog.text = "Да нет же, я тебе о чем и толкую! Наш Город - он живой, его трогать нельзя. Когда его начинают разбирать, какие-то корабли новые мастерить, чтобы уплыть отсюда, он и начинает мстить людям. Вот так то!";
			link.l1 = "Да, дела...";
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
			dialog.text = "Задавай. Если смогу ответить, то обязательно сделаю это.";
			if (!CheckAttribute(NextDiag, NodeName+".l1"))
			{
				link.l1 = "Послушай, что это за остров?";
				link.l1.go = "ansewer_1";
			}
			link.l10 = "Нет вопросов. Извини, приятель...";
			link.l10.go = "exit";
		break;
		case "ansewer_1":
			dialog.text = "Это Город Потерянный Кораблей, приятель. Имей ввиду, что Город - живой. Некоторые граждане в это не верят, но ты знай, что так оно и есть!";
			link.l1 = "Ну, буду имет ввиду, так сказать.";
			link.l1.go = "exit";
			NextDiag.(NodePrevName).l1 = true;
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
