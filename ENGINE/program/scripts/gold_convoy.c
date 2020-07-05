
void GenerateGoldConvoy()
{
	//ищем нацию с наибольшим коофициентом
	//создаем чарактеров
	//назначем начальную локаицю
	//генерим руморс
	//назначаем маршрут
	//квест на тайм аут - конвой ушел.

	int iNation = 0;
	for(int i = 0; i < MAX_NATIONS; i++)
	{
		if(i != PIRATE)
		{
			if(sti(NationsState[iNation].quantity) <= sti(NationsState[i].quantity))
			{
				iNation = i;
			}
		}
	}
	string sColony = FindAlliedColonyForNation(iNation);
	string sNextColony = FindAlliedColonyForNationExceptColony(sColony);

	GenerateRumour("gold_convoy_begin", iNation, FindColony(sColony), FindColony(sNextColony));

	int iChar = GenerateCharacter(iNation, WITH_SHIP, "soldier", MAN, 1, BOSS);
	characters[iChar].cannotsurrender = 1;

	DeleteAttribute(&characters[iChar], "goldconvoy");
	characters[iChar].id = "Head of Gold Squadron";
	for(int k = 1; k < COMPANION_MAX; k++)
	{
		iChar = GenerateCharacter(iNation, WITH_SHIP, "soldier", MAN, 1, BOSS);
		SetCompanionIndex(characterFromID("Head of Gold Squadron"), -1, iChar);
		characters[iChar].cannotsurrender = 1;
	}

	int iGold = GetSquadronFreeSpace(&characters[GetCharacterIndex("Head of Gold Squadron")], GOOD_GOLD);
	iGold = iGold / 2;
	AddCharacterGoods(&characters[GetCharacterIndex("Head of Gold Squadron")], GOOD_GOLD, iGold);
	AddCharacterGoods(&characters[GetCharacterIndex("Head of Gold Squadron")], GOOD_SILVER, iGold);
	
	characters[GetCharacterIndex("Head of Gold Squadron")].goldconvoy.colony.(sColony) = 1;
	characters[GetCharacterIndex("Head of Gold Squadron")].goldconvoy.colony.lastColony = sNextColony;
	characters[GetCharacterIndex("Head of Gold Squadron")].mapEnc.type = "trade";
	characters[GetCharacterIndex("Head of Gold Squadron")].mapEnc.worldMapShip = "Manowar_gold";

	Map_CreateTrader(sColony, sNextColony, "Head of Gold Squadron");

	string sQuest = "DefeatGoldConvoy";
	pchar.quest.(sQuest).win_condition.l1 = "NPC_Death";
	pchar.quest.(sQuest).win_condition.l1.character = GetCharacterIndex("Head of Gold Squadron");
	pchar.quest.(sQuest).win_condition = "DefeatGoldConvoy";
}

void PrepareContinueGoldConvoy()
{
	if(GetCharacterIndex("Head of Gold Squadron") != -1)
	{
		ref chr = characterFromID("Head of Gold Squadron");

		string sLastColony = chr.goldconvoy.colony.lastColony;
		string sNextColony = FindAlliedColonyForNationExceptColonyForGoldSquadron(chr);
		if(sNextColony == "-1")
		{
			GenerateRumour("gold_convoy_end", sti(chr.nation), FindColony(sLastColony), -1);
			
			EmptyGoldConvoy();
		}
		else
		{
			chr.goldconvoy.colony.lastColony = sLastColony;
			chr.goldconvoy.colony.nextColony = sNextColony;

			string sQuest = GetNationNameByType(sti(chr.nation)) + "_gold_convoy_next_step_" + rand(100000);
			
			pchar.quest.(sQuest).win_condition.l1 = "Timer";
			pchar.quest.(sQuest).win_condition.l1.date.day = GetAddingDataDay(0, 0, 5);
			pchar.quest.(sQuest).win_condition.l1.date.month = GetAddingDataMonth(0, 0, 5);
			pchar.quest.(sQuest).win_condition.l1.date.year = GetAddingDataYear(0, 0, 5);
			pchar.quest.(sQuest).win_condition.l1.date.hour = rand(23);
			pchar.quest.(sQuest).win_condition = "ContinueGoldConvoy";
		}
	}
}

void ContinueGoldConvoy()
{
	ref chr = characterFromID("Head of Gold Squadron");
	string sLastColony = chr.goldconvoy.colony.lastColony;
	string sNextColony = chr.goldconvoy.colony.nextColony;

	chr.goldconvoy.colony.(sLastColony) = 1;
	chr.goldconvoy.colony.nextColony = sNextColony

	Map_CreateTrader(sLastColony, sNextColony, "Head of Gold Squadron");
}

string FindAlliedColonyForNationExceptColonyForGoldSquadron(aref chr)
{
	string sColony;
	object TempColonies[MAX_COLONIES];
	object GoldColonies[MAX_COLONIES];

	int m = 0;

	string sLastColony = chr.goldconvoy.colony.lastColony;

	for(int i = 0; i < MAX_COLONIES; i++)
	{
		if(!CheckAttribute(chr, "goldconvoy.colony." + colonies[i].id))
		{
			if(colonies[i].nation != "none")
			{
				if(sti(colonies[i].nation) == sti(chr.nation) && colonies[i].id != sLastColony)
				{
					TempColonies[i].id = colonies[i].id;
					m = m + 1;
				}
				else
				{
					DeleteAttribute(&TempColonies[i], "id");
				}
			}
			else
			{
				DeleteAttribute(&TempColonies[i], "id");
			}
		}
		else
		{
			DeleteAttribute(&TempColonies[i], "id");
		}
	}

	m = 0;
	for(i = 0; i < MAX_COLONIES; i++)
	{
		if(CheckAttribute(&TempColonies[i], "id"))
		{
			GoldColonies[m].id = TempColonies[i].id;
			m = m + 1;
		}
	}

	string sResult = "-1";
	if(m > 0)
	{
		m = rand(m - 1);
		sResult = GoldColonies[m].id
	}

	return sResult;
}

void DefeatGoldConvoy(aref chr)
{
	int iCharIndex = sti(chr.index);
	GenerateRumour("gold_convoy_dead", sti(characters[GetCharacterIndex("Head of Gold Squadron")].nation), iCharIndex, 0);

	if(chr.id != pchar.id)
	{
		int iGold = GetSquadronFreeSpace(chr, GOOD_GOLD);
		if(iGold > 0)
		{
			AddCharacterGoods(chr, GOOD_GOLD, iGold);
		}
	}

	string sQuest = "ClearGoldConvoy";
	pchar.quest.(sQuest).win_condition.l1 = "MapEnter";
	pchar.quest.(sQuest).win_condition = "ClearGoldConvoy";

	Map_ReleaseQuestEncounter("Head of Gold Squadron");
}

void EmptyGoldConvoy()
{
	int iChar;
	for(int i = 0; i < COMPANION_MAX; i++)
	{
		iChar = GetCompanionIndex(characterFromID("Head of Gold Squadron"), i);
		if(iChar != -1)
		{
			DeleteAttribute(&characters[iChar], "characterlock");
			EmptyFantomCharacacter(&characters[iChar]);
		}
	}
}