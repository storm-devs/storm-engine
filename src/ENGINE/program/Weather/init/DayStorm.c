
int Whr_InitDayStorm(int n)
{
// ====================================================================
// Stormy1 - утренний шторм

	Weathers[n].id = "Storm01";
	Weathers[n].Hour.Min = 6.00001;
	Weathers[n].Hour.Max = 10;
	Weathers[n].Lighting = "storm1";
	Weathers[n].LightingLm = "storm";
	Weathers[n].InsideBack = "s";
	Weathers[n].Sounds.Sea.Postfix = "_storm";

	Weathers[n].Storm = true;
	Weathers[n].Tornado = true;
	Weathers[n].Shark = false;
	Weathers[n].Lights = 0;
	Weathers[n].Night = false;

	Weathers[n].Shadow.Density.Head = argb(255,96,96,96);
	Weathers[n].Shadow.Density.Foot = argb(255,96,96,64);

	Weathers[n].Lightning.Enable = true;
	Weathers[n].Lightning.Texture = "Weather\lightning\lightning_storm.tga";
	Weathers[n].Lightning.FlickerTime = 32;
	Weathers[n].Lightning.SubTexX = 4;
	Weathers[n].Lightning.SubTexY = 1;
	Weathers[n].Lightning.ScaleX = 0.7;
	Weathers[n].Lightning.ScaleY = 1.0;
	Weathers[n].Lightning.Flash.Texture = "Weather\lightning\flash.tga";

	Weathers[n].Sky.Dir = "weather\skies\Storm01\";
	Weathers[n].Sky.Color = argb(0,255,255,255);
	Weathers[n].Sky.Rotate.Min = 0.0;
	Weathers[n].Sky.Rotate.Max = 0.0;
	Weathers[n].Sky.Size = 512.0;
	Weathers[n].Sky.Angle = 0.0;

	Weathers[n].Planets.enable = false;

	Weathers[n].Stars.Enable = false;
	Weathers[n].Stars.Texture = "weather\astronomy\stars.tga";
	Weathers[n].Stars.Color = argb(0, 255, 255, 255);
	Weathers[n].Stars.Radius = 2000.0;
	Weathers[n].Stars.Size = 15.0;
	Weathers[n].Stars.HeightFade = 200.0;
	Weathers[n].Stars.SunFade = 1.0;
	Weathers[n].Stars.VisualMagnitude = 8.0;
	
	Weathers[n].Fog.Enable = true;
	Weathers[n].Fog.Height = 2000;
	Weathers[n].Fog.Start = 1;
	Weathers[n].Fog.Density = 0.02;
	Weathers[n].Fog.IslandDensity = 0.005;
	Weathers[n].Fog.SeaDensity = 0.001;
	Weathers[n].Fog.Color = argb(0,50,60,65);
	
	Weathers[n].Rain.NumDrops = 50000;
	Weathers[n].Rain.Color = argb(0,23,23,23);
	Weathers[n].Rain.DropLength = 2.12;
	Weathers[n].Rain.Height = 30.0;
	Weathers[n].Rain.Radius = 30.0;
	Weathers[n].Rain.Speed = 18.0;
	Weathers[n].Rain.Jitter = 0.4;
	Weathers[n].Rain.WindSpeedJitter = 0.5;
	Weathers[n].Rain.MaxBlend = 49;
	Weathers[n].Rain.TimeBlend = 2000;

	Weathers[n].Rain.DropsNearNum = 500;
	Weathers[n].Rain.DropsFarNum = 500;
	//Weathers[n].Rain.DropsNearRadius = 25.0;
	//Weathers[n].Rain.DropsFarRadius = 75.0;
	//Weathers[n].Rain.DropsLifeTime = 0.25;
	//Weathers[n].Rain.DropsSize = 0.05;
	//Weathers[n].Rain.DropsTexture = "weather\rain_drops.tga";
	//Weathers[n].Rain.DropsColor = argb(63, 255, 255, 255);

	Weathers[n].Rainbow.Enable = false;
	Weathers[n].Rainbow.Texture = "weather\rainbow\rainbow.tga";

	Weathers[n].Sun.Color = argb(0,60,60,60);
	Weathers[n].Sun.Ambient = argb(0,20,20,15);
	Weathers[n].Sun.AzimuthAngle = Degree2Radian(90.0);
	Weathers[n].Sun.HeightAngle = 0.95;
	Weathers[n].Sun.Glow.Enable = false;
	Weathers[n].Sun.Glow.Dist = 3500.0;
	Weathers[n].Sun.Glow.Size = 1250.0;
	Weathers[n].Sun.Glow.RotateSpeed = 1.0;
	Weathers[n].Sun.Glow.Texture = "weather\sun\glow\sunglow.tga";
	Weathers[n].Sun.Glow.DecayTime = 8.0;
	Weathers[n].Sun.Glow.TechniqueNoZ = "sunglow_noz";
	Weathers[n].Sun.Glow.TechniqueZ = "sunglow_z";
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

	Weathers[n].Sea.Transparency = 0.05;
	Weathers[n].Sea.FrenelCoefficient = 0.5;
	Weathers[n].Sea.WaterReflection = 0.9;
	Weathers[n].Sea.WaterAttenuation = 0.2;
	Weathers[n].Sea.Sky.Color = argb(0,60,70,80);
	Weathers[n].Sea.Water.Color = argb(0,60,70,70);
	Weathers[n].Sea.Pena.Color = argb(192,175,175,155);
	Weathers[n].Sea.Bump.Dir = "resource\textures\weather\sea\bump_g3\";
	Weathers[n].Sea.Bump.Tile = 0.04;
	Weathers[n].Sea.Bump.Ang = 0.0;
	Weathers[n].Sea.Bump.Speed = 0.01;
	Weathers[n].Sea.Bump.AnimSpeed = 15.0;
	Weathers[n].Sea.Bump.Scale = 2.0;
	Weathers[n].Sea.SunRoad.Start = 0.99;
	Weathers[n].Sea.SunRoad.Color1 = argb(0,128,55,0);
	Weathers[n].Sea.SunRoad.Color2 = argb(0,255,200,150);
	Weathers[n].Sea.SunRoad.Power = 0.2;
	//угол, частота, высота волны в метрах, начальная позиция волны, скорость волны
	Weathers[n].Sea.Harmonics.h1 = "90.0, 1.5, 8.0, 90.0, 130.0";
	Weathers[n].Sea.Harmonics.h2 = "0.0, 3.0, 5.0, 45.0, 160.0";
	Weathers[n].Sea.Harmonics.h3 = "45.0, 6.0, 2.5, 0.0, 120.0";
	Weathers[n].Sea.Harmonics.h4 = "180.0, 24.0, 0.8, 360.0, 240.0";
	Weathers[n].Sea.Harmonics.h5 = "0.0, 18.0, 0.6, 240.0, 180.0";

	Weathers[n].Sun.Reflection.Enable = false;
	Weathers[n].Sun.Reflection.Texture = "weather\sun\reflection\refl_day.tga";
	Weathers[n].Sun.Reflection.Technique = "sunreflection";
	Weathers[n].Sun.Reflection.Dist = 3500.0;
	Weathers[n].Sun.Reflection.Size = 500.0;
	Weathers[n].Sun.Reflection.Color = argb(200, 200, 200, 200);

	Weathers[n].Sea2.BumpScale = 0.05; //МЕЛКИЕ ВОЛНЫ
	Weathers[n].Sea2.PosShift = 2.0; //остроконечность волн
	
	Weathers[n].Sea2.WaterColor = argb(0, 60, 70, 80);
	Weathers[n].Sea2.SkyColor = argb(0, 155, 155, 155);

	Weathers[n].Sea2.Reflection = 0.5;
	Weathers[n].Sea2.Transparency = 0.05;
	Weathers[n].Sea2.Frenel = 0.6; 
	Weathers[n].Sea2.Attenuation = 0.1;

	Weathers[n].Sea2.Amp1 = 40.0; //амплитуда 1 волны
	Weathers[n].Sea2.AnimSpeed1 = 4.0; //скорость анимации
	Weathers[n].Sea2.Scale1 = 0.12; //размеры волны
	Weathers[n].Sea2.MoveSpeed1 = "3.0, 0.0, 3.0"; //скорость движения

	Weathers[n].Sea2.Amp2 = 2.0; //амплитуда 1 волны
	Weathers[n].Sea2.AnimSpeed2 = 9.0; //скорость анимации
	Weathers[n].Sea2.Scale2 = 1.3; //размеры волны
	Weathers[n].Sea2.MoveSpeed2 = "0.0, 0.0, 5.0"; //скорость движения

	Weathers[n].Sea2.FoamK = 0.05;//яркость пены по высоте 0.1 - пена яркая с 10 метров 1.0 через метр после начала
	Weathers[n].Sea2.FoamV = 9.0;//высота с которой начинается пена
	Weathers[n].Sea2.FoamUV = 0.1; //тайлинг пены
	Weathers[n].Sea2.FoamTexDisturb = 0.7;//сдвиг по нормали. х.з. что это надо поюзать. значение от 0.0 до 1.0

	Weathers[n].Wind.Angle = 2.0;
	Weathers[n].Wind.Speed.Min = 15.0;
	Weathers[n].Wind.Speed.Max = 18.0;
	
	n++;

// ====================================================================
// Overcast2 - дневной шторм

	Weathers[n].id = "Storm02";
	Weathers[n].Hour.Min = 10.00001;
	Weathers[n].Hour.Max = 18;
	Weathers[n].Lighting = "storm2";
	Weathers[n].LightingLm = "storm";
	Weathers[n].InsideBack = "s";
	Weathers[n].Sounds.Sea.Postfix = "_storm";

	Weathers[n].Storm = true;
	Weathers[n].Tornado = true;
	Weathers[n].Shark = false;
	Weathers[n].Lights = 0;
	Weathers[n].Night = false;

	Weathers[n].Shadow.Density.Head = argb(255,96,96,96);
	Weathers[n].Shadow.Density.Foot = argb(255,96,96,64);

	Weathers[n].Lightning.Enable = true;
	Weathers[n].Lightning.Texture = "Weather\lightning\lightning_storm.tga";
	Weathers[n].Lightning.FlickerTime = 32;
	Weathers[n].Lightning.SubTexX = 4;
	Weathers[n].Lightning.SubTexY = 1;
	Weathers[n].Lightning.ScaleX = 0.7;
	Weathers[n].Lightning.ScaleY = 1.0;
	Weathers[n].Lightning.Flash.Texture = "Weather\lightning\flash.tga";

	Weathers[n].Sky.Dir = "weather\skies\Storm02\";
	Weathers[n].Sky.Color = argb(0,255,255,255);
	Weathers[n].Sky.Rotate.Min = 0.0;
	Weathers[n].Sky.Rotate.Max = 0.0;
	Weathers[n].Sky.Size = 512.0;
	Weathers[n].Sky.Angle = 0.0;

	Weathers[n].Planets.enable = false;

	Weathers[n].Stars.Enable = false;
	Weathers[n].Stars.Texture = "weather\astronomy\stars.tga";
	Weathers[n].Stars.Color = argb(0, 255, 255, 255);
	Weathers[n].Stars.Radius = 2000.0;
	Weathers[n].Stars.Size = 15.0;
	Weathers[n].Stars.HeightFade = 200.0;
	Weathers[n].Stars.SunFade = 1.0;
	Weathers[n].Stars.VisualMagnitude = 8.0;
	
	Weathers[n].Fog.Enable = true;
	Weathers[n].Fog.Height = 2000;
	Weathers[n].Fog.Start = 1;
	Weathers[n].Fog.Density = 0.02;
	Weathers[n].Fog.IslandDensity = 0.005;
	Weathers[n].Fog.SeaDensity = 0.001;
	Weathers[n].Fog.Color = argb(0,100,95,85);
	//athers[n].Fog.Color = argb(0,100,95,85);
	
	Weathers[n].Rain.NumDrops = 50000;
	Weathers[n].Rain.Color = argb(0,13,13,13);
	Weathers[n].Rain.DropLength = 4.12;
	Weathers[n].Rain.Height = 30.0;
	Weathers[n].Rain.Radius = 30.0;
	Weathers[n].Rain.Speed = 28.0;
	Weathers[n].Rain.Jitter = 0.4;
	Weathers[n].Rain.WindSpeedJitter = 0.5;
	Weathers[n].Rain.MaxBlend = 49;
	Weathers[n].Rain.TimeBlend = 2000;

	Weathers[n].Rain.DropsNearNum = 500;
	Weathers[n].Rain.DropsFarNum = 500;
	//Weathers[n].Rain.DropsNearRadius = 25.0;
	//Weathers[n].Rain.DropsFarRadius = 75.0;
	//Weathers[n].Rain.DropsLifeTime = 0.25;
	//Weathers[n].Rain.DropsSize = 0.05;
	//Weathers[n].Rain.DropsTexture = "weather\rain_drops.tga";
	//Weathers[n].Rain.DropsColor = argb(63, 255, 255, 255);

	Weathers[n].Rainbow.Enable = false;
	Weathers[n].Rainbow.Texture = "weather\rainbow\rainbow.tga";

	Weathers[n].Sun.Color = argb(0,60,60,50);
	Weathers[n].Sun.Ambient = argb(0,25,25,25);
	Weathers[n].Sun.AzimuthAngle = Degree2Radian(90.0);
	Weathers[n].Sun.HeightAngle = 0.95;
	Weathers[n].Sun.Glow.Enable = false;
	Weathers[n].Sun.Glow.Dist = 3500.0;
	Weathers[n].Sun.Glow.Size = 1250.0;
	Weathers[n].Sun.Glow.RotateSpeed = 1.0;
	Weathers[n].Sun.Glow.Texture = "weather\sun\glow\sunglow.tga";
	Weathers[n].Sun.Glow.DecayTime = 8.0;
	Weathers[n].Sun.Glow.TechniqueNoZ = "sunglow_noz";
	Weathers[n].Sun.Glow.TechniqueZ = "sunglow_z";
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

	Weathers[n].Sea.Transparency = 0.1;
	Weathers[n].Sea.FrenelCoefficient = 0.5;
	Weathers[n].Sea.WaterReflection = 0.9;
	Weathers[n].Sea.WaterAttenuation = 0.2;
	Weathers[n].Sea.Sky.Color = argb(0,40,50,60);
	Weathers[n].Sea.Water.Color = argb(0,60,70,70);
	Weathers[n].Sea.Pena.Color = argb(192,175,175,155);
	Weathers[n].Sea.Bump.Dir = "resource\textures\weather\sea\bump_g3\";
	Weathers[n].Sea.Bump.Tile = 0.04;
	Weathers[n].Sea.Bump.Ang = 0.0;
	Weathers[n].Sea.Bump.Speed = 0.01;
	Weathers[n].Sea.Bump.AnimSpeed = 15.0;
	Weathers[n].Sea.Bump.Scale = 2.0;
	Weathers[n].Sea.SunRoad.Start = 0.99;
	Weathers[n].Sea.SunRoad.Color1 = argb(0,128,55,0);
	Weathers[n].Sea.SunRoad.Color2 = argb(0,255,200,150);
	Weathers[n].Sea.SunRoad.Power = 0.2;
	//угол, частота, высота волны в метрах, начальная позиция волны, скорость волны
	Weathers[n].Sea.Harmonics.h1 = "0.0, 0.5, 7.0, 0.0, 100.0";
	Weathers[n].Sea.Harmonics.h2 = "90.0, 3.0, 4.0, 45.0, 220.0";
	Weathers[n].Sea.Harmonics.h3 = "35.0, 6.0, 1.5, 0.0, 260.0";
	Weathers[n].Sea.Harmonics.h4 = "160.0, 14.0, 0.8, 360.0, 320.0";
	Weathers[n].Sea.Harmonics.h5 = "20.0, 18.0, 0.6, 240.0, 260.0";
	Weathers[n].Sea.Harmonics.h6 = "0.0, 50.0, 0.2, 0.0, 460.0";

	Weathers[n].Sun.Reflection.Enable = false;
	Weathers[n].Sun.Reflection.Texture = "weather\sun\reflection\refl_day.tga";
	Weathers[n].Sun.Reflection.Technique = "sunreflection";
	Weathers[n].Sun.Reflection.Dist = 3500.0;
	Weathers[n].Sun.Reflection.Size = 500.0;
	Weathers[n].Sun.Reflection.Color = argb(200, 200, 200, 200);

	Weathers[n].Sea2.BumpScale = 0.05; //МЕЛКИЕ ВОЛНЫ
	Weathers[n].Sea2.PosShift = 0.3; //остроконечность волн
	
	Weathers[n].Sea2.WaterColor = argb(0, 50, 65, 70);
	Weathers[n].Sea2.SkyColor = argb(0, 255, 255, 255);

	Weathers[n].Sea2.Reflection = 0.8;
	Weathers[n].Sea2.Transparency = 0.05;
	Weathers[n].Sea2.Frenel = 0.3; 
	Weathers[n].Sea2.Attenuation = 0.3;

	Weathers[n].Sea2.Amp1 = 100.0; //амплитуда 1 волны
	Weathers[n].Sea2.AnimSpeed1 = 1.0; //скорость анимации
	Weathers[n].Sea2.Scale1 = 0.08; //размеры волны
	Weathers[n].Sea2.MoveSpeed1 = "1.0, 0.0, 10.0"; //скорость движения

	Weathers[n].Sea2.Amp2 = 3.0; //амплитуда 1 волны
	Weathers[n].Sea2.AnimSpeed2 = 2.0; //скорость анимации
	Weathers[n].Sea2.Scale2 = 1.3; //размеры волны
	Weathers[n].Sea2.MoveSpeed2 = "2.0, 0.0, 9.0"; //скорость движения

	Weathers[n].Sea2.FoamK = 0.02;//яркость пены по высоте 0.1 - пена яркая с 10 метров 1.0 через метр после начала
	Weathers[n].Sea2.FoamV = 30.0;//высота с которой начинается пена
	Weathers[n].Sea2.FoamUV = 0.5; //тайлинг пены
	Weathers[n].Sea2.FoamTexDisturb = 0.7;//сдвиг по нормали. х.з. что это надо поюзать. значение от 0.0 до 1.0

	Weathers[n].Wind.Angle = 2.0;
	Weathers[n].Wind.Speed.Min = 12.0;
	Weathers[n].Wind.Speed.Max = 14.0;
	
	n++;

// ====================================================================
// Вечерний шторм

	Weathers[n].id = "Storm03";
	Weathers[n].Hour.Min = 18.00001;
	Weathers[n].Hour.Max = 22;
	Weathers[n].Lighting = "storm3";
	Weathers[n].LightingLm = "storm";
	Weathers[n].InsideBack = "s";
	Weathers[n].Sounds.Sea.Postfix = "_storm";

	Weathers[n].Storm = true;
	Weathers[n].Tornado = true;
	Weathers[n].Shark = false;
	Weathers[n].Lights = 0;
	Weathers[n].Night = true;

	Weathers[n].Shadow.Density.Head = argb(255,96,96,96);
	Weathers[n].Shadow.Density.Foot = argb(255,96,96,64);

	Weathers[n].Lightning.Enable = true;
	Weathers[n].Lightning.Texture = "Weather\lightning\lightning_storm.tga";
	Weathers[n].Lightning.FlickerTime = 32;
	Weathers[n].Lightning.SubTexX = 4;
	Weathers[n].Lightning.SubTexY = 1;
	Weathers[n].Lightning.ScaleX = 0.7;
	Weathers[n].Lightning.ScaleY = 1.0;
	Weathers[n].Lightning.Flash.Texture = "Weather\lightning\flash.tga";

	Weathers[n].Sky.Dir = "weather\skies\Storm03\";
	Weathers[n].Sky.Color = argb(0,255,255,255);
	Weathers[n].Sky.Rotate.Min = 0.0;
	Weathers[n].Sky.Rotate.Max = 0.0;
	Weathers[n].Sky.Size = 512.0;
	Weathers[n].Sky.Angle = 0.0;

	Weathers[n].Planets.enable = false;

	Weathers[n].Stars.Enable = false;
	Weathers[n].Stars.Texture = "weather\astronomy\stars.tga";
	Weathers[n].Stars.Color = argb(0, 255, 255, 255);
	Weathers[n].Stars.Radius = 2000.0;
	Weathers[n].Stars.Size = 15.0;
	Weathers[n].Stars.HeightFade = 200.0;
	Weathers[n].Stars.SunFade = 1.0;
	Weathers[n].Stars.VisualMagnitude = 8.0;
	
	Weathers[n].Fog.Enable = true;
	Weathers[n].Fog.Height = 2000;
	Weathers[n].Fog.Start = 1;
	Weathers[n].Fog.Density = 0.02;
	Weathers[n].Fog.IslandDensity = 0.005;
	Weathers[n].Fog.SeaDensity = 0.001;
	Weathers[n].Fog.Color = argb(0,50,50,50);
	
	Weathers[n].Rain.NumDrops = 50000;
	Weathers[n].Rain.Color = argb(0,33,33,33);
	Weathers[n].Rain.DropLength = 2.12;
	Weathers[n].Rain.Height = 30.0;
	Weathers[n].Rain.Radius = 30.0;
	Weathers[n].Rain.Speed = 18.0;
	Weathers[n].Rain.Jitter = 0.4;
	Weathers[n].Rain.WindSpeedJitter = 0.5;
	Weathers[n].Rain.MaxBlend = 49;
	Weathers[n].Rain.TimeBlend = 2000;

	Weathers[n].Rain.DropsNearNum = 500;
	Weathers[n].Rain.DropsFarNum = 500;
	//Weathers[n].Rain.DropsNearRadius = 25.0;
	//Weathers[n].Rain.DropsFarRadius = 75.0;
	//Weathers[n].Rain.DropsLifeTime = 0.25;
	//Weathers[n].Rain.DropsSize = 0.05;
	//Weathers[n].Rain.DropsTexture = "weather\rain_drops.tga";
	//Weathers[n].Rain.DropsColor = argb(63, 255, 255, 255);

	Weathers[n].Rainbow.Enable = false;
	Weathers[n].Rainbow.Texture = "weather\rainbow\rainbow.tga";

	Weathers[n].Sun.Color = argb(0,85,75,70);
	Weathers[n].Sun.Ambient = argb(0,40,40,40);
	Weathers[n].Sun.AzimuthAngle = Degree2Radian(90.0);
	Weathers[n].Sun.HeightAngle = 0.95;
	Weathers[n].Sun.Glow.Enable = false;
	Weathers[n].Sun.Glow.Dist = 3500.0;
	Weathers[n].Sun.Glow.Size = 1250.0;
	Weathers[n].Sun.Glow.RotateSpeed = 1.0;
	Weathers[n].Sun.Glow.Texture = "weather\sun\glow\sunglow.tga";
	Weathers[n].Sun.Glow.DecayTime = 8.0;
	Weathers[n].Sun.Glow.TechniqueNoZ = "sunglow_noz";
	Weathers[n].Sun.Glow.TechniqueZ = "sunglow_z";
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
	Weathers[n].Sea.WaterReflection = 0.8;
	Weathers[n].Sea.WaterAttenuation = 0.2;
	Weathers[n].Sea.Sky.Color = argb(0,180,180,180);
	Weathers[n].Sea.Water.Color = argb(0,60,95,95);
	Weathers[n].Sea.Pena.Color = argb(0,175,175,155);
	Weathers[n].Sea.Bump.Dir = "resource\textures\weather\sea\bump_g3\";
	Weathers[n].Sea.Bump.Tile = 0.04;
	Weathers[n].Sea.Bump.Ang = 0.0;
	Weathers[n].Sea.Bump.Speed = 0.01;
	Weathers[n].Sea.Bump.AnimSpeed = 15.0;
	Weathers[n].Sea.Bump.Scale = 2.0;
	Weathers[n].Sea.SunRoad.Start = 0.99;
	Weathers[n].Sea.SunRoad.Color1 = argb(0,128,55,0);
	Weathers[n].Sea.SunRoad.Color2 = argb(0,255,200,150);
	Weathers[n].Sea.SunRoad.Power = 0.2;
	//угол, частота, высота волны в метрах, начальная позиция волны, скорость волны
	Weathers[n].Sea.Harmonics.h1 = "0.0, 0.5, 5.0, 0.0, 20.0";
	Weathers[n].Sea.Harmonics.h2 = "45.0, 1.5, 2.0, 0.0, 35.0";
	Weathers[n].Sea.Harmonics.h3 = "90.0, 2.0, 3.5, 0.0, 50.0";
	Weathers[n].Sea.Harmonics.h4 = "135.0, 10.0, 0.4, 0.0, 80.0";
	Weathers[n].Sea.Harmonics.h5 = "270.0, 8.0, 0.45, 0.0, 40.0";


	Weathers[n].Sun.Reflection.Enable = false;
	Weathers[n].Sun.Reflection.Texture = "weather\sun\reflection\refl_evening.tga";
	Weathers[n].Sun.Reflection.Technique = "sunreflection";
	Weathers[n].Sun.Reflection.Dist = 3500.0;
	Weathers[n].Sun.Reflection.Size = 500.0;
	Weathers[n].Sun.Reflection.Color = argb(200, 200, 200, 200);

	Weathers[n].Sea2.BumpScale = 0.05; //МЕЛКИЕ ВОЛНЫ
	Weathers[n].Sea2.PosShift = 0.8; //остроконечность волн
	
	Weathers[n].Sea2.WaterColor = argb(0, 85, 105, 100);
	Weathers[n].Sea2.SkyColor = argb(0, 155, 155, 155);

	Weathers[n].Sea2.Reflection = 0.2;
	Weathers[n].Sea2.Transparency = 0.05;
	Weathers[n].Sea2.Frenel = 0.6; 
	Weathers[n].Sea2.Attenuation = 0.2;

	Weathers[n].Sea2.Amp1 = 70.0; //амплитуда 1 волны
	Weathers[n].Sea2.AnimSpeed1 = 1.5; //скорость анимации
	Weathers[n].Sea2.Scale1 = 0.09; //размеры волны
	Weathers[n].Sea2.MoveSpeed1 = "9.0, 0.0, 0.0"; //скорость движения

	Weathers[n].Sea2.Amp2 = 3.0;
	Weathers[n].Sea2.AnimSpeed2 = 4.0;
	Weathers[n].Sea2.Scale2 = 2.0;
	Weathers[n].Sea2.MoveSpeed2 = "2.5, 0.0, 0.0";

	Weathers[n].Sea2.FoamK = 0.02;//яркость пены по высоте 0.1 - пена яркая с 10 метров 1.0 через метр после начала
	Weathers[n].Sea2.FoamV = 12.0;//высота с которой начинается пена
	Weathers[n].Sea2.FoamUV = 0.3; //тайлинг пены
	Weathers[n].Sea2.FoamTexDisturb = 0.7;//сдвиг по нормали. х.з. что это надо поюзать. значение от 0.0 до 1.0
	

	Weathers[n].Wind.Angle = 2.0;
	Weathers[n].Wind.Speed.Min = 13.0;
	Weathers[n].Wind.Speed.Max = 16.0;
	
	n++;

// ====================================================================
// Ночной шторм

	Weathers[n].id = "Storm04";
	Weathers[n].Hour.Min = 22.00001;
	Weathers[n].Hour.Max = 6;
	Weathers[n].Lighting = "storm4";
	Weathers[n].LightingLm = "storm";
	Weathers[n].InsideBack = "s";
	Weathers[n].Sounds.Sea.Postfix = "_storm";

	Weathers[n].Storm = true;
	Weathers[n].Tornado = true;
	Weathers[n].Shark = false;
	Weathers[n].Lights = 1;
	Weathers[n].Night = true;

	Weathers[n].Shadow.Density.Head = argb(255,96,96,96);
	Weathers[n].Shadow.Density.Foot = argb(255,96,96,64);

	Weathers[n].Lightning.Enable = true;
	Weathers[n].Lightning.Texture = "Weather\lightning\lightning_storm.tga";
	Weathers[n].Lightning.FlickerTime = 32;
	Weathers[n].Lightning.SubTexX = 4;
	Weathers[n].Lightning.SubTexY = 1;
	Weathers[n].Lightning.ScaleX = 0.7;
	Weathers[n].Lightning.ScaleY = 1.0;
	Weathers[n].Lightning.Flash.Texture = "Weather\lightning\flash.tga";

	Weathers[n].Sky.Dir = "weather\skies\Storm04\";
	Weathers[n].Sky.Color = argb(0,255,255,255);
	Weathers[n].Sky.Rotate.Min = 0.0;
	Weathers[n].Sky.Rotate.Max = 0.0;
	Weathers[n].Sky.Size = 512.0;
	Weathers[n].Sky.Angle = 0.0;

	Weathers[n].Planets.enable = false;

	Weathers[n].Stars.Enable = false;
	Weathers[n].Stars.Texture = "weather\astronomy\stars.tga";
	Weathers[n].Stars.Color = argb(0, 255, 255, 255);
	Weathers[n].Stars.Radius = 2000.0;
	Weathers[n].Stars.Size = 15.0;
	Weathers[n].Stars.HeightFade = 200.0;
	Weathers[n].Stars.SunFade = 1.0;
	Weathers[n].Stars.VisualMagnitude = 8.0;
	
	Weathers[n].Fog.Enable = true;
	Weathers[n].Fog.Height = 2000;
	Weathers[n].Fog.Start = 1;
	Weathers[n].Fog.Density = 0.01;
	Weathers[n].Fog.IslandDensity = 0.0025;
	Weathers[n].Fog.SeaDensity = 0.001;
	Weathers[n].Fog.Color = argb(0,40,40,30);
	
	Weathers[n].Rain.NumDrops = 25000;
	Weathers[n].Rain.Color = argb(0,43,43,33);
	Weathers[n].Rain.DropLength = 3.12;
	Weathers[n].Rain.Height = 30.0;
	Weathers[n].Rain.Radius = 30.0;
	Weathers[n].Rain.Speed = 18.0;
	Weathers[n].Rain.Jitter = 0.4;
	Weathers[n].Rain.WindSpeedJitter = 0.5;
	Weathers[n].Rain.MaxBlend = 49;
	Weathers[n].Rain.TimeBlend = 2000;

	Weathers[n].Rain.DropsNearNum = 500;
	Weathers[n].Rain.DropsFarNum = 500;
	//Weathers[n].Rain.DropsNearRadius = 25.0;
	//Weathers[n].Rain.DropsFarRadius = 75.0;
	//Weathers[n].Rain.DropsLifeTime = 0.25;
	//Weathers[n].Rain.DropsSize = 0.05;
	//Weathers[n].Rain.DropsTexture = "weather\rain_drops.tga";
	//Weathers[n].Rain.DropsColor = argb(63, 255, 255, 255);

	Weathers[n].Rainbow.Enable = false;
	Weathers[n].Rainbow.Texture = "weather\rainbow\rainbow.tga";

	Weathers[n].Sun.Color = argb(0,30,30,15);
	Weathers[n].Sun.Ambient = argb(0,30,30,25);
	Weathers[n].Sun.AzimuthAngle = Degree2Radian(90.0);
	Weathers[n].Sun.HeightAngle = 0.95;
	Weathers[n].Sun.Glow.Enable = false;
	Weathers[n].Sun.Glow.Dist = 3500.0;
	Weathers[n].Sun.Glow.Size = 1250.0;
	Weathers[n].Sun.Glow.RotateSpeed = 1.0;
	Weathers[n].Sun.Glow.Texture = "weather\sun\glow\sunglow.tga";
	Weathers[n].Sun.Glow.DecayTime = 8.0;
	Weathers[n].Sun.Glow.TechniqueNoZ = "sunglow_noz";
	Weathers[n].Sun.Glow.TechniqueZ = "sunglow_z";
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
	Weathers[n].Sea.WaterReflection = 0.8;
	Weathers[n].Sea.WaterAttenuation = 0.2;
	Weathers[n].Sea.Sky.Color = argb(0,180,180,180);
	Weathers[n].Sea.Water.Color = argb(0,60,95,95);
	Weathers[n].Sea.Pena.Color = argb(0,175,175,155);
	Weathers[n].Sea.Bump.Dir = "resource\textures\weather\sea\bump_g3\";
	Weathers[n].Sea.Bump.Tile = 0.04;
	Weathers[n].Sea.Bump.Ang = 0.0;
	Weathers[n].Sea.Bump.Speed = 0.01;
	Weathers[n].Sea.Bump.AnimSpeed = 15.0;
	Weathers[n].Sea.Bump.Scale = 2.0;
	Weathers[n].Sea.SunRoad.Start = 0.99;
	Weathers[n].Sea.SunRoad.Color1 = argb(0,128,55,0);
	Weathers[n].Sea.SunRoad.Color2 = argb(0,255,200,150);
	Weathers[n].Sea.SunRoad.Power = 0.2;
	//угол, частота, высота волны в метрах, начальная позиция волны, скорость волны
	Weathers[n].Sea.Harmonics.h1 = "0.0, 0.5, 5.0, 0.0, 20.0";
	Weathers[n].Sea.Harmonics.h2 = "45.0, 1.5, 2.0, 0.0, 35.0";
	Weathers[n].Sea.Harmonics.h3 = "90.0, 2.0, 3.5, 0.0, 50.0";
	Weathers[n].Sea.Harmonics.h4 = "135.0, 10.0, 0.4, 0.0, 80.0";
	Weathers[n].Sea.Harmonics.h5 = "270.0, 8.0, 0.45, 0.0, 40.0";


	Weathers[n].Sun.Reflection.Enable = false;
	Weathers[n].Sun.Reflection.Texture = "weather\sun\reflection\refl_evening.tga";
	Weathers[n].Sun.Reflection.Technique = "sunreflection";
	Weathers[n].Sun.Reflection.Dist = 3500.0;
	Weathers[n].Sun.Reflection.Size = 500.0;
	Weathers[n].Sun.Reflection.Color = argb(200, 200, 200, 200);

	Weathers[n].Sea2.BumpScale = 0.05; //МЕЛКИЕ ВОЛНЫ
	Weathers[n].Sea2.PosShift = 2.0; //остроконечность волн
	
	Weathers[n].Sea2.WaterColor = argb(0, 60, 65, 60);
	Weathers[n].Sea2.SkyColor = argb(0, 255, 255, 255);

	Weathers[n].Sea2.Reflection = 0.7;
	Weathers[n].Sea2.Transparency = 0.05;
	Weathers[n].Sea2.Frenel = 0.6; 
	Weathers[n].Sea2.Attenuation = 0.2;

	Weathers[n].Sea2.Amp1 = 100.0; //амплитуда 1 волны
	Weathers[n].Sea2.AnimSpeed1 = 0.0; //скорость анимации
	Weathers[n].Sea2.Scale1 = 0.07; //размеры волны
	Weathers[n].Sea2.MoveSpeed1 = "15.0, 0.0, 0.0"; //скорость движения

	Weathers[n].Sea2.Amp2 = 2.5;
	Weathers[n].Sea2.AnimSpeed2 = 4.0;
	Weathers[n].Sea2.Scale2 = 2.0;
	Weathers[n].Sea2.MoveSpeed2 = "1.0, 0.0, 0.0";

	Weathers[n].Sea2.FoamK = 0.01;//яркость пены по высоте 0.1 - пена яркая с 10 метров 1.0 через метр после начала
	Weathers[n].Sea2.FoamV = 12.0;//высота с которой начинается пена
	Weathers[n].Sea2.FoamUV = 0.2; //тайлинг пены
	Weathers[n].Sea2.FoamTexDisturb = 0.7;//сдвиг по нормали. х.з. что это надо поюзать. значение от 0.0 до 1.0
	

	Weathers[n].Wind.Angle = 2.0;
	Weathers[n].Wind.Speed.Min = 12.0;
	Weathers[n].Wind.Speed.Max = 17.0;
	
	n++;

// ====================================================================
// добавочные штормы для локций в низкоуровневым волнением
// ====================================================================
// Stormy1 - утренний шторм для локаций

	Weathers[n].id = "Storm01_add";
	Weathers[n].Hour.Min = 6.00001;
	Weathers[n].Hour.Max = 10;
	Weathers[n].Lighting = "storm1";
	Weathers[n].LightingLm = "storm";
	Weathers[n].InsideBack = "s";
	Weathers[n].Sounds.Sea.Postfix = "_storm";

	Weathers[n].Storm = true;
	Weathers[n].skip = true;  //добавленная
	Weathers[n].Tornado = true;
	Weathers[n].Shark = false;
	Weathers[n].Lights = 0;
	Weathers[n].Night = false;

	Weathers[n].Shadow.Density.Head = argb(255,96,96,96);
	Weathers[n].Shadow.Density.Foot = argb(255,96,96,64);

	Weathers[n].Lightning.Enable = true;
	Weathers[n].Lightning.Texture = "Weather\lightning\lightning_storm.tga";
	Weathers[n].Lightning.FlickerTime = 32;
	Weathers[n].Lightning.SubTexX = 4;
	Weathers[n].Lightning.SubTexY = 1;
	Weathers[n].Lightning.ScaleX = 0.7;
	Weathers[n].Lightning.ScaleY = 1.0;
	Weathers[n].Lightning.Flash.Texture = "Weather\lightning\flash.tga";

	Weathers[n].Sky.Dir = "weather\skies\Storm01\";
	Weathers[n].Sky.Color = argb(0,255,255,255);
	Weathers[n].Sky.Rotate.Min = 0.0;
	Weathers[n].Sky.Rotate.Max = 0.0;
	Weathers[n].Sky.Size = 512.0;
	Weathers[n].Sky.Angle = 0.0;

	Weathers[n].Planets.enable = false;

	Weathers[n].Stars.Enable = false;
	Weathers[n].Stars.Texture = "weather\astronomy\stars.tga";
	Weathers[n].Stars.Color = argb(0, 255, 255, 255);
	Weathers[n].Stars.Radius = 2000.0;
	Weathers[n].Stars.Size = 15.0;
	Weathers[n].Stars.HeightFade = 200.0;
	Weathers[n].Stars.SunFade = 1.0;
	Weathers[n].Stars.VisualMagnitude = 8.0;
	
	Weathers[n].Fog.Enable = true;
	Weathers[n].Fog.Height = 2000;
	Weathers[n].Fog.Start = 1;
	Weathers[n].Fog.Density = 0.02;
	Weathers[n].Fog.IslandDensity = 0.005;
	Weathers[n].Fog.SeaDensity = 0.001;
	Weathers[n].Fog.Color = argb(0,50,60,65);
	
	Weathers[n].Rain.NumDrops = 50000;
	Weathers[n].Rain.Color = argb(0,23,23,23);
	Weathers[n].Rain.DropLength = 2.12;
	Weathers[n].Rain.Height = 30.0;
	Weathers[n].Rain.Radius = 30.0;
	Weathers[n].Rain.Speed = 18.0;
	Weathers[n].Rain.Jitter = 0.4;
	Weathers[n].Rain.WindSpeedJitter = 0.5;
	Weathers[n].Rain.MaxBlend = 49;
	Weathers[n].Rain.TimeBlend = 2000;

	Weathers[n].Rain.DropsNearNum = 500;
	Weathers[n].Rain.DropsFarNum = 500;
	//Weathers[n].Rain.DropsNearRadius = 25.0;
	//Weathers[n].Rain.DropsFarRadius = 75.0;
	//Weathers[n].Rain.DropsLifeTime = 0.25;
	//Weathers[n].Rain.DropsSize = 0.05;
	//Weathers[n].Rain.DropsTexture = "weather\rain_drops.tga";
	//Weathers[n].Rain.DropsColor = argb(63, 255, 255, 255);

	Weathers[n].Rainbow.Enable = false;
	Weathers[n].Rainbow.Texture = "weather\rainbow\rainbow.tga";

	Weathers[n].Sun.Color = argb(0,60,60,60);
	Weathers[n].Sun.Ambient = argb(0,20,20,15);
	Weathers[n].Sun.AzimuthAngle = Degree2Radian(90.0);
	Weathers[n].Sun.HeightAngle = 0.95;
	Weathers[n].Sun.Glow.Enable = false;
	Weathers[n].Sun.Glow.Dist = 3500.0;
	Weathers[n].Sun.Glow.Size = 1250.0;
	Weathers[n].Sun.Glow.RotateSpeed = 1.0;
	Weathers[n].Sun.Glow.Texture = "weather\sun\glow\sunglow.tga";
	Weathers[n].Sun.Glow.DecayTime = 8.0;
	Weathers[n].Sun.Glow.TechniqueNoZ = "sunglow_noz";
	Weathers[n].Sun.Glow.TechniqueZ = "sunglow_z";
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

	Weathers[n].Sea.Transparency = 0.05;
	Weathers[n].Sea.FrenelCoefficient = 0.5;
	Weathers[n].Sea.WaterReflection = 0.9;
	Weathers[n].Sea.WaterAttenuation = 0.2;
	Weathers[n].Sea.Sky.Color = argb(0,60,70,80);
	Weathers[n].Sea.Water.Color = argb(0,60,70,70);
	Weathers[n].Sea.Pena.Color = argb(192,175,175,155);
	Weathers[n].Sea.Bump.Dir = "resource\textures\weather\sea\bump_g3\";
	Weathers[n].Sea.Bump.Tile = 0.04;
	Weathers[n].Sea.Bump.Ang = 0.0;
	Weathers[n].Sea.Bump.Speed = 0.01;
	Weathers[n].Sea.Bump.AnimSpeed = 15.0;
	Weathers[n].Sea.Bump.Scale = 2.0;
	Weathers[n].Sea.SunRoad.Start = 0.99;
	Weathers[n].Sea.SunRoad.Color1 = argb(0,128,55,0);
	Weathers[n].Sea.SunRoad.Color2 = argb(0,255,200,150);
	Weathers[n].Sea.SunRoad.Power = 0.2;
	//угол, частота, высота волны в метрах, начальная позиция волны, скорость волны
	Weathers[n].Sea.Harmonics.h1 = "90.0, 1.5, 8.0, 90.0, 130.0";
	Weathers[n].Sea.Harmonics.h2 = "0.0, 3.0, 5.0, 45.0, 160.0";
	Weathers[n].Sea.Harmonics.h3 = "45.0, 6.0, 2.5, 0.0, 120.0";
	Weathers[n].Sea.Harmonics.h4 = "180.0, 24.0, 0.8, 360.0, 240.0";
	Weathers[n].Sea.Harmonics.h5 = "0.0, 18.0, 0.6, 240.0, 180.0";

	Weathers[n].Sun.Reflection.Enable = false;
	Weathers[n].Sun.Reflection.Texture = "weather\sun\reflection\refl_day.tga";
	Weathers[n].Sun.Reflection.Technique = "sunreflection";
	Weathers[n].Sun.Reflection.Dist = 3500.0;
	Weathers[n].Sun.Reflection.Size = 500.0;
	Weathers[n].Sun.Reflection.Color = argb(200, 200, 200, 200);

	Weathers[n].Sea2.BumpScale = 0.05; //МЕЛКИЕ ВОЛНЫ
	Weathers[n].Sea2.PosShift = 2.0; //остроконечность волн
	
	Weathers[n].Sea2.WaterColor = argb(0, 60, 70, 80);
	Weathers[n].Sea2.SkyColor = argb(0, 155, 155, 155);

	Weathers[n].Sea2.Reflection = 0.5;
	Weathers[n].Sea2.Transparency = 0.05;
	Weathers[n].Sea2.Frenel = 0.6; 
	Weathers[n].Sea2.Attenuation = 0.1;

	Weathers[n].Sea2.Amp1 = 50.0; //амплитуда 1 волны
	Weathers[n].Sea2.AnimSpeed1 = 2.0; //скорость анимации
	Weathers[n].Sea2.Scale1 = 1.0; //размеры волны
	Weathers[n].Sea2.MoveSpeed1 = "0.3, 0.0, 5.0"; //скорость движения

	Weathers[n].Sea2.Amp2 = 3.0; //амплитуда 2 волны
	Weathers[n].Sea2.AnimSpeed2 = 2.0; //скорость анимации
	Weathers[n].Sea2.Scale2 = 1.0; //размеры волны
	Weathers[n].Sea2.MoveSpeed2 = "1.0, 0.0, 5.0"; //скорость движения

	Weathers[n].Sea2.FoamK = 0.05;//яркость пены по высоте 0.1 - пена яркая с 10 метров 1.0 через метр после начала
	Weathers[n].Sea2.FoamV = 9.0;//высота с которой начинается пена
	Weathers[n].Sea2.FoamUV = 0.1; //тайлинг пены
	Weathers[n].Sea2.FoamTexDisturb = 0.7;//сдвиг по нормали. х.з. что это надо поюзать. значение от 0.0 до 1.0

	Weathers[n].Wind.Angle = 2.0;
	Weathers[n].Wind.Speed.Min = 15.0;
	Weathers[n].Wind.Speed.Max = 18.0;
	
	n++;

// ====================================================================
// Overcast2 - дневной шторм

	Weathers[n].id = "Storm02_add";
	Weathers[n].Hour.Min = 10.00001;
	Weathers[n].Hour.Max = 18;
	Weathers[n].Lighting = "storm2";
	Weathers[n].LightingLm = "storm";
	Weathers[n].InsideBack = "s";
	Weathers[n].Sounds.Sea.Postfix = "_storm";

	Weathers[n].Storm = true;
	Weathers[n].skip = true;  //добавленная
	Weathers[n].Tornado = true;
	Weathers[n].Shark = false;
	Weathers[n].Lights = 0;
	Weathers[n].Night = false;

	Weathers[n].Shadow.Density.Head = argb(255,96,96,96);
	Weathers[n].Shadow.Density.Foot = argb(255,96,96,64);

	Weathers[n].Lightning.Enable = true;
	Weathers[n].Lightning.Texture = "Weather\lightning\lightning_storm.tga";
	Weathers[n].Lightning.FlickerTime = 32;
	Weathers[n].Lightning.SubTexX = 4;
	Weathers[n].Lightning.SubTexY = 1;
	Weathers[n].Lightning.ScaleX = 0.7;
	Weathers[n].Lightning.ScaleY = 1.0;
	Weathers[n].Lightning.Flash.Texture = "Weather\lightning\flash.tga";

	Weathers[n].Sky.Dir = "weather\skies\Storm02\";
	Weathers[n].Sky.Color = argb(0,255,255,255);
	Weathers[n].Sky.Rotate.Min = 0.0;
	Weathers[n].Sky.Rotate.Max = 0.0;
	Weathers[n].Sky.Size = 512.0;
	Weathers[n].Sky.Angle = 0.0;

	Weathers[n].Planets.enable = false;

	Weathers[n].Stars.Enable = false;
	Weathers[n].Stars.Texture = "weather\astronomy\stars.tga";
	Weathers[n].Stars.Color = argb(0, 255, 255, 255);
	Weathers[n].Stars.Radius = 2000.0;
	Weathers[n].Stars.Size = 15.0;
	Weathers[n].Stars.HeightFade = 200.0;
	Weathers[n].Stars.SunFade = 1.0;
	Weathers[n].Stars.VisualMagnitude = 8.0;
	
	Weathers[n].Fog.Enable = true;
	Weathers[n].Fog.Height = 2000;
	Weathers[n].Fog.Start = 1;
	Weathers[n].Fog.Density = 0.02;
	Weathers[n].Fog.IslandDensity = 0.005;
	Weathers[n].Fog.SeaDensity = 0.001;
	Weathers[n].Fog.Color = argb(0,100,95,85);
	//athers[n].Fog.Color = argb(0,100,95,85);
	
	Weathers[n].Rain.NumDrops = 50000;
	Weathers[n].Rain.Color = argb(0,13,13,13);
	Weathers[n].Rain.DropLength = 4.12;
	Weathers[n].Rain.Height = 30.0;
	Weathers[n].Rain.Radius = 30.0;
	Weathers[n].Rain.Speed = 28.0;
	Weathers[n].Rain.Jitter = 0.4;
	Weathers[n].Rain.WindSpeedJitter = 0.5;
	Weathers[n].Rain.MaxBlend = 49;
	Weathers[n].Rain.TimeBlend = 2000;

	Weathers[n].Rain.DropsNearNum = 500;
	Weathers[n].Rain.DropsFarNum = 500;
	Weathers[n].Rain.DropsNearRadius = 25.0;
	Weathers[n].Rain.DropsFarRadius = 75.0;
	Weathers[n].Rain.DropsLifeTime = 0.25;
	Weathers[n].Rain.DropsSize = 0.05;
	Weathers[n].Rain.DropsTexture = "weather\rain_drops.tga";
	Weathers[n].Rain.DropsColor = argb(63, 255, 255, 255);

	Weathers[n].Rainbow.Enable = false;
	Weathers[n].Rainbow.Texture = "weather\rainbow\rainbow.tga";

	Weathers[n].Sun.Color = argb(0,60,60,50);
	Weathers[n].Sun.Ambient = argb(0,25,25,25);
	Weathers[n].Sun.AzimuthAngle = Degree2Radian(90.0);
	Weathers[n].Sun.HeightAngle = 0.95;
	Weathers[n].Sun.Glow.Enable = false;
	Weathers[n].Sun.Glow.Dist = 3500.0;
	Weathers[n].Sun.Glow.Size = 1250.0;
	Weathers[n].Sun.Glow.RotateSpeed = 1.0;
	Weathers[n].Sun.Glow.Texture = "weather\sun\glow\sunglow.tga";
	Weathers[n].Sun.Glow.DecayTime = 8.0;
	Weathers[n].Sun.Glow.TechniqueNoZ = "sunglow_noz";
	Weathers[n].Sun.Glow.TechniqueZ = "sunglow_z";
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

	Weathers[n].Sea.Transparency = 0.1;
	Weathers[n].Sea.FrenelCoefficient = 0.5;
	Weathers[n].Sea.WaterReflection = 0.9;
	Weathers[n].Sea.WaterAttenuation = 0.2;
	Weathers[n].Sea.Sky.Color = argb(0,40,50,60);
	Weathers[n].Sea.Water.Color = argb(0,60,70,70);
	Weathers[n].Sea.Pena.Color = argb(192,175,175,155);
	Weathers[n].Sea.Bump.Dir = "resource\textures\weather\sea\bump_g3\";
	Weathers[n].Sea.Bump.Tile = 0.04;
	Weathers[n].Sea.Bump.Ang = 0.0;
	Weathers[n].Sea.Bump.Speed = 0.01;
	Weathers[n].Sea.Bump.AnimSpeed = 15.0;
	Weathers[n].Sea.Bump.Scale = 2.0;
	Weathers[n].Sea.SunRoad.Start = 0.99;
	Weathers[n].Sea.SunRoad.Color1 = argb(0,128,55,0);
	Weathers[n].Sea.SunRoad.Color2 = argb(0,255,200,150);
	Weathers[n].Sea.SunRoad.Power = 0.2;
	//угол, частота, высота волны в метрах, начальная позиция волны, скорость волны
	Weathers[n].Sea.Harmonics.h1 = "0.0, 0.5, 7.0, 0.0, 100.0";
	Weathers[n].Sea.Harmonics.h2 = "90.0, 3.0, 4.0, 45.0, 220.0";
	Weathers[n].Sea.Harmonics.h3 = "35.0, 6.0, 1.5, 0.0, 260.0";
	Weathers[n].Sea.Harmonics.h4 = "160.0, 14.0, 0.8, 360.0, 320.0";
	Weathers[n].Sea.Harmonics.h5 = "20.0, 18.0, 0.6, 240.0, 260.0";
	Weathers[n].Sea.Harmonics.h6 = "0.0, 50.0, 0.2, 0.0, 460.0";

	Weathers[n].Sun.Reflection.Enable = false;
	Weathers[n].Sun.Reflection.Texture = "weather\sun\reflection\refl_day.tga";
	Weathers[n].Sun.Reflection.Technique = "sunreflection";
	Weathers[n].Sun.Reflection.Dist = 3500.0;
	Weathers[n].Sun.Reflection.Size = 500.0;
	Weathers[n].Sun.Reflection.Color = argb(200, 200, 200, 200);

	Weathers[n].Sea2.BumpScale = 0.05; //МЕЛКИЕ ВОЛНЫ
	Weathers[n].Sea2.PosShift = 0.3; //остроконечность волн
	
	Weathers[n].Sea2.WaterColor = argb(0, 50, 65, 70);
	Weathers[n].Sea2.SkyColor = argb(0, 255, 255, 255);

	Weathers[n].Sea2.Reflection = 0.8;
	Weathers[n].Sea2.Transparency = 0.05;
	Weathers[n].Sea2.Frenel = 0.3; 
	Weathers[n].Sea2.Attenuation = 0.3;

	Weathers[n].Sea2.Amp1 = 50.0; //амплитуда 1 волны
	Weathers[n].Sea2.AnimSpeed1 = 2.0; //скорость анимации
	Weathers[n].Sea2.Scale1 = 1.0; //размеры волны
	Weathers[n].Sea2.MoveSpeed1 = "0.3, 0.0, 5.0"; //скорость движения

	Weathers[n].Sea2.Amp2 = 3.0; //амплитуда 2 волны
	Weathers[n].Sea2.AnimSpeed2 = 2.0; //скорость анимации
	Weathers[n].Sea2.Scale2 = 1.0; //размеры волны
	Weathers[n].Sea2.MoveSpeed2 = "1.0, 0.0, 5.0"; //скорость движения

	Weathers[n].Sea2.FoamK = 0.02;//яркость пены по высоте 0.1 - пена яркая с 10 метров 1.0 через метр после начала
	Weathers[n].Sea2.FoamV = 30.0;//высота с которой начинается пена
	Weathers[n].Sea2.FoamUV = 0.5; //тайлинг пены
	Weathers[n].Sea2.FoamTexDisturb = 0.7;//сдвиг по нормали. х.з. что это надо поюзать. значение от 0.0 до 1.0

	Weathers[n].Wind.Angle = 2.0;
	Weathers[n].Wind.Speed.Min = 12.0;
	Weathers[n].Wind.Speed.Max = 14.0;
	
	n++;

// ====================================================================
// Вечерний шторм

	Weathers[n].id = "Storm03_add";
	Weathers[n].Hour.Min = 18.00001;
	Weathers[n].Hour.Max = 22;
	Weathers[n].Lighting = "storm3";
	Weathers[n].LightingLm = "storm";
	Weathers[n].InsideBack = "s";
	Weathers[n].Sounds.Sea.Postfix = "_storm";

	Weathers[n].Storm = true;
	Weathers[n].skip = true;  //добавленная
	Weathers[n].Tornado = true;
	Weathers[n].Shark = false;
	Weathers[n].Lights = 1;
	Weathers[n].Night = true;

	Weathers[n].Shadow.Density.Head = argb(255,96,96,96);
	Weathers[n].Shadow.Density.Foot = argb(255,96,96,64);

	Weathers[n].Lightning.Enable = true;
	Weathers[n].Lightning.Texture = "Weather\lightning\lightning_storm.tga";
	Weathers[n].Lightning.FlickerTime = 32;
	Weathers[n].Lightning.SubTexX = 4;
	Weathers[n].Lightning.SubTexY = 1;
	Weathers[n].Lightning.ScaleX = 0.7;
	Weathers[n].Lightning.ScaleY = 1.0;
	Weathers[n].Lightning.Flash.Texture = "Weather\lightning\flash.tga";

	Weathers[n].Sky.Dir = "weather\skies\Storm03\";
	Weathers[n].Sky.Color = argb(0,255,255,255);
	Weathers[n].Sky.Rotate.Min = 0.0;
	Weathers[n].Sky.Rotate.Max = 0.0;
	Weathers[n].Sky.Size = 512.0;
	Weathers[n].Sky.Angle = 0.0;

	Weathers[n].Planets.enable = false;

	Weathers[n].Stars.Enable = false;
	Weathers[n].Stars.Texture = "weather\astronomy\stars.tga";
	Weathers[n].Stars.Color = argb(0, 255, 255, 255);
	Weathers[n].Stars.Radius = 2000.0;
	Weathers[n].Stars.Size = 15.0;
	Weathers[n].Stars.HeightFade = 200.0;
	Weathers[n].Stars.SunFade = 1.0;
	Weathers[n].Stars.VisualMagnitude = 8.0;
	
	Weathers[n].Fog.Enable = true;
	Weathers[n].Fog.Height = 2000;
	Weathers[n].Fog.Start = 1;
	Weathers[n].Fog.Density = 0.02;
	Weathers[n].Fog.IslandDensity = 0.005;
	Weathers[n].Fog.SeaDensity = 0.001;
	Weathers[n].Fog.Color = argb(0,50,50,50);
	
	Weathers[n].Rain.NumDrops = 50000;
	Weathers[n].Rain.Color = argb(0,33,33,33);
	Weathers[n].Rain.DropLength = 2.12;
	Weathers[n].Rain.Height = 30.0;
	Weathers[n].Rain.Radius = 30.0;
	Weathers[n].Rain.Speed = 18.0;
	Weathers[n].Rain.Jitter = 0.4;
	Weathers[n].Rain.WindSpeedJitter = 0.5;
	Weathers[n].Rain.MaxBlend = 49;
	Weathers[n].Rain.TimeBlend = 2000;

	Weathers[n].Rain.DropsNearNum = 500;
	Weathers[n].Rain.DropsFarNum = 500;
	Weathers[n].Rain.DropsNearRadius = 25.0;
	Weathers[n].Rain.DropsFarRadius = 75.0;
	Weathers[n].Rain.DropsLifeTime = 0.25;
	Weathers[n].Rain.DropsSize = 0.05;
	Weathers[n].Rain.DropsTexture = "weather\rain_drops.tga";
	Weathers[n].Rain.DropsColor = argb(63, 255, 255, 255);

	Weathers[n].Rainbow.Enable = false;
	Weathers[n].Rainbow.Texture = "weather\rainbow\rainbow.tga";

	Weathers[n].Sun.Color = argb(0,85,75,70);
	Weathers[n].Sun.Ambient = argb(0,40,40,40);
	Weathers[n].Sun.AzimuthAngle = Degree2Radian(90.0);
	Weathers[n].Sun.HeightAngle = 0.95;
	Weathers[n].Sun.Glow.Enable = false;
	Weathers[n].Sun.Glow.Dist = 3500.0;
	Weathers[n].Sun.Glow.Size = 1250.0;
	Weathers[n].Sun.Glow.RotateSpeed = 1.0;
	Weathers[n].Sun.Glow.Texture = "weather\sun\glow\sunglow.tga";
	Weathers[n].Sun.Glow.DecayTime = 8.0;
	Weathers[n].Sun.Glow.TechniqueNoZ = "sunglow_noz";
	Weathers[n].Sun.Glow.TechniqueZ = "sunglow_z";
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
	Weathers[n].Sea.WaterReflection = 0.8;
	Weathers[n].Sea.WaterAttenuation = 0.2;
	Weathers[n].Sea.Sky.Color = argb(0,180,180,180);
	Weathers[n].Sea.Water.Color = argb(0,60,95,95);
	Weathers[n].Sea.Pena.Color = argb(0,175,175,155);
	Weathers[n].Sea.Bump.Dir = "resource\textures\weather\sea\bump_g3\";
	Weathers[n].Sea.Bump.Tile = 0.04;
	Weathers[n].Sea.Bump.Ang = 0.0;
	Weathers[n].Sea.Bump.Speed = 0.01;
	Weathers[n].Sea.Bump.AnimSpeed = 15.0;
	Weathers[n].Sea.Bump.Scale = 2.0;
	Weathers[n].Sea.SunRoad.Start = 0.99;
	Weathers[n].Sea.SunRoad.Color1 = argb(0,128,55,0);
	Weathers[n].Sea.SunRoad.Color2 = argb(0,255,200,150);
	Weathers[n].Sea.SunRoad.Power = 0.2;
	//угол, частота, высота волны в метрах, начальная позиция волны, скорость волны
	Weathers[n].Sea.Harmonics.h1 = "0.0, 0.5, 5.0, 0.0, 20.0";
	Weathers[n].Sea.Harmonics.h2 = "45.0, 1.5, 2.0, 0.0, 35.0";
	Weathers[n].Sea.Harmonics.h3 = "90.0, 2.0, 3.5, 0.0, 50.0";
	Weathers[n].Sea.Harmonics.h4 = "135.0, 10.0, 0.4, 0.0, 80.0";
	Weathers[n].Sea.Harmonics.h5 = "270.0, 8.0, 0.45, 0.0, 40.0";


	Weathers[n].Sun.Reflection.Enable = false;
	Weathers[n].Sun.Reflection.Texture = "weather\sun\reflection\refl_evening.tga";
	Weathers[n].Sun.Reflection.Technique = "sunreflection";
	Weathers[n].Sun.Reflection.Dist = 3500.0;
	Weathers[n].Sun.Reflection.Size = 500.0;
	Weathers[n].Sun.Reflection.Color = argb(200, 200, 200, 200);

	Weathers[n].Sea2.BumpScale = 0.05; //МЕЛКИЕ ВОЛНЫ
	Weathers[n].Sea2.PosShift = 0.8; //остроконечность волн
	
	Weathers[n].Sea2.WaterColor = argb(0, 85, 105, 100);
	Weathers[n].Sea2.SkyColor = argb(0, 155, 155, 155);

	Weathers[n].Sea2.Reflection = 0.2;
	Weathers[n].Sea2.Transparency = 0.05;
	Weathers[n].Sea2.Frenel = 0.6; 
	Weathers[n].Sea2.Attenuation = 0.2;

	Weathers[n].Sea2.Amp1 = 50.0; //амплитуда 1 волны
	Weathers[n].Sea2.AnimSpeed1 = 2.0; //скорость анимации
	Weathers[n].Sea2.Scale1 = 1.0; //размеры волны
	Weathers[n].Sea2.MoveSpeed1 = "0.3, 0.0, 5.0"; //скорость движения

	Weathers[n].Sea2.Amp2 = 3.0; //амплитуда 2 волны
	Weathers[n].Sea2.AnimSpeed2 = 2.0; //скорость анимации
	Weathers[n].Sea2.Scale2 = 1.0; //размеры волны
	Weathers[n].Sea2.MoveSpeed2 = "1.0, 0.0, 5.0"; //скорость движения

	Weathers[n].Sea2.FoamK = 0.02;//яркость пены по высоте 0.1 - пена яркая с 10 метров 1.0 через метр после начала
	Weathers[n].Sea2.FoamV = 12.0;//высота с которой начинается пена
	Weathers[n].Sea2.FoamUV = 0.3; //тайлинг пены
	Weathers[n].Sea2.FoamTexDisturb = 0.7;//сдвиг по нормали. х.з. что это надо поюзать. значение от 0.0 до 1.0
	

	Weathers[n].Wind.Angle = 2.0;
	Weathers[n].Wind.Speed.Min = 13.0;
	Weathers[n].Wind.Speed.Max = 16.0;
	
	n++;

// ====================================================================
// Ночной шторм

	Weathers[n].id = "Storm04_add";
	Weathers[n].Hour.Min = 22.00001;
	Weathers[n].Hour.Max = 6;
	Weathers[n].Lighting = "storm4";
	Weathers[n].LightingLm = "storm";
	Weathers[n].InsideBack = "s";
	Weathers[n].Sounds.Sea.Postfix = "_storm";

	Weathers[n].Storm = true;
	Weathers[n].skip = true;  //добавленная
	Weathers[n].Tornado = true;
	Weathers[n].Shark = false;
	Weathers[n].Lights = 1;
	Weathers[n].Night = true;

	Weathers[n].Shadow.Density.Head = argb(255,96,96,96);
	Weathers[n].Shadow.Density.Foot = argb(255,96,96,64);

	Weathers[n].Lightning.Enable = true;
	Weathers[n].Lightning.Texture = "Weather\lightning\lightning_storm.tga";
	Weathers[n].Lightning.FlickerTime = 32;
	Weathers[n].Lightning.SubTexX = 4;
	Weathers[n].Lightning.SubTexY = 1;
	Weathers[n].Lightning.ScaleX = 0.7;
	Weathers[n].Lightning.ScaleY = 1.0;
	Weathers[n].Lightning.Flash.Texture = "Weather\lightning\flash.tga";

	Weathers[n].Sky.Dir = "weather\skies\Storm04\";
	Weathers[n].Sky.Color = argb(0,255,255,255);
	Weathers[n].Sky.Rotate.Min = 0.0;
	Weathers[n].Sky.Rotate.Max = 0.0;
	Weathers[n].Sky.Size = 512.0;
	Weathers[n].Sky.Angle = 0.0;

	Weathers[n].Planets.enable = false;

	Weathers[n].Stars.Enable = false;
	Weathers[n].Stars.Texture = "weather\astronomy\stars.tga";
	Weathers[n].Stars.Color = argb(0, 255, 255, 255);
	Weathers[n].Stars.Radius = 2000.0;
	Weathers[n].Stars.Size = 15.0;
	Weathers[n].Stars.HeightFade = 200.0;
	Weathers[n].Stars.SunFade = 1.0;
	Weathers[n].Stars.VisualMagnitude = 8.0;
	
	Weathers[n].Fog.Enable = true;
	Weathers[n].Fog.Height = 2000;
	Weathers[n].Fog.Start = 1;
	Weathers[n].Fog.Density = 0.01;
	Weathers[n].Fog.IslandDensity = 0.0025;
	Weathers[n].Fog.SeaDensity = 0.001;
	Weathers[n].Fog.Color = argb(0,40,40,30);
	
	Weathers[n].Rain.NumDrops = 25000;
	Weathers[n].Rain.Color = argb(0,43,43,33);
	Weathers[n].Rain.DropLength = 3.12;
	Weathers[n].Rain.Height = 30.0;
	Weathers[n].Rain.Radius = 30.0;
	Weathers[n].Rain.Speed = 18.0;
	Weathers[n].Rain.Jitter = 0.4;
	Weathers[n].Rain.WindSpeedJitter = 0.5;
	Weathers[n].Rain.MaxBlend = 49;
	Weathers[n].Rain.TimeBlend = 2000;

	Weathers[n].Rain.DropsNearNum = 500;
	Weathers[n].Rain.DropsFarNum = 500;
	Weathers[n].Rain.DropsNearRadius = 25.0;
	Weathers[n].Rain.DropsFarRadius = 75.0;
	Weathers[n].Rain.DropsLifeTime = 0.25;
	Weathers[n].Rain.DropsSize = 0.05;
	Weathers[n].Rain.DropsTexture = "weather\rain_drops.tga";
	Weathers[n].Rain.DropsColor = argb(63, 255, 255, 255);

	Weathers[n].Rainbow.Enable = false;
	Weathers[n].Rainbow.Texture = "weather\rainbow\rainbow.tga";

	Weathers[n].Sun.Color = argb(0,30,30,15);
	Weathers[n].Sun.Ambient = argb(0,30,30,25);
	Weathers[n].Sun.AzimuthAngle = Degree2Radian(90.0);
	Weathers[n].Sun.HeightAngle = 0.95;
	Weathers[n].Sun.Glow.Enable = false;
	Weathers[n].Sun.Glow.Dist = 3500.0;
	Weathers[n].Sun.Glow.Size = 1250.0;
	Weathers[n].Sun.Glow.RotateSpeed = 1.0;
	Weathers[n].Sun.Glow.Texture = "weather\sun\glow\sunglow.tga";
	Weathers[n].Sun.Glow.DecayTime = 8.0;
	Weathers[n].Sun.Glow.TechniqueNoZ = "sunglow_noz";
	Weathers[n].Sun.Glow.TechniqueZ = "sunglow_z";
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
	Weathers[n].Sea.WaterReflection = 0.8;
	Weathers[n].Sea.WaterAttenuation = 0.2;
	Weathers[n].Sea.Sky.Color = argb(0,180,180,180);
	Weathers[n].Sea.Water.Color = argb(0,60,95,95);
	Weathers[n].Sea.Pena.Color = argb(0,175,175,155);
	Weathers[n].Sea.Bump.Dir = "resource\textures\weather\sea\bump_g3\";
	Weathers[n].Sea.Bump.Tile = 0.04;
	Weathers[n].Sea.Bump.Ang = 0.0;
	Weathers[n].Sea.Bump.Speed = 0.01;
	Weathers[n].Sea.Bump.AnimSpeed = 15.0;
	Weathers[n].Sea.Bump.Scale = 2.0;
	Weathers[n].Sea.SunRoad.Start = 0.99;
	Weathers[n].Sea.SunRoad.Color1 = argb(0,128,55,0);
	Weathers[n].Sea.SunRoad.Color2 = argb(0,255,200,150);
	Weathers[n].Sea.SunRoad.Power = 0.2;
	//угол, частота, высота волны в метрах, начальная позиция волны, скорость волны
	Weathers[n].Sea.Harmonics.h1 = "0.0, 0.5, 5.0, 0.0, 20.0";
	Weathers[n].Sea.Harmonics.h2 = "45.0, 1.5, 2.0, 0.0, 35.0";
	Weathers[n].Sea.Harmonics.h3 = "90.0, 2.0, 3.5, 0.0, 50.0";
	Weathers[n].Sea.Harmonics.h4 = "135.0, 10.0, 0.4, 0.0, 80.0";
	Weathers[n].Sea.Harmonics.h5 = "270.0, 8.0, 0.45, 0.0, 40.0";


	Weathers[n].Sun.Reflection.Enable = false;
	Weathers[n].Sun.Reflection.Texture = "weather\sun\reflection\refl_evening.tga";
	Weathers[n].Sun.Reflection.Technique = "sunreflection";
	Weathers[n].Sun.Reflection.Dist = 3500.0;
	Weathers[n].Sun.Reflection.Size = 500.0;
	Weathers[n].Sun.Reflection.Color = argb(200, 200, 200, 200);

	Weathers[n].Sea2.BumpScale = 0.05; //МЕЛКИЕ ВОЛНЫ
	Weathers[n].Sea2.PosShift = 2.0; //остроконечность волн
	
	Weathers[n].Sea2.WaterColor = argb(0, 60, 65, 60);
	Weathers[n].Sea2.SkyColor = argb(0, 255, 255, 255);

	Weathers[n].Sea2.Reflection = 0.7;
	Weathers[n].Sea2.Transparency = 0.05;
	Weathers[n].Sea2.Frenel = 0.6; 
	Weathers[n].Sea2.Attenuation = 0.2;

	Weathers[n].Sea2.Amp1 = 50.0; //амплитуда 1 волны
	Weathers[n].Sea2.AnimSpeed1 = 2.0; //скорость анимации
	Weathers[n].Sea2.Scale1 = 1.0; //размеры волны
	Weathers[n].Sea2.MoveSpeed1 = "0.3, 0.0, 5.0"; //скорость движения

	Weathers[n].Sea2.Amp2 = 3.0; //амплитуда 2 волны
	Weathers[n].Sea2.AnimSpeed2 = 2.0; //скорость анимации
	Weathers[n].Sea2.Scale2 = 1.0; //размеры волны
	Weathers[n].Sea2.MoveSpeed2 = "1.0, 0.0, 5.0"; //скорость движения

	Weathers[n].Sea2.FoamK = 0.01;//яркость пены по высоте 0.1 - пена яркая с 10 метров 1.0 через метр после начала
	Weathers[n].Sea2.FoamV = 12.0;//высота с которой начинается пена
	Weathers[n].Sea2.FoamUV = 0.2; //тайлинг пены
	Weathers[n].Sea2.FoamTexDisturb = 0.7;//сдвиг по нормали. х.з. что это надо поюзать. значение от 0.0 до 1.0
	

	Weathers[n].Wind.Angle = 2.0;
	Weathers[n].Wind.Speed.Min = 12.0;
	Weathers[n].Wind.Speed.Max = 17.0;
	
	n++;

	return n;
}