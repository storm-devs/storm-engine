object Lightning;

#define WHR_LIGHTNING			"whr_lightn"
#define WHR_LIGHTNING_SOUND		"whr_lightn_snd"
#define WHR_LIGHTNING_DOIT		"whr_lightn_doit"

float fLightningScaleX,	fLightningScaleY;

void WhrDeleteLightningEnvironment()
{
	if (isEntity(&Lightning)) 
	{ 
		DeleteClass(&Lightning);
		DeleteAttribute(&Lightning,"");
	}

	DelEventHandler(WHR_LIGHTNING_SOUND,"Lightning_Sound");
	DelEventHandler(WHR_LIGHTNING_DOIT,"Lightning_DoIt");
}

void WhrCreateLightningEnvironment()
{
	aref aCurWeather = GetCurrentWeather();
	aref aLightning; 	makearef(aLightning,aCurWeather.Lightning);
	
	DeleteAttribute(&Lightning,"");
	Lightning.Clear = "";
	DelEventHandler(WHR_LIGHTNING_DOIT,"Lightning_DoIt");

	if (sti(aLightning.Enable) != true) return;

	if (!isEntity(&Lightning)) 
	{ 
		CreateEntity(&Lightning,"Lightning");
	}

	LayerAddObject("sea_reflection", &Lightning, 10);

	SetEventHandler(WHR_LIGHTNING_SOUND,"Lightning_Sound",0);
	SetEventHandler(WHR_LIGHTNING_DOIT,"Lightning_DoIt",0);

	Lightning.Texture = Whr_GetString(aLightning,"Texture");
	Lightning.Flash.Texture = Whr_GetString(aLightning,"Flash.Texture");
	Lightning.SubTexX = Whr_GetLong(aLightning,"SubTexX");
	Lightning.SubTexY = Whr_GetLong(aLightning,"SubTexY");
	fLightningScaleX = Whr_GetFloat(aLightning,"ScaleX");
	fLightningScaleY = Whr_GetFloat(aLightning,"ScaleY");
	Lightning.isDone = "";

	PostEvent(WHR_LIGHTNING_DOIT,1000 + rand(1000));
}

void MoveLightningToLayers(string sExecuteLayer, string sRealizeLayer)
{
	LayerDelObject("execute",&Lightning);
	LayerDelObject("realize",&Lightning);
	LayerDelObject(SEA_EXECUTE,&Lightning);
	LayerDelObject(SEA_REALIZE,&Lightning);

	LayerAddObject(sExecuteLayer, &Lightning, -2);
	LayerAddObject(sRealizeLayer, &Lightning, -2);
}

void Lightning_DoIt()
{
	if (!isEntity(&Lightning)) { return; }

	// next lightning
	PostEvent(WHR_LIGHTNING_DOIT, 200 + rand(1200));
	
	// if interface launched, return
	if (sti(InterfaceStates.Launched) && CurrentInterface != INTERFACE_MAINMENU) { return; }

	aref aCurWeather = GetCurrentWeather();

	int iTimeLightning = 1000;

	float cx = stf(Camera.Pos.x);
	float cy = stf(Camera.Pos.y);
	float cz = stf(Camera.Pos.z);

	float fDist = 1000.0 + frnd() * 2000.0;
	if (rand(30) == 15) { fDist = 20.0 + frnd() * 200.0; }	// nearest lightning
	float fAngle = frnd() * PIm2;
	int iTimeSound = fDist / 333.0;

	float fTime = frnd() * 0.1 + 0.1;
	float x = cx + fDist * sin(fAngle);
	float y = 590.0;
	float z = cz + fDist * cos(fAngle);

	int iFlickerTime = Whr_GetLong(aCurWeather, "Lightning.FlickerTime");

	int iSubTexture = rand(sti(Lightning.SubTexX) * sti(Lightning.SubTexY) - 1);

	float fFlashSize = 1500.0;
	float fLightningSize = 600.0;
	float fScaleX = fLightningScaleX;
	float fScaleY = fLightningScaleY;
	
	if (fDist < 1000.0)
	{
		fLightningSize = 600.0 * fDist / 1000.0;
		if (fLightningSize < 25) { fLightningSize = 25.0; }
		y = fLightningSize - 10.0 * fDist / 1000.0;
		//fScaleY = fScaleY * 600.0 / fLightningSize;
	}
	
	SendMessage(&Lightning, "llsflffffffsff", MSG_WHR_LIGHTNING_ADD, iSubTexture, "lightning", fTime, iFlickerTime, fLightningSize, fScaleX, fScaleY, x, y, z, "flash_lightning", fFlashSize, fTime / 2.0);

	bool bSound = false;
	if (fDist < 120.0)	{ bSound = true; }
	if (rand(3) == 1)	{ bSound = true; }

	if (bSound) { PostEvent(WHR_LIGHTNING_SOUND, iTimeSound, "fff", x, y, z); }

	Event(WHR_LIGHTNING, "fff", x, y, z);
}

void Lightning_Sound()
{	//if (rand(4) == 1)
	//	{
			float x = GetEventData();
			float y = GetEventData();
			float z = GetEventData();
			Play3DSound("thunder", x, y, z);
	//	}
}
