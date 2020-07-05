ref rEnemyChr;

void InitInterface_R(string iniName, ref chref)
{
    GameInterface.title = "titlePlayGame";
	rEnemyChr = chref;

    SendMessage(&GameInterface,"ls",MSG_INTERFACE_INIT,iniName);

	SetEventHandler("InterfaceBreak","ProcessBreakExit",0)
	SetEventHandler("exitCancel","ProcessCancelExit",0);
	SetEventHandler("ievnt_command","ProcCommand",0);
}

void ProcessBreakExit()
{
	IDoExit( RC_INTERFACE_PLAYGAME_EXIT );
}

void ProcessCancelExit()
{
	IDoExit( RC_INTERFACE_PLAYGAME_EXIT );
}

void IDoExit(int exitCode)
{
	DelEventHandler("InterfaceBreak","ProcessBreakExit");
	DelEventHandler("exitCancel","ProcessCancelExit");
	DelEventHandler("ievnt_command","ProcCommand");

	interfaceResultCommand = exitCode;
	EndCancelInterface(true);
}

void ProcCommand()
{
	string comName = GetEventData();
	string nodName = GetEventData();

	switch(nodName)
	{
	case "B_OK":
	break;
	}
}
