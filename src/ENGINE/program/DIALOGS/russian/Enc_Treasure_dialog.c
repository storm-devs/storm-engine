//navy
#include "DIALOGS\russian\Common_Duel.c" 
// boal 29.05.04 даем карту клада
void ProcessDialogEvent()
{
	ref NPChar;
	aref Link, Diag;
	string NPC_Meeting;
	
	DeleteAttribute(&Dialog,"Links");

	makeref(NPChar,CharacterRef);
	makearef(Link, Dialog.Links);
	makearef(Diag, NPChar.Dialog);
	
	string iDay, iMonth;
	iDay = environment.date.day;
	iMonth = environment.date.month;
	string lastspeak_date = iday + " " + iMonth;
	
	aref   item;
	bool   ok;
	
	if (!CheckAttribute(npchar, "quest.trade_date"))
    {
        npchar.quest.trade_date = "";
    }

	ProcessDuelDialog(NPChar, link, Diag); //navy

	switch(Dialog.CurrentNode)
	{
		case "exit":
			Diag.CurrentNode = Diag.TempNode;
			NPChar.quest.meeting = NPC_Meeting;
			DialogExit();			
		break;

        case "Map_NotBuy":
            dialog.Text = "Разбогатеешь, заверну и в тряпочку, и в платочек носовой.";
			Link.l1 = "Сиди тут я мигом!";
			Link.l1.go = "exit";
		break;
		
		case "First time":
			Diag.TempNode = "First time";

			dialog.Text = "Псс... слушай сюда. У меня есть кое-что для тебя...";
			Link.l1 = "Ты про что?";
			Link.l1.go = "map_treasure_1";
		break;
		
		case "map_treasure_1":
            ok = (GetCharacterItem(Pchar, "map_part1")>0)  && (GetCharacterItem(Pchar, "map_part2")>0);
            if (GetCharacterItem(Pchar, "map_full") > 0 || ok)
            {
                dialog.Text = "Это выпивка!! И-ик... выпей со мной!";
    			Link.l1 = "Да ну тебя. Я думал, ты серьезно.";
    			Link.l1.go = "exit";
            }
            else
            {
                dialog.Text = "Есть у меня одна вещица специально для тебя по сходной цене.";
    			Link.l1 = "И что же это?";
    			Link.l1.go = "map_treasure_2";
    			Link.l2 = "Да ну тебя. Некогда мне.";
    			Link.l2.go = "exit";
			}
		break;
		
		case "map_treasure_2":
            dialog.Text = "Это отличная карта. Такую в магазине не найти. На ней указано, где зарыты сокровища! Сам я туда не доберусь, но карта точно подлинная. Клянусь.";
			Link.l1 = "Очень интересно. Сколько просишь?";
			Link.l1.go = "map_treasure_3";
			Link.l2 = "Да ну тебя. Некогда мне.";
			Link.l2.go = "exit";
		break;
		
		case "map_treasure_3":
		    if (npchar.quest.trade_date != lastspeak_date)
		    {
                npchar.quest.trade_date      = lastspeak_date;
            }
            dialog.Text = "Стоит всего "+Pchar.GenQuest.TreasureMoney+" золотых.";
			Link.l1 = "Давай, заверни в красивую тряпочку.";
			if (sti(Pchar.Money) >= sti(Pchar.GenQuest.TreasureMoney))
			{
			   Link.l1.go = "map_treasure_buy";
			}
			else
			{
			   Link.l1.go = "Map_NotBuy";
			}
			Link.l2 = "Дорого очень. Не нужно.";
			Link.l2.go = "exit";
		break;
		
		case "map_treasure_buy":
            dialog.Text = "Забирай. Теперь ты обязательно разбогатеешь!";
			Link.l1 = "Спасибо!";
			Link.l1.go = "exit";
			AddMoneyToCharacter(pchar, -sti(Pchar.GenQuest.TreasureMoney));
			GiveItem2Character(pchar, "map_full");
			//Items_FindItem("map_full", &item);
			//FillMapForTreasure(item);
			Diag.TempNode = "Temp_treasure";
			npchar.LifeDay = 0; // продал и свалил, если дуэль, то продлится у него жизнь
		break;
		
		case "Temp_treasure":
            dialog.Text = "Давай выпьем? И-ик.. Хороший тут ром!";
			Link.l1 = "Некогда!";
			Link.l1.go = "exit";
			ok = (GetCharacterItem(Pchar, "map_part1")>0)  || (GetCharacterItem(Pchar, "map_part2")>0);
            if (GetCharacterItem(Pchar, "map_full") == 0 && !ok)
            {
    			Link.l2 = "Ты продал мне фальшивку!!";
    			Link.l2.go = "Temp_treasure_1";
			}
			Diag.TempNode = "Temp_treasure";
		break;
		
		case "Temp_treasure_1":
            dialog.Text = "А ты кто? И-ик.. Я тя не знаю и уже не хочу знать.";
			Link.l1 = "Ну и не надо!";
			Link.l1.go = "exit";
			Link.l2 = "Да ты мне продал эту подделку!!! Я тебе счас...";
//			Link.l2.go = "tavern_keeper";
//			Diag.TempNode = "Temp_treasure";
			Link.l2.go = "outraged"; //navy -- дуэли!!!
			Diag.TempNode = "let_s_duel";
			npchar.LifeDay = 1; // чтоб не слетел на выходе
			SaveCurrentNpcQuestDateParam(npchar, "LifeTimeCreate");
		break;
		
		case "tavern_keeper":
            Diag.CurrentNode = Diag.TempNode;
			DialogExit();
			AddDialogExitQuest("tavern_keeper");
		break;
	}
}