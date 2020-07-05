#include "encounters\encounters.h"
#include "encounters\encounters_map.c"
#include "encounters\encounters_quest.c"

//#include "scripts\utils.c"

#define MAX_MAP_ENCOUNTERS		128
object MapEncounters[MAX_MAP_ENCOUNTERS];

extern void InitEncounters();

void EncountersInit()
{
	if(LoadSegment("Encounters\Encounters_init.c"))
	{
		InitEncounters();
		UnloadSegment("Encounters\Encounters_init.c");
	}

	ReleaseMapEncounters();
}

int FindWarEncounter()
{
	int iTypes[100];
	int iNumTypes = 0;

	ref rCharacter = GetMainCharacter();
	int iCharacterRank = sti(rCharacter.rank);

	int iChance = rand(250);// boal 13.03.2004

	for (int i=0; i<MAX_ENCOUNTER_TYPES; i++)
	{
		if (sti(EncountersTypes[i].Type) == ENCOUNTER_WAR)
		{
			if (sti(EncountersTypes[i].Skip)) { continue; }
			// check chance
			if (iChance > sti(EncountersTypes[i].Chance)) { continue; }
			// check MinRank / MaxRank
			if (sti(EncountersTypes[i].MinRank) <= iCharacterRank && sti(EncountersTypes[i].MaxRank) >= iCharacterRank)
			{
				iTypes[iNumTypes] = i;
				iNumTypes++;
			}
		}
	}
	if (iNumTypes == 0) 
	{
		//Trace("FindWarEncounter: not find any war encounters!");
		return -1;
	}
	return iTypes[rand(iNumTypes-1)];
}

int FindMerchantEncounter()
{
	int iTypes[100];
	int iNumTypes = 0;

	ref rCharacter = GetMainCharacter();
	int iCharacterRank = sti(rCharacter.rank);

	int iChance = rand(250);// boal 13.03.2004

	for (int i=0; i<MAX_ENCOUNTER_TYPES; i++)
	{
		if (sti(EncountersTypes[i].Type) == ENCOUNTER_TRADE)
		{
            if (sti(EncountersTypes[i].Skip)) { continue; } // fix
			// check chance
			if (iChance > sti(EncountersTypes[i].Chance)) continue;
			// check MinRank / MaxRank
			if (sti(EncountersTypes[i].MinRank) <= iCharacterRank && sti(EncountersTypes[i].MaxRank) >= iCharacterRank)
			{
				iTypes[iNumTypes] = i;
				iNumTypes++;
			}
		}
	}
	if (iNumTypes == 0) 
	{
		//Trace("FindMerchantEncounter: not find any merchant!");
		return -1;
	}
	return iTypes[rand(iNumTypes - 1)];
}

bool Encounter_CanNation(int iEncounter, int iNation)
{
	string sNation = "Nation.Exclude." + Nations[iNation].Name;
	if (!CheckAttribute(&EncountersTypes[iEncounter], sNation)) return true;
	return !sti(EncountersTypes[iEncounter].(sNation));
}

bool Encounter_GetClassesFromRank(int iEncounter, int iRank, ref rMClassMin, ref rMClassMax, ref rWClassMin, ref rWClassMax)
{
	ref rEnc = &EncountersTypes[iEncounter];

	rMClassMin = 0; rMClassMax = 0; 
	rWClassMin = 0; rWClassMax = 0;

	string sRank = "Rank." + iRank;
	if (!CheckAttribute(rEnc, sRank))
	{
		// find nearest rank 
		if (CheckAttribute(rEnc, "Rank"))
		{
			int iLastRank = -1;
			int iBestRank = 1000;

			aref aRanks; makearef(aRanks, rEnc.Rank);
			int iNumRanks = GetAttributesNum(aRanks);
			for (int i=0; i<iNumRanks; i++)
			{
				aref aRank = GetAttributeN(aRanks, i);
				string sName = GetAttributeName(aRank);
				int iCurRank = sti(sName);
				if (abs(iRank - iCurRank) < iBestRank)
				{
					iBestRank = abs(iRank - iCurRank);
					iLastRank = iCurRank;
				}
			}
			if (iLastRank == -1) { return false; }
			sRank = "Rank." + iLastRank;
		}
		else
		{
			return false;
		}
	}

	rMClassMin = rEnc.(sRank).1;
	rMClassMax = rEnc.(sRank).0;

	rWClassMin = rEnc.(sRank).3;
	rWClassMax = rEnc.(sRank).2;
	
	return true;
}