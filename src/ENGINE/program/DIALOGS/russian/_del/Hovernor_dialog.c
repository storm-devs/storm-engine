
void ProcessDialogEvent()
{
	ref NPChar;
	aref Link, NextDiag;

	DeleteAttribute(&Dialog,"Links");

	makeref(NPChar,CharacterRef);
	makearef(Link, Dialog.Links);
	makearef(NextDiag, NPChar.Dialog);
	
	string sAgreeLink, sDeclineLink;
	switch(Dialog.CurrentNode)
	{
		case "First time":
			dialog.text = "Представтесь, пожалуйста, я хочу знать, с кем разговариваю.";
			link.l1 = "Мое имя " + GetCharacterFullName(pchar.id) + ".";
			link.l1.go = "Second Time";
			NextDiag.TempNode = "Second Time";
		break;

		case "Second Time":
			dialog.text = SelectHovernorGreetingsDialog(npchar);
			link.l2 = "Мне просто захотелось засвидетельствовать вам свое почтение, а теперь я ухожу.";
			link.l2.go = "exit";
			///////////////////////////////////////////////////////
			/// ПРОВЕРКА ПОСТУПЛЕНИЯ НА СЛУЖБУ (НИЗШИЙ ПРИОРИТЕТ)
			///////////////////////////////////////////////////////
			if(!CheckForNationService(sti(npchar.nation)))
			{
				link.l1 = "Я хочу поступить на государственную службу.";
				link.l1.go = "service";
			}
			///////////////////////////////////////////////////////
			/// ПРОВЕРКА ЭПИДЕМИИ НА КОЛОНИИ (ВЫСШИЙ ПРИОРИТЕТ)
			///////////////////////////////////////////////////////
			if(CheckForDisease(npchar) == true && !CheckForNationService(sti(npchar.nation)))
			{
				dialog.text = "Извините меня, но на колонии свирепствует болотная лихорадка...";
				if(GetCargoGoods(pchar, GOOD_MEDICAMENT) > 10)
				{
					link.l1 = "У меня на борту есть медикаменты, может быть я смогу вам помочь?";
					link.l1.go = "disease_mission_completed";
				}
				else
				{
					link.l1 = "Это ни для кого не секрет - смердящие трупы на улицах красноречивее любых слов.";
					link.l1.go = "disease_mission_2";
				}
			}
			///////////////////////////////////////////////////////
			/// ПРОВЕРКА НА ВЫПОЛНЕННЫЕ КВЕСТЫ
			///////////////////////////////////////////////////////
			if(CheckAttribute(npchar, "quest.questflag"))
			{
				if(sti(npchar.quest.questflag) == 1)//взять квест
				{
					link.l2 = "Нет ли у вас для меня какого-нибудь задания?";
					link.l2.go = "taking_quest_line";
				}
				if(sti(npchar.quest.questflag) == 2)//сдать квест
				{
					link.l2 = "Ваше задание выполненно..";
					link.l2.go = "check_quests_lines";
				}
				if(sti(npchar.quest.questflag) == 3)//вспомнить квест
				{
					link.l2 = "Я хочу поговорить с вами о задании, которое вы мне поручали.";
					link.l2.go = "reminder_quests_lines";
				}
			}
			NextDiag.TempNode = "Second Time";
		break;

		///////////////////////////////////////////////////////
		/// КВЕСТ НА ДОСТАВКУ МЕДИКАМЕНТОВ
		///////////////////////////////////////////////////////
		case "disease_mission_2":
			dialog.text = "Увы... А запасы медикаментов подошли к концу... Что вы скажете на то, что я попрошу вас доставить на остров груз медикаментов?";
			link.l1 = "Я скажу, что с радостью возьмусь за столь благородную миссию.";
			link.l1.go = "disease_mission_accept";
			link.l2 = "Это не мои проблемы, так что разбирайтесь сами, как можете.";
			link.l2.go = "disease_mission_refused";
		break;

		case "disease_mission_accept":
			dialog.text = "Слава богу! Поверьте, я не забуду оказанной услуги.";
			if (GetSquadronGoods(pchar, GOOD_MEDICAMENT) >= 10)
			{
				link.l1 = "Не стоит, тем более, что у меня на борту уже есть необходимое вам количество медикаментов, и я с радостью пожертвую их вам.";
				link.l1.go = "disease_mission_completed";
			}
			else
			{
				link.l1 = "Я не сомневаюсь в этом. А теперь позвольте откланяться, я должен выполнить вашу просьбу.";
				link.l1.go = "exit";
				DiseaseMissionInProgress(npchar);
			}
		break;

		case "disease_mission_refused":
			dialog.text = "Не буду вас винить в этом. Что же прощайте.";
			link.l1 = "Прощайте.";
			link.l1.go = "exit";
		break;

		case "disease_mission_completed":
			if (CheckForDisease(npchar))
			{
				dialog.text = "Вы просто ангел! Позвольте вручить вам эту сумму, в знак признательность жителей нашей колонии.";
				link.l1 = "Спасибо.";
				link.l1.go = "exit";
				DiseaseMissionCompleted(npchar, 0);
			}
			else
			{
				dialog.text = "Я выражаю вам признательность, но мы смогли победить болезнь своими силами.";
				link.l1 = "Что же, на нат и суда нет.";
				link.l1.go = "exit";
				DiseaseMissionCompleted(npchar, 1);
			}
		break;

		///////////////////////////////////////////////////////
		/// ВЗЯТИЕ КВЕСТА
		///////////////////////////////////////////////////////
		case "taking_quest_line":
			dialog.text = SelectHovernorQuestText(npchar, &sAgreeLink, &sDeclineLink);
			link.l1 = "Интересное предложение. Я берусь за это задание.";
			link.l1.go = sAgreeLink;
			link.l2 = "Боюсь мне это не подходит.";
			link.l2.go = sDeclineLink;
		break;

		///////////////////////////////////////////////////////
		/// КВЕСТ НА ЗАХВАТ ФОРТА
		///////////////////////////////////////////////////////
		case "capture_fort_accept":
			dialog.text = "Да поможет вам Бог.";
			link.l1 = "Спасибо.";
			link.l1.go = "exit";
			AcceptCaptureFortQuest(npchar);
		break;

		case "capture_fort_decline":
			dialog.text = "Тогда мне придется вас разочаровать - других заданий у меня нет.";
			link.l1 = "Хорошо, в таком случае я зайду попозже..";
			link.l1.go = "exit";
		break;

		///////////////////////////////////////////////////////
		/// КВЕСТ НА ДОСТАВКУ ПРЕДМЕТА
		///////////////////////////////////////////////////////
		case "delivery_item_accept":
			dialog.text = "Тогда я пожелаю вам удачного плавания.";
			link.l1 = "Спасибо.";
			link.l1.go = "exit";
			AcceptDeliveryItemQuest(npchar);
		break;

		case "delivery_item_decline":
			dialog.text = "Тогда мне придется вас разочаровать - других заданий у меня нет.";
			link.l1 = "Хорошо, в таком случае я зайду попозже..";
			link.l1.go = "exit";
		break;

		///////////////////////////////////////////////////////
		/// КВЕСТ НА СОПРОВОЖДЕНИЕ СУДНА
		///////////////////////////////////////////////////////
		case "escort_ship_accept":
			dialog.text = "Тогда я пожелаю вам удачного плавания.";
			link.l1 = "Спасибо.";
			link.l1.go = "exit";
			AcceptEscortQuest(npchar);
		break;

		case "escort_ship_decline":
			dialog.text = "Тогда мне придется вас разочаровать - других заданий у меня нет.";
			link.l1 = "Хорошо, в таком случае я зайду попозже..";
			link.l1.go = "exit";
		break;
		
		///////////////////////////////////////////////////////
		/// КВЕСТ НА ПОТОПЛЕНИЕ/ЗАХВАТ СУДНА
		///////////////////////////////////////////////////////
		case "sink_ship_accept":
			dialog.text = "Тогда я пожелаю вам удачного плавания.";
			link.l1 = "Спасибо.";
			link.l1.go = "exit";
			AcceptSinkShipQuest(npchar);
		break;

		case "sink_ship_decline":
			dialog.text = "Тогда мне придется вас разочаровать - других заданий у меня нет.";
			link.l1 = "Хорошо, в таком случае я зайду попозже..";
			link.l1.go = "exit";
		break;

		///////////////////////////////////////////////////////
		/// ПАССАЖИРСКИЙ КВЕСТ
		///////////////////////////////////////////////////////
		case "delivery_passenger_accept":
			dialog.text = "Тогда я пожелаю вам удачного плавания.";
			link.l1 = "Спасибо.";
			link.l1.go = "exit";
			AcceptDeliveryPassengerQuest(npchar);
		break;

		case "delivery_passenger_decline":
			dialog.text = "Тогда мне придется вас разочаровать - других заданий у меня нет.";
			link.l1 = "Хорошо, в таком случае я зайду попозже..";
			link.l1.go = "exit";
			DeletePassengerQuest(npchar);
		break;

		///////////////////////////////////////////////////////
		/// ТОРГОВЫЙ КВЕСТ
		///////////////////////////////////////////////////////
		case "delivery_trade_accept":
			dialog.text = "Тогда я пожелаю вам удачного плавания.";
			link.l1 = "Спасибо.";
			link.l1.go = "exit";
			AcceptDeliveryTradeQuest(npchar);
		break;

		case "delivery_trade_decline":
			dialog.text = "Тогда мне придется вас разочаровать - других заданий у меня нет.";
			link.l1 = "Хорошо, в таком случае я зайду попозже..";
			link.l1.go = "exit";
		break;

		///////////////////////////////////////////////////////
		/// ПРОВЕРКА ВЫПОЛНЕНИЯ КВЕСТОВ
		///////////////////////////////////////////////////////
		case "check_quests_lines":
			if(CheckAttribute(npchar, "quest.punishment"))
			{
				if(sti(npchar.quest.punishment) > 0)
				{
					dialog.text = "Что же, хотя вы и задержались на " + FindRussianDaysString(sti(npchar.quest.punishment)) + ", в связи с чем я вынужден заплатить вам меньшую сумму, в целом я доволен результатом.";
					link.l1 = "Всегда к вашим услугам.";
					link.l1.go = "quest_done_prepare_exit";
				}
				else
				{
					dialog.text = "Я рад, что вы успешно выполнили возложенное на вас поручение, и выполнили его вовремя. Позвольте поблагодарить вас за хорошую работу.";
					link.l1 = "Благодарю вас за столь высокую оценку моих действий.";
					link.l1.go = "quest_done_prepare_exit";
				}
			}
			else
			{
				dialog.text = "Я рад, что вы успешно выполнили возложенное на вас поручение, и выполнили его вовремя. Позвольте поблагодарить вас за хорошую работу.";
				link.l1 = "Благодарю вас за столь высокую оценку моих действий.";
				link.l1.go = "quest_done_prepare_exit";
			}
		break;

		case "reminder_quests_lines":
			dialog.text = SelectHovernorQuestReminderText(npchar);
			link.l1 = "Я все еще работаю над этим.";
			link.l1.go = "exit";
		break;

		///////////////////////////////////////////////////////
		/// ПОСТУПЛЕНИЕ НА СЛУЖБУ
		///////////////////////////////////////////////////////
		case "service":
			if (CheckAttribute(pchar, GetNationNameByType(sti(npchar.nation)) + "_line_counter"))
			{
				dialog.text = "Если мне не изменяет память, вы уже получали патент моей державы\nНо его анулировали, после того, как вы были замечены в двоерушничестве.";
				link.l1 = "Хм... Очень жаль, тогда прошу извинить меня я уже ухожу.";
				link.l1.go = "exit";
			}
			else
			{
				dialog.text = "Отлично! Мы нуждаемся в таких людях как вы.";
				link.l1 = "Какие бумаги мне необходимо подписать, чтобы утрясти формальности?";
				link.l1.go = "service_2";
			}
		break;
		
		case "service_2":
			dialog.text = "Хорошо подумайте, пережде чем ставить свою подпись. Ибо если после получения этого патента, вы решите сменить хозяина, то потеряете мое расположение, и расположение моей страны.";
			link.l1 = "Все уже обдумано. Давайте бумаги.";
			link.l1.go = "service_3";
			link.l2 = "О! Думаю, мне необходимо время, чтобы принять окончательное решение.";
			link.l2.go = "exit";
		break;
		
		case "service_3":
			dialog.text = "Мне нравится ваша решительность! Теперь вам осталось доказать свою преданость на деле\nтак что, когда вы решите, что патент вам нужен не только для украшения каюты, зайдите ко мне и, возможно, у меня найдется для вас дело.";
			link.l1 = "Конечно.";
			link.l1.go = "service_exit";
		break;

		case "service_exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
			PlayerChangeNation(npchar);
		break;

		case "quest_done_prepare_exit":
			CharacterDoneQuest(npchar);
			if(CheckForNextRank(npchar) > 0)
			{
				dialog.text = "Позвольте поздравить вас с очередным званием, капитан!";
				link.l1 = "Благодарю вас за оказаное доверие.";
				link.l1.go = "quest_done_exit";
				PlayerNextRank(npchar);
			}
			else
			{
				NextDiag.CurrentNode = NextDiag.TempNode;
				DialogExit();
			}
		break;

		case "quest_done_exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
			//CharacterDoneQuest(npchar);
		break;
		
		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;		
	}
}
