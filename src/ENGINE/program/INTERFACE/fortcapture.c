// to_do форма под нож
ref glob_captref;
string sColonyName;

void TEMP_ExitColony()
{
	SetFortCharacterCaptured(glob_captref,true);
	ProcessCancelExit();
}

void InitInterface_R(string iniName,ref captref)
{
	GameInterface.title = "titleColonyCapture";
	
	glob_captref = captref;

	pchar.from_interface.fortCharacterIdx = captref.index;

	SendMessage(&GameInterface,"ls",MSG_INTERFACE_INIT,iniName);

	SetEventHandler("InterfaceBreak","ProcessCancelExit",0);
	SetEventHandler("exitCancel","ProcessCancelExit",0);

	TEMP_ExitColony(); // сразу в порт
}

void ProcessCancelExit()
{
	DelEventHandler("InterfaceBreak","ProcessCancelExit");
	DelEventHandler("exitCancel","ProcessCancelExit");

	interfaceResultCommand = RC_INTERFACE_FORTCAPTURE_EXIT;
	EndCancelInterface(true);
}


