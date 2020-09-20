
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
					dialog.text = "Несмотря на продолжающийся шторм, примите мои поздравления, капитан.";
					link.l1 = "Поздравления?";
					link.l1.go = "FS_1";
				}
				else
				{
					dialog.text = "Мои поздравления, капитан.";
					link.l1 = "Поздравления?";
					link.l1.go = "FT_1";
				}
			}
			else
			{
				dialog.text = "Что вы хотели, сэр?";
				link.l1 = "Послушайте, я ищу одного человека. Не знаете, где он сейчас?";
				link.l1.go = "SeekCitizen";
				link.l2 = "Хочу задать вам вопрос.";
				link.l2.go = "int_quests";
				link.l10 = "Ничего...";
				link.l10.go = "exit";
			}
		break;
		case "FS_1":
			dialog.text = "Ну конечно! Вы пережили крушение, попали сюда вместо того, чтобы кануть в лету. Разве не стоит с этим поздравить? А шторм пройдет уже завтра к вечеру, к тому же теперь он вам не опасен.";
			link.l1 = "Согласен...";
			link.l1.go = "FT_2";
		break;
		case "FT_1":
			dialog.text = "Ну конечно! Вы пережили крушение, попали сюда вместо того, чтобы кануть в лету. Разве не стоит с этим поздравить?";
			link.l1 = "Вам виднее, сэр...";
			link.l1.go = "FT_2";
		break;
		case "FT_2":
			dialog.text = GetFullName(npchar) + ", сэр.";
			link.l1 = "Рад знакомству, Натан. Меня зовут капитан " + GetFullName(pchar) + ".";
			link.l1.go = "FT_3";
		break;
		case "FT_3":
			dialog.text = "Взаимно, капитан. Чем могу помочь?";
			link.l1 = "Давно Вы в городе?";
			link.l1.go = "FT_4";
		break;
		case "FT_4":
			dialog.text = "Очень давно, дорогой капитан. Не скажу, что я здесь дольше всех, но...";
			link.l1 = "Вы были здесь до Каппера?..";
			link.l1.go = "FT_5";
		break;
		case "FT_5":
			dialog.text = "Я... кхм, какое это имеет значение?";
			link.l1 = "Почему никому нельзя покидать город?";
			link.l1.go = "FT_6";
		break;
		case "FT_6":
			dialog.text = "На то есть особые причины, капитан. Я бы не советовал вам лезть в это дело.";
			link.l1 = "Почему?";
			link.l1.go = "FT_7";
		break;
		case "FT_7":
			dialog.text = "Потому что мои поздравления в этом случае будет разумно отменить – вы уже не будете... выжившим.";
			link.l1 = "Даже так?..";
			link.l1.go = "FT_8";
		break;
		case "FT_8":
			dialog.text = "При всём уважении, капитан, я много повидал храбрецов на своём веку, но те из них, что встретились мне в этом городе, давно отправились к Морскому Дьяволу.";
			link.l1 = "Возможно, встреча с ним и мне не повредит... Как бы там ни было, у Каппера есть друзья, правая рука, помощники?";
			link.l1.go = "FT_9";
		break;
		case "FT_9":
			dialog.text = "У него целая милиция правых рук! Но я настаиваю, капитан, чтобы вы не подвергали себя риску. Слишком многим я рассказывал то же, слишком многие стояли вот так предо мной, как вы сейчас, и я видел, глядя на них, отвагу... Но все они мертвы.";
			link.l1 = "Каппер же не бессмертен, правда?";
			link.l1.go = "FT_10";
		break;
		case "FT_10":
			dialog.text = "Кто знает, кто знает...";
			link.l1 = "Что ж, благодарю за беспокойство.";
			link.l1.go = "FT_11";
		break;
		case "FT_11":
			dialog.text = "Берегите себя, капитан.";
			link.l1 = "Всенепременно.";
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
			if (!CheckAttribute(NextDiag, NodeName+".l3"))
			{
				link.l3 = "И насколько долгими бывают штомы здесь?";
				link.l3.go = "ansewer_3";
			}
			if (!CheckAttribute(NextDiag, NodeName+".l1"))
			{
				link.l1 = "Здесь как-то неуютно, вы не находите?";
				link.l1.go = "ansewer_1";
			}
			if (!CheckAttribute(NextDiag, NodeName+".l2"))
			{
				link.l2 = "Откуда берётся товар в магазинах? Разве есть связь с внешним миром?";
				link.l2.go = "ansewer_2";
			}
			link.l10 = "Нет вопросов. Извините...";
			link.l10.go = "exit";
		break;
		case "ansewer_3":
			dialog.text = "По разному. Последний длился неделю и был достаточно сильным. Вообще, конечно, душа в пятки уходит, когда волны начинают биться о крабли Города...";
			link.l1 = "И такое бывает?";
			link.l1.go = "ansewer_3_1";
		break;
		case "ansewer_3_1":
			dialog.text = "Конечно бывает! Но раз Город на месте, значит нет оснований для беспокойства. Я так думаю...";
			link.l1 = "Ну что же, согласен.";
			link.l1.go = "exit";
			NextDiag.(NodePrevName).l3 = true;
		break;
		case "ansewer_1":
			dialog.text = "Ещё бы тут было уютно! Нет, это место не лучше и не хуже многих других, но успокоения тебе здесь не сыскать.";
			link.l1 = "Хм, да я и не ищу.";
			link.l1.go = "exit";
			NextDiag.(NodePrevName).l1 = true;
		break;
		case "ansewer_2":
			dialog.text = "Зачем нам связь? Сюда регулярно приходят корабли, чтобы не вернуться назад... полные трюмы, золотой запас, снабжение для колоний в Новом свете. У нас всего хватает.";
			link.l1 = "Интересно.";
			link.l1.go = "exit";
			NextDiag.(NodePrevName).l2 = true;
		break;
	}
	NextDiag.PrevNode = NodeName;
}
