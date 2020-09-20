
void WhrDeleteSeaEnvironment()
{
	DeleteAttribute(&Sea,"");
}
	
void WhrCreateSeaEnvironment()
{
	aref	aCurWeather = GetCurrentWeather();
	aref	aSea; makearef(aSea,aCurWeather.Sea);
	aref	aSea2; makearef(aSea2, aCurWeather.Sea2);
	aref	aCommon; makearef(aCommon, WhrCommonParams.Sea);
	int   i;
	float fMaxSeaHeight;

	if (CheckAttribute(&Sea,"MaxSeaHeight")) { fMaxSeaHeight = stf(Sea.MaxSeaHeight); }
	DeleteAttribute(&Sea,"");

	Sea.Clear = "";

	Sea.Transparency = Whr_GetFloat(aSea,"Transparency");
	Sea.FrenelCoefficient = Whr_GetFloat(aSea,"FrenelCoefficient");
	Sea.WaterReflection = Whr_GetFloat(aSea,"WaterReflection");
	Sea.WaterAttenuation = Whr_GetFloat(aSea,"WaterAttenuation");

	Sea.Sea2.BumpScale = Whr_GetFloat(aSea2, "BumpScale");

	ref mchr = GetMainCharacter();
	string sLocation = mchr.location;

	float FogDensity = 1.0;
	float FogSeaDensity = 1.0

	if (FindLocation(sLocation) != -1)
	{
		/*if(CheckAttribute(&locations[FindLocation(sLocation)], "fastreload"))
		{*/
			Sea.LodScale = 32.0;
			Sea.MaxVertices = 1000;
			Sea.MaxIndices = 1300;
			Sea.MaxWaveDistance = 10.0;
			Sea.MaxDim = 65536;
			Sea.MinDim = 64;
			Sea.GridStep = GridStepPC*5.0;
			if (CheckAttribute(aCurWeather, "Storm") && sti(aCurWeather.Storm) == true)
			{
				fMaxSeaHeight = 2.0;
			}
			else
            {
				fMaxSeaHeight = 0.5;
			}
			FogDensity = 20.0;
			FogSeaDensity = 20.0;

			Sea.Sea2.LodScale = 2.0;
			Sea.Sea2.GridStep = 0.15;
			Sea.Sea2.BumpScale = 0.3;
		/*}
		else
		{
			Sea.LodScale = 8.0;
			Sea.MaxVertices = 2000;
			Sea.MaxIndices = 2600;
			Sea.MaxWaveDistance = 1000.0;
			Sea.MaxDim = 65536;
			Sea.MinDim = 64;
			Sea.GridStep = GridStepPC;
			fMaxSeaHeight = 0.5;// boal бухты затапливает 5.0;
			Sea.Sea2.LodScale = 2.0;
			Sea.Sea2.GridStep = 0.15;
			//SetSeaSettings();
		}*/
	}
	else
	{
        if (CheckAttribute(aCurWeather, "Storm") && sti(aCurWeather.Storm) == true)
		{
		    SetSeaSettings();
			fMaxSeaHeight = 200.0;
			Log_TestInfo("Шторм включен, волны по Мах разрешены");
		}
		else
		{
			i = FindIsland(sLocation);
			if ( i != -1)
			{
				Sea.LodScale = 4.0;
				Sea.MaxVertices = 4000;
				Sea.MaxIndices = 5200;
				Sea.MaxWaveDistance = 1000.0;
				Sea.MaxDim = 65536;
				Sea.MinDim = 64;
				Sea.GridStep = GridStepPC;
				fMaxSeaHeight = SetMaxSeaHeight(i); //boal
				Sea.Sea2.LodScale = 1.0;
				Sea.Sea2.GridStep = 0.15;
			}
			else
			{
				SetSeaSettings();
				fMaxSeaHeight = 200.0;
			}
		}
	}
	Sea.Lights = aCurWeather.Lights;

	Sea.Water.Color = Whr_GetColor(aSea,"Water.Color");

	Sea.Sun.Color = Whr_GetColor(aCurWeather,"Sun.Color");
	Sea.Sun.HeightAngle = Whr_GetFloat(aCurWeather,"Sun.HeightAngle");
	Sea.Sun.AzimuthAngle = Whr_GetFloat(aCurWeather,"Sun.AzimuthAngle");

	string sCurFog = Whr_GetCurrentFog();
	Sea.Fog.Color = Whr_GetColor(aCurWeather, sCurFog + ".Color");
	Sea.Fog.Enable = Whr_GetLong(aCurWeather, sCurFog + ".Enable");
	Sea.Fog.Start = Whr_GetFloat(aCurWeather, sCurFog + ".Start");
	Sea.Fog.Density = Whr_GetFloat(aCurWeather, sCurFog + ".Density") * FogDensity;
	Sea.Fog.SeaDensity = Whr_GetFloat(aCurWeather, sCurFog + ".SeaDensity") * FogSeaDensity;
	
	Sea.Pena.Color = Whr_GetColor(aSea,"Pena.Color");
	Sea.Pena.DepthSmall = 20.0;
	Sea.Pena.DepthBig = 20.0;
	Sea.Pena.BigIslandMultiply = Whr_GetFloat(aCommon, "Pena.BigIslandMultiply");
	Sea.Pena.SmallIslandMultiply = Whr_GetFloat(aCommon, "Pena.SmallIslandMultiply");
	
	Sea.Bump.Dir = Whr_GetString(aSea, "Bump.Dir");
	Sea.Bump.Tile = Whr_GetFloat(aSea, "Bump.Tile");
	Sea.Bump.Ang = Whr_GetFloat(aSea, "Bump.Ang");
	Sea.Bump.Speed = Whr_GetFloat(aSea, "Bump.Speed");
	Sea.Bump.Scale = Whr_GetFloat(aSea, "Bump.Scale");
	Sea.Bump.AnimSpeed = Whr_GetFloat(aSea, "Bump.AnimSpeed");
	Sea.Bump.MipLevels = 4;
	Sea.Bump.Size = 128;

	Sea.GF2MX.BumpDir = Whr_GetString(aSea, "GF2MX.Bump.Dir");
	Sea.GF2MX.BumpTile = Whr_GetFloat(aSea, "GF2MX.Bump.Tile");
	Sea.GF2MX.BumpAnimSpeed = Whr_GetFloat(aSea, "GF2MX.Bump.AnimSpeed");
	Sea.GF2MX.WaterColor = Whr_GetColor(aSea, "GF2MX.Water.Color");
	Sea.GF2MX.SkyColor = Whr_GetColor(aSea, "GF2MX.Sky.Color");

	Sea.GF3.BumpDir = Whr_GetString(aSea, "Bump.Dir");
	Sea.GF3.BumpTile = Whr_GetFloat(aSea, "Bump.Tile");
	Sea.GF3.BumpAnimSpeed = Whr_GetFloat(aSea, "Bump.AnimSpeed");
	Sea.GF3.WaterColor = Whr_GetColor(aSea, "Water.Color");
	Sea.GF3.SkyColor = Whr_GetColor(aSea, "Sky.Color");

	// Sun road section
	Sea.Road.Start = Whr_GetFloat(aSea, "SunRoad.Start");
	Sea.Road.Color1 = Whr_GetColor(aSea, "SunRoad.Color1");
	Sea.Road.Color2 = Whr_GetColor(aSea, "SunRoad.Color2");
	Sea.Road.Power = Whr_GetFloat(aSea, "SunRoad.Power");

	if (CheckAttribute(aSea, "SunRoad.Special"))
	{
		Sea.Sun.HeightAngle = Whr_GetFloat(aSea, "SunRoad.Special.HeightAngle");
		Sea.Sun.AzimuthAngle = Whr_GetFloat(aSea, "SunRoad.Special.AzimuthAngle");
	}

	Sea.CubeMap.Size = 512;	
	Sea.CubeMap.VectorsSize = 256;
	
	Sea.CubeMap.Format = "r5g6b5";

	Sea.Sky.Color = Whr_GetColor(aSea, "Sky.Color");

	// harmonics
	aref aHarmonics; makearef(aHarmonics, aSea.Harmonics);
	int iNumHarmonics = GetAttributesNum(aHarmonics);
	for (i=0;i<iNumHarmonics;i++)
	{
		aref aHarmonic = GetAttributeN(aHarmonics,i);
		string sTemp = "h" + i;
		Sea.Harmonics.(sTemp) = GetAttributeValue(aHarmonic);
	}

	// Advanced Sea initialize
	Sea.Sea2.WaterColor = Whr_GetColor(aSea2, "WaterColor");
	Sea.Sea2.SkyColor = Whr_GetColor(aSea2, "SkyColor");

	Sea.Sea2.Amp1 = Whr_GetFloat(aSea2, "Amp1");
	Sea.Sea2.AnimSpeed1 = Whr_GetFloat(aSea2, "AnimSpeed1");
	Sea.Sea2.Scale1 = Whr_GetFloat(aSea2, "Scale1");
	Sea.Sea2.MoveSpeed1 = Whr_GetString(aSea2, "MoveSpeed1");

	Sea.Sea2.Amp2 = Whr_GetFloat(aSea2, "Amp2");
	Sea.Sea2.AnimSpeed2 = Whr_GetFloat(aSea2, "AnimSpeed2");
	Sea.Sea2.Scale2 = Whr_GetFloat(aSea2, "Scale2");
	Sea.Sea2.MoveSpeed2 = Whr_GetString(aSea2, "MoveSpeed2");

	Sea.Sea2.PosShift = Whr_GetFloat(aSea2, "PosShift");
	
	Sea.Sea2.Reflection = Whr_GetFloat(aSea2, "Reflection");
	Sea.Sea2.Transparency = Whr_GetFloat(aSea2, "Transparency");
	Sea.Sea2.Attenuation = Whr_GetFloat(aSea2, "Attenuation");
	Sea.Sea2.Frenel = Whr_GetFloat(aSea2, "Frenel");

	Sea.Sea2.SimpleSea = sti(InterfaceStates.SimpleSea);

	Sea.MaxSeaHeight = fMaxSeaHeight;
	Sea.isDone = "";
	//Log_TestInfo("Whether Sea.MaxSeaHeight " + Sea.MaxSeaHeight);
	
	// boal 14/09/06 запоминаем стреднюю волну для моря (не локации)
	if (bSeaActive && !bAbordageStarted)
	{
		pchar.SystemInfo.ScaleSeaHeight = GetScaleSeaHeight();
	}
}

void SetSeaSettings()
{
	Sea.LodScale = LodScale;
	Sea.MaxVertices = MaxVertices;
	Sea.MaxIndices = MaxIndices;
	Sea.MaxWaveDistance = MaxWaveDistance;
	Sea.MaxDim = 65536;
	Sea.MinDim = 128;
	//Sea.GridStep = GridStepPC;
	Sea.GridStep = 0.1 + 0.1 * (1.0 - stf(InterfaceStates.SeaDetails));

	Sea.Sea2.LodScale = 0.5;
	Sea.Sea2.GridStep = 0.07 + 0.1 * (1.0 - stf(InterfaceStates.SeaDetails));
}

void SetSeaGridStep(float SeaDetails)
{
	if (!IsEntity(&Sea)) { return; }

	ref mchr = GetMainCharacter();
	string sLocation = mchr.location;

	// if current location is land - return
	if(FindLocation(sLocation) != -1)
	{
		if(CheckAttribute(&locations[FindLocation(sLocation)], "fastreload"))
		{
			return;
		}
	}

	Sea.Sea2.GridStep = 0.07 + 0.1 * (1.0 - SeaDetails);

	Sea.Sea2.SimpleSea = sti(InterfaceStates.SimpleSea);
}

// boal 14.09.06 относительная высота волны
float GetScaleSeaHeight()
{
	/*
	из ядра
	float fScale = (fMaxSeaHeight >= _fAmp1 + _fAmp2) ? 1.0f : fMaxSeaHeight / (_fAmp1 + _fAmp2);

		fAmp1 = _fAmp1 * fScale;
		fAmp2 = _fAmp2 * fScale;
		
    будеи считать среднюю волну как (а1 + а2)/2
	*/
	float fMaxSeaHeight = 0.0;

	if (CheckAttribute(&Sea, "MaxSeaHeight")) { fMaxSeaHeight = stf(Sea.MaxSeaHeight); }
	
	aref arWeath = GetCurrentWeather();
	float fAmp1, fAmp2;
	
	fAmp1 = stf(arWeath.Sea2.Amp1);
	fAmp2 = stf(arWeath.Sea2.Amp2);
	
	float fScale;
	if (fMaxSeaHeight >= (fAmp1 + fAmp2))
	{
		fScale = 1.0; 
	}
	else
	{
		fScale = fMaxSeaHeight / (fAmp1 + fAmp2);
	}

	fAmp1 = fAmp1 * fScale;
	fAmp2 = fAmp2 * fScale;
		
	return (fAmp1 + fAmp2) / 2.0;
}