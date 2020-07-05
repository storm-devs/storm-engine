
void BeginFreeAdventure(aref chr)
{
	RemoveCharacterCompanion(pchar, chr);
	RemovePassenger(pchar, chr);
	
	int chanse = sti(chr.skill.leadership) + sti(chr.skill.fencing) + sti(chr.skill.accuracy) + sti(chr.skill.defence)+ sti(chr.skill.sneak);
	//50 max
	chanse = chanse + sti(chr.ship.crew.quantity)/50;
	//150 max
	int badfortune = rand(70);
	if (chanse > badfortune)
	{
		string sCharacter = FindFreeAdventure();
		pchar.quest.free_adventure.(sCharacter) = chr.id;
		
		string sQuest = "free_adventure_for_" + sCharacter;
		pchar.quest.(sQuest).win_condition.l1 = "Timer";
		pchar.quest.(sQuest).win_condition.l1.date.day = GetAddingDataDay(0, 0, 30);
		pchar.quest.(sQuest).win_condition.l1.date.month = GetAddingDataMonth(0, 0, 30);
		pchar.quest.(sQuest).win_condition.l1.date.year = GetAddingDataYear(0, 0, 30);
		pchar.quest.(sQuest).win_condition.l1.date.hour = rand(23);
		pchar.quest.(sQuest).win_condition = (sQuest);
	}
	chr.quest.free_adventure = "1";
}

void AdventureCompleted(string sCharacter)
{
	
	
	string sChr;
	sChr = pchar.quest.free_adventure.(sCharacter);
	ref chr = CharacterFromID(sChr);
	
	switch (sCharacter)
	{
		case "character_0":
			ChangeCharacterAddressGroup(chr, "GrandaAvilia_tavern", "sit", "sit2");
		break;
		
		case "character_1":
			ChangeCharacterAddressGroup(chr, "GrandaAvilia_tavern", "sit", "sit3");
		break;
		
		case "character_2":
			ChangeCharacterAddressGroup(chr, "GrandaAvilia_tavern", "sit", "sit4");
		break;
	}
	LAi_SetSitType(chr);
	chr.quest.free_adventure = "2";
	
	int goods = sti(chr.skill.leadership) + sti(chr.skill.sneak) + rand(10);
	goods = goods/10;
	
	int goodquantity;
	
	int igood = rand(23)+6;
	goodquantity = GetCharacterFreeSpace(chr, igood);
	
	AddCharacterGoods(chr, igood, goodquantity);
			
	switch (goods)
	{
		case 0:
			AddMoneyToCharacter(chr, sti(chr.skill.commerce)*300);
		break;
		
		case 1:
			AddMoneyToCharacter(chr, sti(chr.skill.commerce)*500);
		break;
		
		case 2:
			AddMoneyToCharacter(chr, sti(chr.skill.commerce)*700);
		break;
	}
	chr.quest.free_adventure.good = igood;
}

void EndFreeAdventure(aref chr)
{
	
	int iMoney = sti(chr.money);
	AddMoneyToCharacter(pchar, iMoney);
	AddMoneyToCharacter(chr, -iMOney);
	
	SetCompanionIndex(pchar, chr, -1);
	string sQuest;
	
	for (int i = 0; i <3; i++)
	{
		sQuest = "character_" + i;
		if (pchar.quest.free_adventure.(sQuest) == chr.id)
		{
			pchar.quest.free_adventure.(sQuest) = 0;
		}		
	}
	chr.quest.free_adventure = "0";
}

string FindFreeAdventure()
{
	
	string sQuest;
	
	for (int i = 0; i <3; i++)
	{
		sQuest = "character_" + i;
		if (pchar.quest.free_adventure.(sQuest) == "")
		{
			return sQuest;
		}		
	}
	return "-1";
}

void RechargeAdventure(aref chr)
{
	
	int iMoney = sti(chr.money);
	AddMoneyToCharacter(pchar, iMoney);
	AddMoneyToCharacter(chr, -iMOney);
	
	chr.quest.free_adventure = "0";
	
	int good = sti(chr.quest.free_adventure.good);
	int goodquantity = GetCargoGoods(chr, good);
	
	AddCharacterGoods(pchar, good, goodquantity);
	RemoveCharacterGoods(chr,  good, goodquantity);
}