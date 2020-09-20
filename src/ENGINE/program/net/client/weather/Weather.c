#include "net\client\weather\Lightning.c"
#include "net\client\weather\Rain.c"
#include "net\client\weather\Sea.c"
#include "net\client\weather\Sky.c"
#include "net\client\weather\Sun.c"
#include "net\client\weather\Tornado.c"
#include "net\client\weather\Astronomy.c"

object	NCWeather;
int		iClientCurWeatherNum = -1;
string	sClientCurrentFog;
bool	bClientWeatherIsNight = false;
bool	bClientWeatherIsLight = false;
bool	bClientWeatherIsRain = false;
bool	bClientWeatherIsStorm = false;
string	sClientLightingPath;

//#event_handler("EWhr_GetWindAngle", "NetClient_WhrGetWindAngle");
//#event_handler("EWhr_GetWindSpeed", "NetClient_WhrGetWindSpeed");
//#event_handler("EWhr_GetFogDensity", "NetClient_WhrGetFogDensity");

string NetClient_WhrGetCurrentFog()
{
	return sClientCurrentFog;
}

void NetClient_DeleteWeatherEnvironment()
{
	if (isEntity(&NCWeather))
	{
		DeleteClass(&NCWeather);
	}
	DeleteAttribute(&NCWeather, "");

	NetClient_WhrDeleteRainEnvironment();
	NetClient_WhrDeleteSkyEnvironment();
	NetClient_WhrDeleteSunGlowEnvironment();
	NetClient_WhrDeleteLightningEnvironment();
	//NetClient_WhrDeleteTornadoEnvironment();
	NetClient_WhrDeleteSeaEnvironment();
	NetClient_WhrDeleteAstronomyEnvironment();

	DelEventHandler("NetClient_WhrOnCalcFogColor", "NetClient_WhrOnCalcFogColor");
	DelEventHandler("frame", "Whr_OnWindChange");

	bWeatherLoaded = false;
}

aref NetClient_GetCurrentWeather()
{
	aref arWeather; 
	makearef(arWeather, Weathers[iClientCurWeatherNum]);
	return arWeather;
}

void NetClient_CreateWeatherEnvironment(int iMsg)
{
	iClientCurWeatherNum = 0;

	string sWeatherID = NMGetString(iMsg);

	for (int i=0; i<iTotalNumWeathers; i++) 
	{
		if (sWeatherID == Weathers[i].id)
		{
			iClientCurWeatherNum = i;
			break;
		}
	}

	if (iClientCurWeatherNum < 0)
	{
		Trace("NCError: Can't find weather with id " + sWeatherID);
		iClientCurWeatherNum = 0;
	}

	// create main module Weather
	DeleteAttribute(&NCWeather, "");
	NCWeather.Server = false;
	CreateEntity(&NCWeather, "NetWeather");

	aref aCurWeather = NetClient_GetCurrentWeather();

	sClientLightingPath = Whr_GetString(aCurWeather,"Lighting");

	bClientWeatherIsNight = Whr_GetLong(aCurWeather, "Night");
	bClientWeatherIsLight = Whr_GetLong(aCurWeather, "Lights");

	NCWeather.WindAngle = frand(180.0);//Whr_GetFloat(aCurWeather,"Wind.Angle");
	NCWeather.WindSpeed = Whr_GetFloat(aCurWeather, "Wind.Speed");

	sClientCurrentFog = "Fog";
	if (CheckAttribute(aCurWeather, "SpecialSeaFog")) { sClientCurrentFog = "SpecialSeaFog"; }	

	string sCurFog = NetClient_WhrGetCurrentFog();
	NCWeather.Fog.Enable = Whr_GetLong(aCurWeather, sCurFog + ".Enable");
	NCWeather.Fog.Start = Whr_GetFloat(aCurWeather, sCurFog + ".Start");
	NCWeather.Fog.Density = Whr_GetFloat(aCurWeather, sCurFog + ".Density");
	NCWeather.Fog.Color = Whr_GetColor(aCurWeather, sCurFog + ".Color");
	NCWeather.Fog.IslandDensity = Whr_GetFloat(aCurWeather, sCurFog + ".IslandDensity");

	NCWeather.Sun.Color = Whr_GetColor(aCurWeather,"Sun.Color");
	NCWeather.Sun.HeightAngle = Whr_GetFloat(aCurWeather,"Sun.HeightAngle");
	NCWeather.Sun.AzimuthAngle = Whr_GetFloat(aCurWeather,"Sun.AzimuthAngle");
	NCWeather.Sun.Ambient = Whr_GetColor(aCurWeather,"Sun.Ambient");

	float x, y, z;

	x = 5000.0; 
	y = 0.0;
	RotateAroundY(&x, &y, cos(-stf(NCWeather.Sun.HeightAngle)), sin(-stf(NCWeather.Sun.HeightAngle)));
	
	z = 0.0;
	RotateAroundY(&x, &z, cos(stf(NCWeather.Sun.AzimuthAngle)), sin(stf(NCWeather.Sun.AzimuthAngle)));

	NCWeather.Sun.x = x;
	NCWeather.Sun.y = y;
	NCWeather.Sun.z = z;

	NCWeather.isDone = "";

	SetEventHandler("NetClient_WhrOnCalcFogColor", "NetClient_WhrOnCalcFogColor", 0);

	NetClient_WhrCreateRainEnvironment();
	NetClient_WhrCreateSunGlowEnvironment();
	NetClient_WhrCreateLightningEnvironment();
	NetClient_WhrCreateSkyEnvironment();
	NetClient_WhrCreateSeaEnvironment();
	NetClient_WhrCreateAstronomyEnvironment();
	//if (bWhrTornado) { NetClient_WhrCreateTornadoEnvironment(); }

	Particles.windpower = 0.05 * Clampf(Whr_GetWindSpeed() / WIND_NORMAL_POWER);
	Particles.winddirection.x = sin(Whr_GetWindAngle());
	Particles.winddirection.z = cos(Whr_GetWindAngle());

	bWeatherLoaded = true;

	NCIsland.LightingPath = GetLightingPath();
	NCIsland.FogDensity = NCWeather.Fog.IslandDensity;
	SendMessage(&IslandReflModel, "lllf", MSG_MODEL_SET_FOG, 1, 1, stf(NCWeather.Fog.IslandDensity));

	NetClient_MoveWeatherToLayers("net_execute", "net_realize");
}

float NetClient_WhrGetFogDensity() 
{ 
	return stf(NCWeather.Fog.Density); 
}

int NetClient_WhrOnCalcFogColor()
{
	int		iAlpha, iColor;
	float	x, y, z;
	aref	aCurWeather = NetClient_GetCurrentWeather();

	x = GetEventData();
	y = GetEventData();
	z =	GetEventData();

	iAlpha = 255 - MakeInt(255.0 * Clampf(y / stf(aCurWeather.(sClientCurrentFog).Height)));

	//iColor = argb(0,0,0,0);
	int iFogColor = sti(NCWeather.Fog.Color);
	iColor = or(shl(iAlpha, 24), iFogColor);
	return iColor;
}

void NetClient_MoveWeatherToLayers(string sExecuteLayer, string sRealizeLayer)
{
	LayerAddObject(sExecuteLayer, &NCWeather, 1000000);
	LayerAddObject(sRealizeLayer, &NCWeather, 65530);

	NetClient_MoveRainToLayers(sExecuteLayer, sRealizeLayer);
	NetClient_MoveLightningToLayers(sExecuteLayer, sRealizeLayer);
	NetClient_MoveSkyToLayers(sExecuteLayer, sRealizeLayer);
	NetClient_MoveSunGlowToLayers(sExecuteLayer, sRealizeLayer);
	NetClient_MoveAstronomyToLayers(sExecuteLayer, sRealizeLayer);
	NetClient_MoveSeaToLayers(sExecuteLayer, sRealizeLayer);
}

float NetClient_WhrGetWindAngle()
{
	if (!CheckAttribute(&Weather,"WindAngle")) { return 0.0; }
	return stf(NCWeather.WindAngle);
}

float NetClient_WhrGetWindSpeed()
{
	if (!CheckAttribute(&Weather,"WindSpeed")) { return 0.0; }
	return stf(NCWeather.WindSpeed);
}

string NetClient_WhrGetLightingPath() { return sClientLightingPath; }

bool NetClient_WhrIsDay() { return !bClientWeatherIsNight; }
bool NetClient_WhrIsNight() { return bClientWeatherIsNight; }
bool NetClient_WhrIsLight() { return bClientWeatherIsLight; }
bool NetClient_WhrIsRain() { return bClientWeatherIsRain; }
bool NetClient_WhrIsStorm() { return bClientWeatherIsStorm; }