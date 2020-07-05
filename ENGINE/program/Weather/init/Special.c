
int Whr_InitSpecial(int n)
{
// ====================================================================
// Inside

	Weathers[n].id = "Inside";
	Weathers[n].Hour.Min = 0;
	Weathers[n].Hour.Max = 24;
	Weathers[n].Lighting = "inside";
	Weathers[n].LightingLm = "inside";
	Weathers[n].InsideBack = "";
	Weathers[n].Skip = true;

	Weathers[n].Storm = false;
	Weathers[n].Tornado = false;
	Weathers[n].Shark = true;
	Weathers[n].Lights = false;
	Weathers[n].Night = false;

	Weathers[n].Shadow.Density.Head = argb(255,96,96,96);
	Weathers[n].Shadow.Density.Foot = argb(255,96,96,64);

	Weathers[n].Lightning.Enable = false;
	Weathers[n].Lightning.Texture = "Weather\lightning\lightning.tga";
	Weathers[n].Lightning.FlickerTime = 32;
	Weathers[n].Lightning.SubTexX = 1;
	Weathers[n].Lightning.SubTexY = 1;

	Weathers[n].Sky.Dir = "weather\skies\Twilight1\";
	Weathers[n].Sky.Color = argb(0,64,64,64);
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

	Weathers[n].Fog.Enable = false;
	Weathers[n].Fog.Height = 100.0;
	Weathers[n].Fog.Start = 0.0;
	Weathers[n].Fog.Density = 0.001;
	Weathers[n].Fog.SeaDensity = 0.001;
	Weathers[n].Fog.IslandDensity = 0.001;
	Weathers[n].Fog.Color = argb(0,200,220,255);
	
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

	Weathers[n].Rainbow.Enable = false;
	Weathers[n].Rainbow.Texture = "weather\rainbow\rainbow.tga";

	Weathers[n].Sun.Color = argb(0,80,80,68);
	Weathers[n].Sun.Ambient = argb(0,40,30,20);
	Weathers[n].Sun.AzimuthAngle = Degree2Radian(3.0);
	Weathers[n].Sun.HeightAngle = 0.36;
	Weathers[n].Sun.Overflow.Enable = false;
	Weathers[n].Sun.Glow.Enable = false;
	Weathers[n].Sun.Glow.Dist = 3500.0;
	Weathers[n].Sun.Glow.Size = 1250.0;
	Weathers[n].Sun.Glow.RotateSpeed = 1.0;
	Weathers[n].Sun.Glow.Texture = "weather\sun\glow\sunglow.tga";
	Weathers[n].Sun.Glow.DecayTime = 8.0;
	Weathers[n].Sun.Glow.TechniqueNoZ = "sunglow_noz";
	Weathers[n].Sun.Glow.TechniqueZ = "sunglow_z";
	Weathers[n].Sun.Glow.Color = argb(0,255,255,255);
	Weathers[n].Sun.Flare.Enable = true;
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

	Weathers[n].Sea.GF2MX.Sky.Color = argb(0,255,255,255);
	Weathers[n].Sea.GF2MX.Water.Color = argb(0,30,55,100);
	Weathers[n].Sea.GF2MX.Bump.Tile = 0.06;
	Weathers[n].Sea.GF2MX.Bump.AnimSpeed = 23.0;
	Weathers[n].Sea.GF2MX.Bump.Dir = "weather\sea\bump_g2mx\";

	Weathers[n].Sea.Transparency = 0.4;
	Weathers[n].Sea.FrenelCoefficient = 0.7;
	Weathers[n].Sea.WaterReflection = 0.7;
	Weathers[n].Sea.WaterAttenuation = 0.8;
	Weathers[n].Sea.Sky.Color = argb(0,255,255,255);
	Weathers[n].Sea.Bump.Dir = "resource\textures\weather\sea\bump_g3\";
	Weathers[n].Sea.Water.Color = argb(0,30,55,100);
	Weathers[n].Sea.Pena.Color = argb(0,175,175,155);
	Weathers[n].Sea.Bump.Tile = 0.05;
	Weathers[n].Sea.Bump.Ang = 0.0;
	Weathers[n].Sea.Bump.Speed = 0.01;
	Weathers[n].Sea.Bump.AnimSpeed = 15.0;
	Weathers[n].Sea.Bump.Scale = 2.0;
	Weathers[n].Sea.SunRoad.Start = 0.99	;
	Weathers[n].Sea.SunRoad.Color1 = argb(0,128,55,0);
	Weathers[n].Sea.SunRoad.Color2 = argb(0,255,200,150);
	Weathers[n].Sea.SunRoad.Power = 0.2;
	Weathers[n].Sea.Harmonics.h1 = "0.0,10.0,0.1802,80.87,-78.00";
	Weathers[n].Sea.Harmonics.h2 = "1.87,12.0,0.202,82.28,78.00";
	Weathers[n].Sea.Harmonics.h3 = "0.27,7.0,0.2002,82.28,78.00";

	Weathers[n].Sun.Reflection.Enable = true;
	Weathers[n].Sun.Reflection.Texture = "weather\sun\reflection\refl_day.tga";
	Weathers[n].Sun.Reflection.Technique = "sunreflection";
	Weathers[n].Sun.Reflection.Dist = 3500.0;
	Weathers[n].Sun.Reflection.Size = 500.0;
	Weathers[n].Sun.Reflection.Color = argb(200, 200, 200, 200);

	Weathers[n].Sea2.BumpScale = 0.05; //МЕЛКИЕ ВОЛНЫ
	Weathers[n].Sea2.PosShift = 2.0; //остроконечность волн
	
	Weathers[n].Sea2.WaterColor = argb(0, 10, 55, 100);
	Weathers[n].Sea2.SkyColor = argb(0, 255, 255, 255);

	Weathers[n].Sea2.Reflection = 0.7;
	Weathers[n].Sea2.Transparency = 0.9;
	Weathers[n].Sea2.Frenel = 0.3; 
	Weathers[n].Sea2.Attenuation = 0.3;

	Weathers[n].Sea2.Amp1 = 6.0; //амплитуда 1 волны
	Weathers[n].Sea2.AnimSpeed1 = 10.0; //скорость анимации
	Weathers[n].Sea2.Scale1 = 0.5; //размеры волны
	Weathers[n].Sea2.MoveSpeed1 = "5.0, 0.0, 0.0"; //скорость движения

	Weathers[n].Sea2.Amp2 = 0.75;
	Weathers[n].Sea2.AnimSpeed2 = 3.0;
	Weathers[n].Sea2.Scale2 = 2.0;
	Weathers[n].Sea2.MoveSpeed2 = "-5.5, 0.0, 0.0";

	Weathers[n].Wind.Angle = 2.0;
	Weathers[n].Wind.Speed.Min = 5.0;
	Weathers[n].Wind.Speed.Max = 8.0;
	n++;

	// ====================================================================
    //Underwater	
	Weathers[n].id = "Underwater";
	Weathers[n].Hour.Min = 0;
	Weathers[n].Hour.Max = 24;
	Weathers[n].Lighting = "morning10";
	Weathers[n].LightingLm = "morning";
	Weathers[n].InsideBack = "m";
	Weathers[n].Skip = true;

	Weathers[n].Storm = false;
	Weathers[n].Tornado = false;
	Weathers[n].Shark = true;
	Weathers[n].Lights = 0;
	Weathers[n].Night = false;

	Weathers[n].Shadow.Density.Head = argb(255,96,96,96);
	Weathers[n].Shadow.Density.Foot = argb(255,96,96,64);

	Weathers[n].Lightning.Enable = false;
	Weathers[n].Lightning.Texture = "Weather\lightning\lightning.tga";
	Weathers[n].Lightning.FlickerTime = 32;
	Weathers[n].Lightning.SubTexX = 1;
	Weathers[n].Lightning.SubTexY = 1;

	Weathers[n].Sky.Dir = "weather\skies\10\";
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
	Weathers[n].Fog.Density = 0.08;
	Weathers[n].Fog.IslandDensity = 0.002;
	Weathers[n].Fog.SeaDensity = 0.003;//0.001;
	Weathers[n].Fog.Color = argb(0,60,120,160);
	
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

	Weathers[n].Rainbow.Enable = false;
	Weathers[n].Rainbow.Texture = "weather\rainbow\rainbow.tga";

	Weathers[n].Stars.Enable = false;
	Weathers[n].Stars.Texture = "weather\astronomy\stars.tga";
	Weathers[n].Stars.Color = argb(0, 255, 255, 255);
	Weathers[n].Stars.Radius = 2000.0;
	Weathers[n].Stars.Size = 13.0;
	Weathers[n].Stars.HeightFade = 1900.0;
	Weathers[n].Stars.SunFade = 1.0;
	Weathers[n].Stars.VisualMagnitude = 8.0;

	Weathers[n].Planets.Enable = false;

	Weathers[n].Sun.Color = argb(0,160,140,190);
	Weathers[n].Sun.Ambient = argb(0,95,100,110);
	Weathers[n].Sun.AzimuthAngle = Degree2Radian(160.0);
	Weathers[n].Sun.HeightAngle = 0.45;
	//Weathers[n].Sun.AzimuthAngle = Degree2Radian(0.0);
	//Weathers[n].Sun.HeightAngle = 0.35;
	Weathers[n].Sun.Glow.Enable = false;
	Weathers[n].Sun.Glow.Dist = 3500.0;
	Weathers[n].Sun.Glow.Size = 1050.0;
	Weathers[n].Sun.Glow.RotateSpeed = 1.0;
	Weathers[n].Sun.Glow.Texture = "weather\sun\glow\sunglow.tga";
	Weathers[n].Sun.Glow.DecayTime = 8.0;
	Weathers[n].Sun.Glow.TechniqueNoZ = "sunglow_noz";
	Weathers[n].Sun.Glow.TechniqueZ = "sunglow_z";
	Weathers[n].Sun.Glow.Color = argb(0,255,255,255);
	Weathers[n].Sun.Overflow.Enable = true;
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

	Weathers[n].Sea.Transparency = 0.2;
	Weathers[n].Sea.FrenelCoefficient = 0.2;
	Weathers[n].Sea.WaterReflection = 0.9;
	Weathers[n].Sea.WaterAttenuation = 0.1;
	Weathers[n].Sea.Sky.Color = argb(0,250,160,140);
	Weathers[n].Sea.Water.Color = argb(0,50,95,75);
	Weathers[n].Sea.Pena.Color = argb(0,175,175,155);
	Weathers[n].Sea.Bump.Dir = "resource\textures\weather\sea\bump_g3\";
	Weathers[n].Sea.Bump.Tile = 0.04;
	Weathers[n].Sea.Bump.Ang = 0.0;
	Weathers[n].Sea.Bump.Speed = 0.01;
	Weathers[n].Sea.Bump.AnimSpeed = 15.0;
	Weathers[n].Sea.Bump.Scale = 2.0;
	Weathers[n].Sea.SunRoad.Start = 0.99;
	Weathers[n].Sea.SunRoad.Color1 = argb(0,164,55,0);
	Weathers[n].Sea.SunRoad.Color2 = argb(0,255,200,50);
	Weathers[n].Sea.SunRoad.Power = 0.2;
	//угол, частота, высота волны в метрах, начальная позиция волны, скорость волны
	Weathers[n].Sea.Harmonics.h1 = "0.0, 5.0, 1.0, 0.0, 250.0";
	Weathers[n].Sea.Harmonics.h2 = "270.0, 1.0, 5.0, 0.0, 50.0";
	Weathers[n].Sea.Harmonics.h3 = "45.0, 2.0, 3.0, 0.0, 150.0";
	Weathers[n].Sea.Harmonics.h4 = "90.0, 4.0, 2.5, 0.0, 10.0";

	Weathers[n].Sun.Reflection.Enable = true;
	Weathers[n].Sun.Reflection.Texture = "weather\sun\reflection\refl_evening.tga";
	Weathers[n].Sun.Reflection.Technique = "sunreflection";
	Weathers[n].Sun.Reflection.Dist = 3500.0;
	Weathers[n].Sun.Reflection.Size = 500.0;
	Weathers[n].Sun.Reflection.Color = argb(200, 200, 200, 200);

	Weathers[n].Sea2.BumpScale = 0.05; //МЕЛКИЕ ВОЛНЫ
	Weathers[n].Sea2.PosShift = 1.9; //остроконечность волн
	
	Weathers[n].Sea2.WaterColor = argb(0, 65, 100, 110);
	Weathers[n].Sea2.SkyColor = argb(0, 255, 255, 255);

	Weathers[n].Sea2.Reflection = 0.9;
	Weathers[n].Sea2.Transparency = 0.3;
	Weathers[n].Sea2.Frenel = 10.0;
	Weathers[n].Sea2.Attenuation = 0.3;

	Weathers[n].Sea2.Amp1 = 6.0; //амплитуда 1 волны
	Weathers[n].Sea2.AnimSpeed1 = 10.0; //скорость анимации
	Weathers[n].Sea2.Scale1 = 0.8; //размеры волны
	Weathers[n].Sea2.MoveSpeed1 = "0.0, 0.0, 3.0"; //скорость движения

	Weathers[n].Sea2.Amp2 = 1.0;
	Weathers[n].Sea2.AnimSpeed2 = 19.0;
	Weathers[n].Sea2.Scale2 = 2.0;
	Weathers[n].Sea2.MoveSpeed2 = "-2.0, 0.0, 0.0";

	Weathers[n].Sea2.FoamK = 0.3;//яркость пены по высоте 0.1 - пена яркая с 10 метров 1.0 через метр после начала
	Weathers[n].Sea2.FoamV = 2.8;//высота с которой начинается пена
	Weathers[n].Sea2.FoamUV = 1.0; //тайлинг пены
	Weathers[n].Sea2.FoamTexDisturb = 0.7;//сдвиг по нормали. х.з. что это надо поюзать. значение от 0.0 до 1.0

	Weathers[n].Wind.Angle = 2.0;
	Weathers[n].Wind.Speed.Min = 7.0;
	Weathers[n].Wind.Speed.Max = 14.5;	
	n++;

	return n;
}