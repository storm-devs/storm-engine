object BIManager;

#event_handler(EVENT_LOCATION_LOAD,"BIManager_TestStart");
#event_handler(EVENT_LOCATION_UNLOAD,"BIManager_TestRelease");

void BIManager_TestStart()
{
	BIManager_Init();
	BIManager_StartLand();
}

void BIManager_TestRelease()
{
	BIManager_Init();
	BIManager_StartLand();
}

void BIManager_Init()
{
	DeleteAttribute(&BIManager,"");

	BIManager.cursors.size = "32,32";
	BIManager.cursors.startpos = "0,0";
	BIManager.cursors.sensivity = "8,8";

	BIManager.cursors.list.c0.index = 0;
	BIManager.cursors.list.c0.xoffset = 0;
	BIManager.cursors.list.c0.yoffset = 0;
	BIManager.cursors.list.c0.texture = "INTERFACES\cursor32.tga";
	BIManager.cursors.list.c0.uv = "0.0,0.0,0.99,0.99";

	CreateEntity(&BIManager,"BI_InterfaceManager");
}

void BIManager_Release()
{
	DeleteClass(&BIManager);
}

void BIManager_StartSea()
{
	LayerAddObject(SEA_EXECUTE,&BIManager,-1);
	LayerAddObject(SEA_REALIZE,&BIManager,-1);
}

void BIManager_EndSea()
{
	LayerDelObject(SEA_EXECUTE,&BIManager);
	LayerDelObject(SEA_REALIZE,&BIManager);
}

void BIManager_StartLand()
{
	LayerAddObject("execute",&BIManager,-1);
	LayerAddObject("realize",&BIManager,-1);
}

void BIManager_EndLand()
{
	LayerDelObject("execute",&BIManager);
	LayerDelObject("realize",&BIManager);
}
