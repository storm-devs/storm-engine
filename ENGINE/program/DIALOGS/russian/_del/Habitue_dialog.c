
void ProcessDialogEvent()
{
	ref NPChar;
	aref Link, NextDiag;

	DeleteAttribute(&Dialog,"Links");

	makeref(NPChar,CharacterRef);
	makearef(Link, Dialog.Links);
	makearef(NextDiag, NPChar.Dialog);

	int iColonyInfoSumm = SelectColonyInfoUpdate(npchar);
	
	switch(Dialog.CurrentNode)
	{

		case "First Time":
			dialog.text = "Кого я вижу! Да это же... э-э-э... не помню как тебя зовут, да ну и ладно! Присаживайся, выпьем рома, поговорим по душам!";
			link.l1 = "Я бы с радостью, да только некогда мне.";
			link.l1.go = "exit";
			NextDiag.TempNode = "First Time";
		break;

		case "colony_Info_Time":
			if(iColonyInfoSumm > 100)
			{
				dialog.text = "Эй! Как тебя там, ты выглядишь как капитан, а значит тебя может заинтересовать информация которой я располагаю.";
				link.l1 = "Да? И что же ты мне можешь рассказать?";
				link.l1.go = "colony_info_select";
				link.l2 = "Меня это не интересует.";
				link.l2.go = "exit";
			}
			else
			{
				dialog.text = "Извините, но я слишком устал, чтобы разговаривать с вами.";
				link.l1 = "Не буду тебе мешать.";
				link.l1.go = "exit";
			}
			AddDialogExitQuest("RechargeColonyInfoMan");
			NextDiag.TempNode = "First Time";
		break;

		case "colony_info_select":
			dialog.text = "У меня есть последние сведения о колонии " + XI_ConvertString(npchar.colonyinfoupdate) + ", но я хочу увидеть как сумма " + iColonyInfoSumm + " золотых перекочует в мой кошелек.";
			if(sti(pchar.money) >= iColonyInfoSumm)
			{
				link.l1 = "Хорошо. Вот твои деньги.";
				link.l1.go = "colony_info_update";
			}
			link.l2 = "У меня нет таких денег. Прощай.";
			link.l2.go = "exit";
		break;

		case "colony_info_update":
			dialog.text = "Отлично! А теперь слушай... (Вы внимательно выслушиваете полученную информацию).";
			link.l1 = "Благодарю за помощь.";
			link.l1.go = "exit";
			AddMoneyToCharacter(pchar, -iColonyInfoSumm);
			ColoniesInfoUpdateFromMan(npchar);
		break;
                
		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}
}
