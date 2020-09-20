
void ProcessDialogEvent()
{
	ref NPChar;
	aref Link, NextDiag;

	DeleteAttribute(&Dialog,"Links");

	makeref(NPChar,CharacterRef);
	makearef(Link, Dialog.Links);
	makearef(NextDiag, NPChar.Dialog);

	switch(Dialog.CurrentNode)
	{
		case "First time":
			dialog.text = "Не беспокойтесь за меня. Я поднимусь на борт вашего корабля вовремя.";
			link.l1 = "Хорошо.";
			link.l1.go = "exit";

			NextDiag.TempNode = "First time";
		break;

		case "Second time":
			dialog.text = "Не беспокойтесь за меня. Я поднимусь на борт своего корабля вовремя.";
			link.l1 = "Хорошо.";
			link.l1.go = "exit";

			NextDiag.TempNode = "Second time";
		break;

		case "Successful_Travel":
			dialog.text = "Спасибо вам за приятное путешествие.";
			link.l1 = "Удачи.";
			link.l1.go = "succesful_travel_exit";
			NextDiag.TempNode = "First time";
		break;

		case "Failed_Travel":
			dialog.text = "Мы опаздали на " + FindRussianDaysString(sti(Quests[sti(npchar.CurrentQuest)].l.contribution)) + "! Поэтому я просто вынужден удержать часть причитающейся вам оплаты!";
			link.l1 = "Как хочешь, не такие уж это и большие деньги.";
			link.l1.go = "succesful_travel_exit";
			NextDiag.TempNode = "First time";
		break;

		case "Passenger_hear":
			dialog.text = "Извините за беспокойство, но мне послышалось, что вы спрашивали о работе, и думаю я могу предложить вам кое-что.";
			link.l1 = "Вы правы, я действительно ищу небольшую работенку.";
			link.l1.go = "Passenger_hear_2";
			link.l2 = "Вы ошиблись, я не ищу никакой работы.";
			link.l2.go = "Passenger_hear_denied";
		break;

		case "Passenger_hear_2":
			dialog.text = "Как удачно! Видите ли мне необходимо попасть на колонию " + XI_ConvertString("Colony"+Quests[sti(npchar.CurrentQuest)].l.target_colony) + ", которая находится на острове " + XI_ConvertString(colonies[FindColony(Quests[sti(npchar.CurrentQuest)].l.target_colony)].island) + ", за " +FindRussianDaysString(Quests[sti(npchar.CurrentQuest)].l.time) +", и за это я заплачу вам " + FindRussianMoneyString(Quests[sti(npchar.CurrentQuest)].l.money) + ". Что скажете?";
			link.l1 = "Думаю, мы договорились.";
			link.l1.go = "Passenger_hear_accept";
			link.l2 = "Не думаю, что меня это устраивает.";
			link.l2.go = "Passenger_hear_denied";
		break;

		case "Passenger_hear_accept":
			dialog.text = "Рад, что мы договорились, капитан.";
			link.l1 = "Да, да. У меня тут есть еще кое-какие дела, а вам я посоветую быстрее найти мое судно и подняться на борт.";
			link.l1.go = "pass_accept_exit";
		break;

		case "Passenger_hear_denied":
			dialog.text = "О, прошу извинить меня. Прощайте.";
			link.l1 = "Да нет проблем, с кем не бывает.";
			link.l1.go = "pass_decline_exit";
		break;

		case "Passenger_delayed":
			dialog.text = "О, вот и вы, наконец. Я давно вас жду.";
			link.l1 = "Тогда попрошу вас подняться на борт моего корабля, мы отправляемся как только я закончу свои дела в этой колонии.";
			link.l1.go = "pass_accept_exit";
		break;

		case "Convoy_delayed":
			NextDiag.TempNode = "Convoy_delayed";
			dialog.text = "Я жду вас уже несколько дней, и уже начал волноваться! Когда мы сможем отправиться в путь?";
			if(CheckForConvoyFreeStack() == 1)
			{
				link.l1 = "В любое удобное для вас время. Поднимайтесь на борт своего судна. Как только я закончу свои дела, мы поднимаем паруса.";
				link.l1.go = "convoy_accept_exit";
			}
			else
			{
				link.l2 = "У меня в эскадре сейчас нет места для вашего корабля, но как только я что-нибудь придумаю, я тут же к вам подойду.";
				link.l2.go = "convoy_delay_exit";
			}
		break;

		case "Convoy_completed":
			dialog.text = "Спасибо за приятное путешествие.";
			link.l1 = "Не за что.";
			link.l1.go = "convoy_successful_exit";
		break;

		case "convoy_accept_exit":
			NextDiag.TempNode = "Second time";
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
			AcceptEscortQuest(npchar);
		break;

		case "convoy_delay_exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
			DelayEscortQuest(npchar);
		break;

		case "pass_accept_exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
			AcceptDeliveryPassengerQuest(npchar);
		break;

		case "pass_decline_exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
			DeletePassengerQuest(npchar);
		break;

		case "succesful_travel_exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
			CompletePassengerQuest(npchar);
		break;

		case "convoy_successful_exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
			HalfCompleteEscortTradeShipQuest(npchar);
		break;

		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}
}
