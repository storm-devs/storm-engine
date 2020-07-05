object NCRain;

void NetClient_WhrDeleteRainEnvironment()
{
	if (isEntity(&NCRain))
	{
		DeleteClass(&NCRain);
	}
	DeleteAttribute(&NCRain,"");
}

void NetClient_WhrCreateRainEnvironment()
{
	aref aCurWeather = NetClient_GetCurrentWeather();
	aref aRain; 	makearef(aRain, aCurWeather.Rain);
	aref aRainbow; 	makearef(aRainbow, aCurWeather.Rainbow);

	NCRain.Clear = "";
	DeleteAttribute(&NCRain, "");

	bClientWeatherIsRain = false;
	if (sti(aRain.NumDrops) == 0) return;

	NCRain.Server = false;
	CreateEntity(&NCRain, "NetRain");

	NCRain.NumDrops = Whr_GetLong(aRain, "NumDrops");
	NCRain.Color = Whr_GetColor(aRain, "Color");
	NCRain.DropLength = Whr_GetFloat(aRain, "DropLength");
	NCRain.Height = Whr_GetFloat(aRain, "Height");
	NCRain.Radius = Whr_GetFloat(aRain, "Radius");
	NCRain.Speed = Whr_GetFloat(aRain, "Speed");
	NCRain.Jitter = Whr_GetFloat(aRain, "Jitter");
	NCRain.WindSpeedJitter = Whr_GetFloat(aRain, "WindSpeedJitter");
	NCRain.TimeBlend = Whr_GetLong(aRain, "TimeBlend");
	NCRain.MaxBlend = Whr_GetLong(aRain, "MaxBlend");

	NCRain.Rainbow.Enable = Whr_GetLong(aRainbow, "Enable");
	NCRain.Rainbow.Texture = Whr_GetString(aRainbow, "Texture");
	
	NCRain.isDone = "";

	bClientWeatherIsRain = true;
}

void NetClient_MoveRainToLayers(string sExecuteLayer, string sRealizeLayer)
{
	LayerAddObject(sExecuteLayer, &NCRain, 65537);
	LayerAddObject(sRealizeLayer, &NCRain, 65537);
}
