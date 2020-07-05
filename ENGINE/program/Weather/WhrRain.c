object Rain;

void WhrDeleteRainEnvironment()
{
	if (isEntity(&Rain))
	{
		DeleteClass(&Rain);
	}
	DeleteAttribute(&Rain,"");
}

void WhrCreateRainEnvironment()
{
	bWeatherIsRain = false;

	if (!isEntity(&Rain))
	{
		CreateEntity(&Rain,"rain");
	}

	FillRainData(iCurWeatherNum,iBlendWeatherNum);
	Rain.isDone = "";

	if( sti(Rain.NumDrops) > 0 ) {bWeatherIsRain = true;}
}

void ClearRainEnvironment()
{
	if (!isEntity(&Rain))
	{
		CreateEntity(&Rain,"rain");
	}

	FillRainData(iCurWeatherNum,iBlendWeatherNum);
	Rain.NumDrops = 0.0;
	Rain.DropLength = 0.1;
	Rain.isDone = "";

	bWeatherIsRain = false;
}

void FillRainData(int nw1, int nw2)
{
	if( nw1 < 0 || nw1 >= MAX_WEATHERS ) {return;}

	aref aRain1; makearef(aRain1, Weathers[nw1].Rain);
	aref aRainbow1; makearef(aRainbow1, Weathers[nw1].Rainbow);

	Rain.Clear = "";
	DeleteAttribute(&Rain,"");

	if( nw2 < 0 )
	{
		Rain.NumDrops = Whr_GetLong(aRain1,"NumDrops");
		Rain.Color = Whr_GetColor(aRain1,"Color");
		Rain.DropLength = Whr_GetFloat(aRain1,"DropLength");
		Rain.Height = Whr_GetFloat(aRain1,"Height");
		Rain.Radius = Whr_GetFloat(aRain1,"Radius");
		Rain.Speed = Whr_GetFloat(aRain1,"Speed");
		Rain.Jitter = Whr_GetFloat(aRain1,"Jitter");
		Rain.WindSpeedJitter = Whr_GetFloat(aRain1,"WindSpeedJitter");
		Rain.TimeBlend = Whr_GetLong(aRain1,"TimeBlend");
		Rain.MaxBlend = Whr_GetLong(aRain1,"MaxBlend");

		Rain.Rainbow.Enable = Whr_GetLong(aRainbow1,"Enable");
		Rain.Rainbow.Texture = Whr_GetString(aRainbow1,"Texture");
	}
	else
	{
		aref aRain2; makearef(aRain2, Weathers[nw2].Rain);
		aref aRainbow2; makearef(aRainbow2, Weathers[nw2].Rainbow);
		float fBlend = stf(Environment.Time) - sti(Environment.Time);

		Rain.NumDrops = Whr_BlendLong( fBlend, Whr_GetLong(aRain1,"NumDrops"), Whr_GetLong(aRain2,"NumDrops") );
		Rain.Color = Whr_BlendColor( fBlend, Whr_GetColor(aRain1,"Color"), Whr_GetColor(aRain2,"Color") );
		Rain.DropLength = Whr_BlendFloat( fBlend, Whr_GetFloat(aRain1,"DropLength"), Whr_GetFloat(aRain2,"DropLength") );
		Rain.Height = Whr_BlendFloat( fBlend, Whr_GetFloat(aRain1,"Height"), Whr_GetFloat(aRain2,"Height") );
		Rain.Radius = Whr_BlendFloat( fBlend, Whr_GetFloat(aRain1,"Radius"), Whr_GetFloat(aRain2,"Radius") );
		Rain.Speed = Whr_BlendFloat( fBlend, Whr_GetFloat(aRain1,"Speed"), Whr_GetFloat(aRain2,"Speed") );
		Rain.Jitter = Whr_BlendFloat( fBlend, Whr_GetFloat(aRain1,"Jitter"), Whr_GetFloat(aRain2,"Jitter") );
		Rain.WindSpeedJitter = Whr_BlendFloat( fBlend, Whr_GetFloat(aRain1,"WindSpeedJitter"), Whr_GetFloat(aRain2,"WindSpeedJitter") );
		Rain.TimeBlend = Whr_BlendLong( fBlend, Whr_GetLong(aRain1,"TimeBlend"), Whr_GetLong(aRain2,"TimeBlend") );
		Rain.MaxBlend = Whr_BlendLong( fBlend, Whr_GetLong(aRain1,"MaxBlend"), Whr_GetLong(aRain2,"MaxBlend") );

		Rain.Rainbow.Enable = Whr_GetLong(aRainbow1,"Enable") || Whr_GetLong(aRainbow2,"Enable");
		Rain.Rainbow.Texture = Whr_GetString(aRainbow1,"Texture");
	}
}

void MoveRainToLayers(string sExecuteLayer, string sRealizeLayer)
{
	LayerDelObject("execute",&Rain);
	LayerDelObject("realize",&Rain);
	LayerDelObject(SEA_EXECUTE,&Rain);
	LayerDelObject(SEA_REALIZE,&Rain);

	LayerAddObject(sExecuteLayer, &Rain, 65537);
	LayerAddObject(sRealizeLayer, &Rain, 65537);
}

void ShowRain(int bShow)
{
	SendMessage(Rain,"ll",1,bShow);
}
