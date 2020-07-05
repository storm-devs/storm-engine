int iServerDeltaTimeFraction = 0;

void NetServer_DeleteTimeEnvironment()
{
	DelEventHandler("frame", "NetServer_TimeFrame");
}

void NetServer_CreateTimeEnvironment(int iServerEnvSMsg)
{
	iServerTime = 0;
	SetEventHandler("frame", "NetServer_TimeFrame", 0);

	iServerDeltaTimeFraction = 0;
	NMAddDword(iServerEnvSMsg, iServerTime);
}

void NetServer_TimeFrame()
{
	int iFullDeltaTime = MakeInt(fHighPrecisionDeltaTime * 1000000.0);
	int iDeltaTime = iFullDeltaTime / 1000;
	iServerDeltaTimeFraction = iServerDeltaTimeFraction + (iFullDeltaTime - iDeltaTime * 1000);

	iServerTime = iServerTime + iDeltaTime;

	if (iServerDeltaTimeFraction > 1000)
	{
		int iMilliseconds = iServerDeltaTimeFraction / 1000;
		iServerTime = iServerTime + iMilliseconds;
		iServerDeltaTimeFraction = iServerDeltaTimeFraction - (iMilliseconds * 1000);
	}
}
