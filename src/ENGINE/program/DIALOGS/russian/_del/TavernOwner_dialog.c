int iStep = -1;

void ProcessDialogEvent()
{
	ref NPChar;
	aref Link, NextDiag;

	DeleteAttribute(&Dialog,"Links");

	makeref(NPChar,CharacterRef);
	makearef(Link, Dialog.Links);
	makearef(NextDiag, NPChar.Dialog);

	int iQuestResult;

	switch(Dialog.CurrentNode)
	{
		case "First time":
			if(sStory == "cangetmap2" && sti(pchar.money) >= 1000)
			{
			//1
				dialog.text = "А, капитан Шарп! Как раз вы-то мне и нужны. Вы позвольте задать вам один вопрос?";
				link.l1 = "Задавай.";
				link.l1.go = "2";
			}
			else
			{
				if (npchar.quest.meeting == "0")
				{
					dialog.text = "Добро пожаловать в мою таверну. Чем могу быть полезен?";
					npchar.quest.meeting = "1";
				}
				else
				{
					dialog.text = "Чем могу помочь?";
				}
				link.l1 = "Какие услуги ты можешь мне предложить?";
				link.l1.go = "interface_exit";
				link.l2 = "Я ищу работу. Не поможешь мне?";
				link.l2.go = "work";
				link.l3 = "Я хочу остановиться здесь на некоторое время.";
				link.l3.go = "room";
				link.l4 = "Ничем. Я уже ухожу.";
				link.l4.go = "exit";
			}

			NextDiag.TempNode = "First time";
		break;

		case "work":
			dialog.text = "Работу, говоришь... Могу тебе посоветовать обратиться к местному торговцу - ему частенько нужны капитаны для перевозки груза\nЕще можешь посмотреть нет ли в таверне какого-нибудь купца - они часто ищут охрану для своих лоханок\nЕсли же не гнушаешься перевозкой пассжиров, то поспрашивай на улицах - возможно, что кому-нибудь надо добраться до другой колонии\nНу и напоследок могу посоветовать тебе поступить на службу к нашему губернатору. Уж он то будет снабжать тебя заданиями с завидной регулярностью.";
			link.l1 = "Спасибо, ты мне очень помог. До свидания.";
			link.l1.go = "exit";
			link.l2 = "Благодарю за помощь. Давай поговорим о чем-нибудь другом.";
			link.l2.go = "First time";
		break;

		case "interface_exit":
			DialogExit();
			NextDiag.CurrentNode =  NextDiag.TempNode;
			LaunchHireCrew();
		break;

		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;

		case "room":
			dialog.text = "На какой срок вы планируете остановиться?";
			if(CheckForNight() == true)
			{
				link.l1 = "До утра.";
				link.l1.go = "room_day";
			}
			else
			{
				link.l1 = "До ночи.";
				link.l1.go = "room_night";
			}
		break;

		case "room_day":
			dialog.text = "Это обойдется вам в 5 золотых.";
			if (makeint(pchar.money) >= 5)
			{
				link.l1 = "Идет. Вот твои деньги.";
				link.l1.go = "room_day_wait";
			}
			link.l2 = "За такие деньги я переночую в общей зале.";
			link.l2.go = "hall_day_wait";
			link.l3 = "Не сейчас. Давай поговорим о чем-нибудь другом.";
			link.l3.go = "first time";
		break;

		case "room_night":
			dialog.text = "Это обойдется вам в 5 золотых.";
			if (makeint(pchar.money) >= 5)
			{
				link.l1 = "Идет. Вот твои деньги.";
				link.l1.go = "room_night_wait";
			}
			link.l2 = "За такие деньги я переночую в общей зале.";
			link.l2.go = "hall_night_wait";
			link.l3 = "Не сейчас. Давай поговорим о чем-нибудь другом.";
			link.l3.go = "first time";
		break;

		case "room_night_wait":
			DialogExit();
			NextDiag.CurrentNode = NextDiag.TempNode;
			AddMoneyToCharacter(pchar, -5);
			AddDialogExitQuest("sleep_in_tavern");
			TavernWaitDate2("wait_night");
		break;

		case "hall_night_wait":
			DialogExit();
			NextDiag.CurrentNode = NextDiag.TempNode;
			TavernWaitDate("wait_night");
		break;

		case "room_day_wait":
			DialogExit();
			NextDiag.CurrentNode = NextDiag.TempNode;
			AddMoneyToCharacter(pchar, -5);
			AddDialogExitQuest("sleep_in_tavern");
			TavernWaitDate2("wait_day");
		break;

		case "hall_day_wait":
			DialogExit();
			NextDiag.CurrentNode = NextDiag.TempNode;
			TavernWaitDate("wait_day");
		break;

		case "2":
			dialog.text = "Что, если бы некто предложил вам купить старый кусок бумаги за возмутительную сумму? Учитывая то, что он сказал бы вам, что этот кусок бумаги - часть карты к сокровищам.";
			link.l1 = "Это зависит от того, насколько возмутительной будет сумма. Если она будет слишком велика, я могу счесть себя оскорбленным, и, следовательно, мне придется столь же возмутительно отомстить обидчику, дабы удовлетворить мою уязвленную честь...";
			link.l1.go = "3";
		break;

		case "3":
			dialog.text = "...Понимаю. Хм-м... то есть можно наверняка предположить, что вы нашли бы сумму в 900 пиастров оскорбительной?";
			link.l1 = "О, вне всяких сомнений. Вероятно, я свернул бы наглецу шею в то самое мнгновение, когда он посмел бы сделать мне такое предложение. 900 пиастров за сомнительный кусок бумаги - верх наглости, на мой взгляд.";
			link.l1.go = "4";
		break;

		case "4":
			dialog.text = "А что, если этот некто также сообщил бы вам, что на этом куске карты написано ваше имя? Разве это не возбудило бы ваш интерес?";
			link.l1 = "Такое высказывание, конечно, приковало бы мое внимание, и подвергло бы здоровье нашего уважаемого некто серьезной опасности. Наглое завышение цены на нечто мне по-настоящему нужное обычно имеет непредсказуемые, но весьма смертельные последствия для продавца.";
			link.l1.go = "5";
		break;

		case "5":
			dialog.text = "М-да... Вы здорово торгуетесь, капитан. 600 пиастров, и это мое последнее слово. Должен же я получить с этого хоть какую-то прибыль!";
			link.l1 = "Приятно встретить человека, способного разглядеть свет разума в темном океане безумия, окружающем нас. По рукам.";
			link.l1.go = "6";
			link.l2 = "Все равно слишком дорого. К тому же велика вероятность того, что сокровища, указанные на карте, давно уже найдены, или просто никогда не существовали.";
			link.l2.go = "7";
		break;

		case "6":
			dialog.text = "С вами приятно иметь дело, капитан. Уверяю, это приобретение вас не разочарует.";
			link.l1 = "Твоим способностям к вымогательству позавидует любой ростовщик. Надеюсь, карта того стоит, ради твоего же доброго здоровья.";
			link.l1.go = "SL exit map";
		break;

		case "7":
			dialog.text = "Как хотите, капитан. Но учтите, у вас вряд ли будет другая возможность приобрести карту - я знаю множество людей, которым она будет весьма интересна. Я предложил ее вам за такю смешную цену лишь потому, что на карте есть ваше имя. Но если она вам не нужна...";
			link.l1 = "Ну ладно, ладно, черт с тобой! По рукам.";
			link.l1.go = "6";
			link.l2 = "Совершенно верно, не нужна. До свидания.";
			link.l2.go = "SL exit no map";
		break;

		case "SL exit map":
			DialogExit();
			NextDiag.CurrentNode = NextDiag.TempNode;
			//give map, continue SL
			AddDialogExitQuest("get map 2");
		break;

		case "SL exit no map":
			DialogExit();
			NextDiag.CurrentNode = NextDiag.TempNode;
			//wrap up SL
			AddDialogExitQuest("refused map");
		break;
	}
}
