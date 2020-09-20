#include "net\server\weather\Lightning.c"
#include "net\server\weather\Sea.c"
//#include "weather\Tornado.c"

object	NSWeather;
int		iServerCurWeatherNum = -1;
float	fServerWeatherDelta = 0.0;
float	fServerWeatherAngle, fServerWeatherSpeed;
bool	bServerWeatherIsNight = false;
bool	bServerWeatherIsStorm = false;
bool	bServerWhrTornado = false;
string	sServerLightingPath;

void NetServer_DeleteWeatherEnvironment()
{
	NetServer_WhrDeleteSeaEnvironment();
	NetServer_WhrDeleteLightningEnvironment();
	//NetServer_WhrDeleteTornadoEnvironment();

	DelEventHandler("frame", "NetServer_WhrOnWindChange");
}

aref NetServer_GetCurrentWeather()
{
	aref arWeather; 
	makearef(arWeather, Weathers[iServerCurWeatherNum]);
	return arWeather;
}

void NetServer_CreateWeatherEnvironment(int iSMsg)
{
	string sWeatherID = NetServer.WeatherID;

	NMAddString(iSMsg, NetServer.WeatherID, 64);

	iServerCurWeatherNum = -1;

	for (int i=0; i<iTotalNumWeathers; i++) 
	{
		if (sWeatherID == Weathers[i].id)
		{
			iServerCurWeatherNum = i;
			break;
		}
	}

	if (iServerCurWeatherNum < 0)
	{
		Trace("NSError: Can't find weather with id " + sWeatherID);
		iServerCurWeatherNum = 0;
	}
	
	aref aCurWeather = NetServer_GetCurrentWeather();

	sServerLightingPath = Whr_GetString(aCurWeather,"Lighting");

	bServerWeatherIsNight = Whr_GetLong(aCurWeather, "Night");
	bServerWeatherIsStorm = Whr_GetLong(aCurWeather, "Storm");
	bServerWhrTornado = Whr_GetLong(aCurWeather, "Tornado");

	NSWeather.WindAngle = Whr_GetFloat(aCurWeather,"Wind.Angle");
	NSWeather.WindSpeed = Whr_GetFloat(aCurWeather,"Wind.Speed");

	if (NetServer_WhrIsStorm())
	{
		NSWeather.WindSpeed = 7.0;
	}

	SetEventHandler("frame","Whr_OnWindChange",0);

	fServerWeatherDelta = 0.0;
	fServerWeatherAngle = stf(NSWeather.WindAngle);
	fServerWeatherSpeed = stf(NSWeather.WindSpeed);
	
	NetServer_WhrCreateLightningEnvironment();
	NetServer_WhrCreateSeaEnvironment();
	//if (bServerWhrTornado) { NetServer_WhrCreateTornadoEnvironment(); }

	NetServer_MoveWeatherToLayers("net_execute", "net_realize");
}

void NetServer_MoveWeatherToLayers(string sExecuteLayer, string sRealizeLayer)
{
	LayerAddObject(sExecuteLayer, &NSWeather, 1000000);
	LayerAddObject(sRealizeLayer, &NSWeather, 65530);

	NetServer_MoveSeaToLayers(sExecuteLayer, sRealizeLayer);
}

void NetServer_WhrOnWindChange()
{
	float fDeltaTime = MakeFloat(GetDeltaTime()) * 0.001;
	fServerWeatherDelta = fServerWeatherDelta + fDeltaTime;

	float fSpd = fServerWeatherSpeed + (fServerWeatherSpeed / 6.0) * 0.1 * (sin(fServerWeatherDelta) + sin(0.2 * fServerWeatherDelta) + sin(PI + 0.8 * fServerWeatherDelta) + cos(1.5 * fServerWeatherDelta));
	float fAng = fServerWeatherAngle + 0.02 * (sin(fServerWeatherDelta) + sin(0.2 * fServerWeatherDelta) + sin(PI + 0.8 * fServerWeatherDelta) + cos(1.5 * fServerWeatherDelta));

	NSWeather.WindAngle = fAng;
	NSWeather.WindSpeed = fSpd;
}

string NetServer_WhrGetLightingPath() { return sServerLightingPath; }

float NetServer_WhrGetWindAngle() { return stf(NSWeather.WindAngle); }
float NetServer_WhrGetWindSpeed() { return 5.0 * stf(NSWeather.WindSpeed); }

bool NetServer_WhrIsDay() { return !bServerWeatherIsNight; }
bool NetServer_WhrIsNight() { return bServerWeatherIsNight; }
bool NetServer_WhrIsStorm() { return bServerWeatherIsStorm; }