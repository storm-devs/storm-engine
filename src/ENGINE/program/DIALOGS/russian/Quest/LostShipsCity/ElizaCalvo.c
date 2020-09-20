
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
			if (npchar.quest.meeting == "0")
			{
				npchar.quest.meeting = "1";
				if (CheckAttribute(loadedLocation, "storm"))
				{
					dialog.text = "Этот шторм был очень сильным, сеньор, вам очень повезло, что вы смогли выбраться к нам... Меня зовут " + GetFullName(npchar) + ", в этом доме я хозяйка.";
					link.l1 = GetFullName(pchar) + " к вашим услугам, сеньора. Скажите, а как долго продолжался этот шторм?";
					link.l1.go = "FS_1";
				}
				else
				{
					dialog.text = "Здравствуйте! Я так понимаю, что это именно вы спаслись последним.";
					link.l1 = GetFullName(pchar) + " к вашим услугам, сеньора. Да, это я.";
					link.l1.go = "FT_1";
				}
			}
			else
			{
				dialog.text = "Что вам нужно у меня в доме, сеньор?";
				link.l1 = "Вы знаете, я ищу одного человека. Не поможете мне?";
				link.l1.go = "SeekCitizen";
				link.l2 = "Я хотел задать вам вопрос.";
				link.l2.go = "int_quests";
				link.l10 = "Ничего...";
				link.l10.go = "exit";
			}
		break;

		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;

		case "FS_1":
			dialog.text = "Что-то около недели, точно не скажу. Знаете, когда такое происходит вокруг, теряешь счет времени.";
			link.l1 = "Да, понимаю.";
			link.l1.go = "FS_2";
		break;
		case "FS_2":
			dialog.text = "Кстати, все попавшие в Город должны отметиться у адмирала, Чада Каппера, и получить гражданство. Если вы еще не сделали это, то советую поторопиться.";
			link.l1 = "А где он находится, этот Чад Каппер?";
			link.l1.go = "FS_3";
		break;
		case "FS_3":
			dialog.text = "В резиденции города, на варшипе 'Сан Августин'.";
			link.l1 = "Спасибо.";
			link.l1.go = "exit";
		break;
		case "FT_1":
			dialog.text = "А меня зовут " + GetFullName(npchar) + ". Я хозяйка этой каюты или этого дома, как вам будет угодно называть это пристанище.";
			link.l1 = "Наверно, 'дом' будет правильней.";
			link.l1.go = "FT_2";
		break;
		case "FT_2":
			dialog.text = "Я тоже так думаю. Ну что же, добро пожаловать в мой дом.";
			link.l1 = "Спасибо.";
			link.l1.go = "exit";
		break;
		//ищем человека
		case "SeekCitizen":
			dialog.text = "С удовольствием вам помогу, если смогу, конечно. Так кого вы ищете?";
			Link.l1.edit = 3;
			Link.l1 = "";
			Link.l1.go = "SeekCitizen_Choice_1";
		break;
		case "SeekCitizen_Choice_1":
			sld = CheckLSCCitizen();
			if (sld.id == "none")
			{
				dialog.text = "Простите, но я совершенно не понимаю, о ком вы говорите. Чтобы вам помочь, мне нужны имя и фамилия.";
				Link.l1 = "Хм. Давайте я попробую еще раз их назвать.";
				Link.l1.go = "SeekCitizen_Choice_2";				
				Link.l2 = "Благодарю вас, но лучше я сам поищу.";
				Link.l2.go = "exit";	
			}
			else
			{
				if (sld.id == npchar.id)
				{
					dialog.text = "Хм, простите, но вы ищете меня. Это я.";
					link.l1 = "Надо же!";
					link.l1.go = "exit";
					break;
				}				
				if (sld.sex == "man")
				{
					dialog.text = GetFullName(sld) + ", вы его имеете ввиду?";
					Link.l1 = "Именно его.";
					Link.l1.go = "SeekCitizen_agree";				
					Link.l2 = "Нет, я имел ввиду другого.";
					Link.l2.go = "SeekCitizen_Choice_2";
				}
				else
				{
					dialog.text = GetFullName(sld) + ", вы о ней говорили?";
					Link.l1 = "Точно, именно о ней.";
					Link.l1.go = "SeekCitizen_agree";				
					Link.l2 = "Нет, не о ней. Может, я еще раз попробую назвать?";
					Link.l2.go = "SeekCitizen_Choice_2";
				}
				Link.l3 = "Вы знаете, я вам очень благодарен, но я не хочу больше ни о ком спрашивать.";
				Link.l3.go = "exit";
				npchar.quest.seekIdx = sld.index;
			}
		break;
        case "SeekCitizen_Choice_2":
			dialog.text = "Тогда вам придется еще раз назвать имя и фамилию.";
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
					dialog.text = LinkRandPhrase("Понятия не имею, где он.", "Ни малейшего представления не имею, где он сейчас.", "Я не видела его уже прилично по времени. Так что не могу знать.");
					link.l1 = RandPhraseSimple("Понятно.", "Ясно... Что же, спасибо.");
					link.l1.go = "exit";
				}
				else
				{
					dialog.text = LinkRandPhrase("Понятия не имею, где она.", "Ни малейшего представления не имею, где он сейчас.", "Я не видела ее уже прилично по времени. Так что не могу знать.");
					link.l1 = RandPhraseSimple("Понятно.", "Ясно... Что же, спасибо.");
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
							dialog.text = LinkRandPhrase("Он здесь, на этом корабле. Смотрите внимательней, пожалуйста.", "Хм, так ведь он сейчас на этом корабле...", "Он на этом корабле, странно, что вы еще встретились.");
						}
						else
						{
							dialog.text = LinkRandPhrase("Я видела его совсем недавно " + npchar.quest.seekIdx.where + ".", "Вы знаете, я недавно встретила его " + npchar.quest.seekIdx.where + ". Так что ищите его там.", "Насколько мне известно, сейчас он находится " + npchar.quest.seekIdx.where + ".");
						}
					}
					else
					{
						if (npchar.location == sld.location)
						{
							dialog.text = LinkRandPhrase("Простите, но он находится прямо здесь, " + npchar.quest.seekIdx.where + ". Вам нужно быть внимательным.", "Хм, он ведь здесь, среди нас!", "Если бы вы посмотрели вокруг более внимательно, то наверняка уже увидели его...");
						}
						else
						{
							dialog.text = LinkRandPhrase("Я видела его совсем недавно " + npchar.quest.seekIdx.where + ".", "Недавно встретила его " + npchar.quest.seekIdx.where + ". Так что вы сможете увидеть его там.", "Насколько мне известно, сейчас он находится " + npchar.quest.seekIdx.where + ".");
						}
					}
					link.l1 = RandPhraseSimple("Ага, понятно. Спасибо вам!", "Спасибо вам огромное.");
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
							dialog.text = LinkRandPhrase("Она здесь, на этом корабле. Смотрите внимательней, пожалуйста.", "Хм, так ведь она сейчас на этом корабле...", "Она на этом корабле, странно, что вы еще встретились.");
						}
						else
						{
								dialog.text = LinkRandPhrase("Я видела ее совсем недавно " + npchar.quest.seekIdx.where + ".", "Вы знаете, я недавно встретила ее " + npchar.quest.seekIdx.where + ". Так что ищите ее там.", "Насколько мне известно, сейчас она находится " + npchar.quest.seekIdx.where + ".");
						}
					}
					else
					{
						if (npchar.location == sld.location)
						{
							dialog.text = LinkRandPhrase("Простите, но она находится прямо здесь, " + npchar.quest.seekIdx.where + ". Вам нужно быть внимательным.", "Хм, она ведь здесь, среди нас!", "Если бы вы посмотрели вокруг более внимательно, то наверняка уже увидели ее...");
						}
						else
						{
							dialog.text = LinkRandPhrase("Я видела ее совсем недавно " + npchar.quest.seekIdx.where + ".", "Недавно встретила ее " + npchar.quest.seekIdx.where + ". Так что вы сможете увидеть ее там.", "Насколько мне известно, сейчас она находится " + npchar.quest.seekIdx.where + ".");
						}
					}
					link.l1 = RandPhraseSimple("Ага, понятно. Спасибо вам!", "Спасибо вам огромное.");
					link.l1.go = "exit";
				}
			}
		break;	
		//вопросы
		case "int_quests":
			dialog.text = "Внимательно вас слушаю.";
			if (!CheckAttribute(NextDiag, NodeName+".l1"))
			{
				link.l1 = "Послушайте, что это за остров?";
				link.l1.go = "ansewer_1";
			}
			if (!CheckAttribute(NextDiag, NodeName+".l2"))
			{
				link.l2 = "Штормы часто здесь бывают.";
				link.l2.go = "ansewer_2";
			}
			link.l10 = "У меня нет к вам вопросов, извините.";
			link.l10.go = "exit";
		break;
		case "ansewer_1":
			dialog.text = "Этот остров называется Городом Потерянных кораблей. И название, скажу я вам, совершенно точное по сути.";
			link.l1 = "Понятно.";
			link.l1.go = "exit";
			NextDiag.(NodePrevName).l1 = true;
		break;
		case "ansewer_2":
			dialog.text = "Довольно часто. Очень страшно, конечно, но мы привыкли...";
			link.l1 = "Ну да, деваться-то некуда!";
			link.l1.go = "exit";
			NextDiag.(NodePrevName).l2 = true;
		break;
	}
	NextDiag.PrevNode = NodeName;
}
