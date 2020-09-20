#include "Weather\Init\Common.c"
#include "Weather\Init\Day.c"
#include "Weather\Init\Evening.c"
#include "Weather\Init\Morning.c"
#include "Weather\Init\Night.c"
#include "Weather\Init\DayStorm.c"
#include "Weather\Init\Special.c"
#include "Weather\Init\Rain.c" //navy

int InitWeather()
{
	int n = 0;

	n = Whr_InitNight(n);
	n = Whr_InitMorning(n);
	n = Whr_InitDay(n);
	n = Whr_InitEvening(n);
	n = Whr_InitDayStorm(n);
	n = Whr_InitSpecial(n);
	//n = Whr_InitRain(n);

	Whr_InitCommon();

	Trace("Init weathers complete. " + n + " weathers found.");

	for (int i=0; i<n; i++)
	{
		if (!CheckAttribute(&Weathers[i], "Stars.Enable")) { Weathers[i].Stars.Enable = false; }
		if (!CheckAttribute(&Weathers[i], "Planets.Enable")) { Weathers[i].Planets.Enable = false; }
		if (!CheckAttribute(&Weathers[i], "Sun.Reflection.Enable")) { Weathers[i].Sun.Reflection.Enable = false; }
		if (!CheckAttribute(&Weathers[i], "Sounds.Sea.Postfix")) { Weathers[i].Sounds.Sea.Postfix = ""; }
		if (!CheckAttribute(&Weathers[i], "Sun.Moon")) { Weathers[i].Sun.Moon = false; }
	}

	return n;
}
