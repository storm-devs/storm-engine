// SLiB Новый файл для ВМЛ, все от К3 потерто
void TavernWaitDate(string date)
{
	int iTime, iAddTime;
	iTime = sti(environment.time);
	if (date == "wait_day")
	{
		if (iTime >= 21) iAddTime = 24 - iTime + 7;
		if (iTime < 7) iAddTime = 7 - iTime;
		// boal 03.03.05 -->
		if (iTime >= 7 && iTime < 21) iAddTime = 24  + 7 - iTime;
		// boal <--
		StoreDayUpdate();
	}
	else
	{
		if (iTime < 24) iAddtime = 24 - iTime;
	}
	LAi_Fade("", "");
	WaitDate("",0,0,0,iAddtime,5);
	RecalculateJumpTable();
	// boal -->
	RefreshWeather();
	RefreshLandTime();
	// boal <--
}

void TavernWaitDate_LSC(string date)
{
	int iTime, iAddTime;
	iTime = sti(environment.time);
	if (date == "wait_day")
	{
		if (iTime >= 21) iAddTime = 24 - iTime + 7;
		if (iTime < 7) iAddTime = 7 - iTime;
		// boal 03.03.05 -->
		if (iTime >= 7 && iTime < 21) iAddTime = 24  + 7 - iTime;
		// boal <--
		StoreDayUpdate();
	}
	else
	{
		if (iTime < 24) iAddtime = 24 - iTime;
	}
	WaitDate("",0,0,0,iAddtime,5);
	DoReloadCharacterToLocation("FleuronTavern",  "quest", "bed");
	RecalculateJumpTable();
	// boal -->
	RefreshWeather();
	RefreshLandTime();
	// boal <--
}
