
#define MAX_RANGER_SKILL 12;

#event_handler("Map_TraderSucces", "Map_TraderSucces");
#event_handler("Map_WarriorEnd", "Map_WarriorEnd");

void GenerateRangers()
{
	int iNation;
	int iChar;
	int skillPoints = 0;
	int perkPoints = 0;
	
	string sQuest;
	for (int i = 0; i < iRangersRate; i++)
	{
		iNation = rand(HOLLAND);
		iChar = GenerateCharacter(iNation, WITH_SHIP, "citizen", MAN, 1, WARRIOR);

		characters[iChar].id = "ranger " + i;
		characters[iChar].cannotsurrender = 1;
		characters[iChar].fame = rand(20);
		
		string sColony = FindNonEnemyColonyForNation(sti(characters[iChar].nation));
		
		characters[iChar].colony = sColony;
		characters[iChar].money = 2000 + (sti(characters[iChar].rank) + sti(characters[iChar].skill.commerce) + sti(characters[iChar].skill.sneak))*1000;
		characters[iChar].adventure = "begin";

		sQuest = "Check_For_Dead_Ranger_" + iChar;
		pchar.quest.(sQuest).win_condition.l1 = "NPC_Death";
		pchar.quest.(sQuest).win_condition.l1.character = iChar;
		pchar.quest.(sQuest).win_condition = "Check_For_Dead_Ranger";
		pchar.quest.(sQuest).ranger = characters[iChar].id;
	}
	RangerAdventure();
}

void RangerAdventure()
{
	int iChar = GetCharacterIndex("Ranger 0");

	int iMaxRangers = iChar + iRangersRate;

	for (int i = iChar; i<iMaxRangers; i++)
	{
		RechargeRangerAdventure(&characters[i]);
	}
}

void RechargeRangerAdventure(aref chr)
{
	RepairRangerShip(chr);
	SearchForDisbandEscort(chr);
	SearchForEscort(chr);
	
	int iMaxBusinessTypes = 2;
	int iBusiness = rand(iMaxBusinessTypes);

	////////////////////////////////////
	/// nation service quest
	////////////////////////////////////
	if(iBusiness == 0)
	{
		if(!CheckAttribute(chr, "adventure.career"))
		{
			BeginRangerCareer(chr);
		}
		else
		{
			iBusiness = rand(iMaxBusinessTypes - 1) + 1;
		}
	}

	////////////////////////////////////
	/// trade quest
	////////////////////////////////////
	if(iBusiness == 1)
	{
		LoadCargoForRanger(chr);
		GenerateTradeAdventure(chr);
	}

	////////////////////////////////////
	/// war_quest
	////////////////////////////////////
	if(iBusiness == 2)
	{
		GenerateWarAdventure(chr);
	}
	
	chr.adventure = "inprogress";
}

void GenerateWarAdventure(aref chr)
{
	int iNation = chr.nation;
	if(CheckAttribute(chr, "adventure.career"))
	{
		iNation = sti(chr.adventure.career);
	}
	int iEnemy = FindEnemyNation2Nation(iNation);
	
	string sTargetColony = FindRandomColonyExceptColony(chr.colony);
	chr.adventure.type = "war";
	chr.adventure.endColony = sTargetColony;
	chr.adventure.war.enemies = rand(2) + 1;
	chr.adventure.war.enemynation = iEnemy;
	chr.mapEnc.type = "trade";
	chr.mapEnc.worldMapShip = "ranger";
	Map_CreateTrader(chr.colony, chr.adventure.endColony, chr.id);
}

void BeginRangerCareer(aref chr)
{
	int iNation = sti(colonies[FindColony(chr.colony)].nation);
	chr.adventure.career = iNation;
	GenerateRumour("ranger_career", sti(chr.index), iNation, sti(chr.type));
	WaitForRechargeRangerAdventure(chr);
}

void SearchForDisbandEscort(aref chr)
{
	int iChar;
	for(int i = 1; i < COMPANION_MAX; i++)
	{
		iChar = GetCompanionIndex(chr, i);
		if(iChar != -1)
		{
			if(sti(chr.money) > sti(characters[i].fame))
			{
				AddMoneyToCharacter(&characters[i], sti(characters[i].money));
				AddMoneyToCharacter(chr, -sti(characters[i].money));
			}
			else
			{
				RemoveCharacterCompanion(chr, &characters[i]);
				RechargeRangerAdventure(&characters[i]);
			}
		}
	}
}

void SearchForEscort(aref chr)
{
	int iFirstRanger = GetCharacterIndex("ranger 0");
	for(int i = iFirstRanger; i<(iFirstRanger+iRangersRate); i++)
	{
		if(characters[i].colony == chr.colony && chr.id != characters[i].id)
		{
			if(CheckAttribute(&characters[i], "isfreeforquest"))
			{
				if(sti(characters[i].isfreeforquest) == 1)
				{
					if(sti(characters[i].type) == WARRIOR && sti(chr.type) == TRADER)
					{
						if(sti(characters[i].fame) < sti(chr.fame))
						{
							if(GetCompanionQuantity(chr) < 3)
							{
								if(sti(chr.money) > sti(characters[i].fame))
								{
									SetCompanionIndex(chr, -1, i);
									AddMoneyToCharacter(&characters[i], sti(characters[i].money));
									AddMoneyToCharacter(chr, -sti(characters[i].money));
									EndRangerAdventure(&characters[i]);
									trace("ranger " + chr.id + " has hired " + characters[i].id);
								}
							}
						}
					}
				}
			}
		}
	}
}

void LoadCargoForRanger(aref chr)
{
	int iTrade = FindTradeForRanger(chr);
	
	chr.isfreeforquests = 0;
	string sColony = chr.colony;
	int iStoreIdx = FindStore(sColony);
	
	int iPossibleTradeQuantity = GetCharacterFreeSpace(chr, iTrade);
	
	if (iPossibleTradeQuantity > 2)
	{
		int iMoney = sti(chr.money);
		int iSumm = iMoney + 1;
		
		while(iSumm > iMoney)
		{
			iPossibleTradeQuantity = iPossibleTradeQuantity - 2;
			iSumm = GetStoreGoodsPrice(&Stores[iStoreIdx],iTrade,PRICE_TYPE_BUY,chr,1)*iPossibleTradeQuantity/sti(goods[iTrade].units);
		}
		
		AddMoneyTocharacter(chr, -iSumm);
		AddCharacterGoods(chr, iTrade, iPossibleTradeQuantity);
		RemoveStoreGoods(&Stores[iStoreIdx], iTrade, iPossibleTradeQuantity);
	}
}

void UnLoadCargoForRanger(aref chr)
{
	int iTradeQuantity = 0;
	
	string sColony = chr.colony;
	
	int iStoreIdx = FindStore(sColony);
	int iSumm = 0;
	int iExp = 0;
	
	for(int i = GOOD_WHEAT; i < GOOD_SLAVES; i++)
	{
		iTradeQuantity = GetCargoGoods(chr,i);
		if (iTradeQuantity > 0)
		{
			iSumm = GetStoreGoodsPrice(&Stores[iStoreIdx],i,PRICE_TYPE_SELL,chr,1)*iTradeQuantity/sti(goods[i].units);
			
			AddMoneyToCharacter(chr, iSumm);
			
			RemoveCharacterGoods(chr, i, iTradeQuantity);
			
			AddStoreGoods(&Stores[iStoreIdx], i, iTradeQuantity);
			
			iExp = iSumm*10/sti(chr.rank);
			AddCharacterExp(chr, iExp);
		}
	}
	ChangeCharacterReputation(chr, 1, 6);
}

int FindTradeForRanger(aref chr)
{
	string sBeginColony = chr.colony;
	string sEndColony = FindNonEnemyColonyForAdventure(sti(chr.nation), sBeginColony);
	
	chr.adventure.endColony = sEndColony;
	
	int iBeginStoreIdx = FindStore(sBeginColony);
	int iEndStoreIdx = FindStore(sEndColony);
	
	//int TradeTypes[40];
	
	//int step = 0;

	int iTrade = -1;
	
	int iBuyPrice, iSellPrice;

	for(int i = GOOD_WHEAT; i<GOOD_SLAVES; i++)
	{
		iBuyPrice = GetStoreGoodsPrice(&Stores[iBeginStoreIdx],i,PRICE_TYPE_BUY,chr, 1);
		iSellPrice = GetStoreGoodsPrice(&Stores[iEndStoreIdx],i,PRICE_TYPE_SELL,chr, 1);
		if (iBuyPrice <= iSellPrice)
		{
			/*TradeTypes[step] = i;
			step++;
			*/
			iTrade = i;
			break;
		}
	}
	
	/*int iTrade = rand(step - 1);
	
	if (iTrade < 0)
	{
		iTrade = 0;
	}
	
	
	iTrade = TradeTypes[iTrade];
	*/
	
	if (iTrade < 0)
	{
		iTrade = rand(20)+6;
	}
	
	return iTrade;
}

void GenerateTradeAdventure(aref chr)
{
	int iLuck = sti(chr.skill.sneak);
	
	chr.adventure.type = "trade";
	chr.mapEnc.type = "trade";
	chr.mapEnc.worldMapShip = "ranger";
	Map_CreateTrader(chr.colony, chr.adventure.endColony, chr.id);
}

void RangerTimer(aref chr)
{
	if(!CheckAttribute(chr, "adventure.endColony"))
	{
		trace ("ranger " + chr.id + " have no end colony");
	}
	else
	{
		chr.colony = chr.adventure.endColony;

		if(chr.adventure.type == "war")
		{
			GenerateBattleForRanger(chr);
		}
		
		if (chr.adventure.type == "trade")
		{
			UnLoadCargoForRanger(chr);
			WaitForRechargeRangerAdventure(chr);
			chr.fame = chr.fame + 1;
			return;
		}

		if(chr.adventure.type == "go_back")
		{
			WaitForRechargeRangerAdventure(chr);
			chr.fame = chr.fame + sti(chr.adventure.war.enemies);
		}
	}
}

void RangerNextLevel(aref chr)
{
	if(CheckAttribute(chr, "hired"))
	{
		if(sti(chr.hired) == 1)
		{
			return;
		}
	}
	if(CheckAttribute(chr, "isofficer"))
	{
		if(sti(chr.isofficer) == 1)
		{
			return;
		}
	}
	if(chr.id == pchar.id)
	{
		return;
	}

	int skillPoints = sti(chr.Skill.FreeSkill);
	int perkPoints = sti(chr.perks.FreePoints);
	
	int iIncr = 0;
	int iPercIncr = 0;

	iPercIncr = perkPoints;
	if(!CheckAttribute(chr, "perks.list.Tireless") && iPercIncr > 0)
	{
		chr.perks.list.Tireless = true;
		iPercIncr--;
	}
	if(!CheckAttribute(chr, "perks.list.BladeDancer") && iPercIncr > 0)
	{
		chr.perks.list.BladeDancer = true;
		iPercIncr--;
	}
	if(!CheckAttribute(chr, "perks.list.Sliding") && iPercIncr > 0)
	{
		chr.perks.list.Sliding = true;
		iPercIncr--;
	}

	if(!CheckAttribute(chr, "perks.list.BoardingMaster") && iPercIncr > 0)
	{
		chr.perks.list.BoardingMaster = true;
		iPercIncr--;
	}

	if(!CheckAttribute(chr, "perks.list.Musketeer") && iPercIncr > 0)
	{
		chr.perks.list.Musketeer = true;
		iPercIncr--;
	}
	if(!CheckAttribute(chr, "perks.list.BoardingDevil") && iPercIncr > 0)
	{
		chr.perks.list.Musketeer = true;
		iPercIncr--;
	}

	if(!CheckAttribute(chr, "perks.list.Gunner") && iPercIncr > 0)
	{
		chr.perks.list.Gunner = true;
		iPercIncr--;
	}

	if(!CheckAttribute(chr, "perks.list.GunnerCamicadze") && iPercIncr > 0)
	{
		chr.perks.list.GunnerCamicadze = true;
		iPercIncr--;
	}

	if(!CheckAttribute(chr, "perks.list.GunnerSniper") && iPercIncr > 0)
	{
		chr.perks.list.GunnerSniper = true;
		iPercIncr--;
	}

	if(!CheckAttribute(chr, "perks.list.WindCatcher") && iPercIncr > 0)
	{
		chr.perks.list.WindCatcher = true;
		iPercIncr--;
	}

	if(!CheckAttribute(chr, "perks.list.SailsMan") && iPercIncr > 0)
	{
		chr.perks.list.SailsMan = true;
		iPercIncr--;
	}

	if(!CheckAttribute(chr, "perks.list.HardMan") && iPercIncr > 0)
	{
		chr.perks.list.HardMan = true;
		iPercIncr--;
	}

	if(!CheckAttribute(chr, "perks.list.HardHitter") && iPercIncr > 0)
	{
		chr.perks.list.HardHitter = true;
		iPercIncr--;
	}

	if(!CheckAttribute(chr, "perks.list.Berserker") && iPercIncr > 0)
	{
		chr.perks.list.Berserker = true;
		iPercIncr--;
	}
	
	switch(sti(chr.type))
	{
		case TRADER:
			iIncr = rand(skillPoints);
			if (sti(chr.skill.Commerce) < MAX_RANGER_SKILL && sti(chr.skill.Commerce) + iIncr < MAX_RANGER_SKILL)
			{
					chr.skill.commerce = sti(chr.skill.commerce) + iIncr;
					skillPoints = skillPoints - iIncr;
			}
			
			iIncr = rand(skillPoints);
			if (sti(chr.skill.sneak) < MAX_RANGER_SKILL && sti(chr.skill.sneak) + iIncr < MAX_RANGER_SKILL)
			{
				chr.skill.sneak = sti(chr.skill.sneak) + iIncr;
				skillPoints = skillPoints - iIncr;
			}
			
			iIncr = rand(skillPoints);
			if (sti(chr.skill.sailing) < MAX_RANGER_SKILL && (sti(chr.skill.sailing) + iIncr) < MAX_RANGER_SKILL)
			{
				chr.skill.sailing = sti(chr.skill.sailing) + iIncr;
				skillPoints = skillPoints - iIncr;
			}
			
			iIncr = rand(skillPoints);
			if (sti(chr.skill.leadership) < MAX_RANGER_SKILL && (sti(chr.skill.leadership) + iIncr) < MAX_RANGER_SKILL)
			{
				chr.skill.leadership = sti(chr.skill.leadership) + iIncr;
				skillPoints = skillPoints - iIncr;
			}
			
			iIncr = rand(skillPoints);
			if (sti(chr.skill.repair) < MAX_RANGER_SKILL && (sti(chr.skill.repair) + iIncr) < MAX_RANGER_SKILL)
			{
				chr.skill.repair = sti(chr.skill.repair) + iIncr;
				skillPoints = skillPoints - iIncr;
			}
			
			iIncr = rand(skillPoints);
			if (sti(chr.skill.accuracy) < MAX_RANGER_SKILL && (sti(chr.skill.accuracy) + iIncr) < MAX_RANGER_SKILL)
			{
				chr.skill.accuracy = sti(chr.skill.accuracy) + iIncr;
				skillPoints = skillPoints - iIncr;
			}
			
			iIncr = rand(skillPoints);
			if (sti(chr.skill.cannons) < MAX_RANGER_SKILL && (sti(chr.skill.cannons) + iIncr) < MAX_RANGER_SKILL)
			{
				chr.skill.cannons = sti(chr.skill.cannons) + iIncr;
				skillPoints = skillPoints - iIncr;
			}
			
			iIncr = rand(skillPoints);
			if (sti(chr.skill.defence) < MAX_RANGER_SKILL && (sti(chr.skill.defence) + iIncr) < MAX_RANGER_SKILL)
			{
				chr.skill.defence = sti(chr.skill.defence) + iIncr;
				skillPoints = skillPoints - iIncr;
			}
			
			iIncr = rand(skillPoints);
			if (sti(chr.skill.gun) < MAX_RANGER_SKILL && (sti(chr.skill.gun) + iIncr) < MAX_RANGER_SKILL)
			{
				chr.skill.gun = sti(chr.skill.gun) + iIncr;
				skillPoints = skillPoints - iIncr;
			}
			
			iIncr = rand(skillPoints);
			if (sti(chr.skill.fencing) < MAX_RANGER_SKILL && (sti(chr.skill.fencing) + iIncr) < MAX_RANGER_SKILL)
			{
				chr.skill.fencing = sti(chr.skill.fencing) + iIncr;
				skillPoints = skillPoints - iIncr;
			}
		break;
		
		case WARRIOR:

			iIncr = rand(skillPoints);
			if (sti(chr.skill.accuracy) < MAX_RANGER_SKILL && (sti(chr.skill.accuracy) + iIncr) < MAX_RANGER_SKILL)
			{
				chr.skill.accuracy = sti(chr.skill.accuracy) + iIncr;
				skillPoints = skillPoints - iIncr;
			}
			
			iIncr = rand(skillPoints);
			if (sti(chr.skill.cannons) < MAX_RANGER_SKILL && (sti(chr.skill.cannons) + iIncr) < MAX_RANGER_SKILL)
			{
				chr.skill.cannons = sti(chr.skill.cannons) + iIncr;
				skillPoints = skillPoints - iIncr;
			}
			
			iIncr = rand(skillPoints);
			if (sti(chr.skill.fencing) < MAX_RANGER_SKILL && (sti(chr.skill.fencing) + iIncr) < MAX_RANGER_SKILL)
			{
				chr.skill.fencing = sti(chr.skill.fencing) + iIncr;
				skillPoints = skillPoints - iIncr;
			}
			
			iIncr = rand(skillPoints);
			if (sti(chr.skill.gun) < MAX_RANGER_SKILL && (sti(chr.skill.gun) + iIncr) < MAX_RANGER_SKILL)
			{
				chr.skill.gun = sti(chr.skill.gun) + iIncr;
				skillPoints = skillPoints - iIncr;
			}
			
			iIncr = rand(skillPoints);
			if (sti(chr.skill.sailing) < MAX_RANGER_SKILL && (sti(chr.skill.sailing) + iIncr) < MAX_RANGER_SKILL)
			{
				chr.skill.sailing = sti(chr.skill.sailing) + iIncr;
				skillPoints = skillPoints - iIncr;
			}
			
			iIncr = rand(skillPoints);
			if (sti(chr.skill.leadership) < MAX_RANGER_SKILL && (sti(chr.skill.leadership) + iIncr) < MAX_RANGER_SKILL)
			{
				chr.skill.leadership = sti(chr.skill.leadership) + iIncr;
				skillPoints = skillPoints - iIncr;
			}
			
			iIncr = rand(skillPoints);
			if (sti(chr.skill.defence) < MAX_RANGER_SKILL && (sti(chr.skill.defence) + iIncr) < MAX_RANGER_SKILL)
			{
				chr.skill.defence = sti(chr.skill.defence) + iIncr;
				skillPoints = skillPoints - iIncr;
			}
			
			iIncr = rand(skillPoints);
			if (sti(chr.skill.sneak) < MAX_RANGER_SKILL && (sti(chr.skill.sneak) + iIncr) < MAX_RANGER_SKILL)
			{
				chr.skill.sneak = sti(chr.skill.sneak) + iIncr;
				skillPoints = skillPoints - iIncr;
			}
			
			iIncr = rand(skillPoints);
			if (sti(chr.skill.commerce) < MAX_RANGER_SKILL && (sti(chr.skill.commerce) + iIncr) < MAX_RANGER_SKILL)
			{
				chr.skill.commerce = sti(chr.skill.commerce) + iIncr;
				skillPoints = skillPoints - iIncr;
			}
			
			iIncr = rand(skillPoints);
			if (sti(chr.skill.repair) < MAX_RANGER_SKILL && (sti(chr.skill.repair) + iIncr) < MAX_RANGER_SKILL)
			{
				chr.skill.repair = sti(chr.skill.repair) + iIncr;
				skillPoints = skillPoints - iIncr;
			}
		break;

		case CITIZEN:
			chr.skill.accuracy = 1;
			chr.skill.cannons = 1;
			chr.skill.fencing = 1;
			chr.skill.gun = 1;
			chr.skill.sailing = 1;
			chr.skill.leadership = 1;
			chr.skill.defence = 1;
			chr.skill.sneak = 1;
			chr.skill.commerce = 1;
			chr.skill.repair = 1;
			skillPoints = 0;
			perkPoints = 0;
		break;
	}
	chr.skill.temp.leadership = chr.skill.leadership;
	chr.skill.temp.sailing = chr.skill.sailing;
	chr.skill.temp.fencing = chr.skill.fencing;
	chr.skill.temp.gun = chr.skill.gun;
	chr.skill.temp.cannons = chr.skill.cannons;
	chr.skill.temp.accuracy = chr.skill.accuracy;
	chr.skill.temp.grappling = chr.skill.grappling;
	chr.skill.temp.defence = chr.skill.defence;
	chr.skill.temp.repair = chr.skill.repair;
	chr.skill.temp.commerce = chr.skill.commerce;
	chr.skill.temp.sneak = chr.skill.sneak;
	Lai_SetLevelHP(chr);
	
	chr.Skill.FreeSkill = skillPoints;
	chr.perks.FreePoints = perkPoints;
	
	if(chr.ship.type != SHIP_NOTUSED)
	{
		CheckForNewShip(chr);
	}
}

void CheckForNewShip(aref chr)
{
	int iMoney = sti(chr.money);
		
	ref refShip;
	
	refship = GetRealShip(sti(chr.ship.type));
	
	int iOldShipClass = sti(refship.Class);
	int iShipType = sti(chr.ship.type);
	int iOldPrice = sti(refship.Price);
	
	int iNewShipClass;
	int iNewPrice;
	
	int iOldCargo, iNewCargo;
	
	for (int i = iShipType; i <= SHIP_MANOWAR; i++)
	{
		refShip = GetShipByType(i);
		iNewShipClass = sti(refship.Class);
		iNewPrice = sti(refship.Price);
		
		if (iOldShipClass < iNewShipClass || iOldPrice < iNewPrice)
		{
			if (iMoney > iNewPrice+2000)
			{
				iOldCargo = GetCargoLoad(chr);
				iNewCargo = refShip.capacity;
				if (iNewCargo > iOldCargo)
				{
					chr.ship.type = GenerateShip(i, 1);
					SetBaseShipData(chr);
					AddMoneyToCharacter(chr, -iNewPrice);
					break;			
				}
			}
		}
	}
}

void CheckForDeadRanger(string sID)
{
	string sQuest = "Check_For_Dead_Ranger_" + GetCharacterIndex(sID);
	
	pchar.quest.(sQuest).over = "yes";
	
	GenerateRumour("ranger_dead", GetCharacterIndex(sId), sti(characters[GetCharacterIndex(sId)].type), 0);
	Map_ReleaseQuestEncounter("sId");
}

void CheckForDeadRangerFromPlayer(string sQuest)
{
	string sId = pchar.quest.(sQuest).ranger;
	GenerateRumour("ranger_dead", GetCharacterIndex(sId), sti(characters[GetCharacterIndex(sid)].type), 0);
	Map_ReleaseQuestEncounter("sId");
}

void RepairRangerShip(aref chr)
{
	int iShipType = sti(chr.ship.type);
	chr.ship.hp = RealShips[iShipType].hp;
	chr.ship.sp = 100;
	chr.ship.crew.quantity = RealShips[iShipType].maxcrew;
}

void WaitForRechargeRangerAdventure(aref chr)
{
	chr.isfreeforquest = 1;
	
	string sQuest = "wait_for_recharge_ranger_"+chr.id;
	pchar.quest.(sQuest).win_condition.l1 = "timer";
	pchar.quest.(sQuest).win_condition.l1.date.day = GetAddingDataDay(0, 0, 3);
	pchar.quest.(sQuest).win_condition.l1.date.month = GetAddingDataMonth(0, 0, 3);
	pchar.quest.(sQuest).win_condition.l1.date.year = GetAddingDataYear(0, 0, 3);
	pchar.quest.(sQuest).win_condition.l1.date.hour = rand(23);
	pchar.quest.(sQuest).win_condition = "WaitForRechargeRangerAdventure";
	pchar.quest.(sQuest).ranger = chr.id;

	chr.adventure.questsquantity = sti(chr.adventure.questsquantity) + 1;
}

void EndRangerAdventure(aref chr)
{
	chr.isfreeforquest = 0;
	
	string sQuest = "wait_for_recharge_ranger_"+chr.id;
	pchar.quest.(sQuest).over = "yes";
}

void PrepareRechargeRangerAdventure(string sQuest)
{
	int iRanger = GetCharacterIndex(pchar.quest.(sQuest).ranger);

	RechargeRangerAdventure(&characters[iRanger]);	
}

int FindRangerOnColony(string sColony)
{
	
	int iFirstRanger = GetCharacterIndex("ranger 0");
	string sQuest;
	for(int i = iFirstRanger; i<(iFirstRanger+iRangersRate); i++)
	{
		sQuest = "WaitForRechargeRangerAdventure" + characters[i].id;

		if(characters[i].colony == sColony)
		{
			if(CheckAttribute(pchar, "quest."+sQuest))
			{
				pchar.quest.(sQuest).over = "yes";
				return i;
			}
		}
	}

	return -1;
}

void Map_WarriorEnd()
{
	if(!CheckAttribute(pchar, "worldmap.shipcounter"))
	{
		return;
	}
	pchar.worldmap.shipcounter = sti(pchar.worldmap.shipcounter) - 1;
	if(sti(pchar.worldmap.shipcounter) < 0)
	{
		sti(pchar.worldmap.shipcounter) = 0;
	}
}
void Map_TraderSucces()
{
	if(!CheckAttribute(pchar, "worldmap.shipcounter"))
	{
		return;
	}
	pchar.worldmap.shipcounter = sti(pchar.worldmap.shipcounter) - 1;
	if(sti(pchar.worldmap.shipcounter) < 0)
	{
		sti(pchar.worldmap.shipcounter) = 0;
	}
	string sChar = GetEventData();
	if(GetCharacterIndex(sChar) != -1)
	{
		if(CheckAttribute(&characters[GetCharacterIndex(sChar)], "hovernor"))
		{
			if(sti(characters[GetCharacterIndex(sChar)].hovernor) == 1)
			{
				//  to_do MakeNewCapital(sChar);
				return;
			}
		}
	}
	else
	{
		trace("Can't find character with character id " + sChar);
		return;
	}

	if(sChar == "Head of Gold Squadron")
	{
		PrepareContinueGoldConvoy();
		return;
	}
	if(CheckAttribute(&characters[GetCharacterIndex(sChar)], "mapEnc.quest"))
	{
		int iQuest = sti(characters[GetCharacterIndex(sChar)].mapEnc.quest);
		string sQuest = "sink_ship_character_" + sChar + "_wait_before_travel";
		pchar.quest.(sQuest).win_condition.l1 = "timer";
		pchar.quest.(sQuest).win_condition.l1.date.day = GetAddingDataDay(0, 0, 2);
		pchar.quest.(sQuest).win_condition.l1.date.month = GetAddingDataMonth(0, 0, 2);
		pchar.quest.(sQuest).win_condition.l1.date.year = GetAddingDataYear(0, 0, 2);
		pchar.quest.(sQuest).win_condition.l1.date.hour = rand(23);
		pchar.quest.(sQuest).win_condition = "sink_ship_travel";
		pchar.quest.(sQuest).quest = iQuest;

		return;
	}
	RangerTimer(characterFromID(sChar));
}

void GenerateBattleForRanger(aref chr)
{
	if(chr.adventure.type == "in_battle")
	{
		return;
	}
	chr.adventure.type = "in_battle";
	/*

	int iEnemies = sti(chr.adventure.war.enemies);
	int iChar;
	string sGroup = "QuestRangerWar for ranger "+ chr.id;
	Group_CreateGroup(sGroup);
	string sEnemyIndex;
	for(int i = 0; i < iEnemies; i ++)
	{
		iChar = GenerateCharacter(sti(chr.adventure.war.enemynation), WITH_SHIP, "citizen", MAN, 1, WARRIOR);
		Group_AddCharacter(sGroup, characters[iChar].id);
		SetCharacterRelationBoth(iChar, sti(chr.index), RELATION_ENEMY);
		sEnemyIndex = "enemy" + i;
		chr.adventure.war.(sEnemyIndex) = iChar;
	}
	Group_SetGroupCommander(sGroup, characters[iChar].id);
	string sLocator = "quest_ship_" + (rand(2)+4);
	Group_SetAddress(sGroup, colonies[FindColony(chr.adventure.endColony)].island, "Quest_ships", sLocator);

	string sRangerGroup = chr.id + " group for war quest";
	Group_CreateGroup(sRangerGroup);
	for(int k = 1; k < COMPANION_MAX; k++)
	{
		iChar = GetCompanionIndex(chr, k);
		if(iChar != -1)
		{
			Group_AddCharacter(sRangerGroup, characters[iChar].id);
			SetCharacterRelationBoth(iChar, sti(chr.index), RELATION_FRIEND);
		}
	}
	Group_SetGroupCommander(sRangerGroup, chr.id);
	Group_SetAddress(sRangerGroup, colonies[FindColony(chr.adventure.endColony)].island, "Quest_ships", sLocator);

	Group_SetTaskAttack(sGroup, sRangerGroup);
	Group_LockTask(sGroup);
	UpdateRelations();*/

	string sQuest1;
	sQuest1 = "EndBattleForRanger " + chr.id + " on " + chr.adventure.endColony;
	pchar.quest.(sQuest1).win_condition.l1 = "timer";
	pchar.quest.(sQuest1).win_condition.l1.date.day = GetAddingDataDay(0, 0, 3);
	pchar.quest.(sQuest1).win_condition.l1.date.month = GetAddingDataMonth(0, 0, 3);
	pchar.quest.(sQuest1).win_condition.l1.date.year = GetAddingDataYear(0, 0, 3);
	pchar.quest.(sQuest1).win_condition.l1.date.hour = rand(23);
	pchar.quest.(sQuest1).win_condition = "EndBattleForRanger";
	pchar.quest.(sQuest1).ranger = chr.id;

	string sQuest2;
	sQuest2 = "BattleForRanger " + chr.id + " on " + chr.adventure.endColony;
	pchar.quest.(sQuest2).win_condition.l1 = "location";
	pchar.quest.(sQuest2).win_condition.l1.location = chr.adventure.endColony;
	pchar.quest.(sQuest2).win_condition = "BattleForRanger";
	pchar.quest.(sQuest2).ranger = chr.id;

	pchar.quest.(sQuest1).overquest = sQuest2;

	//trace("ranger " + chr.id + " begin a battle with  " + iEnemies + " enemies");
}

void EndBattleForRanger(string sQuest)
{
	string sRangerID = pchar.quest.(sQuest).ranger;
	ref chr = CharacterFromID(sRangerID);

	if(CheckAttribute(pchar, "quest." + sQuest + ".overquest"))
	{
		string sOverQuest = pchar.quest.(sQuest).overquest;
		pchar.quest.(sOverQuest).over = "yes";
	}

	if(CheckAttribute(pchar, "quest." + sQuest + ".enemygroup"))
	{
		Group_SetAddress(pchar.quest.(sQuest).enemygroup, "none", "Quest_ships", "quest_ship1");
		Group_DeleteGroup(pchar.quest.(sQuest).enemygroup);
	}
	if(CheckAttribute(pchar, "quest." + sQuest + ".rangergroup"))
	{
		Group_SetAddress(pchar.quest.(sQuest).rangergroup, "none", "Quest_ships", "quest_ship1");
		Group_DeleteGroup(pchar.quest.(sQuest).rangergroup);
	}
	 
	/*
	string sEnemyGroup = "QuestRangerWar for ranger " + sRangerID;

	string sRangerGroup = chr.id + " group for war quest";

	Group_DeleteGroup(sEnemyGroup);
	Group_DeleteGroup(sRangerGroup);

	int iEnemies = sti(chr.adventure.war.enemies);

	string sEnemyIndex;
	int iChar = 0;
	for(int i = 0; i < iEnemies; i ++)
	{
		sEnemyIndex = "enemy" + i;
		iChar = sti(chr.adventure.war.(sEnemyIndex));
		DeleteAttribute(&characters[iChar], "characterlock");
		EmptyFantomCharacacter(&characters[iChar]);
	}*/


	if(LAi_IsDead(chr) == false)
	{
		int iResult;
		iResult = rand(20);

		if(iResult > 5)
		{
			if(isPrisonedChar(chr) == false)
			{
				iResult = rand(20);
				if(iResult == 0)
				{
					LAi_KillCharacter(chr);
					//trace("ranger " + chr.id + " has been killed");
				}
				else
				{
					int iEnemies = rand(2) + 1;
					chr.adventure.war.sinkedships = (sti(chr.adventure.war.sinkedships) + iEnemies);
					AddCharacterExp(chr, iEnemies*rand(1000));
					//trace("ranger " + chr.id + " won battle");
					chr.adventure.type = "go_back";
					string stempColony = chr.colony;
					chr.colony = chr.adventure.endColony;
					chr.adventure.endColony = stempColony;
					chr.mapEnc.type = "trade";
					chr.mapEnc.worldMapShip = "ranger";
					Map_CreateTrader(chr.colony, chr.adventure.endColony, chr.id);
				}
			}
		}
	}
}

void BattleForRanger(string sQuest)
{
	string sRangerID = pchar.quest.(sQuest).ranger;
	ref chr = CharacterFromID(sRangerID);

	int iEnemies = sti(chr.adventure.war.enemies);
	int iChar;
	string sGroup = "QuestRangerWar for ranger "+ chr.id;
	Group_CreateGroup(sGroup);
	string sEnemyIndex;

	int iColony = FindColony(chr.adventure.endColony);
	for(int i = 0; i < iEnemies; i ++)
	{
		iChar = GenerateCharacter(sti(chr.adventure.war.enemynation), WITH_SHIP, "citizen", MAN, 0, WARRIOR);
		Group_AddCharacter(sGroup, characters[iChar].id);
		SetCharacterRelationBoth(iChar, sti(chr.index), RELATION_ENEMY);
		sEnemyIndex = "enemy" + i;
		chr.adventure.war.(sEnemyIndex) = iChar;
	}
	Group_SetGroupCommander(sGroup, characters[iChar].id);
	string sLocator = "quest_ship_" + (rand(2)+4);
	Group_SetAddress(sGroup, colonies[iColony].island, "Quest_ships", sLocator);

	string sRangerGroup = chr.id + " group for war quest";
	Group_CreateGroup(sRangerGroup);
	for(int k = 1; k < COMPANION_MAX; k++)
	{
		iChar = GetCompanionIndex(chr, k);
		if(iChar != -1)
		{
			Group_AddCharacter(sRangerGroup, characters[iChar].id);
			SetCharacterRelationBoth(iChar, sti(chr.index), RELATION_FRIEND);
		}
	}
	Group_SetGroupCommander(sRangerGroup, chr.id);
	Group_SetAddress(sRangerGroup, colonies[iColony].island, "Quest_ships", sLocator);

	Group_SetTaskAttack(sGroup, sRangerGroup);
	Group_LockTask(sGroup);
	UpdateRelations();

	sQuest = "DeleteRangerEncounterForRanger" + sRangerID;
	pchar.quest.(sQuest).win_condition.l1 = "ExitFromLocation";
	pchar.quest.(sQuest).win_condition.l1.location = colonies[iColony].island;
	pchar.quest.(sQuest).win_condition = "EndBattleForRanger";
	pchar.quest.(sQuest).ranger = sRangerID;
	pchar.quest.(sQuest).enemygroup = sGroup; 
	pchar.quest.(sQuest).rangergroup = sRangerGroup;
}
