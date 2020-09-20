
void ProcessDialogEvent()
{
	ref NPChar;
	aref Link, NextDiag;

	DeleteAttribute(&Dialog,"Links");

	makeref(NPChar,CharacterRef);
	makearef(Link, Dialog.Links);
	makearef(NextDiag, NPChar.Dialog);

	string sAgreeLink, sDeclineLink;

	int iSuccesDelivery;

	switch(Dialog.CurrentNode)
	{
		case "First time":
			if(0)//  to_do CheckAttribute(npchar, "quest.questflag"))
			{
				dialog.text = SelectDeliveryTradeDialogQuestion(npchar);
				if(sti(npchar.quest.questflag) == 2)//сдать квест
				{
					link.l1 = "Да, конечно.";
					link.l1.go = "delivery_trade_succesfull";
				}
				if(sti(npchar.quest.questflag) == 3)//вспомнить квест
				{
					if(Quests[sti(npchar.currentquest)].l.target_npc == npchar.id)
					{
						if(sti(Quests[sti(npchar.currentquest)].l.freespace) > 0 && sti(Quests[sti(npchar.currentquest)].l.isdelayed) == 1)
						{
							link.l1 = "К сожалению, на данный момент мне негде разместить груз.";
							link.l1.go = "delivery_trade_decline";
						}
						else
						{
							link.l1 = "Понимаете ли часть груза попортилась в пути... В общем, я еще не готов предоставить его вам.";
							link.l1.go = "delivery_trade_decline";
						}
					}
					else
					{
						dialog.text = SelectHovernorQuestReminderText(npchar);
						link.l1 = "Ваше задание еще не выполненно, но я работаю над этим. А пока покажите свои товары.";
						link.l1.go = "store";
						link.l2 = "Спасибо, что напомнили. Я сейчас же займусь вашим делом.";
						link.l2.go = "exit";
					}
				}
				if(sti(npchar.quest.questflag) != 2 && sti(npchar.quest.questflag) != 3)
				{
					dialog.text = "Приветствую вас в своем магазине. Хотите взглянуть на мои товары?";
					link.l1 = "Конечно, что у тебя есть?";
					link.l1.go = "trade";
					if(sti(npchar.quest.questflag) == 1)
					{
						link.l2 = "Может быть в другой раз, давай лучше поговорим о делах.";
						link.l2.go = "quests";
					}
					link.l3 = "У меня нет на это времени.";
					link.l3.go = "exit";
				}
			}
			else
			{
				dialog.text = "Приветствую вас в своем магазине. Хотите взглянуть на мои товары?";
				link.l1 = "Конечно, что у тебя есть?";
				link.l1.go = "trade";
				if(CheckAttribute(npchar, "quest.questflag"))
				{
					if(sti(npchar.quest.questflag) == 1)
					{
						link.l2 = "Может быть в другой раз, давай лучше поговорим о делах.";
						link.l2.go = "quests";
					}
				}
				link.l3 = "Нет на это времени...";
				link.l3.go = "exit";
			}

			NextDiag.TempNode = "First time";
		break;

		case "trade":
			dialog.text = "У меня богатый ассортимент различных товаров, капитан.";
			link.l1 = "Покажи мне свои товары, торговец.";
			link.l1.go = "store";
			link.l2 = "Знаешь,  у меня нашлись другие, более важные дела, чем смотреть на твои товары.";
			link.l2.go = "exit";
		break;

		case "store":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
			LaunchStore(sti(NPChar.StoreNum));
		break;

		case "delivery_trade_succesfull":
			if(sti(Quests[sti(npchar.currentquest)].l.isdelayed) == 1)
			{
				dialog.text = "Тогда я сейчас же отдам приказ о начале погрузки товаров на ваш корабль.";
				link.l1 = "Отлично! Мне не терпится отправиться в путь побыстрее.";
				link.l1.go = "exit";
				PrepareAcceptDeliveryQuest(npchar);
			}
			else
			{
				if(sti(Quests[sti(npchar.currentquest)].l.contribution) > 0)
				{
					dialog.text = "Хм.. посмотрим, посмотрим... Вы опаздали на " + FindRussianDaysString(sti(Quests[sti(npchar.currentquest)].l.contribution)) + ", и поэтому я вынужден заплатить вам меньшую сумму, чем было оговорено в кантракте.";
					link.l1 = "Вы думаете у меня есть выбор? Чтож, давайте сюда ваши деньги и потрудитесь побыстрее приступить к разгрузке.";
					link.l1.go = "exit";
					CompleteDeliveryTradeQuest(npchar);
				}
				else
				{
					dialog.text = "Что же, в таком случае, если вы не возражаете, я прикажу своим людям приступить к разгрузке вашего корабля.";
					link.l1 = "Никаких возражений.";
					link.l1.go = "exit";
					CompleteDeliveryTradeQuest(npchar);
				}
			}
		break;

		case "delivery_trade_decline":
			dialog.text = "Очень жаль.";
			link.l1 = "А уж мне-то как жаль, но, может быть, вы покажете мне свои товары?";
			link.l1.go = "trade";
            link.l2 = "Ладно, поболтаем в следующий раз.";
			link.l2.go = "exit";
		break;

		case "quests":
			dialog.text = "Да, я вас внимательно слушаю, капитан.";
			link.l1 = "Нет ли у вас какой-нибудь работенки для меня и моего судна?";
			link.l1.go = "generate_quests";

			iSuccesDelivery = CheckForDeliverySuccess(npchar, "");
			if(iSuccesDelivery == 2)
			{
				if(npchar.id == Quests[sti(npchar.currentquest)].l.target_npc)
				{
					if(sti(Quests[sti(npchar.currentquest)].l.isdelayed) == 0)
					{
						if(CheckAttribute(npchar, "quest.punishment"))
						{
							if(sti(npchar.quest.punishment) > 0)
							{
								link.l2 = "Мной был доставлен груз, который вы ожидаете.";
								link.l2.go = "delivery_trade_success_punishment";
							}
							else
							{
								link.l2 = "Мной был доставлен груз, который вы ожидаете.";
								link.l2.go = "delivery_trade_success";
							}
						}
						else
						{
							link.l2 = "Мной был доставлен груз, который вы ожидаете.";
							link.l2.go = "delivery_trade_success";
						}
					}
					else
					{
						link.l3 = "Мне поручено забрать у вас груз...";
						link.l3.go = "delay_delivery_cargo_load";
					}
				}
			}
		break;

		case "delay_delivery_cargo_load":
            dialog.text = "Да, да, конечно! Я вас уже давно жду. Сейчас я отдам приказ моим людям начать погрузку.";
			link.l1 = "Отлично! Как только погрузка закончится я отправляюсь в обратный путь.";
			link.l1.go = "exit";
			PrepareAcceptDeliveryQuest(npchar);
		break;

		case "generate_quests":
			dialog.text = SelectHovernorQuestText(npchar, &sAgreeLink, &sDeclineLink);
			link.l1 = "Хорошее предложение, я берусь за это дело.";
			link.l1.go = sAgreeLink;
			link.l2 = "Боюсь, мне это не подходит.";
			link.l2.go = "exit";
		break;

		case "delivery_trade_accept":
			dialog.text = "Значит по рукам, капитан? Только одно маленькое условие - вы должны успеть доставить груз за " + FindRussianDaysString(sti(Quests[sti(npchar.CurrentQuest)].l.time)) + ". В противном случае, за каждый день опоздания вы будете терять 5 процентов выплат от обещанной вам суммы.";
			link.l1 = "Да-да, я знаю, дело срочное и не терпит ни малейших отлагательств.";
			link.l1.go = "exit";
			AcceptDeliveryTradeQuest(npchar);
		break;

		case "delivery_trade_success":
			dialog.text = "Отлично! А то я уже начал волноваться. Вот ваша выручка, капитан.";
			link.l1 = "Спасибо, а теперь, покажите какие товары у вас есть в наличии.";
			link.l1.go = "trade";
			link.l2 = "Благодарю вас, надеюсь, мы с вами еще увидимся.";
			link.l2.go = "exit";
			CompleteDeliveryTradeQuest(npchar);
		break;

		case "delivery_trade_success_punishment":
			dialog.text = "Хм... Это радует, но вы опоздали на " + FindRussianDaysString(sti(npchar.quest.punishment)) + "! В связи с эим я просто вынужден скоратить сумму выплат, ибо сам понес доволньно ощутимые убытки.";
			link.l1 = "Звучит вполне справедливо. Но мне хотелось бы взглянутьна ваши товары.";
			link.l1.go = "trade";
			link.l2 = "Я понимаю ваше положение. А теперь давайте покончим с этим делом, ибо я спешу.";
			link.l2.go = "exit";
			CompleteDeliveryTradeQuest(npchar);
		break;

		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}
}
