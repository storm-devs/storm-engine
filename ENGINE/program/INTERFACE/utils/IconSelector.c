bool bIconSelectorActive = false;
string sIS_OkFunc, sIS_CancelFunc;

void CreateIconSelector(ref rObject, int iIconSizeX, int iIconSizeY, string sOkFunc, string sCancelFunc)
{
	sIS_OkFunc = sOkFunc;
	sIS_CancelFunc = sCancelFunc;
}

void KillIconSelector()
{
	if (!bIconSelectorActive) { return; }
}