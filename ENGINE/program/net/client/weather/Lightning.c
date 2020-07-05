object NCLightning;
float fClientLightningScaleX;
float fClientLightningScaleY;

void NetClient_WhrDeleteLightningEnvironment()
{
	if (isEntity(&Lightning)) 
	{ 
		DeleteClass(&NCLightning);
		DeleteAttribute(&NCLightning, "");
	}

	DelEventHandler("NetClient_LightningSound", "NetClient_LightningSound");
	DelEventHandler("NetClient_LightningDoIt", "NetClient_LightningDoIt");
}

void NetClient_WhrCreateLightningEnvironment()
{
	aref aCurWeather = NetClient_GetCurrentWeather();
	aref aLightning; makearef(aLightning,aCurWeather.Lightning);

	DeleteAttribute(&NCLightning, "");
	NCLightning.Clear = "";

	if (sti(aLightning.Enable) != true) return;

	NCLightning.Server = false;
	CreateEntity(&NCLightning, "NetLightning");

	LayerAddObject("sea_reflection", &NCLightning, 10);

	SetEventHandler("NetClient_LightningSound", "NetClient_LightningSound", 0);
	SetEventHandler("NetClient_LightningDoIt", "NetClient_LightningDoIt", 0);

	NCLightning.Texture = Whr_GetString(aLightning, "Texture");
	NCLightning.Flash.Texture = Whr_GetString(aLightning, "Flash.Texture");
	NCLightning.SubTexX = Whr_GetLong(aLightning, "SubTexX");
	NCLightning.SubTexY = Whr_GetLong(aLightning, "SubTexY");
	fClientLightningScaleX = Whr_GetFloat(aLightning, "ScaleX");
	fClientLightningScaleY = Whr_GetFloat(aLightning, "ScaleY");
	NCLightning.isDone = "";
}

void NetClient_LightningAdd(int iMsg)
{
	int iTime = NMGetDword(iMsg);
	int iSubTexture = NMGetWord(iMsg);
	float fLightningTime = NMGetFloat(iMsg);
	float x = NMGetFloat(iMsg);
	float y = NMGetFloat(iMsg);
	float z = NMGetFloat(iMsg);

	int iTime2Lightning = iTime - iClientTime;
	if (iTime2Lightning < 0) { iTime2Lightning = 0;	}

	PostEvent("NetClient_LightningDoIt", iTime2Lightning, "flfff", fLightningTime, iSubTexture, x, y, z);
	//Trace("Client lightning doit : " + x + ", " + y + ", " + z + ", iTime2Lightning = " + iTime2Lightning);
}

void NetClient_LightningSound()
{
	float x = GetEventData();
	float y = GetEventData();
	float z = GetEventData(	);

	Play3DSound("thunder", x, y, z);
}

void NetClient_LightningDoIt()
{
	aref aCurWeather = NetClient_GetCurrentWeather();

	float fTime = GetEventData();
	int iSubTexture = GetEventData();

	float x = GetEventData();
	float y = GetEventData();
	float z = GetEventData();

	int iFlickerTime = Whr_GetLong(aCurWeather, "Lightning.FlickerTime");

	float fFlashSize = 1500.0;
	float fLightningSize = 600.0;
	float fScaleX = fClientLightningScaleX;
	float fScaleY = fClientLightningScaleY;

	SendMessage(&NCLightning, "llsflffffffsff", MSG_WHR_LIGHTNING_ADD, iSubTexture, "lightning", fTime, iFlickerTime, fLightningSize, fScaleX, fScaleY, x, y, z, "flash_lightning", fFlashSize, fTime / 2.0);

	float fDistance = GetDistance2D(x, stf(Camera.Pos.x), z, stf(Camera.Pos.z));

	bool bSound = false;
	if (fDistance < 300.0) { bSound = true; }
	if (rand(2) == 1) { bSound = true; }
	if (bSound) { PostEvent("NetClient_LightningSound", 1000.0 * fDistance / 333.0, "fff", x, y, z); }
}

void NetClient_MoveLightningToLayers(string sExecuteLayer, string sRealizeLayer)
{
	LayerAddObject(sExecuteLayer, &NCLightning, -2);
	LayerAddObject(sRealizeLayer, &NCLightning, -2);
}
