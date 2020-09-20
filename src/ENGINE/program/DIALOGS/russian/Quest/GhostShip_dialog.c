
void ProcessDialogEvent()
{
	ref NPChar;
	aref Link, NextDiag;
	string sTemp;
	bool bOk;

	DeleteAttribute(&Dialog,"Links");

	makeref(NPChar,CharacterRef);
	makearef(Link, Dialog.Links);
	makearef(NextDiag, NPChar.Dialog);
	
	switch(Dialog.CurrentNode)
	{
        case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
		
		case "First time":
			dialog.text = "Приветствую тебя, брат!";
			link.l1 = "Какой я тебе брат? Ты же явно полукровка-индеец. И что ты в таких лохмотьях?";
			link.l1.go = "Meeting_1";
			NextDiag.TempNode = "First time";
		break;

        case "Meeting_1":
			dialog.text = "Моя ошибся? Ты белый человек, я белый, мое имя даже 'Белый Мальчик'. Одежда - память о предках, я носить ее, как смочь одеть, давно.";
			link.l1 = "Ну да, не девочка точно, только лет тебе явно уже за мальчика перевалило, а ума так и нет. Зачем всю жизнь ходить в лохмотьях?";
			link.l1.go = "Meeting_2";
		break;
		
		case "Meeting_2":
			dialog.text = "Твоя не понимай. Я сын дочери главный шаман и белый капитан большой корабль. Я носить так, чтоб духи не видеть меня и не забрать к себе.";
			link.l1 = "Вообщем, очередной юродивый, ясно все. Ты продаешь жемчуг?";
			link.l1.go = "Meeting_3";
		break;
		
		case "Meeting_3":
			dialog.text = "Жемчуг я покупать.";
			link.l1 = "Покупать или продвавать? У тебя есть?";
			link.l1.go = "Meeting_4";
		break;
		
		case "Meeting_4":
			dialog.text = "Нет, сейчас нет, а нужно.";
			link.l1 = "Запутал ты меня, точно юродивый. Прощай.";
			link.l1.go = "Exit";
			NextDiag.TempNode = "AfterMeet";
			ReOpenQuestHeader("GhostShipQuest");
        	AddQuestRecord("GhostShipQuest", "4");
		break;
		
		case "AfterMeet":
			dialog.text = "Белый брат вернулся. Есть что сказать?";
			link.l1 = "Нет, нам не о чем говорить.";
			link.l1.go = "Exit";
			NextDiag.TempNode = "AfterMeet";
			bOk = (sti(PChar.GenQuest.GhostShip.KillMe) > 0) || (sti(PChar.GenQuest.GhostShip.DeadByMe) > 0);
			if (bOk && !CheckAttribute(pchar , "GenQuest.GhostShip.NeedCoins"))
            {
    			Link.l3 = "Что ты знаешь о корабле-призраке?";
    			Link.l3.go = "GhostCapt";
			}
			if (GetCharacterItem(Pchar, "Coins") >= 666 && CheckAttribute(pchar , "GenQuest.GhostShip.NeedCoins"))
			{
                Link.l4 = "Я принес твои жемчужины.";
    			Link.l4.go = "quest_coin_2";
			}
		break;
		
		// корабль
		case "GhostCapt":
			NextDiag.TempNode = "AfterMeet";
			dialog.Text = "Большой корабль? Плавать долго-долго вокруг, раньше быть простым, потом стать призрак с чёрым парусом.";
			Link.l1 = "Когда и как он появился?";
			Link.l1.go = "GhostCapt_1";
		break;
		
		case "GhostCapt_1":
			dialog.Text = "Ошибка, а может злой дух капитана посетил. Они были тут 40 зим назад, жили и торговали в мире. Капитан сильно обидеть племя, взять мою мать силой, разграбить нас, похитить священный черный жемчуг.";
			Link.l1 = "Хм... Нормальное дело, корабль-призрак как возник?";
			Link.l1.go = "GhostCapt_2";
		break;
		
		case "GhostCapt_2":
			dialog.Text = "Я все объяснить уже. Ты не слушать? Наш вождь потребовал жизнь капитана в знак примерения, матросы поддержали капитана и корабль уплыть далеко, далеко. Тогда шаман нашего народа проклял капитана и корабль.";
			Link.l1 = "Проклятие казалось мне чем-то плохим. Как они обрели могущество и бессмертие?";
			Link.l1.go = "GhostCapt_3";
		break;
		
		case "GhostCapt_3":
			dialog.Text = "Они и страдать, сильно страдать. Получаться все не так, могущество священных жемчужин защищать их, они будут мучаться, пока не вернётся всё обратно. Я стоять здесь с рождения и собирать обратно наш жемчуг, менять на белый, они продавать, я искать.";
			Link.l1 = "По-моему страдают от этого проклятия больше другие люди. Порезали бы им глотки ночью и всех дел. Как снять с них бессмертие?";
			Link.l1.go = "GhostCapt_4";
		break;
		
		case "GhostCapt_4":
			dialog.Text = "Осталось мало, они уже почти все разбазарили, я собрать все, что было, осталось совсем мало, нужно 666 черных жемчужин с их корабля. Тогда проклятие обернется правильной стороной.";
			Link.l1 = "Хорошо, жди меня и я вернусь!";
			Link.l1.go = "Exit";
			AddQuestRecord("GhostShipQuest", "5");
			PChar.GenQuest.GhostShip.NeedCoins = true;
		break;
		
		case "quest_coin_2":
            dialog.Text = "Замечательно. Ты спасти всех нас! Скорее давай их. Тебе остается только еще раз встретить корабль и сломать его или взять боем. На этом их пути конец. Теперь они стать просто пираты, без черного паруса.";
			Link.l1 = "Хорошо. Забирай их.";
			Link.l1.go = "Exit";
			TakeNItems(pchar, "Coins", -666);
			AddQuestRecord("GhostShipQuest", "6");
			PChar.GenQuest.GhostShip.LastBattle = true;
			NPChar.location = "none";
			NPChar.LifeDay = 0;
            NextDiag.TempNode = "temp_coins";
		break;
		
		case "temp_coins":
            dialog.Text = "Спасибо, друг.";
			Link.l1 = "Надеюсь, все будет, как ты обещал.";
			Link.l1.go = "Exit";
			NextDiag.TempNode = "temp_coins";
		break;
	}
}