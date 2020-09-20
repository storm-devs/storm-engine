int iNumNetIslands = 0;
int iNetIslandsIndex[MAX_ISLANDS];

ref Net_GetIsland(string sIslandID)
{
	return GetIslandByID(sIslandID);
}
ref Net_GetIslandByIndex(int iIndex) { return GetIslandByIndex(iNetIslandsIndex[iIndex]); }

int Net_GetIslandsNum() { return iNumNetIslands; }
int Net_GetIslandIndex(string sIslandID)
{
	return FindIsland(sIslandID);
}

bool Net_IsIslandHaveForts(ref rIsland)
{
	aref arReload; makearef(arReload, rIsland.reload);
	int iNumReloads = GetAttributesNum(arReload);
	for (int i=0; i<iNumReloads; i++)
	{
		aref arLocator = GetAttributeN(arReload, i);
		if (CheckAttribute(arLocator, "fort")) { return true; }
	}

	return false;
}

int Net_IslandGetNumForts(ref rIsland)
{
	int iNumForts = 0;
	aref arReload; makearef(arReload, rIsland.reload);
	int iNumReloads = GetAttributesNum(arReload);
	for (int i=0; i<iNumReloads; i++)
	{
		aref arLocator = GetAttributeN(arReload, i);
		if (CheckAttribute(arLocator, "fort")) { iNumForts++; }
	}

	return iNumForts;
}

void Net_InitIslands()
{
	if (iNumIslands == 0 || iNumNetIslands == 0)
	{
		IslandsInit();

		for (int i=0; i<iNumIslands; i++)
		{
			ref rIsland = GetIslandByIndex(i);

			if (sti(rIsland.Net))
			{
				iNetIslandsIndex[iNumNetIslands] = i;
				iNumNetIslands++;
			}
		}
	}
}