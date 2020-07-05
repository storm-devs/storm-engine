
void ProcessDialogEvent()
{
	ref NPChar, sld;
	aref Link, NextDiag;

	DeleteAttribute(&Dialog,"Links");

	makeref(NPChar,CharacterRef);
	makearef(Link, Dialog.Links);
	makearef(NextDiag, NPChar.Dialog);
    // вызов диалога по городам -->
    NPChar.FileDialog2 = "DIALOGS\" + LanguageGetLanguage() + "\Tavern\" + NPChar.City + "_Waitress.c";
    if (LoadSegment(NPChar.FileDialog2))
	{
        ProcessCommonDialog(NPChar, Link, NextDiag);
		UnloadSegment(NPChar.FileDialog2);
	}
    // вызов диалога по городам <--
	switch(Dialog.CurrentNode)
	{
        case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
		
		case "First time":
            NextDiag.TempNode = "First time";
			if (CheckAttribute(pchar, "questTemp.different.FackWaitress"))
			{
				if (pchar.questTemp.different == "FackWaitress_toRoom" || pchar.questTemp.different == "FackWaitress_toRoomUp")
				{
					dialog.text = "Я просила, чтобы ты ждал меня наверху. Не стоит маячить здесь, не привлекай внимания...";
					link.l1 = "Хорошо, не буду тебя подставлять...";
					link.l1.go = "exit";
				}
				if (pchar.questTemp.different == "FackWaitress_noMoney" || pchar.questTemp.different == "FackWaitress_fighted")
				{
					dialog.text = "Я здесь новенькая, так что не загружайте меня работой, прошу вас. Я не освоилась еще на новом месте...";
					link.l1 = "А где официантка, что работала до тебя? " + pchar.questTemp.different.FackWaitress.Name + ", кажется...";
					link.l1.go = "Love_IDN";
				}
				if (pchar.questTemp.different == "FackWaitress_facking")
				{
					
					dialog.text = "Заходи еще, дорогой. Будет время - развлечемся еще...";
					link.l1 = "Обязательно!";
					link.l1.go = "exit";
				}
				if (pchar.questTemp.different == "FackWaitress_fackNoMoney")
				{
					
					dialog.text = "Я вас не знаю, не приставайте ко мне...";
					link.l1 = "Где мои деньги?!!";
					link.l1.go = "Love_IDN_1";
				}
				break;
			}
		    switch (Rand(4))
			{
				case 0:
					dialog.text = "Прости, красавчик, я занята. Хороший день для таверны - не всегда хороший день для разговора!";
					link.l1 = "...";
					link.l1.go = "exit";
				break;

				case 1:
					dialog.text = "Пожалуйста, капитан, не устраивайте дебошей! После них так трудно наводить порядок!";
					link.l1 = "Кхм... Вроде и не собирался.";
					link.l1.go = "exit";
				break;

				case 2:
					dialog.text = "О, капитан! Вы хотели бы попасть в мои объятья сегодня? Я не люблю хвастаться, но ...";
					link.l1 = "Увы, сейчас я спешу! Давай в другой раз!";
					link.l1.go = "exit";
					// boal 27.03.2004 -->
					if (pchar.questTemp.different == "free" && !CheckAttribute(pchar, "questTemp.different.FackWaitress") && pchar.questTemp.piratesLine != "Soukins_toPuertoPrincipe" && pchar.questTemp.piratesLine != "Soukins_seekRings")
					{
						link.l2 = "С превеликим удовольствием, крошка!";
						link.l2.go = "Love_1";
					}
					// boal 27.03.2004 <--
				break;

				case 3:
					dialog.text = "Если вы хотите что-либо заказать - обратитесь, пожалуйста, к хозяину. Он стоит за стойкой.";
					link.l1 = "Спасибо за совет.";
					link.l1.go = "exit";
				break;

				case 4:
					dialog.text = "Если вы хотите как следует отдохнуть и поправить здоровье - снимите комнату на ночь. Ночевка в общей зале вряд ли прибавит вам сил.";
					link.l1 = "Благодарю.";
					link.l1.go = "exit";
				break;
			}			
			if (CheckAttribute(pchar, "GenQuest.SeekSpy.City")) //квест мэра поп поиску шпиона
			{
				link.l4 = "Скажи мне, красавица, ты не замечала здесь никого подозрительного в последнее время?";
				if (pchar.GenQuest.SeekSpy.City == "1")
				{
					link.l4.go = "SeekSpy_Seen";
				}
				else
				{
					link.l4.go = "SeekSpy_NotSeen";
				}
			}			
			link.l9 = "Я хочу задать тебе пару вопросов.";
			link.l9.go = "quests";//(перессылка в файл города)
		break;
		
        case "Love_1":
			dialog.text = "Тогда слушай внимательно. Сними комнату у нас в таверне. Иди туда и жди меня. Я незаметно проскочу к тебе попозже...";
			link.l1 = "Хе! Все сделаю, дорогуша! Жду!";
			link.l1.go = "exit";
			pchar.questTemp.different = "FackWaitress_toRoom";
			SetTimerFunction("WaitressFack_null", 0, 0, 1); //освобождаем разрешалку на миниквесты на след. день
			//Шанс, что ограбят, если рендом выпадет на 0. он же делитель сколько она вытащит из кармана
			pchar.questTemp.different.FackWaitress.Kick = rand(2); 
			pchar.questTemp.different.FackWaitress.Name = GetFullName(npchar); //запомним имя официантки
			//делаем клона официантки
			sld = GetCharacter(NPC_GenerateCharacter("WairessQuest", npchar.model, "woman", npchar.model.animation, 5, sti(npchar.nation), 3, false));
			sld.name = npchar.name;
			sld.lastname = npchar.lastname;
			sld.dialog.Filename = "Quest\ForAll_dialog.c";
			sld.dialog.currentnode = "Waitress";	
			Pchar.quest.WaitressFack_inRoom.win_condition.l1 = "location";
			Pchar.quest.WaitressFack_inRoom.win_condition.l1.location = npchar.city + "_tavern_upstairs";
			Pchar.quest.WaitressFack_inRoom.function = "WaitressFack_inRoom";
			ChangeCharacterReputation(pchar, -1);
		break;
        case "Love_IDN":
			dialog.text = "Она здесь больше не работает, уволилась... Простите, у меня много дел.";
			link.l1 = "Хорошо...";
			link.l1.go = "exit";
		break;
        case "Love_IDN_1":
			dialog.text = "Вот что, похотливый кабель. Я не знаю, где твои деньги! А будешь приставать ко мне - вызову стражу, и ты сегодня же окажешься в тюрьме!";
			link.l1 = "Ладно, не надо стражу... Сам дурак.";
			link.l1.go = "Love_IDN_2";
		break;
        case "Love_IDN_2":
			dialog.text = "Вот именно, что сам дурак. Впредь будешь умней и порядочней.";
			link.l1 = "М-да, постараюсь...";
			link.l1.go = "exit";
		break;

		case "without_money":
			NextDiag.TempNode = "first time";
			dialog.text = "Вы очнулись?";
			link.l1 = "М-м-м.";
			link.l1.go = "without_money_2";
		break;

		case "without_money_2":
			dialog.text = "Вам плохо? Болит голова?";
			link.l1 = "Где я, чёрт возьми?";
			link.l1.go = "without_money_3";
		break;

		case "without_money_3":
			dialog.text = "Так вы ничего не помните? Даже то, как вас ограбили?";
			link.l1 = "Что? Ох... моя голова... Кто посмел?!";
			link.l1.go = "without_money_4";
		break;

		case "without_money_4":
			dialog.text = "Откуда мне знать? Пришли, пригрозили убить, обшарили карманы и ушли.";
			link.l2 = "Ладно, что пропало, то пропало.";
			link.l2.go = "exit";
		break;
		//==> eddy. квест мэра, вопросы не замечала ли шпиона
		case "SeekSpy_Seen":
			dialog.text = NPCStringReactionRepeat("Не знаю даже... Подозрительного?.. Хотя, подожди, кажется есть такой. По виду обычный горожанин, но уж больно приставучий с вопросами разными, да и не видела я его раньше в городе. Здесь ты его вряд ли застанешь, а вот в городе поищи.", 
				"Ой, ну мы же говорили про него уже! Я больше ничего сказать не могу.", "Опять то же самое... Я уже все тебе сказала.", "Сколько можно спрашивать одно и то же?! Может тебе что-нибудь другое нужно?", "block", 1, npchar, Dialog.CurrentNode);
			link.l1 = HeroStringReactionRepeat("Спасибо, тебе, красавица. Что же, пойду искать этого типа.", "Да? Ну ладно...",
                      "Хм, да уж...", "Извини, красавица...", npchar, Dialog.CurrentNode);
			link.l1.go = "exit";
		break;
		case "SeekSpy_NotSeen":
			dialog.text = NPCStringReactionRepeat("Нет, никого такого не видела, чтобы подозрительный прямо был.", 
				"Ой, ну мы же говорили про него уже! Я больше ничего сказать не могу.", "Опять то же самое... Я уже все тебе сказала.", "Сколько можно спрашивать одно и то же?! Может тебе что-нибудь другое нужно?", "block", 1, npchar, Dialog.CurrentNode);
			link.l1 = HeroStringReactionRepeat("Спасибо, тебе, красавица. Что же, пойду искать этого типа.", "Да? Ну ладно...",
                      "Хм, да уж...", "Извини, красавица...", npchar, Dialog.CurrentNode);
			link.l1.go = "exit";
		break;
	}
}
 
