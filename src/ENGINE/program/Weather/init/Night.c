
//0,1,2,3,4,5 - ночь
//6,7,8,9,10 - утро
//11,12,13,14,15,16,17,18 - день

int Whr_InitNight(int n)
{

// ====================================================================
// Night1

	Weathers[n].id = "0 Hours";
	Weathers[n].Hour.Min = 0;
	Weathers[n].Hour.Max = 0;
	Weathers[n].Lighting = "night0";
	Weathers[n].LightingLm = "storm";
	Weathers[n].InsideBack = "n";

	Weathers[n].Storm = false;
	Weathers[n].Tornado = false;
	Weathers[n].Shark = true;
	Weathers[n].Lights = 1;
	Weathers[n].Night = true;

	Weathers[n].Shadow.Density.Head = argb(255,46,46,46);
	Weathers[n].Shadow.Density.Foot = argb(255,46,46,44);

	Weathers[n].Lightning.Enable = false;
	Weathers[n].Lightning.Texture = "Weather\lightning\lightning.tga";
	Weathers[n].Lightning.FlickerTime = 32;
	Weathers[n].Lightning.SubTexX = 1;
	Weathers[n].Lightning.SubTexY = 1;

	Weathers[n].Sky.Dir = "weather\skies\24\";
	Weathers[n].Sky.Color = argb(0,64,64,64);
	Weathers[n].Sky.Rotate.Min = 0.0;
	Weathers[n].Sky.Rotate.Max = 0.0;
	Weathers[n].Sky.Size = 512.0;
	Weathers[n].Sky.Angle = 0.0;

	Weathers[n].Planets.enable = true;

	Weathers[n].Stars.Enable = true;
	Weathers[n].Stars.Texture = "weather\astronomy\stars.tga";
	Weathers[n].Stars.Color = argb(0, 255, 255, 255);
	Weathers[n].Stars.Radius = 2000.0;
	Weathers[n].Stars.Size = 25.0;
	Weathers[n].Stars.HeightFade = 200.0;
	Weathers[n].Stars.SunFade = 1.0;
	Weathers[n].Stars.VisualMagnitude = 8.0;
	
	Weathers[n].Fog.Enable = true;
	Weathers[n].Fog.Height = 2000;
	Weathers[n].Fog.Start = 1;
	Weathers[n].Fog.Density = 0.002;
	Weathers[n].Fog.IslandDensity = 0.001;
	Weathers[n].Fog.SeaDensity = 0.001;
	Weathers[n].Fog.Color = argb(0,15,15,15);
	
	Weathers[n].Rain.NumDrops = 0;
	Weathers[n].Rain.Color = argb(0,73,73,73);
	Weathers[n].Rain.DropLength = 2.12;
	Weathers[n].Rain.Height = 30.0;
	Weathers[n].Rain.Radius = 30.0;
	Weathers[n].Rain.Speed = 18.0;
	Weathers[n].Rain.Jitter = 0.4;
	Weathers[n].Rain.WindSpeedJitter = 0.5;
	Weathers[n].Rain.MaxBlend = 49;
	Weathers[n].Rain.TimeBlend = 2000;

	Weathers[n].Planets.enable = true;

	Weathers[n].Rainbow.Enable = false;
	Weathers[n].Rainbow.Texture = "weather\rainbow\rainbow.tga";

	Weathers[n].Sun.Color = argb(0,30,30,30);
	Weathers[n].Sun.Ambient = argb(0,28,28,35);
	Weathers[n].Sun.AzimuthAngle = Degree2Radian(90.0);
	Weathers[n].Sun.HeightAngle = 0.30;
	Weathers[n].Sun.Moon = true;
	Weathers[n].Sun.Glow.Enable = true;
	Weathers[n].Sun.Glow.Dist = 3500.0;
	Weathers[n].Sun.Glow.Size = 110.0;
	Weathers[n].Sun.Glow.RotateSpeed = 0.0;
	Weathers[n].Sun.Glow.Texture = "weather\sun\glow\moon.tga";
	Weathers[n].Sun.Glow.DecayTime = 8.0;
	Weathers[n].Sun.Glow.TechniqueNoZ = "sunglow_noz";
	Weathers[n].Sun.Glow.TechniqueZ = "moon_z";
	Weathers[n].Sun.Glow.Color = argb(0,255,255,255);
	Weathers[n].Sun.Overflow.Enable = false;
	Weathers[n].Sun.Overflow.Texture = "Weather\lightning\flash.tga";
	Weathers[n].Sun.Overflow.Color = argb(0, 100, 100, 100);
	Weathers[n].Sun.Overflow.Size = 5500.0;
	Weathers[n].Sun.Overflow.Technique = "sunoverflow";
	Weathers[n].Sun.Overflow.Start = 0.7;
	Weathers[n].Sun.Flare.Enable = false;
	Weathers[n].Sun.Flare.Texture = "weather\sun\flare\allflares.tga";
	Weathers[n].Sun.Flare.Dist = 3500.0;
	Weathers[n].Sun.Flare.TexSizeX = 2;
	Weathers[n].Sun.Flare.TexSizeY = 2;
	Weathers[n].Sun.Flare.Technique = "sunflare";
	Weathers[n].Sun.Flare.Scale = 2.0;
	Weathers[n].Sun.Flares.f1 = "1.0,200.0,0,0x0F0F0F";
	Weathers[n].Sun.Flares.f2 = "0.9,20.0,1,0x0F0F0F";
	Weathers[n].Sun.Flares.f3 = "0.8,30.0,2,0x0F0F0F";
	Weathers[n].Sun.Flares.f4 = "0.6,150.0,0,0x0F0F0F";
	Weathers[n].Sun.Flares.f5 = "0.5,50.0,0,0x0F0F0F";
	Weathers[n].Sun.Flares.f6 = "0.4,100.0,1,0x0F0F0F";
	Weathers[n].Sun.Flares.f7 = "0.3,200.0,0,0x0F0F0F";
	Weathers[n].Sun.Flares.f8 = "0.2,200.0,0,0x0F0F0F";

	Weathers[n].Sea.GF2MX.Sky.Color = argb(0,160,210,255);
	Weathers[n].Sea.GF2MX.Water.Color = argb(0,0,5,10);
	Weathers[n].Sea.GF2MX.Bump.Tile = 0.05;
	Weathers[n].Sea.GF2MX.Bump.AnimSpeed = 7.0;
	Weathers[n].Sea.GF2MX.Bump.Dir = "weather\sea\bump_g2mx\";

	Weathers[n].Sea.Transparency = 0.4;
	Weathers[n].Sea.FrenelCoefficient = 0.5;
	Weathers[n].Sea.WaterReflection = 0.7;
	Weathers[n].Sea.WaterAttenuation = 0.2;
	Weathers[n].Sea.Sky.Color = argb(0,22,22,22);
	Weathers[n].Sea.Water.Color = argb(0,10,20,20);
	Weathers[n].Sea.Pena.Color = argb(0,75,75,55);
	Weathers[n].Sea.Bump.Dir = "resource\textures\weather\sea\bump_g3\";
	Weathers[n].Sea.Bump.Tile = 0.04;
	Weathers[n].Sea.Bump.Ang = 0.0;
	Weathers[n].Sea.Bump.Speed = 0.01;
	Weathers[n].Sea.Bump.AnimSpeed = 15.0;
	Weathers[n].Sea.Bump.Scale = 2.0;
	Weathers[n].Sea.SunRoad.Start = 0.99;
	Weathers[n].Sea.SunRoad.Color1 = argb(0,50,10,0);
	Weathers[n].Sea.SunRoad.Color2 = argb(0,50,50,50);
	Weathers[n].Sea.SunRoad.Power = 0.0;
	//угол, частота, высота волны в метрах, начальная позиция волны, скорость волны
	Weathers[n].Sea.Harmonics.h1 = "0.0, 2.0, 3.0, 0.0, 10.00";
	Weathers[n].Sea.Harmonics.h2 = "90.0, 4.0, 1.5, 0.0, 50.00";
	Weathers[n].Sea.Harmonics.h3 = "45.0, 7.0, 2.5, 0.0, 200.00";
	Weathers[n].Sea.Harmonics.h3 = "135.0, 5.0, 1.0, 0.0, 150.00";
	Weathers[n].Sea.Harmonics.h4 = "90.0, 40.0, 0.2, 0.0, 20.00";

	Weathers[n].Sun.Reflection.Enable = true;
	Weathers[n].Sun.Reflection.Texture = "weather\sun\reflection\refl_night.tga";
	Weathers[n].Sun.Reflection.Technique = "sunreflection";
	Weathers[n].Sun.Reflection.Dist = 3500.0;
	Weathers[n].Sun.Reflection.Size = 500.0;
	Weathers[n].Sun.Reflection.Color = argb(200, 200, 200, 200);

	Weathers[n].Sea2.BumpScale = 0.05; //МЕЛКИЕ ВОЛНЫ
	Weathers[n].Sea2.PosShift = 1.0; //остроконечность волн
	
	Weathers[n].Sea2.WaterColor = argb(0, 20, 30, 40);
	Weathers[n].Sea2.SkyColor = argb(0, 155, 155, 155);

	Weathers[n].Sea2.Reflection = 0.5;
	Weathers[n].Sea2.Transparency = 0.5;
	Weathers[n].Sea2.Frenel = 0.3; 
	Weathers[n].Sea2.Attenuation = 0.3;

	Weathers[n].Sea2.Amp1 = 10.0; //амплитуда 1 волны
	Weathers[n].Sea2.AnimSpeed1 = 0.5; //скорость анимации
	Weathers[n].Sea2.Scale1 = 0.3; //размеры волны
	Weathers[n].Sea2.MoveSpeed1 = "5.0, 0.0, 0.0"; //скорость движения

	Weathers[n].Sea2.Amp2 = 1.0;
	Weathers[n].Sea2.AnimSpeed2 = 19.0;
	Weathers[n].Sea2.Scale2 = 2.0;
	Weathers[n].Sea2.MoveSpeed2 = "1.0, 0.0, 0.0";

	Weathers[n].Sea2.FoamK = 0.03;//яркость пены по высоте 0.1 - пена яркая с 10 метров 1.0 через метр после начала
	Weathers[n].Sea2.FoamV = 3.0;//высота с которой начинается пена
	Weathers[n].Sea2.FoamUV = 1.0; //тайлинг пены
	Weathers[n].Sea2.FoamTexDisturb = 0.7;//сдвиг по нормали. х.з. что это надо поюзать. значение от 0.0 до 1.0

	Weathers[n].Wind.Angle = 2.0;
	Weathers[n].Wind.Speed.Min = 5.0;
	Weathers[n].Wind.Speed.Max = 14.0;
	
	n++;

// ====================================================================
// Night1

	Weathers[n].id = "1 hour";
	Weathers[n].Hour.Min = 1;
	Weathers[n].Hour.Max = 1;
	Weathers[n].Lighting = "night1";
	Weathers[n].LightingLm = "storm";
	Weathers[n].InsideBack = "n";

	Weathers[n].Storm = false;
	Weathers[n].Tornado = false;
	Weathers[n].Shark = true;
	Weathers[n].Lights = 1;
	Weathers[n].Night = true;

	Weathers[n].Shadow.Density.Head = argb(255,46,46,46);
	Weathers[n].Shadow.Density.Foot = argb(255,46,46,44);

	Weathers[n].Lightning.Enable = false;
	Weathers[n].Lightning.Texture = "Weather\lightning\lightning.tga";
	Weathers[n].Lightning.FlickerTime = 32;
	Weathers[n].Lightning.SubTexX = 1;
	Weathers[n].Lightning.SubTexY = 1;

	Weathers[n].Sky.Dir = "weather\skies\01\";
	Weathers[n].Sky.Color = argb(0,64,64,64);
	Weathers[n].Sky.Rotate.Min = 0.0;
	Weathers[n].Sky.Rotate.Max = 0.0;
	Weathers[n].Sky.Size = 512.0;
	Weathers[n].Sky.Angle = 0.0;

	Weathers[n].Planets.enable = true;

	Weathers[n].Stars.Enable = true;
	Weathers[n].Stars.Texture = "weather\astronomy\stars.tga";
	Weathers[n].Stars.Color = argb(0, 255, 255, 255);
	Weathers[n].Stars.Radius = 2000.0;
	Weathers[n].Stars.Size = 25.0;
	Weathers[n].Stars.HeightFade = 200.0;
	Weathers[n].Stars.SunFade = 1.0;
	Weathers[n].Stars.VisualMagnitude = 8.0;

	Weathers[n].Fog.Enable = true;
	Weathers[n].Fog.Height = 2000;
	Weathers[n].Fog.Start = 1;
	Weathers[n].Fog.Density = 0.002;
	Weathers[n].Fog.IslandDensity = 0.001;
	Weathers[n].Fog.SeaDensity = 0.001;
	Weathers[n].Fog.Color = argb(0,20,15,15);
		
	Weathers[n].Rain.NumDrops = 0;
	Weathers[n].Rain.Color = argb(0,73,73,73);
	Weathers[n].Rain.DropLength = 2.12;
	Weathers[n].Rain.Height = 30.0;
	Weathers[n].Rain.Radius = 30.0;
	Weathers[n].Rain.Speed = 18.0;
	Weathers[n].Rain.Jitter = 0.4;
	Weathers[n].Rain.WindSpeedJitter = 0.5;
	Weathers[n].Rain.MaxBlend = 49;
	Weathers[n].Rain.TimeBlend = 2000;

	Weathers[n].Planets.enable = true;

	Weathers[n].Rainbow.Enable = false;
	Weathers[n].Rainbow.Texture = "weather\rainbow\rainbow.tga";

	Weathers[n].Sun.Color = argb(0,30,30,30);
	Weathers[n].Sun.Ambient = argb(0,30,25,25);
	Weathers[n].Sun.AzimuthAngle = Degree2Radian(90.0);
	Weathers[n].Sun.HeightAngle = 0.30;
	Weathers[n].Sun.Moon = true;
	Weathers[n].Sun.Glow.Enable = true;
	Weathers[n].Sun.Glow.Dist = 3500.0;
	Weathers[n].Sun.Glow.Size = 110.0;
	Weathers[n].Sun.Glow.RotateSpeed = 0.0;
	Weathers[n].Sun.Glow.Texture = "weather\sun\glow\moon.tga";
	Weathers[n].Sun.Glow.DecayTime = 8.0;
	Weathers[n].Sun.Glow.TechniqueNoZ = "sunglow_noz";
	Weathers[n].Sun.Glow.TechniqueZ = "moon_z";
	Weathers[n].Sun.Glow.Color = argb(0,255,255,255);
	Weathers[n].Sun.Overflow.Enable = false;
	Weathers[n].Sun.Overflow.Texture = "Weather\lightning\flash.tga";
	Weathers[n].Sun.Overflow.Color = argb(0, 100, 100, 100);
	Weathers[n].Sun.Overflow.Size = 5500.0;
	Weathers[n].Sun.Overflow.Technique = "sunoverflow";
	Weathers[n].Sun.Overflow.Start = 0.7;
	Weathers[n].Sun.Flare.Enable = false;
	Weathers[n].Sun.Flare.Texture = "weather\sun\flare\allflares.tga";
	Weathers[n].Sun.Flare.Dist = 3500.0;
	Weathers[n].Sun.Flare.TexSizeX = 2;
	Weathers[n].Sun.Flare.TexSizeY = 2;
	Weathers[n].Sun.Flare.Technique = "sunflare";
	Weathers[n].Sun.Flare.Scale = 2.0;
	Weathers[n].Sun.Flares.f1 = "1.0,200.0,0,0x0F0F0F";
	Weathers[n].Sun.Flares.f2 = "0.9,20.0,1,0x0F0F0F";
	Weathers[n].Sun.Flares.f3 = "0.8,30.0,2,0x0F0F0F";
	Weathers[n].Sun.Flares.f4 = "0.6,150.0,0,0x0F0F0F";
	Weathers[n].Sun.Flares.f5 = "0.5,50.0,0,0x0F0F0F";
	Weathers[n].Sun.Flares.f6 = "0.4,100.0,1,0x0F0F0F";
	Weathers[n].Sun.Flares.f7 = "0.3,200.0,0,0x0F0F0F";
	Weathers[n].Sun.Flares.f8 = "0.2,200.0,0,0x0F0F0F";

	Weathers[n].Sea.GF2MX.Sky.Color = argb(0,160,210,255);
	Weathers[n].Sea.GF2MX.Water.Color = argb(0,0,5,10);
	Weathers[n].Sea.GF2MX.Bump.Tile = 0.05;
	Weathers[n].Sea.GF2MX.Bump.AnimSpeed = 7.0;
	Weathers[n].Sea.GF2MX.Bump.Dir = "weather\sea\bump_g2mx\";

	Weathers[n].Sea.Transparency = 0.4;
	Weathers[n].Sea.FrenelCoefficient = 0.5;
	Weathers[n].Sea.WaterReflection = 0.7;
	Weathers[n].Sea.WaterAttenuation = 0.2;
	Weathers[n].Sea.Sky.Color = argb(0,22,22,22);
	Weathers[n].Sea.Water.Color = argb(0,10,20,20);
	Weathers[n].Sea.Pena.Color = argb(0,75,75,55);
	Weathers[n].Sea.Bump.Dir = "resource\textures\weather\sea\bump_g3\";
	Weathers[n].Sea.Bump.Tile = 0.04;
	Weathers[n].Sea.Bump.Ang = 0.0;
	Weathers[n].Sea.Bump.Speed = 0.01;
	Weathers[n].Sea.Bump.AnimSpeed = 15.0;
	Weathers[n].Sea.Bump.Scale = 2.0;
	Weathers[n].Sea.SunRoad.Start = 0.99;
	Weathers[n].Sea.SunRoad.Color1 = argb(0,50,10,0);
	Weathers[n].Sea.SunRoad.Color2 = argb(0,50,50,50);
	Weathers[n].Sea.SunRoad.Power = 0.0;
	//угол, частота, высота волны в метрах, начальная позиция волны, скорость волны
	Weathers[n].Sea.Harmonics.h1 = "0.0, 2.0, 3.0, 0.0, 10.00";
	Weathers[n].Sea.Harmonics.h2 = "90.0, 4.0, 1.5, 0.0, 50.00";
	Weathers[n].Sea.Harmonics.h3 = "45.0, 7.0, 2.5, 0.0, 200.00";
	Weathers[n].Sea.Harmonics.h3 = "135.0, 5.0, 1.0, 0.0, 150.00";
	Weathers[n].Sea.Harmonics.h4 = "90.0, 40.0, 0.2, 0.0, 20.00";

	Weathers[n].Sun.Reflection.Enable = true;
	Weathers[n].Sun.Reflection.Texture = "weather\sun\reflection\refl_night.tga";
	Weathers[n].Sun.Reflection.Technique = "sunreflection";
	Weathers[n].Sun.Reflection.Dist = 3500.0;
	Weathers[n].Sun.Reflection.Size = 500.0;
	Weathers[n].Sun.Reflection.Color = argb(200, 200, 200, 200);

	Weathers[n].Sea2.BumpScale = 0.05; //МЕЛКИЕ ВОЛНЫ
	Weathers[n].Sea2.PosShift = 1.0; //остроконечность волн
	
	Weathers[n].Sea2.WaterColor = argb(0, 35, 30, 30);
	Weathers[n].Sea2.SkyColor = argb(0, 155, 155, 155);

	Weathers[n].Sea2.Reflection = 0.6;
	Weathers[n].Sea2.Transparency = 0.5;
	Weathers[n].Sea2.Frenel = 0.3; 
	Weathers[n].Sea2.Attenuation = 0.3;

	Weathers[n].Sea2.Amp1 = 10.0; //амплитуда 1 волны
	Weathers[n].Sea2.AnimSpeed1 = 0.5; //скорость анимации
	Weathers[n].Sea2.Scale1 = 0.3; //размеры волны
	Weathers[n].Sea2.MoveSpeed1 = "5.0, 0.0, 0.0"; //скорость движения

	Weathers[n].Sea2.Amp2 = 1.0;
	Weathers[n].Sea2.AnimSpeed2 = 19.0;
	Weathers[n].Sea2.Scale2 = 2.0;
	Weathers[n].Sea2.MoveSpeed2 = "1.0, 0.0, 0.0";

	Weathers[n].Sea2.FoamK = 0.035;//яркость пены по высоте 0.1 - пена яркая с 10 метров 1.0 через метр после начала
	Weathers[n].Sea2.FoamV = 3.0;//высота с которой начинается пена
	Weathers[n].Sea2.FoamUV = 1.0; //тайлинг пены
	Weathers[n].Sea2.FoamTexDisturb = 0.7;//сдвиг по нормали. х.з. что это надо поюзать. значение от 0.0 до 1.0

	Weathers[n].Wind.Angle = 2.0;
	Weathers[n].Wind.Speed.Min = 5.0;
	Weathers[n].Wind.Speed.Max = 14.0;
	
	n++;

// ====================================================================
// Night2

	Weathers[n].id = "2 Hours";
	Weathers[n].Hour.Min = 2;
	Weathers[n].Hour.Max = 2;
	Weathers[n].Lighting = "night2";
	Weathers[n].LightingLm = "storm";
	Weathers[n].InsideBack = "n";

	Weathers[n].Storm = false;
	Weathers[n].Tornado = false;
	Weathers[n].Shark = true;
	Weathers[n].Lights = 1;
	Weathers[n].Night = true;

	Weathers[n].Shadow.Density.Head = argb(255,46,46,46);
	Weathers[n].Shadow.Density.Foot = argb(255,46,46,44);

	Weathers[n].Lightning.Enable = false;
	Weathers[n].Lightning.Texture = "Weather\lightning\lightning.tga";
	Weathers[n].Lightning.FlickerTime = 32;
	Weathers[n].Lightning.SubTexX = 1;
	Weathers[n].Lightning.SubTexY = 1;

	Weathers[n].Sky.Dir = "weather\skies\02\";
	Weathers[n].Sky.Color = argb(0,64,64,64);
	Weathers[n].Sky.Rotate.Min = 0.0;
	Weathers[n].Sky.Rotate.Max = 0.0;
	Weathers[n].Sky.Size = 512.0;
	Weathers[n].Sky.Angle = 0.0;

	Weathers[n].Planets.enable = true;

	Weathers[n].Stars.Enable = true;
	Weathers[n].Stars.Texture = "weather\astronomy\stars.tga";
	Weathers[n].Stars.Color = argb(0, 255, 255, 255);
	Weathers[n].Stars.Radius = 2000.0;
	Weathers[n].Stars.Size = 25.0;
	Weathers[n].Stars.HeightFade = 200.0;
	Weathers[n].Stars.SunFade = 1.0;
	Weathers[n].Stars.VisualMagnitude = 8.0;
		
	Weathers[n].Fog.Enable = true;
	Weathers[n].Fog.Height = 2000;
	Weathers[n].Fog.Start = 1;
	Weathers[n].Fog.Density = 0.002;
	Weathers[n].Fog.IslandDensity = 0.001;
	Weathers[n].Fog.SeaDensity = 0.001;
	Weathers[n].Fog.Color = argb(0,15,15,15);
	
	Weathers[n].Rain.NumDrops = 0;
	Weathers[n].Rain.Color = argb(0,73,73,73);
	Weathers[n].Rain.DropLength = 2.12;
	Weathers[n].Rain.Height = 30.0;
	Weathers[n].Rain.Radius = 30.0;
	Weathers[n].Rain.Speed = 18.0;
	Weathers[n].Rain.Jitter = 0.4;
	Weathers[n].Rain.WindSpeedJitter = 0.5;
	Weathers[n].Rain.MaxBlend = 49;
	Weathers[n].Rain.TimeBlend = 2000;

	Weathers[n].Planets.enable = true;

	Weathers[n].Rainbow.Enable = false;
	Weathers[n].Rainbow.Texture = "weather\rainbow\rainbow.tga";

	Weathers[n].Sun.Color = argb(0,30,30,30);
	Weathers[n].Sun.Ambient = argb(0,25,28,28);
	Weathers[n].Sun.AzimuthAngle = Degree2Radian(190.0);
	Weathers[n].Sun.HeightAngle = 0.20;
	Weathers[n].Sun.Moon = true;
	Weathers[n].Sun.Glow.Enable = true;
	Weathers[n].Sun.Glow.Dist = 3500.0;
	Weathers[n].Sun.Glow.Size = 110.0;
	Weathers[n].Sun.Glow.RotateSpeed = 0.0;
	Weathers[n].Sun.Glow.Texture = "weather\sun\glow\moon.tga";
	Weathers[n].Sun.Glow.DecayTime = 8.0;
	Weathers[n].Sun.Glow.TechniqueNoZ = "sunglow_noz";
	Weathers[n].Sun.Glow.TechniqueZ = "moon_z";
	Weathers[n].Sun.Glow.Color = argb(0,255,255,255);
	Weathers[n].Sun.Overflow.Enable = false;
	Weathers[n].Sun.Overflow.Texture = "Weather\lightning\flash.tga";
	Weathers[n].Sun.Overflow.Color = argb(0, 100, 100, 100);
	Weathers[n].Sun.Overflow.Size = 5500.0;
	Weathers[n].Sun.Overflow.Technique = "sunoverflow";
	Weathers[n].Sun.Overflow.Start = 0.7;
	Weathers[n].Sun.Flare.Enable = false;
	Weathers[n].Sun.Flare.Texture = "weather\sun\flare\allflares.tga";
	Weathers[n].Sun.Flare.Dist = 3500.0;
	Weathers[n].Sun.Flare.TexSizeX = 2;
	Weathers[n].Sun.Flare.TexSizeY = 2;
	Weathers[n].Sun.Flare.Technique = "sunflare";
	Weathers[n].Sun.Flare.Scale = 2.0;
	Weathers[n].Sun.Flares.f1 = "1.0,200.0,0,0x0F0F0F";
	Weathers[n].Sun.Flares.f2 = "0.9,20.0,1,0x0F0F0F";
	Weathers[n].Sun.Flares.f3 = "0.8,30.0,2,0x0F0F0F";
	Weathers[n].Sun.Flares.f4 = "0.6,150.0,0,0x0F0F0F";
	Weathers[n].Sun.Flares.f5 = "0.5,50.0,0,0x0F0F0F";
	Weathers[n].Sun.Flares.f6 = "0.4,100.0,1,0x0F0F0F";
	Weathers[n].Sun.Flares.f7 = "0.3,200.0,0,0x0F0F0F";
	Weathers[n].Sun.Flares.f8 = "0.2,200.0,0,0x0F0F0F";

	Weathers[n].Sea.GF2MX.Sky.Color = argb(0,160,210,255);
	Weathers[n].Sea.GF2MX.Water.Color = argb(0,0,5,10);
	Weathers[n].Sea.GF2MX.Bump.Tile = 0.05;
	Weathers[n].Sea.GF2MX.Bump.AnimSpeed = 7.0;
	Weathers[n].Sea.GF2MX.Bump.Dir = "weather\sea\bump_g2mx\";

	Weathers[n].Sea.Transparency = 0.4;
	Weathers[n].Sea.FrenelCoefficient = 0.5;
	Weathers[n].Sea.WaterReflection = 0.7;
	Weathers[n].Sea.WaterAttenuation = 0.2;
	Weathers[n].Sea.Sky.Color = argb(0,22,22,22);
	Weathers[n].Sea.Water.Color = argb(0,10,20,20);
	Weathers[n].Sea.Pena.Color = argb(0,75,75,55);
	Weathers[n].Sea.Bump.Dir = "resource\textures\weather\sea\bump_g3\";
	Weathers[n].Sea.Bump.Tile = 0.04;
	Weathers[n].Sea.Bump.Ang = 0.0;
	Weathers[n].Sea.Bump.Speed = 0.01;
	Weathers[n].Sea.Bump.AnimSpeed = 15.0;
	Weathers[n].Sea.Bump.Scale = 2.0;
	Weathers[n].Sea.SunRoad.Start = 0.99;
	Weathers[n].Sea.SunRoad.Color1 = argb(0,50,10,0);
	Weathers[n].Sea.SunRoad.Color2 = argb(0,50,50,50);
	Weathers[n].Sea.SunRoad.Power = 0.0;
	//угол, частота, высота волны в метрах, начальная позиция волны, скорость волны
	Weathers[n].Sea.Harmonics.h1 = "0.0, 2.0, 3.0, 0.0, 10.00";
	Weathers[n].Sea.Harmonics.h2 = "90.0, 4.0, 1.5, 0.0, 50.00";
	Weathers[n].Sea.Harmonics.h3 = "45.0, 7.0, 2.5, 0.0, 200.00";
	Weathers[n].Sea.Harmonics.h3 = "135.0, 5.0, 1.0, 0.0, 150.00";
	Weathers[n].Sea.Harmonics.h4 = "90.0, 40.0, 0.2, 0.0, 20.00";

	Weathers[n].Sun.Reflection.Enable = true;
	Weathers[n].Sun.Reflection.Texture = "weather\sun\reflection\refl_night.tga";
	Weathers[n].Sun.Reflection.Technique = "sunreflection";
	Weathers[n].Sun.Reflection.Dist = 3500.0;
	Weathers[n].Sun.Reflection.Size = 500.0;
	Weathers[n].Sun.Reflection.Color = argb(200, 200, 200, 200);

	Weathers[n].Sea2.BumpScale = 0.05; //МЕЛКИЕ ВОЛНЫ
	Weathers[n].Sea2.PosShift = 1.6; //остроконечность волн
	
	Weathers[n].Sea2.WaterColor = argb(0, 25, 35, 40);
	Weathers[n].Sea2.SkyColor = argb(0, 185, 155, 155);

	Weathers[n].Sea2.Reflection = 0.8;
	Weathers[n].Sea2.Transparency = 0.5;
	Weathers[n].Sea2.Frenel = 0.3; 
	Weathers[n].Sea2.Attenuation = 0.3;

	Weathers[n].Sea2.Amp1 = 30.0; //амплитуда 1 волны
	Weathers[n].Sea2.AnimSpeed1 = 1.0; //скорость анимации
	Weathers[n].Sea2.Scale1 = 0.15; //размеры волны
	Weathers[n].Sea2.MoveSpeed1 = "10.0, 0.0, 0.0"; //скорость движения

	Weathers[n].Sea2.Amp2 = 1.0;
	Weathers[n].Sea2.AnimSpeed2 = 4.0;
	Weathers[n].Sea2.Scale2 = 2.0;
	Weathers[n].Sea2.MoveSpeed2 = "-2.5, 0.0, 0.0";

	Weathers[n].Sea2.FoamK = 0.01;//яркость пены по высоте 0.1 - пена яркая с 10 метров 1.0 через метр после начала
	Weathers[n].Sea2.FoamV = 6.0;//высота с которой начинается пена
	Weathers[n].Sea2.FoamUV = 0.5; //тайлинг пены
	Weathers[n].Sea2.FoamTexDisturb = 0.2;//сдвиг по нормали. х.з. что это надо поюзать. значение от 0.0 до 1.0

	Weathers[n].Wind.Angle = 2.0;
	Weathers[n].Wind.Speed.Min = 5.0;
	Weathers[n].Wind.Speed.Max = 14.0;
	
	n++;

// ====================================================================
// Night3

	Weathers[n].id = "3 Hours";
	Weathers[n].Hour.Min = 3;
	Weathers[n].Hour.Max = 3;
	Weathers[n].Lighting = "night3";
	Weathers[n].LightingLm = "storm";
	Weathers[n].InsideBack = "n";

	Weathers[n].Storm = false;
	Weathers[n].Tornado = false;
	Weathers[n].Shark = true;
	Weathers[n].Lights = 1;
	Weathers[n].Night = true;

	Weathers[n].Shadow.Density.Head = argb(255,46,46,46);
	Weathers[n].Shadow.Density.Foot = argb(255,46,46,44);

	Weathers[n].Lightning.Enable = false;
	Weathers[n].Lightning.Texture = "Weather\lightning\lightning.tga";
	Weathers[n].Lightning.FlickerTime = 32;
	Weathers[n].Lightning.SubTexX = 1;
	Weathers[n].Lightning.SubTexY = 1;

	Weathers[n].Sky.Dir = "weather\skies\03\";
	Weathers[n].Sky.Color = argb(0,64,64,64);
	Weathers[n].Sky.Rotate.Min = 0.0;
	Weathers[n].Sky.Rotate.Max = 0.0;
	Weathers[n].Sky.Size = 512.0;
	Weathers[n].Sky.Angle = 0.0;

	Weathers[n].Planets.enable = true;

	Weathers[n].Stars.Enable = true;
	Weathers[n].Stars.Texture = "weather\astronomy\stars.tga";
	Weathers[n].Stars.Color = argb(0, 255, 255, 255);
	Weathers[n].Stars.Radius = 2000.0;
	Weathers[n].Stars.Size = 25.0;
	Weathers[n].Stars.HeightFade = 200.0;
	Weathers[n].Stars.SunFade = 1.0;
	Weathers[n].Stars.VisualMagnitude = 8.0;
	
	Weathers[n].Fog.Enable = true;
	Weathers[n].Fog.Height = 2000;
	Weathers[n].Fog.Start = 1;
	Weathers[n].Fog.Density = 0.003;
	Weathers[n].Fog.IslandDensity = 0.0015;
	Weathers[n].Fog.SeaDensity = 0.001;
	Weathers[n].Fog.Color = argb(0,15,15,15);
		
	Weathers[n].Rain.NumDrops = 0;
	Weathers[n].Rain.Color = argb(0,73,73,73);
	Weathers[n].Rain.DropLength = 2.12;
	Weathers[n].Rain.Height = 30.0;
	Weathers[n].Rain.Radius = 30.0;
	Weathers[n].Rain.Speed = 18.0;
	Weathers[n].Rain.Jitter = 0.4;
	Weathers[n].Rain.WindSpeedJitter = 0.5;
	Weathers[n].Rain.MaxBlend = 49;
	Weathers[n].Rain.TimeBlend = 2000;

	Weathers[n].Planets.enable = true;

	Weathers[n].Rainbow.Enable = false;
	Weathers[n].Rainbow.Texture = "weather\rainbow\rainbow.tga";

	Weathers[n].Sun.Color = argb(0,40,40,40);
	Weathers[n].Sun.Ambient = argb(0,30,30,30);
	Weathers[n].Sun.AzimuthAngle = Degree2Radian(130.0);
	Weathers[n].Sun.HeightAngle = 0.50;
	Weathers[n].Sun.Moon = true;
	Weathers[n].Sun.Glow.Enable = true;
	Weathers[n].Sun.Glow.Dist = 3500.0;
	Weathers[n].Sun.Glow.Size = 110.0;
	Weathers[n].Sun.Glow.RotateSpeed = 0.0;
	Weathers[n].Sun.Glow.Texture = "weather\sun\glow\moon.tga";
	Weathers[n].Sun.Glow.DecayTime = 8.0;
	Weathers[n].Sun.Glow.TechniqueNoZ = "sunglow_noz";
	Weathers[n].Sun.Glow.TechniqueZ = "moon_z";
	Weathers[n].Sun.Glow.Color = argb(0,255,255,255);
	Weathers[n].Sun.Overflow.Enable = false;
	Weathers[n].Sun.Overflow.Texture = "Weather\lightning\flash.tga";
	Weathers[n].Sun.Overflow.Color = argb(0, 100, 100, 100);
	Weathers[n].Sun.Overflow.Size = 5500.0;
	Weathers[n].Sun.Overflow.Technique = "sunoverflow";
	Weathers[n].Sun.Overflow.Start = 0.7;
	Weathers[n].Sun.Flare.Enable = false;
	Weathers[n].Sun.Flare.Texture = "weather\sun\flare\allflares.tga";
	Weathers[n].Sun.Flare.Dist = 3500.0;
	Weathers[n].Sun.Flare.TexSizeX = 2;
	Weathers[n].Sun.Flare.TexSizeY = 2;
	Weathers[n].Sun.Flare.Technique = "sunflare";
	Weathers[n].Sun.Flare.Scale = 2.0;
	Weathers[n].Sun.Flares.f1 = "1.0,200.0,0,0x0F0F0F";
	Weathers[n].Sun.Flares.f2 = "0.9,20.0,1,0x0F0F0F";
	Weathers[n].Sun.Flares.f3 = "0.8,30.0,2,0x0F0F0F";
	Weathers[n].Sun.Flares.f4 = "0.6,150.0,0,0x0F0F0F";
	Weathers[n].Sun.Flares.f5 = "0.5,50.0,0,0x0F0F0F";
	Weathers[n].Sun.Flares.f6 = "0.4,100.0,1,0x0F0F0F";
	Weathers[n].Sun.Flares.f7 = "0.3,200.0,0,0x0F0F0F";
	Weathers[n].Sun.Flares.f8 = "0.2,200.0,0,0x0F0F0F";

	Weathers[n].Sea.GF2MX.Sky.Color = argb(0,160,210,255);
	Weathers[n].Sea.GF2MX.Water.Color = argb(0,0,5,10);
	Weathers[n].Sea.GF2MX.Bump.Tile = 0.05;
	Weathers[n].Sea.GF2MX.Bump.AnimSpeed = 7.0;
	Weathers[n].Sea.GF2MX.Bump.Dir = "weather\sea\bump_g2mx\";

	Weathers[n].Sea.Transparency = 0.4;
	Weathers[n].Sea.FrenelCoefficient = 0.5;
	Weathers[n].Sea.WaterReflection = 0.7;
	Weathers[n].Sea.WaterAttenuation = 0.2;
	Weathers[n].Sea.Sky.Color = argb(0,22,22,22);
	Weathers[n].Sea.Water.Color = argb(0,10,20,20);
	Weathers[n].Sea.Pena.Color = argb(0,75,75,55);
	Weathers[n].Sea.Bump.Dir = "resource\textures\weather\sea\bump_g3\";
	Weathers[n].Sea.Bump.Tile = 0.04;
	Weathers[n].Sea.Bump.Ang = 0.0;
	Weathers[n].Sea.Bump.Speed = 0.01;
	Weathers[n].Sea.Bump.AnimSpeed = 15.0;
	Weathers[n].Sea.Bump.Scale = 2.0;
	Weathers[n].Sea.SunRoad.Start = 0.99;
	Weathers[n].Sea.SunRoad.Color1 = argb(0,50,10,0);
	Weathers[n].Sea.SunRoad.Color2 = argb(0,50,50,50);
	Weathers[n].Sea.SunRoad.Power = 0.0;
	//угол, частота, высота волны в метрах, начальная позиция волны, скорость волны
	Weathers[n].Sea.Harmonics.h1 = "0.0, 2.0, 3.0, 0.0, 10.00";
	Weathers[n].Sea.Harmonics.h2 = "90.0, 4.0, 1.5, 0.0, 50.00";
	Weathers[n].Sea.Harmonics.h3 = "45.0, 7.0, 2.5, 0.0, 200.00";
	Weathers[n].Sea.Harmonics.h3 = "135.0, 5.0, 1.0, 0.0, 150.00";
	Weathers[n].Sea.Harmonics.h4 = "90.0, 40.0, 0.2, 0.0, 20.00";

	Weathers[n].Sun.Reflection.Enable = true;
	Weathers[n].Sun.Reflection.Texture = "weather\sun\reflection\refl_night.tga";
	Weathers[n].Sun.Reflection.Technique = "sunreflection";
	Weathers[n].Sun.Reflection.Dist = 3500.0;
	Weathers[n].Sun.Reflection.Size = 500.0;
	Weathers[n].Sun.Reflection.Color = argb(200, 200, 200, 200);

	Weathers[n].Sea2.BumpScale = 0.05; //МЕЛКИЕ ВОЛНЫ
	Weathers[n].Sea2.PosShift = 2.0; //остроконечность волн
	
	Weathers[n].Sea2.WaterColor = argb(0, 15, 25, 35);
	Weathers[n].Sea2.SkyColor = argb(0, 55, 55, 55);

	Weathers[n].Sea2.Reflection = 0.9;
	Weathers[n].Sea2.Transparency = 0.5;
	Weathers[n].Sea2.Frenel = 0.3; 
	Weathers[n].Sea2.Attenuation = 0.3;

	Weathers[n].Sea2.Amp1 = 25.0; //амплитуда 1 волны
	Weathers[n].Sea2.AnimSpeed1 = 1.0; //скорость анимации
	Weathers[n].Sea2.Scale1 = 0.15; //размеры волны
	Weathers[n].Sea2.MoveSpeed1 = "1.0, 0.0, 6.0"; //скорость движения

	Weathers[n].Sea2.Amp2 = 1.0;
	Weathers[n].Sea2.AnimSpeed2 = 19.0;
	Weathers[n].Sea2.Scale2 = 4.0;
	Weathers[n].Sea2.MoveSpeed2 = "0.0, 0.0, 1.0";

	Weathers[n].Sea2.FoamK = 0.03;//яркость пены по высоте 0.1 - пена яркая с 10 метров 1.0 через метр после начала
	Weathers[n].Sea2.FoamV = 7.0;//высота с которой начинается пена
	Weathers[n].Sea2.FoamUV = 0.3; //тайлинг пены
	Weathers[n].Sea2.FoamTexDisturb = 0.1;//сдвиг по нормали. х.з. что это надо поюзать. значение от 0.0 до 1.0

	Weathers[n].Wind.Angle = 2.0;
	Weathers[n].Wind.Speed.Min = 5.0;
	Weathers[n].Wind.Speed.Max = 14.0;
	
	n++;

// ====================================================================
// Night4

	Weathers[n].id = "4 Hours";
	Weathers[n].Hour.Min = 4;
	Weathers[n].Hour.Max = 4;
	Weathers[n].Lighting = "night4";
	Weathers[n].LightingLm = "storm";
	Weathers[n].InsideBack = "n";

	Weathers[n].Storm = false;
	Weathers[n].Tornado = false;
	Weathers[n].Shark = true;
	Weathers[n].Lights = 1;
	Weathers[n].Night = true;

	Weathers[n].Shadow.Density.Head = argb(255,46,46,46);
	Weathers[n].Shadow.Density.Foot = argb(255,46,46,44);;

	Weathers[n].Lightning.Enable = false;
	Weathers[n].Lightning.Texture = "Weather\lightning\lightning.tga";
	Weathers[n].Lightning.FlickerTime = 32;
	Weathers[n].Lightning.SubTexX = 1;
	Weathers[n].Lightning.SubTexY = 1;

	Weathers[n].Sky.Dir = "weather\skies\04\";
	Weathers[n].Sky.Color = argb(0,64,64,64);
	Weathers[n].Sky.Rotate.Min = 0.0;
	Weathers[n].Sky.Rotate.Max = 0.0;
	Weathers[n].Sky.Size = 512.0;
	Weathers[n].Sky.Angle = 0.0;

	Weathers[n].Planets.enable = true;

	Weathers[n].Stars.Enable = true;
	Weathers[n].Stars.Texture = "weather\astronomy\stars.tga";
	Weathers[n].Stars.Color = argb(0, 255, 255, 255);
	Weathers[n].Stars.Radius = 2000.0;
	Weathers[n].Stars.Size = 25.0;
	Weathers[n].Stars.HeightFade = 200.0;
	Weathers[n].Stars.SunFade = 1.0;
	Weathers[n].Stars.VisualMagnitude = 8.0;
	
	Weathers[n].Fog.Enable = true;
	Weathers[n].Fog.Height = 2000;
	Weathers[n].Fog.Start = 1;
	Weathers[n].Fog.Density = 0.002;
	Weathers[n].Fog.IslandDensity = 0.001;
	Weathers[n].Fog.SeaDensity = 0.001;
	Weathers[n].Fog.Color = argb(0,20,20,25);
	
	Weathers[n].Rain.NumDrops = 0;
	Weathers[n].Rain.Color = argb(0,73,73,73);
	Weathers[n].Rain.DropLength = 2.12;
	Weathers[n].Rain.Height = 30.0;
	Weathers[n].Rain.Radius = 30.0;
	Weathers[n].Rain.Speed = 18.0;
	Weathers[n].Rain.Jitter = 0.4;
	Weathers[n].Rain.WindSpeedJitter = 0.5;
	Weathers[n].Rain.MaxBlend = 49;
	Weathers[n].Rain.TimeBlend = 2000;

	Weathers[n].Planets.enable = true;

	Weathers[n].Rainbow.Enable = false;
	Weathers[n].Rainbow.Texture = "weather\rainbow\rainbow.tga";

	Weathers[n].Sun.Color = argb(0,30,30,25);
	Weathers[n].Sun.Ambient = argb(0,48,48,43);
	Weathers[n].Sun.AzimuthAngle = Degree2Radian(320.0);
	Weathers[n].Sun.HeightAngle = 0.28;
	Weathers[n].Sun.Moon = true;
	Weathers[n].Sun.Glow.Enable = true;
	Weathers[n].Sun.Glow.Dist = 3500.0;
	Weathers[n].Sun.Glow.Size = 110.0;
	Weathers[n].Sun.Glow.RotateSpeed = 0.0;
	Weathers[n].Sun.Glow.Texture = "weather\sun\glow\moon.tga";
	Weathers[n].Sun.Glow.DecayTime = 8.0;
	Weathers[n].Sun.Glow.TechniqueNoZ = "sunglow_noz";
	Weathers[n].Sun.Glow.TechniqueZ = "moon_z";
	Weathers[n].Sun.Glow.Color = argb(0,255,255,255);
	Weathers[n].Sun.Overflow.Enable = false;
	Weathers[n].Sun.Overflow.Texture = "Weather\lightning\flash.tga";
	Weathers[n].Sun.Overflow.Color = argb(0, 100, 100, 100);
	Weathers[n].Sun.Overflow.Size = 5500.0;
	Weathers[n].Sun.Overflow.Technique = "sunoverflow";
	Weathers[n].Sun.Overflow.Start = 0.7;
	Weathers[n].Sun.Flare.Enable = false;
	Weathers[n].Sun.Flare.Texture = "weather\sun\flare\allflares.tga";
	Weathers[n].Sun.Flare.Dist = 3500.0;
	Weathers[n].Sun.Flare.TexSizeX = 2;
	Weathers[n].Sun.Flare.TexSizeY = 2;
	Weathers[n].Sun.Flare.Technique = "sunflare";
	Weathers[n].Sun.Flare.Scale = 2.0;
	Weathers[n].Sun.Flares.f1 = "1.0,200.0,0,0x0F0F0F";
	Weathers[n].Sun.Flares.f2 = "0.9,20.0,1,0x0F0F0F";
	Weathers[n].Sun.Flares.f3 = "0.8,30.0,2,0x0F0F0F";
	Weathers[n].Sun.Flares.f4 = "0.6,150.0,0,0x0F0F0F";
	Weathers[n].Sun.Flares.f5 = "0.5,50.0,0,0x0F0F0F";
	Weathers[n].Sun.Flares.f6 = "0.4,100.0,1,0x0F0F0F";
	Weathers[n].Sun.Flares.f7 = "0.3,200.0,0,0x0F0F0F";
	Weathers[n].Sun.Flares.f8 = "0.2,200.0,0,0x0F0F0F";

	Weathers[n].Sea.GF2MX.Sky.Color = argb(0,160,210,255);
	Weathers[n].Sea.GF2MX.Water.Color = argb(0,0,5,10);
	Weathers[n].Sea.GF2MX.Bump.Tile = 0.05;
	Weathers[n].Sea.GF2MX.Bump.AnimSpeed = 7.0;
	Weathers[n].Sea.GF2MX.Bump.Dir = "weather\sea\bump_g2mx\";

	Weathers[n].Sea.Transparency = 0.4;
	Weathers[n].Sea.FrenelCoefficient = 0.5;
	Weathers[n].Sea.WaterReflection = 0.7;
	Weathers[n].Sea.WaterAttenuation = 0.2;
	Weathers[n].Sea.Sky.Color = argb(0,22,22,22);
	Weathers[n].Sea.Water.Color = argb(0,10,20,20);
	Weathers[n].Sea.Pena.Color = argb(0,75,75,55);
	Weathers[n].Sea.Bump.Dir = "resource\textures\weather\sea\bump_g3\";
	Weathers[n].Sea.Bump.Tile = 0.04;
	Weathers[n].Sea.Bump.Ang = 0.0;
	Weathers[n].Sea.Bump.Speed = 0.01;
	Weathers[n].Sea.Bump.AnimSpeed = 15.0;
	Weathers[n].Sea.Bump.Scale = 2.0;
	Weathers[n].Sea.SunRoad.Start = 0.99;
	Weathers[n].Sea.SunRoad.Color1 = argb(0,50,10,0);
	Weathers[n].Sea.SunRoad.Color2 = argb(0,50,50,50);
	Weathers[n].Sea.SunRoad.Power = 0.0;
	//угол, частота, высота волны в метрах, начальная позиция волны, скорость волны
	Weathers[n].Sea.Harmonics.h1 = "0.0, 2.0, 3.0, 0.0, 10.00";
	Weathers[n].Sea.Harmonics.h2 = "90.0, 4.0, 1.5, 0.0, 50.00";
	Weathers[n].Sea.Harmonics.h3 = "45.0, 7.0, 2.5, 0.0, 200.00";
	Weathers[n].Sea.Harmonics.h3 = "135.0, 5.0, 1.0, 0.0, 150.00";
	Weathers[n].Sea.Harmonics.h4 = "90.0, 40.0, 0.2, 0.0, 20.00";

	Weathers[n].Sun.Reflection.Enable = true;
	Weathers[n].Sun.Reflection.Texture = "weather\sun\reflection\refl_night.tga";
	Weathers[n].Sun.Reflection.Technique = "sunreflection";
	Weathers[n].Sun.Reflection.Dist = 3500.0;
	Weathers[n].Sun.Reflection.Size = 500.0;
	Weathers[n].Sun.Reflection.Color = argb(200, 200, 200, 200);

	Weathers[n].Sea2.BumpScale = 0.05; //МЕЛКИЕ ВОЛНЫ
	Weathers[n].Sea2.PosShift = 1.0; //остроконечность волн
	
	Weathers[n].Sea2.WaterColor = argb(0, 10, 30, 40);
	Weathers[n].Sea2.SkyColor = argb(0, 175, 175, 175);

	Weathers[n].Sea2.Reflection = 0.6;
	Weathers[n].Sea2.Transparency = 0.5;
	Weathers[n].Sea2.Frenel = 0.3; 
	Weathers[n].Sea2.Attenuation = 0.3;

	Weathers[n].Sea2.Amp1 = 6.0; //амплитуда 1 волны
	Weathers[n].Sea2.AnimSpeed1 = 10.0; //скорость анимации
	Weathers[n].Sea2.Scale1 = 0.8; //размеры волны
	Weathers[n].Sea2.MoveSpeed1 = "3.0, 0.0, 0.0"; //скорость движения

	Weathers[n].Sea2.Amp2 = 1.25;
	Weathers[n].Sea2.AnimSpeed2 = 19.0;
	Weathers[n].Sea2.Scale2 = 2.5;
	Weathers[n].Sea2.MoveSpeed2 = "0.0, 0.0, -2.5";

	Weathers[n].Sea2.FoamK = 0.1;//яркость пены по высоте 0.1 - пена яркая с 10 метров 1.0 через метр после начала
	Weathers[n].Sea2.FoamV = 2.5;//высота с которой начинается пена
	Weathers[n].Sea2.FoamUV = 1.0; //тайлинг пены
	Weathers[n].Sea2.FoamTexDisturb = 0.1;//сдвиг по нормали. х.з. что это надо поюзать. значение от 0.0 до 1.0

	Weathers[n].Wind.Angle = 2.0;
	Weathers[n].Wind.Speed.Min = 5.0;
	Weathers[n].Wind.Speed.Max = 14.0;
	
	n++;

// ====================================================================
// Night4

	Weathers[n].id = "5 Hours";
	Weathers[n].Hour.Min = 5;
	Weathers[n].Hour.Max = 5;
	Weathers[n].Lighting = "night5";
	Weathers[n].LightingLm = "storm";
	Weathers[n].InsideBack = "n";

	Weathers[n].Storm = false;
	Weathers[n].Tornado = false;
	Weathers[n].Shark = true;
	Weathers[n].Lights = 1;
	Weathers[n].Night = true;

	Weathers[n].Shadow.Density.Head = argb(255,46,46,46);
	Weathers[n].Shadow.Density.Foot = argb(255,46,46,44);;

	Weathers[n].Lightning.Enable = false;
	Weathers[n].Lightning.Texture = "Weather\lightning\lightning.tga";
	Weathers[n].Lightning.FlickerTime = 32;
	Weathers[n].Lightning.SubTexX = 1;
	Weathers[n].Lightning.SubTexY = 1;

	Weathers[n].Sky.Dir = "weather\skies\05\";
	Weathers[n].Sky.Color = argb(0,64,64,64);
	Weathers[n].Sky.Rotate.Min = 0.0;
	Weathers[n].Sky.Rotate.Max = 0.0;
	Weathers[n].Sky.Size = 512.0;
	Weathers[n].Sky.Angle = 0.0;

	Weathers[n].Planets.enable = true;

	Weathers[n].Stars.Enable = true;
	Weathers[n].Stars.Texture = "weather\astronomy\stars.tga";
	Weathers[n].Stars.Color = argb(0, 255, 255, 255);
	Weathers[n].Stars.Radius = 2000.0;
	Weathers[n].Stars.Size = 25.0;
	Weathers[n].Stars.HeightFade = 200.0;
	Weathers[n].Stars.SunFade = 1.0;
	Weathers[n].Stars.VisualMagnitude = 8.0;
		
	Weathers[n].Fog.Enable = true;
	Weathers[n].Fog.Height = 2000;
	Weathers[n].Fog.Start = 1;
	Weathers[n].Fog.Density = 0.005;
	Weathers[n].Fog.IslandDensity = 0.002;
	Weathers[n].Fog.SeaDensity = 0.001;
	Weathers[n].Fog.Color = argb(0,55,45,45);
	
	Weathers[n].Rain.NumDrops = 0;
	Weathers[n].Rain.Color = argb(0,73,73,73);
	Weathers[n].Rain.DropLength = 2.12;
	Weathers[n].Rain.Height = 30.0;
	Weathers[n].Rain.Radius = 30.0;
	Weathers[n].Rain.Speed = 18.0;
	Weathers[n].Rain.Jitter = 0.4;
	Weathers[n].Rain.WindSpeedJitter = 0.5;
	Weathers[n].Rain.MaxBlend = 49;
	Weathers[n].Rain.TimeBlend = 2000;

	Weathers[n].Planets.enable = true;

	Weathers[n].Rainbow.Enable = false;
	Weathers[n].Rainbow.Texture = "weather\rainbow\rainbow.tga";

	Weathers[n].Sun.Color = argb(0,30,30,25);
	Weathers[n].Sun.Ambient = argb(0,58,58,53);
	Weathers[n].Sun.AzimuthAngle = Degree2Radian(320.0);
	Weathers[n].Sun.HeightAngle = 0.28;
	Weathers[n].Sun.Moon = true;
	Weathers[n].Sun.Glow.Enable = true;
	Weathers[n].Sun.Glow.Dist = 3500.0;
	Weathers[n].Sun.Glow.Size = 110.0;
	Weathers[n].Sun.Glow.RotateSpeed = 0.0;
	Weathers[n].Sun.Glow.Texture = "weather\sun\glow\moon.tga";
	Weathers[n].Sun.Glow.DecayTime = 8.0;
	Weathers[n].Sun.Glow.TechniqueNoZ = "sunglow_noz";
	Weathers[n].Sun.Glow.TechniqueZ = "moon_z";
	Weathers[n].Sun.Glow.Color = argb(0,255,255,255);
	Weathers[n].Sun.Overflow.Enable = false;
	Weathers[n].Sun.Overflow.Texture = "Weather\lightning\flash.tga";
	Weathers[n].Sun.Overflow.Color = argb(0, 100, 100, 100);
	Weathers[n].Sun.Overflow.Size = 5500.0;
	Weathers[n].Sun.Overflow.Technique = "sunoverflow";
	Weathers[n].Sun.Overflow.Start = 0.7;
	Weathers[n].Sun.Flare.Enable = false;
	Weathers[n].Sun.Flare.Texture = "weather\sun\flare\allflares.tga";
	Weathers[n].Sun.Flare.Dist = 3500.0;
	Weathers[n].Sun.Flare.TexSizeX = 2;
	Weathers[n].Sun.Flare.TexSizeY = 2;
	Weathers[n].Sun.Flare.Technique = "sunflare";
	Weathers[n].Sun.Flare.Scale = 2.0;
	Weathers[n].Sun.Flares.f1 = "1.0,200.0,0,0x0F0F0F";
	Weathers[n].Sun.Flares.f2 = "0.9,20.0,1,0x0F0F0F";
	Weathers[n].Sun.Flares.f3 = "0.8,30.0,2,0x0F0F0F";
	Weathers[n].Sun.Flares.f4 = "0.6,150.0,0,0x0F0F0F";
	Weathers[n].Sun.Flares.f5 = "0.5,50.0,0,0x0F0F0F";
	Weathers[n].Sun.Flares.f6 = "0.4,100.0,1,0x0F0F0F";
	Weathers[n].Sun.Flares.f7 = "0.3,200.0,0,0x0F0F0F";
	Weathers[n].Sun.Flares.f8 = "0.2,200.0,0,0x0F0F0F";

	Weathers[n].Sea.GF2MX.Sky.Color = argb(0,160,210,255);
	Weathers[n].Sea.GF2MX.Water.Color = argb(0,0,5,10);
	Weathers[n].Sea.GF2MX.Bump.Tile = 0.05;
	Weathers[n].Sea.GF2MX.Bump.AnimSpeed = 7.0;
	Weathers[n].Sea.GF2MX.Bump.Dir = "weather\sea\bump_g2mx\";

	Weathers[n].Sea.Transparency = 0.4;
	Weathers[n].Sea.FrenelCoefficient = 0.5;
	Weathers[n].Sea.WaterReflection = 0.7;
	Weathers[n].Sea.WaterAttenuation = 0.2;
	Weathers[n].Sea.Sky.Color = argb(0,22,22,22);
	Weathers[n].Sea.Water.Color = argb(0,10,20,20);
	Weathers[n].Sea.Pena.Color = argb(0,75,75,55);
	Weathers[n].Sea.Bump.Dir = "resource\textures\weather\sea\bump_g3\";
	Weathers[n].Sea.Bump.Tile = 0.04;
	Weathers[n].Sea.Bump.Ang = 0.0;
	Weathers[n].Sea.Bump.Speed = 0.01;
	Weathers[n].Sea.Bump.AnimSpeed = 15.0;
	Weathers[n].Sea.Bump.Scale = 2.0;
	Weathers[n].Sea.SunRoad.Start = 0.99;
	Weathers[n].Sea.SunRoad.Color1 = argb(0,50,10,0);
	Weathers[n].Sea.SunRoad.Color2 = argb(0,50,50,50);
	Weathers[n].Sea.SunRoad.Power = 0.0;
	//угол, частота, высота волны в метрах, начальная позиция волны, скорость волны
	Weathers[n].Sea.Harmonics.h1 = "0.0, 2.0, 3.0, 0.0, 10.00";
	Weathers[n].Sea.Harmonics.h2 = "90.0, 4.0, 1.5, 0.0, 50.00";
	Weathers[n].Sea.Harmonics.h3 = "45.0, 7.0, 2.5, 0.0, 200.00";
	Weathers[n].Sea.Harmonics.h3 = "135.0, 5.0, 1.0, 0.0, 150.00";
	Weathers[n].Sea.Harmonics.h4 = "90.0, 40.0, 0.2, 0.0, 20.00";

	Weathers[n].Sun.Reflection.Enable = true;
	Weathers[n].Sun.Reflection.Texture = "weather\sun\reflection\refl_night.tga";
	Weathers[n].Sun.Reflection.Technique = "sunreflection";
	Weathers[n].Sun.Reflection.Dist = 3500.0;
	Weathers[n].Sun.Reflection.Size = 500.0;
	Weathers[n].Sun.Reflection.Color = argb(200, 200, 200, 200);

	Weathers[n].Sea2.BumpScale = 0.05; //МЕЛКИЕ ВОЛНЫ
	Weathers[n].Sea2.PosShift = 1.0; //остроконечность волн
	
	Weathers[n].Sea2.WaterColor = argb(0, 20, 30, 45);
	Weathers[n].Sea2.SkyColor = argb(0, 175, 175, 175);

	Weathers[n].Sea2.Reflection = 0.7;
	Weathers[n].Sea2.Transparency = 0.5;
	Weathers[n].Sea2.Frenel = 0.3; 
	Weathers[n].Sea2.Attenuation = 0.3;

	Weathers[n].Sea2.Amp1 = 6.0; //амплитуда 1 волны
	Weathers[n].Sea2.AnimSpeed1 = 10.0; //скорость анимации
	Weathers[n].Sea2.Scale1 = 0.8; //размеры волны
	Weathers[n].Sea2.MoveSpeed1 = "3.0, 0.0, 0.0"; //скорость движения

	Weathers[n].Sea2.Amp2 = 1.25;
	Weathers[n].Sea2.AnimSpeed2 = 19.0;
	Weathers[n].Sea2.Scale2 = 2.5;
	Weathers[n].Sea2.MoveSpeed2 = "0.0, 0.0, -2.5";

	Weathers[n].Sea2.FoamK = 0.1;//яркость пены по высоте 0.1 - пена яркая с 10 метров 1.0 через метр после начала
	Weathers[n].Sea2.FoamV = 3.0;//высота с которой начинается пена
	Weathers[n].Sea2.FoamUV = 1.0; //тайлинг пены
	Weathers[n].Sea2.FoamTexDisturb = 0.7;//сдвиг по нормали. х.з. что это надо поюзать. значение от 0.0 до 1.0

	Weathers[n].Wind.Angle = 2.0;
	Weathers[n].Wind.Speed.Min = 5.0;
	Weathers[n].Wind.Speed.Max = 14.0;
	
	n++;

	return n;
}