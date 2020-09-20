
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
			dialog.text = "Эй! Немедленно положите это на место!";
			link.l1 = "Э... Я сейчас же все это верну вам.";
			link.l1.go = "exit";			
			link.l2 = "Да пошел ты со своими притензиями!";
			link.l2.go = "exit";
			
			NextDiag.TempNode = "First time";
		break;
		
		case "Steal Quest Item":
			dialog.text = "Эй! Не слишком ли много ты себе позволяешь?!";
			link.l1 = "Дьявол... Многое, не многое, кого это волнует?! Мне нужен этот предмет!";
			link.l1.go = "exit3";
			
			NextDiag.TempNode = "First time";
		break;
		
		case "Uuups":
			dialog.text = "Вы арестованы за кражу!";
			link.l1 = "Хорошо, хорошо, я верну все, что было украдено.";
			link.l1.go = "steal";
			link.l2 = "Ха! Сильные слова, теперь попробуй доказать, что ты можешь воплотить их в жизнь!";
			link.l2.go = "fight";
		break;
		
		case "steal":
			dialog.text = "Боюсь вам еще придется заплатить штраф в размере " + (sti(pchar.rank)+100-sti(pchar.reputation))*100 + ".";
			if (sti(pchar.money) >= (sti(pchar.rank)+100-sti(pchar.reputation))*100)
			{
				link.l1 = "Пожалуйста. Вот деньги.";
				link.l1.go = "money";
			}
			link.l2 = "Я не собираюсь платить!";
			link.l2.go = "fight";
		break;
		
		case "money":
			StealMoney();
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
		
		case "fight":
			dialog.text = "Чтоже, мне остается только применить силу.";
			link.l1 = "Превосходно! Теперь посмотрим, какого цвета твоя кровь!";
			link.l1.go = "exit2";
			pchar.steal.nation = npchar.nation;
			pchar.steal.character = npchar.id;
			AddDialogExitQuest("StealFight");
		break;
		
		case "Exit2":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
		
		case "Exit3":
			StealQuestItem(npchar);
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;

		case "Exit":
			AddDialogExitQuest("prepare_steal");
			ReturnStealCharacter(npchar);
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}
}
