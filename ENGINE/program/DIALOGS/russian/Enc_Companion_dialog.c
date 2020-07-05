
void ProcessDialogEvent()
{
	ref NPChar;
	aref Link, NextDiag;

	DeleteAttribute(&Dialog,"Links");

	makeref(NPChar,CharacterRef);
	makearef(Link, Dialog.Links);
	makearef(NextDiag, NPChar.Dialog);

	int iResult;

	
	switch(Dialog.CurrentNode)
	{
		case "First time":	
			if(0)//CheckForQuestFree(npchar) == 1)
			{
				//iResult = GenerateEscortQuest(npchar, 0, "", 0, rand(3), 0, 0);
				trace("iResult = " + iResult);
				if (iResult == 0)
				{
					dialog.text = "Я ищу кого-нибудь, кто согласился бы спопроводить меня и мое судно на колонию " + XI_ConvertString("colony"+Quests[sti(npchar.currentquest)].l.target_colony) + ", находящуюся на острове " + XI_ConvertString(colonies[FindColony(Quests[sti(npchar.currentquest)].l.target_colony)].island) +  ".\nЗа эту услугу вы получите сумму " + FindRussianMoneyString(Quests[sti(npchar.CurrentQuest)].l.money)  + ". Что скажете?";
					link.l1 = "Хорошее предложение. Думаю, я возьмусь за эту работу.";
					link.l1.go = "escort_quest_accept";
					link.l2 = "Не думаю, что мне это подходит.";
					link.l2.go = "exit";
				}
				if(iResult == -1)
				{
					dialog.text = "Извините, но я не в настроении разговаривать.";
					link.l1 = "Хорошо.";
					link.l1.go = "exit";
					//EraseQuestFreeState(npchar);
				}
				if(iResult == -2)
				{
					dialog.text = "Хей, капитан, твоя репутация смердит похуже моих бортфорт, так что я не хочу иметь с тобйо никаких дел.";
					link.l1 = "Да ради бога.";
					link.l1.go = "exit";
				}
				if(iResult == -3)
				{
					dialog.text = "Капитан, я смотрю у вас полная эскадра. а Жаль... ибо у меня есть для тебя одно дельце.";
					link.l1 = "В таком случае поговорим позже.";
					link.l1.go = "exit";
				}
			}
			else
			{
				dialog.text = "Извините, но я не в настроении разговаривать.";
				link.l1 = "Хорошо.";
				link.l1.go = "exit";
			}
			
			NextDiag.TempNode = "First time";
		break;

		case "escort_quest_accept":
			dialog.text = "Значит по рукам, капитан? Только одно маленькое условие - мы, по возможности должны успеть доставить груз в течение " + FindRussianDaysString(Quests[sti(npchar.CurrentQuest)].l.time) + ".";
			link.l1 = "Да-да, я знаю, дело срочное и не терпит ни малейших отлагательств.";
			link.l1.go = "exit";
			AcceptEscortQuest(npchar);
		break;

		case "SelectOffer":
			dialog.text = SelectEscortShipDialog(npchar);
			link.l1 = "По рукам.";
			link.l1.go = "accept_offer_exit";
			link.l2 = "Мне это не интересно.";
			link.l2.go = "decline_offer_exit";
		break;

		case "accept_offer_exit":
			AcceptEscortQuest(npchar);
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;

		case "decline_offer_exit":
			DeleteEscortQuest(npchar);
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;

		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}
}
 
