object NCFortModel, NCFortBlots;

void NetClient_DeleteFortEnvironment()
{
	DeleteClass(NetClient_GetCLient(NET_FORTCLIENT));
	DeleteClass(&NCFortModel);
	DeleteClass(&NCFortBlots);
}

void NetClient_CreateFortEnvironment(int iMsg)
{
	bool bFortLoad = NMGetByte(iMsg);
	if (!bFortLoad) { return; }

	int iIslandIndex = Net_GetIslandIndex(NCServer.IslandID);
	if (iIslandIndex == -1) { return; }

	string sLoc = NMGetString(iMsg);

	ref rIsland = Net_GetIslandByIndex(iIslandIndex);
	aref arReload; makearef(arReload, rIsland.reload);

	if (!CheckAttribute(arReload, sLoc)) { return; }
	aref arLocator; makearef(arLocator, arReload.(sLoc));
	if (!CheckAttribute(arLocator, "fort.model") || arLocator.fort.model == "") { return; }

	ref rFort = NetClient_GetClient(NET_FORTCLIENT);

	rFort.Server = false;
	CreateEntity(rFort, "NetFort");
	LayerAddObject("net_realize", rFort, 10000);
	LayerAddObject("sun_trace", rFort, 1);

	// create fort model
	CreateEntity(&NCFortModel, "MODELR");
	string sNCFortModel = rIsland.filespath.models + "\" + arLocator.fort.model;

	SetTexturePath(0, rIsland.filespath.models + "\");

	SendMessage(&NCFortModel, "ls", MSG_MODEL_SET_LIGHT_PATH, NetClient_WhrGetLightingPath());
	SendMessage(&NCFortModel, "ls", MSG_MODEL_LOAD_GEO, sNCFortModel);
	SendMessage(&Island, "li", MSG_ISLAND_ADD_FORT,  &NCFortModel);
	LayerAddObject("fort_cannon_trace", &NCFortModel, 1);

	SetTexturePath(0, "");

	// create fort blot
	/*CreateEntity(&NCFortBlots, "blots");
	SendMessage(&NCFortBlots, "lia", MSG_BLOTS_SETMODEL, &NCFortModel, NCFort);
	LayerAddObject("net_execute", &NCFortBlots, 10001);
	LayerAddObject("net_realize", &NCFortBlots, 10001);*/

	SendMessage(rFort, "laaii", MSG_NET_ADD_FORT, rIsland, arLocator, &NCFortModel, &NCFortBlots);
}

void NetClient_OnFortCannonDestroy(int iMsg)
{
	float x = NMGetFloat(iMsg);
	float y = NMGetFloat(iMsg);
	float z = NMGetFloat(iMsg);

	//string sFortFireParticle = "FortFire";
	//if (rand(2) == 1) { sFortFireParticle = "ShipFire"; }
	//CreateParticleSystem(sFortFireParticle, x, y, z, NetClient_WhrGetWindSpeed() / 48.0, NetClient_WhrGetWindAngle(), 0.0, 0);

	//CreateParticleSystem("fort_fire" + rand(2), x, y, z, NetClient_WhrGetWindSpeed() / 48.0, NetClient_WhrGetWindAngle(), 0.0, 0);

	string sFortFireParticle = "FortFire_net1";
	int iRand = rand(6);
	if (iRand == 3) { sFortFireParticle = "FortFire_net0"; }
	if (iRand == 2) { sFortFireParticle = "FortFire_net2"; }
	CreateParticleSystem(sFortFireParticle, x, y, z, NetClient_WhrGetWindSpeed() / 48.0, NetClient_WhrGetWindAngle(), 0.0, 0);
}

void NetClient_OnFort(int iMsg)
{
	int iSubCode = NMGetByte(iMsg);

	switch (iSubCode)
	{
		case NSC_FORT_CANNON_DESTROY:		
			NetClient_OnFortCannonDestroy(iMsg); 		
		break;
	}
}