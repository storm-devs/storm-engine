object NSIsland;

void NetServer_DeleteIslandEnvironment()
{
	DeleteClass(&NSIsland);
}

void NetServer_CreateIslandEnvironment(int iSMsg)
{
	if (NetServer.IslandID == "") { return; }
	int iIslandIndex = Net_GetIslandIndex(NetServer.IslandID);
	if (iIslandIndex != -1)
	{
		NSIsland.Server = true;
		CreateEntity(&NSIsland, "NetIsland");
		NSIsland.LightingPath = NetServer_WhrGetLightingPath();
		NSIsland.ImmersionDistance = Islands[iIslandIndex].ImmersionDistance;			// distance = fRadius * ImmersionDistance, from island begin immersion
		NSIsland.ImmersionDepth = Islands[iIslandIndex].ImmersionDepth;			// immersion depth = (Distance2Camera / (fRadius * ImmersionDistance) - 1.0) * ImmersionDepth
		string sTexturePath = "islands\" + Islands[iIslandIndex].TexturePath + "\";
		SetTexturePath(0, sTexturePath);

		SendMessage(&NSIsland, "lsss", MSG_ISLAND_LOAD_GEO, "islands", Islands[iIslandIndex].filespath.models, Islands[iIslandIndex].model);
		LayerAddObject("mast_island_trace", &NSIsland, 1);

		//Fort_Login(iIslandIndex);	
		SetTexturePath(0, "");
	}

}