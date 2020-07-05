object NCShipLights;

void NetClient_DeleteShipLightsEnvironment()
{
	DeleteClass(&NCShipLights);
}

void NetClient_CreateShipLightsEnvironment(int iMsg)
{
	if (LoadSegment("sea_ai\ShipLights.c"))
	{
		InitShipLights();
		UnloadSegment("sea_ai\ShipLights.c");
	}

	CopyAttributes(&NCShipLights, &ShipLights);

	NCShipLights.Server = false;
	CreateEntity(&NCShipLights, "NetShipLights");
	LayerAddObject("net_execute", &NCShipLights, 0);
	LayerAddObject("net_realize", &NCShipLights, -1);
	LayerAddObject("sea_sunroad", &NCShipLights, -1);
}