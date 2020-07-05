
void DuelTimer(string sQuest)
{
	string sQuest2 = pchar.quest.(sQuest).duel_quest;

	string sChar = pchar.quest.(sQuest2).character;

	DeleteAttribute(characterFromID(sChar), "characterlock");

	pchar.quest.(sQuest2).over = "yes";
}

void PrepareForDuel(string sQuest)
{
	LAi_group_SetRelation(LAI_TYPE_WARRIOR, LAI_GROUP_PLAYER, LAI_GROUP_NEITRAL);

	string sChar = pchar.quest.(sQuest).character;
	ref chr = characterFromID(sChar);

	LAi_RemoveLoginTime(chr);

	PlaceCharacter(chr, "goto", "random");

	chr.dialog.filename = "Duel_dialog.c";

	if(sChar == "Drink_man")
	{
		chr.dialog.currentnode = "Drink duel";
		sQuest = "win_drin_duel_"+rand(10000);
		pchar.quest.(sQuest).win_condition.l1 = "NPC_Death";
		pchar.quest.(sQuest).win_condition.l1.character = GetCharacterIndex("Drink_man");
		pchar.quest.(sQuest).win_condition = "Win_duel";
		pchar.quest.(sQuest).character = "Drink_man";
	}

	LAi_SetWarriorType(chr);
	LAi_warrior_DialogEnable(chr, true);
	LAi_warrior_SetStay(chr, true);

	locations[FindLocation("Duel_field")].reload.l1.disable = true;
}

void DrinkDuel()
{
	ref chr = CharacterFromID("Drink_man");

	LAi_SetActorType(chr);
	LAi_ActorAttack(chr, pchar, "");
	pchar.isduel.drink_man.duel = 1;
}

void Win_duel(string sQuest)
{
	string sChar = pchar.quest.(sQuest).character;

	DeleteAttribute(characterFromID(sChar), "characterlock");

	if(!CheckAttribute(pchar, "isduel." + sChar + ".duel"))
	{
		ChangeCharacterReputation(pchar, -10, 7);
	}
	else
	{
		DeleteAttribute(pchar, "isduel." + sChar + ".duel");
	}

	if(LAi_numloginedcharacters == 1)
	{
		DeleteAttribute(pchar, "isduel");

		int iLoc = FindLocation("Duel_field");
		Locations[iLoc].fastreload = Locations[iLoc].parentcolony;
		locations[iLoc].reload.l1.go = Locations[iLoc].parentcolony + "_tavern";
		locations[iLoc].reload.l1.disable = false;

		RecalculateJumpTable();
	}
}