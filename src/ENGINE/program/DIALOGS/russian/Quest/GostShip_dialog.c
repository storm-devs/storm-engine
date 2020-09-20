
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
			dialog.text = "Моя ошибся? Ты белый человек, я белый, мое имя даже 'Белый Мальчик'. Одежда - память о предках, я ность ее, как смочь одеть, давно.";
			link.l1 = "Ну да, не девочка точно, только лет тебе явно уже за мальчика перевалило, а ума так и нет. Зачем всю жизнь ходить в лохмотьях?";
			link.l1.go = "Meeting_2";
		break;
		
		case "Meeting_2":
			dialog.text = "Твоя не понимай. Я сын дочери главный шаман и белый капитан большой корабль. Я носить так, что б духи не видеть меня и не забрать к себе.";
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
			ReOpenQuestHeader("GostShipQuest");
        	AddQuestRecord("GostShipQuest", "4");
		break;
		
		case "AfterMeet":
			dialog.text = "Белый брат вернулся. Есть что сказать?";
			link.l1 = "Нет, я ошибся.";
			link.l1.go = "Exit";
			NextDiag.TempNode = "AfterMeet";
		break;
		
		// корабль
		case "GostCapt":
			dialog.text = "";
			link.l1 = "";
			link.l1.go = "Exit";
			NextDiag.TempNode = "GostCapt";
		break;
	}
}