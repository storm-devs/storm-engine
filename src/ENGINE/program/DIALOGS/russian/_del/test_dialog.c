
void ProcessDialogEvent()
{
	ref NPChar;
	aref Link, NextDiag;

	DeleteAttribute(&Dialog,"Links");

	makeref(NPChar,CharacterRef);
	makearef(Link, Dialog.Links);
	makearef(NextDiag, NPChar.Dialog);

	int iQuest;

	switch(Dialog.CurrentNode)
	{
		case "First time":
			dialog.text = "Представьтесь, я хочу знать с кем разговариваю.";
			link.l1 = "Мое имя " + pchar.name + "...";
			if (sti(pchar.nation) != PIRATE)
			{
				link.l1.go = "Second time";
			}
			else
			{
				link.l1.go = "you_are_pirate";
			}

			NextDiag.TempNode = "Second time";
		break;

		case "Second time":
			if (sti(pchar.nation) != PIRATE)
			{
				dialog.text = "Чем обязан?";
				link.l1 = "Я хочу поговорить с вами о делах.";
				link.l1.go = "quests";
				link.l2 = "О, я уже ухожу.";
				link.l2.go = "exit";
			}
			else
			{
				dialog.text = "Что вам еще нужно? Я же уже просил вас не появляться более в моем кабинете!";
				link.l1 = "Вижу, вы все так же нетерпимы к пиратам. Прощайте.";
				link.l1.go = "exit";
			}

			NextDiag.TempNode = "Second time";
		break;

		case "you_are_pirate":
			dialog.text = "Да, я слышал о вас, грязный пират. А теперь убирайтесь прочь, я не хочу иметь с вами никаких дел.";
			link.l1 = "Как вам будет угодно.";
			link.l1.go = "exit";
		break;

		case "quests":
			dialog.text = "Какие дела вас интересуют?";
			if(!CheckForNationService(sti(npchar.nation)))
			{
				link.l1 = "Я хочу поступить на государственную службу.";
				link.l1.go = "service";
			}
			else
			{
				link.l1 = "Меня интересует информация о заданиях.";
				link.l1.go = "quests_completed";
			}
			link.l99 = "Никакие.";
			link.l99.go = "exit";
		break;

		case "quests_completed":
			dialog.text = "Что именно, вы хотите узнать?";
			link.l1 = "Нет ли у вас какого-либо задания для меня?";
			link.l1.go = "ship_mission";

			///////////////////////////////////////////////////////////////
			/// ПРИЕМ КВЕСТА НА ДОСТАВКУ МЕДИКАМЕНТОВ
			///////////////////////////////////////////////////////////////
			if(CheckAttribute(npchar, "quest.disease"))
			{
				if(npchar.quest.disease == "inprogress" && GetSquadronGoods(pchar, GOOD_MEDICAMENT) >= 10)
				{
					link.l1 = "В трюме мое судна находятся необходимые вам медикаменты";
					link.l1.go = "disease_mission_completed";
				}
			}

			///////////////////////////////////////////////////////////////
			/// ПРИЕМ КВЕСТА НА ПОХИЩЕНИЕ ПРЕДМЕТА
			///////////////////////////////////////////////////////////////
			if (CheckAttribute(npchar, "quest.stealitem") && CheckQuestAttribute("steal_item", "stealed"))
			{
				if (CheckCharacterItem(pchar, npchar.quest.stealitem.item))
				{
					link.l1 = "Ваше задание выполненно.";
					link.l1.go = "steal_item_completed";
				}
				else
				{
					link.l1 = "Увы, мне не удалось выполнить ваше задание.";
					link.l1.go = "steal_item_failed";
				}
			}

			///////////////////////////////////////////////////////////////
			/// ПРИЕМ КВЕСТА НА УБИЙСТВО
			///////////////////////////////////////////////////////////////
			if (CheckAttribute(npchar, "quest.killcharacter"))
			{
				if (CheckCharacterItem(pchar, npchar.quest.killcharacter.item))
				{
					link.l1 = "Ваше задание выполненно.";
					if (npchar.quest.sinkship.condition == "completed" || sti(pchar.skill.luck) > rand(13))
					{
						link.l1.go = "kill_character_mission_complete";
					}
					else
					{
						link.l1.go = "kill_character_mission_failed";
					}
				}
			}

			///////////////////////////////////////////////////////////////
			/// ПРИЕМ КВЕСТА НА ПОТОПЛЕНИЕ КОРАБЛЯ
			///////////////////////////////////////////////////////////////
			if (CheckAttribute(npchar, "quest.sinkship"))
			{
				if (npchar.quest.sinkship.condition == "completed")
				{
					link.l1 = "Ваше задание выполненно.";
					link.l1.go = "sink_ship_mission_complete";
				}
			}

			///////////////////////////////////////////////////////////////
			/// ПРИЕМ КВЕСТА НА ЗАХВАТ ФОРТА
			///////////////////////////////////////////////////////////////
			if (CheckAttribute(npchar, "quest.fort"))
			{
				if (sti(Islands[findisland(npchar.quest.fort)].nation) == sti(npchar.nation))
				{
					link.l1 = "Остров " + GlobalStringConvert(npchar.quest.fort) + " захвачен, и над его башнями развивается французское знамя.";
					link.l1.go = "fort_mission_complete";
				}
			}

			///////////////////////////////////////////////////////////////
			/// ПРИЕМ КВЕСТА НА ЗАХВАТ КОРАБЛЯ
			///////////////////////////////////////////////////////////////
			if (CheckAttribute(npchar, "quest.captureship"))
			{
				if (npchar.quest.captureship.condition == "completed")
				{
					link.l1 = "Ваше задание выполненно.";
					link.l1.go = "ship_mission_complete";
				}
				if (npchar.quest.captureship.condition == "failed")
				{
					link.l1 = "Увы, мне не удалось вернуть вам бумаги, ибо судно похитителя пошло ко дну...";
					link.l1.go = "ship_mission_not_complete";
				}
			}

			///////////////////////////////////////////////////////////////
			/// ПРИЕМ КВЕСТА НА ЗАЩИТУ КОЛОНИИ
			///////////////////////////////////////////////////////////////
			if (CheckAttribute(npchar, "quest.defendcolony"))
			{
				if (sti(Islands[findisland(npchar.quest.defendcolony.colony)].nation) == sti(npchar.nation) && pchar.quest.defendcolony_progress == "completed")
				{
					dialog.text = "Рад слышать о вашем успехе! Позвольте поздравить вас с внеочередным званием!";
					link.l1 = "Благодарю вас.";
					link.l1.go = "exit";
					FortDefendMissionCompleted(sti(npchar.nation), npchar);
				}
				if (sti(Islands[findisland(npchar.quest.defendcolony.colony)].nation) != sti(npchar.nation) && pchar.quest.defendcolony.progress == "completed")
				{
					dialog.text = "Вы не справились с поставленой задачей, и наши позиции в архипелаге заметно ослабли! А теперь оставьте меня, мне необходимо осмыслить ситуацию.";
					link.l1 = "Как вам будет угодно. Но, обещаю. больше я не подведу вас.";
					link.l1.go = "exit";
					FortDefendMissionNotCompleted(sti(npchar.nation), npchar);
				}
			}
			link.l99 = "Ничего, мьсе, оказывается, я располагаю всей необходимой информацией.";
			link.l99.go = "exit";
		break;

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
			link.l1.go = "exit";
			PlayerChangeNation(sti(npchar.nation));
		break;

		case "service_line_quests":
			if(CheckQuestAttribute("work", "0"))
			{
			}
			else
			{
				dialog.text = "Я уже дал вам задание, и, наскольком не известно, вы его еще не выполнили.";
				link.l1 = "Хм... Действительно.";
				link.l1.go = "exit";
			}
		break;

		case "generate_quest":
			if(CheckQuestAttribute("work", "0"))
			{
				dialog.text = "Так, так, дайте подумать, какое дело у нас не требует отлагательств...";
				link.l1 = "...";
				iQuest = rand(3)+1;
				switch (iQuest)
				{
					case 1:
						link.l1.go = "ship_mission";
					break;

					case 2:
						link.l1.go = "ship_sink_mission";
					break;

					case 3:
						link.l1.go = "fort_mission";
					break;

					case 4:
						link.l1.go = "delivery_passenger";
					break;

				}
				if (CheckForDisease())
				{
					link.l1.go = "disease_mission";
				}
				if(CheckForDefendColony(npchar))
				{
					link.l1.go = "defend_colony_mission";
				}
			}
			else
			{
				dialog.text = "Я уже дал вам задание, и, насколько мне известно, вы его еще не выполнили.";
				link.l1 = "Хм... Действительно.";
				link.l1.go = "exit";
			}
		break;

		case "fort_mission":
			if(GenerateFortQuest(sti(npchar.nation), npchar))
			{
				dialog.text = "Вы доказали свою преданость Франции, и поэтому я хочу поручить вам нечто, действительно достойное вас.";
				link.l1 = "Я внимательно слушаю.";
				link.l1.go = "fort_mission_2";
			}
			else
			{
				dialog.text = "Упс... у нас тут бага... В квесте с фортом.";
				link.l1 = "Что? Опять?!";
				link.l1.go = "exit";
			}
		break;

		case "fort_mission_2":
			dialog.text = "Я хочу, чтобы вы захватили остров " + GlobalStringConvert(npchar.quest.fort) + ", и подняли над ним французское знамя.";
			link.l1 = "Действительно, задание не из легких, но я справлюсь.";
			link.l1.go = "exit";
		break;

		case "fort_mission_complete":
			dialog.text = "Браво! Не ожидал, что и это задание окажется вам по зубам. Но вот ваша награда, и, рад поздравить вас с внеочередным повышением звания.";
			link.l1 = "Благодарю вас.";
			link.l1.go = "exit";
			ShipCaptureFortCompleted(sti(npchar.nation), npchar);
		break;

		case "ship_mission":
			iQuest = rand(1);
			if(GenerateCaptureShipQuest(sti(npchar.nation), npchar, iQuest))
			{
				switch(iQuest)
				{
					case 0:
						dialog.text = "Дело в том, что у меня были похищены некоторые бумаги, и я подозреваю, что это сделал некто " + npchar.quest.captureship + "\nВаша задача вернуть мне эти бумаги. В последний раз судно похитителя видели у острова " + GlobalStringConvert(npchar.quest.captureship.island) + ".";
					break;

					case 1:
						dialog.text = "Мне стало известно, что некто " + npchar.quest.captureship + " везет из " + XI_ConvertString("From " + GetNationNameByType(sti(npchar.quest.captureship.nation)))+ " на " + GlobalStringConvert(npchar.quest.captureship.island) + " важные документы. Ваша задача прехватить его судно, и захватить бумаги.";
					break;
				}

				link.l1 = "Я вернусь так быстро, как смогу.";
				link.l1.go = "exit";
				link.l2 = "Извините, о сейчас я не могу взяться за это дело.";
				link.l2.go = "exit";
			}
			else
			{
				dialog.text = "Упс... у нас тут бага... В квесте с захватом судна.";
				link.l1 = "Что? Опять?!";
				link.l1.go = "exit";
			}
		break;

		case "ship_mission_complete":
			dialog.text = "Браво! Вы оправдываете мои ожидания. Но перейдем от слов к делу: вот ваша награда, а заодно позвольте поздравить вас с внеочередным повышением по службе.";
			link.l1 = "Благодарю вас.";
			link.l1.go = "exit";
			ShipCaptureMissionCompleted(sti(npchar.nation), npchar);
		break;

		case "ship_mission_not_complete":
			dialog.text = "Прискорбно слышать это, ведь я связывал с вами определенные надежды. А теперь оставьте меня.";
			link.l1 = "Как вам будет угодно. Надеюсь, следующее поручение я выполню с честью.";
			link.l1.go = "exit";
			ShipCaptureMissionFailed(npchar);
		break;

		case "defend_colony_mission":
			iQuest = rand(3);
			if (iQuest<3)
			{
				dialog.text = "Как вы вовремя! Мне только что сообщили, что " + XI_ConvertString(GetNationNameByType(sti(npchar.quest.defendcolony.nation))) + " напала на французскую колонию " + XI_ConvertString(npchar.quest.defendcolony.colony) + ". К сожалению, у меня нет сейчас свободных военных кораблей, так что защита колонии ложится на ваши плечи.";
				pchar.quest.defencolony.type = "single";
			}
			else
			{
				dialog.text = "Вы вовремя! Мы как раз готовимся к военному рейду. Дело в том, что "  + XI_ConvertString(GetNationNameByType(sti(npchar.quest.defendcolony.nation))) + " напала на французскую колонию " + XI_ConvertString(npchar.quest.defendcolony.colony) + ", и я собираюсь включить вас в состав экспедиции.";
				pchar.quest.defencolony.type = "multi";
			}
			link.l1 = "Я вернусь, сразу после того, как колония будет освобождена.";
			link.l1.go = "defend_colony_mission_accept";
			link.l2 = "Увы, это задание мне не по силам.";
			link.l2.go = "defend_colony_mission_refuse";
		break;

		case "defend_colony_mission_accept":
			dialog.text = "Отлично! И да благословит вас Господь.";
			link.l1 = "Спасибо.";
			link.l1.go = "exit";
			DefendColonyMissionAccept(npchar);
		break;

		case "defend_colony_mission_refuse":
			dialog.text = "Очень жаль.";
			link.l1 = "Поверьте, мне тоже.";
			link.l1.go = "exit";
			DefendColonyMissionRefuse(npchar);
		break;

		case "ship_sink_mission":
			if (GenerateShipSinkQuest(sti(npchar.nation), npchar))
			{
				dialog.text = "В последнее время все больше и большее недовольство моего правительства вызывает " + npchar.quest.sinkship + ", занимающийся разбоем в наших водах, прикрываясь флагом " + XI_ConvertString("From " + GetNationNameByType(sti(npchar.quest.sinkship.nation)))+ ". Как вы понимаете именно вам я поручаю миссию по уничтожению этого негодяя, который как раз отправился на " + GlobalStringConvert(npchar.quest.sinkship.island) + " чтобы сбыть награбленное.";
				link.l1 = "Будет исполненно.";
				link.l1.go = "exit";
			}
			else
			{
				dialog.text = "Упс... у нас тут бага... В квесте с потоплением корабля.";
				link.l1 = "Что? Опять?!";
				link.l1.go = "exit";
			}
		break;

		case "sink_ship_mission_complete":
			dialog.text = "Браво! Вы оправдываете мои ожидания. Но перейдем от слов к делу: вот ваша награда, а заодно позвольте поздравить вас с внеочередным повышением по службе.";
			link.l1 = "Благодарю вас.";
			link.l1.go = "exit";
			ShipSinkMissionCompleted(sti(npchar.nation), npchar);
		break;

		case "kill_enemy":
			if (!CheckAttribute(pchar, "quest.killcharacter"))
			{
				if (GenerateKillQuest(sti(npchar.nation), npchar, 1))
				{
					dialog.text = "Я хочу вам поручить несколько щекотливое дело. Понимаете ли, мне нужно, чтобы некто " + GetCharacterFullName(npchar.quest.killcharacter) + ", исчез навсегда... На данный момент этот человек находится на острове " + GlobalStringConvert(npchar.quest.killcharacter.island) + " и я хочу, чтобы вы первали его жизнь, и принесли, в знак доказательства его кольцо.";
					link.l1 = "Будет исполненно.";
					link.l1.go = "exit";
				}
				else
				{
					dialog.text = "Упс... у нас тут бага... В квесте с убийством.";
					link.l1 = "Что? Опять?!";
					link.l1.go = "exit";
				}
			}
			else
			{
				dialog.text = "В данный момент уменя нет для вас заданий.";
				link.l1 = "Ок.";
				link.l1.go = "exit";
			}
		break;

		case "kill_character_mission_complete":
			dialog.text = "Вижу, вы справились с порученой вам задачей. Примите мои поздравления, внеочередное звание и эту награду.";
			link.l1 = "Спасибо.";
			link.l1.go = "exit";
			KillCharacterMissionCompleted(sti(npchar.nation), npchar);
		break;

		case "kill_character_mission_failed":
			dialog.text = "Я смотрю, кольцо при вас, но, от своих информаторов я узнал, что человк, которого вам было приказано убить все еще жив. Право я разочарован.";
			link.l1 = "Простите, больше такого не повторится.";
			link.l1.go = "exit";
			KillCharacterMissionFailed(npchar);
		break;

		case "steal_item":
			if (GenerateStealItem(sti(npchar.nation), npchar, 0))
			{
				dialog.text = "Мне необходимо, чтобы вы похитили бумаги, которые находятся у губернатора острова " + XI_COnvertString(npchar.quest.stealitem.island) + ".";
				link.l1 = "Будет сделано.";
				link.l1.go = "exit";
			}
			else
			{
				dialog.text = "Упас. У нас тут бага в квесте про похищение бумаг.";
				link.l1 = "Дьявол.";
				link.l1.go = "exit";
			}
		break;

		case "steal_item_completed":
			dialog.text = "Отлично! Вы заметно выросли в моих глазах. Вот ваша награда.";
			link.l1 = "Спасибо.";
			link.l1.go = "exit";
			StealItemMissionCompleted(sti(npchar.nation), npchar, 0);
		break;

		case "steal_item_failed":
			dialog.text = "Отлично! Вы заметно выросли в моих глазах. Вот ваша награда.";
			link.l1 = "Спасибо.";
			link.l1.go = "exit";
			StealItemMissionFailed(npchar);
		break;

		case "disease_mission":
			dialog.text = "Вы, наверняка, уже знаете о том что в нашей колонии бушует болезнь...";
			link.l1 = "Это ни для кого не секрет - смердящие трупы на улицах красноречивее любых слов.";
			link.l1.go = "disease_mission_2";
		break;

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
			link.l1.go = "Прощайте.";
			link.l1.go = "exit";
		break;

		case "disease_mission_completed":
			if (CheckForDisease())
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

		case "delivery_passenger":
			if (GeneratePassengerQuest(npchar))
			{
				dialog.text = "Не возьметесь ли вы сопроводить человека по имени " + npchar.quest.deliverypassenger.passenger + " на колонию " + XI_ConvertString(npchar.quest.deliverypassenger.endcolony) + "\nПо прибытии на место вы получите сумму в количестве " + npchar.quest.deliverypassenger.money  + " пиастров.";
				link.l1 = "Не самая пыльная работенка, так что я возьмусь за нее.";
				link.l1.go = "delivery_passenger_accept";
				link.l2 = "Меня это не интересует.";
				link.l2.go = "delivery_passenger_refuse";
			}
			else
			{
				dialog.text = "У нас тут бага при попытке создания квеста на доставку пассажира.";
				link.l1 = "Бывает, что я могу сказать.";
				link.l1.go = "exit";
			}
		break;

		case "delivery_passenger_accept":
			dialog.text = "Вы окажете мне большую услугу. Вознаграждение вы получите по прибытии на место.";
			link.l1 = "Договорились.";
			link.l1.go = "exit";
			AcceptPassengerQuest(npchar);
		break;

		case "delivery_passenger_refuse":
			dialog.text = "Очень жаль, придется мне искать кого-нибудь другого.";
			link.l1 = "До встречи.";
			link.l1.go = "exit";
			RefusePassengerQuest(npchar);
		break;

		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}
}
