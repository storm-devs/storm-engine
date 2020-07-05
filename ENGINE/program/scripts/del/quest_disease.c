
bool CheckForDisease(aref npchar) 
{
	string sLocation = npchar.location;
	string sColony = Locations[FindLocation(sLocation)].fastreload;
	int iColony = FindColony(sColony);

	if (Colonies[iColony].disease == "1" && sti(Colonies[iColony].disease.time) > 0)
	{
		return true;
	}
	return false;
}

void UpdateDisease()
{
	for (int i=0; i<MAX_COLONIES; i++)
	{
		if(sti(Colonies[i].disease.time) > 0)
		{
			Colonies[i].disease.time = sti(Colonies[i].disease.time) - 1;
			colonies[i].population = sti(colonies[i].population) - sti(Colonies[i].disease.time);
			if(sti(colonies[i].population) < 100)
			{
				colonies[i].population = 100;
			}
			ChangeReputationForColony(Colonies[i].id, -1);
			if (sti(Colonies[i].disease.time) == 0)
			{
				CureColony(i);
				ChangeReputationForColony(Colonies[i].id, 4);
			}
		}
	}
}

bool CheckForCure(aref npchar)
{
	
	string sColony = npchar.location;
	sColony = locations[FindLocation(sColony)].fastreload;
	int iColony = FindColony(sColony);
	int iTime = sti(colonies[iColony].disease.time);
	int iMedicine = GetCargoGoods(pchar, GOOD_MEDICAMENT);
	
	if(iMedicine >= iTime)
	{
		return true;
	}
	return false;
}

void DiseaseMissionInProgress(aref npchar)
{
	npchar.quest.disease = "inprogress";
	
	string sLocation = npchar.location;
	string sColony = Locations[FindLocation(sLocation)].fastreload;
	int iColony = FindColony(sColony);
	
	string slogbook = "DISEASE_ON_COLONY_" + sColony + "_" + rand(10000000);
	SetQuestHeaderEx(slogbook, "Disease");
	AddQuestUserDataForTitle(sLogBook, "sTargetColony", XI_ConvertString("colony"+sColony));
	AddQuestRecordEx(sLogBook, "Disease", "1");
	AddQuestUserData(sLogBook, "sTargetColony", XI_ConvertString("colony"+sColony));

	colonies[iColony].disease.logbook = slogbook;
}

void DiseaseMissionCompleted(aref npchar, int iType)
{
	npchar.quest.disease = "0";
	
	string sLocation = npchar.location;
	string sColony = Locations[FindLocation(sLocation)].fastreload;
	int iNation = sti(npchar.nation);
	
	int iColony = FindColony(sColony);
	string slogbook;
	if(CheckAttribute(&colonies[iColony], "disease.logbook"))
	{
		slogbook = colonies[iColony].disease.logbook;
	}
	else
	{
		slogbook = "DISEASE_ON_COLONY_" + sColony + "_" + rand(10000000);
	}
	
	SetQuestHeaderEx(slogbook, "Disease");
	AddQuestUserDataForTitle(sLogBook, "sTargetColony", XI_ConvertString("colony"+sColony+"Dat"));

	if (iType == 0)
	{
		AddQuestRecordEx(sLogBook, "Disease", "2");
			
		CureColony(FindColony(sColony));
	
		RemoveCharacterGoods(pchar, GOOD_MEDICAMENT, 10);
		AddPartyExp(pchar, 3000);
		AddMoneyToCharacter(pchar, 2500);
		ChangeCharacterReputation(pchar, 2, iNation);
		pchar.quest.questquantity = sti(pchar.quest.questquantity) + 1;
	}
	else
	{
		AddQuestRecordEx(sLogBook, "Disease", "3");
		AddPartyExp(pchar, 300);
		ChangeCharacterReputation(pchar, 1, iNation);
	}
	
	CloseQuestHeader(sLogBook);
	DeleteAttribute(&colonies[iColony], "disease.logbook");
}

void CureColony(int iColony)
{
	
	
	//int iColony = sti(pchar.quest.(qname).island);
	
	Colonies[iColony].disease = "0";
	Colonies[iColony].disease.time = "0";
	
	string sQuest = "contamination"+Colonies[iColony].id + rand(10000);
	pchar.quest.(sQuest).over = "yes";
	
	GenerateRumour("disease_end", iColony, sti(colonies[iColony].nation), 0);

	if(CheckAttribute(&colonies[iColony], "disease.logbook"))
	{
		string slogbook = colonies[iColony].disease.logbook;
		CloseQuestHeader(sLogBook);
		DeleteAttribute(&colonies[iColony], "disease.logbook");
	}

	DeleteAttribute(&Colonies[iColony], "Trade.Import.id4");
}

void CheckForContamination(string qname)
{
	int iColony = sti(pchar.quest.(qname).island);
	
	int iLuck = GetSummonSkillFromName(pchar, SKILL_SNEAK);
	int iDefence = GetSummonSkillFromName(pchar, SKILL_DEFENCE);
	
	int iCureTime, iChanceForContamination;
	
	iChanceForContamination = rand(12);
	if (iLuck < iChanceForContamination)
	{
		iCureTime = sti(colonies[iColony].disease.time);
		if (CheckAttribute(pchar, "ship.crew.disease"))
		{
			if (pchar.ship.crew.disease == "1")
			{
				if(sti(colonies[iColony].disease.time) > sti(pchar.ship.crew.disease.time))
				{
					iCureTime = sti(colonies[iColony].disease.time);
					pchar.ship.crew.disease = "1";
					pchar.ship.crew.disease.time = iCureTime;
				}
			}
		}
	}
}

void CheckForColonyContamination(string sColony)
{
	
	int iColony = FindColony(sColony);
	int iChanceForContamination = rand(50);
	if(!CheckAttribute(pchar, "ship.crew.disease.time"))
	{
		pchar.ship.crew.disease.time = 0;
	}
	int iDiseaseStr = sti(pchar.ship.crew.disease.time);

	if(iDiseaseStr < iChanceForContamination)
	{
		colonies[iColony].disease = 1;
		colonies[iColony].disease.time = (iDiseaseStr - rand(iDiseaseStr/2));
		colonies[iColony].disease.contamination = 1;
		UpdateDisease();

		ref rStore = &Stores[FindStore(Colonies[iColony].id)];
		
		SetStoreGoods(rStore, GOOD_MEDICAMENT, 0);
		Colonies[iColony].Trade.Import.id4 = GOOD_MEDICAMENT;
		
		string sQuest = "contamination" + sColony + rand(10000);
		pchar.quest.(sQuest).win_condition.l1 = "location";
		pchar.quest.(sQuest).win_condition.l1.location = Colonies[iColony].id + "_town";
		pchar.quest.(sQuest).win_condition = "Contamination";
		pchar.quest.(sQuest).island = iColony;

		sQuest = "contamination_alert_for_colony_" + sColony + "_" + rand(10000);
		pchar.quest.(sQuest).win_condition.l1 = "timer";
		pchar.quest.(sQuest).win_condition.l1.date.day = GetAddingDataDay(0, 0, 3);
		pchar.quest.(sQuest).win_condition.l1.date.month = GetAddingDataMonth(0, 0, 3);
		pchar.quest.(sQuest).win_condition.l1.date.year = GetAddingDataYear(0, 0, 3);
		pchar.quest.(sQuest).win_condition.l1.date.hour = rand(23);
		pchar.quest.(sQuest).win_condition = "end_of_contamination_alert_for_colony";
		pchar.quest.(sQuest).colony = sColony;
		
		GenerateRumour("disease_begin", iColony, sti(colonies[iColony].nation), 0);
	}

}

void ContaminationInProgress()
{
	
	int cn;
	
	if (CheckAttribute(pchar, "ship.crew.disease"))
	{
		if (pchar.ship.crew.disease == "1")
		{
			int crewQ;
			
			int iTimer = sti(pchar.ship.crew.disease.time);
			int iMedicine = 0;
			int iLuck, iDefence, iPercent;
			int iTimerTemp = 1;
			
			if (iTimer == 0)
			{
				pchar.ship.crew.disease = "0";
				pchar.ship.crew.disease.time = "0";
			}
			else
			{				
				for(int i=0; i<COMPANION_MAX; i++)
				{
					cn = GetCompanionIndex(pchar,i);
					if( cn>=1 && GetRemovable(&Characters[cn]) )
					{
						crewQ = GetCrewQuantity(GetCharacter(cn));
						
						iLuck = GetSummonSkillFromName(pchar, SKILL_SNEAK);
						iDefence = GetSummonSkillFromName(pchar, SKILL_DEFENCE);
				
						iPercent = 30 - iLuck - iDefence;
						iMedicine = GetCargoGoods(cn, GOOD_MEDICAMENT);
						
						crewQ = crewQ - crewQ*iPercent/100;
						
						if (iMedicine >= crewQ/4)
						{
							iTimerTemp = 3;
							iMedicine = iMedicine - crewQ/4;
						}
						if(GetCargoGoods(cn, GOOD_MEDICAMENT) >= iMedicine)
						{
							AddCharacterGoods(cn, GOOD_MEDICAMENT, iMedicine);
						}
						
						characters[cn].crew.quantity = crewQ;
					}
				}		
			}
			iTimer = iTimer - iTimerTemp;
			pchar.ship.crew.disease.time = iTimer;
			if(sti(pchar.ship.crew.disease.time) < 0)
			{
				pchar.ship.crew.disease.time = 0;
			}
			if(sti(pchar.ship.crew.disease.time) > 0)
			{
				LAi_SetDltHealth(pchar, -0.01);
			}
			else
			{
				LAi_SetDltHealth(pchar, 0.0);
			}
		}
	}
}

int GetDiseaseTime(aref npchar)
{
	string sLocation = npchar.location;
	string sColony = Locations[FindLocation(sLocation)].fastreload;
	int iColony = FindColony(sColony);

	int iDiseaseTime = 0;
	if(CheckAttribute(&colonies[iColony], "disease.time"))
	{
		iDiseaseTime = sti(colonies[iColony].disease.time);
	}

	return iDiseaseTime;
}

void EndContaminationAlert(string sQuest)
{
	string sColony = pchar.quest.(sQuest).colony;

	DeleteAttribute(&colonies[FindColony(sColony)], "disease.contamination");
}

int GetShipDiseaseTime(aref chr)
{
	int iDiseaseTime = 0;
	if(CheckAttribute(chr, "ship.crew.disease.time"))
	{
		if(sti(chr.ship.crew.disease) == 1)
		{
			iDiseaseTime = sti(chr.ship.crew.disease.time);
		}
	}
	return iDiseaseTime;
}
