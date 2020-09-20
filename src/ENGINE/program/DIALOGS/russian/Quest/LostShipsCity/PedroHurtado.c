
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
					dialog.text = "Ага, так ты из это шторма вышел? Ну что же, отлично! Давай знакомиться. Меня зовут " + GetFullName(npchar) + ", а тебя?";
					link.l1 = "Мое имя - " + GetFullName(pchar) + ".";
					link.l1.go = "FS_1";
				}
				else
				{
					dialog.text = "Ага, так это ты из последнего пополнения? Понятно... Ну что же, давай знакомиться. Меня зовут " + GetFullName(npchar) + ", а тебя?";
					link.l1 = "Мое имя - " + GetFullName(pchar) + ".";
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
			dialog.text = "Очень приятно. На шторм внимания не обращай, скоро он закончится. Народ здесь дружный, так что становись гражданином города и все будет в порядке. В общем, добро пожаловать в Город!";
			link.l1 = "Спасибо.";
			link.l1.go = "exit";
		break;
		case "FT_1":
			dialog.text = "Очень приятно. Ну что же, осваивайся здесь понемногу. Народ тут живет довольно дружно, так что добро пожаловать в Город, как говорится...";
			link.l1 = "Спасибо.";
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
			dialog.text = "Слушаю тебя. Спрашивай все, что душе угодно.";
			if (!CheckAttribute(NextDiag, NodeName+".l5"))
			{
				link.l5 = "Чертов шторм! И часто у вас такое бывает?";
				link.l5.go = "ansewer_5";
			}
			if (!CheckAttribute(NextDiag, NodeName+".l1"))
			{
				link.l1 = "Что это за остров?";
				link.l1.go = "ansewer_1";
			}
			if (!CheckAttribute(NextDiag, NodeName+".l2"))
			{
				link.l2 = "Что в городе достойно внимания?";
				link.l2.go = "ansewer_2";
			}
			if (!CheckAttribute(NextDiag, NodeName+".l3"))
			{
				link.l3 = "Ты сам как сюда попал?";
				link.l3.go = "ansewer_3";
			}
			if (!CheckAttribute(NextDiag, NodeName+".l4"))
			{
				link.l4 = "На Большой земле я не один раз слышал об Острове Справедливости. Это, случаем, не ваш Город?";
				link.l4.go = "ansewer_4";
			}
			link.l10 = "Нет вопросов. Извини, приятель...";
			link.l10.go = "exit";
		break;
		case "ansewer_5":
			dialog.text = "Случаются, чего уж... Кстати, тебе надо поговорить с адмиралом Чадом Капером, если ты до сих пор этого не сделал.";
			link.l1 = "Понятно.";
			link.l1.go = "exit";
			NextDiag.(NodePrevName).l5 = true;
		break;
		case "ansewer_1":
			dialog.text = "Это Город Потерянных Кораблей, приятель. Тут живут все те, кто сумел спастись после кораблекрушений. Вот, теперь и ты.";
			link.l1 = "Да, теперь и я... Послушай, а почему тут такое твориться? Есть какое-нибудь всем этим крушениям?";
			link.l1.go = "ansewer_1_1";
			NextDiag.(NodePrevName).l1 = true;
		break;
		case "ansewer_1_1":
			dialog.text = "Точно никто не знает. Кораби, вроде бы, налетают на рифы. Ну а на рифы их несет течение... Но это так догадки. Точно никто не знает.";
			link.l1 = "И никто так и не пытался узнать?";
			link.l1.go = "ansewer_1_2";
		break;
		case "ansewer_1_2":
			dialog.text = "Пытались, чего уж. Только идея эта плохая - гибнут особо любопытные. Так что и тебе не советую особое любопытство проявлять. Или погинешь, или крыша поедет...";
			link.l1 = "Да уж, веселенькие перспективы...";
			link.l1.go = "ansewer_1_3";
		break;
		case "ansewer_1_3":
			dialog.text = "Раз уж попал сюда, то смирись и живи как все. Вот тебе мой совет.";
			link.l1 = "Ну что же, за совет спасибо.";
			link.l1.go = "exit";
		break;
		case "ansewer_2":
			dialog.text = "Во-первых, тебе нужно побыстрей явиться а адмиралу Города, если еще не был у него. Ну а потом можно наведаться в таверну и магазин. Еще есть церковь.";
			link.l1 = "Понятно...";
			link.l1.go = "exit";
			NextDiag.(NodePrevName).l2 = true;
		break;
		case "ansewer_3":
			dialog.text = "Да так же, как и все - через кораблекрушение, приятель...";
			link.l1 = "Понятно.";
			link.l1.go = "exit";
			NextDiag.(NodePrevName).l3 = true;
		break;
		case "ansewer_4":
			dialog.text = "Не знаю... А вообще, дружище, это теперь и твой Город, а не только наш.";
			link.l1 = "Ха, это радует!";
			link.l1.go = "exit";
			NextDiag.(NodePrevName).l4 = true;
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
