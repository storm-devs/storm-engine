
void ProcessDialogEvent()
{
	ref NPChar;
	aref Link, NextDiag;

	DeleteAttribute(&Dialog,"Links");

	makeref(NPChar,CharacterRef);
	makearef(Link, Dialog.Links);
	makearef(NextDiag, NPChar.Dialog);

	if(CheckAttribute(npchar, "quest.questflag"))
	{
		if(sti(npchar.quest.questflag) != 1)
		{
			npchar.quest.questflag = CheckSlaveQuest(npchar);
			npchar.quest.questflag.model = SetModelFlag(npchar);
		}
	}
	
	switch(Dialog.CurrentNode)
	{
		case "First time":
			if(CheckAttribute(npchar, "quest.questflag"))
			{
				dialog.text = "Рад вас видеть, капитан. Вы выполнили наш уговор по доставке " + FindRussianSlavesString(sti(Quests[sti(npchar.currentquest)].l.slaves), "Acc") + "?";
				if(sti(npchar.quest.questflag) == 1)
				{
					dialog.text = SelectHuntForSlavesDialog(npchar);
					link.l1 = "Работка, как раз для меня. Я берусь за нее.";
					link.l1.go = "work_accepted";
					link.l2 = "Это работа не для меня. Поищи кого-нибудь другого.";
					link.l2.go = "work_declined";
				}
				if(sti(npchar.quest.questflag) == 2)
				{
					link.l1 = "Да. Можно приступать к разгрузке трюмов.";
					link.l1.go = "work_completed";
				}
				if(sti(npchar.quest.questflag) == 3)
				{
					link.l1 = "Еще нет, но я работаю над этим.";
					link.l1.go = "exit";
				}
				if(sti(npchar.quest.questflag) == -1)
				{
					dialog.text = "Мне нечего с вами обсуждать.";
					link.l1 = "Ну так проваливай!";
					link.l1.go = "exit";
				}
			}
			else
			{
				dialog.text = "Мне нечего с вами обсуждать.";
				link.l1 = "Ну так проваливай!";
				link.l1.go = "exit";
			}
			
			NextDiag.TempNode = "First time";
		break;

		case "work_completed":
			dialog.text = "С вами чертовски приятно иметь дело! Вот ваши деньги и примите мою искренюю благодарность.";
			link.l1 = "Это чувство взаимно, тем более, что оно подрепляется звонкой монетой с вашей стороны.";
			link.l1.go = "work_completed_exit";
		break;

		case "work_accepted":
			dialog.text = "Тогда я буду ждать вас, капитан. Вас, и то, о чем мы только что договорились.";
			link.l1 = "Постараюсь выполнить поручение так быстро, как смогу.";
			link.l1.go = "exit";
			AcceptSlavesQuest(npchar);
		break;

		case "work_declined":
			dialog.text = "Мое дело предложить, ваше дело отказаться. Прощайте, капитан.";
			link.l1 = "Удачи.";
			link.l1.go = "work_declined_exit";
		break;

		case "work_completed_exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
			CompleteSlavesQuest(npchar);
		break;

		case "work_declined_exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
			DeleteSlavesQuest(npchar);
		break;

		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}
}
