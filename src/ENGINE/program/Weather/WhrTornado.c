object Tornado;

void WhrDeleteTornadoEnvironment()
{
	if (isEntity(&Tornado)) 
	{ 
		DeleteClass(&Tornado);
		DeleteAttribute(&Tornado,"");
	}
}

void WhrCreateTornadoEnvironment()
{
	aref aCurWeather = GetCurrentWeather();
	
	DeleteAttribute(&Tornado,"");

	if (sti(aCurWeather.Tornado) != true) return;

	if (!isEntity(&Tornado)) 
	{ 
		CreateEntity(&Tornado,"Tornado");
	}
	
	Tornado.x = 10000.0;
	Tornado.z = 10000.0;

	SendMessage(&Tornado, "f", 300.0 + frnd() * 600.0);
	LayerAddObject(SEA_EXECUTE, &Tornado, 65535);
	LayerAddObject(SEA_REALIZE, &Tornado, 65535);
}

