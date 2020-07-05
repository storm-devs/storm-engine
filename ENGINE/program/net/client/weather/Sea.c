object NCSea;

void NetClient_WhrDeleteSeaEnvironment()
{
	if (isEntity(&NCSea))
	{
		DeleteClass(&NCSea);
	}
	DeleteAttribute(&NCSea, "");
}
	
void NetClient_WhrCreateSeaEnvironment()
{
	aref	aCurWeather = NetClient_GetCurrentWeather();
	aref	aSea; makearef(aSea, aCurWeather.Sea);
	aref	aSea2; makearef(aSea2, aCurWeather.Sea2);
	aref	aCommon; makearef(aCommon, WhrCommonParams.Sea);
	
	DeleteAttribute(&NCSea, "");

	NCSea.Clear = "";

	NCSea.Server = false;
	NCSea.SimpleSea = sti(InterfaceStates.SimpleSea);	// must be before creating
	CreateEntity(&NCSea, "NetSea");

	NCSea.Transparency = Whr_GetFloat(aSea,"Transparency");
	NCSea.FrenelCoefficient = Whr_GetFloat(aSea,"FrenelCoefficient");
	NCSea.WaterReflection = Whr_GetFloat(aSea,"WaterReflection");
	NCSea.WaterAttenuation = Whr_GetFloat(aSea,"WaterAttenuation");

	NCSea.Sea2.LodScale = LodScale;
	NCSea.MaxVertices = MaxVertices;
	NCSea.MaxIndices = MaxIndices;
	NCSea.MaxWaveDistance = MaxWaveDistance;
	NCSea.MaxDim = 65536;
	NCSea.MinDim = 128;
	//NCSea.Sea2.GridStep = GridStepPC;
	NCSea.Sea2.GridStep = 0.07 + 0.1 * (1.0 - stf(InterfaceStates.SeaDetails));

	NCSea.Lights = aCurWeather.Lights;

	NCSea.Water.Color = Whr_GetColor(aSea,"Water.Color");

	string sCurFog = NetClient_WhrGetCurrentFog();
	NCSea.Fog.Color = Whr_GetColor(aCurWeather, sCurFog + ".Color");
	NCSea.Fog.Enable = Whr_GetLong(aCurWeather, sCurFog + ".Enable");
	NCSea.Fog.Start = Whr_GetFloat(aCurWeather, sCurFog + ".Start");
	NCSea.Fog.Density = Whr_GetFloat(aCurWeather, sCurFog + ".Density");
	NCSea.Fog.SeaDensity = Whr_GetFloat(aCurWeather, sCurFog + ".SeaDensity");
	
	NCSea.Bump.Dir = Whr_GetString(aSea, "Bump.Dir");
	NCSea.Bump.Tile = Whr_GetFloat(aSea, "Bump.Tile");
	NCSea.Bump.Ang = Whr_GetFloat(aSea, "Bump.Ang");
	NCSea.Bump.Speed = Whr_GetFloat(aSea, "Bump.Speed");
	NCSea.Bump.Scale = Whr_GetFloat(aSea, "Bump.Scale");
	NCSea.Bump.Speed = Whr_GetFloat(aSea, "Bump.AnimSpeed");
	NCSea.Bump.MipLevels = 4;
	NCSea.Bump.Size = 128;

	NCSea.CubeMap.Size = 512;	
	NCSea.CubeMap.VectorsSize = 256;
	
	NCSea.CubeMap.Format = "r5g6b5";

	NCSea.Sky.Color = Whr_GetColor(aSea, "Sky.Color");

	// Advanced Sea initialize
	NCSea.Sea2.WaterColor = Whr_GetColor(aSea2, "WaterColor");
	NCSea.Sea2.SkyColor = Whr_GetColor(aSea2, "SkyColor");

	NCSea.Sea2.Amp1 = Whr_GetFloat(aSea2, "Amp1");
	NCSea.Sea2.AnimSpeed1 = Whr_GetFloat(aSea2, "AnimSpeed1");
	NCSea.Sea2.Scale1 = Whr_GetFloat(aSea2, "Scale1");
	NCSea.Sea2.MoveSpeed1 = Whr_GetString(aSea2, "MoveSpeed1");

	NCSea.Sea2.Amp2 = Whr_GetFloat(aSea2, "Amp2");
	NCSea.Sea2.AnimSpeed2 = Whr_GetFloat(aSea2, "AnimSpeed2");
	NCSea.Sea2.Scale2 = Whr_GetFloat(aSea2, "Scale2");
	NCSea.Sea2.MoveSpeed2 = Whr_GetString(aSea2, "MoveSpeed2");

	NCSea.Sea2.PosShift = Whr_GetFloat(aSea2, "PosShift");
	NCSea.Sea2.BumpScale = Whr_GetFloat(aSea2, "BumpScale");

	NCSea.Sea2.Reflection = Whr_GetFloat(aSea2, "Reflection");
	NCSea.Sea2.Transparency = Whr_GetFloat(aSea2, "Transparency");
	NCSea.Sea2.Attenuation = Whr_GetFloat(aSea2, "Attenuation");
	NCSea.Sea2.Frenel = Whr_GetFloat(aSea2, "Frenel");

	NCSea.MaxSeaHeight = 50.0;
	
	NCSea.isDone = "";
}

void NetClient_MoveSeaToLayers(string sExecuteLayer, string sRealizeLayer)
{
	LayerAddObject(sExecuteLayer, &NCSea, 65530);
	LayerAddObject(sRealizeLayer, &NCSea, 65530);
}
