object NCAstronomy;

void NetClient_WhrDeleteAstronomyEnvironment()
{
	if (isEntity(&NCAstronomy))
	{
		DeleteClass(&NCAstronomy);
	}
	DeleteAttribute(&NCAstronomy, "");
}

void NetClient_WhrCreateAstronomyEnvironment()
{
	aref aCurWeather = NetClient_GetCurrentWeather();
	aref aStars;	makearef(aStars, aCurWeather.Stars);
	aref aPlanets;	makearef(aPlanets, aCurWeather.Planets);

	DeleteAttribute(&NCAstronomy, "");

	NCAstronomy.Server = false;
	CreateEntity(&NCAstronomy, "Astronomy");

	NCAstronomy.TimeScale = 1.0;

	NCAstronomy.Stars.Enable = Whr_GetLong(aStars, "Enable");
	if (sti(NCAstronomy.Stars.Enable))
	{
		NCAstronomy.Stars.Catalog = "resource\\hic.dat";
		NCAstronomy.Stars.Texture = Whr_GetString(aStars, "Texture");
		NCAstronomy.Stars.Color = Whr_GetColor(aStars, "Color");
		NCAstronomy.Stars.Radius = Whr_GetFloat(aStars, "Radius");
		NCAstronomy.Stars.Size = Whr_GetFloat(aStars, "Size");
		NCAstronomy.Stars.HeightFade = Whr_GetFloat(aStars, "HeightFade");
		NCAstronomy.Stars.SunFade = Whr_GetFloat(aStars, "SunFade");
		
		NCAstronomy.Stars.VisualMagnitude = Whr_GetFloat(aStars, "VisualMagnitude");
		NCAstronomy.Stars.TelescopeMagnitude = 13.0;

		NCAstronomy.Stars.TextureX = 4;
		NCAstronomy.Stars.TextureY = 4;

		NCAstronomy.Stars.Spectr.W = argb(0, 150, 243, 220);
		NCAstronomy.Stars.Spectr.O = argb(0, 150, 243, 220);
		NCAstronomy.Stars.Spectr.B = argb(0, 226, 244, 223);
		NCAstronomy.Stars.Spectr.A = argb(0, 255, 243, 213);
		NCAstronomy.Stars.Spectr.F = argb(0, 255, 243, 170);
		NCAstronomy.Stars.Spectr.G = argb(0, 255, 244, 123);
		NCAstronomy.Stars.Spectr.K = argb(0, 255, 187, 117);
		NCAstronomy.Stars.Spectr.M = argb(0, 249, 142, 117);
		NCAstronomy.Stars.Spectr.L = argb(0, 223, 142, 117);
		NCAstronomy.Stars.Spectr.T = argb(0, 209, 155, 123);
	}

	NCAstronomy.Planets.Enable = Whr_GetLong(aPlanets, "Enable");
	if (sti(NCAstronomy.Planets.Enable))
	{
		NCAstronomy.Planets.Speed = 10.0;

		NCAstronomy.Planets.Planet.Mercury.Distance = 0.38710;	
		NCAstronomy.Planets.Planet.Mercury.Speed = 48.8;			
		NCAstronomy.Planets.Planet.Mercury.Inclination = 7.0;	
		NCAstronomy.Planets.Planet.Mercury.Diameter = 2440.0;
		NCAstronomy.Planets.Planet.Mercury.Mag.Min = 1.3;
		NCAstronomy.Planets.Planet.Mercury.Mag.Max = -1.0;

		NCAstronomy.Planets.Planet.Venus.Distance = 0.72333;	
		NCAstronomy.Planets.Planet.Venus.Speed = 35.0;			
		NCAstronomy.Planets.Planet.Venus.Inclination = 3.39;	
		NCAstronomy.Planets.Planet.Venus.Diameter = 6050.0;
		NCAstronomy.Planets.Planet.Venus.Mag.Min = -3.0;
		NCAstronomy.Planets.Planet.Venus.Mag.Max = -4.3;

		NCAstronomy.Planets.Planet.Mars.Distance = 1.52363;	
		NCAstronomy.Planets.Planet.Mars.Speed = 24.4;			
		NCAstronomy.Planets.Planet.Mars.Inclination =  1.85;	
		NCAstronomy.Planets.Planet.Mars.Diameter = 3390.0;
		NCAstronomy.Planets.Planet.Mars.Mag.Min = 1.6;
		NCAstronomy.Planets.Planet.Mars.Mag.Max = -2.8;

		NCAstronomy.Planets.Planet.Jupiter.Distance = 5.20441;
		NCAstronomy.Planets.Planet.Jupiter.Speed = 13.06;	
		NCAstronomy.Planets.Planet.Jupiter.Inclination = 1.30;
		NCAstronomy.Planets.Planet.Jupiter.Diameter = 71400.0;
		NCAstronomy.Planets.Planet.Jupiter.Mag.Min = -1.9;
		NCAstronomy.Planets.Planet.Jupiter.Mag.Max = -2.2;

		NCAstronomy.Planets.Planet.Jupiter.Distance = 9.58378;
		NCAstronomy.Planets.Planet.Jupiter.Speed = 9.65;			
		NCAstronomy.Planets.Planet.Jupiter.Inclination = 2.49;	
		NCAstronomy.Planets.Planet.Jupiter.Diameter = 60400.0;
		NCAstronomy.Planets.Planet.Jupiter.Mag.Min = 1.5;
		NCAstronomy.Planets.Planet.Jupiter.Mag.Max = -0.4;

		NCAstronomy.Planets.Planet.Uranus.Distance = 19.18722;	
		NCAstronomy.Planets.Planet.Uranus.Speed = 6.78;			
		NCAstronomy.Planets.Planet.Uranus.Inclination = 0.77;	
		NCAstronomy.Planets.Planet.Uranus.Diameter = 24300.0;
		NCAstronomy.Planets.Planet.Uranus.Mag.Min = 6.0;
		NCAstronomy.Planets.Planet.Uranus.Mag.Max = 5.4;

		NCAstronomy.Planets.Planet.Neptune.Distance = 30.02090;	
		NCAstronomy.Planets.Planet.Neptune.Speed = 5.42;			
		NCAstronomy.Planets.Planet.Neptune.Inclination = 1.77;	
		NCAstronomy.Planets.Planet.Neptune.Diameter = 25050.0;
		NCAstronomy.Planets.Planet.Neptune.Mag.Min = 7.7;
		NCAstronomy.Planets.Planet.Neptune.Mag.Max = 7.6;

		NCAstronomy.Planets.Planet.Pluto.Distance = 39.23107;	
		NCAstronomy.Planets.Planet.Pluto.Speed = 4.73;			
		NCAstronomy.Planets.Planet.Pluto.Inclination = 17.2;	
		NCAstronomy.Planets.Planet.Pluto.Diameter = 1500.0;
		NCAstronomy.Planets.Planet.Pluto.Mag.Min = 14.3;
		NCAstronomy.Planets.Planet.Pluto.Mag.Max = 14.3;
	}

	NCAstronomy.isDone = true;
}

void NetClient_MoveAstronomyToLayers(string sExecuteLayer, string sRealizeLayer)
{
	LayerAddObject(sExecuteLayer, &NCAstronomy, -1);
	LayerAddObject(sRealizeLayer, &NCAstronomy, 3);
}