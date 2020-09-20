object NSSea;

void NetServer_WhrDeleteSeaEnvironment()
{
	if (isEntity(&NSSea)) { DeleteClass(&NSSea); }
	DeleteAttribute(&NSSea, "");
}

void NetServer_WhrCreateSeaEnvironment()
{
	aref	aCurWeather = NetServer_GetCurrentWeather();
	aref	aSea; makearef(aSea, aCurWeather.Sea);
	aref	aSea2; makearef(aSea2, aCurWeather.Sea2);

	NSSea.Server = true;
	NSSea.SimpleSea = true;	// must be before creating, better to set true, because its eat less video memory
	CreateEntity(&NSSea, "NetSea");

	// Advanced Sea initialize
	NSSea.Sea2.WaterColor = Whr_GetColor(aSea2, "WaterColor");
	NSSea.Sea2.SkyColor = Whr_GetColor(aSea2, "SkyColor");

	NSSea.Sea2.Amp1 = Whr_GetFloat(aSea2, "Amp1");
	NSSea.Sea2.AnimSpeed1 = Whr_GetFloat(aSea2, "AnimSpeed1");
	NSSea.Sea2.Scale1 = Whr_GetFloat(aSea2, "Scale1");
	NSSea.Sea2.MoveSpeed1 = Whr_GetString(aSea2, "MoveSpeed1");

	NSSea.Sea2.Amp2 = Whr_GetFloat(aSea2, "Amp2");
	NSSea.Sea2.AnimSpeed2 = Whr_GetFloat(aSea2, "AnimSpeed2");
	NSSea.Sea2.Scale2 = Whr_GetFloat(aSea2, "Scale2");
	NSSea.Sea2.MoveSpeed2 = Whr_GetString(aSea2, "MoveSpeed2");

	NSSea.Sea2.PosShift = Whr_GetFloat(aSea2, "PosShift");
	NSSea.Sea2.BumpScale = Whr_GetFloat(aSea2, "BumpScale");

	NSSea.Sea2.Reflection = Whr_GetFloat(aSea2, "Reflection");
	NSSea.Sea2.Transparency = Whr_GetFloat(aSea2, "Transparency");
	NSSea.Sea2.Attenuation = Whr_GetFloat(aSea2, "Attenuation");
	NSSea.Sea2.Frenel = Whr_GetFloat(aSea2, "Frenel");
		
	NSSea.MaxSeaHeight = 50.0;
	NSSea.isDone = "";
}

void NetServer_MoveSeaToLayers(string sExecuteLayer, string sRealizeLayer)
{
	LayerAddObject(sExecuteLayer, &NSSea, 65530);
	LayerAddObject(sRealizeLayer, &NSSea, 65530);
}
