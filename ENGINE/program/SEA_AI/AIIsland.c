void Island_Start()
{
	SendMessage(&Island, "l", MSG_ISLAND_START);
}

void Island_Reload(string sIslandID, string sNewIslandModel)
{
	int iIslandID = FindIsland(sIslandID);
	if (iIslandID < 0) 
	{
		return;
	}
	ref rIsland = GetIslandByIndex(iIslandID);
	rIsland.model = sNewIslandModel;
	if (bSeaActive)
	{
		SendMessage(&Island, "lss", MSG_ISLAND_LOAD_GEO, "islands", sNewIslandModel);
	}
}

//navy -->
string Island_FindNearestLocator(string group, float x, float z)
{
	ref rIsland;
	aref arGroup, arLocator;
	string retStr = "";
	float fMin = 4000;
	float fDist;
	int i, iNum;
	int iIslandID = FindIsland(PChar.location);
	if (iIslandID < 0) 
	{
		return retStr;
	}

	makearef(arGroup, Islands[iIslandID].(group));
	iNum = GetAttributesNum(arGroup);
	if (iNum < 1) return retStr;

	for (i = 0; i < iNum; i++)
	{
		arLocator = GetAttributeN(arGroup, i);
		fDist = GetDistance2D(x, z, stf(arLocator.x), stf(arLocator.z));
		if (fDist < fMin) 
		{
			fMin = fDist;
			retStr = GetAttributeName(arLocator);
		}
	}
	return retStr;
}
string Island_FindNearestLocator2PChar(string group)
{
	return Island_FindNearestLocator(group, stf(pchar.Ship.Pos.x), stf(pchar.Ship.Pos.z))
}
//navy <--