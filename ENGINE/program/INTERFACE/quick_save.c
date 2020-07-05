
void InitInterface(string iniName)
{
	EngineLayersOffOn(true);
	GameInterface.title = "titleQuickSave";
	
	SendMessage(&GameInterface,"ls",MSG_INTERFACE_INIT,iniName);
	
	
	SetFormatedText("INFO_TEXT", XI_ConvertString("SaveInProgress"));
	
	SendMessage(&GameInterface,"lsl",MSG_INTERFACE_MSG_TO_NODE,"INFO_TEXT",5);
}

void ProcessBreakExit()
{
	IDoExit( RC_INTERFACE_QUICK_SAVE );
	SetTimeScale(1.0);
	TimeScaleCounter = 0;
}

void ProcessCancelExit()
{
	IDoExit( RC_INTERFACE_QUICK_SAVE );
	SetTimeScale(1.0);
	TimeScaleCounter = 0;
}

void IDoExit(int exitCode)
{

}


void DoPostExit()
{
	int exitCode = GetEventData();
	IDoExit(exitCode);
}