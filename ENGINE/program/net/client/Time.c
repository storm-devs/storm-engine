int iClientDeltaTimeFraction = 0;

void NetClient_DeleteTimeEnvironment()
{
	DelEventHandler("frame", "NetClient_TimeFrame");
}

void NetClient_CreateTimeEnvironment(int iMsg)
{
	SetEventHandler("frame", "NetClient_TimeFrame", 0);

	iClientTime = NMGetDword(iMsg);
	iClientDeltaTimeFraction = 0;
}

void NetClient_TimeFrame()
{
	int iPrevClientTime = iClientTime;

	int iFullDeltaTime = MakeInt(fHighPrecisionDeltaTime * 1000000.0);
	int iDeltaTime = iFullDeltaTime / 1000;
	iClientDeltaTimeFraction = iClientDeltaTimeFraction + (iFullDeltaTime - iDeltaTime * 1000);
	//Trace("dt = " + GetDeltaTime() + ", iD2 = " + iD2 + ", iD3 = " + iD3);

	iClientTime = iClientTime + iDeltaTime;
	iClientServerTime = iClientServerTime + iDeltaTime;

	if (iClientDeltaTimeFraction > 1000)
	{
		int iMilliseconds = iClientDeltaTimeFraction / 1000;
		
		iClientTime = iClientTime + iMilliseconds;
		iClientServerTime = iClientServerTime + iMilliseconds;

		iClientDeltaTimeFraction = iClientDeltaTimeFraction - (iMilliseconds * 1000);
	}

	iTestDeltaTime = iTestDeltaTime + iDeltaTime;
	if (iTestDeltaTime >= 100)
	{
		iTestDeltaTime = iTestDeltaTime - 100;

		int iDelta = iClientTime - iClientServerTime;
		if (iDelta)
		{
			//if (abs(iDelta) <= 2) iClientTime = iClientTime - iDelta;
			if (iDelta > 2) { iClientTime = iClientTime - 2; }
			if (iDelta < -2) { iClientTime = iClientTime + 2; }
		}
	}

	//Trace("ClientTime = " + iClientTime + ", ClientServerTime = " + iClientServerTime);

	iClientDeltaTime = iClientTime - iPrevClientTime;
	if (iClientDeltaTime < 0) { iClientDeltaTime = 0; }
}