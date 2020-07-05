void NetClient_DeleteIslandEnvironment()
{
	DeleteClass(&NCIsland);
	DeleteClass(&NCCoastFoam);
}

void NetClient_CreateIslandEnvironment(int iMsg)
{
	string sIslandID = NCServer.IslandID;
	if (sIslandID == "") { return; }

	int iIslandIndex = Net_GetIslandIndex(sIslandID);
	if (iIslandIndex == -1) { return; }

	NCIsland.Server = false;
	CreateEntity(&NCIsland, "NetIsland");
	NCIsland.LightingPath = NetClient_WhrGetLightingPath();
	NCIsland.ImmersionDistance = Islands[iIslandIndex].ImmersionDistance;	// distance = fRadius * ImmersionDistance, from island begin immersion
	NCIsland.ImmersionDepth = Islands[iIslandIndex].ImmersionDepth;			// immersion depth = (Distance2Camera / (fRadius * ImmersionDistance) - 1.0) * ImmersionDepth
	string sTexturePath = "islands\" + Islands[iIslandIndex].TexturePath + "\";
	SetTexturePath(0, sTexturePath);
	NCIsland.FogDensity = NCWeather.Fog.IslandDensity;

	SendMessage(&NCIsland, "lsss", MSG_ISLAND_LOAD_GEO, "islands", Islands[iIslandIndex].filespath.models, Islands[iIslandIndex].model);
	LayerAddObject("net_realize", &NCIsland, 3);
	LayerAddObject("sun_trace", &NCIsland, 1);

	SetTexturePath(0, "");

	NCCoastFoam.Id = sIslandID;
	NCCoastFoam.Server = false;
	CreateEntity(&NCCoastFoam, "NetCoastFoam");
	LayerAddObject("net_realize", &NCCoastFoam, 65534);
	LayerAddObject("net_execute", &NCCoastFoam, 65535);
}