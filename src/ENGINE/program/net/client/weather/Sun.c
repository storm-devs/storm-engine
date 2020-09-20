object NCSunGlow;

void NetClient_WhrDeleteSunGlowEnvironment()
{
	if (isEntity(&NCSunGlow))
	{
		DeleteClass(&NCSunGlow);
	}
	DeleteAttribute(&NCSunGlow, "");
}

void NetClient_WhrCreateSunGlowEnvironment()
{
	aref aCurWeather = NetClient_GetCurrentWeather();
	aref aSun;			makearef(aSun, aCurWeather.Sun);
	aref aGlow;			makearef(aGlow, aSun.Glow);
	aref aFlare;		makearef(aFlare, aSun.Flare);
	aref aOverflow;		makearef(aOverflow, aSun.Overflow);
	aref aReflection;	makearef(aReflection, aSun.Reflection);

	NCSunGlow.Clear = "";
	DeleteAttribute(&NCSunGlow, "");

	NCSunGlow.Server = false;

	if (sti(aSun.Glow.Enable) == false && sti(aSun.Flare.Enable) == false)) return;

	// create sunglow
	CreateEntity(&NCSunGlow, "NetSunGlow");
	LayerAddObject("sea_sunroad", &NCSunGlow, -1);

	if (sti(aSun.Glow.Enable) != false)
	{
		NCSunGlow.Glow.Dist = Whr_GetFloat(aGlow, "Dist");
		NCSunGlow.Glow.Size = Whr_GetFloat(aGlow, "Size");
		NCSunGlow.Glow.Texture = Whr_GetString(aGlow, "Texture");
		NCSunGlow.Glow.Color = Whr_GetColor(aGlow, "Color");
		NCSunGlow.Glow.RotateSpeed = Whr_GetColor(aGlow, "RotateSpeed");
		NCSunGlow.Glow.DecayTime = Whr_GetColor(aGlow, "DecayTime");
		NCSunGlow.Glow.TechniqueZ = Whr_GetString(aGlow, "TechniqueZ");
		NCSunGlow.Glow.TechniqueNoZ = Whr_GetString(aGlow, "TechniqueNoZ");
	}

	if (sti(aSun.Flare.Enable) != false)
	{
		NCSunGlow.Flare.Dist = Whr_GetFloat(aFlare, "Dist");
		NCSunGlow.Flare.Technique = Whr_GetString(aFlare, "Technique");
		NCSunGlow.Flare.Scale = Whr_GetFloat(aFlare, "Scale");
		NCSunGlow.Flare.Texture = Whr_GetString(aFlare, "Texture");
		NCSunGlow.Flare.TexSizeX = Whr_GetString(aFlare, "TexSizeX");
		NCSunGlow.Flare.TexSizeY = Whr_GetString(aFlare, "TexSizeY");

		aref aFlares; makearef(aFlares, aSun.Flares);
		int iNumFlares = GetAttributesNum(aFlares);
		for (int i=0;i<iNumFlares;i++)
		{
			aref aFlareN = GetAttributeN(aFlares,i);
			string sTemp = "f" + i;
			NCSunGlow.Flares.(sTemp) = GetAttributeValue(aFlareN);
		}
	}

	if (sti(aSun.Overflow.Enable) != false)
	{
		NCSunGlow.Overflow.Texture = Whr_GetString(aOverflow, "Texture");
		NCSunGlow.Overflow.Technique = Whr_GetString(aOverflow, "Technique");
		NCSunGlow.Overflow.Size = Whr_GetFloat(aOverflow, "Size");
		NCSunGlow.Overflow.Color = Whr_GetColor(aOverflow, "Color");
		NCSunGlow.Overflow.Start = Whr_GetFloat(aOverflow, "Start");
	}

	if (sti(aSun.Reflection.Enable) != false)
	{
		NCSunGlow.Reflection.Texture = Whr_GetString(aReflection, "Texture");
		NCSunGlow.Reflection.Technique = Whr_GetString(aReflection, "Technique");
		NCSunGlow.Reflection.Size = Whr_GetFloat(aReflection, "Size");
		NCSunGlow.Reflection.Dist = Whr_GetFloat(aReflection, "Dist");
		NCSunGlow.Reflection.Color = Whr_GetColor(aReflection, "Color");
	}

	NCSunGlow.isDone = true;
}

void NetClient_MoveSunGlowToLayers(string sExecuteLayer, string sRealizeLayer)
{
	LayerAddObject(sExecuteLayer, &NCSunGlow, -2);
	LayerAddObject(sRealizeLayer, &NCSunGlow, -2);
}