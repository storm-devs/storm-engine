void NetClient_OnWeather(int iMsg)
{
	int iSubCode = NMGetByte(iMsg);
	switch (iSubCode)
	{
		case NSC_WEATHER_ADD_LIGHTNING:
			NetClient_LightningAdd(iMsg);
		break;
	}
}